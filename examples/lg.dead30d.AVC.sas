  OPTIONS PAGESIZE=100 LINESIZE=132;
*******************************************************************************;
* !HZEXAMPLES/lg.dead30d.AVC                                                   ;
*                                                                              ;
* Descriptive analysis of death after repair                                   ;
* Transformations of scale investigation                                       ;
* Death within 30 days of repair                                               ;
*******************************************************************************;
* Bring in data                                                                ;
  TITLE1 "Repair of Atrioventricular Septal (A-V Canal) Defects";
  TITLE2 "Example Data Set";
  TITLE3 "Death Within 30 Days Of Repair";
  TITLE6 "Investigate Transformations of Scale";
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
  DEAD30D=DEAD; IF DEAD AND INT_DEAD>(30*12/365.2425) THEN DEAD30D=0;
  LABEL DEAD30D = 'Death within 30 days of repair';
*******************************************************************************;
* Bring in LOGIT macro                                                         ;
  FILENAME LOGIT ('!MACROS/logit.sas');
  FILENAME LOGITGR ('!MACROS/logitgr.sas');
  OPTIONS NOSOURCE; %INC LOGIT LOGITGR; OPTIONS SOURCE;
*______________________________________________________________________________;
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:            DEFAULT:                        ;
*                                                                              ;
*   EVENT    = Event (0/1) indicator             EVENT                         ;
*   VAR      = Variable to be categorized                                      ;
*   GROUPS   = Number of groups (deciles)        10                            ;
*   MEAN     = Mean within each group of VAR     MEAN                          ;
*   MAX      = Max within each group of VAR      MAX                           ;
*   MIN      = Min within each group of VAR      MIN                           ;
*   PROB     = Group probability of event        PROB                          ;
*   LOGIT    = Logit for each group              PROB                          ;
*   N        = Number of observations in decile  N                             ;
*   SUM      = Number of events in decile        EVENTS                        ;
*   IN       = Input data set name                                             ;
*   OUT      = Output data set name              LOGIT                         ;
*   GOMPIT   = Use Gompertz rather than logistic 0 (1 for GOMPIT)              ;
*   COX      = Use Cox transform (const. hazard) 0 (1 for COX)                 ;
*   INTERVAL = Interval to event (Cox method)    Not give                      ;
*                                                                              ;
* NOTE:  The output data set consists of GROUP observations from which         ;
*        various plots and relations can be run.                               ;
*______________________________________________________________________________;
*                                                                              ;
* Decile Analysis of AGE                                                       ;
  %LOGIT(VAR=AGE, EVENT=DEAD30D, GROUPS=5, IN=AVCS, GOMPIT=1);
  TITLE5 'Age at Repair (AGE)';
  PROC PRINT DATA=LOGIT;
  DATA LOGIT; SET LOGIT;
  LN_MEAN=LOG(MEAN); IN_MEAN=1/MEAN; IN2MEAN=IN_MEAN*IN_MEAN;
  PROC PLOT; PLOT LOGIT*MEAN='*' LOGIT*LN_MEAN='*' LOGIT*IN_MEAN='*'
             LOGIT*IN2MEAN='*';
*******************************************************************************;
* Decile Analysis of STATUS                                                    ;
  %LOGITGR(VAR=STATUS, EVENT=DEAD30D, GROUPS=STATUS, IN=AVCS, GOMPIT=1);
  TITLE5 'Clinical Status (STATUS)';
  PROC PRINT DATA=LOGIT;
  DATA LOGIT; SET LOGIT;
  LN_MEAN=LOG(MEAN); IN_MEAN=1/MEAN; IN2MEAN=IN_MEAN*IN_MEAN;
  PROC PLOT; PLOT LOGIT*MEAN='*' LOGIT*LN_MEAN='*' LOGIT*IN_MEAN='*'
             LOGIT*IN2MEAN='*';
*******************************************************************************;
* Decile Analysis of INC_SURG                                                  ;
  %LOGIT(VAR=INC_SURG, EVENT=DEAD30D, IN=AVCS, GOMPIT=1);
  TITLE5 'Surgical Assessment of Grade of AV Valve Incompetence (INC_SURG)';
  PROC PRINT DATA=LOGIT;
  DATA LOGIT; SET LOGIT;
  LN_MEAN=LOG(MEAN+1); IN_MEAN=1/(MEAN+1); IN2MEAN=IN_MEAN*IN_MEAN;
  PROC PLOT; PLOT LOGIT*MEAN='*' LOGIT*LN_MEAN='*' LOGIT*IN_MEAN='*'
             LOGIT*IN2MEAN='*';
*******************************************************************************;
* Decile Analysis of OPMOS                                                     ;
  %LOGIT(VAR=OPMOS, EVENT=DEAD30D, GROUPS=4, IN=AVCS, GOMPIT=1);
  TITLE5 'Date of Repair (Months Since 1/67) (OPMOS)';
  PROC PRINT DATA=LOGIT;
  DATA LOGIT; SET LOGIT;
  LN_MEAN=LOG(MEAN); IN_MEAN=1/MEAN; IN2MEAN=IN_MEAN*IN_MEAN;
  PROC PLOT; PLOT LOGIT*MEAN='*' LOGIT*LN_MEAN='*' LOGIT*IN_MEAN='*'
             LOGIT*IN2MEAN='*';
*******************************************************************************;
