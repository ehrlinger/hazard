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

## Windows Source Builds

Windows source builds are supported via MSYS2 UCRT64 (not Cygwin).

Install dependencies in an MSYS2 UCRT64 shell:

```sh
pacman -S --needed --noconfirm \
   autoconf automake bison flex make \
   mingw-w64-ucrt-x86_64-gcc
```

Build from source:

```sh
autoreconf -fi
./configure --prefix="$PWD/dist/hazard"
make -j"$(nproc)"
make install
```

To compile Windows builds with `NDEBUG` defined:

```sh
autoreconf -fi
./configure --prefix="$PWD/dist/hazard" CPPFLAGS="-DNDEBUG"
make -j"$(nproc)"
make install
```

## Build Verification

```sh
# Unit tests
make -C tests/unit clean all check

# Integration-only suite
HAZARD_BIN=$PWD/src/hazard/hazard.exe \
   bash tests/run_all_tests.sh --integration-only --skip-memory --skip-concurrent
```

## SAS Configuration

HAZARD integrates with SAS via its autocall macro library. You must add `HAZAPPS` to the `sasautos` path in your SAS configuration file (`sasv8.cfg`, `sasv9.cfg`, or similar).

**Locate your SAS config file:**

- **Unix:** `~/sasv?.cfg` (in your home directory)
- **Windows:** `C:\Program Files\SAS Institute\SAS\V8\` or `My Documents\My SAS Files\V8\`. Search for `sas*.cfg` if unsure.

**Unix** — edit `sasautos` to add `!HAZAPPS`:
```sh
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
