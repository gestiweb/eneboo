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

#include "qscheck.h"
#include "qsnodes.h"
#include "qsenv.h"
#include "qslexer.h"

// #define QSDEBUG_CHECK

#ifdef QSDEBUG_CHECK
#include <typeinfo>
#endif

QSCheckData::QSCheckData( QSEnv *e, QSGlobalClass *g )
    : currenv( e ),
    lattrs( AttributeNone ),
    ltype( 0 ),
    vbCount( 0 ),
    switchLevel( 0 ),
    noGlobStatements( false ),
    directLookup( true )
{
  if ( g )
    scopeStack.push_front( g );
}

QSCheckData::~QSCheckData()
{
  // sanity checks
  Q_ASSERT( lablist.isEmpty() );
}

void QSCheckData::enterClass( QSClass *c )
{

  scopeStack.push_front( QSScopeResolution( c ) );
}

void QSCheckData::leaveClass()
{
  Q_ASSERT( inClass() );
  scopeStack.pop_front();
  //    clstack.removeLast();
}

void QSCheckData::registerType( QSClass *t )
{
  Q_ASSERT( !t->asClass() );
  QSClass * scope = currentScope();
  // ### Uncertain as to use identifier or name here?
  QSMember member;
  Q_ASSERT( !scope->member( 0, t->identifier(), &member ) );
  scope->addStaticVariableMember( t->identifier(),
                                  env() ->typeClass() ->createType( t ),
                                  AttributeExecutable );
}

QSClass *QSCheckData::typeInfo( uint name ) const
{
  QSMember member;
  QSScopeResolutionList::const_iterator it = scopeStack.begin();
  QSObject tmp;
  tmp.setVal( ( QSShared * ) 0 );
  int offset;
  while ( it != scopeStack.end() ) {
    offset = 0;
    tmp.setType( ( *it ).cl );
    const QSClass *cl;
    if ( ( *it ).cl->name() == QLatin1String( "QObject" ) ) {
      cl = env() ->thisValue().resolveMember( name, &member, tmp.objectType(),
                                              &offset );

    } else {
      cl = tmp.resolveMember( name, &member, tmp.objectType(), &offset );
      tmp.invalidate();
    }
    if ( cl ) {
      /* Avoid calls to deref for QSWrapperClass, when tmp is destroyed */
      tmp.invalidate();
      QSObject o( cl->fetchValue( 0, member ) );
      if ( o.objectType() == env() ->typeClass() )
        return QSTypeClass::classValue( &o );
      else
        return 0;
    }
    ++it;
  }
  return 0;
}


/*!
  Returns a pointer to the innermost class based on the current
  position in the scope. For global scope the global class is returned.
*/
QSClass * QSCheckData::innermostClass() const
{
  QSScopeResolution scope;
  QSScopeResolutionList::const_iterator it = scopeStack.begin();
  while ( it != scopeStack.end() ) {
    scope = *it;
    if ( scope.isClassScope() || scope.isGlobalScope() ) {
      return scope.cl;
    }
    ++it;
  }
  Q_ASSERT( 0 /* should never get here */ );
  return 0;
}

/*!
  Returns the current function if the scope stack top is
  a function. 0 otherwise.
*/

QSFunctionScopeClass * QSCheckData::currentFunction() const
{
  if ( scopeStack.size() > 0 && scopeStack.first().isFunctionScope() ) {
    return static_cast<QSFunctionScopeClass *>( scopeStack.first().cl );
  }
  return 0;
}

/*!
  Returns the current class if the scope stack top is a class. 0 otherwise
*/
QSClass * QSCheckData::currentClass() const
{
  if ( scopeStack.size() > 0 && scopeStack.first().isClassScope() ) {
    return static_cast<QSClass *>( scopeStack.first().cl );
  }
  return 0;
}

