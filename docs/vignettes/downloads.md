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

Pre-compiled release artifacts are published in GitHub Releases for this repository.

Typical artifact names:

| Platform | Artifact pattern |
|---|---|
| Linux x64 | `hazard-vX.Y.Z-linux-x64.tar.gz` |
| Linux arm64 | `hazard-vX.Y.Z-linux-arm64.tar.gz` |
| macOS arm64 | `hazard-vX.Y.Z-macos-arm64.tar.gz` |
| Windows x64 | `hazard-vX.Y.Z-windows-x64.zip` |

macOS Intel (`macos-x64`) binaries are not published — the GitHub Actions free tier retired its `macos-13` runner.  Intel macOS users should build from source; see the `macOS Intel (x86_64)` section of the repository's root [`INSTALL.md`](../../INSTALL.md#macos-intel-x86_64) for the exact commands.

Legacy binary packages may still be available via Cleveland Clinic contact channels.

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

Extract the Windows release `.zip` (for example `hazard-vX.Y.Z-windows-x64.zip`) to a root-level directory such as `C:\`. This creates a `hazard\` tree with the [standard layout](installation.md#3-install-the-binary-executables).

After unpacking, proceed to [environment setup](installation.md#4-set-up-environments).

---

## Source Code Distribution

The C source code compiles on platforms with a standard C toolchain and Autotools.

### Build Requirements

| Tool | Where to get it |
|---|---|
| GCC | https://gcc.gnu.org |
| Flex | https://www.gnu.org/software/flex |
| Bison | https://www.gnu.org/software/bison |
| Autoconf / Automake | https://www.gnu.org/software/autoconf |
| GNU Make | https://www.gnu.org/software/make |

**Windows:** Use [MSYS2](https://www.msys2.org) in the UCRT64 environment.

### Getting the Source

Clone from GitHub:
```bash
git clone https://github.com/ehrlinger/hazard.git
```

Or download the gzipped tar archive: `hazard-latest.tar.gz`

Then follow the [compilation instructions](installation.md#2-compile-the-source-code).

---

## Notes on Platform Compatibility

The C code is ANSI standard and has been ported to multiple systems without problems. However, mathematical precision and library function parameters can vary between platforms. If you encounter issues, please report them to [hazard@bio.ri.ccf.org](mailto:hazard@bio.ri.ccf.org) — platform-specific workarounds can typically be incorporated into future releases.

Contributions of pre-compiled binary packages for additional platforms are welcome. Contact us for packaging instructions.

---

*Previous: [Introduction](introduction.md) · Next: [Installation](installation.md)*
