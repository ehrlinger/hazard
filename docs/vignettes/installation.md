---
title: "Installation"
description: "Build from source, install binary distributions, and configure Unix/Windows/SAS environments"
---

# Installation

## Overview

There are four steps to getting HAZARD running:

1. [Verify your SAS installation](#1-verify-your-sas-installation)
2. [Compile the source code](#2-compile-the-source-code) *(skip if using a binary distribution)*
3. [Install the executables](#3-install-the-binary-executables)
4. [Set up the system and SAS environments](#4-set-up-environments)

Platform-specific steps are marked **Unix** or **Windows** throughout. Unix instructions apply broadly to Linux and macOS.

---

## 1. Verify Your SAS Installation

HAZARD requires SAS for pre- and post-processing of data and results. It has been tested on SAS v8.x through v9.3.2.

**Unix:** From the command line, run:
```bash
sas
```

**Windows:** From the Start menu, go to **Programs → SAS Institute → SAS**.

---

## 2. Compile the Source Code

> Skip this section if you are using a [pre-compiled binary distribution](downloads.md).

HAZARD is written in ANSI C and uses Lex and Yacc for parsing. The following tools must be installed before compiling.

### Build Requirements

| Tool | Source |
|---|---|
| GCC (C compiler) | https://gcc.gnu.org |
| Flex (lexer) | https://www.gnu.org/software/flex |
| Bison (parser) | https://www.gnu.org/software/bison |
| GNU Make | https://www.gnu.org/software/make |
| Autoconf / Automake | https://www.gnu.org/software/autoconf |

**Windows:** Use [MSYS2](https://www.msys2.org) with the **UCRT64** environment.

### Build Steps

```bash
# If building from a fresh git checkout after autotools changes:
# autoreconf -fi

# 1. Configure the build
#    Default install path: /usr/local/hazard
./configure

#    To specify a custom install directory:
./configure --prefix=/path/to/install
#    Example: ./configure --prefix=$HOME/hazard

# 2. Compile
make

# 3. Install
make install
```

**Windows (MSYS2 UCRT64):** install dependencies, then build/install:
```bash
pacman -S --needed --noconfirm \
  autoconf automake bison flex make \
  mingw-w64-ucrt-x86_64-gcc

autoreconf -fi
./configure --prefix="$PWD/dist/hazard"
make -j"$(nproc)"
make install
```

> If you successfully compile on a platform not listed here, please report it to [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org) with your platform details. Contributions of binary packages for new platforms are also welcome.

---

## 3. Install the Binary Executables

If you compiled from source, `make install` completes this step — proceed to [Set up environments](#4-set-up-environments).

For binary distributions, follow the platform-specific instructions in [Downloads](downloads.md), then continue below.

The distribution creates the following directory structure under your install root:

| Directory | Env Variable | Contents |
|---|---|---|
| `hazard/` | `$HAZARD` | Installation root |
| `hazard/bin/` | `$HAZAPPS` | Executables (`hazard`, `hazpred`, etc.) |
| `hazard/doc/` | — | Documentation |
| `hazard/examples/` | `$HZEXAMPLES` | Example SAS programs and data |
| `hazard/examples/data/` | — | Example datasets (XPORT format) |
| `hazard/examples/output/` | — | Reference outputs |
| `hazard/macros/` | `$MACROS` | SAS utility macros |

---

## 4. Set Up Environments

### Unix Environment Variables

Add to your shell startup file (`.profile`, `.bash_profile`, `.zshrc`, etc.):

```bash
# bash / zsh / sh / ksh
export HAZARD=/usr/local/hazard    # or $HOME/hazard
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

Contact your system administrator if you need assistance setting environment variables.

---

### Windows Environment Variables

Navigate to **Control Panel → System → Advanced → Environment Variables**.

Add the following under **User variables** (or **System variables** if you have administrator privileges):

| Variable | Recommended Value | Description |
|---|---|---|
| `TMPDIR` | `%TEMP%` | Temporary file storage |
| `HAZARD` | `C:\hazard` | Installation root |
| `HAZAPPS` | `%HAZARD%\bin` | Executables |
| `HZEXAMPLES` | `%HAZARD%\examples` | Example programs and data |
| `MACROS` | `%HAZARD%\macros` | SAS utility macros |

> A system restart may be required for the variables to take effect.

---

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
/* Setup the SAS System autocall library definition */
-SET SASAUTOS (
  "!sasroot\core\sasmacro"
  "!sasext0\graph\sasmacro"
  "!sasext0\stat\sasmacro"
  "!HAZAPPS"
)

/* Allow external commands to close automatically */
-noxwait
```

The `-noxwait` option prevents SAS from leaving a DOS command window open after each external call to the HAZARD executables.

---

## Testing the Installation

Run a quick local executable smoke test:

```bash
./tests/run_local_example.sh hazard
./tests/run_local_example.sh hazpred
```

Optionally run the integration test runner (skipping memory and concurrent suites for a fast check):

```bash
HAZARD_BIN=$PWD/src/hazard/hazard.exe \
  bash tests/run_all_tests.sh --integration-only --skip-memory --skip-concurrent
```

Then run any example program from SAS:

```sas
%include '!HZEXAMPLES/hm.death.AVC.sas';
```

Expected output:
```
Log likelihood = -181.109
Events conserved = 70.000
Convergence attained after 6 iterations, 9 function evaluations
```

See [Examples](examples.md) for a full catalog of included example programs.

---

## Contact

For installation problems: [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org) or call 216.444.6712.

---

*Previous: [Introduction](introduction.md) · Next: [Examples](examples.md)*
