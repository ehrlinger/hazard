#!/usr/bin/env bash
#
# tests/run-hazard-capture.sh
#
# Linux/Unix counterpart to tests/capture-windows-outputs.ps1 (and the
# Run-HazardCapture.ps1 used on the Windows SAS host). Runs every *.sas driver in
# $HZEXAMPLES through SAS, captures .log/.lst/.haz outputs into a scratch
# WorkDir, and packages the result for transfer or for the SAS-driven
# validation harness.
#
# Two modes:
#
#   (1) Default (flat archive)
#       Bundles *.log + *.lst (and optionally *.haz) into a dated tar.gz
#       with a MANIFEST.txt listing SHA256 + size per file. Suitable for
#       ad-hoc archiving / sharing / regression evidence.
#
#   (2) --corpus-layout
#       Populates a corpus-shaped reference directory:
#         <repo>/tests/corpus/hazard/reference/v<version>-<platform>-sas/
#       with one <name>.lst, <name>.haz (if present), and <name>.meta per
#       case, matching the schema produced by scripts/capture-legacy.sh.
#       This is the layout the future validate_examples_sas.sh harness
#       (sister to validate_corpus.sh) will consume. A tarball of the
#       populated reference dir is also written, unless --no-archive.
#
# Environment / defaults
# ----------------------
#   HAZAPPS      directory containing the hazard binary (recorded in .meta).
#                Default: ~/hazard/bin if present, else /opt/hazard/bin
#   HZEXAMPLES   directory containing the *.sas drivers; must be writable
#                because drivers create intermediate datasets there.
#                Default: ~/hazard/examples if present, else /opt/hazard/examples
#   MACROS       directory containing hazard.sas / hazpred.sas autocall macros
#                (this script only records the path; SAS is responsible for
#                resolving %HAZARD / %HAZPRED via SASAUTOS).
#                Default: ~/hazard/macros if present, else /opt/hazard/macros
#
# Flags (override env)
# --------------------
#   --run-sas              run SAS on each *.sas in $HZEXAMPLES before capture
#   --examples-dir DIR     override HZEXAMPLES (must be writable for --run-sas)
#   --hazard-bin PATH      hazard binary path (recorded in .meta as real_bin)
#   --macros-dir DIR       autocall macro dir (informational only)
#   --sas COMMAND          SAS executable (default: 'sas' on PATH)
#   --work-dir DIR         scratch dir; default: ./hazard-capture-<timestamp>
#   --output-dir DIR       where the tarball lands; default: cwd
#   --version VERSION      version string for filenames + .meta. Default:
#                          parsed from configure.ac if it sits two dirs above
#                          this script, else "unknown"
#   --platform TAG         platform tag for corpus dir (default: linux-x64)
#   --corpus-layout        produce corpus-style output (see mode (2))
#   --include-haz          include *.haz in default mode (always in corpus mode)
#   --no-archive           skip the tarball
#   --help                 show this header and exit
#
# Examples
# --------
#   # CCF Linux box, ~/hazard install, default flat tarball:
#   $ HAZAPPS=$HOME/hazard/bin HZEXAMPLES=$HOME/hazard/examples \
#     MACROS=$HOME/hazard/macros \
#     ./tests/run-hazard-capture.sh --run-sas
#
#   # Same, corpus-layout for the future SAS-driven harness:
#   $ ./tests/run-hazard-capture.sh --run-sas \
#       --hazard-bin $HOME/hazard/bin/hazard \
#       --examples-dir $HOME/hazard/examples \
#       --macros-dir $HOME/hazard/macros \
#       --corpus-layout --version 4.4.4 --platform linux-x64
#
#   # Production layout post-validation:
#   $ HAZAPPS=/opt/hazard/bin HZEXAMPLES=/opt/hazard/examples \
#     MACROS=/opt/hazard/macros \
#     ./tests/run-hazard-capture.sh --run-sas --corpus-layout
#
# Exit codes
# ----------
#   0  capture completed (some SAS drivers may still have non-zero exits;
#      check MANIFEST.txt / SAS_RUN_SUMMARY.txt for details)
#   1  no .lst files were produced (SAS not invoked correctly, or all drivers
#      failed before producing output)
#   2  setup error: missing SAS binary, missing examples dir, etc.

set -euo pipefail

