/* HAZARD Parser  */
%{
  /* C Declarations (hazard_y.y) */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <yyerror.h>
#include <structures.h>
#include <stmtinit.h>
#include <setopt.h>
#include <setparm.h>
#include <errtext.h>
#include <setvar.h>

int yylex(void);

%}

/* BISON/YACC declarations */
%union{
  double real;
  char *name;
}

%token <real> NUMBER
%token <name> NAME
%token PROC HAZARD DATA OUTHAZ MAXITER CONDITION NOPRINT NOLOG NONOTES
%token NOCOV NOCOR PRINTIT CONSERVE NOCONSERVE QUASINEWTON STEEPEST NUMERIC
%token LIBMEM TIME EVENT RCENSOR ICENSOR PARAMETERS MUE DELTA THALF NU
%token M MUC MUL TAU GAMMA ALPHA ETA WEIBULL FIXDELTA FIXTHALF FIXNU FIXM
%token FIXMNU1 FIXTAU FIXGAMMA FIXALPHA FIXETA FIXGE2 FIXGAE2
%token STEPWISE SLENTRY SLSTAY MOVE MAXSTEPS BACKWARD FAST NOPRINTS NOPRINTQ
%token ROBUST SEMIROBUST EXCLUDE INCLUDE START EARLY CONSTANT LATE OBSCOUNT
%token LCENSOR WEIGHT MAXVARS ONEWAY ORDER RESTRICT BUNDLE
%token JOBID JOBIX COMMENT

%%
/* GRAMMER RULES */
/* program is defined as "PROC + HAZARD + hazardopts + otherstmts" */

program :	hazardstmt otherstmts
;

hazardstmt : 	PROC procspec hazardopts ';'
;

  /* the procspec is probably initilized this way
     to attempt to combine HAZARD and HAZPRED */
procspec :	HAZARD				{ 
  /* Initialize the YACC parser (looks like a linked list) */
  current = last = anchor = stmtinit(45); 
}
;

  /* hazardopts: can be any number of hazard options */
hazardopts :	hazardopt
|	hazardopts hazardopt
;

  /* valid hazard options */
hazardopt :	DATA '=' dsfield
|	OUTHAZ '=' dsfield		{ setopt(63); }
|	MAXITER '=' NUMBER		{ setparm(3,$3); }
|	CONDITION '=' NUMBER		{ setparm(4,$3); }
|	NOCOV				{ setopt(1); }
|	NOCOR				{ setopt(2); }
|	PRINTIT				{ setopt(3); }
|	CONSERVE			{ setopt(4); }
|	NOCONSERVE			{ setopt(5); }
|	QUASINEWTON			{ setopt(26); }
|	STEEPEST			{ setopt(27); }
|	NUMERIC				{ setopt(28); }
|	NOPRINT				{ setopt(29); }
|	NOLOG				{ setopt(40); }
|	NONOTES				{ setopt(41); }
|	error				{ errtext(); }
;

  /* */
dsfield :	NAME {}
|	LIBMEM
;

otherstmts :	otherstmt ';'
|	otherstmts otherstmt ';'
;

otherstmt :	timestmt
|	eventstmt
|	rcensorstmt
|	icensorstmt
|	lcensorstmt
|	weightstmt
|	parmstmt
|	stepwisestmt
|	phasestmts
|	restrictstmts
|	bundlestmt
|	obscountstmt
|	jobidstmt
|	jobixstmt
|	error 				{ errtext(); }
|	/* nothing */
	;

timestmt :	TIME NAME			{ setvar(11,$2); }
;

eventstmt :	EVENT NAME			{ setvar(12,$2); }
;

rcensorstmt :	RCENSOR NAME			{ setvar(13,$2); }
;

icensorstmt :	ICENSOR icensorvar '=' icensortime
;

icensorvar:	NAME				{ setvar(14,$1); }
;

icensortime:	NAME				{ setvar(15,$1); }
;

lcensorstmt :	LCENSOR NAME			{ setvar(36,$2); }
;

weightstmt :	WEIGHT NAME			{ setvar(37,$2); }
;

parmstmt :	PARAMETERS parmsopts
;

parmsopts :	parmsopt
|	parmsopts parmsopt
;

