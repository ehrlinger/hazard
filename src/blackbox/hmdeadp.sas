*______________________________________________________________________________;
*                                                                              ;
* /programs/development/hazard/blkbox.unix/hmdeathp.sas
*______________________________________________________________________________;
*                                                                              ;
* Primary valve replacement
*
* Multivariable analysis of death
* Patient risk factors only
*
* Digital nomogram illustration of individual patients
*______________________________________________________________________________;
*                                                                              ;
  OPTIONS PAGESIZE=60 LINESIZE=80;
  DATA DESIGN;
  TITLE1 "Primary Valve Operations";
  TITLE2 "(UAB; 1975 - July 1979; n=1533)";
  TITLE3 "Digital Nomograms of Parametric Survival, and Hazard";
  TITLE4 "Individual Patient Curves";
*______________________________________________________________________________;
*                                                                              ;
* Note 1: The data set is for first time valve REPLACEMENT, with or without    ;
*         previous valve repair and with or without concommitant surgery.      ;
*         The replacement may include aortic, mitral or tricuspid valves, but  ;
*         not pulmonary valve disease.  While the data set is "dated" it is    ;
*         illustrative, and the results of valve replacement have been pretty  ;
*         much static, in a risk-adjust sense, since the 70s.                  ;
*                                                                              ;
* Note 2: Contains only variables likely to be known at the time of treatment  ;
*         recommendation.  Specifically, it does not include globel myocardial ;
*         ischemic time and its interactions, as does the fuller model.        ;
*                                                                              ;
* Model:  HMPDEATH                                                             ;
* Early Hazard Phase:                                                          ;
*   AGE_COP NYHA MV_NYHA DOUBLE AO_PINC                                        ;
* Constant Hazard Phase:                                                       ;
*   AGE_COP BLACK I_PATH                                                       ;
*______________________________________________________________________________;
*                                                                              ;
* Definitions of varibles in the model.  These are the variables that will     ;
* need to take on patient-specific values.                                     ;
  DATA DESIGN;
* AGE_COP=  ;           /* Age at valve replacement (years)                  */
* BLACK= ;              /* Black ethnicity (no=0, yes=1)                     */
* NYHA= ;               /* NYHA Functional Class (1 through 5, where 5       */
                        /*   denotes a patient in shock--usually in valve    */
                        /*   replacemnt, this is a patient done as an        */
                        /*   emergency case for hemodynamic deterioration.)  */
* I_PATH= ;             /* Ischemic etiology of mitral incompetence,         */
                        /*   occurring in the setting of ischemic heart      */
                        /*   disease (no=0, yes=1)                           */
* AO_PINC= ;            /* Pure aortic incompetence as valve pathology       */
                        /*   (no=0, yes=1)                                   */
* MVR= ;                /* Mitral valve replaced (MVR) (no=0, yes=1)         */
* AVR= ;                /* Aortic valve replaced (AVR) (no=0, yes=1)         */
                        /* NOTE:  Subsequently we will create the variable   */
                        /*        DOUBLE to indicate that both aortic and    */
                        /*        mitral valves have been SIMULTANEOUSLY     */
                        /*        replaced.                                  */
*******************************************************************************;
* Set up variable values for several specific patients                         ;
  PATIENT=1; AGE_COP=57; BLACK=0; NYHA=3; I_PATH=0; AO_PINC=0;
             MVR=0; AVR=1; OUTPUT;
  PATIENT=2; AGE_COP=59; BLACK=1; NYHA=3; I_PATH=0; AO_PINC=0;
             MVR=1; AVR=0; OUTPUT;
  PATIENT=3; AGE_COP=63; BLACK=0; NYHA=4; I_PATH=1; AO_PINC=0;
             MVR=1; AVR=0; OUTPUT;
  PATIENT=4; AGE_COP=51; BLACK=0; NYHA=3; I_PATH=0; AO_PINC=1;
             MVR=0; AVR=1; OUTPUT;
  PATIENT=5; AGE_COP=48; BLACK=1; NYHA=4; I_PATH=0; AO_PINC=1;
             MVR=1; AVR=1; OUTPUT;
  PATIENT=6; AGE_COP=50; BLACK=1; NYHA=3; I_PATH=1; AO_PINC=1;
             MVR=1; AVR=1; OUTPUT;
