*******************************************************************************;
* <2003-10-17> This date is an upper bound on the last modified date           ;
* Nelson Cumulative Event Analysis Macro with Approximate Lognormal            ;
* Distribution Confidence Limits                                               ;
  %MACRO NELSONL(INTERVAL=INTERVAL, EVENT=EVENT, CENSORED=CENSORED,
         IN=, OUT=, ID=, CUMDIST=CUMDIST, CLL_CEF=CLL_CEF, CLU_CEF=CLU_CEF,
         STRATIFY='ALL     ', WEIGHT=1, CLEVEL=0.68268948, NOPRINT=0, NOLIST=0,
         PEVENT=1, OEVENT=1, PLOTC=1, PLOTH=0, HLABEL=Years, ELABEL=Event);
*______________________________________________________________________________;
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:            DEFAULT:                        ;
*                                                                              ;
*   INTERVAL = Time variable                     INTERVAL                      ;
*   EVENT    = Event (0/1) indicator             EVENT                         ;
*   CENSORED = Censorer (0/1) indicator          CENSORED                      ;
*   STRATIFY = Stratification variable           'ALL'                         ;
*   CLEVEL   = Confidence coefficient            0.68268948 (1 sd)             ;
*   IN       = Input data set name                                             ;
*   OUT      = Output data set name              EVENT                         ;
*   ID       = Other variables to carry                                        ;
*   CUMDIST  = Cumulative hazard function        CUMDIST                       ;
*   CLL_CEF  = Lower confidence limits           CLL_CEF                       ;
*   CLU_CEF  = Upper confidence limits           CLU_CEF                       ;
*   WEIGHT   = Severity level of event           1                             ;
*   HLABEL   = Label for time axis of plots      Years After Operation         ;
*   ELABEL   = Label for the event               Death                         ;
*   NOPRINT  = No printed output                 0 (1 for no printing)         ;
*   NOLIST   = Only plotter output, no lists     0 (1 for no lists)            ;
*   PEVENT   = Print only the events             1 (0 for censored also)       ;
*   OEVENT   = Output only the events            1 (0 for censored also)       ;
*   PLOTC    = Plot cumulative hazard            1 (0 to skip)                 ;
*   PLOTH    = Plot hazard                       0 (1 to plot)                 ;
*______________________________________________________________________________;
*                                                                              ;
* NOTE:  All patients (including events) are censored at the time of           ;
*        last followup to turn the repeating events NELSONL macro into         ;
*        a terminating events macro.                                           ;
*______________________________________________________________________________;
*                                                                              ;
* Set weighting for non-events to a value of 0                                 ;
* PROC PRINT DATA=&IN;
  DATA &IN; SET &IN;
  RETAIN _CATEG_;
* Set up weights (severity of event)                                           ;
  WEIGHT=&WEIGHT;  IF &EVENT=0 THEN WEIGHT=0;
* 3/19/91 I have added E_WGHT to DIST equation to account for the              ;
* summations below.                                                            ;
  E_WGHT=WEIGHT*&EVENT;
* Set up confidence coefficient                                                ;
  PROB=0.5 + (0.5*&CLEVEL); Z=PROBIT(PROB);
* Determine if there is stratification                                         ;
  STRATIFY=&STRATIFY;
  TEST=0;
  IF _N_=1 THEN _CATEG_=STRATIFY;
    ELSE IF STRATIFY NE _CATEG_ THEN TEST=1;
  PROC SUMMARY DATA=&IN NWAY;
       VAR TEST; OUTPUT OUT=OUTTEST SUM=TEST;
  DATA OUTTEST; SET OUTTEST;
  CALL SYMPUT('TEST',TEST); RUN;
* PROC PRINT DATA=&IN;
*******************************************************************************;
* Collapse all observations with unique intervals within stratifications       ;
* to one observation with summed number of events, censoreds, and weights      ;
  PROC SUMMARY NWAY DATA=&IN; CLASS STRATIFY &INTERVAL &ID; ID Z;
       VAR &EVENT &CENSORED WEIGHT E_WGHT; OUTPUT OUT=DATA1 SUM=;
* PROC PRINT DATA=DATA1;
*******************************************************************************;
* Determine the total number of individuals at risk by summing the             ;
* number of censoreds within each stratification                               ;
  PROC MEANS DATA=DATA1 NOPRINT; VAR &CENSORED; OUTPUT OUT=M1 SUM=NC;
       BY STRATIFY;
* PROC PRINT DATA=M1;
*******************************************************************************;
* Merge the collapsed data set with the one observation defining the           ;
* total number of patients at risk                                             ;
  DATA &OUT; MERGE DATA1 M1; BY STRATIFY;
  RETAIN NRISK 0 DIST 0 &CUMDIST 0 IT 0 I_NRISK 0 LAG_TIME 0 LAG_CHF 0;
  IF FIRST.STRATIFY=1 THEN DO; LAG_TIME=0; LAG_CHF=0;
    NRISK=NC; &CUMDIST=0; END;
  IF &EVENT>0 THEN DO;
