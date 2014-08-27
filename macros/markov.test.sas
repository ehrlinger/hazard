// JOB markov.test.sas
sas markov.test.sas
spool cont to email
//cp markov.test.l*\
//   /programs/apps/sas/macro.library/.
//spool cont printer 'lptxt -l110 -s6 -f LetterGothic-Bold'
splfile markov.test.l*
// FILE markov.test.sas
*______________________________________________________________________________;
*                                                                              ;
* /programs/apps/sas/macro.library/markov.test.sas
*______________________________________________________________________________;
*                                                                              ;
* Test macro for multiple decrement variances
*______________________________________________________________________________;
*                                                                              ;
  options pagesize=107 linesize=132 nofmterr;
  title1 "Test Markov Confidence Limits for Multiple Decrement Analysis";
*******************************************************************************;
* Bring in multiple decrement macro                                            ;
  filename greenwod '!MACROS/markov.sas'; %inc greenwod;
*______________________________________________________________________________;
*                                                                              ;
*               D E S C R I P T I O N   O F   P A R A M E T E R S              ;
*______________________________________________________________________________;
*                                                                              ;
*  Input data set:  SAS dataset or view  Unchanged by the algorithm            ;
*                                                                              ;
*  Output data set:  New SAS data set, whose name is a macro parameter         ;
*                                                                              ;
*  Other interactions:  Creates data sets temp and tempset.  These are not     ;
*      needed at end of execution of the macro.                                ;
*                                                                              ;
*******************************************************************************;
*  Input data set variables used                                               ;
*                                                                              ;
*      The variable names are passed as parameters to the macro. The values of ;
*      the variables are as further stated below.                              ;
*      below.                                                                  ;
*                                                                              ;
*  &timevar    The numeric variable containing outcome times.  There will be   ;
*              exactly one outcome for each time.  Multiple outcomes are       ;
*              handled by multiple records, with the same value of the outcome ;
*              time.                                                           ;
*                                                                              ;
*  &eventvar   the numeric variable containing event info.  Values:            ;
*                   1 --- &lastevnt   transitions to a new state               ;
*                   others            censored, for this use of the macro      ;
*              THE CODE ASSUMES THAT EVENTVAR CONTAINS INTEGERS ONLY, AND DOES ;
*              NOT CHECK FOR THIS CONDITION.  IF THE CONDITION IS VIOLATED,    ;
*              THE RESULTS ARE UNDEFINED.                                      ;
*                                                                              ;
*  &lastevnt   The highest event value that will be analyzed.  This must be an ;
*              integer >= 1. The SAS statements in the code will in principle  ;
*              allow values of &lastevnt <= 99, but the number of variables    ;
*              will become so large as to cause poor execution long before     ;
*              that time.  (Interpretation of large values of &lastevnt is     ;
*              questionable, so this limitation is one of form, rather than    ;
*              substance.)                                                     ;
*                                                                              ;
*              In a typical use of the macro, event 1 will be an event of      ;
*              primary interest, such as structural valve failure, event 2     ;
*              will be death, and other values will represent different forms  ;
*              of censoring.  Using the macro with &lastevnt = 1 will produce  ;
*              the ordinary Kaplan-Meier analysis with death being just one    ;
*              form of censoring.  Using the macro with &lastevnt = 2 will     ;
*              produce the actual incidence of the event of interest, as well  ;
*              as the actual incidence of death.                               ;
*                                                                              ;
*******************************************************************************;
*  Output  Variable names are fixed in the macro, and some labels are          ;
*              supplied.                                                       ;
*                                                                              ;
*  &timevar    The same name as in the input data set. There is one output     ;
*              record for each distinct value in the input data set, as long   ;
*              as there is at least one event occurring at that time.  Other   ;
*              times are ignored.                                              ;
*                                                                              ;
*  atrisk      The number of patients at risk at the time                      ;
*                                                                              ;
*  event1      event&lastevnt                                                  ;
*              The number of events of that type at that time.                 ;
*                                                                              ;
*  censored    The number of censored observations.                            ;
*                                                                              ;
*  incid0      The estimated probability of remaining in state 0.  This is the ;
*              ordinary Kaplan-Meier estimate of freedom from all events.      ;
*                                                                              ;
*  incid1      incid&lastevnt                                                  ;
*              The incidence of that event, up to and including the time.      ;
*              Freedom from event is 1 - incidence.                            ;
*                                                                              ;
*  error0      error&lastevnt                                                  ;
*              The standard errors of each incidence estimate.  These are the  ;
*              square roots of the corresponding entries of the variance       ;
*              matrix.  The user may capture the entire variance matrix for    ;
*              other use.                                                      ;
*______________________________________________________________________________;
*                                                                              ;
   data myset;
   attrib etime label = 'Event Time' evnt label = 'Event Class';
   input etime evnt @@;
   cards;
 1    1      1    2      1    5
 2    1      3    6      4    2
 5    0      1    3      2    1
 8    2      5    1      3    2
 ;
  run;

  Title2 'Data Set';
  proc print label data = myset; run;

  Title2 'Ordinary Kaplan-Meier Analysis';
  %Greenwod(myset, kset, etime, evnt, 1);
  proc print label data = kset; run;

  Title2 'Standard Actual Analysis';
  %Greenwod(myset, actset, etime, evnt, 2);
  proc print label data = actset; run;

  Title2 'Analysis with 4 terminal events';
  %Greenwod(myset, fourset, etime, evnt, 4);
  proc print label data = fourset; run;
*******************************************************************************;
