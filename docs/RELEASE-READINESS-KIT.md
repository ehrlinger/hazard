# HAZARD v4.4 Release Readiness — pointer doc

**Status:** historical — superseded 2026-04-23 by the authoritative docs below.

---

The v4.4.3 release-readiness artefacts now live in the canonical files, not in this doc. Use these as the single source of truth:

| Topic | Authoritative doc |
|---|---|
| v4.3→v4.4 divergence root cause (closed) | [`ROOT-CAUSE-ANALYSIS.md`](../ROOT-CAUSE-ANALYSIS.md) v3.x |
| Corpus authenticity + findings | [`tests/corpus/FINDINGS.md`](../tests/corpus/FINDINGS.md) |
| Capture re-run procedure | [`scripts/CAPTURE_INSTRUCTIONS.md`](../scripts/CAPTURE_INSTRUCTIONS.md) |
| Corpus layout + normalisation | [`tests/corpus/README.md`](../tests/corpus/README.md) |
| Validation harness | [`tests/validate_corpus.sh`](../tests/validate_corpus.sh) |
| Report generator | [`scripts/validate_report.py`](../scripts/validate_report.py) |
| Two-bucket reference model | `FINDINGS.md` §2a, `ROOT-CAUSE-ANALYSIS.md` §4 |

---

## Quick reference for release validation

### Run the acceptance harness locally

```bash
./tests/validate_corpus.sh
```

Auto-selects the reference corpus from the host toolchain family:
- Darwin / arm64 → `tests/corpus/hazard/reference/v4.4.4-macos-arm64/`
- Linux / Windows (MinGW) → `tests/corpus/hazard/reference/v4.3.0/` (gcc-family bucket; numerical parity, cosmetic banner/org diffs pending a v4.4.x Linux recapture)

### Re-capture from CCF

See `scripts/CAPTURE_INSTRUCTIONS.md` for the operator-facing procedure (kit built via `make capture-kit`). Most recent capture archive: `hazard-capture-results-2026-04-23.tar.gz`, binary SHAs recorded in `hazard-capture-provenance.txt`.

### Cross-toolchain evidence (investigation workflows, now retired)

Two one-shot GitHub Actions workflows (`linux-ll-check.yml`, `windows-ll-check.yml`) were used to confirm the v4.3→v4.4 divergence is 100% cross-toolchain FP, 0% code history. They were dispatched, data collected, and deleted — see `ROOT-CAUSE-ANALYSIS.md` §2.2 for the resulting four-cell table.

---

## Superseded content

This document previously carried a five-phase validation-campaign plan, a "commit 557f3ef as root cause" write-up, and machine-local paths (`~/Documents/GitHub/hazard/...`, `~/Documents/ObsidianVault/...`).  All of that is wrong or duplicative now:

- The 557f3ef-as-root-cause hypothesis was disproven on 2026-04-23 — see `ROOT-CAUSE-ANALYSIS.md` §2.1. Accurate story lives there.
- The five-phase plan is replaced by the auto-selecting harness above plus the kit-based capture procedure.
- Machine-local paths removed.

If you arrived here from an older link, follow the table at the top.
