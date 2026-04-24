#!/usr/bin/env bash
#
# tests/validate_corpus.sh
#
# Acceptance harness: runs the current `hazard` (and `hazpred`) binaries
# against every captured example in tests/corpus/, normalises outputs via
# tests/corpus_normalize.sh, and byte-diffs against the selected reference
# corpus.  Any per-example diff is a failure; the diff itself is printed
# so the operator can see what changed.
#
# Shipped reference sets (see tests/corpus/FINDINGS.md §2a for provenance
# and the two-bucket cross-toolchain model):
#
#   v4.3.0                — archived CCF production capture (RHEL 8, gcc,
#                           glibc).  Numerically bit-matches the gcc-family
#                           bucket (Linux + Windows/MinGW) on the LL metric,
#                           but the full .lst differs from v4.4.x on the
#                           version banner and the "Cleveland Clinic" vs
#                           "Cleveland Clinic Foundation" organisation
#                           string.  Useful for audit against the CCF
#                           ground truth; not a drop-in CI reference for
#                           v4.4.x builds without a banner-normalising
#                           patch (not yet implemented).
#   v4.4.2-macos-arm64    — self-consistency reference captured from the
#                           modern binary on macOS/Apple-clang/arm64.
#                           Matches any v4.4.x build on the same
#                           toolchain; the 92 commits v4.3.1 → v4.4.2 are
#                           numerically inert on this toolchain.  Default
#                           when host is Darwin/arm64.
#
#   (future) v4.4.N-linux-gcc     — recapture target for a v4.4.x build on
#                                   Linux gcc; would replace v4.3.0 as the
#                                   default for Linux/gcc hosts.
#
# Environment
# -----------
#   HAZARD_BIN    path to the hazard binary (default: ../src/hazard/hazard)
#   HAZPRED_BIN   path to the hazpred binary (default: ../src/hazpred/hazpred)
#   REFERENCE     reference version directory name.  Default auto-selected
#                 from host toolchain family:
#                   Darwin/arm64  → v4.4.2-macos-arm64
#                   Linux/Windows → v4.3.0  (gcc-bucket; expect banner and
#                                            org-string diffs vs v4.4.x
#                                            builds pending a Linux
#                                            recapture)
#                 Override with e.g. REFERENCE=v4.3.0 on macOS to force
#                 the legacy comparison.
#
# Exit codes
# ----------
#   0  — all examples pass
#   1  — one or more examples failed
#   2  — harness setup error (missing binary, missing corpus, etc.)
#
# Graceful skip: if tests/corpus/<kind>/inputs/ is empty, the harness
# treats that kind as "corpus not yet captured" and reports SKIP, not
# FAIL.  That keeps CI green on the scaffolding PR (which lands the
# harness before the captured tuples).

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
CORPUS_DIR="${SCRIPT_DIR}/corpus"
NORMALIZE="${SCRIPT_DIR}/corpus_normalize.sh"

HAZARD_BIN="${HAZARD_BIN:-${REPO_DIR}/src/hazard/hazard}"
HAZPRED_BIN="${HAZPRED_BIN:-${REPO_DIR}/src/hazpred/hazpred}"

# Auto-select reference based on host toolchain family, unless caller
# overrode REFERENCE explicitly.  See header for the two-bucket model.
if [ -z "${REFERENCE:-}" ]; then
    _KERNEL="$(uname -s 2>/dev/null || echo unknown)"
    _MACH="$(uname -m 2>/dev/null || echo unknown)"
    case "$_KERNEL/$_MACH" in
        Darwin/arm64)   REFERENCE=v4.4.2-macos-arm64 ;;
        # Linux / Windows builds use the CCF v4.3.0 capture as the
        # gcc-family reference.  Numerically identical to a v4.4.x
        # Linux build on the log-likelihood metric (confirmed via
        # the linux-ll-check / windows-ll-check workflows — see
        # ROOT-CAUSE-ANALYSIS.md §2.2), but the version banner and
        # the "Cleveland Clinic" → "Cleveland Clinic Foundation"
        # organisation string differ as genuine inter-version text
        # changes that the normalizer deliberately does NOT mask.
        # Expect per-example cosmetic FAILs under this default on
        # Linux until a v4.4.x Linux reference is captured (see
        # FINDINGS.md §5 item 5).  Override with REFERENCE=... to
        # force a specific bucket.
        *)              REFERENCE=v4.3.0 ;;
    esac
