/* A Bison parser, made from exporter.y
   by GNU bison 1.31.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	TKN_MD3_FILEHDR	257
# define	TKN_MD3_BONEFRAMES	258
# define	TKN_MD3_TAGS	259
# define	TKN_MD3_MESH	260
# define	TKN_MD3_TEXTURENAMES	261
# define	TKN_MD3_TRIANGLEVERTS	262
# define	TKN_MD3_TEXVEC	263
# define	TKN_MD3_VERTEX	264
# define	TKN_END	265
# define	TKN_ID	266
# define	TKN_VERSION	267
# define	TKN_FILENAME	268
# define	TKN_NUM_BONEFRAME	269
# define	TKN_NUM_TAGS	270
# define	TKN_NUM_MESH	271
# define	TKN_MAXTEX_NUM	272
# define	TKN_HEADER_SIZE	273
# define	TKN_TAGS_START	274
# define	TKN_MESH_START	275
# define	TKN_FILE_SIZE	276
# define	TKN_MINS	277
# define	TKN_MAXS	278
# define	TKN_POSITION	279
# define	TKN_SCALE	280
# define	TKN_CREATOR	281
# define	TKN_TAG_NAME	282
# define	TKN_ROTATION_MAT	283
# define	TKN_MESH_NAME	284
# define	TKN_NUM_MESHFRAMES	285
# define	TKN_NUM_SKINS	286
# define	TKN_NUM_VERTS	287
# define	TKN_NUM_TRIS	288
# define	TKN_TRI_START_ADDR	289
# define	TKN_TEXVEC_START_ADDR	290
# define	TKN_VERTEX_START_ADDR	291
# define	TKN_MESH_SIZE	292
# define	TKN_TEX_NAME	293
# define	TKN_VERT_IDX	294
# define	TKN_UV_OR_VEC	295
# define	TKN_VERT	296
# define	TKN_STRING	297
# define	TKN_INTEGER	298
# define	TKN_FLOAT	299

#line 27 "exporter.y"

/*
 *
 *  INCLUDES
 *
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "md3.h"

/*
 *
 *  MACROS
 *
 */

#ifdef _DEBUG  // do only in debug builds
#define YYDEBUG  1 // outputs debug info during parse
#endif


/*
 *
 *  GLOBAL VARIABLES
 *
 */

extern char *yytext;

extern FILE            *outFile;
extern MD3HEADER       hdr;
extern MD3TAG          *tag_data;
extern MD3BONEFRAME    *boneframe_data;
extern MD3MESHFILE     meshhdr_data;
extern char            *textureName_data;
extern TRIANGLEVERT    *triangle_data;
extern VEC2            *texvec_data;
extern MD3VERTEX       *vertex_data;

int boneframeIdx = 0;
int tagIdx = 0;
int texnamesIdx = 0;
int triIdx = 0;
int texvecIdx = 0;
int vertIdx = 0;

char *nextTextureName;


