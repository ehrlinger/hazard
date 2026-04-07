# Installation Guide

These are generic instructions for installing the HAZARD package. For more details, see the [README.md](README.md) and [docs/vignettes/installation.md](docs/vignettes/installation.md).

## Basic Installation

1. **Configure the source code:**
   ```sh
   sh ./configure
   # Or, to install in a custom location:
   sh ./configure --prefix=/your/path
   ```
2. **Compile the package:**
   ```sh
   make
   ```
3. **(Optional) Run self-tests:**
   ```sh
   make check
   ```
4. **Install:**
   ```sh
   make install
   ```
   - Executables: `/prefix/bin`
   - Libraries: `/prefix/lib`
   - Headers: `/prefix/include`
   - Man pages: `/prefix/man/man?`
   - Info files: `/prefix/info`

5. **Uninstall:**
   ```sh
   make uninstall
   ```

6. **Clean up build files:**
   ```sh
   make clean      # Remove binaries/objects
   make distclean  # Remove all build/config files
   ```

## Compiler Options

- `--enable-warnings` — Enable extra compiler warnings (`-Wall -Wextra -Wno-unused-parameter`)
- `--enable-sanitizers` — Enable AddressSanitizer and UndefinedBehaviorSanitizer at configure time
- `make asan` — Rebuild the configured tree with sanitizer flags for a quick local diagnostic build

On macOS, `./configure` now ignores an exported `CC=/opt/homebrew/bin/gcc-14` and falls back to `clang`, because that shell-level compiler setting may be broken. Set `HAZARD_KEEP_CC=1` if you want to force the exported compiler anyway.

For more, see `sh ./configure --help` or the [installation vignette](docs/vignettes/installation.md).
