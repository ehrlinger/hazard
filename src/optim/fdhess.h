#ifndef FDHESS_H
#define FDHESS_H

void FDHESS(double *xc,double *sx,double *H,double *stpsiz,
	    double *fneigh, double fc,
	    void (*fn)(double *,double *,int),int n);

#endif /* FDHESS_H */
