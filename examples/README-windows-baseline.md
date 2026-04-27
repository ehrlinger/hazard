# Windows v4.4.3 Reference Baseline

This directory now ships 18 `.lst` reference outputs captured under SAS 9.4 Foundation on Windows Server 2019 against the v4.4.3 hazard binary. They serve as the regression target for the v4.4.4 maintenance release and any subsequent Windows-on-SAS RC.

## Capture provenance

| Field          | Value                                                       |
|----------------|-------------------------------------------------------------|
| Date           | 2026-04-27                                                  |
| Host           | `windows-sas-host`                                           |
| OS             | Windows Server 2019 Datacenter (10.0.17763)                 |
| SAS            | Foundation 9.4                                              |
| Hazard binary  | v4.4.3 (`C:\hazard\bin\hazard.exe`)                         |
| Capture script | `Run-HazardCapture.ps1` (Cowork session, 2026-04-27)        |

Full details in `../WINDOWS-VALIDATION-2026-04-27.md`.

## Diff convention

When comparing a future RC's captured `.lst` against these files, mask SAS page-header timestamps before diffing. They take the form:

```
<Title> <padding> HH:MM <Day>, <Month> DD, YYYY  <page#>
```

A safe `sed` mask:

```
sed -E 's/[0-9]{1,2}:[0-9]{2} (Mon|Tue|Wed|Thu|Fri|Sat|Sun)[a-z]+, [A-Za-z]+ [0-9]{1,2}, [0-9]{4}/<TS>/g'
```

After masking, all 18 files are expected to be byte-identical between v4.4.3 and v4.4.4 — the v4.4.4 fix is a SAS-macro-only change with no effect on `.lst` content.

## Coverage

22 example drivers exist; 18 produce reference output here. The remaining 4 are unrunnable on this validation host because they require non-redistributable source datasets (CHSS / TGA studies):

| Unrunnable driver | Required source dataset |
|---|---|
| `hp.death.COMPARISON` | `PTCAMHI.HMPDTHE3.DATA` |
| `hp.dthip.PAIVS.time` | `CHSPTR94.HMDTHIPR.DATA` |
| `hs.dthar.TGA.hs1` | `ARTSWT.HSDTARXC.DATA` |
| `hs.dthar.TGA.setup` | `TGASW.HMDTHRI1.DATA` |

These will only validate on a CCF host with the original study librefs mounted.

## Future placement

These files belong long-term in `tests/corpus/hazard/reference/v4.4.3-windows-x64/` to slot into the existing `validate_corpus.sh` host-toolchain auto-selection (alongside `v4.3.0/` and `v4.4.2-macos-arm64/`). They were placed in `examples/` for the v4.4.4 release in keeping with the historic placement of `examples/hm.death.patient.lst`, but a follow-up commit can relocate them once the harness is updated.
