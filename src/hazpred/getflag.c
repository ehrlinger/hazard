#include "hazpred.h"
#include "obsverr.h"
#include "xoget.h"
#include "xvget.h"

int getflag(char *nm,long obsnum){
  xoget(); 
  xvget();
  if(memcmp(nm,in_name,VAR_NAME_LENGTH))
    obsverr(obsnum);
  return (int)in_est;
}
