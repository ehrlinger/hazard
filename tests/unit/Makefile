# tests/unit/Makefile
#
# Builds and runs the HAZARD C unit test suite.
#
# Usage:
#   make           — build all test binaries
#   make check     — build and run all tests
#   make clean     — remove built binaries and object files
#   make asan      — rebuild with AddressSanitizer + UBSan
#
# Prerequisites:
#   gcc (or clang; override with CC=clang)
#   The project source tree must be at ../../src/ relative to this file.

CC      ?= gcc
SRCDIR   = ../../src/common
CFLAGS   = -std=c99 -Wall -Wextra -Wno-unused-parameter -g \
           -I$(SRCDIR)

# All source files from src/common/ required by the unit tests.
# Grouped by what they provide.
COMMON_MATH = \
    $(SRCDIR)/hzd_set_Machn.c      \
    $(SRCDIR)/hzd_set_LnLim.c      \
    $(SRCDIR)/hzd_ln_eX_min1.c     \
    $(SRCDIR)/hzd_ln_A_div_B.c     \
    $(SRCDIR)/hzd_ln_1min_e_negeX.c \
    $(SRCDIR)/dlgaob.c

COMMON_SHAPING = \
    $(SRCDIR)/hzd_ln_G1_and_SG1.c  \
    $(SRCDIR)/hzd_ln_G3_and_SG3.c  \
    $(SRCDIR)/hzd_ln_eBXmin1divB.c  \
    $(SRCDIR)/hzd_ln_eX_plus1.c    \
    $(SRCDIR)/hzd_ln_eeX_min1.c    \
    $(SRCDIR)/hzd_ln_1min_enegX.c  \
    $(SRCDIR)/hzd_Error.c

# Global storage + init (must appear exactly once per binary).
GLOBALS = test_globals.c

# ------------------------------------------------------------------ #
# test_math_helpers                                                    #
#   Tests: hzd_ln_eX_min1, hzd_ln_A_div_B, hzd_ln_1min_e_negeX,     #
#          dlgaob                                                      #
# ------------------------------------------------------------------ #
test_math_helpers: $(GLOBALS) $(COMMON_MATH) test_math_helpers.c
	$(CC) $(CFLAGS) -o $@ $^ -lm

# ------------------------------------------------------------------ #
# test_shaping_functions                                               #
#   Tests: hzd_ln_G1_and_SG1 cases 1–6                               #
# ------------------------------------------------------------------ #
test_shaping_functions: $(GLOBALS) $(COMMON_MATH) $(COMMON_SHAPING) \
                         test_shaping_functions.c
	$(CC) $(CFLAGS) -o $@ $^ -lm

# ------------------------------------------------------------------ #
# test_obs_layout                                                      #
#   Tests: OBS array indexing constants — no global state needed      #
# ------------------------------------------------------------------ #
test_obs_layout: test_obs_layout.c
	$(CC) $(CFLAGS) -o $@ $^ -lm

# ------------------------------------------------------------------ #
# Aggregate targets                                                    #
# ------------------------------------------------------------------ #
ALL_BINS = test_math_helpers test_shaping_functions test_obs_layout

all: $(ALL_BINS)

# Build and run every test binary. Exit with failure if any test fails.
check: $(ALL_BINS)
	@echo ""
	@echo "========================================"
	@echo "Running HAZARD unit tests"
	@echo "========================================"
	@FAILURES=0; \
	for bin in $(ALL_BINS); do \
		echo ""; \
		echo "--- $$bin ---"; \
		./$$bin || FAILURES=$$((FAILURES + 1)); \
	done; \
	echo ""; \
	if [ $$FAILURES -eq 0 ]; then \
		echo "ALL UNIT TESTS PASSED"; \
	else \
		echo "$$FAILURES test binary/binaries FAILED"; \
		exit 1; \
	fi

# AddressSanitizer + UBSan build (requires GCC ≥ 9 or Clang ≥ 6).
asan: CFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer -O1
asan: clean $(ALL_BINS)
	@echo "ASan build complete. Run: make check"

clean:
	rm -f $(ALL_BINS) *.o
	$(MAKE) clean_coverage

# ------------------------------------------------------------------ #
# Coverage analysis (gcov / lcov)                                     #
#   Requires: gcc, lcov  (sudo apt install lcov)                      #
#   Usage:    make coverage                                            #
#   Output:   coverage.info  (for Codecov / CI upload)                #
#             coverage_html/ (browseable HTML report)                 #
# ------------------------------------------------------------------ #

# Separate build directory keeps .gcno / .gcda files isolated.
COV_DIR    = .cov
COV_CFLAGS = -std=c99 -Wall -Wextra -Wno-unused-parameter -O0 \
             -fprofile-arcs -ftest-coverage -I$(SRCDIR)

