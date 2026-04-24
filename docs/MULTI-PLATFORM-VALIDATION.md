# Multi-Platform Corpus Validation Workflow

**Purpose:** Standardized procedure for capturing v4.3.0 references and validating v4.4 releases across Linux/UNIX, macOS, and Windows.

**Status:** Template for v4.4.x release validation campaign (2026-04-23)

---

## Overview

This document provides step-by-step instructions for:
1. **Phase 1:** Capture v4.3.0 references on UNIX production environment
2. **Phase 2:** Validate v4.4 binary on UNIX against v4.3.0 references
3. **Phase 3:** Validate v4.4 binary on macOS against v4.3.0 references
4. **Phase 4:** Validate v4.4 binary on Windows against v4.3.0 references
5. **Phase 5:** Generate platform-specific validation reports

**Expected Timeline:** 
- Phase 1 (UNIX v4.3.0 capture): ~2-4 hours (site-dependent)
- Phases 2-4 (v4.4 validation): ~30 min each per platform
- Phase 5 (reporting): ~1 hour

---

## Prerequisites

All platforms require:
- Git checkout of `release/4.4` branch
- Compiled `hazard` and `hazpred` binaries
- `bash` shell with standard Unix tools (grep, diff, awk)
- Python 3.6+ for analysis scripts
- ~500 MB free disk space for test outputs

### Platform-Specific Setup

**UNIX/Linux (reference site):**
```bash
# Verify SAS environment (if running captures)
which sas
sas -version

# Verify hazard binary from production install
/opt/hazard/bin/hazard.exe -h 2>&1 | head -5
```

**macOS:**
```bash
# Verify build environment
which gcc || which clang
xcode-select --install  # if needed
```

**Windows (MSYS2/MinGW):**
```bash
# Verify MSYS2 shell and toolchain
gcc --version
which bash
```

---

## Phase 1: Capture v4.3.0 References (UNIX Production Site)

### 1.1 Prerequisites

You need:
- Access to the UNIX production environment running v4.3.0
- SAS 9.4+ installed (for driver `.sas` files if re-capturing)
- The 7 prepared `.input` files from corpus (already in repo)
- The corresponding `.dta` data files from prior captures

### 1.2 Procedure

```bash
#!/bin/bash
# PHASE 1: Capture v4.3.0 references on UNIX

set -euo pipefail
SITE_NAME="ccf-rhel8"  # Change to your site identifier
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
CAPTURE_DIR="/tmp/hz-capture-${SITE_NAME}-${TIMESTAMP}"
REPO_DIR="~/Documents/GitHub/hazard"

mkdir -p "$CAPTURE_DIR"

# 1. Verify v4.3.0 binary
V43_BIN="/opt/hazard/bin/hazard.exe"
if [ ! -x "$V43_BIN" ]; then
    echo "ERROR: v4.3.0 binary not found at $V43_BIN"
    exit 1
fi

echo "Using v4.3.0 binary: $V43_BIN"
$V43_BIN -h 2>&1 | head -3

# 2. Copy input corpus to short temp directory (work around 80-byte buffer limit)
WORK_DIR="/tmp/hz-work-$$"
mkdir -p "$WORK_DIR"
cd "$REPO_DIR/tests/corpus/hazard/inputs"
for file in *.input *.dta; do
    if [ -f "$file" ]; then
        cp "$file" "$WORK_DIR/$file"
    fi
done

# 3. Run each example through v4.3.0
EXAMPLES=("hm.deadp.VALVES" "hm.death.AVC.1" "hm.death.AVC.2" \
          "hm.death.AVC.deciles" "hz.deadp.KUL" "hz.death.AVC" "hz.tm123.OMC")

for example in "${EXAMPLES[@]}"; do
    input="$WORK_DIR/${example}.input"
    if [ ! -f "$input" ]; then
        echo "SKIP: $example (input not found)"
        continue
    fi
    
    echo "CAPTURING: $example"
    
    # Run binary and capture output
    (
        cd "$WORK_DIR"
        timeout 300 "$V43_BIN" < "$input" > "${example}.lst" 2> "${example}.stderr"
    ) || EXIT=$?
    
    # Extract .haz output if it was created (some examples generate it)
    if [ -f "$WORK_DIR/${example}.haz" ]; then
        cp "$WORK_DIR/${example}.haz" "$CAPTURE_DIR/${example}.haz"
    fi
    
    # Save .lst output
    if [ -f "$WORK_DIR/${example}.lst" ]; then
        cp "$WORK_DIR/${example}.lst" "$CAPTURE_DIR/${example}.lst"
    fi
    
    # Save stderr (error context if any)
    if [ -s "$WORK_DIR/${example}.stderr" ]; then
        cp "$WORK_DIR/${example}.stderr" "$CAPTURE_DIR/${example}.stderr"
    fi
    
    # Save metadata
    cat > "$CAPTURE_DIR/${example}.meta" << EOF
Platform: $(uname -s) $(uname -r)
Site: $SITE_NAME
Version: v4.3.0
Binary: $V43_BIN
Timestamp: $(date -u +%Y-%m-%dT%H:%M:%SZ)
Exit Code: ${EXIT:-0}
EOF
done

# 4. Clean up temp
rm -rf "$WORK_DIR"

# 5. Create tarball for transport
TARBALL="${CAPTURE_DIR}.tar.gz"
cd /tmp
tar czf "$TARBALL" "hz-capture-${SITE_NAME}-${TIMESTAMP}"

echo ""
echo "=========================================="
echo "Capture complete:"
echo "  Location: $CAPTURE_DIR"
echo "  Tarball:  $TARBALL"
echo "  Size:     $(du -sh "$CAPTURE_DIR" | cut -f1)"
echo ""
echo "Next: Transport $TARBALL to your workstation"
echo "=========================================="
```

