#ifndef HZD_ERROR_DESCRIBE_H
#define HZD_ERROR_DESCRIBE_H

/* Translate a cryptic legacy error code (e.g. "DLG1935", "DTRSFM1150")
   into a short human-readable description. Returns NULL if the code is
   unknown so callers can fall back to printing the raw code. */
const char *hzd_error_describe(const char *code);

#endif /* HZD_ERROR_DESCRIBE_H */
