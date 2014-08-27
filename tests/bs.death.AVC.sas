  OPTIONS PAGESIZE=100 LINESIZE=132;
*______________________________________________________________________________;
*                                                                              ;
* !HZEXAMPLES/bs.death.AVC                                                     ;
*                                                                              ;
* Multivariable analysis of death after repair                                 ;
* Bootstrap method                                                             ;
*______________________________________________________________________________;
*                                                                              ;
* Bring in data                                                                ;
  TITLE1 "Repair of Atrioventricular Septal (A-V Canal) Defects";
  TITLE2 "Example Data Set";
  TITLE3 "Bootstrap Multivariable Analysis of Death After Repair";
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
* Establish output data sets, ready for values (NOTE:  set up data sets        ;
* separately for each hazard phase:  early, constant, and late, even if you    ;
* have only one hazard phase defined.  The macro assumes 3 phases.)            ;
  DATA OUTEARLY; DATA OUTCONST; DATA OUTLATE;
*______________________________________________________________________________;
*                                                                              ;
* Hazard multivariable model for the bootstrap processing                      ;
*______________________________________________________________________________;
*                                                                              ;
  %MACRO HMMODEL;
       TIME INT_DEAD;
       EVENT DEAD;
       PARMS MUE=0.2361697 THALF=0.1511909 NU=1.438631 M=1 FIXM
             FIXTHALF FIXNU
             MUC=0.0005437256;
       EARLY
  /* Demographic variables */
         AGE,
  /* Clinical variables */
         STATUS,
  /* Morphologic variables */
         COM_IV, ORIFICE, MAL, INC_SURG,
  /* Surgical experience variables */
         OPMOS,
  /* Interaction terms */
         OP_AGE
       ;
       CONSTANT
  /* Demographic variables */
         AGE,
  /* Clinical variables */
         STATUS,
  /* Morphologic variables */
         COM_IV/E, ORIFICE, MAL, INC_SURG,
  /* Surgical experience variables */
         OPMOS,
  /* Interaction terms */
         OP_AGE
       ;
  %MEND HMMODEL;
*______________________________________________________________________________;
*                                                                              ;
* Bring in hazard bootstrap macro                                              ;
  FILENAME HAZBOOT ('!MACROS/bootstrap.hazard.sas'); %INC HAZBOOT;
*______________________________________________________________________________;
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:                         DEFAULT:           ;
*                                                                              ;
*   IN       = Input data set name                          IN                 ;
*   OUTEARLY = Output dataset name, early phase             OUTEARLY           ;
*   OUTCONST = Output dataset name, constant phase          OUTCONST           ;
*   OUTLATE  = Output data set name, late phase             OUTLATE            ;
*   SEED     = Label for the event                          -1                 ;
*   RESAMPL  = Number of models to generate                 1000               ;
*   FRACTION = Fractional size of bootstrap                 1.0 (100% of n)    ;
*   SLE      = Criteria for variable entry                  0.07               ;
*   SLS      = Criteria for variable retention              0.05               ;
*                                                                              ;
* IN ADDITION:  A MACRO CALLED HMMODEL IS USED:                                ;
*   HMMODEL  = Macro that defines the model and variables to be used in the    ;
*              bootstrap analysis.  This model needs the following standard    ;
*              PROC HAZARD statements:                                         ;
*      TIME time sas name (this may be interval and left censored as well)     ;
*      EVENT event sas name;                                                   ;
*      PARMS MUE etc MUC etc MUL etc as needed with all shaping parameters     ;
*            FIXed                                                             ;
*      EARLY variable list                                                     ;
*      CONSTANT variable list                                                  ;
*      LATE variable list                                                      ;
*______________________________________________________________________________;
*                                                                              ;
* Call the bootstrap macro                                                     ;
  %HAZBOOT(IN=AVCS, SEED=-1, RESAMPL=5, SLE=0.12, SLS=0.1);
*******************************************************************************;
* Store the results for later statistics                                       ;
* NOTE:  The first observation is empty, so it can be deleted                  ;
  LIBNAME EXAMPLES ('!HZEXAMPLES/sasest');
  DATA EXAMPLES.BSAVCE; SET OUTEARLY; IF _N_>1;
    TITLE5 "Early";
    PROC PRINT;
    RUN;
  DATA EXAMPLES.BSAVCC; SET OUTCONST; IF _N_>1;
    TITLE5 "Constant";
    PROC PRINT;
    RUN;
  DATA EXAMPLES.BSAVCL; SET OUTLATE; IF _N_>1;
    TITLE5 "Late";
    PROC PRINT;
    RUN;
*******************************************************************************;
