*______________________________________________________________________________;
*                                                                              ;
* !HZEXAMPLES/hs.dthar.TGA.hs1                                                 ;
*                                                                              ;
* NOTE:  YOU MUST RUN hs.dthar.TGA.setup BEFORE RUNNING THIS JOB!!!            ;
*                                                                              ;
* Arterial Switch Operations                                                   ;
*                                                                              ;
* Multivariable analysis of risk factors for death after operation             ;
* Patient risk factors                                                         ;
*                                                                              ;
* Average survivorship curves for each experience in the data set, generated   ;
* from the multivariable equation for death.                                   ;
*                                                                              ;
* The estimates have been put to the plot disk by hs.dthar.TGA.setup           ;
*                                                                              ;
* Stratification by Era of Operation                                           ;
*______________________________________________________________________________;
*                                                                              ;
  OPTIONS PAGESIZE=100 LINESIZE=132;
  TITLE1 "Arterial Switch Operations";
  TITLE2 "Death After Operation";
  TITLE3 "Multivariable Analysis of Risk Factors";
  TITLE4 "Patient Risk Factors Only";
  TITLE5 "Application of Multivariable Analysis of Death to Create a
 Risk-Adjusted Curve for Each Patient";
  TITLE6 "Stratify By Era of Operation";
*******************************************************************************;
* Bring in data                                                                ;
  FILENAME INTGA ('!HZEXAMPLES/data/tga');
  DATA ARTSW; INFILE INTGA MISSOVER;
  INPUT STUDY $ 1-10 SIMPLE 12 DEXTROIN 14 CA_1RL2C 16 HYAAPROC 18 NO_TCA 20
      TCA_TIME 22-31 5 AGE_DAYS 33-42 5 ARCIOPYR 44-51 5 DEAD 53 INT_DEAD
      55-64 5 SOURCE $ 66-75 CA1_2_L 77 OPYEAR 79-88 5;
  LABEL
        SIMPLE   = 'Simple TGA'
        DEXTROIN = 'Dextrocardia'
        HYAAPROC = 'Procedure for hypoplastic arch'
        NO_TCA   = 'No TCA'
        TCA_TIME = 'TCA time'
        AGE_DAYS = 'Age at operation'
        INT_DEAD = 'Followup Interval (months) After Repair'
        DEAD     = 'Death'
  ;
*******************************************************************************;
* Stratifying variable                                                         ;
  ERA=.; IF OPYEAR<1986 THEN ERA=1;
         IF OPYEAR>1985 AND OPYEAR<1989 THEN ERA=2;
         IF OPYEAR>1988 THEN ERA=3;
*******************************************************************************;
* Form groupings                                                               ;
  KEEP STUDY SOURCE ERA;
  PROC SORT; BY SOURCE STUDY;
  LIBNAME ARTSWT ('!HZEXAMPLES/data');
  DATA EST; SET ARTSWT.HSDTARXC;
  PROC SORT; BY SOURCE STUDY;
  DATA ARTSW; MERGE ARTSW EST; BY SOURCE STUDY;
  DATA ARTSW; SET ARTSW;
  IF ERA=. THEN DELETE;
  PROC SORT; BY ERA;
  PROC MEANS N MEAN STD MIN MAX SUM; BY ERA;
*******************************************************************************;
* Observed v. expected deaths.                                                 ;
  PROC SUMMARY; BY ERA; VAR
  SIMPLE DEXTROIN CA_1RL2C CA1_2_L HYAAPROC NO_TCA LN_TCAT SIMIAGP1 ARCIOPYR
  DEAD INT_DEAD PROB CUM_HAZ;
      OUTPUT OUT=OUTEXP MEAN= SUM(PROB CUM_HAZ DEAD)=PEXPECT EXPECTED
        ACTUAL N(DEAD)=N;
  DATA OUTEXP; SET OUTEXP;
  FILENAME CHISQGF ('!MACROS/chisqgf.sas');
  OPTIONS NOSOURCE; %INC CHISQGF; OPTIONS SOURCE;
*______________________________________________________________________________;
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:            DEFAULT:                        ;
*                                                                              ;
*   N        = Denominator (sample size)         N                             ;
*   OBSERVED = Number of observed events         OBSERVED                      ;
*   EXPECTED = Number of expected events         EXPECTED                      ;
*   PROB_OBS = Proportion observed               PROB_OBS                      ;
*   PROB_EXP = Proportion expected               PROB_OBS                      ;
*   ZVALUE   = Z-value for difference            Z-VALUE                       ;
*   PVALUE   = P-value for difference            P-VALUE                       ;
*______________________________________________________________________________;
*                                                                              ;
* Statistics                                                                   ;
  %CHISQGF(N=N, OBSERVED=ACTUAL, EXPECTED=EXPECTED);
  PROC PRINT D DATA=OUTEXP; VAR
       SIMPLE DEXTROIN CA_1RL2C CA1_2_L HYAAPROC NO_TCA LN_TCAT SIMIAGP1 ARCIOPYR
       PROB CUM_HAZ DEAD PEXPECT EXPECTED ACTUAL N PVALUE;
