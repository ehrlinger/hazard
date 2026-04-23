# HAZARD v4.3 → v4.4 Root Cause Analysis

**Date:** 2026-04-23 (rewritten)
**Status:** ⚠️ **Open** — root cause not isolated; one suspect ruled out by empirical test
**Commit of interest:** `557f3ef` — "Fix UB `swab(src,src)` calls and rename `__Linux__` macro"

> Supersedes the 2026-04-23 v1.0 of this file, which claimed commit `557f3ef` was the single code change responsible for all numerical differences between v4.3.0 and v4.4.2. That claim is not supported by the evidence. See §6 for the revision history.

---

## Executive Summary

Comparing [tests/corpus/hazard/reference/v4.3.0/](tests/corpus/hazard/reference/v4.3.0/) to [tests/corpus/hazard/reference/v4.4.2/](tests/corpus/hazard/reference/v4.4.2/):

- **7 of 7 hazard examples diverge** (sizes, log-likelihoods, parameter estimates, or output truncation). This is a broad, not isolated, regression relative to v4.3.0.
- **2 of 7 v4.4.2 outputs are shorter than v4.3.0** (`hm.death.AVC.2` 16,877 → 5,871 B; `hm.death.AVC.deciles` 16,877 → 5,871 B), consistent with v4.4.2 exiting `$Note: Procedure HAZARD completed successfully.` before the post-convergence reporting section it used to emit.
- **8 of 8 hazpred examples crash v4.4.2 with SIGSEGV** per [tests/corpus/FINDINGS.md §2b](tests/corpus/FINDINGS.md).
- ~~A separate bug — fixed-buffer overflow in `opnfils.c` when `TMPDIR` is long — affects v4.4.2 on hosts without a short `$TMPDIR`~~ **Fixed 2026-04-23.** [src/hazard/opnfils.c](src/hazard/opnfils.c) and [src/hazpred/opnfils.c](src/hazpred/opnfils.c) now use `PATH_MAX` buffers + length-checked `snprintf`; [tests/validate_corpus.sh](tests/validate_corpus.sh) no longer needs the `/tmp/hz.XXXXXX` symlink shim and passes 7/7 against [reference/v4.4.2](tests/corpus/hazard/reference/v4.4.2/) with the natural inputs-dir path. See [FINDINGS.md §3](tests/corpus/FINDINGS.md).

The earlier "single-commit root cause" story was wrong for multiple reasons, enumerated in §2. The actual root cause is **unknown**; §3 lists the commits that need to be bisected to find it.

---

## 1. What diverges, measured from the on-disk corpus

| Example | v4.3.0 size (B) | v4.4.2 size (B) | Δ-lines in diff | Notable numerical changes |
|---|---|---|---|---|
| hm.deadp.VALVES | 41,224 | 41,241 | 1,550 | LL −1864.76 → −1536.4; iter 1→0; nfncts 3→8; cond 0.2527→0.1312 |
| hm.death.AVC.1 | 12,684 | 49,191 | 4,133 | Stepwise expands substantially; final Constant MUC 0.000145 → 8.40e-05 |
| hm.death.AVC.2 | 16,877 | **5,871** | 744 | v4.4.2 output **truncated** before post-convergence reporting |
| hm.death.AVC.deciles | 16,877 | **5,871** | 744 | Same truncation as AVC.2 |
| hz.deadp.KUL | 9,256 | 10,001 | 227 | Smaller-scale numerical drift |
| hz.death.AVC | 7,864 | 8,598 | 204 | Smaller-scale numerical drift |
| hz.tm123.OMC | 8,942 | 6,197 | 243 | v4.4.2 output shorter |

Regenerate with:
```
for f in tests/corpus/hazard/reference/v4.3.0/*.lst; do
  n=$(basename "$f" .lst)
  echo "== $n =="
  diff "$f" "tests/corpus/hazard/reference/v4.4.2/${n}.lst" | wc -l
done
```

---

## 2. Why the "single commit" story doesn't hold

The original v1.0 of this file claimed commit `557f3ef` — which replaced `swab(src, src, n)` and a `memcpy`/double-`swab`/`memcpy` pattern with new `hzd_bswap_short` / `hzd_bswap_int` helpers — was the cause. Four problems with that claim:

**2.1. Old and new byte-swap are bit-identical on glibc.** I tested this directly:

