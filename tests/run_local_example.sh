#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

usage() {
  cat <<'EOF'
Usage: tests/run_local_example.sh <hazard|hazpred> [--keep]

Runs a minimal local fixture using the built executable.

Arguments:
  hazard    Run src/hazard/hazard.exe with fixture tests/hzr.J24407.X32.*
  hazpred   Run src/hazpred/hazpred.exe with fixture tests/hzp.J7582.X55.*

Options:
  --keep    Keep the temporary staging directory

Environment overrides:
  HAZARD_BIN   Path to hazard executable (for hazard mode)
  HAZPRED_BIN  Path to hazpred executable (for hazpred mode)
EOF
}

if [[ $# -lt 1 ]]; then
  usage
  exit 1
fi

MODE="$1"
shift

KEEP=0
if [[ $# -gt 0 ]]; then
  if [[ "$1" == "--keep" ]]; then
    KEEP=1
  else
    usage
    exit 1
  fi
fi

case "${MODE}" in
  hazard)
    BIN="${HAZARD_BIN:-${REPO_DIR}/src/hazard/hazard.exe}"
    PREFIX="hzr.J24407.X32"
    REQUIRED=("${SCRIPT_DIR}/${PREFIX}.sas" "${SCRIPT_DIR}/${PREFIX}.dta")
    ;;
  hazpred)
    BIN="${HAZPRED_BIN:-${REPO_DIR}/src/hazpred/hazpred.exe}"
    PREFIX="hzp.J7582.X55"
    REQUIRED=("${SCRIPT_DIR}/${PREFIX}.sas" "${SCRIPT_DIR}/${PREFIX}.dta" "${SCRIPT_DIR}/${PREFIX}.haz")
    ;;
  *)
    usage
    exit 1
    ;;
esac

if [[ ! -x "${BIN}" ]]; then
  echo "ERROR: executable not found or not executable: ${BIN}" >&2
  exit 1
fi

for f in "${REQUIRED[@]}"; do
  if [[ ! -f "${f}" ]]; then
    echo "ERROR: required fixture missing: ${f}" >&2
    exit 1
  fi
done

# Keep the staging prefix short because legacy opnfils code uses fixed-size
# path buffers and can overflow with long TMPDIR values.
WORKDIR="$(mktemp -d "/tmp/hzlocal.XXXXXX")"
if [[ ${KEEP} -eq 0 ]]; then
  trap 'rm -rf "${WORKDIR}"' EXIT
fi

cp "${SCRIPT_DIR}/${PREFIX}.sas" "${WORKDIR}/${PREFIX}.sas"
cp "${SCRIPT_DIR}/${PREFIX}.dta" "${WORKDIR}/${PREFIX}.dta"
if [[ "${MODE}" == "hazpred" ]]; then
  cp "${SCRIPT_DIR}/${PREFIX}.haz" "${WORKDIR}/${PREFIX}.haz"
fi

LOGFILE="${WORKDIR}/${PREFIX}.run.lst"
EXPECTED_OUT="${WORKDIR}/${PREFIX}.haz"
if [[ "${MODE}" == "hazpred" ]]; then
  EXPECTED_OUT="${WORKDIR}/${PREFIX}.out"
fi

set +e
(
  cd "${WORKDIR}"
  TMPDIR="${WORKDIR}" "${BIN}" < "${PREFIX}.sas" > "${LOGFILE}" 2>&1
)
RUN_RC=$?
set -e

if [[ ! -f "${EXPECTED_OUT}" ]]; then
  echo "ERROR: expected output was not created: ${EXPECTED_OUT}" >&2
  echo "Check log: ${LOGFILE}" >&2
  exit 1
fi

echo "Run completed."
echo "  mode:       ${MODE}"
echo "  executable: ${BIN}"
echo "  workdir:    ${WORKDIR}"
echo "  exit code:  ${RUN_RC}"
echo "  log:        ${LOGFILE}"
echo "  output:     ${EXPECTED_OUT}"

if [[ ${KEEP} -eq 1 ]]; then
  echo "Temporary files kept (requested with --keep)."
fi
