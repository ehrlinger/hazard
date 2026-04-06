#!/usr/bin/env bash
# tests/integration/validate_concurrent.sh
#
# V7 — Concurrent Execution (Global State Isolation)
#
# Verifies that two hazard analyses running simultaneously produce
# correct, independent output.  The hazard binary historically uses
# global state (HZRstr, Common, Early, Late, G, Params, ...).  If any
# global is mutated by one process and visible to the other, the
# outputs will diverge from the single-process reference.
#
# Strategy:
#   1. Run two different analyses back-to-back (serial) as baseline.
#   2. Run the same two analyses simultaneously (parallel) under the
#      same shell.
#   3. Compare each parallel output to its serial baseline.
#
# A difference between serial and parallel output is evidence of
# shared-state contamination — either the process image is sharing
# memory (impossible for separate processes unless using shared libs
# with global state) or the test revealed a pre-existing file-system
# race (e.g., both writing to the same temp file).
#
# Note: Because each `hazard` invocation is a separate OS process,
# true global-state contamination cannot occur between processes unless
# the binary opens shared files for write.  This test primarily guards
# against regressions where a refactoring introduces process-level
# shared resources (mmap, named semaphores, fixed-path temp files).
#
# See also: §5 V7 in docs/Claude_MODERNIZATION_GUIDE.md

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
# Prerequisites                                                        #
# ------------------------------------------------------------------ #

require_binary() {
    if [ ! -x "${HAZARD_BIN}" ]; then
        echo "SKIP: hazard binary not found at ${HAZARD_BIN}"
        echo "      Set HAZARD_BIN=/path/to/hazard to run integration tests."
        exit 0
    fi
}

# ------------------------------------------------------------------ #
# Helpers                                                              #
# ------------------------------------------------------------------ #

# Extract the final log likelihood line from an output file.
extract_loglik_line() {
    grep "Log likelihood" "$1" 2>/dev/null | tail -1
}

# Extract a single summary stat for quick comparison.
extract_summary() {
    local f="$1"
    {
        grep "Log likelihood"      "${f}" 2>/dev/null | tail -1
        grep "events conserved"    "${f}" 2>/dev/null | tail -1
        grep "terminated after"    "${f}" 2>/dev/null | tail -1
        grep "Convergence attained" "${f}" 2>/dev/null | head -1
    }
}

# Compare two output files on the summary statistics.
# Returns 0 if they match, 1 if they differ.
outputs_match() {
    local a="$1" b="$2"
    local sum_a sum_b
    sum_a="$(extract_summary "${a}")"
    sum_b="$(extract_summary "${b}")"
    [ "${sum_a}" = "${sum_b}" ]
}

# ------------------------------------------------------------------ #
# Serial baseline run                                                  #
# ------------------------------------------------------------------ #

run_serial() {
    local name1="$1" input1="$2"
    local name2="$3" input2="$4"

    echo "  Running serial baseline (${name1} then ${name2})..."

    "${HAZARD_BIN}" < "${input1}" > "${TMPDIR}/${name1}.serial.lst" 2>&1
    "${HAZARD_BIN}" < "${input2}" > "${TMPDIR}/${name2}.serial.lst" 2>&1
}

# ------------------------------------------------------------------ #
# Parallel run                                                         #
# ------------------------------------------------------------------ #

run_parallel() {
    local name1="$1" input1="$2"
    local name2="$3" input2="$4"

    echo "  Running parallel (${name1} and ${name2} simultaneously)..."

    # Launch both in the background; collect PIDs.
    "${HAZARD_BIN}" < "${input1}" > "${TMPDIR}/${name1}.parallel.lst" 2>&1 &
    local pid1=$!

    "${HAZARD_BIN}" < "${input2}" > "${TMPDIR}/${name2}.parallel.lst" 2>&1 &
    local pid2=$!

    # Wait for both and capture exit codes.
    local rc1=0 rc2=0
    wait "${pid1}" || rc1=$?
    wait "${pid2}" || rc2=$?

    if [ "${rc1}" -ne 0 ]; then
        fail "${name1}:parallel_exit" "process exited with code ${rc1}"
    fi
    if [ "${rc2}" -ne 0 ]; then
        fail "${name2}:parallel_exit" "process exited with code ${rc2}"
    fi
}

# ------------------------------------------------------------------ #
# Compare serial vs parallel outputs                                   #
# ------------------------------------------------------------------ #

check_serial_vs_parallel() {
    local name="$1"
    local serial_out="${TMPDIR}/${name}.serial.lst"
    local parallel_out="${TMPDIR}/${name}.parallel.lst"

    if [ ! -f "${serial_out}" ] || [ ! -f "${parallel_out}" ]; then
        skip "${name}:concurrent" "output file(s) missing"; return
    fi

    if outputs_match "${serial_out}" "${parallel_out}"; then
        pass "${name}:concurrent_matches_serial"
    else
        fail "${name}:concurrent" "parallel output differs from serial baseline"
        echo "    --- serial summary ---"
        extract_summary "${serial_out}"   | sed 's/^/    /'
        echo "    --- parallel summary ---"
        extract_summary "${parallel_out}" | sed 's/^/    /'
    fi
}

# ------------------------------------------------------------------ #
# Check that outputs also match their reference .lst files            #
# ------------------------------------------------------------------ #

