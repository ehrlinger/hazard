#ifndef FDGRAD_H
#define FDGRAD_H

void FDGRAD(double *xc,double fc,double *sx,double *g,
	    void (*fn)(double *,double *,long),double eta,long n);
#endif /* FDGRAD_H */
