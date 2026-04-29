/* src/common/hzd_telemetry.c
 *
 * Telemetry subsystem implementation. See hzd_telemetry.h for contract.
 */

/* Request POSIX.1-2008 APIs (clock_gettime, gethostname, gmtime_r, etc.)
   Must appear before any system header. */
#ifndef _WIN32
#  define _POSIX_C_SOURCE 200809L
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <process.h>   /* _getpid */
#include <direct.h>    /* _mkdir  */
#define HZD_GETPID  _getpid
#define mkdir(path, mode) _mkdir(path)
#else
#include <unistd.h>
#include <pwd.h>
#define HZD_GETPID  getpid
#endif

#include "hzd_telemetry.h"
#include "hzd_log.h"

/* Schema version — bump on breaking schema change (per spec A.3.2). */
#define TELEMETRY_SCHEMA_VERSION 1

/* Rotation thresholds. */
#define TELEMETRY_MAX_BYTES (10 * 1024 * 1024)   /* 10 MB */

/* ---------- Process-wide state (single invocation per process) ---- */

static struct {
    int    initialized;
    int    enabled;            /* 0 if opt-out or no writable path */
    char   path[1024];
    struct timespec t_begin;
    char   host[256];
    char   user[128];
    char   hazard_version[64];
    char   parent_job_id[128]; /* may be empty */

    enum hzd_telemetry_input_format  input_format;
    long long input_size_bytes;
    enum hzd_telemetry_output_format output_format;
    long long n_obs;
    long long n_vars;
    int       have_input_format;
    int       have_output_format;
    int       have_n_obs;
    int       have_n_vars;
    int       have_input_size;

    int    finalized;
} S;

/* Test-only reset. */
void hzd_telemetry_test_reset(void)
{
    memset(&S, 0, sizeof(S));
}

/* ---------- Helpers ----------------------------------------------- */

static int dir_exists_writable(const char *p)
{
    struct stat st;
    if (stat(p, &st) != 0) return 0;
    if (!S_ISDIR(st.st_mode)) return 0;
    /* Best-effort writability check: try to create a probe file. */
    char probe[1100];
    snprintf(probe, sizeof(probe), "%s/.hazard_telemetry_probe_%d", p, (int)HZD_GETPID());
    FILE *fp = fopen(probe, "w");
    if (!fp) return 0;
    fclose(fp);
    remove(probe);
    return 1;
}

