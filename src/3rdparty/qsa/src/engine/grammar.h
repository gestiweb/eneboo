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
/* Line 1489 of yacc.c.  */
#line 222 "grammar.tab.h"
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


