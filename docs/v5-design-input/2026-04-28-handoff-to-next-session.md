# Handoff — v5.0 design done; next session picks up implementation

**Session of origin:** 2026-04-28 (brainstorming — Cowork session)
**Spec produced:** `docs/superpowers/specs/2026-04-28-hazard-v5-design.md` — **1400 lines**, 8 sections + 3 appendices + **2026-04-29 telemetry amendment**.
**Branch:** `docs/v5-design-spec` (off `main` at `0423229`). Three commits, not pushed:
- `a08f971` — original 1071-line design spec (8 sections + 3 appendices)
- `ac12a62` — this handoff doc
- `b077a7a` — **2026-04-29 amendment: telemetry subsystem** (329 lines added)

**Status:** awaiting user review of the assembled spec before invoking `writing-plans`. v4.4.5 has shipped and merged to main; the maintenance line is ready for v4.4.6 work to branch from post-merge `main`.

---

## TL;DR for next session

1. **Read** `docs/superpowers/specs/2026-04-28-hazard-v5-design.md` end-to-end. **Note: spec is now 1400 lines including the 2026-04-29 telemetry amendment** at the end. Read all the way through.
2. **Decide** whether the spec stands as-is, needs further amendments, or needs decisions re-litigated.
3. **If approved as-is** → invoke `superpowers:writing-plans` to produce a detailed v4.4.6 implementation plan (now ~370 LoC scope: V8 detection + structured errors + telemetry write side).
4. **If further amendments needed** → make them inline (append a dated "Amendment" section per spec §8.5), then proceed.
5. **In parallel:** start the admin-team conversation about telemetry deployment mechanism (see Coordination Dependencies §5 below). Doesn't gate v4.4.6 ship.

---

## What was decided across both sessions (compressed)

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
| **Telemetry (2026-04-29 amendment)** | **v4.4.6 binary writes JSONL telemetry log of every invocation; v4.4.7 ships Python summary script. Converts the v5.0-alpha "add V8 reader?" gate from manual survey → empirical query. PHI-safe schema by design. Works user-local without admin (`~/.hazard/events.log`); centralized `/var/log/hazard/` deployment is admin-coordinated upgrade.** |

Full decision log + alternatives considered: spec Appendix A. Full telemetry contract: spec §7.7–§7.8 + Amendment A.1–A.8.

---

## Release plan (the corrected numbering)

