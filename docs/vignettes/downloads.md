---
title: "Downloads"
description: "Binary distributions and source code for HAZARD"
---

# Downloads

## About the Package

The HAZARD package is a set of programs for fitting and predicting from parametric hazard functions. Originally developed at the University of Alabama at Birmingham, it is currently maintained at [The Cleveland Clinic Foundation](https://www.clevelandclinic.org).

HAZARD runs from within SAS®. Macros and installation instructions are included to integrate the package into your SAS installation.

---

## Binary Distributions

Pre-compiled binaries are available for the following platforms:

| Platform | Notes |
|---|---|
| IBM AIX | Contact [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org) |
| SUN Solaris | `hazard.solaris.tar.gz` |
| Microsoft Windows | `hazard.win.zip` |

> Binary packages have been compiled, linked, and tested on the platforms listed above. For other platforms, compile from source.

### Installing a Binary Distribution

**Unix/Linux/macOS:**

```bash
# Navigate to your desired install parent (e.g. /usr/local or $HOME)
cd /usr/local

# Unpack the distribution
gzip -d < /path/to/hazard.PLATFORM.tar.gz | tar -xvf -
```

This creates a `hazard/` directory with the [standard layout](installation.md#3-install-the-binary-executables).

**Windows:**

Extract `hazard.win.zip` to a root-level directory such as `C:\`. The package must be placed at the root due to path length constraints. This creates `C:\hazard\` with the standard layout.

After unpacking, proceed to [environment setup](installation.md#4-set-up-environments).

---

## Source Code Distribution

The C source code compiles on any platform with a standard ANSI C toolchain. Known working platforms beyond Windows and Solaris include Linux and macOS.

### Build Requirements

| Tool | Where to get it |
|---|---|
| GCC | https://gcc.gnu.org |
| Flex | https://www.gnu.org/software/flex |
| Bison | https://www.gnu.org/software/bison |
| Autoconf / Automake | https://www.gnu.org/software/autoconf |

**Windows:** Install [Cygwin](https://cygwin.com) (full distribution) — includes all required tools.

### Getting the Source

Clone from GitHub:
```bash
git clone https://github.com/your-org/hazard.git
```

Or download the gzipped tar archive: `hazard-latest.tar.gz`

Then follow the [compilation instructions](installation.md#2-compile-the-source-code).

---

## Notes on Platform Compatibility

The C code is ANSI standard and has been ported to multiple systems without problems. However, mathematical precision and library function parameters can vary between platforms. If you encounter issues, please report them to [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org) — platform-specific workarounds can typically be incorporated into future releases.

Contributions of pre-compiled binary packages for additional platforms are welcome. Contact us for packaging instructions.

---

*Previous: [Introduction](introduction.md) · Next: [Installation](installation.md)*
