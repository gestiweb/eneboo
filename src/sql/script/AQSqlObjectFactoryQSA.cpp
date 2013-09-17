/***************************************************************************
AQSqlObjectFactory.cpp
-------------------
begin                : 27/09/2007
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

#include "AQSqlInterface.h"
#include "AQSqlObjectFactoryQSA.h"
#include "AQSqlQueryInterface.h"
#include "AQVarInterface.h"
#define AQ_OBJ_FACTORY_IMPL
#include "AQSqlCursorInterface.h"

AQSqlObjectFactoryQSA::AQSqlObjectFactoryQSA( AQScriptEngineQSA * eng ) :
    AQObjectFactoryQSA( eng )
{
  registerClass( "AQSql", &AQSqlInterfaceStatic::staticMetaObject, new AQSqlInterfaceStatic );
  registerClass( "AQSqlCursor", &AQSqlCursorInterface::staticMetaObject );
  registerClass( "AQSqlQuery", &AQSqlQueryInterface::staticMetaObject );
  registerClass( "AQVar", &AQVarInterface::staticMetaObject );

  // Compatibilidad con AbanQ v2
  registerClass( "FLSqlCursor", &AQSqlCursorInterfaceAQ2::staticMetaObject );
  registerClass( "FLSqlQuery", &AQSqlQueryInterfaceAQ2::staticMetaObject );
  registerClass( "FLVar", &AQVarInterface::staticMetaObject );
}

QObject *
AQSqlObjectFactoryQSA::create( const QString & className,
                               const QVariantList & arguments,
                               QObject * )
{
  QString msgError;

  if ( className == "AQSqlCursor" ) {
    if ( arguments.size() == 1 )
      return new AQSqlCursorInterface( engine, arguments.at( 0 ).toString() );

    if ( arguments.size() == 2 )
      return new AQSqlCursorInterface( engine, arguments.at( 0 ).toString(),
                                       arguments.at( 1 ).toString() );

    msgError = "\nCandidate: AQSqlCursor( actionName );"
               "\nCandidate: AQSqlCursor( actionName, connectionName );\n";

  } else if ( className == "AQSqlQuery" ) {
    if ( arguments.size() == 0 )
      return new AQSqlQueryInterface( engine );

    if ( arguments.size() == 1 )
      return new AQSqlQueryInterface( engine, arguments.at( 0 ).toString() );

    if ( arguments.size() == 2 )
      return new AQSqlQueryInterface( engine, arguments.at( 0 ).toString(),
                                      arguments.at( 1 ).toString() );

    msgError = "\nCandidate: AQSqlQuery();"
               "\nCandidate: AQSqlQuery( queryName );"
               "\nCandidate: AQSqlQuery( queryName, connectionName );\n";

  } else if ( className == "AQVar" || className == "FLVar" ) {
    if ( arguments.size() == 0 )
      return new AQVarInterface( engine );

    if ( arguments.size() == 1 )
      return new AQVarInterface( engine, arguments.at( 0 ).toString() );

    msgError = "\nCandidate: AQVar();"
               "\nCandidate: AQVar( connectionName );\n";

  } else

    // Compatibilidad con AbanQ v2
    if ( className == "FLSqlCursor" ) {
      if ( arguments.size() == 1 )
        return new AQSqlCursorInterfaceAQ2( engine, arguments.at( 0 ).toString() );

      if ( arguments.size() == 2 )
        return new AQSqlCursorInterfaceAQ2( engine, arguments.at( 0 ).toString(),
                                            arguments.at( 1 ).toString() );

      return new AQSqlCursorInterfaceAQ2( engine, "aqvoid" );
    } else if ( className == "FLSqlQuery" ) {
      if ( arguments.size() == 1 )
        return new AQSqlQueryInterfaceAQ2( engine, arguments.at( 0 ).toString() );

      if ( arguments.size() == 2 )
        return new AQSqlQueryInterfaceAQ2( engine, arguments.at( 0 ).toString(),
                                           arguments.at( 1 ).toString() );

      return new AQSqlQueryInterfaceAQ2( engine );
    }

  interpreter() ->throwError( QLatin1String( "AQSqlObjectFactoryQSA::create Error : " ) + className +
                              msgError );

  return 0;
}

AQObjectInterfaceQSA *
AQSqlObjectFactoryQSA::createInterface( const QString & className,
                                        const QVariantList & arguments,
                                        QObject * context )
{
  return qobject_cast<AQObjectInterfaceQSA *>
         ( create( className, arguments, context ) );
}

AQObjectInterfaceQSA *
AQSqlObjectFactoryQSA::createInterface( AQSharedObjectQSA obj )
{
  if ( obj ) {
    QString className( obj.data() ->metaObject() ->className() );

    if ( className == "AQSqlCursor" )
      AQ_SCR_IFACE( 0, obj, AQSqlCursor );

    return qobject_cast<AQObjectInterfaceQSA *>
           ( create( className, QVariantList(), 0 ) );
  }

  return 0;
}

AQObjectInterfaceQSA *
AQSqlObjectFactoryQSA::createInterfaceAQ2( AQSharedObjectQSA obj )
{
  if ( obj ) {
    QString className( obj.data() ->metaObject() ->className() );

    if ( className == "AQSqlCursor" )
      AQ_SCR_IFACE_AQ2( 0, obj, AQSqlCursor );

    return qobject_cast<AQObjectInterfaceQSA *>
           ( create( className, QVariantList(), 0 ) );
  }

  return 0;
}

AQObjectInterfaceQSA *
AQSqlObjectFactoryQSA::createInterface( QObject * obj )
{
  if ( obj ) {
    QString className( obj->metaObject() ->className() );

    if ( className == "AQSqlCursor" )
      AQ_SCR_IFACE( obj, AQSharedObjectQSA(), AQSqlCursor );

    return qobject_cast<AQObjectInterfaceQSA *>
           ( create( className, QVariantList(), 0 ) );
  }

  return 0;
}

AQObjectInterfaceQSA *
AQSqlObjectFactoryQSA::createInterfaceAQ2( QObject * obj )
{
  if ( obj ) {
    QString className( obj->metaObject() ->className() );

    if ( className == "AQSqlCursor" )
      AQ_SCR_IFACE_AQ2( obj, AQSharedObjectQSA(), AQSqlCursor );

    return qobject_cast<AQObjectInterfaceQSA *>
           ( create( className, QVariantList(), 0 ) );
  }

  return 0;
}

bool AQSqlObjectFactoryQSA::canCreateInterface( const QString & className ) const
{
  return ( className == "AQSqlCursor" );
}