```
v4.4.5 (SHIPPED 2026-04-28) — Windows SAS reference capture; Linux+SAS gap noted; merged to main + tagged
v4.4.6 (NEXT)         — Hardened startup validation: V8 detect + always-on stderr + HAZARD_ERROR_<code> + non-zero exits
                        + TELEMETRY WRITE SIDE: JSONL events.log per invocation (input/output format, exit, duration, scale,
                          parent_job_id), fallback path resolution, opt-out via HAZARD_NO_TELEMETRY=1 / --no-telemetry,
                          self-rotation. Spec §7.7.
v4.4.7                — Binary↔SAS protocol completion: sentinel file + SAS macro updates; UNBLOCKS Linux+SAS reference capture
                        + TELEMETRY READ SIDE: scripts/hazard-telemetry-summary.py (Python 3 stdlib-only). Default output
                          evaluates v5.0 decision gates directly. Multi-source merge. Spec §7.8.
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

1. **v4.4.5 has shipped and merged to main.** v4.4.6 branches from current `main`. (Earlier handoff drafts said "wait for v4.4.5 to merge" — that's done.)

2. **`docs/v5-design-spec` branch is not pushed.** User pushes when ready. After push, the spec is reviewable on GitHub for any external reviewers.

3. **The spec itself sits on `docs/v5-design-spec`, NOT on `main`.** It will land on `main` via PR after user review. v4.4.6 work *can* read the spec directly from this branch by checking out `docs/v5-design-spec` or by merging it to `main` first — recommend the latter (PR + merge → spec on `main` → v4.4.6 branches off `main`).

4. **TemporalHazard's V9 framework is still being built.** Don't expect V9 to hard-fail at v5.0-beta — it's report-only there, hard-fail at v5.0-rc on whatever subset is implemented. Any v4.4.6 work that touches output formats should keep this in mind.

5. **Admin-team conversation runs in parallel with v4.4.6 development** (NEW per 2026-04-29 amendment). Three asks:
   - **Policy:** is application-level invocation telemetry on shared SAS servers OK? Existing precedent or institutional policy?
   - **Mechanism:** preferred — (a) flat file at `/var/log/hazard/` with `logrotate` config, (b) syslog forwarding with dedicated facility (e.g., `LOG_LOCAL0`), or (c) something institutional we plug into?
   - **Communication:** do users need to be notified before v4.4.6 ships, and through what channel?

   **Outcome shapes the deployment, not the binary feature.** Telemetry feature ships in v4.4.6 with user-local fallback (`~/.hazard/events.log`) regardless of admin response. Centralized deployment unlocked when admin team approves the chosen mechanism. The binary supports both flat-file and syslog mechanisms via compile-time flag; choice happens after the conversation, not before.

---

## What v4.4.6 contains (per spec §4.1 + 2026-04-29 amendment)

```
SCOPE
  Original (2026-04-28 design):
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

  Added 2026-04-29 amendment (telemetry write side):
  • events.log JSONL telemetry — one line per invocation:
      schema_version, ts, host, user, hazard_version,
      input_format (XPORT_V5/XPORT_V8/PARQUET/UNKNOWN),
      input_size_bytes, output_format, exit_code, duration_ms,
      n_obs, n_vars, parent_job_id (bootstrap aggregation).
  • PHI-safe BY DESIGN: never logs variable names, values, labels,
    formats, control file content, paths beyond $TMPDIR-temp shape.
  • Path resolution fallback chain:
      1. $HAZARD_TELEMETRY_LOG (explicit)
      2. /var/log/hazard/events.log (system, admin-deployed)
      3. $XDG_STATE_HOME/hazard/events.log (Linux user install)
      4. $HOME/.hazard/events.log (universal user fallback)
      5. (none — silently disabled with one HZD_LOG_WARN)
  • Opt-out: HAZARD_NO_TELEMETRY=1 env, --no-telemetry CLI flag.
    Unaffected by NOPRINT NOLOG (telemetry is the binary's own
    observability, not part of PROC option scope).
  • Self-rotation: ≥10 MB OR oldest event ≥30 days → rename .1,
    fresh .log, delete .2. Bounded ≤20 MB on disk per host.
  • Mechanism: flat file by default; syslog via compile-time flag
    HAZARD_TELEMETRY_BACKEND=SYSLOG once admin team chooses.
  • parent_job_id from SAS env var HAZARD_BOOTSTRAP_JOB_ID for
    bootstrap-aggregation in summary script (set by v4.4.7 macros).

GATE
  • V1 + V3 unchanged
  • New unit tests:
      - V8 banner detection
      - NOPRINT-bypass-for-stderr
      - exit code matrix
      - structured error markers
      - telemetry: each event field correct, fallback chain,
        opt-out, schema_version, rotation, PHI-scope assertions
        (negative tests: var names MUST NOT appear in log output)

EFFORT: ~300 LoC binary side + ~70 LoC tests (was 150+30 pre-amendment)
        Breakdown:
          - V8 detection + structured errors + exit codes: ~150 LoC
          - Telemetry write subsystem (src/common/hzd_telemetry.c): ~150 LoC
        Tests: ~30 LoC for original; ~40 LoC for telemetry.
