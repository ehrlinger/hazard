*___________________________________________________________________________;
*                                                                              ;
* !HZTESTS/hm.dead.n1surg.sas
*______________________________________________________________________________;
*                                                                              ;
* Carcinoma of the Esophagus
* N1 Burden
* CCF, 1983 to 7/2000, n=606
* Surgery only group, with N1
*
* Multivariable analysis of death
*______________________________________________________________________________;
*                                                                              ;
  options pagesize=107 linesize=132 nofmterr;
  libname library V6 '!HZTESTS/datasets';
  libname est V6 '!HZTESTS/estimates';
  data n1burden; set library.ddw;
  title1 "Carcinoma of the Esophagus";
  title2 "N1 Burden in Surgery Only pN1 Subgroup";
  title3 "(CCF; 1985 to 2000; n=606)";
  title4 "Multivariable Analysis of Death";
*______________________________________________________________________________;
*                                                                              ;
* Bring in variables                                                           ;
*______________________________________________________________________________;
*                                                                              ;
  filename vars '!HZTESTS/datasets/vars.sas';
  %inc vars; %vars(missing=1); run;
  data n1burden; set n1burden; if surgonly and pn1;
*______________________________________________________________________________;
*                                                                              ;
*  M U L T I V A R I A B L E   H A Z A R D   F U N C T I O N   A N A L Y S I S
*______________________________________________________________________________;
*                                                                              ;
  %hazard(
   proc hazard data=n1burden nocov nocor condition=14 mi=200;
        event dead;
        time iv_dead;
        parms mue=2.853738 thalf=3.028267 nu=1 fixnu m=0 fixm
              fixthalf fixeta
              mul=0.2754651 tau=1 fixtau alpha=1 fixalpha gamma=1 fixgamma
              eta=0.716051 weibull;

        restrict posnodes ln_npos in_npos in2npos pctpos ln_pos in_pos;

        selection sle=0.5 sls=0.10 maxsteps=5 robust;

       early
  /* Demographic */
       male/E, female/E, age, ln_age, in_age, age2, agee, black/E, white/E,
  /* Preoperative factors */
       prior, prioreg,
  /* Cancer pathology */
       barr, pt, pt_1, pt_1im, pt_1sm, pt_2, pt_3, pt_4, ln_pt, in_pt, in2pt,
       posnodes, ln_npos, in_npos, in2npos, pctpos, ln_pos, in_pos/S,
       in2_pos, sq,
       ad, sig/E, pm1a, pm1b, pm, castage, ln_stage, in_stage, st2b, st3, st4a,
       st4b, posmarg/S, diff, ln_diff, in_diff,
  /* Surgical procedure */
       anas_n, appr_ch, appr_th,
  /* Postoperative therapy */
       anypost/S,
  /* Experience */
       opyrs, ln_opyrs, in_opyrs, in2opyrs, opyrs2

       ;

       late
  /* Demographic */
       male/E, female/E, age, ln_age, in_age, age2, agee, black/E, white/E,
  /* Preoperative factors */
       prior, prioreg,
  /* Cancer pathology */
       barr, pt, pt_1, pt_1im, pt_1sm, pt_2, pt_3, pt_4, ln_pt, in_pt, in2pt,
       posnodes, ln_npos, in_npos, in2npos, pctpos, ln_pos, in_pos, in2_pos, sq,
       ad, sig/E, pm1a/S, pm1b/S, pm, castage, ln_stage, in_stage, st2b, st3,
       st4a, st4b, posmarg/S, diff, ln_diff, in_diff,
  /* Surgical procedure */
       anas_n, appr_ch, appr_th,
  /* Postoperative therapy */
       anypost/S,
  /* Experience */
       opyrs, ln_opyrs/S, in_opyrs, in2opyrs, opyrs2

       ;
  );
  %macro model;
  %mend;
*______________________________________________________________________________;
*                                                                              ;
*               C H E C K   O U T   F I N A L   M O D E L                      ;
*______________________________________________________________________________;
*                                                                              ;
  %macro check;
  %hazard(
  proc hazard data=n1burden nocov nocor condition=14 mi=200;
       event dead;
       time iv_dead;
       parms mue=2.853738 thalf=3.028267 nu=1 fixnu m=0 fixm
             fixthalf fixeta
             mul=0.2754651 tau=1 fixtau alpha=1 fixalpha gamma=1 fixgamma
             eta=0.716051 weibull ;

       selection sle=0.12 sls=0.10 robust maxsteps=0;

       early

         in_age=-3.11266, it=1.339477, pn1=0.7427647, pn1chest=-0.217969
       ;
       late

         ln_npos=-1.0624, in_npos=-10.0637, pm=0.7461937, pn0_pt=1.090695
       ;
  );

  %mend;

*______________________________________________________________________________;
*                                                                              ;
*               F I N A L   M U L T I V A R I A B L  E   M O D E L             ;
*______________________________________________________________________________;
*                                                                              ;
  %hazard(
  proc hazard data=n1burden conserve outhaz=outest condition=14 mi=200;
       event dead;
       time iv_dead;
       parms mue=4.56392 thalf=3.446444 nu=1 fixnu m=0 fixm
             mul=0.6741454 tau=1 fixtau alpha=1 fixalpha gamma=1 fixgamma
             eta=0.7069431 weibull ;
       early
         in_pos=-0.127405, posmarg=0.5395116, anypost=0.5994018;
       late
         pm1a=1.074573, pm1b=1.485148, anypost=-2.01541, ln_opyrs=-0.365839;
  );

  data est.hmdtn1s (type=haz);
    set outest;

  %hazard(
  proc hazard data=n1burden conserve outhaz=outest condition=14 mi=200 steepest
       quasi;
       event dead;
       time iv_dead;
       parms mue=1.002958 thalf=3.298245 nu=1 fixnu m=0 fixm
             mul=0.7151009 tau=1 fixtau alpha=1 fixalpha gamma=1 fixgamma
             eta=0.7236128 weibull ;
       early
         ln_npos=0.4616811, posmarg=0.6858193, anypost=0.6849007;
       late
         pm1a=1.086815, pm1b=1.564425, anypost=-2.06235, ln_opyrs=-0.387502;
  );

  data est.hmdtn1sn (type=haz);
    set outest;
*______________________________________________________________________________;
*                                                                              ;
*     E V A L U A T E   M O D E L   B Y   D E C I L E S   O F   R I S K        ;
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
  filename deciles ('$MACROS/deciles.hazard.sas');
  %inc deciles;
  %deciles(in=n1burden, event=dead, interval=iv_dead, time=5, groups=5,
    vars=age male pt pm1a pm1b posmarg posnodes pctpos anypost opyrs);
  run;
*******************************************************************************;
  %macro final;
  %mend;