static int mkdir_p(const char *path)
{
    /* Create path and any missing parents. Returns 0 on success. */
    char buf[1024];
    strncpy(buf, path, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    for (char *p = buf + 1; *p; ++p) {
        if (*p == '/') {
            *p = '\0';
            mkdir(buf, 0755);
            *p = '/';
        }
    }
    int r = mkdir(buf, 0755);
    if (r == 0 || errno == EEXIST) return 0;
    return -1;
}

/* Resolve the log path via fallback chain. Returns 1 if a path was
   found, 0 otherwise. Writes into S.path on success. */
static int resolve_path(void)
{
    const char *env;

    /* (1) Explicit env override */
    env = getenv("HAZARD_TELEMETRY_LOG");
    if (env && *env) {
        strncpy(S.path, env, sizeof(S.path) - 1);
        return 1;
    }

    /* (2) System-wide /var/log/hazard/ */
    if (dir_exists_writable("/var/log/hazard")) {
        strncpy(S.path, "/var/log/hazard/events.log", sizeof(S.path) - 1);
        return 1;
    }

    /* (3) XDG_STATE_HOME (Linux) */
    env = getenv("XDG_STATE_HOME");
    if (env && *env) {
        /* dir must leave room for "/events.log" (11 chars) in S.path. */
        char dir[1012];
        snprintf(dir, sizeof(dir), "%s/hazard", env);
        if (mkdir_p(dir) == 0) {
            snprintf(S.path, sizeof(S.path), "%s/events.log", dir);
            return 1;
        }
    }

#ifdef _WIN32
    /* (4w) %LOCALAPPDATA%\hazard\events.log */
    env = getenv("LOCALAPPDATA");
    if (env && *env) {
        char dir[1012];
        snprintf(dir, sizeof(dir), "%s\\hazard", env);
        if (mkdir_p(dir) == 0) {
            snprintf(S.path, sizeof(S.path), "%s\\events.log", dir);
            return 1;
        }
    }
    /* (5w) %USERPROFILE%\.hazard\events.log */
    env = getenv("USERPROFILE");
    if (env && *env) {
        char dir[1012];
        snprintf(dir, sizeof(dir), "%s\\.hazard", env);
        if (mkdir_p(dir) == 0) {
            snprintf(S.path, sizeof(S.path), "%s\\events.log", dir);
            return 1;
        }
    }
#else
    /* (4u) $HOME/.hazard/events.log */
    env = getenv("HOME");
    if (env && *env) {
        char dir[1012];
        snprintf(dir, sizeof(dir), "%s/.hazard", env);
        if (mkdir_p(dir) == 0) {
            snprintf(S.path, sizeof(S.path), "%s/events.log", dir);
            return 1;
        }
    }
#endif

    return 0;
}

static void capture_host(char *out, size_t n)
{
#ifdef _WIN32
    DWORD sz = (DWORD)n;
    if (!GetComputerNameA(out, &sz)) {
        strncpy(out, "unknown", n - 1);
        out[n - 1] = '\0';
    }
#else
    if (gethostname(out, n - 1) != 0) {
        strncpy(out, "unknown", n - 1);
    }
    out[n - 1] = '\0';
#endif
}

static void capture_user(char *out, size_t n)
{
    const char *u = getenv("USER");
#ifdef _WIN32
    if (!u || !*u) u = getenv("USERNAME");
#else
    if (!u || !*u) {
        struct passwd *pw = getpwuid(getuid());
        if (pw && pw->pw_name) u = pw->pw_name;
    }
#endif
    strncpy(out, (u && *u) ? u : "unknown", n - 1);
    out[n - 1] = '\0';
}

/* ---------- Public API -------------------------------------------- */

void hzd_telemetry_begin(int no_telemetry)
{
    if (S.initialized) return;
    memset(&S, 0, sizeof(S));
    S.initialized = 1;

    /* Opt-out checks */
    if (no_telemetry) {
        S.enabled = 0;
        return;
    }
    const char *off = getenv("HAZARD_NO_TELEMETRY");
    if (off && *off && strcmp(off, "0") != 0 && strcmp(off, "") != 0) {
        S.enabled = 0;
        return;
    }

    /* Path resolution */
    if (!resolve_path()) {
        HZD_LOG_WARN("telemetry disabled: no writable log path");
        S.enabled = 0;
        return;
    }

    /* Capture invariants */
    clock_gettime(CLOCK_MONOTONIC, &S.t_begin);
    capture_host(S.host, sizeof(S.host));
    capture_user(S.user, sizeof(S.user));

#ifdef PACKAGE_VERSION
    strncpy(S.hazard_version, PACKAGE_VERSION, sizeof(S.hazard_version) - 1);
#else
    strncpy(S.hazard_version, "unknown", sizeof(S.hazard_version) - 1);
#endif

    const char *jid = getenv("HAZARD_BOOTSTRAP_JOB_ID");
    if (jid && *jid) {
        strncpy(S.parent_job_id, jid, sizeof(S.parent_job_id) - 1);
    }

    S.enabled = 1;
}

void hzd_telemetry_set_input_format(enum hzd_telemetry_input_format f)
{
    if (!S.enabled) return;
    S.input_format = f;
    S.have_input_format = 1;
}

void hzd_telemetry_set_input_size_bytes(long long n)
{
    if (!S.enabled) return;
    S.input_size_bytes = n;
    S.have_input_size = 1;
}

void hzd_telemetry_set_output_format(enum hzd_telemetry_output_format f)
{
    if (!S.enabled) return;
    S.output_format = f;
    S.have_output_format = 1;
}

void hzd_telemetry_set_n_obs(long long n)
{
    if (!S.enabled) return;
    S.n_obs = n;
    S.have_n_obs = 1;
}

void hzd_telemetry_set_n_vars(long long n)
{
    if (!S.enabled) return;
    S.n_vars = n;
    S.have_n_vars = 1;
}

/* Write `src` to `fp` as a JSON string value (with surrounding quotes).
   Escapes: backslash, double-quote, and control characters (U+0000–U+001F).
   Truncates at 512 chars to bound output size. */
static void json_write_string(FILE *fp, const char *src)
{
    fputc('"', fp);
    if (src) {
        int count = 0;
        for (const char *p = src; *p && count < 512; ++p, ++count) {
            unsigned char c = (unsigned char)*p;
            if      (c == '"')  { fputs("\\\"", fp); }
            else if (c == '\\') { fputs("\\\\", fp); }
            else if (c == '\n') { fputs("\\n",  fp); }
            else if (c == '\r') { fputs("\\r",  fp); }
            else if (c == '\t') { fputs("\\t",  fp); }
            else if (c < 0x20)  { fprintf(fp, "\\u%04x", (unsigned)c); }
            else                { fputc(c, fp); }
        }
    }
    fputc('"', fp);
}

/* Return ISO-8601 UTC timestamp. */
static void format_ts(char *out, size_t n)
{
    time_t t = time(NULL);
    struct tm tm_utc;
#ifdef _WIN32
    gmtime_s(&tm_utc, &t);
#else
    gmtime_r(&t, &tm_utc);
#endif
    strftime(out, n, "%Y-%m-%dT%H:%M:%SZ", &tm_utc);
}

static const char *input_format_str(enum hzd_telemetry_input_format f, int have)
{
    if (!have) return "UNKNOWN";
    switch (f) {
        case HZD_TELEMETRY_INPUT_XPORT_V5: return "XPORT_V5";
        case HZD_TELEMETRY_INPUT_XPORT_V8: return "XPORT_V8";
        case HZD_TELEMETRY_INPUT_PARQUET:  return "PARQUET";
        default: return "UNKNOWN";
    }
}

static const char *output_format_str(enum hzd_telemetry_output_format f, int have)
{
    if (!have) return NULL;  /* serialize as null */
    switch (f) {
        case HZD_TELEMETRY_OUTPUT_XPORT_V5: return "XPORT_V5";
        case HZD_TELEMETRY_OUTPUT_PARQUET:  return "PARQUET";
        default: return NULL;
    }
}

/* Rotate if needed. Best effort; log a WARN on failure but don't error. */
static void rotate_if_needed(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0) return;
    if (st.st_size < TELEMETRY_MAX_BYTES) return;

    char rotated[1100];
    snprintf(rotated, sizeof(rotated), "%s.1", path);
    /* Remove old .1 if present, then rename current → .1 */
    remove(rotated);
    if (rename(path, rotated) != 0) {
        HZD_LOG_WARN("telemetry rotation failed: %s", strerror(errno));
    }
}

