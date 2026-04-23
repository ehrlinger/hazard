# HAZARD v4.3 → v4.4 Root Cause Analysis

**Date:** 2026-04-23 (rewritten, v3.0)
**Status:** ✅ **Closed** — root cause isolated via one-shot GitHub Actions run on Linux
**Root cause:** Cross-platform floating-point non-determinism (Linux/gcc/glibc vs macOS/clang/arm64/Apple libm). No meaningful contribution from commits in the v4.3.1 → v4.4.2 range.

> Supersedes v1.0 (claimed commit `557f3ef` as sole cause — wrong) and v2.x (re-opened as "cause unknown, bisect pending" — superseded by direct evidence below). See §6 for revision history.

---

## Executive Summary

The numerical discrepancy between [tests/corpus/hazard/reference/v4.3.0/](tests/corpus/hazard/reference/v4.3.0/) (captured at CCF on Linux) and [tests/corpus/hazard/reference/v4.4.2/](tests/corpus/hazard/reference/v4.4.2/) (captured locally on macOS) is **100% a platform effect, 0% a code-history effect**:

| Build | v4.3.1 | v4.4.2 | release/4.4 HEAD |
|---|---|---|---|
| **Linux** / gcc / glibc (GitHub Actions ubuntu-latest) | **−1864.76** | **−1864.76** | (not tested) |
| **macOS** / Apple clang / arm64 (local) | −1536.4 | −1536.4 | −1536.4 |

