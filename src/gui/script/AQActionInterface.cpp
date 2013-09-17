/***************************************************************************
AQActionInterface.cpp
-------------------
begin                : 10/02/2009
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
#include <AQScript/AQFormDBInterface.h>

#include "AQActionInterface.h"

AQActionInterface::AQActionInterface( const AQScriptEngineQSA * eng,
                                      AQSharedAction aqAction ) :
    AQObjectInterfaceQSA( eng, aqAction.data() ),
    obj_( aqAction )
{
  AQObjectFactoryQSA::addInterface( obj_, this );

  connects();
}

AQActionInterface::AQActionInterface( const AQScriptEngineQSA * eng,
                                      const QString & actionName,
                                      const QString & connName ) :
    AQObjectInterfaceQSA( eng )
{
  obj_ = AQAction::create( actionName, dbConName( connName ) );

  AQObjectFactoryQSA::addInterface( obj_, this );

  connects();
}

AQActionInterface::~AQActionInterface()
{
  AQObjectFactoryQSA::removeInterface( obj_ );
}

void AQActionInterface::connects() const
{
  connect( obj_.data(), SIGNAL( accepted() ), this, SIGNAL( accepted() ) );
}

AQFormDBInterface * AQActionInterface::form()
{
  Q_ASSERT( obj_ );
  return qobject_cast<AQFormDBInterface *>
         ( engine->createInterface( obj_->form() ) );
}

QVariant AQActionInterface::exec( const QString & fieldName, const QString & actionNameUi )
{
  Q_ASSERT( obj_ );
  return obj_->exec( fieldName, actionNameUi );
}

AQSqlCursorInterface * AQActionInterface::cursor() const
{
  Q_ASSERT( obj_ );
  return qobject_cast<AQSqlCursorInterface *>
         ( engine->createInterface( obj_->cursor() ) );
}

AQActionInterface * AQActionInterface::aqActionMaster() const
{
  Q_ASSERT( obj_ );
  return qobject_cast<AQActionInterface *>
         ( engine->createInterface( obj_->aqActionMaster() ) );
}

AQActionInterface * AQActionInterface::aqActionField( const QString & fieldName )
{
  Q_ASSERT( obj_ );
  return qobject_cast<AQActionInterface *>
         ( engine->createInterface( obj_->aqActionField( fieldName ) ) );
}

QString AQActionInterface::actName() const
{
  Q_ASSERT( obj_ );
  return obj_->actName();
}

QString AQActionInterface::connName() const
{
  Q_ASSERT( obj_ );
  return obj_->connName();
}

void AQActionInterface::insertRecord( const QString & actionNameUi )
{
  Q_ASSERT( obj_ );
  obj_->insertRecord( actionNameUi );
}

void AQActionInterface::editRecord( const QString & actionNameUi )
{
  Q_ASSERT( obj_ );
  obj_->editRecord( actionNameUi );
}

void AQActionInterface::browseRecord( const QString & actionNameUi )
{
  Q_ASSERT( obj_ );
  obj_->browseRecord( actionNameUi );
}

void AQActionInterface::copyRecord( const QString & actionNameUi )
{
  Q_ASSERT( obj_ );
  obj_->copyRecord( actionNameUi );
}

void AQActionInterface::deleteRecord()
{
  Q_ASSERT( obj_ );
  obj_->deleteRecord();
}

AQActionInterface * AQActionInterfaceStatic::fromSqlCursor( const AQSqlCursorInterfaceAQ2 * cur ) const
{
  return qobject_cast<AQActionInterface *>
         ( engine->createInterface( AQAction::fromSqlCursor( cur->obj_.data() ) ) );
}
