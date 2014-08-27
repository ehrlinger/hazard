#include "hzdinc.h"

/*
 * isblank(char *strg, long leng) overides the C standard isblank.
 * 
 * Standard C isblank checks a single character for whitespace. This goes through a 
 * character string. We probably should rename this to isblank_string...
 */
logical isBlank(char *strg,long leng){
#ifdef DEBUG
	printf("isblank\n");
#endif /*DEBUG*/

  for(; leng; leng--,strg++)
    if(*strg!=' ')
      return FALSE;
  return TRUE;
}
