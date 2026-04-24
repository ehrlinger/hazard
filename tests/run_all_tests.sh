#!/usr/bin/env bash
# tests/run_all_tests.sh
#
# Master test runner for the HAZARD test suite.
#
# Runs unit tests followed by integration tests, reports a summary,
# and exits non-zero if any suite reports failures.
#
# Usage:
#   ./tests/run_all_tests.sh [OPTIONS]
#
# Options:
#   --unit-only          Run only the C unit tests (no binary needed)
#   --integration-only   Run only the integration test scripts
#   --skip-memory        Skip the ASan/UBSan memory safety suite (slow)
#   --skip-concurrent    Skip the concurrent execution suite
#   HAZARD_BIN=<path>    Path to the hazard executable
#
# Environment:
#   HAZARD_BIN   Path to hazard binary (default: ./hazard)
#   CC           C compiler for unit tests (default: gcc)
#
# Exit codes:
#   0  All selected suites passed (or were skipped)
#   1  One or more suites reported failures

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
UNIT_DIR="${SCRIPT_DIR}/unit"
INTG_DIR="${SCRIPT_DIR}/integration"

export HAZARD_BIN="${HAZARD_BIN:-${REPO_DIR}/hazard}"

# ------------------------------------------------------------------ #
# Option parsing                                                       #
# ------------------------------------------------------------------ #

RUN_UNIT=1
RUN_INTEGRATION=1
SKIP_MEMORY=0
SKIP_CONCURRENT=0

for arg in "$@"; do
    case "${arg}" in
        --unit-only)         RUN_INTEGRATION=0 ;;
        --integration-only)  RUN_UNIT=0 ;;
        --skip-memory)       SKIP_MEMORY=1 ;;
        --skip-concurrent)   SKIP_CONCURRENT=1 ;;
        HAZARD_BIN=*)        export HAZARD_BIN="${arg#*=}" ;;
        *)
            echo "Unknown option: ${arg}"
            echo "Usage: $0 [--unit-only] [--integration-only] [--skip-memory] [--skip-concurrent] [HAZARD_BIN=<path>]"
            exit 1
            ;;
    esac
done

# ------------------------------------------------------------------ #
# Tracking                                                             #
# ------------------------------------------------------------------ #

SUITE_PASS=0
SUITE_FAIL=0
SUITE_SKIP=0

suite_passed()  { echo "  [PASS] $1"; SUITE_PASS=$((SUITE_PASS + 1)); }
suite_failed()  { echo "  [FAIL] $1"; SUITE_FAIL=$((SUITE_FAIL + 1)); }
suite_skipped() { echo "  [SKIP] $1 — $2"; SUITE_SKIP=$((SUITE_SKIP + 1)); }

# Run a script and record its outcome.
# Scripts that exit 0 are passed unless they report "SKIP:" on line 1.
# Non-zero exit status is a failure.
run_suite() {
    local label="$1" script="$2"
    shift 2

    if [ ! -f "${script}" ]; then
        suite_skipped "${label}" "script not found: ${script}"; return
    fi

    echo ""
    echo "╔══════════════════════════════════════════╗"
    printf  "║  %-40s║\n" "${label}"
    echo "╚══════════════════════════════════════════╝"

    local rc=0 first_line reason
    local tmp_output
    tmp_output="$(mktemp)"

    if [ "$#" -gt 0 ]; then
        bash "${script}" "$@" >"${tmp_output}" 2>&1 || rc=$?
    else
        bash "${script}" >"${tmp_output}" 2>&1 || rc=$?
    fi
    cat "${tmp_output}"

    first_line="$(head -n 1 "${tmp_output}" || true)"
    rm -f "${tmp_output}"

    echo ""
    if [ "${rc}" -eq 0 ]; then
        if [[ "${first_line}" == SKIP:* ]]; then
            reason="${first_line#SKIP: }"
            suite_skipped "${label}" "${reason:-suite reported skip}"
        else
            suite_passed "${label}"
        fi
    else
        suite_failed "${label} (exit code ${rc})"
    fi
}

# ------------------------------------------------------------------ #
# Main                                                                 #
# ------------------------------------------------------------------ #

echo "========================================================"
echo "HAZARD Test Suite"
echo "HAZARD_BIN: ${HAZARD_BIN}"
echo "Date:       $(date '+%Y-%m-%d %H:%M:%S')"
echo "========================================================"

