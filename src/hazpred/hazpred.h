/**
 * @file hazpred.h
 * @brief HAZPRED-specific definitions: hzpstr, HAZPRED/EXTERN macro.
 *
 * @see hazpred.c for the binary entry point.
 * @see FORTRAN origin: `pred91/dpred` (DPRED subroutine).
 */
#ifndef HAZPRED_H
#define HAZPRED_H

#include "structures.h"
#define HZPVER 400

/**
 * @brief Per-observation prediction outputs from HAZPRED.
 *
 * Filled by the dpred-equivalent computation for each design-matrix row.
 * Contains overall and phase-specific survivorship and hazard values,
 * plus confidence limits.
 */
struct hzpstr {
  double Surviv;    /**< Overall survivorship S(t). */
  double earlyS;    /**< Early-phase survivorship component. */
  double constS;    /**< Constant-phase survivorship component. */
  double lateS;     /**< Late-phase survivorship component. */
  double Hazard;    /**< Overall hazard H(t). */
  double earlyH;    /**< Early-phase hazard component. */
  double constH;    /**< Constant-phase hazard component. */
  double lateH;     /**< Late-phase hazard component. */
  double CL_L_S;    /**< Lower confidence limit for survivorship. */
  double CL_U_S;    /**< Upper confidence limit for survivorship. */
  double CL_L_H;    /**< Lower confidence limit for hazard. */
  double CL_U_H;    /**< Upper confidence limit for hazard. */
  double CLimit;    /**< Confidence level (e.g., 0.95). */
  double Pvalue;    /**< Two-sided p-value from Wald test. */
  double Znormal;   /**< Z-score from normal approximation. */
  short int noCL;   /**< Flag: 1 if confidence limits could not be computed. */
  short int Hflag;  /**< Flag: hazard requested in output. */
  short int Sflag;  /**< Flag: survivorship requested in output. */
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
EXTERN int data_ln;
EXTERN int inhaz_ln;
EXTERN size_t infilect;
EXTERN size_t nvars;
EXTERN int obscnt,totalobs;
EXTERN char **buffer;
EXTERN char *id_curr;
EXTERN char *id_prev;
EXTERN short *buftyp;
EXTERN charVarName in_name;
EXTERN double in_est,in_status;
EXTERN charVarName *names,*namev;
EXTERN charVarName tname;
EXTERN char msgbfr[80];
EXTERN int idleng;
EXTERN logical termin,errlast,gotID;
EXTERN FILE *outputDataFile;
EXTERN struct hzpstr HZPstr;
EXTERN int i;
EXTERN int indexp;
EXTERN int j;
EXTERN int ji;
EXTERN int j1p0;
EXTERN int j1p1;
EXTERN int j1pp;
EXTERN int pj1;
EXTERN int pp1;
EXTERN int jind;
EXTERN int mp;
EXTERN int optmz;
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
