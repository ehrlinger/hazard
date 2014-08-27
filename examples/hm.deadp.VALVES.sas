*______________________________________________________________________________;
*                                                                              ;
* !HZEXAMPLES/hm.deadp.VALVES                                                  ;
*                                                                              ;
* Survival after primary valve replacement                                     ;
*                                                                              ;
* Hazard function for death                                                    ;
*______________________________________________________________________________;
*                                                                              ;
  OPTIONS PAGESIZE=100 LINESIZE=132;
  TITLE1 "Primary Valve Operations";
  TITLE3 "(1975 - July 1979; n=1533)";
  TITLE4 "Multivariable Analysis of Death";
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
*______________________________________________________________________________;
*                                                                              ;
*               M U L T I V A R I A B L  E   A N A L Y S I S                   ;
*______________________________________________________________________________;
*                                                                              ;
* Stepwise analysis                                                            ;
*                                                                              ;
* Since I like to do variable selection mostly 'by hand', lets only allow      ;
* automatic varible selection to go a little way.  We will notice immediately  ;
* that some continous and ordinal variables enter.  Better have checked them   ;
* out in an 'lg.' job as we did for AV canal to see if transformations are     ;
* needed before we go any farther.                                             ;
*                                                                              ;
  %HAZARD(
  PROC HAZARD DATA=VALVES NOCOV NOCOR;
       EVENT DEAD;
       TIME INT_DEAD;
       PARMS MUE=0.09596854 THALF=0.6781774 NU=0 FIXNU M=-2.15596 FIXTHALF FIXM
             MUC=0.00349094;
       SELECTION SLS=0.1 MAXSTEPS=5;
       EARLY
  /* Demographic variables */
       AGE_COP, BLACK, MALE,
  /* Ventricular function variables */
       NYHA,
  /* Valve lesion */
       AO_PINC, I_PATH, NVE,
  /* Valve replaced */
       MITRAL, DOUBLE,
  /* Type of prosthesis used */
       MECHVALV, BIO
       ;
       CONSTANT
  /* Demographic variables */
       AGE_COP, BLACK, MALE,
  /* Ventricular function variables */
       NYHA,
  /* Valve lesion */
       AO_PINC, I_PATH, NVE,
  /* Valve replaced */
       MITRAL, DOUBLE,
  /* Type of prosthesis used */
       MECHVALV, BIO
       ;
  );
*______________________________________________________________________________;
*                                                                              ;
*               F I N A L   M U L T I V A R I A B L  E   M O D E L             ;
*______________________________________________________________________________;
*                                                                              ;
* Here, when I am done, I will enter the variables accompanied by their        ;
* estimates, freeing up the shaping parameters (and checking that the shape    ;
* still holds after entering concomitant variables!).  MUE and MUC will        ;
* need to be the ones that correspond to the variables and the coefficients    ;
* entered (and found on one of the 'Final Results' pages).                     ;
*                                                                              ;
* %HAZARD(
* PROC HAZARD DATA=VALVES P CONSERVE OUTHAZ=OUTEST CONDITION=14 QUASI;
*      EVENT DEAD;
*      TIME INT_DEAD;
*      PARMS MUE=0.09596854 THALF=0.6781774 NU=0 FIXNU M=-2.15596
*            MUC=0.00349094;
*      EARLY NYHA=1.121142, I_PATH=0.9513664; * etc.;
*      CONSTANT BLACK=0.9266753, AGE_COP=0.01897221; * etc.;
* );
* LIBNAME VALVEST ('!HZEXAMPLES/sasest');
* DATA VALVEST.HMDTHR1 (TYPE=HAZ);
*      SET OUTEST;
*______________________________________________________________________________;
*                                                                              ;
RUN;
