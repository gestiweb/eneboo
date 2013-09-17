/***************************************************************************
AQApplicationInterface.cpp
-------------------
begin                : 24/09/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
email                : mail@infosial.com
***************************************************************************/
/***************************************************************************
 *   This program is free software{} you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation{} version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************
   Este  programa es software libre. Puede redistribuirlo y/o modificarlo
   bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
   versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#include <QGroupBox>

#include <AQCore/AQDiskCache.h>
#include <AQCore/AQActionMD.h>
#include <AQSql/AQSqlConnections.h>

#include "AQApplicationInterface.h"
#include "AQMainWindowInterface.h"
#include "../AQApplication.h"
#include "../util/AQUiUtil.h"
#include "../manager/AQUiManager.h"
#include "../component/AQFieldDB.h"
#include "../component/AQTableDB.h"

AQMainWindowInterface * AQApplicationInterface::activeMainWindow() const
{
#ifndef AQ_WUI
  return qobject_cast<AQMainWindowInterface *>
         ( engine->createInterface( AbanQ->activeMainWindow() ) );
#endif
}

AQMainWindowInterface * AQApplicationInterface::mainWindow( const QString & connectionName ) const
{
#ifndef AQ_WUI
  return qobject_cast<AQMainWindowInterface *>
         ( engine->createInterface( AbanQ->mainWindow( connectionName ) ) );
#endif
}

void AQApplicationInterface::newMainWindow()
{
#ifndef AQ_WUI
  AbanQ->newMainWindow();
#endif
}

void AQApplicationInterface::closeMainWindow( const QString & connectionName )
{
#ifndef AQ_WUI
  AbanQ->closeMainWindow( connectionName );
#endif
}

void AQApplicationInterface::processEvents()
{
#ifndef AQ_WUI
  AbanQ->processEvents();
#endif
}

QString AQApplicationInterface::getWidgetList( const QString & wN, const QString & c, const QString & connName ) const
{
  AQSqlDatabase db = dbCon( connName );
  QString actionName;
  AQActionMD * a;
  QWidget * w = 0;

  if ( wN.left( 10 ) == "formRecord" ) {
    actionName = wN.mid( 10, wN.length() );
    a = db.manager() ->action( actionName );

    if ( a )
      w = AQUiManager::createUI( a->formRecord(), 0, db );

  } else if ( wN.left( 10 ) == "formSearch" ) {
    actionName = wN.mid( 10, wN.length() );
    a = db.manager() ->action( actionName );

    if ( a )
      w = AQUiManager::createUI( a->form(), 0, db );

  } else {
    actionName = wN.mid( 4, wN.length() );
    a = db.manager() ->action( actionName );

    if ( a )
      w = AQUiManager::createUI( a->form(), 0, db );
  }

  if ( !w )
    return QString();

  QWidgetList list( w->findChildren<QWidget *>( c ) );
  QWidget * o;
  QString s;
  QString name;
  QString fieldTable;

  for ( QWidgetList::const_iterator it = list.begin(); it != list.end(); ++it ) {
    o = *it;
    name = o->objectName();

    if ( c == "FLFieldDB" || c == "AQFieldDB" ) {
      fieldTable = static_cast<AQFieldDB *>( o ) ->tableName();

      if ( !fieldTable.isEmpty() && fieldTable != a->table() )
        continue;

      s += name + "/" + static_cast<AQFieldDB *>( o ) ->fieldName() + "*";

    } else if ( c == "FLTableDB" || c == "AQTableDB" ) {
      s += name + "/" + static_cast<AQTableDB *>( o ) ->tableName() + "*";

    } else if ( c == "QButton" || c == "Button" ) {
      if ( name == "pushButtonDB" || name == "pbAux" || name == "qt_left_btn" || name == "qt_right_btn" )
        continue;

      s += name + "/" + static_cast<QAbstractButton *>( o ) ->text() + "*";

    } else {
      if ( name == "textLabelDB" || name == "componentDB" || name == "tab pages" ||
           name == "editor" || name == "FrameFind" || name == "TextLabelSearch" ||
           name == "TextLabelIn" || name == "lineEditSearch" || name == "in-combo" || name == "voidTable" )
        continue;

      if ( qobject_cast<QGroupBox *>( o ) )
        s += name + "/" + static_cast<QGroupBox *>( o ) ->title() + "*";
      else
        s += name + "/ " + "*";
    }
  }

  delete w;
  return s;
}

QString AQApplicationInterface::getTabWidgetPages( const QString & wN, const QString & n, const QString & connName ) const
{
  AQSqlDatabase db = dbCon( connName );
  QString actionName;
  AQActionMD * a;
  QWidget * w = 0;

  if ( wN.left( 10 ) == "formRecord" ) {
    actionName = wN.mid( 10, wN.length() );
    a = db.manager() ->action( actionName );

    if ( a )
      w = AQUiManager::createUI( a->formRecord(), 0, db );

  } else if ( wN.left( 10 ) == "formSearch" ) {
    actionName = wN.mid( 10, wN.length() );
    a = db.manager() ->action( actionName );

    if ( a )
      w = AQUiManager::createUI( a->form(), 0, db );

  } else {
    actionName = wN.mid( 4, wN.length() );
    a = db.manager() ->action( actionName );

    if ( a )
      w = AQUiManager::createUI( a->form(), 0, db );
  }

  if ( !w )
    return QString();

  QTabWidget * tabW = w->findChild<QTabWidget *>( n );
  if ( !tabW )
    return QString();

  QString s;
  QWidget * page;

  for ( int i = 0; i < tabW->count(); ++i ) {
    page = tabW->page( i );
    s += QString( page->name() ) + "/" + QString( page->caption() ) + "*";
  }

  delete w;
  return s;
}

QString AQApplicationInterface::diskCacheAbsDirPath() const
{
  return AQ_DISKCACHE_DIRPATH();
}

bool AQApplicationInterface::isLoadedModule( const QString & idM, const QString & connName ) const
{
  return dbCon( connName ).managerModules() ->listAllIdModules().contains( idM );
}

QString AQApplicationInterface::versionModule( const QString & idM, const QString & connName ) const
{
  return dbCon( connName ).managerModules() ->versionModule( idM );
}

QString AQApplicationInterface::nameDriver( const QString & connName ) const
{
  return dbCon( connName ).driverName();
}

QString AQApplicationInterface::nameUser( const QString & connName ) const
{
  return dbCon( connName ).user();
}

QString AQApplicationInterface::nameBD( const QString & connName ) const
{
  return dbCon( connName ).database();
}

QString AQApplicationInterface::nameHost( const QString & connName ) const
{
  return dbCon( connName ).host();
}

QString AQApplicationInterface::idSession( const QString & connName ) const
{
  return dbCon( connName ).idSessionGlobal();
}

bool
AQApplicationInterface::addDatabase( const QString & driverAlias, const QString & nameDB,
                                     const QString & user, const QString & password, const QString & host,
                                     int port, const QString & connectionName ) const
{
  return AQSqlConnections::addDatabase( driverAlias, nameDB, user, password, host, port, connectionName );
}

bool AQApplicationInterface::removeDatabase( const QString & connectionName ) const
{
  return AQSqlConnections::removeDatabase( connectionName );
}

QString AQApplicationInterface::toUnicode( const QString & text, const QString & codecName ) const
{
  QTextCodec * codec = QTextCodec::codecForName( codecName );
  if ( codec )
    return codec->toUnicode( text );
  else
    return text;
}

QByteArray AQApplicationInterface::fromUnicode( const QString & text, const QString & codecName ) const
{
  QTextCodec * codec = QTextCodec::codecForName( codecName );
  if ( codec )
    return codec->fromUnicode( text );
  else
    return text.utf8();
}

void AQApplicationInterfaceAQ2::startTest( const QString &, const QString &, const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::startTest );
}

AQMainWindowInterface * AQApplicationInterfaceAQ2::mainWidget() const
{
  AQ_PORT_AQ2( AQApplication::mainWidget,
               return activeMainWindow(); );
}

void AQApplicationInterfaceAQ2::openMasterForm( const QString & actionName, const QPixmap & )
{
  AQ_PORT_AQ2( AQApplication::openMasterForm( actionName ),
               activeMainWindow() ->activateAction( actionName ); );
}

void AQApplicationInterfaceAQ2::statusHelpMsg( const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::statusHelpMsg() );
}

void AQApplicationInterfaceAQ2::execMainScript( const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::execMainScript() );
}

QString AQApplicationInterfaceAQ2::testSessionsList( const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::testSessionsList() );
  return QString();
}

void AQApplicationInterfaceAQ2::updateAreas()
{
  AQ_VOID_PORT_AQ2( AQApplication::updateAreas() );
}

bool AQApplicationInterfaceAQ2::isTestEnabled()
{
  AQ_VOID_PORT_AQ2( AQApplication::isTestEnabled() );
  return false;
}

QString * AQApplicationInterfaceAQ2::nameObjectScript( const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::nameObjectScript() );
  return 0;
}

QVariant AQApplicationInterfaceAQ2::call( const QString &, const QVariantList &, const QString &, const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::call() );
  return QVariant();
}

QVariant AQApplicationInterfaceAQ2::call( const QString &, const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::call() );
  return QVariant();
}

void AQApplicationInterfaceAQ2::continueTesting( const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::continueTesting() );
}

void AQApplicationInterfaceAQ2::setCaptionMainWidget( const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::setCaptionMainWidget() );
}

QString AQApplicationInterfaceAQ2::lastTextCaption() const
{
  AQ_VOID_PORT_AQ2( AQApplication::lastTextCaption() );
  return QString();
}

void AQApplicationInterfaceAQ2::reinit()
{
  AQ_VOID_PORT_AQ2( AQApplication::reinit );
}

void AQApplicationInterfaceAQ2::cleanupMetaData( const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::cleanupMetaData );
}

void AQApplicationInterfaceAQ2::Mr_Proper( const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::Mr_Proper() );
}

QString AQApplicationInterfaceAQ2::connectionNameForAction( const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::connectionNameForAction() );
  return QString();
}

void AQApplicationInterfaceAQ2::setReimpMethod( const QString &, const QString & )
{
  AQ_VOID_PORT_AQ2( AQApplication::setReimpMethod() );
}

bool AQApplicationInterfaceAQ2::existsReimpMethod( const QString & ) const
{
  AQ_VOID_PORT_AQ2( AQApplication::existsReimpMethod() );
  return false;
}

QVariant AQApplicationInterfaceAQ2::callReimp( const QString &, const QVariantList & )
{
  AQ_VOID_PORT_AQ2( AQApplication::callReimp() );
  return QVariant();
}

AQApplication * AQApplicationInterfaceAQ2::obj() const
{
  AQ_VOID_PORT_AQ2( AQApplication::obj() );
  return 0;
}

#include "moc_AQApplicationInterface.cpp"
