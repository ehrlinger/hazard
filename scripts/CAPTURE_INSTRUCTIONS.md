# Capturing the HAZARD Acceptance-Test Corpus

## What you're helping with

The HAZARD C package (the `hazard` / `hazpred` binaries that SAS invokes as `PROC HAZARD` / `PROC HAZPRED`) is being modernised.  Before that work can proceed safely, we need a **reference corpus**: the exact inputs and outputs from a known-good legacy install running each example `.sas` file.  Once we have that, every future change to the code can be validated against it automatically — the new binary's output must match byte-for-byte.

The capture you're about to do takes maybe 30 minutes of attended work plus however long your SAS runs take.  It's a one-time job.  The output is a tarball that John will drop into the `hazard` repo.

---

## Quick start (if HAZARD v4.3.1 is already installed)

If your site already has v4.3.1 and the SAS environment exports `$HAZAPPS` (install dir) + `$HZEXAMPLES` (example `.sas` dir), the wrapper script is the only thing you need.  Skip the kit tarball.

```sh
# Drop the wrapper on PATH as both `hazard` and `hazpred`
mkdir -p ~/hazard-shadow-bin
cp /path/to/capture-legacy.sh ~/hazard-shadow-bin/
chmod +x ~/hazard-shadow-bin/capture-legacy.sh
ln -sf ~/hazard-shadow-bin/capture-legacy.sh ~/hazard-shadow-bin/hazard
ln -sf ~/hazard-shadow-bin/capture-legacy.sh ~/hazard-shadow-bin/hazpred
export PATH=~/hazard-shadow-bin:$PATH

# Run the example corpus through SAS
for sas in "$HZEXAMPLES"/*.sas; do sas -nodms "$sas"; done

# Ship the results back
tar -czf hazard-capture-results.tar.gz ~/hazard-capture/
```

That's it.  Tuples land in `~/hazard-capture/` (hazard/ and hazpred/ subdirs), uniquely IDed per invocation.  Rest of this doc is for cases where site conventions differ, or troubleshooting is needed.

---

## What you need before starting

- A Unix host (Linux, AIX, Solaris — anywhere the legacy `hazard` install runs)
- **SAS installed** with the HAZARD module configured — however you'd normally run `PROC HAZARD`
- Your SAS session exports **`$HAZAPPS`** pointing at the HAZARD install dir (most sites do this automatically).  If it doesn't, you can set the binary paths by hand — see the "Alternate configuration" section below.
- Your SAS session exports **`$HZEXAMPLES`** pointing at the dir with the `.sas` example files.  Same story — standard on most HAZARD installs.  If unset, the tarball John sent includes a bundled `sas/` copy as a fallback.

---

## One-time setup

### 1. Unpack the kit

```sh
mkdir -p ~/hazard-capture-work
cd ~/hazard-capture-work
tar -xzf /path/to/hazard-capture-kit.tar.gz
ls
# You should see:
#   capture-legacy.sh
#   capture.env.example     (optional — only needed if $HAZAPPS isn't set)
#   README.md               (this file)
#   sas/                    (bundled .sas examples — fallback if $HZEXAMPLES isn't set)
```

### 2. Point SAS at the wrapper

Build a "shadow bin" dir containing symlinks that masquerade as the real binaries, and put it first on `$PATH`:

```sh
mkdir -p ~/hazard-capture-work/shadow-bin
ln -sf ~/hazard-capture-work/capture-legacy.sh ~/hazard-capture-work/shadow-bin/hazard
ln -sf ~/hazard-capture-work/capture-legacy.sh ~/hazard-capture-work/shadow-bin/hazpred
export PATH=~/hazard-capture-work/shadow-bin:$PATH

# Sanity check — both should resolve to the wrapper, not the real binary
which hazard
which hazpred
# Expected: /<home>/hazard-capture-work/shadow-bin/hazard
```

That's it.  The wrapper reads `$HAZAPPS` to find the real binaries and defaults the capture dir to `$HOME/hazard-capture/`.  No other env vars required.

If `which hazard` still points at the real binary, your `$PATH` modification didn't stick — fix that or capture won't happen.

### Alternate configuration (only if `$HAZAPPS` isn't set at your site)

If `echo $HAZAPPS` is empty, tell the wrapper where the real binaries live by copying the example config:

```sh
cp capture.env.example capture.env
# Then edit capture.env to set HAZARD_REAL and HAZPRED_REAL to the
# absolute paths of the real hazard and hazpred executables.  The
# wrapper auto-sources capture.env on every invocation.
```

Find the real binaries with:

```sh
find / -name hazard -type f -executable 2>/dev/null | head
find / -name hazpred -type f -executable 2>/dev/null | head
```

---

## Run the captures

Run each `.sas` file through SAS normally.  The wrapper silently records every invocation.  Preferred source: whatever the site has installed at `$HZEXAMPLES`.  Fallback: the bundled `sas/` directory.

```sh
cd ~/hazard-capture-work

# Prefer the site's installed examples; fall back to the bundled copies
SAS_DIR="${HZEXAMPLES:-$PWD/sas}"
echo "Using .sas files from: $SAS_DIR"

for sas in "$SAS_DIR"/*.sas; do
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
