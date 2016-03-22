#include <string.h>

/*  A Bison parser, made from hazard_y.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	NUMBER	257
#define	NAME	258
#define	PROC	259
#define	HAZARD	260
#define	DATA	261
#define	OUTHAZ	262
#define	MAXITER	263
#define	CONDITION	264
#define	NOPRINT	265
#define	NOLOG	266
#define	NONOTES	267
#define	NOCOV	268
#define	NOCOR	269
#define	PRINTIT	270
#define	CONSERVE	271
#define	NOCONSERVE	272
#define	QUASINEWTON	273
#define	STEEPEST	274
#define	NUMERIC	275
#define	LIBMEM	276
#define	TIME	277
#define	EVENT	278
#define	RCENSOR	279
#define	ICENSOR	280
#define	PARAMETERS	281
#define	MUE	282
#define	DELTA	283
#define	THALF	284
#define	NU	285
#define	M	286
#define	MUC	287
#define	MUL	288
#define	TAU	289
#define	GAMMA	290
#define	ALPHA	291
#define	ETA	292
#define	WEIBULL	293
#define	FIXDELTA	294
#define	FIXTHALF	295
#define	FIXNU	296
#define	FIXM	297
#define	FIXMNU1	298
#define	FIXTAU	299
#define	FIXGAMMA	300
#define	FIXALPHA	301
#define	FIXETA	302
#define	FIXGE2	303
#define	FIXGAE2	304
#define	STEPWISE	305
#define	SLENTRY	306
#define	SLSTAY	307
#define	MOVE	308
#define	MAXSTEPS	309
#define	BACKWARD	310
#define	FAST	311
#define	NOPRINTS	312
#define	NOPRINTQ	313
#define	ROBUST	314
#define	SEMIROBUST	315
#define	EXCLUDE	316
#define	INCLUDE	317
#define	START	318
#define	EARLY	319
#define	CONSTANT	320
#define	LATE	321
#define	OBSCOUNT	322
#define	LCENSOR	323
#define	WEIGHT	324
#define	MAXVARS	325
#define	ONEWAY	326
#define	ORDER	327
#define	RESTRICT	328
#define	BUNDLE	329
#define	JOBID	330
#define	JOBIX	331
#define	COMMENT	332

#line 2 "hazard_y.y"

  /* C Declarations (hazard_y.y) */
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <yyerror.h>
#include <structures.h>
#include <stmtinit.h>
#include <setopt.h>
#include <setparm.h>
#include <errtext.h>
#include <setvar.h>

int yylex(void);


#line 20 "hazard_y.y"
typedef union{
  double real;
  char *name;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		196
#define	YYFLAG		-32768
#define	YYNTBASE	85

#define YYTRANSLATE(x) ((unsigned)(x) <= 332 ? yytranslate[x] : 128)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    81,     2,     2,    82,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    79,    83,
    80,    84,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     8,    10,    12,    15,    19,    23,    27,    31,
    33,    35,    37,    39,    41,    43,    45,    47,    49,    51,
    53,    55,    57,    59,    62,    66,    68,    70,    72,    74,
    76,    78,    80,    82,    84,    86,    88,    90,    92,    94,
    96,    97,   100,   103,   106,   111,   113,   115,   118,   121,
   124,   126,   129,   133,   137,   141,   145,   149,   153,   157,
   161,   165,   169,   173,   175,   177,   179,   181,   183,   185,
   187,   189,   191,   193,   195,   197,   200,   201,   204,   208,
   212,   216,   220,   224,   226,   228,   230,   232,   234,   236,
   238,   240,   243,   246,   249,   252,   254,   256,   258,   260,
   262,   266,   270,   272,   273,   276,   277,   280,   282,   285,
   289,   293,   295,   297,   299,   302,   304,   307,   310,   312,
   316,   318,   321
};

