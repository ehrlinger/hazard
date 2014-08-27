  OPTIONS PAGESIZE=100 LINESIZE=132;
*******************************************************************************;
* !HZEXAMPLES/hm.death.AVC                                                     ;
*                                                                              ;
* Multivariable analysis of death after repair                                 ;
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
*        or Gompertz plots such as in lg.death.AVC                             ;
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
* NOTE:  What to do about the few missing values we have?  Here we suggest     ;
*        standardizing to the mean of the entire group of patients.            ;
*______________________________________________________________________________;
*                                                                              ;
* Stepwise analysis                                                            ;
  %HAZARD(
  PROC HAZARD DATA=AVCS NOCOV NOCOR CONDITION=14;
       TIME INT_DEAD;
       EVENT DEAD;
       PARMS MUE=0.2361697 THALF=0.1511909 NU=1.438631 M=1 FIXM
             FIXTHALF FIXNU
             MUC=0.0005437256;
*______________________________________________________________________________;
*                                                                              ;
* Notice that we fix the shaping parameters during the analysis process.  This ;
* greatly (by an order of magnitude, at least) accelerates the optimization and;
* avoids many problems.  After the analysis, one will want to fit all the      ;
* model parameters.                                                            ;
*______________________________________________________________________________;
*                                                                              ;
       SELECTION SLE=0.2 SLS=0.1;
*______________________________________________________________________________;
*                                                                              ;
* Notice that we try to group the variables in some medically rational way.    ;
* Indeed, organizing the variables, evaluating them individually, and          ;
* discovering their interrelations are necessary prior to any multivariable    ;
* analysis.  Also, this is a plain stepwise foreward variable selection.       ;
* You may wish to try ORDER options, MOVE options, MAXVARS, MAXSTEPS, RESTRICT ;
* and other options.                                                           ;
*______________________________________________________________________________;
*                                                                              ;
       EARLY
  /* Demographic variables */
         AGE,
  /* Clinical variables */
         STATUS,
  /* Morphologic variables */
         COM_IV/S, ORIFICE, MAL/I, INC_SURG,
  /* Surgical experience variables */
         OPMOS,
  /* Interaction terms */
         OP_AGE
       ;
*______________________________________________________________________________;
*                                                                              ;
* Notice that there is a comma separating the variables.  This is necessary    ;
* so that we can add variable control directly to each variable, such as       ;
* COM_IV/S to indicate starting with this variable in the model, and MAL/I     ;
* meaning that we are including this variable in the model.                    ;
*______________________________________________________________________________;
*                                                                              ;
       CONSTANT
  /* Demographic variables */
         AGE,
  /* Clinical variables */
         STATUS,
  /* Morphologic variables */
         COM_IV/E, ORIFICE/S, MAL/S, INC_SURG/I,
  /* Surgical experience variables */
         OPMOS,
  /* Interaction terms */
         OP_AGE/S
       ;
*______________________________________________________________________________;
*                                                                              ;
* ORIFICE and OP_AGE get flagged as having poorly defined Q-statistics.  So    ;
* the way to test that out is simply to use the /S stating variable option     ;
* and try them in the model.  COM_IV causes singularity or attains a large     ;
* negative value in the constant hazard phase.  This is an interesting         ;
* phenomenon, but we have few events attributable to this phase.  It could     ;
* indeed be a large negative risk factor, and should be "forced in" to the     ;
* model no matter what, particularly since it is associated with a P-value     ;
* of .002 (which in this case may be associate with a single death).           ;
*______________________________________________________________________________;
*                                                                              ;
  );
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
  PROC PRINT DATA=OUTEST;
  LIBNAME EXAMPLES ('!HZEXAMPLES/sasest');
  DATA EXAMPLES.HMDEATH (TYPE=HAZ);
       SET OUTEST;
*******************************************************************************;

  RUN;
