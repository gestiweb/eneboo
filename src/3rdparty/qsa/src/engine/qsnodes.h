/****************************************************************************
**
** Copyright (C) 1992-2006 Trolltech AS. All rights reserved.
**
** This file is part of the QSA of the Qt Toolkit.
**
** For QSA Commercial License Holders (non-open source):
** 
** Licensees holding a valid Qt Script for Applications (QSA) License Agreement
** may use this file in accordance with the rights, responsibilities and
** obligations contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of the Licensing Agreement are
** not clear to you.
** 
** Further information about QSA licensing is available at:
** http://www.trolltech.com/products/qsa/licensing.html or by contacting
** info@trolltech.com.
** 
** 
** For Open Source Edition:  
** 
** This file may be used under the terms of the GNU General Public License
** version 2 as published by the Free Software Foundation and appearing in the
** file LICENSE.GPL included in the packaging of this file.  Please review the
** following information to ensure GNU General Public Licensing requirements
** will be met:  http://www.trolltech.com/products/qt/opensource.html 
** 
** If you are unsure which license is appropriate for your use, please review
** the following information:
** http://www.trolltech.com/products/qsa/licensing.html or contact the 
** sales department at sales@trolltech.com.

**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QSNODES_H
#define QSNODES_H

#include "qsmember.h"
#include "qsglobal.h"
#include "qsinternal.h"
#include "qsobject.h"
#include "qsreference.h"
#include "qstypes.h"
#include "qsdebugger.h"
#include "qslexer.h"

#include <QString>
#include <QList>

class QSObject;
class QSCheckData;
class RegExp;
class QSSourceElementsNode;
class QSProgramNode;
class QSTypeNode;
class QSLookupInfo;
class QSLexer;

enum Operator { OpEqual,
                OpEqEq,
                OpNotEq,
                OpStrEq,
                OpStrNEq,
                OpPlusEq,
                OpMinusEq,
                OpMultEq,
                OpDivEq,
                OpPlusPlus,
                OpMinusMinus,
                OpLess,
                OpLessEq,
                OpGreater,
                OpGreaterEq,
                OpAndEq,
                OpXOrEq,
                OpOrEq,
                OpModEq,
                OpAnd,
                OpOr,
                OpBitAnd,
                OpBitXOr,
                OpBitOr,
                OpLShift,
                OpRShift,
                OpURShift,
                OpIs,
                OpIn,
                OpInstanceOf
              };

class QSNode
{
public:
    QSNode() ;
    virtual ~QSNode();
    virtual QSObject evaluate( QSEnv * ) ;
    virtual QSObject rhs( QSEnv * ) const = 0;
    virtual QSReference lhs( QSEnv * ) ;
    int lineNo() const
    {
        return line;
    }
    virtual void setLineNo( int l ) ;
    virtual void check( QSCheckData * ) = 0;
#ifdef QSDEBUGGER
    virtual bool setBreakpoint( int, int, bool ) const
    {
        return false;
    }
#endif
    virtual bool deref()
    {
        Q_ASSERT( refCount > 0 );
        return !--refCount;
    }
    virtual void ref()
    {
        ++refCount;
    }
protected:
    QSObject throwError( QSEnv *env, ErrorType e, const char *msg ) const ;
    int refCount;
private:
    // disallow assignment and copy-construction
    QSNode( const QSNode & );
    QSNode& operator=( const QSNode& );
    int line;
};

class QSStatementNode : public QSNode
{
public:
#ifdef QSDEBUGGER
    QSStatementNode() : l0( -1 ), l1( -1 ), sid( -1 ), breakPoint( false )
    {}
    void setLoc( int line0, int line1, QSLexer * l );
    int firstLine() const
    {
        return l0;
    }
    int lastLine() const
    {
        return l1;
    }
    int sourceId() const
    {
        return sid;
    }
    bool hitStatement( QSEnv *env ) ;
    bool abortStatement( QSEnv *env ) ;
    virtual bool setBreakpoint( int id, int line, bool set ) ;
#endif
    virtual QSObject execute( QSEnv * ) = 0;
    void check( QSCheckData * ) = 0;
    void checkIfGlobalAllowed( QSCheckData * ) ;
    QSObject errorCompletion() ;
    void pushLabel( uint id )
    {
        if ( id > 0 ) ls.push( id );
    }
protected:
    LabelStack ls;
private:
    QSObject evaluate( QSEnv *env )
    {
        return env->createUndefined();
    }
    QSObject rhs( QSEnv *env ) const
    {
        return env->createUndefined();
    }
#ifdef QSDEBUGGER
    int l0, l1;
    int sid;
    bool breakPoint;
#endif
};

class QSAttributeNode : public QSNode
{
public:
    QSAttributeNode()
    {}
    QSAttributeNode( QSAttribute a )
    {
        add( a );
    }
    void add( QSAttribute a )
    {
        attrs.append( a );
    }
    QSObject rhs( QSEnv *env ) const
    {
        return env->createUndefined();
    }
    void check( QSCheckData * ) ;

private:
    QList<QSAttribute> attrs;
};

class QSNullNode : public QSNode
{
public:
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
};

class QSBooleanNode : public QSNode
{
public:
    QSBooleanNode( bool v ) : value( v )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
private:
    bool value;
};

class QSNumberNode : public QSNode
{
public:
    QSNumberNode( double v ) : value( v )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
private:
    double value;
};

class QSStringNode : public QSNode
{
public:
    QSStringNode( uint v )
            : value( v )
    {
        REF_USTR( value );
    }
    ~QSStringNode()
    {
        DEREF_USTR( value );
    }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
private:
    uint value;
};

class QSRegExpNode : public QSNode
{
public:
    QSRegExpNode( uint p, uint f )
            : pattern( p ), flags( f )
    {
        REF_USTR( pattern );
        REF_USTR( flags );
    }
    ~QSRegExpNode()
    {
        DEREF_USTR( pattern );
        DEREF_USTR( flags );
    }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
private:
    uint pattern, flags;
};

class QSThisNode : public QSNode
{
public:
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
};

class QSResolveNode : public QSNode
{
public:

    QSResolveNode( uint s )
            : ident( s ), info( 0 )
    {
        REF_USTR( ident );
    }
    ~QSResolveNode();
    QSObject rhs( QSEnv * ) const ;
    QSReference lhs( QSEnv * ) ;
    void assign( const QSObject &val ) const ;
    virtual void check( QSCheckData * ) ;
private:
    uint ident;
    QSLookupInfo *info;
};

class QSGroupNode : public QSNode
{
public:
    QSGroupNode( QSNode *g ) : group( g )
    { }
    QSObject rhs( QSEnv * ) const ;
    QSReference lhs( QSEnv *env ) ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *group;
};

class QSElisionNode : public QSNode
{
public:
    QSElisionNode( QSElisionNode *e ) : elision( e )
    {}
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSElisionNode *elision;
};

class QSElementNode : public QSNode
{
public:
    QSElementNode( QSElisionNode *e, QSNode *n )
            : list( 0 ), elision( e ), node( n )
    {}
    QSElementNode( QSElementNode *l, QSElisionNode *e, QSNode *n )
            : list( l ), elision( e ), node( n )
    {}
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSElementNode *list;
    QSElisionNode *elision;
    QSNode *node;
};

class QSArrayNode : public QSNode
{
public:
    QSArrayNode( QSElisionNode *e )
            : element( 0 ), elision( e ), opt( true )
    {}
    QSArrayNode( QSElementNode *ele )
            : element( ele ), elision( 0 ), opt( false )
    {}
    QSArrayNode( QSElisionNode *eli, QSElementNode *ele )
            : element( ele ), elision( eli ), opt( true )
    {}
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSElementNode *element;
    QSElisionNode *elision;
    bool opt;
};

class QSObjectLiteralNode : public QSNode
{
public:
    QSObjectLiteralNode( QSNode *l ) : list( l )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *list;
};

class QSPropertyValueNode : public QSNode
{
public:
    QSPropertyValueNode( QSNode *n, QSNode *a, QSNode *l = 0 )
            : name( n ), assign( a ), list( l )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *name, *assign, *list;
};

class QSPropertyNode : public QSNode
{
public:
    QSPropertyNode( double d )
            : numeric( d ), str( 0 )
    { }
    QSPropertyNode( uint s )
            : numeric( 0 ), str( s )
    {
        REF_USTR( str );
    }
    ~QSPropertyNode()
    {
        if ( str != 0 )
            DEREF_USTR( str );
    }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
private:
    double numeric;
    uint str;
};

class QSAccessorNode1 : public QSNode
{
public:
    QSAccessorNode1( QSNode *e1, QSNode *e2 ) : expr1( e1 ), expr2( e2 )
    { }
    QSReference lhs( QSEnv * ) ;
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr1;
    QSNode *expr2;
};

class QSAccessorNode2 : public QSNode
{
public:

    QSAccessorNode2( QSNode *e, uint s )
            : expr( e ), ident( s )
    {
        REF_USTR( ident );
    }
    ~QSAccessorNode2()
    {
        DEREF_USTR( ident );
    }
    QSReference lhs( QSEnv * ) ;
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
    uint ident;
};

class QSArgumentListNode : public QSNode
{
public:
    QSArgumentListNode( QSNode *e )
            : list( 0 ), expr( e )
    { }
    QSArgumentListNode( QSArgumentListNode *l, QSNode *e )
            : list( l ), expr( e )
    { }
    QSObject rhs( QSEnv * ) const ;
    QSList *evaluateList( QSEnv * ) ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSArgumentListNode *list;
    QSNode *expr;
};

class QSArgumentsNode : public QSNode
{
public:
    QSArgumentsNode( QSArgumentListNode *l ) : list( l )
    { }
    QSObject rhs( QSEnv * ) const ;
    QSList *evaluateList( QSEnv * ) ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSArgumentListNode *list;
};

class QSNewExprNode : public QSNode
{
public:
    QSNewExprNode( QSNode *e ) : expr( e ), args( 0 )
    { }
    QSNewExprNode( QSNode *e, QSArgumentsNode *a ) : expr( e ), args( a )
    { }
    QSObject rhs( QSEnv * ) const;
    QSObject evaluate( QSEnv * ) ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
    QSArgumentsNode *args;
};

class QSFunctionCallNode : public QSStatementNode
{
public:
    QSFunctionCallNode( QSNode *e, QSArgumentsNode *a )
            : expr( e ), args( a )
    { }
    QSObject rhs( QSEnv * ) const ;
#ifdef QSDEBUGGER
    void steppingInto( bool in, QSEnv *env ) const ;
    Debugger::Mode previousMode;
#endif
    virtual void check( QSCheckData * ) ;
    virtual QSObject execute( QSEnv * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
    QSArgumentsNode *args;
};

class QSEmitNode : public QSNode
{
public:
    QSEmitNode( QSNode *e, QSArgumentsNode *a )
            : expr( e ), args( a )
    { }
    QSObject rhs( QSEnv * ) const
    {
        return QSObject();
    }
    virtual void check( QSCheckData * )
    { }
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
    QSArgumentsNode *args;
};

class QSPostfixNode : public QSNode
{
public:
    QSPostfixNode( QSNode *e, Operator o ) : expr( e ), oper( o )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData* ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
    Operator oper;
};

class QSDeleteNode : public QSNode
{
public:
    QSDeleteNode( QSNode *e ) : expr( e )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
};

class QSVoidNode : public QSNode
{
public:
    QSVoidNode( QSNode *e ) : expr( e )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
};

class QSTypeOfNode : public QSNode
{
public:
    QSTypeOfNode( QSNode *e ) : expr( e )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
};

class QSPrefixNode : public QSNode
{
public:
    QSPrefixNode( Operator o, QSNode *e ) : oper( o ), expr( e )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    Operator oper;
    QSNode *expr;
};

class QSUnaryPlusNode : public QSNode
{
public:
    QSUnaryPlusNode( QSNode *e ) : expr( e )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
};

class QSNegateNode : public QSNode
{
public:
    QSNegateNode( QSNode *e ) : expr( e )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
};

class QSBitwiseNotNode : public QSNode
{
public:
    QSBitwiseNotNode( QSNode *e ) : expr( e )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
};

class QSLogicalNotNode : public QSNode
{
public:
    QSLogicalNotNode( QSNode *e ) : expr( e )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
};

class QSMultNode : public QSNode
{
public:
    QSMultNode( QSNode *t1, QSNode *t2, char op )
            : term1( t1 ), term2( t2 ), oper( op )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *term1, *term2;
    char oper;
};

class QSAddNode : public QSNode
{
public:
    QSAddNode( QSNode *t1, QSNode *t2, char op )
            : term1( t1 ), term2( t2 ), oper( op )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *term1, *term2;
    char oper;
};

class QSShiftNode : public QSNode
{
public:
    QSShiftNode( QSNode *t1, Operator o, QSNode *t2 )
            : term1( t1 ), term2( t2 ), oper( o )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *term1, *term2;
    Operator oper;
};

class QSRelationalNode : public QSNode
{
public:
    QSRelationalNode( QSNode *e1, Operator o, QSNode *e2 )
            : expr1( e1 ), expr2( e2 ), oper( o )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr1, *expr2;
    Operator oper;
};

class QSEqualNode : public QSNode
{
public:
    QSEqualNode( QSNode *e1, Operator o, QSNode *e2 )
            : expr1( e1 ), expr2( e2 ), oper( o )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr1, *expr2;
    Operator oper;
};

class QSBitOperNode : public QSNode
{
public:
    QSBitOperNode( QSNode *e1, Operator o, QSNode *e2 )
            : expr1( e1 ), expr2( e2 ), oper( o )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr1, *expr2;
    Operator oper;
};

class QSBinaryLogicalNode : public QSNode
{
public:
    QSBinaryLogicalNode( QSNode *e1, Operator o, QSNode *e2 )
            : expr1( e1 ), expr2( e2 ), oper( o )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr1, *expr2;
    Operator oper;
};

class QSConditionalNode : public QSNode
{
public:
    QSConditionalNode( QSNode *l, QSNode *e1, QSNode *e2 )
            : logical( l ), expr1( e1 ), expr2( e2 )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *logical, *expr1, *expr2;
};

class QSAssignNode : public QSNode
{
public:
    QSAssignNode( QSNode *l, Operator o, QSNode *e )
            : left( l ), oper( o ), expr( e )
    { }
    QSObject rhs( QSEnv * ) const ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *left;
    Operator oper;
    QSNode *expr;
};

class QSCommaNode : public QSNode
{
public:
    QSCommaNode( QSNode *e1, QSNode *e2 ) : expr1( e1 ), expr2( e2 )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr1, *expr2;
};

class QSStatListNode : public QSStatementNode
{
public:
    QSStatListNode( QSStatementNode *s ) : statement( s ), list( 0 )
    { }
    QSStatListNode( QSStatListNode *l, QSStatementNode *s )
            : statement( s ), list( l )
    { }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSStatementNode *statement;
    QSStatListNode *list;
};

class QSAssignExprNode : public QSNode
{
public:
    QSAssignExprNode( QSNode *e ) : expr( e )
    { }
    QSObject rhs( QSEnv * ) const ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
};

class QSScopeNode : public QSStatementNode
{
public:
    QSScopeNode() : scope( 0 )
    {}
    virtual void check( QSCheckData * ) ;
    virtual QSObject execute( QSEnv * ) ;
    virtual void checkStatement( QSCheckData * ) = 0;
    virtual QSObject executeStatement( QSEnv * ) = 0;
protected:
    QSBlockScopeClass *scope;
};

class QSBlockNode : public QSScopeNode
{
public:
    QSBlockNode( QSStatListNode *s ) : statlist( s )
    { }
    QSObject executeStatement( QSEnv * ) ;
    void checkStatement( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSStatListNode *statlist;
};

class QSEmptyStatementNode : public QSStatementNode
{
public:
    QSEmptyStatementNode()
    {} // debug
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
};

class QSExprStatementNode : public QSStatementNode
{
public:
    QSExprStatementNode( QSNode *e ) : expr( e )
    { }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
};

class QSIfNode : public QSStatementNode
{
public:
    QSIfNode( QSNode *e, QSStatementNode *s1, QSStatementNode *s2 )
            : expr( e ), statement1( s1 ), statement2( s2 )
    { }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
    QSStatementNode *statement1, *statement2;
};

class QSDoWhileNode : public QSStatementNode
{
public:
    QSDoWhileNode( QSStatementNode *s, QSNode *e )
            : statement( s ), expr( e )
    { }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSStatementNode *statement;
    QSNode *expr;
};

class QSWhileNode : public QSStatementNode
{
public:
    QSWhileNode( QSNode *e, QSStatementNode *s )
            : expr( e ), statement( s )
    { }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
    QSStatementNode *statement;
};

class QSForNode : public QSScopeNode
{
public:
    QSForNode( QSNode *e1, QSNode *e2, QSNode *e3, QSStatementNode *s ) :
            expr1( e1 ), expr2( e2 ), expr3( e3 ), stat( s )
    { }
    QSObject executeStatement( QSEnv * ) ;
    void checkStatement( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr1, *expr2, *expr3;
    QSStatementNode *stat;
};

class QSVarBindingNode;

class QSForInNode : public QSScopeNode
{
public:
    QSForInNode( QSNode *l, QSNode *e, QSStatementNode *s )
            : var( 0 ), lexpr( l ), expr( e ), stat( s )
    { }
    QSForInNode( QSVarBindingNode *v,
                 QSNode *e, QSStatementNode *s )
            : var( v ), lexpr( 0 ), expr( e ), stat( s )
    { }
    QSForInNode( QSVarBindingNode *v, QSNode *l,
                 QSNode *e, QSStatementNode *s )
            : var( v ), lexpr( l ), expr( e ), stat( s )
    { }
    QSObject executeStatement( QSEnv * ) ;
    void checkStatement( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSVarBindingNode *var;
    QSNode *lexpr, *expr;
    QSStatementNode *stat;
};

class QSContinueNode : public QSStatementNode
{
public:
    QSContinueNode() : ident( 0 )
    {}
    QSContinueNode( uint i )
            : ident( i )
    {
        REF_USTR( ident );
    }
    ~QSContinueNode()
    {
        if ( ident != 0 )
            DEREF_USTR( ident );
    }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
private:
    uint ident;
};

class QSBreakNode : public QSStatementNode
{
public:
    QSBreakNode() : ident( 0 )
    {}
    QSBreakNode( uint i )
            : ident( i )
    {
        REF_USTR( ident );
    }
    ~QSBreakNode()
    {
        if ( ident != 0 )
            DEREF_USTR( ident );
    }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
private:
    uint ident;
};

class QSReturnNode : public QSStatementNode
{
public:
    QSReturnNode( QSNode *v ) : value( v )
    { }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *value;
};

class QSWithNode : public QSScopeNode
{
public:
    QSWithNode( QSNode *e, QSStatementNode *s ) : expr( e ), stat( s )
    { }
    QSObject executeStatement( QSEnv * ) ;
    void checkStatement( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
    QSStatementNode *stat;
};

class QSCaseClauseNode: public QSNode
{
public:
    QSCaseClauseNode( QSNode *e, QSStatListNode *l ) : expr( e ), list( l )
    { }
    QSObject rhs( QSEnv * ) const ;
    QSObject evalStatements( QSEnv *env ) ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
    QSStatListNode *list;
};

class QSClauseListNode : public QSNode
{
public:
    QSClauseListNode( QSCaseClauseNode *c ) : cl( c ), nx( 0 )
    { }
    QSClauseListNode( QSCaseClauseNode *c, QSClauseListNode *n ) : cl( c ), nx( n )
    { }
    QSClauseListNode* append( QSCaseClauseNode *c ) ;
    QSCaseClauseNode *clause() const
    {
        return cl;
    }
    QSClauseListNode *next() const
    {
        return nx;
    }
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSObject evaluate( QSEnv * )
    { /* should never be called */ return QSObject();
    }
    QSObject rhs( QSEnv *env ) const
    { /* should never be called */ return env->createUndefined();
    }
    QSCaseClauseNode *cl;
    QSClauseListNode *nx;
};

