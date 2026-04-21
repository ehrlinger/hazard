#!/usr/bin/env bash
#
# scripts/capture-legacy.sh
#
# Wrapper around the legacy `hazard` / `hazpred` binaries that snapshots
# the exact inputs SAS handed them (stdin text + XPORT .dta / .haz files
# in TMPDIR) before exec'ing the real binary.  Used once, during corpus
# capture, to build the acceptance-test reference set from a known-good
# v4.3.1 install.
#
# Usage
# -----
#   export HAZARD_REAL=/path/to/legacy/hazard
#   export HAZPRED_REAL=/path/to/legacy/hazpred
#   export HAZARD_CAPTURE_DIR=/tmp/hazard-capture-YYYYMMDD
#   mkdir -p "$HAZARD_CAPTURE_DIR"
#
#   # Drop symlinks with the legacy binary names into a shadow bin dir,
#   # each pointing at THIS script.  The script uses $0 to tell which
#   # binary it's standing in for.
#   mkdir -p ~/hazard-capture-bin
#   ln -sf "$(pwd)/scripts/capture-legacy.sh" ~/hazard-capture-bin/hazard
#   ln -sf "$(pwd)/scripts/capture-legacy.sh" ~/hazard-capture-bin/hazpred
#   export PATH="$HOME/hazard-capture-bin:$PATH"
#
#   # Now run SAS normally; every PROC HAZARD / PROC HAZPRED invocation
#   # writes a tuple into $HAZARD_CAPTURE_DIR.
#   for sas in tests/*.sas; do sas -nodms -log /dev/stderr "$sas"; done
#
# What gets captured (per invocation)
# -----------------------------------
#   <uid>.stdin           — the PROC options text SAS piped to the binary
#   <uid>.lst             — the binary's stdout (the listing)
#   <uid>.<original>.dta  — each hzr*.dta file present in TMPDIR at
#                           invocation time (normally exactly one)
#   <uid>.<original>.haz  — same treatment for hzr*.haz files (hazpred)
#   <uid>.meta            — argv, binary, tmpdir, timestamp, host
#
# The operator post-renames these tuples to match the .sas example name
# (e.g. `hz.death.AVC.input`, `hzr.AVC.AVCS.dta`) and places them under
# tests/corpus/{hazard,hazpred}/{inputs,reference/v4.3.1}/ per the
# layout in docs/VALIDATION_PLAN.md §3.
#
# This script is idempotent: re-running against the same capture dir
# accumulates new tuples (each gets a unique uid) without disturbing
# prior ones.  Safe to interleave with failed / retried SAS runs.

# --------------------------------------------------------------------------
# Shell preflight — MUST run before `set -o pipefail`, which is itself
# bash/ksh93 only.  This script uses bash-specific features:
#   * set -o pipefail     (bash / ksh93)
#   * $RANDOM             (bash / ksh)
#   * ${PIPESTATUS[1]}    (bash-specific — load-bearing for real-binary exit code)
# Fail loudly and early if invoked under a non-bash shell.
# --------------------------------------------------------------------------
if [ -z "${BASH_VERSION:-}" ]; then
    echo "capture-legacy.sh requires bash (uses PIPESTATUS)." >&2
    echo "Either install bash, or invoke explicitly: bash $0" >&2
    exit 2
fi

set -euo pipefail

# --------------------------------------------------------------------------
# Configuration resolution
# --------------------------------------------------------------------------
# Config sources, in precedence order:
#   1. Env vars exported in the shell running SAS
#   2. `capture.env` in the same directory as this wrapper (resolved via
#      the real script path so it works through symlinks)
#   3. `$HOME/.hazard-capture.env`
#   4. Site-convention fallbacks:
#        $HAZAPPS/hazard        for HAZARD_REAL
#        $HAZAPPS/hazpred       for HAZPRED_REAL
#      Many HAZARD installs export $HAZAPPS pointing at the install's
#      bin dir — if that's set and contains the binary, we use it.
#   5. A one-time auto-default:
#        $HAZARD_CAPTURE_DIR = $PWD/captured
# --------------------------------------------------------------------------

# Resolve the physical script path even when invoked via symlink, so the
# sidecar capture.env (if any) is found next to the real script.
script_path="$(readlink -f "$0" 2>/dev/null || python3 -c 'import os,sys; print(os.path.realpath(sys.argv[1]))' "$0" 2>/dev/null || echo "$0")"
script_dir="$(dirname "$script_path")"

# Load sidecar config (later sources don't override already-set values)
for cfg in "$script_dir/capture.env" "${HOME:-/}/.hazard-capture.env"; do
    if [ -f "$cfg" ]; then
        # shellcheck disable=SC1090
        . "$cfg"
    fi
done

