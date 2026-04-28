# Acceptance-test corpus

This directory holds captured inputs and reference outputs for the HAZARD acceptance harness (`tests/validate_corpus.sh`).  See [`docs/VALIDATION_PLAN.md`](../../docs/VALIDATION_PLAN.md) for the full plan and rationale.

## Layout

```
tests/corpus/
├── hazard/
│   ├── inputs/                       # stdin text + XPORT .dta per example
│   │   ├── <name>.input              # PROC HAZARD options, fed to hazard's stdin
│   │   └── hzr.J<jobid>.X<jobix>.dta # XPORT dataset, found via TMPDIR at runtime
│   └── reference/
│       ├── v4.3.0/                       # archived CCF Linux capture (gcc bucket)
│       │   ├── <name>.lst                # byte-exact output from the legacy binary
│       │   ├── <name>.meta               # wrapper metadata (real_exit, paths)
│       │   └── <name>.haz                # (where PROC HAZARD used OUTHAZ=)
│       └── v4.4.4-macos-arm64/           # self-consistency reference for the
│           └── <name>.lst                #   current binary on macOS Apple Silicon
└── hazpred/
    ├── inputs/                           # same shape — + hzp.J<jobid>.X<jobix>.dta/.haz
    └── reference/
        └── v4.3.0/                       # (no macOS hazpred capture yet — SIGSEGV
                                          #  was fixed 2026-04-23 but the bridging
                                          #  .haz files aren't in inputs/ yet —
                                          #  FINDINGS.md §2b)
```

- `<name>` is the SAS example filename without the `.sas` extension (e.g. `hz.death.AVC`).  Multi-invocation examples get `.1` / `.2` ordinal suffixes.
- `.dta` / `.haz` filenames use the `hzr.J<jobid>.X<jobix>.dta` convention (hazard) or `hzp.J<jobid>.X<jobix>.dta` (hazpred) that SAS's `PROC HAZARD` / `PROC HAZPRED` macros build.  Multiple examples may share the same dataset.

## How to populate

Run the capture wrapper against a Unix host with SAS + the legacy HAZARD binary installed.  See [`scripts/capture-legacy.sh`](../../scripts/capture-legacy.sh) and [`scripts/CAPTURE_INSTRUCTIONS.md`](../../scripts/CAPTURE_INSTRUCTIONS.md) for the full procedure; summary:

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

Default reference is auto-selected from the host toolchain family (see
[`FINDINGS.md`](FINDINGS.md) §2a for the two-bucket cross-toolchain model):

- **Darwin / arm64** → `v4.4.4-macos-arm64` (self-consistency on Apple Silicon;
  passes trivially until someone modifies a numerical code path).
- **Linux, Windows, anything else** → `v4.3.0` (gcc-bucket reference from CCF
  production).  Current v4.4.x builds on Linux match at the numerical level but
  differ on the version banner and "Cleveland Clinic Foundation" org string,
  so expect cosmetic diffs until a v4.4.x Linux recapture lands.

Override explicitly with e.g. `REFERENCE=v4.3.0 ./tests/validate_corpus.sh` to
force the legacy comparison on any host (surfaces the cross-toolchain numerical
drift on macOS — known, documented per [`FINDINGS.md`](FINDINGS.md) §2a).

## When a diff appears

The harness prints the unified diff of the normalised outputs (reference vs. got, first 40 lines).  Interpret it:

1. **Floating-point digits differ** — real numerical regression.  Likely a bug introduced by whatever PR is under review.
2. **Field order / spacing changes** — a formatting change.  Review whether it's intended.
3. **New or missing lines** — a semantic change in the listing.  Almost always a real finding.
4. **Timestamp or version string leaking through** — the normalizer (`tests/corpus_normalize.sh`) is missing a rule.  Add one, with a comment.

## Accepted deviations (not currently populated)

The harness supports an escape-hatch mechanism for diffs that are investigated and judged acceptable.  At `tests/corpus/<kind>/deviations/<reference>/<name>.sed`, an additional sed pipeline is applied to the reference side only when diffing this specific example, with a mandatory sibling `<name>.md` explaining why the deviation is accepted.  No deviation dirs are populated yet; the mechanism is here for when we need it (likely during v5.0 phase work, when some drifts will be intentional).

Deviations are expected to be rare.  The default answer to a diff is "investigate and fix"; the deviation mechanism is the escape hatch, not the default posture.
