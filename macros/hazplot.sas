*******************************************************************************;
* <2013-02-25>                                                                 ;
* Plot of Various Functions from Three-phase Parametric Model                  ;
  %MACRO HAZPLOT(INTERVAL=INTERVAL, EVENT=EVENT, RCENSOR=0, REPEAT=0, 
         WEIGHTED=0,WEIGHT=1,
         STIME=STIME, HAZSCALE=NONE, CLEVEL=0.68268948, IN=, ID=, DSDELETE=0,
         INEST=, STRATIFY='ALL', HLABEL=Months After Operation, ELABEL=Death,
         PLOTS=1, PLOTC=1, PLOTH=1, PLOTI=0, PDETAIL=0, PLOTG=1);
*______________________________________________________________________________;
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:            DEFAULT:                        ;
*                                                                              ;
*   INTERVAL = Time variable                     INTERVAL                      ;
*   EVENT    = Event count                       EVENT                         ;
*   RCENSOR  = Censored count                    0                             ;
*   STIME    = Start time                        0                             ;
*   REPEAT   = Repeating event                   0  (1 for yes)                ;
*   WEIGHTED = Weighted event                    0  (1 for yes)                ;
*   WEIGHT   = Severity of the event             1  (or weight variable)       ;
*   CLEVEL   = Confidence coefficient            0.68268948 (1 sd)             ;
*   IN       = Input data set name                                             ;
*   ID       = Variables retained in data set    none                          ;
*   DSDELETE = Delete data sets created          0 (1 to delete)               ;
*   INEST    = Estimate data set name                                          ;
*   STRATIFY = Variable for stratification       ALL                           ;
*   HLABEL   = Label for time axis of plots      Years After Operation         ;
*   ELABEL   = Label for the event               Death                         ;
*   PLOTS    = Plot survival                     1 (0 to skip)                 ;
*   PLOTC    = Plot cumulative hazard            1 (0 to skip)                 ;
*   PLOTH    = Plot hazard                       1 (0 to skip)                 ;
*   HAZSCALE = Maximum upper range of Y axis     NONE                          ;
*              for plots using _HAZARD _EARLYH and _LATEH                      ;
*   PLOTI    = Plot hazard components            0 (1 to plot)                 ;
*   PDETAIL  = Detailed and log plots            0 (1 to plot)                 ;
*   PLOTG    = Plot goodness of fit              1 (1 to plot)                 ;
*______________________________________________________________________________;
*                                                                              ;
* Calculate nonparametric estimates                                            ;
  filename kaplan ('!MACROS/kaplan.sas'); %inc kaplan;
  filename nelson ('!MACROS/nelsonl.sas'); %inc nelson;
* proc print data=&in;

  DATA &IN; SET &IN;
  
  %if &repeat=0 and &weighted=0 %then %do;
    %kaplan(in=&in, out=plout, interval=&interval, event=&event, id=&id,
            rcensor=&rcensor, stratify=&stratify, oevent=0, noprint=1);
    data plout; set plout; _censrd_=censored;
  %end;

  %if &repeat=0 and &weighted=1 %then %do;
     _rcens1=1;
    %nelsonl(in=&in, out=plout, interval=&interval, repeat=&repeat, event=&event, weight=&weight,
             censored=_rcens1, stratify=&stratify);
    data plout; set plout; _censrd_=_rcens1;
  %end;
 
   %if &repeat=1 and &weighted=0 %then %do;
    %nelsonl(in=&in, out=plout, interval=&interval, repeat=&repeat, event=&event, id=&stime, weight=&weight,
             censored=&rcensor, stratify=&stratify);
    data plout; set plout; _censrd_=&rcensor;
  %end;
  
  %if &repeat=1 and &weighted=1 %then %do;
    %nelsonl(in=&in, out=plout, interval=&interval, repeat=&repeat, event=&event, id=&stime, weight=&weight,
             censored=&rcensor, stratify=&stratify);
    data plout; set plout; _censrd_=&rcensor;
  %end;
