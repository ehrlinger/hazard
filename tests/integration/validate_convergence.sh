#!/usr/bin/env bash
# tests/integration/validate_convergence.sh
#
# V2 — Optimizer Convergence Regression
#
# Verifies that the optimizer converges to exactly the same:
#   - Log likelihood (to 8 decimal places)
#   - Number of function evaluations (exact)
#   - Iteration count (exact)
#   - Condition code (within 10%)
#
# This test is intentionally stricter than validate_numerical.sh.
# It detects silent regressions in the optimization path (e.g. from
# changes to the BFGS implementation, step-size heuristics, or
# extended-precision accumulation).
#
# Reference values come from tests/4.1.0/ which is the known-good
# baseline for v4.1.0.
#
# See also: §5 V2 in docs/Claude_MODERNIZATION_GUIDE.md

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

require_binary() {
    if [ ! -x "${HAZARD_BIN}" ]; then
        echo "SKIP: hazard binary not found at ${HAZARD_BIN}"
        exit 0
    fi
}

pass() { echo "  PASS: $1"; PASSED=$((PASSED + 1)); }
fail() { echo "  FAIL: $1 — $2"; FAILURES=$((FAILURES + 1)); }
skip() { echo "  SKIP: $1 — $2"; SKIPPED=$((SKIPPED + 1)); }

numbers_equal_dp() {
    python3 -c "
import sys
got, want, dp = float('$1'), float('$2'), int('$3')
tol = 0.5 * 10**(-dp)
sys.exit(0 if abs(got - want) <= tol else 1)
" 2>/dev/null
}

numbers_within_pct() {
    # Returns 0 (success) if |got-want|/|want| <= pct/100
    python3 -c "
import sys
got, want, pct = float('$1'), float('$2'), float('$3')
if want == 0:
    sys.exit(0 if abs(got - want) < 1e-10 else 1)
sys.exit(0 if abs(got - want) / abs(want) <= pct / 100.0 else 1)
" 2>/dev/null
}

# Extract the FINAL log likelihood (last occurrence in the log).
extract_final_loglik() {
    grep "Log likelihood" "$1" 2>/dev/null | tail -1 | awk '{print $NF}'
}

# Extract "terminated after N iterations and M function evaluations"
extract_iterations() {
    grep "terminated after" "$1" 2>/dev/null | head -1 | \
        sed 's/.*after *\([0-9]*\) iter.*/\1/'
}

extract_nfncts() {
    grep "terminated after" "$1" 2>/dev/null | head -1 | \
        sed 's/.*and *\([0-9]*\) function.*/\1/'
}

# Extract "Log base 10 of condition code = VALUE"
extract_condcode() {
    grep "condition code" "$1" 2>/dev/null | head -1 | awk '{print $NF}'
}

# ------------------------------------------------------------------ #
run_convergence_check() {
    local test_name="$1"
    local reference="${REFERENCE_DIR}/${test_name}.lst"
    local input_file="${TESTS_DIR}/inputs/${test_name}.input"
    local output_file="${TMPDIR}/${test_name}.lst"

    echo ""
    echo "--- ${test_name} ---"

    if [ ! -f "${reference}" ]; then
        skip "${test_name}" "reference file not found: ${reference}"; return
    fi

    if [ -f "${input_file}" ]; then
        "${HAZARD_BIN}" < "${input_file}" > "${output_file}" 2>&1
    elif [ -f "${TESTS_DIR}/${test_name}.lst" ]; then
        output_file="${TESTS_DIR}/${test_name}.lst"
    else
        skip "${test_name}" "no input available; create tests/inputs/${test_name}.input"; return
    fi

    # 1. Log likelihood (8 decimal places)
    local ll_got ll_want
    ll_got="$(extract_final_loglik "${output_file}")"
    ll_want="$(extract_final_loglik "${reference}")"
    if [ -n "${ll_got}" ] && [ -n "${ll_want}" ]; then
        if numbers_equal_dp "${ll_got}" "${ll_want}" 8; then
            pass "${test_name}:loglik_8dp (${ll_got})"
        else
            fail "${test_name}:loglik_8dp" "got=${ll_got}, want=${ll_want}"
        fi
    else
        skip "${test_name}:loglik" "could not extract"
    fi

    # 2. Number of function evaluations (exact)
    local nf_got nf_want
    nf_got="$(extract_nfncts "${output_file}")"
    nf_want="$(extract_nfncts "${reference}")"
    if [ -n "${nf_got}" ] && [ -n "${nf_want}" ]; then
        if [ "${nf_got}" = "${nf_want}" ]; then
            pass "${test_name}:nfncts=${nf_got}"
        else
            fail "${test_name}:nfncts" "got=${nf_got}, want=${nf_want}"
        fi
    else
        skip "${test_name}:nfncts" "could not extract"
    fi

    # 3. Iteration count (exact)
    local iter_got iter_want
    iter_got="$(extract_iterations "${output_file}")"
    iter_want="$(extract_iterations "${reference}")"
    if [ -n "${iter_got}" ] && [ -n "${iter_want}" ]; then
        if [ "${iter_got}" = "${iter_want}" ]; then
            pass "${test_name}:iterations=${iter_got}"
        else
            fail "${test_name}:iterations" "got=${iter_got}, want=${iter_want}"
        fi
    else
        skip "${test_name}:iterations" "could not extract"
    fi

    # 4. Condition code (within 10%)
    local cc_got cc_want
    cc_got="$(extract_condcode "${output_file}")"
    cc_want="$(extract_condcode "${reference}")"
    if [ -n "${cc_got}" ] && [ -n "${cc_want}" ]; then
        if numbers_within_pct "${cc_got}" "${cc_want}" 10; then
            pass "${test_name}:condcode=${cc_got} (ref=${cc_want})"
        else
            fail "${test_name}:condcode" "got=${cc_got}, want=${cc_want} (>10% difference)"
        fi
    else
        skip "${test_name}:condcode" "could not extract"
    fi

    # 5. Convergence message present
    if grep -q "Convergence attained" "${output_file}" 2>/dev/null; then
        pass "${test_name}:convergence_message"
    else
        fail "${test_name}:convergence_message" "\"Convergence attained\" not found"
    fi
}

# ------------------------------------------------------------------ #
# Main                                                                 #
# ------------------------------------------------------------------ #

require_binary

echo "========================================"
echo "V2 — Optimizer Convergence Tests"
echo "HAZARD_BIN: ${HAZARD_BIN}"
echo "========================================"

# No-covariate baseline: simplest possible convergence test.
run_convergence_check "hz.death.AVC"

# With covariates — more complex optimization landscape.
run_convergence_check "hm.death.AVC"

# Three-phase model — tests all three likelihood phases.
run_convergence_check "hz.deadp.KUL"

# OMC dataset — tests interval-censored observations.
run_convergence_check "hz.te123.OMC"

echo ""
echo "========================================"
echo "Results: ${PASSED} passed, ${FAILURES} failed, ${SKIPPED} skipped"
echo "========================================"

[ "${FAILURES}" -eq 0 ]
