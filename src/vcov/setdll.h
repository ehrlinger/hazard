#ifndef SETDLL_H
#define SETDLL_H

void DAGRAD(double *d1ll,long nvar);
void DAHESS(double *d2ll,long nvar);
void d2lcon(double *d2ll);
void DLLADD(long indx,long iphas,double *d1llad,double *d2llad);

#endif /* SETDLL_H */