static const short yyrhs[] = {    86,
    91,     0,     5,    87,    88,    79,     0,     6,     0,    89,
     0,    88,    89,     0,     7,    80,    90,     0,     8,    80,
    90,     0,     9,    80,     3,     0,    10,    80,     3,     0,
    14,     0,    15,     0,    16,     0,    17,     0,    18,     0,
    19,     0,    20,     0,    21,     0,    11,     0,    12,     0,
    13,     0,     1,     0,     4,     0,    22,     0,    92,    79,
     0,    91,    92,    79,     0,    93,     0,    94,     0,    95,
     0,    96,     0,    99,     0,   100,     0,   101,     0,   104,
     0,   110,     0,   120,     0,   122,     0,   107,     0,   108,
     0,   109,     0,     1,     0,     0,    23,     4,     0,    24,
     4,     0,    25,     4,     0,    26,    97,    80,    98,     0,
     4,     0,     4,     0,    69,     4,     0,    70,     4,     0,
    27,   102,     0,   103,     0,   102,   103,     0,    28,    80,
     3,     0,    29,    80,     3,     0,    30,    80,     3,     0,
    31,    80,     3,     0,    32,    80,     3,     0,    33,    80,
     3,     0,    34,    80,     3,     0,    35,    80,     3,     0,
    36,    80,     3,     0,    37,    80,     3,     0,    38,    80,
     3,     0,    39,     0,    40,     0,    41,     0,    42,     0,
    43,     0,    44,     0,    45,     0,    46,     0,    47,     0,
    48,     0,    49,     0,    50,     0,    51,   105,     0,     0,
   105,   106,     0,    52,    80,     3,     0,    53,    80,     3,
     0,    54,    80,     3,     0,    55,    80,     3,     0,    71,
    80,     3,     0,    51,     0,    56,     0,    57,     0,    58,
     0,    59,     0,    60,     0,    61,     0,    72,     0,    68,
     3,     0,    76,     4,     0,    77,     4,     0,   111,   113,
     0,   112,     0,    65,     0,    66,     0,    67,     0,   114,
     0,   113,    81,   114,     0,   115,   116,   117,     0,     4,
     0,     0,    80,     3,     0,     0,    82,   118,     0,   119,
     0,   118,   119,     0,    54,    80,     3,     0,    73,    80,
     3,     0,    62,     0,    63,     0,    64,     0,   121,   126,
     0,    74,     0,   123,   126,     0,   124,   125,     0,    75,
     0,    83,     4,    84,     0,   127,     0,   126,   127,     0,
     4,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    41,    44,    49,    56,    57,    61,    62,    63,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    80,    81,    84,    85,    88,    89,    90,    91,    92,
    93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
   103,   106,   109,   112,   115,   118,   121,   124,   127,   130,
   133,   134,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
   155,   156,   157,   158,   159,   162,   165,   166,   169,   170,
   171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
   181,   184,   187,   190,   193,   196,   201,   202,   203,   206,
   207,   210,   213,   216,   217,   220,   221,   224,   225,   228,
   229,   230,   231,   232,   235,   238,   241,   244,   247,   250,
   253,   254,   257
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","NUMBER",
"NAME","PROC","HAZARD","DATA","OUTHAZ","MAXITER","CONDITION","NOPRINT","NOLOG",
"NONOTES","NOCOV","NOCOR","PRINTIT","CONSERVE","NOCONSERVE","QUASINEWTON","STEEPEST",
"NUMERIC","LIBMEM","TIME","EVENT","RCENSOR","ICENSOR","PARAMETERS","MUE","DELTA",
"THALF","NU","M","MUC","MUL","TAU","GAMMA","ALPHA","ETA","WEIBULL","FIXDELTA",
"FIXTHALF","FIXNU","FIXM","FIXMNU1","FIXTAU","FIXGAMMA","FIXALPHA","FIXETA",
"FIXGE2","FIXGAE2","STEPWISE","SLENTRY","SLSTAY","MOVE","MAXSTEPS","BACKWARD",
"FAST","NOPRINTS","NOPRINTQ","ROBUST","SEMIROBUST","EXCLUDE","INCLUDE","START",
"EARLY","CONSTANT","LATE","OBSCOUNT","LCENSOR","WEIGHT","MAXVARS","ONEWAY","ORDER",
"RESTRICT","BUNDLE","JOBID","JOBIX","COMMENT","';'","'='","','","'/'","'<'",
"'>'","program","hazardstmt","procspec","hazardopts","hazardopt","dsfield","otherstmts",
"otherstmt","timestmt","eventstmt","rcensorstmt","icensorstmt","icensorvar",
"icensortime","lcensorstmt","weightstmt","parmstmt","parmsopts","parmsopt","stepwisestmt",
"stepwiseopts","stepwiseopt","obscountstmt","jobidstmt","jobixstmt","phasestmts",
"phasecmd","phasename","phasevaropts","phasevaropt","phasevar","phaseval","phaseoptspec",
"phaseopts","phaseopt","restrictstmts","restrictcmd","bundlestmt","bundlespec",
"bundlecmd","bundlename","varlist","varname", NULL
};
#endif

