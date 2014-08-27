#include "structures.h"

void setopt(int which){
#ifdef DEBUG
     printf("setopt\n");
#endif /*DEBUG*/

  current->opts[which-1] = 'Y';
}
