#include <string.h>
#include "hazard.h"

#include <hzfxpc.h>
#include <hzfskp.h>
#include <hzfpag.h>
#include "crpt_dl.h"
#include "crpt_d.h"
#include "crpt_l.h"
#include "crpt.h"

void concrpt(void){
  int j,k;

  if(C->p==0)
    return;
  hzfpag(7);
  H->label = FALSE;
  for(j=0; j<C->p; j++)
    if(*H->labl[j+9]!=' ') {
      H->label = TRUE;
      break;
    }
  hzfxpc("Concomitant Information:",24,5);
  hzfskp(1);
  hzfxpc("========================",24,5);
  hzfskp(3);
  mdel = 0;
  for(k=0; k<C->p; k++)
    if(conmis[k]>0) {
      mdel = 1;
      break;
    }
  if(mdel>0)
    if(H->label)
      crpt_dl();
    else
      crpt_d();
  else
    if(H->label)
      crpt_l();
    else
      crpt();
}
