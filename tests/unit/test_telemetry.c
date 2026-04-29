/* Request POSIX.1-2008 APIs (putenv, unlink, etc.) before any system header. */
#ifndef _WIN32
#  define _POSIX_C_SOURCE 200809L
#endif

/*
 * tests/unit/test_telemetry.c
 *
 * Covers src/common/hzd_telemetry.{h,c}.
 *
 * Test groups:
 *   1. Path resolution fallback chain
 *   2. Opt-out (env var + flag)
 *   3. Schema correctness (each field appears, format valid)
 *   4. PHI invariants (negative — variable names MUST NOT appear)
 *
 * Strategy: each test sets HAZARD_TELEMETRY_LOG to a tmpfile path,
 * calls begin() / set_*() / end(), then reads the file and asserts.
 * Tests that exercise the fallback chain unset HAZARD_TELEMETRY_LOG
 * and use HOME / XDG_STATE_HOME env overrides.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#include <process.h>   /* _getpid */
#include <direct.h>    /* _mkdir, _rmdir */
#define unlink  _unlink
#define getpid  _getpid
#else
#include <unistd.h>
#endif

#include "hzd_telemetry.h"
#include "test_harness.h"

/* Portable env helpers — avoids putenv() lifetime UB. */
#ifdef _WIN32
#  define hzd_test_setenv(k, v)   _putenv_s((k), (v))
#  define hzd_test_unsetenv(k)    _putenv_s((k), "")
#else
#  define hzd_test_setenv(k, v)   setenv((k), (v), 1)
#  define hzd_test_unsetenv(k)    unsetenv((k))
#endif

/*
 * Portable temp directory.
 *
 * MSYS2 UCRT64 test binaries are native Windows PE executables whose C
 * runtime (ucrt) does NOT translate MSYS2 virtual paths (e.g. "/tmp").
 * We must use a real Windows path.  On POSIX, "/tmp" is always writable.
 */
#ifdef _WIN32
static const char *get_tmp_dir(void)
{
    const char *t = getenv("TEMP");
    if (!t || !*t) t = getenv("TMP");
    if (!t || !*t) t = "C:\\Temp";
    return t;
}
#  define TDIR       get_tmp_dir()
#  define TSEP       "\\"
#else
#  define TDIR       "/tmp"
#  define TSEP       "/"
#endif

/* Build a per-test temp path: <TDIR><TSEP><stem>_<pid> */
#define MAKE_TMP(buf, sz, stem) \
    snprintf((buf), (sz), "%s%s%s_%d", TDIR, TSEP, (stem), (int)getpid())

/* ---------- Helpers ------------------------------------------------- */

static char *read_file_to_string(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (!fp) return NULL;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    fread(buf, 1, sz, fp);
    buf[sz] = '\0';
    fclose(fp);
    return buf;
}

static int file_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

/* ---------- Group 1: Path resolution -------------------------------- */

static void test_explicit_env_override(void)
{
    char path[256];
    MAKE_TMP(path, sizeof(path), "hzd_telemetry_test.log");
    unlink(path);

    hzd_test_setenv("HAZARD_TELEMETRY_LOG", path);
    hzd_test_unsetenv("HAZARD_NO_TELEMETRY");

    hzd_telemetry_test_reset();
    hzd_telemetry_begin(0);
    hzd_telemetry_set_input_format(HZD_TELEMETRY_INPUT_XPORT_V5);
    hzd_telemetry_end(0);

    ASSERT_TRUE(file_exists(path));
    char *content = read_file_to_string(path);
    ASSERT_TRUE(content != NULL);
    ASSERT_TRUE(strstr(content, "\"event\":\"invocation\"") != NULL);
    ASSERT_TRUE(strstr(content, "\"input_format\":\"XPORT_V5\"") != NULL);
    free(content);
    unlink(path);
}