```c
short s = 0x1234; short s_old = s;
swab(&s_old, &s_old, 2);                            // old v4.3 pattern
short s_new = hzd_bswap_short(s);                   // new v4.4 helper
// result: old=3412 new=3412 MATCH

int n = 0x12345678; char bfr[8] = {0};
memcpy(bfr, &n, 4);
swab(bfr, bfr+6, 2); swab(bfr+2, bfr+4, 2);         // old v4.3 pattern
int n_old; memcpy(&n_old, bfr+4, 4);
int n_new = hzd_bswap_int(n);                       // new v4.4 helper
// result: old=78563412 new=78563412 MATCH
```

On Linux/x86-64/glibc — the CCF host — both patterns produce identical bytes. The POSIX undefined behaviour is real, but glibc's `swab()` does not mishandle the `src == dst, nbytes = 2` case in practice, and the `bfr` trick is self-consistent. The "different byte-swapped metadata → different optimizer path" narrative therefore does not explain the v4.3/v4.4 divergence on Linux.

UCRT / MSVCRT on Windows is a separate story — the commit message's claim that `_swab` may silently produce wrong results in-place on Windows is plausible and is a good reason for the fix. But the reference outputs at issue here were produced on Linux.

**2.2. 92 commits sit between v4.3.1 and v4.4.2.** Several of them touch code paths with a numerical footprint:

```
git log --oneline v4.3.1..v4.4.2 | wc -l   # → 92
```

High-relevance suspects (abridged):
- `c1860eb` — "Address Copilot PR#15 review: endianness, fixed-width types, nounistd"
- `f92ba88` — "Refactor error messages and memory allocation handling in various modules"
- `c3ef5af` — "Refactor code structure for improved readability and maintainability"
- `1aee1a6` — "Focused warning cleanup: formats, fread checks, comments"
- `b2cd2d9` — "Fix CI build failures: INITIAL lexer, swab portability, errorno auto-reset"
- `e434def` — "Improve hazard.c debugging and fix CRLF parse errors on Windows"
- `557f3ef` — the one formerly blamed

No bisect has been run. Any of these could individually, or in combination, explain the divergence.

**2.3. The truncation of AVC.2 and AVC.deciles is unexplained.** Both v4.4.2 outputs end at `$Note: Procedure HAZARD completed successfully.` — the normal termination banner — but do so *before* the post-convergence reporting block that v4.3.0 emits. That's a code-path change in output generation, not an optimizer-convergence change, and is not explained by any byte-swap argument.

**2.4. hazpred SIGSEGVs are unexplained by the swab fix.** If `557f3ef` repaired correctness issues, you'd expect hazpred to work better, not crash in 8/8 cases. [FINDINGS.md §2b](tests/corpus/FINDINGS.md) flags this as a distinct, open issue.

---

## 3. Plan to isolate the actual cause

1. **Bisect.** `git bisect start v4.4.2 v4.3.1` with the harness script as the good/bad oracle, targeting one diverging example (pick hm.deadp.VALVES — smallest diff, non-truncated). This is the definitive answer and should be done before the release notes are written.
2. **Diagnose hazpred SIGSEGV** independently ([FINDINGS.md §2b](tests/corpus/FINDINGS.md)). Likely a pre-existing latent bug rather than a new regression — CCF never exercised it the way the harness does.
3. **Diagnose AVC.2/deciles truncation** — check whether the stepwise post-report block is gated on something that changed (e.g. a flag, a file-write return value, an error code path that now short-circuits after CI warnings fix).
4. ~~Check whether v4.4.2 depends on the `opnfils.c` TMPDIR-length bug~~ **Done 2026-04-23.** Ruled out as a source of the numerical divergence. With the TMPDIR overflow fixed and the harness workaround removed, all 7 hazard examples produce byte-identical output to the existing `reference/v4.4.2/` corpus (which was itself captured *with* the workaround in place). The divergence vs v4.3.0 persists unchanged — the TMPDIR bug was a separate stability issue, not the numerical cause.

---

## 4. What commit 557f3ef actually did

The commit is still useful — the C99 UB claim is correct, and removing it is worth doing for Windows/UCRT correctness regardless of whether it changes Linux bytes. Accurate description of the change (for release notes, not as "root cause"):

