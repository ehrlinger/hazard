#include <string.h>
#include <stdio.h>
#include "structures.h"

void errtext(void){
  printf("$ERROR: Stmt %d, near \'%s\'\n",yylnctr,yytext);
}

