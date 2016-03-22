#include <string.h>
#include <hzdinc.h>
#include <hzrinc.h>
#include "hazard.h"
#include <stmtopts.h>
#include <notspec.h>
#include <stmtfld.h>
#include <xpmode.h>
/****************************************************************/
/* HAZPPRC called from STMTPRC                                  */
void hazpprc(void){
    /*  I would realy like to enum these stmtopts parameters to 
	make them human legible.  */
  if(stmtopts(1))
    H->nocov = 1;
  if(stmtopts(2))
    H->nocor = 1;
  if(notspec(3))
    ispec(12) = 2;
  else if(stmtfld(3)==ZERO) {
    ispec(12) = 1;
    O->itrlmt = 0;
  } else if(stmtfld(3)<ZERO)
    ispec(12) = 3;
  else {
    ispec(12) = 1;
    O->itrlmt = stmtfld(3);
  }
  if(stmtopts(3))
    prnt = 1;
  if(stmtopts(4))
    ispec(17) = 1;
  if(stmtopts(5))
    ispec(18) = 1;
  if(stmtopts(26))
    H->newton = FALSE;
  if(stmtopts(28)) {
    H->angrad = FALSE;
    H->anhess = FALSE;
  }
  if(stmtopts(29))
    xpmode(0);
  if(stmtopts(27))
    H->truhes = FALSE;
  if(notspec(4))
    ispec(13) = 2;
  else {
    temp_fl = stmtfld(4);
    if(temp_fl<3.0e0)
      ispec(13) = 3;
    else if(temp_fl>14.0e0)
      ispec(13) = 3;
    else {
      ispec(13) = 1;
      O->condno = temp_fl;
    }
  }
}
