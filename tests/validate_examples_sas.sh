#!/usr/bin/env bash
#
# tests/validate_examples_sas.sh
#
# Sister harness to tests/validate_corpus.sh.  Where validate_corpus.sh
# exercises the bare hazard / hazpred binaries with captured .input
# stdin and byte-diffs the bare-binary stdout, this harness compares
# SAS-wrapped .lst outputs against a SAS-driven reference bucket.
#
# Workflow:
#
#   1. Operator runs SAS via tests/run-hazard-capture.sh (or the
#      Edwards-side capture-windows-outputs.ps1) to produce a fresh
#      capture tarball or staged dir of <case>.lst files.
#   2. This harness loads a chosen reference bucket from
#      tests/corpus/hazard/reference/v<X.Y.Z>-<platform>-sas/
#      and the candidate captures from a directory the operator points
#      at via --got-dir.
#   3. For each <case>.lst that exists in BOTH dirs, normalise both
#      sides through tests/corpus_normalize.sh (which masks SAS
#      page-header timestamps as well as the binary version banner)
#      and byte-diff.
#
# Unlike validate_corpus.sh, this harness does NOT run anything itself
# -- the SAS run requires a SAS install + macros, which lives outside
# this repo.  Capture is a separate operator step; this harness only
# does the diff side.
#
# Environment / args
# ------------------
#   REFERENCE     reference bucket name (default: auto-selected from
#                 host -- Linux -> v4.4.5-linux-x64-sas,
#                                Windows / Cygwin -> v4.4.5-windows-x64-sas,
#                                Darwin -> no SAS-on-macOS bucket exists).
#                 Override with REFERENCE=... to compare against a
#                 specific bucket regardless of host.
#
#   --got-dir DIR  Directory containing the candidate <case>.lst files
#                  to diff.  Required.  Typically the extracted output
#                  of run-hazard-capture.sh --corpus-layout.
#
#   --normalize PATH  Path to corpus_normalize.sh.  Default: the
#                     sibling tests/corpus_normalize.sh.
#
# Exit codes
# ----------
#   0  all overlapping cases pass
#   1  one or more cases failed
#   2  setup error (missing reference bucket, missing got-dir, etc.)
#
# Examples
# --------
#   # Diff a fresh Edwards capture against the v4.4.5-windows-x64-sas
#   # reference (auto-selected on Cygwin/Mingw, explicit elsewhere):
#   $ tar -xzf hazard-corpus-v4.4.5-windows-x64-sas-<ts>.tar.gz \
#         -C /tmp/got-windows
#   $ REFERENCE=v4.4.5-windows-x64-sas \
#     tests/validate_examples_sas.sh --got-dir /tmp/got-windows
#
#   # On a Linux host with the SAS reference bucket and a fresh capture:
#   $ tests/validate_examples_sas.sh --got-dir /tmp/got-linux

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CORPUS_DIR="${SCRIPT_DIR}/corpus"
NORMALIZE="${SCRIPT_DIR}/corpus_normalize.sh"

got_dir=""

# Argument parsing.  Each flag that takes a value explicitly checks that a
# value follows -- under `set -u`, accessing $2 when it doesn't exist would
# abort with "unbound variable" rather than emit a useful error message.
while [[ $# -gt 0 ]]; do
    case "$1" in
        --got-dir)
            if [[ $# -lt 2 ]]; then
                echo "ERROR: missing value for --got-dir" >&2
                exit 2
            fi
            got_dir="$2"; shift 2 ;;
        --normalize)
            if [[ $# -lt 2 ]]; then
                echo "ERROR: missing value for --normalize" >&2
                exit 2
            fi
            NORMALIZE="$2"; shift 2 ;;
        --help|-h)
            sed -nE '2,/^# Exit codes/{s/^# ?//; /^!/d; p;}' "${BASH_SOURCE[0]}"
            exit 0 ;;
        *) echo "ERROR: unknown flag: $1" >&2; exit 2 ;;
    esac
done

# ---------------------------------------------------------------------------
# Auto-select REFERENCE per host
# ---------------------------------------------------------------------------
if [[ -z "${REFERENCE:-}" ]]; then
    case "$(uname -s 2>/dev/null || echo unknown)" in
        Linux*)                        REFERENCE=v4.4.5-linux-x64-sas ;;
        CYGWIN*|MINGW*|MSYS*|Windows*) REFERENCE=v4.4.5-windows-x64-sas ;;
        Darwin)
            echo "ERROR: no SAS-on-macOS reference bucket exists." >&2
            echo "       SAS Foundation does not ship for macOS; this harness" >&2
            echo "       compares SAS-wrapped .lst output and is only meaningful" >&2
            echo "       on Linux or Windows hosts.  Override with REFERENCE=..." >&2
            echo "       if you have access to a non-default bucket." >&2
            exit 2 ;;
        *)
            echo "ERROR: cannot auto-select REFERENCE on $(uname -s); pass REFERENCE=... explicitly." >&2
            exit 2 ;;
    esac
