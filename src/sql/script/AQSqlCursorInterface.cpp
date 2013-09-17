/***************************************************************************
AQSqlCursorInterface.cpp
-------------------
begin                : 27/09/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
email                : mail@infosial.com
***************************************************************************/ 
/***************************************************************************
 *   This program is free software{} you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation{} version 2 of the License.               *
 ***************************************************************************/ 
/***************************************************************************
   Este  programa es software libre. Puede redistribuirlo y/o modificarlo
   bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
   versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#include <AQCore/AQActionMD.h>
#include <AQCore/AQTableMD.h>
#include <AQScript/AQScriptEngineQSA.h>
#include <AQScript/AQObjectFactoryQSA.h>
#include <AQScript/AQActionInterface.h>

#include "AQSqlCursorInterface.h"
#include "../AQSqlDatabase.h"

AQSqlCursorInterface::AQSqlCursorInterface( const AQScriptEngineQSA * eng, QSharedPointer<AQSqlCursor> aqCursor ) :
    AQObjectInterfaceQSA( eng, aqCursor.data() ),
    obj_( aqCursor )
{
  AQObjectFactoryQSA::addInterface( obj_, this );

  connects();
}

AQSqlCursorInterface::AQSqlCursorInterface( const AQScriptEngineQSA * eng, const QString & n, const QString & connName ) :
    AQObjectInterfaceQSA( eng )
{
  obj_ = QSharedPointer<AQSqlCursor>( new AQSqlCursor( n, true, dbConName( connName ) ) );

  AQObjectFactoryQSA::addInterface( obj_, this );

  connects();
}

AQSqlCursorInterface::~AQSqlCursorInterface()
{
  AQObjectFactoryQSA::removeInterface( obj_ );
}

void AQSqlCursorInterface::connects() const
{
  connect( obj_.data(), SIGNAL( newBuffer() ), this, SIGNAL( newBuffer() ) );
  connect( obj_.data(), SIGNAL( bufferChanged( const QString & ) ), this, SIGNAL( bufferChanged( const QString & ) ) );
  connect( obj_.data(), SIGNAL( cursorUpdated() ), this, SIGNAL( cursorUpdated() ) );
  connect( obj_.data(), SIGNAL( currentChanged( int ) ), this, SIGNAL( currentChanged( int ) ) );
  connect( obj_.data(), SIGNAL( bufferCommited() ), this, SIGNAL( bufferCommited() ) );
}

QString AQSqlCursorInterface::mainFilter() const
{
  Q_ASSERT( obj_ );
  return obj_->mainFilter();
}

void AQSqlCursorInterface::setMainFilter( const QString & f ) const
{
  Q_ASSERT( obj_ );
  QString s = f;
  s.replace( "NaN", "" );
  s.replace( "nan", "" );
  obj_->setMainFilter( s );
}

int AQSqlCursorInterface::modeAccess() const
{
  Q_ASSERT( obj_ );
  return obj_->modeAccess();
}

void AQSqlCursorInterface::setModeAccess( int m )
{
  Q_ASSERT( obj_ );
  obj_->setModeAccess( ( AQSql::Mode ) m );
}

void AQSqlCursorInterface::setValueBuffer( const QString & fN, const QVariant & v ) const
{
  Q_ASSERT( obj_ );
  QString s = v.toString();
  if ( s.length() == 3 ) {
    s.replace( "NaN", "" );
    s.replace( "nan", "" );
  }
  QVariant vv( s );
  vv.cast( v.type() );
  obj_->setValueBuffer( fN, vv );
}

QVariant AQSqlCursorInterface::valueBuffer( const QString & fN ) const
{
  Q_ASSERT( obj_ );
  return obj_->valueBuffer( fN );
}

QVariant AQSqlCursorInterface::valueBufferCopy( const QString & fN ) const
{
  Q_ASSERT( obj_ );
  return obj_->valueBuffer( fN );
}

bool AQSqlCursorInterface::isNull ( const QString & name ) const
{
  Q_ASSERT( obj_ );
  return obj_->isNull( name );
}

void AQSqlCursorInterface::setNull ( const QString & name ) const
{
  Q_ASSERT( obj_ );
  obj_->setNull( name );
}

bool AQSqlCursorInterface::isCopyNull ( const QString & name ) const
{
  Q_ASSERT( obj_ );
  return obj_->bufferCopyIsNull( name );
}

void AQSqlCursorInterface::setCopyNull ( const QString & name ) const
{
  Q_ASSERT( obj_ );
  obj_->bufferCopySetNull( name );
}

bool AQSqlCursorInterface::fieldDisabled( const QString & fN ) const
{
  Q_ASSERT( obj_ );
  return obj_->fieldDisabled( fN );
}

void AQSqlCursorInterface::setActivatedCheckIntegrity( bool a ) const
{
  Q_ASSERT( obj_ );
  obj_->setActivatedCheckIntegrity( a );
}

void AQSqlCursorInterface::setActivatedCommitActions( bool a ) const
{
  Q_ASSERT( obj_ );
  obj_->setActivatedCommitActions( a );
}

bool AQSqlCursorInterface::checkIntegrity( bool showError ) const
{
  Q_ASSERT( obj_ );
  return obj_->checkIntegrity( showError );
}

void AQSqlCursorInterface::refresh( const QString & fN ) const
{
  Q_ASSERT( obj_ );
  obj_->refresh( fN );
}

bool AQSqlCursorInterface::refreshBuffer() const
{
  Q_ASSERT( obj_ );
  return obj_->refreshBuffer();
}

int AQSqlCursorInterface::at() const
{
  Q_ASSERT( obj_ );
  return obj_->at();
}

bool AQSqlCursorInterface::seek( int i, bool relative, bool emite ) const
{
  Q_ASSERT( obj_ );
  return obj_->seek( i, relative, emite );
}

bool AQSqlCursorInterface::next( bool emite ) const
{
  Q_ASSERT( obj_ );
  return obj_->next( emite );
}

bool AQSqlCursorInterface::prev( bool emite ) const
{
  Q_ASSERT( obj_ );
  return obj_->prev( emite );
}

bool AQSqlCursorInterface::first( bool emite ) const
{
  Q_ASSERT( obj_ );
  return obj_->first( emite );
}

bool AQSqlCursorInterface::last( bool emite ) const
{
  Q_ASSERT( obj_ );
  return obj_->last( emite );
}

int AQSqlCursorInterface::del( bool invalidate ) const
{
  Q_ASSERT( obj_ );
  return obj_->del( invalidate );
}

bool AQSqlCursorInterface::select( const QString & filter ) const
{
  Q_ASSERT( obj_ );
  QString s = filter;
  s.replace( "NaN", "" );
  s.replace( "nan", "" );
  return obj_->select( s );
}

bool AQSqlCursorInterface::select() const
{
  Q_ASSERT( obj_ );
  obj_->Q3SqlCursor::setFilter( obj_->curFilter() );
  return obj_->Q3SqlCursor::select();
}

int AQSqlCursorInterface::atFrom() const
{
  Q_ASSERT( obj_ );
  return obj_->atFrom();
}

int AQSqlCursorInterface::atFromBinarySearch( const QString & fN, const QString & v, bool orderAsc ) const
{
  Q_ASSERT( obj_ );
  return obj_->atFromBinarySearch( fN, v, orderAsc );
}

int AQSqlCursorInterface::size() const
{
  Q_ASSERT( obj_ );
  return obj_->size();
}

bool AQSqlCursorInterface::commitBuffer( bool ) const
{
  Q_ASSERT( obj_ );
  return obj_->commitBuffer();
}

bool AQSqlCursorInterface::commitBufferCursorMaster() const
{
  Q_ASSERT( obj_ );
  return obj_->commitBufferCursorMaster();
}

AQSqlCursorInterface * AQSqlCursorInterface::cursorMaster() const
{
  Q_ASSERT( obj_ );
  return qobject_cast<AQSqlCursorInterface *>
         ( engine->createInterface( obj_->cursorMaster() ) );
}

void AQSqlCursorInterface::setUnLock( const QString & fN, bool v ) const
{
  Q_ASSERT( obj_ );
  obj_->setUnLock( fN, v );
}

bool AQSqlCursorInterface::isLocked() const
{
  Q_ASSERT( obj_ );
  return obj_->isLocked();
}

QString AQSqlCursorInterface::table() const
{
  Q_ASSERT( obj_ );
  return obj_->metadata() ->name();
}

int AQSqlCursorInterface::fieldType( const QString & fN ) const
{
  Q_ASSERT( obj_ );
  return obj_->metadata() ->fieldType( fN );
}

QString AQSqlCursorInterface::primaryKey() const
{
  Q_ASSERT( obj_ );
  return obj_->metadata() ->primaryKey();
}

bool AQSqlCursorInterface::isValid() const
{
  Q_ASSERT( obj_ );
  return obj_->isValid();
}

bool AQSqlCursorInterface::isModifiedBuffer() const
{
  Q_ASSERT( obj_ );
  return obj_->isModifiedBuffer();
}

void AQSqlCursorInterface::emitNewBuffer()
{
  Q_EMIT newBuffer();
}

void AQSqlCursorInterface::emitBufferChanged( const QString & v )
{
  Q_EMIT bufferChanged( v );
}

void AQSqlCursorInterface::emitCursorUpdated()
{
  Q_EMIT cursorUpdated();
}

void AQSqlCursorInterface::emitCurrentChanged( int v )
{
  Q_EMIT currentChanged( v );
}

void AQSqlCursorInterface::emitBufferCommited()
{
  Q_EMIT bufferCommited();
}


AQSqlCursorInterfaceAQ2::AQSqlCursorInterfaceAQ2( const AQScriptEngineQSA * eng, QSharedPointer<AQSqlCursor> aqCursor ) :
    AQSqlCursorInterface( eng, aqCursor ),
    edition_( true ),
    browse_( true )
{
  AQ_PORT_AQ2_NOP( FLSqlCursor, AQSqlCursor; );

  Q_ASSERT( aqCursor );

  AQSharedAction act( AQAction::fromSqlCursor( aqCursor.data() ) );

  if ( act )
    connect( act.data(), SIGNAL( accepted() ), this, SIGNAL( recordChoosed() ) );
}

bool AQSqlCursorInterfaceAQ2::setEditMode()
{
  int modeAccess_;
  bool result;

  AQ_PORT_AQ2(
    AQSqlCursor::setEditMode(),
    result = false;
    modeAccess_ = modeAccess();
    if ( modeAccess_ == AQSql::Insert ) {
    if ( commitBuffer() ) {
        refresh();
        setModeAccess( AQSql::Edit );
        result = refreshBuffer();
      }
    } else if ( modeAccess_ == AQSql::Edit )
    result = true;
  );

  return result;
}

void AQSqlCursorInterfaceAQ2::setEdition( bool b )
{
  AQ_VOID_PORT_AQ2( AQSqlCursor::setEdition );

  edition_ = b;
}

void AQSqlCursorInterfaceAQ2::setBrowse( bool b )
{
  AQ_VOID_PORT_AQ2( AQSqlCursor::setBrowse );

  browse_ = b;
}

bool AQSqlCursorInterfaceAQ2::commitBufferCursorRelation()
{
  AQ_PORT_AQ2( AQSqlCursor::commitBufferCursorRelation(),
               return commitBufferCursorMaster(); );
}

AQSqlCursorInterfaceAQ2 * AQSqlCursorInterfaceAQ2::cursorRelation() const
{
  AQ_PORT_AQ2_NOP( AQSqlCursor::cursorRelation(),
                   cursorMaster(); );

  Q_ASSERT( obj_ );
  return qobject_cast<AQSqlCursorInterfaceAQ2 *>
         ( engine->createInterfaceAQ2( obj_->cursorMaster() ) );
}

AQSqlCursor * AQSqlCursorInterfaceAQ2::obj() const
{
  AQ_VOID_PORT_AQ2( AQSqlCursor::obj() );
  return 0;
}

void AQSqlCursorInterfaceAQ2::emitRecordChoosed()
{
  AQ_VOID_PORT_AQ2( AQSqlCursor::emitRecordChoosed() );
}

void AQSqlCursorInterfaceAQ2::emitAutoCommit()
{
  AQ_VOID_PORT_AQ2( AQSqlCursor::emitAutoCommit() );
}

QString AQSqlCursorInterfaceAQ2::action() const
{
  AQ_PORT_AQ2_NOP( AQSqlCursor::action(),
                   Usar AQAction siempre que sea posible._n \
                   Los cursores asociados a un formulario o un componente se deben_n \
                   manejar a través del objeto AQAction que se obtiene con el método_n \
                   aqAction() disponible en todos los componentes gráficos que enlazan_n \
                   con la base de datos. En AbanQ v3 los objetos del tipo AQSqlCursor_n \
                   son de bajo nivel y sólo deben utilizarse estrictamente para hacer_n \
                   operaciones básicas sobre los registros de una tabla o consulta._n \
                   Ver documentacion de AQAction y AQFormDB para más detalles._n_n
                 Ejemplo: _n_n
                 En AbanQ v2: _n
                   var actionName = this.cursor().action();_n
                 En AbanQ v3: _n
                   var actionName = AQAction.fromSqlCursor( this.cursor() ).actName;_n );

  return AQActionInterfaceStatic( engine ).fromSqlCursor( this ) ->property( "actName" ).toString();
}

void AQSqlCursorInterfaceAQ2::setAction( const QString & actionNameUi )
{
  AQ_PORT_AQ2_NOP( AQSqlCursor::setAction( actionNameUi ),
                   Usar AQAction siempre que sea posible._n \
                   Los cursores asociados a un formulario o un componente se deben_n \
                   manejar a través del objeto AQAction que se obtiene con el método_n \
                   aqAction() disponible en todos los componentes gráficos que enlazan_n \
                   con la base de datos. En AbanQ v3 los objetos del tipo AQSqlCursor_n \
                   son de bajo nivel y sólo deben utilizarse estrictamente para hacer_n \
                   operaciones básicas sobre los registros de una tabla o consulta._n \
                   Ver documentacion de AQAction y AQFormDB para más detalles._n_n
                 Ejemplo: _n_n
                 En AbanQ v2: _n
                   var cur = this.cursor();
                   cur.setAction( "clientes" );
                   cur.editRecord();_n
                 En AbanQ v3: _n
                   AQAction.fromSqlCursor( this.cursor() ).editRecord( "clientes" ); );

  actionNameUi_ = actionNameUi;
}

void AQSqlCursorInterfaceAQ2::editRecord()
{
  AQ_PORT_AQ2_NOP( AQSqlCursor::editRecord(),
                   AQAction.fromSqlCursor( cursor ).editRecord(); );

  AQActionInterfaceStatic( engine ).fromSqlCursor( this ) ->editRecord( actionNameUi_ );
}

void AQSqlCursorInterfaceAQ2::insertRecord()
{
  AQ_PORT_AQ2_NOP( AQSqlCursor::editRecord(),
                   AQAction.fromSqlCursor( cursor ).insertRecord(); );

  AQActionInterfaceStatic( engine ).fromSqlCursor( this ) ->insertRecord( actionNameUi_ );
}

void AQSqlCursorInterfaceAQ2::browseRecord()
{
  AQ_PORT_AQ2_NOP( AQSqlCursor::editRecord(),
                   AQAction.fromSqlCursor( cursor ).browseRecord(); );

  AQActionInterfaceStatic( engine ).fromSqlCursor( this ) ->browseRecord( actionNameUi_ );
}

void AQSqlCursorInterfaceAQ2::chooseRecord()
{
  AQ_VOID_PORT_AQ2( AQSqlCursor::chooseRecord() );

  if ( edition_ )
    editRecord();
  else if ( browse_ )
    browseRecord();

  Q_EMIT recordChoosed();
}

void AQSqlCursorInterfaceAQ2::setForwardOnly( bool )
{
  AQ_VOID_PORT_AQ2( AQSqlCursor::setForwardOnly() );
}

#include "moc_AQSqlCursorInterface.cpp"
