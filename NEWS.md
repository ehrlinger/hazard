# Changelog

See the [NEWS.md](NEWS.md) file for a detailed history of visible changes.

---

## v4.4.6 — Hardened startup validation + invocation telemetry (2026-04-29)

### New behavior

* **XPORT V8 input is now rejected** with a structured stderr error
  pointing users at Parquet (in v5.0+) or V5 fallback. Previously, V8
  files were processed with silently-truncated SAS-generated short
  variable names, producing "variable not found" errors downstream.
  Exit code: 16 (`HAZARD_EXIT_XPORT_V8_REJECTED`).
* **Malformed XPORT files are now rejected** at the I/O boundary with
  a clear error. Exit code: 17 (`HAZARD_EXIT_XPORT_INVALID`).
* **Structured stderr error markers** of the form
  `HAZARD_ERROR_<CODE> <message>` are now emitted on every fatal
  error path. SAS macros and external wrappers can detect failures
  by parsing stderr instead of guessing from `.lst` content.
* **Differentiated non-zero exit codes** replace the universal `exit(16)`
  pattern. New codes: 16 (V8 rejected), 17 (XPORT malformed), 20
  (internal error). Existing codes (12, 14) preserved.
* **Always-on stderr diagnostics**. `NOPRINT` PROC option silences the
  listing only; errors and `HAZARD_ERROR_*` markers always reach stderr.

### New: invocation telemetry

* **Append-only JSONL log** of every hazard invocation. Captures (per
  schema v1): timestamp, host, user, version, input/output format,
  file size, exit code, duration, n_obs, n_vars, parent_job_id (for
  bootstrap aggregation, populated by SAS macros in v4.4.7+).
* **PHI-safe by design.** Schema explicitly excludes variable names,
  values, labels, formats, control file content. Adding fields requires
  a `schema_version` bump and privacy review.
* **Path resolution fallback chain**:
    1. `$HAZARD_TELEMETRY_LOG`
    2. `/var/log/hazard/events.log` (if dir exists + group-writable)
    3. `$XDG_STATE_HOME/hazard/events.log` (Linux)
    4. `$HOME/.hazard/events.log` (universal Unix fallback)
    5. `%LOCALAPPDATA%\hazard\events.log` (Windows)
    6. (none — silently disabled with one HZD_LOG_WARN)
* **Opt-out**: `HAZARD_NO_TELEMETRY=1` env var or `--no-telemetry` CLI flag.
* **Self-rotation**: 10 MB threshold; one rotation kept (`events.log.1`).
* **No SAS-side changes in this release.** v4.4.7 will add the SAS-side
  protocol (sentinel file + bootstrap_job_id env var setting + macro
  updates) plus a Python summary script (`hazard-telemetry-summary.py`).

### Behavior changes affecting existing users

* Exit codes: previously most failures exited 16 regardless. Now they
  reflect the failure class. Wrappers that branched on "any non-zero"
  continue to work; wrappers that branched specifically on `=16`
  may need updates.
* Stderr volume: failures now produce one additional line per error
  (`HAZARD_ERROR_<code> <msg>`). Successful runs are unaffected.

### No functional / numerical changes

V1 + V3 bit-exact regression tested against v4.4.5 reference outputs.
No changes to the numerical core. PROC HAZARD call structure unchanged.

### Coordination

This release is the first half of the binary↔SAS protocol hardening
work. v4.4.7 closes the loop with the SAS macro updates + sentinel file +
Python summary script.

Full v5.0 design context: `docs/superpowers/specs/2026-04-28-hazard-v5-design.md`.

---

## Version 4.4.5 — 2026-04-28

Validation-tooling release. Adds the cross-platform SAS-driven validation harness, ships the Windows + SAS reference corpus that v4.4.4 enabled, and lays groundwork for the v5 startup-XPORT-validation work that will close the remaining Linux + SAS gap.

**No functional code changes.** The hazard / hazpred binary algorithms and `.lst` outputs are unchanged from v4.4.4 — only the embedded `VERSION` string in the binary differs after a rebuild from this tag.

### What shipped

- **`tests/validate_examples_sas.sh`** — sister harness to `tests/validate_corpus.sh`. Where `validate_corpus.sh` exercises the bare hazard / hazpred binaries with captured `.input` stdin and byte-diffs the bare-binary stdout, this new harness compares **SAS-wrapped** `.lst` outputs (page headers, surrounding DATA-step output, "Procedure HAZARD completed successfully" footers) against a SAS-driven reference bucket. Auto-selects `REFERENCE` per host: `Linux → v4.4.5-linux-x64-sas`, `Windows / Cygwin / MSYS → v4.4.5-windows-x64-sas`, `Darwin` errors out (no SAS-on-macOS bucket exists since SAS Foundation does not ship for macOS).

- **`tests/corpus_normalize.sh`** — gains a third sed rule that scrubs the SAS page-header timestamp pattern (`HH:MM Weekday, Month DD, YYYY`) emitted by SAS-wrapped `.lst` outputs. Absorbs flanking whitespace too so weekday-name length variance ("Monday" vs "Tuesday") doesn't leak into the byte-diff. No-op on bare-binary captures.