fi

PASSED=0
FAILED=0
SKIPPED=0

# ------------------------------------------------------------------ #
# Preflight                                                           #
# ------------------------------------------------------------------ #

if [ ! -x "$NORMALIZE" ]; then
    echo "ERROR: normalize filter '$NORMALIZE' is not executable" >&2
    exit 2
fi

# Each binary is optional — we run whatever corpora have both (a) captured
# inputs and (b) a working binary.  Missing-binary = skip that kind, not
# fail the harness.

# ------------------------------------------------------------------ #
# Per-kind driver                                                     #
# ------------------------------------------------------------------ #
#
# Arguments:
#   $1  kind ("hazard" or "hazpred")
#   $2  binary path
#
# Walks tests/corpus/<kind>/inputs/*.input, runs the binary against each
# with TMPDIR pointing at the inputs dir (so hzr*.dta / hzr*.haz resolve),
# normalises the listing, diffs against the reference.

run_kind() {
    local kind="$1"
    local bin="$2"
    local inputs_dir="$CORPUS_DIR/$kind/inputs"
    local ref_dir="$CORPUS_DIR/$kind/reference/$REFERENCE"
    local dev_dir="$CORPUS_DIR/$kind/deviations/$REFERENCE"
    local work_dir
    work_dir="$(mktemp -d)"
    # Cleanup is done explicitly at every return site below — we
    # deliberately do NOT use `trap 'rm -rf "$work_dir"' RETURN`: bash
    # installs RETURN traps shell-wide rather than function-scoped, so
    # the trap would fire on unrelated function returns and, with
    # `set -u`, trip on the now-unset local `$work_dir`.

    echo ""
    echo "=========================================="
    echo "Acceptance corpus — $kind"
    echo "  binary    : $bin"
    echo "  inputs    : $inputs_dir"
    echo "  reference : $ref_dir"
    echo "=========================================="

    if [ ! -d "$inputs_dir" ]; then
        echo "  SKIP: inputs dir missing — $kind corpus not captured yet"
        SKIPPED=$((SKIPPED + 1))
        rm -rf "$work_dir"; return 0
    fi

    shopt -s nullglob
    local inputs=("$inputs_dir"/*.input)
    shopt -u nullglob

    if [ ${#inputs[@]} -eq 0 ]; then
        echo "  SKIP: no .input files under $inputs_dir"
        SKIPPED=$((SKIPPED + 1))
        rm -rf "$work_dir"; return 0
    fi

    if [ ! -x "$bin" ]; then
        echo "  SKIP: binary '$bin' not built"
        SKIPPED=$((SKIPPED + 1))
        rm -rf "$work_dir"; return 0
    fi

    if [ ! -d "$ref_dir" ]; then
        # No references captured for this kind under this version —
        # treated as SKIP, same as the per-example case below.  Common
        # when a binary type is blocked (e.g. hazpred SIGSEGVs under
        # v4.4.2; see tests/corpus/FINDINGS.md §2b).
        echo "  SKIP: no $REFERENCE references captured for $kind ($ref_dir missing)"
        SKIPPED=$((SKIPPED + 1))
        rm -rf "$work_dir"; return 0
    fi

    local input name ref_lst got_lst norm_got norm_ref dev_sed
    for input in "${inputs[@]}"; do
        name="$(basename "$input" .input)"
        ref_lst="$ref_dir/$name.lst"
        got_lst="$work_dir/$name.lst"

        if [ ! -f "$ref_lst" ]; then
            # Reference not yet captured for this example under this
            # version — treat as SKIP.  Missing references under an
            # existing reference dir are a known state when a capture
            # is blocked (e.g. hazpred SIGSEGV under v4.4.2 per
            # tests/corpus/FINDINGS.md §2b).  The example stays visible
            # in the harness output as SKIP rather than quietly dropping.
            echo "  SKIP: $name — no $REFERENCE reference captured"
            SKIPPED=$((SKIPPED + 1))
            continue
        fi

        # Run the binary with TMPDIR pointed at the inputs dir so
        # hzr*.dta / hzr*.haz file lookups resolve directly against the
        # captured copies.  The opnfils.c 80-byte TMPDIR buffer overflow
        # (formerly forcing a /tmp/hz.XXXXXX symlink shim here) was
        # fixed — opnfils.c now uses PATH_MAX buffers and snprintf.
        if ! TMPDIR="$inputs_dir" "$bin" < "$input" > "$got_lst" 2>&1; then
            # Non-zero exit isn't automatically a failure — the legacy
            # binary sometimes exits non-zero on expected-error fixtures.
            # The diff against the reference .lst is the real oracle.
            :
        fi

        norm_got="$work_dir/$name.got.norm"
        norm_ref="$work_dir/$name.ref.norm"
        "$NORMALIZE" < "$got_lst" > "$norm_got"
        "$NORMALIZE" < "$ref_lst" > "$norm_ref"

        # Optional per-example deviation allow-list: an extra sed script
        # applied to the reference side only, to document accepted
        # divergences (e.g. an error-message wording change predating the
        # harness).  Every .sed file MUST have a sibling .md explaining
        # why the deviation is accepted.
        dev_sed="$dev_dir/$name.sed"
        if [ -f "$dev_sed" ]; then
            sed -E -f "$dev_sed" < "$norm_ref" > "$norm_ref.patched"
            mv "$norm_ref.patched" "$norm_ref"
        fi

        if diff -q "$norm_got" "$norm_ref" >/dev/null 2>&1; then
            echo "  PASS: $name"
            PASSED=$((PASSED + 1))
        else
            echo "  FAIL: $name"
            echo "    diff (reference vs. got):"
            # Two non-zero exits to tolerate under `set -euo pipefail`:
            #   1. `diff -u` returns 1 whenever the files differ (the
            #      whole reason we're in this branch).
            #   2. `sed` takes SIGPIPE (exit 141) when `head -40`
            #      closes its stdin early on long diffs, which
            #      `pipefail` would otherwise propagate and abort the
            #      harness on the first FAIL.
            # Wrap the entire pipeline in a `|| true` so both are
            # neutralised and subsequent mismatches still get reported.
            { { diff -u "$norm_ref" "$norm_got" || true; } \
                | sed 's/^/      /' \
                | head -40; } || true
            FAILED=$((FAILED + 1))
        fi
    done
    rm -rf "$work_dir"
}

# ------------------------------------------------------------------ #
# Main                                                                #
# ------------------------------------------------------------------ #

# Toolchain-bucket auto-select (see REFERENCE defaults above).  We no
# longer blanket-skip non-Darwin/Linux hosts now that the two-bucket
# model is in place: Linux + Windows/MinGW both map to the gcc bucket
# (v4.3.0), macOS/arm64 maps to the clang-apple bucket.  The one
# remaining gap is a v4.4.x Linux recapture that would let Linux builds
# byte-match a v4.4.x reference without banner/org normalisation — until
# then, Linux + Windows runs against v4.3.0 will diff on the banner +
# org string even when numerics match, so CI callers either pin
# REFERENCE explicitly or accept those expected diffs.  Set
# HZCORPUS_FORCE=1 to suppress the warning.
HOST_KERNEL="$(uname -s 2>/dev/null || echo unknown)"
HOST_MACH="$(uname -m 2>/dev/null || echo unknown)"
case "$HOST_KERNEL/$HOST_MACH" in
    Darwin/arm64|Linux/*)
        :  # Supported toolchain buckets
        ;;
    *)
        if [ "${HZCORPUS_FORCE:-0}" != "1" ]; then
            echo ""
            echo "=========================================="
            echo "Acceptance corpus — not yet exercised on $HOST_KERNEL/$HOST_MACH"
            echo "=========================================="
            echo "  Bucket auto-detection covers Darwin/arm64 and Linux/*."
            echo "  Windows/MinGW should work via REFERENCE=v4.3.0 but"
            echo "  hasn't been run through this harness end-to-end."
            echo "  Set HZCORPUS_FORCE=1 to run anyway."
            echo "=========================================="
            exit 0
        fi
        ;;
esac

run_kind "hazard"  "$HAZARD_BIN"
run_kind "hazpred" "$HAZPRED_BIN"

echo ""
echo "=========================================="
echo "Acceptance corpus summary"
echo "  passed  : $PASSED"
echo "  failed  : $FAILED"
echo "  skipped : $SKIPPED"
echo "=========================================="

# Skipped kinds don't fail the run — they're expected during the
# scaffolding phase.  Only real regressions (FAIL) are fatal.
[ "$FAILED" -eq 0 ]
