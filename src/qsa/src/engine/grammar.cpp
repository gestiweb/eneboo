
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse         qsyyparse
#define yylex           qsyylex
#define yyerror         qsyyerror
#define yylval          qsyylval
#define yychar          qsyychar
#define yydebug         qsyydebug
#define yynerrs         qsyynerrs
#define yylloc          qsyylloc

/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "grammar.y"

/****************************************************************************
** $Id$
**
** Copyright (C) 2001-2003 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt Script for Applications framework (QSA).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding a valid Qt Script for Applications license may use
** this file in accordance with the Qt Script for Applications License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about QSA Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>
#include "qsengine.h"
#include "qsnodes.h"
#include "qslexer.h"

/* default values for bison */
#define YYMAXDEPTH 10000
#define YYENABLE_NLS 0
#define YYDEBUG 0

#ifdef QSDEBUGGER
# define YYERROR_VERBOSE
#define DBG(l, s, e) { l->setLoc(s.first_line, e.last_line); } // location
#else
# undef YYLSP_NEEDED
# define DBG(l, s, e)
#endif

extern int yylex();
static int yyerror (const char *);
static bool automatic();



/* Line 189 of yacc.c  */
#line 138 "grammar.tab.c"

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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NULLTOKEN = 258,
     TRUETOKEN = 259,
     FALSETOKEN = 260,
     BREAK = 261,
     CASE = 262,
     DEFAULT = 263,
     FOR = 264,
     NEW = 265,
     VAR = 266,
     CONTINUE = 267,
     FUNCTION = 268,
     RETURN = 269,
     QS_VOID = 270,
     QS_DELETE = 271,
     IF = 272,
     THIS = 273,
     DO = 274,
     WHILE = 275,
     ELSE = 276,
     QS_IN = 277,
     INSTANCEOF = 278,
     TYPEOF = 279,
     IS = 280,
     SWITCH = 281,
     WITH = 282,
     RESERVED = 283,
     THROW = 284,
     TRY = 285,
     CATCH = 286,
     FINALLY = 287,
     CLASS = 288,
     CONSTRUCTOR = 289,
     EXTENDS = 290,
     ABSTRACT = 291,
     FINAL = 292,
     PRIVATE = 293,
     PUBLIC = 294,
     STATIC = 295,
     QS_CONST = 296,
     PACKAGE = 297,
     IMPORT = 298,
     IF_WITHOUT_ELSE = 299,
     EQEQ = 300,
     NE = 301,
     STREQ = 302,
     STRNEQ = 303,
     LE = 304,
     GE = 305,
     OR = 306,
     AND = 307,
     PLUSPLUS = 308,
     MINUSMINUS = 309,
     LSHIFT = 310,
     RSHIFT = 311,
     URSHIFT = 312,
     PLUSEQUAL = 313,
     MINUSEQUAL = 314,
     MULTEQUAL = 315,
     DIVEQUAL = 316,
     LSHIFTEQUAL = 317,
     RSHIFTEQUAL = 318,
     URSHIFTEQUAL = 319,
     ANDEQUAL = 320,
     MODEQUAL = 321,
     XOREQUAL = 322,
     OREQUAL = 323,
     NUMBER = 324,
     STRING = 325,
     IDENT = 326
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 57 "grammar.y"

  int                   ival;
  double                dval;
  const QString         *ustr;
  QSNode                *node;
  QSStatementNode       *stat;
  QSParameterNode       *param;
  QSFunctionBodyNode    *body;
  QSFuncDeclNode        *func;
  QSClassDefNode        *clss;
  QSProgramNode         *prog;
  QSAssignExprNode      *init;
  QSSourceElementNode   *src;
  QSSourceElementsNode  *srcs;
  QSStatListNode        *slist;
  QSArgumentsNode       *args;
  QSArgumentListNode    *alist;
  QSCaseBlockNode       *cblk;
  QSClauseListNode      *clist;
  QSCaseClauseNode      *ccl;
  QSElementNode         *elm;
  QSElisionNode         *eli;
  QSTypeNode            *type;
  QSTypedVarNode        *tvar;
  QSVarBindingNode      *vbin;
  QSVarBindingListNode  *blist;
  QSAttributeNode       *attrs;
  QSAttribute         	attr;
  Operator            	op;



