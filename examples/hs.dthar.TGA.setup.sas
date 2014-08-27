*______________________________________________________________________________;
*                                                                              ;
* !HZEXAMPLES/hs.dthar.TGA.setup                                               ;
*                                                                              ;
* Arterial Switch Repairs                                                      ;
*                                                                              ;
* Multivariable analysis of risk factors for death after operation             ;
* Patient, Procedure, Support, and Experience risk factors                     ;
*                                                                              ;
* Predict survivorship curve for each experience in the data set.              ;
*                                                                              ;
* Create the dataset that will subsequently be used to form subgroups          ;
* from these curves.                                                           ;
*______________________________________________________________________________;
*                                                                              ;
  OPTIONS PAGESIZE=100 LINESIZE=132;
  TITLE1 "Arterial Switch Operations";
  TITLE2 "Death After Operation";
  TITLE3 "Multivariable Analysis of Risk Factors";
  TITLE4 "Patient, Procedure, Support, and Experience Risk Factors";
  TITLE5 "Output Predictions For Each Patient";
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
        OPYEAR   = 'Year of Operation'
  ;
*______________________________________________________________________________;
*                                                                              ;
* Risk factor model:                                                           ;
*   Early: SIMPLE DEXTROIN CA_1RL2C CA1_2_L HYAAPROC NO_TCA LN_TCAT SIMIAGP1
*          ARCIOPYR
*______________________________________________________________________________;
*                                                                              ;
* Data transformations                                                         ;
  DATA ARTSW; SET ARTSW;
  LN_TCAT=.; IF TCA_TIME NE . THEN LN_TCAT=LOG(TCA_TIME+1);
  IN_AGEP1=.; IF AGE_DAYS NE . THEN IN_AGEP1=1/((AGE_DAYS + 1)*12/365.2425);
*******************************************************************************;
* Standardize for missing values                                               ;
  PROC STANDARD DATA=ARTSW OUT=ARTSW REPLACE; VAR TCA_TIME LN_TCAT;
*******************************************************************************;
* Interaction terms                                                            ;
  DATA ARTSW; SET ARTSW;
  SIMIAGP1=SIMPLE*IN_AGEP1;
*******************************************************************************;
* Generate expected vs observed deaths                                         ;
  LIBNAME TGASW ('!HZEXAMPLES/sasest');
  %HAZPRED(
  PROC HAZPRED DATA=ARTSW INHAZ=TGASW.HMDTHRI1 OUT=DESIGN NOHAZ;
       TIME INT_DEAD;
  );
  DATA DESIGN; SET DESIGN;
  CUM_HAZ=-LOG(_SURVIV);
  PROB=1-_SURVIV;
  KEEP STUDY SIMPLE DEXTROIN CA_1RL2C HYAAPROC NO_TCA TCA_TIME AGE_DAYS ARCIOPYR
       DEAD INT_DEAD SOURCE CA1_2_L LN_TCAT IN_AGEP1 SIMIAGP1 CUM_HAZ PROB
  ;
  LIBNAME ARTSWT ('!HZEXAMPLES/data');
  DATA ARTSWT.HSDTARXC; SET DESIGN;
  PROC CONTENTS DATA=ARTSWT.HSDTARXC;
*******************************************************************************;
* Calculate parametric estimates of survivorship for each experience              ;
  DATA DESIGN; SET DESIGN;
  KEEP STUDY SOURCE SIMPLE DEXTROIN CA_1RL2C CA1_2_L HYAAPROC NO_TCA LN_TCAT
       SIMIAGP1 ARCIOPYR;
  DATA PREDICT; SET DESIGN; DIGITAL=0;
  MAX0=96; LN_MAX0=LOG(MAX0); INC0=(5+LN_MAX0)/49.9; /* However many points */
  DO LN_TIME=-5 TO LN_MAX0 BY INC0, LN_MAX0;
    MONTHS=EXP(LN_TIME);
    OUTPUT;
  END;
  DROP LN_TIME LN_MAX0 INC0 MAX0;
  DATA DIGITAL; SET DESIGN; DIGITAL=1;
  DO MONTHS=(12*30/365.2425), 3, 6, 12 TO 96 BY 12;
    OUTPUT;
  END;
  DATA PREDICT; SET PREDICT DIGITAL;
  %HAZPRED(
  PROC HAZPRED DATA=PREDICT INHAZ=TGASW.HMDTHRI1 OUT=PREDICT; TIME MONTHS;
  );
  DATA ARTSWT.HSDTARXP; SET PREDICT;
  KEEP STUDY SOURCE MONTHS _SURVIV _CLLSURV _CLUSURV DIGITAL;
  PROC CONTENTS DATA=ARTSWT.HSDTARXP;
*******************************************************************************;
RUN;
