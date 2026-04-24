# Capturing the HAZARD Acceptance-Test Corpus

## What you're helping with

The HAZARD C package (the `hazard` / `hazpred` binaries that SAS invokes as `PROC HAZARD` / `PROC HAZPRED`) is being modernised.  Before that work can proceed safely, we need a **reference corpus**: the exact inputs and outputs from a known-good legacy install running each example `.sas` file.  Once we have that, every future change to the code can be validated against it automatically — the new binary's output must match byte-for-byte.

The capture takes maybe 30 minutes of attended work plus however long your SAS runs take.  One-time job.  You ship back a tarball; the hazard developers take it from there.

---

## Prerequisites

- A Unix host with **SAS + a legacy HAZARD install** (what you'd normally run `PROC HAZARD` against).
- **`bash`** on your PATH.  Every command in this doc and the wrapper itself assumes bash.  If your login shell is csh/tcsh/ksh, run `bash` first.
- **`$HAZAPPS`** exported by your SAS environment, pointing at the HAZARD install dir.  Most sites do this automatically.  If unset, see "If `$HAZAPPS` isn't set" near the bottom.
- **`$HZEXAMPLES`** exported, pointing at the dir with the `.sas` example files.  Same story — standard on most installs.  If unset, the kit tarball includes a bundled `sas/` fallback.

---

## The workflow

You'll build a `hazard-shadow-bin/` directory that contains the wrapper + two symlinks masquerading as the real `hazard` / `hazpred` binaries.  Putting that directory ahead of the real binaries on `$PATH` is what lets us intercept SAS's calls.  Everything you need lives in that one directory.

### 1. Unpack the kit into `~/hazard-shadow-bin/`

```bash
mkdir -p ~/hazard-shadow-bin
cd ~/hazard-shadow-bin
tar -xzf /path/to/hazard-capture-kit.tar.gz --strip-components=1
ls
# Expect:
#   capture-legacy.sh       ← the wrapper
#   capture-order.txt       ← dependency-ordered .sas list
#   capture.env.example     ← optional site-config template
#   README.md               ← this file
#   sas/                    ← bundled .sas examples (fallback if $HZEXAMPLES unset)
#   macros/                 ← bootstrap.hazard.sas, hazplot.sas, etc.
#                             ← used when the site's system MACROS dir is missing them
```

### 2. Make the wrapper executable and create the symlinks

Without `chmod +x` and real symlinks, SAS silently falls back to the real binary and your capture directory stays empty — so verify the listing at the end of this step before moving on.  **Create both `hazard` and `hazard.exe` symlinks** (and the `hazpred` pair) — sites where the PROC HAZARD macro invokes `%sysget(HAZAPPS)/hazard.exe` explicitly (CCF does this) need the `.exe` form to intercept.

```bash
cd ~/hazard-shadow-bin
chmod +x capture-legacy.sh
ln -sf "$PWD/capture-legacy.sh" hazard
ln -sf "$PWD/capture-legacy.sh" hazard.exe
ln -sf "$PWD/capture-legacy.sh" hazpred
ln -sf "$PWD/capture-legacy.sh" hazpred.exe

# Verify — five entries, wrapper executable, four symlinks
ls -la capture-legacy.sh hazard hazard.exe hazpred hazpred.exe
# Expect:
#   -rwxr-xr-x  ... capture-legacy.sh
#   lrwxr-xr-x  ... hazard       -> .../capture-legacy.sh
#   lrwxr-xr-x  ... hazard.exe   -> .../capture-legacy.sh
#   lrwxr-xr-x  ... hazpred      -> .../capture-legacy.sh
#   lrwxr-xr-x  ... hazpred.exe  -> .../capture-legacy.sh
```

If any of those lack the `x` permission or the `l` prefix, fix that before continuing.

### 3. Put the shadow dir first on PATH

```bash
export PATH=~/hazard-shadow-bin:$PATH

# Both should now resolve to the wrapper (NOT the real binary)
which hazard
which hazpred
# Expect: /home/you/hazard-shadow-bin/hazard
```

If either still resolves to the real install's `bin/hazard`, PATH isn't right — fix that first.

### 3a. Configure `capture.env` so the wrapper knows where the real binary lives

**This step is mandatory when step 5 uses `-set HAZAPPS ~/hazard-shadow-bin`** (which most sites need — see the rationale in step 5).  Without it, the wrapper's fallback to find the real binary walks `$HAZAPPS/hazard.exe` → `~/hazard-shadow-bin/hazard.exe` → `capture-legacy.sh` → recursion → fork bomb.

Tell the wrapper explicitly where the REAL binaries live:

```bash
cat > ~/hazard-shadow-bin/capture.env <<'EOF'
HAZARD_REAL=/absolute/path/to/real/hazard.exe
HAZPRED_REAL=/absolute/path/to/real/hazpred.exe
EOF

# Verify
cat ~/hazard-shadow-bin/capture.env
ls -la "$(grep '^HAZARD_REAL=' ~/hazard-shadow-bin/capture.env | cut -d= -f2)"
ls -la "$(grep '^HAZPRED_REAL=' ~/hazard-shadow-bin/capture.env | cut -d= -f2)"
```

At CCF (`lri-sas-p-02`) the paths are:
```
HAZARD_REAL=/programs/apps/sas/hazard/hazard.exe
HAZPRED_REAL=/programs/apps/sas/hazard/hazpred.exe
```

The wrapper auto-sources `capture.env` on every invocation, so this persists across SAS runs without you re-exporting anything.

### 4. Record the real binary's provenance

One pre-flight line, so the tarball you ship back is self-describing. The SHA-256 lets the hazard developers confirm the v4.3.0 binary hasn't changed between capture runs.

```bash
{
    echo "capture-date: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo "host: $(uname -a)"
    for bin in "${HAZARD_REAL:-$HAZAPPS/hazard}" "${HAZARD_REAL:-$HAZAPPS/hazard.exe}" \
               "${HAZPRED_REAL:-$HAZAPPS/hazpred}" "${HAZPRED_REAL:-$HAZAPPS/hazpred.exe}"; do
        [ -f "$bin" ] || continue
        printf 'binary: %s  sha256: %s\n' "$bin" "$(sha256sum "$bin" 2>/dev/null | awk '{print $1}')"
    done
} > ~/hazard-capture-provenance.txt
cat ~/hazard-capture-provenance.txt
```

The file lands in `~/` and will be picked up by the tarball at step 7.

### 5. Smoke-test with one `.sas` file

Before running all 23+ files, run one through SAS and confirm the wrapper fired.  `hm.death.AVC` is a good choice — small, self-contained, no dependencies on earlier runs.

> **Why `-set HAZAPPS`:** the PROC HAZARD macro builds `%let hazpgm=%sysget(HAZAPPS)/hazard.exe` at run-time (see `hazard.sas`).  If `HAZAPPS` resolves to the real install, the macro invokes the real binary directly by absolute path — bypassing your shadow-bin on PATH.  Overriding `HAZAPPS` to your shadow-bin redirects that absolute path to the wrapper.  **The kit ships `hazard.sas` / `hazpred.sas` / `hazdolst.sas` inside the shadow-bin so PROC HAZARD registers under the overridden `HAZAPPS` — without those files, you'd see `ERROR 180-322: Statement is not valid` at PROC HAZARD.**

> **Why `-sasuser ~/sasuser-priv`:** if any other SAS session you own has the shared `$HOME/sasuser*` profile open, your run errors with `Resource is write-locked by another user`.  A private per-run `sasuser` dir side-steps it.  Cheap insurance.

> **Important on site SAS wrappers:** many sites wrap the `sas` command in a startup script that resets `HAZAPPS` (and strips `TMPDIR`) for "consistency" regardless of your shell environment.  Shell `export HAZAPPS=...` gets blown away before SAS's macro engine sees it.  Passing overrides via `-set` runs AFTER any site wrapper's setup and survives into `%sysget()`, `%sysexec` subprocesses, and the spawned binary's environment.  Do NOT `-set MACROS` — use the site's MACROS library so `bootstrap.hazard` (and other site helpers) resolve normally.

```bash
mkdir -p ~/sasuser-priv

SAS_DIR="${HZEXAMPLES:-$HOME/hazard-shadow-bin/sas}"
sas -nodms -sasuser ~/sasuser-priv \
    -set HAZAPPS ~/hazard-shadow-bin \
    -set TMPDIR /saswork \
    -log /tmp/smoke.log -print /tmp/smoke.lst \
    "$SAS_DIR/hm.death.AVC.sas"

# Confirm the wrapper captured tuples
ls ~/hazard-capture/hazard/
# Expect a group of files with a shared uid prefix:
#   1745500432.12345.4711.stdin                     ← piped-in PROC options
#   1745500432.12345.4711.lst                       ← real binary's output
#   1745500432.12345.4711.hzr.J<job>.X<ix>.dta      ← XPORT dataset SAS wrote
#   1745500432.12345.4711.hzr.J<job>.X<ix>.sas      ← stdin file SAS wrote
#   1745500432.12345.4711.meta                      ← metadata (real_exit, paths)
# If PROC HAZARD used OUTHAZ=, also expect a .haz output file in the tuple.
```

If `~/hazard-capture/hazard/` is empty or missing, the wrapper didn't fire — jump to "If the wrapper isn't firing" before running the full batch.

> **How to tell whether your site's SAS resets env vars:** run the probe at the bottom of this doc ("Diagnosing env-var inheritance") to see what `%sysget(HAZAPPS)` returns inside SAS.  If it's different from your shell's `$HAZAPPS`, a wrapper is intervening and you need the `-set` flag.  At all-Intel Cleveland Clinic HAZARD installs (where this was discovered), `-set HAZAPPS` is required.

### 6. Run the full corpus in dependency order

Some `.sas` scripts write estimate datasets under `$HZEXAMPLES/sasest/` that later scripts read.  Running them out of order will produce wrong output for the dependents.  `capture-order.txt` in the shadow-bin dir lists the correct sequence; the loop below reads it and falls back to alphabetical for anything not listed.

```bash
# Fresh run: wipe any previous capture output, dump SAS logs to a
# dedicated dir so they survive reboots and end up in the tarball.
mkdir -p ~/hz-logs
rm -rf ~/hazard-capture

cd ~/hazard-shadow-bin
ORDER_FILE=$PWD/capture-order.txt
SAS_DIR="${HZEXAMPLES:-$PWD/sas}"

{
    # Listed files first, in the given order
    grep -vE '^\s*(#|$)' "$ORDER_FILE" | while read -r name; do
        echo "$SAS_DIR/${name%.sas}.sas"
    done
    # Catch-all: anything on disk but not in the manifest, alphabetical
    listed=$(grep -vE '^\s*(#|$)' "$ORDER_FILE" | sed 's/\.sas$//' | sort -u)
    for sas in "$SAS_DIR"/*.sas; do
        b=$(basename "$sas" .sas)
        grep -qxF "$b" <<< "$listed" || echo "$sas"
    done
} | while read -r sas; do
    [ -f "$sas" ] || continue
    name=$(basename "$sas" .sas)
    echo "--- running $name ---"
    # -sasuser ~/sasuser-priv     : avoid SASUSER profile lock if another
    #                               session holds $HOME/sasuser*
    # -set HAZAPPS ~/hazard-shadow-bin :
    #   PROC HAZARD registers from hazard.sas in the shadow-bin, and
    #   %let hazpgm=%sysget(HAZAPPS)/hazard.exe resolves to the wrapper.
    #   See step 3a — capture.env MUST be set or this recurses.
    # -set TMPDIR /saswork        : short TMPDIR (keeps the legacy v4.3.0
    #                               binary's 80-byte buffer happy).
    # Do NOT -set MACROS — use the site's installed macros library so
    # bootstrap.hazard and other site helpers resolve normally.
    sas -nodms -sasuser ~/sasuser-priv \
        -set HAZAPPS ~/hazard-shadow-bin \
        -set TMPDIR /saswork \
        -log ~/hz-logs/$name.log -print ~/hz-logs/$name.lst "$sas"
    echo "  exit=$? captures so far: $(ls ~/hazard-capture/hazard/ 2>/dev/null | wc -l)"
done
```

Some `.sas` files may error out — that's fine, we want whatever happens.  The wrapper doesn't care about exit status.

> **Optional — redact host/pwd from captured metadata.**  By default the wrapper writes `host=$(uname -a)` and `pwd=$(pwd)` into each `.meta` file, so `lri-sas-p-02.lerner.ccf.org` and your full `$HOME` path end up in the corpus that ships back.  If that's not appropriate for your site, export `HAZARD_CAPTURE_REDACT=1` before the SAS loop — the wrapper will emit `uname -s -m -r` and `pwd=<redacted>` instead, keeping only OS/kernel + binary-hash provenance.

### 7. Tarball and ship

```bash
# Bundle capture artefacts + SAS logs + the provenance file (step 4) so
# the developers can cross-reference any exit=16 / non-zero runs against
# their log output and confirm the binary SHA against the reference set.
TARBALL=~/hazard-capture-results-$(date -u +%Y-%m-%d).tar.gz
tar -czf "$TARBALL" -C ~ \
    hazard-capture hz-logs hazard-capture-provenance.txt
ls -lh "$TARBALL"
# Typical size: 5–15 MB (XPORT .dta files are most of the bulk)
```

Ship the date-stamped tarball back to whoever requested the capture.  The date suffix prevents collisions when re-running for a second confirmation pass; the provenance file embeds the binary SHA so the developers can confirm the real binary hasn't shifted between captures.

---

## Verifying what was captured

After a full run:

```bash
# Count invocation groups per kind
ls ~/hazard-capture/hazard/ 2>/dev/null  | sed -E 's/^([0-9]+\.[0-9]+\.[0-9]+)\..*/\1/' | sort -u | wc -l
ls ~/hazard-capture/hazpred/ 2>/dev/null | sed -E 's/^([0-9]+\.[0-9]+\.[0-9]+)\..*/\1/' | sort -u | wc -l
```

Expect roughly 23+ groups under `hazard/` and some number under `hazpred/`.

Each group is a tuple with the same uid prefix:

| File | What it is |
|---|---|
| `<uid>.stdin` | The PROC options text SAS piped to the binary |
| `<uid>.lst` | The binary's stdout (listing) |
| `<uid>.hzr.<study>.<dset>.dta` | The XPORT dataset the binary read |
| `<uid>.hzr.<study>.<dset>.haz` | (hazpred only) intermediate file from a prior hazard run |
| `<uid>.meta` | Metadata: argv, real-binary exit code, timestamp |

---

## If the wrapper isn't firing

Symptom: `~/hazard-capture/` is empty or missing after running SAS, but SAS produced `.lst` / `.log` files as usual.  Diagnosis:

- `ls -la ~/hazard-shadow-bin/hazard hazard.exe hazpred hazpred.exe` — are all four executable symlinks?  Sites where PROC HAZARD invokes `%sysget(HAZAPPS)/hazard.exe` directly (CCF does this) need the `.exe`-named symlinks too; without them the shell resolves the `.exe` path through the real install.  Redo step 2 if any are missing.
- `which hazard` — is it pointing at the shadow-bin or at the real binary?  If the real binary, step 3 (PATH export) didn't stick in your current shell.  Redo it.
- Is SAS hardcoding the binary path in a site config file (bypassing `$PATH`)?  Common spots: `autoexec.sas`, `sasv9.cfg`, HAZARD's own setup scripts.  Search for `HAZARD_BIN` / direct paths / `x` command invocations.  If hardcoded, either change the config to honour `$PATH`, or point the hardcoded path at `~/hazard-shadow-bin/hazard`.
- `~/hazard-capture/` can be preexisting and empty from a prior failed attempt — that's fine, it just means nothing got written THIS run.

## If `ERROR 180-322: Statement is not valid` appears at `PROC HAZARD`

PROC HAZARD / PROC HAZPRED failed to register under the overridden `-set HAZAPPS ~/hazard-shadow-bin`.  The kit ships `hazard.sas` / `hazpred.sas` / `hazdolst.sas` inside the shadow-bin for exactly this — check they're present:

```bash
ls ~/hazard-shadow-bin/{hazard,hazpred,hazdolst}.sas
```

If any are missing (old kit predating this fix), copy from the real install:

```bash
cp "${HAZAPPS:-/programs/apps/sas/hazard}"/{hazard,hazpred,hazdolst}.sas ~/hazard-shadow-bin/
```

## If the SAS call hangs (or bash reports "shell level (1000) too high")

Infinite-recursion: the wrapper re-exec'd itself.  Cause: `-set HAZAPPS ~/hazard-shadow-bin` redirected the wrapper's real-binary lookup (`$HAZAPPS/hazard.exe`) back at itself.  Fix: check `~/hazard-shadow-bin/capture.env` exists and contains `HAZARD_REAL=/absolute/path/to/real/hazard.exe` (step 3a).  If not, create it, then kill any surviving wrapper/`tee` processes (`pkill -9 -u "$USER" -f 'capture-legacy|hazard-shadow-bin'`) before retrying.

## If `Resource is write-locked by another user` or `SASUSER.PROFILE ... will be opened instead`

Another SAS session holds locks on your `$HOME/sasuser*` profile dir.  Two fixes:

```bash
# Find any lingering SAS processes and kill them
ps -fu "$USER" | grep -iE 'sas|dms' | grep -v grep
pkill -u "$USER" -f 'sas'

# Use a private per-run sasuser dir (already in the sample commands above)
mkdir -p ~/sasuser-priv
sas -nodms -sasuser ~/sasuser-priv ...
```

## If the wrapper fires but `.dta` files aren't captured

- Check one of the `<uid>.meta` files — the `tmpdir=` line records which dir the wrapper scanned.  If that dir doesn't match where SAS actually wrote the XPORT file, we need to adjust.
- SAS may use a different env var: `$TMPQDIR`, `$HAZTEMP`, `$SASWORK`.  Let the hazard developers know which one and we can patch the wrapper.

## If SAS reports an error

- `/tmp/<name>.log` — the SAS log (what SAS saw)
- `~/hazard-capture/hazard/<uid>.meta` → `real_exit=N` — what the hazard binary returned
  - `0` = success
  - `16` = hazard called `hzfxit()` (halting error; see the listing for the reason)
  - `1` = segfault-equivalent

Both signals together usually pinpoint the source.

---

## If `$HAZAPPS` isn't set

The wrapper uses `$HAZAPPS/hazard` and `$HAZAPPS/hazpred` as its fallback path to the real binaries.  At sites without `$HAZAPPS`, tell the wrapper explicitly by copying the example config file:

```bash
cd ~/hazard-shadow-bin
cp capture.env.example capture.env
# Edit capture.env — uncomment and set:
#   HAZARD_REAL=/absolute/path/to/hazard
#   HAZPRED_REAL=/absolute/path/to/hazpred
```

The wrapper auto-sources `capture.env` (in the same dir as the script) on every invocation, so this persists across SAS runs without you re-exporting anything.

Find the real binaries with:

```bash
find / -name hazard -type f -executable 2>/dev/null | grep -v shadow-bin | head
find / -name hazpred -type f -executable 2>/dev/null | grep -v shadow-bin | head
```

---

## FAQ

**Can I re-run a single `.sas` after a failed attempt?**
Yes.  Each invocation produces a unique uid; nothing overwrites.  Rerun the one you want — extra tuples from failures are harmless.

**Do I need root?**
No.  The wrapper inherits your shell's environment and doesn't touch anything outside `$HOME`.  Run as whoever normally runs `PROC HAZARD`.

**Will this affect other users / cron jobs?**
No.  The `$PATH` modification lives in your shell only.

**SAS writes to an auto-cleaning scratch dir — will the `.dta` still get captured?**
Yes.  The wrapper snapshots `$TMPDIR` *before* handing control to the real binary and *before* SAS's cleanup runs.

**How long does the full run take?**
Typically 5–20 minutes wall time.  A couple of the multiphase stepwise examples are the slowest; the rest fit trivially.

**What goes back in the tarball?**
Just the date-stamped `~/hazard-capture-results-YYYY-MM-DD.tar.gz` from step 7.  You can throw away `~/hazard-shadow-bin/` once that tarball is delivered.

---

Questions → whoever requested the capture, or open an issue at https://github.com/ehrlinger/hazard/issues.  Partial captures are still useful — ship whatever you have if something goes sideways.

---

## Appendix — for hazard developers

Building the kit tarball:

```bash
cd /path/to/hazard
make capture-kit
# Produces hazard-capture-kit.tar.gz at the repo root.
```

The target (in `Makefile.am`) bundles the wrapper, `capture-order.txt`, `capture.env.example`, this README, and a fallback `sas/` dir of all `.sas` examples.

If the operator already has HAZARD installed with `$HAZAPPS` / `$HZEXAMPLES` set AND can use the `.sas` files on their system directly, you can skip the tarball and just send them `scripts/capture-legacy.sh` + `scripts/capture-order.txt` + a pointer to this doc.  They'll drop both files into their own `~/hazard-shadow-bin/` and follow the workflow from step 2.

**Before shipping the kit:** check `scripts/capture-order.txt`.  The dependency ordering was populated 2026-04-22 by grepping LIBNAME EXAMPLES / OUTHAZ= / INHAZ= / DATA EXAMPLES.* across `examples/*.sas`; update it if new examples have been added since.
