# Handoff — v5.0 design done; next session picks up implementation

**Session of origin:** 2026-04-28 (brainstorming — Cowork session)
**Spec produced:** `docs/superpowers/specs/2026-04-28-hazard-v5-design.md` (1071 lines, 8 sections + 3 appendices)
**Branch:** `docs/v5-design-spec` (off `main` at `0423229`); commit `a08f971`. Not pushed.
**Status:** awaiting user review of the assembled spec before invoking `writing-plans`.

---

## TL;DR for next session

1. **Read** `docs/superpowers/specs/2026-04-28-hazard-v5-design.md` end-to-end.
2. **Decide** whether the spec stands as-is, needs amendments, or needs decisions re-litigated.
3. **If approved as-is** → invoke `superpowers:writing-plans` to produce a detailed v4.4.6 implementation plan.
4. **If amendments needed** → make them inline (append a dated "Amendment" section per spec §8.5), then proceed.

---

## What was decided this session (compressed)

The spec covers v5.0+v5.1 architectural intent. Key decisions:

| Topic | Decision |
|---|---|
| Scope | v5.0 = Parquet I/O + R/Python first-class. B (analysis-macro rewrite) deferred. PROC HAZARD call structure preserved as additive extension only. |
| Language | Modern C (C17) numerical core + C++17 at I/O boundaries (Arrow Parquet, pybind11 Python). NOT a full C++ rewrite. |
| Engines | `hazard` (C) and `TemporalHazard` (R) are independent peer engines sharing only Parquet as file format. CI cross-validation gate (V9). |
| XPORT compat | v5.0 reads V5 (legacy, frozen) + Parquet (new primary). V8 rejected with structured error pointing at Parquet. |
| Build system | Full CMake migration on `dev/v5.0-alpha` → `main`. `release/4.4` keeps autotools (untouched). |
| Phase 1 | Context struct uses `phases[N_PHASES_MAX]` array layout — anticipates Phase G (N-phase generalization in v5.1). |
| Phase G | v5.1, post-GA. Port TemporalHazard's generalized N-phase parametric form. |
| Sequencing | `Parquet read → 5.1 → 1 → 4 ‖ 2 → Parquet write + pybind11 → 3 → 6` then v5.1 G. |

Full decision log + alternatives considered: spec Appendix A.

---

## Release plan (the corrected numbering)

```
v4.4.5 (shipping NOW on feature/v4.4.5-prep) — Edwards Windows+SAS reference capture; Linux+SAS gap noted
v4.4.6 (NEXT)         — Hardened startup validation: V8 detect + always-on stderr + HAZARD_ERROR_<code> + non-zero exits
v4.4.7                — Binary↔SAS protocol completion: sentinel file + SAS macro updates; UNBLOCKS Linux+SAS reference capture
─── line break: maintenance line above, v5.0 line below ───
v5.0-alpha            — CMake + Parquet READ + Phase 5.1 + Phase 1 + Phase 4
v5.0-beta             — Phase 2 + Parquet WRITE + pybind11 frontend + V9 cross-engine gate (report-only)
v5.0-rc/GA            — Phase 3 + Phase 6 + autotools deprecated; V9 hard-fail
v5.1                  — Phase G (N-phase model); autotools removed entirely
```

Decision deadlines (spec Appendix B):
- **End of v4.4.6 + 30 days:** user-base survey results in (V5/V8/Parquet-ready classification)
- **End of v5.0-alpha:** add V8 reader yes/no based on survey (default: no)
- **End of v5.0-beta:** Parquet-by-default vs defer (default: defer)
- **End of v5.0-GA:** XPORT V5 deprecation timeline (set in v6.0 planning)
- **Pre-v5.0-rc:** pilot dataset selection

---

## Coordination dependencies (DO NOT IGNORE)

1. **`feature/v4.4.5-prep` must merge to `main` first.** Then v4.4.6 branches from the post-merge `main`. Don't branch v4.4.6 work from `feature/v4.4.5-prep` directly.

2. **`docs/v5-design-spec` branch is not pushed.** User pushes when ready. After push, the spec is reviewable on GitHub for any external reviewers.

3. **The spec itself sits on `docs/v5-design-spec`, NOT on `feature/v4.4.5-prep` or `main`.** It will land on `main` via PR after user review. v4.4.6 work *can* read the spec directly from this branch by checking out `docs/v5-design-spec` or by merging it to `main` first — recommend the latter (PR + merge → spec on `main` → v4.4.6 branches off `main`).

