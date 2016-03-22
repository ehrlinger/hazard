#include <string.h>
#include "structures.h"

void setvar(int which,char *name){
  int namelen;

  memcpy(current->parm[which-1].var,name,VAR_NAME_LENGTH);
  for(namelen=strlen(name); namelen<VAR_NAME_LENGTH; namelen++)
    current->parm[which-1].var[namelen] = ' ';
}