/*!
  Returns the class representing the current scope. 0 if there are no
  objects on the scope stack
*/
QSClass * QSCheckData::currentScope() const
{
  Q_ASSERT( !scopeStack.isEmpty() );
  return scopeStack.first().cl;
}

bool QSCheckData::insideClass( uint name ) const
{
  QSScopeResolutionList::const_iterator it = scopeStack.begin();
  while ( it != scopeStack.end() ) {
    if ( ( *it ).isClassScope() && ( *it ).name() == name ) {
      return true;
    }
    ++it;
  }
  return false;
}


/*!
  Resolves the globally qualified name of \name, including
  package / class / function resolution.
*/
QString QSCheckData::globalName( const QString &name ) const
{
  QStringList lst( name );
  QSScopeResolutionList::const_iterator it = scopeStack.begin();
  while ( it != scopeStack.end() ) {
    lst.prepend( USTR( ( *it ).name() ) );
    ++it;
  }
  return lst.join( QLatin1String( "." ) );
}

void QSCheckData::enterFunction( QSFunctionScopeClass * func )
{
  //     qDebug( "QSCheckData::enterFunction( %s ) into %s",
  //      func->identifier().ascii(),
  //      currentScope()->identifier().ascii() );
  scopeStack.push_front( func );
}

void QSCheckData::leaveFunction()
{
  Q_ASSERT( inFunction() );
  scopeStack.pop_front();
}

/*!
  Can we have a return statement at this point, i.e. are we inside
  a function ?
 */

bool QSCheckData::canReturn() const
{
  QSScopeResolutionList::const_iterator it = scopeStack.begin();
  while ( it != scopeStack.end() ) {
    if ( ( *it ).isFunctionScope() )
      return true;
    else if ( !( *it ).isBlockScope() )
      return false;
    else
      ++it;
  }
  return false;
}

void QSCheckData::enterBlock( QSBlockScopeClass * cl )
{
  scopeStack.push_front( cl );
}

void QSCheckData::leaveBlock()
{
  scopeStack.pop_front();
}

void QSCheckData::setLabel( uint label )
{
  lastlab = label;
}

uint QSCheckData::currentLabel() const
{
  return lastlab;
}

bool QSCheckData::seenLabel( uint label ) const
{
  return lablist.contains( label );
}

void QSCheckData::clearLabel()
{
  lastlab = 0;
}

void QSCheckData::enterLoop( uint label )
{
  lablist.append( label );
}

void QSCheckData::leaveLoop()
{
  Q_ASSERT( inLoop() );
  lablist.erase( --lablist.end() );
}

void QSCheckData::enterSwitch()
{
  ++switchLevel;
}

void QSCheckData::leaveSwitch()
{
  Q_ASSERT( switchLevel > 0 );
  --switchLevel;
}


void QSCheckData::enterPackage( const QString &name )
{
  Q_ASSERT( inGlobal() );
  currpack = name;
}

void QSCheckData::leavePackage()
{
  Q_ASSERT( inPackage() );
  currpack = QString();
}


bool QSCheckData::inGlobal() const
{
  return scopeStack.size() > 0 && scopeStack.first().isGlobalScope();
}

void QSCheckData::addError( const QSNode *node, QSErrorCode code,
                            const QString &msg )
{
  ecodes.append( code );
  elines.append( node->lineNo() );
  emsgs.append( QLatin1String( "Error: " ) + msg );
}

void QSCheckData::addError( const QSNode *node, const QString &msg )
{
  addError( node, QSErrGeneral, msg );
}

void QSCheckData::addWarning( const QSNode *node, QSErrorCode code,
                              const QString &msg )
{
  ecodes.append( code );
  elines.append( node->lineNo() );
  emsgs.append( QLatin1String( "Warning: " ) + msg );
}

void QSCheckData::addWarning( const QSNode *node, const QString &msg )
{
  addWarning( node, QSErrGeneral, msg );
}


/////////////////// QSNode::check() implementations //////////////////////////

