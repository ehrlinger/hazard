# Capturing the HAZARD Acceptance-Test Corpus

## What you're helping with

The HAZARD C package (the `hazard` / `hazpred` binaries that SAS invokes as `PROC HAZARD` / `PROC HAZPRED`) is being modernised.  Before that work can proceed safely, we need a **reference corpus**: the exact inputs and outputs from a known-good legacy install running each example `.sas` file.  Once we have that, every future change to the code can be validated against it automatically — the new binary's output must match byte-for-byte.

The capture you're about to do takes maybe 30 minutes of attended work plus however long your SAS runs take.  It's a one-time job.  The output is a tarball that John will drop into the `hazard` repo.

---

## What you need before starting

- A Unix host (Linux, AIX, Solaris — anywhere the legacy `hazard` install runs)
- **SAS installed** with the HAZARD module configured — however you'd normally run `PROC HAZARD`
- **The legacy `hazard` and `hazpred` executables** — find the paths; you'll export them as env vars below.  A typical location is something like `/usr/local/hazard-4.3.1/bin/hazard` but it depends on how the site installed HAZARD.  If unsure, run `which hazard` inside a SAS session or check `$PATH` in the user that normally runs `PROC HAZARD`.
- **The 23 `.sas` example files.**  They're in `tests/*.sas` in the `ehrlinger/hazard` repo.  I'll attach them (or a tarball of them) along with this instruction doc.  They each expect data at paths like `!HZEXAMPLES/data/avc` — the standard HAZARD install layout.  If you're running on the same host where HAZARD is installed, this should Just Work.

---

## One-time setup

### 1. Put the capture script and the .sas files somewhere

```sh
# Pick any working directory
mkdir -p ~/hazard-capture-work
cd ~/hazard-capture-work

# Expand the tarball John sent (contains capture-legacy.sh + all .sas files)
tar -xzf hazard-capture-kit.tar.gz
ls -la
# You should see:
#   capture-legacy.sh
#   sas/              (directory with hz.death.AVC.sas, hm.death.AVC.sas, ...)

chmod +x capture-legacy.sh
```

### 2. Find the legacy binaries and export the env vars

```sh
# Adjust these paths to match YOUR install
export HAZARD_REAL=/usr/local/hazard-4.3.1/bin/hazard
export HAZPRED_REAL=/usr/local/hazard-4.3.1/bin/hazpred

# Sanity check — both should print the binary paths
ls -la "$HAZARD_REAL" "$HAZPRED_REAL"
```

If those paths don't exist, find the right ones before continuing.  A quick way:

```sh
find / -name hazard -type f -executable 2>/dev/null | head
find / -name hazpred -type f -executable 2>/dev/null | head
```

### 3. Create the capture staging directory

```sh
export HAZARD_CAPTURE_DIR=~/hazard-capture-work/captured
mkdir -p "$HAZARD_CAPTURE_DIR"
```

### 4. Build a "shadow bin" directory so SAS picks up the wrapper instead of the real binaries

```sh
mkdir -p ~/hazard-capture-work/shadow-bin
ln -sf ~/hazard-capture-work/capture-legacy.sh ~/hazard-capture-work/shadow-bin/hazard
ln -sf ~/hazard-capture-work/capture-legacy.sh ~/hazard-capture-work/shadow-bin/hazpred

# Make sure shadow-bin comes FIRST in PATH, before wherever the real binaries live
export PATH=~/hazard-capture-work/shadow-bin:$PATH

# Sanity check — these should both resolve to the wrapper script, not the real binary
which hazard
which hazpred
# Expected: /<home>/hazard-capture-work/shadow-bin/hazard
```

If `which hazard` still points at the real binary, PATH isn't right — fix that before continuing or the capture won't happen.

---

## Run the captures

Run each `.sas` file through SAS normally.  The wrapper silently records every invocation.

```sh
cd ~/hazard-capture-work

for sas in sas/*.sas; do
    name=$(basename "$sas" .sas)
    echo ""
    echo "=========================================="
    echo "Running $name"
    echo "=========================================="
    sas -nodms -log /tmp/$name.log -print /tmp/$name.lst "$sas"
    echo "exit=$?"
done
```

A few of the `.sas` files may error out — that's fine, we want to capture whatever happens.  The wrapper doesn't care about exit status.

### How do I know it worked?

After each run, the capture directory should contain new tuples.  After all runs:

```sh
ls $HAZARD_CAPTURE_DIR/hazard/ | head -20
ls $HAZARD_CAPTURE_DIR/hazpred/ | head -20
```

Each invocation produces a group of files with a matching `<uid>` prefix (a timestamp + pid + random number):

