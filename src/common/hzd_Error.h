/**
 * @file hzd_Error.h
 * @brief Non-local error signalling via longjmp.
 *
 * `hzd_Error(msg, errnum)` is a macro that captures `__FILE__`,
 * `__LINE__`, and `__func__` at the call site, then calls
 * hzd_Error_at() which sets `Common.errorno` and longjmps to
 * `Common.errtrap` (established in `main()`).
 *
 * **Usage pattern:**
 * ```c
 * // At program start (hazard.c / hazpred.c):
 * if (setjmp(Common.errtrap) != 0) {
 *     // domain error caught — clean up and exit
 *     hzd_exit(Common.errorno);
 * }
 *
 * // Inside a numerical routine:
 * if (x <= 0.0)
 *     hzd_Error("DLG1900", 2);   // never returns
 * ```
 *
 * `ErrorPassBack(label)` propagates errors one level up when the
 * caller cannot safely longjmp (e.g., inside a value-returning function):
 * ```c
 * result = hzd_ln_A_div_B(a, b);
 * ErrorPassBack("SETLIK400");    // re-triggers longjmp if errorno != 0
 * ```
 *
 * @see hzd_exit_codes.h for the exit code values passed as `errnum`.
 * @see architecture for the complete error handling pattern.
 */
#ifndef HZD_ERROR_H
#define HZD_ERROR_H

/**
 * @brief Record error location and longjmp to Common.errtrap.
 *
 * Sets `Common.errflg`, `Common.errorno`, `Common.errfile`,
 * `Common.errfunc`, `Common.errline`, then calls longjmp().
 * Never returns.
 *
 * @param file     Source file (`__FILE__` from call site).
 * @param line     Source line (`__LINE__` from call site).
 * @param func     Function name (`__func__` from call site).
 * @param msgtext  Short error label (e.g., "DLG1900").
 * @param errnum   Error code (see hzd_exit_codes.h).
 */
void hzd_Error_at(const char *file, int line, const char *func,
                  const char *msgtext, int errnum);

#define hzd_Error(msg, errnum) \
  hzd_Error_at(__FILE__, __LINE__, __func__, (msg), (errnum))

#endif /* HZD_ERROR_H */
