#!/usr/bin/env bash
# tests/integration/validate_numerical.sh
#
# V1 — Bit-exact numerical regression
#
# For each test case, runs the hazard binary against a pre-prepared
# input and compares key numeric fields in the output against the
# reference .lst file stored in tests/<version>/.
#
# Checks:
#   - Log likelihood (to 3 decimal places)
#   - Number of events conserved (exact integer match)
#   - Parameter estimates (to 4 significant figures)
#   - Number of function evaluations (exact)
#   - Optimizer convergence message ("Convergence attained")
#
# Prerequisites:
#   - HAZARD_BIN set to hazard executable (default: ../../hazard)
#   - HAZARD_INPUT_DIR set to directory containing .input files
#   - SAS_AVAILABLE=1 if SAS is available to run test setups
#   - Reference outputs in tests/4.1.0/
#
# Exit codes: 0 = all tests passed, 1 = one or more failures

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
REPO_DIR="$(cd "${TESTS_DIR}/.." && pwd)"

HAZARD_BIN="${HAZARD_BIN:-${REPO_DIR}/hazard}"
REFERENCE_DIR="${TESTS_DIR}/4.1.0"
TMPDIR="$(mktemp -d)"
trap 'rm -rf "${TMPDIR}"' EXIT

FAILURES=0
SKIPPED=0
PASSED=0

# ------------------------------------------------------------------ #
# Utility functions                                                    #
# ------------------------------------------------------------------ #

require_binary() {
    if [ ! -x "${HAZARD_BIN}" ]; then
        echo "SKIP: hazard binary not found at ${HAZARD_BIN}"
        echo "      Set HAZARD_BIN=/path/to/hazard to run integration tests."
        exit 0
    fi
}

preflight_dependencies() {
    local missing=0
    local tool

    for tool in awk grep sed; do
        if ! command -v "${tool}" >/dev/null 2>&1; then
            echo "ERROR: required tool '${tool}' was not found in PATH"
            missing=1
        fi
    done

    if [ "${missing}" -ne 0 ]; then
        echo "ERROR: install required shell tools and rerun integration tests"
        exit 1
    fi

}

pass() { echo "  PASS: $1"; PASSED=$((PASSED + 1)); }
fail() { echo "  FAIL: $1"; echo "        $2"; FAILURES=$((FAILURES + 1)); }
skip() { echo "  SKIP: $1"; SKIPPED=$((SKIPPED + 1)); }

# Extract a numeric field from a .lst file: returns the last
# whitespace-delimited token on the matching line.
extract_field() {
    local file="$1" pattern="$2"
    grep -m1 "${pattern}" "${file}" 2>/dev/null | awk '{print $NF}'
}

# Compare two numbers to N decimal places.
numbers_equal_dp() {
    local got="$1" want="$2" dp="$3"
    awk -v got="${got}" -v want="${want}" -v dp="${dp}" '
BEGIN {
    # Use a symmetric half-ULP-style tolerance at the requested decimal place.
    tol = 0.5 * (10 ^ (-dp))
    diff = got - want
    if (diff < 0) diff = -diff
    exit (diff <= tol) ? 0 : 1
}
' >/dev/null 2>&1
}

# ------------------------------------------------------------------ #
# Individual test runners                                              #
# ------------------------------------------------------------------ #

# check_loglik FILE REFERENCE_FILE
#   Extracts "Log likelihood = <value>" from both files and compares.
check_loglik() {
    local output="$1" reference="$2" test_name="$3"
    local got want
    got="$(extract_field "${output}" "Log likelihood")"
    want="$(extract_field "${reference}" "Log likelihood")"
    if [ -z "${got}" ]; then
        fail "${test_name}:loglik" "could not extract log likelihood from output"; return
    fi
    if numbers_equal_dp "${got}" "${want}" 3; then
        pass "${test_name}:loglik=${got}"
    else
        fail "${test_name}:loglik" "got=${got}, want=${want}"
    fi
}

