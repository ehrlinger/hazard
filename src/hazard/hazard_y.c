/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NUMBER = 258,
     NAME = 259,
     PROC = 260,
     HAZARD = 261,
     DATA = 262,
     OUTHAZ = 263,
     MAXITER = 264,
     CONDITION = 265,
     NOPRINT = 266,
     NOLOG = 267,
     NONOTES = 268,
     NOCOV = 269,
     NOCOR = 270,
     PRINTIT = 271,
     CONSERVE = 272,
     NOCONSERVE = 273,
     QUASINEWTON = 274,
     STEEPEST = 275,
     NUMERIC = 276,
     LIBMEM = 277,
     TIME = 278,
     EVENT = 279,
     RCENSOR = 280,
     ICENSOR = 281,
     PARAMETERS = 282,
     MUE = 283,
     DELTA = 284,
     THALF = 285,
     NU = 286,
     M = 287,
     MUC = 288,
     MUL = 289,
     TAU = 290,
     GAMMA = 291,
     ALPHA = 292,
     ETA = 293,
     WEIBULL = 294,
     FIXDELTA = 295,
     FIXTHALF = 296,
     FIXNU = 297,
     FIXM = 298,
     FIXMNU1 = 299,
     FIXTAU = 300,
     FIXGAMMA = 301,
     FIXALPHA = 302,
     FIXETA = 303,
     FIXGE2 = 304,
     FIXGAE2 = 305,
     STEPWISE = 306,
     SLENTRY = 307,
     SLSTAY = 308,
     MOVE = 309,
     MAXSTEPS = 310,
     BACKWARD = 311,
     FAST = 312,
     NOPRINTS = 313,
     NOPRINTQ = 314,
     ROBUST = 315,
     SEMIROBUST = 316,
     EXCLUDE = 317,
     INCLUDE = 318,
     START = 319,
     EARLY = 320,
     CONSTANT = 321,
     LATE = 322,
     OBSCOUNT = 323,
     LCENSOR = 324,
     WEIGHT = 325,
     MAXVARS = 326,
     ONEWAY = 327,
     ORDER = 328,
     RESTRICT = 329,
     BUNDLE = 330,
     JOBID = 331,
     JOBIX = 332,
     COMMENT = 333
   };
#endif
#define NUMBER 258
#define NAME 259
#define PROC 260
#define HAZARD 261
#define DATA 262
#define OUTHAZ 263
#define MAXITER 264
#define CONDITION 265
#define NOPRINT 266
#define NOLOG 267
#define NONOTES 268
#define NOCOV 269
#define NOCOR 270
#define PRINTIT 271
#define CONSERVE 272
#define NOCONSERVE 273
#define QUASINEWTON 274
#define STEEPEST 275
#define NUMERIC 276
#define LIBMEM 277
#define TIME 278
#define EVENT 279
#define RCENSOR 280
#define ICENSOR 281
#define PARAMETERS 282
#define MUE 283
#define DELTA 284
#define THALF 285
#define NU 286
#define M 287
#define MUC 288
#define MUL 289
#define TAU 290
#define GAMMA 291
#define ALPHA 292
#define ETA 293
#define WEIBULL 294
#define FIXDELTA 295
#define FIXTHALF 296
#define FIXNU 297
#define FIXM 298
#define FIXMNU1 299
#define FIXTAU 300
#define FIXGAMMA 301
#define FIXALPHA 302
#define FIXETA 303
#define FIXGE2 304
#define FIXGAE2 305
#define STEPWISE 306
#define SLENTRY 307
#define SLSTAY 308
#define MOVE 309
#define MAXSTEPS 310
#define BACKWARD 311
#define FAST 312
#define NOPRINTS 313
#define NOPRINTQ 314
#define ROBUST 315
#define SEMIROBUST 316
#define EXCLUDE 317
#define INCLUDE 318
#define START 319
#define EARLY 320
#define CONSTANT 321
#define LATE 322
#define OBSCOUNT 323
#define LCENSOR 324
#define WEIGHT 325
#define MAXVARS 326
#define ONEWAY 327
#define ORDER 328
#define RESTRICT 329
#define BUNDLE 330
#define JOBID 331
#define JOBIX 332
#define COMMENT 333




