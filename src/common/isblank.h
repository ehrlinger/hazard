#ifndef ISBLANK_H
#define ISBLANK_H

#include "structures.h"

logical isblank(char *strg,int leng);

#define ISBLANK(p) isblank(p,sizeof(p))

#endif /* ISBLANK_H */
