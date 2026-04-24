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
    
    # Extract test results.  PASS/FAIL lines are always per-example:
    #   "  PASS: hm.deadp.VALVES"
    # SKIP lines, however, are also emitted for harness preconditions:
    #   "  SKIP: inputs dir missing — ..."
    #   "  SKIP: binary '/.../hazard' not built"
    #   "  SKIP: no v4.3.0 references captured for hazpred (...)"
    # Naïvely capturing the first token after "SKIP:" would record
    # "inputs" / "binary" / "no" as "test names".  Only accept a SKIP
    # as a per-example result when the token after it looks like an
    # example name (contains a dot, like "hm.deadp.VALVES").
    for line in content.split('\n'):
        if match := re.match(r'\s+(PASS|FAIL):\s+(\S+)', line):
            status, name = match.groups()
            if status == 'PASS':
                results['passed'].append(name)
            else:
                results['failed'].append(name)
            continue
        if match := re.match(r'\s+SKIP:\s+(\S+\.[^\s]+)\s*(?:$|\(|-|—)', line):
            results['skipped'].append(match.group(1))
    
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

The following differences are **expected and documented** when comparing v4.4 output against the v4.3.0 reference (authoritative analysis in [`ROOT-CAUSE-ANALYSIS.md`](../ROOT-CAUSE-ANALYSIS.md) v3.x and [`tests/corpus/FINDINGS.md`](../tests/corpus/FINDINGS.md) §2a):

### 1. Organization String Update
- **Reference (v4.3.0):** "The Cleveland Clinic"
- **Current (v4.4):** "The Cleveland Clinic Foundation"
- **Cause:** Metadata string updated in the codebase between versions
- **Impact:** Cosmetic only; not normalised by `tests/corpus_normalize.sh`

### 2. Version Banner
- Normalised by `tests/corpus_normalize.sh` (`C-Version X.Y.Z`)
- Not flagged as a diff after normalisation

### 3. Cross-toolchain Numerical Divergence (macOS / Apple Silicon vs gcc family)

On macOS / Apple clang / arm64, seven of the seven hazard examples produce output numerically distinct from — but self-consistent with — the gcc-family result. On Linux / gcc / glibc and Windows / MinGW-w64 gcc the output bit-matches v4.3.0 exactly on the log-likelihood metric.

| Example | Linux gcc (v4.3.1 and v4.4.2) | macOS clang-apple (v4.3.1 and v4.4.2) |
|---|---|---|
| hm.deadp.VALVES LL | −1864.76 | −1536.4 |

**Root cause:** Cross-toolchain floating-point non-determinism (gcc + glibc-derived libm vs Apple clang + Apple libm on arm64) accumulated over the thousands of FP operations in the parametric-hazard optimiser. The 92 commits between v4.3.1 and v4.4.2 are numerically inert on both toolchains — confirmed by one-shot GHA `linux-ll-check.yml` + `windows-ll-check.yml` runs.

**What this is NOT:**
- ❌ Not caused by commit `557f3ef` (the XPORT byte-swap UB fix). That hypothesis was disproven by a direct test — old `swab(src,src)` and new `hzd_bswap_*` helpers produce bit-identical output on glibc.
- ❌ Not a code-history regression. Git bisect between v4.3.1 and v4.4.2 would find nothing; both endpoints produce the same LL on each platform.

**Implications for release:**
- ✅ CCF Linux upgrades (v4.3.0 → v4.4.x) see **zero numerical drift**
- ✅ Windows/MinGW output bit-matches Linux on the LL metric
- ⚠️ macOS/Apple-Silicon output is distinct-but-self-consistent; reference corpus is split into a `v4.3.0` gcc-family bucket and a `v4.4.2-macos-arm64` clang-apple bucket; `tests/validate_corpus.sh` auto-selects by host

Do **not** claim "v4.4 is more correct" — neither toolchain family has been validated against an independent numerical reference (e.g. SAS PROC LIFEREG, hand-worked example).

## Platform Consistency Check

**For Release Validation:**

1. ✅ Within a toolchain family (gcc-family OR Apple-clang), all builds should produce **identical** test results for v4.4 across the 92-commit window
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

- **Root-Cause Analysis:** [`ROOT-CAUSE-ANALYSIS.md`](../ROOT-CAUSE-ANALYSIS.md) — Definitive analysis of the v4.3 → v4.4 divergence (v3.x, closed)
- **Corpus Findings:** [`tests/corpus/FINDINGS.md`](../tests/corpus/FINDINGS.md) — Acceptance harness findings and known issues
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
