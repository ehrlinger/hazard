#include <string.h>
/******************************************************************** 
    HAZARD Core Routines                                              
    All ANSI C Compilers and Operating Environments                    
    10/xx/93 -- 4.0(0)                                                 
  ********************************************************************/

#include <float.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "setg1.h"
#include "setg3.h"
#include "dtrsmu.h"
#include "setcoe.h"


#include "common.h"

#ifdef QEMULATE

double hzr_qtod(xtended a){
  double x;

  x = a.hi;
  return x;
}
#else

double hzr_qtod(xtended a){
  double x;

  x = a;
  return x;
}

#endif
