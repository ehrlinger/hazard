#include "swab_compat.h"
#include <string.h>
#include "hzdinc.h"
/* SAS transport format is big-endian; always byte-swap on write. */
#define SAS_TRANSPORT_BYTESWAP

void xvputd(struct namestr *xvnsparm,void *xvarparm){
  xvputptr[xvputcnt].xvar = xvarparm;
  xvputptr[xvputcnt].type = xvnsparm->ntype;
  xvputptr[xvputcnt].leng = xvnsparm->nlng;
  xvnsparm->npos = xvputlen;
  xvnsparm->nvar0 = xvputcnt;

#ifdef SAS_TRANSPORT_BYTESWAP
  {
    /* Write a byte-swapped copy; leave the caller's struct intact. */
    struct namestr out_ns = *xvnsparm;
    out_ns.nlng  = hzd_bswap_short(out_ns.nlng);
    out_ns.ntype = hzd_bswap_short(out_ns.ntype);
    out_ns.nvar0 = hzd_bswap_short(out_ns.nvar0);
    out_ns.npos  = hzd_bswap_int(out_ns.npos);
    fwrite(&out_ns, sizeof(struct namestr), 1, outputDataFile);
  }
#else
  fwrite(xvnsparm, sizeof(struct namestr), 1, outputDataFile);
#endif
  xvputcnt++;
  xvputlen += xvnsparm->nlng;
}
