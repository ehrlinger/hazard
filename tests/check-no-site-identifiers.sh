#!/usr/bin/env bash
#
# check-no-site-identifiers.sh — fail if a site hostname, internal server, or
# developer path has been committed.
#
# WHY: this repository is PUBLIC, and its test corpus is produced by capture
# scripts that record provenance (host, pwd, tmpdir, real_bin) from whichever
# machine ran them. Those writers redact by default, but a corpus captured
# before that default flipped — or with HAZARD_CAPTURE_REDACT=0 — will carry
# an internal hostname straight into a public commit. Reviewers do not read
# 67 .meta files, so this has to be mechanical.
#
# Scope: tracked files only. Untracked local captures are yours to keep.
#
# Usage: tests/check-no-site-identifiers.sh
# Exit:  0 = clean, 1 = at least one identifier found, 2 = not a git worktree.

set -uo pipefail
cd "$(dirname "$0")/.." || exit 2

# Scan tracked files via `git grep` when git is usable, else fall back to a
# plain recursive grep over the worktree. The fallback matters: the Windows
# MSYS2 job has no usable git, and a check that cannot run there is worse than
# useless — it would report "cannot check" on the one platform whose captures
# carry `C:\Users\...` paths. A CI checkout has no untracked files, so the two
# engines see the same set.
SELF="tests/check-no-site-identifiers.sh"
if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    ENGINE="git"
else
    ENGINE="grep"
    echo "note: git unavailable — scanning the worktree directly" >&2
fi

# Exit status is meaningful and must be preserved: grep returns 0 for
# "matched", 1 for "no match", and >1 for a real error (invalid regex,
# unreadable tree). Do NOT redirect stderr away here — the caller captures it,
# and an error that cannot be seen is an error that reads as "clean".
scan() {  # $1 = extended regex. 0 = matches, 1 = none, >1 = error
    if [ "${ENGINE}" = "git" ]; then
        git grep -nIE "$1" -- ":!${SELF}"
    else
        grep -rnIE --exclude-dir=.git --exclude="$(basename "${SELF}")" "$1" .
    fi
}

# One pattern per line: extended-regex, matched against tracked file contents.
PATTERNS=(
    'lri-[a-z0-9-]+\.lerner\.ccf\.org'   # internal CCF hosts (SAS, PPM, ...)
    '[a-z0-9-]+\.cchs\.net'              # internal CCF domain
    '<redacted-db>|<redacted-db>'                 # warehouse SQL Server instances
    '<redacted-path>|<redacted-path>'       # developer home paths
    '<redacted-user>'                     # Windows account / domain user
    '<redacted-host>'                        # developer workstation hostname
)

status=0
errfile="$(mktemp)"
trap 'rm -f "${errfile}"' EXIT

for pat in "${PATTERNS[@]}"; do
    # This file necessarily contains the patterns it guards — scan() excludes it.
    hits="$(scan "${pat}" 2>"${errfile}")"
    rc=$?
    case "${rc}" in
        0)
            echo "FAIL: site identifier committed (/${pat}/):"
            echo "${hits}" | sed 's/^/    /'
            status=1
            ;;
        1)  ;;   # no matches — the good case
        *)
            # A scan that errored has NOT proved the tree clean. Treating this
            # like "no match" is how a guard silently stops guarding.
            echo "ERROR: scan failed for /${pat}/ (exit ${rc}) — tree NOT verified:"
            sed 's/^/    /' "${errfile}"
            status=1
            ;;
    esac
done

if [ "${status}" -eq 0 ]; then
    echo "PASS: no site identifiers in tracked files"
else
    cat <<'MSG'

Re-capture with redaction on (it is the default):
    unset HAZARD_CAPTURE_REDACT
and scrub the offending lines before committing. This repo is public.
MSG
fi
exit "${status}"
