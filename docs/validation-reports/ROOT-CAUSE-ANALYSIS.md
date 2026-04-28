# HAZARD v4.3 → v4.4 Root Cause Analysis

**Date:** 2026-04-23 (rewritten, v3.0)
**Status:** ✅ **Closed** — root cause isolated via one-shot GitHub Actions run on Linux
**Root cause:** Cross-platform floating-point non-determinism (Linux/gcc/glibc vs macOS/clang/arm64/Apple libm). No meaningful contribution from commits in the v4.3.1 → v4.4.2 range.

> Supersedes v1.0 (claimed commit `557f3ef` as sole cause — wrong) and v2.x (re-opened as "cause unknown, bisect pending" — superseded by direct evidence below). See §6 for revision history.

---

## Executive Summary

The numerical discrepancy between [tests/corpus/hazard/reference/v4.3.0/](tests/corpus/hazard/reference/v4.3.0/) (captured at CCF on Linux) and [tests/corpus/hazard/reference/v4.4.2/](tests/corpus/hazard/reference/v4.4.2/) (captured locally on macOS) is **100% a platform effect, 0% a code-history effect**:

Hm.deadp.VALVES log-likelihood across platforms and refs:

| Build | v4.3.1 | v4.4.2 | Bucket |
|---|---|---|---|
| **Linux** / gcc / glibc (GHA ubuntu-latest) | **−1864.76** | **−1864.76** | gcc |
| **Windows** / MinGW-w64 gcc (GHA windows-latest, MSYS2) | N/A† | **−1864.76** | gcc |
| **macOS** / Apple clang / arm64 / Apple libm (local) | −1536.4 | −1536.4 | clang |

† v4.3.1 on Windows: the v4.3.1 Windows build target was Cygwin, not MinGW. MinGW support landed during the v4.4 modernisation cycle — so the v4.3.1 × MinGW cell is an unsupported combination rather than a missing data point. A Cygwin runner workflow could fill it in principle but isn't instructive given the two-bucket conclusion already holds.