class QSCaseBlockNode: public QSNode
{
public:
    QSCaseBlockNode( QSClauseListNode *l1, QSCaseClauseNode *d,
                     QSClauseListNode *l2 )
            : list1( l1 ), def( d ), list2( l2 )
    { }
    QSObject evalBlock( QSEnv *env, const QSObject& input ) ;
    virtual void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSObject evaluate( QSEnv * )
    { /* should never be called */ return QSObject();
    }
    QSObject rhs( QSEnv *env ) const
    { /* should never be called */ return env->createUndefined();
    }
    QSClauseListNode *list1;
    QSCaseClauseNode *def;
    QSClauseListNode *list2;
};

class QSSwitchNode : public QSStatementNode
{
public:
    QSSwitchNode( QSNode *e, QSCaseBlockNode *b ) : expr( e ), block( b )
    { }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
    QSCaseBlockNode *block;
};

class QSLabelNode : public QSStatementNode
{
public:
    QSLabelNode( uint l, QSStatementNode *s )
            : label( l ), stat( s )
    {
        REF_USTR( label );
    }
    ~QSLabelNode()
    {
        DEREF_USTR( label );
    }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    uint label;
    QSStatementNode *stat;
};

class QSThrowNode : public QSStatementNode
{
public:
    QSThrowNode( QSNode *e ) : expr( e )
    { }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSNode *expr;
};