### 1.3 Verification

After capture completes:
```bash
# Verify capture contents
ls -la /tmp/hz-capture-*/
wc -l /tmp/hz-capture-*/*.lst | tail -1

# Check for errors
grep -l "ERROR\|Convergence" /tmp/hz-capture-*/*.stderr 2>/dev/null || echo "No errors"
```

---

## Phase 2: Import Captures & Validate on UNIX

### 2.1 Setup

```bash
#!/bin/bash
# PHASE 2: Import v4.3.0 references and validate v4.4 on UNIX

set -euo pipefail
REPO_DIR="~/Documents/GitHub/hazard"
CAPTURE_TARBALL="$1"  # Pass tarball from Phase 1

if [ -z "$CAPTURE_TARBALL" ] || [ ! -f "$CAPTURE_TARBALL" ]; then
    echo "Usage: $0 <path-to-capture-tarball>"
    exit 1
fi

cd "$REPO_DIR"

# 1. Extract captures
echo "Extracting v4.3.0 captures..."
tar xzf "$CAPTURE_TARBALL" -C /tmp
CAPTURE_DIR="/tmp/$(basename "$CAPTURE_TARBALL" .tar.gz)"

# 2. Import into corpus reference directory
REF_DIR="$REPO_DIR/tests/corpus/hazard/reference/v4.3.0-unix-$(date +%Y%m%d)"
mkdir -p "$REF_DIR"

for file in "$CAPTURE_DIR"/*.lst; do
    basename=$(basename "$file")
    cp "$file" "$REF_DIR/$basename"
    # Also copy .meta for provenance
    if [ -f "${file%.lst}.meta" ]; then
        cp "${file%.lst}.meta" "$REF_DIR/${basename%.lst}.meta"
    fi
done

echo "Imported $(ls "$REF_DIR"/*.lst | wc -l) reference files to $REF_DIR"

# 3. Build v4.4 binary if needed
if [ ! -x "src/hazard/hazard" ]; then
    echo "Building v4.4 hazard binary..."
    make clean > /dev/null 2>&1
    make -j4 > /dev/null 2>&1
fi

# 4. Run validation against imported references
cd "$REPO_DIR/tests"
export HAZARD_BIN="$REPO_DIR/src/hazard/hazard"
export REFERENCE="v4.3.0-unix-$(date +%Y%m%d)"

echo "Running corpus validation..."
bash validate_corpus.sh 2>&1 | tee "/tmp/validation-unix-v4.4-$(date +%Y%m%d_%H%M%S).log"
```

---

## Phase 3: Validate v4.4 on macOS

### 3.1 Setup

