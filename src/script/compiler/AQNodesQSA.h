/***************************************************************************
AQNodesAQA.h
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

#ifndef AQNODESQSA_H_
#define AQNODESQSA_H_

#include <qsnodes.h>

#include <QString>
#include <QList>
#include <QMap>
#include <QHash>
#include <QDataStream>

#define QSNOD(N)    (N ? N->qsNode() : 0)
#define QSNOD2(C,N) static_cast<C *>(N ? N->qsNode() : 0)

class AQSourceElementsNode;
class AQProgramNode;
class AQTypeNode;

enum NodeType {
  aqNode = 120,
  aqStatementNode,
  aqAttributeNode,
  aqNullNode,
  aqBooleanNode,
  aqNumberNode,
  aqStringNode,
  aqRegExpNode,
  aqThisNode,
  aqResolveNode,
  aqGroupNode,
  aqElisionNode,
  aqElementNode,
  aqArrayNode,
  aqObjectLiteralNode,
  aqPropertyValueNode,
  aqPropertyNode,
  aqAccessorNode1,
  aqAccessorNode2,
  aqArgumentListNode,
  aqArgumentsNode,
  aqNewExprNode,
  aqFunctionCallNode,
  aqEmitNode,
  aqPostfixNode,
  aqDeleteNode,
  aqVoidNode,
  aqTypeOfNode,
  aqPrefixNode,
  aqUnaryPlusNode,
  aqNegateNode,
  aqBitwiseNotNode,
  aqLogicalNotNode,
  aqMultNode,
  aqAddNode,
  aqShiftNode,
  aqRelationalNode,
  aqEqualNode,
  aqBitOperNode,
  aqBinaryLogicalNode,
  aqConditionalNode,
  aqAssignNode,
  aqCommaNode,
  aqStatListNode,
  aqAssignExprNode,
  aqScopeNode,
  aqBlockNode,
  aqEmptyStatementNode,
  aqExprStatementNode,
  aqIfNode,
  aqDoWhileNode,
  aqWhileNode,
  aqForNode,
  aqForInNode,
  aqContinueNode,
  aqBreakNode,
  aqReturnNode,
  aqWithNode,
  aqCaseClauseNode,
  aqClauseListNode,
  aqCaseBlockNode,
  aqSwitchNode,
  aqLabelNode,
  aqThrowNode,
  aqCatchNode,
  aqFinallyNode,
  aqTryNode,
  aqParameterNode,
  aqFunctionBodyNode,
  aqFuncDeclNode,
  aqFuncExprNode,
  aqSourceElementNode,
  aqSourceElementsNode,
  aqProgramNode,
  aqClassDefNode,
  aqTypeNode,
  aqTypedVarNode,
  aqVarBindingNode,
  aqVarBindingListNode,
  aqVarDefNode,
  aqPackageNode,
  aqImportNode,
  aqNone
};

class AQNode
{
protected:

  AQNode();
  virtual ~AQNode()
  {}

public:

  virtual NodeType nodeType() const
  {
    return aqNode;
  }
  virtual QSNode * qsNode() const
  {
    return qsNode_;
  }
  virtual void setLineNo( qint32 l )
  {
    line = l;
  }
  virtual void setNodeId( quint32 n )
  {
    nodeId_ = n;
  }
  virtual quint32 nodeId() const
  {
    return nodeId_;
  }
  virtual void makeQSNode()
  {
    Q_ASSERT( 0 );
    qsNode_ = new QSThisNode();
  }
  virtual void serialize( QDataStream & out );
  virtual void unserialize( QDataStream & in );
  static void serializeNodes( QDataStream & out );
  static void unserializeNodes( QDataStream & in, QSLexer * l );
  static AQNode * createNode( NodeType n, QSLexer * l );
  static QString nodeTypeToString( NodeType n );
  static void clear();
  static quint32 newUstr( quint32 hash );
  static AQNode * lastNode()
  {
    return lastNode_;
  }

protected:

  quint32 nodeId_;
  qint32 line;
  QSNode * qsNode_;
  bool headOut_;
  static QMap<quint32, AQNode *> nodesMap;
  static QMultiHash<quint32, quint32> labelsHash;
  static QMultiHash<quint32, quint16> attrsHash;
  static QHash<uint, QString> ustrHash;
  static quint32 nodesCount;
  static AQNode * lastNode_;

private:

  AQNode( const AQNode & );
  AQNode& operator=( const AQNode& );
  static bool autoInsertMap;
};

class AQStatementNode : public AQNode
{
public:
#ifdef QSDEBUGGER

  AQStatementNode() : l0( -1 ), l1( -1 ), sid( -1 ), breakPoint( false )
  {}
  void setLoc( qint32 line0, qint32 line1, QSLexer * l );
#endif

  void pushLabel( quint32 id ) const;
  virtual NodeType nodeType() const
  {
    return aqStatementNode;
  }
  virtual void serialize( QDataStream & out );
  virtual void unserialize( QDataStream & in );
  virtual void makeQSNode()
  {
    Q_ASSERT( 0 );
    qsNode_ = new QSThisNode();
  }
private:
#ifdef QSDEBUGGER
  qint32 l0, l1;
  qint32 sid;
  quint8 breakPoint;
#endif
};

class AQAttributeNode : public AQNode
{
public:
  AQAttributeNode()
  { }
  AQAttributeNode( QSAttribute a )
  {
    add( a );
  }
  void add( QSAttribute a ) const;
  NodeType nodeType() const
  {
    return aqAttributeNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
  void makeQSNode()
  {
    qsNode_ = new QSAttributeNode( );
  }
};

class AQNullNode : public AQNode
{
public:
  NodeType nodeType() const
  {
    return aqNullNode;
  }
  void makeQSNode()
  {
    qsNode_ = new QSNullNode();
  }
};

class AQBooleanNode : public AQNode
{
public:
  AQBooleanNode()
  { }
  AQBooleanNode( bool v ) : value( v )
  { }
  NodeType nodeType() const
  {
    return aqBooleanNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
  void makeQSNode()
  {
    qsNode_ = new QSBooleanNode( value );
  }
private:
  bool value;
};

class AQNumberNode : public AQNode
{
public:
  AQNumberNode()
  { }
  AQNumberNode( double v ) : value( v )
  { }
  NodeType nodeType() const
  {
    return aqNumberNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
  void makeQSNode()
  {
    qsNode_ = new QSNumberNode( value );
  }
private:
  double value;
};

class AQStringNode : public AQNode
{
public:
  AQStringNode()
  { }
  AQStringNode( quint32 v )
      : value( v )
  {}
  ~AQStringNode()
  {}
  NodeType nodeType() const
  {
    return aqStringNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
  void makeQSNode()
  {
    qsNode_ = new QSStringNode( value );
  }
private:
  quint32 value;
};

class AQRegExpNode : public AQNode
{
public:
  AQRegExpNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSRegExpNode( pattern, flags );
  }
  AQRegExpNode( quint32 p, quint32 f )
      : pattern( p ), flags( f )
  {}
  ~AQRegExpNode()
  {}
  NodeType nodeType() const
  {
    return aqRegExpNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 pattern, flags;
};

class AQThisNode : public AQNode
{
public:
  void makeQSNode()
  {
    qsNode_ = new QSThisNode();
  }
  NodeType nodeType() const
  {
    return aqThisNode;
  }
};

class AQResolveNode : public AQNode
{
public:
  AQResolveNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSResolveNode( ident );
  }
  AQResolveNode( quint32 s )
      : ident( s )
  {}
  ~AQResolveNode()
  {}
  NodeType nodeType() const
  {
    return aqResolveNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 ident;
};

class AQGroupNode : public AQNode
{
public:
  AQGroupNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSGroupNode( QSNOD( group ) );
  }
  AQGroupNode( AQNode *g ) : group( g )
  { }
  NodeType nodeType() const
  {
    return aqGroupNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode * group;
};

class AQElisionNode : public AQNode
{
public:
  AQElisionNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSElisionNode( QSNOD2( QSElisionNode, elision ) );
  }
  AQElisionNode( AQElisionNode *e ) : elision( e )
  {}
  NodeType nodeType() const
  {
    return aqElisionNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *elision;
};

class AQElementNode : public AQNode
{
public:
  AQElementNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSElementNode( QSNOD2( QSElementNode, list ), QSNOD2( QSElisionNode, elision ), QSNOD( node ) );
  }
  AQElementNode( AQElisionNode *e, AQNode *n )
      : list( 0 ), elision( e ), node( n )
  {}
  AQElementNode( AQElementNode *l, AQElisionNode *e, AQNode *n )
      : list( l ), elision( e ), node( n )
  {}
  NodeType nodeType() const
  {
    return aqElementNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *list;
  AQNode *elision;
  AQNode *node;
};

class AQArrayNode : public AQNode
{
public:
  AQArrayNode()
  { }
  void makeQSNode()
  {
    if ( !opt )
      qsNode_ = new QSArrayNode( QSNOD2( QSElementNode, element ) );
    else
      qsNode_ = new QSArrayNode( QSNOD2( QSElisionNode, elision ), QSNOD2( QSElementNode, element ) );
  }
  AQArrayNode( AQElisionNode *e )
      : element( 0 ), elision( e ), opt( true )
{}
  AQArrayNode( AQElementNode *ele )
      : element( ele ), elision( 0 ), opt( false )
  {}
  AQArrayNode( AQElisionNode *eli, AQElementNode *ele )
      : element( ele ), elision( eli ), opt( true )
  {}
  NodeType nodeType() const
  {
    return aqArrayNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *element;
  AQNode *elision;
  quint32 opt;
};

class AQObjectLiteralNode : public AQNode
{
public:
  AQObjectLiteralNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSObjectLiteralNode( QSNOD( list ) );
  }
  AQObjectLiteralNode( AQNode *l ) : list( l )
  { }
  NodeType nodeType() const
  {
    return aqObjectLiteralNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *list;
};

class AQPropertyValueNode : public AQNode
{
public:
  AQPropertyValueNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSPropertyValueNode( QSNOD( name ), QSNOD( assign ), QSNOD( list ) );
  }
  AQPropertyValueNode( AQNode *n, AQNode *a, AQNode *l = 0 )
      : name( n ), assign( a ), list( l )
  { }
  NodeType nodeType() const
  {
    return aqPropertyValueNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *name, *assign, *list;
};

class AQPropertyNode : public AQNode
{
public:
  AQPropertyNode()
  { }
  void makeQSNode()
  {
    if ( str != 0 )
      qsNode_ = new QSPropertyNode( str );
    else
      qsNode_ = new QSPropertyNode( numeric );
  }
  AQPropertyNode( double d )
      : numeric( d ), str( 0 )
{ }
  AQPropertyNode( quint32 s )
      : numeric( 0 ), str( s )
  {}
  ~AQPropertyNode()
  {}
  NodeType nodeType() const
  {
    return aqPropertyNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  double numeric;
  quint32 str;
};

class AQAccessorNode1 : public AQNode
{
public:
  AQAccessorNode1()
  {}
  void makeQSNode()
  {
    qsNode_ = new QSAccessorNode1( QSNOD( expr1 ), QSNOD( expr2 ) );
  }
  AQAccessorNode1( AQNode *e1, AQNode *e2 ) : expr1( e1 ), expr2( e2 )
  { }
  NodeType nodeType() const
  {
    return aqAccessorNode1;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr1;
  AQNode *expr2;
};

class AQAccessorNode2 : public AQNode
{
public:
  AQAccessorNode2()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSAccessorNode2( QSNOD( expr ), ident );
  }
  AQAccessorNode2( AQNode *e, quint32 s )
      : expr( e ), ident( s )
  {}
  ~AQAccessorNode2()
  {

    ident = 0;
  }
  NodeType nodeType() const
  {
    return aqAccessorNode2;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
  quint32 ident;
};

class AQArgumentListNode : public AQNode
{
public:
  AQArgumentListNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSArgumentListNode( QSNOD2( QSArgumentListNode, list ), QSNOD( expr ) );
  }
  AQArgumentListNode( AQNode *e )
      : list( 0 ), expr( e )
  { }
  AQArgumentListNode( AQArgumentListNode *l, AQNode *e )
      : list( l ), expr( e )
  { }
  NodeType nodeType() const
  {
    return aqArgumentListNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *list;
  AQNode *expr;
};

class AQArgumentsNode : public AQNode
{
public:
  AQArgumentsNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSArgumentsNode( QSNOD2( QSArgumentListNode, list ) );
  }
  AQArgumentsNode( AQArgumentListNode *l ) : list( l )
  { }
  NodeType nodeType() const
  {
    return aqArgumentsNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *list;
};

class AQNewExprNode : public AQNode
{
public:
  AQNewExprNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSNewExprNode( QSNOD( expr ), QSNOD2( QSArgumentsNode, args ) );
  }
  AQNewExprNode( AQNode *e ) : expr( e ), args( 0 )
  { }
  AQNewExprNode( AQNode *e, AQArgumentsNode *a ) : expr( e ), args( a )
  { }
  NodeType nodeType() const
  {
    return aqNewExprNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
  AQNode *args;
};

class AQFunctionCallNode : public AQStatementNode
{
public:
  AQFunctionCallNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSFunctionCallNode( QSNOD( expr ), QSNOD2( QSArgumentsNode, args ) );
  }
  AQFunctionCallNode( AQNode *e, AQArgumentsNode *a )
      : expr( e ), args( a )
  { }
  NodeType nodeType() const
  {
    return aqFunctionCallNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
  AQNode *args;
};

class AQEmitNode : public AQNode
{
public:
  AQEmitNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSEmitNode( QSNOD( expr ), QSNOD2( QSArgumentsNode, args ) );
  }
  AQEmitNode( AQNode *e, AQArgumentsNode *a )
      : expr( e ), args( a )
  { }
  NodeType nodeType() const
  {
    return aqEmitNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
  AQNode *args;
};

class AQPostfixNode : public AQNode
{
public:
  AQPostfixNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSPostfixNode( QSNOD( expr ), ( Operator ) oper );
  }
  AQPostfixNode( AQNode *e, quint32 o ) : expr( e ), oper( o )
  { }
  NodeType nodeType() const
  {
    return aqPostfixNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
  quint8 oper;
};

class AQDeleteNode : public AQNode
{
public:
  AQDeleteNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSDeleteNode( QSNOD( expr ) );
  }
  AQDeleteNode( AQNode *e ) : expr( e )
  { }
  NodeType nodeType() const
  {
    return aqDeleteNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
};

class AQVoidNode : public AQNode
{
public:
  AQVoidNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSVoidNode( QSNOD( expr ) );
  }
  AQVoidNode( AQNode *e ) : expr( e )
  { }
  NodeType nodeType() const
  {
    return aqVoidNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
};

class AQTypeOfNode : public AQNode
{
public:
  AQTypeOfNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSTypeOfNode( QSNOD( expr ) );
  }
  AQTypeOfNode( AQNode *e ) : expr( e )
  { }
  NodeType nodeType() const
  {
    return aqTypeOfNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
};

class AQPrefixNode : public AQNode
{
public:
  AQPrefixNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSPrefixNode( ( Operator ) oper, QSNOD( expr ) );
  }
  AQPrefixNode( quint32 o, AQNode *e ) : oper( o ), expr( e )
  { }
  NodeType nodeType() const
  {
    return aqPrefixNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint8 oper;
  AQNode *expr;
};

class AQUnaryPlusNode : public AQNode
{
public:
  AQUnaryPlusNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSUnaryPlusNode( QSNOD( expr ) );
  }
  AQUnaryPlusNode( AQNode *e ) : expr( e )
  { }
  NodeType nodeType() const
  {
    return aqUnaryPlusNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
};

class AQNegateNode : public AQNode
{
public:
  AQNegateNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSNegateNode( QSNOD( expr ) );
  }
  AQNegateNode( AQNode *e ) : expr( e )
  { }
  NodeType nodeType() const
  {
    return aqNegateNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
};

class AQBitwiseNotNode : public AQNode
{
public:
  AQBitwiseNotNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSBitwiseNotNode( QSNOD( expr ) );
  }
  AQBitwiseNotNode( AQNode *e ) : expr( e )
  { }
  NodeType nodeType() const
  {
    return aqBitwiseNotNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
};

class AQLogicalNotNode : public AQNode
{
public:
  AQLogicalNotNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSLogicalNotNode( QSNOD( expr ) );
  }
  AQLogicalNotNode( AQNode *e ) : expr( e )
  { }
  NodeType nodeType() const
  {
    return aqLogicalNotNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
};

class AQMultNode : public AQNode
{
public:
  AQMultNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSMultNode( QSNOD( term1 ), QSNOD( term2 ), ( char ) oper );
  }
  AQMultNode( AQNode *t1, AQNode *t2, char op )
      : term1( t1 ), term2( t2 ), oper( op )
  { }
  NodeType nodeType() const
  {
    return aqMultNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *term1, *term2;
  quint8 oper;
};

class AQAddNode : public AQNode
{
public:
  AQAddNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSAddNode( QSNOD( term1 ), QSNOD( term2 ), ( char ) oper );
  }
  AQAddNode( AQNode *t1, AQNode *t2, char op )
      : term1( t1 ), term2( t2 ), oper( op )
  { }
  NodeType nodeType() const
  {
    return aqAddNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *term1, *term2;
  quint8 oper;
};

class AQShiftNode : public AQNode
{
public:
  AQShiftNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSShiftNode( QSNOD( term1 ), ( Operator ) oper, QSNOD( term2 ) );
  }
  AQShiftNode( AQNode *t1, quint32 o, AQNode *t2 )
      : term1( t1 ), term2( t2 ), oper( o )
  { }
  NodeType nodeType() const
  {
    return aqShiftNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *term1, *term2;
  quint8 oper;
};

class AQRelationalNode : public AQNode
{
public:
  AQRelationalNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSRelationalNode( QSNOD( expr1 ), ( Operator ) oper, QSNOD( expr2 ) );
  }
  AQRelationalNode( AQNode *e1, quint32 o, AQNode *e2 )
      : expr1( e1 ), expr2( e2 ), oper( o )
  { }
  NodeType nodeType() const
  {
    return aqRelationalNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr1, *expr2;
  quint8 oper;
};

class AQEqualNode : public AQNode
{
public:
  AQEqualNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSEqualNode( QSNOD( expr1 ), ( Operator ) oper, QSNOD( expr2 ) );
  }
  AQEqualNode( AQNode *e1, quint32 o, AQNode *e2 )
      : expr1( e1 ), expr2( e2 ), oper( o )
  { }
  NodeType nodeType() const
  {
    return aqEqualNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr1, *expr2;
  quint8 oper;
};

class AQBitOperNode : public AQNode
{
public:
  AQBitOperNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSBitOperNode( QSNOD( expr1 ), ( Operator ) oper, QSNOD( expr2 ) );
  }
  AQBitOperNode( AQNode *e1, quint32 o, AQNode *e2 )
      : expr1( e1 ), expr2( e2 ), oper( o )
  { }
  NodeType nodeType() const
  {
    return aqBitOperNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr1, *expr2;
  quint8 oper;
};

class AQBinaryLogicalNode : public AQNode
{
public:
  AQBinaryLogicalNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSBinaryLogicalNode( QSNOD( expr1 ), ( Operator ) oper, QSNOD( expr2 ) );
  }
  AQBinaryLogicalNode( AQNode *e1, quint32 o, AQNode *e2 )
      : expr1( e1 ), expr2( e2 ), oper( o )
  { }
  NodeType nodeType() const
  {
    return aqBinaryLogicalNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr1, *expr2;
  quint8 oper;
};

class AQConditionalNode : public AQNode
{
public:
  AQConditionalNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSConditionalNode( QSNOD( logical ), QSNOD( expr1 ), QSNOD( expr2 ) );
  }
  AQConditionalNode( AQNode *l, AQNode *e1, AQNode *e2 )
      : logical( l ), expr1( e1 ), expr2( e2 )
  { }
  NodeType nodeType() const
  {
    return aqConditionalNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *logical, *expr1, *expr2;
};

class AQAssignNode : public AQNode
{
public:
  AQAssignNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSAssignNode( QSNOD( left ), ( Operator ) oper, QSNOD( expr ) );
  }
  AQAssignNode( AQNode *l, quint32 o, AQNode *e )
      : left( l ), oper( o ), expr( e )
  { }
  NodeType nodeType() const
  {
    return aqAssignNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *left;
  quint8 oper;
  AQNode *expr;
};

class AQCommaNode : public AQNode
{
public:
  AQCommaNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSCommaNode( QSNOD( expr1 ), QSNOD( expr2 ) );
  }
  AQCommaNode( AQNode *e1, AQNode *e2 ) : expr1( e1 ), expr2( e2 )
  { }
  NodeType nodeType() const
  {
    return aqCommaNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr1, *expr2;
};

class AQStatListNode : public AQStatementNode
{
public:
  AQStatListNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSStatListNode( QSNOD2( QSStatListNode , list ), QSNOD2( QSStatementNode, statement ) );
  }
  AQStatListNode( AQStatementNode *s ) : statement( s ), list( 0 )
  { }
  AQStatListNode( AQStatListNode *l, AQStatementNode *s )
      : statement( s ), list( l )
  { }
  NodeType nodeType() const
  {
    return aqStatListNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *statement;
  AQNode *list;
};

class AQAssignExprNode : public AQNode
{
public:
  AQAssignExprNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSAssignExprNode( QSNOD( expr ) );
  }
  AQAssignExprNode( AQNode *e ) : expr( e )
  { }
  NodeType nodeType() const
  {
    return aqAssignExprNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
};

class AQScopeNode : public AQStatementNode
{
public:
  AQScopeNode()
  {}
  virtual NodeType nodeType() const
  {
    return aqScopeNode;
  }
  virtual void serialize( QDataStream & out );
  virtual void unserialize( QDataStream & in );
  virtual void makeQSNode()
  {
    Q_ASSERT( 0 );
    qsNode_ = new QSThisNode();
  }
};

class AQBlockNode : public AQScopeNode
{
public:
  AQBlockNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSBlockNode( QSNOD2( QSStatListNode, statlist ) );
  }
  AQBlockNode( AQStatListNode *s ) : statlist( s )
  { }
  NodeType nodeType() const
  {
    return aqBlockNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *statlist;
};

class AQEmptyStatementNode : public AQStatementNode
{
public:
  AQEmptyStatementNode()
  {} // debug
  NodeType nodeType() const
  {
    return aqEmptyStatementNode;
  }
  void makeQSNode()
  {
    qsNode_ = new QSEmptyStatementNode();
  }
};

class AQExprStatementNode : public AQStatementNode
{
public:
  AQExprStatementNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSExprStatementNode( QSNOD( expr ) );
  }
  AQExprStatementNode( AQNode *e ) : expr( e )
  { }
  NodeType nodeType() const
  {
    return aqExprStatementNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
};

class AQIfNode : public AQStatementNode
{
public:
  AQIfNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSIfNode( QSNOD( expr ), QSNOD2( QSStatementNode, statement1 ),
                            QSNOD2( QSStatementNode, statement2 ) );
  }
  AQIfNode( AQNode *e, AQStatementNode *s1, AQStatementNode *s2 )
      : expr( e ), statement1( s1 ), statement2( s2 )
  { }
  NodeType nodeType() const
  {
    return aqIfNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
  AQNode *statement1, *statement2;
};

class AQDoWhileNode : public AQStatementNode
{
public:
  AQDoWhileNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSDoWhileNode( QSNOD2( QSStatementNode, statement ), QSNOD( expr ) );
  }
  AQDoWhileNode( AQStatementNode *s, AQNode *e )
      : statement( s ), expr( e )
  { }
  NodeType nodeType() const
  {
    return aqDoWhileNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *statement;
  AQNode *expr;
};

class AQWhileNode : public AQStatementNode
{
public:
  AQWhileNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSWhileNode( QSNOD( expr ), QSNOD2( QSStatementNode, statement ) );
  }
  AQWhileNode( AQNode *e, AQStatementNode *s )
      : expr( e ), statement( s )
  { }
  NodeType nodeType() const
  {
    return aqWhileNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
  AQNode *statement;
};

class AQForNode : public AQScopeNode
{
public:
  AQForNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSForNode( QSNOD( expr1 ), QSNOD( expr2 ), QSNOD( expr3 ),
                             QSNOD2( QSStatementNode, stat ) );
  }
  AQForNode( AQNode *e1, AQNode *e2, AQNode *e3, AQStatementNode *s ) :
      expr1( e1 ), expr2( e2 ), expr3( e3 ), stat( s )
  { }
  NodeType nodeType() const
  {
    return aqForNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr1, *expr2, *expr3;
  AQNode *stat;
};

class AQContinueNode : public AQStatementNode
{
public:
  AQContinueNode() : ident( 0 )
  {}
  void makeQSNode()
  {
    qsNode_ = new QSContinueNode( ident );
  }
  AQContinueNode( quint32 i )
      : ident( i )
  {}
  ~AQContinueNode()
  { }
  NodeType nodeType() const
  {
    return aqContinueNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 ident;
};

class AQBreakNode : public AQStatementNode
{
public:
  AQBreakNode() : ident( 0 )
  {}
  void makeQSNode()
  {
    qsNode_ = new QSBreakNode( ident );
  }
  AQBreakNode( quint32 i )
      : ident( i )
  {}
  ~AQBreakNode()
  { }
  NodeType nodeType() const
  {
    return aqBreakNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 ident;
};

class AQReturnNode : public AQStatementNode
{
public:
  AQReturnNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSReturnNode( QSNOD( value ) );
  }
  AQReturnNode( AQNode *v ) : value( v )
  { }
  NodeType nodeType() const
  {
    return aqReturnNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *value;
};

class AQWithNode : public AQScopeNode
{
public:
  AQWithNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSWithNode( QSNOD( expr ), QSNOD2( QSStatementNode, stat ) );
  }
  AQWithNode( AQNode *e, AQStatementNode *s ) : expr( e ), stat( s )
  { }
  NodeType nodeType() const
  {
    return aqWithNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
  AQNode *stat;
};

class AQCaseClauseNode: public AQNode
{
public:
  AQCaseClauseNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSCaseClauseNode( QSNOD( expr ), QSNOD2( QSStatListNode, list ) );
  }
  AQCaseClauseNode( AQNode *e, AQStatListNode *l ) : expr( e ), list( l )
  { }
  NodeType nodeType() const
  {
    return aqCaseClauseNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
  AQNode *list;
};

class AQClauseListNode : public AQNode
{
public:
  AQClauseListNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSClauseListNode( QSNOD2( QSCaseClauseNode, cl ), QSNOD2( QSClauseListNode, nx ) );
  }
  AQClauseListNode( AQCaseClauseNode *c ) : cl( c ), nx( 0 )
  { }
  AQClauseListNode( AQCaseClauseNode *c, AQClauseListNode *n ) : cl( c ), nx( n )
  { }
  AQClauseListNode * append( AQCaseClauseNode *c );
  AQClauseListNode * finish();
  NodeType nodeType() const
  {
    return aqClauseListNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *cl;
  AQNode *nx;
};

class AQCaseBlockNode: public AQNode
{
public:
  AQCaseBlockNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSCaseBlockNode( QSNOD2( QSClauseListNode, list1 ), QSNOD2( QSCaseClauseNode, def ),
                                   QSNOD2( QSClauseListNode, list2 ) );
  }
  AQCaseBlockNode( AQClauseListNode *l1, AQCaseClauseNode *d,
                   AQClauseListNode *l2 )
      : list1( l1 ), def( d ), list2( l2 )
  { }
  NodeType nodeType() const
  {
    return aqCaseBlockNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *list1;
  AQNode *def;
  AQNode *list2;
};

class AQSwitchNode : public AQStatementNode
{
public:
  AQSwitchNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSSwitchNode( QSNOD( expr ), QSNOD2( QSCaseBlockNode, block ) );
  }
  AQSwitchNode( AQNode *e, AQCaseBlockNode *b ) : expr( e ), block( b )
  { }
  NodeType nodeType() const
  {
    return aqSwitchNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
  AQNode *block;
};

class AQLabelNode : public AQStatementNode
{
public:
  AQLabelNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSLabelNode( label, QSNOD2( QSStatementNode, stat ) );
  }
  AQLabelNode( quint32 l, AQStatementNode *s )
      : label( l ), stat( s )
  {}
  ~AQLabelNode()
  {}
  NodeType nodeType() const
  {
    return aqLabelNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 label;
  AQNode *stat;
};

class AQThrowNode : public AQStatementNode
{
public:
  AQThrowNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSThrowNode( QSNOD( expr ) );
  }
  AQThrowNode( AQNode *e ) : expr( e )
  { }
  NodeType nodeType() const
  {
    return aqThrowNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *expr;
};

class AQCatchNode : public AQScopeNode
{
public:
  AQCatchNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSCatchNode( ident, QSNOD2( QSStatementNode, block ) );
  }
  AQCatchNode( quint32 i, AQStatementNode *b )
      : ident( i ), block( b )
  {}
  ~AQCatchNode()
  {}
  NodeType nodeType() const
  {
    return aqCatchNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 ident;
  AQNode *block;
};

class AQFinallyNode : public AQStatementNode
{
public:
  AQFinallyNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSFinallyNode( QSNOD2( QSStatementNode, block ) );
  }
  AQFinallyNode( AQStatementNode *b ) : block( b )
  { }
  NodeType nodeType() const
  {
    return aqFinallyNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *block;
};

class AQTryNode : public AQStatementNode
{
public:
  AQTryNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSTryNode( QSNOD2( QSStatementNode, block ),
                             QSNOD2( QSCatchNode, _catch ), QSNOD2( QSFinallyNode, _final ) );
  }
  AQTryNode( AQStatementNode *b, AQNode *c = 0, AQNode *f = 0 )
      : block( b ), _catch( ( AQCatchNode * ) c ),
      _final( ( AQFinallyNode * ) f )
  { }
  NodeType nodeType() const
  {
    return aqTryNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *block;
  AQNode *_catch;
  AQNode *_final;
};

class AQTypeNode : public AQNode
{
public:
  AQTypeNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSTypeNode( ident );
  }
  AQTypeNode( quint32 i )
      : ident( i )
  {}
  ~AQTypeNode()
  {}
  NodeType nodeType() const
  {
    return aqTypeNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 ident;
};

class AQParameterNode : public AQNode
{
public:
  AQParameterNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSParameterNode( id, QSNOD2( QSTypeNode, typ ), QSNOD2( QSParameterNode, next ) );
  }
  AQParameterNode( quint32 i, AQTypeNode *t )
      : id( i ), typ( t ), next( 0 )
  {}
  AQParameterNode( quint32 i, AQTypeNode *t, AQParameterNode *n )
      : id( i ), typ( t ), next( n )
  {}
  ~AQParameterNode()
  {}
  AQParameterNode * append( quint32 i, AQTypeNode *t );
  AQParameterNode * finish();
  NodeType nodeType() const
  {
    return aqParameterNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 id;
  AQNode *typ;
  AQNode *next;
};

// inherited by AQProgramNode
class AQFunctionBodyNode : public AQStatementNode
{
public:
  AQFunctionBodyNode( QSLexer * l ) : lx( l )
  { }
  virtual void makeQSNode()
  {
    qsNode_ = new QSFunctionBodyNode( QSNOD2( QSSourceElementsNode, source ), lx );
  }
  AQFunctionBodyNode( AQSourceElementsNode * s, QSLexer * l );
  virtual ~AQFunctionBodyNode()
  {}
  virtual NodeType nodeType() const
  {
    return aqFunctionBodyNode;
  }
  virtual void serialize( QDataStream & out );
  virtual void unserialize( QDataStream & in );
protected:
  AQNode * source;
  QSLexer * lx;
};

class AQFuncDeclNode : public AQStatementNode
{
public:
  AQFuncDeclNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSFuncDeclNode( ident, QSNOD2( QSParameterNode, param ), QSNOD2( QSTypeNode, rtype ),
                                  QSNOD2( QSFunctionBodyNode, body ), QSNOD2( QSAttributeNode, attrs ) );
  }
  AQFuncDeclNode( quint32 i, AQParameterNode *p, AQTypeNode *t, AQFunctionBodyNode *b )
      : ident( i ), param( p ), rtype( t ), body( b ), attrs( 0 )
  {}
  ~AQFuncDeclNode()
  {}
  void setAttributes( AQAttributeNode *a )
  {
    attrs = a;
  }
  NodeType nodeType() const
  {
    return aqFuncDeclNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 ident;
  AQNode *param;
  AQNode *rtype;
  AQNode *body;
  AQNode *attrs;
};

class AQFuncExprNode : public AQNode
{
public:
  AQFuncExprNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSFuncExprNode( QSNOD2( QSParameterNode, param ), QSNOD2( QSFunctionBodyNode, body ) );
  }
  AQFuncExprNode( AQParameterNode *p, AQFunctionBodyNode *b )
      : param( p ), body( b )
  { }
  NodeType nodeType() const
  {
    return aqFuncExprNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *param;
  AQNode *body;
};

class AQClassDefNode;

class AQSourceElementNode : public AQStatementNode
{
public:
  AQSourceElementNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSSourceElementNode( QSNOD2( QSStatementNode, statement ) );
  }
  AQSourceElementNode( AQStatementNode *s ) : statement( s )
  { }
  NodeType nodeType() const
  {
    return aqSourceElementNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *statement;
};

class AQSourceElementsNode : public AQStatementNode
{
public:
  AQSourceElementsNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSSourceElementsNode( QSNOD2( QSSourceElementsNode, elements ),
                                        QSNOD2( QSSourceElementNode, element ) );
  }
  AQSourceElementsNode( AQSourceElementNode *s1 ) :
      element( s1 ), elements( 0 )
  {}
  AQSourceElementsNode( AQSourceElementsNode *s1, AQSourceElementNode *s2 ) :
      element( s2 ), elements( s1 )
  {}
  NodeType nodeType() const
  {
    return aqSourceElementsNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *element;
  AQNode *elements;
};

class AQProgramNode : public AQFunctionBodyNode
{
public:
  AQProgramNode( QSLexer * l );
  AQProgramNode( AQSourceElementsNode *s, QSLexer * l );
  ~AQProgramNode();
  void makeQSNode()
  {
    qsNode_ = new QSProgramNode( QSNOD2( QSSourceElementsNode, source ), lx );
  }
  NodeType nodeType() const
  {
    return aqProgramNode;
  }
};

class AQClassDefNode : public AQStatementNode
{
public:
  AQClassDefNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSClassDefNode( ident, QSNOD2( QSTypeNode, type ),
                                  QSNOD2( QSFunctionBodyNode, body ), QSNOD2( QSAttributeNode, attrs ) );
  }
  AQClassDefNode( quint32 i, AQTypeNode *t, AQFunctionBodyNode *b )
      : ident( i ), type( t ), body( b ), attrs( 0 )
  {}
  ~AQClassDefNode()
  {}
  void setAttributes( AQAttributeNode *a )
  {
    attrs = a;
  }
  NodeType nodeType() const
  {
    return aqClassDefNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 ident;
  AQNode *type;
  AQNode *body;
  AQNode *attrs;
};

class AQTypedVarNode : public AQNode
{
public:
  AQTypedVarNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSTypedVarNode( ident, QSNOD2( QSTypeNode, type ) );
  }
  AQTypedVarNode( quint32 i, AQTypeNode *t )
      : ident( i ), type( t )
  {}
  ~AQTypedVarNode()
  {}
  NodeType nodeType() const
  {
    return aqTypedVarNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 ident;
  AQNode *type;
};

class AQVarBindingNode : public AQNode
{
public:
  AQVarBindingNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSVarBindingNode( QSNOD2( QSTypedVarNode, var ), QSNOD2( QSAssignNode, assign ) );
  }
  AQVarBindingNode( AQTypedVarNode *v, AQNode *a )
      : var( v ), assign( ( AQAssignNode * ) a )
  { }
  NodeType nodeType() const
  {
    return aqVarBindingNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *var;
  AQNode *assign;
};

class AQForInNode : public AQScopeNode
{
public:
  AQForInNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSForInNode( QSNOD2( QSVarBindingNode, var ), QSNOD( lexpr ),
                               QSNOD( expr ), QSNOD2( QSStatementNode, stat ) );
  }
  AQForInNode( AQNode *l, AQNode *e, AQStatementNode *s )
      : var( 0 ), lexpr( l ), expr( e ), stat( s )
  { }
  AQForInNode( AQVarBindingNode *v,
               AQNode *e, AQStatementNode *s )
      : var( v ), lexpr( 0 ), expr( e ), stat( s )
  { }
  NodeType nodeType() const
  {
    return aqForInNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *var;
  AQNode *lexpr, *expr;
  AQNode *stat;
};

class AQVarBindingListNode : public AQNode
{
public:
  AQVarBindingListNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSVarBindingListNode( QSNOD2( QSVarBindingListNode, list ),
                                        QSNOD2( QSVarBindingNode, binding ) );
  }
  AQVarBindingListNode( AQVarBindingListNode *l, AQVarBindingNode *b )
      : list( l ), binding( b )
  { }
  NodeType nodeType() const
  {
    return aqVarBindingListNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  AQNode *list;
  AQNode *binding;
};

class AQVarDefNode : public AQStatementNode
{
public:
  AQVarDefNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSVarDefNode( kind, QSNOD2( QSVarBindingListNode, list ),
                                QSNOD2( QSAttributeNode, attrs ) );
  }
  AQVarDefNode( int k, AQVarBindingListNode *l )
      : kind( k ), list( l ), attrs( 0 )
  { }
  void setAttributes( AQAttributeNode *a )
  {
    attrs = a;
  }
  NodeType nodeType() const
  {
    return aqVarDefNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 kind;
  AQNode *list;
  AQNode *attrs;
};

class AQPackageNode : public AQStatementNode
{
public:
  AQPackageNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSPackageNode( package, QSNOD2( QSStatementNode, block ) );
  }
  AQPackageNode( quint32 s, AQStatementNode *b )
      : package( s ), block( b )
  {}
  ~AQPackageNode()
  {}
  NodeType nodeType() const
  {
    return aqPackageNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 package;
  AQNode *block;
};

class AQImportNode : public AQStatementNode
{
public:
  AQImportNode()
  { }
  void makeQSNode()
  {
    qsNode_ = new QSImportNode( package );
  }
  AQImportNode( quint32 s )
      : package( s )
  {}
  ~AQImportNode()
  {}
  NodeType nodeType() const
  {
    return aqImportNode;
  }
  void serialize( QDataStream & out );
  void unserialize( QDataStream & in );
private:
  quint32 package;
};

#endif /*AQNODESQSA_H_*/