class QSCatchNode : public QSScopeNode
{
public:
    QSCatchNode( uint i, QSStatementNode *b )
            : ident( i ), block( b )
    {
        REF_USTR( ident );
    }
    ~QSCatchNode()
    {
        DEREF_USTR( ident );
    }
    QSObject executeStatement( QSEnv *env ) ;
    void checkStatement( QSCheckData * ) ;
    void setException( const QSObject &e )
    {
        exception = e;
    }
    bool deref() ;
    void ref() ;
private:
    uint ident;
    QSStatementNode *block;
    int index;
    QSObject exception;
};

class QSFinallyNode : public QSStatementNode
{
public:
    QSFinallyNode( QSStatementNode *b ) : block( b )
    { }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSStatementNode *block;
};

class QSTryNode : public QSStatementNode
{
public:
    QSTryNode( QSStatementNode *b, QSNode *c = 0, QSNode *f = 0 )
            : block( b ), _catch( ( QSCatchNode* ) c ),
            _final( ( QSFinallyNode* ) f )
    { }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSStatementNode *block;
    QSCatchNode *_catch;
    QSFinallyNode *_final;
};

class QSParameterNode : public QSNode
{
public:
    QSParameterNode( uint i, QSTypeNode *t )
            : id( i ), typ( t ), next( 0 )
    {
        REF_USTR( id );
    }
    QSParameterNode( uint i, QSTypeNode *t, QSParameterNode *n )
            : id( i ), typ( t ), next( n )
    {
        REF_USTR( id );
    }
    ~QSParameterNode()
    {
        DEREF_USTR( id );
    }
    QSParameterNode *append( uint i, QSTypeNode *t ) ;
    QSObject rhs( QSEnv * ) const ;
    uint ident() const
    {
        return id;
    }
    QSParameterNode *nextParam() const
    {
        return next;
    }
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    uint id;
    QSTypeNode *typ;
    QSParameterNode *next;
};

