# HAZARD Acceptance Test Plan

> Created: 2026-04-21
> Status: scaffolding landing in v4.4.3; reference corpus capture outstanding
> Scope: C `hazard` (this doc) + C `hazpred` (same protocol); R `TemporalHazard` port covered by a parallel document in that repo.

---

## 1. Why this exists

HAZARD is modernising — logger, error context, then a big refactor toward v5.0 (context-struct, numerical core, stepwise).  The only credible gate on all of that is **"the modern binary reproduces the legacy binary's output byte-for-byte."** Today's `validate_*.sh` scripts cover 4 of ~23 examples and compare against `tests/4.1.0/` — reference outputs from the year 2000 whose provenance is unclear.  We need better.

This plan replaces that ad-hoc setup with a single black-box acceptance harness:

1. Capture the full `.sas` example corpus under the **legacy v4.3.1 Unix binary** — a known-good snapshot of the last pre-modernisation release.
2. Commit the captured inputs (stdin + XPORT `.dta`) and outputs (`.lst`) as the canonical reference.
3. Run the modern binary against those captured inputs, normalise the output, byte-diff against the reference.
4. Wire that single check into CI on both `main` and `release/4.4`.

No parsers, no field-by-field comparators, no tolerance tuning.  Any deviation is a diff; the diff tells you what changed.

---

## 2. Locked decisions

| # | Decision | Rationale |
|---|---|---|
| D1 | **Two reference sets currently checked in: `v4.3.0` (archived CCF legacy capture) and `v4.4.2` (self-consistency baseline of the current modern binary).**  The originally planned `v4.3.1` reference was a pre-capture abstraction; the actual CCF install shipped v4.3.0 and that's what got captured.  Long-term we want a single canonical legacy reference, but we can't collapse onto v4.3.0 yet — see `tests/corpus/FINDINGS.md` §2 for the numerical drift between v4.3.0 and v4.4.x that blocks that consolidation. | Keep the plan honest about what actually shipped.  The harness defaults to `REFERENCE=v4.4.2` (self-consistency check, trivially green until numerical code changes); `REFERENCE=v4.3.0` is available for audit / regression-hunting and is expected to diff. |
| D2 | **SAS generates the inputs during capture.**  We intercept, not translate. | No PROC-options-to-whatever DSL mapping.  We save exactly the bytes SAS handed to the legacy binary. |
| D3 | **Black-box output comparison + tiny normalizer.**  No parser. | Any regression — numerical, formatting, field-order, convergence-message — shows up as a diff.  Maintenance budget stays near zero. |
| D4 | **Harness lives on `main`** (cherry-picked to `release/4.4`), not on a separate long-lived branch. | Every PR runs it.  No silent regressions on feature branches. |
| D5 | **Capture via binary wrapper.**  `scripts/capture-legacy.sh` replaces the `hazard` / `hazpred` binaries in the SAS PATH; snapshots stdin + TMPDIR `.dta` / `.haz` files; exec's the real binary. | Zero SAS-side changes.  Six-line wrapper in user-owned territory. |
| D6 | **hazpred `.haz` captured as static input**, not regenerated from modern hazard at test time. | Isolates hazpred regressions from hazard regressions.  Each binary tested against its own legacy output independently. |
| D7 | **Scaffolded harness + corpus shipped on `feat/acceptance-harness`; v4.4.3 release deferred.**  The original plan called for v4.4.3 to ship as a "reproduces v4.3.x bit-exactly" parity release.  The first end-to-end run (see `tests/corpus/FINDINGS.md`) showed that claim cannot yet be made: v4.4.2 diverges numerically from v4.3.0 and hazpred SIGSEGVs under v4.4.2.  What ships on this PR is the infrastructure + interim dual-reference layout; v4.4.3 parity claim waits on §2/§3 resolution. | A tag claiming parity that doesn't hold would be actively misleading.  The infrastructure is valuable on its own — it detects real regressions. |

Each decision was surfaced in the conversation leading up to this plan; flipping any of them requires explicit discussion, not drift.

---

## 3. File layout

```
tests/corpus/
├── README.md                              # operator-facing usage
├── hazard/
│   ├── inputs/
│   │   ├── hz.death.AVC.input             # stdin text (PROC HAZARD options)
│   │   ├── hzr.AVC.AVCS.dta               # XPORT dataset (read via TMPDIR)
│   │   ├── hm.death.AVC.input
│   │   ├── hzr.AVC.AVCS_MX.dta
│   │   └── ... (per example)
│   └── reference/
│       └── v4.3.1/
│           ├── hz.death.AVC.lst           # legacy binary's raw output
│           ├── hm.death.AVC.lst
│           └── ... (per example)
└── hazpred/
    ├── inputs/
    │   ├── hp.death.AVC.input
    │   ├── hzr.AVC.AVCS.haz               # prior-hazard-run output
    │   └── ... (per example)
    └── reference/
        └── v4.3.1/
            └── hp.death.AVC.lst

scripts/
└── capture-legacy.sh                      # wrapper user drops into SAS PATH

tests/
├── validate_corpus.sh                     # the acceptance harness itself
├── corpus_normalize.sh                    # sed pipeline for transient-field scrubbing
└── run_all_tests.sh                       # (extended) invokes validate_corpus.sh
```

