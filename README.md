# HAZARD

[![CI](https://github.com/ehrlinger/hazard/actions/workflows/ci.yml/badge.svg)](https://github.com/ehrlinger/hazard/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/ehrlinger/hazard/branch/main/graph/badge.svg)](https://codecov.io/gh/ehrlinger/hazard)
[![License: GPL v2](https://img.shields.io/badge/License-GPL_v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

**Parametric hazard analysis for time-to-event data.**

HAZARD is a statistical computing package developed at [Cleveland Clinic](https://www.clevelandclinic.org) for fitting flexible, multi-phase parametric hazard models to survival data. It implements the **Conservation of Events theorem** (Turner et al.) to produce models with early, constant, and late hazard phases — commonly applied to cardiac surgery outcomes and other clinical research.

> Originally developed at the University of Alabama Birmingham. Currently developed and maintained at The Cleveland Clinic Foundation.

---

## Table of Contents

- [HAZARD](#hazard)
  - [Table of Contents](#table-of-contents)

  - [Overview](#overview)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
    - [From Source (Unix/Linux/macOS)](#from-source-unixlinuxmacos)
    - [From Source (Windows via MSYS2)](#from-source-windows-via-msys2)
    - [From Binary Distribution](#from-binary-distribution)
  - [Environment Setup](#environment-setup)
    - [Unix/Linux/macOS](#unixlinuxmacos)
    - [Windows](#windows)
    - [SAS Configuration](#sas-configuration)
  - [Quick Start](#quick-start)
  - [Project Structure](#project-structure)
  - [Documentation](#documentation)
  - [Vignettes](#vignettes)
  - [Citation](#citation)
  - [License](#license)
  - [Contact](#contact)

---


---

## Project Metadata & Links

- [Authors](AUTHORS.md)
- [License](COPYING.md)
- [Installation Guide](INSTALL.md)
- [Changelog](NEWS.md)
- [Acknowledgments](THANKS.md)

---

## Overview

HAZARD fits three-phase parametric hazard models of the form:

```
h(t | X) = μ₁(X) · SG₁(t)   [early phase]
          + μ₂(X)             [constant phase]
          + μ₃(X) · SG₃(t)   [late phase]
```

where each scale factor `μⱼ(X) = exp(intercept_j + βⱼᵀ X)` and the shaping functions `SG₁`, `SG₃` are flexible parametric forms supporting a wide range of hazard shapes.

**Key capabilities:**

- Multi-phase hazard modeling (early, constant, late phases)
- Right censoring, interval censoring, and repeating events
- Time-varying covariates and weighted events
- Automatic stepwise covariate selection (forward, backward, stepwise)
- Conservation of Events theorem for numerically stable parameter estimation
- Covariance and correlation matrix estimation
- SAS integration via XPORT format and `PROC HAZARD` / `PROC HAZPRED` syntax

**Tested SAS versions:** v8.x through v9.3.2

---

## Prerequisites

| Requirement | Purpose |
|---|---|
| SAS (v8 or later) | Data preparation and result post-processing |
| GCC or compatible C compiler | Building from source |
| Flex / GNU Bison | Lexer and parser generation (build only) |
| GNU Make | Build automation |
| Autoconf / Automake | Configure script generation (build only) |

SAS is required at runtime for data I/O. The C executables handle all numerical computation.

---

## Installation

### From Source (Unix/Linux/macOS)

```bash
# 1. Clone or download the source
git clone https://github.com/ehrlinger/hazard
cd hazard

# 2. Configure (default installs to /usr/local/hazard)
./configure

# If building from a fresh git checkout after autotools changes:
# autoreconf -fi

# Optional developer flags:
./configure --enable-warnings
./configure --enable-sanitizers

# On macOS, if your shell exports a broken Homebrew gcc-14 as CC,
# configure now falls back to clang automatically.
# Set HAZARD_KEEP_CC=1 to keep the exported compiler.

# To specify a custom install directory:
./configure --prefix=/path/to/install

# 3. Compile
make

# Or rebuild the configured tree with ASan + UBSan:
make asan

# 4. Smoke-test the built executables with staged local fixtures
./tests/run_local_example.sh hazard
./tests/run_local_example.sh hazpred

# Keep staged temp files/logs for inspection:
./tests/run_local_example.sh hazard --keep

# Notes:
# - The helper validates success by checking expected output artifacts.
# - hazard fixture runs may return non-zero while still producing output;
#   this is treated as a successful smoke test when output is present.

# 5. Install
make install
```

If you are building on a platform not previously tested, please report your experience to [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org) with the platform details.

### From Source (Windows via MSYS2)

Windows source builds are supported via **MSYS2 UCRT64** (same toolchain used in CI releases).

1. Install [MSYS2](https://www.msys2.org) and open the **MSYS2 UCRT64** shell.
2. Install build dependencies:

```bash
pacman -S --needed --noconfirm \
  autoconf automake bison flex make \
  mingw-w64-ucrt-x86_64-gcc
```

3. Build and install:

```bash
# Run this when building from git source after autotools edits
autoreconf -fi

./configure --prefix="$PWD/dist/hazard"
make -j"$(nproc)"
make install
```

### From Binary Distribution

**Unix/Linux/macOS:**

```bash
cd $HOME   # or your preferred install parent
gzip -d < /path/to/hazard.PLATFORM.tar.gz | tar -xvf -
```

This creates a `hazard/` directory with the standard layout.

**Windows:**

Extract the `.zip` distribution with your preferred archive tool to a root directory such as `C:\`. The distribution must be extracted to a root-level directory due to path length constraints.

Official release archives (including Windows `.zip`) are published in GitHub Releases for this repository.
Legacy downloads may also exist at:
https://www.lerner.ccf.org/quantitative-health/software/

### Verify Release Checksums

Each GitHub release includes a `SHA256SUMS.txt` file. Verify downloaded archives before extracting.

**Unix/Linux/macOS:**

```bash
# Linux (sha256sum)
sha256sum -c SHA256SUMS.txt

# macOS fallback if sha256sum is unavailable
shasum -a 256 -c SHA256SUMS.txt
```

**Windows (PowerShell):**

```powershell
# Example: verify one archive against SHA256SUMS.txt
$file = "hazard-vX.Y.Z-windows-x64.zip"
$actual = (Get-FileHash -Algorithm SHA256 $file).Hash.ToLower()
$expected = (Select-String -Path SHA256SUMS.txt -Pattern [regex]::Escape($file)).Line.Split(" ")[0].ToLower()
if ($actual -eq $expected) { "Checksum OK" } else { "Checksum MISMATCH" }
```

---

## Environment Setup

HAZARD requires four environment variables pointing to its installation directories.

### Unix/Linux/macOS

Add to your shell startup file (`.profile`, `.bash_profile`, `.zshrc`, or `.cshrc`):

```bash
# bash / zsh / sh / ksh
export HAZARD=/usr/local/hazard        # or $HOME/hazard
export HAZAPPS=$HAZARD/bin
export HZEXAMPLES=$HAZARD/examples
export MACROS=$HAZARD/macros
```

```csh
# csh / tcsh
setenv HAZARD /usr/local/hazard
setenv HAZAPPS $HAZARD/bin
setenv HZEXAMPLES $HAZARD/examples
setenv MACROS $HAZARD/macros
```

### Windows

Set the following **User** or **System** environment variables via
**Control Panel → System → Advanced → Environment Variables**:

| Variable | Recommended Value | Description |
|---|---|---|
| `TMPDIR` | `%TEMP%` | Temporary file storage |
| `HAZARD` | `C:\hazard` | Root installation directory |
| `HAZAPPS` | `%HAZARD%\bin` | Executables |
| `HZEXAMPLES` | `%HAZARD%\examples` | Example SAS programs and data |
| `MACROS` | `%HAZARD%\macros` | SAS helper macros |

A system restart may be required for the variables to take effect.

### SAS Configuration

HAZARD integrates with SAS via its autocall macro library. You must add `HAZAPPS` to the `sasautos` path in your SAS configuration file (`sasv8.cfg`, `sasv9.cfg`, or similar).

**Locate your SAS config file:**

- **Unix:** `~/sasv?.cfg` (in your home directory)
- **Windows:** `C:\Program Files\SAS Institute\SAS\V8\` or `My Documents\My SAS Files\V8\`. Search for `sas*.cfg` if unsure.

**Unix** — edit `sasautos` to add `!HAZAPPS`:
```
-sasautos ( '!SASROOT8/sasautos' '!HAZAPPS' )
```

**Windows** — edit `sasautos` and add `-noxwait`:
```sas
-SET SASAUTOS (
  "!sasroot\core\sasmacro"
  "!sasext0\graph\sasmacro"
  "!sasext0\stat\sasmacro"
  "!HAZAPPS"
)

/* Allow external commands to close automatically */
-noxwait
```

The `-noxwait` option prevents SAS from leaving a DOS window open after each external call.

---

## Build Verification

After building from source, you can run the test suite runner directly:

```bash
# Unit tests
make -C tests/unit clean all check

# Integration-only run (skip memory/concurrency suites)
HAZARD_BIN=$PWD/src/hazard/hazard.exe \
  bash tests/run_all_tests.sh --integration-only --skip-memory --skip-concurrent
```

If your shell exports a missing compiler path (for example `CC=/opt/homebrew/bin/gcc-14`), override test builds with `CC=cc`.

---

## Quick Start

Run one of the included example programs to verify your installation:

```sas
/* From SAS, with HZEXAMPLES on your path */
%include '!HZEXAMPLES/hm.death.AVC.sas';
```

This fits a three-phase hazard model to the AVC (atrioventricular canal) surgical outcomes dataset (310 observations, 70 events). Expected output:

```
Log likelihood = -181.109
Events conserved = 70.000
Convergence attained after 6 iterations, 9 function evaluations
```

See the [`examples/`](examples/) directory for additional programs demonstrating different model configurations.

---

## Project Structure

```
hazard/
├── src/                    C source code
│   ├── common/             Global data structures and shared utilities
│   ├── llike/              Log-likelihood engine (setlik.c, setcoe.c, setobj.c)
│   ├── optim/              BFGS/quasi-Newton optimizer
│   ├── model/              Early and late phase shaping functions
│   ├── vars/               Stepwise variable selection
│   ├── vcov/               Variance-covariance estimation (Cholesky)
│   ├── stat/               Statistical distributions
│   ├── utils/              Error handling and I/O utilities
│   ├── hazard/             PROC HAZARD executable
│   └── hazpred/            PROC HAZPRED executable
├── docs/                   Documentation
│   ├── hazard.pdf          PROC HAZARD user guide
│   ├── hazpred.pdf         PROC HAZPRED user guide
│   ├── overview.pdf        Parametric hazard analysis overview
│   ├── CODEBASE_ANALYSIS.md    Architectural overview (internal)
│   ├── MODERNIZATION_GUIDE.md  C modernization roadmap (internal)
│   └── R_MIGRATION_GUIDE.md    R migration roadmap (internal)
├── examples/               Example SAS programs and datasets
│   ├── hm.death.AVC.sas    Three-phase model: AV-canal repair mortality
│   ├── hp.death.AVC.sas    Hazard prediction example
│   └── data/               Example XPORT datasets
├── macros/                 SAS helper macros
└── tests/                  Reference test outputs
```

---


## Documentation
## Vignettes

The following vignettes provide detailed guides and examples:

- [Downloads](docs/vignettes/downloads.md)
- [Analysis Examples](docs/vignettes/examples.md)
- [Installation](docs/vignettes/installation.md)
- [Introduction to Hazard Function Technology](docs/vignettes/introduction.md)
- [SAS Utility Macros](docs/vignettes/macros.md)
- [User Guides](docs/vignettes/user_guides.md)

### User Vignettes

| Vignette | Description |
|---|---|
| [Introduction](docs/vignettes/introduction.md) | What is hazard function analysis; multi-phase model overview |
| [Downloads](docs/vignettes/downloads.md) | Binary distributions and source code |
| [Installation](docs/vignettes/installation.md) | Build, install, and configure for Unix/Windows/SAS |
| [Examples](docs/vignettes/examples.md) | Annotated walkthrough of all included example programs |
| [SAS Utility Macros](docs/vignettes/macros.md) | Reference for all helper macros |
| [User Guides](docs/vignettes/user_guides.md) | Full procedure reference documents |

### Reference Manuals

| Document | Formats |
|---|---|
| Overview of Procedures | [HTML](docs/user_guides/overview.html) · [PDF](docs/user_guides/overview.pdf) |
| The HAZARD Procedure | [HTML](docs/user_guides/hazard.html) · [PDF](docs/user_guides/hazard.pdf) |
| The HAZPRED Procedure | [HTML](docs/user_guides/hazpred.html) · [PDF](docs/user_guides/hazpred.pdf) |

### Developer Documentation

| Document | Description |
|---|---|
| [`docs/CODEBASE_ANALYSIS.md`](docs/CODEBASE_ANALYSIS.md) | Architectural overview for contributors |
| [`docs/MODERNIZATION_GUIDE.md`](docs/MODERNIZATION_GUIDE.md) | C modernization roadmap |
| [`docs/R_MIGRATION_GUIDE.md`](docs/R_MIGRATION_GUIDE.md) | R migration roadmap |

---

## Citation

If you use HAZARD in published research, please cite:

> Blackstone EH, Naftel DC, Turner ME Jr. **The decomposition of time-varying hazard into phases, each incorporating a separate stream of concomitant information.** *J Am Stat Assoc.* 1986;81(395):615–624.

> Turner ME Jr, Restrepo C, McCarville C, Hauck WW. **The continuity correction for the logistic disease risk model.** *Stat Med.* 1992.

---

## License

HAZARD is free software distributed under the **GNU General Public License v2**. See [COPYING](COPYING) for the full license text.

Copyright © 2000 The Cleveland Clinic Foundation.

---

## Contact

| | |
|---|---|
| **General inquiries** | [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org) |
| **Clinical/statistical questions** | Dr. Eugene H. Blackstone — [blackse@ccf.org](mailto:blackse@ccf.org) |
| **Bug reports** | [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org) |
| **Phone** | 216.444.6712 |

Contributions and platform compatibility reports are welcome. If you successfully build HAZARD on a new platform, please write to let us know.