void QSNode::check( QSCheckData * )
{
  // ### nothing. should be made pure virtual one day
#ifdef QSDEBUG_CHECK
  qDebug( "check() in %s", typeid( *this ).name() );
#endif
}

/*! When parsing libraries or form code we don't want to have anything
  but class, function and variable declarations at global level. This
  function is called by all other statements and will result in an error
  if we currently are in global level.
*/

void QSStatementNode::checkIfGlobalAllowed( QSCheckData *c )
{
  if ( c->globalStatementsForbidden() && c->inGlobal() )
    c->addError( this, QSErrPositionGlobal,
                 QLatin1String( "Global statements detected. May have unwanted "
                                "side effects during execution" ) );
}

void QSProgramNode::check( QSCheckData *c )
{
  if ( source )
    source->check( c );
  QSClass *cl = c->currentScope();
  if ( cl->numVariables() < c->varBlockCount() ) {
    cl->setNumVariables( c->varBlockCount() );
  }
  static_cast<QSInstanceData *>( c->env() ->currentScope().shVal() ) ->
  ensureSize( c->currentScope() ->numVariables(),
              c->env() ->createUndefined() );
}

void QSFunctionBodyNode::check( QSCheckData *c )
{
  int attrs = c->lastAttributes();
  c->setLastAttributes( AttributeNone );
  if ( source )
    source->check( c );
  c->setLastAttributes( attrs );
}

void QSSourceElementNode::check( QSCheckData *c )
{
  statement->check( c );
}

void QSSourceElementsNode::check( QSCheckData *c )
{
  if ( elements )
    elements->check( c );
  c->clearLabel();
  element->check( c );
}

void QSStatListNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );
  if ( list )
    list->check( c );
  c->clearLabel();
  if ( c->hasError() )
    return ;
  statement->check( c );
}

void QSScopeNode::check( QSCheckData *c )
{
  QSBlockScopeClass * cl = new QSBlockScopeClass( c->env() );

  cl->setNumVariables( c->currentScope() ->numVariables() );
  cl->setEnclosingClass( c->currentScope() );
  c->enterBlock( cl );

  checkStatement( c );

  c->leaveBlock();
  scope = cl;

  if ( cl->numVariables() > c->varBlockCount() )
    c->setVarBlockCount( cl->numVariables() );
}


void QSBlockNode::checkStatement( QSCheckData *c )
{
  checkIfGlobalAllowed( c );
  if ( c->hasError() )
    return ;
  if ( statlist )
    statlist->check( c );
}

void QSAssignExprNode::check( QSCheckData *c )
{
  expr->check( c );
}

void QSEmptyStatementNode::check( QSCheckData * )
{
  //    checkIfGlobalAllowed( c );
}

void QSExprStatementNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );
  expr->check( c );
}

void QSIfNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );

  expr->check( c );
  statement1->check( c );
  if ( statement2 )
    statement2->check( c );
}

void QSDoWhileNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );

  expr->check( c );
  c->enterLoop( c->currentLabel() );
  statement->check( c );
  c->leaveLoop();
}

void QSWhileNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );

  expr->check( c );
  c->enterLoop( c->currentLabel() );
  statement->check( c );
  c->leaveLoop();
}

void QSForNode::checkStatement( QSCheckData *c )
{
  checkIfGlobalAllowed( c );

  if ( expr1 )
    expr1->check( c );
  if ( expr2 )
    expr2->check( c );
  if ( expr3 )
    expr3->check( c );
  if ( c->hasError() )
    return ;

  c->enterLoop( c->currentLabel() );
  stat->check( c );
  c->leaveLoop();
}

void QSForInNode::checkStatement( QSCheckData *c )
{
  checkIfGlobalAllowed( c );

  if ( var )
    var->check( c );
  if ( lexpr )
    lexpr->check( c );
  expr->check( c );
  if ( c->hasError() )
    return ;

  c->enterLoop( c->currentLabel() );
  stat->check( c );
  c->leaveLoop();
}

void QSContinueNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );
  if ( !c->inLoop() )
    c->addError( this, QLatin1String( "'continue' can only be used inside of iteration"
                                      " statements" ) );
  if ( ident > 0 && !c->seenLabel( ident ) )
    c->addError( this, QString( "Unknown label '%1'" ).arg( USTR( ident ) ) );
}

void QSBreakNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );
  if ( !c->inLoop() && !c->inSwitch() )
    c->addError( this, QLatin1String( "'break' can only be used inside of iteration or "
                                      "switch statements" ) );
  // ### true if nested loops & switch ?
  if ( ident > 0 && !c->seenLabel( ident ) )
    c->addError( this, QString( "Unknown label '%1'" ).arg( USTR( ident ) ) );
}

void QSReturnNode::check( QSCheckData *c )
{
  if ( !c->canReturn() )
    c->addError( this, QLatin1String( "Can only return from inside a function" ) );
  if ( value )
    value->check( c );
}

void QSWithNode::checkStatement( QSCheckData *c )
{
  checkIfGlobalAllowed( c );
  bool dlenabled = c->directLookupEnabled();
  c->setDirectLookupEnabled( false );
  expr->check( c );
  stat->check( c );
  c->setDirectLookupEnabled( dlenabled );
}

void QSSwitchNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );

  expr->check( c );

  c->enterSwitch();
  block->check( c );
  c->leaveSwitch();
}

void QSLabelNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );

  c->pushLabel( label );

  if ( stat )
    stat->check( c );

  c->popLabel();
}

void QSThrowNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );
}

void QSCatchNode::checkStatement( QSCheckData *c )
{
  checkIfGlobalAllowed( c );
  QSClass *cl = c->currentScope();
  index = cl->addVariableMember( ident, AttributeNone );
  block->check( c );
}

void QSFinallyNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );
  block->check( c );
}

void QSTryNode::check( QSCheckData *c )
{
  checkIfGlobalAllowed( c );
  block->check( c );
  if ( _catch )
    _catch->check( c );
  if ( _final )
    _final->check( c );
}

void QSTypedVarNode::check( QSCheckData *c )
{
  if ( type )
    type->check( c );
}

void QSVarBindingNode::check( QSCheckData *c )
{
  var->check( c );
  if ( assign )
    assign->check( c );

  if ( !c->directLookupEnabled() ) {
    c->addError( this, QLatin1String( "QSA does not support declaring variables inside "
                                      "a 'with' block" ) );

    return ;
  }

  int attrs = c->lastAttributes();
  QSClass * cl = c->currentScope();
  QSMember m;
  uint ustr( var->identifier() );
  if ( cl->member( 0, ustr, &m ) ) {
    if ( cl->asClass() ) {
      c->addError( this, QString( "Variable '%1' has already been "
                                  "declared in class '%2'" )
                   .arg( ustr )
                   .arg( cl->identifier() ) );
      return ;
    }
    m = QSMember( QSMember::Variable, 0, attrs );
    cl->replaceMember( ustr, &m );
    idx = m.index();
  } else {
    idx = cl->addVariableMember( ustr, attrs );
  }

  // store pointer to init node if this is a class member
  QSClassClass *clcl = const_cast<QSClassClass *>( cl->asClass() );
  if ( clcl ) {
    if ( attrs & AttributeStatic )
      clcl->addStaticInitializer( assign );
    else
      clcl->addMemberInitializer( assign );
    idx = -1; // Disable the variable binding node.
  }
}

void QSVarBindingListNode::check( QSCheckData *c )
{
  if ( list )
    list->check( c );
  if ( binding )
    binding->check( c );
}

