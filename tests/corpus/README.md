# Acceptance-test corpus

This directory holds captured inputs and reference outputs for the HAZARD acceptance harness (`tests/validate_corpus.sh`).  See [`docs/VALIDATION_PLAN.md`](../../docs/VALIDATION_PLAN.md) for the full plan and rationale.

## Layout

```
tests/corpus/
├── hazard/
│   ├── inputs/                       # stdin text + XPORT .dta per example
│   │   ├── <name>.input              # PROC HAZARD options, fed to hazard's stdin
│   │   └── hzr.<STUDY>.<DSET>.dta    # XPORT dataset, found via TMPDIR at runtime
│   ├── reference/
│   │   └── v4.3.1/
│   │       └── <name>.lst            # byte-exact output from the legacy binary
│   └── deviations/
│       └── v4.3.1/
│           └── <name>.{sed,md}       # (rare) documented accepted drifts
└── hazpred/
    ├── inputs/                       # same shape — + hzr.<STUDY>.<DSET>.haz
    ├── reference/v4.3.1/
    └── deviations/v4.3.1/
```

- `<name>` is the SAS example filename without the `.sas` extension (e.g. `hz.death.AVC`).
- `<STUDY>` and `<DSET>` are whatever the PROC statement names; the binary derives the `.dta` / `.haz` filename from them.  Multiple examples may share the same dataset (which is fine — `cp` works at capture time).

## How to populate

Run the capture wrapper against a Unix host with SAS + the legacy v4.3.1 binary installed.  See [`scripts/capture-legacy.sh`](../../scripts/capture-legacy.sh) for the full procedure; summary:

```sh
export HAZARD_REAL=/path/to/legacy/hazard
export HAZPRED_REAL=/path/to/legacy/hazpred
export HAZARD_CAPTURE_DIR=$HOME/hazard-capture
mkdir -p "$HAZARD_CAPTURE_DIR"

# Shadow PATH — see scripts/capture-legacy.sh for the full recipe
export PATH="$HOME/hazard-capture-bin:$PATH"

# Run SAS normally over every .sas example
for sas in tests/*.sas; do sas -nodms -log /dev/stderr "$sas"; done

# The wrapper wrote tuples into $HAZARD_CAPTURE_DIR/<hazard|hazpred>/
# with unique IDs.  Inspect each <uid>.meta + <uid>.stdin to identify
# which .sas example it came from, then rename and move the tuples into
# the appropriate tests/corpus/ subdirectories.
```

## How to run

```sh
./tests/validate_corpus.sh
```

Default reference is `v4.3.1`.  Override with `REFERENCE=v4.4.3 ./tests/validate_corpus.sh` once future reference versions are captured.

## When a diff appears

The harness prints the unified diff of the normalised outputs (reference vs. got, first 40 lines).  Interpret it:

1. **Floating-point digits differ** — real numerical regression.  Likely a bug introduced by whatever PR is under review.
2. **Field order / spacing changes** — a formatting change.  Review whether it's intended.
3. **New or missing lines** — a semantic change in the listing.  Almost always a real finding.
4. **Timestamp or version string leaking through** — the normalizer (`tests/corpus_normalize.sh`) is missing a rule.  Add one, with a comment.

## Accepted deviations

If (after investigation) a regression is judged acceptable — e.g. an error-message wording change predating the harness — it can be documented as a per-example deviation:

- `deviations/<reference>/<name>.sed` — an additional sed pipeline applied to the reference side only when diffing this example.
- `deviations/<reference>/<name>.md` — **mandatory** companion explaining why the deviation is accepted, which commit introduced it, and whether v5.0 should adopt or reverse the behaviour.

Deviations are expected to be rare.  The default answer to a diff is "investigate and fix"; the deviation mechanism is the escape hatch, not the default posture.
