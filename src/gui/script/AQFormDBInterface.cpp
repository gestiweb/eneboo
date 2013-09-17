/***************************************************************************
AQFormDBInterface.cpp
-------------------
begin                : 25/09/2007
copyright            : (C) 2003-2009 by InfoSiAL S.L.
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

#include <AQScript/AQScriptEngineQSA.h>
#include <AQScript/AQObjectFactoryQSA.h>
#include <AQScript/AQSqlCursorInterface.h>

#include "AQFormDBInterface.h"
#include "AQActionInterface.h"
#include "../manager/AQUiManager.h"

AQFormDBInterface::AQFormDBInterface( const AQScriptEngineQSA * eng,
                                      QSharedPointer<AQFormDB> aqFormDB ) :
    AQComponentDBInterface( eng, aqFormDB.data() ),
    obj_( aqFormDB )
{
  AQObjectFactoryQSA::addInterface( obj_, this );

  connects();
}

AQFormDBInterface::AQFormDBInterface( const AQScriptEngineQSA * eng, bool noUi ) :
    AQComponentDBInterface( eng )
{
  if ( !noUi ) {
    obj_ = QSharedPointer<AQFormDB>( new AQFormDB );
    wid_ = obj_.data();

    AQObjectFactoryQSA::addInterface( obj_, this );

    connects();
  }
}

AQFormDBInterface::~AQFormDBInterface()
{
  AQObjectFactoryQSA::removeInterface( obj_ );
}

void AQFormDBInterface::connects() const
{
  connect( obj_.data(), SIGNAL( closeRequest() ), this, SLOT( emitCloseRequest() ) );
  connect( obj_.data(), SIGNAL( accepted() ), this, SIGNAL( accepted() ) );
  connect( obj_.data(), SIGNAL( rejected() ), this, SIGNAL( rejected() ) );
}

void AQFormDBInterface::setAQAction( AQActionInterface * act )
{
  Q_ASSERT( obj_ );
  obj_->setAQAction( act->obj_ );
}

AQActionInterface * AQFormDBInterface::aqAction() const
{
  Q_ASSERT( obj_ );
  return qobject_cast<AQActionInterface *>
         ( engine->createInterface( obj_->aqAction() ) );
}

void AQFormDBInterface::setUiMaster( QWidget * uiMaster )
{
  Q_ASSERT( obj_ );
  obj_->setUiMaster( uiMaster );
}

void AQFormDBInterface::setUiMaster( const QString & uiFileName )
{
  setUiMaster( AQUiManager::createUI( uiFileName, 0, dbCon() ) );
}

QWidget * AQFormDBInterface::uiMaster() const
{
  Q_ASSERT( obj_ );
  return obj_->uiMaster();
}

void AQFormDBInterface::setUiRecord( QWidget * uiRecord )
{
  Q_ASSERT( obj_ );
  obj_->setUiRecord( uiRecord );
}

void AQFormDBInterface::setUiRecord( const QString & uiFileName )
{
  setUiRecord( AQUiManager::createUI( uiFileName, 0, dbCon() ) );
}

QWidget * AQFormDBInterface::uiRecord() const
{
  Q_ASSERT( obj_ );
  return obj_->uiRecord();
}

QWidget * AQFormDBInterface::uiActive() const
{
  Q_ASSERT( obj_ );
  return obj_->uiActive();
}

void AQFormDBInterface::activateMasterUi()
{
  Q_ASSERT( obj_ );
  obj_->activateMasterUi();
}

void AQFormDBInterface::activateDetailUi()
{
  Q_ASSERT( obj_ );
  obj_->activateDetailUi();
}

void AQFormDBInterface::activateRecordUi()
{
  Q_ASSERT( obj_ );
  obj_->activateRecordUi();
}

QVariant AQFormDBInterface::exec( const QString & fieldName )
{
  Q_ASSERT( obj_ );
  return obj_->exec( fieldName );
}

void AQFormDBInterface::exec()
{
  Q_ASSERT( obj_ );
  obj_->exec();
}

void AQFormDBInterface::accept()
{
  Q_ASSERT( obj_ );
  obj_->accept();
}

void AQFormDBInterface::reject()
{
  Q_ASSERT( obj_ );
  obj_->reject();
}

int AQFormDBInterface::result() const
{
  Q_ASSERT( obj_ );
  return obj_->result();
}

void AQFormDBInterface::setDeleteOnClose( bool del )
{
  Q_ASSERT( obj_ );
  obj_->setDeleteOnClose( del );
}

bool AQFormDBInterface::deleteOnClose() const
{
  Q_ASSERT( obj_ );
  return obj_->deleteOnClose();
}

void AQFormDBInterface::setIgnoreCloseEvent( bool ign )
{
  Q_ASSERT( obj_ );
  obj_->setIgnoreCloseEvent( ign );
}

bool AQFormDBInterface::ignoreCloseEvent() const
{
  Q_ASSERT( obj_ );
  return obj_->ignoreCloseEvent();
}

void AQFormDBInterface::emitCloseRequest()
{
  Q_EMIT closeRequest( this );
}

AQFormDBInterfaceAQ2::AQFormDBInterfaceAQ2( const AQScriptEngineQSA * eng, QSharedPointer<AQFormDB> aqFormDB ) :
    AQFormDBInterface( eng, aqFormDB ), cursorNoUi_( 0 )
{
  connect( obj_.data(), SIGNAL( closeRequest() ), this, SIGNAL( closed() ) );
}

AQFormDBInterfaceAQ2::AQFormDBInterfaceAQ2( const AQScriptEngineQSA * eng, bool noUi ) :
    AQFormDBInterface( eng, noUi ), cursorNoUi_( 0 )
{
  if ( !noUi )
    connect( obj_.data(), SIGNAL( closeRequest() ), this, SIGNAL( closed() ) );
}

AQFormDBInterfaceAQ2::AQFormDBInterfaceAQ2( const AQScriptEngineQSA * eng, const QString & a ) :
    AQFormDBInterface( eng ), cursorNoUi_( 0 )
{
  AQ_PORT_AQ2_NOP( FLFormDB( actionName ),
                   var aqAction = new AQAction( actionName );
                   var form = aqAction.form(); );

  AQActionInterface * act = qobject_cast<AQActionInterface *>
                            ( engine->createInterface( AQAction::create( a, dbConName() ) ) );

  if ( act )
    act->setParent( this );

  setAQAction( act );

  connect( obj_.data(), SIGNAL( closeRequest() ), this, SIGNAL( closed() ) );
}

AQFormDBInterfaceAQ2::AQFormDBInterfaceAQ2( const AQScriptEngineQSA * eng, AQSqlCursorInterfaceAQ2 * c, const QString & a ) :
    AQFormDBInterface( eng ), cursorNoUi_( 0 )
{
  AQ_PORT_AQ2_NOP( FLFormDB( cursor, actionName ),
                   actionName_ = ( actionName.isEmpty() ? cursor.table() : actionName );
                   var aqAction = new AQAction( actionName_ );
                   var form = aqAction.form(); );

  c->setAction( a );
  AQActionInterface * act = qobject_cast<AQActionInterface *>
                            ( engine->createInterface( AQAction::create(
                                                         a.isEmpty() ? c->property( "table" ).toString() : a,
                                                         dbConName() ) ) );

  if ( act )
    act->setParent( this );

  setAQAction( act );

  connect( obj_.data(), SIGNAL( closeRequest() ), this, SIGNAL( closed() ) );
}

void AQFormDBInterfaceAQ2::setCursor( AQSqlCursorInterfaceAQ2 * cursor )
{
  AQ_PORT_AQ2_NOP( AQFormDB::setCursor( cursor ),
                   setAQAction( AQAction.fromSqlCursor( cursor ) ); );

  setAQAction( AQActionInterfaceStatic( engine ).fromSqlCursor( cursor ) );
}

AQSqlCursorInterfaceAQ2 * AQFormDBInterfaceAQ2::cursor() const
{
  AQ_PORT_AQ2_NOP( AQFormDB::cursor(),
                   aqAction().cursor(); );

  AQSqlCursor * cur;

  if ( obj_ && obj_->aqAction() )
    cur = obj_->aqAction() ->cursor();
  else
    cur = cursorNoUi_;

  Q_ASSERT( cur );

  return qobject_cast<AQSqlCursorInterfaceAQ2 *>
         ( engine->createInterfaceAQ2( cur ) );
}

QWidget * AQFormDBInterfaceAQ2::mainWidget() const
{
  AQ_PORT_AQ2( AQFormDB::mainWidget(),
               return uiMaster(); );
}

void AQFormDBInterfaceAQ2::setMainWidget( QWidget * widget )
{
  AQ_PORT_AQ2( AQFormDB::setMainWidget( widget ),
               setUiMaster( widget );
               activateMasterUi(); );
}

void AQFormDBInterfaceAQ2::setMainWidget()
{
  AQ_PORT_AQ2( AQFormDB::setMainWidget(),
               activateMasterUi(); );
}

void AQFormDBInterfaceAQ2::setMainWidget( const QString & uiFileName )
{
  AQ_PORT_AQ2( AQFormDB::setMainWidget( uiFileName ),
               setUiMaster( uiFileName );
               activateMasterUi(); );
}

AQFormDB * AQFormDBInterfaceAQ2::obj() const
{
  AQ_VOID_PORT_AQ2( AQFormDB::obj() );
  return 0;
}

void AQFormDBInterfaceAQ2::emitClosed()
{
  AQ_VOID_PORT_AQ2( AQFormDB::emitClosed() );
}

void AQFormDBInterfaceAQ2::setMaximized()
{
  AQ_PORT_AQ2( AQFormDB::setMaximized(),
               setWindowState( windowState() | Qt::WindowMaximized ); );
}

QWidget * AQFormRecordDBInterfaceAQ2::mainWidget() const
{
  AQ_PORT_AQ2( AQFormRecordDB::mainWidget(),
               return uiRecord(); );
}

void AQFormRecordDBInterfaceAQ2::setMainWidget( QWidget * widget )
{
  AQ_PORT_AQ2( AQFormRecordDB::setMainWidget( widget ),
               setUiRecord( widget );
               activateRecordUi(); );
}

void AQFormRecordDBInterfaceAQ2::setMainWidget()
{
  AQ_PORT_AQ2( AQFormRecordDB::setMainWidget(),
               activateRecordUi(); );
}

void AQFormRecordDBInterfaceAQ2::setMainWidget( const QString & uiFileName )
{
  AQ_PORT_AQ2( AQFormRecordDB::setMainWidget( uiFileName ),
               setUiRecord( uiFileName );
               activateRecordUi(); );
}

bool AQFormSearchDBInterfaceAQ2::accepted() const
{
  AQ_PORT_AQ2( AQFormSearchDB::accepted(),
               return result() == AQUi::Accepted; );
}

#include "moc_AQFormDBInterface.cpp"
