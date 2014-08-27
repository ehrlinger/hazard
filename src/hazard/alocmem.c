#include <hzdinc.h>
#include <hzf_memget.h>
#include <hzf_log1.h>
#include <hzfxit.h>
#include <string.h>

#include "hazard.h"

/****************************************************************/
/* ALOCMEM called from main                                     */
void alocmem(void){
  long p_cnt,count,j; 

  /* Allocate analysis memory */
  p_cnt = 11+3*C->p;
  xvgetptr = hzf_memget((C->p+7)*sizeof(struct xvgetstr));
  xvobsbfr = hzf_memget(xvobslen);
  if(outhaz)
    xvputptr = hzf_memget((p_cnt+3)*sizeof(struct xvputstr));
  concom = hzf_memget(C->p*sizeof(double));
  contal = hzf_memget(C->p*sizeof(double));
  conmin = hzf_memget(C->p*sizeof(double));
  conmax = hzf_memget(C->p*sizeof(double));
  comean = hzf_memget(C->p*sizeof(double));
  conmis = hzf_memget(C->p*sizeof(long));
  iearly = hzf_memget(C->p*sizeof(long));
  iconst = hzf_memget(C->p*sizeof(long));
  ilate = hzf_memget(C->p*sizeof(long));
  H->names = hzf_memget(p_cnt*VAR_NAME_LENGTH);
  H->namex = hzf_memget(p_cnt*VAR_NAME_LENGTH);
  H->labl = hzf_memget(p_cnt*LABEL_LENGTH);
  H->lablx = hzf_memget(p_cnt*LABEL_LENGTH);
  C->theta = hzf_memget(p_cnt*sizeof(double));
  line6 = hzf_memget(p_cnt*sizeof(double));
  H->stdErr = hzf_memget(p_cnt*sizeof(double));
  H->zvalue = hzf_memget(p_cnt*sizeof(double));
  H->pvalue = hzf_memget(p_cnt*sizeof(double));
  H->qx = hzf_memget(p_cnt*sizeof(double));
  H->qtols = hzf_memget(p_cnt*sizeof(double));
  C->status = hzf_memget((p_cnt+1)*sizeof(short int));
  pres6 = hzf_memget(p_cnt*sizeof(long));
  H->index = hzf_memget(p_cnt*sizeof(long));
  H->nx = hzf_memget(p_cnt*sizeof(long));
  H->nm1dx = hzf_memget(p_cnt*sizeof(long));
  H->moves = hzf_memget(p_cnt*sizeof(long));
  H->mxmove = hzf_memget(p_cnt*sizeof(long));
  H->flags = hzf_memget(p_cnt*sizeof(long));
  C->obs = hzf_memget((7+C->p)*C->Nobs*sizeof(double));
  names_ns = hzf_memget(p_cnt*sizeof(struct namestr *));
  O->orders = hzf_memget(p_cnt*sizeof(double));
  O->rstvec = hzf_memget(p_cnt*sizeof(long));
  if(hzf_mem_need>0) {
    sprintf(msgbfr,"ERROR: Insufficient memory for Stage II; "
	    "need %ldK additional storage",hzf_mem_need/1024);
    hzf_log1(msgbfr);
    hzfxit("MEMORY");
  }
  for(count=0; count<C->p; count++)
    iearly[count] = iconst[count] = ilate[count] = 0;
  for(count=0; count<p_cnt; count++) {
    H->moves[count] = H->mxmove[count] = C->theta[count] = ZERO;
    C->status[count] = 0;
    O->orders[count] = O->ordmax;
    O->rstvec[count] = 0;
  }
  for(j=1; j<=8; j++)
    C->status[j-1] = t_status(j);
}