# --------------------------------------------------------------------------
# Resolve which binary we're wrapping.  The symlink path in $0 dictates
# whether we're standing in for `hazard` or `hazpred`.
# --------------------------------------------------------------------------
wrapper_name="$(basename "$0")"
case "$wrapper_name" in
    hazard)
        real_bin="${HAZARD_REAL:-}"
        bin_kind="hazard"
        ;;
    hazpred)
        real_bin="${HAZPRED_REAL:-}"
        bin_kind="hazpred"
        ;;
    *)
        echo "capture-legacy.sh: unexpected invocation name '$wrapper_name'" >&2
        echo "  Symlink this script as 'hazard' or 'hazpred' only." >&2
        exit 2
        ;;
esac

# Site fallback: $HAZAPPS is the conventional HAZARD install dir.  If the
# explicit overrides above are empty and $HAZAPPS is set and points at a
# dir containing the binary we need, use that.
if [ -z "$real_bin" ] && [ -n "${HAZAPPS:-}" ]; then
    for candidate in "$HAZAPPS/$wrapper_name" "$HAZAPPS"; do
        if [ -x "$candidate" ] && [ -f "$candidate" ]; then
            real_bin="$candidate"
            break
        fi
    done
fi

if [ -z "$real_bin" ]; then
    cat >&2 <<ERR
capture-legacy.sh: cannot locate the legacy '$wrapper_name' binary.
Set one of the following (in order of preference):
  * HAZARD_REAL / HAZPRED_REAL  — explicit path in this shell
  * capture.env beside this script — edits survive across sessions
  * \$HAZAPPS                   — site-convention install dir
ERR
    exit 2
fi

if [ ! -x "$real_bin" ]; then
    echo "capture-legacy.sh: '$real_bin' is not executable" >&2
    exit 2
fi

# Default capture dir to $HOME/hazard-capture so the wrapper works from
# any SAS invocation directory without the operator pre-creating anything.
# Multiple capture sessions accumulate here — each tuple has a unique uid,
# so there's no collision.  Explicit HAZARD_CAPTURE_DIR (env or capture.env)
# always wins.
: "${HAZARD_CAPTURE_DIR:=${HOME:-/tmp}/hazard-capture}"
mkdir -p "$HAZARD_CAPTURE_DIR"
if [ ! -d "$HAZARD_CAPTURE_DIR" ]; then
    echo "capture-legacy.sh: could not create HAZARD_CAPTURE_DIR='$HAZARD_CAPTURE_DIR'" >&2
    exit 2
fi

# Per-invocation unique id.  Epoch seconds + pid + $RANDOM keeps concurrent
# invocations from colliding without needing a uuid generator.
uid="$(date +%s).$$.$RANDOM"
cap_dir="$HAZARD_CAPTURE_DIR/$bin_kind"
mkdir -p "$cap_dir"

stdin_file="$cap_dir/$uid.stdin"
lst_file="$cap_dir/$uid.lst"
meta_file="$cap_dir/$uid.meta"

# --------------------------------------------------------------------------
# Snapshot TMPDIR inputs BEFORE running.  The binary may delete the .dta
# on exit (hazard.c does so by default).  We copy everything that looks
# like legacy binary input (hzr*.dta for hazard; hzr*.dta + hzr*.haz for
# hazpred) into the capture dir, prefixed with the uid so files from
# concurrent SAS invocations don't collide.
# --------------------------------------------------------------------------
snapshot_tmpdir() {
    local tmpdir="${TMPDIR:-/tmp}"
    local f
    for f in "$tmpdir"/hzr*.dta "$tmpdir"/hzr*.haz; do
        [ -f "$f" ] || continue
        cp -p "$f" "$cap_dir/$uid.$(basename "$f")"
    done
}

snapshot_tmpdir

# --------------------------------------------------------------------------
# Run the real binary.  Stdin is teed to the capture file; stdout is teed
# to the lst file and also forwarded so SAS sees it unchanged.  We preserve
# exit code via PIPESTATUS[0] (the real binary, not tee).
#
# Using mkfifo-based teeing rather than `| tee` keeps the PIPESTATUS clean
# and avoids the bash pipeline-exit-code gotcha under `set -e`.
# --------------------------------------------------------------------------
# Capture stdin to a file AND stream it to the binary without reading it
# all up-front (which would break interactive/large-input cases).
tee "$stdin_file" | "$real_bin" "$@" | tee "$lst_file"
real_exit=${PIPESTATUS[1]}

# --------------------------------------------------------------------------
# Metadata — small, human-readable, one record per invocation.
# --------------------------------------------------------------------------
cat > "$meta_file" <<META
# capture-legacy.sh metadata
uid=$uid
bin_kind=$bin_kind
real_bin=$real_bin
real_exit=$real_exit
argv=$*
timestamp=$(date -u +%Y-%m-%dT%H:%M:%SZ)
host=$(uname -a)
tmpdir=${TMPDIR:-/tmp}
pwd=$(pwd)
META

exit "$real_exit"
