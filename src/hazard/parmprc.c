
#include "hazard.h"
#include "setparmno.h"
#include "setprmf.h"
#include "stmtopts.h"
#include <notspec.h>
#include <hzdinc.h>

/****************************************************************/
/* PARMPRC called from STMTPRC                                  */
void parmprc(void){
  setparmno(16,1,1,&H->e.muE);
  setprmf(17,2,1,7,&H->e.delta);
  setprmf(18,3,2,8,&H->e.tHalf);
  setprmf(19,4,3,9,&H->e.nu);
  setprmf(20,5,4,10,&H->e.m);
  setparmno(21,6,2,&H->c.muC);
  setparmno(22,7,3,&H->l.muL);
  setprmf(23,8,5,12,&H->l.tau);
  setprmf(24,9,6,13,&H->l.gamma);
  setprmf(25,10,7,14,&H->l.alpha);
  setprmf(26,11,8,15,&H->l.eta);
  
  /*  I would realy like to enum these stmtopts parameters to 
      make them human legible.  */
  if(stmtopts(6))
    H->weibul = TRUE;
  if(stmtopts(11))
    E->mNuOne = TRUE;
  if(stmtopts(16))
    L->g_two = TRUE;
  if(stmtopts(17))
    L->ga_two = TRUE;
  /* If NU and FIXMNU1 are specified, but M and FIXM are not,
     set M to zero so that SETG1 will not override NU. */
  if(notspec(20) && !notspec(19) && !stmtopts(10) && E->mNuOne)
    H->e.m = ZERO;
}
