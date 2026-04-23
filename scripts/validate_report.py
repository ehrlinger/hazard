#!/usr/bin/env python3
"""
Generate platform-specific validation report from corpus harness output.

Parses the output from tests/validate_corpus.sh and generates a structured
markdown report with pass/fail/skip statistics, platform details, and
guidance on expected divergences.

Usage:
    python3 scripts/validate_report.py \\
        --log /tmp/validation-macos-v4.4-20260423_101530.log \\
        --platform macos \\
        --reference v4.3.0-unix-20260423 \\
        --version 4.4.2 \\
        --output docs/VALIDATION-REPORT-MACOS-v4.4.2.md
"""

import argparse
import re
import sys
from pathlib import Path
from datetime import datetime


def parse_validation_log(log_file):
    """Parse corpus validation output and extract results."""
    log_path = Path(log_file)
    if not log_path.exists():
        raise FileNotFoundError(f"Log file not found: {log_file}")
    
    with open(log_path) as f:
        content = f.read()
    
    results = {
        'passed': [],
        'failed': [],
        'skipped': [],
        'binary': None,
        'timestamp': None,
        'passed_count': 0,
        'failed_count': 0,
        'skipped_count': 0,
    }
    
    # Extract binary path
    if match := re.search(r'binary\s*:\s*(.+)', content):
        results['binary'] = match.group(1).strip()
    
    # Extract test results (look for PASS/FAIL/SKIP lines)
    for line in content.split('\n'):
        if match := re.match(r'\s+(PASS|FAIL|SKIP):\s+(\S+)', line):
            status, name = match.groups()
            if status == 'PASS':
                results['passed'].append(name)
            elif status == 'FAIL':
                results['failed'].append(name)
            else:
                results['skipped'].append(name)
    
    # Extract summary counts
    if match := re.search(r'passed\s*:\s*(\d+)', content):
        results['passed_count'] = int(match.group(1))
    if match := re.search(r'failed\s*:\s*(\d+)', content):
        results['failed_count'] = int(match.group(1))
    if match := re.search(r'skipped\s*:\s*(\d+)', content):
        results['skipped_count'] = int(match.group(1))
    
    # Fallback: count from parsed results if summary not found
    if results['passed_count'] == 0:
        results['passed_count'] = len(results['passed'])
    if results['failed_count'] == 0:
        results['failed_count'] = len(results['failed'])
    if results['skipped_count'] == 0:
        results['skipped_count'] = len(results['skipped'])
    
    return results


