#include <stdio.h>
#include <string.h>
#include "sas.h"
#include "structures.h"

#include "hzdinc.h"
#include "hzfxit.h"
#include "hzd_log.h"

void xinit(void){
  size_t pos;
  double td;
  unsigned char ts[sizeof(double)];

  size_t size = (9*sizeof(short) + 3*sizeof(charVarName)+sizeof(charLabel)+
		 sizeof(int)+54*sizeof(char));

  /* Sanity check of NAMESTR size */
  if(sizeof(struct namestr)!=size){
    HZD_LOG_ERROR("NAMESTR size mismatch: sizeof(struct namestr)=%zu, expected=%zu",
                  sizeof(struct namestr), size);
    hzfxit("NAMESTR sizing");
  }

  /* More sanity checks for precision errors */
  for(pos=0; pos<sizeof(double); pos++) {
    memset(ts,'\0',sizeof(double));
    ts[pos] = 0x2e;
    td = *(double *)ts;
    td = td+Machine->two;
    td = td-Machine->one;
    td = td-Machine->one;
    if(memcmp(ts,(char *)&td,sizeof(double))!=0)
      break;
  }
  if(pos==sizeof(double))
    hzfxit("MISSVAL");
  memset(MISSING,'\0',sizeof(double));
  MISSING[pos] = 0x2e;
}
