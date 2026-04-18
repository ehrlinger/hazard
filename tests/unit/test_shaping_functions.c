/*
 * tests/unit/test_shaping_functions.c
 *
 * Unit tests for hzd_ln_G1_and_SG1() — the early-phase shaping
 * functions that implement the Turner et al. (1982) bounded survival
 * model.
 *
 * The function reads from globals:
 *   HazG1.T     — time point
 *   HazG1.lnG1  — output: log of cumulative hazard shaping function
 *   HazG1.lnSG1 — output: log of hazard shaping function
 *   Early.*     — early phase parameters (delta0, g1flag, rho, nu, m)
 *   LnLim.*     — log-domain thresholds
 *   Common.errorno / errtrap — error state (longjmp-based)
 *
 * IMPORTANT: hzd_Error() calls longjmp(Common.errtrap, ...) on any
 * arithmetic error.  Every call to hzd_ln_G1_and_SG1() must be
 * wrapped in setjmp(Common.errtrap).  If the longjmp fires, the
 * result is checked against the expected error behaviour.
 *
 * Reference values for Case 1 (Negative Generic, delta=0):
 *   Derived analytically for AVC example parameters:
 *     rho=0.1512, nu=1.4387, m=1 (fixed), t=0.5
 *   All values cross-verified against the C source logic.
 *
 * Reference values for Case 2 (Limiting, M=0, delta=0):
 *   rho=1, nu=1, t=2 → exact: lnG1 = -0.5, lnSG1 = log(0.5) - log(2)
 */

#include "test_harness.h"

#include <setjmp.h>
#include <math.h>

#include "structures.h"
#include "common.h"
#include "hzd_ln_G1_and_SG1.h"
#include "hzd_set_rho.h"

extern void hazard_test_init(void);

/* ------------------------------------------------------------------
 * Helpers to set up Early phase parameters for each G1 case.
 * ------------------------------------------------------------------ */

/* Case 1: M > 0, NU > 0 (Negative Generic Model)
 *   G1 = (1 + (B/rho)^(-1/nu))^(-1/m)
 * Using AVC example final estimates (no delta). */
static void setup_case1_avc(void)
{
    Common.errorno = 0;
    Early.g1flag = 1;
    Early.delta0 = 1;       /* TRUE: delta=0 → B(t)=t */
    Early.deltaN = 0;
    Early.delta  = 0.0;
    Early.rho    = 0.1511975;  /* AVC final THALF (rho=THALF when delta=0, nu=1/m) */
    Early.nu     = 1.438651;
    Early.m      = 1.0;
    /* No need for muE in shaping function computation */
}

/* Case 2: M = 0, NU > 0 (Limiting Exponential)
 *   G1 = exp(-(B/rho)^(-1/nu))
 * Chosen for analytical tractability: rho=1, nu=1, t=2.
 *   B(t) = t = 2; B/rho = 2; lnX = (-1/nu)*log(2) = -log(2)
 *   lnG1  = -exp(-log(2)) = -0.5
 *   lnSG1 = lnG1 + (-1/nu - 1)*log(2) + 0 - log(nu*rho)
 *          = -0.5 + (-2)*log(2) - 0
 *          = -0.5 - 2*log(2) ≈ -1.8863 */
static void setup_case2_analytic(void)
{
    Common.errorno = 0;
    Early.g1flag = 2;
    Early.delta0 = 1;
    Early.deltaN = 0;
    Early.delta  = 0.0;
    Early.rho    = 1.0;
    Early.nu     = 1.0;
    Early.m      = 0.0;   /* unused for case 2 */
}

/* Case 3: M < 0, NU > 0 (Positive Generic Model) */
static void setup_case3(void)
{
    Common.errorno = 0;
    Early.g1flag = 3;
    Early.delta0 = 1;
    Early.deltaN = 0;
    Early.delta  = 0.0;
    Early.rho    = 1.0;
    Early.nu     = 1.0;
    Early.m      = -1.0;
}

/* Case 4: M < 0, NU = 0 (Positive Limiting) */
static void setup_case4(void)
{
    Common.errorno = 0;
    Early.g1flag = 4;
    Early.delta0 = 1;
    Early.deltaN = 0;
    Early.delta  = 0.0;
    Early.rho    = 1.0;
    Early.nu     = 0.0;   /* zero → limiting case */
    Early.m      = -1.0;
}

/* Case 5: M > 0, NU < 0 (Mixed Generic) */
static void setup_case5(void)
{
    Common.errorno = 0;
    Early.g1flag = 5;
    Early.delta0 = 1;
    Early.deltaN = 0;
    Early.delta  = 0.0;
    Early.rho    = 1.0;
    Early.nu     = -1.0;
    Early.m      = 1.0;
}

/* Case 6: M = 0, NU < 0 (Limiting Case 3) */
static void setup_case6(void)
{
    Common.errorno = 0;
    Early.g1flag = 6;
    Early.delta0 = 1;
    Early.deltaN = 0;
    Early.delta  = 0.0;
    Early.rho    = 1.0;
    Early.nu     = -1.0;
    Early.m      = 0.0;
}

