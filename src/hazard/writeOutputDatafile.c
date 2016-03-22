#include <string.h>
#include "hazard.h"

#include <xvputi.h>
#include <xvputd.h>
#include <namfix.h>
#include <xvpute.h>
#include <hzfmis.h>
#include <xvput.h>

/**
    REVISIONS

       HZD4.036-2 (11/23/95)
          Added code to pad output file to nearest 80-byte boundary
          with blanks, because SAS/PC is picking up garbage obs in
          some cases (e.g., when NOCOR NOCOV is specified).
**/

void writeOutputDatafile(void){
  struct namestr oxns;
  double out_est,out_status;
  charVarName out_name;
  int nthetx,i,j,namidx,nconc;
  double *indx;

  /* Initialize some variables */
  nthetx = 0;
  nconc = 0;
  for(i=0; i<=8; i++)
    pres6[nthetx++] = i;

  /* Model parameters */
  memcpy(H->names[0],"DELTA   ",8);
  memcpy(H->names[1],"THALF   ",8);
  memcpy(H->names[2],"NU      ",8);
  memcpy(H->names[3],"M       ",8);
  memcpy(H->names[4],"TAU     ",8);
  memcpy(H->names[5],"GAMMA   ",8);
  memcpy(H->names[6],"ALPHA   ",8);
  memcpy(H->names[7],"ETA     ",8);

  C->theta[0] = E->delta; 
  C->theta[1] = E->tHalf; 
  C->theta[2] = E->nu;
  C->theta[3] = E->m; 
  C->theta[4] = L->tau; 
  C->theta[5] = L->gamma;
  C->theta[6] = L->alpha; 
  C->theta[7] = L->eta;

  /* */
  for(i=9; i<=C->p+8; i++)
    if(C->status[i]==1 || C->status[i+C->p+1]==1 || C->status[i+2*C->p+2]==1){
      nconc++;
      pres6[nthetx++] = i;
    }

  
  if(H->fixmu==1)
    C->status[8] = 1;
  else if(H->fixmu==2)
    C->status[C->p+9] = 1;
  else if(H->fixmu==3)
    C->status[2*C->p+10] = 1;

  pres6[nthetx++] = C->p+9;

  for(i=9; i<=nconc+8; i++)
    pres6[nthetx++] = pres6[i]+C->p+1;

  pres6[nthetx++] = 2*C->p+10;

  for(i=9; i<=nconc+8; i++)
    pres6[nthetx++] = pres6[i]+2*C->p+2;

  if(H->nocov==0)
    xvputi(nthetx+3);
  else
    xvputi(3);

  /* Clear the oxns namestr */
  memset(&oxns,0,sizeof(oxns));
  
  /* Set the defaults */
  oxns.ntype = 2;
  oxns.nlng = 8;
  memset(oxns.nname,' ',VAR_NAME_LENGTH);
  memcpy(oxns.nname,"_NAME_  ",8);
  memset(oxns.nlabel,' ',LABEL_LENGTH);
  memset(oxns.nform,' ',VAR_NAME_LENGTH);
  memset(oxns.niform,' ',VAR_NAME_LENGTH);

  /* */
  xvputd(&oxns,out_name);
  oxns.ntype = 1;
  oxns.nlng = 8;
  memcpy(oxns.nname,"_EST_   ",8);
  xvputd(&oxns,&out_est);
  memcpy(oxns.nname,"_STATUS_",8);
  xvputd(&oxns,&out_status);

  if(H->nocov==0) {
    for(i=0; i<=9+nconc; i++) {
      j = pres6[i];
      memcpy(oxns.nname,H->names[j],VAR_NAME_LENGTH);
      memcpy(oxns.nlabel,H->labl[j],LABEL_LENGTH);
      xvputd(&oxns,&line6[j]);
    }

    namidx = 0;

    for(i=nconc+10; i<=2*nconc+9; i++) {
      j = pres6[i];
      namidx = namidx+1;
      memset(oxns.nname,' ',VAR_NAME_LENGTH);

      if(namidx<10)
	memcpy(oxns.nname,"C0      ",VAR_NAME_LENGTH);
      else
	memcpy(oxns.nname,"C       ",VAR_NAME_LENGTH);

      namfix(oxns.nname,namidx);
      memcpy(H->names[j],oxns.nname,VAR_NAME_LENGTH);
      memcpy(oxns.nlabel,H->labl[j],LABEL_LENGTH);
      xvputd(&oxns,&line6[j]);
    }
    memset(oxns.nname,' ',VAR_NAME_LENGTH);
    memcpy(oxns.nname,"L0      ",8);
    memset(oxns.nlabel,' ',LABEL_LENGTH);
    xvputd(&oxns,&line6[2*C->p+10]);
    namidx = 0;

    for(i=2*nconc+11; i<=3*nconc+10; i++) {
      j = pres6[i];
      namidx = namidx+1;
      memset(oxns.nname,' ',VAR_NAME_LENGTH);

      if(namidx<10)
	memcpy(oxns.nname,"L0      ",8);
      else
	memcpy(oxns.nname,"L       ",8);

      namfix(oxns.nname,namidx);
      memcpy(H->names[j],oxns.nname,VAR_NAME_LENGTH);
      memcpy(oxns.nlabel,H->labl[j],LABEL_LENGTH);
      xvputd(&oxns,&line6[j]);
    }
  }else{
    namidx = 0;

    for(i=nconc+10; i<=2*nconc+9; i++) {
      j = pres6[i];
      namidx = namidx+1;
      memset(oxns.nname,' ',VAR_NAME_LENGTH);
      if(namidx<10)
	memcpy(oxns.nname,"C0      ",8);
      else
	memcpy(oxns.nname,"C       ",8);

      namfix(oxns.nname,namidx);
      memcpy(oxns.nname,H->names[j],VAR_NAME_LENGTH);
    }

    namidx = 0;

    for(i=2*nconc+11; i<=3*nconc+10; i++) {
      j = pres6[i];
      namidx = namidx+1;
      memset(oxns.nname,' ',VAR_NAME_LENGTH);

      if(namidx<10)
	memcpy(oxns.nname,"L0      ",8);
      else
	memcpy(oxns.nname,"L       ",8);

      namfix(oxns.nname,namidx);
      memcpy(H->names[j],oxns.nname,VAR_NAME_LENGTH);
    }
  }
  xvpute();

  for(i=0; i<C->Ntheta; i++)
    MKMISS(line6[i]);

  memset(out_name,' ',VAR_NAME_LENGTH);
  memcpy(out_name,"G1FLAG  ",8);
  out_est = E->g1flag;
  MKMISS(out_status);

  xvput();

  memcpy(out_name,"FIXDEL0 ",8);
  out_est = E->delta0;

  xvput();

  memcpy(out_name,"FIXMNU1 ",8);
  out_est = E->mNuOne;

  xvput();

  memcpy(out_name,"G3FLAG  ",8);
  out_est = L->g3flag;

  xvput();

  memcpy(out_name,"FIXGE2  ",8);
  out_est = L->g_two;

  xvput();

  memcpy(out_name,"FIXGAE2 ",8);
  out_est = L->ga_two;

  xvput();

  indx = H->cov;

  for(namidx=0; namidx<nthetx; namidx++) {
    j = pres6[namidx];
    memcpy(out_name,H->names[j],VAR_NAME_LENGTH);
    out_est = C->theta[j];

    if(C->status[j]==1){
      out_status = ONE;
    }else{
      out_status = ZERO;
    }

    if(H->nocov==0){
      if(C->status[j]==1){
	for(i=0; i<C->Ntheta; i++){
	  if(C->status[i]==1){
	    line6[i] = *indx++;
	  }else{
	    line6[i] = ZERO;
	  }
	}
      }else{
	for(i=0; i<C->Ntheta; i++){
	  line6[i] = ZERO;
	}
      }
    }
    xvput();
  }
  i = 80-ftell(outputDataFile)%80;
  if(i==80)
    return;

  for(; i; i--)
    fputc(' ',outputDataFile);

}