void QSVarDefNode::check( QSCheckData *c )
{
  if ( attrs )
    attrs->check( c );
  else
    c->setLastAttributes( AttributeNone );

  if ( kind == 1 )       // const definition
    c->setLastAttributes( c->lastAttributes() | AttributeNonWritable );

  int as = c->lastAttributes();
  QSClass *cl = c->currentClass();
  if ( cl && !c->inFunction() ) {
    // class member
  } else {
    if ( as & AttributeStatic ) {
      c->addError( this, QSErrAttrStaticContext,
                   QLatin1String( "Static variables cannot be declared outside "
                                  "a class" ) );
      return ;
    }
  }
  list->check( c );

  // unset attributes
  c->setLastAttributes( AttributeNone );
}

void QSParameterNode::check( QSCheckData * c )
{
  Q_ASSERT( c->inFunction() );

  QSClass * cl = c->currentScope();

  if ( id == QSLexer::args_ ) {
    c->addError( this, QString( "Parameter 'arguments' is a predefined value in function '%1'. Use different parameter name." )
                 .arg( cl->identifier() ) );
    return ;
  }

  QSMember m;
  if ( cl->member( 0, id, &m ) ) {
    c->addError( this, QString( "Parameter '%1' already declared in function '%2'" )
                 .arg( USTR( id ) )
                 .arg( USTR( cl->identifier() ) ) );
    return ;
  }

  cl->addVariableMember( id, AttributeNone );

  if ( next )
    next->check( c );

  // create a member from id
  // add member to current scope
  // next.check();
}

void QSFuncDeclNode::check( QSCheckData *c )
{
  //     qDebug( "Function noticed: " + c->globalName(ident) );
  if ( attrs )
    attrs->check( c );
  else
    c->setLastAttributes( AttributeNone );
  int as = c->lastAttributes();
  QSClass * cl = c->currentScope();
  Q_ASSERT( cl );

  if ( ( as & AttributeStatic ) && cl->name() != QLatin1String( "Class" ) ) {
    c->addError( this, QSErrAttrStaticContext,
                 QString( "Function '%1' cannot be declared static "
                          "outside a class" ).arg( USTR( ident ) ) );
    return ;
  }

  // A bit of magic fail early when trying to overwrite a context.
  if ( c->inGlobal() ) {
    QSObject object = c->env() ->globalObject().get( ident );
    if ( object.isValid() ) {
      if ( object.objectType() ->name() == QLatin1String( "QObject" ) ) {
        c->addError( this, QString( "Cannot declare function '%1', already a global object "
                                    "present with same name" ).arg( USTR( ident ) ) );
        return ;
      }
    }
  }

  QSMember m;
  m.setPrivate( as & AttributePrivate );

  if ( cl->member( 0, ident, &m ) ) {
    QSMember mem( body, as );
    cl->replaceMember( ident, &mem );
  } else {
    cl->addFunctionMember( ident, body, as );
  }

  int tmpVarBlockCount = c->varBlockCount();
  c->setVarBlockCount( 0 );
  QSFunctionScopeClass * fscope = new QSFunctionScopeClass( c->env() ->objectClass(), this );
  fscope->setEnclosingClass( cl );
  body->setScopeDefinition( fscope );
  fscope->setFunctionBodyNode( body );
  c->enterFunction( fscope );
  if ( param )
    param->check( c );
  body->check( c );
  c->leaveFunction();

  if ( c->varBlockCount() > fscope->numVariables() )
    fscope->setNumVariables( c->varBlockCount() );
  c->setVarBlockCount( tmpVarBlockCount );

  // Calculate the number of arguments
  int count = 0;
  QSParameterNode * node = param;
  while ( node ) {
    ++count;
    node = node->nextParam();
  }
  fscope->setNumArguments( count );

  // unset attributes
  c->setLastAttributes( AttributeNone );
}

