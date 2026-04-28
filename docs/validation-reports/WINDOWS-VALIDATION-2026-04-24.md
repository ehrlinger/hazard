# Windows Validation Report — v4.4.3 — 2026-04-24

**Version under test:** v4.4.3
**Purpose:** Confirm the HAZARD v4.4.3 test suite builds and runs on Windows, and
that numerical results match the v4.1.0 reference baseline.

**Status:** PASSED (with documented gaps).

---

## Environment

| Item | Value |
|------|-------|
| Host | `WKSOREG-ACSOHVS` |
| Shell | MSYS2 UCRT64 (`/d/Users/John_Ehrlinger/Downloads/hazard-main`) |
| Toolchain | GCC via MSYS2 UCRT64 |
| Binary | `src/hazard/hazard.exe` |
| Reference | `tests/4.1.0/` |
| Date | 2026-04-24 11:28 local |

Related procedure doc: [docs/MULTI-PLATFORM-VALIDATION.md](docs/MULTI-PLATFORM-VALIDATION.md),
Phase 4.

---

## Unit tests — 150 / 150 passed

Invocation: `make -C tests/unit check`

| Suite | Tests | Result |
|-------|------:|--------|
| `test_math_helpers` (hzd_ln_eX_min1, hzd_ln_A_div_B, hzd_ln_1min_e_negeX, dlgaob) | 43 | ALL PASS |
| `test_shaping_functions` (hzd_ln_G1_and_SG1, 6 flag cases + error paths) | 24 | ALL PASS |
| `test_obs_layout` (pp7=7 addressing, Im1p7 indexing, non-overlap, roundtrip) | 34 | ALL PASS |
| `test_error_describe` (dlg1/dlg3/dtrsfm/hzpm/hzpo/misc codes + edge cases) | 27 | ALL PASS |
| `test_log` (env var parsing, threshold handling, emission gating) | 22 | ALL PASS |
| **Total** | **150** | **ALL PASS** |

Deliberate error-path tests emit `[hazard ERROR hzd_Error.c:26 hzd_Error_at] DLG1900 / DLG1910` lines by design (these exercise the `longjmp`/`setjmp` error trap).

---

## Integration tests

Invocation:

```bash
HAZARD_BIN=$PWD/src/hazard/hazard.exe \
  bash tests/run_all_tests.sh --integration-only --skip-memory
```

### V1 — Numerical Regression: 13 / 13 passed

| Case | Checks |
|------|--------|
| `hz.death.AVC` | LL = -210.501, events = 70, convergence, params E2=E3, E3=0.3637061, E0=-1.4432, C0=-0.0347475 — all match |
| `hm.death.AVC` | step0 LL = -181.109, events = 69.99996, convergence — all match |
| `hz.deadp.KUL` | LL = -3740.52, convergence — match |
| `version_regression` | `loglik_stable_across_versions` — (-210.501 vs -210.501) |

### V2 — Optimizer Convergence: 20 / 20 passed

| Case | Checks |
|------|--------|
| `hz.death.AVC` | loglik_8dp, nfncts, iterations, condcode (ref match), convergence_message |
| `hm.death.AVC` | loglik_8dp, nfncts, iterations, condcode (ref match), convergence_message |
| `hz.deadp.KUL` | loglik_8dp (-3740.52), nfncts = 24, iterations = 9, condcode = 0.7194421 (ref 0.7194421), convergence_message |
| `hz.te123.OMC` | loglik_8dp (-311.597), nfncts = 71, iterations = 4, condcode = 3.204826 (ref 3.204763), convergence_message |

**Note:** `hz.te123.OMC` condcode on Windows is `3.204826` vs reference `3.204763`
(delta ≈ 6×10⁻⁵). Inside the V2 tolerance. Likely platform FP rounding in the
condition-number computation. Flagged for monitoring; not actionable today.

### V3 — Stepwise Selection Path: 3 / 3 passed, 1 skipped

| Case | Checks |
|------|--------|
| Reference path validation | `hm.death.AVC:reference_path:first_3_steps_correct` (Step1 delete MAL from CONSTANT, Step2 delete OP_AGE, Step3 add STATUS to EARLY) |
| `hm.death.AVC` | stepwise path — 9 steps match |
| `hm.deadp.VALVES` | stepwise path — 5 steps match |
| `hm.dead.n1surg` | **SKIPPED** — could not extract reference path (pre-existing `.lst` format quirk, not Windows-specific) |

### V7 — Concurrent Execution: reported PASS, actually all 4 skipped

| Case | Status | Reason |
|------|--------|--------|
| Determinism (`hz.death.AVC` run twice) | SKIP | missing `tests/inputs/hz.death.AVC.input` |
| Concurrent `hz.death.AVC` vs `hm.death.AVC` | SKIP | missing `hz.death.AVC.input` |
| Concurrent `hz.deadp.KUL` vs `hz.te123.OMC` | SKIP | missing `hz.deadp.KUL.input` |
| 3-way stress `hz.death.AVC`, `hm.death.AVC`, `hz.deadp.KUL` | SKIP | inputs missing |

**The suite reports `[PASS] V7: Concurrent Execution` because no case failed,
but no concurrent execution actually occurred on Windows.** V1/V2/V3 are able
to validate against the committed `tests/*.lst` reference outputs, but V7 needs
real `.input` files to launch the binary. Those files are pre-prepared from a
SAS run and are not committed to the repo.

**Windows concurrent execution is therefore not yet exercised.** See TODO
below. Linux CI covers this suite fully.

### V6 / V8 — skipped by flag

- **V6 Memory Safety (ASan)** — skipped via `--skip-memory`. GCC AddressSanitizer
  on MSYS2/UCRT64 is historically unreliable; Linux CI covers ASan cleanly.
- **V8 Acceptance Corpus** — skipped (requires `HAZARD_RUN_ACCEPTANCE=1`).

---

## Summary

| Layer | Status |
|-------|--------|
| Unit tests (150) | ALL PASS |
| Numerical regression (V1) | ALL PASS |
| Optimizer convergence (V2) | ALL PASS, one sub-test with minor FP drift noted |
| Stepwise selection (V3) | ALL PASS (1 pre-existing skip) |
| Concurrent execution (V7) | Not actually exercised — inputs missing |
| Memory safety (V6) | Not run on Windows (tooling) |
| Acceptance corpus (V8) | Not run (gated) |

**Conclusion:** The Windows build produces numerically faithful output relative
to the v4.1.0 reference on every case we were able to run. The only genuine
Windows coverage gap is concurrent execution (V7), pending input-file
regeneration.

---

## TODO — Regenerate Windows `.input` files after SAS promotion

**Blocker:** V7 concurrent tests need `tests/inputs/*.input` files, which are
produced by a SAS run (`tests/*.sas`) and are not committed. Once v4.4.3 hits
the SAS production server, re-run the SAS jobs on Windows to produce:

- `tests/inputs/hz.death.AVC.input`
- `tests/inputs/hm.death.AVC.input`
- `tests/inputs/hz.deadp.KUL.input`
- `tests/inputs/hz.te123.OMC.input`

Then re-run:

```bash
HAZARD_BIN=$PWD/src/hazard/hazard.exe \
  bash tests/run_all_tests.sh --integration-only --skip-memory
```

and append a follow-up report with the V7 results.

**Owner:** John Ehrlinger
**Trigger:** this version reaches SAS server
