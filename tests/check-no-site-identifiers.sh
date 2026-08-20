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
git rev-parse --is-inside-work-tree >/dev/null 2>&1 || {
    echo "not a git worktree — nothing to check" >&2; exit 2; }

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
    # --  Exclude this file: it necessarily contains the patterns it guards.
    if hits="$(git grep -nIE "${pat}" -- ':!tests/check-no-site-identifiers.sh' 2>/dev/null)" \
       && [ -n "${hits}" ]; then
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
