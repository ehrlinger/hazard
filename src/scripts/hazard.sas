%macro hazard/parmbuff;
%global hzrnotes hzrdebug hazrc hzrdelds;
run;
%if &hzrnotes eq 1 %then %goto notes;
option nonotes;
/* Handle dummping notes */
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
    %let i=%index(&procstmt,OUTHAZ);
    %if &i eq 0 %then %do;
    %let outhaznm=;
    %goto start;
%end;
%let codeblk=%qsubstr(&code,%eval(&i+6));
%let i=%index(&codeblk,=);
%if &i eq 0 %then goto badouthz;
%let codeblk=%qleft(%qsubstr(&codeblk,%eval(&i+1)));
%let outhaznm=%qscan(&codeblk,1,%str(; ));
%start:

/* Figure out what platform we are running on, using SAS automatic    */
/* variables &SYSSCP and &SYSSCPL.  These are populated by SAS at      */
/* init on every platform and never warn, replacing the                */
/* %sysget(OSTYPE) probe that fired a benign                           */
/* "WARNING: The argument to macro function %SYSGET is not defined as  */
/* a system variable" on Windows where OSTYPE is not an OS env var.    */
%if &SYSSCP = WIN %then %do;
    %let os=%str(Windows);
%end;
%else %if %index(&SYSSCPL, Linux) %then %do;
    %let os=%str(linux);
%end;
%else %if %index(&SYSSCPL, Darwin) %then %do;
    %let os=%str(macos);
%end;
%else %if %substr(&SYSSCP,1,3) = SUN %then %do;
    %let os=%str(solaris);
%end;
%else %do;
    %let os=%lowcase(%substr(&SYSSCP,1,5));
%end;

/* If we are running on a Windows platform, we need */
/* to set up the correct separator character and remove command */
%let jobid=&sysjobid;
%let jobix=&sysindex;


/* Create the program name */
%if &os = Windows %then %do;
    %let separator=\;
    %let decsep=_;
    %let hazpgm=%sysget(HAZAPPS)&separator%str(hazard.bat);
    %let remove=del;
    %end;
%else %do;
    %if &os = solaris %then %put &os;
    %let separator=/;
    %let decsep=.;
    %let hazpgm=%sysget(HAZAPPS)&separator%str(hazard.exe);
    %let remove=rm;
    %end;

/* Create the working file names */
%let prefix=%sysget(TMPDIR)&separator%str(hzr)&decsep%str(J)&jobid&decsep%str(X)&jobix;

/* Dump a message to the screen */
%if &hzrdebug eq 1 %then
    %put Running hazard &hazpgm on &os with data &prefix.;

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
    data _null_;
        i = 1;
        set &dataname nobs=datact point=i;
        file "&prefix..sas" mod;
        put '; OBSCOUNT'+1 datact ';';
        put "JOBID J&jobid;" / "JOBIX X&jobix;";
        stop;
    run;
    
/* Run the hazard program */
/* If we are running on a Windows platform, we need */
/* to set up the correct separator character and remove command */
%if &os = Windows %then %do;
    %let command=&hazpgm &prefix;
    %end;
%else %do;
    %let command=&hazpgm < &prefix..sas > &prefix..lst;
    %end;

/* What is the command we will execute */
%if &hzrdebug eq 1 %then
    %put Execute command: &command;

/* Execute the program */
%sysexec &command;

/* Store the return value */
%let hazrc=&sysrc;
%hazdolst
%if &hazrc ne 0 %then %goto success;
%if &outhaznm eq %then %goto success;

/* Dump the output xport file */
libname HZRCALL xport "&prefix..haz";

option notes;
    data &outhaznm.(TYPE=HAZ);
    set HZRCALL.HZRCALL;
    run;
    option nonotes;
    %success:
    libname HZRCALL clear;
    %if &hzrdebug eq 1 %then %goto fin;

/* Remove temporary files. */
x &remove &prefix..*;

%goto fin;
%baddata:
    %put ERROR: DATA= parameter not found or is invalid.;
%goto finerror;
%badouthz:
    %put ERROR: OUTHAZ= parameter is invalid.;
%goto finerror;
%finerror:
    %put ERROR: Error in syntax, HAZARD not attempted.;
%fin:
    %if &hzrnotes eq -1 %then %goto nonotes;
option notes;
%nonotes: run;
%mend;
