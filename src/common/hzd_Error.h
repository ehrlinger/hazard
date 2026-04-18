#ifndef HZD_ERROR_H
#define HZD_ERROR_H

/* Enriched longjmp-based error path.
   Call sites use hzd_Error(msgtext, errnum) — the macro captures
   __FILE__ / __LINE__ / __func__ at the call site so that the error catcher
   in main() can report where the failure originated. */

void hzd_Error_at(const char *file, int line, const char *func,
                  const char *msgtext, int errnum);

#define hzd_Error(msg, errnum) \
  hzd_Error_at(__FILE__, __LINE__, __func__, (msg), (errnum))

#endif /* HZD_ERROR_H */