// inherited by QSProgramNode
class QSFunctionBodyNode : public QSStatementNode
{
public:
    QSFunctionBodyNode( QSSourceElementsNode * s, QSLexer * l );
    virtual ~QSFunctionBodyNode();
    void check( QSCheckData * ) ;
    QSObject execute( QSEnv * ) ;
    void setScopeDefinition( QSFunctionScopeClass * s )
    {
        scopeDef = s;
    }
    QSFunctionScopeClass * scopeDefinition() const
    {
        return scopeDef;
    }
    uint index() const
    {
        return idx;
    }
    void setIndex( uint i )
    {
        idx = i;
    }
    virtual bool deref();
    void ref();
protected:
    QSSourceElementsNode * source;
    QSFunctionScopeClass * scopeDef;
    uint idx;
    QSLexer * lx;
};

class QSFuncDeclNode : public QSStatementNode
{
public:
    QSFuncDeclNode( uint i, QSParameterNode *p, QSTypeNode *t, QSFunctionBodyNode *b )
            : ident( i ), param( p ), rtype( t ), body( b ), attrs( 0 )
    {
        REF_USTR( ident );
    }
    QSFuncDeclNode( uint i, QSParameterNode *p, QSTypeNode *t, QSFunctionBodyNode *b, QSAttributeNode *a )
            : ident( i ), param( p ), rtype( t ), body( b ), attrs( a )
    {
        REF_USTR( ident );
    }
    ~QSFuncDeclNode()
    {
        DEREF_USTR( ident );
    }
    void setAttributes( QSAttributeNode *a )
    {
        attrs = a;
    }
    QSObject execute( QSEnv * )
    {
        return QSObject();
    }
    void check( QSCheckData * ) ;
    uint identifier() const
    {
        return ident;
    }
    bool deref() ;
    void ref() ;
private:
    uint ident;
    QSParameterNode *param;
    QSTypeNode *rtype;
    QSFunctionBodyNode *body;
    QSAttributeNode *attrs;
};