One entry per example, two directories per binary (inputs + reference), single reference version (`v4.3.1/`).  Future reference versions (v4.4.x, v5.0 post-capture) can live beside v4.3.1 — the harness selects via env var.

---

## 4. Capture protocol

Run once, per binary, on a Unix host with SAS + the legacy v4.3.1 install:

```sh
# 1. Locate the real legacy binaries (leave their SAS-invocation paths alone)
export HAZARD_REAL="/usr/local/hazard-4.3.1/bin/hazard"
export HAZPRED_REAL="/usr/local/hazard-4.3.1/bin/hazpred"

# 2. Create an empty capture staging area
export HAZARD_CAPTURE_DIR="$HOME/hazard-capture-$(date +%Y%m%d)"
mkdir -p "$HAZARD_CAPTURE_DIR"/{hazard,hazpred}

# 3. Drop the wrapper into a directory that precedes the real hazard in PATH,
#    naming the wrapper `hazard` and (symlinked) `hazpred`.  The wrapper reads
#    HAZARD_REAL / HAZPRED_REAL at runtime to find the real binary.
mkdir -p ~/hazard-capture-bin
ln -sf /path/to/this/repo/scripts/capture-legacy.sh ~/hazard-capture-bin/hazard
ln -sf /path/to/this/repo/scripts/capture-legacy.sh ~/hazard-capture-bin/hazpred
export PATH="$HOME/hazard-capture-bin:$PATH"

# 4. Run SAS against each of the 23 .sas example files, normally
for sas in tests/*.sas; do
    sas -nodms -log /dev/stderr "$sas"
done

# 5. Each PROC HAZARD or PROC HAZPRED invocation writes a triple into
#    HAZARD_CAPTURE_DIR: <unique-id>.stdin, <unique-id>.lst, plus whatever
#    .dta / .haz it found in TMPDIR at invocation time.  The wrapper also
#    writes <unique-id>.meta with the PROC options parsed from the stdin
#    so the operator can post-rename the tuples to match the .sas filename.

# 6. Post-process: the operator renames <unique-id>.* → <example-name>.*,
#    moves them into tests/corpus/{hazard,hazpred}/{inputs,reference/v4.3.1}/
#    per the layout in §3, and commits.
```

The wrapper (`scripts/capture-legacy.sh`) is what the harness PR introduces.  Operator doesn't write any capture logic themselves.

### Why capture stdin + `.dta` separately

The C binary reads the PROC options from stdin, parses out STUDY/DATASET names, then `fopen`'s `$TMPDIR/hzr.<STUDY>.<DATASET>.dta` to read the observations.  To reproduce without SAS we need both halves: the stdin (which the wrapper tees) and the `.dta` file (which the wrapper copies out of TMPDIR before the binary runs, because the binary conditionally deletes it on exit).

### hazpred capture wrinkle

`hazpred` reads a `.haz` file (the intermediate format `hazard` produces on certain runs) in addition to the XPORT `.dta`.  The wrapper treats `.haz` identically to `.dta` — snapshot whatever's in TMPDIR that matches `hzr*.haz` before the real binary runs.  Per D6, we commit the v4.3.1-produced `.haz` as a static hazpred input rather than regenerating it at test time, so hazpred is tested in isolation from any hazard changes.

---

## 5. Output normalisation

One `sed` pipeline (`tests/corpus_normalize.sh`), applied identically to the captured reference `.lst` and the modern-binary `.lst` before `diff`:

```sed
# Strip the HAZARD version string — changes per release by design
s/C-Version [0-9]\+\.[0-9]\+\.[0-9]\+/C-Version X.Y.Z/

# Strip the top-of-listing timestamp
s/^[A-Z][a-z][a-z] [A-Z][a-z][a-z][ ]\+[0-9]\+ [0-9:]\+ [0-9]\+/<TIMESTAMP>/

# No other substitutions by default.  Floating-point formatting is stable
# across the compilers we care about (clang on macOS, gcc on Linux,
# gcc under MSYS2/UCRT64 on Windows).  If a future platform introduces
# float-format drift, add a rule here with a comment explaining the case.
```

**Anything else that differs is a real finding.**  The normalizer's job is to absorb transient metadata, not to hide regressions.

---

## 6. The harness itself — `tests/validate_corpus.sh`

Pseudocode — see the script file for the real implementation:

