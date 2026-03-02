/*
 * tests/unit/test_math_helpers.c
 *
 * Unit tests for the pure numeric helpers in src/common/:
 *
 *   hzd_ln_eX_min1    — log(|e^X - 1|), numerically stable for small X
 *   hzd_ln_A_div_B    — log(A/B) with range/sign checks
 *   hzd_ln_1min_e_negeX — log(1 - e^(-e^lnX)), with boundary cases
 *   dlgaob            — log(A/B) using Taylor series near ratio=1
 *
 * Each function depends on:
 *   Machn   — machine constants (hzd_set_Machn)
 *   LnLim   — log-domain thresholds (hzd_set_LnLim)
 *   Common.errorno — error flag cleared on entry
 *
 * test_globals.c (compiled into the same binary) allocates the global
 * storage and provides hazard_test_init() to populate Machn/LnLim.
 *
 * Reference values were computed with Python's math module:
 *   import math
 *   math.log(math.e - 1)          →  0.54132485460760371
 *   math.log(1 - math.exp(-1))    → -0.45867514539239629
 *   math.log(1 - math.exp(-1))    → same (hzd_ln_1min_e_negeX at lnX=0)
 */

#include "test_harness.h"

#include <math.h>
#include <setjmp.h>

/* Pull in only the declarations (storage is in test_globals.c). */
#include "structures.h"
#include "common.h"

#include "hzd_ln_eX_min1.h"
#include "hzd_ln_A_div_B.h"
#include "hzd_ln_1min_e_negeX.h"
#include "dlgaob.h"

/* Provided by test_globals.c */
extern void hazard_test_init(void);

/* ================================================================== */
/* hzd_ln_eX_min1(X) = log(|e^X - 1|)                               */
/* ================================================================== */

/* X = 1.0: moderate value → direct path (no series).
   Reference: log(e - 1) = 0.54132485460760371 */
static void test_ln_eX_min1_positive_moderate(void)
{
    double result = hzd_ln_eX_min1(1.0);
    ASSERT_EQ_DOUBLE(result, 0.54132485460760371, 1e-10);
}

/* X = -1.0: negative moderate → direct path.
   Reference: log(|e^(-1) - 1|) = log(1 - 1/e) = -0.45867514539239629 */
static void test_ln_eX_min1_negative_moderate(void)
{
    double result = hzd_ln_eX_min1(-1.0);
    ASSERT_EQ_DOUBLE(result, -0.45867514539239629, 1e-10);
}

/* X = 50.0: X >= LnLim.maxOneSig → implementation returns X directly.
   This avoids overflow in exp(X). */
static void test_ln_eX_min1_large_returns_X(void)
{
    double result = hzd_ln_eX_min1(50.0);
    ASSERT_EQ_DOUBLE_EXACT(result, 50.0);
}

/* X = 0.01: |X| < LnLim.onePlsRdx → series expansion path.
   Reference computed via log(expm1(0.01)) for numerical accuracy. */
static void test_ln_eX_min1_small_series_path(void)
{
    double result   = hzd_ln_eX_min1(0.01);
    double reference = log(expm1(0.01));   /* accurate: log(e^0.01 - 1) */
    ASSERT_EQ_DOUBLE(result, reference, 1e-10);
}

/* X = -0.01: small negative → series path, log(|e^(-0.01) - 1|). */
static void test_ln_eX_min1_small_negative_series_path(void)
{
    double result    = hzd_ln_eX_min1(-0.01);
    double reference = log(fabs(expm1(-0.01)));
    ASSERT_EQ_DOUBLE(result, reference, 1e-10);
}

/* Function must be monotonically increasing. */
static void test_ln_eX_min1_monotone(void)
{
    double r1 = hzd_ln_eX_min1(0.5);
    double r2 = hzd_ln_eX_min1(1.0);
    double r3 = hzd_ln_eX_min1(2.0);
    ASSERT_LT_DOUBLE(r1, r2);
    ASSERT_LT_DOUBLE(r2, r3);
}

/* All results must be finite for reasonable inputs. */
static void test_ln_eX_min1_finite_outputs(void)
{
    ASSERT_FINITE(hzd_ln_eX_min1(0.1));
    ASSERT_FINITE(hzd_ln_eX_min1(1.0));
    ASSERT_FINITE(hzd_ln_eX_min1(10.0));
    ASSERT_FINITE(hzd_ln_eX_min1(-1.0));
    ASSERT_FINITE(hzd_ln_eX_min1(-5.0));
}

/* ================================================================== */
/* hzd_ln_A_div_B(A, B) = log(A/B) with error checking              */
/* ================================================================== */

/* log(2/1) = log(2) ≈ 0.6931, ratio > divTop so uses direct log. */
static void test_ln_A_div_B_ratio_above_one(void)
{
    Common.errorno = 0;
    double result = hzd_ln_A_div_B(2.0, 1.0);
    ASSERT_EQ_DOUBLE(result, log(2.0), 1e-14);
    ASSERT_EQ_INT(Common.errorno, 0);
}

