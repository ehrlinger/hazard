#ifndef DCOVAR_H
#define DCOVAR_H

void DCOVAR(double *beta,int nvar,logical noshap,double fx,int *index,
	    double *theta,double *d2ll,double *cov,double *corr, 
	    double *sd, double *znorm,double *pnorm,int *flag,
	    logical *stopt);
#endif /* DCOVAR_H */
