#!/usr/bin/env bash
#
# tests/corpus_normalize.sh
#
# Filter that reads a hazard / hazpred .lst file on stdin and writes a
# normalised copy on stdout — suitable for byte-diffing two listings from
# different builds/versions.  Scrubs only transient metadata; everything
# else passes through verbatim.  Any diff after normalisation is a real
# finding.
#
# Rules added here require a comment explaining why the field is
# transient.  When in doubt, leave it alone — the harness's job is to
# flag real regressions, not to paper over them.

set -euo pipefail

exec sed -E \
    -e 's/C-Version [0-9]+\.[0-9]+\.[0-9]+/C-Version X.Y.Z/' \
    -e 's/^[A-Z][a-z]{2} [A-Z][a-z]{2}[[:space:]]+[0-9]+ [0-9]{2}:[0-9]{2}:[0-9]{2} [0-9]{4}/<TIMESTAMP>/' \
    -e 's/[[:space:]]*[0-9]{1,2}:[0-9]{2} (Mon|Tues|Wednes|Thurs|Fri|Satur|Sun)day,[[:space:]]+[A-Za-z]+[[:space:]]+[0-9]{1,2},[[:space:]]+[0-9]{4}[[:space:]]*/ <SAS_TS> /g'

# Rule intents (keep in sync with the sed above):
#
#  1. s/C-Version X.Y.Z/.../
#     The listing carries the hazard binary's version string at the top.
#     Every release bumps it by design; normalising lets v4.3.1 and
#     v4.4.3 diffs be meaningful.
#
#  2. s/^<asctime>/.../
#     The listing embeds the wall-clock time the run began (format:
#     "Mon Apr 20 10:45:32 2026").  Same output, different timestamps
#     must compare equal.
#
#  3. s/<SAS-page-header-timestamp>/<SAS_TS>/
#     SAS-wrapped .lst outputs (produced by PROC HAZARD inside a SAS
#     session, vs. the bare hazard.exe stdout that capture-legacy.sh
#     records) carry a per-page header timestamp in the right margin,
#     formatted "HH:MM <Weekday>day, <Month> DD, YYYY".  Different runs
#     have different times by definition; weekday-name length variance
#     ("Monday" vs "Tuesday") also shifts the surrounding right-aligned
#     padding by 1 char, so the rule eats the flanking whitespace and
#     emits a single " <SAS_TS> " sentinel.  Only relevant for the
#     SAS-driven reference buckets (v<X.Y.Z>-{linux,windows}-x64-sas);
#     bare-binary captures don't contain SAS page headers and the rule
#     is a no-op there.
