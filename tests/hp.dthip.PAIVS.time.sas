  OPTIONS PS=100 LS=132;
*______________________________________________________________________________;
*                                                                              ;
* !HZEXAMPLES/hp.dthip.PAIVS.time                                              ;
*                                                                              ;
* Survival after initial for pulmonary atresia with intact ventricular septum  ;
*                                                                              ;
* Time-related survival after various operations at several Zs for TV          ;
*                                                                              ;
* Birth weight 3.1 kg (median), date of admission 1991.                        ;
*______________________________________________________________________________;
*                                                                              ;
  TITLE1 "Pulmonary Atresia with Intact Ventricular Septum";
  TITLE2 "(Congenital Heart Surgeons Society; 1987 to Oct 1993)";
  TITLE3 "Death After Initial Achieved Procedure";
  TITLE4 "Time-Related Survival After Various Treatment Strategies";
*______________________________________________________________________________;
*                                                                              ;
* Model:                                                                       ;
*   Early Hazard Phase
*    LN_WTBRT MISSBTHW RVDEPCC NCANOM RVNSTVSE ATBDPV BAS
*    SHAS1GRP PVS1GRP TAP1GZ2T TAPS1ZMT PV1Z2TV
*   Constant Hazard Phase
*    RVSP RVENL TVINC_SE RAE APCOL SHNT1GRP SHAS1GRP PVS1GRP
*    TAPS1GRP TAPS1ZMT
*______________________________________________________________________________;
*                                                                              ;
* Set values for standard model incremental risk factors variables.            ;
  DATA DESIGN;
  WT_BIRTH=3.1;  /* Birth weight (kg)                                 */
  MISSBTHW=0;    /* Missing birth weight                              */
  RVDEPCC=0;     /* RV dependent coronary circulation                 */
  NCANOM=0;      /* Noncardiac anomaly                                */
  TVINC_SE=0;    /* TV incompetence (severe)                          */
  RVENL=0;       /* RV enlarged                                       */
  RAE=0;         /* Large right atrium                                */
  APCOL=0;       /* Large AP colateral arteries                       */
  ATBDPV=0;      /* Previous attempted balloon valvotomy              */
  BAS=0;         /* Previous balloon atrial septostomy                */
  SHAS1GRP=0;    /* Shunt + atrial septectomy group                   */
  TAP1GRP=0;     /* Surgical TAP without a shunt                      */
  TAPS1GRP=0;    /* Surgical TAP with a shunt                         */
  SHNT1GRP=0;    /* Shunt only                                        */
  PVS1GRP=0;     /* Surgical pulmonary valvotomy with a shunt         */
  PV1GRP=0;      /* Surgical pulmonary valvotomy without a shunt      */
  ZMTVECHO=-2;   /* Dimension (Z) of tricuspid valve                  */
  RVSP=110;      /* RV peak systolic pressure                         */
  MONTHS=6;      /* Time of observation (months)                      */
*******************************************************************************;
* Transformations of variables                                                 ;
  RVNS=0; IF TAP1GRP=1 OR PV1GRP=1 THEN RVNS=1;
  PV1=0; IF PV1GRP=1 OR PVS1GRP=1 THEN PV1=1;
* Logarithpic:                                                                 ;
  LN_WTBRT=LOG(WT_BIRTH);
* Power:                                                                       ;
  Z2MTVECH=ZMTVECHO**2;
* Interaction:                                                                 ;
  RVNSTVSE=TVINC_SE*RVNS;
  TAP1GZ2T=TAP1GRP*Z2MTVECH;
  TAPS1ZMT=TAPS1GRP*ZMTVECHO;
  PV1Z2TV=PV1*Z2MTVECH;
  YEARS=MONTHS/12;
*******************************************************************************;
* Generate "isobars" of risk factors here, but check for possible              ;
* transformations that may be needed.                                          ;
  DO ZMTVECHO=-1, -2, -3, -4;
    MULT=ABS(ZMTVECHO);
    Z2MTVECH=ZMTVECHO**2;
    OUTPUT;
  END;
  DATA DESIGN; SET DESIGN;
* Shunt alone                                                                  ;
  SHAS1GRP=0; SHNT1GRP=1; PV1GRP=0; PVS1GRP=0; TAP1GRP=0; TAPS1GRP=0;
    PLOT=2*MULT+1; PLOTLINE=1; PLTCHR='S'; PLTCL='s'; DECISION='Shunt Alone ';
    PATIENT=1; OUTPUT;
* Pulmonary valvotomy without a shunt:                                         ;
  SHAS1GRP=0; SHNT1GRP=0; PV1GRP=1; PVS1GRP=0; TAP1GRP=0; TAPS1GRP=0;
    PLOT=2*MULT+1; PLOTLINE=2; PLTCHR='V'; PLTCL='v'; DECISION='Valvot Alone';
    PATIENT=2; OUTPUT;