class QSFuncExprNode : public QSNode
{
public:
    QSFuncExprNode( QSParameterNode *p, QSFunctionBodyNode *b )
            : param( p ), body( b )
    { }
    QSObject rhs( QSEnv * ) const ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSParameterNode *param;
    QSFunctionBodyNode *body;
};

class QSClassDefNode;

class QSSourceElementNode : public QSStatementNode
{
public:
    QSSourceElementNode( QSStatementNode *s ) : statement( s )
    { }
    void check( QSCheckData * ) ;
    QSObject execute( QSEnv * ) ;
    void deleteStatements() ;
    bool deref() ;
    void ref() ;
private:
    QSStatementNode *statement;
};

class QSSourceElementsNode : public QSStatementNode
{
public:
    QSSourceElementsNode( QSSourceElementNode *s1 ) :
            element( s1 ), elements( 0 )
    {}
    QSSourceElementsNode( QSSourceElementsNode *s1, QSSourceElementNode *s2 ) :
            element( s2 ), elements( s1 )
    {}
    void check( QSCheckData * ) ;
    QSObject execute( QSEnv * ) ;
    void deleteStatements() ;
    bool deref() ;
    void ref() ;
private:
    QSSourceElementNode *element;
    QSSourceElementsNode *elements;
};

class QSProgramNode : public QSFunctionBodyNode
{
public:
    QSProgramNode( QSSourceElementsNode *s, QSLexer * l );
    ~QSProgramNode();
    void check( QSCheckData * );
    void deleteGlobalStatements();
    bool deref();
};

