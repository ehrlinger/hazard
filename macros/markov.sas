/**************************************************************************/
/**********    Algorithms for actual and actuarial Analysis    ************/
/**************************************************************************/

/********************************************************************************/
/*   Usage                                                                      */
/*                                                                              */
/*   %Greenwod(inset =, outset =, timevar =, eventvar= , lastevnt=)             */
/*      A value must be given for each of the five macro parameters             */
/********************************************************************************/

/***********************************************************************************************************************/
/*                                                                                                                     */
/* Computes actual incidence of events, together with standard errors from the matrix form of Greenwood's formula.     */
/*                                                                                                                     */
/*   Coded into SAS by Bill Anderson, September 1999                                                                   */
/*  The code uses macro assignments instead of array multplication -- perhaps the latter would be cleaner              */
/***********************************************************************************************************************/

/***********************************************************************************************************************/
/*  Input data set:  SAS dataset or view  Unchanged by the algorithm                                                   */
/*                                                                                                                     */
/*  Output data set:  New SAS data set, whose name is a macro parameter                                                */
/*                                                                                                                     */
/*  Other interactions:  Creates data sets temp and tempset.  These are not needed at end of execution of the macro.   */
/*                                                                                                                     */
/*  Input data set variables used                                                                                      */
/*                                                                                                                     */
/*      The variable names are passed as parameters to the macro; the values of the variables are as further stated    */
/*      below.                                                                                                         */
/*                                                                                                                     */
/*  &timevar    The numeric variable containing outcome times.  There will be exactly one outcome for each time;       */
/*              multiple outcomes are handled by multiple records, with the same value of the outcome time.            */
/*                                                                                                                     */
/*  &eventvar   the numeric variable containing event info.  Values:                                                   */
/*                   1 --- &lastevnt   transitions to a new state                                                      */
/*                   others            censored, for this use of the macro                                             */
/*              THE CODE ASSUMES THAT EVENTVAR CONTAINS INTEGERS ONLY, AND DOES NOT CHECK FOR THIS CONDITION.  IF      */
/*              THE CONDITION IS VIOLATED, THE RESULTS ARE UNDEFINED.                                                  */
/*                                                                                                                     */
/*  &lastevnt   The highest event value that will be analyzed.  This must be an integer >= 1. The SAS statements in    */
/*              the code will in principle allow values of &lastevnt <= 99, but the number of variables will become    */
/*              so large as to cause poor execution long before that time.  (Interpretation of large values of         */
/*              &lastevnt is questionable, so this limitation is one of form, rather than substance.)                  */
/*                                                                                                                     */
/*              In a typical use of the macro, event 1 will be an event of primary interest, such as structural        */
/*              valve failure, event 2 will be death, and other values will represent different forms of censoring.    */
/*              Using the macro with &lastevnt = 1 will produce the ordinary Kaplan-Meier analysis with death being    */
/*              just one form of censoring; using the macro with &lastevnt = 2 will produce the actual incidence of    */
/*              the event of interest, as well as the actual incidence of death.                                       */
/*                                                                                                                     */
/*      Output   Variable names are fixed in the macro, and some labels are supplied.                                  */
/*                                                                                                                     */
/*   &timevar   The same name as in the input data set. There is one output record for each distinct value in the      */
/*              input data set, as long as there is at least one event occurring at that time; other times are ignored.*/
/*                                                                                                                     */
/*    atrisk    The number of patients at risk at the time                                                             */
/*                                                                                                                     */
/*    event1 - event&lastevnt                                                                                          */
/*              The number of events of that type at that time.                                                        */
/*                                                                                                                     */
/*    censored  The number of censored observations.                                                                   */
/*                                                                                                                     */
/*                                                                                                                     */
/*    incid0    The estimated probability of remaining in state 0.  This is the ordinary Kaplan-Meier estimate of      */
/*              freedom from all events.                                                                               */
/*                                                                                                                     */
/*    incid1 - incid&lastevnt                                                                                          */
/*              The incidence of that event, up to and including the time; freedom from event is 1 - incidence.        */
/*                                                                                                                     */
/*    error0 - error&lastevnt                                                                                          */
/*              The standard errors of each incidence estimate.  These are the square roots of the corresponding       */
/*              entries of the variance matrix;  the user may capture the entire variance matrix for other use.        */
/*                                                                                                                     */
/***********************************************************************************************************************/

%macro Greenwod(inset =, outset =, timevar =, eventvar =, lastevnt =);

data tempset (keep = &timevar &eventvar); set &inset;
/* Kill undefined times, and reclassify events */
 if &timevar >= 0;
 if &eventvar < 1 | &eventvar > &lastevnt then &eventvar = 0;  /* Censor these events */
 run;

