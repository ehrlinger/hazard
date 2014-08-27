*______________________________________________________________________________;
* <2012-07-31>                                                                 ;
*                                                                              ;
* Nelson Cumulative Event Analysis Macro with Approximate Lognormal            ;
* Distribution Confidence Limits for a Terminating Event                       ;
  %MACRO NELSONT(INTERVAL=INTERVAL, EVENT=EVENT, RCENSOR=0, STRATIFY='ALL',
                 CLEVEL=0.68268948, IN=, OUT=EVENT, ELABEL=Death, NOPRINT=0,
                 HLABEL=Years After Operation, PEVENT=1, OEVENT=1, PLOTS=1,
                 PLOTC=1, PLOTH=0, PLOTL=0);
*______________________________________________________________________________;
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:            DEFAULT:                        ;
*                                                                              ;
*   INTERVAL = Time variable                     INTERVAL                      ;
*   EVENT    = Number of events                  EVENT                         ;
*   RCENSOR  = Number of censored cases          0                             ;
*   STRATIFY = Stratification variable           'ALL'                         ;
*   CLEVEL   = Confidence coefficient            0.68268948 (1 sd)             ;
*   IN       = Input data set name                                             ;
*   OUT      = Output data set name              EVENT                         ;
*   HLABEL   = Label for time axis of plots      Years After Operation         ;
*   ELABEL   = Label for the event               Death                         ;
*   NOPRINT  = No printed output                 0 (1 for no printing)         ;
*   PEVENT   = Print only the events             1 (0 for censored also)       ;
*   OEVENT   = Output only the events            1 (0 for censored also)       ;
*   PLOTS    = Plot survival                     1 (0 to skip)                 ;
*   PLOTC    = Plot cumulative hazard            1 (0 to skip)                 ;
*   PLOTH    = Plot hazard                       0 (1 to plot)                 ;
*   PLOTL    = Plot integrated survivorship      0 (1 to plot)                 ;
*______________________________________________________________________________;
*                                                                              ;
* NOTE:  All patients (including events) are censored at the time of last      ;
*        followup.  This turns the repeating events macro NELSONL into a       ;
*        macro for terminating events.                                         ;
*______________________________________________________________________________;
*                                                                              ;
* Refine data set                                                              ;
* PROC PRINT DATA=&IN;
  DATA _REFINE_; SET &IN;
* Define censored individuals                                                  ;
  CENSORED=&RCENSOR;
  IF &EVENT=0 AND CENSORED=0 THEN CENSORED=1;
* Remove observations with inadmissible values                                 ;
  IF &INTERVAL GE 0 AND &EVENT GE 0 AND CENSORED GE 0;
* PROC PRINT DATA=_REFINE_;
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
  KEEP _CATG &INTERVAL &EVENT CENSORED T_ALPHA TEST N;
* PROC PRINT DATA=_REFINE_;
  PROC SUMMARY DATA=_REFINE_ NWAY;
       VAR TEST; OUTPUT OUT=OUTTEST SUM=TEST;
* PROC PRINT DATA=OUTTEST;
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
  PROC SUMMARY DATA=_REFINE_ NWAY; CLASS _CATG;
       VAR &INTERVAL &EVENT CENSORED;
       OUTPUT OUT=S1 MIN(&INTERVAL)=MIN MAX(&INTERVAL)=MAX SUM=SUM
              &EVENT CENSORED;
* PROC PRINT DATA=S1;
  DATA S1; SET S1; N=&EVENT + CENSORED;
  %IF &NOPRINT %THEN %GOTO START;
    PROC PRINT D DATA=S1; VAR _CATG N &EVENT CENSORED MIN MAX SUM;
  %START:
  DATA S1; SET S1; KEEP _CATG N;
*******************************************************************************;
* Sort data set in ascending order of intervals, but with censored             ;
* observations following uncensored ones.                                      ;
  PROC SUMMARY DATA=_REFINE_ NWAY; CLASS _CATG &INTERVAL;
       VAR &EVENT CENSORED;
       OUTPUT OUT=BASIC1 SUM=&EVENT CENSORED;
       ID T_ALPHA;
  PROC SORT DATA=BASIC1; BY _CATG &INTERVAL DESCENDING &EVENT;
* PROC PRINT DATA=BASIC1;
*******************************************************************************;
* Merge the collapsed data set with the one observation defining the           ;
* total number of patients at risk                                             ;
  DATA &OUT; MERGE BASIC1 S1; BY _CATG;
  RETAIN NUMRISK 0 DIST 0 CUM_HAZ 0 IT 0 I_NRISK 0 LAG_TIME 0 LAG_CHF 0
         CUM_SURV 1 LAG_SURV 1 LAG_LIFE 0;
  IF FIRST._CATG=1 THEN DO; NUMRISK=N; CUM_HAZ=0; LAG_TIME=0; LAG_CHF=0;
     LAG_SURV=1.00; LAG_LIFE=0; LIFE=0; PROPLIFE=1; CUM_SURV=1; END;
  IF &EVENT>0 THEN DO;
    DIST=&EVENT/NUMRISK;
    CUM_HAZ=DIST + CUM_HAZ;
    CUM_SURV=EXP(-CUM_HAZ);
    NUMBER=NUMRISK;
    I_NRISK=I_NRISK + 1/NUMRISK;
    IT = IT + 1;
