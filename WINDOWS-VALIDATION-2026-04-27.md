# Windows Validation — v4.4.3 baseline + v4.4.4 prep

**Date:** 2026-04-27
**Host:** `AWOR-PDSASAPP03`
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

The 18 capture `.lst` outputs are committed under `examples/` as the canonical v4.4.3 Windows reference. When v4.4.4 ships, these files become the regression target for `validate_corpus.sh` on Windows hosts.

Suggested follow-up: relocate to `tests/corpus/hazard/reference/v4.4.3-windows-x64/` to slot into the existing reference-corpus structure, alongside `v4.3.0/` (Linux/Windows MinGW gcc) and `v4.4.2-macos-arm64/` (Apple clang). That placement makes the validate harness auto-select the right reference per host.

## Capture provenance

- Run 1 (with read-only `HZEXAMPLES`): `hazard-capture-20260427-133838.zip` — diagnostic only, three drivers wrecked by write-permission denials, **discarded**.
- Run 2 (with writable `HZEXAMPLES`): `hazard-capture-20260427-134953.zip` — **canonical v4.4.3 Windows baseline**.

Run 2 was diffed against Run 1 to confirm that all data deltas in the three formerly-failing drivers (`bs.death.AVC`, `hm.death.AVC`, `hz.death.AVC`) are explained by the upstream driver populating data correctly when not blocked by perms. No HAZARD-binary deltas were observed.
