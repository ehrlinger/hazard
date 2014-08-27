*______________________________________________________________________________;
*                                                                              ;
* !HZEXAMPLES/hp.death.COMPARISON                                              ;
*______________________________________________________________________________;
*                                                                              ;
* This is an example of comparison of different treatment strategies based     ;
* on patient-specific estimates.                                               ;
*                                                                              ;
* Here we explore simple survival differences.                                 ;
*                                                                              ;
* In the macros directory, macros are included that also do hazard ratio       ;
* comparisons and differences in lifetime between strategies.                  ;
*______________________________________________________________________________;
*                                                                              ;
* Comparison of primary CABG (KUL) vs primary PTCA (MHI) vs VA medical for:
* Left main disease and 1, 2, and 3 system disease
*
* Other factors: Age 65, male, no proximal LAD (GE 50%), distal RCA (70%),
*                or posterolateral 95% disease, angina class 2 (severe, stable,
*                chronic angina), 1 previous MI, no acute evolving infarct,
*                hemodynamically stable, EF=55%, no LVH, no ischemic mitral
*                incompetence, no peripheral vascular disease, no small
*                coronaries, no hypertension, no diabetes, normal pulmonary
*                function, smoker, triglycerides 175, HDL of 40
*
*   PTCA-specific: elective PTCA, 11 years after start of experience,
*                PTCA of left main for left main disease, no dilation of
*                chronic occlusion.
*
*   CABG-specific: not Surgeon #1, complete revascularization, no sequentials,
*                IMA used, no endarterectomy,
*
*   Medical-specific: weight/height ratio = 0.43
*______________________________________________________________________________;
*                                                                              ;
  OPTIONS PAGESIZE=100 LINESIZE=132;
  TITLE1 "Percutaneous Transluminal Coronary Angioplasty";
  TITLE2 "MHI; June 1980 - July 1991";
  TITLE3 "Primary PTCA Without Previous CABG";
  TITLE4 "Multivariable Analysis of Death";
  TITLE5 "Patient, Procedure and Experience Risk Factors";
  TITLE6 "Comparison of CABG (KUL) and PTCA";
  TITLE7 "Left Main Stenosis, 1, 2, and 3 system disease";
*______________________________________________________________________________;
*                                                                              ;
*    M U L T I V A R I A B L E   M O D E L   F O R   D E A T H   A F T E R     ;
*                        P T C A   (M H I)                                     ;
*                                                                              ;
* Early Hazard Phase:                                                          ;
*   FEMALE IN_AGE LN_CREAT ANGMMS2 AMI_TM URGENT EFC2 DVES _PLAD50 _RPLAT95    ;
*   _MRCA50 RCA_DIST OCCLNDIL IMP_BLUE PTCA_LM LN_YRS                          ;
* Constant Hazard Phase:                                                       ;
*   AGE2 IN_AGE LN_CREAT IN_CREAT HISTSMOK DIABETRX CAC0NRMI ANGREST EFC2 DVES ;
*   PTCA_LM PTOTDLAD                                                           ;
* Late Hazard Phase:                                                           ;
*   FEMALE IN_AGE HYPERTEN DIABETRX LN_CREAT AMI_SEND _PLADOC                  ;
*______________________________________________________________________________;
*                                                                              ;
*    M U L T I V A R I A B L E   M O D E L   F O R   D E A T H   A F T E R     ;
*                        C A B G   (K U L)                                     ;
*                                                                              ;
* Early Hazard Phase:                                                          ;
*   AGE LMAIN_90 EF LVH ANGCLS HEMO2CLS VASCPATH MM1_DIST DIABETES             ;
*   EXP1SECP OPYRS SURGEON1 ENDART_N SEQUENTL                                  ;
* Constant Hazard Phase:                                                       ;
*   EF VASCPATH MITRAL_I UNST_ANG ANG_NO HDL INCOMPL                           ;
* Late Hazard Phase:                                                           ;
*   AGE EF VASCPATH MM1_DIST DIABETES EXP1SECP VESSDIS LTRIGL IMA_NO           ;
*   LN_AGE                                                                     ;
*______________________________________________________________________________;
*                                                                              ;
*    M U L T I V A R I A B L E   M O D E L   F O R   D E A T H   A F T E R     ;
*       I N I T I A L   M E D I C A L   T H E R A P Y   (V A S T U D Y)        ;
*                                                                              ;
* Early Hazard Phase:                                                          ;
*   NYHAB LVNLM_50 LV_LM_50                                                    ;
* Constant Hazard Phase:                                                       ;
*   IN_WTHT LM_50 LV_NVD_E NUMPRVMI ANG_SV HISTHYP                             ;
* Late Hazard Phase:                                                           ;
*   WT_HT                                                                      ;
*______________________________________________________________________________;
*                                                                              ;
* Set values for standard model incremental risk factors variables.            ;
  DATA DESIGN;
