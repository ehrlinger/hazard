*******************************************************************************;
* <2003-10-17> This date is an pper bound on the last modified date            ;
*                                                                              ;
* Kaplan and Meier Nonparametric Product Limit Actuarial Method                ;
  %MACRO KAPLAN(INTERVAL=INTERVAL, EVENT=EVENT, RCENSOR=0,
         STRATIFY='ALL', ID=, OTHSUMS=, CLEVEL=0.68268948, IN=, OUT=EVENT,
         HLABEL=Years After Operation, ELABEL=Death, PEVENT=1, OEVENT=1,
         PLOTS=1, PLOTC=1, PLOTH=0, PLOTL=0, NOPRINT=0, NOLIST=0);          
*______________________________________________________________________________;
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:            DEFAULT:                        ;
*                                                                              ;
*   INTERVAL = Time variable                     INTERVAL                      ;
*   EVENT    = Number of events                  EVENT                         ;
*   RCENSOR  = Number of censored cases          0                             ;
*   STRATIFY = Stratification variable           'ALL'                         ;
*   OTHSUMS  = Other variables to be summed      none                          ;
*   ID       = Variables retained in data set    none                          ;
*   CLEVEL   = Confidence coefficient            0.68268948 (1 sd)             ;
*   IN       = Input data set name                                             ;
*   OUT      = Output data set name              EVENT                         ;
*   HLABEL   = Label for time axis of plots      Years After Operation         ;
*   ELABEL   = Label for the event               Death                         ;
*   NOPRINT  = No printed output                 0 (1 for no printing)         ;
*   NOLIST   = Only plotter output, no lists     0 (1 for no lists)            ;
*   PEVENT   = Print only the events             1 (0 for censored also)       ;
*   OEVENT   = Output only the events            1 (0 for censored also)       ;
*   PLOTS    = Plot survival                     1 (0 to skip)                 ;
*   PLOTC    = Plot cumulative hazard            1 (0 to skip)                 ;
*   PLOTH    = Plot hazard                       0 (1 to plot)                 ;
*   PLOTL    = Plot integrated survivorship      0 (1 to plot)                 ;
*______________________________________________________________________________;
*                                                                              ;
* Refine data set                                                              ;
  DATA _REFINE_; SET &IN;
* Define censored individuals                                                  ;
  CENSORED=&RCENSOR;
  IF &EVENT=0 AND CENSORED=0 THEN CENSORED=1;
* Remove observations with inadmissible values                                 ;
  IF &INTERVAL GE 0 AND &EVENT GE 0 AND CENSORED GE 0;

  DATA _REFINE_; SET _REFINE_;
  RETAIN _CATGY_;
* Number of cases for this followup time                                       ;
  N=&EVENT + CENSORED;
* Get confidence coefficient                                                   ;
  T_ALPHA=PROBIT(0.5+(0.5*&CLEVEL));
* Determine if there is stratification and if so do comparisons                ;
  _CATG=&STRATIFY;
  TEST=0;
  IF _N_=1 THEN _CATGY_=_CATG;
    ELSE IF _CATG NE _CATGY_ THEN TEST=1;
  _STATUS_=&EVENT+1;
  KEEP _CATG &INTERVAL &EVENT CENSORED _STATUS_ T_ALPHA _CATGY_ TEST
       N &ID &OTHSUMS;
  PROC SUMMARY DATA=_REFINE_ NWAY;
       VAR TEST; OUTPUT OUT=OUTTEST SUM=TEST;
  DATA OUTTEST; SET OUTTEST;
  CALL SYMPUT('TEST',TEST); RUN;
*******************************************************************************;
* Compare actuarial strata                                                     ;
  %IF &NOPRINT %THEN %GOTO CALCULAT;
    %IF &TEST>0 %THEN %DO;
      PROC LIFETEST DATA=_REFINE_ NOTABLE;
           STRATA _CATG; TIME &INTERVAL*&EVENT(0); FREQ N;
    %END;
