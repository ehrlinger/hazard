/*
 * tests/unit/test_transforms.c
 *
 * Unit tests for the parameter ↔ theta transform functions:
 *   hzd_early_p2t  — early phase params → unconstrained theta
 *   hzd_early_t2p  — unconstrained theta → early phase TempE
 *   hzd_late_p2t   — late phase params → unconstrained theta
 *   hzd_late_t2p   — unconstrained theta → late phase TempL
 *
 * These are pure reparameterisation functions: p2t maps bounded params
 * onto the real line so the optimizer can move freely; t2p maps back.
 * They have no I/O, no file system access, and no random state — they
 * only touch the global structs Early/Late/TempE/TempL/Common.
 *
 * Test strategy
 * -------------
 * Each test function:
 *   1. Zeros all relevant globals via reset helpers.
 *   2. Sets up Common.status[], Common.theta[] / Early.* / Late.* as
 *      needed.
 *   3. Calls the function under test inside a setjmp block so that
 *      hzd_Error() longjmps are caught gracefully.
 *   4. Asserts on outputs (sTheta[] or TempE.* / TempL.*) or on the
 *      fact that an error was raised.
 *
 * Global storage is allocated by test_globals.c (STRUCT / HAZARD
 * macros).  Common.status and Common.theta must be pointed at local
 * arrays — they are pointers in the struct, not embedded arrays.
 */

#include <math.h>
#include <setjmp.h>
#include <string.h>

#include "test_harness.h"
#include "structures.h"
#include "common.h"
#include "hzd_early_p2t.h"
#include "hzd_early_t2p.h"
#include "hzd_late_p2t.h"
#include "hzd_late_t2p.h"

extern void hazard_test_init(void);

/* ------------------------------------------------------------------ */
/* Convenience: point Common.status / Common.theta at local storage   */
/* ------------------------------------------------------------------ */

/* 8 parameters (0–3 early, 4–7 late) — always enough for these tests */
#define NTHETA 8

static short int g_status[NTHETA];
static double    g_theta[NTHETA];

static void setup_globals(void)
{
    hazard_test_init();
    memset(g_status, 0, sizeof(g_status));
    memset(g_theta,  0, sizeof(g_theta));
    Common.status = g_status;
    Common.theta  = g_theta;
    Common.Ntheta = NTHETA;
    memset(&Early, 0, sizeof(Early));
    memset(&Late,  0, sizeof(Late));
    memset(&TempE, 0, sizeof(TempE));
    memset(&TempL, 0, sizeof(TempL));
    memset(Common.phase, 0, sizeof(Common.phase));
    Common.errorno = 0;
}

/* ================================================================== */
/* Group 1: hzd_early_p2t                                             */
/* ================================================================== */

/* status[0]=1, valid delta: sTheta[0] = log(-log(|delta|)) */
static void test_early_p2t_delta_valid(void)
{
    setup_globals();
    g_status[0] = 1;
    Early.delta  = 0.5;   /* |delta| = 0.5, < 1 → valid */

    double sTheta[NTHETA];
    memset(sTheta, 0, sizeof(sTheta));

    if (setjmp(Common.errtrap) == 0) {
        hzd_early_p2t(sTheta);
        /* sTheta[0] = log(-log(0.5)) = log(log(2)) */
        double expected = log(-log(0.5));
        ASSERT_EQ_DOUBLE(sTheta[0], expected, 1e-12);
    } else {
        ASSERT_TRUE(0);  /* unexpected error */
    }
}

/* status[0]=1, |delta| >= 1: hzd_Error raised */
static void test_early_p2t_delta_invalid(void)
{
    setup_globals();
    g_status[0] = 1;
    Early.delta  = 1.0;   /* |delta| = 1 → invalid */

    double sTheta[NTHETA];
    int got_error = 0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_early_p2t(sTheta);
    } else {
        got_error = 1;
    }
    ASSERT_TRUE(got_error);
}