* Demographic                                                                  ;
  FEMALE=0;      /* Sex                                               */
  AGE=65;        /* Age (years) at time zero                          */
  WT_HT=0.43;    /* Weight-height ratio (kg/cm)                       */
* Coronary disease                                                             ;
  IMP_BLUE=0     /* Dummy imputation variable--set to zero            */
* VESSDIS=2;     /* Number of systems diseased (1-3): 50% criterion   */
* LMCA=0;        /* Percent stenosis of left main coronary artery     */
  _PLAD50=0;     /* Proximal LAD disease (50% criterion)              */
  _PLADOC=0;     /* Occludid proximal LAD                             */
  _MRCA50=0;     /* Mid-RCA disease (50% criterion)                   */
  RCA_DIST=0;    /* % Distal RCA disease                              */
  _RPLAT95=0;    /* Right posterolateral disease (95% criterion)      */
* Angina                                                                       ;
  ANGCLS=2;      /* Angina grade according to KUL                     */
                 /*   0 = No anginal symptoms (11% KUL)               */
                 /*   1 = Minimal anginal symptoms (9% KUL)           */
                 /*       (probably grade I-II of CCS)                */
                 /*   2 = Moderate angina (at normal activities),     */
                 /*       the usual "chronic stable angina" (41% LUL) */
                 /*       (probably grade III of CCS)                 */
                 /*   3 = Severe anginal symptoms at night/rest       */
                 /*       (21% KUL; probably grade IV of CCS)         */
                 /*   4 = Unstable angina or acute myocardial infarct */
                 /*       (18% KUL)                                   */
  UNST_MED=0;    /* On medications for unstable angina at operation   */
* Myocardial infarction                                                        ;
  NUMPRVMI=1;    /* Number of previous myocardial infarcts            */
  ACUTE_MI=0;    /* Acute myocardial infarction (0,1) (within 24 hrs  */
                 /*       of revascularizaton in the KUL experience)  */
  AMI_TM=0;      /* Acute evolving transmural myocardial infarction   */
  HEMO_UNS=0;    /* Hemodynamically unstable (this includes acute     */
                 /*       cardiac failure with unstable angina, the   */
                 /*       acute (hours) phase of myocardial infarct   */
                 /*       and cardiogenic shock)                      */
  SHOCK=0;       /* Cardiogenic shock                                 */
  URGENT=0;      /* Urgent PTCA                                       */
* LV function                                                                  ;
  EF=0.55;       /* Ejection fraction (0 to 1)                        */
  LVH=0;         /* LV Hypertrophy (0,1)                              */
  MITRAL_I=0;    /* Important, but untreated, ischemic mitral incomp. */
* Aggressiveness of atherosclerotic process                                    ;
  VASCPATH=0;    /* Peripheral vascular disease (0,1)                 */
  MM1_DIST=0;    /* Normalized # of distals 1 mm or less in diameter  */
* Coexisting conditions                                                        ;
  HYPERTEN=0;    /* History of hypertension                           */
  DIABETES=0;    /* Diabetic patient (in KUL this is insulin treated) */
  DIABETRX=0;    /* Diabetic patient (insulin or  is insulin treated) */
  EXP1SECP=100;  /* One second expired volume (% of normal)           */
  CREAT=1.1;     /* Serum creatinine                                  */
  HISTSMOK=1;    /* History of smoking                                */
