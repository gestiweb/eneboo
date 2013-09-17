%{
/***************************************************************************
AQGrammar
-------------------
begin                : 08/11/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
email                : mail@infosial.com
***************************************************************************/ 
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/ 
/***************************************************************************
   Este  programa es software libre. Puede redistribuirlo y/o modificarlo
   bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
   versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/
 
#include <string.h>
#include "AQNodesQSA.h"

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

%}

%union {
  qint32                ival;
  double                dval;
  quint32               ustr;
  AQNode                *node;
  AQStatementNode       *stat;
  AQParameterNode       *param;
  AQFunctionBodyNode    *body;
  AQFuncDeclNode        *func;
  AQClassDefNode        *clss;
  AQProgramNode         *prog;
  AQAssignExprNode      *init;
  AQSourceElementNode   *src;
  AQSourceElementsNode  *srcs;
  AQStatListNode        *slist;
  AQArgumentsNode       *args;
  AQArgumentListNode    *alist;
  AQCaseBlockNode       *cblk;
  AQClauseListNode      *clist;
  AQCaseClauseNode      *ccl;
  AQElementNode         *elm;
  AQElisionNode         *eli;
  AQTypeNode            *type;
  AQTypedVarNode        *tvar;
  AQVarBindingNode      *vbin;
  AQVarBindingListNode  *blist;
  AQAttributeNode       *attrs;
  QSAttribute         	attr;
  Operator            	op;
}

%start Program

%pure-parser
%parse-param { QSLexer *lx }
%lex-param { QSLexer *lx }

/* literals */
%token NULLTOKEN TRUETOKEN FALSETOKEN

/* keywords */
%token BREAK CASE DEFAULT FOR NEW VAR CONTINUE
%token FUNCTION RETURN AQ_VOID AQ_DELETE
%token IF THIS DO WHILE ELSE AQ_IN INSTANCEOF TYPEOF IS
%token SWITCH WITH RESERVED
%token THROW TRY CATCH FINALLY
%token CLASS CONSTRUCTOR EXTENDS
%token ABSTRACT FINAL PRIVATE PUBLIC STATIC AQ_CONST
%token PACKAGE IMPORT

/* give an if without an else higher precedence than an else to resolve the ambiguity */
%nonassoc IF_WITHOUT_ELSE
%nonassoc ELSE

/* punctuators */
%token EQEQ NE                     /* == and != */
%token STREQ STRNEQ                /* === and !== */
%token LE GE                       /* < and > */
%token OR AND                      /* || and && */
%token PLUSPLUS MINUSMINUS         /* ++ and --  */
%token LSHIFT                      /* << */
%token RSHIFT URSHIFT              /* >> and >>> */
%token PLUSEQUAL MINUSEQUAL        /* += and -= */
%token MULTEQUAL DIVEQUAL          /* *= and /= */
%token LSHIFTEQUAL                 /* <<= */
%token RSHIFTEQUAL URSHIFTEQUAL    /* >>= and >>>= */
%token ANDEQUAL MODEQUAL           /* &= and %= */
%token XOREQUAL OREQUAL            /* ^= and |= */

/* terminal types */
%token <dval> NUMBER
%token <ustr> STRING
%token <ustr> IDENT
%destructor { DEREF_USTR($$); } STRING IDENT

/* non-terminal types */
%type <node>  Literal ArrayLiteral

%type <node>  PrimaryExpr PrimaryExprNoBrace
%type <node>  MemberExpr MemberExprNoBF
%type <node>  NewExpr NewExprNoBF
%type <node>  CallExpr CallExprNoBF
%type <node>  LeftHandSideExpr LeftHandSideExprNoBF
%type <node>  PostfixExpr PostfixExprNoBF
%type <node>  UnaryExpr UnaryExprNoBF UnaryExprCommon
%type <node>  MultiplicativeExpr MultiplicativeExprNoBF
%type <node>  AdditiveExpr AdditiveExprNoBF
%type <node>  ShiftExpr ShiftExprNoBF
%type <node>  RelationalExpr RelationalExprNoIn RelationalExprNoBF
%type <node>  EqualityExpr EqualityExprNoIn EqualityExprNoBF
%type <node>  BitwiseANDExpr BitwiseANDExprNoIn BitwiseANDExprNoBF
%type <node>  BitwiseXORExpr BitwiseXORExprNoIn BitwiseXORExprNoBF
%type <node>  BitwiseORExpr BitwiseORExprNoIn BitwiseORExprNoBF
%type <node>  LogicalANDExpr LogicalANDExprNoIn LogicalANDExprNoBF
%type <node>  LogicalORExpr LogicalORExprNoIn LogicalORExprNoBF
%type <node>  ConditionalExpr ConditionalExprNoIn ConditionalExprNoBF
%type <node>  AssignmentExpr AssignmentExprNoIn AssignmentExprNoBF
%type <node>  Expr ExprNoIn ExprNoBF

%type <node>  ExprOpt ExprNoInOpt

%type <node>  FunctionExpr
%type <node>  PropertyName PropertyNameAndValueList
%type <node>  Catch Finally

%type <stat>  Statement Block
%type <stat>  VariableDefinition EmptyStatement ExprStatement
%type <stat>  IfStatement IterationStatement ContinueStatement
%type <stat>  BreakStatement ReturnStatement WithStatement
%type <stat>  SwitchStatement LabelledStatement
%type <stat>  ThrowStatement TryStatement
%type <stat>  PackageDefinition ImportStatement
%type <clss>  ClassDefinition

%type <slist> StatementList
%type <func>  FunctionDeclaration
%type <body>  FunctionBody
%type <src>   SourceElement
%type <srcs>  SourceElements
%type <param> FormalParameterList
%type <op>    AssignmentOperator
%type <prog>  Program
%type <args>  Arguments
%type <alist> ArgumentList
%type <cblk>  CaseBlock
%type <ccl>   CaseClause DefaultClause
%type <clist> CaseClauses  CaseClausesOpt
%type <eli>   Elision ElisionOpt
%type <elm>   ElementList
%type <ival>  VariableDefinitionKind
%type <attr>  Attribute
%type <attrs> Attributes
%type <type>  TypeExpression ResultSignature
%type <tvar>  TypedVariable
%type <vbin>  VariableBinding VariableBindingNoIn
%type <blist> VariableBindingList VariableBindingListNoIn
%type <ustr>  PackageName

%%

Literal:
    NULLTOKEN                      		{ $$ = new AQNullNode(); }
  | TRUETOKEN                      		{ $$ = new AQBooleanNode(true); }
  | FALSETOKEN                     		{ $$ = new AQBooleanNode(false); }
  | NUMBER                         		{ $$ = new AQNumberNode($1); }
  | STRING                         		{ $$ = new AQStringNode($1); }
  | '/'       /* a RegExp ? */			{ if (!lx->scanRegExp()) YYABORT; $$ = new AQRegExpNode(lx->pattern,lx->flags); }
;

PropertyName:
    IDENT                          		{ $$ = new AQPropertyNode($1); }
  | STRING                         		{ $$ = new AQPropertyNode($1); }
  | NUMBER                         		{ $$ = new AQPropertyNode($1); }
;

PropertyNameAndValueList:
    PropertyName ':' AssignmentExpr     { $$ = new AQPropertyValueNode($1, $3); }
  | PropertyNameAndValueList ',' PropertyName ':' AssignmentExpr
                                   		{ $$ = new AQPropertyValueNode($3, $5, $1); }
;

PrimaryExpr:
    PrimaryExprNoBrace
  | '{' '}'                        		{ $$ = new AQObjectLiteralNode(0L); }
  | '{' PropertyNameAndValueList '}'   	{ $$ = new AQObjectLiteralNode($2); }
  | '{' PropertyNameAndValueList ',' '}'
  										{ $$ = new AQObjectLiteralNode($2); }
;

PrimaryExprNoBrace:
    THIS                           		{ $$ = new AQThisNode(); }
  | Literal
  | ArrayLiteral
  | IDENT                          		{ $$ = new AQResolveNode($1); }
  | '(' Expr ')'                   		{ $$ = new AQGroupNode($2); }
;

ArrayLiteral:
    '[' ElisionOpt ']'                 { $$ = new AQArrayNode($2); }
  | '[' ElementList ']'                { $$ = new AQArrayNode($2); }
  | '[' ElementList ',' ElisionOpt ']' { $$ = new AQArrayNode($4, $2); }
;

ElementList:
    ElisionOpt AssignmentExpr      		{ $$ = new AQElementNode($1, $2); }
  | ElementList ',' ElisionOpt AssignmentExpr
                                   		{ $$ = new AQElementNode($1, $3, $4); }
;

ElisionOpt:
    /* nothing */						{ $$ = 0L; }
  | Elision
;

Elision:
    ','                            		{ $$ = new AQElisionNode(0L); }
  | Elision ','                    		{ $$ = new AQElisionNode($1); }
;

MemberExpr:
    PrimaryExpr
  | FunctionExpr						{ $$ = $1; }	
  | MemberExpr '[' Expr ']'        		{ $$ = new AQAccessorNode1($1, $3); }
  | MemberExpr '.' IDENT           		{ $$ = new AQAccessorNode2($1, $3); }
  | NEW MemberExpr Arguments       		{ $$ = new AQNewExprNode($2, $3); }
;

MemberExprNoBF:
    PrimaryExprNoBrace	
  | MemberExprNoBF '[' Expr ']'        	{ $$ = new AQAccessorNode1($1, $3); }
  | MemberExprNoBF '.' IDENT           	{ $$ = new AQAccessorNode2($1, $3); }
  | NEW MemberExpr Arguments       		{ $$ = new AQNewExprNode($2, $3); }
;

NewExpr:
    MemberExpr
  | NEW NewExpr                    		{ $$ = new AQNewExprNode($2); }
;

NewExprNoBF:
    MemberExprNoBF
  | NEW NewExpr							{ $$ = new AQNewExprNode($2); }
;

CallExpr:
    MemberExpr Arguments           		{ $$ = new AQFunctionCallNode($1, $2); DBG(static_cast<AQFunctionCallNode *>($$), @1, @2); }
  | CallExpr Arguments             		{ $$ = new AQFunctionCallNode($1, $2); DBG(static_cast<AQFunctionCallNode *>($$), @1, @2); }
  | CallExpr '[' Expr ']'          		{ $$ = new AQAccessorNode1($1, $3); }
  | CallExpr '.' IDENT             		{ $$ = new AQAccessorNode2($1, $3); }
;

CallExprNoBF:
    MemberExprNoBF Arguments            { $$ = new AQFunctionCallNode($1, $2); DBG(static_cast<AQFunctionCallNode *>($$), @1, @2); }
  | CallExprNoBF Arguments              { $$ = new AQFunctionCallNode($1, $2); DBG(static_cast<AQFunctionCallNode *>($$), @1, @2); }
  | CallExprNoBF '[' Expr ']'           { $$ = new AQAccessorNode1($1, $3); }
  | CallExprNoBF '.' IDENT              { $$ = new AQAccessorNode2($1, $3); }
;

Arguments:
    '(' ')'                        		{ $$ = new AQArgumentsNode(0L); }
  | '(' ArgumentList ')'           		{ $$ = new AQArgumentsNode($2); }
;

ArgumentList:
    AssignmentExpr                  	{ $$ = new AQArgumentListNode($1); }
  | ArgumentList ',' AssignmentExpr 	{ $$ = new AQArgumentListNode($1, $3); }
;

LeftHandSideExpr:
    NewExpr
  | CallExpr
;

LeftHandSideExprNoBF:
    NewExprNoBF
  | CallExprNoBF
;

PostfixExpr:
    LeftHandSideExpr
  | LeftHandSideExpr PLUSPLUS      		{ $$ = new AQPostfixNode($1, OpPlusPlus); }
  | LeftHandSideExpr MINUSMINUS    		{ $$ = new AQPostfixNode($1, OpMinusMinus); }
;

PostfixExprNoBF:
    LeftHandSideExprNoBF
  | LeftHandSideExprNoBF PLUSPLUS		{ $$ = new AQPostfixNode($1, OpPlusPlus); }
  | LeftHandSideExprNoBF MINUSMINUS		{ $$ = new AQPostfixNode($1, OpMinusMinus); }
;

UnaryExprCommon:
    AQ_DELETE UnaryExpr            		{ $$ = new AQDeleteNode($2); }
  | AQ_VOID UnaryExpr              		{ $$ = new AQVoidNode($2); }
  | TYPEOF UnaryExpr               		{ $$ = new AQTypeOfNode($2); }
  | PLUSPLUS UnaryExpr             		{ $$ = new AQPrefixNode(OpPlusPlus, $2); }
  | MINUSMINUS UnaryExpr           		{ $$ = new AQPrefixNode(OpMinusMinus, $2); }
  | '+' UnaryExpr                  		{ $$ = new AQUnaryPlusNode($2); }
  | '-' UnaryExpr                  		{ $$ = new AQNegateNode($2); }
  | '~' UnaryExpr                  		{ $$ = new AQBitwiseNotNode($2); }
  | '!' UnaryExpr                  		{ $$ = new AQLogicalNotNode($2); }
;

UnaryExpr:
    PostfixExpr
  | UnaryExprCommon
;

UnaryExprNoBF:
    PostfixExprNoBF
  | UnaryExprCommon
;

MultiplicativeExpr:
    UnaryExpr
  | MultiplicativeExpr '*' UnaryExpr 	{ $$ = new AQMultNode($1, $3, '*'); }
  | MultiplicativeExpr '/' UnaryExpr 	{ $$ = new AQMultNode($1, $3, '/'); }
  | MultiplicativeExpr '%' UnaryExpr 	{ $$ = new AQMultNode($1,$3,'%'); }
;

MultiplicativeExprNoBF:
    UnaryExprNoBF
  | MultiplicativeExprNoBF '*' UnaryExpr
                                        { $$ = new AQMultNode($1, $3, '*'); }
  | MultiplicativeExprNoBF '/' UnaryExpr
                                        { $$ = new AQMultNode($1, $3, '/'); }
  | MultiplicativeExprNoBF '%' UnaryExpr
                                        { $$ = new AQMultNode($1,$3,'%'); }
;

AdditiveExpr:
    MultiplicativeExpr
  | AdditiveExpr '+' MultiplicativeExpr { $$ = new AQAddNode($1, $3, '+'); }
  | AdditiveExpr '-' MultiplicativeExpr { $$ = new AQAddNode($1, $3, '-'); }
;

AdditiveExprNoBF:
    MultiplicativeExprNoBF
  | AdditiveExprNoBF '+' MultiplicativeExpr
                                        { $$ = new AQAddNode($1, $3, '+'); }
  | AdditiveExprNoBF '-' MultiplicativeExpr
                                        { $$ = new AQAddNode($1, $3, '-'); }
;

ShiftExpr:
    AdditiveExpr
  | ShiftExpr LSHIFT AdditiveExpr  		{ $$ = new AQShiftNode($1, OpLShift, $3); }
  | ShiftExpr RSHIFT AdditiveExpr  		{ $$ = new AQShiftNode($1, OpRShift, $3); }
  | ShiftExpr URSHIFT AdditiveExpr 		{ $$ = new AQShiftNode($1, OpURShift, $3); }
;

ShiftExprNoBF:
    AdditiveExprNoBF
  | ShiftExprNoBF LSHIFT AdditiveExpr  	{ $$ = new AQShiftNode($1, OpLShift, $3); }
  | ShiftExprNoBF RSHIFT AdditiveExpr  	{ $$ = new AQShiftNode($1, OpRShift, $3); }
  | ShiftExprNoBF URSHIFT AdditiveExpr 	{ $$ = new AQShiftNode($1, OpURShift, $3); }
;

RelationalExpr:
    ShiftExpr
  | RelationalExpr '<' ShiftExpr		{ $$ = new AQRelationalNode($1, OpLess, $3); }
  | RelationalExpr '>' ShiftExpr		{ $$ = new AQRelationalNode($1, OpGreater, $3); }
  | RelationalExpr LE ShiftExpr			{ $$ = new AQRelationalNode($1, OpLessEq, $3); }
  | RelationalExpr GE ShiftExpr			{ $$ = new AQRelationalNode($1, OpGreaterEq, $3); }
  | RelationalExpr IS ShiftExpr			{ $$ = new AQRelationalNode($1, OpIs, $3); }
  | RelationalExpr INSTANCEOF ShiftExpr	{ $$ = new AQRelationalNode($1, OpInstanceOf, $3); }
  | RelationalExpr AQ_IN ShiftExpr		{ $$ = new AQRelationalNode($1, OpIn, $3); }
;

RelationalExprNoIn:
    ShiftExpr
  | RelationalExprNoIn '<' ShiftExpr	{ $$ = new AQRelationalNode($1, OpLess, $3); }
  | RelationalExprNoIn '>' ShiftExpr	{ $$ = new AQRelationalNode($1, OpGreater, $3); }
  | RelationalExprNoIn LE ShiftExpr		{ $$ = new AQRelationalNode($1, OpLessEq, $3); }
  | RelationalExprNoIn GE ShiftExpr		{ $$ = new AQRelationalNode($1, OpGreaterEq, $3); }
  | RelationalExprNoIn INSTANCEOF ShiftExpr	
  										{ $$ = new AQRelationalNode($1, OpInstanceOf, $3); }
;

RelationalExprNoBF:
    ShiftExprNoBF
  | RelationalExprNoBF '<' ShiftExpr	{ $$ = new AQRelationalNode($1, OpLess, $3); }
  | RelationalExprNoBF '>' ShiftExpr	{ $$ = new AQRelationalNode($1, OpGreater, $3); }
  | RelationalExprNoBF LE ShiftExpr		{ $$ = new AQRelationalNode($1, OpLessEq, $3); }
  | RelationalExprNoBF GE ShiftExpr		{ $$ = new AQRelationalNode($1, OpGreaterEq, $3); }
  | RelationalExprNoBF IS ShiftExpr		{ $$ = new AQRelationalNode($1, OpIs, $3); }
  | RelationalExprNoBF INSTANCEOF ShiftExpr	
  										{ $$ = new AQRelationalNode($1, OpInstanceOf, $3); }
  | RelationalExprNoBF AQ_IN ShiftExpr	{ $$ = new AQRelationalNode($1, OpIn, $3); }
;

EqualityExpr:
    RelationalExpr
  | EqualityExpr EQEQ RelationalExpr   	{ $$ = new AQEqualNode($1, OpEqEq, $3); }
  | EqualityExpr NE RelationalExpr     	{ $$ = new AQEqualNode($1, OpNotEq, $3); }
  | EqualityExpr STREQ RelationalExpr  	{ $$ = new AQEqualNode($1, OpStrEq, $3); }
  | EqualityExpr STRNEQ RelationalExpr 	{ $$ = new AQEqualNode($1, OpStrNEq, $3); }
;

EqualityExprNoIn:
    RelationalExprNoIn
  | EqualityExprNoIn EQEQ RelationalExprNoIn   	
  										{ $$ = new AQEqualNode($1, OpEqEq, $3); }
  | EqualityExprNoIn NE RelationalExprNoIn
  										{ $$ = new AQEqualNode($1, OpNotEq, $3); }
  | EqualityExprNoIn STREQ RelationalExprNoIn
  										{ $$ = new AQEqualNode($1, OpStrEq, $3); }
  | EqualityExprNoIn STRNEQ RelationalExprNoIn
  										{ $$ = new AQEqualNode($1, OpStrNEq, $3); }
;

EqualityExprNoBF:
    RelationalExprNoBF
  | EqualityExprNoBF EQEQ RelationalExpr
  										{ $$ = new AQEqualNode($1, OpEqEq, $3); }
  | EqualityExprNoBF NE RelationalExpr  
  										{ $$ = new AQEqualNode($1, OpNotEq, $3); }
  | EqualityExprNoBF STREQ RelationalExpr
  										{ $$ = new AQEqualNode($1, OpStrEq, $3); }
  | EqualityExprNoBF STRNEQ RelationalExpr
  										{ $$ = new AQEqualNode($1, OpStrNEq, $3); }
;

BitwiseANDExpr:
    EqualityExpr
  | BitwiseANDExpr '&' EqualityExpr 	{ $$ = new AQBitOperNode($1, OpBitAnd, $3); }
;

BitwiseANDExprNoIn:
    EqualityExprNoIn
  | BitwiseANDExprNoIn '&' EqualityExprNoIn
  										{ $$ = new AQBitOperNode($1, OpBitAnd, $3); }
;

BitwiseANDExprNoBF:
    EqualityExprNoBF
  | BitwiseANDExprNoBF '&' EqualityExpr { $$ = new AQBitOperNode($1, OpBitAnd, $3); }
;

BitwiseXORExpr:
    BitwiseANDExpr
  | BitwiseXORExpr '^' BitwiseANDExpr 	{ $$ = new AQBitOperNode($1, OpBitXOr, $3); }
;

BitwiseXORExprNoIn:
    BitwiseANDExprNoIn
  | BitwiseXORExprNoIn '^' BitwiseANDExprNoIn
  										{ $$ = new AQBitOperNode($1, OpBitXOr, $3); }
;

BitwiseXORExprNoBF:
    BitwiseANDExprNoBF
  | BitwiseXORExprNoBF '^' BitwiseANDExpr 
  										{ $$ = new AQBitOperNode($1, OpBitXOr, $3); }
;

BitwiseORExpr:
    BitwiseXORExpr
  | BitwiseORExpr '|' BitwiseXORExpr 	{ $$ = new AQBitOperNode($1, OpBitOr, $3); }
;

BitwiseORExprNoIn:
    BitwiseXORExprNoIn
  | BitwiseORExprNoIn '|' BitwiseXORExprNoIn 
  										{ $$ = new AQBitOperNode($1, OpBitOr, $3); }
;

BitwiseORExprNoBF:
    BitwiseXORExprNoBF
  | BitwiseORExprNoBF '|' BitwiseXORExpr
  										{ $$ = new AQBitOperNode($1, OpBitOr, $3); }
;

LogicalANDExpr:
    BitwiseORExpr
  | LogicalANDExpr AND BitwiseORExpr	{ $$ = new AQBinaryLogicalNode($1, OpAnd, $3); }
;

LogicalANDExprNoIn:
    BitwiseORExprNoIn
  | LogicalANDExprNoIn AND BitwiseORExprNoIn
                           				{ $$ = new AQBinaryLogicalNode($1, OpAnd, $3); }
;

LogicalANDExprNoBF:
    BitwiseORExprNoBF
  | LogicalANDExprNoBF AND BitwiseORExpr
                           				{ $$ = new AQBinaryLogicalNode($1, OpAnd, $3); }
;

LogicalORExpr:
    LogicalANDExpr
  | LogicalORExpr OR LogicalANDExpr		{ $$ = new AQBinaryLogicalNode($1, OpOr, $3); }
;

LogicalORExprNoIn:
    LogicalANDExprNoIn
  | LogicalORExprNoIn OR LogicalANDExprNoIn
                           				{ $$ = new AQBinaryLogicalNode($1, OpOr, $3); }
;

LogicalORExprNoBF:
    LogicalANDExprNoBF
  | LogicalORExprNoBF OR LogicalANDExpr
                           				{ $$ = new AQBinaryLogicalNode($1, OpOr, $3); }
;

ConditionalExpr:
    LogicalORExpr
  | LogicalORExpr '?' AssignmentExpr ':' AssignmentExpr
                           				{ $$ = new AQConditionalNode($1, $3, $5); }
;

ConditionalExprNoIn:
    LogicalORExprNoIn
  | LogicalORExprNoIn '?' AssignmentExprNoIn ':' AssignmentExprNoIn
                           				{ $$ = new AQConditionalNode($1, $3, $5); }
;

ConditionalExprNoBF:
    LogicalORExprNoBF
  | LogicalORExprNoBF '?' AssignmentExpr ':' AssignmentExpr
                           				{ $$ = new AQConditionalNode($1, $3, $5); }
;

AssignmentExpr:
    ConditionalExpr
  | LeftHandSideExpr AssignmentOperator AssignmentExpr
                           				{ $$ = new AQAssignNode($1, $2, $3); }
;

AssignmentExprNoIn:
    ConditionalExprNoIn
  | LeftHandSideExpr AssignmentOperator AssignmentExprNoIn
                           				{ $$ = new AQAssignNode($1, $2, $3); }
;

AssignmentExprNoBF:
    ConditionalExprNoBF
  | LeftHandSideExprNoBF AssignmentOperator AssignmentExpr
                           				{ $$ = new AQAssignNode($1, $2, $3); }
;

AssignmentOperator:
    '='                            		{ $$ = OpEqual; }
  | PLUSEQUAL                      		{ $$ = OpPlusEq; }
  | MINUSEQUAL                     		{ $$ = OpMinusEq; }
  | MULTEQUAL                      		{ $$ = OpMultEq; }
  | DIVEQUAL                       		{ $$ = OpDivEq; }
  | LSHIFTEQUAL                    		{ $$ = OpLShift; }
  | RSHIFTEQUAL                    		{ $$ = OpRShift; }
  | URSHIFTEQUAL                   		{ $$ = OpURShift; }
  | ANDEQUAL                       		{ $$ = OpAndEq; }
  | XOREQUAL                       		{ $$ = OpXOrEq; }
  | OREQUAL                        		{ $$ = OpOrEq; }
  | MODEQUAL                       		{ $$ = OpModEq; }
;

Expr:
    AssignmentExpr
  | Expr ',' AssignmentExpr        		{ $$ = new AQCommaNode($1, $3); }
;

ExprNoIn:
    AssignmentExprNoIn
  | ExprNoIn ',' AssignmentExprNoIn     { $$ = new AQCommaNode($1, $3); }
;

ExprNoBF:
    AssignmentExprNoBF
  | ExprNoBF ',' AssignmentExpr         { $$ = new AQCommaNode($1, $3); }
;

Statement:
    Block
  | VariableDefinition
  | ClassDefinition                		{ $$ = $1; }
  | Attributes ClassDefinition     		{ static_cast<AQClassDefNode *>($2)->setAttributes( $1 ); $$ = $2; }
  | PackageDefinition
  | EmptyStatement
  | ExprStatement
  | IfStatement
  | IterationStatement
  | ContinueStatement
  | BreakStatement
  | ReturnStatement
  | WithStatement
  | SwitchStatement
  | LabelledStatement
  | ThrowStatement
  | TryStatement
  | ImportStatement
;

Block:
    '{' '}'                        		{ $$ = new AQBlockNode(0L); DBG($$, @2, @2); }
  | '{' StatementList '}'          		{ $$ = new AQBlockNode($2); DBG($$, @3, @3); }
;

StatementList:
    Statement                      		{ $$ = new AQStatListNode($1); }
  | StatementList Statement        		{ $$ = new AQStatListNode($1, $2); }
;

EmptyStatement:
    ';'                            		{ $$ = new AQEmptyStatementNode(); }
;

ExprStatement:
    ExprNoBF ';'                       	{ $$ = new AQExprStatementNode($1); DBG($$, @1, @2); }
  | ExprNoBF error                     	{ $$ = new AQExprStatementNode($1); DBG($$, @1, @1); AUTO_SEMICOLON; }
;

IfStatement:
    IF '(' Expr ')' Statement  %prec IF_WITHOUT_ELSE
    									{ $$ = new AQIfNode($3,$5,0L); DBG($$,@1,@4); }
  | IF '(' Expr ')' Statement ELSE Statement
                                   		{ $$ = new AQIfNode($3,$5,$7); DBG($$,@1,@4); }
;

IterationStatement:
	DO Statement WHILE '(' Expr ')' ';' { $$ = new AQDoWhileNode($2,$5); DBG($$,@1,@3); }
  | DO Statement WHILE '(' Expr ')'	error
  										{ $$ = new AQDoWhileNode($2,$5); DBG($$,@1,@3); }
  | WHILE '(' Expr ')' Statement   		{ $$ = new AQWhileNode($3,$5); DBG($$,@1,@4); }
  | FOR '(' ExprNoInOpt ';' ExprOpt ';' ExprOpt ')' Statement
  										{ $$ = new AQForNode($3,$5,$7,$9); DBG($$,@1,@8); }
  | FOR '(' VAR VariableBindingListNoIn ';' ExprOpt ';' ExprOpt ')' Statement
  										{ $$ = new AQForNode($4,$6,$8,$10); DBG($$,@1,@9); }
  | FOR '(' LeftHandSideExpr AQ_IN Expr ')' Statement
  										{ $$ = new AQForInNode($3, $5, $7); DBG($$,@1,@6); }
  | FOR '(' VAR VariableBindingNoIn AQ_IN Expr ')' Statement
  										{ $$ = new AQForInNode($4,$6,$8); DBG($$,@1,@7); }
;

ExprOpt:
    /* nothing */                  		{ $$ = 0L; }
  | Expr
;

ExprNoInOpt:
    /* nothing */                  		{ $$ = 0L; }
  | ExprNoIn
;

ContinueStatement:
    CONTINUE ';'                   		{ $$ = new AQContinueNode(); DBG($$,@1,@2); }
  | CONTINUE error                 		{ $$ = new AQContinueNode(); DBG($$,@1,@2); AUTO_SEMICOLON; }
  | CONTINUE IDENT ';'             		{ $$ = new AQContinueNode($2); DBG($$,@1,@3); }
  | CONTINUE IDENT error           		{ $$ = new AQContinueNode($2); DBG($$,@1,@2); AUTO_SEMICOLON; }
;

BreakStatement:
    BREAK ';'                      		{ $$ = new AQBreakNode();DBG($$,@1,@2); }
  | BREAK error                    		{ $$ = new AQBreakNode(); DBG($$,@1,@1); AUTO_SEMICOLON; }
  | BREAK IDENT ';'                		{ $$ = new AQBreakNode($2); DBG($$,@1,@3); }
  | BREAK IDENT error              		{ $$ = new AQBreakNode($2); DBG($$,@1,@2); AUTO_SEMICOLON; }
;

ReturnStatement:
    RETURN ';'                     		{ $$ = new AQReturnNode(0L); DBG($$,@1,@2); }
  | RETURN error                   		{ $$ = new AQReturnNode(0L); DBG($$,@1,@1); AUTO_SEMICOLON; }
  | RETURN Expr ';'                		{ $$ = new AQReturnNode($2); DBG($$,@1,@2); }
  | RETURN Expr error              		{ $$ = new AQReturnNode($2); DBG($$,@1,@2); AUTO_SEMICOLON; }
;

WithStatement:
    WITH '(' Expr ')' Statement    		{ $$ = new AQWithNode($3,$5); DBG($$, @1, @4); }
;

SwitchStatement:
    SWITCH '(' Expr ')' CaseBlock  		{ $$ = new AQSwitchNode($3, $5); DBG($$, @1, @4); }
;

CaseBlock:
    '{' CaseClausesOpt '}'         		{ $$ = new AQCaseBlockNode($2, 0L, 0L); }
  | '{' CaseClausesOpt DefaultClause CaseClausesOpt '}'
                                   		{ $$ = new AQCaseBlockNode($2, $3, $4); }
;

CaseClausesOpt:
    /* nothing */                  		{ $$ = 0L; }
  | CaseClauses							{ $$ = static_cast<AQClauseListNode *>($1)->finish(); }
;

CaseClauses:
    CaseClause                     		{ $$ = new AQClauseListNode($1); }
  | CaseClauses CaseClause         		{ $$ = static_cast<AQClauseListNode *>($1)->append($2); }
;

CaseClause:
    CASE Expr ':'                  		{ $$ = new AQCaseClauseNode($2, 0L); }
  | CASE Expr ':' StatementList    		{ $$ = new AQCaseClauseNode($2, $4); }
;

DefaultClause:
    DEFAULT ':'                    		{ $$ = new AQCaseClauseNode(0L, 0L); }
  | DEFAULT ':' StatementList      		{ $$ = new AQCaseClauseNode(0L, $3); }
;

LabelledStatement:
    IDENT ':' Statement            		{ static_cast<AQStatementNode *>($3)->pushLabel($1); $$ = new AQLabelNode($1, $3); }
;

ThrowStatement:
    THROW Expr ';'                 		{ $$ = new AQThrowNode($2); DBG($$, @1, @3); }
  | THROW Expr error                 	{ $$ = new AQThrowNode($2); DBG($$, @1, @2); AUTO_SEMICOLON; }
;

TryStatement:
    TRY Block Catch                		{ $$ = new AQTryNode($2, $3); }
  | TRY Block Finally              		{ $$ = new AQTryNode($2, 0L, $3); }
  | TRY Block Catch Finally        		{ $$ = new AQTryNode($2, $3, $4); }
;

Catch:
    CATCH '(' IDENT ')' Block      		{ $$ = new AQCatchNode($3, $5); }
;

Finally:
    FINALLY Block                  		{ $$ = new AQFinallyNode($2); }
;

PackageDefinition:
    PACKAGE PackageName FunctionBody	{ $$ = new AQPackageNode($2, $3); }
;

ImportStatement:
    IMPORT PackageName ';'         		{ $$ = new AQImportNode($2); }
;

PackageName:
      IDENT                        		{ $$ = $1; }
    | PackageName '.' IDENT        		{ $$ = NEW_USTR( USTR($1) + QLatin1String(".") + USTR($3) ); }
;

FunctionDeclaration:
    FUNCTION IDENT '(' ')' ResultSignature FunctionBody
                        				{ $$ = new AQFuncDeclNode($2, 0L, $5, $6); }
  | FUNCTION IDENT '(' FormalParameterList ')' ResultSignature FunctionBody
                        				{ $$ = new AQFuncDeclNode($2, static_cast<AQParameterNode *>($4)->finish(), $6, $7); }
  | Attributes FUNCTION IDENT '(' ')'  ResultSignature FunctionBody
                        				{ $$ = new AQFuncDeclNode($3, 0L, $6, $7);
			  							  static_cast<AQFuncDeclNode *>($$)->setAttributes( $1 ); }
  | Attributes FUNCTION IDENT '(' FormalParameterList ')' ResultSignature FunctionBody        
  										{ $$ = new AQFuncDeclNode($3, static_cast<AQParameterNode *>($5)->finish(), $7, $8);
                          				  static_cast<AQFuncDeclNode *>($$)->setAttributes( $1 ); }
;

ResultSignature:
                                   		{ $$ = 0L; }
  | ':' TypeExpression             		{ $$ = $2; }
;

FunctionExpr:
    FUNCTION '(' ')' FunctionBody		{ $$ = new AQFuncExprNode(0L, $4); }
  | FUNCTION '(' FormalParameterList ')' FunctionBody
                                   		{ $$ = new AQFuncExprNode(static_cast<AQParameterNode *>($3)->finish(), $5); }
;

FormalParameterList:
    IDENT                          		{ $$ = new AQParameterNode($1, 0L); }
  | IDENT ':' TypeExpression       		{ $$ = new AQParameterNode($1, $3); }
  | FormalParameterList ',' IDENT  		{ $$ = static_cast<AQParameterNode *>($1)->append($3, 0L); }
  | FormalParameterList ',' IDENT ':' TypeExpression
                                   		{ $$ = static_cast<AQParameterNode *>($1)->append($3, $5); }
;

FunctionBody:
    '{' '}'  /* TODO: spec ??? */  		{ $$ = new AQFunctionBodyNode(0L, lx); }
  | '{' SourceElements '}'         		{ $$ = new AQFunctionBodyNode($2, lx); }
;

ClassDefinition:
    CLASS IDENT ';'                		{ $$ = new AQClassDefNode($2, 0L, 0L); }
  | CLASS IDENT FunctionBody       		{ $$ = new AQClassDefNode($2, 0L, $3); }
  | CLASS IDENT EXTENDS TypeExpression FunctionBody
                                   		{ $$ = new AQClassDefNode($2, $4, $5); }
;

Program:
    /* empty */                    		{ $$ = new AQProgramNode(0L, lx); }
  | SourceElements                 		{ $$ = new AQProgramNode($1, lx); }
;

SourceElements:
    SourceElement                  		{ $$ = new AQSourceElementsNode($1); }
  | SourceElements SourceElement   		{ $$ = new AQSourceElementsNode($1, $2); }
;

SourceElement:
    Statement                      		{ $$ = new AQSourceElementNode($1); }
  | FunctionDeclaration            		{ $$ = new AQSourceElementNode($1); }
;

Attributes:
    Attribute                      		{ $$ = new AQAttributeNode($1); }
  | Attributes Attribute           		{ static_cast<AQAttributeNode *>($1)->add($2); $$ = $1; }
;

Attribute:
    ABSTRACT                       		{ $$ = AttributeAbstract; }
  | FINAL                          		{ $$ = AttributeFinal; }
  | PRIVATE                        		{ $$ = AttributePrivate; }
  | PUBLIC                         		{ $$ = AttributePublic; }
  | STATIC                         		{ $$ = AttributeStatic; }
  | TRUETOKEN                      		{ $$ = AttributeTrue; }
  | FALSETOKEN                     		{ $$ = AttributeFalse; }
  | CONSTRUCTOR                    		{ $$ = AttributeConstructor; }
;

/************************* Variable Definition ******************************/

VariableDefinition:
    VariableDefinitionKind VariableBindingList ';'
                             			{ $$ = new AQVarDefNode($1, $2); DBG($$, @1, @3); }
  | VariableDefinitionKind VariableBindingList error
                             			{ $$ = new AQVarDefNode($1, $2); DBG($$, @1, @2);  AUTO_SEMICOLON; }
  | Attributes VariableDefinitionKind VariableBindingList ';'
                             			{ $$ = new AQVarDefNode($2, $3); DBG($$, @1, @4); 
                             			  static_cast<AQVarDefNode *>($$)->setAttributes($1); }
  | Attributes VariableDefinitionKind VariableBindingList error
                             			{ $$ = new AQVarDefNode($2, $3); DBG($$, @1, @3); 
                             			  static_cast<AQVarDefNode *>($$)->setAttributes($1); AUTO_SEMICOLON; }
;

VariableDefinitionKind:
    VAR 								{ $$ = 0; }
  | AQ_CONST 							{ $$ = 1; }
;

VariableBindingList:
    VariableBinding                		{ $$ = new AQVarBindingListNode(0L, $1); }
  | VariableBindingList ',' VariableBinding
                                   		{ $$ = new AQVarBindingListNode($1, $3); }
;

VariableBindingListNoIn:
    VariableBindingNoIn               	{ $$ = new AQVarBindingListNode(0L, $1); }
  | VariableBindingListNoIn ',' VariableBindingNoIn
                                   		{ $$ = new AQVarBindingListNode($1, $3); }
;

VariableBinding:
    TypedVariable                      	{ $$ = new AQVarBindingNode($1, 0L); }
  | TypedVariable '=' AssignmentExpr   	{ $$ = new AQVarBindingNode($1, $3); }
;

VariableBindingNoIn:
    TypedVariable                      	{ $$ = new AQVarBindingNode($1, 0L); }
  | TypedVariable '=' AssignmentExprNoIn
  										{ $$ = new AQVarBindingNode($1, $3); }
;

TypedVariable:
    IDENT                          		{ $$ = new AQTypedVarNode($1, 0L); }
  | IDENT ':' TypeExpression       		{ $$ = new AQTypedVarNode($1, $3); }
;

/************************* Type Expressions *********************************/

TypeExpression:
    IDENT                          		{ $$ = new AQTypeNode($1); }
;

%%

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