$(COV_DIR):
	@mkdir -p $(COV_DIR)

# ---- per-file rules -----------------------------------------------

$(COV_DIR)/test_globals.o: test_globals.c | $(COV_DIR)
	$(CC) $(COV_CFLAGS) -c -o $@ $<

$(COV_DIR)/test_math_helpers.o: test_math_helpers.c | $(COV_DIR)
	$(CC) $(COV_CFLAGS) -c -o $@ $<

$(COV_DIR)/test_shaping_functions.o: test_shaping_functions.c | $(COV_DIR)
	$(CC) $(COV_CFLAGS) -c -o $@ $<

$(COV_DIR)/test_obs_layout.o: test_obs_layout.c | $(COV_DIR)
	$(CC) $(COV_CFLAGS) -c -o $@ $<

# Sources from src/common/
$(COV_DIR)/%.o: $(SRCDIR)/%.c | $(COV_DIR)
	$(CC) $(COV_CFLAGS) -c -o $@ $<

# ---- object lists --------------------------------------------------

COV_COMMON_MATH = \
    $(COV_DIR)/hzd_set_Machn.o       \
    $(COV_DIR)/hzd_set_LnLim.o       \
    $(COV_DIR)/hzd_ln_eX_min1.o      \
    $(COV_DIR)/hzd_ln_A_div_B.o      \
    $(COV_DIR)/hzd_ln_1min_e_negeX.o \
    $(COV_DIR)/dlgaob.o

COV_COMMON_SHAPE = \
    $(COV_DIR)/hzd_ln_G1_and_SG1.o  \
    $(COV_DIR)/hzd_ln_G3_and_SG3.o  \
    $(COV_DIR)/hzd_ln_eBXmin1divB.o  \
    $(COV_DIR)/hzd_ln_eX_plus1.o    \
    $(COV_DIR)/hzd_ln_eeX_min1.o    \
    $(COV_DIR)/hzd_ln_1min_enegX.o  \
    $(COV_DIR)/hzd_Error.o

# ---- coverage binaries ---------------------------------------------

$(COV_DIR)/test_math_helpers: $(COV_DIR)/test_globals.o \
                               $(COV_COMMON_MATH) \
                               $(COV_DIR)/test_math_helpers.o
	$(CC) $(COV_CFLAGS) -o $@ $^ -lm

$(COV_DIR)/test_shaping_functions: $(COV_DIR)/test_globals.o \
                                    $(COV_COMMON_MATH) \
                                    $(COV_COMMON_SHAPE) \
                                    $(COV_DIR)/test_shaping_functions.o
	$(CC) $(COV_CFLAGS) -o $@ $^ -lm

$(COV_DIR)/test_obs_layout: $(COV_DIR)/test_obs_layout.o
	$(CC) $(COV_CFLAGS) -o $@ $^ -lm

COV_BINS = \
    $(COV_DIR)/test_math_helpers    \
    $(COV_DIR)/test_shaping_functions \
    $(COV_DIR)/test_obs_layout

# ---- aggregate target ----------------------------------------------

coverage: $(COV_BINS)
	@echo ""
	@echo "========================================"
	@echo "Running tests for coverage measurement"
	@echo "========================================"
	@FAILURES=0; \
	for bin in $(COV_BINS); do \
		echo "  · $$bin"; \
		./$$bin 2>/dev/null || FAILURES=$$((FAILURES + 1)); \
	done; \
	if [ $$FAILURES -gt 0 ]; then \
		echo "WARNING: $$FAILURES test binary/binaries failed"; \
	fi
	@echo ""
	@echo "Collecting coverage data..."
	lcov --capture \
	     --directory $(COV_DIR) \
	     --base-directory $(CURDIR) \
	     --output-file coverage.info \
	     --rc geninfo_unexecuted_blocks=1 2>/dev/null || \
	lcov --capture \
	     --directory $(COV_DIR) \
	     --base-directory $(CURDIR) \
	     --output-file coverage.info
	@# Remove test scaffolding and system headers — keep only src/common/
	lcov --remove coverage.info \
	     '*/tests/unit/test_*' \
	     '/usr/*' \
	     --output-file coverage.info \
	     --ignore-errors unused
	genhtml coverage.info \
	        --output-directory coverage_html \
	        --title "HAZARD Unit Test Coverage" \
	        --legend \
	        --quiet
	@echo ""
	@echo "========================================"
	@lcov --summary coverage.info 2>&1 | grep -E "lines|functions|branches" || true
	@echo "========================================"
	@echo "HTML report: coverage_html/index.html"

clean_coverage:
	rm -rf $(COV_DIR) coverage.info coverage_html

.PHONY: all check asan coverage clean_coverage clean