* Hyperlipidemia                                                               ;
  TRIGLYC=175;   /* Triglyceride level                                */
  HDL=40;        /* High density lipoprotein level                    */
* Procedural factors: CABG                                                     ;
  INCOMPL=0;     /* Incomplete revascularization (0,1)                */
  ENDART_N=0;    /* Number of endarterectomized vessels               */
  SEQUENTL=1;    /* Ratio of distals to conduits                      */
  IMA=1;         /* Use of IMA                                        */
* Procedural factors: PTCA                                                     ;
* PTCA_LM=1;     /* PTCA of left main stenosis                        */
  PTOTDLAD=0;    /* PTCA of occluded distal LAD                       */
  OCCLNDIL=0;    /* Chronic occlusion not dilated                     */
* Experience factors                                                           ;
  PTCAYRS=11;    /* Date of operation (years from 6/1/80)             */
  SURGEON1=0;    /* High-risk surgeon                                 */
* MONTHS=6;      /* Time of observation (months)                      */
*******************************************************************************;
* Generate isobars for various variables                                       ;
  PPLOT=1; VESSDIS=2; LMCA=90; PTCA_LM=1; OUTPUT;
  PPLOT=2; VESSDIS=1; LMCA=0; PTCA_LM=0; OUTPUT;
  PPLOT=3; VESSDIS=2; LMCA=0; PTCA_LM=0; OUTPUT;
  PPLOT=4; VESSDIS=3; LMCA=0; PTCA_LM=0; OUTPUT;
  DATA DESIGN; SET DESIGN;
*******************************************************************************;
* Transformations of variables into those needed for the various               ;
* predictions:                                                                 ;
  LN_AGE=LOG(AGE);
  IN_AGE=50/AGE;
  AGE2=(AGE/50)**2;
  IN_WTHT=1/WT_HT;

  IMP_LVC=0; IF EF LE 0.5 THEN IMP_LVC=1;

  LMAIN_90=(LMCA GE 90);
  LM_50=(LMCA GE 50);
  DVES=VESSDIS;
  NVD_50=VESSDIS;
  NVD_EXP=EXP(NVD_50);
  IF LM_50 THEN DO;
    NVD_50=0;
    NVD_EXP=0;
  END;
  LV_NVD_E=IMP_LVC*NVD_EXP;
  LV_LM_50=IMP_LVC*LM_50;
  LVNLM_50=(1-IMP_LVC)*LM_50;

  ACUTEMI=ACUTE_MI;
  AMI_SEND=0; IF ACUTEMI=1 AND AMI_TM=0 THEN AMI_SEND=1;

  HEMO2CLS=HEMO_UNS; IF ACUTE_MI OR UNST_MED THEN HEMO2CLS=1;
    IF SHOCK THEN HEMO2CLS=4;
  IF ANGCLS=0 THEN ANG_NO=1; ELSE ANG_NO=0;
  IF ANGCLS=3 THEN DO; NITE_ANG=1; ANGREST=1; END;
    ELSE DO; NITE_ANG=0; ANGREST=0; END;
  IF ACUTE_MI=0 AND ANGCLS=4 THEN UNST_ANG=1; ELSE UNST_ANG=0;
  ANGMMS=ANGCLS; IF ACUTEMI=1 THEN ANGMMS=0;
  NYHAB=ANGCLS; IF ANGCLS>1 THEN NYHAB=ANGCLS+1;
  ANG_SV=ANGCLS > 2;
  CAC0NRMI=0; IF ANGCLS=0 AND ACUTEMI=0 AND NUMPRVMI=0 THEN CAC0NRMI=1;

  LTRIGL=LOG(TRIGLYC);
  LN_CREAT=LOG(CREAT);
  IN_CREAT=1/CREAT;
  ANGMMS2=ANGMMS*ANGMMS;
  OPYRS=PTCAYRS + 9.4156622;
  LN_YRS=LOG(PTCAYRS);
  EFC=(1-EF)/1; EFC2=EFC*EFC;

  HISTHYP=HYPERTEN;

  IMA_NO=(IMA=0);
  PROC PRINT DATA=DESIGN;
