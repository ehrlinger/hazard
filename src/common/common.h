#ifndef COMMON_H
#define COMMON_H

#include "structures.h"

/* We'll define variables common to both hazard and hazpred in this
   header file. This should make the other libraries independant of
   the hazard header file.
*/
typedef double xtended;

struct params{
  double dnmrlo,dnmrhi;
  double fdheta;
  double lsralp,lsrbta;
  double um0scl;
  double normlb[5],normlr,lsr2pi;
};
struct hzropt {
  double typf,stptol,gradtl,maxstp,fplus,rcondn,condno;
  double fc,eta,ivar;
  double ostep;
  double ordmax;
  double *orders,*swordr;
  int *rstvec;
  struct rsttbl *rsttbl;
  int fdigit,N,itrlmt,prtcod,iret,itrcnt,trmcod,consec;
  int orderv,nosw,restct,bdlvar,bdlcnt;
};

struct hzrstr {
  struct early e;
  struct constant c;
  struct late l;
  double Tmax;
  double factor;
  double *Mu[4];
  double slike;
  int mode,fixmu,ivar,nfncts;
  int ierr;
  logical weibul,truhes,newton,anhess,ihpass,angrad;
  logical fixg1,fixg3;
  int nocor,nocov;
  int *index;
  double *lng1,*lng2,*lng3;
  double *lnsg1,*lnsg3;
  double *lng1ct,*lng2ct,*lng3ct;
  double *lng1st,*lng2st,*lng3st;
  double *lnmu1,*lnmu2,*lnmu3;
  int *indxx,*indxp,*indxt;
  double *d2llad,*d2ll,*d1ll1,*d1ll2,*d1ll3;
  double *d2ll11,*d2ll12,*d2ll13,*d2ll22,*d2ll23,*d2ll33;
  double *beta,*x;
  int *indx,*indx2;
  double *cov,*corr,*sd,*znorm,*pnorm,*wk;
  int *flag;
  double *lvec,*x0,*xc,*xplus,*gc,*gplus,*svec,*yvec,*sx,*typx;
  int pj1,pp1,pp7,mp;
  char chgflg[12];
  double sle,sls;
  int sw,bw,f,mov,nps,npq,done,only1,step,retry,lstvar,N;
  charVarName vstep,fstep;
  double zstep,pstep;
  int swadel,hpsn,fdone;
  int mxstop,mxstep,mxvars;
  double *stdErr,*zvalue,*pvalue,*qx,*qtols;
  int *nx,*nm1dx,*moves,*mxmove,*flags;
  int maxn,phase1,phase2,phase3;
  charVarName *names,*namex;
  charLabel *labl,*lablx;
  logical label;
  int cnt,cntp[4];
  struct hzropt opt;
};


#ifdef HAZARD
#define EXTERN
#else
#ifdef HAZPRED
#define EXTERN
#else
#define EXTERN extern
#endif /* HAZPRED */
#endif /* HAZARD */

EXTERN struct namestr *T_ns;
EXTERN struct params Params;
EXTERN struct {
  xtended lnlike;
  int I,Im1p7;
  double lmu[4];
  double d1llp[4],d2llij[4][4];
  enum {SETCOE,CONSTP,CONSRV,DLLIKE,D2LCON} entry;
  logical dograd,dohess;
} G;

#define ISMISS(x) (memcmp((char *)&x,MISSING,sizeof(double))==0)
#define CLEAR(p) memset(p,' ',sizeof(p))

#define QNEG(x) (-(x))
#define QSUM(x,y) (x)+(y) 
#define QMUL(x,y) (x)*(y) 
#define QDIV(x,y) (x)/(y) 

#define QEXTD(x) hzr_qextd(x)
#define QTOD(x) hzr_qtod(x)

EXTERN struct hzrstr HZRstr;
EXTERN double gte,gteva;


#undef EXTERN
#endif /* COMMON_H */