4. **TemporalHazard's V9 framework is still being built.** Don't expect V9 to hard-fail at v5.0-beta — it's report-only there, hard-fail at v5.0-rc on whatever subset is implemented. Any v4.4.6 work that touches output formats should keep this in mind.

---

## What v4.4.6 contains (per spec §4.1)

```
SCOPE
  • XPORT V8 detection + rejection at I/O boundary
    (validate header #2 banner: "LIBRARY" → V5 OK; "LIBV8" → reject)
  • Always-on stderr diagnostics — NOPRINT silences listing, NOT errors
  • Structured stdout/stderr error markers:
      HAZARD_ERROR_<code> <human-readable message>
  • Non-zero exit codes per existing hzfxit convention:
      16 = XPORT_V8_REJECTED
      17 = XPORT_INVALID_HEADER / INPUT_FORMAT_UNKNOWN
      (existing 12=OPTIMIZER_DIVERGED, 14=MEMORY_EXHAUSTED, 20=INTERNAL_ERROR retained)
  • User-base survey kicked off post-release (out-of-code action)
GATE
  • V1 + V3 unchanged
  • New unit tests: V8 banner detection, NOPRINT-bypass-for-stderr,
    exit code matrix, structured error markers
EFFORT: ~150 LoC binary side + 30 LoC tests
```

The v4.4.6 message text for V8 rejection is fully specified in spec §7.3 — copy verbatim.

---

## What v4.4.7 contains (next-after-v4.4.6, FYI)

- Sentinel file protocol (binary writes `$TMPDIR/hzr.<jobid>.<jobix>.ok` on success)
- SAS macro updates: `hazard.sas`, `hazpred.sas`, `bootstrap.hazard.sas` check `&SYSRC` first, sentinel second, `.lst` parsing third — kills the retry-on-parse-failure loop that caused the 2026-04-28 Linux+SAS failure
- Unblocks Linux+SAS reference capture
- Documents the binary↔SAS protocol in `docs/BINARY-SAS-PROTOCOL.md`

Don't start v4.4.7 in the same session as v4.4.6. Each release deserves its own session for clean review.

---

## Things NOT to do in the next session

1. **Don't start coding before the spec is approved.** Read it first. Process matters here — the spec is the contract for everything that follows.
2. **Don't skip the `writing-plans` step.** Going from spec directly to implementation skips the per-file decomposition that makes PR review and rollback clean.
3. **Don't entangle v4.4.6 work with v4.4.5 prep.** v4.4.5 ships first; v4.4.6 starts after.
4. **Don't push `docs/v5-design-spec` without asking John.** It's his call when external reviewers see this.
5. **Don't expand v4.4.6 scope.** v4.4.7 is a separate release for a reason. Sentinel file and SAS macro updates are v4.4.7, not v4.4.6.

---

## Things to revisit in the next session

- **The user-base survey questions** — spec §7.3 frames them, but exact wording for the V8/V5-truncation outreach hasn't been written. Probably worth drafting in a v4.4.6 session before v4.4.6 ships.
- **Edwards re-capture after v4.4.6.** v4.4.6 changes binary stderr behavior. Confirm the existing Windows+SAS reference (committed in v4.4.5 at `tests/corpus/hazard/reference/v4.4.5-windows-x64-sas/`) doesn't drift in unexpected ways once v4.4.6 lands. Spec doesn't explicitly call this out as a v4.4.6 gate — it should.

---

## Quick-reference paths

| What | Where |
|---|---|
| The spec | `docs/superpowers/specs/2026-04-28-hazard-v5-design.md` (this branch: `docs/v5-design-spec`) |
| The postmortem driving §7 | `docs/v5-design-input/2026-04-28-silent-failure-postmortem.md` |
| Existing modernization guide (lives forward as the implementation tracker) | `docs/Claude_MODERNIZATION_GUIDE.md` (on `main`) |
| Obsidian project note | `~/Documents/ObsidianVault/Projects/HAZARD.md` |
| Obsidian SAS migration context | `~/Documents/ObsidianVault/Projects/SAS to R Migration.md` |
| Cowork session log for this session | `~/Documents/ObsidianVault/Claude/Sessions/2026-04-28 *.md` (after `/close`) |

---

*This handoff doc is itself part of the `docs/v5-design-spec` branch and travels with the spec. It can be deleted once v4.4.6 ships and the next-session purpose is fulfilled.*
