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
     HAZPRED = 261,
     DATA = 262,
     INHAZ = 263,
     OUT = 264,
     CLIMITS = 265,
     NOCL = 266,
     NOSURV = 267,
     NOHAZ = 268,
     NOLOG = 269,
     NONOTES = 270,
     TIME = 271,
     ID = 272,
     LIBMEM = 273,
     OBSCOUNT = 274,
     HAZCOUNT = 275,
     JOBID = 276,
     JOBIX = 277
   };
#endif
/* Tokens.  */
#define NUMBER 258
#define NAME 259
#define PROC 260
#define HAZPRED 261
#define DATA 262
#define INHAZ 263
#define OUT 264
#define CLIMITS 265
#define NOCL 266
#define NOSURV 267
#define NOHAZ 268
#define NOLOG 269
#define NONOTES 270
#define TIME 271
#define ID 272
#define LIBMEM 273
#define OBSCOUNT 274
#define HAZCOUNT 275
#define JOBID 276
#define JOBIX 277




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 19 "hazpred_y.y"
{
	double real;
	char *name;
}
/* Line 1529 of yacc.c.  */
#line 98 "hazpred_y.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

