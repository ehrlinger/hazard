*______________________________________________________________________________;
*                                                                              ;
* !HZEXAMPLES/hz.tm123.OMC                                                     ;
*                                                                              ;
* Hazard function for permanent morbidity from thromboembolic events.          ;
* Morbidity index:  0=no permanent morbidity                                   ;
*                   1=mild permanent morbidity                                 ;
*                   2=moderate permanent morbidity                             ;
*                   3=severe permanent morbidity                               ;
*                   4=death                                                    ;
*______________________________________________________________________________;
*                                                                              ;
  OPTIONS PAGESIZE=100 LINESIZE=132;
  TITLE1 "Primary Isolated Mitral Commissurotomy";
  TITLE2 "Permanent Morbidity Analysis from Repeated Thromboembolic Events";
*******************************************************************************;
* Bring in data                                                                ;
  FILENAME INOMC ('!HZEXAMPLES/data/omc');
  DATA PRIMISOL; INFILE INOMC MISSOVER;
  INPUT STUDY $ 1-10 TE1 12 TE2 14 TE3 16 TE1DJUL 18-25 TE2DJUL 27-33 TE3DJUL
        35-41 AVP_RP1 $ 43 AVP_RP2 $ 45 AVP_RP3 $ 47 MVP_RP1 $ 49 MVP_RP2 $ 51
        MVP_RP3 $ 53 TVP_RP1 $ 55 TVP_RP2 $ 57 TVP_RP3 $ 59 RP1DJUL 61-67
        RP2DJUL 69-75 RP3DJUL 77-83 REOP1DAT $ 85-92 REOP2DAT $ 94-101 REOP3DAT
        $ 103-110 INT_DEAD 111-120 5 OPDJUL 122-128 DEAD 130 FUPDJUL 132-138
        TE1GRADE 140 TE2GRADE 142 TE3GRADE 144;

  LABEL
        STUDY    = 'Study number'
        TE1      = 'First TE'
        TE2      = 'Second TE'
        TE3      = 'Third TE'
        TE1DJUL  = 'First TE Julian date'
        TE2DJUL  = 'Second TE Julian date'
        TE3DJUL  = 'Third TE Julian date'
        AVP_RP1  = 'Aortc Replacement'
        AVP_RP2  = 'Aortc Replacement'
        AVP_RP3  = 'Aortc Replacement'
        MVP_RP1  = 'Mitral Replacement'
        MVP_RP2  = 'Mitral Replacement'
        MVP_RP3  = 'Mitral Replacement'
        TVP_RP1  = 'Tricuspid Replacement'
        TVP_RP2  = 'Tricuspid Replacement'
        TVP_RP3  = 'Tricuspid Replacement'
        RP1DJUL  = 'First reop Julian date'
        RP2DJUL  = 'Second reop Julian date'
        RP3DJUL  = 'Thrid reop Julian date'
        DEAD     = 'Death'
        INT_DEAD = 'Followup interval (months) after repair'
        FUPDJUL  = 'Followup Julian date'
  ;
*******************************************************************************;
* Prepare a separate observation for each intra-embolic interval               ;
  IF TE1=1 AND TE1DJUL=. THEN DELETE;
  IF TE1=. THEN TE1=0; IF TE2=. THEN TE2=0; IF TE3=. THEN TE3=0;
  IF AVP_RP3='R' OR MVP_RP3='R' OR TVP_RP3='R' THEN DO;
    REPL=1; REPLDJUL=RP3DJUL; REPLDATE=REOP3DAT; END;
  IF AVP_RP2='R' OR MVP_RP2='R' OR TVP_RP2='R' THEN DO;
    REPL=1; REPLDJUL=RP2DJUL; REPLDATE=REOP2DAT; END;
  IF AVP_RP1='R' OR MVP_RP1='R' OR TVP_RP1='R' THEN DO;
    REPL=1; REPLDJUL=RP1DJUL; REPLDATE=REOP1DAT; END;
  IF REPL=. THEN REPL=0;
  IF REPL=0 THEN REPLDJUL=FUPDJUL;

  IF REPL=1 THEN DO ;
    IF TE1=1 THEN DO; IF TE1DJUL GE REPLDJUL THEN TE1=0; END;
    IF TE2=1 THEN DO; IF TE2DJUL GE REPLDJUL THEN TE2=0; END;
    IF TE3=1 THEN DO; IF TE3DJUL GE REPLDJUL THEN TE3=0; END;
  END;

  INT_REPL=INT_DEAD;
  IF REPL=1 THEN INT_REPL=(REPLDJUL-OPDJUL)*12/365.2425;

  CENSORED=1; INT_TE=INT_REPL; TE=0; STARTTME=0; MORBID=0;
  IF TE1=1 THEN STARTTME=12*(TE1DJUL-OPDJUL)/365.2425;
      /* 2 patients had embolic events in the hospital */
    IF STUDY='039' THEN STARTTME=(3/24)*12/365.2425;
    IF STUDY='096' THEN STARTTME=(2/24)*12/365.2425;
      /* 1 patient died on same day as te */
 /*    IF STUDY='173' THEN DO; STARTTME=0; TE=1; END; */
