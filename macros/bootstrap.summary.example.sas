//JOB bh.dead.summary.sas
sas8 bh.dead.summary.sas
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
  filename sumboot "!MACRO/bootstrap.summary.sas";  %inc sumboot;
  title1 "Mechanical Consequenses of  Myocardial Infarction";
  title2 "(CCF; 1972 to 1998; n=130)";
  title3 "Multivariable Analysis of Death";
  title4 "Bootstrap Analysis (Resampling) Using Completely Stepwise Selection";
  title5 "Summary of Results";
*______________________________________________________________________________;
*                                                                              ;
*                B O O T S T R A P   R E S A M P L I N G                       ;
*                                                                              ;
*               B A G G I N G   S U M M A R I Z A T I O N                      ;
*______________________________________________________________________________;
*                                                                              ;
* Early phase                                                                  ;
  title5 "Early Phase";
  %sumboot(in=est.deade);
  run;
*******************************************************************************;
* Late phase                                                                   ;
  title5 "Late Phase";
  %sumboot(in=est.deadl);
  run;
*******************************************************************************;
