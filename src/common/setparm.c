#include <string.h>
#include "structures.h"

void setparm(int which,double towhat){
  current->parm[which-1].num = towhat;
}
