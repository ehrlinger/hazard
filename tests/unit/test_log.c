/*
 * tests/unit/test_log.c
 *
 * Covers src/common/hzd_log.{h,c} — the levelled logger.
 *
 * The logger writes to stderr; these tests redirect stderr to a temp
 * file so they can verify the on-disk payload (level name, file, line,
 * function, formatted message).
 *
 * Zero global-state dependency.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hzd_log.h"
#include "test_harness.h"

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

/* Capture stderr written by `body` into `buf` (up to bufsz-1 bytes). */
static void capture_stderr(void (*body)(void), char *buf, size_t bufsz)
{
    char tmpl[] = "/tmp/hzdlog_XXXXXX";
    int fd = mkstemp(tmpl);
    if (fd < 0) { buf[0] = '\0'; return; }

    fflush(stderr);
    int saved = dup(fileno(stderr));
    dup2(fd, fileno(stderr));

    body();

    fflush(stderr);
    dup2(saved, fileno(stderr));
    close(saved);

    lseek(fd, 0, SEEK_SET);
    ssize_t n = read(fd, buf, bufsz - 1);
    buf[(n > 0) ? (size_t)n : 0] = '\0';

    close(fd);
    unlink(tmpl);
}

/* ------------------------------------------------------------------ */
/* Threshold tests                                                     */
/* ------------------------------------------------------------------ */

static void test_set_threshold_roundtrip(void)
{
    hzd_log_set_threshold(HZD_LOG_LVL_DEBUG);
    ASSERT_EQ_INT(hzd_log_threshold(), HZD_LOG_LVL_DEBUG);

    hzd_log_set_threshold(HZD_LOG_LVL_INFO);
    ASSERT_EQ_INT(hzd_log_threshold(), HZD_LOG_LVL_INFO);

    hzd_log_set_threshold(HZD_LOG_LVL_WARN);
    ASSERT_EQ_INT(hzd_log_threshold(), HZD_LOG_LVL_WARN);

    hzd_log_set_threshold(HZD_LOG_LVL_ERROR);
    ASSERT_EQ_INT(hzd_log_threshold(), HZD_LOG_LVL_ERROR);

    hzd_log_set_threshold(HZD_LOG_LVL_NONE);
    ASSERT_EQ_INT(hzd_log_threshold(), HZD_LOG_LVL_NONE);
}

static void test_set_threshold_clamps(void)
{
    /* Below DEBUG clamps up to DEBUG */
    hzd_log_set_threshold(-42);
    ASSERT_EQ_INT(hzd_log_threshold(), HZD_LOG_LVL_DEBUG);

    /* Above NONE clamps down to NONE */
    hzd_log_set_threshold(9999);
    ASSERT_EQ_INT(hzd_log_threshold(), HZD_LOG_LVL_NONE);
}

/* ------------------------------------------------------------------ */
/* Emission tests                                                      */
/* ------------------------------------------------------------------ */

static void emit_debug_hello(void) { HZD_LOG_DEBUG("hello %d", 7); }
static void emit_info_hello(void)  { HZD_LOG_INFO("info-line");    }
static void emit_warn_hello(void)  { HZD_LOG_WARN("warn-line");    }
static void emit_error_hello(void) { HZD_LOG_ERROR("err-line");    }

static void test_emits_at_or_above_threshold(void)
{
    char buf[512];

    hzd_log_set_threshold(HZD_LOG_LVL_DEBUG);
    capture_stderr(emit_debug_hello, buf, sizeof(buf));
    ASSERT_TRUE(strstr(buf, "DEBUG") != NULL);
    ASSERT_TRUE(strstr(buf, "hello 7") != NULL);
    ASSERT_TRUE(strstr(buf, "test_log.c") != NULL);
    /* The emitting wrapper's function name, captured at macro expansion. */
    ASSERT_TRUE(strstr(buf, "emit_debug_hello") != NULL);
}

static void test_suppresses_below_threshold(void)
{
    char buf[256];

    hzd_log_set_threshold(HZD_LOG_LVL_WARN);
    capture_stderr(emit_debug_hello, buf, sizeof(buf));
    ASSERT_EQ_INT((int)strlen(buf), 0);

    capture_stderr(emit_info_hello, buf, sizeof(buf));
    ASSERT_EQ_INT((int)strlen(buf), 0);

    /* WARN is at threshold — should emit. */
    capture_stderr(emit_warn_hello, buf, sizeof(buf));
    ASSERT_TRUE(strstr(buf, "WARN") != NULL);
    ASSERT_TRUE(strstr(buf, "warn-line") != NULL);

    /* ERROR is above threshold — should emit. */
    capture_stderr(emit_error_hello, buf, sizeof(buf));
    ASSERT_TRUE(strstr(buf, "ERROR") != NULL);
    ASSERT_TRUE(strstr(buf, "err-line") != NULL);
}

static void test_none_suppresses_everything(void)
{
    char buf[256];

    hzd_log_set_threshold(HZD_LOG_LVL_NONE);
    capture_stderr(emit_debug_hello, buf, sizeof(buf));
    ASSERT_EQ_INT((int)strlen(buf), 0);
    capture_stderr(emit_info_hello, buf, sizeof(buf));
    ASSERT_EQ_INT((int)strlen(buf), 0);
    capture_stderr(emit_warn_hello, buf, sizeof(buf));
    ASSERT_EQ_INT((int)strlen(buf), 0);
    capture_stderr(emit_error_hello, buf, sizeof(buf));
    ASSERT_EQ_INT((int)strlen(buf), 0);
}

/* ------------------------------------------------------------------ */

/* Env parsing runs once on the first hzd_log_threshold() call in the
   process, so this must execute BEFORE any other threshold test. */
static void test_env_variable_is_parsed(void)
{
    setenv("HZD_LOG_LEVEL", "info", 1);  /* lower-case — exercises case-fold */
    ASSERT_EQ_INT(hzd_log_threshold(), HZD_LOG_LVL_INFO);
    unsetenv("HZD_LOG_LEVEL");
}

int main(void)
{
    TEST_SUITE("hzd_log — env variable parsing");
    RUN_TEST(test_env_variable_is_parsed);

    TEST_SUITE("hzd_log — threshold handling");
    RUN_TEST(test_set_threshold_roundtrip);
    RUN_TEST(test_set_threshold_clamps);

    TEST_SUITE("hzd_log — emission");
    RUN_TEST(test_emits_at_or_above_threshold);
    RUN_TEST(test_suppresses_below_threshold);
    RUN_TEST(test_none_suppresses_everything);

    print_test_results();
    return test_exit_code();
}
