/***************************************************************************
AQFieldDBInterface.cpp
-------------------
begin                : 11/02/2009
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

#include "AQFieldDBInterface.h"

AQFieldDBInterface::AQFieldDBInterface( const AQScriptEngineQSA * eng,
                                        QSharedPointer<AQFieldDB> aqFieldDB ) :
    AQComponentDBInterface( eng, aqFieldDB.data() ),
    obj_( aqFieldDB )
{
  AQObjectFactoryQSA::addInterface( obj_, this );
}

AQFieldDBInterface::~AQFieldDBInterface()
{
  AQObjectFactoryQSA::removeInterface( obj_ );
}

#include "moc_AQFieldDBInterface.cpp"