void hzd_telemetry_end(int exit_code)
{
    if (!S.enabled || S.finalized) return;
    S.finalized = 1;

    /* Compute duration */
    struct timespec t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    long long dur_ms = (t_end.tv_sec - S.t_begin.tv_sec) * 1000LL +
                       (t_end.tv_nsec - S.t_begin.tv_nsec) / 1000000LL;

    rotate_if_needed(S.path);

    FILE *fp = fopen(S.path, "a");
    if (!fp) {
        HZD_LOG_WARN("telemetry write failed: %s", strerror(errno));
        return;
    }

    char ts[32];
    format_ts(ts, sizeof(ts));

    const char *out_fmt = output_format_str(S.output_format, S.have_output_format);

    fprintf(fp, "{\"event\":\"invocation\",\"schema_version\":%d,\"ts\":\"%s\",",
            TELEMETRY_SCHEMA_VERSION, ts);
    fputs("\"host\":", fp);   json_write_string(fp, S.host);
    fputs(",\"user\":", fp);  json_write_string(fp, S.user);
    fputs(",\"hazard_version\":", fp); json_write_string(fp, S.hazard_version);
    fprintf(fp, ",\"input_format\":\"%s\",",
            input_format_str(S.input_format, S.have_input_format));

    /* input_size_bytes: number or null */
    if (S.have_input_size) fprintf(fp, "\"input_size_bytes\":%lld,", S.input_size_bytes);
    else                   fprintf(fp, "\"input_size_bytes\":null,");

    /* output_format: string or null */
    if (out_fmt) fprintf(fp, "\"output_format\":\"%s\",", out_fmt);
    else         fprintf(fp, "\"output_format\":null,");

    fprintf(fp, "\"exit_code\":%d,", exit_code);
    fprintf(fp, "\"duration_ms\":%lld,", dur_ms);

    if (S.have_n_obs) fprintf(fp, "\"n_obs\":%lld,", S.n_obs);
    else              fprintf(fp, "\"n_obs\":null,");

    if (S.have_n_vars) fprintf(fp, "\"n_vars\":%lld,", S.n_vars);
    else               fprintf(fp, "\"n_vars\":null,");

    if (S.parent_job_id[0]) {
        fputs("\"parent_job_id\":", fp);
        json_write_string(fp, S.parent_job_id);
    } else {
        fputs("\"parent_job_id\":null", fp);
    }

    fprintf(fp, "}\n");
    fclose(fp);
}
