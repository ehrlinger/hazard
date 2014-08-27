  OPTIONS PAGESIZE=100 LINESIZE=132;
*******************************************************************************;
* !HZEXAMPLES/ac.death.AVC                                                     ;
*                                                                              ;
* Life table for death after repair of atrioventricular septal defects         ;
*******************************************************************************;
* Bring in data                                                                ;
  TITLE1 "Repair of Atrioventricular Septal (A-V Canal) Defects";
  TITLE2 "Example Data Set";
  TITLE3 "Life Table For Death After Repair";
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
  KEEP INT_DEAD DEAD COM_IV;
*******************************************************************************;
* Bring in actuarial macros                                                    ;
  FILENAME KAPLAN ('!MACROS/kaplan.sas');
  FILENAME NELSON ('!MACROS/nelsont.sas');
  OPTIONS NOSOURCE; %INC KAPLAN NELSON; OPTIONS SOURCE;
*______________________________________________________________________________;
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:            DEFAULT:                        ;
*                                                                              ;
*   INTERVAL = Time variable                     INTERVAL                      ;
*   EVENT    = Number of events                  EVENT                         ;
*   RCENSOR  = Number of censored cases          0                             ;
*   STRATIFY = Stratification variable           'ALL'                         ;
*   OTHSUMS  = Other variables to be summed      none                          ;
*   ID       = Variables retained in data set    none                          ;
*   CLEVEL   = Confidence coefficient            0.68268948 (1 sd)             ;
*   IN       = Input data set name                                             ;
*   OUT      = Output data set name              EVENT                         ;
*   HLABEL   = Label for time axis of plots      Years After Operation         ;
*   ELABEL   = Label for the event               Death                         ;
*   NOPRINT  = No printed output                 0 (1 for no printing)         ;
*   NOLIST   = Only plotter output, no lists     0 (1 for no lists)            ;
*   PEVENT   = Print only the events             1 (0 for censored also)       ;
*   OEVENT   = Output only the events            1 (0 for censored also)       ;
*   PLOTS    = Plot survival                     1 (0 to skip)                 ;
*   PLOTC    = Plot cumulative hazard            1 (0 to skip)                 ;
*   PLOTH    = Plot hazard                       0 (1 to plot)                 ;
*   PLOTL    = Plot integrated survivorship      0 (1 to plot)                 ;
*______________________________________________________________________________;
*                                                                              ;
  %KAPLAN(IN=AVCS, INTERVAL=INT_DEAD, EVENT=DEAD, PLOTH=1, PLOTL=1,
          HLABEL=Months After Repair);
  %NELSONT(IN=AVCS, INTERVAL=INT_DEAD, EVENT=DEAD, PLOTH=1, PLOTL=1,
          HLABEL=Months After Repair);
  %KAPLAN(IN=AVCS, INTERVAL=INT_DEAD, EVENT=DEAD, PLOTH=1, PLOTL=1,
          HLABEL=Months After Repair, STRATIFY=COM_IV);
*******************************************************************************;
RUN;
