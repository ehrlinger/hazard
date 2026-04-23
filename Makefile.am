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

.PHONY: asan capture-kit

asan:
	$(MAKE) clean
	$(MAKE) all CFLAGS="$(CFLAGS) $(SANITIZER_FLAGS)" LDFLAGS="$(LDFLAGS) $(SANITIZER_FLAGS)"

# Assemble the hazard-capture-kit.tar.gz tarball the operator uses to
# capture reference inputs/outputs from the legacy v4.3.1 binary.  See
# scripts/CAPTURE_INSTRUCTIONS.md for the operator-facing workflow and
# docs/VALIDATION_PLAN.md for the overall design.
#
# The kit bundles:
#   * capture-legacy.sh       — the wrapper script
#   * capture.env.example     — optional per-site config template
#   * README.md               — instructions (a copy of CAPTURE_INSTRUCTIONS.md)
#   * sas/                    — fallback .sas examples, used only at sites that
#                               do not export $HZEXAMPLES
#   * macros/                 — bootstrap.hazard.sas, hazplot.sas, etc.  Some
#                               .sas examples %INC these via !MACROS/...;
#                               sites where the system MACROS dir is missing
#                               them (e.g. CCF's /programs/apps/sas/macro.library)
#                               point $MACROS at this shipped copy via -set.
CAPTURE_KIT_DIR = hazard-capture-kit
CAPTURE_KIT_TARBALL = hazard-capture-kit.tar.gz

capture-kit:
	rm -rf $(CAPTURE_KIT_DIR) $(CAPTURE_KIT_TARBALL)
	mkdir -p $(CAPTURE_KIT_DIR)/sas $(CAPTURE_KIT_DIR)/macros
	cp $(top_srcdir)/scripts/capture-legacy.sh        $(CAPTURE_KIT_DIR)/
	cp $(top_srcdir)/scripts/capture.env.example      $(CAPTURE_KIT_DIR)/
	cp $(top_srcdir)/scripts/capture-order.txt        $(CAPTURE_KIT_DIR)/
	cp $(top_srcdir)/scripts/CAPTURE_INSTRUCTIONS.md  $(CAPTURE_KIT_DIR)/README.md
	cp $(top_srcdir)/tests/*.sas     $(CAPTURE_KIT_DIR)/sas/ 2>/dev/null || true
	cp $(top_srcdir)/examples/*.sas  $(CAPTURE_KIT_DIR)/sas/ 2>/dev/null || true
	cp $(top_srcdir)/macros/*.sas    $(CAPTURE_KIT_DIR)/macros/ 2>/dev/null || true
	# Some sites' installed .sas examples %INC macros by unsuffixed name
	# (e.g. FILENAME HAZBOOT ('$MACROS/bootstrap.hazard')), while the
	# repo carries the .sas suffix.  Drop symlinks so both forms resolve.
	cd $(CAPTURE_KIT_DIR)/macros && for f in *.sas; do ln -sf "$$f" "$${f%.sas}"; done
	chmod +x $(CAPTURE_KIT_DIR)/capture-legacy.sh
	tar -czf $(CAPTURE_KIT_TARBALL) $(CAPTURE_KIT_DIR)
	rm -rf $(CAPTURE_KIT_DIR)
	@echo ""
	@echo "Built $(CAPTURE_KIT_TARBALL) ($$(du -h $(CAPTURE_KIT_TARBALL) | cut -f1)):"
	@tar -tzf $(CAPTURE_KIT_TARBALL) | sed 's/^/    /' | head -30
	@echo ""
	@echo "Hand this tarball to the operator; it embeds its own README.md."
