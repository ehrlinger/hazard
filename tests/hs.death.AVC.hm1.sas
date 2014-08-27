  OPTIONS PAGESIZE=100 LINESIZE=132;
*******************************************************************************;
* !HZEXAMPLES/hs.death.AVC.hm1                                                 ;
*                                                                              ;
* Multivariable analysis of death after repair                                 ;
*                                                                              ;
* Another strength of a completely parametric survival analysis is that the    ;
* investigator can test the "goodness" of the model.  Specifically, the        ;
* theory of mixture distributions would indicate that if a survival curve      ;
* (or death density function) is generated for each observation, the mean      ;
* of these should be the overall survival curve.  And for any subset of the    ;
* data, such a subset mean should well fit the empiric survival curve (unless  ;
* a risk factor has not been taken into account).                              ;
*                                                                              ;
* The theory of conservation of events also suggests that we can sum the       ;
* cumulative hazard for each patient and predict the number of expected        ;
* deaths, comparing this with the actual number.  One has to be a bit          ;
* careful here, since the cumulative hazard has a limitless upper bound.  One  ;
* could make the case for limiting any cumulative hazard estimate greater      ;
* than 1.0 to that number.                                                     ;
*                                                                              ;
* As a type of internal validation, although the variable is in the model,     ;
* we will predict partial and complete AV Canal from the analysis, overlaying  ;
* it with the nonparametric life table estimates to check out the model.       ;
* Recall that we were concerned that the log-likelihood was better when        ;
* COM_IV was present in the constant hazard phase, but we could not obtain     ;
* reliable estimates for that factor.  This will help us see how badly we      ;
* miss the mark by ignoring this factor in that phase.                         ;
*                                                                              ;
* In actual practice, what I do is to have a "setup" job that generates the    ;
* curves for each patient in the data set and the cumulative hazard values.    ;
* Once stored (often temporarily, for the data set size can be huge), I can    ;
* then stratify the data set in every which way to check myself out using a    ;
* separate job or set of jobs.                                                 ;
*******************************************************************************;
* Bring in data                                                                ;
  TITLE1 'Repair of Atrioventricular Septal (A-V Canal) Defects';
  TITLE2 'Example Data Set';
  TITLE3 'Multivariable Analysis of Death After Repair';
  TITLE4 'Solve Multivariable Equation for Death for Each Patient';
  TITLE5 'Stratify By Type of AV Canal';
*______________________________________________________________________________;
*                                                                              ;
* NOTE:  Normally this would be a SAS data set, probably built from the raw    ;
*        data.  For illustration, a few variables from a data set for repair   ;
*        of atrioventricular septal defects will be read from a "flat file."   ;
*______________________________________________________________________________;
*                                                                              ;
  FILENAME INAVC ('!HZEXAMPLES/data/avc');
  DATA AVCS; INFILE INAVC MISSOVER;
  INPUT STUDY $ STATUS INC_SURG OPMOS AGE MAL COM_IV ORIFICE DEAD INT_DEAD
        OP_AGE;
  LABEL
        STUDY    = 'Study number'
        STATUS   = 'Clinical status (NYHA I-V)'
        INC_SURG = 'Surgical grade of AV valve incompetence'
        OPMOS    = 'Date of operation (months since 1/67)'
        AGE      = 'Age (months) at repair'
        MAL      = 'Important associated cardiac anomaly'
        COM_IV   = 'Interventricular communication'
        ORIFICE  = 'Accessory left AV valve orifice'
        DEAD     = 'Death'
        INT_DEAD = 'Followup interval (months) after repair'
  ;
*******************************************************************************;
* Model:                                                                       ;
* Early Hazard Phase:                                                          ;
*   AGE COM_IV MAL OPMOS OP_AGE STATUS                                         ;
* Constant Hazard Phase:                                                       ;
*   INC_SURG ORIFICE STATUS                                                    ;
*******************************************************************************;
* Standardize variables with missing values                                    ;
  PROC STANDARD DATA=AVCS OUT=AVCS REPLACE; VAR INC_SURG;
