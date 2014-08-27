  OPTIONS PAGESIZE=100 LINESIZE=132;
*******************************************************************************;
* !HZEXAMPLES/hz.death.AVC                                                     ;
*                                                                              ;
* Death after repair of atrioventricular septal defects                        ;
*******************************************************************************;
* Bring in data                                                                ;
  TITLE1 'Repair of Atrioventricular Septal (A-V Canal) Defects';
  TITLE2 'Example Data Set';
  TITLE3 'Death After Repair';
  TITLE4 'Determnine Hazard Function';
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
  KEEP INT_DEAD DEAD;
*******************************************************************************;
* Estimate parameters of the distribution of the event                         ;
  LIBNAME EXAMPLES ('!HZEXAMPLES/sasest');
*options mprint mlogic;
  %let hzrdelds = 1;
  %HAZARD(
  PROC HAZARD DATA=AVCS CONSERVE P OUTHAZ=EXAMPLES.HZDEATH
       STEEPEST QUASI CONDITION=14;
       EVENT DEAD;
       TIME INT_DEAD;
       PARMS MUE=0.2361727 THALF=0.1512095 NU=1.438652 M=1 FIXM
             MUC=0.0005436977;
*      PARMS MUE=0.25 THALF=0.15 NU=1  M=1  MUC=0.0005;
*      PARMS MUE=0.25 THALF=0.15 NU=-1 M=1  MUC=0.0005;
*      PARMS MUE=0.25 THALF=0.15 NU=1  M=-1 MUC=0.0005;
  );
*______________________________________________________________________________;
*                                                                              ;
* NOTE:  Normally the cumulative hazard function plot will help in determining ;
*        the number of phases and the starting values for parameters, as       ;
*        described in the documentation.                                       ;
*                                                                              ;
*        Then, for the early phase, we have explored the 3 branches:           ;
*              NU>0, M>0                                                       ;
*              NU<0, M>0                                                       ;
*              NU>0, M<0                                                       ;
*        as seen above by the commented out PARMS statements.  We did this     ;
*        with the plotting stuff below all excluded by /*  */ s.  This job,    ;
*        then, represents the final output of our explorations.  We chose      ;
*        to use STEEPEST QUASI, but the algorithm QUASI would have served      ;
*        as well.  These explorations revealed that for NU>0, M>0, M was not   ;
*        different from 1 (LL -210.5).  For NU<0, M>0, M also was not different;
*        from 1 (LL -210.5), as we might expect, since these branches converge ;
*        when M=1.  For NU>0, M<0, NU is tending toward 2 (LL -210.8).  We     ;
*        have chosen as the final model NU>0, M=1.                             ;
*                                                                              ;
*        An early phase alone has a LL of -215, so the constant phase is       ;
*        needed.                                                               ;
*______________________________________________________________________________;
*                                                                              ;
*******************************************************************************;
* Plot various survival functions                                              ;
  FILENAME HAZPLOT ('!MACROS/hazplot.sas');
  OPTIONS NOSOURCE; %INC HAZPLOT; OPTIONS SOURCE;
  %HAZPLOT(INTERVAL=INT_DEAD,EVENT=DEAD,IN=AVCS,INEST=EXAMPLES.HZDEATH,
           PDETAIL=1, PLOTI=1, HLABEL=Months After Repair,
           ELABEL=Death After Repair);
*******************************************************************************;
* Digital nomogram                                                             ;
  DATA DIG;
  DTY=12/365.2425;
  DO MONTHS=1*DTY,2*DTY,3*DTY,4*DTY,5*DTY,6*DTY,7*DTY,14*DTY,30*DTY,
  1,2,3,6,12,18,24 TO 180 BY 12;
    YEARS=MONTHS/12;
    OUTPUT;
  END;
  %HAZPRED(
  PROC HAZPRED DATA=DIG INHAZ=EXAMPLES.HZDEATH OUT=DIG; TIME MONTHS;
  );
  PROC PRINT D; VAR MONTHS YEARS
       _SURVIV _CLLSURV _CLUSURV _HAZARD _CLLHAZ _CLUHAZ;
*******************************************************************************;
RUN;
