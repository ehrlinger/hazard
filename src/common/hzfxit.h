#ifndef HZFXIT_H
#define HZFXIT_H

/* Terminal-exit helper. Callers use hzfxit("SEMANTIC"), hzfxit("MEMORY"),
   etc. The macro captures __FILE__ / __LINE__ / __func__ so operators can
   tell which site terminated the run. */

void hzfxit_at(const char *file, int line, const char *func, const char *type);

#define hzfxit(type) hzfxit_at(__FILE__, __LINE__, __func__, (type))

#endif /* HZFXIT_H */