*******************************************************************************;
* First, get the life table estimates for survival after repair                ;
* Stratify by COM_IV                                                           ;
  FILENAME KAPLAN ('!MACROS/kaplan.sas');
  OPTIONS NOSOURCE; %INC KAPLAN; OPTIONS SOURCE;
  %KAPLAN(IN=AVCS, OUT=PLOUT, INTERVAL=INT_DEAD, EVENT=DEAD,
          OEVENT=0, PLOTS=0, PLOTC=0, PLOTH=0, PLOTL=0,
          STRATIFY=COM_IV, HLABEL=Months After Repair);
*******************************************************************************;
* Generate expected vs observed deaths                                         ;
  LIBNAME EXAMPLES ('!HZEXAMPLES/sasest');
  %HAZPRED (
  PROC HAZPRED DATA=AVCS INHAZ=EXAMPLES.HMDEATH OUT=AVCS;
       TIME INT_DEAD;
  );
  DATA AVCS; SET AVCS;
  CUM_HAZ=-LOG(_SURVIV);
  PROB=1-_SURVIV;
  PROC SORT; BY COM_IV CUM_HAZ;
  TITLE6 'Predict number of deaths';
  PROC SUMMARY DATA=AVCS; BY COM_IV; VAR
         STATUS INC_SURG OPMOS AGE MAL ORIFICE OP_AGE
         DEAD INT_DEAD PROB CUM_HAZ;
       OUTPUT OUT=OUTEXP MEAN= SUM(PROB CUM_HAZ DEAD)=PEXPECT EXPECTED
         ACTUAL;
  PROC PRINT D DATA=OUTEXP; VAR
       STATUS INC_SURG OPMOS AGE MAL COM_IV ORIFICE
       DEAD INT_DEAD PROB CUM_HAZ DEAD PEXPECT EXPECTED ACTUAL;
  DATA AVCS; SET AVCS;
  KEEP STATUS INC_SURG OPMOS AGE MAL COM_IV ORIFICE OP_AGE DEAD INT_DEAD;
*******************************************************************************;
* Digital nomogram                                                             ;
* Calculate parametric estimates of survivorship for each patient              ;
  DATA DIGITAL; SET AVCS;
  DO MONTHS=1*12/365.2425,7*12/365.2425,14*12/365.2425,30*12/365.2425,
     6, 12 TO 180 BY 12;
     YEARS=MONTHS/12;
     OUTPUT;
  END;
  %HAZPRED (
  PROC HAZPRED DATA=DIGITAL INHAZ=EXAMPLES.HMDEATH OUT=DIGITAL NOHAZ;
       TIME MONTHS;
  );
* Mean curve                                                                   ;
  PROC SORT; BY COM_IV;
  PROC SUMMARY DATA=DIGITAL NWAY; BY COM_IV; ID MONTHS;
       CLASS YEARS; VAR _SURVIV _CLLSURV _CLUSURV;
       OUTPUT OUT=MEANSD MEAN=MSURVIV MCLLSURV MCLUSURV
         N=NSURVIV NCLLSURV NCLUSURV;
  PROC DELETE DATA=DIGITAL;
* Print the results                                                            ;
  PROC SORT; BY COM_IV YEARS;
  PROC PRINT; BY COM_IV; VAR YEARS NSURVIV MSURVIV MCLLSURV MCLUSURV;
  TITLE5 'Predicted Average Survivorship';
* Comparison with actuarial curves                                             ;
  DATA PLOTD; SET MEANSD PLOUT;
  IF DEAD=1 THEN DO; PLTCHR='N'; PLTCL='-'; END;
  IF DEAD=0 THEN DO; PLTCHR='.'; PLTCL=' '; END;
  PROC SORT; BY COM_IV;
  PROC PLOT; BY COM_IV;
       PLOT MSURVIV*MONTHS='P' (MCLLSURV MCLUSURV)*MONTHS='.'
            CUM_SURV*INT_DEAD='N' (CL_LOWER CL_UPPER)*INT_DEAD='-'
            /OVERLAY VAXIS=0 TO 1 BY 0.1;
*******************************************************************************;
RUN;