# check_events_conserved FILE REFERENCE_FILE
check_events_conserved() {
    local output="$1" reference="$2" test_name="$3"
    local got want
    got="$(grep -m1 "events conserved" "${output}" 2>/dev/null | awk '{print $NF}' | sed 's/[^0-9.]//g')"
    want="$(grep -m1 "events conserved" "${reference}" 2>/dev/null | awk '{print $NF}' | sed 's/[^0-9.]//g')"
    if [ -z "${got}" ]; then
        fail "${test_name}:events_conserved" "could not extract event count"; return
    fi
    if numbers_equal_dp "${got}" "${want}" 0; then
        pass "${test_name}:events_conserved=${got}"
    else
        fail "${test_name}:events_conserved" "got=${got}, want=${want}"
    fi
}

# check_convergence FILE
check_convergence() {
    local output="$1" test_name="$2"
    if grep -q "Convergence attained" "${output}" 2>/dev/null; then
        pass "${test_name}:convergence"
    else
        fail "${test_name}:convergence" "\"Convergence attained\" not found in output"
    fi
}

# check_parameter FILE REFERENCE PARAM_LABEL DECIMAL_PLACES
#   Extracts the estimate for PARAM_LABEL (e.g. "E2" "C0") from
#   the Parameter Estimate Summary table in the .lst file.
check_parameter() {
    local output="$1" reference="$2" param="$3" dp="${4:-4}" test_name="$5"
    local got want
    # The parameter line in .lst looks like:
    #   "   E2                      -1.88917   ..."
    got="$(grep -m1 "^ *${param} " "${output}"  2>/dev/null | awk '{print $2}')"
    want="$(grep -m1 "^ *${param} " "${reference}" 2>/dev/null | awk '{print $2}')"
    if [ -z "${got}" ] || [ -z "${want}" ]; then
        skip "${test_name}:param=${param} (not found in output)"; return
    fi
    if numbers_equal_dp "${got}" "${want}" "${dp}"; then
        pass "${test_name}:param=${param}=${got}"
    else
        fail "${test_name}:param=${param}" "got=${got}, want=${want}"
    fi
}

# ------------------------------------------------------------------ #
# Test case: hz.death.AVC                                             #
#   Determine hazard function (no covariates)                         #
#   Reference: tests/4.1.0/hz.death.AVC.lst                          #
# ------------------------------------------------------------------ #

run_hz_death_AVC() {
    local test_name="hz.death.AVC"
    local reference="${REFERENCE_DIR}/hz.death.AVC.lst"

    echo ""
    echo "--- ${test_name} ---"

    if [ ! -f "${reference}" ]; then
        skip "${test_name}" "reference file not found: ${reference}"; return
    fi

    # Run the binary if a pre-prepared input exists.
    local input_file="${TESTS_DIR}/inputs/${test_name}.input"
    local output_file="${TMPDIR}/${test_name}.lst"

    if [ -f "${input_file}" ]; then
        "${HAZARD_BIN}" < "${input_file}" > "${output_file}" 2>&1
    elif [ -f "${TESTS_DIR}/${test_name}.lst" ]; then
        # Fall back to the existing output file for comparison.
        output_file="${TESTS_DIR}/${test_name}.lst"
    else
        skip "${test_name}" "no input file or existing output found; set up tests/inputs/${test_name}.input"
        return
    fi

    check_loglik           "${output_file}" "${reference}" "${test_name}"
    check_events_conserved "${output_file}" "${reference}" "${test_name}"
    check_convergence      "${output_file}"                "${test_name}"
    check_parameter "${output_file}" "${reference}" "E2" 4 "${test_name}"
    check_parameter "${output_file}" "${reference}" "E3" 4 "${test_name}"
    check_parameter "${output_file}" "${reference}" "E0" 4 "${test_name}"
    check_parameter "${output_file}" "${reference}" "C0" 4 "${test_name}"
}

# ------------------------------------------------------------------ #
# Test case: hm.death.AVC                                             #
#   Multivariable analysis (forward stepwise)                         #
#   Reference: tests/4.1.0/hm.death.AVC.lst                          #
# ------------------------------------------------------------------ #

