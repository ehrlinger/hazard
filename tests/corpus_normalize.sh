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
    -e 's/^[A-Z][a-z]{2} [A-Z][a-z]{2}[[:space:]]+[0-9]+ [0-9]{2}:[0-9]{2}:[0-9]{2} [0-9]{4}/<TIMESTAMP>/'

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