* proc print;
*******************************************************************************;
* Set up variables for goodness of fit calculations                            ;
  data plout; set plout;
  total=&event + _censrd_;
  %if &repeat=0 and &weighted=0 %then %do;
    retain nobs;
    if _n_=1 then nobs=number;
  %end;
  %if &repeat=1 or &weighted=1 %then %do;
    retain nobs 0;
    if _censrd_=&event then total=total-_censrd_;
    nobs=nobs+total;
*   proc print;
    data nobs; set plout end=last;
    if last; number=nobs;
    keep number;
*   proc print data=nobs;
    data plout; set plout; drop nobs;
    data plout; merge nobs plout;
    data plout; set plout;
    retain nobs;
    if _n_=1 then nobs=number;
*   proc print;
  %end;
*******************************************************************************;
* Calculate parametric estimates at start time for repeated events             ;
  %hazpred(
  proc hazpred data=plout inhaz=&inest out=predict; time &interval; );
  data predict; set predict;
  rcumhaz=0; rcumhaze=0; rcumhazc=0; rcumhazl=0;
* proc print data=predict;
*******************************************************************************;
* Calculate estimates from start time if repeat events                         ;
  %if &repeat=1  %then %do;
    %hazpred(
    proc hazpred data=plout inhaz=&inest out=predicts; time &stime; );
    data predicts; set predicts;
    rcumhaz=-log(_surviv); rcumhaze=-log(_earlys); rcumhazc=-log(_consts);
    rcumhazl=-log(_lates);
    keep rcumhaz rcumhaze rcumhazc rcumhazl;
    data predict; merge predict predicts;
*   proc print data=predicts;
*   proc print data=predict;
  %end;
  %if &dsdelete %then %do; proc delete data=&in; proc delete data=plout; %end;
*******************************************************************************;
* Plot various survival functions                                              ;
  DATA PREDICT; SET PREDICT; IF &INTERVAL LE 0 THEN DELETE;
  IF &EVENT GE 1 THEN DO; PLTCHR='+'; PLTCL='-'; END;
  IF &EVENT=0 THEN DO; PLTCHR='.'; PLTCL=' '; END;
  PLTPAR='*';
  LABEL
         _SURVIV   = Proportion Free of &ELABEL
         _HAZARD   = Hazard for &ELABEL
         _EARLYH   = Hazard Components
         &INTERVAL = &HLABEL
  ;
  %IF &PDETAIL %THEN %DO;
    LN_TIME=LOG(&INTERVAL);
    LABEL LN_TIME  = Ln(&HLABEL);
  %END;
* Survivorship function                                                        ;
  %IF &PLOTS & &PDETAIL & &repeat=0 & &weighted=0 %THEN %DO;
    D_SURVIV=CUM_SURV-_SURVIV;
    LABEL D_SURVIV  = Actuarial Minus Parametric Estimates;
  %END;
* Hazard function                                                              ;
  %IF &PLOTH %THEN %DO;
    _LNHAZ=.; IF _HAZARD>0 THEN _LNHAZ=LOG(_HAZARD);
    _LNCLLHZ=.; IF _CLLHAZ>0 THEN _LNCLLHZ=LOG(_CLLHAZ);
    _LNCLUHZ=.; IF _CLUHAZ>0 THEN _LNCLUHZ=LOG(_CLUHAZ);
    LABEL _LNHAZ = Logarithm of Hazard Function;
    %IF &PLOTI %THEN %DO;
       _EARLYR=0;  IF _EARLYH>0 THEN _EARLYR=100*_EARLYH/_HAZARD;
       _CONSTR=0;  IF _CONSTH>0 THEN _CONSTR=100*_CONSTH/_HAZARD;
       _LATER=0;  IF _LATEH>0 THEN _LATER=100*_LATEH/_HAZARD;
       LABEL
              _EARLYR   = '% of hazard in early phase'
              _CONSTR   = '% of hazard in constant phase'
              _LATER    = '% of hazard in late phase'
       ;
    %END;
  %END;