*******************************************************************************;
* Summarize the various strata                                                 ;
  %CALCULAT:
  PROC SUMMARY DATA=_REFINE_ NWAY; CLASS _CATG; VAR &INTERVAL &EVENT
       CENSORED &OTHSUMS;
       OUTPUT OUT=S1 MIN(&INTERVAL)=MIN MAX(&INTERVAL)=MAX SUM=SUM
              &EVENT CENSORED &OTHSUMS;
       ID &ID;
  DATA S1; SET S1; N=&EVENT + CENSORED;
  %IF &NOPRINT %THEN %GOTO START;
    PROC PRINT D DATA=S1; VAR _CATG N &EVENT CENSORED MIN MAX SUM;
  %START:
  DATA NSET; SET S1; KEEP N _CATG;
*******************************************************************************;
* Do basic counting                                                            ;
  PROC SUMMARY DATA=_REFINE_ NWAY; CLASS _CATG &INTERVAL;
       VAR &EVENT CENSORED &OTHSUMS;
       OUTPUT OUT=BASIC1 SUM=&EVENT CENSORED &OTHSUMS;
       ID T_ALPHA &ID;
  PROC DELETE DATA=_REFINE_;
  DATA BASIC; SET BASIC1; BY _CATG; RETAIN N 0;
  IF FIRST._CATG=1 THEN N=0; N=SUM(&EVENT, CENSORED);
  PROC DELETE DATA=BASIC1;
*******************************************************************************;
* Product limit estimate                                                       ;
  DATA KAPLAN;  MERGE BASIC NSET;  BY _CATG;     RETAIN;
  IF FIRST._CATG=1 THEN DO; NUMBER=N; LAG_SURV=1.00; LAG_TIME=0;
     CUM_SURV=1.0; LAG_LIFE=0; LIFE=0; PROPLIFE=1; END;
  P_EVENT=&EVENT/NUMBER; P_SURV=1-P_EVENT; CUM_SURV=P_SURV*CUM_SURV;
  IF FIRST._CATG=1 &((NUMBER=0)|((NUMBER-&EVENT)=0))
     THEN VAR_PROD=0 ;
  IF NUMBER>0 & (NUMBER-&EVENT)>0 THEN DO;
    IF FIRST._CATG=1 THEN VAR_PROD=1/(NUMBER-&EVENT)-1/NUMBER+1;
    ELSE VAR_PROD=VAR_PROD*(1/(NUMBER-&EVENT)-1/NUMBER+1) ;
  END;
*******************************************************************************;
* Product limit variance and confidence limits estimates                       ;
  VAREXACT=CUM_SURV*CUM_SURV*(VAR_PROD-1);
  SE_EXACT=SQRT(VAREXACT);
  SI_EXACT=0.; IF CUM_SURV<1.
    THEN SI_EXACT = SQRT(VAR_PROD - 1.)*(1./(1. - CUM_SURV));
  CL_LOWER=CUM_SURV/(CUM_SURV+(1.-CUM_SURV)*EXP(T_ALPHA*SI_EXACT));
  CL_UPPER=CUM_SURV/(CUM_SURV+(1.-CUM_SURV)*EXP(-T_ALPHA*SI_EXACT));
*******************************************************************************;
* Cumulative hazard and hazard estimates from transforms and slopes            ;
* as well as integral of survivorship and proportionate life length            ;
  CUM_HAZ=.; IF CUM_SURV>0 THEN CUM_HAZ = - LOG(CUM_SURV);
  LN_CUMHZ=.; IF CUM_HAZ>0 THEN LN_CUMHZ = LOG(CUM_HAZ);
  IF &INTERVAL>0 THEN LN_INT=LOG(&INTERVAL); ELSE LN_INT=.;
  HAZARD=.; DENSITY=.; MID_INT=.;
  IF &EVENT>0 THEN DO;
     DELTA_TM = &INTERVAL-LAG_TIME;
     IF CUM_SURV>0 AND DELTA_TM>0 THEN DO;
        HAZARD=(LOG(LAG_SURV/CUM_SURV))/DELTA_TM;LN_HAZ=LOG(HAZARD);
     END;
     IF DELTA_TM>0 THEN DENSITY=(LAG_SURV-CUM_SURV)/DELTA_TM;
     MID_INT=(LAG_TIME+&INTERVAL)/2;
     LIFE=LAG_LIFE + DELTA_TM*(3*CUM_SURV - LAG_SURV)/2;
     IF &INTERVAL>0 THEN PROPLIFE=LIFE/&INTERVAL;
     LAG_SURV=CUM_SURV;
     LAG_TIME=&INTERVAL;
     LAG_LIFE=LIFE;
  END;
