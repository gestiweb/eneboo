/***************************************************************************
AQCoreObjectFactoryQSA.cpp
-------------------
begin                : 24/09/2007
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

#include <qsinterpreter.h>

#include <AQScript/AQScriptEngineQSA.h>

#include "AQCoreObjectFactoryQSA.h"
#include "AQUtilInterface.h"

AQCoreObjectFactoryQSA::AQCoreObjectFactoryQSA( AQScriptEngineQSA * eng ) :
    AQObjectFactoryQSA( eng )
{
  registerClass( "AQUtil", &AQUtilInterface::staticMetaObject,
                 new AQUtilInterface( engine ) );

  // Compatibilidad con AbanQ v2
  registerClass( "FLUtil", &AQUtilInterfaceAQ2::staticMetaObject );
  registerClass( "FLProcess", &QObject::staticMetaObject );
}

QObject *
AQCoreObjectFactoryQSA::create( const QString & className,
                                const QVariantList &,
                                QObject * )
{
  QString msgError;

  if ( className == "AQUtil" ) {
    aqWarn( engine->aqScriptAndLineState() +
            "\nLa forma 'new AQUtil' es obsoleta y ya no se soporta.\n"
            "Se puede utilizar AQUtil como objeto global estático. Ejemplo: AQUtil.getOS()" );
  }


  // Compatibilidad con AbanQ v2
  if ( className == "FLUtil" )
    return new AQUtilInterfaceAQ2( engine );

  if ( className == "FLProcess" ) {
    AQ_VOID_PORT_AQ2( FLProcess() );
    msgError = "\nSe debe utilizar la clase Process incluida en QSA";
  }

  interpreter() ->throwError( QLatin1String( "AQCoreObjectFactoryQSA::create Error : " ) + className +
                              msgError );

  return 0;
}

AQObjectInterfaceQSA *
AQCoreObjectFactoryQSA::createInterface( const QString & className, const QVariantList & arguments, QObject * context )
{
  return qobject_cast<AQObjectInterfaceQSA *>( create( className, arguments, context ) );
}

bool
AQCoreObjectFactoryQSA::canCreateInterface( const QString & ) const
{
  return false;
}
