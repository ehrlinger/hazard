#include <string.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include "structures.h"
#include "hzdinc.h"
#include "hzfmis.h"

struct stmtstr *stmtinit(int howmany){
#ifdef DEBUG
     printf("stmtinit\n");
#endif /*DEBUG*/

  int numbytes,namelen,i;
  struct stmtstr *new;

  /* Calculate the size of the structure */
  numbytes = sizeof(struct stmtstr)+(howmany-1)*sizeof(union stmtval);

  /* allocate that memory */
  new = (struct stmtstr *)malloc(numbytes);

  /* Copy the current string in yytext into the new structure */
  memcpy(new->name,yytext,VAR_NAME_LENGTH);

  /* Clear the remainder of the string */
  for(namelen=strlen(yytext); namelen < VAR_NAME_LENGTH; namelen++)
    new->name[namelen] = ' ';

  /* Initialize the opts variable in the structure to N */
  memset(new->opts,'N',64);
  
  /* */
  for(i=0; i < howmany; i++)
    hzfmis(&new->parm[i].num);

  /* set structure variables */
  new->size = howmany;
  new->next = NULL;

  return new;
}
