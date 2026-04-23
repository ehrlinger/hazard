# Acceptance corpus — findings from first end-to-end run

> First populated: 2026-04-21
> Reference capture: v4.3.0 legacy binary at Cleveland Clinic (`/opt/hazard/bin/hazard.exe`, `hazpred.exe`) running under SAS 9.4 TS1M8 on RHEL 8
> Modern binary under test: `v4.4.2` (current `release/4.4` HEAD)

This document records what the harness has surfaced on its first real run.  It's append-only — new findings land here with a date stamp and a decision when the team makes one.

---

## 1. Captured reference corpus — v4.3.0

**15 clean captures** (real_exit=0) across 23 `.sas` examples run through SAS.  Mapping from PROC invocation to example file used the `hzr.J<jobid>.X<jobix>` / `hzp.J<jobid>.X<jobix>` prefix naming the `opnfils.c` path builder generates.

| Kind | Examples | Inputs | .lst references | `.haz` outputs |
|---|---|---|---|---|
| `hazard` | 7 | `hm.deadp.VALVES`, `hm.death.AVC.1` (stepwise), `hm.death.AVC.2` (final), `hm.death.AVC.deciles`, `hz.deadp.KUL`, `hz.death.AVC`, `hz.tm123.OMC` | 7 | 5 |
| `hazpred` | 8 | `hm.death.AVC.deciles.{1,2}`, `hp.death.AVC`, `hp.death.AVC.hm{1,2}`, `hs.death.AVC.hm1.{1,2}`, `hz.tm123.OMC` | 8 | 0 |

**5 captures failed at v4.3.0** (not included): `real_exit=16` errors on hazpred examples that depend on prior hazard runs' `.haz` outputs.  The first capture pass ran examples alphabetically; `capture-order.txt` was a placeholder.  These will clear when the order manifest is populated and the corpus is re-captured.

**1 capture crashed v4.3.0** (`real_exit=1`): `hz.te123.OMC`.  Distinct legacy-binary bug.  Deferred.

---

## 2. Modern v4.4.2 vs v4.3.0 — numerical divergence

Running the same captured `.input` + `.dta` tuples through v4.4.2 produces output that differs from v4.3.0 in two ways:

### 2a. Optimiser converges to different values *(root-caused 2026-04-23: cross-platform FP, not code history)*

Example: `hm.death.AVC.1` (stepwise analysis).  Both versions start from the same initial Constant MUC (`0.0005437256`) but converge to different terminal estimates:

| Field | v4.3.0 reference (CCF Linux) | v4.4.2 current (captured on macOS) |
|---|---|---|
| Constant MUC (final) | `0.0001452016` | `8.403295E-05` |
| Output size | 12,684 bytes | 49,191 bytes |

**Root cause identified 2026-04-23 (see [ROOT-CAUSE-ANALYSIS.md](../../ROOT-CAUSE-ANALYSIS.md) §2):** the divergence is 100% cross-platform floating-point non-determinism (Linux/gcc/glibc vs macOS/Apple-clang/arm64/Apple-libm), **0%** contribution from the 92 commits between v4.3.1 and v4.4.2.

Evidence: workflow [linux-ll-check.yml](../../.github/workflows/linux-ll-check.yml) dispatched on `ubuntu-latest` at both v4.3.1 and v4.4.2 produced **LL = −1864.76** for `hm.deadp.VALVES` — matching the CCF v4.3.0 reference exactly at both tags.  The same experiment on macOS at both tags produces LL = −1536.4 (matching the macOS-captured `reference/v4.4.2/`).  Neither platform's numbers move across the commit range; only the platform axis moves them.

The earlier "likely root cause: commit `557f3ef`" hypothesis was disproven by a direct test — old `swab(src,src,n)` and new `hzd_bswap_*` helpers produce bit-identical output on glibc.  The 557f3ef fix is still correct for Windows/UCRT, but it is not what moved the numbers here.

**Implications:**
- On **Linux** (including CCF RHEL 8), v4.4.x produces **byte-identical output to v4.3.0**.  CCF users upgrading see zero numerical drift.
- On **macOS**, v4.4.x produces distinct-but-self-consistent output, invariant across the 92 commits.
- On **Windows**, unverified — expected to produce a third, self-consistent output family.

**Decision (updated 2026-04-23):** maintain **per-platform** reference corpora.  Proposed layout:
- `reference/v4.4-linux/`   — promote from `reference/v4.3.0/` (bit-identical per the CI runs above).  The canonical numerical reference.
- `reference/v4.4-macos/`   — rename `reference/v4.4.2/` (all captures to date were on macOS).
- `reference/v4.4-windows/` — TBD; one dispatch against `windows-latest` closes the table.

The tolerance policy in `docs/Claude_MODERNIZATION_GUIDE.md` §1 ("log-likelihoods and parameter estimates should be bit-exact vs the production baseline") is satisfied on Linux.  It is not satisfied cross-platform, and achieving that would require compiler-level intervention (`-ffp-contract=off`, FMA disable, common libm, fixed rounding modes) — v5.0-scale work, out of scope for v4.4.

### 2b. Hazpred SIGSEGVs under v4.4.2 *(fixed 2026-04-23)*

**History.** All 8 hazpred captures crashed the v4.4.2 binary with SIGSEGV (exit=−11).  Reproduced with a short `TMPDIR`, so not the §3 path-length overflow.

