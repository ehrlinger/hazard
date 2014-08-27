#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <setjmp.h>
#include <stdio.h>
#include <math.h>

#include "sas.h"

/* Define TRUE and FALSE, 
   I'm sure these are defined somewhere else. */

/* TRUE and FALSE get defined so many times, */
/* let's not get in the way of other definitions. */
#ifndef TRUE
#define	TRUE  1
#endif
#ifndef FALSE
#define	FALSE 0
#endif
#ifndef ERR
#define	ERR -1
#endif
#ifndef OK
#define	OK  0
#endif

typedef short int logical;

#define HZDVER 400


#ifndef LOGe
#define LOGe(x) log(x)
#endif
#ifndef ePOW
#define ePOW(x) exp(x)
#endif
/* Why are these necessary? */
#define ZERO 0.0e0
#define ONE 1.0e0
#define TWO 2.0e0
#define PMIN 0.0001e0

#define ErrorPassBack(x) if(Common.errorno) hzd_Error(x,0)
#define ErrorReturn(e,v) { Common.errorno = e; return v; }
#define ErrorReset Common.errorno=0

/**
 * Structure containing Early Phase hazard model variables.
 */
struct early {
  double muE;  /* early phase intercept, equivalenced to mu(1) */
  double delta;
  double tHalf;
  double rho;
  double nu;
  double m;
  int g1flag;
  logical delta0;
  logical deltaN;
  logical mNuOne;
};

/**
 * Structure containing Constant Phase hazard model variables.
 */
struct constant{
  double muC;  /* constant phase intercept, equivalenced to mu(2)  */
};

/**
 * Structure containing Late Phase hazard model variables.
 */
struct late {
  double muL; /* late phase intercept, equivalenced to mu(3) */
  double tau;
  double gamma;
  double alpha;
  double eta;
  int g3flag;
  logical g_two;
  logical ga_two;
};

/**
 * Structure containing calculated constant machine
 * variables. We calculate machine constants to aid 
 * the portablilty of the code.
 * 
 * 12/9/03 This should be handled by autoconf. JE
 */
struct machn {
  double zero;
  double half;
  double one;
  double two;
  double absMin;
  double absMax;
  double negMax;
  double Eps;
  struct {
    double C0,C1,C2;
    double D1,D2,D3;
  } norinv;
};

struct intfc {
  void *(*malloc)(size_t);
};

struct common {
  struct Version *version;
  struct early *early;
  struct constant *constant;
  struct late *late;
  struct machn *machn;
  struct intfc *intfc;
  struct hzrstr *hzrstr;
  struct hzpstr *hzpstr;
  short int *status;
  double *theta;
  double *obs;
  double *cov;
  long errorno;
  long Nobs;
  long Ntheta;
  long p;
  short int phase[4];
  char errflg[48];
  jmp_buf errtrap;
};

struct hazg1 {
  double T;
  double lnG1;
  double lnSG1;
};

struct hazg3 {
  double T;
  double lnG3;
  double lnSG3;
};

struct Version {
  short int core_ver;
  short int hzdc_ver;
};

struct namestr {
  short ntype;
  short nhfun;
  short nlng;
  short nvar0;
  charVarName nname;
  charLabel nlabel;
  charVarName nform;
  short nfl;
  short nfd;
  short nfj;
  char nfill[2];
  charVarName niform;
  short nifl;
  short nifd;
  long npos;
  char rest[52];
};

union stmtval{
  double num;
  charVarName var;
};

struct stmtstr {
  struct stmtstr *next;
  long size;
  char name[VAR_NAME_LENGTH];
  char opts[64];
  union stmtval parm[1];
};

struct xvgetstr {
  struct namestr *xvns;
  void *xvar;
};

struct xvputstr {
  void *xvar;
  long type,leng;
};
struct lnlim {
  double two;
  double twotwo;
  double absMin;
  double absMax;
  double divBtm;
  double divTop;
  double fpRdx;
  double onePlsRdx;
  double oneMinRdx;
  double maxOneSig;
  int bndSeries;
};

#ifdef STRUCT
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN struct lnlim LnLim;
/*------------------------------------------------------------------*/
EXTERN struct Version Version;
EXTERN struct early Early,TempE;
EXTERN struct constant Constant;
EXTERN struct late Late,TempL;
EXTERN struct machn Machn;
EXTERN struct intfc Intfc;
EXTERN struct common Common;
EXTERN struct hazg1 HazG1;
EXTERN struct hazg3 HazG3;

EXTERN struct stmtstr *anchor;
EXTERN struct stmtstr *current;
EXTERN struct stmtstr *last;
EXTERN char MISSING[sizeof(double)];

EXTERN char *yytext;
EXTERN int yylnctr;

EXTERN struct xvgetstr *xvgetptr;
EXTERN struct xvputstr *xvputptr;

EXTERN struct stmtstr *stmtptr;
EXTERN struct namestr *ns;

EXTERN struct common *C;
EXTERN struct early *E;
EXTERN struct late *L;
EXTERN struct machn *Machine;

#undef EXTERN
#endif /*  STRUCTURES_H */