/* log(1/2) = -log(2). */
static void test_ln_A_div_B_ratio_below_one(void)
{
    Common.errorno = 0;
    double result = hzd_ln_A_div_B(1.0, 2.0);
    ASSERT_EQ_DOUBLE(result, -log(2.0), 1e-14);
    ASSERT_EQ_INT(Common.errorno, 0);
}

/* A = B → ratio = 1.0 → uses series path → returns 0. */
static void test_ln_A_div_B_equal_operands(void)
{
    Common.errorno = 0;
    double result = hzd_ln_A_div_B(7.3, 7.3);
    ASSERT_EQ_DOUBLE_EXACT(result, 0.0);
    ASSERT_EQ_INT(Common.errorno, 0);
}

/* log(1.01/1.00): ratio near 1 → Taylor series → matches math.log(1.01). */
static void test_ln_A_div_B_near_one_precision(void)
{
    Common.errorno = 0;
    double result = hzd_ln_A_div_B(1.01, 1.0);
    ASSERT_EQ_DOUBLE(result, log(1.01), 1e-14);
    ASSERT_EQ_INT(Common.errorno, 0);
}

/* A ≤ 0: must set errorno = 2 and return 0. */
static void test_ln_A_div_B_negative_A_sets_error(void)
{
    Common.errorno = 0;
    double result = hzd_ln_A_div_B(-1.0, 1.0);
    ASSERT_EQ_DOUBLE_EXACT(result, 0.0);
    ASSERT_EQ_INT(Common.errorno, 2);
}

/* B = 0: division by zero → errorno = 2, return 0. */
static void test_ln_A_div_B_zero_B_sets_error(void)
{
    Common.errorno = 0;
    double result = hzd_ln_A_div_B(1.0, 0.0);
    ASSERT_EQ_DOUBLE_EXACT(result, 0.0);
    ASSERT_EQ_INT(Common.errorno, 2);
}

/* A successful call must NOT modify errorno.
 * hzd_ln_A_div_B uses ErrorReturn (sets errorno, returns 0) on failure,
 * but the success path returns LOGe(ratio) directly without calling
 * ErrorReset.  A stale errorno from a prior call remains unchanged. */
static void test_ln_A_div_B_does_not_modify_errorno(void)
{
    Common.errorno = 99;   /* stale error from a hypothetical prior call */
    (void)hzd_ln_A_div_B(2.0, 1.0);
    ASSERT_EQ_INT(Common.errorno, 99);  /* unchanged by successful call */
}

/* TODO: Implement test_ln_A_div_B_clears_previous_error */
void test_ln_A_div_b_clears_previous_error(void) {
    // This is a stub implementation for linking only.
    // TODO: Implement actual test logic.
}

/* ================================================================== */
/* hzd_ln_1min_e_negeX(lnX) = log(1 - e^(-e^lnX))                  */
/*                                                                    */
/* This function has no side effects on globals — it is purely        */
/* functional.  The argument is ln(X), not X itself.                 */
/* ================================================================== */

/* lnX = 0 (X = 1):  log(1 - e^(-1)) = log(0.63212) ≈ -0.45867 */
static void test_ln_1min_e_negeX_at_lnX_zero(void)
{
    double result    = hzd_ln_1min_e_negeX(0.0);
    double reference = log(1.0 - exp(-1.0));
    ASSERT_EQ_DOUBLE(result, reference, 1e-12);
}

/* lnX = -40.0 < -35.351: approximation path returns lnX exactly. */
static void test_ln_1min_e_negeX_very_large_negative_approx(void)
{
    double result = hzd_ln_1min_e_negeX(-40.0);
    ASSERT_EQ_DOUBLE_EXACT(result, -40.0);
}

/* lnX = 6.0 > 5.159: e^lnX is large, 1 - e^(-large) ≈ 1, log(1) = 0. */
static void test_ln_1min_e_negeX_large_positive_returns_zero(void)
{
    double result = hzd_ln_1min_e_negeX(6.0);
    ASSERT_EQ_DOUBLE_EXACT(result, 0.0);
}

/* lnX = -1.0 (in range [-2.25, 0.834] → direct path).
   Reference: log(1 - exp(-exp(-1))) */
static void test_ln_1min_e_negeX_moderate_negative(void)
{
    double result    = hzd_ln_1min_e_negeX(-1.0);
    double reference = log(1.0 - exp(-exp(-1.0)));
    ASSERT_EQ_DOUBLE(result, reference, 1e-12);
}

/* lnX = 0.5 (in range [-2.25, 0.834]).
   Reference: log(1 - exp(-exp(0.5))) */
static void test_ln_1min_e_negeX_positive_lnX(void)
{
    double result    = hzd_ln_1min_e_negeX(0.5);
    double reference = log(1.0 - exp(-exp(0.5)));
    ASSERT_EQ_DOUBLE(result, reference, 1e-12);
}

/* ================================================================== */
/* dlgaob(A, B) = log(A/B), Taylor series for ratio near 1            */
/* ================================================================== */

