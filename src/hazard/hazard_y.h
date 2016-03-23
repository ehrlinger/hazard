/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 20 "hazard_y.y"
{
  double real;
  char *name;
}
/* Line 1529 of yacc.c.  */
#line 210 "hazard_y.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

