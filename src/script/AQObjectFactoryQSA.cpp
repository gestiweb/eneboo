/***************************************************************************
AQObjectFactoryQSA.cpp
-------------------
begin                : 30/03/2008
copyright            : (C) 2003-2008 by InfoSiAL S.L.
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

#include "AQObjectFactoryQSA.h"

QHash<QObject *, AQObjectInterfaceQSA *> AQObjectFactoryQSA::ifacesHash;

AQObjectFactoryQSA::AQObjectFactoryQSA( AQScriptEngineQSA * eng ) :
    QSObjectFactory(),
    engine( eng )
{}

AQObjectInterfaceQSA *
AQObjectFactoryQSA::getInterface( AQSharedObjectQSA obj )
{
  return ifacesHash.value( obj.data() );
}

AQObjectInterfaceQSA *
AQObjectFactoryQSA::getInterface( QObject * obj )
{
  return ifacesHash.value( obj );
}

int AQObjectFactoryQSA::removeInterface( AQSharedObjectQSA obj )
{
  return ifacesHash.remove( obj.data() );
}

void AQObjectFactoryQSA::addInterface( AQSharedObjectQSA obj,
                                       AQObjectInterfaceQSA * iface )
{
  QObject * obj_ = obj.data();
  if ( !ifacesHash.contains( obj_ ) )
    ifacesHash.insert( obj_, iface );
}