class QSClassDefNode : public QSStatementNode
{
public:
    QSClassDefNode( uint i, QSTypeNode *t, QSFunctionBodyNode *b )
            : ident( i ), type( t ), body( b ), cldef( 0 ), attrs( 0 )
    {
        REF_USTR( ident );
    }
    QSClassDefNode( uint i, QSTypeNode *t, QSFunctionBodyNode *b, QSAttributeNode *a )
            : ident( i ), type( t ), body( b ), cldef( 0 ), attrs( a )
    {
        REF_USTR( ident );
    }
    ~QSClassDefNode()
    {
        DEREF_USTR( ident );
    }
    void setAttributes( QSAttributeNode *a )
    {
        attrs = a;
    }
    void check( QSCheckData * ) ;
    QSObject execute( QSEnv * ) ;
    uint identifier() const
    {
        return ident;
    }
    void setClassDefinition( QSClassClass *cl )
    {
        cldef = cl;
    }
    bool deref() ;
    void ref() ;
private:
    uint ident;
    QSTypeNode *type;
    QSFunctionBodyNode *body;
    QSClassClass *cldef;
    QSAttributeNode *attrs;
};

class QSTypeNode : public QSNode
{
public:
    QSTypeNode( uint i )
            : ident( i )
    {
        REF_USTR( ident );
    }
    ~QSTypeNode()
    {
        DEREF_USTR( ident );
    }
    void check( QSCheckData * ) ;
    QSObject rhs( QSEnv * ) const ;
    uint identifier() const
    {
        return ident;
    };
private:
    uint ident;
};

