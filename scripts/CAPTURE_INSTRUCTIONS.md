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
```

### 2. Make the wrapper executable and create the symlinks

**This is the step the first run missed.**  Without `chmod +x` and real symlinks, SAS quietly falls back to the real binary on PATH and your capture directory stays empty.

```bash
cd ~/hazard-shadow-bin
chmod +x capture-legacy.sh
ln -sf "$PWD/capture-legacy.sh" hazard
ln -sf "$PWD/capture-legacy.sh" hazpred

# Verify — all three should be executable, and hazard/hazpred should be symlinks
ls -la capture-legacy.sh hazard hazpred
# Expect:
#   -rwxr-xr-x  ... capture-legacy.sh
#   lrwxr-xr-x  ... hazard    -> /home/you/hazard-shadow-bin/capture-legacy.sh
#   lrwxr-xr-x  ... hazpred   -> /home/you/hazard-shadow-bin/capture-legacy.sh
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

### 4. Smoke-test with one `.sas` file

Before running all 23+ files, run one through SAS and confirm the wrapper fired.  `hm.death.AVC` is a good choice — small, self-contained, no dependencies on earlier runs.

```bash
SAS_DIR="${HZEXAMPLES:-$HOME/hazard-shadow-bin/sas}"
sas -nodms -log /tmp/smoke.log -print /tmp/smoke.lst "$SAS_DIR/hm.death.AVC.sas"

# Confirm the wrapper captured tuples
ls ~/hazard-capture/hazard/
# Expect a group of files with a shared uid prefix, something like:
#   1745500432.12345.4711.stdin
#   1745500432.12345.4711.lst
#   1745500432.12345.4711.hzr.AVC.AVCS.dta
#   1745500432.12345.4711.meta
```

If `~/hazard-capture/hazard/` is empty or missing, the wrapper didn't fire — jump to "If the wrapper isn't firing" before running the full batch.

### 5. Run the full corpus in dependency order

Some `.sas` scripts write estimate datasets under `$HZEXAMPLES/sasest/` that later scripts read.  Running them out of order will produce wrong output for the dependents.  `capture-order.txt` in the shadow-bin dir lists the correct sequence; the loop below reads it and falls back to alphabetical for anything not listed.

```bash
cd ~/hazard-shadow-bin
SAS_DIR="${HZEXAMPLES:-$PWD/sas}"
ORDER_FILE=$PWD/capture-order.txt

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
    echo ""
    echo "=========================================="
    echo "Running $name"
    echo "=========================================="
    sas -nodms -log /tmp/$name.log -print /tmp/$name.lst "$sas"
    echo "exit=$?"
done
```

Some `.sas` files may error out — that's fine, we want whatever happens.  The wrapper doesn't care about exit status.

### 6. Tarball and ship

```bash
tar -czf ~/hazard-capture-results.tar.gz -C ~ hazard-capture
ls -la ~/hazard-capture-results.tar.gz
# Typical size: 5–15 MB (XPORT .dta files are most of the bulk)
```

Ship `~/hazard-capture-results.tar.gz` back to whoever requested the capture.  You can also include your `/tmp/*.log` + `/tmp/*.lst` SAS outputs — the hazard developers may cross-reference them.

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

- `ls -la ~/hazard-shadow-bin/hazard ~/hazard-shadow-bin/hazpred` — are they executable symlinks?  If they're plain files (no `l` prefix) or lack `x` permission, SAS failed to exec them and fell back to the real binary silently.  Redo step 2.
- `which hazard` — is it pointing at the shadow-bin or at the real binary?  If the real binary, step 3 (PATH export) didn't stick in your current shell.  Redo it.
- Is SAS hardcoding the binary path in a site config file (bypassing `$PATH`)?  Common spots: `autoexec.sas`, `sasv9.cfg`, HAZARD's own setup scripts.  Search for `HAZARD_BIN` / direct paths / `x` command invocations.  If hardcoded, either change the config to honour `$PATH`, or point the hardcoded path at `~/hazard-shadow-bin/hazard`.
- `~/hazard-capture/` can be preexisting and empty from a prior failed attempt — that's fine, it just means nothing got written THIS run.

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
Just `~/hazard-capture-results.tar.gz`.  You can throw away `~/hazard-shadow-bin/` once that tarball is delivered.

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

**Before shipping the kit:** make sure `scripts/capture-order.txt` has the real dependency order filled in — it's a placeholder until the hazard developers populate it.