**Diagnosis.** lldb stack at crash (reproduced on `hp.death.AVC`):
```
frame #0: flockfile + 28
frame #1: fread + 116
frame #2: hazpred`opnfils at opnfils.c:88
```
Line 72 of `src/hazpred/opnfils.c` had `hazfile = fopen(bfr,"rb")` with no null check; subsequent `fread(bfr, 80, 1, hazfile)` calls dereferenced a NULL `FILE*`.  `infile` and `outfile` both had the defensive null check right next to it; `hazfile` was the one missed.

**Fix.** Added the matching null check + `hzfxit("hazfile")` call so a missing INHAZ file fails with a clear "cannot open INHAZ file" stderr line and exit=16 instead of SIGSEGV.

**Corpus status after fix.** All 8 hazpred inputs now exit=16 with the missing-INHAZ message, since the bridging `.haz` files are absent from `tests/corpus/hazpred/inputs/` (the v4.3.0 capture never produced them — SAS writes them at PROC-HAZPRED time via the LIBNAME EXAMPLES `.sas7bdat` → XPORT conversion, which happens outside the wrapper scope).  This is a corpus-completeness gap (§5 item 2), not a binary bug.  Once the corpus is re-captured with proper producer→consumer ordering so the `.haz` intermediates land in `inputs/`, the `reference/v4.4-*/hazpred/` fixtures can be populated and the harness will start comparing them.

---

## 3. Hazard binary — fixed-buffer overflow in `opnfils.c` *(fixed 2026-04-23)*

**History.** Reproducing any hazard capture with an absolute `TMPDIR` longer than ~60 characters used to crash the binary with SIGTRAP (exit 133, or −5 via Python `subprocess`). Cause: `opnfils.c` used an 80-byte `char bfr[80]` to construct `$TMPDIR/hzr.<study>.<dataset>.dta` via `strcat`. Long absolute repo paths like `/Users/.../hazard/tests/corpus/hazard/inputs/hzr.J971099.X3.dta` exceeded the buffer. At the CCF capture site `$TMPDIR=/saswork` (9 chars), which is why the bug never bit the operator's workflow.

**Fix.** [src/hazard/opnfils.c](../../src/hazard/opnfils.c) and [src/hazpred/opnfils.c](../../src/hazpred/opnfils.c) now size both path buffers to `PATH_MAX` (4096) and build filenames with length-checked `snprintf` calls. A too-long `$TMPDIR` now calls `hzfxit("TMPDIR path too long")` with a clear log message instead of SIGTRAP-ing.

**Harness impact.** [tests/validate_corpus.sh](../validate_corpus.sh) no longer needs its `/tmp/hz.XXXXXX` symlink shim — `TMPDIR` can point at the real `tests/corpus/hazard/inputs/` directory directly. The 7-example corpus passes 7/7 under the long `$TMPDIR` path post-fix.

---

## 4. What's usable right now

**Corpus data:**
- `tests/corpus/hazard/inputs/*.input` + `*.dta` (7 examples, with 2 cases sharing a `.dta`)
- `tests/corpus/hazard/reference/v4.3.0/*.lst` + `*.meta` + `*.haz` (7 tuples, byte-exact legacy output)
- `tests/corpus/hazard/reference/v4.4.2/*.lst` + `*.haz` (5 tuples — 2 examples exit 1 partway through)
- `tests/corpus/hazpred/inputs/*.input` + `*.dta` (8 examples)
- `tests/corpus/hazpred/reference/v4.3.0/*.lst` + `*.meta` (8 tuples)
- `tests/corpus/hazpred/reference/v4.4.2/` — empty, blocked on the SIGSEGV

**Harness:**
- `tests/validate_corpus.sh` runs all 15 inputs through the modern binary, handles the TMPDIR buffer bug via `/tmp`-based symlink dir, normalises version strings + timestamps, byte-diffs against the selected reference.
- `REFERENCE=v4.3.0 ./tests/validate_corpus.sh` — compare current binary to legacy (expects divergences per §2).
- `REFERENCE=v4.4.2 ./tests/validate_corpus.sh` — compare current binary to modern baseline (expect all-green when the binary hasn't changed since capture).

---

## 5. Open items, prioritised

1. **Investigate hazpred SIGSEGV** — blocks §2b's `reference/v4.4.2/hazpred/`.  Run under `lldb` / `valgrind`; likely a pre-existing legacy bug that didn't manifest at CCF.
2. **Populate `scripts/capture-order.txt`** with real dependency order, re-capture the 5 `real_exit=16` hazpred cases to complete the v4.3.0 reference.
3. **Fix the 80-byte TMPDIR buffer in `opnfils.c`** — real fix, Phase 2 work.  Until then the harness workaround is load-bearing.
4. **Decide reference-version policy** (§2a) — is v4.4.x's UB-fixed output acceptable, or must v5.0 match v4.3.0 bit-exactly?  This is the gate for §2a's status.
5. **Capture v4.4.0 / v4.4.1 baseline** if "match production" is the answer to #4.

---

## 6. What this proves

1. The capture pipeline works end-to-end at a real production site (CCF, RHEL 8, SAS 9.4 TS1M8, `/opt/hazard/bin` install).
2. The harness detects real regressions — both numerical (§2a) and crashes (§2b, §3).
3. The reference-corpus approach is viable: 2 MB of artefacts, 98 files, structured and auditable.
4. The v4.4.x branch as currently implemented is NOT a byte-for-byte replacement for v4.3.0, contrary to the working assumption of the original release plan.

The v4.4.3 "acceptance-harness-proven baseline" release the roadmap sketches is not ready to ship.  What's ready: the harness infrastructure, the v4.3.0 reference corpus, and a concrete list of the drifts and bugs that need resolution before a parity claim can be made.
