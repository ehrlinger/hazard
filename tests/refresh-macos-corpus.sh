#!/usr/bin/env bash
#
# tests/refresh-macos-corpus.sh
#
# Direct-binary capture for the macOS / Apple-clang reference corpus.
# Walks tests/corpus/<kind>/inputs/*.input, runs the local hazard /
# hazpred binary against each under a per-example TMPDIR seeded with the
# captured .dta / .haz fixtures (mirroring the validate_corpus.sh setup),
# and writes the resulting .lst (+ any new .haz output) plus a .meta file
# into tests/corpus/<kind>/reference/<ref-name>/.
#
# Closes the historical hazpred-on-macOS gap (FINDINGS.md §2b: the SIGSEGV
# fix landed 2026-04-23 but the reference set was never captured) and
# optionally refreshes the hazard bucket under a v4.4.x version label.
#
# Defaults
# --------
#   --version    parsed from configure.ac if available, else "unknown"
#   --ref-name   v<version>-macos-arm64
#   --kind       both (hazard + hazpred); also accepts hazard | hazpred
#   --hazard-bin  ../src/hazard/hazard
#   --hazpred-bin ../src/hazpred/hazpred
#   --force      overwrite an existing reference dir without prompting
#   --dry-run    list what would be captured, don't write
#
# Examples
# --------
#   # Refresh both kinds under v4.4.5-macos-arm64:
#   $ ./tests/refresh-macos-corpus.sh --version 4.4.5
#
#   # Capture only hazpred to close the v4.4.2 gap:
#   $ ./tests/refresh-macos-corpus.sh --kind hazpred --ref-name v4.4.4-macos-arm64
#
# Exit codes
# ----------
#   0  capture wrote the requested reference dirs
#   1  one or more inputs produced no .lst (binary failed before output)
#   2  setup error (missing binary, missing inputs dir)

set -euo pipefail

SCRIPT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/$(basename "${BASH_SOURCE[0]}")"
SCRIPT_DIR="$(dirname "$SCRIPT_PATH")"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
CORPUS_DIR="$SCRIPT_DIR/corpus"

version=""
ref_name=""
kind="both"
hazard_bin="$REPO_ROOT/src/hazard/hazard"
hazpred_bin="$REPO_ROOT/src/hazpred/hazpred"
force=false
dry_run=false

usage() {
    awk 'NR==1 {next} /^#/{sub(/^# ?/, ""); print; next} {exit}' "$SCRIPT_PATH"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --version)      version="$2"; shift 2 ;;
        --ref-name)     ref_name="$2"; shift 2 ;;
        --kind)         kind="$2"; shift 2 ;;
        --hazard-bin)   hazard_bin="$2"; shift 2 ;;
        --hazpred-bin)  hazpred_bin="$2"; shift 2 ;;
        --force)        force=true; shift ;;
        --dry-run)      dry_run=true; shift ;;
        --help|-h)      usage; exit 0 ;;
        *) echo "ERROR: unknown flag: $1" >&2; usage >&2; exit 2 ;;
    esac
done

# Resolve version + ref-name defaults
if [[ -z "$version" ]]; then
    if [[ -f "$REPO_ROOT/configure.ac" ]]; then
        # Skip commented-out AC_INIT lines (the file conventionally keeps
        # a `# AC_INIT([Hazard_Package],[4.3.0])` historical reference
        # above the active AC_INIT).
        version=$(grep -v '^[[:space:]]*#' "$REPO_ROOT/configure.ac" \
                  | grep -oE 'AC_INIT\(\[Hazard_Package\],\[[0-9.]+\]' \
                  | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1 || true)
    fi
    : "${version:=unknown}"
fi
if [[ -z "$ref_name" ]]; then
    ref_name="v${version}-macos-arm64"
fi

if [[ "$kind" != "both" && "$kind" != "hazard" && "$kind" != "hazpred" ]]; then
    echo "ERROR: --kind must be 'hazard', 'hazpred', or 'both'; got '$kind'" >&2
    exit 2
fi