* 3/19/91 I have added E_WGHT to DIST equation to account for the              ;
* summations above.                                                            ;
    DIST=E_WGHT/NRISK; &CUMDIST=DIST + &CUMDIST; NUMRISK=NRISK;
 /* DIST=WEIGHT/NRISK; &CUMDIST=DIST + &CUMDIST; NUMRISK=NRISK; */
    I_NRISK=I_NRISK + 1/NRISK; IT = IT + 1;
  if &cumdist>0 then ln_cumhz=log(&cumdist);
  if &interval>0 then ln_int=log(&interval);
*******************************************************************************;
* Variance of each cumulative hazard estimate, made at the time of each        ;
* of the i events (variable name IT):                                          ;
*   s**2 = sum(1/NRISK)*cumulative hazard/IT                                   ;
    VAR_CEF=I_NRISK*&CUMDIST/IT; SE_CEF=SQRT(VAR_CEF);
*******************************************************************************;
* The lognormal distribution requires the parameters mu and sigma              ;
*   mu = ln(cumulative hazard) - (sigma**2 / 2).                               ;
*   sigma**2 = ln((sum(1/NRISK)/(IT*cumulative hazard) + 1)                    ;
    IF &CUMDIST>0 THEN DO;
      SIGMA_2=LOG((I_NRISK/(IT*&CUMDIST))+1); SIGMA=SQRT(SIGMA_2);
      MU=LOG(&CUMDIST) - SIGMA_2/2;
      CLU_LCEF=MU+Z*SIGMA; CLL_LCEF=MU-Z*SIGMA;
*******************************************************************************;
* Exponentiate to obtain CLs in cumulative hazard domain                       ;
      &CLU_CEF=EXP(CLU_LCEF); &CLL_CEF=EXP(CLL_LCEF);
    END;
    ELSE DO; &CLU_CEF=0; &CLL_CEF=0; END;
  END;
*******************************************************************************;
* Estimate hazard                                                              ;
  IF &EVENT>0 THEN DO;
    DELTA_TM = &INTERVAL-LAG_TIME;
    MID_INT=(LAG_TIME+&INTERVAL)/2;
    IF DELTA_TM>0 THEN HAZARD=(&CUMDIST-LAG_CHF)/DELTA_TM;
    LAG_CHF=&CUMDIST;
    LAG_TIME=&INTERVAL;
  END;
*******************************************************************************;
  IF &CENSORED>0 THEN NRISK=NRISK-&CENSORED;
*******************************************************************************;
* Restore stratification variable                                              ;
  %IF &TEST>0 %THEN &STRATIFY=STRATIFY;
  ;
*******************************************************************************;
* Print out estimates and plot them                                            ;
  PLOTCHAR=LEFT(STRATIFY);
  IF &CENSORED>0 AND &EVENT=0 THEN PLOTCHAR='.'; OUTPUT;
  %IF &NOPRINT %THEN %GOTO FINISH;
  DATA PRINT; SET &OUT;
  IF &EVENT=0 AND &PEVENT=1 THEN DELETE;
    %IF &NOLIST=0 %THEN %DO;
      PROC PRINT; BY STRATIFY; SUMBY STRATIFY; SUM &EVENT;
           var &interval numrisk it &event
               &censored &cumdist se_cef &cll_cef &clu_cef hazard mid_int;
    %END;
  DATA PLOT; SET &OUT;
  LABEL
         &CUMDIST  = Cumulative Hazard for &ELABEL
         HAZARD    = Hazard Function
         &INTERVAL = &HLABEL
         MID_INT   = &HLABEL
         LN_CUMHZ  = Ln(Cumulative Hazard)
         LN_INT    = Ln(&HLABEL)
  ;

  %IF &PLOTC=1 | &PLOTH=1 %THEN %DO;
    PROC PLOT;
      %IF &PLOTC=1 %THEN %DO;
         PLOT &CUMDIST*&INTERVAL=PLOTCHAR
              &cll_cef*&interval='.' &clu_cef*&interval='.'/overlay;
         PLOT CUMDIST*LN_INT=PLOTCHAR;
         PLOT LN_CUMHZ*&INTERVAL=PLOTCHAR;
         PLOT LN_CUMHZ*LN_INT=PLOTCHAR;
      %END;
      %IF &PLOTH=1 %THEN %DO;
         PLOT HAZARD*MID_INT=PLOTCHAR;
      %END;
  %END;
  %FINISH:
*******************************************************************************;
* Clean up data set                                                            ;
  DATA &OUT; SET &OUT;
  DROP Z I_NRISK SIGMA_2 SIGMA MU CLU_LCEF CLL_LCEF _FREQ_ _TYPE_
       STRATIFY VAR_CEF;
  if &oevent=1 then do;
    if &event=0 then delete;
  end;
*******************************************************************************;
  %MEND NELSONL;
*******************************************************************************;
