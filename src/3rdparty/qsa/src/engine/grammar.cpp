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
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse qsyyparse
#define yylex   qsyylex
#define yyerror qsyyerror
#define yylval  qsyylval
#define yychar  qsyychar
#define yydebug qsyydebug
#define yynerrs qsyynerrs
#define yylloc qsyylloc

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
/* Tokens.  */
#define NULLTOKEN 258
#define TRUETOKEN 259
#define FALSETOKEN 260
#define BREAK 261
#define CASE 262
#define DEFAULT 263
#define FOR 264
#define NEW 265
#define VAR 266
#define CONTINUE 267
#define FUNCTION 268
#define RETURN 269
#define QS_VOID 270
#define QS_DELETE 271
#define IF 272
#define THIS 273
#define DO 274
#define WHILE 275
#define ELSE 276
#define QS_IN 277
#define INSTANCEOF 278
#define TYPEOF 279
#define IS 280
#define SWITCH 281
#define WITH 282
#define RESERVED 283
#define THROW 284
#define TRY 285
#define CATCH 286
#define FINALLY 287
#define CLASS 288
#define CONSTRUCTOR 289
#define EXTENDS 290
#define ABSTRACT 291
#define FINAL 292
#define PRIVATE 293
#define PUBLIC 294
#define STATIC 295
#define QS_CONST 296
#define PACKAGE 297
#define IMPORT 298
#define IF_WITHOUT_ELSE 299
#define EQEQ 300
#define NE 301
#define STREQ 302
#define STRNEQ 303
#define LE 304
#define GE 305
#define OR 306
#define AND 307
#define PLUSPLUS 308
#define MINUSMINUS 309
#define LSHIFT 310
#define RSHIFT 311
#define URSHIFT 312
#define PLUSEQUAL 313
#define MINUSEQUAL 314
#define MULTEQUAL 315
#define DIVEQUAL 316
#define LSHIFTEQUAL 317
#define RSHIFTEQUAL 318
#define URSHIFTEQUAL 319
#define ANDEQUAL 320
#define MODEQUAL 321
#define XOREQUAL 322
#define OREQUAL 323
#define NUMBER 324
#define STRING 325
#define IDENT 326




/* Copy the first part of user declarations.  */
#line 1 "grammar.y"

/****************************************************************************
**
** Copyright (C) 1992-$THISYEAR$ Trolltech AS. All rights reserved.
** Copyright (C) 2003-$THISYEAR$ InfoSiAL S.L. All rights reserved.
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $LICENSE$
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>
#include "qsengine.h"
#include "qsnodes.h"

#define YYMAXDEPTH 10000
#define YYENABLE_NLS 0

/* default values for bison */
#define YYDEBUG 0

int yylex( YYSTYPE *, YYLTYPE *, QSLexer * );
int yyerror ( YYLTYPE *, QSLexer *, const char * );
bool allowAutomaticSemicolon( int, QSLexer * );

#ifdef QSDEBUGGER
# define YYERROR_VERBOSE
# define DBG(l, s, e) { l->setLoc(s.first_line, e.last_line, lx ); }
#else
# undef YYLSP_NEEDED
# define DBG(l, s, e)
#endif

#define AUTO_SEMICOLON do { if (!allowAutomaticSemicolon(yychar, lx)) YYABORT; } while (0)



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
#line 45 "grammar.y"
{
  int                   ival;
  double                dval;
  uint                  ustr;
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
}
/* Line 187 of yacc.c.  */
#line 321 "grammar.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
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