/* Count total at risk */
/* Add dummy observations to ensure that all designated events are reflected by variables in the output data set */
/* These will have missing time, and will be killed in a couple of steps */
data tempset; set tempset nobs = numpats end = last;
 output;
 if last then do; &timevar = .; do &eventvar = 1 to &lastevnt; output; end; end;
 call symput('numpats', put(numpats, 10.));
 run;

proc sort data = tempset;
 by &timevar;
 run;

proc freq noprint data = tempset;
 by &timevar;
 tables &eventvar/out = temp;
 run;

proc transpose data = temp out = &outset;
 by &timevar;
 id &eventvar;
 var count;
 run;

data &outset; set &outset; if &timevar ^= .; run; /* kill the dummy observations */

/* Now add an at risk column and put missing counts to zero */
/* Also make the variable names as promised */
data &outset; set &outset;
 rename _0 = censored;
 %do event = 1 %to &lastevnt; rename _&event = event&event; %end;
 attrib atrisk label = 'Patients at Risk';
 retain atrisk newrisk &numpats;
 drop newrisk;
 atrisk = newrisk;
  if _0 = . then _0 = 0; newrisk = atrisk - _0;
  %do event = 1 %to &lastevnt;
   if _&event = . then _&event = 0;
   newrisk = newrisk - _&event;
   %end;
 run;

/* Now do the actual computations */
data &outset; set &outset;

 /* Some labels for variables coming in */
 attrib
  censored label = 'Censored Observations'
  %do event = 1 %to &lastevnt; event&event label = "Incidents of Event &event" %end;
  ;

 /* Components of present incidence vector */
 /* (In the general model, this is the first row of the transition matrix) */
 /* Only the first row is variable */
 attrib
  pk0_0 label = 'Freedom from all events'
  %do event = 1 %to &lastevnt;
   pk0_&event label = "Incidence of event &event"
   %end;;

  /* Components of previous incidence vector */
 attrib
  pkb0_0 label = 'Previous freedom from all events'
  %do event = 1 %to &lastevnt;
   pkb0_&event label = "Previous incidence of event &event"
   %end;;

  /* Components of present covariance matrix */
 attrib
  %do row = 0 %to &lastevnt; %do col = 0 %to &lastevnt;
    ck&row._&col label = "Variance component (&row, &col)"
    %end; %end;;

  /* Components of previous covariance matrix */
 attrib
  %do row = 0 %to &lastevnt; %do col = 0 %to &lastevnt;
    ckb&row._&col label = "Previous variance component (&row, &col)"
    %end; %end;;

  /* Standard Errors */
 attrib error0 label = 'Standard Error of Freedom from All Events'
  %do event = 1 %to &lastevnt; error&event label = "Standard Error of Incidence of Event &event" %end;;

  /* Components of variance addition due to new terms */
  /* These are from the multinomial distribution */
 attrib
  %do row = 0 %to &lastevnt; %do col = 0 %to &lastevnt;
    va&row._&col label = "Additional variance component (&row, &col)"
    %end; %end;;

 /* Components of differential transition Matrix */
 /* All but the first row are constant, but are made explicit for clarity of code */
 attrib
  %do row = 0 %to &lastevnt; %do col = 0 %to &lastevnt;
    da&row._&col label = "Differential transition (&row, &col)"
   %end; %end;;

 /* retain variables needed for the recursion */
 /* also initialize, for use the first time around */
 retain
  pkb0_0 1 %do event = 1 %to &lastevnt; pkb0_&event 0 %end;
  %do row = 0 %to &lastevnt; %do col = 0 %to &lastevnt;
    ckb&row._&col 0
    %end; %end;;


 /* Now to the general case */
 /* Compute terms needed for the update */
 /* Differential transition matrix */
 da0_0 = 1;
 %do event = 1 %to &lastevnt;
  da0_&event = event&event/atrisk;
  da0_0 = da0_0 - da0_&event;
  %end;
 /* The rest of this section is logically redundant, and kept in to clarify the matrix multiplication */
 %do row = 1 %to &lastevnt; %do col = 0 %to &lastevnt;
   %if &row = &col %then %do; da&row._&col = 1; %end;
   %else %do; da&row._&col = 0; %end;
   %end; %end;

 /* Additional variance component */
 /* The matrix is symmetric; for clarity all terms are explicitly created */
 %do row = 0 %to &lastevnt; %do col = 0 %to &lastevnt;
   %if &row = &col %then %do; va&row._&col = da0_&row*(1 - da0_&row)/atrisk; %end;
   %else %do; va&row._&col = -da0_&row*da0_&col/atrisk; %end;
   %end; %end;

 /* Updating the incidence vector is one matrix multiplication */
 %do col = 0 %to &lastevnt;
   pk0_&col = 0;
   %do event = 0 %to &lastevnt;
     pk0_&col = pk0_&col + pkb0_&event*da&event._&col;
     %end; %end;

 /* Updating the Variance matrix is more complicated */
 /* There is the sum of two products of three matrices each */
 /* The first part of the first product has the temporary name t1 */
 %do row = 0 %to &lastevnt; %do col = 0 %to &lastevnt;
   t1&row._&col = 0;
   %do event = 0 %to &lastevnt;
    t1&row._&col = t1&row._&col + da&event._&row*ckb&event._&col;
    %end; %end; %end;

 /* Finishing the first product */
 %do row = 0 %to &lastevnt; %do col = 0 %to &lastevnt;
   ck&row._&col = 0;
   %do event = 0 %to &lastevnt;
    ck&row._&col = ck&row._&col + t1&row._&event*da&event._&col;
    %end; %end; %end;

 /* Now add in the second term */
 %do row = 0 %to &lastevnt; %do col = 0 %to &lastevnt;
   ck&row._&col = ck&row._&col + va&row._&col*pkb0_0**2;
   %end; %end;

 /* Finally fix up the terms to be retained for next time around */
 %do event = 0 %to &lastevnt; pkb0_&event = pk0_&event; %end;
 %do row = 0 %to &lastevnt; %do col = 0 %to &lastevnt;
   ckb&row._&col = ck&row._&col;
   %end; %end;

 /* And the output standard errors */
 %do event = 0 %to &lastevnt; error&event = sqrt(ck&event._&event); %end;
 run;