parmsopt :	MUE '=' NUMBER			{ setparm(16,$3); }
|	DELTA '=' NUMBER		{ setparm(17,$3); }
|	THALF '=' NUMBER		{ setparm(18,$3); }
|	NU '=' NUMBER			{ setparm(19,$3); }
|	M '=' NUMBER			{ setparm(20,$3); }
|	MUC '=' NUMBER			{ setparm(21,$3); }
|	MUL '=' NUMBER			{ setparm(22,$3); }
|	TAU '=' NUMBER			{ setparm(23,$3); }
|	GAMMA '=' NUMBER		{ setparm(24,$3); }
|	ALPHA '=' NUMBER		{ setparm(25,$3); }
|	ETA '=' NUMBER			{ setparm(26,$3); }
|	WEIBULL				{ setopt(6); }
|	FIXDELTA			{ setopt(7); }
|	FIXTHALF			{ setopt(8); }
|	FIXNU				{ setopt(9); }
|	FIXM				{ setopt(10); }
|	FIXMNU1				{ setopt(11); }
|	FIXTAU				{ setopt(12); }
|	FIXGAMMA			{ setopt(13); }
|	FIXALPHA			{ setopt(14); }
|	FIXETA				{ setopt(15); }
|	FIXGE2				{ setopt(16); }
|	FIXGAE2				{ setopt(17); }
;

stepwisestmt :	STEPWISE stepwiseopts		{ setopt(33); }
;

stepwiseopts :	/*nothing*/
|	stepwiseopts stepwiseopt
;

stepwiseopt :	SLENTRY '=' NUMBER		{ setparm(31,$3); }
|	SLSTAY '=' NUMBER		{ setparm(32,$3); }
|	MOVE '=' NUMBER			{ setparm(33,$3); }
|	MAXSTEPS '=' NUMBER		{ setparm(34,$3); }
|	MAXVARS '=' NUMBER		{ setparm(35,$3); }
|	STEPWISE			{ setopt(21); }
|	BACKWARD			{ setopt(22); }
|	FAST				{ setopt(23); }
|	NOPRINTS			{ setopt(24); }
|	NOPRINTQ			{ setopt(25); }
|	ROBUST				{ setopt(31); }
|	SEMIROBUST			{ setopt(32); }
|	ONEWAY				{ setopt(34); }
;

obscountstmt :	OBSCOUNT NUMBER			{ setparm(43,$2); }
;

jobidstmt :	JOBID NAME			{ setvar(44,$2); }
;

jobixstmt :	JOBIX NAME			{ setvar(45,$2); }
;

phasestmts :	phasecmd phasevaropts		{ current = anchor; }
;

phasecmd :	phasename			{ last = current = 
						    (last->next = stmtinit(1));
                         }
;

phasename :	EARLY
|	CONSTANT
|	LATE
;

phasevaropts :	phasevaropt 
|	phasevaropts ',' phasevaropt
;

phasevaropt :	phasevar phaseval phaseoptspec
;

phasevar :	NAME				{ current = last = (last->next = stmtinit(10)); }
;

phaseval :	/*nothing*/
|	'=' NUMBER			{ setparm(2,$2); }
;

phaseoptspec :	/*nothing*/
|	'/' phaseopts
;

phaseopts :	phaseopt
|	phaseopts phaseopt
;

phaseopt :	MOVE '=' NUMBER			{ setparm(3,$3); }
|	ORDER '=' NUMBER		{ setparm(4,$3); }
|	EXCLUDE				{ setopt(1); }
|	INCLUDE				{ setopt(2); }
|	START				{ setopt(3); }
;

restrictstmts :	restrictcmd varlist		{ current = anchor; }
;

restrictcmd :	RESTRICT			{ current = last = (last->next = stmtinit(1)); }
;

bundlestmt :	bundlespec varlist		{ current = anchor; }
;

bundlespec :	bundlecmd bundlename
;

bundlecmd :	BUNDLE				{ current = last = (last->next = stmtinit(5)); }
;

bundlename :	'<' NAME '>'			{ setvar(2,$2); }
;

varlist :	varname
|	varlist varname
;

varname :	NAME				{ current = last = (last->next = stmtinit(2)); setvar(2,$1); }
;
%%
/* Addition C Code */