```
1745500432.12345.4711.stdin              # stdin text the binary read
1745500432.12345.4711.lst                # the binary's stdout
1745500432.12345.4711.hzr.AVC.AVCS.dta   # the XPORT dataset it read
1745500432.12345.4711.meta               # our metadata — argv, timestamp, etc.
```

If you see those, capture is working.  Expect roughly 23 groups under `hazard/` plus some number under `hazpred/`.

---

## If something looks wrong

### The wrapper isn't firing (no files appear in $HAZARD_CAPTURE_DIR)

- `which hazard` — is it pointing at the wrapper or at the real binary?  If the real binary, PATH is wrong.
- Is SAS perhaps hardcoding the path to the real binary in a config file (bypass `$PATH`)?  Common spots: `autoexec.sas`, `sasv9.cfg`, HAZARD's own config scripts.  Look for `HAZARD_BIN` or direct paths.  If hardcoded, change the config to use PATH resolution, or point it at the shadow-bin symlink.

### The wrapper fires but the .dta file isn't captured

- What's `$TMPDIR`?  The wrapper snapshots `$TMPDIR/hzr*.dta` before calling the real binary.  If SAS writes elsewhere (e.g. `$TMPQDIR`, `$HAZTEMP`), we need to adjust — tell John and we'll patch the wrapper.
- Check `$HAZARD_CAPTURE_DIR/hazard/*.meta` — the `tmpdir=` line records which dir the wrapper looked in.

### SAS reports an error but I don't know if it's from SAS or hazard

- `/tmp/<name>.log` — the SAS log
- `$HAZARD_CAPTURE_DIR/hazard/<uid>.meta` — `real_exit=N` tells you the hazard binary's exit code.  0 = fine, 16 = hazard called `hzfxit()` (a non-fatal but halting error; see the .lst for reason), 1 = segfault-equivalent.

---

## When you're done

Restore the original `PATH` in your shell (open a new one or `unset PATH`-then-re-source your profile) so nothing else accidentally runs through the wrapper.

Then tarball the capture directory and send it back:

```sh
cd ~/hazard-capture-work
tar -czf hazard-capture-results.tar.gz captured/
ls -la hazard-capture-results.tar.gz
```

Ship `hazard-capture-results.tar.gz` to John.  A typical tarball size is ~5–15 MB — the XPORT `.dta` files are most of the bulk.

That's it.  You can remove `~/hazard-capture-work/` once the tarball is safely delivered.

---

## Quick troubleshooting FAQ

**Q. Can I re-run a single `.sas` file after an earlier failed run?**
Yes.  Each invocation generates a new `<uid>`, so nothing overwrites.  Just run the one you want.  Extra tuples from failed attempts are harmless — we'll sort them out at the end.

**Q. Do I need to run as a specific user?**
Run as whoever you'd normally run `PROC HAZARD` as.  The wrapper inherits everything from your shell; it doesn't need root.

**Q. Will this affect production SAS jobs?**
Only in your shell session (because you set `PATH` in that shell).  Other users' sessions and cron jobs are unaffected.  The wrapper is only reachable through your modified `PATH`.

**Q. My SAS writes output to a directory that auto-cleans after the run — will the .dta still get captured?**
Yes.  The wrapper copies the `.dta` out of `$TMPDIR` *before* handing control to the real binary, and *before* SAS's cleanup runs.  Whatever's in `$TMPDIR` at wrapper-start time gets saved.

**Q. I got a `SKIP: $TMPDIR not set` or similar from the wrapper — what now?**
Export `TMPDIR`.  Point it at whatever dir SAS is actually using for scratch (often `/tmp` or `/var/tmp`).  Re-run.

**Q. How long does the full 23-file capture take?**
Depends on your hardware, but typically 5–20 minutes wall time.  Most examples fit trivially; a couple of the multiphase stepwise examples may take a minute or two each.

---

Send questions to John (`ehrlinj@ccf.org`) if anything unexpected happens.  Worst case, ship whatever you captured and any error logs — partial captures are still useful.

---

## Appendix — for John: building the capture-kit tarball

Before handing this off to the operator, assemble the kit:

```sh
# From the hazard repo root
cd /path/to/hazard

mkdir -p /tmp/hazard-capture-kit/sas
cp scripts/capture-legacy.sh /tmp/hazard-capture-kit/
cp scripts/CAPTURE_INSTRUCTIONS.md /tmp/hazard-capture-kit/README.md
cp tests/*.sas /tmp/hazard-capture-kit/sas/
# Optionally include examples/*.sas too — they exercise the same binaries
cp examples/*.sas /tmp/hazard-capture-kit/sas/ 2>/dev/null || true

cd /tmp
tar -czf hazard-capture-kit.tar.gz hazard-capture-kit/
ls -la hazard-capture-kit.tar.gz
```

Ship `hazard-capture-kit.tar.gz` + a pointer to this doc.  The operator's workflow assumes that's what they receive.
