*______________________________________________________________________________;
* <2003-10-24>                                                                 ;            
*______________________________________________________________________________;
*                                                                              ;
* Compare observed and predicted events by means of a decile table from        ;
* a multivariable analysis in the hazard function domain                       ;
*______________________________________________________________________________;
*                                                                              ;
%MACRO DECILES(IN=IN, OUTEST=OUTEST, EVENT=EVENT, INTERVAL=INTERVAL, TIME=12,
    VARS=, GROUPS=10);
*______________________________________________________________________________;
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:                      DEFAULT:              ;
*                                                                              ;
*   IN       = Input data set of values for variables                          ;
*   OUTEST   = PROC HAZARD parameter estimates           OUTEST                ;
*   EVENT    = Event (0/1) indicator                     EVENT                 ;
*   INTERVAL = Interval for the event                    INTERVAL              ;
*   TIME     = Single time interval value for tests      12                    ;
*              (In time units matching INTEVENT)                               ;
*   VARS     = Other variables for means                 none                  ;
*   GROUPS   = Number of groups in table                 10                    ;
*______________________________________________________________________________;
*                                                                              ;
* Check to see if we have a model.                                             ;
    OPTIONS NONOTES;
* %GLOBAL HAZRC; /* This must be in the main program if PROC HAZARD is not */
                 /* called preceding call to DECILES.  Use a LET statement */
                 /* to set the return code to 0.                           */
                     %PUT hazrc &HAZRC;
    %IF &HAZRC=0 %THEN %DO;
* Calculate cumulative hazard at the time of follow-up for each observation    ;
        DATA DECILES; SET &IN;
            %HAZPRED(
        PROC HAZPRED DATA=DECILES INHAZ=&OUTEST OUT=DECILES NOCL NOHAZ NOLOG NONOTES;
            TIME &INTERVAL;);
        OPTIONS NONOTES;
        DATA DECILES; SET DECILES; CUM_HAZ=-LOG(_SURVIV);
            DROP _SURVIV _CLLSURV _CLUSURV _EARLYS _CONSTS _LATES _HAZARD _CLLHAZ
                _CLUHAZ _EARLYH _CONSTH _LATEH;
*******************************************************************************;
* Calculate parametric estimates of survivorship at the point "TIME"           ;
            SET_TIME=&TIME;
            %HAZPRED(
        PROC HAZPRED DATA=DECILES INHAZ=&OUTEST OUT=DECILES NOCL NOHAZ NOLOG NONOTES;
            TIME SET_TIME;);
        OPTIONS NONOTES;
        DATA DECILES;
            SET DECILES(KEEP=&EVENT &INTERVAL SET_TIME CUM_HAZ _SURVIV &VARS);
*******************************************************************************;
* Construct a decile table, with groups of equal number.                       ;
* NOTE:  If the variables in the model do not permit this, the groups will     ;
*        be unequal in number.                                                 ;
        PROC RANK DATA=DECILES OUT=DECILE1 GROUPS=&GROUPS TIES=MEAN;
            VAR _SURVIV; RANKS _DECILE_;
*******************************************************************************;
* Calculate the number of predicted events (by summing individual cumulative   ;
* hazard estimates)                                                            ;
        PROC SUMMARY DATA=DECILE1; CLASS _DECILE_; VAR _SURVIV CUM_HAZ &EVENT &VARS;
            OUTPUT OUT=DECILE2 MEAN= N(CUM_HAZ)=CASES
                SUM(CUM_HAZ &EVENT)=EXPECTED ACTUAL;
*******************************************************************************;
* Bring in goodness of fit macro for formal comparison of predicted and actual ;
* events within each decile grouping.                                          ;
            FILENAME CHISQGF ('!MACROS/chisqgf.sas');
        OPTIONS NOSOURCE; %INC CHISQGF; OPTIONS SOURCE;
* Compare with actual number of events                                         ;
        DATA DECILE2; SET DECILE2;
            %CHISQGF(N=CASES, OBSERVED=ACTUAL, EXPECTED=EXPECTED);
            ABS_DIFF=EXPECTED - ACTUAL;
            PCT_DIFF=CUM_HAZ - &EVENT;
*******************************************************************************;
* Print decile table                                                           ;
            TITLE10 "D E C I L E   A N A L Y S I S   O F   E V E N T   A T   T I M E   &TIME";
        PROC PRINT D DATA=DECILE2; ID _DECILE_;
            VAR CASES EXPECTED CUM_HAZ ACTUAL &EVENT ABS_DIFF PCT_DIFF PVALUE
                _SURVIV &VARS;
* Get rid of files formed                                                      ;
        PROC DELETE DATA=DECILES DECILE1 DECILE2;
            TITLE10 '';
        OPTIONS NOTES;
        %END;
    %MEND DECILES;
