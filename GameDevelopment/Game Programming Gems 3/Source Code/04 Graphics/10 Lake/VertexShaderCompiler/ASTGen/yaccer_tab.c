
/*  A Bison parser, made from yaccer.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_STRING	258
#define	T_FLOAT	259
#define	T_PLUS	260
#define	T_MINUS	261
#define	T_MULTIPLY	262
#define	T_DIVIDE	263
#define	T_DOT4	264
#define	T_NORMALIZE	265
#define	T_SQRT	266
#define	T_NEGATE	267
#define	T_POWER	268
#define	T_MAXWITH0	269
#define	T_CLAMPTO1	270
#define	T_SIN	271
#define	T_COS	272
#define	T_FLOOR	273
#define	T_CEILING	274
#define	T_EQUALS	275
#define	T_LBRACE	276
#define	T_RBRACE	277
#define	T_LPAREN	278
#define	T_RPAREN	279
#define	T_SEMICOLON	280
#define	T_COMMA	281
#define	T_TYPE_FLOAT	282
#define	T_TYPE_CLAMP	283
#define	T_DOT3	284

#line 1 "yaccer.y"



#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "CastNode.h"
#include "CSymbolTable.h"
#include "CCodeGenerator.h"

//this is the root node for the main AST (abstract syntax tree) sent to the codegenerator
CastNode *root = NULL;
//declaration of symbol table used during compilation.
CSymbolTable symTab;

int eWhichRegComponent = eRegCompNone;

int yyerror(char *s);

extern int yylex();

//needed this so it would compile
int isatty(int flag) { return 0; }

#line 28 "yaccer.y"
typedef union {
	float fval;
	int ival;
	int token;
	char *sval;
	//can't make it an CastNode because its a C++ class
	void *node;
	} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		86
#define	YYFLAG		-32768
#define	YYNTBASE	30

#define YYTRANSLATE(x) ((unsigned)(x) <= 284 ? yytranslate[x] : 36)

static const char yytranslate[] = {     0,
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
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     7,    14,    17,    19,    24,    28,    32,
    36,    40,    44,    48,    52,    56,    60,    65,    70,    75,
    80,    85,    90,    95,   100,   105,   107,   109,   113
};

static const short yyrhs[] = {    31,
     0,    31,    32,     0,    32,     0,    35,    23,    24,    21,
    33,    22,     0,    33,    34,     0,    34,     0,    35,    20,
    35,    25,     0,    27,     3,    25,     0,    28,     3,    25,
     0,    35,     5,    35,     0,    35,     6,    35,     0,    35,
     7,    35,     0,    35,     8,    35,     0,    35,     9,    35,
     0,    35,    29,    35,     0,    35,    13,    35,     0,    10,
    23,    35,    24,     0,    11,    23,    35,    24,     0,    14,
    23,    35,    24,     0,    12,    23,    35,    24,     0,    15,
    23,    35,    24,     0,    16,    23,    35,    24,     0,    17,
    23,    35,    24,     0,    18,    23,    35,    24,     0,    19,
    23,    35,    24,     0,     3,     0,     4,     0,    23,    35,
    24,     0,    23,    35,    26,    35,    26,    35,    26,    35,
    24,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    51,    60,    67,    74,    83,    91,    99,   116,   125,   138,
   146,   154,   162,   170,   178,   187,   196,   204,   212,   221,
   229,   237,   245,   253,   261,   271,   278,   284,   287
};

static const char * const yytname[] = {   "$","error","$undefined.","T_STRING",
"T_FLOAT","T_PLUS","T_MINUS","T_MULTIPLY","T_DIVIDE","T_DOT4","T_NORMALIZE",
"T_SQRT","T_NEGATE","T_POWER","T_MAXWITH0","T_CLAMPTO1","T_SIN","T_COS","T_FLOOR",
"T_CEILING","T_EQUALS","T_LBRACE","T_RBRACE","T_LPAREN","T_RPAREN","T_SEMICOLON",
"T_COMMA","T_TYPE_FLOAT","T_TYPE_CLAMP","T_DOT3","Program","Functions","Function",
"StmtList","Stmt","Expr",""
};
#endif

static const short yyr1[] = {     0,
    30,    31,    31,    32,    33,    33,    34,    34,    34,    35,
    35,    35,    35,    35,    35,    35,    35,    35,    35,    35,
    35,    35,    35,    35,    35,    35,    35,    35,    35
};

static const short yyr2[] = {     0,
     1,     2,     1,     6,     2,     1,     4,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     1,     1,     3,     9
};

static const short yydefact[] = {     0,
    26,    27,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     1,     3,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     2,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    28,     0,    10,    11,    12,    13,    14,
    16,     0,    15,    17,    18,    20,    19,    21,    22,    23,
    24,    25,     0,     0,     0,     0,     0,     0,     6,     0,
     0,     0,     0,     4,     5,     0,     0,     8,     9,     0,
     0,     7,    29,     0,     0,     0
};

static const short yydefgoto[] = {    84,
    13,    14,    68,    69,    15
};

static const short yypact[] = {   337,
-32768,-32768,   -22,   -21,   -20,   -10,    -9,    -2,     2,     3,
     4,   337,   337,-32768,    89,   337,   337,   337,   337,   337,
   337,   337,   337,   337,    80,-32768,   337,   337,   337,   337,
   337,   337,     5,   337,   108,   117,   142,   151,   176,   185,
   210,   219,   244,-32768,   337,   268,   268,-32768,-32768,-32768,
-32768,     7,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   253,    56,   337,    37,    38,    20,-32768,   278,
   287,    19,    21,-32768,-32768,   337,   337,-32768,-32768,   296,
   321,-32768,-32768,    45,    49,-32768
};

static const short yypgoto[] = {-32768,
-32768,    41,-32768,   -18,   -12
};


#define	YYLAST		360


static const short yytable[] = {    25,
    16,    17,    18,    35,    36,    37,    38,    39,    40,    41,
    42,    43,    19,    20,    46,    47,    48,    49,    50,    51,
    21,    53,     1,     2,    22,    23,    24,    64,    52,     3,
     4,     5,    63,     6,     7,     8,     9,    10,    11,    72,
    73,    74,    12,    78,    85,    79,    66,    67,    86,    75,
     0,    70,    71,    26,     0,    70,     0,     0,     1,     2,
     0,     0,     0,    80,    81,     3,     4,     5,     0,     6,
     7,     8,     9,    10,    11,     0,     0,     0,    12,     0,
     0,     0,    66,    67,    27,    28,    29,    30,    31,     0,
     0,     0,    32,    27,    28,    29,    30,    31,     0,     0,
     0,    32,     0,    44,     0,    45,     0,     0,    34,     0,
     0,    33,    27,    28,    29,    30,    31,    34,     0,     0,
    32,    27,    28,    29,    30,    31,     0,     0,     0,    32,
     0,    54,     0,     0,     0,     0,    34,     0,     0,     0,
    55,     0,     0,     0,     0,    34,    27,    28,    29,    30,
    31,     0,     0,     0,    32,    27,    28,    29,    30,    31,
     0,     0,     0,    32,     0,    56,     0,     0,     0,     0,
    34,     0,     0,     0,    57,     0,     0,     0,     0,    34,
    27,    28,    29,    30,    31,     0,     0,     0,    32,    27,
    28,    29,    30,    31,     0,     0,     0,    32,     0,    58,
     0,     0,     0,     0,    34,     0,     0,     0,    59,     0,
     0,     0,     0,    34,    27,    28,    29,    30,    31,     0,
     0,     0,    32,    27,    28,    29,    30,    31,     0,     0,
     0,    32,     0,    60,     0,     0,     0,     0,    34,     0,
     0,     0,    61,     0,     0,     0,     0,    34,    27,    28,
    29,    30,    31,     0,     0,     0,    32,    27,    28,    29,
    30,    31,     0,     0,     0,    32,     0,    62,     0,     0,
     0,     0,    34,     0,    29,    30,    31,     0,    65,     0,
    32,    34,    27,    28,    29,    30,    31,     0,     0,     0,
    32,    27,    28,    29,    30,    31,    34,    76,     0,    32,
    27,    28,    29,    30,    31,     0,    34,     0,    32,     0,
     0,     0,    77,     0,     0,    34,     0,     0,     0,     0,
    82,     0,     0,     0,    34,    27,    28,    29,    30,    31,
     0,     0,     0,    32,     0,     0,     0,     0,     0,     1,
     2,     0,     0,     0,    83,     0,     3,     4,     5,    34,
     6,     7,     8,     9,    10,    11,     0,     0,     0,    12
};

static const short yycheck[] = {    12,
    23,    23,    23,    16,    17,    18,    19,    20,    21,    22,
    23,    24,    23,    23,    27,    28,    29,    30,    31,    32,
    23,    34,     3,     4,    23,    23,    23,    21,    24,    10,
    11,    12,    45,    14,    15,    16,    17,    18,    19,     3,
     3,    22,    23,    25,     0,    25,    27,    28,     0,    68,
    -1,    64,    65,    13,    -1,    68,    -1,    -1,     3,     4,
    -1,    -1,    -1,    76,    77,    10,    11,    12,    -1,    14,
    15,    16,    17,    18,    19,    -1,    -1,    -1,    23,    -1,
    -1,    -1,    27,    28,     5,     6,     7,     8,     9,    -1,
    -1,    -1,    13,     5,     6,     7,     8,     9,    -1,    -1,
    -1,    13,    -1,    24,    -1,    26,    -1,    -1,    29,    -1,
    -1,    23,     5,     6,     7,     8,     9,    29,    -1,    -1,
    13,     5,     6,     7,     8,     9,    -1,    -1,    -1,    13,
    -1,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,
    24,    -1,    -1,    -1,    -1,    29,     5,     6,     7,     8,
     9,    -1,    -1,    -1,    13,     5,     6,     7,     8,     9,
    -1,    -1,    -1,    13,    -1,    24,    -1,    -1,    -1,    -1,
    29,    -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,    29,
     5,     6,     7,     8,     9,    -1,    -1,    -1,    13,     5,
     6,     7,     8,     9,    -1,    -1,    -1,    13,    -1,    24,
    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    24,    -1,
    -1,    -1,    -1,    29,     5,     6,     7,     8,     9,    -1,
    -1,    -1,    13,     5,     6,     7,     8,     9,    -1,    -1,
    -1,    13,    -1,    24,    -1,    -1,    -1,    -1,    29,    -1,
    -1,    -1,    24,    -1,    -1,    -1,    -1,    29,     5,     6,
     7,     8,     9,    -1,    -1,    -1,    13,     5,     6,     7,
     8,     9,    -1,    -1,    -1,    13,    -1,    24,    -1,    -1,
    -1,    -1,    29,    -1,     7,     8,     9,    -1,    26,    -1,
    13,    29,     5,     6,     7,     8,     9,    -1,    -1,    -1,
    13,     5,     6,     7,     8,     9,    29,    20,    -1,    13,
     5,     6,     7,     8,     9,    -1,    29,    -1,    13,    -1,
    -1,    -1,    26,    -1,    -1,    29,    -1,    -1,    -1,    -1,
    25,    -1,    -1,    -1,    29,     5,     6,     7,     8,     9,
    -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,     3,
     4,    -1,    -1,    -1,    24,    -1,    10,    11,    12,    29,
    14,    15,    16,    17,    18,    19,    -1,    -1,    -1,    23
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
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

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
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
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 192 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(YYPARSE_PARAM)
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
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
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

case 1:
#line 51 "yaccer.y"
{
					printf("DONE\n");
					yyval.node = new CastNode("Program");
					((CastNode *)yyval.node)->setDescription("Beginning of function");
					((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[0].node);			
					root = (CastNode *)yyval.node;
					//CastNode::inOrderTreeWalk(root);
					;
    break;}
case 2:
#line 60 "yaccer.y"
{
								printf("functions function\n");
								yyval.node = new CastNode("Functions");
								((CastNode *)yyval.node)->setDescription("List of Functions");
								((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[-1].node);
								((CastNode *)yyval.node)->addChildNode(1,(CastNode *)yyvsp[0].node);
								;
    break;}
case 3:
#line 67 "yaccer.y"
{
								printf("function\n");
								yyval.node = new CastNode("Functions");
								((CastNode *)yyval.node)->setDescription("Single Function");
								((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[0].node);
								;
    break;}
case 4:
#line 74 "yaccer.y"
{
															//TODO: this maybe should not be expr but a T_STRING instead?
															printf("FUNCTION is complete\n");
															yyval.node = new CastNode("Function");
															((CastNode *)yyval.node)->setDescription("Function");
															((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[-5].node);
															((CastNode *)yyval.node)->addChildNode(4,(CastNode *)yyvsp[-1].node);
															;
    break;}
case 5:
#line 84 "yaccer.y"
{
						printf("stmt_list: stmt_list stmt\n");
						yyval.node = new CastNode("StmtList");
						((CastNode *)yyval.node)->setDescription("List of Statements");
						((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[-1].node);
						((CastNode *)yyval.node)->addChildNode(1,(CastNode *)yyvsp[0].node);
						;
    break;}
case 6:
#line 92 "yaccer.y"
{
				printf("stmt_list: stmt\n");
				yyval.node = new CastNode("StmtList");
				((CastNode *)yyval.node)->setDescription("List of Statements");
				((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[0].node);
				;
    break;}
case 7:
#line 99 "yaccer.y"
{
										printf("expr EQUALS expr\n");
										yyval.node = new CastNode("Stmt");
										((CastNode *)yyval.node)->setDescription("equals");
										//CastNode::inOrderTreeWalk((CastNode *)$1);
										((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[-3].node);
										
										CastNode *pNewNode = new CastNode("Equals");
										
										//((CastNode *)$1)->setDescription("Equals description");
										((CastNode *)yyval.node)->addChildNode(1, (CastNode *)pNewNode);
									
										printf("******************************\n");
										CastNode::inOrderTreeWalk((CastNode *)yyvsp[-1].node);
										printf("******************************\n");
										((CastNode *)yyval.node)->addChildNode(2,(CastNode *)yyvsp[-1].node);		
										;
    break;}
case 8:
#line 116 "yaccer.y"
{
										printf("declaration\n");
										yyval.node = new CastNode("Stmt");
										((CastNode *)yyval.node)->setDescription("float declaration ");
										//TODO: figure out how i want to do this
										//determine if symbol exists, if not, add to symbol table, if it does
										symTab.AddSymbol(yyvsp[-1].sval,"float",T_STRING,eRegTypeNone,-1);
										//if necessary, add a child node with the proper data
										;
    break;}
case 9:
#line 125 "yaccer.y"
{
										printf("declaration\n");
										yyval.node = new CastNode("Stmt");
										((CastNode *)yyval.node)->setDescription("clamp declaration MORE STUFF WILL GO HERE");
										symTab.AddSymbol(yyvsp[-1].sval,"clamp",T_STRING,eRegTypeNone,-1);
										//TODO: figure out how i want to do this
										
										
										;
    break;}
case 10:
#line 138 "yaccer.y"
{
								printf("expr complete\n");
								yyval.node = new CastNode("Expr");
								((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[-2].node);
								CastNode *pNewNode = new CastNode("T_PLUS");
								((CastNode *)yyval.node)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)yyval.node)->addChildNode(2,(CastNode *)yyvsp[0].node);
						;
    break;}
case 11:
#line 146 "yaccer.y"
{
								yyval.node = new CastNode("Expr");
								((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[-2].node);
								CastNode *pNewNode = new CastNode("T_MINUS");
								((CastNode *)yyval.node)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)yyval.node)->addChildNode(2,(CastNode *)yyvsp[0].node);

						;
    break;}
case 12:
#line 154 "yaccer.y"
{
								yyval.node = new CastNode("Expr");
								((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[-2].node);
								CastNode *pNewNode = new CastNode("T_MULTIPLY");
								((CastNode *)yyval.node)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)yyval.node)->addChildNode(2,(CastNode *)yyvsp[0].node);

						;
    break;}
case 13:
#line 162 "yaccer.y"
{
								yyval.node = new CastNode("Expr");
								((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[-2].node);
								CastNode *pNewNode = new CastNode("T_DIVIDE");
								((CastNode *)yyval.node)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)yyval.node)->addChildNode(2,(CastNode *)yyvsp[0].node);

						;
    break;}
case 14:
#line 170 "yaccer.y"
{
								yyval.node = new CastNode("Expr");
								((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[-2].node);
								CastNode *pNewNode = new CastNode("T_DOT4");
								((CastNode *)yyval.node)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)yyval.node)->addChildNode(2,(CastNode *)yyvsp[0].node);

						;
    break;}
case 15:
#line 178 "yaccer.y"
{
								yyval.node = new CastNode("Expr");
								((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[-2].node);
								CastNode *pNewNode = new CastNode("T_DOT3");
								((CastNode *)yyval.node)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)yyval.node)->addChildNode(2,(CastNode *)yyvsp[0].node);

						;
    break;}
case 16:
#line 187 "yaccer.y"
{
								yyval.node = new CastNode("Expr");
								((CastNode *)yyval.node)->addChildNode(0,(CastNode *)yyvsp[-2].node);
								CastNode *pNewNode = new CastNode("T_POWER");
								((CastNode *)yyval.node)->addChildNode(1,(CastNode *)pNewNode);
								((CastNode *)yyval.node)->addChildNode(2,(CastNode *)yyvsp[0].node);

						;
    break;}
case 17:
#line 197 "yaccer.y"
{
							printf("recognized normalize\n");
							yyval.node = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_NORMALIZE");
							((CastNode *)yyval.node)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)yyval.node)->addChildNode(1,(CastNode *)yyvsp[-1].node);
						;
    break;}
case 18:
#line 205 "yaccer.y"
{
							printf("recognized normalize\n");
							yyval.node = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_SQRT");
							((CastNode *)yyval.node)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)yyval.node)->addChildNode(1,(CastNode *)yyvsp[-1].node);
						;
    break;}
case 19:
#line 213 "yaccer.y"
{
							printf("recognized maxwith0\n");
							yyval.node = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_MAXWITH0");
							((CastNode *)yyval.node)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)yyval.node)->addChildNode(1,(CastNode *)yyvsp[-1].node);
						;
    break;}
case 20:
#line 222 "yaccer.y"
{
							printf("recognized normalize\n");
							yyval.node = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_NEGATE");
							((CastNode *)yyval.node)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)yyval.node)->addChildNode(1,(CastNode *)yyvsp[-1].node);
						;
    break;}
case 21:
#line 230 "yaccer.y"
{
							printf("recognized normalize\n");
							yyval.node = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_CLAMPTO1");
							((CastNode *)yyval.node)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)yyval.node)->addChildNode(1,(CastNode *)yyvsp[-1].node);
						;
    break;}
case 22:
#line 238 "yaccer.y"
{
							printf("recognized normalize\n");
							yyval.node = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_SIN");
							((CastNode *)yyval.node)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)yyval.node)->addChildNode(1,(CastNode *)yyvsp[-1].node);
						;
    break;}
case 23:
#line 246 "yaccer.y"
{
							printf("recognized normalize\n");
							yyval.node = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_COS");
							((CastNode *)yyval.node)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)yyval.node)->addChildNode(1,(CastNode *)yyvsp[-1].node);
						;
    break;}
case 24:
#line 254 "yaccer.y"
{
							printf("recognized normalize\n");
							yyval.node = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_FLOOR");
							((CastNode *)yyval.node)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)yyval.node)->addChildNode(1,(CastNode *)yyvsp[-1].node);
						;
    break;}
case 25:
#line 262 "yaccer.y"
{
							printf("recognized normalize\n");
							yyval.node = new CastNode("Expr");
							CastNode *pNewNode = new CastNode("T_CEILING");
							((CastNode *)yyval.node)->addChildNode(0,(CastNode *)pNewNode);
							((CastNode *)yyval.node)->addChildNode(1,(CastNode *)yyvsp[-1].node);
						;
    break;}
case 26:
#line 271 "yaccer.y"
{
								printf("expr: STRING: %s is string  \n", yyvsp[0].sval);
								yyval.node = new CastNode("Expr");
								((CastNode *)yyval.node)->setDescription("T_STRING");
								((CastNode *)yyval.node)->sval = strdup(yylval.sval);
								((CastNode *)yyval.node)->setRegComponent(eWhichRegComponent);
							;
    break;}
case 27:
#line 279 "yaccer.y"
{
								yyval.node = new CastNode("Expr");
								((CastNode *)yyval.node)->setDescription("T_FLOAT");
								((CastNode *)yyval.node)->fval = yylval.fval;
							;
    break;}
case 28:
#line 284 "yaccer.y"
{
							yyval.node = yyvsp[-1].node;
							;
    break;}
case 29:
#line 288 "yaccer.y"
{
								printf("FLOAT VECTOR RECOGNIZED\n");
								yyval.node = new CastNode("Expr");
								//create the vector node subgraph
								CastNode *pVectorNode = NULL;
								pVectorNode = new CastNode("Vector");
								pVectorNode->addChildNode(0,(CastNode *)yyvsp[-7].node);
								pVectorNode->addChildNode(1,(CastNode *)yyvsp[-5].node);
								pVectorNode->addChildNode(2,(CastNode *)yyvsp[-3].node);
								pVectorNode->addChildNode(3,(CastNode *)yyvsp[-1].node);
								//attach it to the rest of the graph
								((CastNode *)yyval.node)->addChildNode(0,pVectorNode);
								
							;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 487 "bison.simple"

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
}
#line 302 "yaccer.y"

extern FILE *yyin;

void InsertSymbols();
void CodeGen(char *outputFileName);

/*
	main()

	Main entry point for entire program.  
	First, insert symbols (keywords) into the symbol table.
	Second, call yyparse() which is the main parsing function.
	Third, call the code Generator to generate code using the AST generated by the 
	scanner and parser in the function yyparse().
	
	*/
