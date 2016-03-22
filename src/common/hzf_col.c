#include <string.h>
#include <stdio.h>

void hzf_col(int column){
  if(column>0)
    printf("@%ld,",column);
  else if(column<0)
    printf("+%ld,",-column);
  else
    printf(";");
}
