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

### 2a. Optimiser converges to different values

Example: `hm.death.AVC.1` (stepwise analysis).  Both versions start from the same initial Constant MUC (`0.0005437256`) but converge to different terminal estimates:

| Field | v4.3.0 reference | v4.4.2 current |
|---|---|---|
| Constant MUC (final) | `0.0001452016` | `8.403295E-05` |
| Output size | 12,684 bytes | 49,191 bytes |

The size delta suggests v4.4.2 is running more stepwise iterations — the stepwise selection is exploring more of the variable space before terminating.  Likely root cause: commit `557f3ef Fix UB swab(src,src) calls` (on `release/4.4` history) replaced undefined behaviour in the XPORT byte-swap path with well-defined endian handling.  The UB happened to produce one numerical result; the fix produces another.  Neither is "wrong," but they differ, and v4.4.x has diverged from the on-site v4.3.0 install's output as a result.

**Decision needed:** is v4.4.x's UB-fixed output "more correct and acceptable," or is bit-compat with v4.3.0 required?  Per the roadmap's tolerance policy (`docs/Claude_MODERNIZATION_GUIDE.md` §1), log-likelihoods and parameter estimates should be bit-exact vs the production baseline.  v4.4.0 was that baseline, but we don't have a v4.4.0 capture — only v4.3.0 (legacy) and v4.4.2 (current).  Options:
- Capture v4.4.0 output as the canonical reference (requires running v4.4.0 somewhere; v4.4.0 was never formally tagged so v4.4.1 is the earliest reproducible point).
- Accept v4.4.2 output as the baseline and document the drift from v4.3.0 in release notes.
- Revert the UB fix and audit every numerical side effect.

### 2b. Hazpred crashes under v4.4.2

All 8 hazpred captures crash the v4.4.2 binary with SIGSEGV (exit=-11).  This is distinct from the hazard TMPDIR buffer issue (§3).  Reproduces with a short `TMPDIR`, so not a path length overflow.  Root cause unknown; needs investigation under a debugger.

Until this is diagnosed, no v4.4.2 references exist under `reference/v4.4.2/hazpred/` — the harness will skip hazpred comparisons with a clear message.

---

## 3. Hazard binary — fixed-buffer overflow in `opnfils.c`

Reproducing any hazard capture with an absolute `TMPDIR` longer than ~60 characters crashes the binary with SIGTRAP (exit 133, or -5 via Python `subprocess`).  Cause: `opnfils.c` uses an 80-byte `char bfr[80]` to construct `$TMPDIR/hzr.<study>.<dataset>.dta` via `strcat`.  Long absolute repo paths like `/Users/.../hazard/tests/corpus/hazard/inputs/hzr.J971099.X3.dta` exceed the buffer.

**Workaround (in use):** `tests/validate_corpus.sh` creates a short `$TMPDIR` under `/tmp/hz.XXXXXX` via `mktemp -d` and symlinks the `.dta` / `.haz` files into it.  Binary then sees short paths, completes normally.

**Real fix:** replace the fixed buffer with dynamic sizing (`snprintf` into a heap buffer, or a length-checked write).  Phase 2/4 work in the v5.0 roadmap.  Until then, never pass the binary a long `$TMPDIR`.

At the CCF capture site, `$TMPDIR=/saswork` — 9 characters — which is why the bug has never bitten the operator's workflow.

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