* Pulmonary valvotomy with a shunt:                                            ;
  SHAS1GRP=0; SHNT1GRP=0; PV1GRP=0; PVS1GRP=1; TAP1GRP=0; TAPS1GRP=0;
    PLOT=2*MULT+1; PLOTLINE=3; PLTCHR='W'; PLTCL='w'; DECISION='Valvot+Shunt';
    PATIENT=3; OUTPUT;
* Shunt alone                                                                  ;
  SHAS1GRP=0; SHNT1GRP=1; PV1GRP=0; PVS1GRP=0; TAP1GRP=0; TAPS1GRP=0;
    PLOT=2*MULT+2; PLOTLINE=1; PLTCHR='S'; PLTCL='s'; DECISION='Shunt Alone ';
    PATIENT=4; OUTPUT;
* TAP without a shunt:                                                         ;
  SHAS1GRP=0; SHNT1GRP=0; PV1GRP=0; PVS1GRP=0; TAP1GRP=1; TAPS1GRP=0;
    PLOT=2*MULT+2; PLOTLINE=2; PLTCHR='T'; PLTCL='t'; DECISION='TAP Alone   ';
    PATIENT=5; OUTPUT;
* TAP with a shunt:                                                            ;
  SHAS1GRP=0; SHNT1GRP=0; PV1GRP=0; PVS1GRP=0; TAP1GRP=0; TAPS1GRP=1;
    PLOT=2*MULT+2; PLOTLINE=3; PLTCHR='W'; PLTCL='w'; DECISION='TAP + Shunt ';
    PATIENT=6; OUTPUT;

  DATA DESIGN; SET DESIGN;
* Transformations of variables                                                 ;
  RVNS=0; IF TAP1GRP=1 OR PV1GRP=1 THEN RVNS=1;
  PV1=0; IF PV1GRP=1 OR PVS1GRP=1 THEN PV1=1;
* Power:                                                                       ;
  Z2MTVECH=ZMTVECHO**2;
* Interaction:                                                                 ;
  RVNSTVSE=TVINC_SE*RVNS;
  TAP1GZ2T=TAP1GRP*Z2MTVECH;
  TAPS1ZMT=TAPS1GRP*ZMTVECHO;
  PV1Z2TV=PV1*Z2MTVECH;

  PROC PRINT;
*******************************************************************************;
* Generate and output points                                                   ;
  DATA PREDICT; SET DESIGN; DIGITAL=0;
  MAX=84; LN_MAX=LOG(MAX); INC=(5+LN_MAX)/999.9;
  DO LN_TIME=-5 TO LN_MAX BY INC,LN_MAX;
    MONTHS=EXP(LN_TIME); YEARS=MONTHS/12;
    OUTPUT;
  END;
  DROP LN_TIME MAX LN_MAX INC;

  DATA DIGITAL; SET DESIGN; DIGITAL=1;
  DTY=12/365.2425;
  DO MONTHS=1*DTY,2*DTY,3*DTY,4*DTY,5*DTY,6*DTY,7*DTY,14*DTY,30*DTY,
  1,2,3,6,12 TO 84 BY 12;
    YEARS=MONTHS/12;
    OUTPUT;
  END;
  DROP DTY;
  DATA PREDICT; SET PREDICT DIGITAL;
* Stipulate region of available data                                           ;
  IF (PATIENT=2 OR PATIENT=3) AND ZMTVECHO<-4.4 THEN DELETE;
  IF PATIENT=5 AND ZMTVECHO<-3.7 THEN DELETE;
  IF PATIENT=6 AND ZMTVECHO<-5.7 THEN DELETE;
*******************************************************************************;
* Do predictions for atresia                                                   ;
  PROC SORT; BY PATIENT;
  LIBNAME CHSPTR94 ('!HZEXAMPLES/sasest');
  %HAZPRED(
  PROC HAZPRED DATA=PREDICT INHAZ=CHSPTR94.HMDTHIPR OUT=PREDICT;
       TIME MONTHS; ID PATIENT;
  );
*******************************************************************************;
* Printer plot survival and hazard                                             ;
  DATA PREDICT; SET PREDICT;
  LABEL _SURVIV  = 'Probability of Survival';
  PROC SORT; BY PLOT;
  PROC PLOT DATA=PREDICT; BY PLOT;
       PLOT _SURVIV*YEARS=PLTCHR
           _CLLSURV*YEARS=PLTCL _CLUSURV*YEARS=PLTCL
            /OVERLAY VAXIS=0 TO 1 BY 0.1;
*******************************************************************************;
* Digital nomogram                                                             ;
  DATA DIGITAL; SET PREDICT; IF DIGITAL=1;
  PROC SORT; BY PLOT DECISION ZMTVECHO;
  PROC PRINT; BY PLOT DECISION ZMTVECHO; VAR MONTHS YEARS
    WT_BIRTH RVDEPCC NCANOM TVINC_SE RVENL RAE APCOL ATBDPV BAS SHAS1GRP RVNS
    TAP1GRP TAPS1GRP SHNT1GRP PVS1GRP PV1 ZMTVECHO RVSP
    _SURVIV _CLLSURV _CLUSURV;
*******************************************************************************;
RUN;
