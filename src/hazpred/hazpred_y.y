/* HAZPRED Parser  */
%{
 /* C Declarations (hazpred_y.y) */

#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "structures.h"
#include "stmtinit.h"
#include "setopt.h"
#include "setparm.h"
#include "setvar.h"
#include "errtext.h"
#include "yyerror.h"
int yylex(void);
%}

/* BISON/YACC declarations */
%union {
	double real;
	char *name;
}

%token <real> NUMBER
%token <name> NAME
%token PROC HAZPRED DATA INHAZ OUT CLIMITS NOCL NOSURV NOHAZ NOLOG NONOTES
%token TIME ID LIBMEM
%token OBSCOUNT HAZCOUNT JOBID JOBIX

%%
/* GRAMMER RULES */
/* program is defined as "PROC + HAZARD + hazardopts + otherstmts" */

program :	hazpredstmt otherstmts
	;

hazpredstmt :	PROC procspec hazpredopts ';'
	;

procspec :	HAZPRED				{ 
  /* Initialize the YACC parser (looks like a linked list) */
  current = last = anchor = stmtinit(15); 
}
	;

hazpredopts :	hazpredopt
	|	hazpredopts hazpredopt
	;

hazpredopt :	DATA '=' dsfield
	|	INHAZ '=' dsfield
	|	OUT '=' dsfield
	|	CLIMITS '=' NUMBER		{ setparm(4,$3); }
	|	NOCL				{ setopt(1); }
	|	NOSURV				{ setopt(2); }
	|	NOHAZ				{ setopt(3); }
	|	NOLOG				{ setopt(4); }
	|	NONOTES				{ setopt(5); }
	|	error				{ errtext(); }
	;

dsfield :	NAME {}
	|	LIBMEM
	;

otherstmts :	otherstmt ';'
	|	otherstmts otherstmt ';'
	;

otherstmt :	timestmt
	|	idstmt
	|	obscountstmt
	|	hazcountstmt
	|	jobidstmt
	|	jobixstmt
	|	error				{ errtext(); }
	|	/* nothing */
	;

timestmt :	TIME NAME			{ setvar(11,$2); }
	;

idstmt :	ID NAME				{ setvar(9,$2); }
	;

obscountstmt :	OBSCOUNT NUMBER			{ setparm(12,$2); }
	;

hazcountstmt :	HAZCOUNT NUMBER			{ setparm(13,$2); }
	;

jobidstmt :	JOBID NAME			{ setvar(14,$2); }
	;

jobixstmt :	JOBIX NAME			{ setvar(15,$2); }
	;

%%
/* Addition C Code */
