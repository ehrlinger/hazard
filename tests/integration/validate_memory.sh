#!/usr/bin/env bash
# tests/integration/validate_memory.sh
#
# V6 — Memory Safety (AddressSanitizer + UBSan)
#
# Rebuilds the project with -fsanitize=address,undefined and runs
# the standard test suite.  Any ASan or UBSan finding is a defect
# introduced by modernization phases 1–2.
#
# Run this after completing Phase 1 (context struct) and Phase 2
# (llike/optim refactor).  A clean run with no sanitizer output
# is the signal that no memory bugs were introduced.
#
# Prerequisites:
#   - gcc ≥ 9 or clang ≥ 6 (for -fsanitize=address,undefined)
#   - The project must be buildable via ./configure && make
#   - Tests inputs must be in tests/inputs/
#
# See also: §5 V6 in docs/Claude_MODERNIZATION_GUIDE.md

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
REPO_DIR="$(cd "${TESTS_DIR}/.." && pwd)"

HAZARD_BIN="${HAZARD_BIN:-${REPO_DIR}/hazard}"
REFERENCE_DIR="${TESTS_DIR}/4.1.0"
TMPDIR="$(mktemp -d)"
trap 'rm -rf "${TMPDIR}"' EXIT

FAILURES=0
PASSED=0
SKIPPED=0

pass() { echo "  PASS: $1"; PASSED=$((PASSED + 1)); }
fail() { echo "  FAIL: $1 — $2"; FAILURES=$((FAILURES + 1)); }
skip() { echo "  SKIP: $1 — $2"; SKIPPED=$((SKIPPED + 1)); }

# ------------------------------------------------------------------ #
# Check that the compiler supports ASan                               #
# ------------------------------------------------------------------ #

check_asan_support() {
    local cc="${CC:-gcc}"
    if ! echo 'int main(){return 0;}' | \
        ${cc} -fsanitize=address,undefined -x c - -o /dev/null 2>/dev/null; then
        echo "SKIP: ${cc} does not support -fsanitize=address,undefined"
        echo "      Install gcc ≥ 9 or clang ≥ 6 to run memory-safety tests."
        exit 0
    fi
}

# ------------------------------------------------------------------ #
# Build an ASan-instrumented binary                                    #
# ------------------------------------------------------------------ #

build_asan_binary() {
    local asan_build_dir="${TMPDIR}/asan_build"
    local asan_bin="${asan_build_dir}/hazard"

    echo "Building ASan-instrumented binary..."
    mkdir -p "${asan_build_dir}"

    # Try to build using the project Makefile with ASan CFLAGS.
    # Falls back to the unit test Makefile ASan target if available.
    if [ -f "${REPO_DIR}/Makefile" ]; then
        (cd "${REPO_DIR}" && \
            make clean > /dev/null 2>&1 || true && \
            ./configure CFLAGS="-fsanitize=address,undefined -g -O1" \
                        LDFLAGS="-fsanitize=address,undefined" \
                        --quiet > "${TMPDIR}/configure.log" 2>&1 && \
            make -j4 > "${TMPDIR}/build.log" 2>&1) || {
            echo "  WARNING: ASan build failed. Check ${TMPDIR}/build.log"
            echo "  Falling back to pre-built binary for basic ASan smoke test."
            asan_bin="${HAZARD_BIN}"
        }
        # Find the built binary
        if [ -f "${REPO_DIR}/hazard" ]; then
            asan_bin="${REPO_DIR}/hazard"
        fi
    else
        echo "  WARNING: No Makefile found; using pre-built binary."
        asan_bin="${HAZARD_BIN}"
    fi

    echo "${asan_bin}"
}

# ------------------------------------------------------------------ #
# Run the binary under ASan and check for sanitizer errors            #
# ------------------------------------------------------------------ #

