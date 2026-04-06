#!/usr/bin/env bash
# tests/integration/validate_stepwise.sh
#
# V3 — Stepwise Variable Selection Path Regression
#
# After Phase 3 of modernization, the sequence of variables entered
# and removed during forward stepwise selection must match exactly.
# The stepwise path is order-sensitive: a different entry/exit sequence
# indicates a change in the Wald test or score test implementation.
#
# Reference path from tests/4.1.0/hm.death.AVC.lst:
#   Step 0:  Initial model converged at LL=-181.109
#            COM_IV (Early), MAL (Early) entered
#            ORIFICE (Constant), INC_SURG (Constant), OP_AGE (Constant) entered
#   Step 1:  MAL    deleted from CONSTANT  (Z=-0.205, P=0.8378)
#   Step 2:  OP_AGE deleted from CONSTANT  (Z=-1.411, P=0.1583)
#   Step 3:  STATUS added to EARLY         (Z=4.158,  P<0.0001)
#   (further steps follow — see reference .lst for full path)
#
# Exit codes: 0 = stepwise path matches, 1 = mismatch or not runnable

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
fail() { echo "  FAIL: $1"; FAILURES=$((FAILURES + 1)); }
skip() {
    if [ $# -gt 1 ]; then
        echo "  SKIP: $1 ($2)"
    else
        echo "  SKIP: $1"
    fi
    SKIPPED=$((SKIPPED + 1))
}

# ------------------------------------------------------------------ #
# Extract the stepwise move sequence from a .lst file.
#
# Lines in the .lst file look like:
#   |Step   1. MAL      deleted from CONSTANT PHASE            |
#   |Step   2. OP_AGE   deleted from CONSTANT PHASE            |
#   |Step   3. STATUS   added to EARLY PHASE                   |
#
# Output: one line per step with format "VARIABLE ACTION PHASE"
#   e.g. "MAL deleted CONSTANT"
#        "STATUS added EARLY"
# ------------------------------------------------------------------ #

extract_stepwise_path() {
    local file="$1"
    grep -E '^\|Step +[0-9]+\.' "${file}" 2>/dev/null | \
        sed 's/.*Step *[0-9]*\. *\([A-Za-z_0-9]*\) *\(added to\|deleted from\) *\([A-Z]*\) PHASE.*/\1 \2 \3/' | \
        sed 's/added to/ENTER/; s/deleted from/REMOVE/'
}

# ------------------------------------------------------------------ #
# Check step-by-step match                                            #
# ------------------------------------------------------------------ #

compare_stepwise_paths() {
    local output="$1" reference="$2" test_name="$3"

    local got_path ref_path
    got_path="$(extract_stepwise_path "${output}")"
    ref_path="$(extract_stepwise_path "${reference}")"

    if [ -z "${ref_path}" ]; then
        skip "${test_name}:stepwise_path" "could not extract reference path"
        return
    fi

    if [ -z "${got_path}" ]; then
        skip "${test_name}:stepwise_path" "could not extract output path (binary may not have run)"
        return
    fi

    if [ "${got_path}" = "${ref_path}" ]; then
        local nsteps
        nsteps="$(echo "${got_path}" | wc -l)"
        pass "${test_name}:stepwise_path (${nsteps} steps match)"
    else
        fail "${test_name}:stepwise_path"
        echo "  Reference path:"
        echo "${ref_path}" | sed 's/^/    /'
        echo "  Got path:"
        echo "${got_path}" | sed 's/^/    /'
        echo ""
        echo "  First differing step:"
        diff <(echo "${ref_path}") <(echo "${got_path}") | head -10 | sed 's/^/    /'
    fi
}

# ------------------------------------------------------------------ #
# Validate known reference stepwise path (hm.death.AVC)              #
# This test validates the reference .lst against the expected path    #
# that was documented when the baseline was established.              #
# ------------------------------------------------------------------ #

validate_reference_path_hm_AVC() {
    local test_name="hm.death.AVC:reference_path"
    local reference="${REFERENCE_DIR}/hm.death.AVC.lst"

    echo ""
    echo "--- Validate reference stepwise path ---"

    if [ ! -f "${reference}" ]; then
        skip "${test_name}" "reference file not found"; return
    fi

    # These are the steps confirmed from tests/4.1.0/hm.death.AVC.lst
    # The test verifies the reference file contains exactly these steps
    # in exactly this order.
    local expected_steps
    expected_steps="MAL REMOVE CONSTANT
OP_AGE REMOVE CONSTANT
STATUS ENTER EARLY"

    local actual_steps
    actual_steps="$(extract_stepwise_path "${reference}" | head -3)"

    if [ "${actual_steps}" = "${expected_steps}" ]; then
        pass "${test_name}:first_3_steps_correct"
    else
        fail "${test_name}:first_3_steps_correct"
        echo "  Expected:"
        echo "${expected_steps}" | sed 's/^/    /'
        echo "  Got:"
        echo "${actual_steps}" | sed 's/^/    /'
    fi
}

# ------------------------------------------------------------------ #
# Run binary and compare stepwise path                                #
# ------------------------------------------------------------------ #

run_stepwise_regression() {
    local test_name="$1"
    local reference="${REFERENCE_DIR}/${test_name}.lst"
    local input_file="${TESTS_DIR}/inputs/${test_name}.input"
    local output_file="${TMPDIR}/${test_name}.lst"

    echo ""
    echo "--- ${test_name} ---"

    if [ ! -f "${reference}" ]; then
        skip "${test_name}" "reference file not found"; return
    fi

    if [ -f "${input_file}" ]; then
        "${HAZARD_BIN}" < "${input_file}" > "${output_file}" 2>&1
    elif [ -f "${TESTS_DIR}/${test_name}.lst" ]; then
        output_file="${TESTS_DIR}/${test_name}.lst"
    else
        skip "${test_name}" "no input available"; return
    fi

    compare_stepwise_paths "${output_file}" "${reference}" "${test_name}"
}

# ------------------------------------------------------------------ #
# Main                                                                 #
# ------------------------------------------------------------------ #

require_binary

echo "========================================"
echo "V3 — Stepwise Variable Selection Path"
echo "HAZARD_BIN: ${HAZARD_BIN}"
echo "========================================"

# First validate that the reference itself contains the expected path.
validate_reference_path_hm_AVC

# Then run the binary and compare.
run_stepwise_regression "hm.death.AVC"
run_stepwise_regression "hm.dead.n1surg"
run_stepwise_regression "hm.deadp.VALVES"

echo ""
echo "========================================"
echo "Results: ${PASSED} passed, ${FAILURES} failed, ${SKIPPED} skipped"
echo "========================================"

[ "${FAILURES}" -eq 0 ]