*******************************************************************************;
* Stratified actuarial analysis                                                ;
  FILENAME KAPLAN ('!MACROS/kaplan.sas');
  OPTIONS NOSOURCE; %INC KAPLAN; OPTIONS SOURCE;
*______________________________________________________________________________;
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:            DEFAULT:                        ;
*                                                                              ;
*   INTERVAL = Time variable                     INTERVAL                      ;
*   EVENT    = Event (0/1) indicator             EVENT                         ;
*   STRATIFY = Stratification variable           'ALL'                         ;
*   CLEVEL   = Confidence coefficient            0.68268948 (1 sd)             ;
*   IN       = Input data set name                                             ;
*   OUT      = Output data set name              EVENT                         ;
*   HLABEL   = Label for time axis of plots      Years After Operation         ;
*   ELABEL   = Label for the event               Death                         ;
*   PEVENT   = Print only the events             1 (0 for censored also)       ;
*   OEVENT   = Output only the events            1 (0 for censored also)       ;
*   PLOTS    = Plot survival                     1 (0 to skip)                 ;
*   PLOTC    = Plot cumulative hazard            1 (0 to skip)                 ;
*   PLOTH    = Plot hazard                       0 (1 to plot)                 ;
*   PLOTL    = Plot integrated survivorship      0 (1 to plot)                 ;
*______________________________________________________________________________;
*                                                                              ;
  %KAPLAN(IN=ARTSW, OUT=PLOUT, INTERVAL=INT_DEAD, EVENT=DEAD, PEVENT=0,
          OEVENT=0, PLOTS=0, PLOTC=0, STRATIFY=ERA,
          HLABEL=Months After Operation);
*******************************************************************************;
* Time-related nomograms                                                       ;
  DATA ARTSW; SET ARTSW;
  KEEP STUDY SOURCE ERA;
  PROC SORT DATA=ARTSW; BY SOURCE STUDY;
  DATA PREDICT; SET ARTSWT.HSDTARXP;
  PROC SORT DATA=PREDICT; BY SOURCE STUDY;
  DATA PREDICT; MERGE ARTSW PREDICT; BY SOURCE STUDY;
  IF ERA=. THEN DELETE;
*******************************************************************************;
* Digital nomogram                                                             ;
  DATA DIGITAL; SET PREDICT; IF DIGITAL=1;
  PROC SORT DATA=DIGITAL; BY ERA;
* Mean curve                                                                   ;
  PROC SUMMARY DATA=DIGITAL NWAY; BY ERA;
       CLASS MONTHS; VAR _SURVIV _CLLSURV _CLUSURV;
       OUTPUT OUT=MEANS MEAN=MSURVIV MCLLSURV MCLUSURV
         N=NSURVIV NCLLSURV NCLUSURV;
* Print the results                                                            ;
  PROC SORT DATA=MEANS; BY ERA MONTHS;
  PROC PRINT D; BY ERA; VAR MONTHS NSURVIV MSURVIV MCLLSURV MCLUSURV;
*******************************************************************************;
* Calculate parametric survivorship curves for each subgroup                   ;
  DATA PREDICT; SET PREDICT; IF DIGITAL=0;
  PROC SORT DATA=PREDICT; BY ERA;
* Mean curve                                                                   ;
  PROC SUMMARY DATA=PREDICT NWAY; BY ERA;
       CLASS MONTHS; VAR _SURVIV _CLLSURV _CLUSURV;
       OUTPUT OUT=MEANS MEAN=MSURVIV MCLLSURV MCLUSURV
         N=NSURVIV NCLLSURV NCLUSURV;
  PROC DELETE DATA=PREDICT;
* Compare survivorships with stratified actuarial estimates                    ;
  DATA PLOT; SET MEANS PLOUT;
  IF DEAD=1 THEN DO; PLTCHR='N'; PLTCL='-'; END;
  IF DEAD=0 THEN DO; PLTCHR='.'; PLTCL=' '; END;
  PROC SORT; BY ERA;
  PROC PLOT; BY ERA;
       PLOT MSURVIV*MONTHS=ERA (MCLLSURV MCLUSURV)*MONTHS='.'
            CUM_SURV*INT_DEAD='N' (CL_LOWER CL_UPPER)*INT_DEAD='-'
            /OVERLAY VAXIS=0 TO 1 BY 0.1;
*******************************************************************************;
RUN;