static const short yyr1[] = {     0,
    85,    86,    87,    88,    88,    89,    89,    89,    89,    89,
    89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
    89,    90,    90,    91,    91,    92,    92,    92,    92,    92,
    92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
    92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
   102,   102,   103,   103,   103,   103,   103,   103,   103,   103,
   103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
   103,   103,   103,   103,   103,   104,   105,   105,   106,   106,
   106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
   106,   107,   108,   109,   110,   111,   112,   112,   112,   113,
   113,   114,   115,   116,   116,   117,   117,   118,   118,   119,
   119,   119,   119,   119,   120,   121,   122,   123,   124,   125,
   126,   126,   127
};

static const short yyr2[] = {     0,
     2,     4,     1,     1,     2,     3,     3,     3,     3,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     2,     3,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     0,     2,     2,     2,     4,     1,     1,     2,     2,     2,
     1,     2,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     0,     2,     3,     3,
     3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
     1,     2,     2,     2,     2,     1,     1,     1,     1,     1,
     3,     3,     1,     0,     2,     0,     2,     1,     2,     3,
     3,     1,     1,     1,     2,     1,     2,     2,     1,     3,
     1,     2,     1
};

static const short yydefact[] = {     0,
     0,     0,     3,     0,    40,     0,     0,     0,     0,     0,
    77,    97,    98,    99,     0,     0,     0,   116,   119,     0,
     0,     0,     0,    26,    27,    28,    29,    30,    31,    32,
    33,    37,    38,    39,    34,     0,    96,    35,     0,    36,
     0,     0,    21,     0,     0,     0,     0,    18,    19,    20,
    10,    11,    12,    13,    14,    15,    16,    17,     0,     4,
    42,    43,    44,    46,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    64,    65,    66,    67,
    68,    69,    70,    71,    72,    73,    74,    75,    50,    51,
    76,    92,    48,    49,    93,    94,     0,    24,   103,    95,
   100,   104,   123,   115,   121,   117,     0,   118,     0,     0,
     0,     0,     2,     5,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    52,    84,     0,     0,
     0,     0,    85,    86,    87,    88,    89,    90,     0,    91,
    78,    25,     0,     0,   106,   122,     0,    22,    23,     6,
     7,     8,     9,    47,    45,    53,    54,    55,    56,    57,
    58,    59,    60,    61,    62,    63,     0,     0,     0,     0,
     0,   101,   105,     0,   102,   120,    79,    80,    81,    82,
    83,     0,   112,   113,   114,     0,   107,   108,     0,     0,
   109,   110,   111,     0,     0,     0
};

static const short yydefgoto[] = {   194,
     2,     4,    59,    60,   150,    22,    23,    24,    25,    26,
    27,    65,   155,    28,    29,    30,    89,    90,    31,    91,
   141,    32,    33,    34,    35,    36,    37,   100,   101,   102,
   145,   175,   187,   188,    38,    39,    40,    41,    42,   108,
   104,   105
};