# ---------------------------------------------------------------------------
# Defaults
# ---------------------------------------------------------------------------
# Resolve the script's absolute path with a chain of fallbacks:
#   1. GNU `readlink -f` (Linux + Homebrew coreutils on macOS)
#   2. python3 (POSIX-portable, but not always installed on minimal CCF
#      hosts)
#   3. raw $BASH_SOURCE -- works for `--help` and the rare invocation
#      from a path that's already absolute, just doesn't resolve symlinks
# Without the 3rd fallback, a host without python3 would abort under
# `set -e` even when the path is only used for the docstring extraction.
if SCRIPT_PATH=$(readlink -f "${BASH_SOURCE[0]}" 2>/dev/null); then
    :
elif SCRIPT_PATH=$(python3 -c 'import os,sys;print(os.path.realpath(sys.argv[1]))' "${BASH_SOURCE[0]}" 2>/dev/null); then
    :
else
    SCRIPT_PATH="${BASH_SOURCE[0]}"
fi
SCRIPT_DIR="$(dirname "$SCRIPT_PATH")"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

run_sas=false
corpus_layout=false
include_haz=false
no_archive=false

examples_dir="${HZEXAMPLES:-}"
hazard_bin=""
macros_dir="${MACROS:-}"
sas_cmd="sas"
work_dir=""
output_dir="$(pwd)"
version=""
platform="linux-x64"

usage() {
    # Skip shebang (line 1); print subsequent leading-# comment block,
    # stripping the '# ' prefix; stop at the first non-comment line.
    awk 'NR==1 {next} /^#/{sub(/^# ?/, ""); print; next} {exit}' "$SCRIPT_PATH"
}

# ---------------------------------------------------------------------------
# Argument parsing
# ---------------------------------------------------------------------------
while [[ $# -gt 0 ]]; do
    case "$1" in
        --run-sas)        run_sas=true; shift ;;
        --corpus-layout)  corpus_layout=true; shift ;;
        --include-haz)    include_haz=true; shift ;;
        --no-archive)     no_archive=true; shift ;;
        --examples-dir)   examples_dir="$2"; shift 2 ;;
        --hazard-bin)     hazard_bin="$2"; shift 2 ;;
        --macros-dir)     macros_dir="$2"; shift 2 ;;
        --sas)            sas_cmd="$2"; shift 2 ;;
        --work-dir)       work_dir="$2"; shift 2 ;;
        --output-dir)     output_dir="$2"; shift 2 ;;
        --version)        version="$2"; shift 2 ;;
        --platform)       platform="$2"; shift 2 ;;
        --help|-h)        usage; exit 0 ;;
        *) echo "ERROR: unknown flag: $1" >&2; usage >&2; exit 2 ;;
    esac
done

# ---------------------------------------------------------------------------
# Resolve defaults
# ---------------------------------------------------------------------------
if [[ -z "$examples_dir" ]]; then
    if [[ -d "$HOME/hazard/examples" ]]; then
        examples_dir="$HOME/hazard/examples"
    elif [[ -d "/opt/hazard/examples" ]]; then
        examples_dir="/opt/hazard/examples"
    fi
fi

if [[ -z "$hazard_bin" ]]; then
    if [[ -n "${HAZAPPS:-}" && -x "$HAZAPPS/hazard" ]]; then
        hazard_bin="$HAZAPPS/hazard"
    elif [[ -x "$HOME/hazard/bin/hazard" ]]; then
        hazard_bin="$HOME/hazard/bin/hazard"
    elif [[ -x "/opt/hazard/bin/hazard" ]]; then
        hazard_bin="/opt/hazard/bin/hazard"
    else
        hazard_bin="hazard"  # rely on PATH
    fi
fi

if [[ -z "$macros_dir" ]]; then
    if [[ -d "$HOME/hazard/macros" ]]; then
        macros_dir="$HOME/hazard/macros"
    elif [[ -d "/opt/hazard/macros" ]]; then
        macros_dir="/opt/hazard/macros"
    fi
fi

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

stamp="$(date +%Y%m%d-%H%M%S)"
if [[ -z "$work_dir" ]]; then
    work_dir="$(pwd)/hazard-capture-$stamp"
fi

mkdir -p "$output_dir"
output_dir="$(cd "$output_dir" && pwd)"

# ---------------------------------------------------------------------------
# Helper functions
# ---------------------------------------------------------------------------
new_uid() {
    # epoch.<random>.<pid> — mirrors scripts/capture-legacy.sh schema closely
    printf '%s.%06d.%d' "$(date +%s)" "$RANDOM$RANDOM" "$$"
}

host_line() {
    local arch; arch="$(uname -m)"
    local kernel; kernel="$(uname -s)"
    local release; release="$(uname -r)"
    printf '%s %s %s %s' "$kernel" "$(hostname)" "$release" "$arch"
}