```bash
#!/bin/bash
# PHASE 3: Validate v4.4 on macOS using imported v4.3.0 references

set -euo pipefail
REPO_DIR="$HOME/Documents/GitHub/hazard"

cd "$REPO_DIR"

# 1. Verify branch is release/4.4
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [ "$CURRENT_BRANCH" != "release/4.4" ]; then
    echo "ERROR: Not on release/4.4 branch (currently: $CURRENT_BRANCH)"
    exit 1
fi

# 2. Pull latest changes
git pull origin release/4.4

# 3. Build v4.4 for macOS
echo "Building v4.4 on macOS..."
make clean
./configure --quiet
make -j4

# 4. Run validation tests
cd "$REPO_DIR/tests"
export HAZARD_BIN="$REPO_DIR/src/hazard/hazard"

# Test against v4.3.0 reference (expect known divergences)
echo "=== Validating against v4.3.0 reference ==="
REFERENCE="v4.3.0-unix-$(ls ../tests/corpus/hazard/reference/ | grep v4.3.0 | head -1 | sed 's/.*v4.3.0/v4.3.0/')"
bash validate_corpus.sh 2>&1 | tee "/tmp/validation-macos-v4.4-$(date +%Y%m%d_%H%M%S).log"

# Also test self-consistency (if v4.4.2 reference exists)
if [ -d "corpus/hazard/reference/v4.4.2" ]; then
    echo ""
    echo "=== Validating v4.4 self-consistency ==="
    REFERENCE="v4.4.2" bash validate_corpus.sh 2>&1 | tee -a "/tmp/validation-macos-v4.4-$(date +%Y%m%d_%H%M%S).log"
fi
```

### 3.2 Run on macOS

```bash
# SSH into your macOS machine or run locally
ssh macos-host "bash /path/to/phase3-macos.sh"

# Or run directly if already on macOS
bash phase3-macos.sh
```

---

## Phase 4: Validate v4.4 on Windows

### 4.1 Setup (MSYS2)

```bash
#!/bin/bash
# PHASE 4: Validate v4.4 on Windows (MSYS2/MinGW)

set -euo pipefail
REPO_DIR="C:/Users/$(whoami)/Documents/GitHub/hazard"

# Convert Windows paths to MSYS2 format
REPO_DIR="${REPO_DIR//C:\//\/c\/}"
REPO_DIR="${REPO_DIR//\\/\/}"

cd "$REPO_DIR"

# 1. Verify branch
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [ "$CURRENT_BRANCH" != "release/4.4" ]; then
    echo "ERROR: Not on release/4.4 branch (currently: $CURRENT_BRANCH)"
    exit 1
fi

# 2. Pull latest
git pull origin release/4.4

# 3. Configure and build (on Windows, use Release build)
echo "Building v4.4 on Windows (Release)..."
make clean
CPPFLAGS="-DNDEBUG" ./configure --quiet
make -j4

# 4. Verify binary was created
if [ ! -f "src/hazard/hazard.exe" ]; then
    echo "ERROR: Binary not created at src/hazard/hazard.exe"
    exit 1
fi

# 5. Run validation
cd "$REPO_DIR/tests"
export HAZARD_BIN="$REPO_DIR/src/hazard/hazard.exe"

# Test against v4.3.0 reference
echo "=== Validating v4.4 on Windows ==="
REFERENCE="v4.3.0-unix-$(ls ../tests/corpus/hazard/reference/ | grep v4.3.0 | head -1 | sed 's/.*v4.3.0/v4.3.0/')"
bash validate_corpus.sh 2>&1 | tee "/tmp/validation-windows-v4.4-$(date +%Y%m%d_%H%M%S).log"
```

### 4.2 Run on Windows

```powershell
# From PowerShell on Windows machine
C:\Users\<user>\Documents\GitHub\hazard\phase4-windows.sh
```

Or via SSH:
```bash
ssh windows-host "bash ~/Documents/GitHub/hazard/phase4-windows.sh"
```

---

## Phase 5: Generate Platform-Specific Validation Reports

### 5.1 Analysis Script

Create `scripts/validate_report.py`:

```python
#!/usr/bin/env python3
"""
Generate platform-specific validation report from corpus harness output.

Usage:
    python3 validate_report.py \
        --log /tmp/validation-<platform>-v4.4-*.log \
        --platform macos|linux|windows \
        --reference v4.3.0-unix-20260423 \
        --version 4.4.2
"""

import argparse
import re
import sys
from pathlib import Path
from datetime import datetime

def parse_validation_log(log_file):
    """Parse corpus validation output and extract results."""
    with open(log_file) as f:
        content = f.read()
    
    results = {
        'passed': [],
        'failed': [],
        'skipped': [],
        'platform': None,
        'timestamp': None,
    }
    
    # Extract header info
    if match := re.search(r'binary\s*:\s*(.+)', content):
        results['binary'] = match.group(1).strip()
    
    # Extract results
    for line in content.split('\n'):
        if match := re.match(r'\s+(PASS|FAIL|SKIP):\s+(\S+)', line):
            status, name = match.groups()
            if status == 'PASS':
                results['passed'].append(name)
            elif status == 'FAIL':
                results['failed'].append(name)
            else:
                results['skipped'].append(name)
    
    # Extract summary
    if match := re.search(r'passed\s*:\s*(\d+)', content):
        results['passed_count'] = int(match.group(1))
    if match := re.search(r'failed\s*:\s*(\d+)', content):
        results['failed_count'] = int(match.group(1))
    if match := re.search(r'skipped\s*:\s*(\d+)', content):
        results['skipped_count'] = int(match.group(1))
    
    return results

def generate_report(platform, version, reference, log_file, output_file):
    """Generate markdown validation report."""
    results = parse_validation_log(log_file)
    
    report = f"""---
tags: [hazard, validation, v{version}, {platform}]
date: {datetime.now().isoformat()}
platform: {platform}
version: v{version}
reference: {reference}
---

# HAZARD v{version} Validation Report — {platform.upper()}

**Report Date:** {datetime.now().strftime('%Y-%m-%d %H:%M UTC')}  
**Platform:** {platform}  
**Version:** v{version}  
**Reference:** {reference}  
**Binary:** {results.get('binary', 'unknown')}

## Summary

| Metric | Count | Status |
|---|---|---|
| Passed | {results.get('passed_count', 0)} | ✅ |
| Failed | {results.get('failed_count', 0)} | {"✅" if results.get('failed_count', 0) == 0 else "❌"} |
| Skipped | {results.get('skipped_count', 0)} | ℹ️ |

## Test Results

### Passed Tests
"""
    
    for test in sorted(results['passed']):
        report += f"- ✅ {test}\n"
    
    if results['failed']:
        report += "\n### Failed Tests\n"
        for test in sorted(results['failed']):
            report += f"- ❌ {test}\n"
    
    if results['skipped']:
        report += "\n### Skipped Tests\n"
        for test in sorted(results['skipped']):
            report += f"- ℹ️ {test}\n"
    
    report += f"""
## Platform Details

- **OS:** {platform}
- **Test Date:** {datetime.now().isoformat()}
- **Log File:** {log_file}

## Known Divergences

See `docs/CODEBASE_ANALYSIS.md` for expected differences between v4.3.0 and v4.4.x.

### Expected (v4.3.0 vs v4.4):
- **hm.deadp.VALVES:** Numerical divergence due to UB fix in byte-swap code (commit 557f3ef)
- **Organization name:** "The Cleveland Clinic" vs "The Cleveland Clinic Foundation"

## Next Steps

1. Compare reports across all three platforms
2. Verify consistency (v4.4 should produce identical output on all platforms)
3. Document any platform-specific issues
4. Proceed to release if all platforms pass

---

*Generated by `scripts/validate_report.py`*
"""
    
    with open(output_file, 'w') as f:
        f.write(report)
    
    print(f"Report written to {output_file}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--log', required=True, help='Validation log file')
    parser.add_argument('--platform', required=True, 
                       choices=['linux', 'macos', 'windows'])
    parser.add_argument('--reference', required=True, help='Reference version')
    parser.add_argument('--version', required=True, help='v4.4.x version')
    parser.add_argument('--output', help='Output report file')
    
    args = parser.parse_args()
    
    output = args.output or f"docs/VALIDATION-REPORT-{args.platform.upper()}-v{args.version}.md"
    generate_report(args.platform, args.version, args.reference, args.log, output)
```

### 5.2 Run Report Generation

After validation on each platform:

```bash
# macOS report
python3 scripts/validate_report.py \
    --log /tmp/validation-macos-v4.4-*.log \
    --platform macos \
    --reference v4.3.0-unix-20260423 \
    --version 4.4.2

# Linux report
python3 scripts/validate_report.py \
    --log /tmp/validation-linux-v4.4-*.log \
    --platform linux \
    --reference v4.3.0-unix-20260423 \
    --version 4.4.2

# Windows report
python3 scripts/validate_report.py \
    --log /tmp/validation-windows-v4.4-*.log \
    --platform windows \
    --reference v4.3.0-unix-20260423 \
    --version 4.4.2
```

---

## Complete Workflow Script

