/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

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
/* Tokens.  */
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

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 20 "hazard_y.y"
{
  double real;
  char *name;
}
/* Line 193 of yacc.c.  */
#line 274 "hazard_y.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 287 "hazard_y.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
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
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   198

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  85
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  124
/* YYNRULES -- Number of states.  */
#define YYNSTATES  196

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   333

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
static const yytype_uint16 yyprhs[] =
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

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
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
static const yytype_uint16 yyrline[] =
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

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUMBER", "NAME", "PROC", "HAZARD",
  "DATA", "OUTHAZ", "MAXITER", "CONDITION", "NOPRINT", "NOLOG", "NONOTES",
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
static const yytype_uint16 yytoknum[] =
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
static const yytype_uint8 yyr1[] =
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
static const yytype_uint8 yyr2[] =
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
static const yytype_uint8 yydefact[] =
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

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
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
static const yytype_int16 yypact[] =
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
static const yytype_int16 yypgoto[] =
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
static const yytype_int16 yytable[] =
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

static const yytype_int16 yycheck[] =
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
static const yytype_uint8 yystos[] =
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

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


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
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

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
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
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

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
    { setparm(3,(yyvsp[(3) - (3)].real)); }
    break;

  case 10:
#line 64 "hazard_y.y"
    { setparm(4,(yyvsp[(3) - (3)].real)); }
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
    { setvar(11,(yyvsp[(2) - (2)].name)); }
    break;

  case 44:
#line 109 "hazard_y.y"
    { setvar(12,(yyvsp[(2) - (2)].name)); }
    break;

  case 45:
#line 112 "hazard_y.y"
    { setvar(13,(yyvsp[(2) - (2)].name)); }
    break;

  case 47:
#line 118 "hazard_y.y"
    { setvar(14,(yyvsp[(1) - (1)].name)); }
    break;

  case 48:
#line 121 "hazard_y.y"
    { setvar(15,(yyvsp[(1) - (1)].name)); }
    break;

  case 49:
#line 124 "hazard_y.y"
    { setvar(36,(yyvsp[(2) - (2)].name)); }
    break;

  case 50:
#line 127 "hazard_y.y"
    { setvar(37,(yyvsp[(2) - (2)].name)); }
    break;

  case 54:
#line 137 "hazard_y.y"
    { setparm(16,(yyvsp[(3) - (3)].real)); }
    break;

  case 55:
#line 138 "hazard_y.y"
    { setparm(17,(yyvsp[(3) - (3)].real)); }
    break;

  case 56:
#line 139 "hazard_y.y"
    { setparm(18,(yyvsp[(3) - (3)].real)); }
    break;

  case 57:
#line 140 "hazard_y.y"
    { setparm(19,(yyvsp[(3) - (3)].real)); }
    break;

  case 58:
#line 141 "hazard_y.y"
    { setparm(20,(yyvsp[(3) - (3)].real)); }
    break;

  case 59:
#line 142 "hazard_y.y"
    { setparm(21,(yyvsp[(3) - (3)].real)); }
    break;

  case 60:
#line 143 "hazard_y.y"
    { setparm(22,(yyvsp[(3) - (3)].real)); }
    break;

  case 61:
#line 144 "hazard_y.y"
    { setparm(23,(yyvsp[(3) - (3)].real)); }
    break;

  case 62:
#line 145 "hazard_y.y"
    { setparm(24,(yyvsp[(3) - (3)].real)); }
    break;

  case 63:
#line 146 "hazard_y.y"
    { setparm(25,(yyvsp[(3) - (3)].real)); }
    break;

  case 64:
#line 147 "hazard_y.y"
    { setparm(26,(yyvsp[(3) - (3)].real)); }
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
    { setparm(31,(yyvsp[(3) - (3)].real)); }
    break;

  case 81:
#line 170 "hazard_y.y"
    { setparm(32,(yyvsp[(3) - (3)].real)); }
    break;

  case 82:
#line 171 "hazard_y.y"
    { setparm(33,(yyvsp[(3) - (3)].real)); }
    break;

  case 83:
#line 172 "hazard_y.y"
    { setparm(34,(yyvsp[(3) - (3)].real)); }
    break;

  case 84:
#line 173 "hazard_y.y"
    { setparm(35,(yyvsp[(3) - (3)].real)); }
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
    { setparm(43,(yyvsp[(2) - (2)].real)); }
    break;

  case 94:
#line 187 "hazard_y.y"
    { setvar(44,(yyvsp[(2) - (2)].name)); }
    break;

  case 95:
#line 190 "hazard_y.y"
    { setvar(45,(yyvsp[(2) - (2)].name)); }
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
    { setparm(2,(yyvsp[(2) - (2)].real)); }
    break;

  case 111:
#line 228 "hazard_y.y"
    { setparm(3,(yyvsp[(3) - (3)].real)); }
    break;

  case 112:
#line 229 "hazard_y.y"
    { setparm(4,(yyvsp[(3) - (3)].real)); }
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
    { setvar(2,(yyvsp[(2) - (3)].name)); }
    break;

  case 124:
#line 257 "hazard_y.y"
    { current = last = (last->next = stmtinit(2)); setvar(2,(yyvsp[(1) - (1)].name)); }
    break;


/* Line 1267 of yacc.c.  */
#line 2089 "hazard_y.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
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
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 259 "hazard_y.y"

/* Addition C Code */