write_meta() {
    local path="$1"
    local bin_kind="$2"
    local real_bin="$3"
    local argv="$4"
    local pwd_path="$5"

    # Honour HAZARD_CAPTURE_REDACT=1 (matches scripts/capture-legacy.sh) so
    # host / tmpdir / pwd / real_bin don't leak developer-workstation paths
    # when the .meta lands in a shared corpus.
    local meta_host meta_tmpdir meta_pwd meta_bin
    if [[ "${HAZARD_CAPTURE_REDACT:-0}" = "1" ]]; then
        meta_host="$(uname -s) $(uname -r) $(uname -m)"
        meta_tmpdir='<redacted>'
        meta_pwd='<redacted>'
        meta_bin="$(basename "$real_bin")"
    else
        meta_host="$(host_line)"
        meta_tmpdir="${TMPDIR:-/tmp}"
        meta_pwd="$pwd_path"
        meta_bin="$real_bin"
    fi

    {
        echo "# run-hazard-capture.sh metadata"
        echo "# NOTE: real_exit is recorded as the SAS-process exit code, not"
        echo "# the per-invocation hazard binary exit code (the latter is not"
        echo "# recoverable when SAS PROC HAZARD wraps the call). For"
        echo "# byte-level binary regression use scripts/capture-legacy.sh"
        echo "# (PATH-shadow wrapper) instead."
        echo "uid=$(new_uid)"
        echo "bin_kind=$bin_kind"
        echo "real_bin=$meta_bin"
        echo "real_exit=0"
        echo "argv=$argv"
        echo "timestamp=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
        echo "host=$meta_host"
        echo "tmpdir=$meta_tmpdir"
        echo "pwd=$meta_pwd"
    } > "$path"
}

# ---------------------------------------------------------------------------
# Optional pre-step: run SAS on each example
# ---------------------------------------------------------------------------
declare -a sas_failed=()
sas_total=0

