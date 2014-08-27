#ifndef SETCOE_H
#define SETCOE_H

#ifdef SETCOE_DEFS
#define EXTERN 
#else
#define EXTERN extern
#endif

EXTERN struct {
  xtended qsumcz,qsumcj;
  double sc1pc3,c3w;
  double sumcf[4];
  double cumhct;
} Setcoe;

void setcoe(void);

#undef EXTERN
#endif /* SETCOE_H */
