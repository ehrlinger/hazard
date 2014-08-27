

void namfix(char *varname,long varidx){
#ifdef DEBUG
     printf("namfix\n");
#endif /*DEBUG*/

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
