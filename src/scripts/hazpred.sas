%macro hazpred/parmbuff;

/*
  For debugging puroses
  hzpnotes - show notes throughout execution.
  hzpdebug - show debug info
  hazrc    - do not delete temporary files 
*/
%global hzpnotes hzpdebug hazrc;
run;

%if &hzpnotes eq 1 %then %goto notes;
options nonotes;

/* Handle dumping notes */
%notes:
    %let code=%qupcase(&syspbuff);
    %let i=%index(&code,%str(;));
    %let procstmt=%qsubstr(&code,1,&i);
    %let i=%index(&procstmt,DATA);
    %if &i eq 0 %then %goto baddata;
    %let codeblk=%qsubstr(&code,%eval(&i+4));
    %let i=%index(&codeblk,=);
    %if &i eq 0 %then %goto baddata;
    %let codeblk=%qleft(%qsubstr(&codeblk,%eval(&i+1)));
    %let dataname=%qscan(&codeblk,1,%str(; ));
    %let i=%index(&procstmt,INHAZ);
    %if &i eq 0 %then %goto badinhaz;
    %let codeblk=%qsubstr(&code,%eval(&i+5));
    %let i=%index(&codeblk,=);
    %if &i eq 0 %then %goto badinhaz;
    %let codeblk=%qleft(%qsubstr(&codeblk,%eval(&i+1)));
    %let inhaznam=%qscan(&codeblk,1,%str(; ));
    %let i=%index(&procstmt,%str( OUT));
    %if &i eq 0 %then %goto badout;
    %let codeblk=%qsubstr(&code,%eval(&i+3));
    %let i=%index(&codeblk,=);
    %if &i eq 0 %then %goto badout;
    %let codeblk=%qleft(%qsubstr(&codeblk,%eval(&i+1)));
    %let outname=%qscan(&codeblk,1,%str(; ));

/* This is where the prefix stuff goes. */
/* Figure out what platform we are running on */
%if %length(%sysget(OSTYPE)) gt 0 %then %do;
    %let os=%substr(%sysget(OSTYPE),1,7);
    %end;
%else %do;
    %let os=%str(Windows);
    %end;

/* If we are running on a Windows platform, we need */
/* to set up the correct separator character and remove command */
%let jobid=&sysjobid;
%let jobix=&sysindex;
		
%put "Operating System Detector &os";
/* Create the program name */
%if &os = Windows %then %do;
    %let separator=\;
    %let decsep=_;
    %let hazpgm=%sysget(HAZAPPS)&separator%str(hazpred.bat);
    %let remove=del;
    %end;
%else %do;
    %let separator=/;
    %let decsep=.;
    %let hazpgm=%sysget(HAZAPPS)&separator%str(hazpred);
    %let remove=rm;
    %end;

/* Find the system temp space. */
%let tmpspace=&separator%str(tmp);

%if %length(%sysget(TEMPDIR)) gt 0 %then %do;
	%let tmpspace=%sysget(TEMPDIR);
	%end;
%else %do;
%if %length(%sysget(TMPDIR)) gt 0 %then %do;
	%let tmpspace=%sysget(TMPDIR);
	%end;
%else %do;
%if %length(%sysget(TEMP)) gt 0 %then %do;
	%let tmpspace=%sysget(TEMP);
	%end;
%end;
%end;

/* Make sure we can write there */
/* %if %sysfunc(fileexist(&tmpspace)) %then %do */

options notes;

/* Create the working file names */
%let prefix=&tmpspace&separator%str(hzp)&decsep%str(J)&jobid&decsep%str(X)&jobix;

/* Dump a message to the screen */
%if &hzpdebug eq 1 %then
    %put Running hazpred &hazpgm on &os with data &prefix.;

data _null_;
   file "&prefix..sas";
   %do %while(%length(&code)>150);
       %let i = 150;
       %do %while(%qsubstr(&code,&i,1)^=%quote( ));
           %let i = %eval(&i-1);
       %end;
       %let codeblk=%qsubstr(&code,1,&i);
       put "&codeblk" @;
       %let code=%qsubstr(&code,&i);
   %end;
   put "&code";
   run;

   libname HZRCALL xport "&prefix..dta";
   data HZRCALL.HZRCALL;
   set &dataname;
   run;
   libname HZRCALL xport "&prefix..haz";
   data HZRCALL.HZRCALL;
   set &inhaznam;
   run;
data _null_;
   i = 1;
   set &dataname nobs=datact point=i;
   set &inhaznam nobs=inhazct point=i;
   file "&prefix..sas" mod;
   put 'OBSCOUNT' +1 datact ';' / 'HAZCOUNT' +1 inhazct ';';
   put "JOBID J&jobid;" / "JOBIX X&jobix;";
   stop;
   run;

/* Run the program */
/* If we are running on a Windows platform, we need */
/* to set up the correct separator character and remove command */
%if &os = Windows %then %do;
    %let command=&hazpgm &prefix;
    %end;
%else %do;
    %let command=&hazpgm < &prefix..sas > &prefix..lst;
    %end;

/* What is the command we will execute */
%if &hzpdebug eq 1 %then
    %put Execute command: &command;

/* Execute the program */
%sysexec &command;

/* Store the return value */
%let hazrc=&sysrc;
%hazdolst

%if &hazrc ne 0 %then %goto success;

/* Dump the output xport file */
libname HZRCALL xport "&prefix..out";
option notes;
data &outname.;
   set HZRCALL.HZRCALL;
   run;
option nonotes;

%success:
libname HZRCALL clear;
%if &hzpdebug eq 1 %then %goto fin;
    /* Remove temporary files. */
    x &remove &prefix..*;
    
%goto fin;
%baddata:
   %put ERROR: DATA= parameter not found or is invalid.;
   %goto finerror;
%badinhaz:
   %put ERROR: INHAZ= parameter not found or is invalid.;
   %goto finerror;
%badout:
   %put ERROR: OUT= parameter not found or is invalid.;
   %goto finerror;
%finerror:
   %put ERROR: Error in syntax, HAZPRED not attempted.;
%fin:
%if &hzpnotes eq -1 %then %goto nonotes;
options notes;
%nonotes:run;
%mend;
