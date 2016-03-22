#include <string.h>
/*
  yywrap()
  When the lexer encounters an end of file it calls yywrap. If yywrap
  returns 0, the lexer continues scanning, if it returns 1, it 
  returns a 0 to report the EOF condition. The packaged yywrap always
  returns a 1.

  Since this is the only routine resolved by the lex library, we
  can replace it and not have to worry about linking in the correct 
  lib (lex needs -ll, flex needs -lfl).
 */

int yywrap(void){
  return(1);
}