*______________________________________________________________________________;
* Generate horizontal axis                                                     ;
  DATA PREDICT; SET DESIGN; DIGITAL=0;
  MAX=LOG(120); MIN=-5; INC=(MAX-MIN)/999.9;
                       *INC=(MAX-MIN)/9.9;
  DO LN_TIME=MIN TO MAX BY INC, MAX;
    MONTHS=EXP(LN_TIME);
    YEARS=MONTHS/12;
    OUTPUT;
  END;
  DROP LN_TIME MIN MAX INC;
* and for digital nomogram                                                     ;
  DATA DIGITAL; SET DESIGN; DIGITAL=1;
  DO MONTHS=30*12/365.2425, 12, 60, 120, 180, 240;
    YEARS=MONTHS/12;
    OUTPUT;
  END;
* then put them together                                                       ;
  DATA PREDICT; SET PREDICT DIGITAL;
*______________________________________________________________________________;
*                                                                              ;
*    M U L T I V A R I A B L E   M O D E L   F O R   D E A T H   A F T E R     ;
*                        P T C A   (M H I)                                     ;
*                                                                              ;
*                      P R E D I C T I O N S                                   ;
*______________________________________________________________________________;
*                                                                              ;
  LIBNAME PTCAMHI ('!HZEXAMPLES/sasest');
  %HAZPRED(
  PROC HAZPRED DATA=PREDICT INHAZ=PTCAMHI.HMPDTHE3 OUT=PREDICT;
       TIME MONTHS;
  );
  TITLE6 'PTCA Equation';
* Rename variables for the comparison                                          ;
  DATA PREDICT; SET PREDICT;
  _SURVIVP=_SURVIV; CLLPSURV=_CLLSURV; CLUPSURV=_CLUSURV;
  DROP _SURVIV _CLLSURV _CLUSURV _EARLYS _CONSTS _LATES _HAZARD _CLLHAZ
       _CLUHAZ _EARLYH _CONSTH _LATEH;
*______________________________________________________________________________;
*                                                                              ;
*    M U L T I V A R I A B L E   M O D E L   F O R   D E A T H   A F T E R     ;
*                        C A B G   (K U L)                                     ;
*                                                                              ;
*                      P R E D I C T I O N S                                   ;
*______________________________________________________________________________;
*                                                                              ;
  LIBNAME CABGKUL ('!HZEXAMPLES/sasest');
  %HAZPRED(
  PROC HAZPRED DATA=PREDICT INHAZ=CABGKUL.HMDEADP OUT=PREDICT;
       TIME MONTHS;
  );
  TITLE6 'KUL Equation';
* Rename variables for comparison                                              ;
  DATA PREDICT; SET PREDICT;
  _SURVIVC=_SURVIV; CLLCSURV=_CLLSURV; CLUCSURV=_CLUSURV;
  DROP _SURVIV _CLLSURV _CLUSURV _EARLYS _CONSTS _LATES _HAZARD _CLLHAZ
       _CLUHAZ _EARLYH _CONSTH _LATEH;
*______________________________________________________________________________;
*                                                                              ;
*    M U L T I V A R I A B L E   M O D E L   F O R   D E A T H   A F T E R     ;
*       I N I T I A L   M E D I C A L   T H E R A P Y   (V A S T U D Y)        ;
*                                                                              ;
*                      P R E D I C T I O N S                                   ;
*______________________________________________________________________________;
*                                                                              ;
  LIBNAME VAMEDCBG ('!HZEXAMPLES/sasest/ihd');
  %HAZPRED(
  PROC HAZPRED DATA=PREDICT INHAZ=VAMEDCBG.HMDEATH OUT=PREDICT;
       TIME MONTHS;
  );
  TITLE6 'VA Medical Equation';
* Rename variables for comparison                                              ;
  DATA PREDICT; SET PREDICT;
  _SURVIVM=_SURVIV; CLLMSURV=_CLLSURV; CLUMSURV=_CLUSURV;
  DROP _SURVIV _CLLSURV _CLUSURV _EARLYS _CONSTS _LATES _HAZARD _CLLHAZ
       _CLUHAZ _EARLYH _CONSTH _LATEH;