#ifndef _WIN32
static void test_fallback_to_home_when_env_unset(void)
{
    /* Ensure HAZARD_TELEMETRY_LOG is unset; ensure /var/log/hazard not
       writable in test env (shouldn't be). HOME-based fallback gets used. */

    hzd_test_unsetenv("HAZARD_TELEMETRY_LOG");  /* clear */

    char home[256];
    MAKE_TMP(home, sizeof(home), "hzd_telemetry_home");
    /* Use separate commands to avoid truncation: each fits comfortably in 512. */
    char rm_cmd[512];
    snprintf(rm_cmd, sizeof(rm_cmd), "rm -rf %s", home);
    system(rm_cmd);
    char mk_cmd[512];
    snprintf(mk_cmd, sizeof(mk_cmd), "mkdir -p %s", home);
    system(mk_cmd);

    hzd_test_setenv("HOME", home);
    /* Also clear XDG_STATE_HOME so HOME wins */
    hzd_test_unsetenv("XDG_STATE_HOME");

    hzd_telemetry_test_reset();
    hzd_telemetry_begin(0);
    hzd_telemetry_end(0);

    char expected[512];
    snprintf(expected, sizeof(expected), "%s/.hazard/events.log", home);
    ASSERT_TRUE(file_exists(expected));

    snprintf(rm_cmd, sizeof(rm_cmd), "rm -rf %s", home);
    system(rm_cmd);
}
#else
/*
 * On Windows the HOME-based fallback is not compiled (telemetry uses
 * LOCALAPPDATA / USERPROFILE instead).  Exercise XDG_STATE_HOME (step 3
 * of the fallback chain).
 *
 * IMPORTANT: MSYS2 UCRT64 test binaries are native Windows PE executables
 * whose C runtime (ucrt) does NOT translate MSYS2 virtual paths such as
 * "/tmp".  We must use a real Windows path from %TEMP%.  The XDG branch in
 * hzd_telemetry.c uses forward slashes ("%s/hazard") which ucrt's mkdir /
 * fopen accept as valid path separators on Windows.
 */
static void test_fallback_to_xdg_when_env_unset(void)
{
    hzd_test_unsetenv("HAZARD_TELEMETRY_LOG");

    /* Build XDG base dir using a real Windows temp path. */
    char xdg[256];
    MAKE_TMP(xdg, sizeof(xdg), "hzd_telemetry_xdg");

    /* Create the directory.  _mkdir is already defined in <direct.h>. */
    _mkdir(xdg);

    hzd_test_setenv("XDG_STATE_HOME", xdg);
    hzd_test_unsetenv("HAZARD_NO_TELEMETRY");

    hzd_telemetry_test_reset();
    hzd_telemetry_begin(0);
    hzd_telemetry_end(0);

    /* Telemetry creates xdg/hazard/ and writes events.log there. */
    char expected[512];
    snprintf(expected, sizeof(expected), "%s/hazard/events.log", xdg);
    ASSERT_TRUE(file_exists(expected));

    /* Best-effort cleanup. */
    remove(expected);
    char hazdir[512];
    snprintf(hazdir, sizeof(hazdir), "%s/hazard", xdg);
    _rmdir(hazdir);
    _rmdir(xdg);

    hzd_test_unsetenv("XDG_STATE_HOME");
}
#endif

/* ---------- Group 2: Opt-out --------------------------------------- */

static void test_opt_out_via_env(void)
{
    char path[256];
    MAKE_TMP(path, sizeof(path), "hzd_telemetry_optout.log");
    unlink(path);

    hzd_test_setenv("HAZARD_TELEMETRY_LOG", path);
    hzd_test_setenv("HAZARD_NO_TELEMETRY", "1");

    hzd_telemetry_test_reset();
    hzd_telemetry_begin(0);
    hzd_telemetry_set_input_format(HZD_TELEMETRY_INPUT_XPORT_V5);
    hzd_telemetry_end(0);

    /* File MUST NOT be created. */
    ASSERT_TRUE(!file_exists(path));

    hzd_test_unsetenv("HAZARD_NO_TELEMETRY");  /* clear for subsequent tests */
}

static void test_opt_out_via_flag(void)
{
    char path[256];
    MAKE_TMP(path, sizeof(path), "hzd_telemetry_flag.log");
    unlink(path);

    hzd_test_setenv("HAZARD_TELEMETRY_LOG", path);
    hzd_test_unsetenv("HAZARD_NO_TELEMETRY");  /* not set */

    hzd_telemetry_test_reset();
    hzd_telemetry_begin(1);  /* no_telemetry=1 from --no-telemetry */
    hzd_telemetry_set_input_format(HZD_TELEMETRY_INPUT_XPORT_V5);
    hzd_telemetry_end(0);

    ASSERT_TRUE(!file_exists(path));
}

/* ---------- Group 3: Schema correctness ----------------------------- */

