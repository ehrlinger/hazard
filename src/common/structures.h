/**
 * @file structures.h
 * @brief Global struct definitions and the STRUCT/HAZARD macro pattern.
 *
 * This header defines all global data structures for the HAZARD binary.
 * The `#ifdef STRUCT` / `#define EXTERN` pattern ensures exactly one
 * translation unit allocates storage:
 *
 * ```c
 * // In exactly ONE .c file (hazard.c or hazpred.c):
 * #define STRUCT
 * #include "structures.h"   // allocates storage
 *
 * // In all other .c files:
 * #include "structures.h"   // gets extern declarations
 * ```
 *
 * All global variables are declared at the bottom of this file using
 * the `EXTERN` macro, which resolves to nothing (allocating) or `extern`
 * (declaring) depending on whether `STRUCT` is defined.
 *
 * @note Call hzd_set_Machn() then hzd_set_LnLim() before using any
 *       math helper functions — they depend on `Machn` and `LnLim`.
 */
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
 * @brief Early-phase hazard model parameters.
 *
 * Stores the four shape parameters plus derived quantities for the G1
 * early-phase shaping function. Initialized by SETG1 / hzd_early_t2p().
 *
 * The early-phase cumulative hazard is:
 * \f$ CF_1(t) = \mu_E \cdot G_1(t;\,\delta,\,t_{1/2},\,\nu,\,M) \f$
 */
struct early {
  double muE;       /**< Early-phase scale (mu_E); solved by conservation. */
  double delta;     /**< Exponential time-transform parameter delta. B(t)=(e^{delta*t}-1)/delta. */
  double tHalf;     /**< Half-time: t at which G1 = 0.5. */
  double rho;       /**< Derived normalisation: rho = B(tHalf). Set by hzd_set_rho(). */
  double nu;        /**< G1 shape near t=0 (nu > 0: negative generic; nu < 0: mixed). */
  double m;         /**< G1 overall shape (m > 0 or m < 0 selects model family). */
  int g1flag;       /**< Flag 1–6 selecting the G1 special case (set by setg1()). */
  logical delta0;   /**< TRUE when delta is fixed at 0 (B(t) = t). */
  logical deltaN;   /**< TRUE when delta is negative. */
  logical mNuOne;   /**< TRUE when m*nu is fixed at ±1.0. */
};

/**
 * @brief Constant-phase hazard model parameter.
 *
 * The constant (time-independent background) phase contributes
 * \f$ CF_2(t) = \mu_C \cdot t \f$ to the cumulative hazard.
 */
struct constant{
  double muC;   /**< Constant-phase scale (mu_C); solved by conservation. */
};

/**
 * @brief Late-phase hazard model parameters.
 *
 * Stores the four shape parameters for the G3 late-phase shaping function.
 * Initialized by SETG3 / hzd_late_t2p().
 *
 * The late-phase cumulative hazard is:
 * \f$ CF_3(t) = \mu_L \cdot G_3(t;\,\tau,\,\gamma,\,\alpha,\,\eta) \f$
 */
struct late {
  double muL;    /**< Late-phase scale (mu_L); solved by conservation. */
  double tau;    /**< Inflection time: when the rising hazard accelerates. */
  double gamma;  /**< Acceleration rate (shape of (T/tau)^gamma term). */
  double alpha;  /**< Asymptotic shape parameter (alpha=0 → exponential limit). */
  double eta;    /**< Overall shaping power. */
  int g3flag;    /**< Flag 1–4 selecting the G3 special case (set by setg3()). */
  logical g_two;   /**< TRUE when gamma*eta is constrained to 2.0. */
  logical ga_two;  /**< TRUE when gamma*eta/alpha is constrained to 2.0. */
};

/**
 * @brief Machine floating-point constants.
 *
 * Computed at startup by hzd_set_Machn() to ensure portability.
 * All math helpers read from this struct via the `Machn` global.
 */
struct machn {
  double zero;    /**< 0.0 */
  double half;    /**< 0.5 */
  double one;     /**< 1.0 */
  double two;     /**< 2.0 */
  double absMin;  /**< Smallest positive representable double. */
  double absMax;  /**< Largest representable double. */
  double negMax;  /**< Most negative representable double. */
  double Eps;     /**< Machine epsilon (unit of rounding error, MACHEP). */
  struct {
    double C0, C1, C2;  /**< Numerator polynomial coefficients for NORINV. */
    double D1, D2, D3;  /**< Denominator polynomial coefficients for NORINV. */
  } norinv;             /**< Inverse-normal polynomial constants (from stat91/norinv). */
};

struct intfc {
  void *(*malloc)(size_t);
};

