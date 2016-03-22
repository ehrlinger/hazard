#include <string.h>
#include "hzdinc.h"
#define __Linux__

void xvputd(struct namestr *xvnsparm,void *xvarparm){
#ifdef __Linux__
  struct namestr DOS_ns;
  char bfr[8];
#endif

  xvputptr[xvputcnt].xvar = xvarparm;
  xvputptr[xvputcnt].type = xvnsparm->ntype;
  xvputptr[xvputcnt].leng = xvnsparm->nlng;
  xvnsparm->npos = xvputlen;
  xvnsparm->nvar0 = xvputcnt;

#ifdef __Linux__
  /*
    Because of the way that Windows (MSDOS) apps store binary data,
    we need to modify the bitwise representation before writing.

    This keeps the files written (maybe) cross platform compatible
  */
  DOS_ns = *xvnsparm;
  swab((void *)&DOS_ns.nlng,(void *)&DOS_ns.nlng,sizeof(short));
  swab((void *)&DOS_ns.ntype,(void *)&DOS_ns.ntype,sizeof(short));
  swab((void *)&DOS_ns.nvar0,(void *)&DOS_ns.nvar0,sizeof(short));
  memcpy(bfr,(void *)&DOS_ns.npos,4);
  swab(bfr,bfr+6,2);
  swab(bfr+2,bfr+4,2);
  memcpy((void *)&DOS_ns.npos,bfr+4,4);
  fwrite(&DOS_ns,sizeof(struct namestr),1,outputDataFile);
#else
  fwrite(xvnsparm,sizeof(struct namestr),1,outputDataFile);
#endif
  xvputcnt++;
  xvputlen += xvnsparm->nlng;
}
