/*
 * tests/unit/test_xport_v8_detection.c
 *
 * Covers src/blackbox/xportHandler.c — V8 banner check added in v4.4.6.
 *
 * Test approach: a pure unit-testable version of the banner-check
 * logic is extracted into a static helper that takes the 80-byte
 * header #2 buffer and returns one of:
 *   XPORT_VERSION_V5 / XPORT_VERSION_V8 / XPORT_VERSION_UNKNOWN
 *
 * To test it, we declare the same enum + helper signature here and
 * implement a thin wrapper. The actual xportHandler.c implementation
 * gets the same helper inline.
 *
 * For unit testing we don't open a real .xpt file — we feed the
 * helper the exact 80-byte header strings.
 */

#include <stdio.h>
#include <string.h>
#include "test_harness.h"

/* These constants and the function under test are mirrored from
   src/blackbox/xport_version.h (created in Task C2). */
#include "xport_version.h"

/* Real V5 LIBRARY HEADER record (80 bytes, padded with spaces) */
static const char XPORT_V5_HEADER_2[] =
    "HEADER RECORD*******LIBRARY HEADER RECORD!!!!!!!000000000000000000000000000000  ";

/* Real V8 LIBV8 HEADER record */
static const char XPORT_V8_HEADER_2[] =
    "HEADER RECORD*******LIBV8   HEADER RECORD!!!!!!!000000000000000000000000000000  ";

/* Garbage / unknown */
static const char XPORT_UNKNOWN_HEADER_2[] =
    "HEADER RECORD*******GARBAGE HEADER RECORD!!!!!!!000000000000000000000000000000  ";

/* Truncated / short */
static const char XPORT_SHORT_HEADER_2[] = "TOO SHORT";

static void test_v5_banner_detected(void)
{
    enum xport_version v = xport_detect_version(XPORT_V5_HEADER_2,
                                                sizeof(XPORT_V5_HEADER_2) - 1);
    ASSERT_EQ_INT(v, XPORT_VERSION_V5);
}

static void test_v8_banner_detected(void)
{
    enum xport_version v = xport_detect_version(XPORT_V8_HEADER_2,
                                                sizeof(XPORT_V8_HEADER_2) - 1);
    ASSERT_EQ_INT(v, XPORT_VERSION_V8);
}

static void test_unknown_banner(void)
{
    enum xport_version v = xport_detect_version(XPORT_UNKNOWN_HEADER_2,
                                                sizeof(XPORT_UNKNOWN_HEADER_2) - 1);
    ASSERT_EQ_INT(v, XPORT_VERSION_UNKNOWN);
}

static void test_short_buffer_unknown(void)
{
    enum xport_version v = xport_detect_version(XPORT_SHORT_HEADER_2,
                                                sizeof(XPORT_SHORT_HEADER_2) - 1);
    ASSERT_EQ_INT(v, XPORT_VERSION_UNKNOWN);
}

static void test_null_buffer_unknown(void)
{
    enum xport_version v = xport_detect_version(NULL, 80);
    ASSERT_EQ_INT(v, XPORT_VERSION_UNKNOWN);
}

int main(void)
{
    TEST_SUITE("xport_detect_version — banner classification");
    RUN_TEST(test_v5_banner_detected);
    RUN_TEST(test_v8_banner_detected);
    RUN_TEST(test_unknown_banner);
    RUN_TEST(test_short_buffer_unknown);
    RUN_TEST(test_null_buffer_unknown);

    print_test_results();
    return test_exit_code();
}
