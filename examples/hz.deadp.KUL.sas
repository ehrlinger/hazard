*______________________________________________________________________________;
*                                                                              ;
* !HZEXAMPLES/hz.deadp.KUL                                                     ;
*                                                                              ;
* Survival after primary isolated CABG                                         ;
*                                                                              ;
* Hazard function for death                                                    ;
*______________________________________________________________________________;
*                                                                              ;
  OPTIONS PAGESIZE=100 LINESIZE=132;
  TITLE1 "Surgery for Ischemic Heart Disease";
  TITLE2 "KUL; 1971-Jul 1987";
  TITLE3 "Primary Isolated CABG";
  TITLE4 "Hazard Function for Death";
*******************************************************************************;
* Bring in data                                                                ;
  FILENAME INCABG ('!HZEXAMPLES/data/cabgkul');
  DATA CABG; INFILE INCABG MISSOVER;
  INPUT INT_DEAD 1-10 5 DEAD 12-15 INT_ANG 17-26 5 ANGINA 28-31 INT_REIN 33-42 5
        REINT 44-47;
  LABEL
        INT_DEAD = 'Followup Interval (months) After Repair'
        DEAD     = 'Death'
        INT_ANG  = 'Interval (months) to Return of Angina'
        ANGINA   = 'Return of Angina'
        INT_REIN = 'Interval (months) to Reintervention'
        REINT    = 'First Reintervention'
  ;
  KEEP INT_DEAD DEAD;
*******************************************************************************;
* Estimate parameters of the distribution of the event                         ;
  %HAZARD(
  PROC HAZARD DATA=CABG P OUTHAZ=OUTEST NOCONSERVE STEEPEST QUASI CONDITION=14;
       TIME INT_DEAD;
       EVENT DEAD;
       PARMS MUE=.02 THALF=0.2 NU=1 M=1 FIXTHALF FIXNU FIXM
             MUC=0.0008
             MUL=1.0E-9 TAU=1 FIXTAU ALPHA=1 FIXALPHA ETA=1 FIXETA FIXGAMMA
             GAMMA=3 WEIBULL;
  );
*______________________________________________________________________________;
*                                                                              ;
* To begin a 3-phase analysis, get the scale factors, for example, as above.   ;
* Then you are ready to switch to CONSERVE, remove FIXTHALF FIXNU FIXM and     ;
* FIXGAMMA, after you have entered for MUE MUC and MUL the values obtained     ;
* from the above.  Go from there, eventually getting a model and checking it   ;
* out by removing the /* and */ below.                                         ;
*______________________________________________________________________________;
*                                                                              ;
  /*
*******************************************************************************;
* Plot various survival functions                                              ;
  FILENAME HAZPLOT ('!MACROS/hazplot.sas');
  OPTIONS NOSOURCE; %INC HAZPLOT; OPTIONS SOURCE;
  %HAZPLOT(INTERVAL=INT_DEAD,EVENT=DEAD,IN=CABG,INEST=OUTEST,
           PLOTI=1, PDETAIL=1, HLABEL=Months After CABG);
*******************************************************************************;
* Digital nomogram                                                             ;
  DATA DIG;
  YRMO=12/365.2425;
  DO MONTHS=7*YRMO,14*YRMO,30*YRMO, 2 TO 36 BY 1, 48 TO 240 BY 6;
    YEARS=MONTHS/12;
    OUTPUT;
  END;
  %HAZPRED(
  PROC HAZPRED DATA=DIG INHAZ=OUTEST OUT=HFDIG; TIME MONTHS;
  );
  DATA PRINT; SET HFDIG;
  PROC PRINT D; VAR MONTHS YEARS
       _SURVIV _CLLSURV _CLUSURV _HAZARD _CLLHAZ _CLUHAZ
  ;
*******************************************************************************;
 */
 RUN;

*______________________________________________________________________________;
*                                                                              ;
* EHB notes:                                                                   ;
*      TIME INT_DEAD;                                                          ;
*      EVENT DEAD;                                                             ;
*      PARMS MUE=.02260252 THALF=0.1647423 NU=1.358020 M=0 FIXM                ;
*            MUC=0.0008272497                                                  ;
*            MUL=1.28230E-9 TAU=1 FIXTAU ALPHA=1 FIXALPHA ETA=1 FIXETA         ;
*            GAMMA=3.761092 WEIBULL;                                           ;
*                                                                              ;
*      TIME INT_ANG;                                                           ;
*      EVENT ANGINA;                                                           ;
*      PARMS MUE=0.03829104 THALF=5.028068 NU=1 FIXNU M=0 FIXM                 ;
*            MUC=0.002302776                                                   ;
*            MUL=2.24566E-8 TAU=1 FIXTAU ETA=1 FIXETA GAMMA=3.274861           ;
*            ALPHA=1 FIXALPHA WEIBULL;                                         ;
*                                                                              ;
*      TIME INT_REIN;                                                          ;
*      EVENT REINT;                                                            ;
*      PARMS MUE=0.008098863 THALF=7.900485 NU=0.3200757 M=1 FIXM              ;
*            MUL=0.05174078 TAU=122.3389 ALPHA=0 FIXALPHA                      ;
*            GAMMA=2 FIXGAMMA ETA=1 FIXETA WEIBULL;                            ;
*______________________________________________________________________________;
*                                                                              ;