**Files changed: 5 (`git show --stat 557f3ef`):**

```
src/common/swab_compat.h   | +19      (new hzd_bswap_short, hzd_bswap_int)
src/common/xvputd.c        | +36/−36  (write-side byte-swap rewrite)
src/hazard/hazard_l.l      | +1       (%option nounistd for flex)
src/hazard/opnfils.c       | +10/−21
src/hazpred/opnfils.c      | +18/−35
                             5 files, +52/−60 (net −8)
```

**Behavioural changes:**
- Removes C99 §7.24.7.4 UB from three byte-swap sites. Portable on UCRT/musl, where the old pattern was genuinely unsafe.
- Renames `#define __Linux__` → `#define SAS_TRANSPORT_BYTESWAP` (with matching `#ifdef`). Functionally equivalent — the guard was always unconditionally defined, so this is a naming/clarity change.
- Adds `%option nounistd` to `hazard_l.l` so the flex-generated scanner doesn't unconditionally include `<unistd.h>` (Windows fix).

**Effect on Linux/glibc bytes:** none, per §2.1.

---

## 5. Recommendations

1. **Do not claim "v4.4 is more correct" in release notes.** The evidence for that claim doesn't exist yet — neither v4.3.0 nor v4.4.2 has been validated against an independent reference (SAS PROC LIFEREG / survreg, hand-worked examples, etc.). [FINDINGS.md §2a](tests/corpus/FINDINGS.md) has this right: "Neither is 'wrong,' but they differ."
2. **Do not cut v4.4.3 as an "acceptance-harness-proven baseline"** until the divergence is root-caused, the hazpred SIGSEGV is fixed, and the TMPDIR buffer is either fixed or waived with a documented workaround.
3. **Use the validated v4.3.0 reference corpus.** [CAPTURE-COMPARISON-2026-04-22.md](CAPTURE-COMPARISON-2026-04-22.md) confirms its authenticity by SHA-256 against a fresh CCF capture. Numerical *correctness* is a separate question, but the corpus can be trusted as a faithful snapshot of v4.3.0 production behaviour.

---

## 6. Revision history

| Date | Version | Author | Notes |
|---|---|---|---|
| 2026-04-23 | 1.0 | Analysis | Claimed commit `557f3ef` as sole root cause; claimed "1 example diverges (hm.deadp.VALVES)"; claimed "v4.4 is more correct"; merged data from hm.deadp.VALVES and hm.death.AVC.1 into one table row; did not address hazpred SIGSEGV or TMPDIR buffer. |
| 2026-04-23 | 2.0 | Ehrlinger | Rewrite. (a) Corpus check shows 7/7 divergences, not 1. (b) Empirical test: old and new byte-swap produce identical bytes on glibc — ruling 557f3ef out as the sole cause on the CCF host. (c) 92 commits between v4.3.1 and v4.4.2 — root cause requires bisect, not yet done. (d) Restored status to "open". (e) Added hazpred SIGSEGV and AVC.2/deciles truncation as separate open items. (f) Corrected commit file-count (5 files incl. `hazard_l.l`, not 4). |
| 2026-04-23 | 2.1 | Ehrlinger | Open item (a) resolved: `opnfils.c` 80-byte `TMPDIR` buffer overflow fixed via `PATH_MAX`/`snprintf` in [src/hazard/opnfils.c](src/hazard/opnfils.c) and [src/hazpred/opnfils.c](src/hazpred/opnfils.c); harness workaround removed from [tests/validate_corpus.sh](tests/validate_corpus.sh). Corpus still passes 7/7 against v4.4.2 reference — confirms TMPDIR bug was a stability issue, not part of the v4.3→v4.4 numerical divergence. |

---

## References

- [tests/corpus/FINDINGS.md](tests/corpus/FINDINGS.md) — canonical, cautious findings document; this file now aligns with it
- [CAPTURE-COMPARISON-2026-04-22.md](CAPTURE-COMPARISON-2026-04-22.md) — v4.3.0 reference authenticity
- [hazard-capture-results.tar.gz](hazard-capture-results.tar.gz) → [hazard-capture/](hazard-capture/) — 2026-04-22 capture archive from CCF
- `git show 557f3ef` — the commit under discussion
- `docs/Claude_MODERNIZATION_GUIDE.md` — modernization policy, byte-compatibility expectations