check_matches_reference() {
    local name="$1"
    local reference="${REFERENCE_DIR}/${name}.lst"
    local parallel_out="${TMPDIR}/${name}.parallel.lst"

    if [ ! -f "${reference}" ]; then
        skip "${name}:concurrent_reference" "reference file not found: ${reference}"; return
    fi

    if [ ! -f "${parallel_out}" ]; then
        skip "${name}:concurrent_reference" "parallel output missing"; return
    fi

    if outputs_match "${parallel_out}" "${reference}"; then
        pass "${name}:concurrent_matches_reference"
    else
        fail "${name}:concurrent_reference" "parallel output diverges from reference"
        echo "    --- expected (from ${reference}) ---"
        extract_summary "${reference}"    | sed 's/^/    /'
        echo "    --- got (parallel run) ---"
        extract_summary "${parallel_out}" | sed 's/^/    /'
    fi
}

# ------------------------------------------------------------------ #
# Main concurrent test runner                                          #
# ------------------------------------------------------------------ #

run_concurrent_test() {
    local name1="$1" name2="$2"
    local input1="${TESTS_DIR}/inputs/${name1}.input"
    local input2="${TESTS_DIR}/inputs/${name2}.input"

    echo ""
    echo "--- Concurrent: ${name1} vs ${name2} ---"

    if [ ! -f "${input1}" ]; then
        skip "concurrent:${name1}" "input not found: ${input1}"; return
    fi
    if [ ! -f "${input2}" ]; then
        skip "concurrent:${name2}" "input not found: ${input2}"; return
    fi

    run_serial   "${name1}" "${input1}" "${name2}" "${input2}"
    run_parallel "${name1}" "${input1}" "${name2}" "${input2}"

    check_serial_vs_parallel   "${name1}"
    check_serial_vs_parallel   "${name2}"
    check_matches_reference    "${name1}"
    check_matches_reference    "${name2}"
}

# ------------------------------------------------------------------ #
# Smoke test: single process produces correct output twice in a row   #
# (tests determinism, not concurrency)                                #
# ------------------------------------------------------------------ #

run_determinism_test() {
    local name="$1"
    local input="${TESTS_DIR}/inputs/${name}.input"

    echo ""
    echo "--- Determinism (run twice): ${name} ---"

    if [ ! -f "${input}" ]; then
        skip "determinism:${name}" "input not found: ${input}"; return
    fi

    "${HAZARD_BIN}" < "${input}" > "${TMPDIR}/${name}.det1.lst" 2>&1
    "${HAZARD_BIN}" < "${input}" > "${TMPDIR}/${name}.det2.lst" 2>&1

    if outputs_match "${TMPDIR}/${name}.det1.lst" "${TMPDIR}/${name}.det2.lst"; then
        pass "${name}:deterministic"
    else
        fail "${name}:deterministic" "two sequential runs produced different output"
    fi
}

# ------------------------------------------------------------------ #
# Stress: three simultaneous analyses                                  #
# ------------------------------------------------------------------ #

run_stress_3way() {
    local name1="$1" name2="$2" name3="$3"
    local input1="${TESTS_DIR}/inputs/${name1}.input"
    local input2="${TESTS_DIR}/inputs/${name2}.input"
    local input3="${TESTS_DIR}/inputs/${name3}.input"

    echo ""
    echo "--- 3-way concurrent stress: ${name1}, ${name2}, ${name3} ---"

    # Skip silently if any input is missing; this is a bonus test.
    if [ ! -f "${input1}" ] || [ ! -f "${input2}" ] || [ ! -f "${input3}" ]; then
        skip "stress_3way" "one or more inputs missing — need all three"; return
    fi

    "${HAZARD_BIN}" < "${input1}" > "${TMPDIR}/${name1}.stress.lst" 2>&1 &
    local p1=$!
    "${HAZARD_BIN}" < "${input2}" > "${TMPDIR}/${name2}.stress.lst" 2>&1 &
    local p2=$!
    "${HAZARD_BIN}" < "${input3}" > "${TMPDIR}/${name3}.stress.lst" 2>&1 &
    local p3=$!

    wait "${p1}" || true
    wait "${p2}" || true
    wait "${p3}" || true

    local ok=1
    for name in "${name1}" "${name2}" "${name3}"; do
        local serial_out="${TMPDIR}/${name}.serial.lst"
        local stress_out="${TMPDIR}/${name}.stress.lst"
        if [ -f "${serial_out}" ] && [ -f "${stress_out}" ]; then
            if ! outputs_match "${serial_out}" "${stress_out}"; then
                fail "stress_3way:${name}" "3-way output differs from serial"; ok=0
            fi
        fi
    done
    if [ "${ok}" -eq 1 ]; then
        pass "stress_3way:all_match_serial"
    fi
}

# ------------------------------------------------------------------ #
# Main                                                                 #
# ------------------------------------------------------------------ #

require_binary

echo "========================================"
echo "V7 — Concurrent Execution Tests"
echo "HAZARD_BIN: ${HAZARD_BIN}"
echo "========================================"

# Determinism guard — must pass before concurrent tests are meaningful.
run_determinism_test "hz.death.AVC"

# Primary concurrent pair: one simple, one multivariable.
run_concurrent_test "hz.death.AVC" "hm.death.AVC"

# Three-phase model paired with multivariable.
run_concurrent_test "hz.deadp.KUL" "hz.te123.OMC"

# Optional 3-way stress (skipped gracefully if inputs are missing).
run_stress_3way "hz.death.AVC" "hm.death.AVC" "hz.deadp.KUL"

echo ""
echo "========================================"
echo "Results: ${PASSED} passed, ${FAILURES} failed, ${SKIPPED} skipped"
echo "========================================"

[ "${FAILURES}" -eq 0 ]