#line 79 "exporter.y"
#ifndef YYSTYPE
typedef union
{
   int      integer;     // for INTEGER token
   char     string[80];  // for STRING token
   float    fp;          // for FLOAT token
} yystype;
# define YYSTYPE yystype
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		157
#define	YYFLAG		-32768
#define	YYNTBASE	46

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 299 ? yytranslate[x] : 96)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     5,     6,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    60,    61,    66,    67,
      68,    69,    70,    71,    93,    94,    99,   100,   101,   102,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   169,   170,   175,   176,   180,   181,   186,   187,
     193,   194,   199,   200,   205,   206,   211,   212
};
static const short yyrhs[] =
{
      48,    60,    67,    47,     0,     0,    47,    72,    84,    87,
      90,    93,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     3,    12,    43,    49,    13,    44,
      50,    14,    43,    51,    15,    44,    52,    16,    44,    53,
      17,    44,    54,    18,    44,    55,    19,    44,    56,    20,
      44,    57,    21,    44,    58,    22,    44,    59,    11,     0,
       0,     4,    61,    62,    11,     0,     0,     0,     0,     0,
       0,    62,    23,    45,    45,    45,    63,    24,    45,    45,
      45,    64,    25,    45,    45,    45,    65,    26,    45,    66,
      27,    43,     0,     0,     5,    68,    69,    11,     0,     0,
       0,     0,    69,    28,    43,    70,    25,    45,    45,    45,
      71,    29,    45,    45,    45,    45,    45,    45,    45,    45,
      45,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     6,    12,    43,    73,    30,    43,    74,
      31,    44,    75,    32,    44,    76,    33,    44,    77,    34,
      44,    78,    35,    44,    79,    19,    44,    80,    36,    44,
      81,    37,    44,    82,    38,    44,    83,    11,     0,     0,
       7,    85,    86,    11,     0,     0,    86,    39,    43,     0,
       0,     8,    88,    89,    11,     0,     0,    89,    40,    44,
      44,    44,     0,     0,     9,    91,    92,    11,     0,     0,
      92,    41,    45,    45,     0,     0,    10,    94,    95,    11,
       0,     0,    95,    42,    44,    44,    44,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   117,   123,   124,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   158,   158,   171,   172,
     172,   172,   172,   172,   197,   197,   206,   207,   207,   207,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   270,   270,   287,   288,   297,   297,   312,   313,
     321,   321,   334,   335,   342,   342,   355,   356
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "TKN_MD3_FILEHDR", "TKN_MD3_BONEFRAMES", 
  "TKN_MD3_TAGS", "TKN_MD3_MESH", "TKN_MD3_TEXTURENAMES", 
  "TKN_MD3_TRIANGLEVERTS", "TKN_MD3_TEXVEC", "TKN_MD3_VERTEX", "TKN_END", 
  "TKN_ID", "TKN_VERSION", "TKN_FILENAME", "TKN_NUM_BONEFRAME", 
  "TKN_NUM_TAGS", "TKN_NUM_MESH", "TKN_MAXTEX_NUM", "TKN_HEADER_SIZE", 
  "TKN_TAGS_START", "TKN_MESH_START", "TKN_FILE_SIZE", "TKN_MINS", 
  "TKN_MAXS", "TKN_POSITION", "TKN_SCALE", "TKN_CREATOR", "TKN_TAG_NAME", 
  "TKN_ROTATION_MAT", "TKN_MESH_NAME", "TKN_NUM_MESHFRAMES", 
  "TKN_NUM_SKINS", "TKN_NUM_VERTS", "TKN_NUM_TRIS", "TKN_TRI_START_ADDR", 
  "TKN_TEXVEC_START_ADDR", "TKN_VERTEX_START_ADDR", "TKN_MESH_SIZE", 
  "TKN_TEX_NAME", "TKN_VERT_IDX", "TKN_UV_OR_VEC", "TKN_VERT", 
  "TKN_STRING", "TKN_INTEGER", "TKN_FLOAT", "md3_file", "md3_meshes", 
  "md3_filehdr", "@1", "@2", "@3", "@4", "@5", "@6", "@7", "@8", "@9", 
  "@10", "@11", "md3_boneframes", "@12", "md3_boneframe", "@13", "@14", 
  "@15", "@16", "md3_tags", "@17", "md3_tag", "@18", "@19", "md3_mesh", 
  "@20", "@21", "@22", "@23", "@24", "@25", "@26", "@27", "@28", "@29", 
  "@30", "md3_texturenames", "@31", "md3_texturename", "md3_triverts", 
  "@32", "md3_trivert", "md3_texvecs", "@33", "md3_texvec", 
  "md3_vertices", "@34", "md3_vertex", NULL
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    46,    47,    47,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    48,    61,    60,    62,    63,
      64,    65,    66,    62,    68,    67,    69,    70,    71,    69,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    72,    85,    84,    86,    86,    88,    87,    89,    89,
      91,    90,    92,    92,    94,    93,    95,    95
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     4,     0,     6,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    35,     0,     4,     0,     0,
       0,     0,     0,    21,     0,     4,     0,     0,     0,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    35,     0,     4,     0,     3,     0,     4,     0,     5,
       0,     4,     0,     4,     0,     4,     0,     5
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       0,     0,     0,     0,    16,     0,     4,    18,    24,     2,
       0,     0,    26,     1,     0,    17,     0,     0,     0,     0,
       5,     0,    25,     0,     0,    42,     0,     0,     0,    27,
      30,    44,    46,     0,     0,    19,     0,     0,     0,    48,
      50,     0,     6,     0,     0,     0,    43,     0,     0,    52,
      54,     3,     0,     0,     0,    31,    45,    47,     0,     0,
      56,     0,     0,     0,     0,     0,    51,     0,     0,     7,
       0,    28,     0,     0,     0,    55,     0,     0,    20,     0,
      32,    49,    53,     0,     0,     0,     0,     0,     0,     8,
       0,     0,     0,    57,     0,     0,     0,    33,     0,     0,
       0,     0,     9,    21,     0,     0,     0,     0,     0,    34,
       0,     0,     0,     0,    10,    22,     0,     0,     0,     0,
       0,    35,     0,     0,    29,     0,    11,    23,     0,     0,
      36,     0,     0,    12,     0,     0,    37,     0,     0,    13,
       0,     0,    38,     0,     0,    14,     0,     0,    39,    15,
       0,     0,    40,     0,    41,     0,     0,     0
};