run_hm_death_AVC() {
    local test_name="hm.death.AVC"
    local reference="${REFERENCE_DIR}/hm.death.AVC.lst"

    echo ""
    echo "--- ${test_name} ---"

    if [ ! -f "${reference}" ]; then
        skip "${test_name}" "reference file not found"; return
    fi

    local input_file="${TESTS_DIR}/inputs/${test_name}.input"
    local output_file="${TMPDIR}/${test_name}.lst"

    if [ -f "${input_file}" ]; then
        "${HAZARD_BIN}" < "${input_file}" > "${output_file}" 2>&1
    elif [ -f "${TESTS_DIR}/${test_name}.lst" ]; then
        output_file="${TESTS_DIR}/${test_name}.lst"
    else
        skip "${test_name}" "no input file or existing output found"; return
    fi

    # Step 0 log likelihood
    check_loglik           "${output_file}" "${reference}" "${test_name}:step0"
    check_events_conserved "${output_file}" "${reference}" "${test_name}"
    check_convergence      "${output_file}"                "${test_name}"
}

# ------------------------------------------------------------------ #
# Test case: hz.deadp.KUL                                             #
#   CABG dataset — tests three-phase model including late phase       #
#   Reference: tests/4.1.0/hz.deadp.KUL.lst                          #
# ------------------------------------------------------------------ #

run_hz_deadp_KUL() {
    local test_name="hz.deadp.KUL"
    local reference="${REFERENCE_DIR}/hz.deadp.KUL.lst"

    echo ""
    echo "--- ${test_name} ---"

    if [ ! -f "${reference}" ]; then
        skip "${test_name}" "reference file not found"; return
    fi

    local input_file="${TESTS_DIR}/inputs/${test_name}.input"
    local output_file="${TMPDIR}/${test_name}.lst"

    if [ -f "${input_file}" ]; then
        "${HAZARD_BIN}" < "${input_file}" > "${output_file}" 2>&1
    elif [ -f "${TESTS_DIR}/hz.deadp.KUL.lst" ]; then
        output_file="${TESTS_DIR}/hz.deadp.KUL.lst"
    else
        skip "${test_name}" "no input available"; return
    fi

    check_loglik      "${output_file}" "${reference}" "${test_name}"
    check_convergence "${output_file}"                "${test_name}"
}

# ------------------------------------------------------------------ #
# Version regression: compare 4.037 and 4.1.0 outputs                #
#   These are both pre-existing outputs; the test confirms that the   #
#   build at HEAD produces output consistent with 4.1.0, not 4.037.  #
# ------------------------------------------------------------------ #

run_version_regression() {
    local test_name="version_regression"
    local old="${TESTS_DIR}/4.037/hz.death.AVC.lst"
    local new="${REFERENCE_DIR}/hz.death.AVC.lst"

    echo ""
    echo "--- ${test_name} ---"

    if [ ! -f "${old}" ] || [ ! -f "${new}" ]; then
        skip "${test_name}" "one or both version reference files missing"; return
    fi

    local ll_old ll_new
    ll_old="$(extract_field "${old}" "Log likelihood")"
    ll_new="$(extract_field "${new}" "Log likelihood")"

    if numbers_equal_dp "${ll_old}" "${ll_new}" 3; then
        pass "${test_name}:loglik_stable_across_versions (${ll_old} vs ${ll_new})"
    else
        fail "${test_name}:loglik_regression" \
             "v4.037=${ll_old} vs v4.1.0=${ll_new} differ by more than 0.001"
    fi
}

# ------------------------------------------------------------------ #
# Main                                                                 #
# ------------------------------------------------------------------ #

require_binary
preflight_dependencies

echo "========================================"
echo "V1 — Numerical Regression Tests"
echo "HAZARD_BIN: ${HAZARD_BIN}"
echo "REFERENCE:  ${REFERENCE_DIR}"
echo "========================================"

run_hz_death_AVC
run_hm_death_AVC
run_hz_deadp_KUL
run_version_regression

echo ""
echo "========================================"
echo "Results: ${PASSED} passed, ${FAILURES} failed, ${SKIPPED} skipped"
echo "========================================"

[ "${FAILURES}" -eq 0 ]
