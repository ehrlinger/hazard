*______________________________________________________________________________;
*                                                                              ;
* !HZEXAMPLES/hm.death.patient.sas
*
* Primary valve replacement
*
* Multivariable analysis of death
*
* Patient variables only
*______________________________________________________________________________;
*                                                                              ;
  OPTIONS PAGESIZE=80 LINESIZE=132;
  TITLE1 'Primary Valve Operations';
  TITLE2 '(UAB; 1975 - July 1979; n=1533)';
  TITLE3 'Multivariable Analysis of Death (Original Equation)';
*******************************************************************************;
* Bring in data                                                                ;
  FILENAME INVALV ('!HZEXAMPLES/data/valves');
  DATA VALVES; INFILE INVALV MISSOVER;
  INPUT AGE_COP 1-10 5 NYHA 12 MITRAL 14 DOUBLE 16 AO_PINC 18 BLACK 20
      I_PATH 22 NVE 24 MECHVALV 26 MALE 28 INT_DEAD 30-39 5 DEAD 41 INT_PVE
      43-52 5 PVE 54 BIO 56 INT_RDG 58-67 5 REOP_DG 69 INT_REOP 71-80 5
      REOP 82;
  LABEL
        AGE_COP  = 'Age (years) at current operation'
        NYHA     = 'Augmented NYHA functional class'
        MITRAL   = 'Mitral valve replacement only'
        DOUBLE   = 'Multiple valve replacement'
        AO_PINC  = 'Pure aortic incompetence'
        BLACK    = 'Black race'
        I_PATH   = 'Ischemic mitral pathology'
        NVE      = 'Native valve endocarditis'
        MECHVALV = 'Mechanic prosthesis inserted'
        MALE     = 'Male patient'
        MALE     = 'Male patient'
        INT_DEAD = 'Followup Interval (months) After Repair'
        DEAD     = 'Death'
        INT_PVE  = 'Interval (months) to first PVE'
        PVE      = 'Prosthetic Valve Endocarditis (event)'
        BIO      = 'Bioprosthesis inserted'
        INT_RDG  = 'Interval (months) to Bio degeneration'
        REOP_DG  = 'Reoperation for Bio degeneration'
        INT_REOP = 'Interval (months) to and reoperation'
        REOP     = 'First Reoperation'
  ;
*******************************************************************************;
* Standardize for missing values                                               ;
  PROC STANDARD DATA=VALVES OUT=VALVES REPLACE; VAR NYHA;
*******************************************************************************;
* Data transformations                                                         ;
  DATA VALVES; SET VALVES;
  MV_NYHA=MITRAL*NYHA;
  MV_INCNI=(MV_INC=1 AND I_PATH=0);
  run;
*______________________________________________________________________________;
*                                                                              ;
*               F I N A L   M Uls T I V A R I A B L  E   M O D E L             ;
*______________________________________________________________________________;
*                                                                              ;
  %HAZARD(
  PROC HAZARD DATA=VALVES OUTHAZ=OUTEST CONSERVE CONDITION=14 P;
       TIME INT_DEAD; EVENT DEAD;
       PARMS MUE=0.0008955212 THALF=3.894791 NU=1.958339 M=1 FIXM
             MUC=0.001145244;
       EARLY
         AGE_COP=0.02485742, NYHA=0.9373906, MV_NYHA=0.3270209, DOUBLE=1.040199,
         AO_PINC=0.7010022 ;
       CONSTANT
         AGE_COP=0.01439241, BLACK=0.9098664, I_PATH=1.45418;
   );
   libname est ('!HZEXAMPLES');
   data est.hmdeadp (type=haz);
        set outest;
   run;
   libname out xport '!HZEXAMPLES/hmdeadp.xport';
   data out.out; set est.hmdeadp; run;
*******************************************************************************;