# ------------------------------------------------------------------ #
# Unit tests                                                           #
# ------------------------------------------------------------------ #

if [ "${RUN_UNIT}" -eq 1 ]; then
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "UNIT TESTS"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

    if [ ! -f "${UNIT_DIR}/Makefile" ]; then
        suite_skipped "unit tests" "tests/unit/Makefile not found"
    else
        echo ""
        echo "╔══════════════════════════════════════════╗"
        printf  "║  %-40s║\n" "C Unit Tests (make check)"
        echo "╚══════════════════════════════════════════╝"

        local_rc=0
        (cd "${UNIT_DIR}" && make -s check) || local_rc=$?

        echo ""
        if [ "${local_rc}" -eq 0 ]; then
            suite_passed "C unit tests"
        else
            suite_failed "C unit tests (exit code ${local_rc})"
        fi
    fi
fi

# ------------------------------------------------------------------ #
# Integration tests                                                    #
# ------------------------------------------------------------------ #

if [ "${RUN_INTEGRATION}" -eq 1 ]; then
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "INTEGRATION TESTS"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

    # V1 — Numerical regression (bit-exact to 3 d.p.)
    run_suite "V1: Numerical Regression" \
        "${INTG_DIR}/validate_numerical.sh"

    # V2 — Optimizer convergence (8 d.p., exact nfncts/iterations)
    run_suite "V2: Optimizer Convergence" \
        "${INTG_DIR}/validate_convergence.sh"

    # V3 — Stepwise variable selection path
    run_suite "V3: Stepwise Selection Path" \
        "${INTG_DIR}/validate_stepwise.sh"

    # V6 — Memory safety (AddressSanitizer + UBSan)
    if [ "${SKIP_MEMORY}" -eq 1 ]; then
        suite_skipped "V6: Memory Safety (ASan)" "--skip-memory specified"
    else
        run_suite "V6: Memory Safety (ASan + UBSan)" \
            "${INTG_DIR}/validate_memory.sh"
    fi

    # V7 — Concurrent execution
    if [ "${SKIP_CONCURRENT}" -eq 1 ]; then
        suite_skipped "V7: Concurrent Execution" "--skip-concurrent specified"
    else
        run_suite "V7: Concurrent Execution" \
            "${INTG_DIR}/validate_concurrent.sh"
    fi

    # V8 — Acceptance corpus.  Black-box byte-diff of modern-binary
    # output against the auto-selected reference for the host toolchain
    # family: Darwin/arm64 → v4.4.2-macos-arm64, Linux/Windows → v4.3.0
    # (gcc-bucket CCF capture).
    #
    # Off by default: on gcc-family hosts the default v4.3.0 reference
    # bucket has known banner + org-string diffs vs v4.4.x binaries that
    # the normalizer deliberately does NOT mask, so running V8 from CI
    # unconditionally turns green CI red even when numerics are bit-
    # identical.  Until a v4.4.x Linux recapture lands (FINDINGS.md §5
    # item 5), gate V8 behind HAZARD_RUN_ACCEPTANCE=1 — operators still
    # get the full signal via a direct `./tests/validate_corpus.sh`
    # invocation.  See docs/VALIDATION_PLAN.md, tests/corpus/README.md,
    # and tests/corpus/FINDINGS.md §2a.
    if [ "${HAZARD_RUN_ACCEPTANCE:-0}" = "1" ]; then
        run_suite "V8: Acceptance Corpus" \
            "${SCRIPT_DIR}/validate_corpus.sh"
    else
        suite_skipped "V8: Acceptance Corpus" \
            "set HAZARD_RUN_ACCEPTANCE=1 to enable (or run tests/validate_corpus.sh directly)"
    fi
fi

# ------------------------------------------------------------------ #
# Summary                                                              #
# ------------------------------------------------------------------ #

echo ""
echo "========================================================"
echo "SUMMARY"
echo "========================================================"
printf "  Suites passed:  %d\n" "${SUITE_PASS}"
printf "  Suites failed:  %d\n" "${SUITE_FAIL}"
printf "  Suites skipped: %d\n" "${SUITE_SKIP}"
echo "--------------------------------------------------------"

if [ "${SUITE_FAIL}" -gt 0 ]; then
    echo "  RESULT: FAILED"
    echo "========================================================"
    exit 1
else
    echo "  RESULT: PASSED"
    echo "========================================================"
    exit 0
fi