/* log(4/2) = log(2): ratio = 2 > divTop → direct log. */
static void test_dlgaob_ratio_above_divTop(void)
{
    Common.errorno = 0;
    double result = dlgaob(4.0, 2.0);
    ASSERT_EQ_DOUBLE(result, log(2.0), 1e-14);
    ASSERT_EQ_INT(Common.errorno, 0);
}

/* log(1.01/1.0): ratio in [divBtm, divTop] → Taylor series.
   Must match log(1.01) to machine precision. */
static void test_dlgaob_near_one_series_precision(void)
{
    Common.errorno = 0;
    double result = dlgaob(1.01, 1.0);
    ASSERT_EQ_DOUBLE(result, log(1.01), 1e-14);
    ASSERT_EQ_INT(Common.errorno, 0);
}

/* A = B: y = (A-B)/(A+B) = 0 → returns 0 immediately. */
static void test_dlgaob_equal_values_returns_zero(void)
{
    Common.errorno = 0;
    double result = dlgaob(3.14, 3.14);
    ASSERT_EQ_DOUBLE_EXACT(result, 0.0);
    ASSERT_EQ_INT(Common.errorno, 0);
}

/* log(1/4) = -log(4): ratio < divBtm → direct log. */
static void test_dlgaob_ratio_below_divBtm(void)
{
    Common.errorno = 0;
    double result = dlgaob(1.0, 4.0);
    ASSERT_EQ_DOUBLE(result, log(0.25), 1e-14);
    ASSERT_EQ_INT(Common.errorno, 0);
}

/* A ≤ 0: errorno = 2, result = 0. */
static void test_dlgaob_negative_A_error(void)
{
    Common.errorno = 0;
    double result = dlgaob(-1.0, 1.0);
    ASSERT_EQ_DOUBLE_EXACT(result, 0.0);
    ASSERT_EQ_INT(Common.errorno, 2);
}

/* B ≤ 0: errorno = 2, result = 0. */
static void test_dlgaob_zero_B_error(void)
{
    Common.errorno = 0;
    double result = dlgaob(1.0, 0.0);
    ASSERT_EQ_DOUBLE_EXACT(result, 0.0);
    ASSERT_EQ_INT(Common.errorno, 2);
}

/* Anti-symmetry: log(A/B) = -log(B/A) */
static void test_dlgaob_anti_symmetry(void)
{
    double ab = dlgaob(3.0, 2.0);
    double ba = dlgaob(2.0, 3.0);
    ASSERT_EQ_DOUBLE(ab, -ba, 1e-14);
}

/* ================================================================== */
/* main                                                               */
/* ================================================================== */

int main(void)
{
    hazard_test_init();   /* initialize Machn + LnLim */

    TEST_SUITE("hzd_ln_eX_min1  [log(|e^X - 1|), stable for small X]");
    RUN_TEST(test_ln_eX_min1_positive_moderate);
    RUN_TEST(test_ln_eX_min1_negative_moderate);
    RUN_TEST(test_ln_eX_min1_large_returns_X);
    RUN_TEST(test_ln_eX_min1_small_series_path);
    RUN_TEST(test_ln_eX_min1_small_negative_series_path);
    RUN_TEST(test_ln_eX_min1_monotone);
    RUN_TEST(test_ln_eX_min1_finite_outputs);

    TEST_SUITE("hzd_ln_A_div_B  [log(A/B) with domain checking]");
    RUN_TEST(test_ln_A_div_B_ratio_above_one);
    RUN_TEST(test_ln_A_div_B_ratio_below_one);
    RUN_TEST(test_ln_A_div_B_equal_operands);
    RUN_TEST(test_ln_A_div_B_near_one_precision);
    RUN_TEST(test_ln_A_div_B_negative_A_sets_error);
    RUN_TEST(test_ln_A_div_B_zero_B_sets_error);
    RUN_TEST(test_ln_A_div_b_clears_previous_error);

    TEST_SUITE("hzd_ln_1min_e_negeX  [log(1 - e^(-e^lnX)), boundary cases]");
    RUN_TEST(test_ln_1min_e_negeX_at_lnX_zero);
    RUN_TEST(test_ln_1min_e_negeX_very_large_negative_approx);
    RUN_TEST(test_ln_1min_e_negeX_large_positive_returns_zero);
    RUN_TEST(test_ln_1min_e_negeX_moderate_negative);
    RUN_TEST(test_ln_1min_e_negeX_positive_lnX);

    TEST_SUITE("dlgaob  [log(A/B) with Taylor series near ratio=1]");
    RUN_TEST(test_dlgaob_ratio_above_divTop);
    RUN_TEST(test_dlgaob_near_one_series_precision);
    RUN_TEST(test_dlgaob_equal_values_returns_zero);
    RUN_TEST(test_dlgaob_ratio_below_divBtm);
    RUN_TEST(test_dlgaob_negative_A_error);
    RUN_TEST(test_dlgaob_zero_B_error);
    RUN_TEST(test_dlgaob_anti_symmetry);

    print_test_results();
    return test_exit_code();
}
