#ifndef DQSTAT_H
#define DQSTAT_H

void DQSTAT(double *chol,int nvar,int indx,int iphas,double *qbeta,
	    double *qse,double *qz,double *qp,int *qflag,double *qtol,
	    double *d2llad,double *wk);

#endif /* DQSTAT_H */
