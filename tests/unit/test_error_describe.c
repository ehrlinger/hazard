/*
 * tests/unit/test_error_describe.c
 *
 * Covers hzd_error_describe() — the lookup table that maps legacy
 * cryptic hzd_Error() codes (DLG1935, DTRSFM1150, HZPM, ...) to short
 * human descriptions.
 *
 * Zero global-state dependency.
 */

#include <string.h>

#include "hzd_error_describe.h"
#include "test_harness.h"

/* ------------------------------------------------------------------ */
/* Known codes from each legacy category                              */
/* ------------------------------------------------------------------ */

static void test_dlg1_codes(void)
{
    ASSERT_TRUE(hzd_error_describe("DLG1900") != NULL);
    ASSERT_TRUE(hzd_error_describe("DLG1910") != NULL);
    ASSERT_TRUE(hzd_error_describe("DLG1935") != NULL);
    ASSERT_TRUE(hzd_error_describe("DLG1950") != NULL);
    /* g1flag message should reference the valid range */
    ASSERT_TRUE(strstr(hzd_error_describe("DLG1935"), "1..6") != NULL);
}

static void test_dlg3_codes(void)
{
    ASSERT_TRUE(hzd_error_describe("DLG3900") != NULL);
    ASSERT_TRUE(hzd_error_describe("DLG3910") != NULL);
    ASSERT_TRUE(hzd_error_describe("DLG3930") != NULL);
}

static void test_dtrsfm_codes(void)
{
    ASSERT_TRUE(hzd_error_describe("DTRSFM970")  != NULL);
    ASSERT_TRUE(hzd_error_describe("DTRSFM1150") != NULL);
    ASSERT_TRUE(hzd_error_describe("DTRSFM1220") != NULL);
}

static void test_hzpm_code(void)
{
    const char *d = hzd_error_describe("HZPM");
    ASSERT_TRUE(d != NULL);
    ASSERT_TRUE(strstr(d, "hazpred") != NULL);
}

static void test_hzpo_codes(void)
{
    ASSERT_TRUE(hzd_error_describe("HZPO900")  != NULL);
    ASSERT_TRUE(hzd_error_describe("HZPO1000") != NULL);
}

static void test_misc_codes(void)
{
    ASSERT_TRUE(hzd_error_describe("DLEM1D50")  != NULL);
    ASSERT_TRUE(hzd_error_describe("DG1RHO960") != NULL);
}

/* ------------------------------------------------------------------ */
/* Unknown / edge cases                                                */
/* ------------------------------------------------------------------ */

static void test_unknown_code_returns_null(void)
{
    ASSERT_TRUE(hzd_error_describe("ZZZ9999") == NULL);
    ASSERT_TRUE(hzd_error_describe("") == NULL);
}

static void test_null_input_returns_null(void)
{
    ASSERT_TRUE(hzd_error_describe(NULL) == NULL);
}

/* "HZPM " with a trailing space appears in src/libhazp/hzpm.c and must
   resolve to the same description as "HZPM". */
static void test_trailing_whitespace_matches(void)
{
    const char *a = hzd_error_describe("HZPM");
    const char *b = hzd_error_describe("HZPM ");
    const char *c = hzd_error_describe("HZPM\t");
    ASSERT_TRUE(a != NULL);
    ASSERT_TRUE(b != NULL);
    ASSERT_TRUE(c != NULL);
    ASSERT_TRUE(a == b);  /* same table entry pointer */
    ASSERT_TRUE(a == c);
}

/* A prefix must not accidentally match a longer unrelated code: HZPM
   should not match something like HZPMX if a caller ever introduces one. */
static void test_prefix_does_not_overmatch(void)
{
    ASSERT_TRUE(hzd_error_describe("HZPMX") == NULL);
    ASSERT_TRUE(hzd_error_describe("DLG1900EXTRA") == NULL);
}

/* ------------------------------------------------------------------ */

int main(void)
{
    TEST_SUITE("hzd_error_describe — known codes");
    RUN_TEST(test_dlg1_codes);
    RUN_TEST(test_dlg3_codes);
    RUN_TEST(test_dtrsfm_codes);
    RUN_TEST(test_hzpm_code);
    RUN_TEST(test_hzpo_codes);
    RUN_TEST(test_misc_codes);

    TEST_SUITE("hzd_error_describe — edge cases");
    RUN_TEST(test_unknown_code_returns_null);
    RUN_TEST(test_null_input_returns_null);
    RUN_TEST(test_trailing_whitespace_matches);
    RUN_TEST(test_prefix_does_not_overmatch);

    print_test_results();
    return test_exit_code();
}
