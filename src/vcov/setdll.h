#ifndef SETDLL_H
#define SETDLL_H

void DAGRAD(double *d1ll,int nvar);
void DAHESS(double *d2ll,int nvar);
void d2lcon(double *d2ll);
void DLLADD(int indx,int iphas,double *d1llad,double *d2llad);

#endif /* SETDLL_H */
