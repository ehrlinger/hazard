#ifndef HZRINC_H
#define HZRINC_H

/******************************************************************* 
    HAZARD Common External Definitions                             
    All ANSI C Compilers and Operating Environments                    
    01/05/95 -- 4.0(0)                                                 
  ******************************************************************/

#include <hzdinc.h>
#include "hazard.h"


struct rsttbl {
  charVarName rstnam;
  int rstvar;
  int rstctr;
};

#endif /* HZRINC_H */
