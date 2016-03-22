#include <string.h>


void namfix(char *varname,int varidx){
  for(; *varname!=' '; varname++);
  if(varidx>=100) {
    *varname++ = '0'+(varidx/100);
    varidx = varidx%100;
  }
  if(varidx>=10) {
    *varname++ = '0'+(varidx/10);
    varidx = varidx%10;
  }
  *varname = '0'+varidx;
}
