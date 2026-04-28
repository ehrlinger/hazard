# 2026-04-28 — Silent-failure postmortem (v5 design input)

**Context:** an afternoon of debugging the v4.4.5 Linux + SAS reference capture on the CCF Linux SAS host (`lri-sas-p-02.lerner.ccf.org`). Captures completed on Windows SAS host; the same workflow fails on Linux SAS in a way that is **silent at every layer** until the failure cascades into a downstream parser explosion.

This document is a forward-looking input to v5 planning. The conclusion: the v5 roadmap item **"detect / validate XPORT input format at hazard.exe startup"** is the structurally correct fix for the failure mode we observed today, and the design should be informed by exactly what failed silently and where.

---

## TL;DR

- **`hazard.exe` exited with `rc=0` and produced 0 bytes of stdout** when invoked from SAS PROC HAZARD with a `NOPRINT NOLOG` bootstrap-mode control file.
- **A SAS macro layer wrote `!16\n` (4 bytes) to the `.lst`** as a "stand-in" for the binary's normal output.
- **A downstream SAS DATA step** then tried to parse that `.lst` as if it were a normal hazard output listing, hit `ERROR: Unrecognized line from print file`, and **looped 10,000+ times** because the bootstrap macro retries on parse failure. Result: **115 MB log file, 19 minutes of CPU**, never converging.
- **Same failure with `/opt/hazard/bin/hazard` (production v4.x install) as with our fresh build** — so it isn't a build regression, it's an environment + protocol issue.
- **Windows SAS host does not exhibit this** — same bootstrap macros, same example drivers, runs in 7 seconds with `exit=0`.

The root cause was not pinned down today; the symptom suggests a Linux-SAS-specific path through `bootstrap.hazard.sas` that doesn't tolerate the NOPRINT-mode brief output marker. **What today's session does prove is that the binary↔SAS protocol fails open: there is no point in the stack where a hazard binary can say "I'm refusing to run, here's why" in a way that SAS will surface.** Every layer assumes the next is well-behaved.

---

## What we observed (chronological)

**Step 1 — Run capture script on CCF Linux + SAS.** First driver `ac.death.AVC.sas` (a clean exit=0 driver on every other host) failed `exit=2` in <2 seconds with:

```
WARNING: Physical file does not exist, /programs/apps/sas/macro.library/kaplan.sas.
ERROR: Cannot open %INCLUDE file KAPLAN.
```

Diagnosis: the SAS-internal `MACROS` namespace is separate from the OS env var. Site `sasv9.cfg` sets `MACROS=/programs/apps/sas/macro.library/`, but that dir is missing `kaplan.sas`. **Caught by SAS quickly because the missing file is referenced by a `FILENAME ('!MACROS/kaplan.sas')` construct that SAS itself validates at parse time.**

Fix: pass `-set MACROS "$macros_dir"` and `-set SASAUTOS "(...)"` on the SAS invocation. This worked.

**Step 2 — Re-run with macros configured correctly.** `ac.death.AVC` now finishes `ok` in <2s. Second driver `bs.death.AVC.sas` runs normally for ~30 seconds, then keeps running. After a few minutes the operator notices the `.log` file is growing fast.

**Step 3 — Inspect the runaway log.** `bs.death.AVC.log` reaches **17 MB** at minute 1, **115 MB** at minute 19. CPU time: 9.5 min real. Tail of the log shows:

```
NOTE: The infile "/saswork/hzr.J1331669.X57393.lst" is:
      ... File Size (bytes)=4
ERROR: Unrecognized line from print file.
RULE:     ----+----1----+----2----+----3----+...
1         !16 3
ll=3 bfr=16 code=! lleft=57 bfrlen=2 count=. i=. _ERROR_=1 _N_=1
```

10,182 page-headers in the log = SAS retrying the parse step roughly that many times.

**Step 4 — Reproduce the binary failure manually.** `cat /saswork/hzr.J1331669.X57393.sas` shows the 668-byte SAS-written control file: a multi-phase PROC HAZARD with `EARLY` + `CONSTANT` clauses, `SELECTION` (stepwise), and **`NOPRINT NOLOG`**. Running:

```
TMPDIR=/saswork $HOME/hazard/bin/hazard < hzr.J1331669.X57393.sas > /tmp/manual.out 2>&1
echo "rc=$?"   # rc=0
ls -la /tmp/manual.out  # 0 bytes
```

The binary **exits cleanly with no output** when given this NOPRINT control file. The 4-byte `!16` in the SAS-context `.lst` is not from the binary at all — it's from the SAS macro layer wrapping the call. The binary itself produced nothing.

**Step 5 — Confirm the binary is otherwise fine.** `validate_corpus.sh` (which runs the binary directly with bare-stdin captures, no SAS) produces real output and only the documented cosmetic diffs against `v4.3.0`. A clean direct-binary stdin test against `hz.death.AVC.input` (with TMPDIR seeded) prints full hazard listing output. **The binary works for non-NOPRINT direct-binary use.**

**Step 6 — Test the production binary at `/opt/hazard/bin/hazard`.** Same failure. So the issue isn't our build; it's the protocol between SAS bootstrap macro and any v4.x hazard binary on this Linux SAS install.

---

## Root cause (partial)

We pinned down **what doesn't matter** but not **what does**:

- ❌ Not the v4.4.4 SAS-macro fix (`%sysget(OSTYPE)` → `&SYSSCP`); fails identically with v4.4.3 macros.
- ❌ Not our fresh Linux gcc build; fails identically with `/opt/hazard/bin/hazard`.
- ❌ Not missing libraries, missing libm, missing glibc — `ldd` is clean.
- ❌ Not a segfault or signal; `rc=0`.
- ❌ Not the binary itself producing the malformed `!16` — it produces empty output.

What we know:
- The SAS macro layer is writing the `!16` marker to the `.lst` somehow, expecting hazard.exe to overwrite it with real content.
- In NOPRINT mode the binary writes nothing to stdout (designed behavior — output goes to `OUTHAZ=` instead).
- The SAS macro then can't tell hazard succeeded vs. failed, parses `!16`, errors, retries.

**What we need to learn from this for v5:**

1. **The binary has no way to communicate failure clearly.** Even if it had attempted to print an error, NOPRINT mode would have suppressed it. There is no out-of-band channel.
2. **The SAS macro has no way to know the binary ran successfully other than parsing the `.lst`.** No exit code propagation, no stderr reading, no "OK marker" file written by the binary.
3. **The retry-on-parse-failure pattern in the bootstrap macro turns a single bad invocation into 10,000+ bad invocations** before anyone notices. There's no circuit breaker.

---

## v5 design implications

### 1. Startup XPORT version detection (already on roadmap)

Detect XPORT V5 vs V8 (vs unrecognized) at the very start of the binary, **before any procedure-mode flag like `NOPRINT` is honored**. If the input format is unsupported or malformed:

- Write a structured error to stderr (always — never suppressed by NOPRINT)
- Write a 1-line marker to stdout: `XPORT_VERSION_REJECTED <reason>` or similar machine-readable
- Exit with a non-zero code in the **15–20 range** matching the existing `hzfxit` convention (e.g. `exit 16` for "input format not recognized")

This catches not just today's failure mode but every "binary received something it can't process" case. It would have saved the afternoon: hazard.exe would have exited with `rc=16` and a clear `ERROR: BOOTSTRP dataset is V5 but expected V<...>` line, the SAS macro could check `&SYSRC` and bail with a clear message instead of looping.

### 2. Always-on stderr diagnostics (orthogonal but related)

Even when `NOPRINT` is on, errors should go to stderr. The procedure flag should silence the **listing**, not the **diagnostics**. Today the binary was silent on every channel, including the channel that should never be silenceable.

### 3. Out-of-band success marker

When the binary succeeds, write a small sentinel file (e.g. `$TMPDIR/hzr.<JOBID>.<JOBIX>.ok`) containing exit_code, version, dataset_count. SAS macros check that sentinel before parsing the `.lst`. This decouples "did the binary run" from "what does the listing look like" — and it kills the retry-on-parse-failure pattern that escalated today's bug from "one bad invocation" to "10,000 bad invocations".

### 4. Exit code propagation through the SAS shell-out

`hazard.sas` invokes the binary via `%sysexec`. Currently the macro doesn't capture the exit code (or it does and ignores it). v5 should make exit code propagation the **primary** success signal, with `.lst` parsing as a secondary "did the binary do useful work?" check. If `&SYSRC ne 0`, the macro should `%abort` immediately, not retry.

### 5. Refuse to proceed on input mismatch

Related to (1) but stronger: hazard.exe should refuse to run if the input XPORT's variable layout doesn't match the control-file's variable references. Today the binary accepted whatever XPORT was at `$TMPDIR/hzr.<id>.dta` and tried to make sense of it. If V5 stays as the wire format in v5.x (vs. the Parquet jump), at minimum the binary should validate that the variable names referenced in the control file all exist in the XPORT before doing anything else — and exit with a structured error if they don't.

---

## Specific asks for the v5 planning session

1. **Bake (1) startup XPORT detection into the v5 v4.4.6 / v5.0-alpha milestone.** Today's session is the empirical case for it; ship it before any other v5 work that might multiply the failure surface.
2. **Treat the SAS-macro side as a co-equal design surface.** The binary can't fix this alone — `hazard.sas` and `bootstrap.hazard.sas` need the corresponding "check exit code first, parse listing second" change. Without that, even a perfect binary still loops on parse failure.
3. **Decide on the wire-format end-state** before designing (1)'s reject-on-V8 behavior. If v5.0 jumps straight to Parquet ([HAZARD.md v5.0 candidate](../../HAZARD.md) recommends this), then "detect V8 and reject" is moot — V8 never enters the picture. If v5.0 stays on XPORT V5/V8, the V8-detection path matters. The HAZARD.md analysis from 2026-04-28 already lays out the trade-offs; this postmortem is a vote in favor of the Parquet jump because it eliminates one of the ambiguity classes that produce today's silent failures (typed columns, length-aware names, no fixed-width namestr surprises).
4. **Document the failure mode in the v5 spec.** Concrete value: anyone reviewing the v5 design has a real failure to test the design against, not just "imagine if XPORT input were wrong, here's how we'd handle it."

---

## Status of v4.4.5 vis-à-vis this issue

v4.4.5 ships with the Linux + SAS reference bucket as a **known gap** (see `NEWS.md` v4.4.5 entry). Direct-binary Linux validation continues via `tests/validate_corpus.sh` against the `v4.3.0` reference. Windows + SAS reference IS captured (Windows SAS host, 2026-04-28). The harness `tests/validate_examples_sas.sh` is in place and consumes whatever buckets are committed; the moment a Linux + SAS bucket is captured (post-v5-startup-validation, presumably), the harness picks it up automatically with no further code change.

The v5 work, when landed, is what unblocks closing this gap.