class QSTypedVarNode : public QSNode
{
public:
    QSTypedVarNode( uint i, QSTypeNode *t )
            : ident( i ), type( t )
    {
        REF_USTR( ident );
    }
    ~QSTypedVarNode()
    {
        DEREF_USTR( ident );
    }
    QSObject rhs( QSEnv * ) const ;
    uint identifier() const
    {
        return ident;
    }
    QSTypeNode *typeNode() const
    {
        return type;
    }
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    uint ident;
    QSTypeNode *type;
};

class QSVarBindingNode : public QSNode
{
public:
    QSVarBindingNode( QSTypedVarNode *v, QSNode *a )
            : var( v ), assign( ( QSAssignNode* ) a ), idx( 0 )
    { }
    void declare( QSEnv *env ) const ;
    void check( QSCheckData * ) ;
    int index() const
    {
        return idx;
    }
    bool deref() ;
    void ref() ;
private:
    QSObject rhs( QSEnv *env ) const
    {
        declare( env ); return QSObject();
    }
    QSTypedVarNode *var;
    QSAssignNode *assign;
    int idx;
};

class QSVarBindingListNode : public QSNode
{
public:
    QSVarBindingListNode( QSVarBindingListNode *l, QSVarBindingNode *b )
            : list( l ), binding( b )
    { }
    void declare( QSEnv *env ) const ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    QSObject rhs( QSEnv *env ) const
    {
        declare( env ); return QSObject();
    }
    QSVarBindingListNode *list;
    QSVarBindingNode *binding;
};