/* Line 216 of yacc.c.  */
#line 346 "grammar.tab.c"

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
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
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
#define YYFINAL  216
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1411

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  96
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  108
/* YYNRULES -- Number of rules.  */
#define YYNRULES  321
/* YYNRULES -- Number of states.  */
#define YYNSTATES  559

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
     842,   846,   850,   854,   858,   862,   867,   873,   876,   880,
     884,   886,   890,   897,   905,   913,   922,   923,   926,   931,
     937,   939,   943,   947,   953,   956,   960,   964,   968,   974,
     975,   977,   979,   982,   984,   986,   988,   991,   993,   995,
     997,   999,  1001,  1003,  1005,  1007,  1011,  1015,  1020,  1025,
    1027,  1029,  1031,  1035,  1037,  1041,  1043,  1047,  1049,  1053,
    1055,  1059
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
     160,    -1,    71,    73,   158,    -1,    29,   155,    95,    -1,
      29,   155,     1,    -1,    30,   159,   180,    -1,    30,   159,
     181,    -1,    30,   159,   180,   181,    -1,    31,    77,    71,
      78,   159,    -1,    32,   159,    -1,    42,   184,   189,    -1,
      43,   184,    95,    -1,    71,    -1,   184,    81,    71,    -1,
      13,    71,    77,    78,   186,   189,    -1,    13,    71,    77,
     188,    78,   186,   189,    -1,   194,    13,    71,    77,    78,
     186,   189,    -1,   194,    13,    71,    77,   188,    78,   186,
     189,    -1,    -1,    73,   203,    -1,    13,    77,    78,   189,
      -1,    13,    77,   188,    78,   189,    -1,    71,    -1,    71,
      73,   203,    -1,   188,    74,    71,    -1,   188,    74,    71,
      73,   203,    -1,    75,    76,    -1,    75,   192,    76,    -1,
      33,    71,    95,    -1,    33,    71,   189,    -1,    33,    71,
      35,   203,   189,    -1,    -1,   192,    -1,   193,    -1,   192,
     193,    -1,   158,    -1,   185,    -1,   195,    -1,   194,   195,
      -1,    36,    -1,    37,    -1,    38,    -1,    39,    -1,    40,
      -1,     4,    -1,     5,    -1,    34,    -1,   197,   198,    95,
      -1,   197,   198,     1,    -1,   194,   197,   198,    95,    -1,
     194,   197,   198,     1,    -1,    11,    -1,    41,    -1,   200,
      -1,   198,    74,   200,    -1,   201,    -1,   199,    74,   201,
      -1,   202,    -1,   202,    94,   151,    -1,   202,    -1,   202,
      94,   152,    -1,    71,    -1,    71,    73,   203,    -1,    71,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   185,   185,   186,   187,   188,   189,   190,   194,   195,
     196,   200,   201,   206,   207,   208,   209,   214,   215,   216,
     217,   218,   222,   223,   224,   228,   229,   234,   235,   239,
     240,   244,   245,   246,   247,   248,   252,   253,   254,   255,
     259,   260,   264,   265,   269,   270,   271,   272,   276,   277,
     278,   279,   283,   284,   288,   289,   293,   294,   298,   299,
     303,   304,   305,   309,   310,   311,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   327,   328,   332,   333,   337,
     338,   339,   340,   344,   345,   347,   349,   354,   355,   356,
     360,   361,   363,   368,   369,   370,   371,   375,   376,   377,
     378,   382,   383,   384,   385,   386,   387,   388,   389,   393,
     394,   395,   396,   397,   398,   403,   404,   405,   406,   407,
     408,   409,   411,   415,   416,   417,   418,   419,   423,   424,
     426,   428,   430,   435,   436,   438,   440,   442,   447,   448,
     452,   453,   458,   459,   463,   464,   468,   469,   474,   475,
     480,   481,   485,   486,   491,   492,   497,   498,   502,   503,
     508,   509,   514,   515,   519,   520,   525,   526,   531,   532,
     537,   538,   543,   544,   549,   550,   555,   556,   561,   562,
     567,   568,   569,   570,   571,   572,   573,   574,   575,   576,
     577,   578,   582,   583,   587,   588,   592,   593,   597,   598,
     599,   600,   601,   602,   603,   604,   605,   606,   607,   608,
     609,   610,   611,   612,   613,   614,   618,   619,   623,   624,
     628,   632,   633,   637,   639,   644,   645,   647,   648,   650,
     652,   654,   659,   660,   664,   665,   669,   670,   671,   672,
     676,   677,   678,   679,   683,   684,   685,   686,   690,   694,
     698,   699,   704,   705,   709,   710,   714,   715,   719,   720,
     724,   728,   729,   733,   734,   735,   739,   743,   747,   751,
     755,   756,   760,   762,   764,   767,   773,   774,   778,   779,
     784,   785,   786,   787,   792,   793,   797,   798,   799,   804,
     805,   809,   810,   814,   815,   819,   820,   824,   825,   826,
     827,   828,   829,   830,   831,   837,   839,   841,   844,   850,
     851,   855,   856,   861,   862,   867,   868,   872,   873,   878,
     879,   885
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
     177,   178,   178,   179,   179,   179,   180,   181,   182,   183,
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
       3,     3,     3,     3,     3,     4,     5,     2,     3,     3,
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
     261,     0,     0,   263,   264,     0,     0,   286,   287,     0,
     268,   269,   260,   217,   219,    21,    27,    23,    22,    25,
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
     267,   265,   321,     0,   284,     0,   271,     0,     0,    53,
      37,    50,     0,     0,   308,   307,   320,   312,   316,     0,
     232,     0,     0,     0,    60,   177,   114,   112,   113,   110,
     111,   129,   130,   131,   132,   141,   147,   153,   159,   165,
       0,   195,   233,     0,     0,   278,     0,     0,    11,    16,
       0,    33,     0,     0,   276,    46,     0,   223,     0,   227,
     252,   249,   248,     0,   288,   285,    24,    26,    55,   173,
     276,     0,   314,     0,     0,   318,     0,     0,   232,   281,
     282,   279,     0,   277,   272,     0,   169,     0,     0,     0,
       0,   253,   254,     0,     0,   276,   232,     0,   230,   171,
       0,     0,    12,   273,   224,   226,   225,     0,     0,   250,
     252,   255,   266,   274,     0,     0,   231,     0,   283,   256,
     258,     0,   275,     0,   228,   257,   259,   251,   229
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    46,   251,   252,   104,   105,    48,   159,   160,   161,
     115,    49,   116,    50,   117,    51,   169,   322,   135,    52,
     119,    53,   120,   121,    55,   122,    56,   123,    57,   124,
      58,   125,   233,    59,   126,   234,    60,   127,   235,    61,
     128,   236,    62,   129,   237,    63,   130,   238,    64,   131,
     239,    65,   132,   240,    66,   133,   241,    67,   366,   472,
     242,    68,    69,    70,   156,    71,    72,    73,    74,   473,
     243,    75,    76,    77,    78,    79,   491,   520,   521,   522,
     540,    80,    81,    82,   303,   304,    83,    84,   149,    85,
     483,   108,   387,   308,    86,    87,    88,    89,   140,    91,
      92,    93,   225,   362,   226,   363,   227,   433
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -430
static const yytype_int16 yypact[] =
{
     762,  -430,   426,  1128,    10,    -7,   255,  -430,    15,    32,
     238,  1326,  1326,    29,  -430,  1023,    81,  1326,    84,   100,
    1326,    46,   151,  -430,  -430,  -430,  -430,  -430,  -430,  -430,
     176,   176,  1326,  1326,  -430,  -430,   141,  -430,   849,  1326,
      24,  1326,  1326,  1326,  1326,  -430,  -430,  -430,  -430,   197,
    -430,   222,  1020,  -430,  -430,  -430,    -3,   -15,   325,   144,
     190,     3,    96,   169,   219,    -6,  -430,  -430,    16,  -430,
    -430,  -430,  -430,  -430,  -430,  -430,  -430,  -430,  -430,  -430,
    -430,  -430,  -430,  -430,  -430,  -430,  -430,   290,   762,  -430,
    1238,  -430,  -430,   245,  -430,    22,  -430,  1106,  -430,  -430,
     255,   218,  -430,   114,  -430,  -430,   258,  -430,  -430,  -430,
      27,  -430,   242,  -430,  -430,   258,  -430,   259,  1310,  -430,
    -430,  -430,   110,   116,   365,   156,   326,   241,   237,   251,
     306,    51,  -430,  -430,    18,   230,  -430,  -430,  1326,   340,
     357,  1326,  -430,  1326,  1326,    21,   273,    80,  -430,   140,
      -4,  -430,  -430,  1023,  -430,  -430,   936,    11,  -430,   143,
    1133,   309,  -430,  -430,  -430,  -430,  1216,  1326,   318,  -430,
    1326,   335,  -430,  -430,  -430,  -430,  -430,  -430,  -430,  -430,
    -430,  -430,  -430,  -430,  -430,  -430,  -430,  1326,  1326,  1326,
    1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,
    1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,
    1326,  1326,  1326,  -430,  1326,  -430,  -430,  -430,  -430,  -430,
     336,  -430,  -430,   245,   291,    25,  -430,   315,  -430,  -430,
     245,  1294,   365,   123,   330,   320,   321,   322,   371,    61,
    -430,  -430,   351,   332,   258,  -430,     2,  -430,  -430,  -430,
    -430,   359,   201,  1326,   362,  -430,  -430,  -430,   117,  -430,
    1326,   363,  -430,  -430,  -430,  1326,  1326,  1326,  1326,  1326,
    1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,
    1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,
    1326,  -430,  1326,  -430,   189,   368,   192,   215,   224,  -430,
    -430,   372,    46,   416,  -430,   379,   587,  -430,  -430,   380,
    -430,  -430,  -430,  -430,  -430,  -430,    24,  -430,  -430,  -430,
    -430,  -430,   268,  -430,   145,  -430,   160,  -430,  -430,  -430,
    -430,  -430,   110,   110,   116,   116,   116,   365,   365,   365,
     365,   365,   365,   365,   156,   156,   156,   156,   326,   241,
     237,   251,   306,   384,  -430,   375,    35,   379,  -430,   245,
    -430,  1326,   -23,   431,   364,  1326,  1326,  1326,  1326,  1326,
    1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,  1326,
    1326,  1326,  1326,  1326,  -430,   388,   393,   270,  1326,   131,
    -430,   172,  -430,   396,   271,   175,  -430,  -430,  -430,  -430,
    -430,   110,   110,   116,   116,   116,   365,   365,   365,   365,
     365,   365,   365,   156,   156,   156,   156,   326,   241,   237,
     251,   306,   398,  -430,  1023,  1326,  1023,   397,  1023,   402,
    -430,  -430,  -430,   393,  -430,   675,  -430,  1243,  1326,  -430,
    -430,  -430,  1326,   142,  -430,  -430,  -430,  -430,  -430,   245,
    1326,  1326,  1326,   276,  1310,  -430,   365,   365,   365,   365,
     365,   123,   123,   123,   123,   330,   320,   321,   322,   371,
     401,  -430,   410,   381,   379,  -430,   404,   393,  -430,  -430,
     412,  -430,   379,   393,   396,  -430,  1326,   465,   278,  -430,
     480,  -430,  -430,   411,  -430,  -430,  -430,  -430,  -430,  -430,
     396,   279,  -430,   399,   285,  -430,  1023,  1326,  1326,  -430,
     415,  -430,  1326,  -430,  -430,   393,  -430,  1023,    34,  1326,
      63,   480,  -430,    46,   393,   396,  1326,  1023,  -430,  -430,
     413,   379,  -430,  -430,  -430,  -430,  -430,   293,   419,  -430,
     480,  -430,  -430,  -430,   393,   417,  -430,  1023,  -430,  1023,
    1023,   420,  -430,  1023,  -430,  1023,  1023,  -430,  -430
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -430,  -430,   108,  -430,  -430,     0,  -430,  -430,   182,  -430,
      14,  -430,    36,  -430,  -430,  -430,   -41,  -430,     4,  -430,
    -430,  -430,    12,    20,  -430,  -151,  -430,  -147,  -430,   203,
    -430,   -54,    44,  -430,  -158,   124,  -430,  -154,   122,  -430,
    -153,   127,  -430,  -150,   128,  -430,  -152,   121,  -430,  -430,
    -430,  -430,  -430,  -430,  -430,  -157,  -348,  -430,    23,    19,
    -430,  -430,   -13,   -20,  -238,  -430,  -430,  -430,  -430,  -429,
    -430,  -430,  -430,  -430,  -430,  -430,  -430,   -38,  -430,   -11,
    -430,  -430,  -430,  -430,  -430,   205,  -430,  -430,   478,  -430,
    -418,  -430,  -251,  -136,   -46,  -430,   207,   -84,     8,   -12,
    -430,    86,   292,  -430,   161,    70,  -225,  -351
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -304
static const yytype_int16 yytable[] =
{
      47,   146,   139,   319,   217,   364,   446,   394,    90,   323,
     172,    94,    54,   310,   118,    47,   109,   213,   455,   291,
     106,   503,   299,   228,   118,   155,   358,    54,   256,   134,
     328,   136,   137,   470,   471,   535,   444,   142,    47,   145,
     332,   333,   107,   118,   221,   211,   334,   335,   336,   348,
      54,   449,   151,   152,   349,   353,   350,   354,   157,   352,
     351,   162,   163,   164,   165,   255,   515,   191,   192,   188,
      97,   538,   450,   385,   259,   187,   262,   309,   222,   530,
     386,    95,   524,   189,   190,   292,   110,   212,    47,   315,
     214,   311,   292,   207,   221,   292,    90,   545,   158,   359,
      54,   231,   289,   112,   505,    96,   138,   544,   397,   359,
     111,   215,   380,   293,   244,   305,   300,   229,   401,   402,
     360,    38,   257,   509,   403,   404,   405,   417,   222,   536,
     445,   513,   418,   422,   419,   423,   245,   421,   420,   539,
     312,   265,   118,   314,   290,   118,   367,   118,   118,   344,
     345,   346,   347,    47,   381,   306,    47,   294,   141,   529,
     296,   143,   297,   298,   118,    54,   196,   197,    54,   198,
     118,   118,   368,   369,   118,   307,   223,   144,   274,   275,
     548,   276,   266,   247,   248,   249,   324,   208,   385,   326,
     250,   118,   501,   199,   200,   393,   267,   268,   269,   270,
     247,   248,   249,   384,   448,   277,   278,   479,   329,   330,
     331,   370,   371,   385,   153,   306,   118,   316,   118,   292,
     500,   309,   147,   317,   364,   440,   223,   413,   414,   415,
     416,   478,   201,   202,   292,   203,   204,   205,   206,   113,
     441,     1,    98,    99,   279,   280,   292,   148,   100,   292,
     475,   101,   481,    11,    12,   485,    14,   118,     1,    98,
      99,   209,    17,   292,   118,   100,   292,   424,   101,   118,
     426,   210,   391,    14,   166,   389,   167,   390,   168,   395,
     497,   498,   430,   263,   264,   499,   398,   399,   400,   292,
     216,    32,    33,   427,   118,   246,   118,   494,   292,   166,
     232,   170,   428,   171,   301,   302,    47,    34,    35,   102,
      37,   555,   556,   103,    90,    39,   224,    40,    54,   258,
      41,    42,    43,    44,    34,    35,   102,    37,   286,   516,
     103,   285,    39,   114,    40,   166,   166,   253,   260,   254,
     261,   511,   438,   287,   476,   476,   439,   514,   477,   484,
     292,   217,   292,   476,   506,   532,   518,   525,   288,   292,
     295,   218,   219,   527,   357,   118,   549,   292,     7,   118,
     454,   281,   282,   283,   284,   372,   373,   374,   375,   533,
     193,   194,   195,   320,   453,   454,   454,   118,   543,   325,
      22,    23,   118,    24,    25,    26,    27,    28,    29,   337,
     338,   339,   340,   341,   342,   343,   327,   355,   552,   361,
     376,   487,   377,   489,   378,   492,   461,   462,   463,   464,
     271,   272,   273,   379,    47,   382,    47,   383,    47,   118,
    -302,  -302,   388,   392,   396,    47,    54,  -302,    54,  -302,
      54,   118,   118,    90,   488,   425,   118,    54,   302,   429,
     432,   436,   443,   451,   118,   118,   454,   442,   452,  -302,
    -302,   474,  -302,  -302,  -302,  -302,  -302,  -302,   306,   482,
     504,   486,   490,   493,   507,   510,   508,   406,   407,   408,
     409,   410,   411,   412,   292,   512,   517,   519,   531,   523,
     118,   547,   550,   528,   526,   553,   557,   480,   437,   466,
     465,   469,   551,   542,   534,   467,    47,   468,   431,   150,
     541,   454,   118,   435,   546,   356,   118,    47,    54,   502,
     447,     0,     0,   118,     0,     0,     0,    47,     0,    54,
     118,     0,     0,     0,   554,     0,   155,   155,   537,    54,
     558,     0,   314,   314,     0,     0,     0,    47,     0,    47,
      47,     0,     0,    47,     0,    47,    47,     0,     0,    54,
       0,    54,    54,     0,     0,    54,     0,    54,    54,   232,
     456,   457,   458,   459,   460,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,     0,     0,     0,     0,
       1,     2,     3,     4,     0,     0,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,     0,     0,
       0,    17,     0,    18,    19,     0,    20,    21,     0,     0,
      22,    23,     0,    24,    25,    26,    27,    28,    29,    30,
      31,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      32,    33,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   232,    34,    35,    36,    37,
       0,     0,    38,   434,    39,     0,    40,     0,     0,    41,
      42,    43,    44,     0,     0,     0,     0,     0,     1,     2,
       3,     4,    45,     0,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,     0,     0,     0,    17,
       0,    18,    19,     0,    20,    21,     0,     0,    22,    23,
     232,    24,    25,    26,    27,    28,    29,    30,    31,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,    36,    37,     0,     0,
      38,   495,    39,     0,    40,     0,     0,    41,    42,    43,
      44,     0,     0,     0,     0,     1,     2,     3,     4,     0,
      45,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,     0,     0,     0,    17,     0,    18,    19,
       0,    20,    21,     0,     0,    22,    23,     0,    24,    25,
      26,    27,    28,    29,    30,    31,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    32,    33,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    34,    35,    36,    37,     0,     0,    38,     0,    39,
       0,    40,     0,     0,    41,    42,    43,    44,     0,     0,
       0,     0,     1,     2,     3,     4,     0,    45,     5,     6,
       7,     8,     0,    10,    11,    12,    13,    14,    15,    16,
       0,     0,     0,    17,     0,    18,    19,     0,    20,    21,
       0,     0,    22,    23,     0,    24,    25,    26,    27,    28,
      29,    30,    31,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    32,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,     0,     0,    38,   154,    39,     0,    40,     0,
       0,    41,    42,    43,    44,     0,     0,     0,     0,     1,
       2,     3,     4,     0,    45,     5,     6,     7,     8,     0,
      10,    11,    12,    13,    14,    15,    16,     0,     0,     0,
      17,     0,    18,    19,     0,    20,    21,     0,     0,    22,
      23,     0,    24,    25,    26,    27,    28,    29,    30,    31,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    32,
      33,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    34,    35,    36,    37,     0,
       0,    38,   313,    39,     0,    40,     0,     0,    41,    42,
      43,    44,     0,     0,     0,     0,     1,     2,     3,     4,
       0,    45,     5,     6,     7,     8,     0,    10,    11,    12,
      13,    14,    15,    16,     0,     0,     0,    17,     0,    18,
      19,     0,    20,    21,     0,     0,    22,    23,     0,    24,
      25,    26,    27,    28,    29,    30,    31,     0,     0,     0,
       0,     0,     0,   173,   174,     0,    32,    33,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,     0,
       0,     0,    34,    35,    36,    37,     0,     0,    38,     0,
      39,     0,    40,     0,     0,    41,    42,    43,    44,     1,
      98,    99,     0,     0,   186,     0,   100,   230,    45,   101,
       0,    11,    12,     0,    14,     0,     0,     0,     0,     0,
      17,     0,  -303,  -303,     0,     0,     1,    98,    99,  -303,
       0,  -303,     0,   100,     0,     0,   101,     0,    11,    12,
       0,    14,     0,     0,     0,     0,     0,    17,     0,    32,
      33,  -303,  -303,     0,  -303,  -303,  -303,  -303,  -303,  -303,
       0,     0,     0,     0,     0,    34,    35,   102,    37,     0,
       0,   103,     0,    39,     0,    40,    32,    33,    41,    42,
      43,    44,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    34,    35,   102,    37,     0,     0,   103,     0,
      39,     0,    40,   318,     0,    41,    42,    43,    44,     1,
      98,    99,     0,     0,     0,     0,   100,     0,     0,   101,
       0,    11,    12,     0,    14,     0,     0,     0,     0,     0,
      17,     0,   218,   219,     0,     0,     1,    98,    99,     7,
       0,   220,     0,   100,     0,     0,   101,     0,    11,    12,
       0,    14,     0,     0,     0,     0,     0,    17,     0,    32,
      33,    22,    23,     0,    24,    25,    26,    27,    28,    29,
       0,     0,     0,     0,     0,    34,    35,   102,    37,     0,
       0,   103,     0,    39,   321,    40,    32,    33,    41,    42,
      43,    44,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    34,    35,   102,    37,   365,     0,   103,     0,
      39,     0,    40,   496,     0,    41,    42,    43,    44,     1,
      98,    99,     0,     0,     0,     0,   100,     0,     0,   101,
       0,    11,    12,     0,    14,     0,     0,   263,   264,     0,
      17,     0,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   263,   264,     0,     0,     0,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,    32,
      33,     0,     0,     0,     0,     0,     0,     0,   186,     0,
       0,     0,     0,     0,     0,    34,    35,   102,    37,     0,
       0,   103,     0,    39,   186,    40,     0,     0,    41,    42,
      43,    44
};

static const yytype_int16 yycheck[] =
{
       0,    21,    15,   160,    88,   230,   357,   258,     0,   166,
      51,     1,     0,   149,    10,    15,     1,     1,   366,     1,
       6,   450,     1,     1,    20,    38,     1,    15,     1,    10,
     187,    11,    12,   381,   382,     1,     1,    17,    38,    20,
     191,   192,     6,    39,    90,    51,   193,   194,   195,   207,
      38,    74,    32,    33,   208,   212,   209,   214,    39,   211,
     210,    41,    42,    43,    44,   106,   484,    82,    83,    72,
      77,     8,    95,    71,   115,    52,   117,    81,    90,   508,
      78,    71,   500,    86,    87,    74,    71,    93,    88,    78,
      74,    95,    74,    90,   140,    74,    88,   526,    74,    74,
      88,    97,    51,    71,   452,    95,    77,   525,   265,    74,
      95,    95,    51,    95,   100,    35,    95,    95,   269,   270,
      95,    75,    95,   474,   271,   272,   273,   285,   140,    95,
      95,   482,   286,   290,   287,   292,   100,   289,   288,    76,
     153,   118,   138,   156,    93,   141,    23,   143,   144,   203,
     204,   205,   206,   153,    93,    75,   156,   138,    77,   507,
     141,    77,   143,   144,   160,   153,    22,    23,   156,    25,
     166,   167,    49,    50,   170,    95,    90,    77,    22,    23,
     531,    25,    72,    69,    70,    71,   167,    91,    71,   170,
      76,   187,   443,    49,    50,    78,    86,    87,    82,    83,
      69,    70,    71,   244,   361,    49,    50,    76,   188,   189,
     190,    88,    89,    71,    73,    75,   212,    74,   214,    74,
      78,    81,    71,    80,   449,    80,   140,   281,   282,   283,
     284,   388,    88,    89,    74,    45,    46,    47,    48,     1,
      80,     3,     4,     5,    88,    89,    74,    71,    10,    74,
     386,    13,    80,    15,    16,    80,    18,   253,     3,     4,
       5,    92,    24,    74,   260,    10,    74,    78,    13,   265,
      78,    52,   253,    18,    77,    74,    79,    76,    81,   260,
     437,   438,   302,    53,    54,   442,   266,   267,   268,    74,
       0,    53,    54,    78,   290,    77,   292,   433,    74,    77,
      97,    79,    78,    81,    31,    32,   306,    69,    70,    71,
      72,   549,   550,    75,   306,    77,    71,    79,   306,    77,
      82,    83,    84,    85,    69,    70,    71,    72,    91,   486,
      75,    90,    77,    95,    79,    77,    77,    79,    79,    81,
      81,   477,    74,    92,    74,    74,    78,   483,    78,    78,
      74,   435,    74,    74,    78,   512,    78,    78,    52,    74,
      20,     4,     5,    78,    73,   361,    73,    74,    11,   365,
     366,    45,    46,    47,    48,    45,    46,    47,    48,   515,
      55,    56,    57,    74,   365,   381,   382,   383,   524,    71,
      33,    34,   388,    36,    37,    38,    39,    40,    41,   196,
     197,   198,   199,   200,   201,   202,    71,    71,   544,    94,
      90,   424,    91,   426,    92,   428,   372,   373,   374,   375,
      55,    56,    57,    52,   424,    74,   426,    95,   428,   425,
       4,     5,    73,    71,    71,   435,   424,    11,   426,    13,
     428,   437,   438,   435,   425,    77,   442,   435,    32,    77,
      71,    71,    77,    22,   450,   451,   452,    73,    94,    33,
      34,    73,    36,    37,    38,    39,    40,    41,    75,    73,
     451,    73,    75,    71,    73,    71,    95,   274,   275,   276,
     277,   278,   279,   280,    74,    73,    21,     7,    73,    78,
     486,    78,    73,   506,    95,    78,    76,   389,   316,   377,
     376,   380,   540,   523,   517,   378,   506,   379,   303,    31,
     521,   507,   508,   306,   527,   223,   512,   517,   506,   449,
     359,    -1,    -1,   519,    -1,    -1,    -1,   527,    -1,   517,
     526,    -1,    -1,    -1,   547,    -1,   549,   550,   519,   527,
     553,    -1,   555,   556,    -1,    -1,    -1,   547,    -1,   549,
     550,    -1,    -1,   553,    -1,   555,   556,    -1,    -1,   547,
      -1,   549,   550,    -1,    -1,   553,    -1,   555,   556,   366,
     367,   368,   369,   370,   371,   372,   373,   374,   375,   376,
     377,   378,   379,   380,   381,   382,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    -1,
      -1,    24,    -1,    26,    27,    -1,    29,    30,    -1,    -1,
      33,    34,    -1,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   452,    69,    70,    71,    72,
      -1,    -1,    75,    76,    77,    -1,    79,    -1,    -1,    82,
      83,    84,    85,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    95,    -1,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    -1,    -1,    -1,    24,
      -1,    26,    27,    -1,    29,    30,    -1,    -1,    33,    34,
     507,    36,    37,    38,    39,    40,    41,    42,    43,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    54,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    69,    70,    71,    72,    -1,    -1,
      75,    76,    77,    -1,    79,    -1,    -1,    82,    83,    84,
      85,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
      95,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    -1,    -1,    -1,    24,    -1,    26,    27,
      -1,    29,    30,    -1,    -1,    33,    34,    -1,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    53,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    69,    70,    71,    72,    -1,    -1,    75,    -1,    77,
      -1,    79,    -1,    -1,    82,    83,    84,    85,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,    -1,    95,     9,    10,
      11,    12,    -1,    14,    15,    16,    17,    18,    19,    20,
      -1,    -1,    -1,    24,    -1,    26,    27,    -1,    29,    30,
      -1,    -1,    33,    34,    -1,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,
      71,    72,    -1,    -1,    75,    76,    77,    -1,    79,    -1,
      -1,    82,    83,    84,    85,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    95,     9,    10,    11,    12,    -1,
      14,    15,    16,    17,    18,    19,    20,    -1,    -1,    -1,
      24,    -1,    26,    27,    -1,    29,    30,    -1,    -1,    33,
      34,    -1,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    -1,
      -1,    75,    76,    77,    -1,    79,    -1,    -1,    82,    83,
      84,    85,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,    95,     9,    10,    11,    12,    -1,    14,    15,    16,
      17,    18,    19,    20,    -1,    -1,    -1,    24,    -1,    26,
      27,    -1,    29,    30,    -1,    -1,    33,    34,    -1,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    54,    -1,    53,    54,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    -1,
      -1,    -1,    69,    70,    71,    72,    -1,    -1,    75,    -1,
      77,    -1,    79,    -1,    -1,    82,    83,    84,    85,     3,
       4,     5,    -1,    -1,    94,    -1,    10,    11,    95,    13,
      -1,    15,    16,    -1,    18,    -1,    -1,    -1,    -1,    -1,
      24,    -1,     4,     5,    -1,    -1,     3,     4,     5,    11,
      -1,    13,    -1,    10,    -1,    -1,    13,    -1,    15,    16,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    24,    -1,    53,
      54,    33,    34,    -1,    36,    37,    38,    39,    40,    41,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    -1,
      -1,    75,    -1,    77,    -1,    79,    53,    54,    82,    83,
      84,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    70,    71,    72,    -1,    -1,    75,    -1,
      77,    -1,    79,    80,    -1,    82,    83,    84,    85,     3,
       4,     5,    -1,    -1,    -1,    -1,    10,    -1,    -1,    13,
      -1,    15,    16,    -1,    18,    -1,    -1,    -1,    -1,    -1,
      24,    -1,     4,     5,    -1,    -1,     3,     4,     5,    11,
      -1,    13,    -1,    10,    -1,    -1,    13,    -1,    15,    16,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    24,    -1,    53,
      54,    33,    34,    -1,    36,    37,    38,    39,    40,    41,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    -1,
      -1,    75,    -1,    77,    78,    79,    53,    54,    82,    83,
      84,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    70,    71,    72,    22,    -1,    75,    -1,
      77,    -1,    79,    80,    -1,    82,    83,    84,    85,     3,
       4,     5,    -1,    -1,    -1,    -1,    10,    -1,    -1,    13,
      -1,    15,    16,    -1,    18,    -1,    -1,    53,    54,    -1,
      24,    -1,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    53,    54,    -1,    -1,    -1,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    53,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    -1,
      -1,    75,    -1,    77,    94,    79,    -1,    -1,    82,    83,
      84,    85
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
      93,     1,    74,    95,   155,    20,   155,   155,   155,     1,
      95,    31,    32,   180,   181,    35,    75,    95,   189,    81,
     189,    95,   158,    76,   158,    78,    74,    80,    80,   151,
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
     159,   181,    71,   203,    76,   192,    71,   104,    74,    78,
      80,    80,    73,    77,     1,    95,   203,   200,   151,    74,
      95,    22,    94,   155,   114,   152,   125,   125,   125,   125,
     125,   128,   128,   128,   128,   131,   134,   137,   140,   143,
     152,   152,   155,   165,    73,   189,    74,    78,   151,    76,
      98,    80,    73,   186,    78,    80,    73,   158,   155,   158,
      75,   172,   158,    71,   189,    76,    80,   151,   151,   151,
      78,   188,   201,   165,   155,   152,    78,    73,    95,   203,
      71,   189,    73,   203,   189,   186,   151,    21,    78,     7,
     173,   174,   175,    78,   186,    78,    95,    78,   158,   152,
     165,    73,   151,   189,   158,     1,    95,   155,     8,    76,
     176,   175,   159,   189,   186,   165,   158,    78,   203,    73,
      73,   173,   189,    78,   158,   160,   160,    76,   158
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
      yyerror (&yylloc, lx, YY_("syntax error: cannot back up")); \
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
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, lx)
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
		  Type, Value, Location, lx); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, QSLexer *lx)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, lx)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    QSLexer *lx;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (lx);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, QSLexer *lx)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, lx)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    QSLexer *lx;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, lx);
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, QSLexer *lx)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, lx)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    QSLexer *lx;
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
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , lx);
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, lx); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, QSLexer *lx)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, lx)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    QSLexer *lx;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (lx);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 70: /* "STRING" */
#line 118 "grammar.y"
	{ DEREF_USTR((yyvaluep->ustr)); };