static void test_schema_required_fields(void)
{
    char path[256];
    MAKE_TMP(path, sizeof(path), "hzd_telemetry_schema.log");
    unlink(path);

    hzd_test_setenv("HAZARD_TELEMETRY_LOG", path);
    hzd_test_unsetenv("HAZARD_NO_TELEMETRY");

    hzd_telemetry_test_reset();
    hzd_telemetry_begin(0);
    hzd_telemetry_set_input_format(HZD_TELEMETRY_INPUT_XPORT_V5);
    hzd_telemetry_set_input_size_bytes(124816);
    hzd_telemetry_set_output_format(HZD_TELEMETRY_OUTPUT_XPORT_V5);
    hzd_telemetry_set_n_obs(1500);
    hzd_telemetry_set_n_vars(12);
    hzd_telemetry_end(0);

    char *c = read_file_to_string(path);
    ASSERT_TRUE(c != NULL);

    /* Each schema field must appear */
    ASSERT_TRUE(strstr(c, "\"event\":\"invocation\"") != NULL);
    ASSERT_TRUE(strstr(c, "\"schema_version\":1") != NULL);
    ASSERT_TRUE(strstr(c, "\"ts\":\"") != NULL);
    ASSERT_TRUE(strstr(c, "\"host\":\"") != NULL);
    ASSERT_TRUE(strstr(c, "\"user\":\"") != NULL);
    ASSERT_TRUE(strstr(c, "\"hazard_version\":\"") != NULL);
    ASSERT_TRUE(strstr(c, "\"input_format\":\"XPORT_V5\"") != NULL);
    ASSERT_TRUE(strstr(c, "\"input_size_bytes\":124816") != NULL);
    ASSERT_TRUE(strstr(c, "\"output_format\":\"XPORT_V5\"") != NULL);
    ASSERT_TRUE(strstr(c, "\"exit_code\":0") != NULL);
    ASSERT_TRUE(strstr(c, "\"duration_ms\":") != NULL);
    ASSERT_TRUE(strstr(c, "\"n_obs\":1500") != NULL);
    ASSERT_TRUE(strstr(c, "\"n_vars\":12") != NULL);

    /* JSON line must end with newline */
    size_t L = strlen(c);
    ASSERT_TRUE(L > 0 && c[L-1] == '\n');

    free(c);
    unlink(path);
}

static void test_schema_null_for_unset_fields(void)
{
    char path[256];
    MAKE_TMP(path, sizeof(path), "hzd_telemetry_null.log");
    unlink(path);
    hzd_test_setenv("HAZARD_TELEMETRY_LOG", path);

    hzd_telemetry_test_reset();
    hzd_telemetry_begin(0);
    /* Don't set input_format / n_obs / n_vars — expect null/UNKNOWN */
    hzd_telemetry_end(16);

    char *c = read_file_to_string(path);
    ASSERT_TRUE(c != NULL);
    ASSERT_TRUE(strstr(c, "\"input_format\":\"UNKNOWN\"") != NULL ||
                strstr(c, "\"input_format\":null") != NULL);
    ASSERT_TRUE(strstr(c, "\"n_obs\":null") != NULL);
    ASSERT_TRUE(strstr(c, "\"n_vars\":null") != NULL);
    ASSERT_TRUE(strstr(c, "\"exit_code\":16") != NULL);
    free(c);
    unlink(path);
}

/* ---------- Group 4: PHI invariants (CRITICAL — negative tests) ---- */

static void test_phi_no_var_names_in_log(void)
{
    char path[256];
    MAKE_TMP(path, sizeof(path), "hzd_telemetry_phi.log");
    unlink(path);
    hzd_test_setenv("HAZARD_TELEMETRY_LOG", path);

    /* Plant a "variable name" into HAZARD_BOOTSTRAP_JOB_ID. */
    hzd_test_setenv("HAZARD_BOOTSTRAP_JOB_ID", "BIOPSY_RESULT_PHI_TOKEN_xyz");

    hzd_telemetry_test_reset();
    hzd_telemetry_begin(0);
    hzd_telemetry_end(0);

    char *c = read_file_to_string(path);
    ASSERT_TRUE(c != NULL);

    /* Token must appear EXACTLY ONCE in parent_job_id. */
    int count = 0;
    const char *p = c;
    while ((p = strstr(p, "BIOPSY_RESULT_PHI_TOKEN_xyz")) != NULL) {
        count++;
        p++;
    }
    ASSERT_EQ_INT(count, 1);
    ASSERT_TRUE(strstr(c, "\"parent_job_id\":\"BIOPSY_RESULT_PHI_TOKEN_xyz\"") != NULL);

    hzd_test_unsetenv("HAZARD_BOOTSTRAP_JOB_ID");
    free(c);
    unlink(path);
}

/* ---------- main --------------------------------------------------- */

int main(void)
{
    TEST_SUITE("hzd_telemetry — path resolution");
    RUN_TEST(test_explicit_env_override);
#ifndef _WIN32
    RUN_TEST(test_fallback_to_home_when_env_unset);
#else
    RUN_TEST(test_fallback_to_xdg_when_env_unset);
#endif

    TEST_SUITE("hzd_telemetry — opt-out");
    RUN_TEST(test_opt_out_via_env);
    RUN_TEST(test_opt_out_via_flag);

    TEST_SUITE("hzd_telemetry — schema");
    RUN_TEST(test_schema_required_fields);
    RUN_TEST(test_schema_null_for_unset_fields);

    TEST_SUITE("hzd_telemetry — PHI invariants");
    RUN_TEST(test_phi_no_var_names_in_log);

    print_test_results();
    return test_exit_code();
}
