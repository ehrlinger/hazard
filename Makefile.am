# Copyright (C) 2000 The Cleveland Clinic Foundation <hazard@bio.ri.ccf.org>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

main_DATA = AUTHORS.md ChangeLog COPYING INSTALL.md NEWS.md README.md THANKS.md
SANITIZER_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer

# Because the "make dist" target does not include DATA files...
EXTRA_DIST = $(main_DATA)

SUBDIRS = src macros docs examples

.PHONY: asan

asan:
	$(MAKE) clean
	$(MAKE) all CFLAGS="$(CFLAGS) $(SANITIZER_FLAGS)" LDFLAGS="$(LDFLAGS) $(SANITIZER_FLAGS)"