```

The v4.4.6 message text for V8 rejection is fully specified in spec §7.3 — copy verbatim.

The telemetry contract is fully specified in spec §7.7 + Amendment A.3. JSON schema is fixed at `schema_version: 1`; adding fields requires a schema bump and privacy review per Amendment A.3.2.

---

## What v4.4.7 contains (next-after-v4.4.6, FYI)

Original (2026-04-28 design):
- Sentinel file protocol (binary writes `$TMPDIR/hzr.<jobid>.<jobix>.ok` on success)
- SAS macro updates: `hazard.sas`, `hazpred.sas`, `bootstrap.hazard.sas` check `&SYSRC` first, sentinel second, `.lst` parsing third — kills the retry-on-parse-failure loop that caused the 2026-04-28 Linux+SAS failure
- Bootstrap macros set `HAZARD_BOOTSTRAP_JOB_ID` env var before each `%sysexec` (feeds the v4.4.6 `parent_job_id` field)
- Unblocks Linux+SAS reference capture
- Documents the binary↔SAS protocol in `docs/BINARY-SAS-PROTOCOL.md`

Added 2026-04-29 amendment (telemetry read side):
- `scripts/hazard-telemetry-summary.py` — Python 3 stdlib-only utility
- Default invocation produces single-screen summary that directly evaluates
  the v5.0-alpha "add V8 reader?" gate AND the v5.0-beta "default Parquet?" gate
- Multi-source merge for combining server + user-local logs across hosts
- Time-windowed analysis (`--since`, `--until`)
- Machine-readable `--csv` / `--json` output for piping to R / Excel
- `--per-user`, `--per-day` detail breakdowns
- `--bootstrap-mode {include|exclude|only}` for handling bootstrap volume
- `--gates` mode for CI automation (exit 3 on threshold cross)
- Awk cookbook in script docstring as fallback for SAS-shop users
- Documented in `docs/BINARY-SAS-PROTOCOL.md` §telemetry; spec §7.8

EFFORT: ~200 LoC binary + ~100 LoC SAS macros + ~250 LoC Python + ~80 LoC Python tests
        (was just ~200 binary + ~100 SAS pre-amendment; +330 LoC from telemetry script)

Don't start v4.4.7 in the same session as v4.4.6. Each release deserves its own session for clean review.

**Strategic note on v4.4.6 → v4.4.7 spacing:** the gap between releases lets v4.4.6's `events.log` accumulate ~3-4 weeks of data, so the v4.4.7 summary tool ships against real data instead of synthetic test fixtures. v4.4.7 release narrative also strengthens — "now with usage analytics" instead of just "protocol fixes."

---

## Things NOT to do in the next session

1. **Don't start coding before the spec is approved.** Read it first — including the 2026-04-29 amendment at the end. Process matters here — the spec is the contract for everything that follows.
2. **Don't skip the `writing-plans` step.** Going from spec directly to implementation skips the per-file decomposition that makes PR review and rollback clean.
3. **Don't add fields to the telemetry schema without a `schema_version` bump and privacy review.** The PHI-safe-by-design property of `events.log` rests on the schema being stable. v4.4.6 ships `schema_version: 1`. Future additions are explicit, not silent.
4. **Don't push `docs/v5-design-spec` without asking John.** It's his call when external reviewers see this.
5. **Don't expand v4.4.6 scope beyond what's listed above.** v4.4.7 is a separate release for a reason. Sentinel file, SAS macro updates, and the Python summary script are all v4.4.7, not v4.4.6.
6. **Don't gate v4.4.6 ship on the admin-team conversation.** Feature works user-local without admin. Admin coordination unlocks centralized deployment; it does NOT block the binary feature itself.

---

## Things to revisit in the next session

- **The user-base survey questions** — spec §7.3 frames them, but exact wording for the V8/V5-truncation outreach hasn't been written. **Note (2026-04-29 amendment):** with telemetry shipping in v4.4.6, the survey is now *supplementary* to the empirical log data — useful for context ("what are users doing with V8 and why?") but no longer the primary data source for the v5.0-alpha decision gate.
- **Windows re-capture after v4.4.6.** v4.4.6 changes binary stderr behavior. Confirm the existing Windows+SAS reference (committed in v4.4.5 at `tests/corpus/hazard/reference/v4.4.5-windows-x64-sas/`) doesn't drift in unexpected ways once v4.4.6 lands. Spec doesn't explicitly call this out as a v4.4.6 gate — it should.
- **Admin-team conversation** about telemetry deployment mechanism (NEW per 2026-04-29 amendment). Three asks documented in Coordination Dependencies §5 above. Do this in parallel with v4.4.6 development; outcome shapes the centralized deployment, not the binary feature.
- **PHI review of telemetry schema** — spec §7.7 + Amendment A.3.2 lock the v4.4.6 field set as PHI-safe by design (no variable names, no values, no labels). Worth a 5-minute confirmatory review with whoever owns the institutional position on application telemetry — the spec is conservative but a sign-off is cheap insurance.

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
