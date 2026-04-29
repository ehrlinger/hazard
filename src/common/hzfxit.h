#ifndef HZFXIT_H
#define HZFXIT_H

/* hzfxit() — fatal exit with a type string (legacy interface).
   Defaults to exit code 16 (HAZARD_EXIT_XPORT_V8_REJECTED equivalent
   range; preserved for backward compatibility with v4.4.5 callers). */

void hzfxit_at(const char *file, int line, const char *func, const char *type);

/* hzfxit_with_code() — v4.4.6+ — explicit exit code.
   Use this for new call sites. The type string is mapped to a
   HAZARD_ERROR_<code> marker on stderr before exiting. */
void hzfxit_with_code_at(const char *file, int line, const char *func,
                         const char *code, const char *msg, int exit_code);

#define hzfxit(type) \
  hzfxit_at(__FILE__, __LINE__, __func__, (type))

#define hzfxit_with_code(code, msg, exit_code) \
  hzfxit_with_code_at(__FILE__, __LINE__, __func__, (code), (msg), (exit_code))

#endif /* HZFXIT_H */
