#include <hzdinc.h>
#include <string.h>
#include "hazard.h"
#include "vfynvar.h"
#include <xvname.h>

/****************************************************************/
/* GETRISK called from main                                     */
void getrisk(void){
  long i,j;
  logical found;
  	  
  /* Copy the variable names into the risk name array */
  /* Hash the name structs into the risk_ns array */
  C->p = 0;
  for(i=0; i<k; i++) {
    found = FALSE;
    for(j=0; j<C->p && !found; j++){
      if(memcmp(risk[j],varnam[i], VAR_NAME_LENGTH)==0){
	found = TRUE;
      }
    }
    if(!found) {
      memcpy(risk[C->p],varnam[i],VAR_NAME_LENGTH);
      risk_ns[C->p] = xvname(risk[C->p]);
      vfynvar(risk_ns[C->p],risk[C->p]);
      C->p++;
    }
  }
}
