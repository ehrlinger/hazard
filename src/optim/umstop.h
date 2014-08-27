#ifndef UMSTOP_H
#define UMSTOP_H


void UMSTOP(double *xc,double *xplus,double *gplus,double *sx,
	    double fplus, double typf,double gradtl,double stptol,
	    long iret,long itrcnt, long itrlmt,long *trmcod,
	    long *consec,logical maxtkn,long n);

#endif /* UMSTOP_H */
