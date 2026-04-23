# Changelog

See the [NEWS.md](NEWS.md) file for a detailed history of visible changes.

---

## Version 4.4.3 — 2026-04-23

Acceptance-harness-proven baseline. Binary behaviour is identical to v4.4.2; this release carries a parity certification, not new features.

- Acceptance harness `tests/validate_corpus.sh` ships the full captured reference corpus (15 hazard + 8 hazpred examples) and byte-diffs modern output against a host-selected reference (Darwin/arm64 → `v4.4.2-macos-arm64`; Linux/Windows → `v4.3.0`).
- Fix hazpred SIGSEGV on missing INHAZ (null `FILE*` deref in `src/hazpred/opnfils.c`; now exits cleanly with a readable error).
- Fix 80-byte `TMPDIR` buffer overflow in `opnfils.c` via `PATH_MAX` + `snprintf`.
- Document the cross-toolchain floating-point divergence between gcc (Linux + Windows/MinGW) and Apple clang on arm64 — not a code-history regression (`ROOT-CAUSE-ANALYSIS.md`, `tests/corpus/FINDINGS.md`).
- `make capture-kit` target packages the operator-facing capture scripts for regenerating the reference corpus on a CCF legacy host.

---

## Version 4.1
- Posted Hazard to a new distribution site: www.clevelandclinic.org/heartcenter/hazard
- Packaged source code using autoconf for easier compiling and installation

---

For copying conditions, see the [COPYING.md](COPYING.md) file.

Please send bug reports to [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org).
