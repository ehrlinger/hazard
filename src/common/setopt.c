#include <string.h>
#include "structures.h"

void setopt(int which){
  current->opts[which-1] = 'Y';
}
