#ifndef LINESR_H
#define LINESR_H

void LINESR(double *xc,double *xplus,double *gc,double *gplus,double fc,
	    double *fplus,double *p,double *wk,double *sx,double maxstp,
	    double stptol,void (*fn)(double *,double *,long),
	    void (*grad)(double *,long),double eta,long n,long *iret,
	    logical *maxtkn,logical angrad,logical newton);

#endif /* LINESR_H */