/* ------------------------------------------------------------------
 * Case 1: Negative Generic — AVC example parameters
 * ------------------------------------------------------------------ */

/* lnG1 must be negative (G1 is a cumulative probability ≤ 1). */
static void test_case1_lnG1_is_negative(void)
{
    setup_case1_avc();
    HazG1.T = 0.5;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_LT_DOUBLE(HazG1.lnG1, 0.0);
    } else {
        /* Unexpected error */
        ASSERT_TRUE(0);
    }
}

/* lnSG1 is the log hazard function — must be finite. */
static void test_case1_lnSG1_finite(void)
{
    setup_case1_avc();
    HazG1.T = 0.5;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_FINITE(HazG1.lnSG1);
    } else {
        ASSERT_TRUE(0);
    }
}

/* Both outputs must match Python reference (computed from C formula):
 *   rho=0.1511975, nu=1.438651, m=1, t=0.5, delta=0:
 *   Bt = 0.5
 *   termNu = log(Bt/rho) = log(0.5/0.1511975) = 1.19650
 *   mInvNu = -1/1.438651 = -0.69511
 *   lnX    = mInvNu * termNu                   = -0.83153
 *   termM  = log(e^lnX + 1) = log(1.43571)     = 0.36196
 *   lnG1   = -1 * termM                        = -0.36196
 *   lnSG1  = (-1.69511)*1.19650 + (-2)*0.36196
 *              + 0 - log(1*1.438651*0.1511975)
 *           = -2.02826 - 0.72392 + 1.52636     = -1.22582     */
static void test_case1_numerical_lnG1(void)
{
    setup_case1_avc();
    HazG1.T = 0.5;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_EQ_DOUBLE(HazG1.lnG1, -0.36196, 2e-3);
    } else {
        ASSERT_TRUE(0);
    }
}

static void test_case1_numerical_lnSG1(void)
{
    setup_case1_avc();
    HazG1.T = 0.5;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_EQ_DOUBLE(HazG1.lnSG1, -1.22582, 2e-3);
    } else {
        ASSERT_TRUE(0);
    }
}

/* G1 is monotone increasing in T: lnG1(t1) < lnG1(t2) for t1 < t2. */
static void test_case1_monotone_in_time(void)
{
    double g1_t1, g1_t2;

    setup_case1_avc();
    HazG1.T = 0.1;
    if (setjmp(Common.errtrap) != 0) { ASSERT_TRUE(0); return; }
    hzd_ln_G1_and_SG1();
    g1_t1 = HazG1.lnG1;

    setup_case1_avc();
    HazG1.T = 1.0;
    if (setjmp(Common.errtrap) != 0) { ASSERT_TRUE(0); return; }
    hzd_ln_G1_and_SG1();
    g1_t2 = HazG1.lnG1;

    ASSERT_LT_DOUBLE(g1_t1, g1_t2);
}

/* G1 approaches 1 (lnG1 → 0) as T → ∞ for Case 1. */
static void test_case1_approaches_one_at_large_t(void)
{
    setup_case1_avc();
    HazG1.T = 1000.0;  /* very large */
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        /* lnG1 should be close to 0 from below */
        ASSERT_LT_DOUBLE(HazG1.lnG1, 0.0);
        ASSERT_LT_DOUBLE(-0.01, HazG1.lnG1);  /* lnG1 > -0.01 */
    } else {
        ASSERT_TRUE(0);
    }
}

/* T = 0 must trigger an error (HazG1.T <= 0 check in C code). */
static void test_case1_zero_time_triggers_error(void)
{
    setup_case1_avc();
    HazG1.T = 0.0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_TRUE(0);   /* should not reach here */
    } else {
        ASSERT_TRUE(Common.errorno != 0);
    }
}

/* ------------------------------------------------------------------
 * Case 2: Limiting (M=0, NU>0) — exact analytic values
 * ------------------------------------------------------------------ */

/* lnG1 = -exp((-1/nu)*log(B/rho))
 *       = -exp(-log(2)) = -0.5   for rho=1, nu=1, T=2 */