void main(int argc, char **argv)
{
	InsertSymbols();
	printf("%s is file to open\n",argv[1]);
	if(argc > 1)
	{
		FILE *file;

		file = fopen(argv[1], "r");
		if(!file)
		{
			printf("Error: Could not open file %s\n",argv[1]);
			exit(-1);
		}
		else
		{ 
			yyin = file;
		}
		yyparse();
	}
	printf("Program Parse Complete\nPress Any Key to Start Code Generation...");
	getchar();
	CodeGen(argv[2]);
	printf("Code Generation Complete\nPress Any Key to End");
	getchar();
}

/* 
	yyerror()
	
	yyerror is the built-in error function.  See Levine's Lex and Yacc book for more information on
	using this function to report errors.  It is expected you overload this with a more sophisticated
	error handling routine.
*/

int yyerror(char *s)
{
	fprintf(stderr,"%s IS MY ERROR\n",s);
	return 1;
}

/*
	InsertSymbols()

	This function adds symbols (mostly keywords) to the symbol table to be used
	during parsing.
*/

void InsertSymbols()
{

#define NEXTAVAILABLEREG 36.y

	symTab.AddSymbol("float","keyword",T_TYPE_FLOAT,eRegTypeNone,-1);
	symTab.AddSymbol("clamp","keyword",T_TYPE_CLAMP,eRegTypeNone,-1);

	symTab.AddSymbol("+","keyword",T_PLUS,eRegTypeNone,-1);
	symTab.AddSymbol("-","keyword",T_MINUS,eRegTypeNone,-1);
	symTab.AddSymbol("*","keyword",T_MULTIPLY,eRegTypeNone,-1);
	symTab.AddSymbol("/","keyword",T_DIVIDE,eRegTypeNone,-1);
	symTab.AddSymbol("^","keyword",T_POWER,eRegTypeNone,-1);

	symTab.AddSymbol("'dot4'","keyword",T_DOT4,eRegTypeNone,-1);
	symTab.AddSymbol("'dot3'","keyword",T_DOT3,eRegTypeNone,-1);
	symTab.AddSymbol("'normalize'","keyword",T_NORMALIZE,eRegTypeNone,-1);
	symTab.AddSymbol("'sqrt'","keyword",T_SQRT,eRegTypeNone,-1);
	symTab.AddSymbol("'negate'","keyword",T_NEGATE,eRegTypeNone,-1);
	symTab.AddSymbol("'maxWith0'","keyword",T_MAXWITH0,eRegTypeNone,-1);
	symTab.AddSymbol("'clampTo1'","keyword",T_CLAMPTO1,eRegTypeNone,-1);
	symTab.AddSymbol("'cos'","keyword",T_COS,eRegTypeNone,-1);
	symTab.AddSymbol("'sin'","keyword",T_SIN,eRegTypeNone,-1);
	symTab.AddSymbol("'floor'","keyword",T_FLOOR,eRegTypeNone,-1);
	symTab.AddSymbol("'ceiling'","keyword",T_CEILING,eRegTypeNone,-1);


	symTab.AddSymbol("=","keyword",T_EQUALS,eRegTypeNone,-1);
	symTab.AddSymbol("{","keyword",T_LBRACE,eRegTypeNone,-1);
	symTab.AddSymbol("}","keyword",T_RBRACE,eRegTypeNone,-1);
	symTab.AddSymbol("(","keyword",T_LPAREN,eRegTypeNone,-1);
	symTab.AddSymbol(")","keyword",T_RPAREN,eRegTypeNone,-1);
	symTab.AddSymbol(";","keyword",T_SEMICOLON,eRegTypeNone,-1);
	symTab.AddSymbol(",","keyword",T_COMMA,eRegTypeNone,-1);

	//shader keywords	
	//Matrix Info
	symTab.AddSymbol("MatrixWorldView","keyword",T_STRING,eRegTypeConst,4);
	symTab.AddSymbol("MatrixView","keyword",T_STRING,eRegTypeConst,12);
	symTab.AddSymbol("MatrixProj","keyword",T_STRING, eRegTypeConst,28);
	symTab.AddSymbol("ViewerPos","keyword",T_STRING, eRegTypeConst,29);


	//Materials and Light data
	symTab.AddSymbol("LightDir0","keyword",T_STRING, eRegTypeConst,26);
	symTab.AddSymbol("LightAmb0","keyword",T_STRING, eRegTypeConst,24);
	symTab.AddSymbol("LightDif0","keyword",T_STRING, eRegTypeConst,25);
	symTab.AddSymbol("LightSpec0","keyword",T_STRING, eRegTypeConst,31);
	symTab.AddSymbol("LightPos0","keyword",T_STRING, eRegTypeConst,20);

	symTab.AddSymbol("LightDir1","keyword",T_STRING, eRegTypeConst,37);
	symTab.AddSymbol("LightAmb1","keyword",T_STRING, eRegTypeConst,38);
	symTab.AddSymbol("LightDif1","keyword",T_STRING, eRegTypeConst,39);
	symTab.AddSymbol("LightSpec1","keyword",T_STRING, eRegTypeConst,40);
	symTab.AddSymbol("LightPos1","keyword",T_STRING, eRegTypeConst,41);

	symTab.AddSymbol("LightDir2","keyword",T_STRING, eRegTypeConst,42);
	symTab.AddSymbol("LightAmb2","keyword",T_STRING, eRegTypeConst,43);
	symTab.AddSymbol("LightDif2","keyword",T_STRING, eRegTypeConst,44);
	symTab.AddSymbol("LightSpec2","keyword",T_STRING, eRegTypeConst,45);
	symTab.AddSymbol("LightPos2","keyword",T_STRING, eRegTypeConst,46);

	symTab.AddSymbol("LightDir3","keyword",T_STRING, eRegTypeConst,47);
	symTab.AddSymbol("LightAmb3","keyword",T_STRING, eRegTypeConst,48);
	symTab.AddSymbol("LightDif3","keyword",T_STRING, eRegTypeConst,49);
	symTab.AddSymbol("LightSpec3","keyword",T_STRING, eRegTypeConst,50);
	symTab.AddSymbol("LightPos3","keyword",T_STRING, eRegTypeConst,51);
	
	//when there is only one material light combo, as in a simple demo app

	symTab.AddSymbol("PostCombinedDiffuse","keyword",T_STRING, eRegTypeConst,21);	
	
	symTab.AddSymbol("MatAmb","keyword",T_STRING, eRegTypeConst,22);
	//used in place of PostCombinedDiffuse now
	symTab.AddSymbol("MatDif","keyword",T_STRING, eRegTypeConst,21);
	symTab.AddSymbol("MatSpec","keyword",T_STRING, eRegTypeConst,27);
	symTab.AddSymbol("MatShininess","keyword",T_STRING, eRegTypeConst,30);
	
	//Vertex data
	symTab.AddSymbol("Pos","keyword",T_STRING,eRegTypeVertex, 0);
	symTab.AddSymbol("Normal","keyword",T_STRING,eRegTypeVertex, 3);
	symTab.AddSymbol("TexCoord","keyword",T_STRING,eRegTypeVertex, 6);

	//Output Regs
	symTab.AddSymbol("oPos","keyword",T_STRING,eRegTypeOutputPosition, 0 );
	symTab.AddSymbol("oColor","keyword",T_STRING,eRegTypeOutputColor, 0);
	symTab.AddSymbol("oFog","keyword",T_STRING,eRegTypeOutputFog, 0);
	symTab.AddSymbol("oTexture","keyword",T_STRING,eRegTypeOutputTexture, 0);
	
}

/*
	CodeGen()

	This function is the entry point to the other half of the compiler.
	Here, we create a code generator, give it a handle to a file to write to, 
	then pass it a program in the form of an AST
*/

void CodeGen(char *outputFileName)
{
	//call the codeGen class with an AST pointer
	FILE *fp = NULL;

	fp = fopen(outputFileName,"w");
	if(fp == NULL)
	{
		printf("output.vsh open for writing failed\n");
	}

	CCodeGenerator *pCodeGenerator = new CCodeGenerator();
	pCodeGenerator->SetFileHandle(fp);
	pCodeGenerator->HandleProgram(root);
	
	if(fp != NULL)	fclose(fp);
	delete pCodeGenerator;

}