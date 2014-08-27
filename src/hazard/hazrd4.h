#ifndef HAZRD4_H
#define HAZRD4_H
/*
  Definition location for hazrd4.c file.
*/

#ifdef HAZRD4
#define EXTERN 

/* We may be able to get rid of these static definitions */
double hval;
double rz,rp;
long stepk1,stepk2,stepk3,h4nvar;
long stepf,movrem,nrow;
long ri,rset,rbase,rvar;
logical hz4stop,adone,noshap,stopt;
logical faststop;
char cflag1,cflag2,cflag3,cflag4;

void HZ4LOOP(void);
void STPWISE(void);
void STEPPRT(void);
void SPRT_L(void);
void SPRT(void);
void SHDR_L(void);
void SHDR(void);
void SPRT2_L(void);
void SPRT2(void);
void SHDR2_L(void);
void SHDR2(void);
void SPRT3_L(void);
void SPRT3(void);
void SHDR3_L(void);
void SHDR3(void);
void HAZ4TRM(long);
void RSTRICT(void);
void RSTBACK(void);
void RSTBSET(void);
void RSTRPHZ(long);
void RSTBPHZ(long);
void FAST(void);
void FASLOOP(void);
void FASTSET(long,long);
void FASNOTE(void);
void FASTPRT(void);
void FPRT_L(void);
void FPRT(void);
void FHDR_L(void);
void FHDR(void);
void FASTTRM(void);
void FINEMOD(void);
void FINCMOD(void);
void FINLMOD(void);
void FINPARM(char *,long,double);
void MTRXPRT(double *);
void MTRXHDR(void);
void HAZ5TRM(void);

#else
#define EXTERN extern
#endif /* HAZRD4 */

EXTERN void HZ4I(struct common *);
EXTERN void HZ4G(void);
EXTERN void HZ5G(void);

#undef EXTERN

#endif /* _HAZRD4_H_ */
