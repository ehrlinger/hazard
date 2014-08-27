//JOB bh.dead.summary.sas
sas8.2 bh.dead.summary.sas
//cp bh.dead.summary.l*\
//   /studies/cardiac/ischemic/infarct/rupture/analyses/.
//spool cont printer 'lptxt -l110 -s6 -f LetterGothic-Bold'
spool cont to email
splfile bh.dead.summary.l*
// FILE bh.dead.summary.sas
* <2003-10-24>                                                                 ;
*______________________________________________________________________________;
*                                                                              ;
* /studies/cardiac/ischemic/infarct/rupture/analyses/bh.dead.summary.sas
*______________________________________________________________________________;
*                                                                              ;
* Mechanical Consequenses of  Myocardial Infarction                            ;
* (CCF, 1972 to 1998, n=130)                                                   ;
*                                                                              ;
* Multivariable analysis of death
* Boostrap variable selection method, looking for possible outliers
*                                                                              ;
* SUMMARY                                                                      ;
*______________________________________________________________________________;
*                                                                              ;
  options pagesize=107 linesize=132;
  libname est     '/studies/cardiac/ischemic/infarct/rupture/estimates';
  filename sumboot "!MACROS/bootstrap.summary.sas";  %inc sumboot;
  filename cluster "!MACROS/bootstrap.clusters.sas";  %inc cluster;
  title1 "Mechanical Consequenses of  Myocardial Infarction";
  title2 "(CCF; 1972 to 1998; n=130)";
  title3 "Multivariable Analysis of Death";
  title4 "Bootstrap Analysis (Resampling) Using Completely Stepwise Selection";
  title5 "Summary of Results";
*______________________________________________________________________________;
*                                                                              ;
*                  B O O T S T R A P   S A M P L E S                           ;
*______________________________________________________________________________;
*                                                                              ;
  data bhearly; set est.deade;
* data bhconst;*set est.deadc;
  data bhlate;  set est.deadl;
*______________________________________________________________________________;
*                                                                              ;
*                B O O T S T R A P   R E S A M P L I N G                       ;
*                                                                              ;
*               B A G G I N G   S U M M A R I Z A T I O N                      ;
*______________________________________________________________________________;
*                                                                              ;
* Early hazard phase                                                           ;
  title7 "Early Hazard Phase";
  %sumboot(in=bhearly);
  run;
*******************************************************************************;
* Constant hazard phase                                                        ;
* title7 "Late Hazard Phase";
* %sumboot(in=bhlate);
* run;
*******************************************************************************;
* Late hazard phase                                                            ;
  title7 "Late Hazard Phase";
  %sumboot(in=bhlate);
  run;
*______________________________________________________________________________;
*                                                                              ;
*         B O O T S T R A P   B A G G I N G   C L U S T E R I N G
*______________________________________________________________________________;
*                                                                              ;
* Bootstrap bagging clustering algorithm.                                      ;
* The purpose is to look for variables in a list of highly correlated variables;
* (a cluster) and determine 1) how often each varible appeared (variable name  :
* cluster), and 2) how often at least one variable in the cluster appears      :
* (variable name ncluster)                                                     :
*______________________________________________________________________________:
*                                                                              ;
* DEFINITIONS OF CALLING ARGUMENTS:                      DEFAULT:              ;
*                                                                              ;
*   IN       = Bootstrap estimates                       IN                    ;
*   OUT      = Clustering output  (complete data)        OUT                   ;
*              To the input dataset are added                                  ;
*              CLUSTER = cluster appeared (1) or not (0)                       ;
*              NCLUSTER = Number of multiple appearances                       ;
*                         of a cluster name                                    ;
*                                                                              ;
*   CLUSTER  = Summary clustering                        CLUSTER               ;
*              Frequency of cluster appearance                                 ;
*                (PCTCLUST) and of multiple
*                appearances of variables (NCLUSTER)                           ;
*                                                                              ;
*   CLNAME   = Cluster name                              cluster name          ;
*   PRINT    = Printed output                            1 (0 for none)        ;
*   LIST     = Variables included in cluster             none                  ;
*              varname_1 varname_2 ... varname_n                               ;
*______________________________________________________________________________;
*                                                                              ;
* Cluster related to age                                                       ;
  title7 "Early Phase Age Cluster";
  %cluster(in=bhearly, cluster=cluste1, clname=Age,
           list=age ln_age in_age age2 agee);
  run;

  title7 "Late Phase Age Cluster";
  %cluster(in=bhlate, cluster=clustl1, clname=Age,
           list=age ln_age in_age age2 agee);
  run;
*******************************************************************************;
* Cluster related to NYHA                                                      ;
  title7 "Early Phase NYHA Cluster";
  %cluster(in=bhearly, cluster=cluste2, clname=NYHA,
           list=nyhc_fc ln_nyhc in_nyhc nyha_aug ln_nyhaa in_nyhaa);
  run;

  title7 "Late Phase NYHA Cluster";
  %cluster(in=bhlate, cluster=clustl2, clname=NYHA,
           list=nyhc_fc ln_nyhc in_nyhc nyha_aug ln_nyhaa in_nyhaa);
  run;
*******************************************************************************;
* Cluster related to LV function                                               ;
  title7 "Early Phase LV Function Cluster";
  %cluster(in=bhearly, cluster=cluste3, clname=LV Function,
           list=lvf ln_lvf in_lvf lvf2 lvfe lv_norm lv_mild lv_mod lv_mods
           lv_sev);
  run;

  title7 "Late Phase LV Function Cluster";
  %cluster(in=bhlate, cluster=clustl3, clname=LV Function,
           list=lvf ln_lvf in_lvf lvf2 lvfe lv_norm lv_mild lv_mod lv_mods
           lv_sev);
  run;
*______________________________________________________________________________;
*                                                                              ;
*                   S U M M A R I Z E   C L U S T E R S                        ;
*______________________________________________________________________________;
*                                                                              ;
  title7 "Cluster Summary: Early Hazard Phase";
  data tote; set cluste1 cluste2 cluste3;
  proc print data=tote; run;

  title7 "Cluster Summary: Late Hazard Phase";
  data totl; set clustl1 clustl2 clustl3;
  proc print data=totl; run;
*******************************************************************************;