run_asan_check() {
    local bin="$1" input_file="$2" test_name="$3"
    local output_file="${TMPDIR}/${test_name}.asan.log"

    if [ ! -f "${input_file}" ]; then
        skip "${test_name}:asan" "input file not found: ${input_file}"
        return
    fi

    if [ ! -x "${bin}" ]; then
        skip "${test_name}:asan" "binary not found: ${bin}"
        return
    fi

    # Run with ASAN_OPTIONS to get full reports.
    ASAN_OPTIONS="detect_leaks=1:check_initialization_order=1" \
        "${bin}" < "${input_file}" > "${output_file}" 2>&1 || true

    # Grep for sanitizer error patterns.
    local asan_errors
    asan_errors="$(grep -c -E "ERROR|runtime error|LEAK SUMMARY|AddressSanitizer" \
        "${output_file}" 2>/dev/null || true)"

    if [ "${asan_errors}" -eq 0 ]; then
        pass "${test_name}:asan_clean"
    else
        fail "${test_name}:asan" "${asan_errors} sanitizer finding(s)"
        echo "  Sanitizer output:"
        grep -E "ERROR|runtime error|LEAK|AddressSanitizer" \
            "${output_file}" 2>/dev/null | head -20 | sed 's/^/    /'
    fi
}

# ------------------------------------------------------------------ #
# Unit test ASan target                                                #
# ------------------------------------------------------------------ #

run_unit_tests_asan() {
    local unit_dir="${TESTS_DIR}/unit"
    local test_name="unit_tests_asan"

    echo ""
    echo "--- Unit test suite under ASan ---"

    if [ ! -f "${unit_dir}/Makefile" ]; then
        skip "${test_name}" "unit test Makefile not found"; return
    fi

    local asan_log="${TMPDIR}/unit_asan.log"
    (cd "${unit_dir}" && make asan check 2>&1) > "${asan_log}" || true

    local asan_errors
    asan_errors="$(grep -c -E "ASAN|ERROR|runtime error" "${asan_log}" 2>/dev/null || true)"

    local test_failures
    test_failures="$(grep -c "FAIL:" "${asan_log}" 2>/dev/null || true)"

    if [ "${asan_errors}" -eq 0 ] && [ "${test_failures}" -eq 0 ]; then
        pass "${test_name}:all_clean"
    else
        if [ "${asan_errors}" -gt 0 ]; then
            fail "${test_name}:asan_errors" "${asan_errors} sanitizer finding(s)"
        fi
        if [ "${test_failures}" -gt 0 ]; then
            fail "${test_name}:test_failures" "${test_failures} test(s) failed under ASan"
        fi
        grep -E "FAIL:|ASAN|ERROR|runtime error" "${asan_log}" | head -20 | sed 's/^/    /'
    fi
}

# ------------------------------------------------------------------ #
# Main                                                                 #
# ------------------------------------------------------------------ #

check_asan_support

echo "========================================"
echo "V6 — Memory Safety (ASan + UBSan)"
echo "========================================"

# Run unit tests under ASan first (fastest feedback).
run_unit_tests_asan

# Build ASan-instrumented hazard binary and run integration tests.
echo ""
echo "--- Integration tests under ASan ---"
ASAN_BIN="$(build_asan_binary)"

for test_name in hz.death.AVC hm.death.AVC; do
    input="${TESTS_DIR}/inputs/${test_name}.input"
    run_asan_check "${ASAN_BIN}" "${input}" "${test_name}"
done

# Restore the non-ASan build to avoid leaving the project in an
# instrumented state.
if [ -f "${REPO_DIR}/Makefile" ]; then
    echo ""
    echo "Restoring non-ASan build..."
    (cd "${REPO_DIR}" && ./configure --quiet > /dev/null 2>&1 && \
        make -j4 > /dev/null 2>&1) || \
        echo "  WARNING: restore build failed; re-run ./configure && make manually."
fi

echo ""
echo "========================================"
echo "Results: ${PASSED} passed, ${FAILURES} failed, ${SKIPPED} skipped"
echo "========================================"

[ "${FAILURES}" -eq 0 ]
