#ifndef HAZPRED_H
#define HAZPRED_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "structures.h"
#define HZPVER 400

struct hzpstr {
  double Surviv,earlyS,constS,lateS;
  double Hazard,earlyH,constH,lateH;
  double CL_L_S,CL_U_S,CL_L_H,CL_U_H;
  double CLimit;
  double Pvalue;
  double Znormal;
  short int noCL;
  short int Hflag,Sflag;
};

#ifdef HAZPRED
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN struct hzpstr *H;

EXTERN struct namestr *ID_ns;
EXTERN struct namestr *data_ns;
EXTERN struct namestr *inhaz_ns;
EXTERN struct namestr *out_ns;
EXTERN struct xvgetstr *data_xv;
EXTERN struct xvgetstr *inhaz_xv;
EXTERN FILE *infile;
EXTERN FILE *hazfile;
EXTERN FILE *outfile;
EXTERN long data_ln;
EXTERN long inhaz_ln;
EXTERN size_t infilect;
EXTERN size_t nvars;
EXTERN long obscnt,totalobs;
EXTERN char **buffer;
EXTERN char *id_curr;
EXTERN char *id_prev;
EXTERN short *buftyp;
EXTERN charVarName in_name;
EXTERN double in_est,in_status;
EXTERN charVarName *names,*namev;
EXTERN charVarName tname;
EXTERN char msgbfr[80];
EXTERN long idleng;
EXTERN logical termin,errlast,gotID;
EXTERN FILE *outputDataFile;
EXTERN struct hzpstr HZPstr;
EXTERN long int i;
EXTERN long int indexp;
EXTERN long int j;
EXTERN long int ji;
EXTERN long int j1p0;
EXTERN long int j1p1;
EXTERN long int j1pp;
EXTERN long int pj1;
EXTERN long int pp1;
EXTERN long int jind;
EXTERN long int mp;
EXTERN long int optmz;
EXTERN double Time;
EXTERN double lnTime;
EXTERN double lnMu[3];
EXTERN double lnCf[3];
EXTERN double lnHf[3];
EXTERN double Cf[3];
EXTERN double Hf[3];
EXTERN double sqrEps;
EXTERN double lnCf_;
EXTERN double lnHf_;
EXTERN double tempj;
EXTERN double Cf_;
EXTERN double Hf_;
EXTERN double *dCf;
EXTERN double *dHf;
EXTERN double *p1;
EXTERN double *p2;
EXTERN double CXsX;
EXTERN double HXsX;
EXTERN double cumHaz;
EXTERN double Z;
EXTERN double Zl;
EXTERN double Zu;
EXTERN double seZ;
EXTERN double DlnG[8];
EXTERN double DlnSG[8];
EXTERN double step[8];

EXTERN struct early SaveE;
EXTERN struct late SaveL;

#define Chgd_Time 1
#define Chgd_Obs  2
#define Chgd_Both 3

#undef EXTERN
#endif /* HAZPRED_H */