*******************************************************************************;
* Variance of each cumulative hazard estimate, made at the time of each        ;
* of the i events (variable name IT):                                          ;
*   s**2 = sum(1/NUMRISK)*cumulative hazard/IT                                  ;
    VAR_CEF=I_NRISK*CUM_HAZ/IT;
    SE_CEF=SQRT(VAR_CEF);
*******************************************************************************;
* The lognormal distribution requires the parameters mu and sigma              ;
*   mu = ln(cumulative hazard) - (sigma**2 / 2).                               ;
*   sigma**2 = ln((sum(1/NUMRISK)/(IT*cumulative hazard) + 1)                   ;
    IF CUM_HAZ>0 THEN DO;
      SIGMA_2=LOG((I_NRISK/(IT*CUM_HAZ))+1); SIGMA=SQRT(SIGMA_2);
      MU=LOG(CUM_HAZ) - SIGMA_2/2;
      CLU_LCHF=MU+T_ALPHA*SIGMA; CLL_LCHF=MU-T_ALPHA*SIGMA;
*******************************************************************************;
* Exponentiate to obtain CLs in cumulative hazard domain                       ;
      CLU_CHF=EXP(CLU_LCHF); CLL_CHF=EXP(CLL_LCHF);
      CL_LOWER=EXP(-CLU_CHF); CL_UPPER=EXP(-CLL_CHF);
    END;
    ELSE DO; CLU_CHF=0; CLL_CHF=0; CL_LOWER=0; CL_UPPER=0; END;
  END;
*******************************************************************************;
* Estimate hazard and density                                                  ;
  IF &EVENT>0 THEN DO;
    DELTA_TM = &INTERVAL-LAG_TIME;
    MID_INT=(LAG_TIME+&INTERVAL)/2;
    IF DELTA_TM>0 THEN DO;
      HAZARD=(CUM_HAZ-LAG_CHF)/DELTA_TM;
      DENSITY=(LAG_SURV-CUM_SURV)/DELTA_TM;
    END;
    LAG_CHF=CUM_HAZ;
    LAG_TIME=&INTERVAL;
    LAG_SURV=CUM_SURV;
  END;
*******************************************************************************;
* Number remaining at risk after this event                                    ;
  NUMRISK=NUMRISK-&EVENT-CENSORED;
*******************************************************************************;
* Integral of survivorship and proportion of potential lifetime                ;
  IF &EVENT>0 THEN DO;
    LIFE=LAG_LIFE + DELTA_TM*(3*CUM_SURV - LAG_SURV)/2;
    IF &INTERVAL>0 THEN PROPLIFE=LIFE/&INTERVAL;
    LAG_LIFE=LIFE;
  END;
*******************************************************************************;
* Print out estimates and plot them                                            ;
* PROC PRINT DATA=&OUT;
  %IF &NOPRINT %THEN %GOTO FINISH;
    DATA PRINT; SET &OUT;
    IF &EVENT=0 AND &PEVENT=1 THEN DELETE;
    PROC PRINT; BY _CATG; SUMBY _CATG; SUM &EVENT;
       VAR &INTERVAL NUMBER CENSORED NUMRISK &EVENT CUM_SURV CL_LOWER CL_UPPER
           CUM_HAZ HAZARD DENSITY MID_INT LIFE PROPLIFE;
    DATA PLOT;  SET &OUT;
    LN_CUMHZ=.; IF CUM_HAZ>0 THEN LN_CUMHZ = LOG(CUM_HAZ);
    LN_HAZ=.; IF HAZARD>0 THEN LN_HAZ = LOG(HAZARD);
    IF &INTERVAL>0 THEN LN_INT=LOG(&INTERVAL); ELSE LN_INT=.;
    PLOTCHAR=LEFT(_CATG);
    IF CENSORED>0 AND &EVENT=0 THEN PLOTCHAR='.'; OUTPUT;
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
           PLOT LN_HAZ*MID_INT=PLOTCHAR;
        %END;
        %IF &PLOTL=1 %THEN %DO;
           PLOT LIFE*&INTERVAL=PLOTCHAR;
           PLOT PROPLIFE*&INTERVAL=PLOTCHAR/ VAXIS=0.0 TO 1.0 BY 0.1;
        %END;
      %END;
  %FINISH:
*******************************************************************************;
* Clean up output data set                                                     ;
  DATA &OUT; SET &OUT;
  IF &OEVENT=1 THEN DO; IF &EVENT=0 THEN DELETE; END;
  %IF &TEST>0 %THEN &STRATIFY=_CATG;;
  DATA &OUT; SET &OUT;
  DROP T_ALPHA _FREQ_ _TYPE_ N DIST IT I_NRISK LAG_TIME LAG_CHF LAG_SURV
       LAG_LIFE SE_CEF SIGMA_2 CLU_LCHF CLL_LCHF DELTA_TM _CATG;
*******************************************************************************;
  %MEND NELSONT;
*______________________________________________________________________________;
*                                                                              ;
