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

scan() {  # $1 = extended regex; prints matches, returns 1 when none
    if [ "${ENGINE}" = "git" ]; then
        git grep -nIE "$1" -- ":!${SELF}" 2>/dev/null
    else
        grep -rnIE --exclude-dir=.git --exclude="$(basename "${SELF}")" "$1" . 2>/dev/null
    fi
}

# One pattern per line: extended-regex, matched against tracked file contents.
PATTERNS=(
    'lri-[a-z0-9-]+\.lerner\.ccf\.org'   # internal CCF hosts (SAS, PPM, ...)
    '[a-z0-9-]+\.cchs\.net'              # internal CCF domain
    'ESQLPROD|ESQLPLDAG'                 # warehouse SQL Server instances
    '/home/ehrlinj|/Users/ehrlinj'       # developer home paths
    'John_Ehrlinger'                     # Windows account / domain user
    'ehrlinj4F71'                        # developer workstation hostname
)

status=0
for pat in "${PATTERNS[@]}"; do
    # This file necessarily contains the patterns it guards — scan() excludes it.
    if hits="$(scan "${pat}")" && [ -n "${hits}" ]; then
        echo "FAIL: site identifier committed (/${pat}/):"
        echo "${hits}" | sed 's/^/    /'
        status=1
    fi
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
