# Changelog

See the [NEWS.md](NEWS.md) file for a detailed history of visible changes.

---

## Version 4.4.4 — 2026-04-27

Cosmetic-only maintenance release. SAS macro hygiene fix; binary behaviour and `.lst` output content are byte-identical to v4.4.3 modulo SAS-side timestamp drift.

- Replace `%sysget(OSTYPE)` OS-detection probe in `src/scripts/hazard.sas` and `src/scripts/hazpred.sas` with SAS automatic macro variables `&SYSSCP` / `&SYSSCPL`. Silences the benign `WARNING: The argument to macro function %SYSGET is not defined as a system variable` that fired on every Windows invocation, flipping 13 of 22 example drivers from SAS exit=1 to exit=0 with no functional change.
- Add explicit macOS branch via `&SYSSCPL` containing `Darwin`; previously macOS was silently classified as Windows when the Linux `OSTYPE` env var was unset.
- Commit Windows v4.4.3 reference baseline (18 of 22 driver `.lst` outputs; 4 unrunnable on the validation host due to absent CHSS/TGA source datasets). Captured 2026-04-27 on `AWOR-PDSASAPP03` (Windows Server 2019, SAS 9.4). See `docs/validation-reports/WINDOWS-VALIDATION-2026-04-27.md`.

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