/* Copy the first part of user declarations.  */
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



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 20 "hazard_y.y"
typedef union YYSTYPE {
  double real;
  char *name;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 253 "hazard_y.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 265 "hazard_y.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   198

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  85
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  44
/* YYNRULES -- Number of rules. */
#define YYNRULES  124
/* YYNRULES -- Number of states. */
#define YYNSTATES  196

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   333

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    81,     2,     2,    82,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    79,
      83,    80,    84,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     6,    11,    13,    15,    18,    22,    26,
      30,    34,    36,    38,    40,    42,    44,    46,    48,    50,
      52,    54,    56,    58,    60,    62,    65,    69,    71,    73,
      75,    77,    79,    81,    83,    85,    87,    89,    91,    93,
      95,    97,    99,   100,   103,   106,   109,   114,   116,   118,
     121,   124,   127,   129,   132,   136,   140,   144,   148,   152,
     156,   160,   164,   168,   172,   176,   178,   180,   182,   184,
     186,   188,   190,   192,   194,   196,   198,   200,   203,   204,
     207,   211,   215,   219,   223,   227,   229,   231,   233,   235,
     237,   239,   241,   243,   246,   249,   252,   255,   257,   259,
     261,   263,   265,   269,   273,   275,   276,   279,   280,   283,
     285,   288,   292,   296,   298,   300,   302,   305,   307,   310,
     313,   315,   319,   321,   324
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
      86,     0,    -1,    87,    92,    -1,     5,    88,    89,    79,
      -1,     6,    -1,    90,    -1,    89,    90,    -1,     7,    80,
      91,    -1,     8,    80,    91,    -1,     9,    80,     3,    -1,
      10,    80,     3,    -1,    14,    -1,    15,    -1,    16,    -1,
      17,    -1,    18,    -1,    19,    -1,    20,    -1,    21,    -1,
      11,    -1,    12,    -1,    13,    -1,     1,    -1,     4,    -1,
      22,    -1,    93,    79,    -1,    92,    93,    79,    -1,    94,
      -1,    95,    -1,    96,    -1,    97,    -1,   100,    -1,   101,
      -1,   102,    -1,   105,    -1,   111,    -1,   121,    -1,   123,
      -1,   108,    -1,   109,    -1,   110,    -1,     1,    -1,    -1,
      23,     4,    -1,    24,     4,    -1,    25,     4,    -1,    26,
      98,    80,    99,    -1,     4,    -1,     4,    -1,    69,     4,
      -1,    70,     4,    -1,    27,   103,    -1,   104,    -1,   103,
     104,    -1,    28,    80,     3,    -1,    29,    80,     3,    -1,
      30,    80,     3,    -1,    31,    80,     3,    -1,    32,    80,
       3,    -1,    33,    80,     3,    -1,    34,    80,     3,    -1,
      35,    80,     3,    -1,    36,    80,     3,    -1,    37,    80,
       3,    -1,    38,    80,     3,    -1,    39,    -1,    40,    -1,
      41,    -1,    42,    -1,    43,    -1,    44,    -1,    45,    -1,
      46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,
      51,   106,    -1,    -1,   106,   107,    -1,    52,    80,     3,
      -1,    53,    80,     3,    -1,    54,    80,     3,    -1,    55,
      80,     3,    -1,    71,    80,     3,    -1,    51,    -1,    56,
      -1,    57,    -1,    58,    -1,    59,    -1,    60,    -1,    61,
      -1,    72,    -1,    68,     3,    -1,    76,     4,    -1,    77,
       4,    -1,   112,   114,    -1,   113,    -1,    65,    -1,    66,
      -1,    67,    -1,   115,    -1,   114,    81,   115,    -1,   116,
     117,   118,    -1,     4,    -1,    -1,    80,     3,    -1,    -1,
      82,   119,    -1,   120,    -1,   119,   120,    -1,    54,    80,
       3,    -1,    73,    80,     3,    -1,    62,    -1,    63,    -1,
      64,    -1,   122,   127,    -1,    74,    -1,   124,   127,    -1,
     125,   126,    -1,    75,    -1,    83,     4,    84,    -1,   128,
      -1,   127,   128,    -1,     4,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,    41,    41,    44,    49,    56,    57,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    80,    81,    84,    85,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   106,   109,   112,   115,   118,   121,   124,
     127,   130,   133,   134,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   162,   165,   166,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   184,   187,   190,   193,   196,   201,   202,
     203,   206,   207,   210,   213,   216,   217,   220,   221,   224,
     225,   228,   229,   230,   231,   232,   235,   238,   241,   244,
     247,   250,   253,   254,   257
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUMBER", "NAME", "PROC", "HAZARD", "DATA", 
  "OUTHAZ", "MAXITER", "CONDITION", "NOPRINT", "NOLOG", "NONOTES", 
  "NOCOV", "NOCOR", "PRINTIT", "CONSERVE", "NOCONSERVE", "QUASINEWTON", 
  "STEEPEST", "NUMERIC", "LIBMEM", "TIME", "EVENT", "RCENSOR", "ICENSOR", 
  "PARAMETERS", "MUE", "DELTA", "THALF", "NU", "M", "MUC", "MUL", "TAU", 
  "GAMMA", "ALPHA", "ETA", "WEIBULL", "FIXDELTA", "FIXTHALF", "FIXNU", 
  "FIXM", "FIXMNU1", "FIXTAU", "FIXGAMMA", "FIXALPHA", "FIXETA", "FIXGE2", 
  "FIXGAE2", "STEPWISE", "SLENTRY", "SLSTAY", "MOVE", "MAXSTEPS", 
  "BACKWARD", "FAST", "NOPRINTS", "NOPRINTQ", "ROBUST", "SEMIROBUST", 
  "EXCLUDE", "INCLUDE", "START", "EARLY", "CONSTANT", "LATE", "OBSCOUNT", 
  "LCENSOR", "WEIGHT", "MAXVARS", "ONEWAY", "ORDER", "RESTRICT", "BUNDLE", 
  "JOBID", "JOBIX", "COMMENT", "';'", "'='", "','", "'/'", "'<'", "'>'", 
  "$accept", "program", "hazardstmt", "procspec", "hazardopts", 
  "hazardopt", "dsfield", "otherstmts", "otherstmt", "timestmt", 
  "eventstmt", "rcensorstmt", "icensorstmt", "icensorvar", "icensortime", 
  "lcensorstmt", "weightstmt", "parmstmt", "parmsopts", "parmsopt", 
  "stepwisestmt", "stepwiseopts", "stepwiseopt", "obscountstmt", 
  "jobidstmt", "jobixstmt", "phasestmts", "phasecmd", "phasename", 
  "phasevaropts", "phasevaropt", "phasevar", "phaseval", "phaseoptspec", 
  "phaseopts", "phaseopt", "restrictstmts", "restrictcmd", "bundlestmt", 
  "bundlespec", "bundlecmd", "bundlename", "varlist", "varname", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,    59,
      61,    44,    47,    60,    62
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    85,    86,    87,    88,    89,    89,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    91,    91,    92,    92,    93,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    93,
      93,    93,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   103,   104,   104,   104,   104,   104,   104,
     104,   104,   104,   104,   104,   104,   104,   104,   104,   104,
     104,   104,   104,   104,   104,   104,   104,   105,   106,   106,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   108,   109,   110,   111,   112,   113,   113,
     113,   114,   114,   115,   116,   117,   117,   118,   118,   119,
     119,   120,   120,   120,   120,   120,   121,   122,   123,   124,
     125,   126,   127,   127,   128
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     4,     1,     1,     2,     3,     3,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     2,     2,     2,     4,     1,     1,     2,
       2,     2,     1,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     0,     2,
       3,     3,     3,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     2,     2,     1,     1,     1,
       1,     1,     3,     3,     1,     0,     2,     0,     2,     1,
       2,     3,     3,     1,     1,     1,     2,     1,     2,     2,
       1,     3,     1,     2,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     0,     4,     0,     1,    41,     0,     0,
       0,     0,     0,    78,    98,    99,   100,     0,     0,     0,
     117,   120,     0,     0,     0,     0,    27,    28,    29,    30,
      31,    32,    33,    34,    38,    39,    40,    35,     0,    97,
      36,     0,    37,     0,     0,    22,     0,     0,     0,     0,
      19,    20,    21,    11,    12,    13,    14,    15,    16,    17,
      18,     0,     5,    43,    44,    45,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    51,    52,    77,    93,    49,    50,    94,    95,     0,
      25,   104,    96,   101,   105,   124,   116,   122,   118,     0,
     119,     0,     0,     0,     0,     3,     6,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    53,
      85,     0,     0,     0,     0,    86,    87,    88,    89,    90,
      91,     0,    92,    79,    26,     0,     0,   107,   123,     0,
      23,    24,     7,     8,     9,    10,    48,    46,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,     0,
       0,     0,     0,     0,   102,   106,     0,   103,   121,    80,
      81,    82,    83,    84,     0,   113,   114,   115,     0,   108,
     109,     0,     0,   110,   111,   112
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     2,     3,     5,    61,    62,   152,    24,    25,    26,
      27,    28,    29,    67,   157,    30,    31,    32,    91,    92,
      33,    93,   143,    34,    35,    36,    37,    38,    39,   102,
     103,   104,   147,   177,   189,   190,    40,    41,    42,    43,
      44,   110,   106,   107
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -104
static const short yypact[] =
{
      -1,     2,     6,    20,  -104,   123,  -104,  -104,     3,     5,
       7,     8,    73,  -104,  -104,  -104,  -104,    10,    11,    12,
    -104,  -104,    13,    14,     0,   -69,  -104,  -104,  -104,  -104,
    -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,    15,  -104,
    -104,    44,  -104,    44,   -34,  -104,   -66,   -30,   -17,   -16,
    -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,
    -104,    21,  -104,  -104,  -104,  -104,  -104,     4,    18,    45,
      46,    47,    48,    49,    66,    67,    68,    69,    70,  -104,
    -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,
    -104,    73,  -104,     1,  -104,  -104,  -104,  -104,  -104,    72,
    -104,  -104,    -3,  -104,    76,  -104,    44,  -104,    44,    77,
    -104,    -2,    -2,    79,    80,  -104,  -104,    87,    89,    90,
     149,   154,   155,   156,   157,   158,   159,   160,   162,  -104,
    -104,    86,    88,    92,    93,  -104,  -104,  -104,  -104,  -104,
    -104,    94,  -104,  -104,  -104,    15,   164,    95,  -104,    -4,
    -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,
    -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,   166,
     167,   168,   172,   173,  -104,  -104,    91,  -104,  -104,  -104,
    -104,  -104,  -104,  -104,    98,  -104,  -104,  -104,    99,    91,
    -104,   177,   178,  -104,  -104,  -104
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -104,  -104,  -104,  -104,  -104,   121,    71,  -104,   161,  -104,
    -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,    96,
    -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,  -104,
      39,  -104,  -104,  -104,  -104,     9,  -104,  -104,  -104,  -104,
    -104,  -104,   143,  -103
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -43
static const short yytable[] =
{
      -2,     7,   150,   148,     1,   148,     6,    63,     4,    64,
     100,    65,    66,    94,   111,    95,    96,    97,    98,   101,
     151,     7,    45,     8,     9,    10,    11,    12,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,     8,     9,    10,    11,    12,   105,   109,
     112,    13,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   113,   114,    14,    15,    16,    17,    18,
      19,    13,   141,   142,    20,    21,    22,    23,   145,   -42,
     178,   149,   154,   155,   117,    14,    15,    16,    17,    18,
      19,   156,   158,   159,    20,    21,    22,    23,   118,   -42,
     115,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    45,   119,   120,   121,   122,   123,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,   184,   124,   125,   126,   127,
     128,   144,   160,   185,   186,   187,   146,   161,   162,   163,
     164,   165,   166,   167,   188,   168,   169,   175,   170,   179,
     180,   181,   171,   172,   173,   182,   183,   176,   191,   192,
     194,   195,   116,   153,   174,    99,   108,   129,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   193
};

static const short yycheck[] =
{
       0,     1,     4,   106,     5,   108,     0,     4,     6,     4,
      79,     4,     4,     3,    80,     4,     4,     4,     4,     4,
      22,     1,     1,    23,    24,    25,    26,    27,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    23,    24,    25,    26,    27,     4,    83,
      80,    51,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    80,    80,    65,    66,    67,    68,    69,
      70,    51,    71,    72,    74,    75,    76,    77,    81,    79,
      84,     4,     3,     3,    80,    65,    66,    67,    68,    69,
      70,     4,     3,     3,    74,    75,    76,    77,    80,    79,
      79,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,     1,    80,    80,    80,    80,    80,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    54,    80,    80,    80,    80,
      80,    79,     3,    62,    63,    64,    80,     3,     3,     3,
       3,     3,     3,     3,    73,     3,    80,     3,    80,     3,
       3,     3,    80,    80,    80,     3,     3,    82,    80,    80,
       3,     3,    61,   112,   145,    24,    43,    91,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   189
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     5,    86,    87,     6,    88,     0,     1,    23,    24,
      25,    26,    27,    51,    65,    66,    67,    68,    69,    70,
      74,    75,    76,    77,    92,    93,    94,    95,    96,    97,
     100,   101,   102,   105,   108,   109,   110,   111,   112,   113,
     121,   122,   123,   124,   125,     1,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    89,    90,     4,     4,     4,     4,    98,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,   103,   104,   106,     3,     4,     4,     4,     4,    93,
      79,     4,   114,   115,   116,     4,   127,   128,   127,    83,
     126,    80,    80,    80,    80,    79,    90,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,   104,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    71,    72,   107,    79,    81,    80,   117,   128,     4,
       4,    22,    91,    91,     3,     3,     4,    99,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,    80,
      80,    80,    80,    80,   115,     3,    82,   118,    84,     3,
       3,     3,     3,     3,    54,    62,    63,    64,    73,   119,
     120,    80,    80,   120,     3,     3
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
#line 49 "hazard_y.y"
    { 
  /* Initialize the YACC parser (looks like a linked list) */
  current = last = anchor = stmtinit(45); 
}
    break;

  case 8:
#line 62 "hazard_y.y"
    { setopt(63); }
    break;

  case 9:
#line 63 "hazard_y.y"
    { setparm(3,yyvsp[0].real); }
    break;

  case 10:
#line 64 "hazard_y.y"
    { setparm(4,yyvsp[0].real); }
    break;

  case 11:
#line 65 "hazard_y.y"
    { setopt(1); }
    break;

  case 12:
#line 66 "hazard_y.y"
    { setopt(2); }
    break;

  case 13:
#line 67 "hazard_y.y"
    { setopt(3); }
    break;

  case 14:
#line 68 "hazard_y.y"
    { setopt(4); }
    break;

  case 15:
#line 69 "hazard_y.y"
    { setopt(5); }
    break;

  case 16:
#line 70 "hazard_y.y"
    { setopt(26); }
    break;

  case 17:
#line 71 "hazard_y.y"
    { setopt(27); }
    break;

  case 18:
#line 72 "hazard_y.y"
    { setopt(28); }
    break;

  case 19:
#line 73 "hazard_y.y"
    { setopt(29); }
    break;

  case 20:
#line 74 "hazard_y.y"
    { setopt(40); }
    break;

  case 21:
#line 75 "hazard_y.y"
    { setopt(41); }
    break;

  case 22:
#line 76 "hazard_y.y"
    { errtext(); }
    break;

  case 23:
#line 80 "hazard_y.y"
    {}
    break;

  case 41:
#line 102 "hazard_y.y"
    { errtext(); }
    break;

  case 43:
#line 106 "hazard_y.y"
    { setvar(11,yyvsp[0].name); }
    break;

  case 44:
#line 109 "hazard_y.y"
    { setvar(12,yyvsp[0].name); }
    break;

  case 45:
#line 112 "hazard_y.y"
    { setvar(13,yyvsp[0].name); }
    break;

  case 47:
#line 118 "hazard_y.y"
    { setvar(14,yyvsp[0].name); }
    break;

  case 48:
#line 121 "hazard_y.y"
    { setvar(15,yyvsp[0].name); }
    break;

  case 49:
#line 124 "hazard_y.y"
    { setvar(36,yyvsp[0].name); }
    break;

  case 50:
#line 127 "hazard_y.y"
    { setvar(37,yyvsp[0].name); }
    break;

  case 54:
#line 137 "hazard_y.y"
    { setparm(16,yyvsp[0].real); }
    break;

  case 55:
#line 138 "hazard_y.y"
    { setparm(17,yyvsp[0].real); }
    break;

  case 56:
#line 139 "hazard_y.y"
    { setparm(18,yyvsp[0].real); }
    break;

  case 57:
#line 140 "hazard_y.y"
    { setparm(19,yyvsp[0].real); }
    break;

  case 58:
#line 141 "hazard_y.y"
    { setparm(20,yyvsp[0].real); }
    break;

  case 59:
#line 142 "hazard_y.y"
    { setparm(21,yyvsp[0].real); }
    break;

  case 60:
#line 143 "hazard_y.y"
    { setparm(22,yyvsp[0].real); }
    break;

  case 61:
#line 144 "hazard_y.y"
    { setparm(23,yyvsp[0].real); }
    break;

  case 62:
#line 145 "hazard_y.y"
    { setparm(24,yyvsp[0].real); }
    break;

  case 63:
#line 146 "hazard_y.y"
    { setparm(25,yyvsp[0].real); }
    break;

  case 64:
#line 147 "hazard_y.y"
    { setparm(26,yyvsp[0].real); }
    break;

  case 65:
#line 148 "hazard_y.y"
    { setopt(6); }
    break;

  case 66:
#line 149 "hazard_y.y"
    { setopt(7); }
    break;

  case 67:
#line 150 "hazard_y.y"
    { setopt(8); }
    break;

  case 68:
#line 151 "hazard_y.y"
    { setopt(9); }
    break;

  case 69:
#line 152 "hazard_y.y"
    { setopt(10); }
    break;

  case 70:
#line 153 "hazard_y.y"
    { setopt(11); }
    break;

  case 71:
#line 154 "hazard_y.y"
    { setopt(12); }
    break;

  case 72:
#line 155 "hazard_y.y"
    { setopt(13); }
    break;

  case 73:
#line 156 "hazard_y.y"
    { setopt(14); }
    break;

  case 74:
#line 157 "hazard_y.y"
    { setopt(15); }
    break;

  case 75:
#line 158 "hazard_y.y"
    { setopt(16); }
    break;

  case 76:
#line 159 "hazard_y.y"
    { setopt(17); }
    break;

  case 77:
#line 162 "hazard_y.y"
    { setopt(33); }
    break;

  case 80:
#line 169 "hazard_y.y"
    { setparm(31,yyvsp[0].real); }
    break;

  case 81:
#line 170 "hazard_y.y"
    { setparm(32,yyvsp[0].real); }
    break;

  case 82:
#line 171 "hazard_y.y"
    { setparm(33,yyvsp[0].real); }
    break;

  case 83:
#line 172 "hazard_y.y"
    { setparm(34,yyvsp[0].real); }
    break;

  case 84:
#line 173 "hazard_y.y"
    { setparm(35,yyvsp[0].real); }
    break;

  case 85:
#line 174 "hazard_y.y"
    { setopt(21); }
    break;

  case 86:
#line 175 "hazard_y.y"
    { setopt(22); }
    break;

  case 87:
#line 176 "hazard_y.y"
    { setopt(23); }
    break;

  case 88:
#line 177 "hazard_y.y"
    { setopt(24); }
    break;

  case 89:
#line 178 "hazard_y.y"
    { setopt(25); }
    break;

  case 90:
#line 179 "hazard_y.y"
    { setopt(31); }
    break;

  case 91:
#line 180 "hazard_y.y"
    { setopt(32); }
    break;

  case 92:
#line 181 "hazard_y.y"
    { setopt(34); }
    break;

  case 93:
#line 184 "hazard_y.y"
    { setparm(43,yyvsp[0].real); }
    break;

  case 94:
#line 187 "hazard_y.y"
    { setvar(44,yyvsp[0].name); }
    break;

  case 95:
#line 190 "hazard_y.y"
    { setvar(45,yyvsp[0].name); }
    break;

  case 96:
#line 193 "hazard_y.y"
    { current = anchor; }
    break;

  case 97:
#line 196 "hazard_y.y"
    { last = current = 
						    (last->next = stmtinit(1));
                         }
    break;

  case 104:
#line 213 "hazard_y.y"
    { current = last = (last->next = stmtinit(10)); }
    break;

  case 106:
#line 217 "hazard_y.y"
    { setparm(2,yyvsp[0].real); }
    break;

  case 111:
#line 228 "hazard_y.y"
    { setparm(3,yyvsp[0].real); }
    break;

  case 112:
#line 229 "hazard_y.y"
    { setparm(4,yyvsp[0].real); }
    break;

  case 113:
#line 230 "hazard_y.y"
    { setopt(1); }
    break;

  case 114:
#line 231 "hazard_y.y"
    { setopt(2); }
    break;

  case 115:
#line 232 "hazard_y.y"
    { setopt(3); }
    break;

  case 116:
#line 235 "hazard_y.y"
    { current = anchor; }
    break;

  case 117:
#line 238 "hazard_y.y"
    { current = last = (last->next = stmtinit(1)); }
    break;

  case 118:
#line 241 "hazard_y.y"
    { current = anchor; }
    break;

  case 120:
#line 247 "hazard_y.y"
    { current = last = (last->next = stmtinit(5)); }
    break;

  case 121:
#line 250 "hazard_y.y"
    { setvar(2,yyvsp[-1].name); }
    break;

  case 124:
#line 257 "hazard_y.y"
    { current = last = (last->next = stmtinit(2)); setvar(2,yyvsp[0].name); }
    break;


    }

/* Line 991 of yacc.c.  */
#line 1748 "hazard_y.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  */
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__)
  __attribute__ ((__unused__))
#endif


  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 259 "hazard_y.y"

/* Addition C Code */

