#ifndef UMSTOP_H
#define UMSTOP_H


void UMSTOP(double *xc,double *xplus,double *gplus,double *sx,
	    double fplus, double typf,double gradtl,double stptol,
	    int iret,int itrcnt, int itrlmt,int *trmcod,
	    int *consec,logical maxtkn,int n);

#endif /* UMSTOP_H */
