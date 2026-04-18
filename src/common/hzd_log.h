#ifndef HZD_LOG_H
#define HZD_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

enum hzd_log_level {
  HZD_LOG_LVL_DEBUG = 0,
  HZD_LOG_LVL_INFO  = 1,
  HZD_LOG_LVL_WARN  = 2,
  HZD_LOG_LVL_ERROR = 3,
  HZD_LOG_LVL_NONE  = 4
};

/* Returns the active threshold. Messages with a level lower than this are
   suppressed. Initialised from the HZD_LOG_LEVEL environment variable on
   first call (DEBUG, INFO, WARN, ERROR, NONE — case-insensitive);
   defaults to WARN. */
int hzd_log_threshold(void);

/* Override the threshold at runtime. */
void hzd_log_set_threshold(int level);

/* Formatted emitter. Callers normally use the HZD_LOG_* macros below so
   that __FILE__/__LINE__/__func__ are captured at the call site. */
void hzd_log_emit(int level, const char *file, int line, const char *func,
                  const char *fmt, ...)
#if defined(__GNUC__) || defined(__clang__)
  __attribute__((format(printf, 5, 6)))
#endif
  ;

#define HZD_LOG(lvl, ...)                                                     \
  do {                                                                        \
    if ((lvl) >= hzd_log_threshold())                                         \
      hzd_log_emit((lvl), __FILE__, __LINE__, __func__, __VA_ARGS__);         \
  } while (0)

#define HZD_LOG_DEBUG(...) HZD_LOG(HZD_LOG_LVL_DEBUG, __VA_ARGS__)
#define HZD_LOG_INFO(...)  HZD_LOG(HZD_LOG_LVL_INFO,  __VA_ARGS__)
#define HZD_LOG_WARN(...)  HZD_LOG(HZD_LOG_LVL_WARN,  __VA_ARGS__)
#define HZD_LOG_ERROR(...) HZD_LOG(HZD_LOG_LVL_ERROR, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* HZD_LOG_H */