#line 1994 "grammar.tab.c"
	break;
      case 71: /* "IDENT" */
#line 118 "grammar.y"
	{ DEREF_USTR((yyvaluep->ustr)); };
#line 1999 "grammar.tab.c"
	break;

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
int yyparse (QSLexer *lx);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






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
yyparse (QSLexer *lx)
#else
int
yyparse (lx)
    QSLexer *lx;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;

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

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

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
  yylsp = yyls;
#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
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
#line 185 "grammar.y"
    { (yyval.node) = new QSNullNode(); ;}
    break;

  case 3:
#line 186 "grammar.y"
    { (yyval.node) = new QSBooleanNode(true); ;}
    break;

  case 4:
#line 187 "grammar.y"
    { (yyval.node) = new QSBooleanNode(false); ;}
    break;

  case 5:
#line 188 "grammar.y"
    { (yyval.node) = new QSNumberNode((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 6:
#line 189 "grammar.y"
    { (yyval.node) = new QSStringNode((yyvsp[(1) - (1)].ustr)); ;}
    break;

  case 7:
#line 190 "grammar.y"
    { if (!lx->scanRegExp()) YYABORT; (yyval.node) = new QSRegExpNode(lx->pattern, lx->flags); ;}
    break;

  case 8:
#line 194 "grammar.y"
    { (yyval.node) = new QSPropertyNode((yyvsp[(1) - (1)].ustr)); ;}
    break;

  case 9:
#line 195 "grammar.y"
    { (yyval.node) = new QSPropertyNode((yyvsp[(1) - (1)].ustr)); ;}
    break;

  case 10:
#line 196 "grammar.y"
    { (yyval.node) = new QSPropertyNode((yyvsp[(1) - (1)].dval)); ;}
    break;

  case 11:
#line 200 "grammar.y"
    { (yyval.node) = new QSPropertyValueNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 12:
#line 202 "grammar.y"
    { (yyval.node) = new QSPropertyValueNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node), (yyvsp[(1) - (5)].node)); ;}
    break;

  case 14:
#line 207 "grammar.y"
    { (yyval.node) = new QSObjectLiteralNode(0L); ;}
    break;

  case 15:
#line 208 "grammar.y"
    { (yyval.node) = new QSObjectLiteralNode((yyvsp[(2) - (3)].node)); ;}
    break;

  case 16:
#line 210 "grammar.y"
    { (yyval.node) = new QSObjectLiteralNode((yyvsp[(2) - (4)].node)); ;}
    break;

  case 17:
#line 214 "grammar.y"
    { (yyval.node) = new QSThisNode(); ;}
    break;

  case 20:
#line 217 "grammar.y"
    { (yyval.node) = new QSResolveNode((yyvsp[(1) - (1)].ustr)); ;}
    break;

  case 21:
#line 218 "grammar.y"
    { (yyval.node) = new QSGroupNode((yyvsp[(2) - (3)].node)); ;}
    break;

  case 22:
#line 222 "grammar.y"
    { (yyval.node) = new QSArrayNode((yyvsp[(2) - (3)].eli)); ;}
    break;

  case 23:
#line 223 "grammar.y"
    { (yyval.node) = new QSArrayNode((yyvsp[(2) - (3)].elm)); ;}
    break;

  case 24:
#line 224 "grammar.y"
    { (yyval.node) = new QSArrayNode((yyvsp[(4) - (5)].eli), (yyvsp[(2) - (5)].elm)); ;}
    break;

  case 25:
#line 228 "grammar.y"
    { (yyval.elm) = new QSElementNode((yyvsp[(1) - (2)].eli), (yyvsp[(2) - (2)].node)); ;}
    break;

  case 26:
#line 230 "grammar.y"
    { (yyval.elm) = new QSElementNode((yyvsp[(1) - (4)].elm), (yyvsp[(3) - (4)].eli), (yyvsp[(4) - (4)].node)); ;}
    break;

  case 27:
#line 234 "grammar.y"
    { (yyval.eli) = 0L; ;}
    break;

  case 29:
#line 239 "grammar.y"
    { (yyval.eli) = new QSElisionNode(0L); ;}
    break;

  case 30:
#line 240 "grammar.y"
    { (yyval.eli) = new QSElisionNode((yyvsp[(1) - (2)].eli)); ;}
    break;

  case 32:
#line 245 "grammar.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); ;}
    break;

  case 33:
#line 246 "grammar.y"
    { (yyval.node) = new QSAccessorNode1((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); ;}
    break;

  case 34:
#line 247 "grammar.y"
    { (yyval.node) = new QSAccessorNode2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].ustr)); ;}
    break;

  case 35:
#line 248 "grammar.y"
    { (yyval.node) = new QSNewExprNode((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].args)); static_cast<QSNewExprNode *>((yyval.node))->setLineNo(lx->lineNo()); ;}
    break;

  case 37:
#line 253 "grammar.y"
    { (yyval.node) = new QSAccessorNode1((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); ;}
    break;

  case 38:
#line 254 "grammar.y"
    { (yyval.node) = new QSAccessorNode2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].ustr)); ;}
    break;

  case 39:
#line 255 "grammar.y"
    { (yyval.node) = new QSNewExprNode((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].args)); static_cast<QSNewExprNode *>((yyval.node))->setLineNo(lx->lineNo()); ;}
    break;

  case 41:
#line 260 "grammar.y"
    { (yyval.node) = new QSNewExprNode((yyvsp[(2) - (2)].node)); static_cast<QSNewExprNode *>((yyval.node))->setLineNo(lx->lineNo()); ;}
    break;

  case 43:
#line 265 "grammar.y"
    { (yyval.node) = new QSNewExprNode((yyvsp[(2) - (2)].node)); static_cast<QSNewExprNode *>((yyval.node))->setLineNo(lx->lineNo()); ;}
    break;

  case 44:
#line 269 "grammar.y"
    { (yyval.node) = new QSFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); DBG(static_cast<QSFunctionCallNode *>((yyval.node)), (yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 45:
#line 270 "grammar.y"
    { (yyval.node) = new QSFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); DBG(static_cast<QSFunctionCallNode *>((yyval.node)), (yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 46:
#line 271 "grammar.y"
    { (yyval.node) = new QSAccessorNode1((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); ;}
    break;

  case 47:
#line 272 "grammar.y"
    { (yyval.node) = new QSAccessorNode2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].ustr)); ;}
    break;

  case 48:
#line 276 "grammar.y"
    { (yyval.node) = new QSFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); DBG(static_cast<QSFunctionCallNode *>((yyval.node)), (yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 49:
#line 277 "grammar.y"
    { (yyval.node) = new QSFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); DBG(static_cast<QSFunctionCallNode *>((yyval.node)), (yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 50:
#line 278 "grammar.y"
    { (yyval.node) = new QSAccessorNode1((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); ;}
    break;

  case 51:
#line 279 "grammar.y"
    { (yyval.node) = new QSAccessorNode2((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].ustr)); ;}
    break;

  case 52:
#line 283 "grammar.y"
    { (yyval.args) = new QSArgumentsNode(0L); ;}
    break;

  case 53:
#line 284 "grammar.y"
    { (yyval.args) = new QSArgumentsNode((yyvsp[(2) - (3)].alist)); ;}
    break;

  case 54:
#line 288 "grammar.y"
    { (yyval.alist) = new QSArgumentListNode((yyvsp[(1) - (1)].node)); ;}
    break;

  case 55:
#line 289 "grammar.y"
    { (yyval.alist) = new QSArgumentListNode((yyvsp[(1) - (3)].alist), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 61:
#line 304 "grammar.y"
    { (yyval.node) = new QSPostfixNode((yyvsp[(1) - (2)].node), OpPlusPlus); ;}
    break;

  case 62:
#line 305 "grammar.y"
    { (yyval.node) = new QSPostfixNode((yyvsp[(1) - (2)].node), OpMinusMinus); ;}
    break;

  case 64:
#line 310 "grammar.y"
    { (yyval.node) = new QSPostfixNode((yyvsp[(1) - (2)].node), OpPlusPlus); ;}
    break;

  case 65:
#line 311 "grammar.y"
    { (yyval.node) = new QSPostfixNode((yyvsp[(1) - (2)].node), OpMinusMinus); ;}
    break;

  case 66:
#line 315 "grammar.y"
    { (yyval.node) = new QSDeleteNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 67:
#line 316 "grammar.y"
    { (yyval.node) = new QSVoidNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 68:
#line 317 "grammar.y"
    { (yyval.node) = new QSTypeOfNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 69:
#line 318 "grammar.y"
    { (yyval.node) = new QSPrefixNode(OpPlusPlus, (yyvsp[(2) - (2)].node)); ;}
    break;

  case 70:
#line 319 "grammar.y"
    { (yyval.node) = new QSPrefixNode(OpMinusMinus, (yyvsp[(2) - (2)].node)); ;}
    break;

  case 71:
#line 320 "grammar.y"
    { (yyval.node) = new QSUnaryPlusNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 72:
#line 321 "grammar.y"
    { (yyval.node) = new QSNegateNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 73:
#line 322 "grammar.y"
    { (yyval.node) = new QSBitwiseNotNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 74:
#line 323 "grammar.y"
    { (yyval.node) = new QSLogicalNotNode((yyvsp[(2) - (2)].node)); ;}
    break;

  case 80:
#line 338 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '*'); ;}
    break;

  case 81:
#line 339 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '/'); ;}
    break;

  case 82:
#line 340 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),'%'); ;}
    break;

  case 84:
#line 346 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '*'); ;}
    break;

  case 85:
#line 348 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '/'); ;}
    break;

  case 86:
#line 350 "grammar.y"
    { (yyval.node) = new QSMultNode((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),'%'); ;}
    break;

  case 88:
#line 355 "grammar.y"
    { (yyval.node) = new QSAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '+'); ;}
    break;

  case 89:
#line 356 "grammar.y"
    { (yyval.node) = new QSAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '-'); ;}
    break;

  case 91:
#line 362 "grammar.y"
    { (yyval.node) = new QSAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '+'); ;}
    break;

  case 92:
#line 364 "grammar.y"
    { (yyval.node) = new QSAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), '-'); ;}
    break;

  case 94:
#line 369 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpLShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 95:
#line 370 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpRShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 96:
#line 371 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpURShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 98:
#line 376 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpLShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 99:
#line 377 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpRShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 100:
#line 378 "grammar.y"
    { (yyval.node) = new QSShiftNode((yyvsp[(1) - (3)].node), OpURShift, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 102:
#line 383 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 103:
#line 384 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 104:
#line 385 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 105:
#line 386 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 106:
#line 387 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpIs, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 107:
#line 388 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 108:
#line 389 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpIn, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 110:
#line 394 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 111:
#line 395 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 112:
#line 396 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 113:
#line 397 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 114:
#line 399 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 116:
#line 404 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 117:
#line 405 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 118:
#line 406 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 119:
#line 407 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 120:
#line 408 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpIs, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 121:
#line 410 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 122:
#line 411 "grammar.y"
    { (yyval.node) = new QSRelationalNode((yyvsp[(1) - (3)].node), OpIn, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 124:
#line 416 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 125:
#line 417 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 126:
#line 418 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 127:
#line 419 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 129:
#line 425 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 130:
#line 427 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 131:
#line 429 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 132:
#line 431 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 134:
#line 437 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 135:
#line 439 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 136:
#line 441 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 137:
#line 443 "grammar.y"
    { (yyval.node) = new QSEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 139:
#line 448 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 141:
#line 454 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 143:
#line 459 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 145:
#line 464 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 147:
#line 470 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 149:
#line 476 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 151:
#line 481 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 153:
#line 487 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 155:
#line 493 "grammar.y"
    { (yyval.node) = new QSBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 157:
#line 498 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 159:
#line 504 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 161:
#line 510 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 163:
#line 515 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 165:
#line 521 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 167:
#line 527 "grammar.y"
    { (yyval.node) = new QSBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 169:
#line 533 "grammar.y"
    { (yyval.node) = new QSConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); ;}
    break;

  case 171:
#line 539 "grammar.y"
    { (yyval.node) = new QSConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); ;}
    break;

  case 173:
#line 545 "grammar.y"
    { (yyval.node) = new QSConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); ;}
    break;

  case 175:
#line 551 "grammar.y"
    { (yyval.node) = new QSAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 177:
#line 557 "grammar.y"
    { (yyval.node) = new QSAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 179:
#line 563 "grammar.y"
    { (yyval.node) = new QSAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 180:
#line 567 "grammar.y"
    { (yyval.op) = OpEqual; ;}
    break;

  case 181:
#line 568 "grammar.y"
    { (yyval.op) = OpPlusEq; ;}
    break;

  case 182:
#line 569 "grammar.y"
    { (yyval.op) = OpMinusEq; ;}
    break;

  case 183:
#line 570 "grammar.y"
    { (yyval.op) = OpMultEq; ;}
    break;

  case 184:
#line 571 "grammar.y"
    { (yyval.op) = OpDivEq; ;}
    break;

  case 185:
#line 572 "grammar.y"
    { (yyval.op) = OpLShift; ;}
    break;

  case 186:
#line 573 "grammar.y"
    { (yyval.op) = OpRShift; ;}
    break;

  case 187:
#line 574 "grammar.y"
    { (yyval.op) = OpURShift; ;}
    break;

  case 188:
#line 575 "grammar.y"
    { (yyval.op) = OpAndEq; ;}
    break;

  case 189:
#line 576 "grammar.y"
    { (yyval.op) = OpXOrEq; ;}
    break;

  case 190:
#line 577 "grammar.y"
    { (yyval.op) = OpOrEq; ;}
    break;

  case 191:
#line 578 "grammar.y"
    { (yyval.op) = OpModEq; ;}
    break;

  case 193:
#line 583 "grammar.y"
    { (yyval.node) = new QSCommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 195:
#line 588 "grammar.y"
    { (yyval.node) = new QSCommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 197:
#line 593 "grammar.y"
    { (yyval.node) = new QSCommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 200:
#line 599 "grammar.y"
    { (yyval.stat) = (yyvsp[(1) - (1)].clss); ;}
    break;

  case 201:
#line 600 "grammar.y"
    { static_cast<QSClassDefNode *>((yyvsp[(2) - (2)].clss))->setAttributes( (yyvsp[(1) - (2)].attrs) ); (yyval.stat) = (yyvsp[(2) - (2)].clss); ;}
    break;

  case 216:
#line 618 "grammar.y"
    { (yyval.stat) = new QSBlockNode(0L); DBG((yyval.stat), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 217:
#line 619 "grammar.y"
    { (yyval.stat) = new QSBlockNode((yyvsp[(2) - (3)].slist)); DBG((yyval.stat), (yylsp[(3) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 218:
#line 623 "grammar.y"
    { (yyval.slist) = new QSStatListNode((yyvsp[(1) - (1)].stat)); ;}
    break;

  case 219:
#line 624 "grammar.y"
    { (yyval.slist) = new QSStatListNode((yyvsp[(1) - (2)].slist), (yyvsp[(2) - (2)].stat)); ;}
    break;

  case 220:
#line 628 "grammar.y"
    { (yyval.stat) = new QSEmptyStatementNode(); ;}
    break;

  case 221:
#line 632 "grammar.y"
    { (yyval.stat) = new QSExprStatementNode((yyvsp[(1) - (2)].node)); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 222:
#line 633 "grammar.y"
    { (yyval.stat) = new QSExprStatementNode((yyvsp[(1) - (2)].node)); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); AUTO_SEMICOLON; ;}
    break;

  case 223:
#line 638 "grammar.y"
    { (yyval.stat) = new QSIfNode((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].stat),0L); DBG((yyval.stat),(yylsp[(1) - (5)]),(yylsp[(4) - (5)])); ;}
    break;

  case 224:
#line 640 "grammar.y"
    { (yyval.stat) = new QSIfNode((yyvsp[(3) - (7)].node),(yyvsp[(5) - (7)].stat),(yyvsp[(7) - (7)].stat)); DBG((yyval.stat),(yylsp[(1) - (7)]),(yylsp[(4) - (7)])); ;}
    break;

  case 225:
#line 644 "grammar.y"
    { (yyval.stat) = new QSDoWhileNode((yyvsp[(2) - (7)].stat),(yyvsp[(5) - (7)].node)); DBG((yyval.stat),(yylsp[(1) - (7)]),(yylsp[(3) - (7)])); ;}
    break;

  case 226:
#line 646 "grammar.y"
    { (yyval.stat) = new QSDoWhileNode((yyvsp[(2) - (7)].stat),(yyvsp[(5) - (7)].node)); DBG((yyval.stat),(yylsp[(1) - (7)]),(yylsp[(3) - (7)])); ;}
    break;

  case 227:
#line 647 "grammar.y"
    { (yyval.stat) = new QSWhileNode((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].stat)); DBG((yyval.stat),(yylsp[(1) - (5)]),(yylsp[(4) - (5)])); ;}
    break;

  case 228:
#line 649 "grammar.y"
    { (yyval.stat) = new QSForNode((yyvsp[(3) - (9)].node),(yyvsp[(5) - (9)].node),(yyvsp[(7) - (9)].node),(yyvsp[(9) - (9)].stat)); DBG((yyval.stat),(yylsp[(1) - (9)]),(yylsp[(8) - (9)])); ;}
    break;

  case 229:
#line 651 "grammar.y"
    { (yyval.stat) = new QSForNode((yyvsp[(4) - (10)].blist),(yyvsp[(6) - (10)].node),(yyvsp[(8) - (10)].node),(yyvsp[(10) - (10)].stat)); DBG((yyval.stat),(yylsp[(1) - (10)]),(yylsp[(9) - (10)])); ;}
    break;

  case 230:
#line 653 "grammar.y"
    { (yyval.stat) = new QSForInNode((yyvsp[(3) - (7)].node), (yyvsp[(5) - (7)].node), (yyvsp[(7) - (7)].stat)); DBG((yyval.stat),(yylsp[(1) - (7)]),(yylsp[(6) - (7)])); ;}
    break;

  case 231:
#line 655 "grammar.y"
    { (yyval.stat) = new QSForInNode((yyvsp[(4) - (8)].vbin),(yyvsp[(6) - (8)].node),(yyvsp[(8) - (8)].stat)); DBG((yyval.stat),(yylsp[(1) - (8)]),(yylsp[(7) - (8)])); ;}
    break;

  case 232:
#line 659 "grammar.y"
    { (yyval.node) = 0L; ;}
    break;

  case 234:
#line 664 "grammar.y"
    { (yyval.node) = 0L; ;}
    break;

  case 236:
#line 669 "grammar.y"
    { (yyval.stat) = new QSContinueNode(); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 237:
#line 670 "grammar.y"
    { (yyval.stat) = new QSContinueNode(); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)])); AUTO_SEMICOLON; ;}
    break;

  case 238:
#line 671 "grammar.y"
    { (yyval.stat) = new QSContinueNode((yyvsp[(2) - (3)].ustr)); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(3) - (3)])); ;}
    break;

  case 239:
#line 672 "grammar.y"
    { (yyval.stat) = new QSContinueNode((yyvsp[(2) - (3)].ustr)); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)])); AUTO_SEMICOLON; ;}
    break;

  case 240:
#line 676 "grammar.y"
    { (yyval.stat) = new QSBreakNode();DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 241:
#line 677 "grammar.y"
    { (yyval.stat) = new QSBreakNode(); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(1) - (2)])); AUTO_SEMICOLON; ;}
    break;

  case 242:
#line 678 "grammar.y"
    { (yyval.stat) = new QSBreakNode((yyvsp[(2) - (3)].ustr)); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(3) - (3)])); ;}
    break;

  case 243:
#line 679 "grammar.y"
    { (yyval.stat) = new QSBreakNode((yyvsp[(2) - (3)].ustr)); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)])); AUTO_SEMICOLON; ;}
    break;

  case 244:
#line 683 "grammar.y"
    { (yyval.stat) = new QSReturnNode(0L); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(2) - (2)])); ;}
    break;

  case 245:
#line 684 "grammar.y"
    { (yyval.stat) = new QSReturnNode(0L); DBG((yyval.stat),(yylsp[(1) - (2)]),(yylsp[(1) - (2)])); AUTO_SEMICOLON; ;}
    break;

  case 246:
#line 685 "grammar.y"
    { (yyval.stat) = new QSReturnNode((yyvsp[(2) - (3)].node)); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)])); ;}
    break;

  case 247:
#line 686 "grammar.y"
    { (yyval.stat) = new QSReturnNode((yyvsp[(2) - (3)].node)); DBG((yyval.stat),(yylsp[(1) - (3)]),(yylsp[(2) - (3)])); AUTO_SEMICOLON; ;}
    break;

  case 248:
#line 690 "grammar.y"
    { (yyval.stat) = new QSWithNode((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].stat)); DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); ;}
    break;

  case 249:
#line 694 "grammar.y"
    { (yyval.stat) = new QSSwitchNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].cblk)); DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); ;}
    break;

  case 250:
#line 698 "grammar.y"
    { (yyval.cblk) = new QSCaseBlockNode((yyvsp[(2) - (3)].clist), 0L, 0L); ;}
    break;

  case 251:
#line 700 "grammar.y"
    { (yyval.cblk) = new QSCaseBlockNode((yyvsp[(2) - (5)].clist), (yyvsp[(3) - (5)].ccl), (yyvsp[(4) - (5)].clist)); ;}
    break;

  case 252:
#line 704 "grammar.y"
    { (yyval.clist) = 0L; ;}
    break;

  case 254:
#line 709 "grammar.y"
    { (yyval.clist) = new QSClauseListNode((yyvsp[(1) - (1)].ccl)); ;}
    break;

  case 255:
#line 710 "grammar.y"
    { (yyval.clist) = static_cast<QSClauseListNode *>((yyvsp[(1) - (2)].clist))->append((yyvsp[(2) - (2)].ccl)); ;}
    break;

  case 256:
#line 714 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode((yyvsp[(2) - (3)].node), 0L); ;}
    break;

  case 257:
#line 715 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].slist)); ;}
    break;

  case 258:
#line 719 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode(0L, 0L); ;}
    break;

  case 259:
#line 720 "grammar.y"
    { (yyval.ccl) = new QSCaseClauseNode(0L, (yyvsp[(3) - (3)].slist)); ;}
    break;

  case 260:
#line 724 "grammar.y"
    { static_cast<QSStatementNode *>((yyvsp[(3) - (3)].stat))->pushLabel((yyvsp[(1) - (3)].ustr)); (yyval.stat) = new QSLabelNode((yyvsp[(1) - (3)].ustr), (yyvsp[(3) - (3)].stat)); ;}
    break;

  case 261:
#line 728 "grammar.y"
    { (yyval.stat) = new QSThrowNode((yyvsp[(2) - (3)].node)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 262:
#line 729 "grammar.y"
    { (yyval.stat) = new QSThrowNode((yyvsp[(2) - (3)].node)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; ;}
    break;

  case 263:
#line 733 "grammar.y"
    { (yyval.stat) = new QSTryNode((yyvsp[(2) - (3)].stat), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 264:
#line 734 "grammar.y"
    { (yyval.stat) = new QSTryNode((yyvsp[(2) - (3)].stat), 0L, (yyvsp[(3) - (3)].node)); ;}
    break;

  case 265:
#line 735 "grammar.y"
    { (yyval.stat) = new QSTryNode((yyvsp[(2) - (4)].stat), (yyvsp[(3) - (4)].node), (yyvsp[(4) - (4)].node)); ;}
    break;

  case 266:
#line 739 "grammar.y"
    { (yyval.node) = new QSCatchNode((yyvsp[(3) - (5)].ustr), (yyvsp[(5) - (5)].stat)); ;}
    break;

  case 267:
#line 743 "grammar.y"
    { (yyval.node) = new QSFinallyNode((yyvsp[(2) - (2)].stat)); ;}
    break;

  case 268:
#line 747 "grammar.y"
    { (yyval.stat) = new QSPackageNode((yyvsp[(2) - (3)].ustr), (yyvsp[(3) - (3)].body)); ;}
    break;

  case 269:
#line 751 "grammar.y"
    { (yyval.stat) = new QSImportNode((yyvsp[(2) - (3)].ustr)); ;}
    break;

  case 270:
#line 755 "grammar.y"
    { (yyval.ustr) = (yyvsp[(1) - (1)].ustr); ;}
    break;

  case 271:
#line 756 "grammar.y"
    { (yyval.ustr) = NEW_USTR( USTR((yyvsp[(1) - (3)].ustr)) + QLatin1String(".") + USTR((yyvsp[(3) - (3)].ustr)) ); ;}
    break;

  case 272:
#line 761 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode((yyvsp[(2) - (6)].ustr), 0L, (yyvsp[(5) - (6)].type), (yyvsp[(6) - (6)].body)); ;}
    break;

  case 273:
#line 763 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode((yyvsp[(2) - (7)].ustr), (yyvsp[(4) - (7)].param), (yyvsp[(6) - (7)].type), (yyvsp[(7) - (7)].body)); ;}
    break;

  case 274:
#line 765 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode((yyvsp[(3) - (7)].ustr), 0L, (yyvsp[(6) - (7)].type), (yyvsp[(7) - (7)].body));
			  							  static_cast<QSFuncDeclNode *>((yyval.func))->setAttributes( (yyvsp[(1) - (7)].attrs) ); ;}
    break;

  case 275:
#line 768 "grammar.y"
    { (yyval.func) = new QSFuncDeclNode((yyvsp[(3) - (8)].ustr), (yyvsp[(5) - (8)].param), (yyvsp[(7) - (8)].type), (yyvsp[(8) - (8)].body));
                          				  static_cast<QSFuncDeclNode *>((yyval.func))->setAttributes( (yyvsp[(1) - (8)].attrs) ); ;}
    break;

  case 276:
#line 773 "grammar.y"
    { (yyval.type) = 0L; ;}
    break;

  case 277:
#line 774 "grammar.y"
    { (yyval.type) = (yyvsp[(2) - (2)].type); ;}
    break;

  case 278:
#line 778 "grammar.y"
    { (yyval.node) = new QSFuncExprNode(0L, (yyvsp[(4) - (4)].body)); ;}
    break;

  case 279:
#line 780 "grammar.y"
    { (yyval.node) = new QSFuncExprNode((yyvsp[(3) - (5)].param), (yyvsp[(5) - (5)].body)); ;}
    break;

  case 280:
#line 784 "grammar.y"
    { (yyval.param) = new QSParameterNode((yyvsp[(1) - (1)].ustr), 0L); ;}
    break;

  case 281:
#line 785 "grammar.y"
    { (yyval.param) = new QSParameterNode((yyvsp[(1) - (3)].ustr), (yyvsp[(3) - (3)].type)); ;}
    break;

  case 282:
#line 786 "grammar.y"
    { (yyval.param) = static_cast<QSParameterNode *>((yyvsp[(1) - (3)].param))->append((yyvsp[(3) - (3)].ustr), 0L); ;}
    break;

  case 283:
#line 788 "grammar.y"
    { (yyval.param) = static_cast<QSParameterNode *>((yyvsp[(1) - (5)].param))->append((yyvsp[(3) - (5)].ustr), (yyvsp[(5) - (5)].type)); ;}
    break;

  case 284:
#line 792 "grammar.y"
    { (yyval.body) = new QSFunctionBodyNode(0L, lx); ;}
    break;

  case 285:
#line 793 "grammar.y"
    { (yyval.body) = new QSFunctionBodyNode((yyvsp[(2) - (3)].srcs), lx); ;}
    break;

  case 286:
#line 797 "grammar.y"
    { (yyval.clss) = new QSClassDefNode((yyvsp[(2) - (3)].ustr), 0L, 0L); ;}
    break;

  case 287:
#line 798 "grammar.y"
    { (yyval.clss) = new QSClassDefNode((yyvsp[(2) - (3)].ustr), 0L, (yyvsp[(3) - (3)].body)); ;}
    break;

  case 288:
#line 800 "grammar.y"
    { (yyval.clss) = new QSClassDefNode((yyvsp[(2) - (5)].ustr), (yyvsp[(4) - (5)].type), (yyvsp[(5) - (5)].body)); ;}
    break;

  case 289:
#line 804 "grammar.y"
    { (yyval.prog) = new QSProgramNode(0L, lx); ;}
    break;

  case 290:
#line 805 "grammar.y"
    { (yyval.prog) = new QSProgramNode((yyvsp[(1) - (1)].srcs), lx); ;}
    break;

  case 291:
#line 809 "grammar.y"
    { (yyval.srcs) = new QSSourceElementsNode((yyvsp[(1) - (1)].src)); ;}
    break;

  case 292:
#line 810 "grammar.y"
    { (yyval.srcs) = new QSSourceElementsNode((yyvsp[(1) - (2)].srcs), (yyvsp[(2) - (2)].src)); ;}
    break;

  case 293:
#line 814 "grammar.y"
    { (yyval.src) = new QSSourceElementNode((yyvsp[(1) - (1)].stat)); ;}
    break;

  case 294:
#line 815 "grammar.y"
    { (yyval.src) = new QSSourceElementNode((yyvsp[(1) - (1)].func)); ;}
    break;

  case 295:
#line 819 "grammar.y"
    { (yyval.attrs) = new QSAttributeNode((yyvsp[(1) - (1)].attr)); ;}
    break;

  case 296:
#line 820 "grammar.y"
    { static_cast<QSAttributeNode *>((yyvsp[(1) - (2)].attrs))->add((yyvsp[(2) - (2)].attr)); (yyval.attrs) = (yyvsp[(1) - (2)].attrs); ;}
    break;

  case 297:
#line 824 "grammar.y"
    { (yyval.attr) = AttributeAbstract; ;}
    break;

  case 298:
#line 825 "grammar.y"
    { (yyval.attr) = AttributeFinal; ;}
    break;

  case 299:
#line 826 "grammar.y"
    { (yyval.attr) = AttributePrivate; ;}
    break;

  case 300:
#line 827 "grammar.y"
    { (yyval.attr) = AttributePublic; ;}
    break;

  case 301:
#line 828 "grammar.y"
    { (yyval.attr) = AttributeStatic; ;}
    break;

  case 302:
#line 829 "grammar.y"
    { (yyval.attr) = AttributeTrue; ;}
    break;

  case 303:
#line 830 "grammar.y"
    { (yyval.attr) = AttributeFalse; ;}
    break;

  case 304:
#line 831 "grammar.y"
    { (yyval.attr) = AttributeConstructor; ;}
    break;

  case 305:
#line 838 "grammar.y"
    { (yyval.stat) = new QSVarDefNode((yyvsp[(1) - (3)].ival), (yyvsp[(2) - (3)].blist)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 306:
#line 840 "grammar.y"
    { (yyval.stat) = new QSVarDefNode((yyvsp[(1) - (3)].ival), (yyvsp[(2) - (3)].blist)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)]));  AUTO_SEMICOLON; ;}
    break;

  case 307:
#line 842 "grammar.y"
    { (yyval.stat) = new QSVarDefNode((yyvsp[(2) - (4)].ival), (yyvsp[(3) - (4)].blist)); DBG((yyval.stat), (yylsp[(1) - (4)]), (yylsp[(4) - (4)])); 
                             			  static_cast<QSVarDefNode *>((yyval.stat))->setAttributes((yyvsp[(1) - (4)].attrs)); ;}
    break;

  case 308:
#line 845 "grammar.y"
    { (yyval.stat) = new QSVarDefNode((yyvsp[(2) - (4)].ival), (yyvsp[(3) - (4)].blist)); DBG((yyval.stat), (yylsp[(1) - (4)]), (yylsp[(3) - (4)])); 
                             			  static_cast<QSVarDefNode *>((yyval.stat))->setAttributes((yyvsp[(1) - (4)].attrs)); AUTO_SEMICOLON; ;}
    break;

  case 309:
#line 850 "grammar.y"
    { (yyval.ival) = 0; ;}
    break;

  case 310:
#line 851 "grammar.y"
    { (yyval.ival) = 1; ;}
    break;

  case 311:
#line 855 "grammar.y"
    { (yyval.blist) = new QSVarBindingListNode(0L, (yyvsp[(1) - (1)].vbin)); ;}
    break;

  case 312:
#line 857 "grammar.y"
    { (yyval.blist) = new QSVarBindingListNode((yyvsp[(1) - (3)].blist), (yyvsp[(3) - (3)].vbin)); ;}
    break;

  case 313:
#line 861 "grammar.y"
    { (yyval.blist) = new QSVarBindingListNode(0L, (yyvsp[(1) - (1)].vbin)); ;}
    break;

  case 314:
#line 863 "grammar.y"
    { (yyval.blist) = new QSVarBindingListNode((yyvsp[(1) - (3)].blist), (yyvsp[(3) - (3)].vbin)); ;}
    break;

  case 315:
#line 867 "grammar.y"
    { (yyval.vbin) = new QSVarBindingNode((yyvsp[(1) - (1)].tvar), 0L); ;}
    break;

  case 316:
#line 868 "grammar.y"
    { (yyval.vbin) = new QSVarBindingNode((yyvsp[(1) - (3)].tvar), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 317:
#line 872 "grammar.y"
    { (yyval.vbin) = new QSVarBindingNode((yyvsp[(1) - (1)].tvar), 0L); ;}
    break;

  case 318:
#line 874 "grammar.y"
    { (yyval.vbin) = new QSVarBindingNode((yyvsp[(1) - (3)].tvar), (yyvsp[(3) - (3)].node)); ;}
    break;

  case 319:
#line 878 "grammar.y"
    { (yyval.tvar) = new QSTypedVarNode((yyvsp[(1) - (1)].ustr), 0L); ;}
    break;

  case 320:
#line 879 "grammar.y"
    { (yyval.tvar) = new QSTypedVarNode((yyvsp[(1) - (3)].ustr), (yyvsp[(3) - (3)].type)); ;}
    break;

  case 321:
#line 885 "grammar.y"
    { (yyval.type) = new QSTypeNode((yyvsp[(1) - (1)].ustr)); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3562 "grammar.tab.c"
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
      yyerror (&yylloc, lx, YY_("syntax error"));
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
	    yyerror (&yylloc, lx, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, lx, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

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
		      yytoken, &yylval, &yylloc, lx);
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
		  yystos[yystate], yyvsp, yylsp, lx);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though.  */
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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, lx, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, lx);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, lx);
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


#line 888 "grammar.y"


inline int yylex( YYSTYPE *lvalp, YYLTYPE *llocp, QSLexer *lx )
{
	return lx->lex( lvalp, llocp );
}

/* Called by yyparse on error */
inline int yyerror ( YYLTYPE *, QSLexer *lx, const char *errstr )
{
  lx->setErrorMessage( errstr );
  return 1;
}

/* may we automatically insert a semicolon ? */
inline bool allowAutomaticSemicolon( int yychar, QSLexer *lx )
{
    return yychar == '}' || yychar == 0 || lx->prevTerminator();
}



