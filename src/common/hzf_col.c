#include <stdio.h>

void hzf_col(long column){
#ifdef DEBUG
     printf("hzf_col\n");
#endif /*DEBUG*/

  if(column>0)
    printf("@%ld,",column);
  else if(column<0)
    printf("+%ld,",-column);
  else
    printf(";");
}
