#ifndef ISBLANCK_H
#define ISBLANCK_H

#include "structures.h"

logical isblanck(char *strg,int leng);

#define ISBLANCK(p) isblanck(p,sizeof(p))

#endif /* ISBLANCK_H */
