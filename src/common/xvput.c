#include <string.h>
#include <math.h>
#include "hzdinc.h"

void xvput(void){
  double fpno;
  int i,mant;
  int exponent,fpdigit,negative;
  unsigned char fpbyte[8];

  for(i=0; i<xvputcnt; i++) {
    if(xvputptr[i].type==2)
      fwrite(xvputptr[i].xvar,(size_t)xvputptr[i].leng,1,outputDataFile);
    else if(memcmp(xvputptr[i].xvar,MISSING,sizeof(double))==0)
      fwrite("\x2e\0\0\0\0\0\0\0",(size_t)xvputptr[i].leng,
	     1,outputDataFile);
    else {
      exponent = 64;
      fpno = *(double *)xvputptr[i].xvar;
      if(fpno<ZERO) {
	negative = 128;
	fpno = -fpno;
      }else
	negative = 0;

      while(fpno>=ONE){
	fpno /= 16.0e0;
	exponent++;
      }

      if(fpno==ZERO)
	exponent = 0;
      else
	while(fpno<0.0625e0) {
	  fpno *= 16.0e0;
	  exponent--;
	}
      if(exponent>127) {
	exponent = 127;
	fpno = 0.0625e0;
      } else if(exponent<0) {
	exponent = 0;
	fpno = 0.0625e0;
      }
      fpbyte[0] = (unsigned int)(exponent+negative);
      for(mant=1; mant<=7; mant++) {
	fpno = fpno*256.0e0;
	fpdigit = floor(fpno);
	fpbyte[mant] = (unsigned int)fpdigit;
	fpno = fpno-fpdigit;
      }
      fwrite(fpbyte,(size_t)xvputptr[i].leng,1,outputDataFile);
    }
  }
}