class QSVarDefNode : public QSStatementNode
{
public:
    QSVarDefNode( int k, QSVarBindingListNode *l )
            : kind( k ), list( l ), attrs( 0 )
    { }
    QSVarDefNode( int k, QSVarBindingListNode *l, QSAttributeNode *a )
            : kind( k ), list( l ), attrs( a )
    { }
    void setAttributes( QSAttributeNode *a )
    {
        attrs = a;
    }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    int kind;
    QSVarBindingListNode *list;
    QSAttributeNode *attrs;
};

class QSPackageNode : public QSStatementNode
{
public:
    QSPackageNode( uint s, QSStatementNode *b )
            : package( s ), block( b )
    {
        REF_USTR( package );
    }
    ~QSPackageNode()
    {
        DEREF_USTR( package );
    }
    QSObject execute( QSEnv * )
    {
        return QSObject();
    }
    void check( QSCheckData * ) ;
    bool deref() ;
    void ref() ;
private:
    uint package;
    QSStatementNode *block;
};

class QSImportNode : public QSStatementNode
{
public:
    QSImportNode( uint s )
            : package( s )
    {
        REF_USTR( package );
    }
    ~QSImportNode()
    {
        DEREF_USTR( package );
    }
    QSObject execute( QSEnv * ) ;
    void check( QSCheckData * ) ;
private:
    uint package;
};


/*!
  Utility class for use with resolvenode's by index lookup.
*/
class QSLookupInfo
{
public:
    QSLookupInfo( int lvl, const QSMember &mem )
            : level( lvl ), member( mem )
    { }
    int level;
    QSMember member;
};

class QSNodeList : public QList<QSNode *>
{
public:
    QSNodeList() : QList<QSNode *>()
    { }
}
;

#endif
