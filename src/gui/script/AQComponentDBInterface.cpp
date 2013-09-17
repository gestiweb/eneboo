/***************************************************************************
AQComponentDBInterface.cpp
-------------------
begin                : 25/02/2009
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

#include <QWidget>

#include <AQScript/AQScriptEngineQSA.h>
#include <AQUi/AQComponentDB.h>

#ifdef AQ_WUI
#include <AQUi/WQWidget.h>
#endif

#include "AQComponentDBInterface.h"

AQComponentDBInterface::AQComponentDBInterface( const AQScriptEngineQSA * eng, QWidget * wid ) :
    AQObjectInterfaceQSA( eng, wid ),
    wid_( wid )
{}

#ifdef AQ_WUI
AQComponentDBInterface::AQComponentDBInterface( const AQScriptEngineQSA * eng, WQWidget * wid ) :
    AQObjectInterfaceQSA( eng, wid->qWidget() ),
    wid_( wid->qWidget() )
{}
#endif

AQComponentDBInterface * AQComponentDBInterface::child( const QString & name )
{
  Q_ASSERT( wid_ );

  QWidget * widChild = wid_->findChild<AQComponentDB *>( name );

  if ( !widChild )
    widChild = wid_->findChild<QWidget *>( name );

  return qobject_cast<AQComponentDBInterface *>
         ( engine->createInterfaceAQ2( widChild ) );
}

void AQComponentDBInterface::show()
{
  Q_ASSERT( wid_ );
  wid_->show();
}

void AQComponentDBInterface::close()
{
  Q_ASSERT( wid_ );
  wid_->close();
}

void AQComponentDBInterface::hide()
{
  Q_ASSERT( wid_ );
  wid_->hide();
}

void AQComponentDBInterface::setDisabled( bool disable )
{
  Q_ASSERT( wid_ );
  wid_->setDisabled( disable );
}

void AQComponentDBInterface::setFocus()
{
  Q_ASSERT( wid_ );
  wid_->setFocus();
}

bool AQComponentDBInterface::isEnabled () const
{
  Q_ASSERT( wid_ );
  return wid_->isEnabled();
}

void AQComponentDBInterface::setEnabled ( bool enabled )
{
  Q_ASSERT( wid_ );
  wid_->setEnabled( enabled );
}

void AQComponentDBInterface::setWindowState( Qt::WindowStates windowState )
{
  Q_ASSERT( wid_ );
  wid_->setWindowState( windowState );
}

Qt::WindowStates AQComponentDBInterface::windowState() const
{
  Q_ASSERT( wid_ );
  return wid_->windowState();
}
