/***************************************************************************
AQNodesQSA.cpp
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

#include <AQCore/AQCoreUtil.h>

#include "AQNodesQSA.h"

#define N_U(H)    AQNode::newUstr(H)
#define MK_NODE   if ( !qsNode_ ) this->makeQSNode()
#define OUT_IDHEAD \
  if ( !headOut_ ) { \
   quint8 node_Type = this->nodeType(); \
   quint32 node_Id = this->nodeId(); \
   out << node_Type << node_Id; \
   headOut_ = true; \
 } \

QMap<uint, AQNode *> AQNode::nodesMap;
QMultiHash<quint32, quint32> AQNode::labelsHash;
QMultiHash<quint32, quint16> AQNode::attrsHash;
QHash<uint, QString> AQNode::ustrHash;

uint AQNode::nodesCount = 0;
bool AQNode::autoInsertMap = true;
AQNode * AQNode::lastNode_ = 0;

AQNode::AQNode() :
    line( -1 ),
    qsNode_( 0 ),
    headOut_( false )
{
  if ( autoInsertMap ) {
    nodeId_ = ++nodesCount;
    nodesMap.insert( nodeId_, this );
  }
}

void AQNode::serialize( QDataStream & out )
{
  OUT_IDHEAD;
}

void AQNode::unserialize( QDataStream & )
{
  MK_NODE;
}

void AQNode::serializeNodes( QDataStream & out )
{
  out << labelsHash;
  out << attrsHash;
  for ( QMap<uint, AQNode *>::const_iterator it = nodesMap.begin(); it != nodesMap.end(); ++it )
    ( *it ) ->serialize( out );
  quint8 temp = aqNone;
  out << temp;
  out << ustrHash;
}

void AQNode::unserializeNodes( QDataStream & in, QSLexer * l )
{
  autoInsertMap = false;

  in >> labelsHash;
  in >> attrsHash;

  quint8 nType = aqNone;
  quint32 nId = 0;
  AQNode * node = 0;

  while ( true ) {
    in >> nType;
    if ( nType == aqNone )
      break;
    in >> nId;

    node = createNode( ( NodeType ) nType, l );
    if ( node ) {
      node->setNodeId( nId );
      nodesMap.insert( nId, node );
      node->unserialize( in );
    }

    if ( in.status() != QDataStream::Ok ) {
      l->setErrorMessage( AQ_TR( "ScriptEngineQSA", "Error interpretando ByteCode, flujo de datos fuera de secuencia" ) );
      l->setState( QSLexer::Bad );
      return ;
    }
  }

  QHash<uint, QString> & h = l->ustringHash();
  uint k;
  QString t;
  quint32 n;
  in >> n;

  for ( quint32 i = 0; i < n; ++i ) {
    if ( in.status() != QDataStream::Ok ) {
      l->setErrorMessage( AQ_TR( "ScriptEngineQSA", "Error interpretando ByteCode, flujo de datos fuera de secuencia" ) );
      l->setState( QSLexer::Bad );
      return ;
    }
    in >> k >> t;
    if ( !h.contains( k ) ) {
      h.insert( k, t );
      if ( t.length() < 64 )
        REF_USTR( k );
    }
  }

  autoInsertMap = true;
}

void AQNode::clear()
{
  while ( !nodesMap.isEmpty() ) {
    AQNode * n = nodesMap.begin().value();
    nodesMap.erase( nodesMap.begin() );
    if ( n )
      delete n;
  }
  ustrHash.clear();
  labelsHash.clear();
  attrsHash.clear();
  nodesCount = 0;
  autoInsertMap = true;
  lastNode_ = 0;
}

uint AQNode::newUstr( uint hash )
{
  if ( !ustrHash.contains( hash ) )
    ustrHash.insert( hash, USTR( hash ) );
  return hash;
}

#ifdef QSDEBUGGER
void AQStatementNode::setLoc( int line0, int line1, QSLexer * l )
{
  l0 = line0;
  l1 = line1;
  if ( l ) {
    sid = l->sourceId();
    setLineNo( l->lineNo() );
  }
}
#endif

void AQStatementNode::pushLabel( quint32 id ) const
{
  AQNode::labelsHash.insert( this->nodeId(), id );
}

void AQStatementNode::serialize( QDataStream & out )
{
  OUT_IDHEAD;
  AQNode::serialize( out );
}

void AQStatementNode::unserialize( QDataStream & in )
{
  MK_NODE;
  QList<quint32> list( AQNode::labelsHash.values( this->nodeId() ) );
  for ( int i = list.size() - 1; i >= 0; --i )
    static_cast<QSStatementNode *>( qsNode_ ) ->pushLabel( list.at( i ) );
  AQNode::unserialize( in );
}

void AQScopeNode::serialize( QDataStream & out )
{
  OUT_IDHEAD;
  AQStatementNode::serialize( out );
}

void AQScopeNode::unserialize( QDataStream & in )
{
  MK_NODE;
  AQStatementNode::unserialize( in );
}

void AQAttributeNode::add( QSAttribute a ) const
{
  AQNode::attrsHash.insert( this->nodeId(), a );
}

void AQAttributeNode::serialize( QDataStream & out )
{
  OUT_IDHEAD;
  AQNode::serialize( out );
}

void AQAttributeNode::unserialize( QDataStream & in )
{
  MK_NODE;
  QList<quint16> list( AQNode::attrsHash.values( this->nodeId() ) );
  for ( int i = list.size() - 1; i >= 0; --i )
    static_cast<QSAttributeNode *>( qsNode_ ) ->add( ( QSAttribute ) list.at( i ) );
  AQNode::unserialize( in );
}

void AQPropertyNode::serialize( QDataStream & out )
{
  OUT_IDHEAD;
  out << ( double ) numeric << ( quint32 ) N_U( str );
  AQNode::serialize( out );
}

void AQPropertyNode::unserialize( QDataStream & in )
{
  in >> numeric >> str;
  MK_NODE;
  AQNode::unserialize( in );
}

AQParameterNode * AQParameterNode::append( uint i, AQTypeNode *t )
{
  AQParameterNode * p = this;
  while ( p->next )
    p = static_cast<AQParameterNode *>( p->next );
  p->next = new AQParameterNode( i, t );
  return this;
}

AQParameterNode * AQParameterNode::finish()
{
  if ( next )
    static_cast<AQParameterNode *>( next ) ->finish();
  if ( typ ) {
    AQNode::nodesMap.remove( typ->nodeId() );
    typ->setNodeId( ++AQNode::nodesCount );
    AQNode::nodesMap.insert( typ->nodeId(), typ );
  }
  AQNode::nodesMap.remove( this->nodeId() );
  this->setNodeId( ++AQNode::nodesCount );
  AQNode::nodesMap.insert( this->nodeId(), this );
  return this;
}

AQClauseListNode * AQClauseListNode::append( AQCaseClauseNode *c )
{
  AQClauseListNode * l = this;
  while ( l->nx )
    l = static_cast<AQClauseListNode *>( l->nx );
  l->nx = new AQClauseListNode( c );
  return this;
}

AQClauseListNode * AQClauseListNode::finish()
{
  if ( nx )
    static_cast<AQClauseListNode *>( nx ) ->finish();
  if ( cl ) {
    AQNode::nodesMap.remove( cl->nodeId() );
    cl->setNodeId( ++AQNode::nodesCount );
    AQNode::nodesMap.insert( cl->nodeId(), cl );
  }
  AQNode::nodesMap.remove( this->nodeId() );
  this->setNodeId( ++AQNode::nodesCount );
  AQNode::nodesMap.insert( this->nodeId(), this );
  return this;
}

AQFunctionBodyNode::AQFunctionBodyNode( AQSourceElementsNode * s, QSLexer * l ) :
    source( s ),
    lx( l )
{
#ifdef QSDEBUGGER
  setLoc( -1, -1, l );
#endif
}

AQProgramNode::AQProgramNode( QSLexer * l ) :
    AQFunctionBodyNode( l )
{
  AQNode::lastNode_ = this;
}

AQProgramNode::AQProgramNode( AQSourceElementsNode *s, QSLexer * l ) :
    AQFunctionBodyNode( s, l )
{
  AQNode::lastNode_ = this;
}

AQProgramNode::~AQProgramNode()
{
  if ( AQNode::lastNode_ == this )
    AQNode::lastNode_ = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#define AQNODE_SER_IMPL_1(base, name, x) \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << x; \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      in >> x; \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_1U(base, name, u1) \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) N_U( u1 ); \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      in >> u1; \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_2U(base, name, u1, u2) \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) N_U( u1 ); \
     out << ( quint32 ) N_U( u2 ); \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      in >> u1; \
      in >> u2; \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_1N(base, name, node1) \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) ( node1 ? node1->nodeId() : 0 ); \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      quint32 nId = 0; \
      in >> nId; node1 = nodesMap.value( nId ); \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_1N_1(base, name, node1, x) \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) ( node1 ? node1->nodeId() : 0 ); \
     out << x; \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      quint32 nId = 0; \
      in >> nId; node1 = nodesMap.value( nId ); \
      in >> x; \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_1N_1U(base, name, node1, u1) \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) ( node1 ? node1->nodeId() : 0 ); \
     out << ( quint32 ) N_U( u1 ); \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      quint32 nId = 0; \
      in >> nId; node1 = nodesMap.value( nId ); \
      in >> u1; \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_2N(base, name, node1, node2)   \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) ( node1 ? node1->nodeId() : 0 ); \
     out << ( quint32 ) ( node2 ? node2->nodeId() : 0 ); \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      quint32 nId = 0; \
      in >> nId; node1 = nodesMap.value( nId ); \
      in >> nId; node2 = nodesMap.value( nId ); \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_2N_1(base, name, node1, node2, x)   \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) ( node1 ? node1->nodeId() : 0 ); \
     out << ( quint32 ) ( node2 ? node2->nodeId() : 0 ); \
     out << x; \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      quint32 nId = 0; \
      in >> nId; node1 = nodesMap.value( nId ); \
      in >> nId; node2 = nodesMap.value( nId ); \
      in >> x; \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_2N_1U(base, name, node1, node2, u1)   \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) ( node1 ? node1->nodeId() : 0 ); \
     out << ( quint32 ) ( node2 ? node2->nodeId() : 0 ); \
     out << ( quint32 ) N_U( u1 ); \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      quint32 nId = 0; \
      in >> nId; node1 = nodesMap.value( nId ); \
      in >> nId; node2 = nodesMap.value( nId ); \
      in >> u1; \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_3N(base, name, node1, node2, node3) \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) ( node1 ? node1->nodeId() : 0 ); \
     out << ( quint32 ) ( node2 ? node2->nodeId() : 0 ); \
     out << ( quint32 ) ( node3 ? node3->nodeId() : 0 ); \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      quint32 nId = 0; \
      in >> nId; node1 = nodesMap.value( nId ); \
      in >> nId; node2 = nodesMap.value( nId ); \
      in >> nId; node3 = nodesMap.value( nId ); \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_3N_1U(base, name, node1, node2, node3, u1) \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) ( node1 ? node1->nodeId() : 0 ); \
     out << ( quint32 ) ( node2 ? node2->nodeId() : 0 ); \
     out << ( quint32 ) ( node3 ? node3->nodeId() : 0 ); \
     out << ( quint32 ) N_U( u1 ); \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      quint32 nId = 0; \
      in >> nId; node1 = nodesMap.value( nId ); \
      in >> nId; node2 = nodesMap.value( nId ); \
      in >> nId; node3 = nodesMap.value( nId ); \
      in >> u1; \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_4N(base, name, node1, node2, node3, node4) \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) ( node1 ? node1->nodeId() : 0 ); \
     out << ( quint32 ) ( node2 ? node2->nodeId() : 0 ); \
     out << ( quint32 ) ( node3 ? node3->nodeId() : 0 ); \
     out << ( quint32 ) ( node4 ? node4->nodeId() : 0 ); \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      quint32 nId = 0; \
      in >> nId; node1 = nodesMap.value( nId ); \
      in >> nId; node2 = nodesMap.value( nId ); \
      in >> nId; node3 = nodesMap.value( nId ); \
      in >> nId; node4 = nodesMap.value( nId ); \
      MK_NODE; \
      base::unserialize( in ); \
    } \

#define AQNODE_SER_IMPL_4N_1U(base, name, node1, node2, node3, node4, u1) \
    void name::serialize( QDataStream & out ) { \
     OUT_IDHEAD; \
     out << ( quint32 ) ( node1 ? node1->nodeId() : 0 ); \
     out << ( quint32 ) ( node2 ? node2->nodeId() : 0 ); \
     out << ( quint32 ) ( node3 ? node3->nodeId() : 0 ); \
     out << ( quint32 ) ( node4 ? node4->nodeId() : 0 ); \
     out << ( quint32 ) N_U( u1 ); \
     base::serialize( out ); \
    } \
    void name::unserialize( QDataStream & in ) { \
      quint32 nId = 0; \
      in >> nId; node1 = nodesMap.value( nId ); \
      in >> nId; node2 = nodesMap.value( nId ); \
      in >> nId; node3 = nodesMap.value( nId ); \
      in >> nId; node4 = nodesMap.value( nId ); \
      in >> u1; \
      MK_NODE; \
      base::unserialize( in ); \
    } \

AQNODE_SER_IMPL_1( AQNode, AQBooleanNode, value )
AQNODE_SER_IMPL_1( AQNode, AQNumberNode, value )
AQNODE_SER_IMPL_1U( AQNode, AQStringNode, value )
AQNODE_SER_IMPL_2U( AQNode, AQRegExpNode, pattern, flags )
AQNODE_SER_IMPL_1U( AQNode, AQResolveNode, ident )
AQNODE_SER_IMPL_1N( AQNode, AQGroupNode, group )
AQNODE_SER_IMPL_1N( AQNode, AQElisionNode, elision )
AQNODE_SER_IMPL_3N( AQNode, AQElementNode, list, elision, node )
AQNODE_SER_IMPL_2N_1( AQNode, AQArrayNode, element, elision, opt )
AQNODE_SER_IMPL_1N( AQNode, AQObjectLiteralNode, list )
AQNODE_SER_IMPL_3N( AQNode, AQPropertyValueNode, name, assign, list )
AQNODE_SER_IMPL_2N( AQNode, AQAccessorNode1, expr1, expr2 )
AQNODE_SER_IMPL_1N_1U( AQNode, AQAccessorNode2, expr, ident )
AQNODE_SER_IMPL_2N( AQNode, AQArgumentListNode, list, expr )
AQNODE_SER_IMPL_1N( AQNode, AQArgumentsNode, list )
AQNODE_SER_IMPL_2N( AQNode, AQNewExprNode, expr, args )
AQNODE_SER_IMPL_2N( AQNode, AQFunctionCallNode, expr, args )
AQNODE_SER_IMPL_2N( AQNode, AQEmitNode, expr, args )
AQNODE_SER_IMPL_1N_1( AQNode, AQPostfixNode, expr, oper )
AQNODE_SER_IMPL_1N( AQNode, AQDeleteNode, expr )
AQNODE_SER_IMPL_1N( AQNode, AQVoidNode, expr )
AQNODE_SER_IMPL_1N( AQNode, AQTypeOfNode, expr )
AQNODE_SER_IMPL_1N_1( AQNode, AQPrefixNode, expr, oper )
AQNODE_SER_IMPL_1N( AQNode, AQUnaryPlusNode, expr )
AQNODE_SER_IMPL_1N( AQNode, AQNegateNode, expr )
AQNODE_SER_IMPL_1N( AQNode, AQBitwiseNotNode, expr )
AQNODE_SER_IMPL_1N( AQNode, AQLogicalNotNode, expr )
AQNODE_SER_IMPL_2N_1( AQNode, AQMultNode, term1, term2, oper )
AQNODE_SER_IMPL_2N_1( AQNode, AQAddNode, term1, term2, oper )
AQNODE_SER_IMPL_2N_1( AQNode, AQShiftNode, term1, term2, oper )
AQNODE_SER_IMPL_2N_1( AQNode, AQRelationalNode, expr1, expr2, oper )
AQNODE_SER_IMPL_2N_1( AQNode, AQEqualNode, expr1, expr2, oper )
AQNODE_SER_IMPL_2N_1( AQNode, AQBitOperNode, expr1, expr2, oper )
AQNODE_SER_IMPL_2N_1( AQNode, AQBinaryLogicalNode, expr1, expr2, oper )
AQNODE_SER_IMPL_3N( AQNode, AQConditionalNode, logical, expr1, expr2 )
AQNODE_SER_IMPL_2N_1( AQNode, AQAssignNode, left, expr, oper )
AQNODE_SER_IMPL_2N( AQNode, AQCommaNode, expr1, expr2 )
AQNODE_SER_IMPL_2N( AQNode, AQStatListNode, statement, list )
AQNODE_SER_IMPL_1N( AQNode, AQAssignExprNode, expr )
AQNODE_SER_IMPL_1N( AQScopeNode, AQBlockNode, statlist )
AQNODE_SER_IMPL_1N( AQStatementNode, AQExprStatementNode, expr )
AQNODE_SER_IMPL_3N( AQStatementNode, AQIfNode, expr, statement1, statement2 )
AQNODE_SER_IMPL_2N( AQStatementNode, AQDoWhileNode, statement, expr )
AQNODE_SER_IMPL_2N( AQStatementNode, AQWhileNode, statement, expr )
AQNODE_SER_IMPL_4N( AQScopeNode, AQForNode, expr1, expr2, expr3, stat )
AQNODE_SER_IMPL_4N( AQScopeNode, AQForInNode, var, lexpr, expr, stat )
AQNODE_SER_IMPL_1U( AQStatementNode, AQContinueNode, ident )
AQNODE_SER_IMPL_1U( AQStatementNode, AQBreakNode, ident )
AQNODE_SER_IMPL_1N( AQStatementNode, AQReturnNode, value )
AQNODE_SER_IMPL_2N( AQScopeNode, AQWithNode, expr, stat )
AQNODE_SER_IMPL_2N( AQNode, AQCaseClauseNode, expr, list )
AQNODE_SER_IMPL_2N( AQNode, AQClauseListNode, cl, nx )
AQNODE_SER_IMPL_3N( AQNode, AQCaseBlockNode, list1, def, list2 )
AQNODE_SER_IMPL_2N( AQStatementNode, AQSwitchNode, expr, block )
AQNODE_SER_IMPL_1N_1U( AQStatementNode, AQLabelNode, stat, label )
AQNODE_SER_IMPL_1N( AQStatementNode, AQThrowNode, expr )
AQNODE_SER_IMPL_1N_1U( AQScopeNode, AQCatchNode, block, ident )
AQNODE_SER_IMPL_1N( AQStatementNode, AQFinallyNode, block )
AQNODE_SER_IMPL_3N( AQStatementNode, AQTryNode, block, _catch, _final )
AQNODE_SER_IMPL_2N_1U( AQNode, AQParameterNode, typ, next, id )
AQNODE_SER_IMPL_1N( AQStatementNode, AQFunctionBodyNode, source )
AQNODE_SER_IMPL_4N_1U( AQStatementNode, AQFuncDeclNode, param, rtype, body, attrs, ident )
AQNODE_SER_IMPL_2N( AQNode, AQFuncExprNode, param, body )
AQNODE_SER_IMPL_1N( AQStatementNode, AQSourceElementNode, statement )
AQNODE_SER_IMPL_2N( AQStatementNode, AQSourceElementsNode, element, elements )
AQNODE_SER_IMPL_3N_1U( AQStatementNode, AQClassDefNode, type, body, attrs, ident )
AQNODE_SER_IMPL_1U( AQNode, AQTypeNode, ident )
AQNODE_SER_IMPL_1N_1U( AQNode, AQTypedVarNode, type, ident )
AQNODE_SER_IMPL_2N( AQNode, AQVarBindingNode, var, assign )
AQNODE_SER_IMPL_2N( AQNode, AQVarBindingListNode, list, binding )
AQNODE_SER_IMPL_2N_1( AQStatementNode, AQVarDefNode, list, attrs, kind )
AQNODE_SER_IMPL_1N_1U( AQStatementNode, AQPackageNode, block, package )
AQNODE_SER_IMPL_1U( AQStatementNode, AQImportNode, package )

AQNode * AQNode::createNode( NodeType n, QSLexer * l )
{
  switch ( n ) {
  case aqNode:
    return new AQNode();
    break;
  case aqStatementNode:
    return new AQStatementNode();
    break;
  case aqAttributeNode:
    return new AQAttributeNode();
    break;
  case aqNullNode:
    return new AQNullNode();
    break;
  case aqBooleanNode:
    return new AQBooleanNode();
    break;
  case aqNumberNode:
    return new AQNumberNode();
    break;
  case aqStringNode:
    return new AQStringNode();
    break;
  case aqRegExpNode:
    return new AQRegExpNode();
    break;
  case aqThisNode:
    return new AQThisNode();
    break;
  case aqResolveNode:
    return new AQResolveNode();
    break;
  case aqGroupNode:
    return new AQGroupNode();
    break;
  case aqElisionNode:
    return new AQElisionNode();
    break;
  case aqElementNode:
    return new AQElementNode();
    break;
  case aqArrayNode:
    return new AQArrayNode();
    break;
  case aqObjectLiteralNode:
    return new AQObjectLiteralNode();
    break;
  case aqPropertyValueNode:
    return new AQPropertyValueNode();
    break;
  case aqPropertyNode:
    return new AQPropertyNode();
    break;
  case aqAccessorNode1:
    return new AQAccessorNode1();
    break;
  case aqAccessorNode2:
    return new AQAccessorNode2();
    break;
  case aqArgumentListNode:
    return new AQArgumentListNode();
    break;
  case aqArgumentsNode:
    return new AQArgumentsNode();
    break;
  case aqNewExprNode:
    return new AQNewExprNode();
    break;
  case aqFunctionCallNode:
    return new AQFunctionCallNode();
    break;
  case aqEmitNode:
    return new AQEmitNode();
    break;
  case aqPostfixNode:
    return new AQPostfixNode();
    break;
  case aqDeleteNode:
    return new AQDeleteNode();
    break;
  case aqVoidNode:
    return new AQVoidNode();
    break;
  case aqTypeOfNode:
    return new AQTypeOfNode();
    break;
  case aqPrefixNode:
    return new AQPrefixNode();
    break;
  case aqUnaryPlusNode:
    return new AQUnaryPlusNode();
    break;
  case aqNegateNode:
    return new AQNegateNode();
    break;
  case aqBitwiseNotNode:
    return new AQBitwiseNotNode();
    break;
  case aqLogicalNotNode:
    return new AQLogicalNotNode();
    break;
  case aqMultNode:
    return new AQMultNode();
    break;
  case aqAddNode:
    return new AQAddNode();
    break;
  case aqShiftNode:
    return new AQShiftNode();
    break;
  case aqRelationalNode:
    return new AQRelationalNode();
    break;
  case aqEqualNode:
    return new AQEqualNode();
    break;
  case aqBitOperNode:
    return new AQBitOperNode();
    break;
  case aqBinaryLogicalNode:
    return new AQBinaryLogicalNode();
    break;
  case aqConditionalNode:
    return new AQConditionalNode();
    break;
  case aqAssignNode:
    return new AQAssignNode();
    break;
  case aqCommaNode:
    return new AQCommaNode();
    break;
  case aqStatListNode:
    return new AQStatListNode();
    break;
  case aqAssignExprNode:
    return new AQAssignExprNode();
    break;
  case aqScopeNode:
    return new AQScopeNode();
    break;
  case aqBlockNode:
    return new AQBlockNode();
    break;
  case aqEmptyStatementNode:
    return new AQEmptyStatementNode();
    break;
  case aqExprStatementNode:
    return new AQExprStatementNode();
    break;
  case aqIfNode:
    return new AQIfNode();
    break;
  case aqDoWhileNode:
    return new AQDoWhileNode();
    break;
  case aqWhileNode:
    return new AQWhileNode();
    break;
  case aqForNode:
    return new AQForNode();
    break;
  case aqForInNode:
    return new AQForInNode();
    break;
  case aqContinueNode:
    return new AQContinueNode();
    break;
  case aqBreakNode:
    return new AQBreakNode();
    break;
  case aqReturnNode:
    return new AQReturnNode();
    break;
  case aqWithNode:
    return new AQWithNode();
    break;
  case aqCaseClauseNode:
    return new AQCaseClauseNode();
    break;
  case aqClauseListNode:
    return new AQClauseListNode();
    break;
  case aqCaseBlockNode:
    return new AQCaseBlockNode();
    break;
  case aqSwitchNode:
    return new AQSwitchNode();
    break;
  case aqLabelNode:
    return new AQLabelNode();
    break;
  case aqThrowNode:
    return new AQThrowNode();
    break;
  case aqCatchNode:
    return new AQCatchNode();
    break;
  case aqFinallyNode:
    return new AQFinallyNode();
    break;
  case aqTryNode:
    return new AQTryNode();
    break;
  case aqParameterNode:
    return new AQParameterNode();
    break;
  case aqFunctionBodyNode:
    return new AQFunctionBodyNode( l );
    break;
  case aqFuncDeclNode:
    return new AQFuncDeclNode();
    break;
  case aqFuncExprNode:
    return new AQFuncExprNode();
    break;
  case aqSourceElementNode:
    return new AQSourceElementNode();
    break;
  case aqSourceElementsNode:
    return new AQSourceElementsNode();
    break;
  case aqProgramNode:
    return new AQProgramNode( l );
    break;
  case aqClassDefNode:
    return new AQClassDefNode();
    break;
  case aqTypeNode:
    return new AQTypeNode();
    break;
  case aqTypedVarNode:
    return new AQTypedVarNode();
    break;
  case aqVarBindingNode:
    return new AQVarBindingNode();
    break;
  case aqVarBindingListNode:
    return new AQVarBindingListNode();
    break;
  case aqVarDefNode:
    return new AQVarDefNode();
    break;
  case aqPackageNode:
    return new AQPackageNode();
    break;
  case aqImportNode:
    return new AQImportNode();
    break;
  default:
    return 0;
  }
  return 0;
}

QString AQNode::nodeTypeToString( NodeType n )
{
  switch ( n ) {
  case aqNode:
    return "AQNode";
    break;
  case aqStatementNode:
    return "AQStatementNode";
    break;
  case aqAttributeNode:
    return "AQAttributeNode";
    break;
  case aqNullNode:
    return "AQNullNode";
    break;
  case aqBooleanNode:
    return "AQBooleanNode";
    break;
  case aqNumberNode:
    return "AQNumberNode";
    break;
  case aqStringNode:
    return "AQStringNode";
    break;
  case aqRegExpNode:
    return "AQRegExpNode";
    break;
  case aqThisNode:
    return "AQThisNode";
    break;
  case aqResolveNode:
    return "AQResolveNode";
    break;
  case aqGroupNode:
    return "AQGroupNode";
    break;
  case aqElisionNode:
    return "AQElisionNode";
    break;
  case aqElementNode:
    return "AQElementNode";
    break;
  case aqArrayNode:
    return "AQArrayNode";
    break;
  case aqObjectLiteralNode:
    return "AQObjectLiteralNode";
    break;
  case aqPropertyValueNode:
    return "AQPropertyValueNode";
    break;
  case aqPropertyNode:
    return "AQPropertyNode";
    break;
  case aqAccessorNode1:
    return "AQAccessorNode1";
    break;
  case aqAccessorNode2:
    return "AQAccessorNode2";
    break;
  case aqArgumentListNode:
    return "AQArgumentListNode";
    break;
  case aqArgumentsNode:
    return "AQArgumentsNode";
    break;
  case aqNewExprNode:
    return "AQNewExprNode";
    break;
  case aqFunctionCallNode:
    return "AQFunctionCallNode";
    break;
  case aqEmitNode:
    return "AQEmitNode";
    break;
  case aqPostfixNode:
    return "AQPostfixNode";
    break;
  case aqDeleteNode:
    return "AQDeleteNode";
    break;
  case aqVoidNode:
    return "AQVoidNode";
    break;
  case aqTypeOfNode:
    return "AQTypeOfNode";
    break;
  case aqPrefixNode:
    return "AQPrefixNode";
    break;
  case aqUnaryPlusNode:
    return "AQUnaryPlusNode";
    break;
  case aqNegateNode:
    return "AQNegateNode";
    break;
  case aqBitwiseNotNode:
    return "AQBitwiseNotNode";
    break;
  case aqLogicalNotNode:
    return "AQLogicalNotNode";
    break;
  case aqMultNode:
    return "AQMultNode";
    break;
  case aqAddNode:
    return "AQAddNode";
    break;
  case aqShiftNode:
    return "AQShiftNode";
    break;
  case aqRelationalNode:
    return "AQRelationalNode";
    break;
  case aqEqualNode:
    return "AQEqualNode";
    break;
  case aqBitOperNode:
    return "AQBitOperNode";
    break;
  case aqBinaryLogicalNode:
    return "AQBinaryLogicalNode";
    break;
  case aqConditionalNode:
    return "AQConditionalNode";
    break;
  case aqAssignNode:
    return "AQAssignNode";
    break;
  case aqCommaNode:
    return "AQCommaNode";
    break;
  case aqStatListNode:
    return "AQStatListNode";
    break;
  case aqAssignExprNode:
    return "AQAssignExprNode";
    break;
  case aqScopeNode:
    return "AQScopeNode";
    break;
  case aqBlockNode:
    return "AQBlockNode";
    break;
  case aqEmptyStatementNode:
    return "AQEmptyStatementNode";
    break;
  case aqExprStatementNode:
    return "AQExprStatementNode";
    break;
  case aqIfNode:
    return "AQIfNode";
    break;
  case aqDoWhileNode:
    return "AQDoWhileNode";
    break;
  case aqWhileNode:
    return "AQWhileNode";
    break;
  case aqForNode:
    return "AQForNode";
    break;
  case aqForInNode:
    return "AQForInNode";
    break;
  case aqContinueNode:
    return "AQContinueNode";
    break;
  case aqBreakNode:
    return "AQBreakNode";
    break;
  case aqReturnNode:
    return "AQReturnNode";
    break;
  case aqWithNode:
    return "AQWithNode";
    break;
  case aqCaseClauseNode:
    return "AQCaseClauseNode";
    break;
  case aqClauseListNode:
    return "AQClauseListNode";
    break;
  case aqCaseBlockNode:
    return "AQCaseBlockNode";
    break;
  case aqSwitchNode:
    return "AQSwitchNode";
    break;
  case aqLabelNode:
    return "AQLabelNode";
    break;
  case aqThrowNode:
    return "AQThrowNode";
    break;
  case aqCatchNode:
    return "AQCatchNode";
    break;
  case aqFinallyNode:
    return "AQFinallyNode";
    break;
  case aqTryNode:
    return "AQTryNode";
    break;
  case aqParameterNode:
    return "AQParameterNode";
    break;
  case aqFunctionBodyNode:
    return "AQFunctionBodyNode";
    break;
  case aqFuncDeclNode:
    return "AQFuncDeclNode";
    break;
  case aqFuncExprNode:
    return "AQFuncExprNode";
    break;
  case aqSourceElementNode:
    return "AQSourceElementNode";
    break;
  case aqSourceElementsNode:
    return "AQSourceElementsNode";
    break;
  case aqProgramNode:
    return "AQProgramNode";
    break;
  case aqClassDefNode:
    return "AQClassDefNode";
    break;
  case aqTypeNode:
    return "AQTypeNode";
    break;
  case aqTypedVarNode:
    return "AQTypedVarNode";
    break;
  case aqVarBindingNode:
    return "AQVarBindingNode";
    break;
  case aqVarBindingListNode:
    return "AQVarBindingListNode";
    break;
  case aqVarDefNode:
    return "AQVarDefNode";
    break;
  case aqPackageNode:
    return "AQPackageNode";
    break;
  case aqImportNode:
    return "AQImportNode";
    break;
  default:
    return QString();
  }
  return QString();
}
