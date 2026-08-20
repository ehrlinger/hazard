#!/usr/bin/env bash
#
# check-no-site-identifiers.sh — fail if a site hostname, internal server, or
# unredacted capture provenance has been committed.
#
# WHY: this repository is PUBLIC, and its test corpus is produced by capture
# scripts that record provenance (host, pwd, tmpdir, real_bin) from whichever
# machine ran them. Those writers redact by default, but a corpus captured
# before that default flipped — or with HAZARD_CAPTURE_REDACT=0 — will carry an
# internal hostname straight into a public commit. Reviewers do not read 67
# .meta files, so this has to be mechanical.
#
# THIS FILE NAMES NOTHING INTERNAL, BY DESIGN. An earlier version listed the
# actual hostname, domain account and SQL instance names as literal patterns —
# publishing, in a tracked file of a public repo, the very strings a history
# rewrite had just been run to remove. A guard must not be the leak.
#
# So detection is structural wherever possible: the capture writers emit a
# fixed schema, and a redacted record has a known SHAPE. Asserting the shape
# beats listing forbidden values, and it catches a capture from a host nobody
# has ever enumerated — which is what a literal list can never do.
#
# Scope: tracked files only. Untracked local captures are yours to keep.
#
# Usage: tests/check-no-site-identifiers.sh
# Exit:  0 = clean
#        1 = an identifier or unredacted provenance field was found
#        2 = environment/setup failure (cannot cd, no scan engine)
#        3 = the guard itself is disarmed (a pattern was rewritten away)

set -uo pipefail
cd "$(dirname "$0")/.." || exit 2

# Scan tracked files via `git grep` when git is usable, else fall back to a
# plain recursive grep over the worktree. The fallback matters: the Windows
# MSYS2 job has no usable git, and a check that cannot run there is worse than
# useless — it would report "cannot check" on the one platform whose captures
# carry Windows paths. A CI checkout has no untracked files, so the two engines
# see the same set.
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

# Class patterns — each matches a category, never a specific site value.
#   1. any host under the organisation's INTERNAL domains. Deliberately not
#      bare `ccf.org`: the project's own public support address
#      (hazard@bio.ri.ccf.org) is in every copyright header and SAS log since
#      2000 — ~180 legitimate hits — so a bare-domain pattern is unusable. The
#      leading class also excludes an `@`, so mail addresses under the internal
#      domains do not trip it either.
#   2. warehouse SQL Server instances, by naming convention rather than name.
PATTERNS=(
    '(^|[^@A-Za-z0-9._-])[a-z0-9-]+\.(lerner\.ccf\.org|cchs\.net)'
    'ESQL[A-Z0-9]{2,}'
)

# Public, citable URLs that legitimately appear in AUTHORS.md / README.md and
# must not fail the scan. ERE has no negative lookahead, so allowlisting is a
# second pass over the hits rather than part of the pattern.
ALLOW='www\.lerner\.ccf\.org'

# Guard the guard. A `git filter-repo --replace-text` rewrite treats this file
# like any other and can rewrite a pattern into a placeholder — which happened
# on 2026-08-20, disarming four of six patterns while the scan went on
# reporting PASS. Class patterns are far less exposed (they contain no site
# literal to replace), but the check costs nothing and the failure it prevents
# is silent.
for pat in "${PATTERNS[@]}"; do
    case "${pat}" in
        *'<redacted'*|*'<internal-'*|*'<path-to'*)
            echo "FATAL: pattern '${pat}' has been replaced by a redaction placeholder." >&2
            echo "       This scan cannot detect what it was written to detect." >&2
            echo "       Restore the class patterns (see git history) before relying on it." >&2
            exit 3
            ;;
    esac
done

status=0
errfile="$(mktemp)"
trap 'rm -f "${errfile}"' EXIT

for pat in "${PATTERNS[@]}"; do
    # This file necessarily describes what it guards — scan() excludes it.
    raw="$(scan "${pat}" 2>"${errfile}")"
    rc=$?
    case "${rc}" in
        0)
            hits="$(printf '%s\n' "${raw}" | grep -vE "${ALLOW}")" || hits=""
            if [ -n "${hits}" ]; then
                echo "FAIL: site identifier committed (/${pat}/):"
                echo "${hits}" | sed 's/^/    /'
                status=1
            fi
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

# ---------------------------------------------------------------------------
# Structural provenance checks. These name no value; they assert the shape a
# redacted record must have, so an unredacted capture from any machine fails.
# ---------------------------------------------------------------------------
list_tracked() {  # $1 = find-style name glob, $2 = root dir
    if [ "${ENGINE}" = "git" ]; then git ls-files -- "$2" | grep -E "$1\$" || true
    else find "$2" -name "*${1##*\\.}" 2>/dev/null || true; fi
}

bad=0

# .meta — written by the capture scripts. Redacted form is:
#   host=<OS> <release> <machine>   (exactly 3 fields; `uname -a` has 6+ and
#                                    carries the hostname in field 2)
#   pwd=<redacted>   tmpdir=<redacted>   real_bin=<basename, no separator>
while IFS= read -r f; do
    [ -f "${f}" ] || continue
    while IFS= read -r line || [ -n "${line}" ]; do
        case "${line}" in
            host=*)
                # shellcheck disable=SC2086
                set -- ${line#host=}
                [ "$#" -eq 3 ] || { echo "    ${f}: host= has $# fields, expected 3 (unredacted \`uname -a\`?)"; bad=1; }
                ;;
            pwd=*)    [ "${line}" = "pwd=<redacted>" ]    || { echo "    ${f}: pwd= is not <redacted>"; bad=1; } ;;
            tmpdir=*) [ "${line}" = "tmpdir=<redacted>" ] || { echo "    ${f}: tmpdir= is not <redacted>"; bad=1; } ;;
            real_bin=*)
                case "${line#real_bin=}" in
                    */*|*\\*) echo "    ${f}: real_bin= is a path, expected a bare basename"; bad=1 ;;
                esac
                ;;
        esac
    done < "${f}"
done < <(list_tracked '\.meta' 'tests/corpus')

# .lst — SAS listings. corpus_normalize.sh maps these two fields to <PATH> /
# <OWNER> on both sides of a diff, so the committed reference must already
# carry the placeholder rather than a real path or DOMAIN\user.
while IFS= read -r f; do
    [ -f "${f}" ] || continue
    # The value must START with '<'. Note [^<] alone is wrong: [[:space:]]+ can
    # give back a space for [^<] to match, so every scrubbed line would "fail".
    if LC_ALL=C grep -qE '^[[:space:]]*(Filename|Owner Name)[[:space:]]+[^<[:space:]]' "${f}" 2>/dev/null; then
        echo "    ${f}: Filename/Owner Name is not <PATH>/<OWNER>"
        bad=1
    fi
done < <(list_tracked '\.lst' 'tests/corpus')

if [ "${bad}" -ne 0 ]; then
    echo "FAIL: unredacted capture provenance above"
    status=1
fi

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
