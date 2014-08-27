#include "hazard.h"
/***************************************************************
 * SETREST called from main                                    *
 *                                                             *
 * Setup RESTRICTed variables.                                 *
 *                                                             *
 * There are O->restct restricted variable names passed in     *
 * through the PROC command string. Each RESTRICTed variable   *
 * name is stored in O->rsttbl[i].rstnam. We are creating a    *
 * vector (O->rsttbl[i].rstvar) of RESTRICTed variable indices *
****************************************************************/
/* Comment out this statement to print debugging messages to the
   stderr stream. This statement effects this file only */
#define NDEBUG 

void set_restrict(void){
  long i,j;

  /* Loop through the named RESTRICTed variables (i)
     matching to variable names (j)                           */
  for(i = 0; i < O->restct; i++) {
    
    /* This for loop assumes we will find a match, if not
     it will RESTRICT the C->p variable without meaning to.
     It hasn't happened, but it could if the data isn't 
     setup correctly.
    */
    for(j = 0; j < C->p &&
	  /* Compare the stored vs current variable names */
	  /* Until a match is found                       */
	  memcmp(risk[j],O->rsttbl[i].rstnam, VAR_NAME_LENGTH); j++);
      
#ifndef NDEBUG
    fprintf(stderr, "RESTRICTing %s\n", O->rsttbl[i].rstnam);
#endif /* NDEBUG */

    /* Once we have a match, add the index to the RESTRICT vector */
    O->rsttbl[i].rstvar = (j<C->p) ? (j+1) : 0;  
  }
}
