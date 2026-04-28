# Windows + SAS reference (v4.4.5, x64-sas bucket)

**20 driver `.lst` outputs** captured on `windows-sas-host` (Windows SAS host) on 2026-04-28 against:

- **OS**: Windows Server 2019 Datacenter (10.0.17763)
- **SAS**: SAS Foundation 9.4 (`C:\Program Files\SASHome\SASFoundation\9.4\sas.exe`)
- **Hazard binary**: v4.4.3 install at `C:\hazard\bin\hazard.exe` with the **v4.4.4 patched SAS macros** (`hazard.sas` / `hazpred.sas`) overlaid into `C:\hazard\macros\`. Binary-level behaviour is byte-identical to v4.4.4 (no C-code changes between the two releases).

Source archive: `hazard-capture-20260428-073323.zip`.

## Coverage vs. the 22 example drivers

| Class | Count | Drivers |
|---|---|---|
| `.lst` captured here | 20 | (the files in this directory) |
| Failed to produce `.lst` | 3 | `bs.death.AVC.summary` (column-order issue, see "known gaps" below); `hs.dthar.TGA.hs1` (errored before output: `Variable SOURCE not found`); `hmdeadp` (bogus — turned out to be a SAS dataset binary file misnamed `hmdeadp.sas`, not a real driver) |

## Validation

`tests/validate_examples_sas.sh` auto-selects this bucket on Cygwin / MSYS / MinGW hosts. Override on Linux / macOS to compare a Windows + SAS capture against this reference:

```sh
REFERENCE=v4.4.5-windows-x64-sas \
    tests/validate_examples_sas.sh --got-dir <extracted-tarball-dir>
```

The harness uses `tests/corpus_normalize.sh`, which masks the per-page SAS timestamp (`HH:MM Weekday, Month DD, YYYY`) along with the binary version banner, so byte-diffs survive run-to-run timestamp drift and cross-version banner changes.

## Known gaps

### `bs.death.AVC.summary` and `bs.death.AVC` column ordering

`bs.death.AVC.sas` calls `%HAZBOOT(IN=AVCS, SEED=-1, ...)`. `SEED=-1` means "use the current wall-clock time as the bootstrap-resampling seed", so different runs on different days produce different stepwise-selection paths and thus different column orderings in the resulting `EXAMPLES.BSEARLY` dataset. `bs.death.AVC.summary.sas` then references that dataset with the SAS `AGE--ORIFICE` range syntax, which only works if `AGE` actually appears before `ORIFICE` in the dataset's column order — which the time-seeded bootstrap may or may not produce.

Two consequences for this reference set:

1. **`bs.death.AVC.summary.lst` is not in this bucket** because its run on 2026-04-28 happened to draw a seed that produced `ORIFICE`-before-`AGE` ordering, and the summary driver errored out before producing output. A different day's seed could just as easily produce a working ordering.
2. **`bs.death.AVC.lst` is here, but its specific column ordering is run-specific** rather than reproducible. A future v4.4.5+ capture may not byte-match this `bs.death.AVC.lst` until the macro is changed to use a fixed seed (or `bs.death.AVC.summary.sas` is rewritten to be order-agnostic). Treat the bs.death.AVC capture as a snapshot of "what v4.4.4 macros + SAS 9.4 produced on this date with this random draw," not as a deterministic reference.

Tracked for future v4.4.x or v5 work.

### `hs.dthar.TGA.hs1`

Errored with "Variable SOURCE not found / Variable STUDY not found" before reaching PROC HAZARD. The driver depends on a TGA-study libref that isn't populated in the Windows SAS host examples workspace. Same shape as the 4 always-unrunnable drivers (CHSS / TGA librefs), but this one happens to fail earlier in the SAS data step rather than in PROC HAZARD itself, so no `.lst` is produced. The 4 partial-`.lst` drivers (`hp.death.COMPARISON`, `hp.dthip.PAIVS.time`, `hs.dthar.TGA.setup` are committed here; `hs.dthar.TGA.hs1` is the one that fails earlier and produces nothing).

### Linux + SAS reference: deferred

A parallel `v4.4.5-linux-x64-sas/` reference bucket is **not** shipped in v4.4.5. The CCF Linux SAS install exhibits a silent-failure mode in bootstrap-mode drivers regardless of which hazard binary is invoked. Full root cause and v5 design implications: [`docs/v5-design-input/2026-04-28-silent-failure-postmortem.md`](../../../../docs/v5-design-input/2026-04-28-silent-failure-postmortem.md). The closing fix is the v5 roadmap item "detect / validate XPORT input format at hazard.exe startup".

## .meta files

Each `<case>.meta` file mirrors the `scripts/capture-legacy.sh` schema with a synthesized timestamp (`2026-04-28T07:33:23Z`, the start of the source capture run). `real_exit=0` is honest for the 17 drivers that exited cleanly *and* the 3 that exited with `exit=1` (the SAS exit-code bump from a benign cosmetic warning that v4.4.4's macro fix was intended to eliminate; the .lst content of those 3 is correct anyway).
