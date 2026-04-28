# HAZARD v4.3.0 Capture Comparison — 2026-04-22

**Source:** v4.3.0 production binary (`/opt/hazard/bin/hazard.exe`) on Linux UNIX (lri-sas-p-02)  
**Timestamp:** 2026-04-22T19:58:23Z – 19:58:33Z (10 s span)  
**Total Captures:** 8 runs (7 exit=0, 1 exit=1) → **6 unique output hashes**  
**Host:** Linux 4.18.0-553.89.1.el8_10.x86_64  
**Capture archive:** [hazard-capture-results.tar.gz](../../tests/corpus/archives/hazard-capture-results.tar.gz) → extracted to [hazard-capture/hazard/](../../tests/corpus/archives/hazard-capture/hazard/)  
**Revision history:** §7

---

## Executive Summary

✅ **All 7 successful captures are SHA-256-identical to a file in [tests/corpus/hazard/reference/v4.3.0/](tests/corpus/hazard/reference/v4.3.0/).** The failed capture (exit=1) matches nothing, as expected.

**What this proves:** the v4.3.0 production binary at CCF is deterministic on its own host — running the same inputs twice ten days apart produces identical bytes. It does **not** prove the v4.3.0 outputs are numerically "correct"; it proves only that the repo reference set is an authentic copy of what that binary produces.

**Caveat:** two of the successful captures hash-collide on the identical 16,877-byte `hm.death.AVC.2` / `hm.death.AVC.deciles` content (those two reference files are themselves bit-identical), so the 7 successful captures cover **6 unique outputs**, not 7.

### Metadata

| Runs | Exit 0 | Exit 1 | Hash-matched to reference | Unique outputs |
|---|---|---|---|---|
| 8 | 7 | 1 | 7/7 of exit=0 | 6 |

---

## Detailed Results

### ✅ IDENTICAL Captures (Byte-for-Byte)

#### 1. **hm.deadp.VALVES** (Hardening of Myocardium — Death post VALVES)
- **Capture UID:** `1776887903.1199006.806`
- **Job ID:** `J1198866`
- **Exit Code:** 0 ✅
- **Capture Size:** 41,224 B
- **Reference Size:** 41,224 B
- **Match:** ✅ **PERFECT** (byte-for-byte identical)
- **Status:** Reference-grade capture ✨

#### 2. **hm.death.AVC.2** (Hardening of Myocardium — Death, Aortic Valve Correction, variant 2)
- **Capture UID:** `1776887905.1199922.15376`
- **Job ID:** `J1199589` (variant marker: X9)
- **Exit Code:** 0 ✅
- **Capture Size:** 16,877 B
- **Reference Size:** 16,877 B
- **Match:** ✅ **PERFECT** (byte-for-byte identical)
- **Status:** Reference-grade capture ✨

#### 3. **hm.death.AVC.1** (Hardening of Myocardium — Death, Aortic Valve Correction, variant 1)
- **Capture UID:** `1776887905.1199794.3358`
- **Job ID:** `J1199589` (variant marker: X3)
- **Exit Code:** 0 ✅
- **Capture Size:** 12,684 B
- **Reference Size:** 12,684 B
- **Match:** ✅ **PERFECT** (byte-for-byte identical)
- **Status:** Reference-grade capture ✨

#### 4. **hz.tm123.OMC** (Hazard Time Month — OMC data)
- **Capture UID:** `1776887913.1205069.5584`
- **Job ID:** `J1204939`
- **Exit Code:** 0 ✅
- **Capture Size:** 8,942 B
- **Reference Size:** 8,942 B
- **Match:** ✅ **PERFECT** (byte-for-byte identical)
- **Status:** Reference-grade capture ✨

### ⚠️ Captures with Ambiguous / Corrected Labels

The following captures were run with labels that didn't match their hash-identified content. SHA-256 identity is definitive; job-ID labels are descriptive.

#### 5. **hm.death.AVC.2 / .deciles** (label said hm.death.AVC.1)
- **Capture UID:** `1776887904.1199267.17547`
- **Job ID:** `J1199128.X5`
- **Exit Code:** 0 ✅
- **Capture Size:** 16,877 B
- **SHA-256:** `6c8aa8dd…89f290`
- **Matches:** `hm.death.AVC.2.lst` AND `hm.death.AVC.deciles.lst` (those two reference files are themselves bit-identical)
- **Does NOT match:** `hz.death.AVC.lst` (7,864 B, hash `0f9db27c…170962`) — this correction supersedes the original v1.0 claim that this UID was `hz.death.AVC`.

#### 6. **hz.deadp.KUL** (label said hm.death.AVC.deciles)
- **Capture UID:** `1776887910.1203496.15298`
- **Job ID:** `J1203364.X5`
- **Exit Code:** 0 ✅
- **Capture Size:** 9,256 B
- **SHA-256 match:** `hz.deadp.KUL.lst` ✅

#### 7. **hz.death.AVC** (label said hz.deadp.KUL)
- **Capture UID:** `1776887911.1203762.14238`
- **Job ID:** `J1203629.X5`
- **Exit Code:** 0 ✅
- **Capture Size:** 7,864 B
- **SHA-256 match:** `hz.death.AVC.lst` ✅ (this is the only true `hz.death.AVC` capture in the set)

### ❌ Problematic Capture

#### 8. **hz.death.AVC** (Exit Code 1 — FAILED)
- **Capture UID:** `1776887911.1204341.7895`
- **Job ID:** `J1204208`
- **Exit Code:** 1 ❌ **FAILURE**
- **Capture Size:** 9,689 B
- **Expected Sizes:**
  - hz.deadp.KUL: 9,256 B (diff: +433 B)
  - hz.death.AVC: 7,864 B (diff: +1,825 B)