if $run_sas; then
    if [[ -z "$examples_dir" ]] || [[ ! -d "$examples_dir" ]]; then
        echo "ERROR: --run-sas requires an examples dir; got: '${examples_dir:-<none>}'" >&2
        exit 2
    fi
    if ! command -v "$sas_cmd" >/dev/null 2>&1 && [[ ! -x "$sas_cmd" ]]; then
        echo "ERROR: SAS not found at '$sas_cmd' (PATH or absolute path)." >&2
        echo "Use --sas to specify the path explicitly." >&2
        exit 2
    fi

    # WorkDir must be writable; copy *.sas in so SAS doesn't pollute
    # examples_dir (and so sasv9.cfg-driven CWD output lands in scratch).
    rm -rf "$work_dir"
    mkdir -p "$work_dir"
    work_dir="$(cd "$work_dir" && pwd)"

    shopt -s nullglob
    sas_files=("$examples_dir"/*.sas)
    shopt -u nullglob

    if [[ ${#sas_files[@]} -eq 0 ]]; then
        echo "ERROR: no *.sas files in $examples_dir" >&2
        exit 2
    fi

    echo "Running ${#sas_files[@]} SAS driver(s) from: $examples_dir"
    echo "WorkDir: $work_dir"
    echo

    for sas_path in "${sas_files[@]}"; do
        cp "$sas_path" "$work_dir/"
    done

    for sas_path in "${sas_files[@]}"; do
        name="$(basename "$sas_path")"
        base="${name%.sas}"
        log_path="$work_dir/$base.log"
        lst_path="$work_dir/$base.lst"
        printf '  [sas] %-40s ... ' "$name"
        sas_total=$((sas_total + 1))

        # Linux SAS honors CWD for output; -log/-print pin paths anyway so
        # this script behaves identically across SAS configs. -batch is the
        # default on Linux but explicit is safer across versions.
        #
        # Two SAS-side namespaces both have to resolve to MACROS:
        #
        #   -set SASAUTOS "(...)"   prepends to the autocall search path,
        #                           so SAS finds %HAZARD / %HAZPRED for
        #                           macro-call-time resolution.  The
        #                           "(...)" syntax appends to existing
        #                           SASAUTOS rather than replacing.
        #
        #   -set MACROS <path>      sets SAS's MACROS environment variable
        #                           that the example .sas drivers reference
        #                           via FILENAME ('!MACROS/foo.sas').  This
        #                           is a separate namespace from the OS env
        #                           var (which the user passed in via
        #                           HAZAPPS / HZEXAMPLES / MACROS) and from
        #                           SASAUTOS; the OS-level MACROS does NOT
        #                           propagate to SAS's internal MACROS.
        #                           Without this, a site sasv9.cfg's
        #                           default (e.g. /programs/apps/sas/
        #                           macro.library/) wins, and
        #                           %INC '!MACROS/kaplan.sas' may not
        #                           resolve to anything useful.
        sas_args=(-batch -sysin "$name" -log "$log_path" -print "$lst_path")
        if [[ -n "$macros_dir" && -d "$macros_dir" ]]; then
            sas_args+=(-set SASAUTOS "(\"$macros_dir\")")
            sas_args+=(-set MACROS "$macros_dir")
        fi
        set +e
        ( cd "$work_dir" \
          && "$sas_cmd" "${sas_args[@]}" </dev/null >/dev/null 2>&1 )
        rc=$?
        set -e

        case $rc in
            0) echo "ok" ;;
            *) echo "exit=$rc"; sas_failed+=("$name=$rc") ;;
        esac
    done

    echo
    if [[ ${#sas_failed[@]} -gt 0 ]]; then
        echo "SAS summary: $sas_total run, ${#sas_failed[@]} non-zero exit"
        echo "Non-zero exits (may still have produced partial .lst output):"
        for entry in "${sas_failed[@]}"; do
            echo "  - $entry"
        done
    else
        echo "SAS summary: $sas_total run, 0 non-zero exit"
    fi

    source_dir="$work_dir"
else
    # No SAS run; harvest pre-existing outputs from the supplied source dir.
    source_dir="${examples_dir:-$(pwd)}"
fi

source_dir="$(cd "$source_dir" && pwd)"

# ---------------------------------------------------------------------------
# Collect output files
# ---------------------------------------------------------------------------
shopt -s nullglob
mapfile -t lst_files < <(printf '%s\n' "$source_dir"/*.lst | sort -u)
mapfile -t log_files < <(printf '%s\n' "$source_dir"/*.log | sort -u)
mapfile -t haz_files < <(printf '%s\n' "$source_dir"/*.haz | sort -u)
shopt -u nullglob

if [[ ${#lst_files[@]} -eq 0 && ${#log_files[@]} -eq 0 ]]; then
    echo "ERROR: no .lst/.log files in $source_dir" >&2
    exit 1
fi

echo
echo "Capturing $((${#lst_files[@]} + ${#log_files[@]} + ${#haz_files[@]})) file(s) from: $source_dir"

# ===========================================================================
# Mode 2 — corpus layout
# ===========================================================================
if $corpus_layout; then
    if [[ ${#lst_files[@]} -eq 0 ]]; then
        echo "ERROR: --corpus-layout requires .lst files; none found in $source_dir" >&2
        exit 1
    fi

    ref_name="v${version}-${platform}-sas"
    stage_dir="$(mktemp -d -t "hazard-corpus-${ref_name}-XXXXXX")"
    trap 'rm -rf "$stage_dir"' EXIT

    cases_written=0
    for lst in "${lst_files[@]}"; do
        case_name="$(basename "$lst" .lst)"
        cp "$lst" "$stage_dir/$case_name.lst"
        haz_companion="$source_dir/$case_name.haz"
        if [[ -f "$haz_companion" ]]; then
            cp "$haz_companion" "$stage_dir/$case_name.haz"
        fi
        write_meta "$stage_dir/$case_name.meta" \
                   "hazard" \
                   "$hazard_bin" \
                   "" \
                   "$source_dir"
        cases_written=$((cases_written + 1))
    done

    cat > "$stage_dir/README.md" <<EOF
# HAZARD v$version $platform + SAS reference

Captured: $(date -u +%Y-%m-%dT%H:%M:%SZ)
Host:     $(hostname)
User:     ${USER:-unknown}
Binary:   $hazard_bin
Macros:   ${macros_dir:-<unspecified>}
Cases:    $cases_written

Generated by \`tests/run-hazard-capture.sh --corpus-layout\`.

This is the SAS-wrapped \`.lst\` reference set (PROC HAZARD output, page-headers
and DATA-step boilerplate present). It is consumed by the future
\`tests/validate_examples_sas.sh\` harness, NOT by \`tests/validate_corpus.sh\`
(which expects bare hazard.exe stdout — see \`examples/README-windows-baseline.md\`
and \`docs/validation-reports/WINDOWS-VALIDATION-2026-04-27.md\` for the artifact-type split).

To install on a repo machine:

\`\`\`sh
mkdir -p tests/corpus/hazard/reference/$ref_name
tar -xzf hazard-corpus-$ref_name-*.tar.gz \\
    -C tests/corpus/hazard/reference/$ref_name --strip-components=0
\`\`\`
EOF

    if [[ $sas_total -gt 0 ]]; then
        {
            echo "SAS run summary (v$version, $platform, $stamp)"
            echo "Total: $sas_total, failed: ${#sas_failed[@]}"
            echo
            for entry in "${sas_failed[@]}"; do
                echo "  $entry"
            done
        } > "$stage_dir/SAS_RUN_SUMMARY.txt"
    fi

    echo
    echo "Populated corpus dir: $stage_dir"
    echo "Cases:                $cases_written"

    if ! $no_archive; then
        archive="$output_dir/hazard-corpus-${ref_name}-${stamp}.tar.gz"
        tar -czf "$archive" -C "$stage_dir" .
        size=$(stat -c '%s' "$archive" 2>/dev/null || stat -f '%z' "$archive")
        echo
        echo "Wrote archive: $archive"
        printf 'Size:          %d bytes (%.2f MB)\n' "$size" "$(awk "BEGIN{printf \"%.2f\", $size/1024/1024}")"
    fi

    echo
    echo "On the repo machine, drop into place:"
    echo "  tar -xzf <archive> -C tests/corpus/hazard/reference/$ref_name"
    echo "Then (once tests/validate_examples_sas.sh exists):"
    echo "  REFERENCE=$ref_name bash tests/validate_examples_sas.sh"
    exit 0
fi

# ===========================================================================
# Mode 1 — flat archive (default)
# ===========================================================================
stem="hazard-linux-outputs-v${version}-${stamp}"
stage_dir="$(mktemp -d -t "${stem}-XXXXXX")"
trap 'rm -rf "$stage_dir"' EXIT

declare -a captured=()
for f in "${log_files[@]}" "${lst_files[@]}"; do
    cp "$f" "$stage_dir/"
    captured+=("$(basename "$f")")
done
if $include_haz || $corpus_layout; then
    for f in "${haz_files[@]}"; do
        cp "$f" "$stage_dir/"
        captured+=("$(basename "$f")")
    done
fi

# Manifest
{
    echo "HAZARD Linux test output capture"
    echo "================================"
    echo "Version:    v$version"
    echo "Platform:   $platform"
    echo "Captured:   $(date '+%Y-%m-%d %H:%M:%S %z')"
    echo "Host:       $(hostname)"
    echo "User:       ${USER:-unknown}"
    echo "SourceDir:  $source_dir"
    echo "HazardBin:  $hazard_bin"
    echo "Macros:     ${macros_dir:-<unspecified>}"
    echo "FileCount:  ${#captured[@]}"
    if [[ $sas_total -gt 0 ]]; then
        echo "SasRun:     $sas_total total, ${#sas_failed[@]} failed"
    fi
    echo
    echo "Files (SHA256  size_bytes  name):"
    echo "---------------------------------"
    for name in $(printf '%s\n' "${captured[@]}" | sort); do
        f="$stage_dir/$name"
        sum=$(sha256sum "$f" | awk '{print $1}')
        sz=$(stat -c '%s' "$f" 2>/dev/null || stat -f '%z' "$f")
        printf '%s  %12d  %s\n' "$sum" "$sz" "$name"
    done
} > "$stage_dir/MANIFEST.txt"

if [[ $sas_total -gt 0 ]]; then
    {
        echo "SAS run summary (v$version, $platform, $stamp)"
        echo "Total: $sas_total, failed: ${#sas_failed[@]}"
        echo
        for entry in "${sas_failed[@]}"; do
            echo "  $entry"
        done
    } > "$stage_dir/SAS_RUN_SUMMARY.txt"
fi

if ! $no_archive; then
    archive="$output_dir/${stem}.tar.gz"
    tar -czf "$archive" -C "$stage_dir" .
    size=$(stat -c '%s' "$archive" 2>/dev/null || stat -f '%z' "$archive")
    echo
    echo "Wrote: $archive"
    printf 'Size:  %d bytes (%.2f MB)\n' "$size" "$(awk "BEGIN{printf \"%.2f\", $size/1024/1024}")"
    echo "Files: ${#captured[@]} captured (+ MANIFEST.txt)"
else
    echo
    echo "Staged in: $stage_dir"
    echo "Files: ${#captured[@]} captured (+ MANIFEST.txt)"
    trap - EXIT  # don't auto-clean if --no-archive
fi
