#ifndef ISBLANK_H
#define ISBLANK_H

#include "structures.h"

logical isBlank(char *strg,long leng);

#define ISBLANK(p) isBlank(p, sizeof(p))

#endif /* ISBLANK_H */