void QSFuncExprNode::check( QSCheckData *c )
{
  // ### Check attributes and refactor code with QSFuncDeclNode
  QSClass * cl = c->currentScope();
  QSFunctionScopeClass * fscope = new QSFunctionScopeClass( c->env() ->objectClass() );
  fscope->setEnclosingClass( cl );
  body->setScopeDefinition( fscope );
  fscope->setFunctionBodyNode( body );
  c->enterFunction( fscope );
  if ( param )
    param->check( c );
  body->check( c );
  c->leaveFunction();

  // Calculate the number of arguments
  int count = 0;
  QSParameterNode * node = param;
  while ( node ) {
    ++count;
    node = node->nextParam();
  }
  fscope->setNumArguments( count );
}

void QSClassDefNode::check( QSCheckData *c )
{
  //     qDebug( "Class noticed: " + c->globalName( ident ) );

  // forward declaration ?
  if ( !body ) {
    c->addWarning( this, QLatin1String( "Forward declarations not supported, yet" ) );
    return ;
  }

  if ( attrs )
    attrs->check( c );
  else
    c->setLastAttributes( AttributeNone );
  int as = c->lastAttributes();

  // base class
  QSClass *b = c->env() ->objectClass();
  if ( type ) {
    type->check( c );
    if ( c->hasError() )
      return ;
    Q_ASSERT( c->lastType() );
    b = c->lastType();
    // try to cast
    if ( b->isFinal() ) {
      c->addError( this, QSErrClassBaseFinal,
                   QLatin1String( "Base class has been declared final" ) );
      return ;
    }
  }

  // create new class description
  QSClass *scope = c->currentScope();
  if ( scope->name() == QLatin1String( "Class" ) ) {
    if ( !( as & AttributeStatic ) ) {
      c->addError( this, QSErrNestedClass,
                   QString( "Nested class '%1' in class '%2' must be "
                            "declared static" )
                   .arg( USTR( ident ) ).arg( USTR( scope->identifier() ) ) );
      return ;
    }
  } else if ( as & AttributeStatic ) {
    c->addError( this, QSErrAttributeConflict,
                 QString( "Static class '%1' cannot be declared outside "
                          "a class" ).arg( USTR( ident ) ) );
    return ;
  } else if ( scope->name() == QLatin1String( "FunctionScope" ) ) {
    c->addError( this, QSErrNestedClass,
                 QString( "Class '%1' cannot be declared inside function '%2'" )
                 .arg( USTR( ident ) ).arg( USTR( scope->identifier() ) ) );
    return ;
  }

  QSClass *absBase = c->env() ->classByIdentifier( ident );
  bool usesAbstractBase = absBase && absBase->name() == QLatin1String( "AbstractBase" );

  QSClassClass *cl = new QSClassClass( b, as, ident );
  ref(); // Compensate for setClassDefNode below. Dereffed in QSClassClass destructor.
  cl->setClassDefNode( this );
  cl->setEnclosingClass( scope );

  QSMember member;
  QSObject tobj = c->env() ->typeClass() ->createType( cl );
  if ( scope->member( 0, cl->identifier(), &member ) ) {
    scope->replaceMember( cl->identifier(), &member, tobj );
  } else {
    scope->addStaticVariableMember( cl->identifier(), tobj,
                                    AttributeExecutable );
  }

  c->enterClass( cl );
  body->check( c );
  c->leaveClass();

  // if there is a function with the same name it becomes the constructor
  QSMember ctor;
  if ( cl->member( 0, ident, &ctor ) ) {
    if ( ctor.isExecutable() ) {
      cl->setHasDefaultConstructor( true );
    } else {
      qWarning( "Constructor is no function" ); // pedantic
    }
  }

  cl->setClassBodyNode( body );
  QSFunctionScopeClass *fs = new QSFunctionScopeClass( c->env() ->objectClass() );
  fs->setEnclosingClass( cl );
  body->setScopeDefinition( fs );
  fs->setFunctionBodyNode( body );

  // will be used in execute()
  this->cldef = cl;

  // If the classname exists in env and is a previously undefined type, we have to
  // replace it with this one.
  if ( usesAbstractBase ) {
    static_cast<QSAbstractBaseClass *>( absBase ) ->replace( cl );
  }

}