static const short yydefgoto[] =
{
     155,    13,     2,    10,    27,    52,    77,    94,   106,   118,
     129,   135,   141,   147,     5,     7,    11,    43,    85,   107,
     119,     9,    12,    17,    36,    79,    19,    37,    64,    87,
     101,   113,   125,   132,   138,   144,   150,   153,    26,    31,
      38,    33,    39,    48,    41,    49,    59,    51,    60,    68
};

static const short yypact[] =
{
       3,    -5,     4,   -34,-32768,     5,-32768,-32768,-32768,-32768,
      -2,    -7,-32768,     6,   -31,-32768,   -30,    -8,     2,    10,
  -32768,   -27,-32768,   -24,   -22,-32768,    14,     9,   -21,-32768,
  -32768,-32768,-32768,    16,   -17,-32768,    11,    -3,    -6,-32768,
  -32768,    18,-32768,    13,   -16,    -4,-32768,    -1,   -10,-32768,
  -32768,-32768,    19,     0,     1,-32768,-32768,-32768,     7,    -9,
  -32768,     8,    12,    15,    17,    20,-32768,    21,   -11,-32768,
      22,-32768,    24,    25,    26,-32768,    28,    27,-32768,    29,
  -32768,-32768,-32768,    30,    31,    34,    32,    23,    35,-32768,
      33,    36,    38,-32768,    37,    39,    40,-32768,    42,    43,
      44,    47,-32768,-32768,    45,    48,    52,    50,    46,-32768,
      49,    51,    53,    60,-32768,-32768,    54,    56,    64,    68,
      57,-32768,    59,    58,-32768,    62,-32768,-32768,    61,    41,
  -32768,    63,    85,-32768,    65,    66,-32768,    67,    70,-32768,
      69,    86,-32768,    71,    73,-32768,    72,   101,-32768,-32768,
      76,    74,-32768,   106,-32768,   119,   120,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768
};


#define	YYLAST		120


