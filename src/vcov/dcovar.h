#ifndef DCOVAR_H
#define DCOVAR_H

void DCOVAR(double *beta,long nvar,logical noshap,double fx,long *index,
	    double *theta,double *d2ll,double *cov,double *corr, 
	    double *sd, double *znorm,double *pnorm,long *flag,
	    logical *stopt);
#endif /* DCOVAR_H */