/**
 * @brief Run-wide state: error trap, parameter vectors, observation data.
 *
 * Central coordinator struct accessible from all modules. Holds the
 * optimizer's theta vector, the OBS array, error handling state, and
 * the setjmp buffer used for non-local error exits.
 */
struct common {
  struct Version *version;      /**< Binary version (core + hazard-C version). */
  struct early *early;          /**< Pointer to Early global (for pointer-based access). */
  struct constant *constant;    /**< Pointer to Constant global. */
  struct late *late;            /**< Pointer to Late global. */
  struct machn *machn;          /**< Pointer to Machn global. */
  struct intfc *intfc;          /**< Memory allocation interface. */
  struct hzrstr *hzrstr;        /**< HAZARD-specific run state. */
  struct hzpstr *hzpstr;        /**< HAZPRED-specific run state. */
  short int *status;            /**< status[i]=1 if theta[i] is actively estimated. */
  double *theta;                /**< Unconstrained optimizer parameter vector. */
  double *obs;                  /**< Flat observation array: pp7 columns per row. */
  double *cov;                  /**< Covariance matrix (Ntheta×Ntheta). */
  int errorno;                  /**< Current error code (0 = no error). */
  int Nobs;                     /**< Number of observations. */
  int Ntheta;                   /**< Number of model parameters (active + fixed). */
  int p;                        /**< Number of concomitant variables. */
  short int phase[4];           /**< phase[1..3]: 1 if that phase is in model. */
  char errflg[48];              /**< Error label string from hzd_Error() call site. */
  jmp_buf errtrap;              /**< setjmp buffer: hzd_Error() longjmps here. */
  const char *errfile;          /**< Source file of last hzd_Error() call. */
  const char *errfunc;          /**< Function name of last hzd_Error() call. */
  int errline;                  /**< Source line of last hzd_Error() call. */
};

/**
 * @brief Per-evaluation inputs/outputs for the G1 shaping function.
 *
 * Set `HazG1.T` to the evaluation time, then call hzd_ln_G1_and_SG1().
 * Results appear in `HazG1.lnG1` and `HazG1.lnSG1`.
 */
struct hazg1 {
  double T;      /**< Evaluation time t. */
  double lnG1;   /**< Output: ln(G1(t)) — log cumulative hazard shaping. */
  double lnSG1;  /**< Output: ln(SG1(t)) — log hazard shaping (time derivative). */
};

/**
 * @brief Per-evaluation inputs/outputs for the G3 shaping function.
 *
 * Set `HazG3.T`, call hzd_ln_G3_and_SG3(), read lnG3 and lnSG3.
 */
struct hazg3 {
  double T;      /**< Evaluation time t. */
  double lnG3;   /**< Output: ln(G3(t)). */
  double lnSG3;  /**< Output: ln(SG3(t)). */
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
  char nname[8];
  char nlabel[40];
  char nform[8];
  short nfl;
  short nfd;
  short nfj;
  char nfill[2];
  char niform[8];
  short nifl;
  short nifd;
  int npos;
  char rest[52];
};

union stmtval{
  double num;
  charVarName var;
};

struct stmtstr {
  struct stmtstr *next;
  int size;
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
  int type,leng;
};
/**
 * @brief Log-domain boundary constants for math helpers.
 *
 * Precomputed by hzd_set_LnLim() using values from `Machn`.
 * Defines the series-vs-direct evaluation switching thresholds
 * used by hzd_ln_eX_min1(), dlgaob(), and related functions.
 */
struct lnlim {
  double two;        /**< ln(2.0) */
  double twotwo;     /**< ln(2.0)*2.0 */
  double absMin;     /**< ln(Machn.absMin): log underflow threshold. */
  double absMax;     /**< ln(Machn.absMax): log overflow threshold. */
  double divBtm;     /**< exp(-0.1): lower ratio threshold for DLGAOB series. */
  double divTop;     /**< exp(0.1): upper ratio threshold for DLGAOB series. */
  double fpRdx;      /**< Floating-point radix (typically ln(2) for IEEE 754). */
  double onePlsRdx;  /**< ln(1 + machine_epsilon): series upper bound. */
  double oneMinRdx;  /**< ln(1 - machine_epsilon): series lower bound. */
  double maxOneSig;  /**< Upper threshold where ln(e^x - 1) = x exactly. */
  int bndSeries;     /**< Maximum series terms for convergence. */
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

/*
  Flex scanner globals are defined in lexer translation units
  (hazard_l.* / hazpred_l.*). Keep declarations-only here to avoid
  duplicate symbol definitions when scanners are regenerated in CI.
*/
extern char *yytext;
extern int yylnctr;

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
