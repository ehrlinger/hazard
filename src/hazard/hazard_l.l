%p 4000
%e 2000
%{
/*
  (12/21/96) RAM
         Changed hazard.l RESTRICT production added yylval.nam = yytext
         RESTRICT command had stopped working somtime mid 1995
         This change is made pending testing under PC version of gnu yacc/flex.
*/
#include <stdio.h>
#include "hazard_y.h" /* parser defines */
int yylnctr,yysynerr;
%}

/* Regular expressions */
/* ------------------- */
ws		[ \t\n\)]+
integer		[0-9]+
dreal		([0-9]*"."[0-9]+)
ereal		([0-9]*"."[0-9]+"E"[+-]?[0-9]+)
realge0		{integer}|{dreal}|{ereal}
reallt0		("-"({realge0}))
number		{realge0}|{reallt0}
name		([_A-Z][_A-Z0-9]*)
libmem		({name}"."{name})

/* start conditions make things more complicated */
%start HZRP DSNM STMT VARS ICNS PARM STEP PHVR PHOP CTRL BNDL
%{
#undef YYLMAX
#define YYLMAX 4096
%}
%%

{ws}		;
<STMT>\*[^;]*;		yylnctr++;
;		{ BEGIN STMT; yylnctr++; return ';'; }
<HZRP,PARM,STEP,PHVR,PHOP,CTRL>{number}	{ sscanf(yytext,"%lf",&yylval.real);
                                          return NUMBER; }
