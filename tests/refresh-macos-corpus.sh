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
#   $ ./tests/refresh-macos-corpus.sh --kind hazpred --ref-name v4.4.2-macos-arm64
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

    if [[ ! -x "$bin" ]]; then
        echo "  SKIP: binary not found or not executable: $bin"
        return 0
    fi
    if [[ ! -d "$inputs_dir" ]]; then
        echo "  SKIP: no inputs dir at $inputs_dir"
        return 0
    fi

    shopt -s nullglob
    local inputs=("$inputs_dir"/*.input)
    shopt -u nullglob
    if [[ ${#inputs[@]} -eq 0 ]]; then
        echo "  SKIP: no *.input files in $inputs_dir"
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

    rm -rf "$ref_dir"
    mkdir -p "$ref_dir"

    local work_root; work_root="$(mktemp -d -t "refresh-macos-${kind}.XXXXXX")"
    trap 'rm -rf "$work_root"' RETURN

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

        # Capture any new .haz produced by the run (binary writes to TMPDIR;
        # we copy fresh files, ignoring the symlinked input fixtures).
        local out_haz
        for out_haz in "$run_tmp"/*.haz; do
            [[ -f "$out_haz" && ! -L "$out_haz" ]] || continue
            cp "$out_haz" "$ref_dir/$(basename "$out_haz")"
        done

        # Write .meta — matching scripts/capture-legacy.sh schema where it
        # makes sense; real_exit is honest (the binary's actual rc).
        {
            echo "# refresh-macos-corpus.sh metadata"
            echo "uid=$(new_uid)"
            echo "bin_kind=$kind"
            echo "real_bin=$bin"
            echo "real_exit=$rc"
            echo "argv="
            echo "timestamp=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
            echo "host=$(host_line)"
            echo "tmpdir=$run_tmp"
            echo "pwd=$(pwd)"
        } > "$ref_dir/$name.meta"

        if [[ -s "$got_lst" ]]; then
            success=$((success + 1))
            printf '  PASS: %-30s rc=%d  %d bytes\n' "$name" "$rc" "$(wc -c < "$got_lst")"
        else
            empty=$((empty + 1))
            printf '  EMPTY: %-30s rc=%d  (binary produced no output)\n' "$name" "$rc"
        fi
    done

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

if [[ "$kind" == "both" || "$kind" == "hazard" ]]; then
    capture_kind hazard "$hazard_bin" || overall_rc=1
fi
if [[ "$kind" == "both" || "$kind" == "hazpred" ]]; then
    capture_kind hazpred "$hazpred_bin" || overall_rc=1
fi

echo
if [[ $overall_rc -eq 0 ]]; then
    echo "Done. Run tests/validate_corpus.sh REFERENCE=$ref_name to confirm self-consistency."
else
    echo "Some inputs produced no output — check binary and inputs."
fi
exit $overall_rc