def generate_report(platform, version, reference, log_file, output_file):
    """Generate markdown validation report."""
    results = parse_validation_log(log_file)
    
    # Map platform names for display
    platform_display = {
        'linux': 'Linux/UNIX',
        'macos': 'macOS',
        'windows': 'Windows',
    }
    platform_name = platform_display.get(platform, platform.upper())
    
    # Status emoji
    pass_status = "✅ PASS" if results['failed_count'] == 0 else "⚠️ ISSUES"
    
    report = f"""---
tags: [hazard, validation, v{version}, {platform}, test-harness]
date: {datetime.now().isoformat()}
platform: {platform}
version: v{version}
reference: {reference}
---

# HAZARD v{version} Validation Report — {platform_name}

**Report Generated:** {datetime.now().strftime('%Y-%m-%d %H:%M UTC')}  
**Platform:** {platform_name}  
**Version Tested:** v{version}  
**Reference Corpus:** {reference}  
**Binary:** `{results.get('binary', 'unknown')}`

## Executive Summary

| Metric | Result | Status |
|---|---|---|
| **Tests Passed** | {results['passed_count']} | ✅ |
| **Tests Failed** | {results['failed_count']} | {"✅ None" if results['failed_count'] == 0 else f"❌ {results['failed_count']}"} |
| **Tests Skipped** | {results['skipped_count']} | ℹ️ |
| **Overall** | {pass_status} |

## Test Results

### ✅ Passed Tests ({len(results['passed'])})
"""
    
    if results['passed']:
        for test in sorted(results['passed']):
            report += f"- {test}\n"
    else:
        report += "_None_\n"
    
    if results['failed']:
        report += f"""
### ❌ Failed Tests ({len(results['failed'])})
"""
        for test in sorted(results['failed']):
            report += f"- {test}\n"
    
    if results['skipped']:
        report += f"""
### ℹ️ Skipped Tests ({len(results['skipped'])})
"""
        for test in sorted(results['skipped']):
            report += f"- {test}\n"
    
    report += f"""
## Platform Configuration

- **Operating System:** {platform_name}
- **Test Timestamp:** {datetime.now().isoformat()}
- **Reference Source:** {reference}
- **Log File:** [{Path(log_file).name}]({Path(log_file).absolute()})

## Known Expected Divergences

The following differences are **expected and documented** when comparing v4.4 output against v4.3.0 reference:

### 1. Organization String Update
- **Reference (v4.3.0):** "The Cleveland Clinic"
- **Current (v4.4):** "The Cleveland Clinic Foundation"
- **Cause:** Metadata string updated in codebase
- **Impact:** String matching only; no numerical effect

### 2. Numerical Convergence Divergence (hm.deadp.VALVES)

**Expected:** One example (`hm.deadp.VALVES`) shows numerical parameter divergence:

| Parameter | v4.3.0 | v4.4 | Change |
|---|---|---|---|
| Iterations | 1 | 0 | Different depth |
| Function evals | 3 | 8 | More steps explored |
| Constant MUC | 0.0001452016 | 8.403295E-05 | −42% |
| Log-likelihood | −1864.76 | −1536.4 | +150.4 |
| Output size | 12,684 B | 49,191 B | +288% |

**Root Cause:** Commit `557f3ef` fixes undefined behaviour in XPORT byte-swapping:
- **v4.3.0:** Used overlapping `swab()` calls (UB, corrupted variable metadata)
- **v4.4:** Uses portable `hzd_bswap_short()` / `hzd_bswap_int()` helpers (well-defined)
- **Result:** Different byte order → different parsed variable structure → different optimizer path

**Why this is NOT a regression:**
- ✅ v4.4 is **more correct** (UB fixed)
- ✅ All v4.4 outputs should be **identical across platforms** (self-consistent)
- ✅ This divergence is **documented** in `tests/corpus/FINDINGS.md` §2a

For full technical analysis, see: [`docs/CODEBASE_ANALYSIS.md`](../docs/Claude_CODEBASE_ANALYSIS.md)

## Platform Consistency Check

**For Release Validation:**

1. ✅ All platforms should show **identical test results** for v4.4
2. ✅ Known divergence (hm.deadp.VALVES) should appear on all platforms
3. ❌ Any platform-specific failures indicate a real bug
4. ℹ️ Skipped tests (hazpred) are expected until hazpred references are captured

## Next Steps

### If All Platforms Pass:
1. ✅ Proceed to release (v4.4.x is platform-consistent)
2. ✅ Document reference versions in release notes
3. ✅ Archive validation reports for audit trail

### If Any Platform Fails:
1. ❌ Investigate the failure (platform-specific compiler bug?)
2. ❌ Recompile on that platform with `-DNDEBUG` if not already done
3. ❌ Run `make check` to verify unit tests pass
4. ❌ File issue with platform-specific details

## Related Documentation

- **Codebase Analysis:** [`docs/CODEBASE_ANALYSIS.md`](../docs/Claude_CODEBASE_ANALYSIS.md) — Root cause of v4.3 → v4.4 differences
- **Corpus Findings:** [`tests/corpus/FINDINGS.md`](../tests/corpus/FINDINGS.md) — Acceptance harness findings and known issues
- **Multi-Platform Workflow:** [`docs/MULTI-PLATFORM-VALIDATION.md`](./MULTI-PLATFORM-VALIDATION.md) — Step-by-step instructions for this validation campaign
- **Modernization Policy:** [`docs/Claude_MODERNIZATION_GUIDE.md`](./Claude_MODERNIZATION_GUIDE.md) — Policy on numerical correctness and bit-compatibility

## Validation Campaign Metadata

```yaml
version: v{version}
platform: {platform}
reference: {reference}
campaign_date: {datetime.now().strftime('%Y-%m-%d')}
generated: {datetime.now().isoformat()}
binary: {results.get('binary', 'unknown')}
tests_passed: {results['passed_count']}
tests_failed: {results['failed_count']}
tests_skipped: {results['skipped_count']}
```

---

**Generated by:** `scripts/validate_report.py`  
**Report Template Version:** 1.0
"""
    
    output_path = Path(output_file)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    
    with open(output_path, 'w') as f:
        f.write(report)
    
    print(f"✅ Report generated: {output_path}")
    print(f"   Tests passed: {results['passed_count']}")
    print(f"   Tests failed: {results['failed_count']}")
    print(f"   Tests skipped: {results['skipped_count']}")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument(
        '--log', 
        required=True, 
        help='Path to validation log file'
    )
    parser.add_argument(
        '--platform', 
        required=True,
        choices=['linux', 'macos', 'windows'],
        help='Platform this validation ran on'
    )
    parser.add_argument(
        '--reference', 
        required=True, 
        help='Reference corpus version (e.g., v4.3.0-unix-20260423)'
    )
    parser.add_argument(
        '--version', 
        required=True, 
        help='HAZARD version being validated (e.g., 4.4.2)'
    )
    parser.add_argument(
        '--output', 
        help='Output report file (default: docs/VALIDATION-REPORT-<PLATFORM>-v<VERSION>.md)'
    )
    
    args = parser.parse_args()
    
    if args.output is None:
        platform_upper = args.platform.upper()
        args.output = f"docs/VALIDATION-REPORT-{platform_upper}-v{args.version}.md"
    
    try:
        generate_report(
            args.platform,
            args.version,
            args.reference,
            args.log,
            args.output
        )
    except Exception as e:
        print(f"❌ Error: {e}", file=sys.stderr)
        sys.exit(1)
