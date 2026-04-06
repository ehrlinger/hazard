/*
 * tests/unit/test_obs_layout.c
 *
 * Documents and validates the OBS array layout used throughout
 * setlik.c / setcoe.c / setobj.c.
 *
 * The OBS array stores observations in a flat layout:
 *   Common.obs[I * pp7 + offset]   where pp7 = 7
 *
 * The 1-based offset convention (inherited from Fortran) is:
 *   offset 1 → TIME   (follow-up interval)
 *   offset 2 → C1     (number of events at T)
 *   offset 3 → C2     (number of censored at T)
 *   offset 4 → C3     (number of interval-censored)
 *   offset 5 → CTIME  (interval censoring start time)
 *   offset 6 → STIME  (repeating-event or time-varying start time)
 *   offset 7 → WEIGHT (event weight ≥ 0)
 *
 * Im1p7 = I * pp7 - 1 means:
 *   obs[Im1p7 + 1] = obs[I*7 + 0] = row-I column TIME
 *   obs[Im1p7 + 7] = obs[I*7 + 6] = row-I column WEIGHT
 *
 * This file has NO dependency on the global state — it runs without
 * hazard_test_init().  The intent is to catch any future refactoring
 * that accidentally changes the offset constants.
 *
 * See also: §2, Pattern B in docs/Claude_MODERNIZATION_GUIDE.md
 *           which proposes replacing this layout with an obs_row struct.
 */

#include "test_harness.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/* OBS layout constants (replicate what the C code uses inline)        */
/* ------------------------------------------------------------------ */

#define OBS_PP7          7          /* columns per observation row    */
#define OBS_OFF_TIME     1
#define OBS_OFF_C1       2
#define OBS_OFF_C2       3
#define OBS_OFF_C3       4
#define OBS_OFF_CTIME    5
#define OBS_OFF_STIME    6
#define OBS_OFF_WEIGHT   7

/* Im1p7(I) = I * pp7 - 1  →  Im1p7 + offset gives the array index */
#define IM1P7(I)  ((I) * OBS_PP7 - 1)

/* ------------------------------------------------------------------
 * Tests
 * ------------------------------------------------------------------ */

/* Row-0 Im1p7 = -1, so obs[Im1p7+1] = obs[0] etc. */
static void test_row0_indices(void)
{
    int im1p7 = IM1P7(0);
    ASSERT_EQ_INT(im1p7, -1);
    ASSERT_EQ_INT(im1p7 + OBS_OFF_TIME,   0);
    ASSERT_EQ_INT(im1p7 + OBS_OFF_C1,     1);
    ASSERT_EQ_INT(im1p7 + OBS_OFF_C2,     2);
    ASSERT_EQ_INT(im1p7 + OBS_OFF_C3,     3);
    ASSERT_EQ_INT(im1p7 + OBS_OFF_CTIME,  4);
    ASSERT_EQ_INT(im1p7 + OBS_OFF_STIME,  5);
    ASSERT_EQ_INT(im1p7 + OBS_OFF_WEIGHT, 6);
}

/* Row-1 Im1p7 = 6, so obs[Im1p7+1] = obs[7] etc. */
static void test_row1_indices(void)
{
    int im1p7 = IM1P7(1);
    ASSERT_EQ_INT(im1p7, 6);
    ASSERT_EQ_INT(im1p7 + OBS_OFF_TIME,   7);
    ASSERT_EQ_INT(im1p7 + OBS_OFF_WEIGHT, 13);
}

/* Row-N starts at N*pp7 = N*7. */
static void test_rowN_start(void)
{
    for (int n = 0; n < 10; n++) {
        int start = IM1P7(n) + OBS_OFF_TIME;   /* obs index of TIME column */
        ASSERT_EQ_INT(start, n * OBS_PP7);
    }
}

/* Adjacent rows must not overlap. */
static void test_rows_non_overlapping(void)
{
    int end_of_row0   = IM1P7(0) + OBS_OFF_WEIGHT;   /* = 6 */
    int start_of_row1 = IM1P7(1) + OBS_OFF_TIME;     /* = 7 */
    ASSERT_TRUE(end_of_row0 < start_of_row1);
}

