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
  TITLE4 'Determine Hazard Function';
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
* Bring in actuarial macros                                                    ;
  FILENAME KAPLAN ('!MACROS/kaplan.sas');
  FILENAME NELSON ('!MACROS/nelsont.sas');
  %INC KAPLAN NELSON;
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
  %KAPLAN(IN=AVCS, OUT=PLOUT, INTERVAL=INT_DEAD, EVENT=DEAD, PEVENT=1,
          HLABEL=Months After Repair);
  DATA PLOUT; SET PLOUT; IF DEAD=1;
  CUM_SURV=CUM_SURV*100;
  BAR=0; CL_LOWER=CL_LOWER*100; CL_UPPER=CL_UPPER*100;
  IF NUMBER IN (14,77,201) THEN BAR=1;
  IF BAR=0 THEN DO; CL_LOWER=.; CL_UPPER=.; END;
*******************************************************************************;
* Generate and output time points                                              ;
  DATA PREDICT; DIGITAL=0;
  MAX=180; LN_MAX=LOG(MAX); INC=(5+LN_MAX)/99.9;
  DO LN_TIME=-5 TO LN_MAX BY INC,LN_MAX;
    MONTHS=EXP(LN_TIME);
    OUTPUT;
  END;
  DROP MAX LN_MAX INC LN_TIME;

  DATA DIGITAL; DIGITAL=1;
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
  LIBNAME EXAMPLES ('!HZEXAMPLES/sasest');
  %HAZPRED(
  PROC HAZPRED DATA=PREDICT INHAZ=EXAMPLES.HZDEATH OUT=PREDICT; TIME MONTHS; );
*******************************************************************************;
* Printer plot survival and hazard                                             ;
  DATA PLOT; SET PREDICT PLOUT; CUMSURV=CUM_SURV/100;
  PROC PLOT DATA=PLOT;
       PLOT _SURVIV*MONTHS='*' _CLLSURV*MONTHS='.' _CLUSURV*MONTHS='.'
            CUMSURV*INT_DEAD='K'/OVERLAY VAXIS=0 TO 1 BY 0.1;
       PLOT _HAZARD*MONTHS='*' _CLLHAZ*MONTHS='.' _CLUHAZ*MONTHS='.'
            /OVERLAY VAXIS=0 TO .2 BY 0.02;
*******************************************************************************;
* Digital nomogram                                                             ;
  DATA DIGITAL; SET PREDICT; IF DIGITAL=1;
  PROC PRINT D; VAR YEARS MONTHS
       _SURVIV _CLLSURV _CLUSURV _HAZARD _CLLHAZ _CLUHAZ;
*______________________________________________________________________________;
*                                                                              ;
* Scale surviorship function to percent                                        ;
  DATA PREDICT; SET PREDICT; IF DIGITAL=0;
  _SURVIV=100*_SURVIV; _CLLSURV=100*_CLLSURV; _CLUSURV=100*_CLUSURV;
*******************************************************************************;
* Bring in PostScript plot macro                                               ;
  FILENAME PLT ('!MACROS/plot.sas');
  OPTIONS NOSOURCE; %INC PLT; OPTIONS SOURCE;
*******************************************************************************;
* Output plot file                                                             ;
  FILENAME GSASFILE 'avc.hp.death.AVC';
  %PLOT(
        ID L="avc.hp.death.AVC survival", END;
        LABELX L="Interval (Months) After Repair", END;
        LABELY L="Percent Survival", END;
        LOGO3 L="A-V Septal Defects", NL, L="(Example; Date; n=310)", END;
        AXISX ORDER=(0 TO 180 BY 12), MINOR=(NUMBER=1), END;
        AXISY ORDER=(0 TO 100 BY 10), MINOR=(NUMBER=4), END;
        TUPLE SET=PREDICT, X=MONTHS,
              Y=_SURVIV, CLL=_CLLSURV, CLU=_CLUSURV, LINECL=20, END;
        TUPLE SET=PLOUT, SYMBOL=CIRCLE,
              SYMBSIZE=3/4, EBARSIZE=3/4, LINEPE=0, LINECL=0, EBAR=1,
              X=INT_DEAD, Y=CUM_SURV, CLL=CL_LOWER, CLU=CL_UPPER, END;
        );

  %PLOT(
        ID L="avc.hp.death.AVC hazard", END;
        LABELX L="Interval (Months) After Repair", END;
        LABELY L="Deaths", BUL, L="Year", SUP="-1", END;
        LOGO1 L="A-V Septal Defects", NL, L="(Example; Date; n=310)", END;
        AXISX ORDER=(0 TO 180 BY 12), MINOR=(NUMBER=1), END;
        AXISY ORDER=(0 TO .05 BY .01), MINOR=(NUMBER=4), END;
        TUPLE SET=PREDICT, X=MONTHS,
              Y=_HAZARD, CLL=_CLLHAZ, CLU=_CLUHAZ, LINECL=20, END;
        );
*______________________________________________________________________________;
*                                                                              ;
RUN;
