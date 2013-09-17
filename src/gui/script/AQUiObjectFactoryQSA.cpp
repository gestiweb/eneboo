/***************************************************************************
AQUiObjectFactoryQSA.cpp
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
#include <AQScript/AQSqlCursorInterface.h>

#include "AQUiInterface.h"
#include "AQUiObjectFactoryQSA.h"
#include "AQApplicationInterface.h"
#define AQ_OBJ_FACTORY_IMPL
#include "AQMainWindowInterface.h"
#include "AQActionInterface.h"
#include "AQFieldDBInterface.h"
#include "AQTableDBInterface.h"
#include "AQFormDBInterface.h"
#include "AQ3TableInterface.h"
#include "AQ3ListViewInterface.h"
#include "AQ3ListViewItemInterface.h"
#include "AQTabWidgetInterface.h"
#include "AQSEditorInterface.h"
#include "AQCodBarInterface.h"
#include "AQReportViewerInterface.h"

class QtNamespace : public QObject
{
public:
  const QMetaObject * metaObject() const
  {
    return & QObject::staticQtMetaObject;
  }
};

AQUiObjectFactoryQSA::AQUiObjectFactoryQSA( AQScriptEngineQSA * eng ) :
    AQObjectFactoryQSA( eng )
{
  registerClass( "Qt", 0, new QtNamespace );
  registerClass( "AQUi", &AQUiInterfaceStatic::staticMetaObject, new AQUiInterfaceStatic );
  registerClass( "AbanQ", &AQApplicationInterface::staticMetaObject, new AQApplicationInterface( engine ) );
  registerClass( "AQAction", &AQActionInterface::staticMetaObject, new AQActionInterfaceStatic( eng ) );
  registerClass( "AQFormDB", &AQFormDBInterface::staticMetaObject );

  // Compatibilidad con AbanQ v2
  registerClass( "FLApplication", &AQApplicationInterfaceAQ2::staticMetaObject );
  registerClass( "FLFormDB", &AQFormDBInterfaceAQ2::staticMetaObject );
  registerClass( "FLFormRecordDB", &AQFormRecordDBInterfaceAQ2::staticMetaObject );
  registerClass( "FLFormSearchDB", &AQFormSearchDBInterfaceAQ2::staticMetaObject );
  registerClass( "FLCodBar", &AQCodBarInterfaceAQ2::staticMetaObject );
  registerClass( "FLScriptEditor", &AQSEditorInterfaceAQ2::staticMetaObject );
  registerClass( "FLReportViewer", &AQReportViewerInterfaceAQ2::staticMetaObject );
}

QObject * AQUiObjectFactoryQSA::create( const QString & className,
                                        const QVariantList & arguments,
                                        QObject * context )
{
  QString msgError;

  if ( className == "AQFormDB" ) {
    if ( arguments.size() == 0 )
      return new AQFormDBInterface( engine );

    msgError = "\nCandidate: AQFormDB()\n";

  } else if ( className == "AQAction" ) {
    if ( arguments.size() == 1 )
      return new AQActionInterface( engine, arguments.at( 0 ).toString() );

    if ( arguments.size() == 2 )
      return new AQActionInterface( engine, arguments.at( 0 ).toString(),
                                    arguments.at( 1 ).toString() );

    msgError = "\nCandidate: AQAction( actionName );"
               "\nCandidate: AQAction( actionName, connectionName );\n";

  } else

    // Compatibilidad con AbanQ v2
    if ( className == "FLApplication" && arguments.size() == 0 )
      return new AQApplicationInterfaceAQ2( engine );

    else if ( className == "FLFormDBNoUi" )
      return new AQFormDBInterfaceAQ2( engine, true );

    else if ( className == "FLFormDB" ) {
      if ( arguments.size() > 0 && arguments.size() < 4 )
        return new AQFormDBInterfaceAQ2( engine, arguments.at( 0 ).toString() );

      if ( arguments.size() == 4 )
        return new AQFormDBInterfaceAQ2( engine,
                                         arguments.at( 0 ).value<AQSqlCursorInterfaceAQ2 *>(),
                                         arguments.at( 1 ).toString() );

      if ( !arguments.isEmpty() || context )
        AQ_PORT_AQ2_NOP( FLFormDB, AQFormDB );

      return new AQFormDBInterfaceAQ2( engine );

    } else if ( className == "FLFormRecordDBNoUi" )
      return new AQFormRecordDBInterfaceAQ2( engine, true );

    else if ( className == "FLFormRecordDB" ) {
      if ( arguments.size() == 4 )
        return new AQFormRecordDBInterfaceAQ2( engine,
                                               arguments.at( 0 ).value<AQSqlCursorInterfaceAQ2 *>(),
                                               arguments.at( 1 ).toString() );

      if ( !arguments.isEmpty() || context )
        AQ_PORT_AQ2_NOP( FLFormRecordDB, AQFormDB );

      return new AQFormRecordDBInterfaceAQ2( engine );

    } else if ( className == "FLFormSearchDBNoUi" )
      return new AQFormSearchDBInterfaceAQ2( engine, true );

    else if ( className == "FLFormSearchDB" ) {
      if ( arguments.size() == 1 )
        return new AQFormSearchDBInterfaceAQ2( engine, arguments.at( 0 ).toString() );

      if ( arguments.size() == 2 )
        return new AQFormSearchDBInterfaceAQ2( engine,
                                               arguments.at( 0 ).value<AQSqlCursorInterfaceAQ2 *>(),
                                               arguments.at( 1 ).toString() );

      if ( !arguments.isEmpty() || context )
        AQ_PORT_AQ2_NOP( FLFormSearchDB, AQFormDB );

      return new AQFormSearchDBInterfaceAQ2( engine );

    } else if ( className == "FLCodBar" ) {
      return new AQCodBarInterfaceAQ2( engine,
                                       arguments.at( 0 ).toString(), arguments.at( 1 ).toInt(),
                                       arguments.at( 2 ).toInt(), arguments.at( 3 ).toDouble(),
                                       arguments.at( 4 ).toDouble(), arguments.at( 5 ).toInt(),
                                       arguments.at( 6 ).toBool() );

    } else if ( className == "FLScriptEditor" ) {
      return new AQSEditorInterfaceAQ2( engine,
                                        arguments.at( 0 ).toString() );

    } else if ( className == "FLReportViewer" ) {
      return new AQReportViewerInterfaceAQ2( engine );
    }


  interpreter() ->throwError( QLatin1String( "AQUiObjectFactoryQSA::create Error : " ) + className +
                              msgError );

  return 0;
}

AQObjectInterfaceQSA *
AQUiObjectFactoryQSA::createInterface( const QString & className,
                                       const QVariantList & arguments,
                                       QObject * context )
{
  return qobject_cast<AQObjectInterfaceQSA *>( create( className, arguments, context ) );
}

AQObjectInterfaceQSA *
AQUiObjectFactoryQSA::createInterface( AQSharedObjectQSA obj )
{
  if ( obj ) {
    QString className( obj.data() ->metaObject() ->className() );

    if ( className == "AQMainWindow" )
      AQ_SCR_IFACE( 0, obj, AQMainWindow );

    if ( className == "AQAction" )
      AQ_SCR_IFACE( 0, obj, AQAction );

    if ( className == "AQFieldDB" )
      AQ_SCR_IFACE( 0, obj, AQFieldDB );

    if ( className == "AQTableDB" )
      AQ_SCR_IFACE( 0, obj, AQTableDB );

    if ( className == "AQFormDB" )
      AQ_SCR_IFACE( 0, obj, AQFormDB );

    return qobject_cast<AQObjectInterfaceQSA *>( create( className, QVariantList(), 0 ) );
  }

  return 0;
}

AQObjectInterfaceQSA *
AQUiObjectFactoryQSA::createInterfaceAQ2( AQSharedObjectQSA obj )
{
  if ( obj ) {
    QString className( obj.data() ->metaObject() ->className() );

    if ( className == "AQFieldDB" )
      AQ_SCR_IFACE_AQ2( 0, obj, AQFieldDB );

    if ( className == "AQTableDB" )
      AQ_SCR_IFACE_AQ2( 0, obj, AQTableDB );

    if ( className == "AQFormDB" )
      AQ_SCR_IFACE_AQ2( 0, obj, AQFormDB );

    if ( className == "Q3Table" || className == "AQ3Table" )
      AQ_SCR_IFACE_AQ2( 0, obj, AQ3Table );

    if ( className == "Q3ListView" || className == "AQ3ListView" )
      AQ_SCR_IFACE_AQ2( 0, obj, AQ3ListView );

    if ( className == "Q3ListViewItem" || className == "AQ3ListViewItem" )
      AQ_SCR_IFACE_AQ2( 0, obj, AQ3ListViewItem );

    if ( className == "QTabWidget" || className == "AQTabWidget" )
      AQ_SCR_IFACE_AQ2( 0, obj, AQTabWidget );

    if ( className == "AQCodBar" )
      AQ_SCR_IFACE_AQ2( 0, obj, AQCodBar );

    if ( className == "AQSEditor" )
      AQ_SCR_IFACE_AQ2( 0, obj, AQSEditor );

    if ( className == "AQReportViewer" )
      AQ_SCR_IFACE_AQ2( 0, obj, AQReportViewer );

    return qobject_cast<AQObjectInterfaceQSA *>( create( className, QVariantList(), 0 ) );
  }

  return 0;
}

AQObjectInterfaceQSA *
AQUiObjectFactoryQSA::createInterface( QObject * obj )
{
  if ( obj ) {
    QString className( obj->metaObject() ->className() );

    if ( className == "AQMainWindow" )
      AQ_SCR_IFACE( obj, AQSharedObjectQSA(), AQMainWindow );

    if ( className == "AQAction" )
      AQ_SCR_IFACE( obj, AQSharedObjectQSA(), AQAction );

    if ( className == "AQFieldDB" )
      AQ_SCR_IFACE( obj, AQSharedObjectQSA(), AQFieldDB );

    if ( className == "AQTableDB" )
      AQ_SCR_IFACE( obj, AQSharedObjectQSA(), AQTableDB );

    if ( className == "AQFormDB" )
      AQ_SCR_IFACE( obj, AQSharedObjectQSA(), AQFormDB );

    return qobject_cast<AQObjectInterfaceQSA *>( create( className, QVariantList(), 0 ) );
  }

  return 0;
}

AQObjectInterfaceQSA *
AQUiObjectFactoryQSA::createInterfaceAQ2( QObject * obj )
{
  if ( obj ) {
    QString className( obj->metaObject() ->className() );

    if ( className == "AQFieldDB" || className == "FLFieldDB" )
      AQ_SCR_IFACE_AQ2( obj, AQSharedObjectQSA(), AQFieldDB );

    if ( className == "AQTableDB" || className == "FLTableDB" )
      AQ_SCR_IFACE_AQ2( obj, AQSharedObjectQSA(), AQTableDB );

    if ( className == "AQFormDB" )
      AQ_SCR_IFACE_AQ2( obj, AQSharedObjectQSA(), AQFormDB );

    if ( className == "Q3Table" || className == "AQ3Table" )
      AQ_SCR_IFACE_AQ2( obj, AQSharedObjectQSA(), AQ3Table );

    if ( className == "Q3ListView" || className == "AQ3ListView" )
      AQ_SCR_IFACE_AQ2( obj, AQSharedObjectQSA(), AQ3ListView );

    if ( className == "Q3ListViewItem" || className == "AQ3ListViewItem" )
      AQ_SCR_IFACE_AQ2( obj, AQSharedObjectQSA(), AQ3ListViewItem );

    if ( className == "QTabWidget" || className == "AQTabWidget" )
      AQ_SCR_IFACE_AQ2( obj, AQSharedObjectQSA(), AQTabWidget );

    if ( className == "AQCodBar" )
      AQ_SCR_IFACE_AQ2( obj, AQSharedObjectQSA(), AQCodBar );

    if ( className == "AQSEditor" )
      AQ_SCR_IFACE_AQ2( obj, AQSharedObjectQSA(), AQSEditor );

    if ( className == "AQReportViewer" )
      AQ_SCR_IFACE_AQ2( obj, AQSharedObjectQSA(), AQReportViewer );

    return qobject_cast<AQObjectInterfaceQSA *>( create( className, QVariantList(), 0 ) );
  }

  return 0;
}

bool AQUiObjectFactoryQSA::canCreateInterface( const QString & className ) const
{
  return ( className == "AQFormDB" || className == "AQMainWindow" ||
           className == "AQAction" || className == "AQFieldDB" ||
           className == "AQTableDB" || className == "Q3Table" ||
           className == "AQ3Table" || className == "Q3ListView" ||
           className == "AQ3ListView" || className == "Q3ListViewItem" ||
           className == "AQ3ListViewItem" || className == "QTabWidget" ||
           className == "AQTabWidget" || className == "AQCodBar" ||
           className == "AQSEditor" || className == "AQReportViewer" );
}
