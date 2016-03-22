#include <string.h>
#include <math.h>
#include "xMISSgot.h"
#include "hzdinc.h"

void xvget(void){
  double rs6000fp;
  int b,varix;
  unsigned char bytes[8];
  struct namestr *nsx;

  for(varix=0; varix<xvgetcnt; varix++) {
    nsx = xvgetptr[varix].xvns;
    if(nsx->ntype==2) {
      memcpy(xvgetptr[varix].xvar,xvobsbfr+nsx->npos,
	     nsx->nlng);
      continue;
    } else if(xMISSgot(xvobsbfr+nsx->npos,nsx->nlng)) {
      memcpy(xvgetptr[varix].xvar,MISSING,sizeof(double));
      continue;
    }
    memset(bytes,0,8);
    memcpy(bytes,xvobsbfr+nsx->npos,nsx->nlng);
    rs6000fp = ZERO;
    for(b=7; b>=1; b--)
      rs6000fp = (rs6000fp+bytes[b])/256.0e0;
    if(rs6000fp==ZERO)
      if(bytes[0]!=0)
	memcpy(xvgetptr[varix].xvar,bytes,8);
      else
	*(double *)xvgetptr[varix].xvar = ZERO;
    else {
      rs6000fp = rs6000fp*pow(16.0e0,(bytes[0]&0x7f)-64.0e0);
      if(bytes[0]&0x80)
	rs6000fp = -rs6000fp;
      *(double *)xvgetptr[varix].xvar = rs6000fp;
    }
  }
}
