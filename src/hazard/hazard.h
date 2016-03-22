#ifndef HAZARD_H
#define HAZARD_H

#define HZRVER 400

#include <structures.h>
#include <common.h>

#include <hzrinc.h> /* Hazard specific structures */
#include <hzdinc.h>


/*
  Definition location for hazard.c file.
*/
#ifdef HAZARD
#define EXTERN 
#else
#define EXTERN extern
#endif /* HAZARD */

/* We are looking for variables unique to hazard */
EXTERN struct hzrstr *H;

/* Global variables */
/* We probably should break these up, so they are grouped logically */ 
EXTERN logical outhaz;

EXTERN int prnt,nrow;

EXTERN double temp_fl;
EXTERN double T,C1,C2,C3,CT;
EXTERN double LC,WT;

EXTERN struct namestr **risk_ns,**names_ns;

EXTERN double *stval;
EXTERN double *line6;
EXTERN int *nomovs;
EXTERN int *pres6;

EXTERN struct namestr *C1_ns;
EXTERN struct namestr *C2_ns;
EXTERN struct namestr *C3_ns;
EXTERN struct namestr *CT_ns;
EXTERN struct namestr *LC_ns,*WT_ns;
EXTERN int k,pp7,iexc,icxc,ilxc;
EXTERN int ic10,ic20,ic30,obsix;

EXTERN int im1p,jp,maxnv2,maxnt2,memory;
EXTERN int oldbas,newbas,bascov,basopt;

EXTERN int ilng1,ilngc1,ilnsg1,ilng2,ilngc2,ilng3,ilngc3,ilnsg3;
EXTERN int ilngs1,ilngs2,ilngs3;
EXTERN int ilnmu1,ilnmu2,ilnmu3;
EXTERN int iindxx,iindxp,iindxt;
EXTERN int id2lla,id2ll,id1l1,id1l2,id1l3;
EXTERN int id2l11,id2l12,id2l13,id2l22,id2l23,id2l33;
EXTERN int ibeta,ix,iindx,iindx2;
EXTERN int icov,icorr,isd,iznorm,ipnorm,iflags,iwk;
EXTERN int il,ix0,ixc,ixplus,igc,igplus,is,iy,isx,itypx;
EXTERN int Ispec[20],T_status[8];
#define ispec(x) Ispec[(x)-1]
#define t_status(x) T_status[(x)-1]
EXTERN logical swnewt,swhess;
EXTERN logical svnewt,svhess,hz2stop;

EXTERN logical semerr;
EXTERN double tally;
EXTERN double tmin,c1min,c2min,c3min,ctmin;
EXTERN double lcmin,wtmin;
EXTERN double c1max,c2max,c3max,ctmax;
EXTERN double lcmax,wtmax;
EXTERN double rtcen,intcen,nosubs,events;
EXTERN int tdel,c1del,c2del,c3del,ctdel,alldel;
EXTERN int lcdel,wtdel,lcens;
EXTERN int mtdel,mc1del,mc2del,mc3del,mctdel;
EXTERN int mlcdel,mwtdel;
EXTERN int c1flag,c2flag,c3flag,c2chg;
EXTERN int delet,ivent,mdelet,del,mdel;
EXTERN charVarName tname,c1name,c2name,c3name,ctname;
EXTERN charVarName lcname,wtname;
EXTERN charVarName phasenm;
EXTERN charLabel c1labl,c2labl,c3labl,ctlabl,tlabl;
EXTERN charLabel lclabl,wtlabl;
EXTERN double *concom,*contal,*conmin,*conmax,*comean;
EXTERN charVarName *risk,*faze,*varnam,*inex;
EXTERN int *conmis,*iearly,*iconst,*ilate;
EXTERN char msgbfr[80];
/* From hzrcor.c */
EXTERN double k_xmul;

EXTERN struct hzropt *O;

#undef EXTERN

#endif /* HAZARD_H */
