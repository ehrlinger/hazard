#include "structures.h"
#include <string.h>

void setvar(int which,char *name){
#ifdef DEBUG
     printf("setvar\n");
#endif /*DEBUG*/

  int namelen;

  memcpy(current->parm[which-1].var,name,VAR_NAME_LENGTH);
  for(namelen=strlen(name); namelen<VAR_NAME_LENGTH; namelen++)
    current->parm[which-1].var[namelen] = ' ';
}
