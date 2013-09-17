/***************************************************************************
AQXmlObjectFactoryQSA.cpp
-------------------
begin                : 03/03/2009
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

#include <qsinterpreter.h>

#include <AQScript/AQScriptEngineQSA.h>

#include "AQXmlObjectFactoryQSA.h"
#include "AQDomNodeListInterface.h"

AQXmlObjectFactoryQSA::AQXmlObjectFactoryQSA( AQScriptEngineQSA * eng ) :
    AQObjectFactoryQSA( eng )
{
  registerClass( "AQDomNodeList", &AQDomNodeListInterface::staticMetaObject );

  // Compatibilidad con AbanQ v2
  registerClass( "FLDomNodeList", &AQDomNodeListInterface::staticMetaObject );
}

QObject *
AQXmlObjectFactoryQSA::create( const QString & className,
                               const QVariantList &,
                               QObject * )
{
  if ( className == "AQDomNodeList" || className == "FLDomNodeList" )
    return new AQDomNodeListInterface( engine );

  interpreter() ->throwError( QLatin1String( "AQXmlObjectFactoryQSA::create Error : " ) + className );

  return 0;
}

AQObjectInterfaceQSA *
AQXmlObjectFactoryQSA::createInterface( const QString & className,
                                        const QVariantList & arguments,
                                        QObject * context )
{
  return qobject_cast<AQObjectInterfaceQSA *>
         ( create( className, arguments, context ) );
}

AQObjectInterfaceQSA *
AQXmlObjectFactoryQSA::createInterface( AQSharedObjectQSA )
{
  return 0;
}

AQObjectInterfaceQSA *
AQXmlObjectFactoryQSA::createInterface( QObject * )
{
  return 0;
}

bool AQXmlObjectFactoryQSA::canCreateInterface( const QString & ) const
{
  return false;
}