* Cumulative hazard function                                                   ;
  %IF &PLOTC or &plotg %THEN %DO;
    %if &repeat=0 and &weighted=0 %then %do;
      cum_haz=-log(cum_surv); cll_chz=-log(cl_upper); clu_chz=-log(cl_lower);
    %end;
    %if &repeat=1 or &weighted=1 %then %do;
      cum_haz=cumdist; cll_chz=cll_cef; clu_chz=clu_cef;
    %end;
    _CUMHAZ=-LOG(_SURVIV);
    _CLLCHZ=-LOG(_CLUSURV); _CLUCHZ=-LOG(_CLLSURV);
    _earlyc=-log(_earlys);
    _constc=-log(_consts);
    _latec=-log(_lates);
    LABEL _CUMHAZ = Cumulative Hazard Function;
    %IF &PDETAIL %THEN %DO;
      D_CUMHAZ=CUM_HAZ-_CUMHAZ;
      LABEL D_CUMHAZ  = Cumulative Hazard Minus Parametric Estimates;
      LN_CHAZ_=.; IF CUM_HAZ>0 THEN LN_CHAZ_=LOG(CUM_HAZ);
      LN_CUMHZ=.; IF _CUMHAZ>0 THEN LN_CUMHZ=LOG(_CUMHAZ);
      LABEL LN_CUMHZ = Log Cumulative Hazard Function;
      _LNCLLCH=.; IF _CLLCHZ>0 THEN _LNCLLCH=LOG(_CLLCHZ);
      _LNCLUCH=.; IF _CLUCHZ>0 THEN _LNCLUCH=LOG(_CLUCHZ);
      LN_CLLCH=.; IF CLL_CHZ>0 THEN LN_CLLCH=LOG(CLL_CHZ);
      LN_CLUCH=.; IF CLU_CHZ>0 THEN LN_CLUCH=LOG(CLU_CHZ);
    %END;
  %END;
* Goodness of fit by the method of conservation of events                      ;
  %IF &PLOTG %THEN %DO;
    _CUMHAZ=-LOG(_SURVIV);
*******************************************************************************;
*                                                                              ;
* G O O D N E S S   O F   F I T   O F   P A R A M E T R I C   M O D E L        ;
*                                                                              ;
* T O   T H E   D A T A   B A S E D   O N   T H E   T H E O R Y   O F          ;
*                                                                              ;
*         C O N S E R V A T I O N   O F   E V E N T S                          ;
*                                                                              ;
* The theory is that the summed cumulative hazard for all patients at          ;
* risk should equal the number of deaths.  This is true (exactly if            ;
* CONSERVE option was on for PROC HAZARD, and very nearly true if not)         ;
* for the entire interval of followup.  However, at any given moment in        ;
* the total span of followup time, the accumulated cumulative hazard to        ;
* that time will be more or less equal to the number of events,                ;
* depending on the goodness of fit of the model to the data.                   ;
*                                                                              ;
* In the case of no concomitant information,  all patients at risk to a        ;
* given point in time have the same value for cumulative hazard.               ;
* Therefore, we merely have to multiply the number at risk by the              ;
* value for cumulative hazard between each unique followup interval to         ;
* obtain the integrated value to that point.                                   ;
*                                                                              ;
* In practice, it is about as easy to recompute integrated cumulative          ;
* hazard for patients at risk beyond the interval as to keep summers           ;
* going.  So the computational strategy is to do the summations for the        ;
* patients no longer at risk, and add this to the amount for all               ;
* patients followed beyond that.                                               ;
*                                                                              ;
* NOTE: This code does not as yet work for repeated events analyses            ;
*                                                                              ;
    RETAIN S_CUMHAZ 0 TDEAD 0 SECUMHAZ 0 SCCUMHAZ 0 SLCUMHAZ 0 UNIQUE 0;
* Number of observations that have been censored (or uncensored) at and        ;
* before this particular followup interval.                                    ;
* NOTE:  observations are in order of the followup interval.                   ;
    UNIQUE = UNIQUE + TOTAL;
