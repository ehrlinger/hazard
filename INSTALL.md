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

- `--with-warnings` — Enable extra compiler warnings
- `--disable-assert` — Disable assertions (not recommended for development)
- `--with-f2c`, `--with-g77`, `--with-f77=F77` — Fortran compiler options

For more, see `sh ./configure --help` or the [installation vignette](docs/vignettes/installation.md).