```sh
for example in tests/corpus/hazard/inputs/*.input; do
    name=$(basename "$example" .input)
    # Set TMPDIR to the inputs dir so the binary finds the hzr.*.dta file
    TMPDIR="tests/corpus/hazard/inputs" "$HAZARD_BIN" < "$example" > "$TMPDIR_OUT/$name.lst"
    diff <(corpus_normalize < "$TMPDIR_OUT/$name.lst") \
         <(corpus_normalize < "tests/corpus/hazard/reference/v4.3.1/$name.lst") \
        && echo "PASS: $name" \
        || { echo "FAIL: $name"; FAILURES=$((FAILURES+1)); }
done

# Same loop for hazpred against tests/corpus/hazpred/

exit $(( FAILURES > 0 ))
```

Runs in CI via `tests/run_all_tests.sh`; failures are per-example so the diff output points at exactly what regressed.

### Allow-list for accepted deviations

If v4.4.x already drifted from v4.3.1 in some tolerable way (e.g. an error-message wording change that predates the harness), the harness supports a per-example allow-list:

```
tests/corpus/hazard/deviations/v4.3.1/hm.death.AVC.sed
```

A sed script applied *additionally* to the reference side when comparing that specific example.  Every such file requires a sibling `.md` explaining the deviation — why it's acceptable, which commit introduced it, whether v5.0 should adopt or reverse it.  Empty directory by default; we expect to populate it only for documented historical drifts found during capture.

---

## 7. CI integration

- **New job `Acceptance Corpus`** added to `.github/workflows/ci.yml` on both `main` and `release/4.4`.  Runs `tests/validate_corpus.sh`, fails the PR if any example diffs.
- **Legacy V1 / V2 / V3 scripts stay** initially — they cover overlapping ground and provide field-level diagnostic messages (LL to 8 dp, exact iteration count, stepwise path match).  After a release or two of acceptance-corpus green-ness, we evaluate retiring V1/V2/V3 in favour of the black-box check.
- **Speed budget:** each example runs the hazard binary once; 23 examples × ~0.5 s each ≈ 12 s.  Well within CI latency budgets; no need for parallelisation yet.

---

## 8. Release plan — v4.4.3

Version bump from 4.4.2 → 4.4.3 happens **in the same PR that lands the captured corpus**, not in the scaffolding PR.  The progression:

1. **Scaffolding PR** (this plan + scripts + empty `tests/corpus/` tree + harness script): lands first, doesn't bump version.  Establishes the shape.
2. **Corpus capture PR** (the `.input` + `.dta` + `.lst` tuples populated): bumps `configure.ac` to 4.4.3, updates `version-badge.json`, adds release notes entry.  Merges once all 23 examples pass the harness.
3. **Tag and release:** `git tag v4.4.3 <commit>` → Release workflow publishes the 4 platform archives automatically.
4. **Cherry-pick to `main`**: single follow-up PR that brings the scripts + corpus + harness invocation to `main`, without the version bump (main stays on its own trajectory toward v5.0-alpha).

**Release note framing:** v4.4.3 is the "acceptance-harness-proven baseline."  Binary behaviour is identical to v4.4.2; the release carries a parity certification, not new features.

---

## 9. Mirror for TemporalHazard (R port) — out of scope for this doc

The captured inputs (`.dta` XPORT files) and reference outputs (`.lst`) are format-agnostic.  The R port's acceptance harness will:

1. Copy or submodule-pull the same `tests/corpus/` tree from this repo.
2. Read each `.dta` via `haven::read_xpt()` to get a data frame.
3. Translate the PROC options in the corresponding `.input` file to `temporal_hazard::hazard()` arguments via a mapping table (the one part of this that isn't automatic).
4. Run the R fit, extract a textual summary matching the field set in the `.lst` reference.
5. Compare via a field-specific comparator (log-likelihood, parameters, etc.) — not byte-diff, because the R output format is legitimately different.

The R-port plan will live at `temporal_hazard/docs/VALIDATION_PLAN.md`.  Decisions D1 (v4.3.1 as reference) and D3 (acceptance-test framing) carry across directly; the PROC-to-R-args mapping (D2-equivalent) is the only additional design work.

---

## 10. Rollout checklist

Work items, in order:

- [ ] **Scaffolding PR** (this plan + scripts/capture-legacy.sh + tests/validate_corpus.sh + empty tests/corpus/ tree + README)
- [ ] **Operator runs capture** on the Unix + SAS + v4.3.1 host, commits tuples to tests/corpus/ via a second PR
- [ ] **Corpus PR** bumps version to 4.4.3, lands captured tuples, confirms harness green on all 23 examples
- [ ] **Cut v4.4.3** (tag + GitHub release auto-builds)
- [ ] **Cherry-pick to main** (harness only, no version bump) so v5.0 dev runs the same gate
- [ ] **Progress-log entry** in `docs/Claude_MODERNIZATION_GUIDE.md` §7 for v4.4.3
- [ ] **Retire V1/V2/V3 scripts** after one or two green CI cycles (evaluate, don't auto-delete)
- [ ] **TemporalHazard mirror** as a separate project in the R repo

Each step is its own PR.  No big-bang drops.
