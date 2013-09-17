/***************************************************************************
AQMainWindowInterface.cpp
-------------------
begin                : 09/02/2009
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

#include <AQScript/AQObjectFactoryQSA.h>

#include "AQMainWindowInterface.h"
#include "AQActionInterface.h"
#include "AQFormDBInterface.h"

AQMainWindowInterface::AQMainWindowInterface( const AQScriptEngineQSA * eng, QSharedPointer<AQMainWindow> aqMainWindow ) :
    AQComponentDBInterface( eng, aqMainWindow.data() ),
    obj_( aqMainWindow )
{
  AQObjectFactoryQSA::addInterface( obj_, this );
}

AQMainWindowInterface::~AQMainWindowInterface()
{
  AQObjectFactoryQSA::removeInterface( obj_ );
}

AQFormDBInterface * AQMainWindowInterface::activateAction( const QString & idAction )
{
  Q_ASSERT( obj_ );

#ifndef AQ_WUI

  return qobject_cast<AQFormDBInterface *>
         ( engine->createInterface( obj_->activateAction( idAction ) ) );

#endif
}

AQFormDBInterface * AQMainWindowInterface::activateAction( AQActionInterface * aqAction )
{
  Q_ASSERT( obj_ );

#ifndef AQ_WUI

  return qobject_cast<AQFormDBInterface *>
         ( engine->createInterface( obj_->activateAction( aqAction->obj_ ) ) );

#endif
}
