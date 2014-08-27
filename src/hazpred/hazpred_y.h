typedef union {
	double real;
	char *name;
} YYSTYPE;
#define	NUMBER	257
#define	NAME	258
#define	PROC	259
#define	HAZPRED	260
#define	DATA	261
#define	INHAZ	262
#define	OUT	263
#define	CLIMITS	264
#define	NOCL	265
#define	NOSURV	266
#define	NOHAZ	267
#define	NOLOG	268
#define	NONOTES	269
#define	TIME	270
#define	ID	271
#define	LIBMEM	272
#define	OBSCOUNT	273
#define	HAZCOUNT	274
#define	JOBID	275
#define	JOBIX	276


extern YYSTYPE yylval;