*******************************************************************************;
* Transformations of variables                                                 ;
  DATA DESIGN; SET DESIGN;
  AORTIC=0; MITRAL=0; DOUBLE=0;
    IF AVR>0 THEN AORTIC=1; IF MVR>0 THEN MITRAL=1;
    IF AORTIC AND MITRAL THEN DO;  DOUBLE=1; AORTIC=0; MITRAL=0; END;
* Output an "impossible" patient to check every variable                       ;
  IF PATIENT=6 THEN DO; AORTIC=1; MITRAL=1; END;
* Interaction term                                                             ;
  MV_NYHA=MITRAL*NYHA;

  PROC PRINT D;
*******************************************************************************;
* Generate and output points                                                   ;
  DATA PREDICT; SET DESIGN; DIGITAL=0;
  MAX=72; LN_MAX=LOG(MAX); INC=(5+LN_MAX)/999.9;
  DO LN_TIME=-5 TO LN_MAX BY INC,LN_MAX;
    MONTHS=EXP(LN_TIME);
    OUTPUT;
  END;
  DATA DIGITAL; SET DESIGN; DIGITAL=1;
  DO MONTHS=30*12/365.2425, 6, 12 TO 72 BY 12;
    OUTPUT;
  END;
  DATA PREDICT; SET PREDICT DIGITAL;
*******************************************************************************;
* Do predictions                                                               ;
  libname est '/programs/development/hazard/examples';
  %HAZPRED(
  PROC HAZPRED DATA=PREDICT INHAZ=est.hmdeadp OUT=PREDICT; TIME MONTHS;
  )
*******************************************************************************;
* Printer plot                                                                 ;
  DATA PREDICT; SET PREDICT;
  _LHAZ=LOG(_HAZARD); _CLLLHAZ=LOG(_CLLHAZ); _CLULHAZ=LOG(_CLUHAZ);
  PCT_SURV=100*_SURVIV; PCT_CLLS=100*_CLLSURV; PCT_CLUS=100*_CLUSURV;
  LABEL PCT_SURV = 'Percent Survival'
        MONTHS   = 'Interval (Months) After Replacement'
        _HAZARD  = 'Deaths Per Month'
        _LHAZ    = 'Deaths Per Month (Nat. Log Scale)'
  ;
  PROC SORT; BY PATIENT;
  PROC PLOT; BY PATIENT;
       PLOT _SURVIV*MONTHS='*' _CLLSURV*MONTHS='.' _CLUSURV*MONTHS='.'
            /OVERLAY VAXIS=0 TO 1 BY 0.1 HAXIS=0 TO 72 BY 12;
       PLOT PCT_SURV*MONTHS='*' PCT_CLLS*MONTHS='.' PCT_CLUS*MONTHS='.'
            /OVERLAY VAXIS=0 TO 100 BY 10 HAXIS=0 TO 72 BY 12;
       PLOT _HAZARD*MONTHS='*' _CLLHAZ*MONTHS='.' _CLUHAZ*MONTHS='.'
            /OVERLAY VAXIS=0 TO 0.1 BY 0.01 HAXIS=0 TO 72 BY 12;
       PLOT _LHAZ*MONTHS='*' _CLLLHAZ*MONTHS='.' _CLULHAZ*MONTHS='.'
            /OVERLAY HAXIS=0 TO 72 BY 12;
*******************************************************************************;
* Digital nomogram                                                             ;
  DATA DIGITAL; SET PREDICT; IF DIGITAL=1;
  PROC SORT; BY PATIENT;
  PROC PRINT D NOOBS; BY PATIENT; VAR MONTHS
       AGE_COP BLACK NYHA AVR MVR I_PATH AO_PINC
       PCT_SURV PCT_CLLS PCT_CLUS _HAZARD _CLLHAZ _CLUHAZ;
       PAGEBY PATIENT;
  run;
*******************************************************************************;