* Find the total (integrated) cumulative hazard at that interval for the       ;
* observations censored or uncensored at exactly that interval                 ;
    T_CUMHAZ = TOTAL*_CUMHAZ - rcumhaz;    /* Overall            */
    TECUMHAZ = TOTAL*(_earlyc - rcumhaze); /* Early component    */
    TCCUMHAZ = TOTAL*(_constc - rcumhazc); /* Constant component */
    TLCUMHAZ = TOTAL*(_latec - rcumhazl);  /* Late component     */
* Alternative calculations from the documentation                              ;
    NEHAZ=(&EVENT*_EARLYH/_HAZARD);             /* Early component    */
    NCHAZ=(&EVENT*_CONSTH/_HAZARD);             /* Constant component */
    NLHAZ=(&EVENT*_LATEH/_HAZARD) ;             /* Late component     */
*                                                                              ;
* Add this to the integrated total from the previously censored and            ;
* uncensored patients not haveing followup beyond the current interval         ;
    S_CUMHAZ=T_CUMHAZ + S_CUMHAZ;     /* Overall            */
    SECUMHAZ=TECUMHAZ + SECUMHAZ;     /* Early component    */
    SCCUMHAZ=TCCUMHAZ + SCCUMHAZ;     /* Constant component */
    SLCUMHAZ=TLCUMHAZ + SLCUMHAZ;     /* Late component     */

*   S_CUMHAZ=&event   + S_CUMHAZ;     /* Overall            */
*   SECUMHAZ=nehaz    + SECUMHAZ;     /* Early component    */
*   SCCUMHAZ=nchaz    + SCCUMHAZ;     /* Constant component */
*   SLCUMHAZ=nlhaz    + SLCUMHAZ;     /* Late component     */
*                                                                              ;
* Get cumulative hazard integral from time zero or start time up to this       ;
* interval for all patients who have followup beyond this interval.  Add this  ;
* to the parital integral from patients not to be traced beyond this interval  ;

    SUM_CHF=(NOBS - UNIQUE)*(_CUMHAZ - rcumhaz) + S_CUMHAZ;  /* Overall    */
    SUMECHF=(NOBS - UNIQUE)*(_earlyc - rcumhaze) + SECUMHAZ; /* Early      */
    SUMCCHF=(NOBS - UNIQUE)*(_constc - rcumhazc) + SCCUMHAZ; /* Constant   */
    SUMLCHF=(NOBS - UNIQUE)*(_latec - rcumhazl) + SLCUMHAZ;  /* Late       */
* Now sum the observed number of events                                        ;
    TDEAD = &EVENT + TDEAD;
    D_DEAD = SUM_CHF - TDEAD;
  %END;
* Cumulative hazard components                                                 ;
  %IF &PLOTI %THEN %DO;
    _EARLYC=0; LN_EARLC=.; P_EARLY=0;
    LABEL _EARLYC = Early Cumulative Hazard Function;
      IF _EARLYS>0 AND _EARLYS<1 THEN DO;
        P_EARLY=1; _EARLYC=-LOG(_EARLYS);
        %IF &PDETAIL %THEN %DO;
          IF _EARLYC>0 THEN LN_EARLC=LOG(_EARLYC);
          LABEL LN_EARLC = Log Early Cumulative Hazard Function;
        %END;
      END;
    _CONSTC=0; LN_CONSC=.; P_CONST=0;
    LABEL _CONSTC = Constant Cumulative Hazard Function;
      IF _CONSTS>0 AND _CONSTS<1 THEN DO;
        P_CONST=1; _CONSTC=-LOG(_CONSTS);
      END;
    _LATEC=0; LN_LATEC=.; P_LATE=0;
    LABEL _LATEC = Late Cumulative Hazard Function;
      IF _LATES>0 AND _LATES<1 THEN DO;
        P_LATE=1; _LATEC=-LOG(_LATES);
        %IF &PDETAIL %THEN %DO;
          IF _LATEC>0 THEN LN_LATEC=LOG(_LATEC);
          LABEL LN_LATEC = Log Late Cumulative Hazard Function;
        %END;
      END;
