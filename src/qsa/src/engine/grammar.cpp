
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
#define YYERROR_VERBOSE
#define DBG(l, s, e) { l->setLoc(s.first_line, e.last_line); } // location
#else
#undef YYLSP_NEEDED
#define DBG(l, s, e)
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
  QSAttribute            attr;
  Operator               op;



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
#define YYFINAL  200
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1418

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  96
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  74
/* YYNRULES -- Number of rules.  */
#define YYNRULES  225
/* YYNRULES -- Number of states.  */
#define YYNSTATES  410

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
      73,    74,    86,    82,    79,    83,    81,    72,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    80,    95,
      88,    94,    89,    93,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    77,     2,    78,    91,     2,     2,     2,     2,     2,
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
      19,    21,    23,    27,    30,    34,    38,    42,    48,    51,
      56,    57,    59,    61,    64,    68,    74,    76,    78,    80,
      82,    84,    89,    93,    97,    99,   102,   105,   108,   113,
     117,   120,   124,   126,   130,   132,   134,   136,   139,   142,
     144,   147,   150,   153,   156,   159,   162,   165,   168,   171,
     173,   177,   181,   185,   187,   191,   195,   197,   201,   205,
     209,   211,   215,   219,   223,   227,   231,   235,   239,   241,
     245,   249,   253,   257,   259,   263,   265,   269,   271,   275,
     277,   281,   283,   287,   289,   295,   297,   301,   303,   305,
     307,   309,   311,   313,   315,   317,   319,   321,   323,   325,
     327,   331,   333,   335,   337,   340,   342,   344,   346,   348,
     350,   352,   354,   356,   358,   360,   362,   364,   366,   368,
     371,   375,   377,   380,   382,   385,   388,   394,   402,   409,
     415,   425,   436,   444,   453,   454,   456,   459,   462,   466,
     470,   473,   476,   480,   484,   487,   490,   494,   498,   504,
     510,   514,   520,   521,   523,   525,   528,   532,   537,   540,
     544,   548,   554,   558,   562,   566,   571,   577,   580,   584,
     588,   590,   594,   601,   609,   617,   626,   627,   630,   635,
     641,   643,   647,   651,   657,   660,   664,   668,   672,   678,
     679,   681,   683,   686,   688,   690,   692,   695,   697,   699,
     701,   703,   705,   707,   709,   711,   714,   718,   720,   722,
     724,   728,   730,   734,   736,   740
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     159,     0,    -1,     3,    -1,     4,    -1,     5,    -1,    69,
      -1,    70,    -1,    72,    -1,    18,    -1,    71,    -1,    97,
      -1,    99,    -1,    73,   126,    74,    -1,    75,    76,    -1,
      75,   103,    76,    -1,    77,   101,    78,    -1,    77,   100,
      78,    -1,    77,   100,    79,   101,    78,    -1,   101,   124,
      -1,   100,    79,   101,   124,    -1,    -1,   102,    -1,    79,
      -1,   102,    79,    -1,   104,    80,   124,    -1,   103,    79,
     104,    80,   124,    -1,    71,    -1,    70,    -1,    69,    -1,
      98,    -1,   155,    -1,   105,    77,   126,    78,    -1,   105,
      81,    71,    -1,    10,   105,   108,    -1,   105,    -1,    10,
     106,    -1,   105,   108,    -1,   107,   108,    -1,   107,    77,
     126,    78,    -1,   107,    81,    71,    -1,    73,    74,    -1,
      73,   109,    74,    -1,   124,    -1,   109,    79,   124,    -1,
     106,    -1,   107,    -1,   110,    -1,   110,    53,    -1,   110,
      54,    -1,   111,    -1,    16,   112,    -1,    15,   112,    -1,
      24,   112,    -1,    53,   112,    -1,    54,   112,    -1,    82,
     112,    -1,    83,   112,    -1,    84,   112,    -1,    85,   112,
      -1,   112,    -1,   113,    86,   112,    -1,   113,    72,   112,
      -1,   113,    87,   112,    -1,   113,    -1,   114,    82,   113,
      -1,   114,    83,   113,    -1,   114,    -1,   115,    55,   114,
      -1,   115,    56,   114,    -1,   115,    57,   114,    -1,   115,
      -1,   116,    88,   115,    -1,   116,    89,   115,    -1,   116,
      49,   115,    -1,   116,    50,   115,    -1,   116,    25,   115,
      -1,   116,    23,   115,    -1,   116,    22,   115,    -1,   116,
      -1,   117,    45,   116,    -1,   117,    46,   116,    -1,   117,
      47,   116,    -1,   117,    48,   116,    -1,   117,    -1,   118,
      90,   117,    -1,   118,    -1,   119,    91,   118,    -1,   119,
      -1,   120,    92,   119,    -1,   120,    -1,   121,    52,   120,
      -1,   121,    -1,   122,    51,   121,    -1,   122,    -1,   122,
      93,   124,    80,   124,    -1,   123,    -1,   110,   125,   124,
      -1,    94,    -1,    58,    -1,    59,    -1,    60,    -1,    61,
      -1,    62,    -1,    63,    -1,    64,    -1,    65,    -1,    67,
      -1,    68,    -1,    66,    -1,   124,    -1,   126,    79,   124,
      -1,   128,    -1,   164,    -1,   158,    -1,   162,   158,    -1,
     150,    -1,   130,    -1,   131,    -1,   132,    -1,   133,    -1,
     135,    -1,   136,    -1,   137,    -1,   138,    -1,   139,    -1,
     145,    -1,   146,    -1,   147,    -1,   151,    -1,    75,    76,
      -1,    75,   129,    76,    -1,   127,    -1,   129,   127,    -1,
      95,    -1,   126,    95,    -1,   126,     1,    -1,    17,    73,
     126,    74,   127,    -1,    17,    73,   126,    74,   127,    21,
     127,    -1,    19,   127,    20,    73,   126,    74,    -1,    20,
      73,   126,    74,   127,    -1,     9,    73,   134,    95,   134,
      95,   134,    74,   127,    -1,     9,    73,    11,   166,    95,
     134,    95,   134,    74,   127,    -1,     9,    73,   110,    22,
     126,    74,   127,    -1,     9,    73,    11,   167,    22,   126,
      74,   127,    -1,    -1,   126,    -1,    12,    95,    -1,    12,
       1,    -1,    12,    71,    95,    -1,    12,    71,     1,    -1,
       6,    95,    -1,     6,     1,    -1,     6,    71,    95,    -1,
       6,    71,     1,    -1,    14,    95,    -1,    14,     1,    -1,
      14,   126,    95,    -1,    14,   126,     1,    -1,    27,    73,
     126,    74,   127,    -1,    26,    73,   126,    74,   140,    -1,
      75,   141,    76,    -1,    75,   141,   144,   141,    76,    -1,
      -1,   142,    -1,   143,    -1,   142,   143,    -1,     7,   126,
      80,    -1,     7,   126,    80,   129,    -1,     8,    80,    -1,
       8,    80,   129,    -1,    71,    80,   127,    -1,    71,    80,
      71,    94,     1,    -1,    29,   126,    95,    -1,    30,   128,
     148,    -1,    30,   128,   149,    -1,    30,   128,   148,   149,
      -1,    31,    73,    71,    74,   128,    -1,    32,   128,    -1,
      42,   152,   157,    -1,    43,   152,    95,    -1,    71,    -1,
     152,    81,    71,    -1,    13,    71,    73,    74,   154,   157,
      -1,    13,    71,    73,   156,    74,   154,   157,    -1,   162,
      13,    71,    73,    74,   154,   157,    -1,   162,    13,    71,
      73,   156,    74,   154,   157,    -1,    -1,    80,   169,    -1,
      13,    73,    74,   157,    -1,    13,    73,   156,    74,   157,
      -1,    71,    -1,    71,    80,   169,    -1,   156,    79,    71,
      -1,   156,    79,    71,    80,   169,    -1,    75,    76,    -1,
      75,   160,    76,    -1,    33,    71,    95,    -1,    33,    71,
     157,    -1,    33,    71,    35,   169,   157,    -1,    -1,   160,
      -1,   161,    -1,   160,   161,    -1,   127,    -1,   153,    -1,
     163,    -1,   162,   163,    -1,    36,    -1,    37,    -1,    38,
      -1,    39,    -1,    40,    -1,     4,    -1,     5,    -1,    34,
      -1,   165,   166,    -1,   162,   165,   166,    -1,    11,    -1,
      41,    -1,   167,    -1,   166,    79,   167,    -1,   168,    -1,
     168,    94,   124,    -1,    71,    -1,    71,    80,   169,    -1,
      71,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   175,   175,   176,   177,   178,   179,   180,   186,   187,
     188,   189,   190,   191,   192,   196,   197,   198,   202,   203,
     208,   209,   213,   214,   218,   219,   224,   225,   226,   230,
     231,   232,   233,   234,   238,   239,   243,   244,   245,   246,
     250,   251,   255,   256,   260,   261,   265,   266,   267,   271,
     272,   273,   274,   275,   276,   277,   278,   279,   280,   284,
     285,   286,   287,   291,   292,   293,   297,   298,   299,   300,
     304,   305,   307,   309,   311,   313,   315,   317,   322,   323,
     324,   325,   326,   330,   331,   335,   336,   340,   341,   345,
     346,   351,   352,   357,   358,   363,   364,   369,   370,   371,
     372,   373,   374,   375,   376,   377,   378,   379,   380,   384,
     385,   389,   390,   391,   392,   393,   394,   395,   396,   397,
     398,   399,   400,   401,   402,   403,   404,   405,   406,   410,
     411,   415,   416,   420,   424,   426,   433,   434,   438,   439,
     440,   443,   446,   449,   455,   456,   460,   461,   465,   466,
     472,   473,   476,   477,   483,   484,   487,   488,   495,   500,
     505,   506,   511,   512,   516,   517,   521,   522,   526,   527,
     531,   533,   537,   541,   542,   543,   547,   551,   555,   559,
     563,   564,   568,   570,   572,   574,   579,   580,   584,   585,
     591,   592,   593,   594,   599,   600,   604,   605,   606,   611,
     612,   616,   617,   621,   622,   626,   627,   631,   632,   633,
     634,   635,   636,   637,   638,   644,   646,   652,   653,   657,
     658,   663,   664,   668,   669,   675
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
  "XOREQUAL", "OREQUAL", "NUMBER", "STRING", "IDENT", "'/'", "'('", "')'",
  "'{'", "'}'", "'['", "']'", "','", "':'", "'.'", "'+'", "'-'", "'~'",
  "'!'", "'*'", "'%'", "'<'", "'>'", "'&'", "'^'", "'|'", "'?'", "'='",
  "';'", "$accept", "Literal", "PrimaryExpr", "ArrayLiteral",
  "ElementList", "ElisionOpt", "Elision", "PropertyNameAndValueList",
  "PropertyName", "MemberExpr", "NewExpr", "CallExpr", "Arguments",
  "ArgumentList", "LeftHandSideExpr", "PostfixExpr", "UnaryExpr",
  "MultiplicativeExpr", "AdditiveExpr", "ShiftExpr", "RelationalExpr",
  "EqualityExpr", "BitwiseANDExpr", "BitwiseXORExpr", "BitwiseORExpr",
  "LogicalANDExpr", "LogicalORExpr", "ConditionalExpr", "AssignmentExpr",
  "AssignmentOperator", "Expr", "Statement", "Block", "StatementList",
  "EmptyStatement", "ExprStatement", "IfStatement", "IterationStatement",
  "ExprOpt", "ContinueStatement", "BreakStatement", "ReturnStatement",
  "WithStatement", "SwitchStatement", "CaseBlock", "CaseClausesOpt",
  "CaseClauses", "CaseClause", "DefaultClause", "LabelledStatement",
  "ThrowStatement", "TryStatement", "Catch", "Finally",
  "PackageDefinition", "ImportStatement", "PackageName",
  "FunctionDeclaration", "ResultSignature", "FunctionExpr",
  "FormalParameterList", "FunctionBody", "ClassDefinition", "Program",
  "SourceElements", "SourceElement", "Attributes", "Attribute",
  "VariableDefinition", "VariableDefinitionKind", "VariableBindingList",
  "VariableBinding", "TypedVariable", "TypeExpression", 0
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
     325,   326,    47,    40,    41,   123,   125,    91,    93,    44,
      58,    46,    43,    45,   126,    33,    42,    37,    60,    62,
      38,    94,   124,    63,    61,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    96,    97,    97,    97,    97,    97,    97,    98,    98,
      98,    98,    98,    98,    98,    99,    99,    99,   100,   100,
     101,   101,   102,   102,   103,   103,   104,   104,   104,   105,
     105,   105,   105,   105,   106,   106,   107,   107,   107,   107,
     108,   108,   109,   109,   110,   110,   111,   111,   111,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   113,
     113,   113,   113,   114,   114,   114,   115,   115,   115,   115,
     116,   116,   116,   116,   116,   116,   116,   116,   117,   117,
     117,   117,   117,   118,   118,   119,   119,   120,   120,   121,
     121,   122,   122,   123,   123,   124,   124,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   126,
     126,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   128,
     128,   129,   129,   130,   131,   131,   132,   132,   133,   133,
     133,   133,   133,   133,   134,   134,   135,   135,   135,   135,
     136,   136,   136,   136,   137,   137,   137,   137,   138,   139,
     140,   140,   141,   141,   142,   142,   143,   143,   144,   144,
     145,   145,   146,   147,   147,   147,   148,   149,   150,   151,
     152,   152,   153,   153,   153,   153,   154,   154,   155,   155,
     156,   156,   156,   156,   157,   157,   158,   158,   158,   159,
     159,   160,   160,   161,   161,   162,   162,   163,   163,   163,
     163,   163,   163,   163,   163,   164,   164,   165,   165,   166,
     166,   167,   167,   168,   168,   169
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     2,     3,     3,     3,     5,     2,     4,
       0,     1,     1,     2,     3,     5,     1,     1,     1,     1,
       1,     4,     3,     3,     1,     2,     2,     2,     4,     3,
       2,     3,     1,     3,     1,     1,     1,     2,     2,     1,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     1,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     3,
       1,     3,     3,     3,     3,     3,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     5,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       3,     1,     2,     1,     2,     2,     5,     7,     6,     5,
       9,    10,     7,     8,     0,     1,     2,     2,     3,     3,
       2,     2,     3,     3,     2,     2,     3,     3,     5,     5,
       3,     5,     0,     1,     1,     2,     3,     4,     2,     3,
       3,     5,     3,     3,     3,     4,     5,     2,     3,     3,
       1,     3,     6,     7,     7,     8,     0,     2,     4,     5,
       1,     3,     3,     5,     2,     3,     3,     3,     5,     0,
       1,     1,     2,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     1,     1,     1,
       3,     1,     3,     1,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     199,     2,     3,     4,     0,     0,     0,   217,     0,     0,
       0,     0,     0,     0,     8,     0,     0,     0,     0,     0,
       0,     0,     0,   214,   207,   208,   209,   210,   211,   218,
       0,     0,     0,     0,     5,     6,     9,     7,     0,     0,
      20,     0,     0,     0,     0,   133,    10,    29,    11,    34,
      44,    45,    46,    49,    59,    63,    66,    70,    78,    83,
      85,    87,    89,    91,    93,    95,   109,     0,   203,   111,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   115,   128,   204,    30,   113,     0,   200,   201,
       0,   205,   112,     0,   151,     0,   150,   144,     3,     4,
       0,     9,     0,    34,    35,   147,     0,   146,     0,     0,
     155,   154,     0,    46,    51,    50,     0,     0,     0,     0,
      52,     0,     0,     0,     0,     0,     0,   180,     0,     0,
      53,    54,     0,     0,     5,     6,     9,    13,     0,     0,
     131,     0,    22,     0,     0,    21,    55,    56,    57,    58,
       0,     0,     0,    36,     0,     0,    37,    47,    48,    98,
      99,   100,   101,   102,   103,   104,   105,   108,   106,   107,
      97,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   135,     0,   134,
       1,   202,   212,   213,     0,   114,   206,     0,   223,   215,
     219,   221,   153,   152,     0,    46,   145,     0,    28,    27,
      26,    13,    33,   149,   148,     0,   190,     0,     0,   157,
     156,     0,     0,     0,     0,     0,   172,   129,     0,     0,
     173,   174,     0,     0,   196,   197,     0,   178,   179,     9,
     170,    12,    14,     0,     0,   130,   132,    16,    20,    15,
      18,    23,    40,     0,    42,     0,    32,     0,    39,    96,
      61,    60,    62,    64,    65,    67,    68,    69,    77,    76,
      75,    73,    74,    71,    72,    79,    80,    81,    82,    84,
      86,    88,    90,    92,     0,   110,     0,   216,     0,     0,
       0,     0,   219,     0,   144,   186,     0,     0,   188,     0,
       0,     0,     0,     0,     0,     0,     0,   177,   175,   225,
       0,   194,     0,   181,     0,     0,    24,     0,    41,     0,
      31,    38,     0,     0,   224,   220,   222,   144,     0,     0,
       0,     0,     0,   186,   191,   189,   192,   136,     0,   139,
     162,   159,   158,     0,   198,   195,   171,     0,    17,    19,
      43,    94,   186,     0,     0,     0,     0,   144,   187,   182,
       0,     0,     0,   138,     0,     0,   163,   164,     0,    25,
       0,   186,   144,     0,   142,     0,   183,   193,   137,     0,
       0,   160,   162,   165,   176,   184,     0,     0,   143,     0,
     166,   168,     0,   185,     0,   140,   167,   169,   161,   141
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    46,    47,    48,   143,   144,   145,   138,   139,    49,
      50,    51,   153,   263,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,   171,
      67,    68,    69,   141,    70,    71,    72,    73,   217,    74,
      75,    76,    77,    78,   351,   375,   376,   377,   392,    79,
      80,    81,   240,   241,    82,    83,   128,    84,   342,    85,
     228,   245,    86,    87,    88,    89,   118,    91,    92,    93,
     209,   210,   211,   320
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -318
static const yytype_int16 yypact[] =
{
     850,  -318,   341,  1329,    16,   -41,   238,  -318,    21,    23,
      66,  1276,  1276,    -7,  -318,   937,     4,  1276,    27,    33,
    1276,    -2,    31,  -318,  -318,  -318,  -318,  -318,  -318,  -318,
      37,    37,  1276,  1276,  -318,  -318,    43,  -318,  1276,   415,
      61,  1276,  1276,  1276,  1276,  -318,  -318,  -318,  -318,    24,
    -318,    81,   273,  -318,  -318,   -31,    50,   235,   106,   213,
      54,    55,    67,   101,   -32,  -318,  -318,    19,  -318,  -318,
    -318,  -318,  -318,  -318,  -318,  -318,  -318,  -318,  -318,  -318,
    -318,  -318,  -318,  -318,  -318,  -318,  -318,   163,   850,  -318,
    1339,  -318,  -318,   110,  -318,     8,  -318,   199,  -318,  -318,
     119,  -318,   113,    24,  -318,  -318,    29,  -318,   145,    86,
    -318,  -318,    20,     0,  -318,  -318,  1276,   179,  1377,  1276,
    -318,  1276,  1276,   -48,   502,   156,   -12,  -318,   -23,   -38,
    -318,  -318,  1024,     1,   142,   165,    43,  1117,   122,   182,
    -318,   589,  -318,   223,  1158,   128,  -318,  -318,  -318,  -318,
    1200,  1276,   196,  -318,  1276,   204,  -318,  -318,  -318,  -318,
    -318,  -318,  -318,  -318,  -318,  -318,  -318,  -318,  -318,  -318,
    -318,  1276,  1276,  1276,  1276,  1276,  1276,  1276,  1276,  1276,
    1276,  1276,  1276,  1276,  1276,  1276,  1276,  1276,  1276,  1276,
    1276,  1276,  1276,  1276,  1276,  1276,  1276,  -318,  1276,  -318,
    -318,  -318,  -318,  -318,   216,  -318,  -318,   110,   217,   171,
    -318,   205,  -318,  -318,   110,   112,   233,   198,  -318,  -318,
    -318,  -318,  -318,  -318,  -318,   214,   234,   249,    48,  -318,
    -318,    51,   257,    73,   111,   126,  -318,  -318,   271,    -2,
     321,  -318,   284,   676,  -318,  -318,   285,  -318,  -318,   -29,
    -318,  -318,  -318,   247,  1276,  -318,  -318,  -318,    61,  -318,
    -318,  -318,  -318,   134,  -318,   242,  -318,   244,  -318,  -318,
    -318,  -318,  -318,   -31,   -31,    50,    50,    50,   235,   235,
     235,   235,   235,   235,   235,   106,   106,   106,   106,   213,
      54,    55,    67,   101,   280,  -318,   288,   171,   284,   110,
    1276,   -45,   340,  1276,  1276,   283,   137,   284,  -318,   249,
     293,   937,  1276,   937,   291,   937,   298,  -318,  -318,  -318,
     249,  -318,   763,  -318,   369,   292,  -318,  1234,  -318,  1276,
    -318,  -318,  1276,   215,  -318,  -318,  -318,  1276,  1276,   157,
     276,   284,   249,   283,  -318,  -318,   296,   362,   158,  -318,
     380,  -318,  -318,   314,  -318,  -318,  -318,  1276,  -318,  -318,
    -318,  -318,   283,   161,   295,   170,   937,  1276,  -318,  -318,
     249,   284,   937,  -318,  1276,    10,   380,  -318,    -2,  -318,
     249,   283,  1276,   937,  -318,   319,  -318,  -318,  -318,   263,
     342,  -318,   380,  -318,  -318,  -318,   249,   349,  -318,   937,
     937,   937,   360,  -318,   937,  -318,   937,   937,  -318,  -318
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -318,  -318,  -318,  -318,  -318,   180,  -318,  -318,   184,   434,
     437,  -318,   -18,  -318,   222,  -318,    -5,   172,   127,    44,
      90,   255,   258,   254,   265,   266,  -318,  -318,  -136,  -318,
      -9,   -15,   -19,   -51,  -318,  -318,  -318,  -318,  -289,  -318,
    -318,  -318,  -318,  -318,  -318,    68,  -318,    87,  -318,  -318,
    -318,  -318,  -318,   224,  -318,  -318,   431,  -318,  -317,  -318,
    -212,  -123,   -50,  -318,   227,   -84,     3,   -46,  -318,   -44,
    -165,  -204,  -318,  -282
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -214
static const yytype_int16 yytable[] =
{
     117,   112,   125,    90,   201,   247,   114,   115,   260,   212,
     302,   123,   120,   306,   264,   340,   334,    94,   390,   195,
     197,   229,   105,   242,   140,   344,   370,   130,   131,   133,
     223,   198,    97,   156,   299,   269,   146,   147,   148,   149,
     205,   172,   297,   246,   206,   380,   207,   236,   364,   301,
     337,   132,   243,   157,   158,   173,   174,   248,   246,   368,
     294,   196,   295,   243,   396,   324,   116,   110,   205,     1,
      98,    99,   206,   124,   207,   251,     6,   119,   385,   100,
     198,    11,    12,   244,    14,   222,   391,    95,   216,   387,
      17,    90,   106,   397,   108,   335,   109,   150,   198,   198,
     121,   151,   126,   213,   308,   152,   122,   231,   127,   140,
     233,    96,   234,   235,   199,   230,   107,   250,   326,    32,
      33,   363,   309,   132,   224,   311,   256,   310,   180,   181,
     198,   182,   175,   176,   303,    34,    35,   101,    37,    38,
     142,   102,   265,    40,   191,   267,   192,   313,    41,    42,
      43,    44,   198,   194,   150,   183,   184,   226,   154,   193,
     227,   111,   155,   200,   336,   157,   158,   270,   271,   272,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   208,   218,   219,   220,   314,   345,   238,   239,   221,
     198,   359,   109,   360,   185,   186,   361,   354,   252,   232,
     315,   253,     1,    98,    99,   198,   170,   261,   328,     6,
     214,   343,   100,   329,    11,    12,   310,    14,   225,   369,
     317,   379,   -28,    17,   278,   279,   280,   281,   282,   283,
     284,   366,   373,   113,   113,   381,   198,   198,   201,   113,
     310,     1,    98,    99,   383,   -27,    90,   386,     6,   198,
     299,   100,    32,    33,   113,   113,    14,   395,   187,   188,
     189,   190,   254,   113,   113,   113,   113,   266,    34,    35,
     101,    37,    38,   403,   102,   268,    40,   285,   286,   287,
     288,    41,    42,    43,    44,   226,   226,   296,   305,   362,
     177,   178,   179,   304,   339,   216,   347,   298,   349,   300,
     352,   257,   258,   348,   275,   276,   277,    34,    35,   101,
      37,    38,   198,   102,   307,    40,   218,   219,   220,   215,
     330,   198,   331,   198,   243,    90,   157,   158,   216,   365,
     312,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   198,   400,   316,  -212,  -212,   273,   274,   406,
     407,   384,  -212,   239,  -212,   319,   323,   388,   216,   394,
     332,   333,   338,   341,   346,   389,   350,   170,   398,   353,
     356,   367,   357,   216,  -212,  -212,   371,  -212,  -212,  -212,
    -212,  -212,  -212,   372,   405,   140,   140,   374,   378,   409,
     382,   256,   256,   399,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,     1,     2,
       3,     4,   401,   404,     5,     6,     7,     8,   100,    10,
      11,    12,    13,    14,    15,    16,   408,   325,   327,    17,
     103,    18,    19,   104,    20,    21,   289,   291,    22,    23,
     290,    24,    25,    26,    27,    28,    29,    30,    31,   292,
     402,   293,   129,   393,   318,     0,     0,     0,    32,    33,
     322,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   134,   135,   136,    37,    38,     0,
      39,   137,    40,     0,     0,     0,     0,    41,    42,    43,
      44,     0,     0,     0,     0,     1,     2,     3,     4,     0,
      45,     5,     6,     7,     8,   100,    10,    11,    12,    13,
      14,    15,    16,     0,     0,     0,    17,     0,    18,    19,
       0,    20,    21,     0,     0,    22,    23,     0,    24,    25,
      26,    27,    28,    29,    30,    31,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    32,    33,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    34,    35,    36,    37,    38,     0,    39,   237,    40,
       0,     0,     0,     0,    41,    42,    43,    44,     0,     0,
       0,     0,     1,     2,     3,     4,     0,    45,     5,     6,
       7,     8,   100,    10,    11,    12,    13,    14,    15,    16,
       0,     0,     0,    17,     0,    18,    19,     0,    20,    21,
       0,     0,    22,    23,     0,    24,    25,    26,    27,    28,
      29,    30,    31,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    32,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,    38,     0,    39,   255,    40,     0,     0,     0,
       0,    41,    42,    43,    44,     0,     0,     0,     0,     1,
       2,     3,     4,     0,    45,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,     0,     0,
      17,     0,    18,    19,     0,    20,    21,     0,     0,    22,
      23,     0,    24,    25,    26,    27,    28,    29,    30,    31,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    32,
      33,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    34,    35,    36,    37,    38,
       0,    39,   321,    40,     0,     0,     0,     0,    41,    42,
      43,    44,     0,     0,     0,     0,     1,     2,     3,     4,
       0,    45,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,     0,     0,     0,    17,     0,    18,
      19,     0,    20,    21,     0,     0,    22,    23,     0,    24,
      25,    26,    27,    28,    29,    30,    31,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    32,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    34,    35,    36,    37,    38,     0,    39,   355,
      40,     0,     0,     0,     0,    41,    42,    43,    44,     0,
       0,     0,     0,     1,     2,     3,     4,     0,    45,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,     0,     0,     0,    17,     0,    18,    19,     0,    20,
      21,     0,     0,    22,    23,     0,    24,    25,    26,    27,
      28,    29,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    32,    33,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    34,
      35,    36,    37,    38,     0,    39,     0,    40,     0,     0,
       0,     0,    41,    42,    43,    44,     0,     0,     0,     0,
       1,     2,     3,     4,     0,    45,     5,     6,     7,     8,
     100,    10,    11,    12,    13,    14,    15,    16,     0,     0,
       0,    17,     0,    18,    19,     0,    20,    21,     0,     0,
      22,    23,     0,    24,    25,    26,    27,    28,    29,    30,
      31,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      32,    33,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    34,    35,    36,    37,
      38,     0,    39,     0,    40,     0,     0,     0,     0,    41,
      42,    43,    44,     0,     0,     0,     0,     1,     2,     3,
       4,     0,    45,     5,     6,     7,     8,   100,    10,    11,
      12,    13,    14,    15,    16,     0,     0,     0,    17,     0,
      18,    19,     0,    20,    21,     0,     0,    22,    23,     0,
      24,    25,    26,    27,    28,    29,    30,    31,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    32,    33,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    34,    35,   249,    37,    38,     0,    39,
       0,    40,     0,     0,     0,     0,    41,    42,    43,    44,
       0,     0,     0,     0,     0,     0,     0,  -129,     0,    45,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,     0,
       0,  -129,     0,  -129,  -129,     0,  -129,  -129,     0,     0,
    -129,  -129,     0,  -129,  -129,  -129,  -129,  -129,  -129,  -129,
    -129,     1,    98,    99,     0,     0,     0,     0,     6,     0,
       0,   100,     0,    11,    12,     0,    14,     0,     0,     0,
       0,     0,    17,     0,     0,     0,  -129,  -129,  -129,     0,
       0,     0,  -129,  -129,     0,     0,     0,     0,     0,     0,
       0,  -129,  -129,     1,    98,    99,     0,     0,     0,     0,
       6,    32,    33,   100,     0,    11,    12,     0,    14,     0,
       0,     0,     0,     0,    17,     0,     0,    34,    35,   101,
      37,    38,     0,   102,     0,    40,   259,     1,    98,    99,
      41,    42,    43,    44,     6,     0,     0,   100,     0,    11,
      12,     0,    14,    32,    33,     0,     0,     0,    17,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    34,
      35,   101,    37,    38,   262,   102,     0,    40,     0,     1,
      98,    99,    41,    42,    43,    44,     6,    32,    33,   100,
       0,    11,    12,     0,    14,     0,     0,     0,     0,     0,
      17,     0,     0,    34,    35,   101,    37,    38,     0,   102,
       0,    40,   358,     0,     0,     0,    41,    42,    43,    44,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    32,
      33,     0,     0,  -213,  -213,     0,     0,     0,     0,     0,
    -213,     0,  -213,   202,   203,    34,    35,   101,    37,    38,
       7,   102,   204,    40,     0,     0,     0,     0,    41,    42,
      43,    44,  -213,  -213,     0,  -213,  -213,  -213,  -213,  -213,
    -213,     0,    22,    23,     0,    24,    25,    26,    27,    28,
      29,   202,   203,     0,     0,     0,     0,     0,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      22,    23,     0,    24,    25,    26,    27,    28,    29
};

static const yytype_int16 yycheck[] =
{
      15,    10,    21,     0,    88,   128,    11,    12,   144,     1,
     214,    20,    17,   225,   150,   304,   298,     1,     8,    51,
       1,     1,     1,    35,    39,   307,   343,    32,    33,    38,
       1,    79,    73,    51,    79,   171,    41,    42,    43,    44,
      90,    72,   207,    81,    90,   362,    90,    95,   337,   214,
      95,    80,    75,    53,    54,    86,    87,    95,    81,   341,
     196,    93,   198,    75,   381,    94,    73,     1,   118,     3,
       4,     5,   118,    75,   118,    74,    10,    73,   367,    13,
      79,    15,    16,    95,    18,   103,    76,    71,    97,   371,
      24,    88,    71,   382,    71,   299,    73,    73,    79,    79,
      73,    77,    71,    95,   227,    81,    73,   116,    71,   124,
     119,    95,   121,   122,    95,    95,    95,   132,   254,    53,
      54,   333,    74,    80,    95,    74,   141,    79,    22,    23,
      79,    25,    82,    83,    22,    69,    70,    71,    72,    73,
      79,    75,   151,    77,    90,   154,    91,    74,    82,    83,
      84,    85,    79,    52,    73,    49,    50,    71,    77,    92,
      74,    95,    81,     0,   300,    53,    54,   172,   173,   174,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    71,    69,    70,    71,    74,   309,    31,    32,    76,
      79,   327,    73,   329,    88,    89,   332,   320,    76,    20,
      74,    79,     3,     4,     5,    79,    94,    79,    74,    10,
      11,    74,    13,    79,    15,    16,    79,    18,    73,   342,
     239,   357,    80,    24,   180,   181,   182,   183,   184,   185,
     186,    74,    74,    11,    12,    74,    79,    79,   322,    17,
      79,     3,     4,     5,    74,    80,   243,   370,    10,    79,
      79,    13,    53,    54,    32,    33,    18,   380,    45,    46,
      47,    48,    80,    41,    42,    43,    44,    71,    69,    70,
      71,    72,    73,   396,    75,    71,    77,   187,   188,   189,
     190,    82,    83,    84,    85,    71,    71,    71,    74,    74,
      55,    56,    57,    95,   303,   304,   311,    80,   313,    94,
     315,    78,    79,   312,   177,   178,   179,    69,    70,    71,
      72,    73,    79,    75,    80,    77,    69,    70,    71,    97,
      78,    79,    78,    79,    75,   322,    53,    54,   337,   338,
      73,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    79,    80,    73,     4,     5,   175,   176,   400,
     401,   366,    11,    32,    13,    71,    71,   372,   367,   378,
      80,    73,    22,    80,    71,   374,    75,    94,   383,    71,
       1,    95,    80,   382,    33,    34,    80,    36,    37,    38,
      39,    40,    41,    21,   399,   400,   401,     7,    74,   404,
      95,   406,   407,    74,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,     3,     4,
       5,     6,    80,    74,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    76,   253,   258,    24,
       6,    26,    27,     6,    29,    30,   191,   193,    33,    34,
     192,    36,    37,    38,    39,    40,    41,    42,    43,   194,
     392,   195,    31,   376,   240,    -1,    -1,    -1,    53,    54,
     243,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,    -1,
      75,    76,    77,    -1,    -1,    -1,    -1,    82,    83,    84,
      85,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      95,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    -1,    -1,    -1,    24,    -1,    26,    27,
      -1,    29,    30,    -1,    -1,    33,    34,    -1,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    53,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    69,    70,    71,    72,    73,    -1,    75,    76,    77,
      -1,    -1,    -1,    -1,    82,    83,    84,    85,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,    -1,    95,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    24,    -1,    26,    27,    -1,    29,    30,
      -1,    -1,    33,    34,    -1,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,
      71,    72,    73,    -1,    75,    76,    77,    -1,    -1,    -1,
      -1,    82,    83,    84,    85,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    95,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    -1,    -1,
      24,    -1,    26,    27,    -1,    29,    30,    -1,    -1,    33,
      34,    -1,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,
      -1,    75,    76,    77,    -1,    -1,    -1,    -1,    82,    83,
      84,    85,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    95,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    24,    -1,    26,
      27,    -1,    29,    30,    -1,    -1,    33,    34,    -1,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    53,    54,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    70,    71,    72,    73,    -1,    75,    76,
      77,    -1,    -1,    -1,    -1,    82,    83,    84,    85,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,    -1,    95,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    -1,    -1,    24,    -1,    26,    27,    -1,    29,
      30,    -1,    -1,    33,    34,    -1,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,
      70,    71,    72,    73,    -1,    75,    -1,    77,    -1,    -1,
      -1,    -1,    82,    83,    84,    85,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,    95,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    -1,
      -1,    24,    -1,    26,    27,    -1,    29,    30,    -1,    -1,
      33,    34,    -1,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,
      73,    -1,    75,    -1,    77,    -1,    -1,    -1,    -1,    82,
      83,    84,    85,    -1,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,    95,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    -1,    -1,    24,    -1,
      26,    27,    -1,    29,    30,    -1,    -1,    33,    34,    -1,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    54,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    69,    70,    71,    72,    73,    -1,    75,
      -1,    77,    -1,    -1,    -1,    -1,    82,    83,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,    95,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    -1,
      -1,    24,    -1,    26,    27,    -1,    29,    30,    -1,    -1,
      33,    34,    -1,    36,    37,    38,    39,    40,    41,    42,
      43,     3,     4,     5,    -1,    -1,    -1,    -1,    10,    -1,
      -1,    13,    -1,    15,    16,    -1,    18,    -1,    -1,    -1,
      -1,    -1,    24,    -1,    -1,    -1,    69,    70,    71,    -1,
      -1,    -1,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,     3,     4,     5,    -1,    -1,    -1,    -1,
      10,    53,    54,    13,    -1,    15,    16,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    69,    70,    71,
      72,    73,    -1,    75,    -1,    77,    78,     3,     4,     5,
      82,    83,    84,    85,    10,    -1,    -1,    13,    -1,    15,
      16,    -1,    18,    53,    54,    -1,    -1,    -1,    24,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,
      70,    71,    72,    73,    74,    75,    -1,    77,    -1,     3,
       4,     5,    82,    83,    84,    85,    10,    53,    54,    13,
      -1,    15,    16,    -1,    18,    -1,    -1,    -1,    -1,    -1,
      24,    -1,    -1,    69,    70,    71,    72,    73,    -1,    75,
      -1,    77,    78,    -1,    -1,    -1,    82,    83,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,
      54,    -1,    -1,     4,     5,    -1,    -1,    -1,    -1,    -1,
      11,    -1,    13,     4,     5,    69,    70,    71,    72,    73,
      11,    75,    13,    77,    -1,    -1,    -1,    -1,    82,    83,
      84,    85,    33,    34,    -1,    36,    37,    38,    39,    40,
      41,    -1,    33,    34,    -1,    36,    37,    38,    39,    40,
      41,     4,     5,    -1,    -1,    -1,    -1,    -1,    11,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    34,    -1,    36,    37,    38,    39,    40,    41
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    24,    26,    27,
      29,    30,    33,    34,    36,    37,    38,    39,    40,    41,
      42,    43,    53,    54,    69,    70,    71,    72,    73,    75,
      77,    82,    83,    84,    85,    95,    97,    98,    99,   105,
     106,   107,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   126,   127,   128,
     130,   131,   132,   133,   135,   136,   137,   138,   139,   145,
     146,   147,   150,   151,   153,   155,   158,   159,   160,   161,
     162,   163,   164,   165,     1,    71,    95,    73,     4,     5,
      13,    71,    75,   105,   106,     1,    71,    95,    71,    73,
       1,    95,   126,   110,   112,   112,    73,   127,   162,    73,
     112,    73,    73,   126,    75,   128,    71,    71,   152,   152,
     112,   112,    80,   126,    69,    70,    71,    76,   103,   104,
     127,   129,    79,   100,   101,   102,   112,   112,   112,   112,
      73,    77,    81,   108,    77,    81,   108,    53,    54,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      94,   125,    72,    86,    87,    82,    83,    55,    56,    57,
      22,    23,    25,    49,    50,    88,    89,    45,    46,    47,
      48,    90,    91,    92,    52,    51,    93,     1,    79,    95,
       0,   161,     4,     5,    13,   158,   163,   165,    71,   166,
     167,   168,     1,    95,    11,   110,   126,   134,    69,    70,
      71,    76,   108,     1,    95,    73,    71,    74,   156,     1,
      95,   126,    20,   126,   126,   126,    95,    76,    31,    32,
     148,   149,    35,    75,    95,   157,    81,   157,    95,    71,
     127,    74,    76,    79,    80,    76,   127,    78,    79,    78,
     124,    79,    74,   109,   124,   126,    71,   126,    71,   124,
     112,   112,   112,   113,   113,   114,   114,   114,   115,   115,
     115,   115,   115,   115,   115,   116,   116,   116,   116,   117,
     118,   119,   120,   121,   124,   124,    71,   166,    80,    79,
      94,   166,   167,    22,    95,    74,   156,    80,   157,    74,
      79,    74,    73,    74,    74,    74,    73,   128,   149,    71,
     169,    76,   160,    71,    94,   104,   124,   101,    74,    79,
      78,    78,    80,    73,   169,   167,   124,    95,    22,   126,
     134,    80,   154,    74,   169,   157,    71,   127,   126,   127,
      75,   140,   127,    71,   157,    76,     1,    80,    78,   124,
     124,   124,    74,   156,   134,   126,    74,    95,   169,   157,
     154,    80,    21,    74,     7,   141,   142,   143,    74,   124,
     154,    74,    95,    74,   127,   134,   157,   169,   127,   126,
       8,    76,   144,   143,   128,   157,   154,   134,   127,    74,
      80,    80,   141,   157,    74,   127,   129,   129,    76,   127
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
#line 175 "grammar.y"
    { (yyval.node) = new QSNullNode(); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 176 "grammar.y"
    { (yyval.node) = new QSBooleanNode(true); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 177 "grammar.y"
    { (yyval.node) = new QSBooleanNode(false); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 178 "grammar.y"
    { (yyval.node) = new QSNumberNode((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 179 "grammar.y"
    { (yyval.node) = new QSStringNode(*((yyvsp[(1) - (1)].ustr))); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 180 "grammar.y"
    { QSLexer *l = QSLexer::lexer();
                                     if (!l->scanRegExp()) YYABORT;
                                     (yyval.node) = new QSRegExpNode(l->pattern,l->flags);;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 186 "grammar.y"
    { (yyval.node) = new QSThisNode(); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 187 "grammar.y"
    { (yyval.node) = new QSResolveNode(*((yyvsp[(1) - (1)].ustr))); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 190 "grammar.y"
    { (yyval.node) = new QSGroupNode((yyvsp[(2) - (3)].node)); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 191 "grammar.y"
    { (yyval.node) = new QSObjectLiteralNode(0L); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 192 "grammar.y"
    { (yyval.node) = new QSObjectLiteralNode((yyvsp[(2) - (3)].node)); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 196 "grammar.y"
    { (yyval.node) = new QSArrayNode((yyvsp[(2) - (3)].eli)); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 197 "grammar.y"
    { (yyval.node) = new QSArrayNode((yyvsp[(2) - (3)].elm)); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 198 "grammar.y"
    { (yyval.node) = new QSArrayNode((yyvsp[(4) - (5)].eli), (yyvsp[(2) - (5)].elm)); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 202 "grammar.y"
    { (yyval.elm) = new QSElementNode((yyvsp[(1) - (2)].eli), (yyvsp[(2) - (2)].node)); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 204 "grammar.y"
    { (yyval.elm) = new QSElementNode((yyvsp[(1) - (4)].elm), (yyvsp[(3) - (4)].eli), (yyvsp[(4) - (4)].node)); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 208 "grammar.y"
    { (yyval.eli) = 0L; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 213 "grammar.y"
    { (yyval.eli) = new QSElisionNode(0L); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 214 "grammar.y"
    { (yyval.eli) = new QSElisionNode((yyvsp[(1) - (2)].eli)); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 218 "grammar.y"
    { (yyval.node) = new QSPropertyValueNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 220 "grammar.y"
    { (yyval.node) = new QSPropertyValueNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node), (yyvsp[(1) - (5)].node)); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 224 "grammar.y"
    { (yyval.node) = new QSPropertyNode(*((yyvsp[(1) - (1)].ustr))); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 225 "grammar.y"
    { (yyval.node) = new QSPropertyNode(*((yyvsp[(1) - (1)].ustr))); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 226 "grammar.y"
    { (yyval.node) = new QSPropertyNode((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 232 "grammar.y"
    { (yyval.node) = new QSAccessorNode1((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 233 "grammar.y"
    { (yyval.node) = new QSAccessorNode2((yyvsp[(1) - (3)].node), *((yyvsp[(3) - (3)].ustr))); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 234 "grammar.y"
    { (yyval.node) = new QSNewExprNode((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].args)); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 239 "grammar.y"
    { (yyval.node) = new QSNewExprNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 243 "grammar.y"
    { (yyval.node) = new QSFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 244 "grammar.y"
    { (yyval.node) = new QSFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 245 "grammar.y"
    { (yyval.node) = new QSAccessorNode1((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 246 "grammar.y"
    { (yyval.node) = new QSAccessorNode2((yyvsp[(1) - (3)].node), *((yyvsp[(3) - (3)].ustr))); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 250 "grammar.y"
    { (yyval.args) = new QSArgumentsNode(0L); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 251 "grammar.y"
    { (yyval.args) = new QSArgumentsNode((yyvsp[(2) - (3)].alist)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 255 "grammar.y"
    { (yyval.alist) = new QSArgumentListNode((yyvsp[(1) - (1)].node)); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 256 "grammar.y"
    { (yyval.alist) = new QSArgumentListNode((yyvsp[(1) - (3)].alist), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 266 "grammar.y"
    { (yyval.node) = new QSPostfixNode((yyvsp[(1) - (2)].node), OpPlusPlus); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 267 "grammar.y"
    { (yyval.node) = new QSPostfixNode((yyvsp[(1) - (2)].node), OpMinusMinus); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 272 "grammar.y"
    { (yyval.node) = new QSDeleteNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 273 "grammar.y"
    { (yyval.node) = new QSVoidNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 274 "grammar.y"
    { (yyval.node) = new QSTypeOfNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 275 "grammar.y"
    { (yyval.node) = new QSPrefixNode(OpPlusPlus, (yyvsp[(2) - (2)].node)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 276 "grammar.y"
    { (yyval.node) = new QSPrefixNode(OpMinusMinus, (yyvsp[(2) - (2)].node)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 277 "grammar.y"
    { (yyval.node) = new QSUnaryPlusNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 278 "grammar.y"
    { (yyval.node) = new QSNegateNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 279 "grammar.y"
    { (yyval.node) = new QSBitwiseNotNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 280 "grammar.y"
    { (yyval.node) = new QSLogicalNotNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 285 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '*'); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 286 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '/'); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 287 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),'%'); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 292 "grammar.y"
    { (yyval.node) = new QSAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '+'); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 293 "grammar.y"
    { (yyval.node) = new QSAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '-'); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 298 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpLShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 299 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpRShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 300 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpURShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 306 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 308 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 310 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 312 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 314 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpIs, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 316 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 318 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpIn, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 323 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 324 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 325 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 326 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node));;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 331 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 336 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 341 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 347 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 353 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 359 "grammar.y"
    { (yyval.node) = new QSConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 365 "grammar.y"
    { (yyval.node) = new QSAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node));;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 369 "grammar.y"
    { (yyval.op) = OpEqual; ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 370 "grammar.y"
    { (yyval.op) = OpPlusEq; ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 371 "grammar.y"
    { (yyval.op) = OpMinusEq; ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 372 "grammar.y"
    { (yyval.op) = OpMultEq; ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 373 "grammar.y"
    { (yyval.op) = OpDivEq; ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 374 "grammar.y"
    { (yyval.op) = OpLShift; ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 375 "grammar.y"
    { (yyval.op) = OpRShift; ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 376 "grammar.y"
    { (yyval.op) = OpURShift; ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 377 "grammar.y"
    { (yyval.op) = OpAndEq; ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 378 "grammar.y"
    { (yyval.op) = OpXOrEq; ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 379 "grammar.y"
    { (yyval.op) = OpOrEq; ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 380 "grammar.y"
    { (yyval.op) = OpModEq; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 385 "grammar.y"
    { (yyval.node) = new QSCommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 391 "grammar.y"
    { (yyval.stat) = (yyvsp[(1) - (1)].clss); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 392 "grammar.y"
    { (yyvsp[(2) - (2)].clss)->setAttributes( (yyvsp[(1) - (2)].attrs) ); (yyval.stat) = (yyvsp[(2) - (2)].clss); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 410 "grammar.y"
    { (yyval.stat) = new QSBlockNode(0L); DBG((yyval.stat), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 411 "grammar.y"
    { (yyval.stat) = new QSBlockNode((yyvsp[(2) - (3)].slist)); DBG((yyval.stat), (yylsp[(3) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 415 "grammar.y"
    { (yyval.slist) = new QSStatListNode((yyvsp[(1) - (1)].stat)); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 416 "grammar.y"
    { (yyval.slist) = new QSStatListNode((yyvsp[(1) - (2)].slist), (yyvsp[(2) - (2)].stat)); ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 420 "grammar.y"
    { (yyval.stat) = new QSEmptyStatementNode(); ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 424 "grammar.y"
    { (yyval.stat) = new QSExprStatementNode((yyvsp[(1) - (2)].node));
                                     DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 426 "grammar.y"
    { if (automatic()) {
                                       (yyval.stat) = new QSExprStatementNode((yyvsp[(1) - (2)].node));
                                       DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)]));
                                     } else YYABORT; ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 433 "grammar.y"
    { (yyval.stat) = new QSIfNode((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].stat),0L); DBG((yyval.stat),(yylsp[(1) - (5)]),(yylsp[(4) - (5)])); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 434 "grammar.y"
    { (yyval.stat) = new QSIfNode((yyvsp[(3) - (7)].node),(yyvsp[(5) - (7)].stat),(yyvsp[(7) - (7)].stat)); DBG((yyval.stat),(yylsp[(1) - (7)]),(yylsp[(4) - (7)])); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 438 "grammar.y"
    { (yyval.stat)=new QSDoWhileNode((yyvsp[(2) - (6)].stat),(yyvsp[(5) - (6)].node));DBG((yyval.stat),(yylsp[(1) - (6)]),(yylsp[(3) - (6)]));;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 439 "grammar.y"
    { (yyval.stat) = new QSWhileNode((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].stat));DBG((yyval.stat),(yylsp[(1) - (5)]),(yylsp[(4) - (5)])); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 441 "grammar.y"
    { (yyval.stat) = new QSForNode((yyvsp[(3) - (9)].node),(yyvsp[(5) - (9)].node),(yyvsp[(7) - (9)].node),(yyvsp[(9) - (9)].stat));
	                             DBG((yyval.stat),(yylsp[(1) - (9)]),(yylsp[(8) - (9)])); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 444 "grammar.y"
    { (yyval.stat) = new QSForNode((yyvsp[(4) - (10)].blist),(yyvsp[(6) - (10)].node),(yyvsp[(8) - (10)].node),(yyvsp[(10) - (10)].stat));
	                             DBG((yyval.stat),(yylsp[(1) - (10)]),(yylsp[(9) - (10)])); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 447 "grammar.y"
    { (yyval.stat) = new QSForInNode((yyvsp[(3) - (7)].node), (yyvsp[(5) - (7)].node), (yyvsp[(7) - (7)].stat));
	                             DBG((yyval.stat),(yylsp[(1) - (7)]),(yylsp[(6) - (7)])); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 450 "grammar.y"
    { (yyval.stat) = new QSForInNode((yyvsp[(4) - (8)].vbin),(yyvsp[(6) - (8)].node),(yyvsp[(8) - (8)].stat));
	                             DBG((yyval.stat),(yylsp[(1) - (8)]),(yylsp[(7) - (8)])); ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 455 "grammar.y"
    { (yyval.node) = 0L; ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 460 "grammar.y"
    { (yyval.stat) = new QSContinueNode(); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 461 "grammar.y"
    { if (automatic()) {
                                       (yyval.stat) = new QSContinueNode(); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)]));
                                     } else
				       YYABORT; ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 465 "grammar.y"
    { (yyval.stat) = new QSContinueNode(*((yyvsp[(2) - (3)].ustr))); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(3) - (3)])); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 466 "grammar.y"
    { if (automatic()) {
                                       (yyval.stat) = new QSContinueNode(*((yyvsp[(2) - (3)].ustr)));DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)]));
                                     } else YYABORT; ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 472 "grammar.y"
    { (yyval.stat) = new QSBreakNode();DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 473 "grammar.y"
    { if (automatic()) {
                                       (yyval.stat) = new QSBreakNode(); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(1) - (2)]));
                                     } else YYABORT; ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 476 "grammar.y"
    { (yyval.stat) = new QSBreakNode(*((yyvsp[(2) - (3)].ustr))); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(3) - (3)])); ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 477 "grammar.y"
    { if (automatic()) {
                                       (yyval.stat) = new QSBreakNode(*((yyvsp[(2) - (3)].ustr))); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)]));
                                     } else YYABORT; ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 483 "grammar.y"
    { (yyval.stat) = new QSReturnNode(0L); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 484 "grammar.y"
    { if (automatic()) {
                                       (yyval.stat) = new QSReturnNode(0L); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(1) - (2)]));
                                     } else YYABORT; ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 487 "grammar.y"
    { (yyval.stat) = new QSReturnNode((yyvsp[(2) - (3)].node)); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)])); ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 488 "grammar.y"
    { if (automatic()) {
                                       (yyval.stat) = new QSReturnNode((yyvsp[(2) - (3)].node));
                                       DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)]));
                                     } else YYABORT; ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 495 "grammar.y"
    { (yyval.stat) = new QSWithNode((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].stat));
                                     DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 500 "grammar.y"
    { (yyval.stat) = new QSSwitchNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].cblk));
                                     DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 505 "grammar.y"
    { (yyval.cblk) = new QSCaseBlockNode((yyvsp[(2) - (3)].clist), 0L, 0L); ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 507 "grammar.y"
    { (yyval.cblk) = new QSCaseBlockNode((yyvsp[(2) - (5)].clist), (yyvsp[(3) - (5)].ccl), (yyvsp[(4) - (5)].clist)); ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 511 "grammar.y"
    { (yyval.clist) = 0L; ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 516 "grammar.y"
    { (yyval.clist) = new QSClauseListNode((yyvsp[(1) - (1)].ccl)); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 517 "grammar.y"
    { (yyval.clist) = (yyvsp[(1) - (2)].clist)->append((yyvsp[(2) - (2)].ccl)); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 521 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode((yyvsp[(2) - (3)].node), 0L); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 522 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].slist)); ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 526 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode(0L, 0L);; ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 527 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode(0L, (yyvsp[(3) - (3)].slist)); ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 531 "grammar.y"
    { if ((yyvsp[(1) - (3)].ustr)) (yyvsp[(3) - (3)].stat)->pushLabel(*((yyvsp[(1) - (3)].ustr)));
                                     (yyval.stat) = new QSLabelNode(*((yyvsp[(1) - (3)].ustr)), (yyvsp[(3) - (3)].stat)); ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 533 "grammar.y"
    { DBG((yyval.stat),(yylsp[(1) - (5)]),(yylsp[(3) - (5)])); YYABORT; ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 537 "grammar.y"
    { (yyval.stat) = new QSThrowNode((yyvsp[(2) - (3)].node)); ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 541 "grammar.y"
    { (yyval.stat) = new QSTryNode((yyvsp[(2) - (3)].stat), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 542 "grammar.y"
    { (yyval.stat) = new QSTryNode((yyvsp[(2) - (3)].stat), 0L, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 543 "grammar.y"
    { (yyval.stat) = new QSTryNode((yyvsp[(2) - (4)].stat), (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].node)); ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 547 "grammar.y"
    { (yyval.node) = new QSCatchNode(*((yyvsp[(3) - (5)].ustr)), (yyvsp[(5) - (5)].stat)); ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 551 "grammar.y"
    { (yyval.node) = new QSFinallyNode((yyvsp[(2) - (2)].stat)); ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 555 "grammar.y"
    { (yyval.stat) = new QSPackageNode(*((yyvsp[(2) - (3)].ustr)), (yyvsp[(3) - (3)].body)); ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 559 "grammar.y"
    { (yyval.stat) = new QSImportNode(*((yyvsp[(2) - (3)].ustr))); ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 563 "grammar.y"
    { (yyval.ustr) = (yyvsp[(1) - (1)].ustr); ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 564 "grammar.y"
    { (yyval.ustr) = (yyvsp[(3) - (3)].ustr); /**$$ += QString::fromLatin1(".") + *($3);*/ ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 569 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode(*((yyvsp[(2) - (6)].ustr)), 0L, (yyvsp[(5) - (6)].type), (yyvsp[(6) - (6)].body)); ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 571 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode(*((yyvsp[(2) - (7)].ustr)), (yyvsp[(4) - (7)].param), (yyvsp[(6) - (7)].type), (yyvsp[(7) - (7)].body)); ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 573 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode(*((yyvsp[(3) - (7)].ustr)), 0L, (yyvsp[(6) - (7)].type), (yyvsp[(7) - (7)].body)); (yyval.func)->setAttributes((yyvsp[(1) - (7)].attrs)); ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 575 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode(*((yyvsp[(3) - (8)].ustr)), (yyvsp[(5) - (8)].param), (yyvsp[(7) - (8)].type), (yyvsp[(8) - (8)].body)); (yyval.func)->setAttributes( (yyvsp[(1) - (8)].attrs) ); ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 579 "grammar.y"
    { (yyval.type) = 0L; ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 580 "grammar.y"
    { (yyval.type) = (yyvsp[(2) - (2)].type); ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 584 "grammar.y"
    { (yyval.node) = new QSFuncExprNode(0L, (yyvsp[(4) - (4)].body)); ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 586 "grammar.y"
    { (yyval.node) = new QSFuncExprNode((yyvsp[(3) - (5)].param), (yyvsp[(5) - (5)].body)); ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 591 "grammar.y"
    { (yyval.param) = new QSParameterNode(*((yyvsp[(1) - (1)].ustr)), 0L); ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 592 "grammar.y"
    { (yyval.param) = new QSParameterNode(*((yyvsp[(1) - (3)].ustr)), (yyvsp[(3) - (3)].type)); ; ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 593 "grammar.y"
    { (yyval.param) = (yyvsp[(1) - (3)].param)->append(*((yyvsp[(3) - (3)].ustr)), 0L); ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 595 "grammar.y"
    { (yyval.param) = (yyvsp[(1) - (5)].param)->append(*((yyvsp[(3) - (5)].ustr)), (yyvsp[(5) - (5)].type)); ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 599 "grammar.y"
    { (yyval.body) = new QSFunctionBodyNode(0L); ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 600 "grammar.y"
    { (yyval.body) = new QSFunctionBodyNode((yyvsp[(2) - (3)].srcs)); ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 604 "grammar.y"
    { (yyval.clss) = new QSClassDefNode(*((yyvsp[(2) - (3)].ustr)), 0L, 0L); ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 605 "grammar.y"
    { (yyval.clss) = new QSClassDefNode(*((yyvsp[(2) - (3)].ustr)), 0L, (yyvsp[(3) - (3)].body)); ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 607 "grammar.y"
    { (yyval.clss) = new QSClassDefNode(*((yyvsp[(2) - (5)].ustr)), (yyvsp[(4) - (5)].type), (yyvsp[(5) - (5)].body)); ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 611 "grammar.y"
    { (yyval.prog) = new QSProgramNode(0L); QSLexer::lexer()->clearUstr(); ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 612 "grammar.y"
    { (yyval.prog) = new QSProgramNode((yyvsp[(1) - (1)].srcs)); QSLexer::lexer()->clearUstr(); ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 616 "grammar.y"
    { (yyval.srcs) = new QSSourceElementsNode((yyvsp[(1) - (1)].src)); ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 617 "grammar.y"
    { (yyval.srcs) = new QSSourceElementsNode((yyvsp[(1) - (2)].srcs), (yyvsp[(2) - (2)].src)); ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 621 "grammar.y"
    { (yyval.src) = new QSSourceElementNode((yyvsp[(1) - (1)].stat)); ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 622 "grammar.y"
    { (yyval.src) = new QSSourceElementNode((yyvsp[(1) - (1)].func)); ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 626 "grammar.y"
    { (yyval.attrs) = new QSAttributeNode((yyvsp[(1) - (1)].attr)); ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 627 "grammar.y"
    { (yyvsp[(1) - (2)].attrs)->add((yyvsp[(2) - (2)].attr)); (yyval.attrs) = (yyvsp[(1) - (2)].attrs); ;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 631 "grammar.y"
    { (yyval.attr) = AttributeAbstract; ;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 632 "grammar.y"
    { (yyval.attr) = AttributeFinal; ;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 633 "grammar.y"
    { (yyval.attr) = AttributePrivate; ;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 634 "grammar.y"
    { (yyval.attr) = AttributePublic; ;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 635 "grammar.y"
    { (yyval.attr) = AttributeStatic; ;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 636 "grammar.y"
    { (yyval.attr) = AttributeTrue; ;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 637 "grammar.y"
    { (yyval.attr) = AttributeFalse; ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 638 "grammar.y"
    { (yyval.attr) = AttributeConstructor; ;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 645 "grammar.y"
    { (yyval.stat) = new QSVarDefNode((yyvsp[(1) - (2)].ival), (yyvsp[(2) - (2)].blist)); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 647 "grammar.y"
    { (yyval.stat) = new QSVarDefNode((yyvsp[(2) - (3)].ival), (yyvsp[(3) - (3)].blist)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
                               ((QSVarDefNode*)(yyval.stat))->setAttributes( (yyvsp[(1) - (3)].attrs) ); ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 652 "grammar.y"
    { (yyval.ival) = 0; ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 653 "grammar.y"
    { (yyval.ival) = 1; ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 657 "grammar.y"
    { (yyval.blist) = new QSVarBindingListNode(0L, (yyvsp[(1) - (1)].vbin)); ;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 659 "grammar.y"
    { (yyval.blist) = new QSVarBindingListNode((yyvsp[(1) - (3)].blist), (yyvsp[(3) - (3)].vbin)); ;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 663 "grammar.y"
    { (yyval.vbin) = new QSVarBindingNode((yyvsp[(1) - (1)].tvar), 0L); ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 664 "grammar.y"
    { (yyval.vbin) = new QSVarBindingNode((yyvsp[(1) - (3)].tvar), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 668 "grammar.y"
    { (yyval.tvar) = new QSTypedVarNode(*((yyvsp[(1) - (1)].ustr)), 0L); ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 669 "grammar.y"
    { (yyval.tvar) = new QSTypedVarNode(*((yyvsp[(1) - (3)].ustr)), (yyvsp[(3) - (3)].type)); ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 675 "grammar.y"
    { (yyval.type) = new QSTypeNode(*((yyvsp[(1) - (1)].ustr))); ;}
    break;



/* Line 1455 of yacc.c  */
#line 3450 "grammar.tab.c"
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
#line 678 "grammar.y"


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

