  OPTIONS PAGESIZE=100 LINESIZE=132;
*******************************************************************************;
* !HZEXAMPLES/hp.death.AVC.hm2                                                 ;
*                                                                              ;
* Multivariable analysis of death after repair                                 ;
*                                                                              ;
* Exploration of strength of risk factors.                                     ;
*                                                                              ;
* Display strength of date of repair in partial and complete forms of AV Canal ;
*******************************************************************************;
  TITLE1 'Repair of Atrioventricular Septal (A-V Canal) Defects';
  TITLE2 'Example Data Set';
  TITLE3 'Multivariable Analysis of Death After Repair';
  TITLE4 'Solve Multivariable Equation for 6-Months Survival';
  TITLE5 'In Complete and Partial Canal According to Date of Repair';
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
* COM_IV=1;      /* Interventricular communication present                    */
  MAL=0;         /* Major associated cardiac anomaly                          */
  ORIFICE=0;     /* Accessory left AV valve orifice                           */
  INC_SURG=0;    /* Degree of AV valve incompetence pre-repair                */
  STATUS=3;      /* Clinical status (NYHA I-IV)                               */
* OPMOS=180;     /* Current data (1982)                                       */
  MONTHS=6;      /* Time of observation (months)                              */
*______________________________________________________________________________;
*                                                                              ;
* Transformations of variables                                                 ;
* OPYEAR=1967 + OPMOS/12;
* OP_AGE=OPMOS*AGE;
  YEARS=MONTHS/12;
*******************************************************************************;
* Generate "isobars" of risk factors here, but check for possible              ;
* transformations that may be needed.                                          ;
  DO COM_IV=0,1;
    OUTPUT;
  END;
  PROC PRINT;
*******************************************************************************;
* Generate and output points                                                   ;
  DATA PREDICT; SET DESIGN; DIGITAL=0;
  MIN=0; MAX=180; INC=(MAX-MIN)/99.9;
  DO OPMOS=MIN TO MAX BY INC, MAX;
    OUTPUT;
  END;
  DROP MIN MAX INC;

  DATA DIGITAL; SET DESIGN; DIGITAL=1;
  DO OPMOS=0 TO 180 BY 12;
    OUTPUT;
  END;

  DATA PREDICT; SET PREDICT DIGITAL;
  OPYEAR=1967 + OPMOS/12;
  OP_AGE=OPMOS*AGE;
*******************************************************************************;
* Do predictions                                                               ;
  LIBNAME EXAMPLES ('!HZEXAMPLES/sasest');
  %HAZPRED (
  PROC HAZPRED DATA=PREDICT INHAZ=EXAMPLES.HMDEATH OUT=PREDICT;
       TIME MONTHS;
  );
*******************************************************************************;
* Printer plot survival and hazard                                             ;
  PROC PLOT DATA=PREDICT; PLOT _SURVIV*OPYEAR=COM_IV
               _CLLSURV*OPYEAR='.' _CLUSURV*OPYEAR='.'
               /OVERLAY VAXIS=0 TO 1 BY 0.1;
*******************************************************************************;
* Digital nomogram                                                             ;
  DATA DIGITAL; SET PREDICT; IF DIGITAL=1;
  PROC SORT; BY COM_IV;
  PROC PRINT D; BY COM_IV; VAR MONTHS YEARS
       OPYEAR OPMOS AGE COM_IV MAL INC_SURG ORIFICE STATUS
       _SURVIV _CLLSURV _CLUSURV _HAZARD _CLLHAZ _CLUHAZ;
*******************************************************************************;
RUN;