*******************************************************************************;
* Print out estimates and plot them                                            ;
  PLOTCHAR=LEFT(_CATG);
  IF CENSORED>0 AND &EVENT=0 THEN PLOTCHAR='.'; OUTPUT;
  NUMBER=NUMBER-&EVENT-CENSORED; DROP SI_EXACT VAREXACT VAR_PROD P_EVENT;
  %IF &NOPRINT %THEN %GOTO FINISH;
    DATA PRINT; SET KAPLAN;
    IF &EVENT=0 AND &PEVENT=1 THEN DELETE;
    %IF &NOLIST=0 %THEN %DO;
      PROC PRINT; BY _CATG; SUMBY _CATG; SUM &EVENT;
      VAR &INTERVAL NUMBER CENSORED &EVENT CUM_SURV SE_EXACT
          CL_LOWER CL_UPPER CUM_HAZ HAZARD DENSITY MID_INT LIFE PROPLIFE;
    %END;
    DATA PLOT;  SET KAPLAN;
    LABEL
         CUM_SURV  = Proportion Free of &ELABEL
         CUM_HAZ   = Cumulative Hazard Function
         HAZARD    = Hazard Function
         DENSITY   = Density Function
         &INTERVAL = &HLABEL
         MID_INT   = &HLABEL
         LN_HAZ    = Ln(Hazard)
         LN_INT    = Ln(&HLABEL)
         LIFE      = Integral of Survivorship
         PROPLIFE  = Integral of Survivorship/t
    ;
    %IF &PLOTS=1 | &PLOTC=1 | &PLOTH=1 | &PLOTL=1 %THEN %DO;
      PROC PLOT;
        %IF &PLOTS=1 %THEN %DO;
           PLOT CUM_SURV*&INTERVAL=PLOTCHAR/ VAXIS=0.0 TO 1.0 BY 0.1;
        %END;
        %IF &PLOTC=1 %THEN %DO;
           PLOT CUM_HAZ*&INTERVAL=PLOTCHAR;
           PLOT CUM_HAZ*LN_INT=PLOTCHAR;
           PLOT LN_CUMHZ*&INTERVAL=PLOTCHAR;
           PLOT LN_CUMHZ*LN_INT=PLOTCHAR;
        %END;
        %IF &PLOTH=1 %THEN %DO;
           PLOT HAZARD*MID_INT=PLOTCHAR;
           PLOT LN_HAZ*MID_INT=PLOTCHAR;
        %END;
        %IF &PLOTL=1 %THEN %DO;
           PLOT LIFE*&INTERVAL=PLOTCHAR;
           PLOT PROPLIFE*&INTERVAL=PLOTCHAR/ VAXIS=0.0 TO 1.0 BY 0.1;
        %END;
      %END;
    PROC DELETE DATA=OUTTEST;
  %FINISH:
  PROC DELETE DATA=BASIC S1 NSET;
  DATA &OUT; SET KAPLAN;
  IF &OEVENT=1 THEN DO;
    IF &EVENT=0 THEN DELETE;
  END;
  %IF &TEST>0 %THEN &STRATIFY=_CATG;
  ;PROC DELETE DATA=KAPLAN;
*******************************************************************************;
* Clean up output data set                                                     ;
  DATA &OUT; SET &OUT;
  DROP _CATG _FREQ_ _TYPE_ DELTA_TM LAG_LIFE LAG_SURV LAG_TIME
       LN_CUMHZ LN_HAZ LN_INT N P_SURV PLOTCHAR T_ALPHA;
RUN;
*******************************************************************************;
  %MEND KAPLAN;
*******************************************************************************;