*______________________________________________________________________________;
*                                                                              ;
* Generate survivorship differences                                            ;
* Bring in analysis macro:                                                     ;
  FILENAME SURDIFP ('!MACROS/surdifp.sas');
  OPTIONS NOSOURCE; %INC SURDIFP; OPTIONS SOURCE;
* PTCA vs. CABG                                                                ;
  %SURDIFP(IN=PREDICT, PVALUES=1, CUMSURVS=_SURVIVC, CUMSURVM=_SURVIVP,
           CLLSSURV=CLLCSURV, CLUSSURV=CLUCSURV, CLLMSURV=CLLPSURV,
           CLUMSURV=CLUPSURV, CUMSURVD=CUMSURPC, CLLDSURV=CLLDSUPC,
           CLUDSURV=CLUDSUPC, CUMSURVP=CUMSVPPC);
* CABG vs. VA Medical                                                          ;
  %SURDIFP(IN=PREDICT, PVALUES=1, CUMSURVS=_SURVIVC, CUMSURVM=_SURVIVM,
           CLLSSURV=CLLCSURV, CLUSSURV=CLUCSURV, CLLMSURV=CLLMSURV,
           CLUMSURV=CLUMSURV, CUMSURVD=CUMSURCM, CLLDSURV=CLLDSUCM,
           CLUDSURV=CLUDSUCM, CUMSURVP=CUMSVPCM);
* PTCA vs. VA Medical                                                          ;
  %SURDIFP(IN=PREDICT, PVALUES=1, CUMSURVS=_SURVIVP, CUMSURVM=_SURVIVM,
           CLLSSURV=CLLPSURV, CLUSSURV=CLUPSURV, CLLMSURV=CLLMSURV,
           CLUMSURV=CLUMSURV, CUMSURVD=CUMSURPM, CLLDSURV=CLLDSUPM,
           CLUDSURV=CLUDSUPM, CUMSURVP=CUMSVPPM);
*******************************************************************************;
* Printer plot                                                                 ;
* Survivorship Functions                                                       ;
  PROC SORT DATA=PREDICT; BY PPLOT;
  PROC PLOT DATA=PREDICT; BY PPLOT;
       PLOT _SURVIVP*YEARS='P' CLLPSURV*YEARS='.' CLUPSURV*YEARS='.'
            _SURVIVC*YEARS='C' CLLCSURV*YEARS='-' CLUCSURV*YEARS='-'
            _SURVIVM*YEARS='M' CLLMSURV*YEARS='+' CLUMSURV*YEARS='+'
            /OVERLAY VAXIS=0 TO 1 BY 0.1;
* Comparative Benefit:  Survivorship Difference                                ;
  PROC PLOT DATA=PREDICT; BY PPLOT;
       PLOT CUMSURPC*YEARS='C' CLLDSUPC*YEARS='.' CLUDSUPC*YEARS='.'
            /OVERLAY VAXIS=-0.5 to 0.5 by 0.1;
       PLOT CUMSURCM*YEARS='M' CLLDSUCM*YEARS='.' CLUDSUCM*YEARS='.'
            /OVERLAY VAXIS=-0.5 to 0.5 by 0.1;
       PLOT CUMSURPM*YEARS='P' CLLDSUPM*YEARS='.' CLUDSUPM*YEARS='.'
            /OVERLAY VAXIS=-0.5 to 0.5 by 0.1;
* Comparative Benefit:  Survivorship Difference P-value                        ;
  PROC PLOT DATA=PREDICT; BY PPLOT;
       PLOT CUMSVPPC*YEARS='C' CUMSVPCM*YEARS='M' CUMSVPPM*YEARS='P'
            /OVERLAY;
*******************************************************************************;
* Digital nomogram                                                             ;
  DATA DIGITAL; SET PREDICT; IF DIGITAL=1;
  PROC SORT DATA=DIGITAL; BY PPLOT;
  PROC PRINT D DATA=DIGITAL; VAR MONTHS YEARS _SURVIVC _SURVIVP _SURVIVM
       CUMSURPC CUMSVPPC CUMSURCM CUMSVPCM CUMSURPM CUMSVPPM; BY PPLOT;
*******************************************************************************;
RUN;