fi

ref_dir="$CORPUS_DIR/hazard/reference/$REFERENCE"
if [[ ! -d "$ref_dir" ]]; then
    echo "ERROR: reference bucket not found: $ref_dir" >&2
    echo "       Has it been captured + committed yet?  See tests/run-hazard-capture.sh" >&2
    echo "       (Linux / production CCF) or tests/capture-windows-outputs.ps1 (Edwards)." >&2
    exit 2
fi

if [[ -z "$got_dir" ]]; then
    echo "ERROR: --got-dir is required.  Point at the directory containing the" >&2
    echo "       candidate <case>.lst files (typically the extracted contents" >&2
    echo "       of a run-hazard-capture.sh --corpus-layout tarball)." >&2
    exit 2
fi
if [[ ! -d "$got_dir" ]]; then
    echo "ERROR: --got-dir does not exist: $got_dir" >&2
    exit 2
fi
got_dir="$(cd "$got_dir" && pwd)"

if [[ ! -x "$NORMALIZE" ]]; then
    echo "ERROR: normalize filter is not executable: $NORMALIZE" >&2
    exit 2
fi

# ---------------------------------------------------------------------------
# Diff loop
# ---------------------------------------------------------------------------
work_dir="$(mktemp -d -t validate-examples-sas.XXXXXX)"
trap 'rm -rf "$work_dir"' EXIT

PASSED=0
FAILED=0
SKIPPED=0

echo "=========================================="
echo "Acceptance corpus -- hazard SAS-wrapped"
echo "  reference : $ref_dir"
echo "  got       : $got_dir"
echo "  normalize : $NORMALIZE"
echo "=========================================="

shopt -s nullglob
ref_lsts=("$ref_dir"/*.lst)
shopt -u nullglob
if [[ ${#ref_lsts[@]} -eq 0 ]]; then
    echo "  SKIP: reference bucket is empty"
    echo "Acceptance corpus summary"
    echo "  passed  : 0"
    echo "  failed  : 0"
    echo "  skipped : 0"
    exit 0
fi

for ref_lst in "${ref_lsts[@]}"; do
    case_name="$(basename "$ref_lst" .lst)"
    got_lst="$got_dir/$case_name.lst"

    if [[ ! -f "$got_lst" ]]; then
        echo "  SKIP: $case_name -- not present in --got-dir"
        SKIPPED=$((SKIPPED + 1))
        continue
    fi

    norm_ref="$work_dir/$case_name.ref.norm"
    norm_got="$work_dir/$case_name.got.norm"
    "$NORMALIZE" < "$ref_lst" > "$norm_ref"
    "$NORMALIZE" < "$got_lst" > "$norm_got"

    if diff -q "$norm_ref" "$norm_got" >/dev/null 2>&1; then
        echo "  PASS: $case_name"
        PASSED=$((PASSED + 1))
    else
        echo "  FAIL: $case_name"
        echo "    diff (reference vs. got):"
        { { diff -u "$norm_ref" "$norm_got" || true; } \
            | sed 's/^/      /' \
            | head -40; } || true
        FAILED=$((FAILED + 1))
    fi
done

echo ""
echo "=========================================="
echo "Acceptance corpus summary"
echo "  passed  : $PASSED"
echo "  failed  : $FAILED"
echo "  skipped : $SKIPPED"
echo "=========================================="

if [[ $FAILED -gt 0 ]]; then
    exit 1
fi
exit 0