Orchestrate all phases:

```bash
#!/bin/bash
# orchestrate-multi-platform-validation.sh
# Full validation campaign across all platforms

set -euo pipefail

REPO_DIR="$HOME/Documents/GitHub/hazard"
VERSION="4.4.2"
CAMPAIGN_DATE=$(date +%Y%m%d)
REPORTS_DIR="$REPO_DIR/docs/validation-reports-${CAMPAIGN_DATE}"

mkdir -p "$REPORTS_DIR"

echo "=========================================="
echo "HAZARD v${VERSION} Multi-Platform Validation"
echo "Campaign Date: $CAMPAIGN_DATE"
echo "Reports: $REPORTS_DIR"
echo "=========================================="

# Phase 1: UNIX capture (manual, requires production access)
echo ""
echo "[1/5] PHASE 1: Capture v4.3.0 on UNIX (manual)"
echo "  Run on production UNIX machine:"
echo "  $ bash ~/scripts/phase1-capture-v430.sh"
echo ""
read -p "Press enter when v4.3.0 capture complete and tarball received..."

CAPTURE_TARBALL="$REPORTS_DIR/hz-capture-*.tar.gz"
if [ ! -f "$CAPTURE_TARBALL" ]; then
    echo "ERROR: No capture tarball found in $REPORTS_DIR"
    exit 1
fi

# Phase 2: UNIX validation
echo "[2/5] PHASE 2: Validate v4.4 on UNIX..."
ssh unix-host "bash /path/to/phase2-validate-unix.sh '$CAPTURE_TARBALL'" \
    > "$REPORTS_DIR/validation-unix-v${VERSION}.log" 2>&1

# Phase 3: macOS validation
echo "[3/5] PHASE 3: Validate v4.4 on macOS..."
ssh macos-host "bash /path/to/phase3-validate-macos.sh" \
    > "$REPORTS_DIR/validation-macos-v${VERSION}.log" 2>&1

# Phase 4: Windows validation
echo "[4/5] PHASE 4: Validate v4.4 on Windows..."
ssh windows-host "bash /path/to/phase4-validate-windows.sh" \
    > "$REPORTS_DIR/validation-windows-v${VERSION}.log" 2>&1

# Phase 5: Generate reports
echo "[5/5] PHASE 5: Generate platform reports..."
cd "$REPO_DIR"
for platform in linux macos windows; do
    python3 scripts/validate_report.py \
        --log "$REPORTS_DIR/validation-${platform}-*.log" \
        --platform "$platform" \
        --reference "v4.3.0-unix-${CAMPAIGN_DATE}" \
        --version "$VERSION"
done

echo ""
echo "=========================================="
echo "Campaign complete!"
echo "Reports: $REPORTS_DIR/"
ls -la "$REPORTS_DIR"/*.md
echo "=========================================="
```

---

## Troubleshooting

### "TMPDIR buffer overflow" Error

**Symptom:** SIGTRAP (exit 133) on long paths

**Solution:** Harness automatically creates short `/tmp` symlinks. If manual testing:
```bash
WORK_DIR=$(mktemp -d /tmp/hz.XXXXXX)
ln -s /actual/corpus/path/file.dta "$WORK_DIR/file.dta"
TMPDIR="$WORK_DIR" /path/to/hazard
```

### Byte-Order Differences

**Symptom:** Numerical divergence on first run

**Expected:** hm.deadp.VALVES shows known divergence from v4.3.0 due to UB fix (commit 557f3ef). This is **not** a regression.

**Verify:** All v4.4 outputs should be **identical across platforms** (byte-for-byte if using same compiler flags).

### Missing References

**Symptom:** "SKIP: no v4.x.x references captured"

**Solution:** Run Phase 1 capture, then import with correct directory name:
```bash
mkdir -p tests/corpus/hazard/reference/v4.3.0-unix-20260423
cp /path/to/capture/*.lst tests/corpus/hazard/reference/v4.3.0-unix-20260423/
```

---

## Related Documentation

- `docs/CODEBASE_ANALYSIS.md` — Root cause of v4.3 → v4.4 differences
- `tests/corpus/FINDINGS.md` — Acceptance harness findings and known issues
- `docs/Claude_MODERNIZATION_GUIDE.md` — Policy on numerical correctness

---

## Version History

| Date | Version | Changes |
|---|---|---|
| 2026-04-23 | 1.0 | Initial multi-platform validation workflow |

