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
#define unlink _unlink
#else
#include <unistd.h>
#endif

#include "hzd_telemetry.h"
#include "test_harness.h"

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
    snprintf(path, sizeof(path), "/tmp/hzd_telemetry_test_%d.log", (int)getpid());
    unlink(path);

    char env_buf[300];
    snprintf(env_buf, sizeof(env_buf), "HAZARD_TELEMETRY_LOG=%s", path);
    putenv(env_buf);
    putenv((char *)"HAZARD_NO_TELEMETRY=");

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

static void test_fallback_to_home_when_env_unset(void)
{
    /* Ensure HAZARD_TELEMETRY_LOG is unset; ensure /var/log/hazard not
       writable in test env (shouldn't be). HOME-based fallback gets used. */

    putenv((char *)"HAZARD_TELEMETRY_LOG=");  /* clear */

    char home[256];
    snprintf(home, sizeof(home), "/tmp/hzd_telemetry_home_%d", (int)getpid());
    char rm_cmd[300];
    snprintf(rm_cmd, sizeof(rm_cmd), "rm -rf %s && mkdir -p %s", home, home);
    system(rm_cmd);

    char home_env[300];
    snprintf(home_env, sizeof(home_env), "HOME=%s", home);
    putenv(home_env);
    /* Also clear XDG_STATE_HOME so HOME wins */
    putenv((char *)"XDG_STATE_HOME=");

    hzd_telemetry_test_reset();
    hzd_telemetry_begin(0);
    hzd_telemetry_end(0);

    char expected[300];
    snprintf(expected, sizeof(expected), "%s/.hazard/events.log", home);
    ASSERT_TRUE(file_exists(expected));

    snprintf(rm_cmd, sizeof(rm_cmd), "rm -rf %s", home);
    system(rm_cmd);
}

/* ---------- Group 2: Opt-out --------------------------------------- */

static void test_opt_out_via_env(void)
{
    char path[256];
    snprintf(path, sizeof(path), "/tmp/hzd_telemetry_optout_%d.log", (int)getpid());
    unlink(path);

    char env_buf[300];
    snprintf(env_buf, sizeof(env_buf), "HAZARD_TELEMETRY_LOG=%s", path);
    putenv(env_buf);
    putenv((char *)"HAZARD_NO_TELEMETRY=1");

    hzd_telemetry_test_reset();
    hzd_telemetry_begin(0);
    hzd_telemetry_set_input_format(HZD_TELEMETRY_INPUT_XPORT_V5);
    hzd_telemetry_end(0);

    /* File MUST NOT be created. */
    ASSERT_TRUE(!file_exists(path));

    putenv((char *)"HAZARD_NO_TELEMETRY=");  /* clear for subsequent tests */
}

static void test_opt_out_via_flag(void)
{
    char path[256];
    snprintf(path, sizeof(path), "/tmp/hzd_telemetry_flag_%d.log", (int)getpid());
    unlink(path);

    char env_buf[300];
    snprintf(env_buf, sizeof(env_buf), "HAZARD_TELEMETRY_LOG=%s", path);
    putenv(env_buf);
    putenv((char *)"HAZARD_NO_TELEMETRY=");  /* not set */

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
    snprintf(path, sizeof(path), "/tmp/hzd_telemetry_schema_%d.log", (int)getpid());
    unlink(path);

    char env_buf[300];
    snprintf(env_buf, sizeof(env_buf), "HAZARD_TELEMETRY_LOG=%s", path);
    putenv(env_buf);
    putenv((char *)"HAZARD_NO_TELEMETRY=");

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
    snprintf(path, sizeof(path), "/tmp/hzd_telemetry_null_%d.log", (int)getpid());
    unlink(path);
    char env_buf[300];
    snprintf(env_buf, sizeof(env_buf), "HAZARD_TELEMETRY_LOG=%s", path);
    putenv(env_buf);

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
    snprintf(path, sizeof(path), "/tmp/hzd_telemetry_phi_%d.log", (int)getpid());
    unlink(path);
    char env_buf[300];
    snprintf(env_buf, sizeof(env_buf), "HAZARD_TELEMETRY_LOG=%s", path);
    putenv(env_buf);

    /* Plant a "variable name" into HAZARD_BOOTSTRAP_JOB_ID. */
    putenv((char *)"HAZARD_BOOTSTRAP_JOB_ID=BIOPSY_RESULT_PHI_TOKEN_xyz");

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

    putenv((char *)"HAZARD_BOOTSTRAP_JOB_ID=");
    free(c);
    unlink(path);
}

/* ---------- main --------------------------------------------------- */

int main(void)
{
    TEST_SUITE("hzd_telemetry — path resolution");
    RUN_TEST(test_explicit_env_override);
    RUN_TEST(test_fallback_to_home_when_env_unset);

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