static const short yytable[] =
{
      75,    57,    66,    22,    15,    46,     1,     3,     4,     6,
       8,    14,    18,    20,    24,    21,    16,    25,    28,    29,
      23,    30,    32,    34,    35,    40,    42,    45,    50,    54,
      58,    76,    67,    47,    61,     0,    44,    53,     0,    55,
       0,     0,    56,    84,     0,    62,    63,     0,    72,     0,
       0,    65,    69,     0,    98,    92,     0,    70,    86,    90,
      71,   131,     0,     0,    73,     0,    74,    78,    80,    81,
     110,    82,    83,     0,    88,    89,   111,    91,    95,    93,
     105,    96,    97,   122,    99,   100,   102,   137,   103,   104,
     108,   112,   109,   114,   117,   123,   115,   128,   116,   120,
     121,   127,   124,   126,   134,   130,   140,   133,   143,   136,
     146,   139,   149,   142,   151,   145,   148,   154,   152,   156,
     157
};

static const short yycheck[] =
{
      11,    11,    11,    11,    11,    11,     3,    12,     4,    43,
       5,    13,     6,    44,    12,    45,    23,     7,    45,    43,
      28,    43,     8,    14,    45,     9,    43,    30,    10,    45,
      40,    42,    41,    39,    15,    -1,    25,    24,    -1,    43,
      -1,    -1,    43,    16,    -1,    45,    45,    -1,    31,    -1,
      -1,    44,    44,    -1,    17,    32,    -1,    45,    29,    25,
      45,    20,    -1,    -1,    44,    -1,    45,    45,    44,    44,
      18,    45,    44,    -1,    44,    44,    26,    45,    45,    44,
      33,    45,    44,    19,    45,    45,    44,    21,    45,    45,
      45,    45,    44,    44,    34,    27,    45,    35,    45,    45,
      44,    43,    45,    44,    19,    44,    36,    44,    22,    44,
      37,    44,    11,    44,    38,    44,    44,    11,    44,     0,
       0
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001 Free Software Foundation, Inc.

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

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#ifdef __cplusplus
# define YYSTD(x) std::x
#else
# define YYSTD(x) x
#endif

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
#  define YYSIZE_T YYSTD (size_t)
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#    define YYSIZE_T YYSTD (size_t)
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
#  ifdef __cplusplus
#   include <cstdlib> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T std::size_t
#  else
#   ifdef __STDC__
#    include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#    define YYSIZE_T size_t
#   endif
#  endif
#  define YYSTACK_ALLOC YYSTD (malloc)
#  define YYSTACK_FREE YYSTD (free)
# endif

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Relocate the TYPE STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Type, Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	yymemcpy ((char *) yyptr, (char *) (Stack),			\
		  yysize * (YYSIZE_T) sizeof (Type));			\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (Type) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# ifdef __cplusplus
#  include <cstddef> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T std::size_t
# else
#  ifdef __STDC__
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
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
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  ifdef __cplusplus
#   include <cstdio>  /* INFRINGES ON USER NAME SPACE */
#  else
#   include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYFPRINTF YYSTD (fprintf)
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace. [The following comment makes no
   sense to me.  Could someone clarify it?  --akim] Since this is
   uninitialized, it does not stop multiple parsers from coexisting.
   */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
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

#if ! defined (yyoverflow) && ! defined (yymemcpy)
# if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#  define yymemcpy __builtin_memcpy
# else				/* not GNU C or C++ */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
#  if defined (__STDC__) || defined (__cplusplus)
yymemcpy (char *yyto, const char *yyfrom, YYSIZE_T yycount)
#  else
yymemcpy (yyto, yyfrom, yycount)
     char *yyto;
     const char *yyfrom;
     YYSIZE_T yycount;
#  endif
{
  register const char *yyf = yyfrom;
  register char *yyt = yyto;
  register YYSIZE_T yyi = yycount;

  while (yyi-- != 0)
    *yyt++ = *yyf++;
}
# endif
#endif

#ifdef YYERROR_VERBOSE

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
#endif

#line 341 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# ifdef __cplusplus
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else /* !__cplusplus */
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif /* !__cplusplus */
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
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
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
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

  if (yyssp >= yyss + yystacksize - 1)
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
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (short, yyss);
	YYSTACK_RELOCATE (YYSTYPE, yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (YYLTYPE, yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
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
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
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
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 4:
#line 132 "exporter.y"
{ strncpy (hdr.id, yyvsp[0].string, 4);;
    break;}
case 5:
#line 134 "exporter.y"
{ hdr.iVersion = yyvsp[0].integer; ;
    break;}
case 6:
#line 136 "exporter.y"
{ strcpy (hdr.cFileName, yyvsp[0].string);;
    break;}
case 7:
#line 138 "exporter.y"
{ hdr.iBoneFrameNum = yyvsp[0].integer; ;
    break;}
case 8:
#line 140 "exporter.y"
{ hdr.iTagNum = yyvsp[0].integer; ;
    break;}
case 9:
#line 142 "exporter.y"
{ hdr.iMeshNum = yyvsp[0].integer; ;
    break;}
case 10:
#line 144 "exporter.y"
{ hdr.iMaxTextureNum = yyvsp[0].integer; ;
    break;}
case 11:
#line 146 "exporter.y"
{ hdr.iHeaderSize = yyvsp[0].integer; ;
    break;}
case 12:
#line 148 "exporter.y"
{ hdr.iTagStart = yyvsp[0].integer; ;
    break;}
case 13:
#line 150 "exporter.y"
{ hdr.iMeshStart = yyvsp[0].integer; ;
    break;}
case 14:
#line 152 "exporter.y"
{ hdr.iFileSize = yyvsp[0].integer; ;
    break;}
case 15:
#line 154 "exporter.y"
{ md3BinOutHeader (outFile, &hdr);
                              ;
    break;}
case 16:
#line 159 "exporter.y"
{ int size = sizeof (MD3BONEFRAME) *
                                    hdr.iBoneFrameNum;
                                boneframe_data = (MD3BONEFRAME *)
                                    malloc (sizeof (MD3BONEFRAME) *
                                    hdr.iBoneFrameNum); 
                              ;
    break;}
case 17:
#line 167 "exporter.y"
{ md3BinOutBoneFrames (outFile, boneframe_data, hdr.iBoneFrameNum, &hdr);
                              ;
    break;}
case 19:
#line 173 "exporter.y"
{ boneframe_data[ boneframeIdx ].fMins[0] = yyvsp[-2].fp;
                                boneframe_data[ boneframeIdx ].fMins[1] = yyvsp[-1].fp;
                                boneframe_data[ boneframeIdx ].fMins[2] = yyvsp[0].fp;
                              ;
    break;}
case 20:
#line 178 "exporter.y"
{ boneframe_data[ boneframeIdx ].fMaxs[0] = yyvsp[-2].fp;
                                boneframe_data[ boneframeIdx ].fMaxs[1] = yyvsp[-1].fp;
                                boneframe_data[ boneframeIdx ].fMaxs[2] = yyvsp[0].fp;
                              ;
    break;}
case 21:
#line 183 "exporter.y"
{ boneframe_data[ boneframeIdx ].pos[0] = yyvsp[-2].fp;
                                boneframe_data[ boneframeIdx ].pos[1] = yyvsp[-1].fp;
                                boneframe_data[ boneframeIdx ].pos[2] = yyvsp[0].fp;
                              ;
    break;}
case 22:
#line 188 "exporter.y"
{ boneframe_data[ boneframeIdx ].scale = yyvsp[0].fp;
                              ;
    break;}
case 23:
#line 191 "exporter.y"
{ strncpy (boneframe_data[ boneframeIdx ].creator, yyvsp[0].string, 15);
                                boneframe_data[ boneframeIdx ].creator[15] = '\0';
                                boneframeIdx++;
                              ;
    break;}
case 24:
#line 198 "exporter.y"
{ int size = sizeof (MD3TAG) * hdr.iTagNum * hdr.iBoneFrameNum;
                                tag_data = (MD3TAG *) malloc (sizeof (MD3TAG) * hdr.iTagNum * hdr.iBoneFrameNum); ;
    break;}
case 25:
#line 202 "exporter.y"
{ md3BinOutTags (outFile, tag_data, hdr.iTagNum * hdr.iBoneFrameNum, &hdr);
                              ;
    break;}
case 27:
#line 208 "exporter.y"
{ strncpy (tag_data[ tagIdx ].cTagName, yyvsp[0].string, 12);
                                tag_data[ tagIdx ].cTagName[11] = '\0';
                              ;
    break;}
case 28:
#line 212 "exporter.y"
{ tag_data[ tagIdx ].pos[0] = yyvsp[-2].fp;
                                tag_data[ tagIdx ].pos[1] = yyvsp[-1].fp;
                                tag_data[ tagIdx ].pos[2] = yyvsp[0].fp;
                              ;
    break;}
case 29:
#line 217 "exporter.y"
{ tag_data[ tagIdx ].fMatrix[0][0] = yyvsp[-8].fp;
                                tag_data[ tagIdx ].fMatrix[0][1] = yyvsp[-7].fp;
                                tag_data[ tagIdx ].fMatrix[0][2] = yyvsp[-6].fp;
                                tag_data[ tagIdx ].fMatrix[1][0] = yyvsp[-5].fp;
                                tag_data[ tagIdx ].fMatrix[1][1] = yyvsp[-4].fp;
                                tag_data[ tagIdx ].fMatrix[1][2] = yyvsp[-3].fp;
                                tag_data[ tagIdx ].fMatrix[2][0] = yyvsp[-2].fp;
                                tag_data[ tagIdx ].fMatrix[2][1] = yyvsp[-1].fp;
                                tag_data[ tagIdx ].fMatrix[2][2] = yyvsp[0].fp;
                                tagIdx++;
                              ;
    break;}
case 30:
#line 232 "exporter.y"
{ strncpy (meshhdr_data.cId, yyvsp[0].string, 4);
                              ;
    break;}
case 31:
#line 235 "exporter.y"
{ strncpy (meshhdr_data.cName, yyvsp[0].string, 68);
                                meshhdr_data.cName[67] = '\0';
                              ;
    break;}
case 32:
#line 239 "exporter.y"
{ meshhdr_data.iMeshFrameNum = yyvsp[0].integer;
                              ;
    break;}
case 33:
#line 242 "exporter.y"
{ meshhdr_data.iTextureNum = yyvsp[0].integer;
                              ;
    break;}
case 34:
#line 245 "exporter.y"
{ meshhdr_data.iVertexNum = yyvsp[0].integer;
                              ;
    break;}
case 35:
#line 248 "exporter.y"
{ meshhdr_data.iTriangleNum = yyvsp[0].integer;
                              ;
    break;}
case 36:
#line 251 "exporter.y"
{ meshhdr_data.iTriangleStart = yyvsp[0].integer;
                              ;
    break;}
case 37:
#line 254 "exporter.y"
{ meshhdr_data.iHeaderSize = yyvsp[0].integer;
                              ;
    break;}
case 38:
#line 257 "exporter.y"
{ meshhdr_data.iTecVecStart = yyvsp[0].integer;
                              ;
    break;}
case 39:
#line 260 "exporter.y"
{ meshhdr_data.iVertexStart = yyvsp[0].integer;
                              ;
    break;}
case 40:
#line 263 "exporter.y"
{ meshhdr_data.iMeshSize = yyvsp[0].integer;
                              ;
    break;}
case 41:
#line 266 "exporter.y"
{ md3BinOutMeshHeader (outFile, &meshhdr_data);
                              ;
    break;}
case 42:
#line 271 "exporter.y"
{ int size = sizeof(char) * 68 * meshhdr_data.iTextureNum;

                                textureName_data = (char *) malloc (size);

                                if (textureName_data == NULL)
                                    YYERROR;

                                nextTextureName = textureName_data;
                              ;
    break;}
case 43:
#line 282 "exporter.y"
{ md3BinOutTextureNames (outFile, textureName_data, meshhdr_data.iTextureNum, &meshhdr_data);
                                free (textureName_data);
                              ;
    break;}
case 45:
#line 289 "exporter.y"
{  strncpy (nextTextureName, yyvsp[0].string, 68);
                                 nextTextureName[67] = '\0';
                                 texnamesIdx++;
                                 nextTextureName += 68 * texnamesIdx;
                              ;
    break;}
case 46:
#line 298 "exporter.y"
{
                                 int size = sizeof (TRIANGLEVERT);
                                 triangle_data = (TRIANGLEVERT *) malloc (sizeof (TRIANGLEVERT) * meshhdr_data.iTriangleNum);
                                 if (triangle_data == NULL)
                                     YYERROR;
                                 triIdx = 0;
                              ;
    break;}
case 47:
#line 307 "exporter.y"
{ md3BinOutTriangleData (outFile, triangle_data, meshhdr_data.iTriangleNum,  &meshhdr_data);
                                free (triangle_data);
                              ;
    break;}
case 49:
#line 314 "exporter.y"
{ triangle_data[ triIdx ][0] = yyvsp[-2].integer;
                             triangle_data[ triIdx ][1] = yyvsp[-1].integer; 
                             triangle_data[ triIdx ][2] = yyvsp[0].integer;
                             triIdx++;
                           ;
    break;}
case 50:
#line 322 "exporter.y"
{  texvec_data = (VEC2 *) malloc (sizeof (VEC2) * meshhdr_data.iVertexNum);
                                 if (texvec_data == NULL)
                                    YYERROR;
                                 texvecIdx = 0;
                              ;
    break;}
case 51:
#line 329 "exporter.y"
{ md3BinOutTexVecData (outFile, texvec_data, meshhdr_data.iVertexNum,  &meshhdr_data);
                                free (texvec_data);
                              ;
    break;}
case 53:
#line 336 "exporter.y"
{ texvec_data[ texvecIdx ][0] = yyvsp[-1].fp;
                                texvec_data[ texvecIdx ][1] = yyvsp[0].fp;
                                texvecIdx++;
                              ;
    break;}
case 54:
#line 343 "exporter.y"
{ vertex_data = (MD3VERTEX *) malloc (sizeof (MD3VERTEX) * meshhdr_data.iVertexNum);
                                if (vertex_data == NULL)
                                   YYERROR;
                                vertIdx = 0;
                              ;
    break;}
case 55:
#line 350 "exporter.y"
{ md3BinOutVertexData (outFile, vertex_data, meshhdr_data.iVertexNum,  &meshhdr_data);
                                free (vertex_data);
                              ;
    break;}
case 57:
#line 357 "exporter.y"
{ vertex_data[ vertIdx ].vec[0] = yyvsp[-2].integer;
                                vertex_data[ vertIdx ].vec[1] = yyvsp[-1].integer;
                                vertex_data[ vertIdx ].vec[2] = yyvsp[0].integer;
                                vertIdx++;
                              ;
    break;}
}

#line 727 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
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
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
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

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 364 "exporter.y"

/* C code section for yacc specification */

/*
 *-----------------------------------------------------------------------------
 *
 * FUNCTION: yyerror
 *    Call if a parse error is encountered.
 *    
 *
 *
 * RETURN:
 *    0 to signal error and stop parse.
 *
 * OUTPUTS:
 *    none
 *
 * INPUTS:
 *    none
 *
 *-----------------------------------------------------------------------------
 */
int yyerror (const char *msg)
{
   printf ("%s at '%s'\n", msg, yytext);

   return 0;
}