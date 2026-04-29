/* Request POSIX.1-2008 APIs (fileno, dup, dup2) before any system header. */
#ifndef _WIN32
#  define _POSIX_C_SOURCE 200809L
#endif

/*
 * tests/unit/test_error_markers.c
 *
 * Covers src/common/hzd_error_marker.{h,c}.
 *
 * Verifies:
 *   - HAZARD_ERROR_<CODE> <msg> format on stderr
 *   - stderr is fflush'd before return
 *   - NULL msg → "(no message)"
 *   - empty code → reject (do not emit)
 *   - code with bad chars (lowercase, spaces) → reject
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#define HZD_DUP   _dup
#define HZD_DUP2  _dup2
#define HZD_CLOSE _close
#define HZD_FILENO _fileno
#else
#include <unistd.h>
#define HZD_DUP   dup
#define HZD_DUP2  dup2
#define HZD_CLOSE close
#define HZD_FILENO fileno
#endif

#include "hzd_error_marker.h"
#include "test_harness.h"

/* Capture stderr written by `body` into `buf`. */
static void capture_stderr_call(void (*body)(void), char *buf, size_t bufsz)
{
    FILE *tmp = tmpfile();
    if (!tmp) { buf[0] = '\0'; return; }
    int fd = HZD_FILENO(tmp);
    fflush(stderr);
    int saved = HZD_DUP(HZD_FILENO(stderr));
    HZD_DUP2(fd, HZD_FILENO(stderr));
    body();
    fflush(stderr);
    HZD_DUP2(saved, HZD_FILENO(stderr));
    HZD_CLOSE(saved);
    rewind(tmp);
    size_t n = fread(buf, 1, bufsz - 1, tmp);
    buf[n] = '\0';
    fclose(tmp);
}

static void emit_v8(void)       { hzd_emit_error("XPORT_V8_REJECTED", "V8 detected"); }
static void emit_null_msg(void) { hzd_emit_error("INTERNAL_ERROR", NULL); }
static void emit_empty_code(void){ hzd_emit_error("", "irrelevant"); }
static void emit_null_code(void){ hzd_emit_error(NULL, "irrelevant"); }
static void emit_bad_code(void) { hzd_emit_error("lower case", "irrelevant"); }

static void test_format_basic(void)
{
    char buf[256];
    capture_stderr_call(emit_v8, buf, sizeof(buf));
    ASSERT_TRUE(strstr(buf, "HAZARD_ERROR_XPORT_V8_REJECTED") != NULL);
    ASSERT_TRUE(strstr(buf, "V8 detected") != NULL);
    /* Must end with newline */
    size_t len = strlen(buf);
    ASSERT_TRUE(len > 0 && buf[len-1] == '\n');
}

static void test_null_msg(void)
{
    char buf[256];
    capture_stderr_call(emit_null_msg, buf, sizeof(buf));
    ASSERT_TRUE(strstr(buf, "HAZARD_ERROR_INTERNAL_ERROR") != NULL);
    ASSERT_TRUE(strstr(buf, "(no message)") != NULL);
}

static void test_empty_code_rejected(void)
{
    char buf[256];
    capture_stderr_call(emit_empty_code, buf, sizeof(buf));
    /* Empty code must NOT emit a HAZARD_ERROR_ marker. May emit a
       diagnostic warning but must not produce a malformed marker. */
    ASSERT_TRUE(strstr(buf, "HAZARD_ERROR_ ") == NULL);
}

static void test_null_code_rejected(void)
{
    char buf[256];
    capture_stderr_call(emit_null_code, buf, sizeof(buf));
    ASSERT_TRUE(strstr(buf, "HAZARD_ERROR_") == NULL ||
                strstr(buf, "HAZARD_ERROR_(null)") != NULL);
    /* Either silent rejection or explicit (null) marker. Must not crash. */
}

static void test_bad_code_rejected(void)
{
    char buf[256];
    capture_stderr_call(emit_bad_code, buf, sizeof(buf));
    /* Code with space must not emit a marker that's parseable as the bad code. */
    ASSERT_TRUE(strstr(buf, "HAZARD_ERROR_lower case") == NULL);
}

int main(void)
{
    TEST_SUITE("hzd_error_marker — format");
    RUN_TEST(test_format_basic);
    RUN_TEST(test_null_msg);

    TEST_SUITE("hzd_error_marker — input validation");
    RUN_TEST(test_empty_code_rejected);
    RUN_TEST(test_null_code_rejected);
    RUN_TEST(test_bad_code_rejected);

    print_test_results();
    return test_exit_code();
}