static void test_case2_exact_lnG1(void)
{
    setup_case2_analytic();
    HazG1.T = 2.0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_EQ_DOUBLE(HazG1.lnG1, -0.5, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* lnSG1 = lnG1 + (mInvNu-1)*termNu + dBt - log(nu*rho)
 *        = -0.5 + (-2)*log(2) + 0 - 0
 *        = -0.5 - 2*log(2) */
static void test_case2_exact_lnSG1(void)
{
    setup_case2_analytic();
    HazG1.T = 2.0;
    double expected = -0.5 - 2.0 * log(2.0);
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_EQ_DOUBLE(HazG1.lnSG1, expected, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* ------------------------------------------------------------------
 * Case 3: Positive Generic — basic validity checks
 * ------------------------------------------------------------------ */

static void test_case3_valid_output(void)
{
    setup_case3();
    HazG1.T = 0.5;
    Common.errorno = 0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_FINITE(HazG1.lnG1);
        ASSERT_FINITE(HazG1.lnSG1);
    } else {
        ASSERT_TRUE(0);
    }
}

/* For Positive Generic, G1 rises from 0 to 1 but the cumulative hazard
 * is bounded above — lnG1 must remain < 0. */
static void test_case3_lnG1_negative(void)
{
    setup_case3();
    HazG1.T = 1.0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_LT_DOUBLE(HazG1.lnG1, 0.0);
    } else {
        ASSERT_TRUE(0);
    }
}

/* ------------------------------------------------------------------
 * Case 4: Positive Limiting (M<0, NU=0) — basic validity
 * ------------------------------------------------------------------ */

static void test_case4_valid_output(void)
{
    setup_case4();
    HazG1.T = 1.0;
    Common.errorno = 0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_FINITE(HazG1.lnG1);
        ASSERT_FINITE(HazG1.lnSG1);
    } else {
        ASSERT_TRUE(0);
    }
}

/* ------------------------------------------------------------------
 * Case 5: Mixed Generic (M>0, NU<0) — basic validity
 * ------------------------------------------------------------------ */

static void test_case5_valid_output(void)
{
    setup_case5();
    HazG1.T = 0.5;
    Common.errorno = 0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_FINITE(HazG1.lnG1);
        ASSERT_FINITE(HazG1.lnSG1);
    } else {
        ASSERT_TRUE(0);
    }
}

/* ------------------------------------------------------------------
 * Case 6: Limiting Case 3 (M=0, NU<0) — basic validity
 * ------------------------------------------------------------------ */

static void test_case6_valid_output(void)
{
    setup_case6();
    HazG1.T = 0.5;
    Common.errorno = 0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_FINITE(HazG1.lnG1);
        ASSERT_FINITE(HazG1.lnSG1);
    } else {
        ASSERT_TRUE(0);
    }
}

/* ------------------------------------------------------------------
 * Invalid g1flag (boundary check in source: must be 1–6)
 * ------------------------------------------------------------------ */

static void test_invalid_g1flag_triggers_error(void)
{
    setup_case1_avc();
    Early.g1flag = 0;   /* invalid */
    HazG1.T = 1.0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_TRUE(0);  /* should not reach here */
    } else {
        ASSERT_TRUE(Common.errorno != 0);
    }
}

/* hzd_Error() is a macro that threads __FILE__/__LINE__/__func__ through
   to hzd_Error_at().  When a shaping routine raises, the Common struct
   must hold the origin of the failure so the top-level catch handler
   in hazard.c can print it. */
static void test_error_populates_origin_context(void)
{
    setup_case1_avc();
    Early.g1flag = 0;
    HazG1.T = 1.0;
    Common.errfile = NULL;
    Common.errfunc = NULL;
    Common.errline = 0;

    if (setjmp(Common.errtrap) == 0) {
        hzd_ln_G1_and_SG1();
        ASSERT_TRUE(0);
    } else {
        /* errflg is wrapped in "(...)" by hzd_Error_at. */
        ASSERT_TRUE(Common.errflg[0] == '(');
        ASSERT_TRUE(Common.errfile != NULL);
        ASSERT_TRUE(Common.errfunc != NULL);
        ASSERT_TRUE(Common.errline > 0);
    }
}

/* ------------------------------------------------------------------
 * main
 * ------------------------------------------------------------------ */

int main(void)
{
    hazard_test_init();

    TEST_SUITE("Case 1 — Negative Generic (M>0, NU>0, delta=0) [AVC params]");
    RUN_TEST(test_case1_lnG1_is_negative);
    RUN_TEST(test_case1_lnSG1_finite);
    RUN_TEST(test_case1_numerical_lnG1);
    RUN_TEST(test_case1_numerical_lnSG1);
    RUN_TEST(test_case1_monotone_in_time);
    RUN_TEST(test_case1_approaches_one_at_large_t);
    RUN_TEST(test_case1_zero_time_triggers_error);

    TEST_SUITE("Case 2 — Limiting (M=0, NU>0, delta=0) [exact analytic]");
    RUN_TEST(test_case2_exact_lnG1);
    RUN_TEST(test_case2_exact_lnSG1);

    TEST_SUITE("Case 3 — Positive Generic (M<0, NU>0) [validity]");
    RUN_TEST(test_case3_valid_output);
    RUN_TEST(test_case3_lnG1_negative);

    TEST_SUITE("Case 4 — Positive Limiting (M<0, NU=0) [validity]");
    RUN_TEST(test_case4_valid_output);

    TEST_SUITE("Case 5 — Mixed Generic (M>0, NU<0) [validity]");
    RUN_TEST(test_case5_valid_output);

    TEST_SUITE("Case 6 — Limiting Case 3 (M=0, NU<0) [validity]");
    RUN_TEST(test_case6_valid_output);

    TEST_SUITE("Error handling");
    RUN_TEST(test_invalid_g1flag_triggers_error);
    RUN_TEST(test_error_populates_origin_context);

    print_test_results();
    return test_exit_code();
}