data &outset /* keep only those variables likely to be of interest */
 (keep = &timevar atrisk censored
         %do event = 1 %to &lastevnt; event&event %end;
         %do event = 0 %to &lastevnt; pk0_&event %end;
         %do event = 0 %to &lastevnt; error&event %end;);
 set &outset;
 run;

/***********************************************************************************************************************/
/*                                                                                                                     */
/* Based on the Markov Chain approach, as presented in the textbook of Andersen, Borgan, Gill, and Keiding.            */
/*                                                                                                                     */
/* The survival process is modeled as an n state Markov chain, where n = &lastevnt + 1.  The initial state is state 0, */
/* and states 1, ..., &lastevnt  are absorbing states.   The first row of the transition matrix consists of the        */
/* incidence functionss for the various states, with the (0,0) element being the freedom from all events.  The         */
/* remaining rows of the transition matrix are structurally constant in this application.                              */
/*                                                                                                                     */
/* The variance matrix would be n^2 x n^2, and the variance is given iteratively by formula 4.4.19 of Andersen, et al. */
/* However, since all but the last row is structurally constant, the variance reduces to n x n, and the variance is    */
/*                                                                                                                     */
/* More specifically, let P_k be the transition matrix from the start to event time k, C_k its variance matrix, and    */
/*  VA_k the matrix of variances of the new terms.   Then Greenwood's formula is                                       */
/*                         C_{k+1} = P_{k+1}^{transpose}*C_k*P_{k+1} + VA_{k+1}*S_k^2,                                 */
/*  where S_k is the overall freedom from all events, and * represents matrix product.                                 */
/*                                                                                                                     */
/* This recursion is 4.4.19 of Andersen, et al.  It is an easy algebraic identity to see that this recursion gives     */
/* the familiar Greenwood formula in the scalar case;  it also agrees with the formula of Gaynor et al. in the actual  */
/* survival (competing risks) case.                                                                                    */
/*                                                                                                                     */
/* The SAS macro code defines all elements of the transition matrix, and ignores the symmetry of the variance.  This   */
/* Is a tradeoff between execution speed and code clarity.                                                             */
/*                                                                                                                     */
/*  References:                                                                                                        */
/*                                                                                                                     */
/*  Andersen, PK, Borgan, O, Gill, R., and Keiding, N, Statistical Models based on Counting Processes, Springer, 1993  */
/*                                                                                                                     */
/*  Gaynor, JJ, et al., On the use of Cause-Specific Failure and Conditional Failure Probabilities:  Examples for      */
/*  Clinical Oncology, JASA, vol 88, 1993, pages 400 - 409.                                                            */
/***********************************************************************************************************************/


 %mend;
 %macro sample;

/*   Sample Driver */

options mprint;

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

Title 'Data Set';
proc print label data = myset; run;

Title 'Ordinary Kaplan-Meier Analysis';
%Greenwod(inset = myset, outset = kset, timevar = etime, eventvar = evnt, lastevnt = 1)
proc print label data = kset; run;

Title 'Standard Actual Analysis';
%Greenwod(inset = myset, outset = actset, timevar = etime, eventvar = evnt, lastevnt = 2)
proc print label data = actset; run;

Title 'Analysis with 4 terminal events';
%Greenwod(inset = myset, outset = fourset, timevar = etime, eventvar = evnt, lastevnt = 4)
proc print label data = fourset; run;

 %mend;
