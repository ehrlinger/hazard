%{
#include <stdio.h>
#include "hazpred_y.h" /* parser defines */

int yylnctr,yysynerr;
%}
ws		[ \t\n\)]+
integer		[0-9]+
dreal		([0-9]*"."[0-9]+)
ereal		([0-9]*"."[0-9]+"E"[+-]?[0-9]+)
number		{integer}|{dreal}|{ereal}
name		([_A-Z][_A-Z0-9]*)
libmem		({name}"."{name})

%start HZPP DSNM STMT VARS CTRL
%{
#undef YYLMAX
#define YYLMAX 4096
%}
%%

{ws}		;
<STMT>\*[^;]*;		yylnctr++;
;			{ BEGIN STMT; yylnctr++; return ';'; }
<HZPP,CTRL>{number}	{ sscanf(yytext,"%lf",&yylval.real); return NUMBER; }
<HZPP,DSNM>=		return '=';
\(			{ BEGIN HZPP; yysynerr = 0; yylnctr = 1; }
<HZPP>PROC		return PROC;
<HZPP>HAZPRED		return HAZPRED;
<HZPP>DATA		{ BEGIN DSNM; return DATA; }
<HZPP>INHAZ		{ BEGIN DSNM; return INHAZ; }
<HZPP>OUT		{ BEGIN DSNM; return OUT; }
<HZPP>CL	|
<HZPP>CLIMITS		return CLIMITS;
<HZPP>NOCL		return NOCL;
<HZPP>NOS 	|
<HZPP>NOSURV		return NOSURV;
<HZPP>NOH	|
<HZPP>NOHAZ		return NOHAZ;
<HZPP>NOLOG		return NOLOG;
<HZPP>NONOTES		return NONOTES;
<DSNM>{libmem}		{ BEGIN HZPP; return LIBMEM; }
<DSNM>{name}		{ BEGIN HZPP; return NAME; }
<STMT>TIME		{ BEGIN VARS; return TIME; }
<STMT>ID		{ BEGIN VARS; return ID; }
<STMT>OBSCOUNT		{ BEGIN CTRL; return OBSCOUNT; }
<STMT>HAZCOUNT		{ BEGIN CTRL; return HAZCOUNT; }
<STMT>JOBID		{ BEGIN CTRL; return JOBID; }
<STMT>JOBIX		{ BEGIN CTRL; return JOBIX; }
<VARS,CTRL>{name}	{ yylval.name = yytext; return NAME; }
[\.\-_A-Z0-9][\.\-_A-Z0-9]*	{ printf("$ERROR: Unexpected text, stmt %d: %s\n",yylnctr,yytext); yysynerr = 1; }
.		{ printf("$ERROR: Unexpected char, stmt %d: %s\n",yylnctr,yytext); yysynerr = 1; }