<DSNM,HZRP,PARM,STEP,PHVR,PHOP,ICNS>=	return '=';
\(			{ BEGIN HZRP; yysynerr = 0; yylnctr = 1; }
<HZRP>PROC		return PROC;
<HZRP>HAZARD		{ yylval.name = yytext; return HAZARD; }
<HZRP>DATA		{ BEGIN DSNM; return DATA; }
<HZRP>OUTHAZ		{ BEGIN DSNM; return OUTHAZ; }
<HZRP>MI	|
<HZRP>MAXITER			return MAXITER;
<HZRP>CONDITION		return CONDITION;
<HZRP>NOCOV		return NOCOV;
<HZRP>NOCOR		return NOCOR;
<HZRP>P	|
<HZRP>PRINTIT		return PRINTIT;
<HZRP>CON	|
<HZRP>CONSERVE		return CONSERVE;
<HZRP>NOCON	|
<HZRP>NOCONSERVE	return NOCONSERVE;
<HZRP>QUASI	|
<HZRP>QUASINEWTON	return QUASINEWTON;
<HZRP>STEEPEST		return STEEPEST;
<HZRP>NUMERIC		return NUMERIC;
<HZRP>NOPRINT		return NOPRINT;
<HZRP>NOLOG		return NOLOG;
<HZRP>NONOTES		return NONOTES;
<DSNM>{libmem}		{ BEGIN HZRP; return LIBMEM; }
<DSNM>{name}		{ BEGIN HZRP; yylval.name = yytext; return NAME;}
<STMT>TIME		{ BEGIN VARS; return TIME; }
<STMT>EVENT		{ BEGIN VARS; return EVENT; }
<STMT>RCENSOR		{ BEGIN VARS; return RCENSOR; }
<STMT>ICENSOR		{ BEGIN ICNS; return ICENSOR; }
<STMT>LCENSOR		{ BEGIN VARS; return LCENSOR; }
<STMT>WEIGHT		{ BEGIN VARS; return WEIGHT; }
<STMT>PARMS	|
<STMT>PARAMETERS	{ BEGIN PARM; return PARAMETERS; }
<PARM>MUE		return MUE;
<PARM>DELTA		return DELTA;
<PARM>THALF		return THALF;
<PARM>NU		return NU;
<PARM>M			return M;
<PARM>MUC		return MUC;
<PARM>MUL		return MUL;
<PARM>TAU		return TAU;
<PARM>GAMMA		return GAMMA;
<PARM>ALPHA		return ALPHA;
<PARM>ETA		return ETA;
<PARM>WEIBULL		return WEIBULL;
<PARM>FIXDELTA		return FIXDELTA;
<PARM>FIXTHALF		return FIXTHALF;
<PARM>FIXNU		return FIXNU;
<PARM>FIXM		return FIXM;
<PARM>FIXMNU1		return FIXMNU1;
<PARM>FIXTAU		return FIXTAU;
<PARM>FIXGAMMA		return FIXGAMMA;
<PARM>FIXALPHA		return FIXALPHA;
<PARM>FIXETA		return FIXETA;
<PARM>FIXGE2		return FIXGE2;
<PARM>FIXGAE2		return FIXGAE2;
<STMT>STEPWISE |
<STMT>SELECT |
<STMT>SELECTION		{ BEGIN STEP; return STEPWISE; }
<STEP>FW |
<STEP>FORWARD |
<STEP>SW |
<STEP>STEPWISE		return STEPWISE;
<STEP>SLE |
<STEP>SLENTRY		return SLENTRY;
<STEP>SLS |
<STEP>SLSTAY		return SLSTAY;
<STEP>M |
<STEP>MOVE		return MOVE;
<STEP>MAXS |
<STEP>MAXSTEPS		return MAXSTEPS;
<STEP>MAXV |
<STEP>MAXVARS		return MAXVARS;
<STEP>BW |
<STEP>BACKWARD		return BACKWARD;
<STEP>F |
<STEP>FAST		return FAST;
<STEP>NOS |
<STEP>NOPRINTS		return NOPRINTS;
<STEP>NOQ |
<STEP>NOPRINTQ		return NOPRINTQ;
<STEP>ROBUST		return ROBUST;
<STEP>SEMI |
<STEP>SEMIROBUST	return SEMIROBUST;
<STEP>NOSW |
<STEP>NOSTEPWISE	return ONEWAY;
<STMT>EARLY		{ BEGIN PHVR; 
                          yylval.name = yytext; 
                          return EARLY; }
<STMT>CONSTANT		{ BEGIN PHVR; 
                          yylval.name = yytext;
                          return CONSTANT; }
<STMT>LATE		{ BEGIN PHVR; 
                          yylval.name = yytext; 
                          return LATE; }
<PHVR>\/		{ BEGIN PHOP; return '/'; }
<PHVR>\,		return ',';
<PHOP>\,		{ BEGIN PHVR; return ','; }
<PHOP>E |
<PHOP>EXCLUDE		return EXCLUDE;
<PHOP>I |
<PHOP>INCLUDE		return INCLUDE;
<PHOP>S |
<PHOP>START		return START;
<PHOP>M |
<PHOP>MOVE		return MOVE;
<PHOP>O |
<PHOP>ORDER		return ORDER;
<STMT>RESTRICT |
<STMT>EXCLUSIVE		{ BEGIN VARS;
                          yylval.name = yytext;
                          return RESTRICT; }
<STMT>BUNDLE		{ BEGIN BNDL; return BUNDLE; }
<BNDL>\<		return '<';
<BNDL>\>		{ BEGIN VARS; return '>'; }
<STMT>OBSCOUNT		{ BEGIN CTRL; return OBSCOUNT; }
<STMT>JOBID		{ BEGIN CTRL; return JOBID; }
<STMT>JOBIX		{ BEGIN CTRL; return JOBIX; }
<VARS,PHVR,CTRL,ICNS,BNDL>{name}	{ yylval.name = yytext;
                                          return NAME; }
[\.\-_A-Z0-9][\.\-_A-Z0-9]*	{ printf("$ERROR: Unexpected text, stmt %d: %s\n",yylnctr,yytext);
                                  yysynerr = 1; }
.		{ printf("$ERROR: Unexpected char, stmt %d: %s\n",yylnctr,yytext);
                  yysynerr = 1; }
%%