/* status[1]=1, valid tHalf: sTheta[1] = log(tHalf) */
static void test_early_p2t_tHalf_valid(void)
{
    setup_globals();
    g_status[1] = 1;
    Early.tHalf  = 2.0;

    double sTheta[NTHETA];
    memset(sTheta, 0, sizeof(sTheta));
    if (setjmp(Common.errtrap) == 0) {
        hzd_early_p2t(sTheta);
        ASSERT_EQ_DOUBLE(sTheta[1], log(2.0), 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[1]=1, tHalf <= 0: hzd_Error raised */
static void test_early_p2t_tHalf_zero(void)
{
    setup_globals();
    g_status[1] = 1;
    Early.tHalf  = 0.0;

    double sTheta[NTHETA];
    int got_error = 0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_early_p2t(sTheta);
    } else {
        got_error = 1;
    }
    ASSERT_TRUE(got_error);
}

/* status[2]=1: sTheta[2] = log(|nu|) */
static void test_early_p2t_nu(void)
{
    setup_globals();
    g_status[2] = 1;
    Early.nu     = -3.0;   /* fabs → 3.0 */

    double sTheta[NTHETA];
    memset(sTheta, 0, sizeof(sTheta));
    if (setjmp(Common.errtrap) == 0) {
        hzd_early_p2t(sTheta);
        ASSERT_EQ_DOUBLE(sTheta[2], log(3.0), 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[3]=1: sTheta[3] = log(|m|) */
static void test_early_p2t_m(void)
{
    setup_globals();
    g_status[3] = 1;
    Early.m      = 4.0;

    double sTheta[NTHETA];
    memset(sTheta, 0, sizeof(sTheta));
    if (setjmp(Common.errtrap) == 0) {
        hzd_early_p2t(sTheta);
        ASSERT_EQ_DOUBLE(sTheta[3], log(4.0), 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status all 0: sTheta untouched */
static void test_early_p2t_no_active(void)
{
    setup_globals();
    /* all g_status[] = 0 */
    double sTheta[NTHETA];
    double sentinel = 999.0;
    for (int i = 0; i < NTHETA; i++) sTheta[i] = sentinel;

    if (setjmp(Common.errtrap) == 0) {
        hzd_early_p2t(sTheta);
        for (int i = 0; i < 4; i++)
            ASSERT_EQ_DOUBLE(sTheta[i], sentinel, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* ================================================================== */
/* Group 2: hzd_early_t2p                                             */
/* ================================================================== */

/* status[0]=1, normal theta[0]: TempE.delta = exp(-exp(theta[0])) */
static void test_early_t2p_delta_normal(void)
{
    setup_globals();
    g_status[0]  = 1;
    g_theta[0]   = 0.5;
    Early.deltaN = 0;

    if (setjmp(Common.errtrap) == 0) {
        hzd_early_t2p();
        double expected = exp(-exp(0.5));
        ASSERT_EQ_DOUBLE(TempE.delta, expected, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[0]=1, deltaN=1: result negated */
static void test_early_t2p_delta_negative(void)
{
    setup_globals();
    g_status[0]  = 1;
    g_theta[0]   = 0.5;
    Early.deltaN = 1;

    if (setjmp(Common.errtrap) == 0) {
        hzd_early_t2p();
        double expected = -exp(-exp(0.5));
        ASSERT_EQ_DOUBLE(TempE.delta, expected, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[0]=1, theta[0] < LnLim.absMin: TempE.delta = 1 (clamped) */
static void test_early_t2p_delta_clamp(void)
{
    setup_globals();
    g_status[0]  = 1;
    g_theta[0]   = LnLim.absMin - 1.0;  /* below minimum */
    Early.deltaN = 0;

    if (setjmp(Common.errtrap) == 0) {
        hzd_early_t2p();
        ASSERT_EQ_DOUBLE(TempE.delta, 1.0, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[0]=1, theta[0] >= log(absMax): error */
static void test_early_t2p_delta_overflow(void)
{
    setup_globals();
    g_status[0]  = 1;
    g_theta[0]   = log(LnLim.absMax) + 1.0;

    int got_error = 0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_early_t2p();
    } else {
        got_error = 1;
    }
    ASSERT_TRUE(got_error);
}

/* status[1]=1, valid theta[1]: TempE.tHalf = exp(theta[1]) */
static void test_early_t2p_tHalf_valid(void)
{
    setup_globals();
    g_status[1] = 1;
    g_theta[1]  = log(3.5);

    if (setjmp(Common.errtrap) == 0) {
        hzd_early_t2p();
        ASSERT_EQ_DOUBLE(TempE.tHalf, 3.5, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[2]=1, g1flag=1 (nu positive): TempE.nu = exp(theta[2]) */
static void test_early_t2p_nu_positive(void)
{
    setup_globals();
    g_status[2]  = 1;
    g_theta[2]   = log(2.0);
    Early.g1flag = 1;   /* flags < 5 → nu positive */

    if (setjmp(Common.errtrap) == 0) {
        hzd_early_t2p();
        ASSERT_EQ_DOUBLE(TempE.nu, 2.0, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[2]=1, g1flag=5 (nu negated): TempE.nu = -exp(theta[2]) */
static void test_early_t2p_nu_negative(void)
{
    setup_globals();
    g_status[2]  = 1;
    g_theta[2]   = log(2.0);
    Early.g1flag = 5;

    if (setjmp(Common.errtrap) == 0) {
        hzd_early_t2p();
        ASSERT_EQ_DOUBLE(TempE.nu, -2.0, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[3]=1, g1flag=3 (m negated): TempE.m = -exp(theta[3]) */
static void test_early_t2p_m_negated(void)
{
    setup_globals();
    g_status[3]  = 1;
    g_theta[3]   = log(1.5);
    Early.g1flag = 3;

    if (setjmp(Common.errtrap) == 0) {
        hzd_early_t2p();
        ASSERT_EQ_DOUBLE(TempE.m, -1.5, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* mNuOne=1, status[2]=1: TempE.m = 1/TempE.nu, g1flag=1 → positive */
static void test_early_t2p_mNuOne_from_nu(void)
{
    setup_globals();
    g_status[2]   = 1;
    g_theta[2]    = log(4.0);
    Early.g1flag  = 1;
    Early.mNuOne  = 1;

    if (setjmp(Common.errtrap) == 0) {
        hzd_early_t2p();
        /* nu = exp(log(4)) = 4; mNuOne: m = 1/nu = 0.25 */
        ASSERT_EQ_DOUBLE(TempE.nu, 4.0,  1e-12);
        ASSERT_EQ_DOUBLE(TempE.m,  0.25, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* ================================================================== */
/* Group 3: hzd_late_p2t                                              */
/* ================================================================== */

/* status[4]=1, valid tau: sTheta[4] = log(tau) */
static void test_late_p2t_tau_valid(void)
{
    setup_globals();
    g_status[4] = 1;
    Late.tau     = 5.0;

    double sTheta[NTHETA];
    memset(sTheta, 0, sizeof(sTheta));
    if (setjmp(Common.errtrap) == 0) {
        hzd_late_p2t(sTheta);
        ASSERT_EQ_DOUBLE(sTheta[4], log(5.0), 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[4]=1, tau <= 0: error */
static void test_late_p2t_tau_zero(void)
{
    setup_globals();
    g_status[4] = 1;
    Late.tau     = 0.0;

    double sTheta[NTHETA];
    int got_error = 0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_late_p2t(sTheta);
    } else {
        got_error = 1;
    }
    ASSERT_TRUE(got_error);
}

/* status[5]=1, g_two=1: sTheta[5] = log(gamma) */
static void test_late_p2t_gamma_g_two(void)
{
    setup_globals();
    g_status[5] = 1;
    Late.g_two   = 1;
    Late.gamma   = 3.0;

    double sTheta[NTHETA];
    memset(sTheta, 0, sizeof(sTheta));
    if (setjmp(Common.errtrap) == 0) {
        hzd_late_p2t(sTheta);
        ASSERT_EQ_DOUBLE(sTheta[5], log(3.0), 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[5]=1, g_two=0, status[7]=0, g3flag<3:
   ge_2 = gamma*eta - 2; sTheta[5] = log(ge_2) */
static void test_late_p2t_gamma_ge_parameterisation(void)
{
    setup_globals();
    g_status[5] = 1;
    Late.g_two   = 0;
    Late.g3flag  = 0;
    Late.gamma   = 4.0;
    Late.eta     = 1.5;   /* ge_2 = 4*1.5 - 2 = 4.0 */

    double sTheta[NTHETA];
    memset(sTheta, 0, sizeof(sTheta));
    if (setjmp(Common.errtrap) == 0) {
        hzd_late_p2t(sTheta);
        ASSERT_EQ_DOUBLE(sTheta[5], log(4.0), 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[5]=1, g_two=0, ge_2 <= 0: error */
static void test_late_p2t_gamma_ge_invalid(void)
{
    setup_globals();
    g_status[5] = 1;
    Late.g_two   = 0;
    Late.g3flag  = 0;
    Late.gamma   = 1.0;
    Late.eta     = 1.0;   /* ge_2 = 1*1 - 2 = -1 → error */

    double sTheta[NTHETA];
    int got_error = 0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_late_p2t(sTheta);
    } else {
        got_error = 1;
    }
    ASSERT_TRUE(got_error);
}

/* status[7]=1, g_two=1: sTheta[7] = log(eta) */
static void test_late_p2t_eta_g_two(void)
{
    setup_globals();
    g_status[7] = 1;
    Late.g_two   = 1;
    Late.eta     = 2.5;

    double sTheta[NTHETA];
    memset(sTheta, 0, sizeof(sTheta));
    if (setjmp(Common.errtrap) == 0) {
        hzd_late_p2t(sTheta);
        ASSERT_EQ_DOUBLE(sTheta[7], log(2.5), 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[6]=1, ga_two=1, g_two=0: sTheta[6] = log(alpha) */
static void test_late_p2t_alpha_ga_two(void)
{
    setup_globals();
    g_status[6] = 1;
    Late.ga_two  = 1;
    Late.g_two   = 0;
    Late.alpha   = 7.0;

    double sTheta[NTHETA];
    memset(sTheta, 0, sizeof(sTheta));
    if (setjmp(Common.errtrap) == 0) {
        hzd_late_p2t(sTheta);
        ASSERT_EQ_DOUBLE(sTheta[6], log(7.0), 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[6]=1, ga_two=0, g_two=0:
   ge_a_2 = (gamma*eta/alpha) - 2; sTheta[6] = log(ge_a_2) */
static void test_late_p2t_alpha_ge_a_parameterisation(void)
{
    setup_globals();
    g_status[6] = 1;
    Late.ga_two  = 0;
    Late.g_two   = 0;
    Late.g3flag  = 0;
    Late.gamma   = 6.0;
    Late.eta     = 2.0;
    Late.alpha   = 3.0;   /* ge_a_2 = (6*2/3) - 2 = 2.0 */

    double sTheta[NTHETA];
    memset(sTheta, 0, sizeof(sTheta));
    if (setjmp(Common.errtrap) == 0) {
        hzd_late_p2t(sTheta);
        ASSERT_EQ_DOUBLE(sTheta[6], log(2.0), 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* ================================================================== */
/* Group 4: hzd_late_t2p                                              */
/* ================================================================== */

/* status[4]=1, valid theta[4]: TempL.tau = exp(theta[4]) */
static void test_late_t2p_tau_valid(void)
{
    setup_globals();
    g_status[4] = 1;
    g_theta[4]  = log(6.0);

    if (setjmp(Common.errtrap) == 0) {
        hzd_late_t2p();
        ASSERT_EQ_DOUBLE(TempL.tau, 6.0, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[4]=1, theta[4] out of range: error */
static void test_late_t2p_tau_overflow(void)
{
    setup_globals();
    g_status[4] = 1;
    g_theta[4]  = LnLim.absMax + 1.0;

    int got_error = 0;
    if (setjmp(Common.errtrap) == 0) {
        hzd_late_t2p();
    } else {
        got_error = 1;
    }
    ASSERT_TRUE(got_error);
}

/* status[5]=1, g_two=1: TempL.gamma=exp(theta), TempL.eta=2/gamma */
static void test_late_t2p_gamma_g_two(void)
{
    setup_globals();
    g_status[5] = 1;
    g_theta[5]  = log(4.0);
    Late.g_two  = 1;

    if (setjmp(Common.errtrap) == 0) {
        hzd_late_t2p();
        ASSERT_EQ_DOUBLE(TempL.gamma, 4.0, 1e-12);
        ASSERT_EQ_DOUBLE(TempL.eta,   0.5, 1e-12);  /* 2/4 */
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[5]=1, g_two=0, status[7]=0, g3flag<3:
   TempL.gamma = (exp(theta[5]) + 2) / Late.eta */
static void test_late_t2p_gamma_ge_parameterisation(void)
{
    setup_globals();
    g_status[5] = 1;
    g_theta[5]  = log(2.0);   /* exp = 2; (2+2)/2 = 2 */
    Late.g_two  = 0;
    Late.g3flag = 0;
    Late.eta    = 2.0;

    if (setjmp(Common.errtrap) == 0) {
        hzd_late_t2p();
        ASSERT_EQ_DOUBLE(TempL.gamma, 2.0, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[7]=1, g_two=1: TempL.eta=exp(theta), TempL.gamma=2/eta */
static void test_late_t2p_eta_g_two(void)
{
    setup_globals();
    g_status[7] = 1;
    g_theta[7]  = log(5.0);
    Late.g_two  = 1;

    if (setjmp(Common.errtrap) == 0) {
        hzd_late_t2p();
        ASSERT_EQ_DOUBLE(TempL.eta,   5.0, 1e-12);
        ASSERT_EQ_DOUBLE(TempL.gamma, 0.4, 1e-12);  /* 2/5 */
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[7]=1, g_two=0: TempL.eta = (exp(theta[7]) + 2) / Late.gamma */
static void test_late_t2p_eta_ge_parameterisation(void)
{
    setup_globals();
    g_status[7] = 1;
    g_theta[7]  = log(3.0);   /* exp=3; (3+2)/5 = 1 */
    Late.g_two  = 0;
    Late.g3flag = 0;
    Late.gamma  = 5.0;

    if (setjmp(Common.errtrap) == 0) {
        hzd_late_t2p();
        ASSERT_EQ_DOUBLE(TempL.eta, 1.0, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[6]=1, ga_two=1, g_two=0: TempL.alpha = exp(theta[6]) */
static void test_late_t2p_alpha_ga_two(void)
{
    setup_globals();
    g_status[6] = 1;
    g_theta[6]  = log(9.0);
    Late.ga_two = 1;
    Late.g_two  = 0;
    Late.g3flag = 0;

    if (setjmp(Common.errtrap) == 0) {
        hzd_late_t2p();
        ASSERT_EQ_DOUBLE(TempL.alpha, 9.0, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[6]=0, ga_two=1: TempL.alpha = gamma*eta/2 */
static void test_late_t2p_alpha_ga_two_derived(void)
{
    setup_globals();
    /* status[6]=0 — alpha derived from gamma*eta */
    Late.ga_two  = 1;
    TempL.gamma  = 4.0;
    TempL.eta    = 3.0;

    if (setjmp(Common.errtrap) == 0) {
        hzd_late_t2p();
        /* alpha = 4*3/2 = 6 */
        ASSERT_EQ_DOUBLE(TempL.alpha, 6.0, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* status[6]=1, ga_two=0, g_two=0:
   TempL.alpha = gamma_eta / (exp(theta[6]) + 2) */
static void test_late_t2p_alpha_ge_parameterisation(void)
{
    setup_globals();
    g_status[6]  = 1;
    g_theta[6]   = log(2.0);   /* exp=2; denom=4 */
    Late.ga_two  = 0;
    Late.g_two   = 0;
    Late.g3flag  = 0;
    TempL.gamma  = 4.0;
    TempL.eta    = 3.0;   /* gamma_eta=12; alpha=12/4=3 */

    if (setjmp(Common.errtrap) == 0) {
        hzd_late_t2p();
        ASSERT_EQ_DOUBLE(TempL.alpha, 3.0, 1e-12);
    } else {
        ASSERT_TRUE(0);
    }
}

/* ================================================================== */
/* main                                                               */
/* ================================================================== */

int main(void)
{
    TEST_SUITE("hzd_early_p2t — params to theta");
    RUN_TEST(test_early_p2t_delta_valid);
    RUN_TEST(test_early_p2t_delta_invalid);
    RUN_TEST(test_early_p2t_tHalf_valid);
    RUN_TEST(test_early_p2t_tHalf_zero);
    RUN_TEST(test_early_p2t_nu);
    RUN_TEST(test_early_p2t_m);
    RUN_TEST(test_early_p2t_no_active);

    TEST_SUITE("hzd_early_t2p — theta to params");
    RUN_TEST(test_early_t2p_delta_normal);
    RUN_TEST(test_early_t2p_delta_negative);
    RUN_TEST(test_early_t2p_delta_clamp);
    RUN_TEST(test_early_t2p_delta_overflow);
    RUN_TEST(test_early_t2p_tHalf_valid);
    RUN_TEST(test_early_t2p_nu_positive);
    RUN_TEST(test_early_t2p_nu_negative);
    RUN_TEST(test_early_t2p_m_negated);
    RUN_TEST(test_early_t2p_mNuOne_from_nu);

    TEST_SUITE("hzd_late_p2t — params to theta");
    RUN_TEST(test_late_p2t_tau_valid);
    RUN_TEST(test_late_p2t_tau_zero);
    RUN_TEST(test_late_p2t_gamma_g_two);
    RUN_TEST(test_late_p2t_gamma_ge_parameterisation);
    RUN_TEST(test_late_p2t_gamma_ge_invalid);
    RUN_TEST(test_late_p2t_eta_g_two);
    RUN_TEST(test_late_p2t_alpha_ga_two);
    RUN_TEST(test_late_p2t_alpha_ge_a_parameterisation);

    TEST_SUITE("hzd_late_t2p — theta to params");
    RUN_TEST(test_late_t2p_tau_valid);
    RUN_TEST(test_late_t2p_tau_overflow);
    RUN_TEST(test_late_t2p_gamma_g_two);
    RUN_TEST(test_late_t2p_gamma_ge_parameterisation);
    RUN_TEST(test_late_t2p_eta_g_two);
    RUN_TEST(test_late_t2p_eta_ge_parameterisation);
    RUN_TEST(test_late_t2p_alpha_ga_two);
    RUN_TEST(test_late_t2p_alpha_ga_two_derived);
    RUN_TEST(test_late_t2p_alpha_ge_parameterisation);

    print_test_results();
    return test_exit_code();
}
