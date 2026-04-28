# Windows Validation — v4.4.3 baseline + v4.4.4 prep

**Date:** 2026-04-27
**Host:** `windows-sas-host`
**OS:** Microsoft Windows Server 2019 Datacenter (10.0.17763)
**SAS:** SAS Foundation 9.4 (`C:\Program Files\SASHome\SASFoundation\9.4\sas.exe`)
**Hazard binary:** v4.4.3 (`C:\hazard\bin\hazard.exe`)
**Capture script:** `Run-HazardCapture.ps1` (driven by Cowork session 2026-04-27)

This report supersedes the prior interim diagnosis in `WINDOWS-VALIDATION-2026-04-24.md` (MSYS2 UCRT64). It establishes the v4.4.3 reference baseline on production Windows + SAS, and motivates the cosmetic v4.4.4 maintenance release.

---

## Environment

| Variable | Value |
|---|---|
| `HAZAPPS` | `C:\hazard\bin` |
| `HZEXAMPLES` | `C:\Users\John_Ehrlinger\Documents\dist\examples\` (re-pointed from read-only `C:\hazard\examples` so drivers can write `EXAMPLES.<lib>` outputs) |
| `MACROS` | `C:\hazard\macros` |

The first run used the read-only `C:\hazard\examples` directory and produced `exit=2` for three drivers due to SAS write-permission denials when the drivers tried to populate `EXAMPLES.BSAVCE.DATA` and similar back into the examples directory. Pointing `HZEXAMPLES` at a writable workspace resolved those failures and produced the canonical baseline.

## Result matrix (22 drivers)

| Class | Count | Drivers |
|---|---|---|
| `exit=0` (fully clean) | 5 | `ac.death.AVC`, `bs.death.AVC.summary`, `lg.dead30d.AVC`, `lg.deadlate.AVC`, `lg.death.AVC` |
| `exit=1` (cosmetic `%SYSGET` warning, output produced) | 13 | `bs.death.AVC`, `hm.deadp.VALVES`, `hm.death.AVC`, `hm.death.AVC.deciles`, `hm.death.patient`, `hp.death.AVC`, `hp.death.AVC.hm1`, `hp.death.AVC.hm2`, `hs.death.AVC.hm1`, `hz.deadp.KUL`, `hz.death.AVC`, `hz.te123.OMC`, `hz.tm123.OMC` |
| `exit=2` (unrunnable on this host) | 4 | `hp.death.COMPARISON` (missing `PTCAMHI.HMPDTHE3.DATA`), `hp.dthip.PAIVS.time` (missing `CHSPTR94.HMDTHIPR.DATA`), `hs.dthar.TGA.hs1` (missing `ARTSWT.HSDTARXC.DATA`), `hs.dthar.TGA.setup` (missing `TGASW.HMDTHRI1.DATA`) — all reference CHSS / TGA source datasets that are not present (and not appropriate to stage) on this validation box |

**Net usable baseline: 18 of 22 drivers** (5 exit=0 + 13 exit=1).

## Diagnosis of the `%SYSGET` warning

The benign warning fires inside the `%HAZARD()` and `%HAZPRED()` macros at the OS-detection step:

```sas
%if %length(%sysget(OSTYPE)) gt 0 %then ...
```

`OSTYPE` is a Linux/Unix shell environment variable; it is not present on Windows. SAS responds with the WARNING — but `%sysget` returns an empty string, `%length(...)` is zero, and the macro correctly falls through to its Windows branch. The procedure runs, produces full output, and prints `Procedure HAZARD completed successfully`.

The cost is:

1. SAS's "any WARNING ⇒ exit=1" policy demotes 13 successful runs to exit=1.
2. The log line is noise that distracts from real warnings.
3. Future Darwin/macOS builds would be silently misclassified as Windows under the same logic.

## v4.4.4 fix

Replace the `%sysget(OSTYPE)` probe with SAS automatic macro variables `&SYSSCP` and `&SYSSCPL`, which are populated by SAS at init on every platform and never warn:

```sas
%if &SYSSCP = WIN %then %let os=%str(Windows);
%else %if %index(&SYSSCPL, Linux)  %then %let os=%str(linux);
%else %if %index(&SYSSCPL, Darwin) %then %let os=%str(macos);
%else %if %substr(&SYSSCP,1,3) = SUN %then %let os=%str(solaris);
%else %let os=%lowcase(%substr(&SYSSCP,1,5));
```

Applied identically in `src/scripts/hazard.sas` and `src/scripts/hazpred.sas`.

**Expected outcome of v4.4.4 re-capture against this baseline:**

- 13 drivers move `exit=1` → `exit=0` (warning eliminated)
- All 18 usable `.lst` outputs: byte-identical content modulo SAS page-header timestamp drift (`HH:MM <Day>, <Month> DD, YYYY` mask)
- 4 unrunnable drivers: unchanged

## Reference baseline

The 18 capture `.lst` outputs are committed under `examples/` as the canonical v4.4.3 Windows + SAS reference. When v4.4.4 ships, these files are the **direct-diff regression target** for an operator on a Windows + SAS host: capture v4.4.4 outputs, mask SAS page-header timestamps (recipe in `examples/README-windows-baseline.md`), and `diff` against the committed `.lst`s.

### How this relates to `tests/validate_corpus.sh`

The two artifact types serve different validation flows and are **not** byte-comparable:

| Artifact | Capture mechanism | Content | Consumer |
|---|---|---|---|
| `tests/corpus/hazard/reference/v*/*.lst` | PATH-shadow wrapper (`scripts/capture-legacy.sh`) | Bare `hazard.exe` stdout from `.input` on stdin | `tests/validate_corpus.sh` (direct-binary harness, runs in CI) |
| `examples/*.lst` (this PR) | SAS `PROC HAZARD` | SAS-wrapped listing: page headers, surrounding DATA-step output, "Procedure HAZARD completed successfully" footer | Manual operator diff (per `examples/README-windows-baseline.md`) |

The existing direct-binary harness (`validate_corpus.sh`) already covers cross-platform validation for this PR: it runs on Linux + macOS in CI and was independently verified on Windows MSYS2/UCRT64 (V1/V2/V3 all green vs `v4.3.0` reference, see `WINDOWS-VALIDATION-2026-04-24.md`). The v4.4.4 SAS-macro change does not affect the binary path it exercises, so no harness changes are required for this release.

### Future SAS-driven validation harness (not in this PR)

A separate sister harness that runs SAS, captures `.lst`, masks page-header timestamps, and diffs against `examples/*.lst` would close the SAS-level coverage gap. That work is gated on:

1. A Unix + SAS recapture of the same 18 examples against a v4.4.x Linux build (the user is compiling/testing v4.4.x on Unix after this PR).
2. A new `tests/validate_examples_sas.sh` (or extension to `validate_corpus.sh`) that wraps the SAS invocation pattern.
3. SAS page-header timestamp masking added to `tests/corpus_normalize.sh`.
4. CI runner with SAS available (CCF self-hosted), or a manual gate.

Tracked as a follow-up; intentionally out of scope for this v4.4.4 cosmetic release.

## Capture provenance

- Run 1 (with read-only `HZEXAMPLES`): `hazard-capture-20260427-133838.zip` — diagnostic only, three drivers wrecked by write-permission denials, **discarded**.
- Run 2 (with writable `HZEXAMPLES`): `hazard-capture-20260427-134953.zip` — **canonical v4.4.3 Windows baseline**.

Run 2 was diffed against Run 1 to confirm that all data deltas in the three formerly-failing drivers (`bs.death.AVC`, `hm.death.AVC`, `hz.death.AVC`) are explained by the upstream driver populating data correctly when not blocked by perms. No HAZARD-binary deltas were observed.
