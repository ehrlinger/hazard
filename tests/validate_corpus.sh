#!/usr/bin/env bash
#
# tests/validate_corpus.sh
#
# Acceptance harness: runs the current `hazard` (and `hazpred`) binaries
# against every captured example in tests/corpus/, normalises outputs via
# tests/corpus_normalize.sh, and byte-diffs against the v4.3.1 reference.
# Any per-example diff is a failure; the diff itself is printed so the
# operator can see what changed.
#
# Environment
# -----------
#   HAZARD_BIN    path to the hazard binary (default: ../src/hazard/hazard)
#   HAZPRED_BIN   path to the hazpred binary (default: ../src/hazpred/hazpred)
#   REFERENCE     reference version directory name (default: v4.4.2)
#                 Current binary self-consistency check — should pass
#                 trivially until someone modifies numerical code paths.
#                 Set REFERENCE=v4.3.0 to diff against the archived legacy
#                 (CCF captured) corpus; expect divergences per
#                 tests/corpus/FINDINGS.md §2.
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
REFERENCE="${REFERENCE:-v4.4.2}"

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
    trap 'rm -rf "$work_dir"' RETURN

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
        return 0
    fi

    shopt -s nullglob
    local inputs=("$inputs_dir"/*.input)
    shopt -u nullglob

    if [ ${#inputs[@]} -eq 0 ]; then
        echo "  SKIP: no .input files under $inputs_dir"
        SKIPPED=$((SKIPPED + 1))
        return 0
    fi

    if [ ! -x "$bin" ]; then
        echo "  SKIP: binary '$bin' not built"
        SKIPPED=$((SKIPPED + 1))
        return 0
    fi

    if [ ! -d "$ref_dir" ]; then
        # No references captured for this kind under this version —
        # treated as SKIP, same as the per-example case below.  Common
        # when a binary type is blocked (e.g. hazpred SIGSEGVs under
        # v4.4.2; see tests/corpus/FINDINGS.md §2b).
        echo "  SKIP: no $REFERENCE references captured for $kind ($ref_dir missing)"
        SKIPPED=$((SKIPPED + 1))
        return 0
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
        # hzr*.dta / hzr*.haz file lookups resolve against our captured
        # copies.  Use a SHORT symlinked TMPDIR under /tmp to avoid an
        # 80-byte path buffer overflow in opnfils.c (known legacy bug
        # in pre-v5.0 binaries).  Each example gets its own tmpdir so
        # concurrent runs don't collide, and symlinks are only created
        # for .dta / .haz files the binary needs.
        # Force /tmp (not /var/folders on macOS, where mktemp -t ignores
        # TMPDIR) so the full $TMPDIR/hzr.J<job>.X<ix>.dta path stays
        # under 80 bytes — otherwise the opnfils.c fixed buffer
        # overflows and SIGTRAPs.  Pass absolute template directly so
        # both GNU and BSD mktemp honour it.
        local short_tmp
        short_tmp="$(mktemp -d /tmp/hz.XXXXXXXX)"
        for f in "$inputs_dir"/hz?*.dta "$inputs_dir"/hz?*.haz; do
            [ -f "$f" ] || continue
            ln -sf "$f" "$short_tmp/$(basename "$f")"
        done
        if ! TMPDIR="$short_tmp" "$bin" < "$input" > "$got_lst" 2>&1; then
            # Non-zero exit isn't automatically a failure — the legacy
            # binary sometimes exits non-zero on expected-error fixtures.
            # The diff against the reference .lst is the real oracle.
            :
        fi
        rm -rf "$short_tmp"

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
            diff -u "$norm_ref" "$norm_got" | sed 's/^/      /' | head -40
            FAILED=$((FAILED + 1))
        fi
    done
}

# ------------------------------------------------------------------ #
# Main                                                                #
# ------------------------------------------------------------------ #

# The v4.4.2 self-consistency reference was captured on macOS/clang.
# Cross-platform float formatting and line-ending conventions differ
# enough between macOS, Linux, and MinGW/Windows that byte-exact
# comparison across platforms isn't meaningful without per-platform
# reference sets, which we don't yet have.  Skip the whole harness on
# non-host platforms until we capture more baselines.  Set
# HZCORPUS_FORCE=1 to override (useful locally when iterating the
# harness itself).
HOST_KERNEL="$(uname -s 2>/dev/null || echo unknown)"
case "$HOST_KERNEL" in
    Darwin|Linux)
        :  # Supported reference platforms (v4.4.2 was captured on Darwin,
           # Linux CI runs are byte-compatible on the examples we ship)
        ;;
    *)
        if [ "${HZCORPUS_FORCE:-0}" != "1" ]; then
            echo ""
            echo "=========================================="
            echo "Acceptance corpus — skipped on $HOST_KERNEL"
            echo "=========================================="
            echo "  v4.4.2 reference is macOS-captured; cross-platform"
            echo "  float formatting differs.  Set HZCORPUS_FORCE=1 to"
            echo "  run anyway (expect diffs)."
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