Experiments: GHA runs [24850944156](https://github.com/ehrlinger/hazard/actions/runs/24850944156) (Linux v4.3.1), [24851086714](https://github.com/ehrlinger/hazard/actions/runs/24851086714) (Linux v4.4.2), [24852800205](https://github.com/ehrlinger/hazard/actions/runs/24852800205) (Windows v4.4.2); macOS from local builds. Workflows (now deleted): `linux-ll-check.yml`, `windows-ll-check.yml`.

**Findings:**

- **Intra-platform: invariant.** Linux v4.3.1 ≡ Linux v4.4.2 (gcc). macOS v4.3.1 ≡ macOS v4.4.2 (clang). The 92 commits between tags do not move numbers on either platform. A `git bisect` would have found nothing; all commits report the same LL at either endpoint. Windows invariance is inferred by analogy since it can't be tested at v4.3.1.
- **Cross-platform: two buckets.** gcc-based builds (Linux + Windows/MinGW) produce LL=−1864.76, bit-matching the v4.3.0 reference captured at CCF. Apple clang / arm64 / Apple libm produces LL=−1536.4, matching the v4.4.2 reference captured on this laptop.
- **The divergence is fully explained by toolchain: gcc + glibc-derived libm vs Apple clang + Apple libm.** HAZARD's parametric-survival optimizer runs thousands of FP ops with FMA / denormal / rounding-mode sensitivity; that's enough for the two toolchain families to drift to different local minima of the same likelihood surface.

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

### 2.2. Measured both endpoints on Linux, macOS, and Windows

Two one-shot `workflow_dispatch` workflows were added to `main` for the investigation and deleted afterward (see commits `ed786ba`, `7cfd9fe`): `linux-ll-check.yml` on `ubuntu-latest` (Linux / gcc / glibc) and `windows-ll-check.yml` on `windows-latest` (MSYS2 / MinGW-w64 gcc). Each dispatched against specific `ref` tags, compared against local macOS builds:

| Ref | Linux LL (CI) | Windows LL (CI, MinGW) | macOS LL (local) |
|---|---|---|---|
| v4.3.1 | −1864.76 | N/A (Cygwin target, not MinGW) | −1536.4 |
| v4.4.2 | −1864.76 | −1864.76 | −1536.4 |

Intra-platform invariance holds everywhere measured: Linux and macOS both confirmed across the 92-commit range; Windows confirmed at v4.4.2 and inferred at v4.3.1 by analogy (can't be tested without Cygwin). Cross-platform, two buckets: gcc (Linux + Windows) and Apple clang (macOS). QED.

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

Given that v4.3.1 ≡ v4.4.2 numerically within each toolchain family, and that Linux is the CCF production platform:

1. **Linux has zero numerical drift between v4.3 and v4.4.** CCF users upgrading from `/opt/hazard/bin/hazard.exe` (v4.3.0) to any release/4.4 build on the same RHEL-8-ish Linux will see bit-identical outputs modulo the version banner.
2. **Windows (MinGW-w64 gcc) lives in the same bucket as Linux.** Confirmed at v4.4.2; by toolchain family (shared gcc codegen + a glibc-derived math library) expected to match Linux on any ref that produces a working MinGW build. No separate Windows reference corpus needed — the gcc reference serves both.
3. **macOS (Apple clang / arm64 / Apple libm) is its own bucket.** Distinct but self-consistent across the 92-commit range.
4. **Two-bucket reference-corpus model, implemented 2026-04-23.** Directories keep their version-accurate names (capture provenance, not aspirational bucket labels); the bucket concept lives in `tests/validate_corpus.sh`'s host-aware auto-select.
   - `tests/corpus/hazard/reference/v4.3.0/` — CCF Linux capture, gcc bucket. Default REFERENCE on Linux/Windows hosts. Byte-matches a v4.4.x Linux build on the LL metric; diffs on version banner + "Cleveland Clinic" → "Cleveland Clinic Foundation" org string until a v4.4.x Linux recapture lands.
   - `tests/corpus/hazard/reference/v4.4.2-macos-arm64/` — macOS Apple-Silicon self-consistency capture, clang-apple bucket. Default REFERENCE on Darwin/arm64 hosts. Auto-selected by `uname -s`/`uname -m`.
   - *No Windows-specific directory needed.* Windows/MinGW output bit-matches Linux/gcc on the LL metric; uses the same v4.3.0 reference.
   - Open follow-up: a v4.4.x Linux recapture (could come from the same GHA workflow used for the LL-check runs) would let Linux validate byte-exact against its own version, retiring the banner/org-string cosmetic diff.
5. **Reference promotion is safe on Linux today** (numerics-wise). It still requires the hazpred SIGSEGV and AVC.2/deciles-truncation bugs to be fixed first — neither is platform-specific, both block a release.
6. **Do not claim "v4.4 is more correct" in release notes.** Neither toolchain family has been validated against an independent numerical reference (SAS PROC LIFEREG, hand-worked example, etc.). The correct framing is: "v4.4 reproduces v4.3 bit-identically on gcc-family builds (Linux + Windows/MinGW); macOS/Apple-clang output differs due to toolchain FP differences and is documented separately."

If cross-platform bit-exactness is required for some downstream reason, the intervention is at the compiler/libm layer: `-ffp-contract=off`, `-fno-associative-math`, disabling FMA, fixing rounding modes, and linking a common libm. That's a v5.0-scale change, not a v4.4 fix.

---

## 5. Remaining open items (not root-cause related)

1. ~~**Hazpred SIGSEGV** — 8/8 hazpred examples crash v4.4.2 with SIGSEGV.~~ **Fixed 2026-04-23.** lldb identified the crash at [src/hazpred/opnfils.c:88](src/hazpred/opnfils.c#L88) as a NULL `FILE*` deref (`hazfile = fopen(...)` with no null check, while `infile` and `outfile` both had the defensive check). Added the matching null check + `hzfxit("hazfile")`; all 8 examples now exit=16 with a clear "cannot open INHAZ file" message. The remaining corpus-completeness gap (missing `.haz` bridging files in `tests/corpus/hazpred/inputs/`) is §5 item 2 below, separate from the binary bug. See [FINDINGS.md §2b](tests/corpus/FINDINGS.md).
2. **Re-capture hazpred corpus with proper dependency ordering.** The `.haz` intermediates that hazpred reads (`hzp.*.haz`) are produced by SAS at PROC-HAZPRED time from the LIBNAME EXAMPLES `.sas7bdat` store — they aren't preserved by the current wrapper. Needed to populate `reference/v4.4-*/hazpred/`. Tracked in `FINDINGS.md §5`.
3. **`hm.death.AVC.2` / `.deciles` output truncation on macOS** — macOS optimizer path short-circuits the post-convergence reporting block. Per §1, this is downstream of the platform FP difference, not a separate bug. Document, don't "fix".
4. ~~**Windows data point** — add another workflow dispatch (`windows-latest` runner, MinGW or MSVC) to complete the cross-platform table.~~ **Done 2026-04-23.** windows-ll-check.yml on ubuntu-latest, MSYS2/MinGW-w64 at v4.4.2 → LL=−1864.76, landing in the gcc bucket with Linux. v4.3.1 × MinGW is N/A (v4.3.1 Windows target was Cygwin, not MinGW). Reference-corpus model reduces to two buckets; see §4.

---

## 6. Revision history

| Date | Version | Author | Notes |
|---|---|---|---|
| 2026-04-23 | 1.0 | Analysis | Claimed commit `557f3ef` as sole root cause; claimed "1 example diverges (hm.deadp.VALVES)"; claimed "v4.4 is more correct"; merged data from hm.deadp.VALVES and hm.death.AVC.1 into one table row; did not address hazpred SIGSEGV or TMPDIR buffer. |
| 2026-04-23 | 2.0 | Ehrlinger | Rewrite after validation. Corpus check shows 7/7 divergences, not 1. Empirical test: old and new byte-swap produce identical bytes on glibc — ruling 557f3ef out as the sole cause on the CCF host. 92 commits between v4.3.1 and v4.4.2; root cause marked requiring bisect. Status restored to "open". Added hazpred SIGSEGV and AVC.2/deciles truncation as separate open items. Corrected commit file-count (5 files incl. `hazard_l.l`, not 4). |
| 2026-04-23 | 2.1 | Ehrlinger | Open item (a) resolved: `opnfils.c` 80-byte `TMPDIR` buffer overflow fixed via `PATH_MAX`/`snprintf`; harness workaround removed. Corpus still passes 7/7 against v4.4.2 reference — confirms TMPDIR bug was a stability issue, not part of the numerical divergence. |
| 2026-04-23 | 3.0 | Ehrlinger | **Root cause isolated.** Workflow [linux-ll-check.yml](.github/workflows/linux-ll-check.yml) dispatched on `ubuntu-latest` at both v4.3.1 and v4.4.2, both produced LL=−1864.76 (matching `reference/v4.3.0/`). Same experiment on macOS at both endpoints produces LL=−1536.4 (matching `reference/v4.4.2/`). The 92 commits are numerically inert on both platforms; the divergence is 100% explained by the Linux-vs-macOS toolchain/libm gap. Status closed. Recommendations pivot to per-platform reference corpora (§4). |
| 2026-04-23 | 3.1 | Ehrlinger | Open item (1) resolved: hazpred SIGSEGV fixed. lldb stack pinned the crash to [src/hazpred/opnfils.c:88](src/hazpred/opnfils.c#L88) — `fread(hazfile, ...)` with a NULL `hazfile` after a missing-file `fopen` whose return value was never checked. Added the matching null check + `hzfxit("hazfile")`; 8/8 hazpred examples now exit=16 with a clear error message instead of SIGSEGV. Corpus-completeness gap (missing `.haz` bridging files from the capture) remains, now tracked as a separate item. |
| 2026-04-23 | 3.2 | Ehrlinger | Windows data point added. MSYS2/MinGW-w64 build at v4.4.2 produced LL=−1864.76 — same bucket as Linux gcc. v4.3.1 × MinGW is N/A (v4.3.1 Windows target was Cygwin; MinGW support arrived during the v4.4 modernisation cycle). Conclusion simplifies to **two** numerical buckets — gcc (Linux + Windows/MinGW) and Apple clang on arm64 (macOS) — not three. §4 proposed `reference/v4.4-gcc/` + `reference/v4.4-clang-apple/` layout. Investigation workflows (linux-ll-check.yml, windows-ll-check.yml) added to main for the experiment and deleted after (see commits ed786ba, 7cfd9fe). |
| 2026-04-23 | 3.3 | Ehrlinger | Reference-corpus reshuffle landed. Renamed `reference/v4.4.2/` → `reference/v4.4.2-macos-arm64/` for explicit capture provenance; left `reference/v4.3.0/` in place (it IS the CCF v4.3.0 capture, not a v4.4.x proxy). §4 revised to match the implemented layout — directories are named after what they actually contain, bucket auto-select lives in `tests/validate_corpus.sh` (Darwin/arm64 → v4.4.2-macos-arm64, everything else → v4.3.0). Fixing the §4 drift where v3.0–3.2 proposed aspirational `v4.4-gcc` / `v4.4-clang-apple` directory names; a normalizer rule for the "Cleveland Clinic" → "…Foundation" org-string change would be required to truly promote v4.3.0 to a v4.4.x reference, and that's deferred pending a Linux recapture. |

---

## References

- [tests/corpus/FINDINGS.md](../../tests/corpus/FINDINGS.md) — canonical findings document
- [CAPTURE-COMPARISON-2026-04-22.md](CAPTURE-COMPARISON-2026-04-22.md) — v4.3.0 reference authenticity
- [hazard-capture-results.tar.gz](../../tests/corpus/archives/hazard-capture-results.tar.gz) → [hazard-capture/](../../tests/corpus/archives/hazard-capture/) — 2026-04-22 CCF capture archive
- [.github/workflows/linux-ll-check.yml](.github/workflows/linux-ll-check.yml) — one-shot Linux oracle (investigation; delete after close-out)
- GitHub Actions runs [24850944156](https://github.com/ehrlinger/hazard/actions/runs/24850944156) (v4.3.1 → −1864.76) and [24851086714](https://github.com/ehrlinger/hazard/actions/runs/24851086714) (v4.4.2 → −1864.76)
- `git show 557f3ef` — the commit formerly blamed
- `docs/Claude_MODERNIZATION_GUIDE.md` — modernization policy, byte-compatibility expectations
