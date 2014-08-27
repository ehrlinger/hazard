#ifndef SETOBJ_H
#define SETOBJ_H
#include <structures.h> /* Defines the logical data type */

#ifdef SETOBJ_DEFS
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN struct {
  xtended lnlike;
  double lfactr;
  logical facall;
} Setobj;

void DHAZRD(double *beta,double *fx,long nvar);
void DCEOBJ(double *fx);
void DCEOBJ_main_logic(double *fx);
logical DCEOBJ_obs_loop(void);

#undef EXTERN
#endif /* SETOBJ_H */