* Phase components with others removed                                         ;
    R_EARLYC=CUM_HAZ-_CONSTC-_LATEC;
    R_CONSTC=CUM_HAZ-_EARLYC-_LATEC;
    R_LATEC=CUM_HAZ-_EARLYC-_CONSTC;
    LABEL
          R_EARLYC = Early Cumulative Hazard Component
          R_CONSTC = Constant Cumulative Hazard Component
          R_LATEC  = Late Cumulative Hazard Component
    ;
    %IF &PDETAIL %THEN %DO;
      LN_EC=.; IF R_EARLYC>0 THEN LN_EC=LOG(R_EARLYC);
      LN_CC=.; IF R_CONSTC>0 THEN LN_CC=LOG(R_CONSTC);
      LN_LC=.; IF R_LATEC>0 THEN LN_LC=LOG(R_LATEC);
      LABEL
            LN_EC = Log Early Cumulative Hazard Component
            LN_CC = Log Constant Cumulative Hazard Component
            LN_LC = Log Late Cumulative Hazard Component
      ;
    %END;
* Set up tests for the plots                                                   ;
    PROC SUMMARY DATA=PREDICT NWAY; VAR P_EARLY P_CONST P_LATE;
         OUTPUT OUT=OUTTEST SUM=P_EARLY P_CONST P_LATE;
    DATA OUTTEST; SET OUTTEST;
    CALL SYMPUT('P_EARLY',P_EARLY);
    CALL SYMPUT('P_CONST',P_CONST);
    CALL SYMPUT('P_LATE',P_LATE);
    RUN;
  %END;
* Do plots                                                                     ;
  * Determine upper limit of Y axis for _HAZARD _EARLYH _LATEH;
  %IF &HAZSCALE = NONE %then %let _VAXIS=%STR();
  %else %do;
    data _null_;
      length limit $10;
      limit=put((&HAZSCALE/10),10.5);
      call symput('LIMIT',limit);
    run;
    %let _VAXIS=vaxis=0 to &hazscale by &limit;
  %end;
  PROC PLOT DATA=PREDICT;
    %IF &PLOTS & &repeat=0 & &weighted=0 %THEN %DO;
       PLOT _SURVIV*&INTERVAL=PLTPAR CUM_SURV*&INTERVAL=PLTCHR
            (_CLLSURV _CLUSURV)*&INTERVAL='.'
            (CL_LOWER CL_UPPER)*&INTERVAL=PLTCL
            /OVERLAY VAXIS=0 TO 1 BY 0.1;
      %IF &PDETAIL %THEN %DO;
       PLOT D_SURVIV*&INTERVAL=PLTCHR;
      %END;
    %END;
    %IF &PLOTC %THEN %DO;
       PLOT CUM_HAZ*&INTERVAL=PLTCHR _CUMHAZ*&INTERVAL=PLTPAR
            (_CLLCHZ _CLUCHZ)*&INTERVAL='.'
            (CLL_CHZ CLU_CHZ)*&INTERVAL=PLTCL/OVERLAY;
      %IF &PDETAIL %THEN %DO;
       PLOT D_CUMHAZ*&INTERVAL=PLTCHR;
       PLOT LN_CHAZ_*&INTERVAL=PLTCHR LN_CUMHZ*&INTERVAL=PLTPAR
            (_LNCLLCH _LNCLUCH)*&INTERVAL='.'
            (LN_CLLCH LN_CLUCH)*&INTERVAL=PLTCL/OVERLAY;
       PLOT CUM_HAZ*LN_TIME=PLTCHR _CUMHAZ*LN_TIME=PLTPAR
            (_CLLCHZ _CLUCHZ)*LN_TIME='.'
            (CLL_CHZ CLU_CHZ)*LN_TIME=PLTCL/OVERLAY;
       PLOT LN_CHAZ_*LN_TIME=PLTCHR LN_CUMHZ*LN_TIME=PLTPAR
            (_LNCLLCH _LNCLUCH)*LN_TIME='.'
            (LN_CLLCH LN_CLUCH)*LN_TIME=PLTCL/OVERLAY;
      %END;
    %END;
    %IF &PLOTH %THEN %DO;
       PLOT _HAZARD*&INTERVAL=PLTPAR (_CLLHAZ _CLUHAZ)*&INTERVAL='.'
            /OVERLAY &_VAXIS;
       PLOT _LNHAZ*&INTERVAL=PLTPAR (_LNCLLHZ _LNCLUHZ)*&INTERVAL='.'
            /OVERLAY &_VAXIS;
      %IF &PDETAIL %THEN %DO;
       PLOT _HAZARD*LN_TIME=PLTPAR (_CLLHAZ _CLUHAZ)*LN_TIME='.'
            /OVERLAY &_VAXIS;
       PLOT _LNHAZ*LN_TIME=PLTPAR (_LNCLLHZ _LNCLUHZ)*LN_TIME='.'
            /OVERLAY;
      %END;
    %END;
    %IF &PLOTI %THEN %DO;
       PLOT _EARLYH*&INTERVAL='E' _CONSTH*&INTERVAL='C'
            _LATEH*&INTERVAL='L'/OVERLAY &_VAXIS;
       PLOT _EARLYR*&INTERVAL='E' _CONSTR*&INTERVAL='C'
            _LATER*&INTERVAL='L'/OVERLAY VAXIS=0 TO 100 BY 10;
      %IF &P_EARLY %THEN %DO;
       PLOT _EARLYC*&INTERVAL=PLTPAR R_EARLYC*&INTERVAL=PLTCHR/OVERLAY;
        %IF &PDETAIL %THEN %DO;
       PLOT LN_EARLC*LN_TIME=PLTPAR LN_EC*LN_TIME=PLTCHR/OVERLAY;
        %END;
      %END;
      %IF &P_CONST %THEN %DO;
       PLOT _CONSTC*&INTERVAL=PLTPAR R_CONSTC*&INTERVAL=PLTCHR/OVERLAY;
      %END;
      %IF &P_LATE %THEN %DO;
       PLOT _LATEC*&INTERVAL=PLTPAR R_LATEC*&INTERVAL=PLTCHR/OVERLAY;
        %IF &PDETAIL %THEN %DO;
       PLOT LN_LATEC*LN_TIME=PLTPAR LN_LC*LN_TIME=PLTCHR/OVERLAY;
       PLOT LN_LATEC*&INTERVAL=PLTPAR LN_LC*&INTERVAL=PLTCHR/OVERLAY;
        %END;
      %END;
    %END;