# Sanity check: this script is named macos and only meaningful on Darwin/arm64.
# Allow override via env for cross-platform testing, but warn loudly.
if [[ "$(uname -s)" != "Darwin" && "${ALLOW_NON_DARWIN:-0}" != "1" ]]; then
    echo "WARNING: not running on Darwin; ref-name '$ref_name' may mislabel the bucket." >&2
    echo "  Set ALLOW_NON_DARWIN=1 to suppress this warning." >&2
fi

new_uid() {
    printf '%s.%06d.%d' "$(date +%s)" "$RANDOM$RANDOM" "$$"
}

host_line() {
    printf '%s %s %s %s' "$(uname -s)" "$(hostname)" "$(uname -r)" "$(uname -m)"
}

# Capture one kind into its reference dir.
#
# Args:
#   $1  kind ("hazard" or "hazpred")
#   $2  binary path
capture_kind() {
    local kind="$1"
    local bin="$2"
    local inputs_dir="$CORPUS_DIR/$kind/inputs"
    local ref_dir="$CORPUS_DIR/$kind/reference/$ref_name"

    echo "==================================================="
    echo " $kind  ->  $ref_dir"
    echo "==================================================="

    # Setup-error conditions (rc=2 per script header docs): missing binary
    # or missing inputs dir is a real misconfiguration the caller probably
    # wants to know about, not a benign skip.  No-input-files in an existing
    # inputs dir IS still treated as a (rc=0) skip — that's a legitimate
    # "this kind has no captured corpus to refresh" state.
    if [[ ! -x "$bin" ]]; then
        echo "  ERROR: binary not found or not executable: $bin" >&2
        return 2
    fi
    if [[ ! -d "$inputs_dir" ]]; then
        echo "  ERROR: no inputs dir at $inputs_dir" >&2
        return 2
    fi

    shopt -s nullglob
    local inputs=("$inputs_dir"/*.input)
    shopt -u nullglob
    if [[ ${#inputs[@]} -eq 0 ]]; then
        echo "  SKIP: no *.input files in $inputs_dir (corpus not yet captured for $kind)"
        return 0
    fi

    if [[ -d "$ref_dir" ]] && ! $force && ! $dry_run; then
        echo "ERROR: reference dir exists: $ref_dir" >&2
        echo "  Re-run with --force to overwrite, or pick a different --ref-name." >&2
        exit 2
    fi

    if $dry_run; then
        echo "  [dry-run] would capture ${#inputs[@]} input(s) into $ref_dir"
        for input in "${inputs[@]}"; do
            echo "    - $(basename "$input" .input)"
        done
        return 0
    fi

    # Surgical cleanup: only delete the file types this script produces
    # (.lst and .meta).  Preserve any committed .haz fixtures, since this
    # direct-binary capture path does not reproduce them — they come from
    # the SAS PROC HAZARD wrapper used by scripts/capture-legacy.sh, which
    # writes OUTHAZ= bridging files that the present script can't replicate.
    # Wiping the whole ref dir would orphan those.
    mkdir -p "$ref_dir"
    shopt -s nullglob
    local stale
    for stale in "$ref_dir"/*.lst "$ref_dir"/*.meta; do rm -f -- "$stale"; done
    shopt -u nullglob

    # NOTE: bash `trap ... RETURN` is shell-scoped, not function-scoped --
    # it would persist past this function and fire on later returns with
    # $work_root out of scope.  Use explicit cleanup at every exit path
    # below, mirroring the pattern in tests/validate_corpus.sh.
    local work_root; work_root="$(mktemp -d -t "refresh-macos-${kind}.XXXXXX")"

    local empty=0 success=0
    for input in "${inputs[@]}"; do
        local name; name="$(basename "$input" .input)"
        local run_tmp="$work_root/$name"
        mkdir -p "$run_tmp"

        # Seed TMPDIR with .dta / .haz fixtures (symlinked, read-only).
        # Mirrors the run_kind logic in validate_corpus.sh.
        shopt -s nullglob
        local fixture
        for fixture in "$inputs_dir"/hzr.*.dta "$inputs_dir"/hzr.*.haz \
                       "$inputs_dir"/hzp.*.dta "$inputs_dir"/hzp.*.haz; do
            ln -sf "$fixture" "$run_tmp/$(basename "$fixture")"
        done
        shopt -u nullglob

        local got_lst="$ref_dir/$name.lst"
        local rc=0
        if ! TMPDIR="$run_tmp" "$bin" < "$input" > "$got_lst" 2>&1; then
            rc=$?
        fi

        # NOTE: deliberately not copying *.haz files the binary may write
        # into TMPDIR.  Direct-binary invocation produces hzr.J<id>.X<id>.haz
        # outputs with internal-job-id names that don't map to the
        # case-name <case>.haz convention used by the committed bridging
        # fixtures (which come from scripts/capture-legacy.sh's SAS PROC
        # HAZARD wrapper).  Copying them would orphan them under
        # different names than the committed reference set.  The committed
        # <case>.haz files are preserved by the surgical-cleanup pass above.

        # Write .meta — matching scripts/capture-legacy.sh schema where it
        # makes sense; real_exit is honest (the binary's actual rc).  Honour
        # HAZARD_CAPTURE_REDACT=1 for shared-corpus commits so host /
        # tmpdir / pwd / real_bin don't leak developer-workstation paths.
        local meta_host meta_tmpdir meta_pwd meta_bin
        if [[ "${HAZARD_CAPTURE_REDACT:-0}" = "1" ]]; then
            meta_host="$(uname -s) $(uname -r) $(uname -m)"
            meta_tmpdir='<redacted>'
            meta_pwd='<redacted>'
            meta_bin="$(basename "$bin")"
        else
            meta_host="$(host_line)"
            meta_tmpdir="$run_tmp"
            meta_pwd="$(pwd)"
            meta_bin="$bin"
        fi
        {
            echo "# refresh-macos-corpus.sh metadata"
            echo "uid=$(new_uid)"
            echo "bin_kind=$kind"
            echo "real_bin=$meta_bin"
            echo "real_exit=$rc"
            echo "argv="
            echo "timestamp=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
            echo "host=$meta_host"
            echo "tmpdir=$meta_tmpdir"
            echo "pwd=$meta_pwd"
        } > "$ref_dir/$name.meta"

        if [[ -s "$got_lst" ]]; then
            success=$((success + 1))
            printf '  PASS: %-30s rc=%d  %d bytes\n' "$name" "$rc" "$(wc -c < "$got_lst")"
        else
            empty=$((empty + 1))
            printf '  EMPTY: %-30s rc=%d  (binary produced no output)\n' "$name" "$rc"
        fi
    done

    # Explicit cleanup of the per-function work_root — not a trap RETURN,
    # see comment above the mktemp call.
    rm -rf "$work_root"

    echo
    echo "  $kind: $success captured, $empty empty"
    if [[ $empty -gt 0 ]]; then
        return 1
    fi
}

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------
echo "Refreshing macOS corpus reference"
echo "  Version:  v$version"
echo "  Ref name: $ref_name"
echo "  Kind:     $kind"
echo "  Repo:     $REPO_ROOT"
echo

overall_rc=0

# Preserve capture_kind's rc precisely (1 = empty captures, 2 = setup error)
# rather than collapsing all non-zero to 1.  A setup error in either kind
# should override an empty-captures rc from the other.
run_kind() {
    capture_kind "$1" "$2"
    local rc=$?
    if [[ $rc -gt $overall_rc ]]; then overall_rc=$rc; fi
}

if [[ "$kind" == "both" || "$kind" == "hazard" ]]; then
    run_kind hazard "$hazard_bin"
fi
if [[ "$kind" == "both" || "$kind" == "hazpred" ]]; then
    run_kind hazpred "$hazpred_bin"
fi

echo
case $overall_rc in
    0) echo "Done. Run tests/validate_corpus.sh REFERENCE=$ref_name to confirm self-consistency." ;;
    1) echo "Some inputs produced no output -- check binary and inputs." ;;
    2) echo "Setup error -- check that the binary path and inputs dir exist (see header)." ;;
esac
exit $overall_rc