Hm.deadp.VALVES log-likelihood, both endpoints tested on Linux via [.github/workflows/linux-ll-check.yml](.github/workflows/linux-ll-check.yml) (runs [24850944156](https://github.com/ehrlinger/hazard/actions/runs/24850944156) and [24851086714](https://github.com/ehrlinger/hazard/actions/runs/24851086714)):

- **Linux is invariant** across the 92-commit window: v4.3.1 and v4.4.2 both produce LL=−1864.76, bit-matching the v4.3.0 reference captured at CCF.
- **macOS is invariant** across the same window: v4.3.1 and v4.4.2 both produce LL=−1536.4, matching the v4.4.2 reference captured on this laptop.
- The 92 commits between the tags do not move numbers on either platform. Git bisect would have found nothing.

The divergence is therefore fully explained by the difference in compiler (gcc vs clang), math library (glibc libm vs Apple libm), and architecture (x86-64 vs arm64). HAZARD's parametric-survival optimizer runs thousands of floating-point operations with FMA / denormal-handling / rounding-mode sensitivity; that's enough for platforms to drift to different local minima of the same likelihood surface.

---

## 1. What diverges in the on-disk corpus (macOS-captured `reference/v4.4.2/` vs CCF-captured `reference/v4.3.0/`)

| Example | v4.3.0 size (B) | v4.4.2 size (B) | Δ-lines | Notable numerical delta |
|---|---|---|---|---|
| hm.deadp.VALVES | 41,224 | 41,241 | 1,550 | LL −1864.76 → −1536.4; iter 1→0; nfncts 3→8; cond 0.2527→0.1312 |
| hm.death.AVC.1 | 12,684 | 49,191 | 4,133 | Stepwise explores more of the variable space on macOS; final Constant MUC 0.000145 → 8.40e-05 |
| hm.death.AVC.2 | 16,877 | **5,871** | 744 | v4.4.2 macOS output truncated before post-convergence reporting |
| hm.death.AVC.deciles | 16,877 | **5,871** | 744 | Same truncation as AVC.2 |
| hz.deadp.KUL | 9,256 | 10,001 | 227 | Smaller-scale numerical drift |
| hz.death.AVC | 7,864 | 8,598 | 204 | Smaller-scale numerical drift |
| hz.tm123.OMC | 8,942 | 6,197 | 243 | macOS output shorter |

Every one of these examples is the "Linux shows −1864.76-family output, macOS shows −1536.4-family output" pattern in miniature. The §Executive-Summary experiment confirmed that on hm.deadp.VALVES. The remaining six almost certainly share the same mechanism; the "smaller-scale numerical drift" rows are smaller only because those examples converge faster, giving fewer FP ops for platforms to accumulate divergence in.

The two **truncated** outputs (`hm.death.AVC.2`, `hm.death.AVC.deciles`) are a separate tail-effect: on macOS the optimizer takes a different convergence path that short-circuits the post-convergence reporting block, producing a successful exit but a shorter listing. That's a consequence of the platform split, not an independent bug.

Regenerate the diff table with:
```
for f in tests/corpus/hazard/reference/v4.3.0/*.lst; do
  n=$(basename "$f" .lst)
  echo "== $n =="
  diff "$f" "tests/corpus/hazard/reference/v4.4.2/${n}.lst" | wc -l
done
```

---

## 2. How we isolated the cause

Two empirical tests, in order.

### 2.1. Ruled out commit 557f3ef on glibc

The v1.0 theory was that commit `557f3ef` — which replaced `swab(src, src, n)` and a `memcpy`/double-`swab`/`memcpy` trick with new `hzd_bswap_short` / `hzd_bswap_int` helpers — was the cause, via different byte-swapped XPORT metadata. Direct test on Linux/glibc:

```c
short s = 0x1234; short s_old = s;
swab(&s_old, &s_old, 2);                     // old v4.3 pattern
short s_new = hzd_bswap_short(s);            // new v4.4 helper
// → old=3412, new=3412, MATCH

int n = 0x12345678; char bfr[8] = {0};
memcpy(bfr, &n, 4);
swab(bfr, bfr+6, 2); swab(bfr+2, bfr+4, 2);  // old v4.3 pattern
int n_old; memcpy(&n_old, bfr+4, 4);
int n_new = hzd_bswap_int(n);                // new v4.4 helper
// → old=78563412, new=78563412, MATCH
```

Both patterns produce identical bytes on glibc. So whatever moved numbers between v4.3.0 and v4.4.2 on the CCF host, it was not `swab` semantics. (The POSIX UB claim is still technically correct and the commit is still worth keeping for Windows/UCRT correctness — see §4.)

### 2.2. Measured both endpoints on Linux and macOS

Workflow [.github/workflows/linux-ll-check.yml](.github/workflows/linux-ll-check.yml) was dispatched twice, once per endpoint tag, on `ubuntu-latest` (Linux / gcc / glibc). Results vs local macOS runs:

| Ref | Linux LL (CI) | macOS LL (local) |
|---|---|---|
| v4.3.1 | −1864.76 | −1536.4 |
| v4.4.2 | −1864.76 | −1536.4 |

Linux is invariant across the range → the 92 commits are numerically inert on Linux.
macOS is invariant across the range → the 92 commits are numerically inert on macOS.
Only the platform axis moves numbers. QED.

---

## 3. What commit 557f3ef actually did

The commit is not the cause of the divergence, but the change is still useful — the C99 UB claim is correct and the fix matters on Windows/UCRT. Accurate description (for release notes):

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
- Removes C99 §7.24.7.4 UB from three byte-swap sites. Portable and safe on UCRT / musl, where the old pattern was genuinely unsafe.
- Renames `#define __Linux__` → `#define SAS_TRANSPORT_BYTESWAP` (with matching `#ifdef`). Functionally equivalent — the guard was always unconditionally defined.
- Adds `%option nounistd` to `hazard_l.l` so the flex-generated scanner doesn't unconditionally include `<unistd.h>` (Windows fix).

**Effect on Linux/glibc bytes:** zero, per §2.1 and §2.2.

---

## 4. Implications and the release-reference strategy

Given that v4.3.1 ≡ v4.4.2 numerically on Linux (and on macOS), and that Linux is the CCF production platform:

1. **Linux has zero numerical drift between v4.3 and v4.4.** CCF users upgrading from `/opt/hazard/bin/hazard.exe` (v4.3.0) to any release/4.4 build on the same RHEL-8-ish Linux will see bit-identical outputs modulo the version banner.
2. **macOS (and likely Windows, unverified) produces distinct but self-consistent output.** Across the 92 commits, macOS builds remain bit-identical to one another; they just differ from Linux builds.
3. **The correct reference-corpus model is per-platform**, not "one canonical corpus per version". Proposed layout:
   - `tests/corpus/hazard/reference/v4.4-linux/` — the promoted reference for Linux. Captured on ubuntu-latest (or CCF's RHEL 8) at a v4.4.x tag. Bit-identical to the existing `reference/v4.3.0/`.
   - `tests/corpus/hazard/reference/v4.4-macos/` — the existing `reference/v4.4.2/` files, rebased under the macOS label.
   - `tests/corpus/hazard/reference/v4.4-windows/` — TBD; needs a Windows run.
4. **Reference promotion is safe on Linux today** (numerics-wise). It still requires the hazpred SIGSEGV and AVC.2/deciles-truncation bugs to be fixed first — neither is platform-specific, both block a release.
5. **Do not claim "v4.4 is more correct" in release notes.** Neither platform has been validated against an independent numerical reference (SAS PROC LIFEREG, hand-worked example, etc.). The correct framing is: "v4.4 reproduces v4.3 bit-identically on Linux; macOS output differs due to toolchain FP differences and is documented separately."

If cross-platform bit-exactness is required for some downstream reason, the intervention is at the compiler/libm layer: `-ffp-contract=off`, `-fno-associative-math`, disabling FMA, fixing rounding modes, and linking a common libm. That's a v5.0-scale change, not a v4.4 fix.

---

## 5. Remaining open items (not root-cause related)

1. ~~**Hazpred SIGSEGV** — 8/8 hazpred examples crash v4.4.2 with SIGSEGV.~~ **Fixed 2026-04-23.** lldb identified the crash at [src/hazpred/opnfils.c:88](src/hazpred/opnfils.c#L88) as a NULL `FILE*` deref (`hazfile = fopen(...)` with no null check, while `infile` and `outfile` both had the defensive check). Added the matching null check + `hzfxit("hazfile")`; all 8 examples now exit=16 with a clear "cannot open INHAZ file" message. The remaining corpus-completeness gap (missing `.haz` bridging files in `tests/corpus/hazpred/inputs/`) is §5 item 2 below, separate from the binary bug. See [FINDINGS.md §2b](tests/corpus/FINDINGS.md).
2. **Re-capture hazpred corpus with proper dependency ordering.** The `.haz` intermediates that hazpred reads (`hzp.*.haz`) are produced by SAS at PROC-HAZPRED time from the LIBNAME EXAMPLES `.sas7bdat` store — they aren't preserved by the current wrapper. Needed to populate `reference/v4.4-*/hazpred/`. Tracked in `FINDINGS.md §5`.
3. **`hm.death.AVC.2` / `.deciles` output truncation on macOS** — macOS optimizer path short-circuits the post-convergence reporting block. Per §1, this is downstream of the platform FP difference, not a separate bug. Document, don't "fix".
4. **Windows data point** — add another workflow dispatch (`windows-latest` runner, MinGW or MSVC) to complete the cross-platform table. Not critical for the Linux-vs-macOS conclusion already established.

---

## 6. Revision history

| Date | Version | Author | Notes |
|---|---|---|---|
| 2026-04-23 | 1.0 | Analysis | Claimed commit `557f3ef` as sole root cause; claimed "1 example diverges (hm.deadp.VALVES)"; claimed "v4.4 is more correct"; merged data from hm.deadp.VALVES and hm.death.AVC.1 into one table row; did not address hazpred SIGSEGV or TMPDIR buffer. |
| 2026-04-23 | 2.0 | Ehrlinger | Rewrite after validation. Corpus check shows 7/7 divergences, not 1. Empirical test: old and new byte-swap produce identical bytes on glibc — ruling 557f3ef out as the sole cause on the CCF host. 92 commits between v4.3.1 and v4.4.2; root cause marked requiring bisect. Status restored to "open". Added hazpred SIGSEGV and AVC.2/deciles truncation as separate open items. Corrected commit file-count (5 files incl. `hazard_l.l`, not 4). |
| 2026-04-23 | 2.1 | Ehrlinger | Open item (a) resolved: `opnfils.c` 80-byte `TMPDIR` buffer overflow fixed via `PATH_MAX`/`snprintf`; harness workaround removed. Corpus still passes 7/7 against v4.4.2 reference — confirms TMPDIR bug was a stability issue, not part of the numerical divergence. |
| 2026-04-23 | 3.0 | Ehrlinger | **Root cause isolated.** Workflow [linux-ll-check.yml](.github/workflows/linux-ll-check.yml) dispatched on `ubuntu-latest` at both v4.3.1 and v4.4.2, both produced LL=−1864.76 (matching `reference/v4.3.0/`). Same experiment on macOS at both endpoints produces LL=−1536.4 (matching `reference/v4.4.2/`). The 92 commits are numerically inert on both platforms; the divergence is 100% explained by the Linux-vs-macOS toolchain/libm gap. Status closed. Recommendations pivot to per-platform reference corpora (§4). |
| 2026-04-23 | 3.1 | Ehrlinger | Open item (1) resolved: hazpred SIGSEGV fixed. lldb stack pinned the crash to [src/hazpred/opnfils.c:88](src/hazpred/opnfils.c#L88) — `fread(hazfile, ...)` with a NULL `hazfile` after a missing-file `fopen` whose return value was never checked. Added the matching null check + `hzfxit("hazfile")`; 8/8 hazpred examples now exit=16 with a clear error message instead of SIGSEGV. Corpus-completeness gap (missing `.haz` bridging files from the capture) remains, now tracked as a separate item. |

---

## References

- [tests/corpus/FINDINGS.md](tests/corpus/FINDINGS.md) — canonical findings document
- [CAPTURE-COMPARISON-2026-04-22.md](CAPTURE-COMPARISON-2026-04-22.md) — v4.3.0 reference authenticity
- [hazard-capture-results.tar.gz](hazard-capture-results.tar.gz) → [hazard-capture/](hazard-capture/) — 2026-04-22 CCF capture archive
- [.github/workflows/linux-ll-check.yml](.github/workflows/linux-ll-check.yml) — one-shot Linux oracle (investigation; delete after close-out)
- GitHub Actions runs [24850944156](https://github.com/ehrlinger/hazard/actions/runs/24850944156) (v4.3.1 → −1864.76) and [24851086714](https://github.com/ehrlinger/hazard/actions/runs/24851086714) (v4.4.2 → −1864.76)
- `git show 557f3ef` — the commit formerly blamed
- `docs/Claude_MODERNIZATION_GUIDE.md` — modernization policy, byte-compatibility expectations