// has to be in sync with QSAttribute enum (qsglobal.h)
static const char * const attrStr[] =
  {
    "",
    "abstract",
    "final",
    "private",
    "public",
    "static",
    "true",
    "false"
    "constructor"
  };

/*! Map a QSAttribute value (2^n) to index n for above string table
 */
static int attrNo( int abit )
{
  if ( abit == 0 )
    return 0;
  int no = -1;
  while ( abit )
    no++, abit >>= 1;
  return no;
}

void QSAttributeNode::check( QSCheckData *c )
{
  int asum = AttributeNone;
  for ( int i = 0; i < attrs.size(); ++i ) {
    QSAttribute attribute = attrs.at( i );
    if ( asum & attribute ) {
      QString msg = QString( "Duplicate attribute `%1'" )
                    .arg( QLatin1String( attrStr[ attrNo( attribute ) ] ) );
      c->addWarning( this, QSErrAttributeDuplication, msg );
    }
    asum |= attribute;
  }
  // check for invalid combinations
  if ( ( asum & AttributePrivate && asum & AttributePublic ) ||
       ( asum & AttributeTrue && asum & AttributeFalse ) ) {
    // ### more checks, more detailed error message
    c->addError( this, QSErrAttributeConflict, QLatin1String( "Conflicting attributes" ) );
  }
  c->setLastAttributes( asum );
}

void QSTypeNode::check( QSCheckData *c )
{
  QSClass * t = c->typeInfo( ident );
  // Create dummy to allow cross referencing classes between files.
  if ( !t ) {
    t = c->env() ->classByIdentifier( ident );
    if ( !t )
      t = new QSAbstractBaseClass( c->env() ->objectClass(), ident );
  }
  c->setLastType( t );
}

void QSPackageNode::check( QSCheckData *c )
{
  if ( c->inPackage() )
    c->addError( this, QLatin1String( "Cannot defined nested packages" ) );
  // possibly too strict
  if ( !c->inGlobal() )
    c->addError( this, QLatin1String( "Packages can only be defined at global scope" ) );
  c->enterPackage( USTR( package ) );
  block->check( c );
  c->leavePackage();
}

void QSImportNode::check( QSCheckData *c )
{
  // possibly too strict
  if ( !c->inGlobal() )
    c->addError( this, QLatin1String( "Packages can only be imported at global scope" ) );
}

void QSAssignNode::check( QSCheckData *c )
{
  expr->check( c );
  left->check( c );
}

void QSGroupNode::check( QSCheckData *c )
{
  if ( group )
    group->check( c );
}

void QSElisionNode::check( QSCheckData *c )
{
  if ( elision )
    elision->check( c );
}

void QSElementNode::check( QSCheckData *c )
{
  if ( list )
    list->check( c );
  if ( elision )
    elision->check( c );
  if ( node )
    node->check( c );
}

void QSArrayNode::check( QSCheckData *c )
{
  if ( element )
    element->check( c );
  if ( elision )
    elision->check( c );
}

void QSObjectLiteralNode::check( QSCheckData *c )
{
  if ( list )
    list->check( c );
}

void QSPropertyValueNode::check( QSCheckData *c )
{
  if ( name )
    name->check( c );
  if ( assign )
    assign->check( c );
  if ( list )
    list->check( c );
}

void QSAccessorNode1::check( QSCheckData *c )
{
  if ( expr1 )
    expr1->check( c );
  if ( expr2 )
    expr2->check( c );
}

void QSAccessorNode2::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
}

void QSNewExprNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
  if ( args )
    args->check( c );
}

void QSFunctionCallNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
  if ( args )
    args->check( c );
}

void QSDeleteNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
}

void QSVoidNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
}

void QSTypeOfNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
}

void QSPrefixNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
}

void QSUnaryPlusNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
}

void QSBitwiseNotNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
}

void QSLogicalNotNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
}

void QSAddNode::check( QSCheckData *c )
{
  if ( term1 )
    term1->check( c );
  if ( term2 )
    term2->check( c );
}

void QSMultNode::check( QSCheckData *c )
{
  if ( term1 )
    term1->check( c );
  if ( term2 )
    term2->check( c );
}

void QSShiftNode::check( QSCheckData *c )
{
  if ( term1 )
    term1->check( c );
  if ( term2 )
    term2->check( c );
}

void QSRelationalNode::check( QSCheckData *c )
{
  if ( expr1 )
    expr1->check( c );
  if ( expr2 )
    expr2->check( c );
}

void QSEqualNode::check( QSCheckData *c )
{
  if ( expr1 )
    expr1->check( c );
  if ( expr2 )
    expr2->check( c );
}

void QSBitOperNode::check( QSCheckData *c )
{
  if ( expr1 )
    expr1->check( c );
  if ( expr2 )
    expr2->check( c );
}

void QSBinaryLogicalNode::check( QSCheckData *c )
{
  if ( expr1 )
    expr1->check( c );
  if ( expr2 )
    expr2->check( c );
}

void QSConditionalNode::check( QSCheckData *c )
{
  if ( logical )
    logical->check( c );
  if ( expr1 )
    expr1->check( c );
  if ( expr2 )
    expr2->check( c );
}

void QSCommaNode::check( QSCheckData *c )
{
  if ( expr1 )
    expr1->check( c );
  if ( expr2 )
    expr2->check( c );
}

void QSClauseListNode::check( QSCheckData *c )
{
  if ( cl )
    cl->check( c );
  if ( nx )
    nx->check( c );
}

void QSCaseBlockNode::check( QSCheckData *c )
{
  if ( list1 )
    list1->check( c );
  if ( def )
    def->check( c );
  if ( list2 )
    list2->check( c );
}

void QSArgumentListNode::check( QSCheckData *c )
{
  if ( list )
    list->check( c );
  if ( expr )
    expr->check( c );
}

void QSArgumentsNode::check( QSCheckData *c )
{
  if ( list )
    list->check( c );
}

void QSNegateNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
}

void QSCaseClauseNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
  if ( list )
    list->check( c );
}

void QSNumberNode::check( QSCheckData * )
{}

void QSBooleanNode::check( QSCheckData * )
{}

void QSStringNode::check( QSCheckData * )
{}

void QSRegExpNode::check( QSCheckData * )
{}

void QSResolveNode::check( QSCheckData *c )
{
  if ( !c->directLookupEnabled() )
    return ;
  QSClass * cl = c->currentScope();
  QSClass *cont = cl;
  int uplvl = 0;
  int blocks = 0;
  QSMember member;
  while ( cont ) {
    QSMember mem;
    if ( cont->member( 0, ident, &mem ) ) {
      if ( mem.type() == QSMember::Variable && !mem.isStatic() ) {
        member = mem;
        break;
      }
    }
    ++uplvl;
    cont = cont->enclosingClass();
  }
  if ( member.isDefined() ) {

    /* If the containing class has an undefined base, the indexes will be moved
       when the we call QSAbstractBaseClass::replace() and the lookup info
       will be crap. Therefore, we don't create it. */
    QSClass * tmp = cont->base();
    while ( tmp ) {
      if ( tmp->name() == QLatin1String( "AbstractBase" ) )
        return ;
      tmp = tmp->base();
    }

    // Due to special handling of block scopes in the global object...
    if ( cont == c->env() ->globalClass() )
      uplvl += blocks;
    info = new QSLookupInfo( uplvl, member );
  }
}

void QSNullNode::check( QSCheckData * )
{}

void QSThisNode::check( QSCheckData * )
{}

void QSPostfixNode::check( QSCheckData *c )
{
  if ( expr )
    expr->check( c );
}

void QSPropertyNode::check( QSCheckData * )
{}