- **Match:** ❌ **DIFFERENT** (did not capture successfully)
- **Reason:** Binary exit code 1 indicates an error during execution
- **Action Required:** This example should be re-captured

---

## Summary Table (hash-verified)

| Capture UID | Exit | Size | SHA-256 matches | Label notes |
|---|---|---|---|---|
| 1776887903.1199006.806 | 0 | 41,224 | hm.deadp.VALVES | correct |
| 1776887904.1199267.17547 | 0 | 16,877 | hm.death.AVC.2 / .deciles | label said AVC.1 |
| 1776887905.1199794.3358 | 0 | 12,684 | hm.death.AVC.1 | correct |
| 1776887905.1199922.15376 | 0 | 16,877 | hm.death.AVC.2 / .deciles | correct |
| 1776887910.1203496.15298 | 0 | 9,256 | hz.deadp.KUL | label said AVC.deciles |
| 1776887911.1203762.14238 | 0 | 7,864 | hz.death.AVC | label said KUL |
| 1776887911.1204341.7895 | **1** | 9,689 | (none) | failed run |
| 1776887913.1205069.5584 | 0 | 8,942 | hz.tm123.OMC | correct |

Unique output hashes across the 7 successful captures: **6** (AVC.2 and AVC.deciles reference files are bit-identical, so two UIDs map to the same hash).

---

## What This Does (and Does Not) Prove

### What it proves
1. **The v4.3.0 binary at CCF is deterministic.** Rerunning 7 examples ten days after the reference capture produces bit-identical output.
2. **The repo reference corpus is an authentic copy** of what `/opt/hazard/bin/hazard.exe` produces on `lri-sas-p-02`. It has not been tampered with or regenerated on a different host.
3. **Capture-order labels cannot be trusted.** 3 of 7 captures were labelled for an example whose hash belongs to a different example. Hash identity, not the job-ID label, is authoritative.

### What it does NOT prove
1. **It does not prove v4.3.0 is "correct."** Reproducibility on one host ≠ correctness. Two runs on the same Linux/x86-64 host invoking the same `swab()` implementation and the same libc will produce the same bytes whether or not the underlying computation is portable or standards-compliant.
2. **It does not isolate v4.3 → v4.4 divergences to a single cause.** That claim lives in [ROOT-CAUSE-ANALYSIS.md](ROOT-CAUSE-ANALYSIS.md) and depends on evidence not in this document.
3. **It does not cover hazpred.** Only hazard captures were hash-checked here. hazpred captures exist in [hazard-capture/hazpred/](../../tests/corpus/archives/hazard-capture/hazpred/) and are out of scope for this file.

### Action Items

1. **Re-capture UID 1776887911.1204341.7895** (exit=1) to complete the set. Consult the operator-side SAS log for the reason — likely dataset or environment, not a binary crash, since an output file was still written.
2. **Correct label mapping** wherever the three mislabelled UIDs are referenced — labels follow hash identity, not job ID.

---

## Appendix: SHA-256 Digests

```
07b121c8a79f13624e83d82862b07bbbfe0367e96f17b7cab41fccd68a198104  hz.deadp.KUL
0f9db27c7d6e2046df0024278ec02d7e9f483a50bed186fac09a392b63170962  hz.death.AVC
442175bfbb2aab1f1db1368be8fe5c428731e0f676114acf6b48e1e7cf1f2837  hm.death.AVC.1
5690f4ed1be8573d606cdd5f7367049c4fb7a4eae13636748b87c435f97f584a  hz.tm123.OMC
6721a7731dacfc111af306ca86e2e1ee6e7fe5bbccaa6d0db8dd8be726fec508  hm.deadp.VALVES
6c8aa8dd3462c5e0752040c7bf29e4d3b2c701997398277400d768f45d89f290  hm.death.AVC.2  AND  hm.death.AVC.deciles
40f97616d0e88d9d10f3a2428f0619d573f86649fd313b34ae6d54a62bc69d49  UID 1776887911.1204341.7895 (failed run, no reference match)
```

Regenerate with:
```
shasum -a 256 tests/corpus/hazard/reference/v4.3.0/*.lst hazard-capture/hazard/*.lst
```

---

## Document Version

| Date | Version | Author | Notes |
|---|---|---|---|
| 2026-04-23 | 1.0 | Analysis | Initial comparison against v4.3.0 references |
| 2026-04-23 | 1.1 | Ehrlinger | Hash-verified against extracted [hazard-capture-results.tar.gz](../../tests/corpus/archives/hazard-capture-results.tar.gz). Corrected UID 1776887904.1199267.17547 content label (AVC.2/.deciles, not hz.death.AVC). Clarified "6 unique outputs" vs earlier "7 unique examples". Removed unsupported "v4.4 validation is sound" and "ready for release" claims — those live in [ROOT-CAUSE-ANALYSIS.md](ROOT-CAUSE-ANALYSIS.md) and [tests/corpus/FINDINGS.md](../../tests/corpus/FINDINGS.md), respectively. |

---

**Status:** ✅ v4.3.0 reference corpus confirmed as a faithful copy of CCF production output. Numerical correctness and v4.4 parity are separate questions — see [ROOT-CAUSE-ANALYSIS.md](ROOT-CAUSE-ANALYSIS.md) and [tests/corpus/FINDINGS.md](../../tests/corpus/FINDINGS.md).
