#include <string.h>
#include "hazard.h"

#include <hzf_memget.h>
#include "memclc1.h"
#include "memclc2.h"
#include <hzf_log1.h>
#include <hzfxit.h>

/****************************************************************/
/* ALOCWRK called from main                                     */
void alocwrk(void){
  int i,j;
  double *Work;

  H->maxn = 0;
  for(i=0; i<8; i++)
    if(C->status[i]==1)
      H->maxn++;
  for(i=1; i<=3; i++)
    if(C->phase[i]==1) {
      H->maxn++;
      if(C->p>0) {
	im1p = (i-1)*C->p+i+8;
	for(j=0; j<C->p; j++) {
	  jp = im1p+j;
	  if(C->status[jp]==1 || H->mxmove[jp]>0)
	    H->maxn++;
	}
      }
    }
  oldbas = 1;
  ilng1 = oldbas;
  memclc1(1,ilng1,&ilngs1,&ilngc1,&ilnsg1,&ilng2);
  if(C->phase[2]==1) {
    ilngs2 = ilng2+C->Nobs;
    ilngc2 = ilngs2+C->Nobs;
    if(intcen>0)
      ilng3 = ilngc2+C->Nobs;
    else
      ilng3 = ilngc2+1;
  } else {
    ilngs2 = ilng2+1;
    ilngc2 = ilngs2+1;
    ilng3 = ilngc2+1;
  }
  memclc1(3,ilng3,&ilngs3,&ilngc3,&ilnsg3,&ilnmu1);
  memclc2(1,ilnmu1,&ilnmu2);
  memclc2(2,ilnmu2,&ilnmu3);
  memclc2(3,ilnmu3,&iindxx);
  maxnv2 = H->maxn/2;
  maxnt2 = maxnv2*2;
  if(maxnt2<H->maxn)
    maxnv2 = maxnv2+1;
  iindxp = iindxx+maxnv2;
  iindxt = iindxp+maxnv2;
  newbas = iindxt+maxnv2;
  oldbas = newbas;
  id2lla = oldbas;
  id2ll = id2lla+H->maxn+1;
  id1l1 = id2ll+H->maxn*H->maxn;
  memclc2(1,id1l1,&id1l2);
  memclc2(2,id1l2,&id1l3);
  memclc2(3,id1l3,&id2l11);
  if(C->phase[1]==1) {
    id2l12 = id2l11+C->Nobs;
    if(C->phase[2]==1)
      id2l13 = id2l12+C->Nobs;
    else
      id2l13 = id2l12+1;
    if(C->phase[3]==1)
      id2l22 = id2l13+C->Nobs;
    else
      id2l22 = id2l13+1;
  } else {
    id2l12 = id2l11+1;
    id2l13 = id2l12+1;
    id2l22 = id2l13+1;
  }
  if(C->phase[2]==1) {
    id2l23 = id2l22+C->Nobs;
    if(C->phase[3]==1)
      id2l33 = id2l23+C->Nobs;
    else
      id2l33 = id2l23+1;
  } else {
    id2l23 = id2l22+1;
    id2l33 = id2l23+1;
  }
  if(C->phase[3]==1)
    newbas = id2l33+C->Nobs;
  else
    newbas = id2l33+1;
  oldbas = newbas;
  ibeta = oldbas;
  ix = ibeta+H->maxn;
  iindx = ix+H->maxn;
  iindx2 = iindx+H->maxn;
  oldbas = iindx2+maxnv2;
  icov = oldbas;
  icorr = icov+H->maxn*H->maxn;
  isd = icorr+H->maxn*H->maxn;
  iznorm = isd+H->maxn;
  ipnorm = iznorm+H->maxn;
  iflags = ipnorm+H->maxn;
  iwk = iflags+maxnv2;
  bascov = iwk+H->maxn;
  il = oldbas;
  ix0 = il+H->maxn*H->maxn;
  ixc = ix0+H->maxn;
  ixplus = ixc+H->maxn;
  igc = ixplus+H->maxn;
  igplus = igc+H->maxn;
  is = igplus+H->maxn;
  iy = is+H->maxn;
  isx = iy+H->maxn;
  itypx = isx+H->maxn;
  basopt = itypx+H->maxn;
  memory = basopt>bascov ? basopt : bascov;
  Work = hzf_memget(memory*sizeof(double));
  if(hzf_mem_need>0) {
    sprintf(msgbfr,"ERROR: Insufficient memory for Stage III; "
	    "need %ldK additional storage",hzf_mem_need/1024);
    hzf_log1(msgbfr);
    hzfxit("MEMORY");
  }
  H->lng1 = Work+ilng1;
  H->lng1ct = Work+ilngc1;
  H->lng1st = Work+ilngs1;
  H->lnsg1 = Work+ilnsg1;
  H->lng2 = Work+ilng2;
  H->lng2ct = Work+ilngc2;
  H->lng2st = Work+ilngs2;
  H->lng3 = Work+ilng3;
  H->lng3ct = Work+ilngc3;
  H->lng3st = Work+ilngs3;
  H->lnsg3 = Work+ilnsg3;
  H->lnmu1 = Work+ilnmu1;
  H->lnmu2 = Work+ilnmu2;
  H->lnmu3 = Work+ilnmu3;
  H->indxx = (int *)(Work+iindxx);
  H->indxp = (int *)(Work+iindxp);
  H->indxt = (int *)(Work+iindxt);
  H->d2llad = Work+id2lla;
  H->d2ll = Work+id2ll;
  H->d1ll1 = Work+id1l1;
  H->d1ll2 = Work+id1l2;
  H->d1ll3 = Work+id1l3;
  H->d2ll11 = Work+id2l11;
  H->d2ll12 = Work+id2l12;
  H->d2ll13 = Work+id2l13;
  H->d2ll22 = Work+id2l22;
  H->d2ll23 = Work+id2l23;
  H->d2ll33 = Work+id2l33;
  H->beta = Work+ibeta;
  H->x = Work+ix;
  H->indx = (int *)(Work+iindx);
  H->indx2 = (int *)(Work+iindx2);
  H->cov = Work+icov;
  H->corr = Work+icorr;
  H->sd = Work+isd;
  H->znorm = Work+iznorm;
  H->pnorm = Work+ipnorm;
  H->flag = (int *)(Work+iflags);
  H->wk = Work+iwk;
  H->lvec = Work+il;
  H->x0 = Work+ix0;
  H->xc = Work+ixc;
  H->xplus = Work+ixplus;
  H->gc = Work+igc;
  H->gplus = Work+igplus;
  H->svec = Work+is;
  H->yvec = Work+iy;
  H->sx = Work+isx;
  H->typx = Work+itypx;
}