static const short yypact[] = {    -1,
     2,    20,-32768,   123,-32768,     3,     5,     6,     7,    73,
-32768,-32768,-32768,-32768,     9,    10,    11,-32768,-32768,    12,
    13,     0,   -73,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    14,-32768,-32768,    15,-32768,
    15,   -70,-32768,   -32,   -31,   -30,   -17,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    21,-32768,
-32768,-32768,-32768,-32768,   -16,     4,    18,    45,    46,    47,
    48,    49,    66,    67,    68,    69,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    73,-32768,
     1,-32768,-32768,-32768,-32768,-32768,    71,-32768,-32768,    -3,
-32768,    72,-32768,    15,-32768,    15,    76,-32768,    -2,    -2,
    78,    79,-32768,-32768,    87,    80,    89,    90,   148,   153,
   154,   155,   156,   157,   158,   159,-32768,-32768,    83,    85,
    86,    88,-32768,-32768,-32768,-32768,-32768,-32768,    92,-32768,
-32768,-32768,    14,   164,    93,-32768,    94,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   166,   167,   168,   170,
   171,-32768,-32768,    91,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    96,-32768,-32768,-32768,    97,    91,-32768,   176,   177,
-32768,-32768,-32768,   181,   182,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,   124,    74,-32768,   163,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    98,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    43,-32768,
-32768,-32768,-32768,     8,-32768,-32768,-32768,-32768,-32768,-32768,
   147,  -101
};


#define	YYLAST		195


static const short yytable[] = {    -1,
     5,   148,   146,     1,   146,    98,    61,     3,    62,    63,
    64,    92,   107,    93,    94,    95,    96,    99,   103,   149,
     5,    43,     6,     7,     8,     9,    10,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,     6,     7,     8,     9,    10,   109,   110,   111,
    11,   128,   129,   130,   131,   132,   133,   134,   135,   136,
   137,   138,   112,   115,    12,    13,    14,    15,    16,    17,
    11,   139,   140,    18,    19,    20,    21,   143,   -41,   147,
   152,   153,   156,   116,    12,    13,    14,    15,    16,    17,
   154,   157,   158,    18,    19,    20,    21,   117,   -41,   113,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
    86,    87,    88,    43,   118,   119,   120,   121,   122,    44,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,    57,    58,   182,   123,   124,   125,   126,   142,
   159,   144,   183,   184,   185,   160,   161,   162,   163,   164,
   165,   166,   167,   186,   168,   169,   173,   170,   177,   178,
   179,   171,   180,   181,   174,   189,   190,   176,   192,   193,
   195,   196,   114,   151,    97,   172,   127,   106,     0,     0,
     0,     0,     0,     0,   191
};

