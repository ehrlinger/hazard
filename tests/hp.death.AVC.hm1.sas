  OPTIONS PAGESIZE=100 LINESIZE=132;
*******************************************************************************;
* !HZEXAMPLES/hp.death.AVC.hm1                                                 ;
*                                                                              ;
* Multivariable analysis of death after repair of atrioventricular canals      ;
*                                                                              ;
* Exploration of strength of risk factors.  A major strength of completely     ;
* parametric models is that once parameter estimates are available, the        ;
* resulting equation can be solved for any given set of risk factors.  This    ;
* permits, for example, solving the equation for the time-related survival     ;
* of an individual patient by "plugging in" that patients specific risk        ;
* factors (patient-specific prediction).                                       ;
*                                                                              ;
* In this example, we exploit the parametric model by exploring the shape      ;
* of risk factors.  Here, for a given set of risk factors, we compare survival ;
* in two otherwise similar patients, except that one has an additional         ;
* major cardiac anomaly.                                                       ;
*******************************************************************************;
  TITLE1 'Repair of Atrioventricular Septal (A-V Canal) Defects';
  TITLE2 'Example Data Set';
  TITLE3 'Multivariable Analysis of Death After Repair';
  TITLE4 'Solve Multivariable Equation for Survival and Hazard';
  TITLE5 'In Complete Canal With and Without Associated Anomalies';
  TITLE6 'For a 6-month Old Infant in the Current Era';
*______________________________________________________________________________;
*                                                                              ;
* Model:                                                                       ;
* Early Hazard Phase:                                                          ;
*   AGE COM_IV MAL OPMOS OP_AGE STATUS                                         ;
* Constant Hazard Phase:                                                       ;
*   INC_SURG ORIFICE STATUS                                                    ;
*______________________________________________________________________________;
*                                                                              ;
* Set values for standard model incremental risk factors variables.            ;
  DATA DESIGN;
  AGE=6;         /* Six months of age                                         */
  COM_IV=1;      /* Interventricular communication present                    */
* MAL=0;         /* Major associated cardiac anomaly                          */
  ORIFICE=0;     /* Accessory left AV valve orifice                           */
  INC_SURG=0;    /* Degree of AV valve incompetence pre-repair                */
  STATUS=3;      /* Clinical status (NYHA I-IV)                               */
  OPMOS=180;     /* Current data (1982)                                       */
* MONTHS=6;      /* Time of observation (months)                              */
*______________________________________________________________________________;
*                                                                              ;
* Transformations of variables                                                 ;
  OPYEAR=1967 + OPMOS/12;
  OP_AGE=OPMOS*AGE;
* YEARS=MONTHS/12;
*******************************************************************************;
* Generate "isobars" of risk factors here, but check for possible              ;
* transformations that may be needed.                                          ;
  DO MAL=0,1;
    OUTPUT;
  END;
  PROC PRINT;
*******************************************************************************;
* Generate and output points                                                   ;
  DATA PREDICT; SET DESIGN; DIGITAL=0;
  MAX=180; LN_MAX=LOG(MAX); INC=(5+LN_MAX)/99.9;
  DO LN_TIME=-5 TO LN_MAX BY INC,LN_MAX;
    MONTHS=EXP(LN_TIME);
    OUTPUT;
  END;
  DROP MAX LN_MAX INC LN_TIME;

  DATA DIGITAL; SET DESIGN; DIGITAL=1;
  DTY=12/365.2425;
  DO MONTHS=1*DTY,2*DTY,3*DTY,4*DTY,5*DTY,6*DTY,7*DTY,14*DTY,30*DTY,
            1,2,3,6,12,18,24 TO 180 BY 12;
    OUTPUT;
  END;
  DROP DTY;

  DATA PREDICT; SET PREDICT DIGITAL;
  YEARS=MONTHS/12;
*******************************************************************************;
* Do predictions                                                               ;
  PROC SORT DATA=PREDICT; BY MAL;
  LIBNAME EXAMPLES '!HZEXAMPLES/sasest';
  %HAZPRED (
  PROC HAZPRED DATA=PREDICT INHAZ=EXAMPLES.HMDEATH OUT=PREDICT;
       TIME MONTHS; ID MAL;
  );
*******************************************************************************;
* Printer plot survival and hazard                                             ;
  PROC PLOT DATA=PREDICT; PLOT _SURVIV*YEARS=MAL
               _CLLSURV*YEARS='.' _CLUSURV*YEARS='.'
               /OVERLAY VAXIS=0 TO 1 BY 0.1;
                          PLOT _HAZARD*YEARS=MAL
               _CLLHAZ*YEARS='.' _CLUHAZ*YEARS='.'
               /OVERLAY VAXIS=0 TO 0.1 BY 0.01;
*******************************************************************************;
* Digital nomogram                                                             ;
  DATA DIGITAL; SET PREDICT; IF DIGITAL=1;
  PROC SORT; BY MAL;
  PROC PRINT D; BY MAL; VAR MONTHS YEARS
       OPYEAR OPMOS AGE COM_IV MAL INC_SURG ORIFICE STATUS
       _SURVIV _CLLSURV _CLUSURV _HAZARD _CLLHAZ _CLUHAZ;
*******************************************************************************;
RUN; 
