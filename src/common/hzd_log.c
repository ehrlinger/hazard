#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hzd_log.h"

static int hzd_log_level_cache = -1;

static int ieq(const char *a, const char *b) {
  while (*a && *b) {
    int ca = tolower((unsigned char)*a++);
    int cb = tolower((unsigned char)*b++);
    if (ca != cb) return 0;
  }
  return *a == *b;
}

static int parse_level(const char *s) {
  if (!s || !*s) return -1;
  if (ieq(s, "DEBUG")) return HZD_LOG_LVL_DEBUG;
  if (ieq(s, "INFO"))  return HZD_LOG_LVL_INFO;
  if (ieq(s, "WARN"))  return HZD_LOG_LVL_WARN;
  if (ieq(s, "WARNING")) return HZD_LOG_LVL_WARN;
  if (ieq(s, "ERROR")) return HZD_LOG_LVL_ERROR;
  if (ieq(s, "NONE"))  return HZD_LOG_LVL_NONE;
  if (ieq(s, "OFF"))   return HZD_LOG_LVL_NONE;
  return -1;
}

int hzd_log_threshold(void) {
  if (hzd_log_level_cache < 0) {
    int lvl = parse_level(getenv("HZD_LOG_LEVEL"));
    hzd_log_level_cache = (lvl < 0) ? HZD_LOG_LVL_WARN : lvl;
  }
  return hzd_log_level_cache;
}

void hzd_log_set_threshold(int level) {
  if (level < HZD_LOG_LVL_DEBUG) level = HZD_LOG_LVL_DEBUG;
  if (level > HZD_LOG_LVL_NONE)  level = HZD_LOG_LVL_NONE;
  hzd_log_level_cache = level;
}

static const char *level_name(int lvl) {
  switch (lvl) {
    case HZD_LOG_LVL_DEBUG: return "DEBUG";
    case HZD_LOG_LVL_INFO:  return "INFO";
    case HZD_LOG_LVL_WARN:  return "WARN";
    case HZD_LOG_LVL_ERROR: return "ERROR";
    default: return "?";
  }
}

static const char *short_file(const char *path) {
  const char *slash = strrchr(path, '/');
  return slash ? slash + 1 : path;
}

void hzd_log_emit(int level, const char *file, int line, const char *func,
                  const char *fmt, ...) {
  va_list ap;
  fprintf(stderr, "[hazard %s %s:%d %s] ",
          level_name(level), short_file(file), line, func);
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fputc('\n', stderr);
  fflush(stderr);
}
