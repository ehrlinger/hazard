#include "structures.h"

void setparm(int which,double towhat){
#ifdef DEBUG
     printf("setparm\n");
#endif /*DEBUG*/

  current->parm[which-1].num = towhat;
}