* The previous statement is an effort to remove a zero censored interval, but
* it is placed incorrectly and produces and error (TE is remodulated at wrong
* level). ;
  IF TE2=1 THEN STARTTME=12*(TE2DJUL-OPDJUL)/365.2425;
  IF TE3=1 THEN STARTTME=12*(TE3DJUL-OPDJUL)/365.2425;
  OUTPUT;

  IF TE1=1 /* AND STUDY NE '173' */ THEN DO;
* See previous comment about study # 173;
    STARTTME=0; INT_TE=12*(TE1DJUL-OPDJUL)/365.2425;
    /* 2 patients had embolic events in the hospital */
    IF STUDY='039' THEN INT_TE=(3/24)*12/365.2425;
    IF STUDY='096' THEN INT_TE=(2/24)*12/365.2425;
    CENSORED=0; TE=1;
    MORBID=TE1GRADE; IF TE1GRADE=. THEN MORBID=1.70;
    OUTPUT;
  END;

  IF TE2=1 THEN DO;
    STARTTME=12*(TE1DJUL-OPDJUL)/365.2425;
    IF STUDY='039' THEN STARTTME=(3/24)*12/365.2425;
    IF STUDY='096' THEN STARTTME=(2/24)*12/365.2425;
    INT_TE=12*(TE2DJUL-OPDJUL)/365.2425;
    CENSORED=0; TE=1;
    MORBID=TE2GRADE; IF TE2GRADE=. THEN MORBID=2.17;
    OUTPUT;
  END;

  IF TE3=1 THEN DO;
    STARTTME=12*(TE2DJUL-OPDJUL)/365.2425;
    INT_TE=12*(TE3DJUL-OPDJUL)/365.2425;
    CENSORED=0; TE=1;
    MORBID=TE3GRADE; IF TE3GRADE=. THEN MORBID=4.00;
    OUTPUT;
  END;
  KEEP STUDY TE1 TE2 TE3 INT_TE STARTTME CENSORED TE MORBID;
*******************************************************************************;
* Estimate parameters of the distribution of the event                         ;
  %HAZARD(
  PROC HAZARD DATA=PRIMISOL CONSERVE P OUTHAZ=OUTEST
       STEEPEST QUASI CONDITION=14;
       LCENSOR STARTTME;
       TIME INT_TE;
       RCENSOR CENSORED;
       WEIGHT MORBID;
       EVENT TE;
       PARMS MUE=0.018568 THALF=0.0109 NU=1 FIXNU M=1.0E-6 FIXM
             MUL=0.0002495 TAU=1 FIXTAU ALPHA=1 FIXALPHA GAMMA=1 FIXGAMMA
             ETA=1.4112   WEIBULL;
*      PARMS MUE=0.01777599 THALF=0.05927402 NU=1 M=1
             MUC=0.0009970977;
  );
*******************************************************************************;
* Bring in Nelson life table method for repeating, morbid events               ;
  FILENAME NELSONL ('!MACROS/nelsonl.sas');
  OPTIONS NOSOURCE; %INC NELSONL; OPTIONS SOURCE;
*______________________________________________________________________________;
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:            DEFAULT:                        ;
*                                                                              ;
*   INTERVAL = Time variable                     INTERVAL                      ;
*   EVENT    = Event (0/1) indicator             EVENT                         ;
*   CENSORED = Censorer (0/1) indicator          CENSORED                      ;
*   STRATIFY = Stratification variable           'ALL'                         ;
*   CLEVEL   = Confidence coefficient            0.68268948 (1 sd)             ;
*   IN       = Input data set name                                             ;
*   OUT      = Output data set name              EVENT                         ;
*   CUMDIST  = Cumulative hazard function        CUMDIST                       ;
*   WEIGHT   = Severity level of event           1                             ;
*                                                                              ;
*______________________________________________________________________________;
*                                                                              ;
  %NELSONL(INTERVAL=INT_TE, EVENT=TE, CENSORED=CENSORED, IN=PRIMISOL,
           OUT=NELOUT, CUMDIST=CUMDIST, WEIGHT=MORBID);
  PROC PRINT; VAR INT_TE TE CENSORED WEIGHT NRISK NUMRISK CUMDIST
                  SE_CEF CLL_CEF CLU_CEF;
*******************************************************************************;
* Calculate parametric estimates of survivorship and hazard functions          ;
  %HAZPRED(
  PROC HAZPRED DATA=NELOUT INHAZ=OUTEST OUT=PREDICT; TIME INT_TE;);
  DATA PREDICT; SET PREDICT;
  _CUMHAZ=-LOG(_SURVIV); _CLLCUMH=-LOG(_CLUSURV); _CLUCUMH=-LOG(_CLLSURV);
  D_CUMHAZ=CUMDIST-_CUMHAZ;
*******************************************************************************;
* Plot various survival functions                                              ;
  PROC PLOT;
       PLOT CUMDIST*INT_TE='*' CLL_CEF*INT_TE='-' CLU_CEF*INT_TE='-'
            _CUMHAZ*INT_TE='P' _CLLCUMH*INT_TE='.' _CLUCUMH*INT_TE='.'
            /OVERLAY;
       PLOT D_CUMHAZ*INT_TE='*';
*******************************************************************************;
RUN;