- **`tests/run-hazard-capture.sh`** — fixes for SAS-on-Linux invocation: passes `-set MACROS "$macros_dir"` (the SAS-internal `MACROS` namespace, separate from the OS env var), adds the HAZAPPS bin dir to `SASAUTOS` so SAS finds the entry-point `hazard.sas` / `hazpred.sas` regardless of whether the install puts them in `bin/` (production convention) or `macros/` (manual-overlay convention). Together these unblock SAS-driven captures against the production `/opt/hazard/` install on the CCF Linux box.

- **Windows + SAS reference corpus**: `tests/corpus/hazard/reference/v4.4.5-windows-x64-sas/` — 22 driver `.lst` outputs captured on `windows-sas-host` (Windows SAS host, Windows Server 2019, SAS Foundation 9.4) against the v4.4.4 patched macros. Replaces the prior loose `examples/*.lst` placement from v4.4.4 with a corpus-bucketed structure that `validate_examples_sas.sh` consumes directly.

### Known gap

Linux + SAS reference corpus is **not** shipped in v4.4.5. Investigation on `lri-sas-p-02.lerner.ccf.org` (CCF Linux SAS host) found that bootstrap-mode example drivers (`bs.*`, `lg.*`) silently fail at the binary↔SAS protocol layer regardless of which hazard binary is invoked (fresh v4.4.5 build *and* `/opt/hazard/bin/hazard` production binary fail identically). The binary exits `rc=0` with 0 bytes of output in NOPRINT mode; the SAS bootstrap macro then loops on the resulting brief stand-in output for 10,000+ iterations before being killed. Full root-cause and v5 design implications: [`docs/v5-design-input/2026-04-28-silent-failure-postmortem.md`](docs/v5-design-input/2026-04-28-silent-failure-postmortem.md).

The closing fix is the v5 roadmap item "detect / validate XPORT input format at hazard.exe startup" — combined with stderr-always-on diagnostics and exit-code propagation through `%sysexec`, the binary will be able to refuse-to-proceed cleanly on the inputs that today silently produce empty output. Until then, Linux + SAS validation is out-of-band; direct-binary validation continues via `tests/validate_corpus.sh` against the `v4.3.0` reference (FINDINGS-documented cosmetic banner / org-string FAILs are expected and unrelated to this gap).

---

## Version 4.4.4 — 2026-04-27

Cosmetic-only maintenance release. SAS macro hygiene fix; binary behaviour and `.lst` output content are byte-identical to v4.4.3 modulo SAS-side timestamp drift.

- Replace `%sysget(OSTYPE)` OS-detection probe in `src/scripts/hazard.sas` and `src/scripts/hazpred.sas` with SAS automatic macro variables `&SYSSCP` / `&SYSSCPL`. Silences the benign `WARNING: The argument to macro function %SYSGET is not defined as a system variable` that fired on every Windows invocation, flipping 13 of 22 example drivers from SAS exit=1 to exit=0 with no functional change.
- Add explicit macOS branch via `&SYSSCPL` containing `Darwin`; previously macOS was silently classified as Windows when the Linux `OSTYPE` env var was unset.
- Commit Windows v4.4.3 reference baseline (18 of 22 driver `.lst` outputs; 4 unrunnable on the validation host due to absent CHSS/TGA source datasets). Captured 2026-04-27 on `windows-sas-host` (Windows Server 2019, SAS 9.4). See `docs/validation-reports/WINDOWS-VALIDATION-2026-04-27.md`.

---

## Version 4.4.3 — 2026-04-23

Acceptance-harness-proven baseline. Binary behaviour is identical to v4.4.2; this release carries a parity certification, not new features.

- Acceptance harness `tests/validate_corpus.sh` ships the full captured reference corpus (15 hazard + 8 hazpred examples) and byte-diffs modern output against a host-selected reference (Darwin/arm64 → `v4.4.2-macos-arm64`; Linux/Windows → `v4.3.0`).
- Fix hazpred SIGSEGV on missing INHAZ (null `FILE*` deref in `src/hazpred/opnfils.c`; now exits cleanly with a readable error).
- Fix 80-byte `TMPDIR` buffer overflow in `opnfils.c` via `PATH_MAX` + `snprintf`.
- Document the cross-toolchain floating-point divergence between gcc (Linux + Windows/MinGW) and Apple clang on arm64 — not a code-history regression (`docs/validation-reports/ROOT-CAUSE-ANALYSIS.md`, `tests/corpus/FINDINGS.md`).
- `make capture-kit` target packages the operator-facing capture scripts for regenerating the reference corpus on a CCF legacy host.

---

## Version 4.1
- Posted Hazard to a new distribution site: www.clevelandclinic.org/heartcenter/hazard
- Packaged source code using autoconf for easier compiling and installation

---

For copying conditions, see the [COPYING.md](COPYING.md) file.

Please send bug reports to [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org).
