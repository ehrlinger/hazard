#include "hazpred.h"
#include "hzdinc.h"
#include <string.h>

#include "hzf_memget.h"
#include "xvputi.h"
#include "hzf_log1.h"
#include "hzfxit.h"

void alocmem(void){
  long i;

  data_xv = hzf_memget(infilect*sizeof(struct xvgetstr));
  inhaz_xv = hzf_memget(nvars*sizeof(struct xvgetstr));
  xvobsbfr = hzf_memget(data_ln>inhaz_ln ? data_ln : inhaz_ln);
  xvputptr = hzf_memget((infilect+12)*sizeof(struct xvputstr));
  out_ns = hzf_memget((infilect+12)*sizeof(struct namestr));
  xvputi(infilect+12);
  names = hzf_memget(C->Ntheta*VAR_NAME_LENGTH);
  namev = hzf_memget(C->Ntheta*VAR_NAME_LENGTH);
  buffer = hzf_memget(infilect*sizeof(void *));
  buftyp = hzf_memget(infilect*sizeof(short));
  for(i=0; i<infilect; i++) {
    buftyp[i] = data_ns[i].ntype;
    if(buftyp[i]==2)
      buffer[i] = hzf_memget(data_ns[i].nlng);
    else
      buffer[i] = hzf_memget(sizeof(double));
  }
  if(gotID) {
    if(ID_ns->ntype==2)
      idleng = ID_ns->nlng;
    else
      idleng = sizeof(double);
    id_curr = hzf_memget(idleng);
    id_prev = hzf_memget(idleng);
  }
  if(hzf_mem_need>0) {
    hzf_log1("ERROR: Insufficent memory for HAZPRED.");
    hzfxit("MEMORY");
  }
  memcpy(names[0],"DELTA   ",8);
  memcpy(names[1],"THALF   ",8);
  memcpy(names[2],"NU      ",8);
  memcpy(names[3],"M       ",8);
  memcpy(names[4],"TAU     ",8);
  memcpy(names[5],"GAMMA   ",8);
  memcpy(names[6],"ALPHA   ",8);
  memcpy(names[7],"ETA     ",8);
  memcpy(names[8],"E0      ",8);
  for(i=0; i<(C->Ntheta*C->Ntheta); i++)
    C->cov[i] = ZERO;
}
