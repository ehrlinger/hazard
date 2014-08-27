#include <stdio.h>
#include "structures.h"

void errtext(void){
#ifdef DEBUG
     printf("errtext\n");
#endif /*DEBUG*/

  printf("$ERROR: Stmt %d, near \'%s\'\n",yylnctr,yytext);
}