* Plot goodness of fit for events only                                         ;
  %IF &PLOTG & &repeat=0 and &weighted=0 %THEN %DO;
    DATA PREDICT; SET PREDICT;
    LABEL
      T_CUMHAZ = 'Total expected events (cum. hazard)'
      TECUMHAZ = 'Calculated early hazard phase events'
      TCCUMHAZ = 'Calculated constant hazard phase events'
      TLCUMHAZ = 'Calculated late hazard phase events'
      NEHAZ    = 'Alternative calc. early phase events'
      NCHAZ    = 'Alternative calc. constant phase events'
      NLHAZ    = 'Alternative calc. late phase events'
      TDEAD    = 'Number of Observed Events'
    ;
*   proc print data=predict;
    PROC MEANS SUM;
         VAR &EVENT T_CUMHAZ TECUMHAZ TCCUMHAZ TLCUMHAZ NEHAZ NCHAZ NLHAZ;
    DATA PREDICT; SET PREDICT; IF &EVENT>0;
    L_INT=LOG(&INTERVAL);
    LABEL
      SUM_CHF='Expected Events From Cum. Haz. Function'
      D_DEAD ='Expected Minus Observed No. of Events'
      L_INT  ='Log(interval)';
    ;
    PROC PLOT;
         PLOT SUM_CHF*TDEAD='P' TDEAD*TDEAD='.'/OVERLAY;
         PLOT D_DEAD*&INTERVAL/VREF=0;
         PLOT D_DEAD*L_INT/VREF=0;
  %END;
* proc print data=predict;
  %IF &DSDELETE %THEN %DO; PROC DELETE DATA=PREDICT; %END;
*******************************************************************************;
  %MEND HAZPLOT;
*******************************************************************************;
