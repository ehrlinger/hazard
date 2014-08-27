  OPTIONS PAGESIZE=100 LINESIZE=132;
*******************************************************************************;
* !HZEXAMPLES/hm.death.AVC.deciles                                             ;
*                                                                              ;
* Multivariable analysis of death after repair                                 ;
* Decile goodness of fit testing included                                      ;
* NOTE:  we will use just a few of the variables for illustration              ;
*******************************************************************************;
* Bring in data                                                                ;
  TITLE1 'Repair of Atrioventricular Septal (A-V Canal) Defects';
  TITLE2 'Example Data Set';
  TITLE3 'Multivariable Analysis of Death After Repair';
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
* Data transformations                                                         ;
* Logarithmic:                                                                 ;
  LN_AGE=LOG(AGE);
  LN_OPMOS=LOG(OPMOS);
  LN_INC=.; IF INC_SURG NE . THEN LN_INC=LOG(INC_SURG+1);
  LN_NYHA=LOG(STATUS);
* Inverse:                                                                     ;
* Power:                                                                       ;
* Exponential:                                                                 ;
* Categorizations:                                                             ;
* New variables:                                                               ;
*______________________________________________________________________________;
*                                                                              ;
* NOTE:  These transformations would normally be suggested by some logistic    ;
*        or Gompertz plots such as in lg.death.AVC.deciles.deciles                             ;
*______________________________________________________________________________;
*                                                                              ;
*******************************************************************************;
* Interactions                                                                 ;
* OP_AGE=OPMOS*AGE; /* Interaction term already is on the input file */
*______________________________________________________________________________;
*                                                                              ;
* NOTE:  From a medical standpoint, important interactions with the two types  ;
*        a AV canal (complete and partial, as embodies in COM_IV) would be     ;
*        important.  This was done with separate analyses, and no interactions ;
*        were, in fact, identified.                                            ;
*______________________________________________________________________________;
*                                                                              ;
*******************************************************************************;
* Standardize variables with missing values                                    ;
  PROC STANDARD DATA=AVCS OUT=AVCS REPLACE; VAR INC_SURG LN_INC;
*______________________________________________________________________________;
*                                                                              ;
*               F I N A L   M U L T I V A R I A B L  E   M O D E L             ;
*______________________________________________________________________________;
*                                                                              ;
  %HAZARD(
  PROC HAZARD DATA=AVCS P CONSERVE OUTHAZ=OUTEST CONDITION=14
                QUASI;
       TIME INT_DEAD;
       EVENT DEAD;
       PARMS MUE=0.3504743 THALF=0.1905077 NU=1.437416 M=1 FIXM
             MUC=4.391673E-07;
       EARLY AGE=-0.03205774, COM_IV=1.336675, MAL=0.6872028,
             OPMOS=-0.01963377, OP_AGE=0.0002086689, STATUS=0.5169533;
       CONSTANT INC_SURG=1.375285, ORIFICE=3.11765, STATUS=1.054988;
  );
*______________________________________________________________________________;
*                                                                              ;
*     D E C I L E   A N A L Y S I S   O F   G O O D N E S S   O F   F I T      ;
*______________________________________________________________________________;
*                                                                              ;
* Bring in decile macro                                                        ;
* At 10-year point, test for sum of curves and deciles                         ;
  FILENAME DECILES ('!MACROS/deciles.hazard.sas');
  %INC DECILES;
  %DECILES(IN=AVCS, EVENT=DEAD, INTERVAL=INT_DEAD, TIME=120, GROUPS=5,
           VARS=AGE COM_IV MAL OPMOS STATUS ORIFICE INC_SURG,
           OUTEST=OUTEST);
*******************************************************************************;
  RUN;