/* Line 214 of yacc.c  */
#line 278 "grammar.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 303 "grammar.tab.c"

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
# if YYENABLE_NLS
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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  216
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1603

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  96
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  108
/* YYNRULES -- Number of rules.  */
#define YYNRULES  321
/* YYNRULES -- Number of states.  */
#define YYNSTATES  561

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   326

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    85,     2,     2,     2,    87,    90,     2,
      77,    78,    86,    82,    74,    83,    81,    72,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    73,    95,
      88,    94,    89,    93,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    79,     2,    80,    91,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    75,    92,    76,    84,     2,     2,     2,
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
      65,    66,    67,    68,    69,    70,    71
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    21,    25,    31,    33,    36,    40,    45,    47,    49,
      51,    53,    57,    61,    65,    71,    74,    79,    80,    82,
      84,    87,    89,    91,    96,   100,   104,   106,   111,   115,
     119,   121,   124,   126,   129,   132,   135,   140,   144,   147,
     150,   155,   159,   162,   166,   168,   172,   174,   176,   178,
     180,   182,   185,   188,   190,   193,   196,   199,   202,   205,
     208,   211,   214,   217,   220,   223,   225,   227,   229,   231,
     233,   237,   241,   245,   247,   251,   255,   259,   261,   265,
     269,   271,   275,   279,   281,   285,   289,   293,   295,   299,
     303,   307,   309,   313,   317,   321,   325,   329,   333,   337,
     339,   343,   347,   351,   355,   359,   361,   365,   369,   373,
     377,   381,   385,   389,   391,   395,   399,   403,   407,   409,
     413,   417,   421,   425,   427,   431,   435,   439,   443,   445,
     449,   451,   455,   457,   461,   463,   467,   469,   473,   475,
     479,   481,   485,   487,   491,   493,   497,   499,   503,   505,
     509,   511,   515,   517,   521,   523,   527,   529,   533,   535,
     541,   543,   549,   551,   557,   559,   563,   565,   569,   571,
     575,   577,   579,   581,   583,   585,   587,   589,   591,   593,
     595,   597,   599,   601,   605,   607,   611,   613,   617,   619,
     621,   623,   626,   628,   630,   632,   634,   636,   638,   640,
     642,   644,   646,   648,   650,   652,   654,   657,   661,   663,
     666,   668,   671,   674,   680,   688,   696,   704,   710,   720,
     731,   739,   748,   749,   751,   752,   754,   757,   760,   764,
     768,   771,   774,   778,   782,   785,   788,   792,   796,   802,
     808,   812,   818,   819,   821,   823,   826,   830,   835,   838,
     842,   848,   852,   856,   860,   864,   869,   875,   878,   882,
     886,   888,   892,   899,   907,   915,   924,   925,   928,   933,
     939,   941,   945,   949,   955,   958,   962,   966,   970,   976,
     977,   979,   981,   984,   986,   988,   990,   993,   995,   997,
     999,  1001,  1003,  1005,  1007,  1009,  1013,  1017,  1022,  1027,
    1029,  1031,  1033,  1037,  1039,  1043,  1045,  1049,  1051,  1055,
    1057,  1061
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     191,     0,    -1,     3,    -1,     4,    -1,     5,    -1,    69,
      -1,    70,    -1,    72,    -1,    71,    -1,    70,    -1,    69,
      -1,    98,    73,   151,    -1,    99,    74,    98,    73,   151,
      -1,   101,    -1,    75,    76,    -1,    75,    99,    76,    -1,
      75,    99,    74,    76,    -1,    18,    -1,    97,    -1,   102,
      -1,    71,    -1,    77,   155,    78,    -1,    79,   104,    80,
      -1,    79,   103,    80,    -1,    79,   103,    74,   104,    80,
      -1,   104,   151,    -1,   103,    74,   104,   151,    -1,    -1,
     105,    -1,    74,    -1,   105,    74,    -1,   100,    -1,   187,
      -1,   106,    79,   155,    80,    -1,   106,    81,    71,    -1,
      10,   106,   112,    -1,   101,    -1,   107,    79,   155,    80,
      -1,   107,    81,    71,    -1,    10,   106,   112,    -1,   106,
      -1,    10,   108,    -1,   107,    -1,    10,   108,    -1,   106,
     112,    -1,   110,   112,    -1,   110,    79,   155,    80,    -1,
     110,    81,    71,    -1,   107,   112,    -1,   111,   112,    -1,
     111,    79,   155,    80,    -1,   111,    81,    71,    -1,    77,
      78,    -1,    77,   113,    78,    -1,   151,    -1,   113,    74,
     151,    -1,   108,    -1,   110,    -1,   109,    -1,   111,    -1,
     114,    -1,   114,    53,    -1,   114,    54,    -1,   115,    -1,
     115,    53,    -1,   115,    54,    -1,    16,   119,    -1,    15,
     119,    -1,    24,   119,    -1,    53,   119,    -1,    54,   119,
      -1,    82,   119,    -1,    83,   119,    -1,    84,   119,    -1,
      85,   119,    -1,   116,    -1,   118,    -1,   117,    -1,   118,
      -1,   119,    -1,   121,    86,   119,    -1,   121,    72,   119,
      -1,   121,    87,   119,    -1,   120,    -1,   122,    86,   119,
      -1,   122,    72,   119,    -1,   122,    87,   119,    -1,   121,
      -1,   123,    82,   121,    -1,   123,    83,   121,    -1,   122,
      -1,   124,    82,   121,    -1,   124,    83,   121,    -1,   123,
      -1,   125,    55,   123,    -1,   125,    56,   123,    -1,   125,
      57,   123,    -1,   124,    -1,   126,    55,   123,    -1,   126,
      56,   123,    -1,   126,    57,   123,    -1,   125,    -1,   127,
      88,   125,    -1,   127,    89,   125,    -1,   127,    49,   125,
      -1,   127,    50,   125,    -1,   127,    25,   125,    -1,   127,
      23,   125,    -1,   127,    22,   125,    -1,   125,    -1,   128,
      88,   125,    -1,   128,    89,   125,    -1,   128,    49,   125,
      -1,   128,    50,   125,    -1,   128,    23,   125,    -1,   126,
      -1,   129,    88,   125,    -1,   129,    89,   125,    -1,   129,
      49,   125,    -1,   129,    50,   125,    -1,   129,    25,   125,
      -1,   129,    23,   125,    -1,   129,    22,   125,    -1,   127,
      -1,   130,    45,   127,    -1,   130,    46,   127,    -1,   130,
      47,   127,    -1,   130,    48,   127,    -1,   128,    -1,   131,
      45,   128,    -1,   131,    46,   128,    -1,   131,    47,   128,
      -1,   131,    48,   128,    -1,   129,    -1,   132,    45,   127,
      -1,   132,    46,   127,    -1,   132,    47,   127,    -1,   132,
      48,   127,    -1,   130,    -1,   133,    90,   130,    -1,   131,
      -1,   134,    90,   131,    -1,   132,    -1,   135,    90,   130,
      -1,   133,    -1,   136,    91,   133,    -1,   134,    -1,   137,
      91,   134,    -1,   135,    -1,   138,    91,   133,    -1,   136,
      -1,   139,    92,   136,    -1,   137,    -1,   140,    92,   137,
      -1,   138,    -1,   141,    92,   136,    -1,   139,    -1,   142,
      52,   139,    -1,   140,    -1,   143,    52,   140,    -1,   141,
      -1,   144,    52,   139,    -1,   142,    -1,   145,    51,   142,
      -1,   143,    -1,   146,    51,   143,    -1,   144,    -1,   147,
      51,   142,    -1,   145,    -1,   145,    93,   151,    73,   151,
      -1,   146,    -1,   146,    93,   152,    73,   152,    -1,   147,
      -1,   147,    93,   151,    73,   151,    -1,   148,    -1,   114,
     154,   151,    -1,   149,    -1,   114,   154,   152,    -1,   150,
      -1,   115,   154,   151,    -1,    94,    -1,    58,    -1,    59,
      -1,    60,    -1,    61,    -1,    62,    -1,    63,    -1,    64,
      -1,    65,    -1,    67,    -1,    68,    -1,    66,    -1,   151,
      -1,   155,    74,   151,    -1,   152,    -1,   156,    74,   152,
      -1,   153,    -1,   157,    74,   151,    -1,   159,    -1,   196,
      -1,   190,    -1,   194,   190,    -1,   182,    -1,   161,    -1,
     162,    -1,   163,    -1,   164,    -1,   167,    -1,   168,    -1,
     169,    -1,   170,    -1,   171,    -1,   177,    -1,   178,    -1,
     179,    -1,   183,    -1,    75,    76,    -1,    75,   160,    76,
      -1,   158,    -1,   160,   158,    -1,    95,    -1,   157,    95,
      -1,   157,     1,    -1,    17,    77,   155,    78,   158,    -1,
      17,    77,   155,    78,   158,    21,   158,    -1,    19,   158,
      20,    77,   155,    78,    95,    -1,    19,   158,    20,    77,
     155,    78,     1,    -1,    20,    77,   155,    78,   158,    -1,
       9,    77,   166,    95,   165,    95,   165,    78,   158,    -1,
       9,    77,    11,   199,    95,   165,    95,   165,    78,   158,
      -1,     9,    77,   114,    22,   155,    78,   158,    -1,     9,
      77,    11,   201,    22,   155,    78,   158,    -1,    -1,   155,
      -1,    -1,   156,    -1,    12,    95,    -1,    12,     1,    -1,
      12,    71,    95,    -1,    12,    71,     1,    -1,     6,    95,
      -1,     6,     1,    -1,     6,    71,    95,    -1,     6,    71,
       1,    -1,    14,    95,    -1,    14,     1,    -1,    14,   155,
      95,    -1,    14,   155,     1,    -1,    27,    77,   155,    78,
     158,    -1,    26,    77,   155,    78,   172,    -1,    75,   173,
      76,    -1,    75,   173,   176,   173,    76,    -1,    -1,   174,
      -1,   175,    -1,   174,   175,    -1,     7,   155,    73,    -1,
       7,   155,    73,   160,    -1,     8,    73,    -1,     8,    73,
     160,    -1,    71,    73,    71,    94,     1,    -1,    71,    73,
     158,    -1,    29,   155,    95,    -1,    30,   159,   180,    -1,
      30,   159,   181,    -1,    30,   159,   180,   181,    -1,    31,
      77,    71,    78,   159,    -1,    32,   159,    -1,    42,   184,
     189,    -1,    43,   184,    95,    -1,    71,    -1,   184,    81,
      71,    -1,    13,    71,    77,    78,   186,   189,    -1,    13,
      71,    77,   188,    78,   186,   189,    -1,   194,    13,    71,
      77,    78,   186,   189,    -1,   194,    13,    71,    77,   188,
      78,   186,   189,    -1,    -1,    73,   203,    -1,    13,    77,
      78,   189,    -1,    13,    77,   188,    78,   189,    -1,    71,
      -1,    71,    73,   203,    -1,   188,    74,    71,    -1,   188,
      74,    71,    73,   203,    -1,    75,    76,    -1,    75,   192,
      76,    -1,    33,    71,    95,    -1,    33,    71,   189,    -1,
      33,    71,    35,   203,   189,    -1,    -1,   192,    -1,   193,
      -1,   192,   193,    -1,   158,    -1,   185,    -1,   195,    -1,
     194,   195,    -1,    36,    -1,    37,    -1,    38,    -1,    39,
      -1,    40,    -1,     4,    -1,     5,    -1,    34,    -1,   197,
     198,    95,    -1,   197,   198,     1,    -1,   194,   197,   198,
      95,    -1,   194,   197,   198,     1,    -1,    11,    -1,    41,
      -1,   200,    -1,   198,    74,   200,    -1,   201,    -1,   199,
      74,   201,    -1,   202,    -1,   202,    94,   151,    -1,   202,
      -1,   202,    94,   152,    -1,    71,    -1,    71,    73,   203,
      -1,    71,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   195,   195,   196,   197,   198,   199,   200,   204,   205,
     206,   210,   211,   216,   217,   218,   219,   224,   225,   226,
     227,   228,   232,   233,   234,   238,   239,   244,   245,   249,
     250,   254,   255,   256,   257,   258,   262,   263,   264,   265,
     269,   270,   274,   275,   279,   280,   281,   282,   286,   287,
     288,   289,   293,   294,   298,   299,   303,   304,   308,   309,
     313,   314,   315,   319,   320,   321,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   337,   338,   342,   343,   347,
     348,   349,   350,   354,   355,   357,   359,   364,   365,   366,
     370,   371,   373,   378,   379,   380,   381,   385,   386,   387,
     388,   392,   393,   394,   395,   396,   397,   398,   399,   403,
     404,   405,   406,   407,   408,   413,   414,   415,   416,   417,
     418,   419,   421,   425,   426,   427,   428,   429,   433,   434,
     436,   438,   440,   445,   446,   448,   450,   452,   457,   458,
     462,   463,   468,   469,   473,   474,   478,   479,   484,   485,
     490,   491,   495,   496,   501,   502,   507,   508,   512,   513,
     518,   519,   524,   525,   529,   530,   535,   536,   541,   542,
     547,   548,   553,   554,   559,   560,   565,   566,   571,   572,
     577,   578,   579,   580,   581,   582,   583,   584,   585,   586,
     587,   588,   592,   593,   597,   598,   602,   603,   607,   608,
     609,   610,   611,   612,   613,   614,   615,   616,   617,   618,
     619,   620,   621,   622,   623,   624,   628,   629,   633,   634,
     638,   642,   643,   647,   648,   652,   653,   655,   656,   658,
     660,   662,   667,   668,   672,   673,   677,   678,   679,   680,
     684,   685,   686,   687,   691,   692,   693,   694,   698,   702,
     706,   707,   712,   713,   717,   718,   722,   723,   727,   728,
     732,   733,   737,   741,   742,   743,   747,   751,   755,   759,
     763,   764,   768,   770,   772,   774,   780,   781,   785,   786,
     791,   792,   793,   794,   799,   800,   804,   805,   806,   811,
     812,   816,   817,   821,   822,   826,   827,   831,   832,   833,
     834,   835,   836,   837,   838,   844,   846,   848,   851,   857,
     858,   862,   863,   868,   869,   874,   875,   879,   880,   885,
     886,   892
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NULLTOKEN", "TRUETOKEN", "FALSETOKEN",
  "BREAK", "CASE", "DEFAULT", "FOR", "NEW", "VAR", "CONTINUE", "FUNCTION",
  "RETURN", "QS_VOID", "QS_DELETE", "IF", "THIS", "DO", "WHILE", "ELSE",
  "QS_IN", "INSTANCEOF", "TYPEOF", "IS", "SWITCH", "WITH", "RESERVED",
  "THROW", "TRY", "CATCH", "FINALLY", "CLASS", "CONSTRUCTOR", "EXTENDS",
  "ABSTRACT", "FINAL", "PRIVATE", "PUBLIC", "STATIC", "QS_CONST",
  "PACKAGE", "IMPORT", "IF_WITHOUT_ELSE", "EQEQ", "NE", "STREQ", "STRNEQ",
  "LE", "GE", "OR", "AND", "PLUSPLUS", "MINUSMINUS", "LSHIFT", "RSHIFT",
  "URSHIFT", "PLUSEQUAL", "MINUSEQUAL", "MULTEQUAL", "DIVEQUAL",
  "LSHIFTEQUAL", "RSHIFTEQUAL", "URSHIFTEQUAL", "ANDEQUAL", "MODEQUAL",
  "XOREQUAL", "OREQUAL", "NUMBER", "STRING", "IDENT", "'/'", "':'", "','",
  "'{'", "'}'", "'('", "')'", "'['", "']'", "'.'", "'+'", "'-'", "'~'",
  "'!'", "'*'", "'%'", "'<'", "'>'", "'&'", "'^'", "'|'", "'?'", "'='",
  "';'", "$accept", "Literal", "PropertyName", "PropertyNameAndValueList",
  "PrimaryExpr", "PrimaryExprNoBrace", "ArrayLiteral", "ElementList",
  "ElisionOpt", "Elision", "MemberExpr", "MemberExprNoBF", "NewExpr",
  "NewExprNoBF", "CallExpr", "CallExprNoBF", "Arguments", "ArgumentList",
  "LeftHandSideExpr", "LeftHandSideExprNoBF", "PostfixExpr",
  "PostfixExprNoBF", "UnaryExprCommon", "UnaryExpr", "UnaryExprNoBF",
  "MultiplicativeExpr", "MultiplicativeExprNoBF", "AdditiveExpr",
  "AdditiveExprNoBF", "ShiftExpr", "ShiftExprNoBF", "RelationalExpr",
  "RelationalExprNoIn", "RelationalExprNoBF", "EqualityExpr",
  "EqualityExprNoIn", "EqualityExprNoBF", "BitwiseANDExpr",
  "BitwiseANDExprNoIn", "BitwiseANDExprNoBF", "BitwiseXORExpr",
  "BitwiseXORExprNoIn", "BitwiseXORExprNoBF", "BitwiseORExpr",
  "BitwiseORExprNoIn", "BitwiseORExprNoBF", "LogicalANDExpr",
  "LogicalANDExprNoIn", "LogicalANDExprNoBF", "LogicalORExpr",
  "LogicalORExprNoIn", "LogicalORExprNoBF", "ConditionalExpr",
  "ConditionalExprNoIn", "ConditionalExprNoBF", "AssignmentExpr",
  "AssignmentExprNoIn", "AssignmentExprNoBF", "AssignmentOperator", "Expr",
  "ExprNoIn", "ExprNoBF", "Statement", "Block", "StatementList",
  "EmptyStatement", "ExprStatement", "IfStatement", "IterationStatement",
  "ExprOpt", "ExprNoInOpt", "ContinueStatement", "BreakStatement",
  "ReturnStatement", "WithStatement", "SwitchStatement", "CaseBlock",
  "CaseClausesOpt", "CaseClauses", "CaseClause", "DefaultClause",
  "LabelledStatement", "ThrowStatement", "TryStatement", "Catch",
  "Finally", "PackageDefinition", "ImportStatement", "PackageName",
  "FunctionDeclaration", "ResultSignature", "FunctionExpr",
  "FormalParameterList", "FunctionBody", "ClassDefinition", "Program",
  "SourceElements", "SourceElement", "Attributes", "Attribute",
  "VariableDefinition", "VariableDefinitionKind", "VariableBindingList",
  "VariableBindingListNoIn", "VariableBinding", "VariableBindingNoIn",
  "TypedVariable", "TypeExpression", 0
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
     325,   326,    47,    58,    44,   123,   125,    40,    41,    91,
      93,    46,    43,    45,   126,    33,    42,    37,    60,    62,
      38,    94,   124,    63,    61,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    96,    97,    97,    97,    97,    97,    97,    98,    98,
      98,    99,    99,   100,   100,   100,   100,   101,   101,   101,
     101,   101,   102,   102,   102,   103,   103,   104,   104,   105,
     105,   106,   106,   106,   106,   106,   107,   107,   107,   107,
     108,   108,   109,   109,   110,   110,   110,   110,   111,   111,
     111,   111,   112,   112,   113,   113,   114,   114,   115,   115,
     116,   116,   116,   117,   117,   117,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   119,   119,   120,   120,   121,
     121,   121,   121,   122,   122,   122,   122,   123,   123,   123,
     124,   124,   124,   125,   125,   125,   125,   126,   126,   126,
     126,   127,   127,   127,   127,   127,   127,   127,   127,   128,
     128,   128,   128,   128,   128,   129,   129,   129,   129,   129,
     129,   129,   129,   130,   130,   130,   130,   130,   131,   131,
     131,   131,   131,   132,   132,   132,   132,   132,   133,   133,
     134,   134,   135,   135,   136,   136,   137,   137,   138,   138,
     139,   139,   140,   140,   141,   141,   142,   142,   143,   143,
     144,   144,   145,   145,   146,   146,   147,   147,   148,   148,
     149,   149,   150,   150,   151,   151,   152,   152,   153,   153,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   155,   155,   156,   156,   157,   157,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   159,   159,   160,   160,
     161,   162,   162,   163,   163,   164,   164,   164,   164,   164,
     164,   164,   165,   165,   166,   166,   167,   167,   167,   167,
     168,   168,   168,   168,   169,   169,   169,   169,   170,   171,
     172,   172,   173,   173,   174,   174,   175,   175,   176,   176,
     177,   177,   178,   179,   179,   179,   180,   181,   182,   183,
     184,   184,   185,   185,   185,   185,   186,   186,   187,   187,
     188,   188,   188,   188,   189,   189,   190,   190,   190,   191,
     191,   192,   192,   193,   193,   194,   194,   195,   195,   195,
     195,   195,   195,   195,   195,   196,   196,   196,   196,   197,
     197,   198,   198,   199,   199,   200,   200,   201,   201,   202,
     202,   203
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     5,     1,     2,     3,     4,     1,     1,     1,
       1,     3,     3,     3,     5,     2,     4,     0,     1,     1,
       2,     1,     1,     4,     3,     3,     1,     4,     3,     3,
       1,     2,     1,     2,     2,     2,     4,     3,     2,     2,
       4,     3,     2,     3,     1,     3,     1,     1,     1,     1,
       1,     2,     2,     1,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     1,     1,     1,     1,     1,
       3,     3,     3,     1,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     1,     3,     3,
       3,     1,     3,     3,     3,     3,     3,     3,     3,     1,
       3,     3,     3,     3,     3,     1,     3,     3,     3,     3,
       3,     3,     3,     1,     3,     3,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     3,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     5,
       1,     5,     1,     5,     1,     3,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     3,     1,     3,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     3,     1,     2,
       1,     2,     2,     5,     7,     7,     7,     5,     9,    10,
       7,     8,     0,     1,     0,     1,     2,     2,     3,     3,
       2,     2,     3,     3,     2,     2,     3,     3,     5,     5,
       3,     5,     0,     1,     1,     2,     3,     4,     2,     3,
       5,     3,     3,     3,     3,     4,     5,     2,     3,     3,
       1,     3,     6,     7,     7,     8,     0,     2,     4,     5,
       1,     3,     3,     5,     2,     3,     3,     3,     5,     0,
       1,     1,     2,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     4,     4,     1,
       1,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
     289,     2,     3,     4,     0,     0,     0,   309,     0,     0,
       0,     0,     0,     0,    17,     0,     0,     0,     0,     0,
       0,     0,     0,   304,   297,   298,   299,   300,   301,   310,
       0,     0,     0,     0,     5,     6,    20,     7,     0,     0,
      27,     0,     0,     0,     0,   220,    18,    36,    19,    42,
      58,    59,    63,    77,    78,    83,    90,    97,   115,   133,
     142,   148,   154,   160,   166,   172,   178,   196,     0,   293,
     198,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   202,   215,   294,   200,     0,   290,   291,
       0,   295,   199,     0,   241,     0,   240,   234,     3,     4,
       0,     0,    20,     0,    31,    13,    40,    43,    32,   237,
       0,   236,     0,   245,   244,    40,    56,    57,    60,    75,
      76,    79,    87,    93,   101,   123,   138,   144,   150,   156,
     162,   168,   174,   192,     0,    60,    67,    66,     0,     0,
       0,     0,    68,     0,     0,     0,     0,     0,   270,     0,
       0,    69,    70,     0,   216,   218,     0,     0,    29,     0,
       0,    28,    71,    72,    73,    74,     0,     0,     0,    48,
       0,     0,    49,    64,    65,   181,   182,   183,   184,   185,
     186,   187,   188,   191,   189,   190,   180,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   222,     0,   221,     1,   292,   302,   303,
       0,   201,   296,     0,   319,     0,   311,   315,   243,   242,
       0,    60,   109,   128,   140,   146,   152,   158,   164,   170,
     176,   194,   235,     0,    40,    41,     0,    10,     9,     8,
      14,     0,     0,     0,     0,    39,   239,   238,     0,    44,
       0,     0,    45,    61,    62,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   247,     0,   246,     0,     0,     0,     0,     0,   262,
       0,     0,   263,   264,     0,     0,   286,   287,     0,   268,
     269,    20,   261,   217,   219,    21,    27,    23,    22,    25,
      30,    52,     0,    54,     0,    38,     0,    51,   179,    85,
      84,    86,    91,    92,    98,    99,   100,   122,   121,   120,
     118,   119,   116,   117,   134,   135,   136,   137,   143,   149,
     155,   161,   167,     0,   197,     0,     0,     0,   306,     0,
     305,     0,     0,   313,   317,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   232,    35,   280,     0,     0,     0,     0,
      15,     0,    34,   276,     0,     0,    47,   175,    81,    80,
      82,    88,    89,    94,    95,    96,   108,   107,   106,   104,
     105,   102,   103,   124,   125,   126,   127,   139,   145,   151,
     157,   163,     0,   193,     0,     0,     0,     0,     0,     0,
     267,   265,   321,     0,   284,     0,   271,     0,     0,     0,
      53,    37,    50,     0,     0,   308,   307,   320,   312,   316,
       0,   232,     0,     0,     0,    60,   177,   114,   112,   113,
     110,   111,   129,   130,   131,   132,   141,   147,   153,   159,
     165,     0,   195,   233,     0,     0,   278,     0,     0,    11,
      16,     0,    33,     0,     0,   276,    46,     0,   223,     0,
     227,   252,   249,   248,     0,   288,   285,   260,    24,    26,
      55,   173,   276,     0,   314,     0,     0,   318,     0,     0,
     232,   281,   282,   279,     0,   277,   272,     0,   169,     0,
       0,     0,     0,   253,   254,     0,     0,   276,   232,     0,
     230,   171,     0,     0,    12,   273,   224,   226,   225,     0,
       0,   250,   252,   255,   266,   274,     0,     0,   231,     0,
     283,   256,   258,     0,   275,     0,   228,   257,   259,   251,
     229
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    46,   251,   252,   104,   105,    48,   159,   160,   161,
     115,    49,   116,    50,   117,    51,   169,   322,   135,    52,
     119,    53,   120,   121,    55,   122,    56,   123,    57,   124,
      58,   125,   233,    59,   126,   234,    60,   127,   235,    61,
     128,   236,    62,   129,   237,    63,   130,   238,    64,   131,
     239,    65,   132,   240,    66,   133,   241,    67,   366,   473,
     242,    68,    69,    70,   156,    71,    72,    73,    74,   474,
     243,    75,    76,    77,    78,    79,   492,   522,   523,   524,
     542,    80,    81,    82,   302,   303,    83,    84,   149,    85,
     484,   108,   387,   307,    86,    87,    88,    89,   140,    91,
      92,    93,   225,   362,   226,   363,   227,   433
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -430
static const yytype_int16 yypact[] =
{
     867,  -430,   595,  1320,    15,     8,   256,  -430,    18,   -26,
     606,  1518,  1518,    83,  -430,  1128,    94,  1518,   129,   146,
    1518,    79,   119,  -430,  -430,  -430,  -430,  -430,  -430,  -430,
     174,   174,  1518,  1518,  -430,  -430,   199,  -430,   954,  1518,
     173,  1518,  1518,  1518,  1518,  -430,  -430,  -430,  -430,    -5,
    -430,    78,   397,  -430,  -430,  -430,    77,   -28,   177,   218,
     312,   198,   200,   202,   261,    91,  -430,  -430,     9,  -430,
    -430,  -430,  -430,  -430,  -430,  -430,  -430,  -430,  -430,  -430,
    -430,  -430,  -430,  -430,  -430,  -430,  -430,   308,   867,  -430,
    1430,  -430,  -430,   278,  -430,    25,  -430,  1298,  -430,  -430,
     256,   244,  -430,   104,  -430,  -430,   205,  -430,  -430,  -430,
      27,  -430,   255,  -430,  -430,   205,  -430,   208,   415,  -430,
    -430,  -430,    80,   170,   306,   248,   341,   258,   274,   292,
     338,    93,  -430,  -430,    13,   210,  -430,  -430,  1518,   372,
     542,  1518,  -430,  1518,  1518,   -32,   268,    55,  -430,   -19,
     -44,  -430,  -430,  1215,  -430,  -430,  1041,    -3,  -430,    29,
    1325,   319,  -430,  -430,  -430,  -430,  1408,  1518,   328,  -430,
    1518,   333,  -430,  -430,  -430,  -430,  -430,  -430,  -430,  -430,
    -430,  -430,  -430,  -430,  -430,  -430,  -430,  1518,  1518,  1518,
    1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,
    1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,
    1518,  1518,  1518,  -430,  1518,  -430,  -430,  -430,  -430,  -430,
     335,  -430,  -430,   278,   334,    19,  -430,   314,  -430,  -430,
     278,   154,   306,    17,   350,   320,   318,   327,   368,    96,
    -430,  -430,   347,   330,   205,  -430,    70,  -430,  -430,  -430,
    -430,   349,   163,  1518,   352,  -430,  -430,  -430,   114,  -430,
    1518,   356,  -430,  -430,  -430,  1518,  1518,  1518,  1518,  1518,
    1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,
    1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,
    1518,  -430,  1518,  -430,   131,   353,   172,   184,   236,  -430,
     354,    79,   400,  -430,   358,   693,  -430,  -430,   362,  -430,
    -430,   -37,  -430,  -430,  -430,  -430,   173,  -430,  -430,  -430,
    -430,  -430,   237,  -430,   108,  -430,   113,  -430,  -430,  -430,
    -430,  -430,    80,    80,   170,   170,   170,   306,   306,   306,
     306,   306,   306,   306,   248,   248,   248,   248,   341,   258,
     274,   292,   338,   361,  -430,   360,    20,   358,  -430,   278,
    -430,  1518,   -30,   419,   351,  1518,  1518,  1518,  1518,  1518,
    1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,  1518,
    1518,  1518,  1518,  1518,  -430,   373,   374,   242,  1518,   159,
    -430,   130,  -430,   375,   260,   162,  -430,  -430,  -430,  -430,
    -430,    80,    80,   170,   170,   170,   306,   306,   306,   306,
     306,   306,   306,   248,   248,   248,   248,   341,   258,   274,
     292,   338,   379,  -430,  1128,  1518,  1128,   378,  1128,   395,
    -430,  -430,  -430,   374,  -430,   780,  -430,   466,  1435,  1518,
    -430,  -430,  -430,  1518,   160,  -430,  -430,  -430,  -430,  -430,
     278,  1518,  1518,  1518,   266,   415,  -430,   306,   306,   306,
     306,   306,    17,    17,    17,    17,   350,   320,   318,   327,
     368,   398,  -430,   396,   377,   358,  -430,   413,   374,  -430,
    -430,   412,  -430,   358,   374,   375,  -430,  1518,   465,   267,
    -430,   480,  -430,  -430,   410,  -430,  -430,  -430,  -430,  -430,
    -430,  -430,   375,   276,  -430,   394,   277,  -430,  1128,  1518,
    1518,  -430,   417,  -430,  1518,  -430,  -430,   374,  -430,  1128,
      30,  1518,    21,   480,  -430,    79,   374,   375,  1518,  1128,
    -430,  -430,   414,   358,  -430,  -430,  -430,  -430,  -430,   250,
     420,  -430,   480,  -430,  -430,  -430,   374,   416,  -430,  1128,
    -430,  1128,  1128,   421,  -430,  1128,  -430,  1128,  1128,  -430,
    -430
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -430,  -430,   107,  -430,  -430,     0,  -430,  -430,   182,  -430,
       7,  -430,    26,  -430,  -430,  -430,   -33,  -430,   152,  -430,
    -430,  -430,    12,    36,  -430,  -158,  -430,   -16,  -430,     1,
    -430,  -145,    28,  -430,  -168,   123,  -430,  -167,   124,  -430,
    -166,   122,  -430,  -164,   125,  -430,  -162,   134,  -430,  -430,
    -430,  -430,  -430,  -430,  -430,   -96,  -358,  -430,   -17,    -9,
    -430,  -430,   -13,   -18,  -222,  -430,  -430,  -430,  -430,  -429,
    -430,  -430,  -430,  -430,  -430,  -430,  -430,   -40,  -430,   -20,
    -430,  -430,  -430,  -430,  -430,   203,  -430,  -430,   484,  -430,
    -357,  -430,  -249,  -132,   -38,  -430,   206,   -82,     4,     6,
    -430,    43,   298,  -430,   164,    72,  -223,  -352
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -304
static const yytype_int16 yytable[] =
{
      47,   134,   139,   146,    90,   447,   217,   364,   456,   394,
     213,   145,    54,   106,   291,    47,    94,   309,   172,   109,
     358,   445,   505,   471,   472,   155,   228,    54,   256,   540,
     157,   537,   107,   332,   333,   187,   153,   308,    47,   348,
     367,   349,   292,   350,   450,   112,   351,   136,   137,   352,
      54,   310,   221,   142,   191,   192,   305,   437,   344,   345,
     346,   347,   308,   299,   319,   451,   368,   369,   151,   152,
     323,   292,   166,   255,   167,   315,   168,   162,   163,   164,
     165,   532,   259,   214,   262,    97,    95,   292,    47,   110,
     304,   328,    90,   359,   359,   507,   222,   541,   232,   547,
      54,   265,   221,   316,   215,   370,   371,   244,   293,   317,
      96,   401,   402,   111,   360,   446,   353,   417,   354,   418,
     229,   419,   257,   511,   420,   538,   245,   421,   517,   294,
     305,   515,   296,   223,   297,   298,   413,   414,   415,   416,
     312,   385,   211,   314,   289,   526,   222,   380,   386,   188,
     306,   531,   266,    47,    38,   166,    47,   170,   324,   171,
     138,   326,   118,   189,   190,    54,   267,   268,    54,   397,
     546,   141,   118,   247,   248,   249,   365,   334,   335,   336,
     250,   550,   292,   223,   212,   385,   290,   292,   441,   381,
     147,   118,   393,   442,   422,   503,   423,   337,   338,   339,
     340,   341,   342,   343,   292,   292,   143,   263,   264,   424,
     482,   384,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   144,   329,   330,   331,   364,   247,   248,
     249,   385,   193,   194,   195,   480,   292,   389,   502,   390,
     196,   197,   486,   198,   391,   148,   292,   158,   186,   231,
     426,   395,   269,   270,   476,   403,   404,   405,   292,     1,
      98,    99,   427,   263,   264,   449,   100,   199,   200,   101,
     274,   275,   153,   276,    14,   406,   407,   408,   409,   410,
     411,   412,   166,   430,   253,   166,   254,   260,   207,   261,
     118,   208,   479,   118,   209,   118,   118,   277,   278,   300,
     301,   495,   398,   399,   400,    47,   201,   202,   216,    90,
     292,   439,   118,   210,   428,   440,   477,    54,   118,   118,
     478,   246,   118,   551,   292,    34,    35,   102,    37,   557,
     558,   103,   258,    39,   477,    40,   279,   280,   485,   118,
     292,   292,   499,   500,   508,   520,   513,   501,   285,   224,
     477,   292,   516,   217,   527,   529,   454,   203,   204,   205,
     206,   271,   272,   273,   118,   286,   118,   232,   457,   458,
     459,   460,   461,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   287,   535,   281,   282,   283,   284,
     288,   518,   295,   320,   545,   372,   373,   374,   375,   325,
     462,   463,   464,   465,   327,   118,   355,   357,   361,   377,
     376,   488,   118,   490,   554,   493,   489,   118,   534,   378,
     379,   382,   388,   392,    47,   383,    47,   396,    47,   432,
     425,   429,   301,   436,   443,    47,    54,   444,    54,    90,
      54,   452,   118,   506,   118,   453,   475,    54,   483,   305,
     173,   174,   487,   491,   232,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   494,   497,   263,   264,
     292,   509,   510,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   512,   514,   519,   521,   525,   528,
     533,   186,   549,   552,   555,   530,   481,   559,   438,   466,
     468,   467,   553,   543,   469,   431,   536,   544,    47,   186,
     232,   435,   539,   118,   470,   150,   548,   118,   455,    47,
      54,   356,   504,   448,     0,     0,     0,     0,     0,    47,
       0,    54,     0,   455,   455,   118,   556,     0,   155,   155,
     118,    54,   560,     0,   314,   314,   218,   219,     0,    47,
       0,    47,    47,     7,     0,    47,     0,    47,    47,     0,
       0,    54,     0,    54,    54,     0,     0,    54,     0,    54,
      54,     0,     0,     0,     0,    22,    23,   118,    24,    25,
      26,    27,    28,    29,     0,     0,     0,     0,     0,     0,
     118,   118,     0,     0,     0,   118,     0,     0,     0,  -302,
    -302,     0,     0,   118,   118,   455,  -302,   113,  -302,     1,
      98,    99,     0,     0,     0,     0,   100,     0,     0,   101,
       0,    11,    12,     0,    14,     0,     0,     0,  -302,  -302,
      17,  -302,  -302,  -302,  -302,  -302,  -302,     0,     0,   118,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    32,
      33,   455,   118,     0,     0,     0,   118,     0,     0,     0,
       0,     0,     0,   118,     0,    34,    35,   102,    37,     0,
     118,   103,     0,    39,     0,    40,     0,     0,    41,    42,
      43,    44,     0,     0,     0,     0,     1,     2,     3,     4,
       0,   114,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,     0,     0,     0,    17,     0,    18,
      19,     0,    20,    21,     0,     0,    22,    23,     0,    24,
      25,    26,    27,    28,    29,    30,    31,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    32,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    34,    35,    36,    37,     0,     0,    38,   434,
      39,     0,    40,     0,     0,    41,    42,    43,    44,     0,
       0,     0,     0,     1,     2,     3,     4,     0,    45,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,     0,     0,     0,    17,     0,    18,    19,     0,    20,
      21,     0,     0,    22,    23,     0,    24,    25,    26,    27,
      28,    29,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    32,    33,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    34,
      35,    36,    37,     0,     0,    38,   496,    39,     0,    40,
       0,     0,    41,    42,    43,    44,     0,     0,     0,     0,
       1,     2,     3,     4,     0,    45,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,     0,     0,
       0,    17,     0,    18,    19,     0,    20,    21,     0,     0,
      22,    23,     0,    24,    25,    26,    27,    28,    29,    30,
      31,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      32,    33,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    34,    35,    36,    37,
       0,     0,    38,     0,    39,     0,    40,     0,     0,    41,
      42,    43,    44,     0,     0,     0,     0,     1,     2,     3,
       4,     0,    45,     5,     6,     7,     8,     0,    10,    11,
      12,    13,    14,    15,    16,     0,     0,     0,    17,     0,
      18,    19,     0,    20,    21,     0,     0,    22,    23,     0,
      24,    25,    26,    27,    28,    29,    30,    31,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    32,    33,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    34,    35,    36,    37,     0,     0,    38,
     154,    39,     0,    40,     0,     0,    41,    42,    43,    44,
       0,     0,     0,     0,     1,     2,     3,     4,     0,    45,
       5,     6,     7,     8,     0,    10,    11,    12,    13,    14,
      15,    16,     0,     0,     0,    17,     0,    18,    19,     0,
      20,    21,     0,     0,    22,    23,     0,    24,    25,    26,
      27,    28,    29,    30,    31,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    32,    33,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      34,    35,    36,    37,     0,     0,    38,   313,    39,     0,
      40,     0,     0,    41,    42,    43,    44,     0,     0,     0,
       0,     1,     2,     3,     4,     0,    45,     5,     6,     7,
       8,     0,    10,    11,    12,    13,    14,    15,    16,     0,
       0,     0,    17,     0,    18,    19,     0,    20,    21,     0,
       0,    22,    23,     0,    24,    25,    26,    27,    28,    29,
      30,    31,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    32,    33,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    34,    35,    36,
      37,     0,     0,    38,     0,    39,     0,    40,     0,     0,
      41,    42,    43,    44,     0,     0,     0,     0,     1,     2,
       3,     4,     0,    45,     5,     6,     7,     8,     0,    10,
      11,    12,    13,    14,    15,    16,     0,     0,     0,    17,
       0,    18,    19,     0,    20,    21,     0,     0,    22,    23,
       0,    24,    25,    26,    27,    28,    29,    30,    31,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   311,    37,     0,     0,
      38,     0,    39,     0,    40,     0,     0,    41,    42,    43,
      44,     1,    98,    99,     0,     0,     0,     0,   100,   230,
      45,   101,     0,    11,    12,     0,    14,     0,     0,     0,
       0,     0,    17,     0,  -303,  -303,     0,     0,     1,    98,
      99,  -303,     0,  -303,     0,   100,     0,     0,   101,     0,
      11,    12,     0,    14,     0,     0,     0,     0,     0,    17,
       0,    32,    33,  -303,  -303,     0,  -303,  -303,  -303,  -303,
    -303,  -303,     0,     0,     0,     0,     0,    34,    35,   102,
      37,     0,     0,   103,     0,    39,     0,    40,    32,    33,
      41,    42,    43,    44,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   102,    37,     0,     0,
     103,     0,    39,     0,    40,   318,     0,    41,    42,    43,
      44,     1,    98,    99,     0,     0,     0,     0,   100,     0,
       0,   101,     0,    11,    12,     0,    14,     0,     0,     0,
       0,     0,    17,     0,   218,   219,     0,     0,     1,    98,
      99,     7,     0,   220,     0,   100,     0,     0,   101,     0,
      11,    12,     0,    14,     0,     0,     0,     0,     0,    17,
       0,    32,    33,    22,    23,     0,    24,    25,    26,    27,
      28,    29,     0,     0,     0,     0,     0,    34,    35,   102,
      37,     0,     0,   103,     0,    39,   321,    40,    32,    33,
      41,    42,    43,    44,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   102,    37,     0,     0,
     103,     0,    39,     0,    40,   498,     0,    41,    42,    43,
      44,     1,    98,    99,     0,     0,     0,     0,   100,     0,
       0,   101,     0,    11,    12,     0,    14,     0,     0,     0,
       0,     0,    17,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    32,    33,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    34,    35,   102,
      37,     0,     0,   103,     0,    39,     0,    40,     0,     0,
      41,    42,    43,    44
};

static const yytype_int16 yycheck[] =
{
       0,    10,    15,    21,     0,   357,    88,   230,   366,   258,
       1,    20,     0,     6,     1,    15,     1,   149,    51,     1,
       1,     1,   451,   381,   382,    38,     1,    15,     1,     8,
      39,     1,     6,   191,   192,    52,    73,    81,    38,   207,
      23,   208,    74,   209,    74,    71,   210,    11,    12,   211,
      38,    95,    90,    17,    82,    83,    75,    94,   203,   204,
     205,   206,    81,    95,   160,    95,    49,    50,    32,    33,
     166,    74,    77,   106,    79,    78,    81,    41,    42,    43,
      44,   510,   115,    74,   117,    77,    71,    74,    88,    71,
      35,   187,    88,    74,    74,   453,    90,    76,    97,   528,
      88,   118,   140,    74,    95,    88,    89,   100,    95,    80,
      95,   269,   270,    95,    95,    95,   212,   285,   214,   286,
      95,   287,    95,   475,   288,    95,   100,   289,   485,   138,
      75,   483,   141,    90,   143,   144,   281,   282,   283,   284,
     153,    71,    51,   156,    51,   502,   140,    51,    78,    72,
      95,   509,    72,   153,    75,    77,   156,    79,   167,    81,
      77,   170,    10,    86,    87,   153,    86,    87,   156,   265,
     527,    77,    20,    69,    70,    71,    22,   193,   194,   195,
      76,   533,    74,   140,    93,    71,    93,    74,    80,    93,
      71,    39,    78,    80,   290,   444,   292,   196,   197,   198,
     199,   200,   201,   202,    74,    74,    77,    53,    54,    78,
      80,   244,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    77,   188,   189,   190,   450,    69,    70,
      71,    71,    55,    56,    57,    76,    74,    74,    78,    76,
      22,    23,    80,    25,   253,    71,    74,    74,    94,    97,
      78,   260,    82,    83,   386,   271,   272,   273,    74,     3,
       4,     5,    78,    53,    54,   361,    10,    49,    50,    13,
      22,    23,    73,    25,    18,   274,   275,   276,   277,   278,
     279,   280,    77,   301,    79,    77,    81,    79,    90,    81,
     138,    91,   388,   141,    92,   143,   144,    49,    50,    31,
      32,   433,   266,   267,   268,   305,    88,    89,     0,   305,
      74,    74,   160,    52,    78,    78,    74,   305,   166,   167,
      78,    77,   170,    73,    74,    69,    70,    71,    72,   551,
     552,    75,    77,    77,    74,    79,    88,    89,    78,   187,
      74,    74,   438,   439,    78,    78,   478,   443,    90,    71,
      74,    74,   484,   435,    78,    78,   365,    45,    46,    47,
      48,    55,    56,    57,   212,    91,   214,   366,   367,   368,
     369,   370,   371,   372,   373,   374,   375,   376,   377,   378,
     379,   380,   381,   382,    92,   517,    45,    46,    47,    48,
      52,   487,    20,    74,   526,    45,    46,    47,    48,    71,
     372,   373,   374,   375,    71,   253,    71,    73,    94,    91,
      90,   424,   260,   426,   546,   428,   425,   265,   514,    92,
      52,    74,    73,    71,   424,    95,   426,    71,   428,    71,
      77,    77,    32,    71,    73,   435,   424,    77,   426,   435,
     428,    22,   290,   452,   292,    94,    73,   435,    73,    75,
      53,    54,    73,    75,   453,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    71,     1,    53,    54,
      74,    73,    95,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    71,    73,    21,     7,    78,    95,
      73,    94,    78,    73,    78,   508,   389,    76,   316,   376,
     378,   377,   542,   523,   379,   302,   519,   525,   508,    94,
     509,   305,   521,   361,   380,    31,   529,   365,   366,   519,
     508,   223,   450,   359,    -1,    -1,    -1,    -1,    -1,   529,
      -1,   519,    -1,   381,   382,   383,   549,    -1,   551,   552,
     388,   529,   555,    -1,   557,   558,     4,     5,    -1,   549,
      -1,   551,   552,    11,    -1,   555,    -1,   557,   558,    -1,
      -1,   549,    -1,   551,   552,    -1,    -1,   555,    -1,   557,
     558,    -1,    -1,    -1,    -1,    33,    34,   425,    36,    37,
      38,    39,    40,    41,    -1,    -1,    -1,    -1,    -1,    -1,
     438,   439,    -1,    -1,    -1,   443,    -1,    -1,    -1,     4,
       5,    -1,    -1,   451,   452,   453,    11,     1,    13,     3,
       4,     5,    -1,    -1,    -1,    -1,    10,    -1,    -1,    13,
      -1,    15,    16,    -1,    18,    -1,    -1,    -1,    33,    34,
      24,    36,    37,    38,    39,    40,    41,    -1,    -1,   487,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,
      54,   509,   510,    -1,    -1,    -1,   514,    -1,    -1,    -1,
      -1,    -1,    -1,   521,    -1,    69,    70,    71,    72,    -1,
     528,    75,    -1,    77,    -1,    79,    -1,    -1,    82,    83,
      84,    85,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    95,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    24,    -1,    26,
      27,    -1,    29,    30,    -1,    -1,    33,    34,    -1,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    53,    54,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    70,    71,    72,    -1,    -1,    75,    76,
      77,    -1,    79,    -1,    -1,    82,    83,    84,    85,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,    -1,    95,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    -1,    -1,    24,    -1,    26,    27,    -1,    29,
      30,    -1,    -1,    33,    34,    -1,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,
      70,    71,    72,    -1,    -1,    75,    76,    77,    -1,    79,
      -1,    -1,    82,    83,    84,    85,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,    95,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    -1,
      -1,    24,    -1,    26,    27,    -1,    29,    30,    -1,    -1,
      33,    34,    -1,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,
      -1,    -1,    75,    -1,    77,    -1,    79,    -1,    -1,    82,
      83,    84,    85,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    95,     9,    10,    11,    12,    -1,    14,    15,
      16,    17,    18,    19,    20,    -1,    -1,    -1,    24,    -1,
      26,    27,    -1,    29,    30,    -1,    -1,    33,    34,    -1,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    54,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    69,    70,    71,    72,    -1,    -1,    75,
      76,    77,    -1,    79,    -1,    -1,    82,    83,    84,    85,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,    95,
       9,    10,    11,    12,    -1,    14,    15,    16,    17,    18,
      19,    20,    -1,    -1,    -1,    24,    -1,    26,    27,    -1,
      29,    30,    -1,    -1,    33,    34,    -1,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    53,    54,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      69,    70,    71,    72,    -1,    -1,    75,    76,    77,    -1,
      79,    -1,    -1,    82,    83,    84,    85,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    -1,    95,     9,    10,    11,
      12,    -1,    14,    15,    16,    17,    18,    19,    20,    -1,
      -1,    -1,    24,    -1,    26,    27,    -1,    29,    30,    -1,
      -1,    33,    34,    -1,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,
      72,    -1,    -1,    75,    -1,    77,    -1,    79,    -1,    -1,
      82,    83,    84,    85,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    95,     9,    10,    11,    12,    -1,    14,
      15,    16,    17,    18,    19,    20,    -1,    -1,    -1,    24,
      -1,    26,    27,    -1,    29,    30,    -1,    -1,    33,    34,
      -1,    36,    37,    38,    39,    40,    41,    42,    43,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    54,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    69,    70,    71,    72,    -1,    -1,
      75,    -1,    77,    -1,    79,    -1,    -1,    82,    83,    84,
      85,     3,     4,     5,    -1,    -1,    -1,    -1,    10,    11,
      95,    13,    -1,    15,    16,    -1,    18,    -1,    -1,    -1,
      -1,    -1,    24,    -1,     4,     5,    -1,    -1,     3,     4,
       5,    11,    -1,    13,    -1,    10,    -1,    -1,    13,    -1,
      15,    16,    -1,    18,    -1,    -1,    -1,    -1,    -1,    24,
      -1,    53,    54,    33,    34,    -1,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,
      72,    -1,    -1,    75,    -1,    77,    -1,    79,    53,    54,
      82,    83,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    69,    70,    71,    72,    -1,    -1,
      75,    -1,    77,    -1,    79,    80,    -1,    82,    83,    84,
      85,     3,     4,     5,    -1,    -1,    -1,    -1,    10,    -1,
      -1,    13,    -1,    15,    16,    -1,    18,    -1,    -1,    -1,
      -1,    -1,    24,    -1,     4,     5,    -1,    -1,     3,     4,
       5,    11,    -1,    13,    -1,    10,    -1,    -1,    13,    -1,
      15,    16,    -1,    18,    -1,    -1,    -1,    -1,    -1,    24,
      -1,    53,    54,    33,    34,    -1,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,
      72,    -1,    -1,    75,    -1,    77,    78,    79,    53,    54,
      82,    83,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    69,    70,    71,    72,    -1,    -1,
      75,    -1,    77,    -1,    79,    80,    -1,    82,    83,    84,
      85,     3,     4,     5,    -1,    -1,    -1,    -1,    10,    -1,
      -1,    13,    -1,    15,    16,    -1,    18,    -1,    -1,    -1,
      -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,
      72,    -1,    -1,    75,    -1,    77,    -1,    79,    -1,    -1,
      82,    83,    84,    85
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    24,    26,    27,
      29,    30,    33,    34,    36,    37,    38,    39,    40,    41,
      42,    43,    53,    54,    69,    70,    71,    72,    75,    77,
      79,    82,    83,    84,    85,    95,    97,   101,   102,   107,
     109,   111,   115,   117,   118,   120,   122,   124,   126,   129,
     132,   135,   138,   141,   144,   147,   150,   153,   157,   158,
     159,   161,   162,   163,   164,   167,   168,   169,   170,   171,
     177,   178,   179,   182,   183,   185,   190,   191,   192,   193,
     194,   195,   196,   197,     1,    71,    95,    77,     4,     5,
      10,    13,    71,    75,   100,   101,   106,   108,   187,     1,
      71,    95,    71,     1,    95,   106,   108,   110,   114,   116,
     118,   119,   121,   123,   125,   127,   130,   133,   136,   139,
     142,   145,   148,   151,   155,   114,   119,   119,    77,   158,
     194,    77,   119,    77,    77,   155,   159,    71,    71,   184,
     184,   119,   119,    73,    76,   158,   160,   155,    74,   103,
     104,   105,   119,   119,   119,   119,    77,    79,    81,   112,
      79,    81,   112,    53,    54,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    94,   154,    72,    86,
      87,    82,    83,    55,    56,    57,    22,    23,    25,    49,
      50,    88,    89,    45,    46,    47,    48,    90,    91,    92,
      52,    51,    93,     1,    74,    95,     0,   193,     4,     5,
      13,   190,   195,   197,    71,   198,   200,   202,     1,    95,
      11,   114,   125,   128,   131,   134,   137,   140,   143,   146,
     149,   152,   156,   166,   106,   108,    77,    69,    70,    71,
      76,    98,    99,    79,    81,   112,     1,    95,    77,   112,
      79,    81,   112,    53,    54,   154,    72,    86,    87,    82,
      83,    55,    56,    57,    22,    23,    25,    49,    50,    88,
      89,    45,    46,    47,    48,    90,    91,    92,    52,    51,
      93,     1,    74,    95,   155,    20,   155,   155,   155,    95,
      31,    32,   180,   181,    35,    75,    95,   189,    81,   189,
      95,    71,   158,    76,   158,    78,    74,    80,    80,   151,
      74,    78,   113,   151,   155,    71,   155,    71,   151,   119,
     119,   119,   121,   121,   123,   123,   123,   125,   125,   125,
     125,   125,   125,   125,   127,   127,   127,   127,   130,   133,
     136,   139,   142,   151,   151,    71,   198,    73,     1,    74,
      95,    94,   199,   201,   202,    22,   154,    23,    49,    50,
      88,    89,    45,    46,    47,    48,    90,    91,    92,    52,
      51,    93,    74,    95,   112,    71,    78,   188,    73,    74,
      76,   155,    71,    78,   188,   155,    71,   151,   119,   119,
     119,   121,   121,   123,   123,   123,   125,   125,   125,   125,
     125,   125,   125,   127,   127,   127,   127,   130,   133,   136,
     139,   142,   151,   151,    78,    77,    78,    78,    78,    77,
     159,   181,    71,   203,    76,   192,    71,    94,   104,    74,
      78,    80,    80,    73,    77,     1,    95,   203,   200,   151,
      74,    95,    22,    94,   155,   114,   152,   125,   125,   125,
     125,   125,   128,   128,   128,   128,   131,   134,   137,   140,
     143,   152,   152,   155,   165,    73,   189,    74,    78,   151,
      76,    98,    80,    73,   186,    78,    80,    73,   158,   155,
     158,    75,   172,   158,    71,   189,    76,     1,    80,   151,
     151,   151,    78,   188,   201,   165,   155,   152,    78,    73,
      95,   203,    71,   189,    73,   203,   189,   186,   151,    21,
      78,     7,   173,   174,   175,    78,   186,    78,    95,    78,
     158,   152,   165,    73,   151,   189,   158,     1,    95,   155,
       8,    76,   176,   175,   159,   189,   186,   165,   158,    78,
     203,    73,    73,   173,   189,    78,   158,   160,   160,    76,
     158
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
# if YYLTYPE_IS_TRIVIAL
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
		  Type, Value, Location); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

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
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 195 "grammar.y"
    { (yyval.node) = new QSNullNode(); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 196 "grammar.y"
    { (yyval.node) = newTrueNode();/*new QSBooleanNode(true);*/ ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 197 "grammar.y"
    { (yyval.node) = newFalseNode();/*new QSBooleanNode(false);*/ ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 198 "grammar.y"
    { (yyval.node) = new QSNumberNode((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 199 "grammar.y"
    { (yyval.node) = new QSStringNode(*((yyvsp[(1) - (1)].ustr))); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 200 "grammar.y"
    { QSLexer *l = QSLexer::lexer(); if (!l->scanRegExp()) {YYABORT;} else {(yyval.node) = new QSRegExpNode(l->pattern,l->flags);};}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 204 "grammar.y"
    { (yyval.node) = new QSPropertyStrNode(*((yyvsp[(1) - (1)].ustr))); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 205 "grammar.y"
    { (yyval.node) = new QSPropertyStrNode(*((yyvsp[(1) - (1)].ustr))); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 206 "grammar.y"
    { (yyval.node) = new QSPropertyNumNode((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 210 "grammar.y"
    { (yyval.node) = new QSPropertyValueNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 212 "grammar.y"
    { (yyval.node) = new QSPropertyValueNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node), (yyvsp[(1) - (5)].node)); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 217 "grammar.y"
    { (yyval.node) = new QSObjectLiteralNode(0L); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 218 "grammar.y"
    { (yyval.node) = new QSObjectLiteralNode((yyvsp[(2) - (3)].node)); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 220 "grammar.y"
    { (yyval.node) = new QSObjectLiteralNode((yyvsp[(2) - (4)].node)); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 224 "grammar.y"
    { (yyval.node) = newThisNode();/*new QSThisNode();*/ ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 227 "grammar.y"
    { (yyval.node) = new QSResolveNode(*((yyvsp[(1) - (1)].ustr))); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 228 "grammar.y"
    { (yyval.node) = new QSGroupNode((yyvsp[(2) - (3)].node)); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 232 "grammar.y"
    { (yyval.node) = new QSArrayNode((yyvsp[(2) - (3)].eli)); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 233 "grammar.y"
    { (yyval.node) = new QSArrayNode((yyvsp[(2) - (3)].elm)); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 234 "grammar.y"
    { (yyval.node) = new QSArrayNode((yyvsp[(4) - (5)].eli), (yyvsp[(2) - (5)].elm)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 238 "grammar.y"
    { (yyval.elm) = new QSElementNode((yyvsp[(1) - (2)].eli), (yyvsp[(2) - (2)].node)); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 240 "grammar.y"
    { (yyval.elm) = new QSElementNode((yyvsp[(1) - (4)].elm), (yyvsp[(3) - (4)].eli), (yyvsp[(4) - (4)].node)); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 244 "grammar.y"
    { (yyval.eli) = 0L; ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 249 "grammar.y"
    { (yyval.eli) = new QSElisionNode(0L); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 250 "grammar.y"
    { (yyval.eli) = new QSElisionNode((yyvsp[(1) - (2)].eli)); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 255 "grammar.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 256 "grammar.y"
    { (yyval.node) = new QSAccessorNode1((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 257 "grammar.y"
    { (yyval.node) = new QSAccessorNode2((yyvsp[(1) - (3)].node), *((yyvsp[(3) - (3)].ustr))); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 258 "grammar.y"
    { (yyval.node) = new QSNewExprNode((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].args)); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 263 "grammar.y"
    { (yyval.node) = new QSAccessorNode1((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 264 "grammar.y"
    { (yyval.node) = new QSAccessorNode2((yyvsp[(1) - (3)].node), *((yyvsp[(3) - (3)].ustr))); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 265 "grammar.y"
    { (yyval.node) = new QSNewExprNode((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].args)); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 270 "grammar.y"
    { (yyval.node) = new QSNewExprNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 275 "grammar.y"
    { (yyval.node) = new QSNewExprNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 279 "grammar.y"
    { (yyval.node) = new QSFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 280 "grammar.y"
    { (yyval.node) = new QSFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 281 "grammar.y"
    { (yyval.node) = new QSAccessorNode1((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 282 "grammar.y"
    { (yyval.node) = new QSAccessorNode2((yyvsp[(1) - (3)].node), *((yyvsp[(3) - (3)].ustr))); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 286 "grammar.y"
    { (yyval.node) = new QSFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 287 "grammar.y"
    { (yyval.node) = new QSFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 288 "grammar.y"
    { (yyval.node) = new QSAccessorNode1((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 289 "grammar.y"
    { (yyval.node) = new QSAccessorNode2((yyvsp[(1) - (3)].node), *((yyvsp[(3) - (3)].ustr))); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 293 "grammar.y"
    { (yyval.args) = new QSArgumentsNode(0L); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 294 "grammar.y"
    { (yyval.args) = new QSArgumentsNode((yyvsp[(2) - (3)].alist)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 298 "grammar.y"
    { (yyval.alist) = new QSArgumentListNode((yyvsp[(1) - (1)].node)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 299 "grammar.y"
    { (yyval.alist) = new QSArgumentListNode((yyvsp[(1) - (3)].alist), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 314 "grammar.y"
    { (yyval.node) = new QSPostfixNode((yyvsp[(1) - (2)].node), OpPlusPlus); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 315 "grammar.y"
    { (yyval.node) = new QSPostfixNode((yyvsp[(1) - (2)].node), OpMinusMinus); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 320 "grammar.y"
    { (yyval.node) = new QSPostfixNode((yyvsp[(1) - (2)].node), OpPlusPlus); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 321 "grammar.y"
    { (yyval.node) = new QSPostfixNode((yyvsp[(1) - (2)].node), OpMinusMinus); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 325 "grammar.y"
    { (yyval.node) = new QSDeleteNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 326 "grammar.y"
    { (yyval.node) = new QSVoidNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 327 "grammar.y"
    { (yyval.node) = new QSTypeOfNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 328 "grammar.y"
    { (yyval.node) = new QSPrefixNode(OpPlusPlus, (yyvsp[(2) - (2)].node)); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 329 "grammar.y"
    { (yyval.node) = new QSPrefixNode(OpMinusMinus, (yyvsp[(2) - (2)].node)); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 330 "grammar.y"
    { (yyval.node) = new QSUnaryPlusNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 331 "grammar.y"
    { (yyval.node) = new QSNegateNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 332 "grammar.y"
    { (yyval.node) = new QSBitwiseNotNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 333 "grammar.y"
    { (yyval.node) = new QSLogicalNotNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 348 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '*'); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 349 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '/'); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 350 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),'%'); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 356 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '*'); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 358 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '/'); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 360 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),'%'); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 365 "grammar.y"
    { (yyval.node) = new QSAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '+'); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 366 "grammar.y"
    { (yyval.node) = new QSAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '-'); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 372 "grammar.y"
    { (yyval.node) = new QSAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '+'); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 374 "grammar.y"
    { (yyval.node) = new QSAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '-'); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 379 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpLShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 380 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpRShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 381 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpURShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 386 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpLShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 387 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpRShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 388 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpURShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 393 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 394 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 395 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 396 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 397 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpIs, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 398 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 399 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpIn, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 404 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 405 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 406 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 407 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 409 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 414 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 415 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 416 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 417 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 418 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpIs, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 420 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 421 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpIn, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 426 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 427 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 428 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 429 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 435 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 437 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 439 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 441 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 447 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 449 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 451 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 453 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 458 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 464 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 469 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 474 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 480 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 486 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 491 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 497 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 503 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 508 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 514 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 520 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 525 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 531 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 537 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 543 "grammar.y"
    { (yyval.node) = new QSConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 549 "grammar.y"
    { (yyval.node) = new QSConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 555 "grammar.y"
    { (yyval.node) = new QSConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 561 "grammar.y"
    { (yyval.node) = new QSAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 567 "grammar.y"
    { (yyval.node) = new QSAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 573 "grammar.y"
    { (yyval.node) = new QSAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 577 "grammar.y"
    { (yyval.op) = OpEqual; ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 578 "grammar.y"
    { (yyval.op) = OpPlusEq; ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 579 "grammar.y"
    { (yyval.op) = OpMinusEq; ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 580 "grammar.y"
    { (yyval.op) = OpMultEq; ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 581 "grammar.y"
    { (yyval.op) = OpDivEq; ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 582 "grammar.y"
    { (yyval.op) = OpLShift; ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 583 "grammar.y"
    { (yyval.op) = OpRShift; ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 584 "grammar.y"
    { (yyval.op) = OpURShift; ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 585 "grammar.y"
    { (yyval.op) = OpAndEq; ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 586 "grammar.y"
    { (yyval.op) = OpXOrEq; ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 587 "grammar.y"
    { (yyval.op) = OpOrEq; ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 588 "grammar.y"
    { (yyval.op) = OpModEq; ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 593 "grammar.y"
    { (yyval.node) = new QSCommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 598 "grammar.y"
    { (yyval.node) = new QSCommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 603 "grammar.y"
    { (yyval.node) = new QSCommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 609 "grammar.y"
    { (yyval.stat) = (yyvsp[(1) - (1)].clss); ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 610 "grammar.y"
    { static_cast<QSClassDefNode *>((yyvsp[(2) - (2)].clss))->setAttributes( (yyvsp[(1) - (2)].attrs) ); (yyval.stat) = (yyvsp[(2) - (2)].clss); ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 628 "grammar.y"
    { (yyval.stat) = new QSBlockNode(0L); DBG((yyval.stat), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 629 "grammar.y"
    { (yyval.stat) = new QSBlockNode((yyvsp[(2) - (3)].slist)); DBG((yyval.stat), (yylsp[(3) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 633 "grammar.y"
    { (yyval.slist) = new QSStatListNode((yyvsp[(1) - (1)].stat)); ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 634 "grammar.y"
    { (yyval.slist) = new QSStatListNode((yyvsp[(1) - (2)].slist), (yyvsp[(2) - (2)].stat)); ;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 638 "grammar.y"
    { (yyval.stat) = new QSEmptyStatementNode(); ;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 642 "grammar.y"
    { (yyval.stat) = new QSExprStatementNode((yyvsp[(1) - (2)].node)); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 643 "grammar.y"
    { if (automatic()) { (yyval.stat) = new QSExprStatementNode((yyvsp[(1) - (2)].node)); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); } else YYABORT; ;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 647 "grammar.y"
    { (yyval.stat) = new QSIfNode((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].stat),0L); DBG((yyval.stat),(yylsp[(1) - (5)]),(yylsp[(4) - (5)])); ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 648 "grammar.y"
    { (yyval.stat) = new QSIfNode((yyvsp[(3) - (7)].node),(yyvsp[(5) - (7)].stat),(yyvsp[(7) - (7)].stat)); DBG((yyval.stat),(yylsp[(1) - (7)]),(yylsp[(4) - (7)])); ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 652 "grammar.y"
    { (yyval.stat) = new QSDoWhileNode((yyvsp[(2) - (7)].stat),(yyvsp[(5) - (7)].node)); DBG((yyval.stat),(yylsp[(1) - (7)]),(yylsp[(3) - (7)])); ;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 654 "grammar.y"
    { (yyval.stat) = new QSDoWhileNode((yyvsp[(2) - (7)].stat),(yyvsp[(5) - (7)].node)); DBG((yyval.stat),(yylsp[(1) - (7)]),(yylsp[(3) - (7)])); ;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 655 "grammar.y"
    { (yyval.stat) = new QSWhileNode((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].stat)); DBG((yyval.stat),(yylsp[(1) - (5)]),(yylsp[(4) - (5)])); ;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 657 "grammar.y"
    { (yyval.stat) = new QSForNode((yyvsp[(3) - (9)].node),(yyvsp[(5) - (9)].node),(yyvsp[(7) - (9)].node),(yyvsp[(9) - (9)].stat)); DBG((yyval.stat),(yylsp[(1) - (9)]),(yylsp[(8) - (9)])); ;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 659 "grammar.y"
    { (yyval.stat) = new QSForNode((yyvsp[(4) - (10)].blist),(yyvsp[(6) - (10)].node),(yyvsp[(8) - (10)].node),(yyvsp[(10) - (10)].stat)); DBG((yyval.stat),(yylsp[(1) - (10)]),(yylsp[(9) - (10)])); ;}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 661 "grammar.y"
    { (yyval.stat) = new QSForInNode((yyvsp[(3) - (7)].node), (yyvsp[(5) - (7)].node), (yyvsp[(7) - (7)].stat)); DBG((yyval.stat),(yylsp[(1) - (7)]),(yylsp[(6) - (7)])); ;}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 663 "grammar.y"
    { (yyval.stat) = new QSForInNode((yyvsp[(4) - (8)].vbin),(yyvsp[(6) - (8)].node),(yyvsp[(8) - (8)].stat)); DBG((yyval.stat),(yylsp[(1) - (8)]),(yylsp[(7) - (8)])); ;}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 667 "grammar.y"
    { (yyval.node) = 0L; ;}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 672 "grammar.y"
    { (yyval.node) = 0L; ;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 677 "grammar.y"
    { (yyval.stat) = new QSContinueVoidNode(); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 678 "grammar.y"
    { if (automatic()) { (yyval.stat) = new QSContinueVoidNode(); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)])); } else YYABORT; ;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 679 "grammar.y"
    { (yyval.stat) = new QSContinueNode(*((yyvsp[(2) - (3)].ustr))); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(3) - (3)])); ;}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 680 "grammar.y"
    { if (automatic()) { (yyval.stat) = new QSContinueNode(*((yyvsp[(2) - (3)].ustr)));DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)])); } else YYABORT; ;}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 684 "grammar.y"
    { (yyval.stat) = new QSBreakVoidNode();DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 685 "grammar.y"
    { if (automatic()) { (yyval.stat) = new QSBreakVoidNode(); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(1) - (2)])); } else YYABORT; ;}
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 686 "grammar.y"
    { (yyval.stat) = new QSBreakNode(*((yyvsp[(2) - (3)].ustr))); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(3) - (3)])); ;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 687 "grammar.y"
    { if (automatic()) { (yyval.stat) = new QSBreakNode(*((yyvsp[(2) - (3)].ustr))); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)])); } else YYABORT; ;}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 691 "grammar.y"
    { (yyval.stat) = new QSReturnVoidNode(); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 692 "grammar.y"
    { if (automatic()) { (yyval.stat) = new QSReturnVoidNode(); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(1) - (2)])); } else YYABORT; ;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 693 "grammar.y"
    { (yyval.stat) = new QSReturnNode((yyvsp[(2) - (3)].node)); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)])); ;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 694 "grammar.y"
    { if (automatic()) { (yyval.stat) = new QSReturnNode((yyvsp[(2) - (3)].node)); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)])); } else YYABORT; ;}
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 698 "grammar.y"
    { (yyval.stat) = new QSWithNode((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].stat)); DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); ;}
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 702 "grammar.y"
    { (yyval.stat) = new QSSwitchNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].cblk)); DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); ;}
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 706 "grammar.y"
    { (yyval.cblk) = new QSCaseBlockNode((yyvsp[(2) - (3)].clist), 0L, 0L); ;}
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 708 "grammar.y"
    { (yyval.cblk) = new QSCaseBlockNode((yyvsp[(2) - (5)].clist), (yyvsp[(3) - (5)].ccl), (yyvsp[(4) - (5)].clist)); ;}
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 712 "grammar.y"
    { (yyval.clist) = 0L; ;}
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 717 "grammar.y"
    { (yyval.clist) = new QSClauseListNode((yyvsp[(1) - (1)].ccl)); ;}
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 718 "grammar.y"
    { (yyval.clist) = static_cast<QSClauseListNode *>((yyvsp[(1) - (2)].clist))->append((yyvsp[(2) - (2)].ccl)); ;}
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 722 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode((yyvsp[(2) - (3)].node), 0L); ;}
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 723 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].slist)); ;}
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 727 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode(0L, 0L);; ;}
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 728 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode(0L, (yyvsp[(3) - (3)].slist)); ;}
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 732 "grammar.y"
    { (yyval.stat) = new QSEmptyStatementNode(); DBG((yyval.stat),(yylsp[(1) - (5)]),(yylsp[(3) - (5)])); YYABORT; ;}
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 733 "grammar.y"
    { (yyval.stat) = new QSLabelNode(*((yyvsp[(1) - (3)].ustr)), (yyvsp[(3) - (3)].stat)); ;}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 737 "grammar.y"
    { (yyval.stat) = new QSThrowNode((yyvsp[(2) - (3)].node)); ;}
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 741 "grammar.y"
    { (yyval.stat) = new QSTryNode((yyvsp[(2) - (3)].stat), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 264:

/* Line 1455 of yacc.c  */
#line 742 "grammar.y"
    { (yyval.stat) = new QSTryNode((yyvsp[(2) - (3)].stat), 0L, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 265:

/* Line 1455 of yacc.c  */
#line 743 "grammar.y"
    { (yyval.stat) = new QSTryNode((yyvsp[(2) - (4)].stat), (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].node)); ;}
    break;

  case 266:

/* Line 1455 of yacc.c  */
#line 747 "grammar.y"
    { (yyval.node) = new QSCatchNode(*((yyvsp[(3) - (5)].ustr)), (yyvsp[(5) - (5)].stat)); ;}
    break;

  case 267:

/* Line 1455 of yacc.c  */
#line 751 "grammar.y"
    { (yyval.node) = new QSFinallyNode((yyvsp[(2) - (2)].stat)); ;}
    break;

  case 268:

/* Line 1455 of yacc.c  */
#line 755 "grammar.y"
    { (yyval.stat) = new QSPackageNode(*((yyvsp[(2) - (3)].ustr)), (yyvsp[(3) - (3)].body)); ;}
    break;

  case 269:

/* Line 1455 of yacc.c  */
#line 759 "grammar.y"
    { (yyval.stat) = new QSImportNode(*((yyvsp[(2) - (3)].ustr))); ;}
    break;

  case 270:

/* Line 1455 of yacc.c  */
#line 763 "grammar.y"
    { (yyval.ustr) = (yyvsp[(1) - (1)].ustr); ;}
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 764 "grammar.y"
    { (yyval.ustr) = (yyvsp[(3) - (3)].ustr); /**$$ += QString::fromLatin1(".") + *($3);*/ ;}
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 769 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode(*((yyvsp[(2) - (6)].ustr)), 0L, (yyvsp[(5) - (6)].type), (yyvsp[(6) - (6)].body)); ;}
    break;

  case 273:

/* Line 1455 of yacc.c  */
#line 771 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode(*((yyvsp[(2) - (7)].ustr)), (yyvsp[(4) - (7)].param), (yyvsp[(6) - (7)].type), (yyvsp[(7) - (7)].body)); ;}
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 773 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode(*((yyvsp[(3) - (7)].ustr)), 0L, (yyvsp[(6) - (7)].type), (yyvsp[(7) - (7)].body)); static_cast<QSFuncDeclNode *>((yyval.func))->setAttributes((yyvsp[(1) - (7)].attrs)); ;}
    break;

  case 275:

/* Line 1455 of yacc.c  */
#line 775 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode(*((yyvsp[(3) - (8)].ustr)), (yyvsp[(5) - (8)].param), (yyvsp[(7) - (8)].type), (yyvsp[(8) - (8)].body)); 
                                    static_cast<QSFuncDeclNode *>((yyval.func))->setAttributes( (yyvsp[(1) - (8)].attrs) ); ;}
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 780 "grammar.y"
    { (yyval.type) = 0L; ;}
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 781 "grammar.y"
    { (yyval.type) = (yyvsp[(2) - (2)].type); ;}
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 785 "grammar.y"
    { (yyval.node) = new QSFuncExprNode(0L, (yyvsp[(4) - (4)].body)); ;}
    break;

  case 279:

/* Line 1455 of yacc.c  */
#line 787 "grammar.y"
    { (yyval.node) = new QSFuncExprNode((yyvsp[(3) - (5)].param), (yyvsp[(5) - (5)].body)); ;}
    break;

  case 280:

/* Line 1455 of yacc.c  */
#line 791 "grammar.y"
    { (yyval.param) = new QSParameterNode(*((yyvsp[(1) - (1)].ustr)), 0L); ;}
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 792 "grammar.y"
    { (yyval.param) = new QSParameterNode(*((yyvsp[(1) - (3)].ustr)), (yyvsp[(3) - (3)].type)); ; ;}
    break;

  case 282:

/* Line 1455 of yacc.c  */
#line 793 "grammar.y"
    { (yyval.param) = static_cast<QSParameterNode *>((yyvsp[(1) - (3)].param))->append(*((yyvsp[(3) - (3)].ustr)), 0L); ;}
    break;

  case 283:

/* Line 1455 of yacc.c  */
#line 795 "grammar.y"
    { (yyval.param) = static_cast<QSParameterNode *>((yyvsp[(1) - (5)].param))->append(*((yyvsp[(3) - (5)].ustr)), (yyvsp[(5) - (5)].type)); ;}
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 799 "grammar.y"
    { (yyval.body) = new QSFunctionBodyNode(0L); ;}
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 800 "grammar.y"
    { (yyval.body) = new QSFunctionBodyNode((yyvsp[(2) - (3)].srcs)); ;}
    break;

  case 286:

/* Line 1455 of yacc.c  */
#line 804 "grammar.y"
    { (yyval.clss) = new QSClassDefNode(*((yyvsp[(2) - (3)].ustr)), 0L, 0L); ;}
    break;

  case 287:

/* Line 1455 of yacc.c  */
#line 805 "grammar.y"
    { (yyval.clss) = new QSClassDefNode(*((yyvsp[(2) - (3)].ustr)), 0L, (yyvsp[(3) - (3)].body)); ;}
    break;

  case 288:

/* Line 1455 of yacc.c  */
#line 807 "grammar.y"
    { (yyval.clss) = new QSClassDefNode(*((yyvsp[(2) - (5)].ustr)), (yyvsp[(4) - (5)].type), (yyvsp[(5) - (5)].body)); ;}
    break;

  case 289:

/* Line 1455 of yacc.c  */
#line 811 "grammar.y"
    { (yyval.prog) = new QSProgramNode(0L); QSLexer::lexer()->clearUstr(); ;}
    break;

  case 290:

/* Line 1455 of yacc.c  */
#line 812 "grammar.y"
    { (yyval.prog) = new QSProgramNode((yyvsp[(1) - (1)].srcs)); QSLexer::lexer()->clearUstr(); ;}
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 816 "grammar.y"
    { (yyval.srcs) = new QSSourceElementsNode((yyvsp[(1) - (1)].src)); ;}
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 817 "grammar.y"
    { (yyval.srcs) = new QSSourceElementsNode((yyvsp[(1) - (2)].srcs), (yyvsp[(2) - (2)].src)); ;}
    break;

  case 293:

/* Line 1455 of yacc.c  */
#line 821 "grammar.y"
    { (yyval.src) = new QSSourceElementNode((yyvsp[(1) - (1)].stat)); ;}
    break;

  case 294:

/* Line 1455 of yacc.c  */
#line 822 "grammar.y"
    { (yyval.src) = new QSSourceElementNode((yyvsp[(1) - (1)].func)); ;}
    break;

  case 295:

/* Line 1455 of yacc.c  */
#line 826 "grammar.y"
    { (yyval.attrs) = new QSAttributeNode((yyvsp[(1) - (1)].attr)); ;}
    break;

  case 296:

/* Line 1455 of yacc.c  */
#line 827 "grammar.y"
    { static_cast<QSAttributeNode *>((yyvsp[(1) - (2)].attrs))->add((yyvsp[(2) - (2)].attr)); (yyval.attrs) = (yyvsp[(1) - (2)].attrs); ;}
    break;

  case 297:

/* Line 1455 of yacc.c  */
#line 831 "grammar.y"
    { (yyval.attr) = AttributeAbstract; ;}
    break;

  case 298:

/* Line 1455 of yacc.c  */
#line 832 "grammar.y"
    { (yyval.attr) = AttributeFinal; ;}
    break;

  case 299:

/* Line 1455 of yacc.c  */
#line 833 "grammar.y"
    { (yyval.attr) = AttributePrivate; ;}
    break;

  case 300:

/* Line 1455 of yacc.c  */
#line 834 "grammar.y"
    { (yyval.attr) = AttributePublic; ;}
    break;

  case 301:

/* Line 1455 of yacc.c  */
#line 835 "grammar.y"
    { (yyval.attr) = AttributeStatic; ;}
    break;

  case 302:

/* Line 1455 of yacc.c  */
#line 836 "grammar.y"
    { (yyval.attr) = AttributeTrue; ;}
    break;

  case 303:

/* Line 1455 of yacc.c  */
#line 837 "grammar.y"
    { (yyval.attr) = AttributeFalse; ;}
    break;

  case 304:

/* Line 1455 of yacc.c  */
#line 838 "grammar.y"
    { (yyval.attr) = AttributeConstructor; ;}
    break;

  case 305:

/* Line 1455 of yacc.c  */
#line 845 "grammar.y"
    { (yyval.stat) = new QSVarDefNode((yyvsp[(1) - (3)].ival), (yyvsp[(2) - (3)].blist)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 306:

/* Line 1455 of yacc.c  */
#line 847 "grammar.y"
    { if (automatic()) { (yyval.stat) = new QSVarDefNode((yyvsp[(1) - (3)].ival), (yyvsp[(2) - (3)].blist)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); } else YYABORT; ;}
    break;

  case 307:

/* Line 1455 of yacc.c  */
#line 849 "grammar.y"
    { (yyval.stat) = new QSVarDefNode((yyvsp[(2) - (4)].ival), (yyvsp[(3) - (4)].blist)); DBG((yyval.stat), (yylsp[(1) - (4)]), (yylsp[(4) - (4)])); 
                             			  static_cast<QSVarDefNode *>((yyval.stat))->setAttributes((yyvsp[(1) - (4)].attrs)); ;}
    break;

  case 308:

/* Line 1455 of yacc.c  */
#line 852 "grammar.y"
    { if (automatic()) { (yyval.stat) = new QSVarDefNode((yyvsp[(2) - (4)].ival), (yyvsp[(3) - (4)].blist)); DBG((yyval.stat), (yylsp[(1) - (4)]), (yylsp[(3) - (4)])); 
                             			  static_cast<QSVarDefNode *>((yyval.stat))->setAttributes((yyvsp[(1) - (4)].attrs)); } else YYABORT ;}
    break;

  case 309:

/* Line 1455 of yacc.c  */
#line 857 "grammar.y"
    { (yyval.ival) = 0; ;}
    break;

  case 310:

/* Line 1455 of yacc.c  */
#line 858 "grammar.y"
    { (yyval.ival) = 1; ;}
    break;

  case 311:

/* Line 1455 of yacc.c  */
#line 862 "grammar.y"
    { (yyval.blist) = new QSVarBindingListNode(0L, (yyvsp[(1) - (1)].vbin)); ;}
    break;

  case 312:

/* Line 1455 of yacc.c  */
#line 864 "grammar.y"
    { (yyval.blist) = new QSVarBindingListNode((yyvsp[(1) - (3)].blist), (yyvsp[(3) - (3)].vbin)); ;}
    break;

  case 313:

/* Line 1455 of yacc.c  */
#line 868 "grammar.y"
    { (yyval.blist) = new QSVarBindingListNode(0L, (yyvsp[(1) - (1)].vbin)); ;}
    break;

  case 314:

/* Line 1455 of yacc.c  */
#line 870 "grammar.y"
    { (yyval.blist) = new QSVarBindingListNode((yyvsp[(1) - (3)].blist), (yyvsp[(3) - (3)].vbin)); ;}
    break;

  case 315:

/* Line 1455 of yacc.c  */
#line 874 "grammar.y"
    { (yyval.vbin) = new QSVarBindingNode((yyvsp[(1) - (1)].tvar), 0L); ;}
    break;

  case 316:

/* Line 1455 of yacc.c  */
#line 875 "grammar.y"
    { (yyval.vbin) = new QSVarBindingNode((yyvsp[(1) - (3)].tvar), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 317:

/* Line 1455 of yacc.c  */
#line 879 "grammar.y"
    { (yyval.vbin) = new QSVarBindingNode((yyvsp[(1) - (1)].tvar), 0L); ;}
    break;

  case 318:

/* Line 1455 of yacc.c  */
#line 881 "grammar.y"
    { (yyval.vbin) = new QSVarBindingNode((yyvsp[(1) - (3)].tvar), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 319:

/* Line 1455 of yacc.c  */
#line 885 "grammar.y"
    { (yyval.tvar) = new QSTypedVarNode(*((yyvsp[(1) - (1)].ustr)), 0L); ;}
    break;

  case 320:

/* Line 1455 of yacc.c  */
#line 886 "grammar.y"
    { (yyval.tvar) = new QSTypedVarNode(*((yyvsp[(1) - (3)].ustr)), (yyvsp[(3) - (3)].type)); ;}
    break;

  case 321:

/* Line 1455 of yacc.c  */
#line 892 "grammar.y"
    { (yyval.type) = new QSTypeNode(*((yyvsp[(1) - (1)].ustr))); ;}
    break;



/* Line 1455 of yacc.c  */
#line 4048 "grammar.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
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

  yyerror_range[0] = yylsp[1-yylen];
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
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



/* Line 1675 of yacc.c  */
#line 895 "grammar.y"


int yyerror ( const char *errstr )  /* Called by yyparse on error */
{
  QSLexer::lexer()->setErrorMessage(errstr);
  return 1;
}

/* may we automatically insert a semicolon ? */
bool automatic()
{
  if ( yychar == '}' || yychar == 0 )
    return true;
  else if ( QSLexer::lexer()->prevTerminator() )
    return true;

  return false;
}

