#include <string.h>
#include <stdio.h>

void hzf_col(int column){
  if(column>0)
    printf("@%d,",column);
  else if(column<0)
    printf("+%d,",-column);
  else
    printf(";");
}