static const short yycheck[] = {     0,
     1,     4,   104,     5,   106,    79,     4,     6,     4,     4,
     4,     3,    83,     4,     4,     4,     4,     4,     4,    22,
     1,     1,    23,    24,    25,    26,    27,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    21,    23,    24,    25,    26,    27,    80,    80,    80,
    51,    51,    52,    53,    54,    55,    56,    57,    58,    59,
    60,    61,    80,    80,    65,    66,    67,    68,    69,    70,
    51,    71,    72,    74,    75,    76,    77,    81,    79,     4,
     3,     3,     3,    80,    65,    66,    67,    68,    69,    70,
     4,     3,     3,    74,    75,    76,    77,    80,    79,    79,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,     1,    80,    80,    80,    80,    80,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    20,    21,    54,    80,    80,    80,    80,    79,
     3,    80,    62,    63,    64,     3,     3,     3,     3,     3,
     3,     3,    80,    73,    80,    80,     3,    80,     3,     3,
     3,    80,     3,     3,    82,    80,    80,    84,     3,     3,
     0,     0,    59,   110,    22,   143,    89,    41,    -1,    -1,
    -1,    -1,    -1,    -1,   187
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   aint with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/local/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 3:
#line 49 "hazard_y.y"
{ 
  /* Initialize the YACC parser (looks like a linked list) */
  current = last = anchor = stmtinit(45); 
;
    break;}
case 7:
#line 62 "hazard_y.y"
{ setopt(63); ;
    break;}
case 8:
#line 63 "hazard_y.y"
{ setparm(3,yyvsp[0].real); ;
    break;}
case 9:
#line 64 "hazard_y.y"
{ setparm(4,yyvsp[0].real); ;
    break;}
case 10:
#line 65 "hazard_y.y"
{ setopt(1); ;
    break;}
case 11:
#line 66 "hazard_y.y"
{ setopt(2); ;
    break;}
case 12:
#line 67 "hazard_y.y"
{ setopt(3); ;
    break;}
case 13:
#line 68 "hazard_y.y"
{ setopt(4); ;
    break;}
case 14:
#line 69 "hazard_y.y"
{ setopt(5); ;
    break;}
case 15:
#line 70 "hazard_y.y"
{ setopt(26); ;
    break;}
case 16:
#line 71 "hazard_y.y"
{ setopt(27); ;
    break;}
case 17:
#line 72 "hazard_y.y"
{ setopt(28); ;
    break;}
case 18:
#line 73 "hazard_y.y"
{ setopt(29); ;
    break;}
case 19:
#line 74 "hazard_y.y"
{ setopt(40); ;
    break;}
case 20:
#line 75 "hazard_y.y"
{ setopt(41); ;
    break;}
case 21:
#line 76 "hazard_y.y"
{ errtext(); ;
    break;}
case 22:
#line 80 "hazard_y.y"
{;
    break;}
case 40:
#line 102 "hazard_y.y"
{ errtext(); ;
    break;}
case 42:
#line 106 "hazard_y.y"
{ setvar(11,yyvsp[0].name); ;
    break;}
case 43:
#line 109 "hazard_y.y"
{ setvar(12,yyvsp[0].name); ;
    break;}
case 44:
#line 112 "hazard_y.y"
{ setvar(13,yyvsp[0].name); ;
    break;}
case 46:
#line 118 "hazard_y.y"
{ setvar(14,yyvsp[0].name); ;
    break;}
case 47:
#line 121 "hazard_y.y"
{ setvar(15,yyvsp[0].name); ;
    break;}
case 48:
#line 124 "hazard_y.y"
{ setvar(36,yyvsp[0].name); ;
    break;}
case 49:
#line 127 "hazard_y.y"
{ setvar(37,yyvsp[0].name); ;
    break;}
case 53:
#line 137 "hazard_y.y"
{ setparm(16,yyvsp[0].real); ;
    break;}
case 54:
#line 138 "hazard_y.y"
{ setparm(17,yyvsp[0].real); ;
    break;}
case 55:
#line 139 "hazard_y.y"
{ setparm(18,yyvsp[0].real); ;
    break;}
case 56:
#line 140 "hazard_y.y"
{ setparm(19,yyvsp[0].real); ;
    break;}
case 57:
#line 141 "hazard_y.y"
{ setparm(20,yyvsp[0].real); ;
    break;}
case 58:
#line 142 "hazard_y.y"
{ setparm(21,yyvsp[0].real); ;
    break;}
case 59:
#line 143 "hazard_y.y"
{ setparm(22,yyvsp[0].real); ;
    break;}
case 60:
#line 144 "hazard_y.y"
{ setparm(23,yyvsp[0].real); ;
    break;}
case 61:
#line 145 "hazard_y.y"
{ setparm(24,yyvsp[0].real); ;
    break;}
case 62:
#line 146 "hazard_y.y"
{ setparm(25,yyvsp[0].real); ;
    break;}
case 63:
#line 147 "hazard_y.y"
{ setparm(26,yyvsp[0].real); ;
    break;}
case 64:
#line 148 "hazard_y.y"
{ setopt(6); ;
    break;}
case 65:
#line 149 "hazard_y.y"
{ setopt(7); ;
    break;}
case 66:
#line 150 "hazard_y.y"
{ setopt(8); ;
    break;}
case 67:
#line 151 "hazard_y.y"
{ setopt(9); ;
    break;}
case 68:
#line 152 "hazard_y.y"
{ setopt(10); ;
    break;}
case 69:
#line 153 "hazard_y.y"
{ setopt(11); ;
    break;}
case 70:
#line 154 "hazard_y.y"
{ setopt(12); ;
    break;}
case 71:
#line 155 "hazard_y.y"
{ setopt(13); ;
    break;}
case 72:
#line 156 "hazard_y.y"
{ setopt(14); ;
    break;}
case 73:
#line 157 "hazard_y.y"
{ setopt(15); ;
    break;}
case 74:
#line 158 "hazard_y.y"
{ setopt(16); ;
    break;}
case 75:
#line 159 "hazard_y.y"
{ setopt(17); ;
    break;}
case 76:
#line 162 "hazard_y.y"
{ setopt(33); ;
    break;}
case 79:
#line 169 "hazard_y.y"
{ setparm(31,yyvsp[0].real); ;
    break;}
case 80:
#line 170 "hazard_y.y"
{ setparm(32,yyvsp[0].real); ;
    break;}
case 81:
#line 171 "hazard_y.y"
{ setparm(33,yyvsp[0].real); ;
    break;}
case 82:
#line 172 "hazard_y.y"
{ setparm(34,yyvsp[0].real); ;
    break;}
case 83:
#line 173 "hazard_y.y"
{ setparm(35,yyvsp[0].real); ;
    break;}
case 84:
#line 174 "hazard_y.y"
{ setopt(21); ;
    break;}
case 85:
#line 175 "hazard_y.y"
{ setopt(22); ;
    break;}
case 86:
#line 176 "hazard_y.y"
{ setopt(23); ;
    break;}
case 87:
#line 177 "hazard_y.y"
{ setopt(24); ;
    break;}
case 88:
#line 178 "hazard_y.y"
{ setopt(25); ;
    break;}
case 89:
#line 179 "hazard_y.y"
{ setopt(31); ;
    break;}
case 90:
#line 180 "hazard_y.y"
{ setopt(32); ;
    break;}
case 91:
#line 181 "hazard_y.y"
{ setopt(34); ;
    break;}
case 92:
#line 184 "hazard_y.y"
{ setparm(43,yyvsp[0].real); ;
    break;}
case 93:
#line 187 "hazard_y.y"
{ setvar(44,yyvsp[0].name); ;
    break;}
case 94:
#line 190 "hazard_y.y"
{ setvar(45,yyvsp[0].name); ;
    break;}
case 95:
#line 193 "hazard_y.y"
{ current = anchor; ;
    break;}
case 96:
#line 196 "hazard_y.y"
{ last = current = 
						    (last->next = stmtinit(1));
                         ;
    break;}
case 103:
#line 213 "hazard_y.y"
{ current = last = (last->next = stmtinit(10)); ;
    break;}
case 105:
#line 217 "hazard_y.y"
{ setparm(2,yyvsp[0].real); ;
    break;}
case 110:
#line 228 "hazard_y.y"
{ setparm(3,yyvsp[0].real); ;
    break;}
case 111:
#line 229 "hazard_y.y"
{ setparm(4,yyvsp[0].real); ;
    break;}
case 112:
#line 230 "hazard_y.y"
{ setopt(1); ;
    break;}
case 113:
#line 231 "hazard_y.y"
{ setopt(2); ;
    break;}
case 114:
#line 232 "hazard_y.y"
{ setopt(3); ;
    break;}
case 115:
#line 235 "hazard_y.y"
{ current = anchor; ;
    break;}
case 116:
#line 238 "hazard_y.y"
{ current = last = (last->next = stmtinit(1)); ;
    break;}
case 117:
#line 241 "hazard_y.y"
{ current = anchor; ;
    break;}
case 119:
#line 247 "hazard_y.y"
{ current = last = (last->next = stmtinit(5)); ;
    break;}
case 120:
#line 250 "hazard_y.y"
{ setvar(2,yyvsp[-1].name); ;
    break;}
case 123:
#line 257 "hazard_y.y"
{ current = last = (last->next = stmtinit(2)); setvar(2,yyvsp[0].name); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/local/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 259 "hazard_y.y"

/* Addition C Code */