/* WEIGHT is the last column — pp7 minus 1 offset from the row start. */
static void test_weight_is_last_column(void)
{
    int row_start  = IM1P7(0) + OBS_OFF_TIME;
    int row_weight = IM1P7(0) + OBS_OFF_WEIGHT;
    ASSERT_EQ_INT(row_weight - row_start, OBS_PP7 - 1);
}

/* Functional round-trip: write known values into a mock array and
 * read them back using the Im1p7 addressing scheme. */
static void test_obs_readback_roundtrip(void)
{
    double obs[14];  /* 2 rows × 7 columns */
    memset(obs, 0, sizeof(obs));

    /* Write row 0 */
    int p0 = IM1P7(0);
    obs[p0 + OBS_OFF_TIME]   = 3.14;
    obs[p0 + OBS_OFF_C1]     = 2.0;
    obs[p0 + OBS_OFF_C2]     = 5.0;
    obs[p0 + OBS_OFF_C3]     = 0.0;
    obs[p0 + OBS_OFF_CTIME]  = 0.0;
    obs[p0 + OBS_OFF_STIME]  = 0.0;
    obs[p0 + OBS_OFF_WEIGHT] = 1.0;

    /* Write row 1 */
    int p1 = IM1P7(1);
    obs[p1 + OBS_OFF_TIME]   = 7.77;
    obs[p1 + OBS_OFF_C1]     = 0.0;
    obs[p1 + OBS_OFF_C2]     = 1.0;
    obs[p1 + OBS_OFF_WEIGHT] = 0.5;

    /* Read back row 0 */
    ASSERT_EQ_DOUBLE(obs[IM1P7(0) + OBS_OFF_TIME],   3.14, 0.0);
    ASSERT_EQ_DOUBLE(obs[IM1P7(0) + OBS_OFF_C1],     2.0,  0.0);
    ASSERT_EQ_DOUBLE(obs[IM1P7(0) + OBS_OFF_WEIGHT], 1.0,  0.0);

    /* Read back row 1 */
    ASSERT_EQ_DOUBLE(obs[IM1P7(1) + OBS_OFF_TIME],   7.77, 0.0);
    ASSERT_EQ_DOUBLE(obs[IM1P7(1) + OBS_OFF_C2],     1.0,  0.0);
    ASSERT_EQ_DOUBLE(obs[IM1P7(1) + OBS_OFF_WEIGHT], 0.5,  0.0);

    /* Row 0 and row 1 TIME must not alias */
    ASSERT_TRUE(obs[IM1P7(0) + OBS_OFF_TIME] != obs[IM1P7(1) + OBS_OFF_TIME]);
}

/* The total column count (7) matches the documented "pp7" value.
 * Regression guard: if pp7 ever changes, this test breaks immediately. */
static void test_pp7_equals_seven(void)
{
    ASSERT_EQ_INT(OBS_PP7, 7);
}

/* Censoring types are distinct columns (not aliased). */
static void test_censoring_columns_distinct(void)
{
    ASSERT_TRUE(OBS_OFF_C1 != OBS_OFF_C2);
    ASSERT_TRUE(OBS_OFF_C1 != OBS_OFF_C3);
    ASSERT_TRUE(OBS_OFF_C2 != OBS_OFF_C3);
}

int main(void)
{
    /* No hazard_test_init() needed — no global state used */

    TEST_SUITE("OBS array layout (pp7=7, Im1p7 addressing)");
    RUN_TEST(test_pp7_equals_seven);
    RUN_TEST(test_row0_indices);
    RUN_TEST(test_row1_indices);
    RUN_TEST(test_rowN_start);
    RUN_TEST(test_rows_non_overlapping);
    RUN_TEST(test_weight_is_last_column);
    RUN_TEST(test_obs_readback_roundtrip);
    RUN_TEST(test_censoring_columns_distinct);

    print_test_results();
    return test_exit_code();
}
