/***************************************************************************
AQApplication.cpp
-------------------
begin                : 06/08/2007
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

#include <QStyleFactory>
#include <QSettings>
#include <QSignalMapper>
#include <QLocale>
#include <QTimer>
#include <QTextCodec>
#include <QDir>

#include <AQCore/AQCore.h>
#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQTranslator.h>
#include <AQScript/AQScriptEngineQSA.h>
#include <AQSql/AQSqlConnections.h>
#include <AQSql/AQSqlDatabase.h>
#include <AQSql/AQManagerModules.h>

#ifdef AQ_WARN
#include <AQSql/AQSqlCursor.h>
#include <AQSql/AQSqlQuery.h>
#endif

#include "AQApplication.h"
#include "AQUi.h"
#include "util/AQUiUtil.h"
#include "mainwindow/AQMainWindow.h"
#include "mainwindow/AQTabView.h"
#include "dialog/AQConnectDB.h"
#include "dialog/AQAbout.h"
#include "manager/AQUiManager.h"

#ifdef AQ_WARN
#include "AQAction.h"
#include "mainwindow/AQFormDB.h"
#endif

static void ( *qt_default_message_handler ) ( QtMsgType, const char *msg );

// Para interceptar y manejar mesajes
void aqMsgHandler( QtMsgType type, const char *msg )
{
  if ( !msg || QString( msg ).isEmpty() )
    return ;

  if ( qt_default_message_handler )
    qt_default_message_handler( type, msg );

  switch ( type ) {
    case QtDebugMsg:
      fprintf( stderr, "Debug: %s\n", msg );
      fflush( stderr );
      break;
    case QtWarningMsg:
      fprintf( stderr, "Warning: %s\n", msg );
      fflush( stderr );
      break;
    case QtCriticalMsg:
      AQ_MSGBOX_CRIT( AQ_TR( "MsgHandler", "Mensaje del Sistema" ), QString( msg ) );
      break;
    case QtFatalMsg:
      fprintf( stderr, "Fatal: %s\n", msg );
      abort();
  }

  QCoreApplication::flush();
}

AQApplication::AQApplication( int &argc, char **argv ) :
    QApplication( argc, argv ), tabView_( 0 ), connCount_( 0 )
{
  qt_default_message_handler = qInstallMsgHandler( aqMsgHandler );

  AQConfig::init();
  AQSqlConnections::init();

  setOrganizationName( "InfoSiAL" );
  setOrganizationDomain( "infosial.com" );
  setApplicationName( "AbanQ" );

  QTextCodec::setCodecForTr( QTextCodec::codecForName( "UTF-8" ) );
  QTextCodec::setCodecForLocale( QTextCodec::codecForName( "UTF-8" ) );
  QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );

  setLibraryPaths( QStringList() << AQPREFIX + "/plugins" << AQPREFIX + "/lib" << AQQTDIR + "/lib" );

  init();
}

AQApplication::~AQApplication()
{
  qInstallMsgHandler( 0 );

  finish();

  delete tabView_;

  AQSqlConnections::close();

  aqWarn( QLatin1String( "AQFormDB::countRefForm -> " ) + QString::number( AQFormDB::countRefForm ) );
  aqWarn( QLatin1String( "AQAction::countRefAction -> " ) + QString::number( AQAction::countRefAction ) );
  aqWarn( QLatin1String( "AQSqlCursor::countRefCursor -> " ) + QString::number( AQSqlCursor::countRefCursor ) );
  aqWarn( QLatin1String( "AQSqlQuery::countRefQuery -> " ) + QString::number( AQSqlQuery::countRefQuery ) );
}

QActionGroup * AQApplication::agStyles( QObject * parent ) const
{
  QActionGroup * ag = new QActionGroup( parent );

  ag->setExclusive( true );

  QSignalMapper * mapper = new QSignalMapper( ag );
  QAction * a;
  QStringList list( QStyleFactory::keys() );
  QString key;

  list.sort();

  for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
    key = *it;
    a = new QAction( key, ag );
    a->setCheckable( true );

    if ( key.toUpper() == curStyle_.toUpper() )
      a->setChecked( true );

    connect( a, SIGNAL( triggered() ), mapper, SLOT( map() ) );

    mapper->setMapping( a, key );
  }

  connect( mapper, SIGNAL( mapped( const QString & ) ), this, SLOT( setAQStyle( const QString & ) ) );

  return ag;
}

QActionGroup * AQApplication::agStyleSheets( QObject * parent ) const
{
  QActionGroup * ag = new QActionGroup( parent );

  ag->setExclusive( true );

  QSignalMapper * mapper = new QSignalMapper( ag );
  QAction * a;
  QDir d( ":/qss" );
  QStringList list( d.entryList( "*.qss" ) );
  QString sheet, key;

  for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
    key = *it;
    a = new QAction( key, ag );
    a->setCheckable( true );

    if ( key.toUpper() == curStyleSheet_.toUpper() )
      a->setChecked( true );

    connect( a, SIGNAL( triggered() ), mapper, SLOT( map() ) );

    mapper->setMapping( a, key );
  }

  connect( mapper, SIGNAL( mapped( const QString & ) ), this, SLOT( setAQStyleSheet( const QString & ) ) );

  return ag;
}

AQMainWindow * AQApplication::activeMainWindow() const
{
  return qobject_cast<AQMainWindow *> ( tabView_->currentWidget() );
}

AQMainWindow * AQApplication::mainWindow( const QString & connectionName ) const
{
  return mapMainWindow_.value( connectionName );
}

AQTabView * AQApplication::tabView() const
{
  return tabView_;
}

void AQApplication::setAQStyle( const QString & styleName )
{
  QSettings settings( QSettings::UserScope, "InfoSiAL" );

  if ( !styleName.isEmpty() ) {
    setStyle( styleName );
    settings.setValue( AQ_KEYBASE + "estilo", styleName );
    curStyle_ = styleName;
  } else {
    QString style_( settings.value( AQ_KEYBASE + "estilo", style() ->objectName() ).toString() );
    setStyle( style_ );
    curStyle_ = style_;
  }
}

void AQApplication::setAQStyleSheet( const QString & styleSheet )
{
  QSettings settings( QSettings::UserScope, "InfoSiAL" );

  if ( !styleSheet.isEmpty() ) {
    QFile file( ":/qss/" + styleSheet );

    file.open( QFile::ReadOnly );
    QString styleSheet_ = QLatin1String( file.readAll() );
    setStyleSheet( styleSheet_ );
    settings.setValue( AQ_KEYBASE + "qss", styleSheet );
    curStyleSheet_ = styleSheet;
  } else {
    QString sheet( settings.value( AQ_KEYBASE + "qss", "default.qss" ).toString() );
    QFile file( ":/qss/" + sheet );

    file.open( QFile::ReadOnly );

    QString styleSheet_( QLatin1String( file.readAll() ) );

    setStyleSheet( styleSheet_ );
    curStyleSheet_ = sheet;
  }
}

void AQApplication::aboutAbanQ()
{
  AQAbout aboutDialog( AQVERSION, activeWindow() );
  aboutDialog.exec();
}

bool AQApplication::tryExit()
{
  if ( AQ_No == AQ_MSGBOX_QUES( tr( "Salir..." ), tr( "¿ Quiere salir de AbanQ ?" ) ) )
    return false;

  if ( finish() ) {
    QTimer::singleShot( 500, this, SLOT( quit() ) );
    return true;
  }

  return false;
}

void AQApplication::newMainWindow()
{
  QString connName( connCount_ ? "conn" + QString::number( connCount_ ) : "default" );

  if ( openConnectionDB( connName ) ) {
    AQSqlDatabase db = AQSqlConnections::database( connName );
    QString dbName( db.database() );
    AQMainWindow * win = new AQMainWindow( tabView_, db );

    win->setObjectName( dbName );

    connect( win, SIGNAL( closeRequest( AQMainWindow * ) ), this, SLOT( closeMainWindow( AQMainWindow * ) ) );

    tabView_->insertTab( tabView_->count() + 1, win, dbName.toUpper() + " (" + connName + ")" );
    win->init();

    QSettings settings( QSettings::UserScope, "InfoSiAL" );

    win->restoreState( settings.value( "AbanQ/State/MainWidow/" + dbName ).toByteArray() );
    tabView_->setCurrentWidget( win );
    win->setFocus();

    mapMainWindow_.insert( connName, win );
    ++connCount_;
  }
}

void AQApplication::closeMainWindow( AQMainWindow * win )
{
  Q_ASSERT( win );

  QSettings settings( QSettings::UserScope, "InfoSiAL" );

  settings.setValue( "AbanQ/State/MainWidow/" + win->db().database(), win->saveState() );

  if ( tabView_->count() > 1 ) {
    if ( win->finish() ) {
      tabView_->closeTab( win );
      QTimer::singleShot( 0, win, SLOT( deleteLater() ) );

      QString connName( win->db().connectionName() );

      AQSqlConnections::removeDatabase( connName );
      mapMainWindow_.remove( connName );
    }
  } else
    tryExit();
}

void AQApplication::closeMainWindow( const QString & connectionName )
{
  closeMainWindow( mainWindow( connectionName ) );
}

void AQApplication::closeActiveMainWindow()
{
  closeMainWindow( activeMainWindow() );
}

void AQApplication::openTabMenu( const QPoint & pos )
{
  QTabBar * tabBar = tabView_->findChild<QTabBar *> ();
  int currTabIndex = -1;

  for ( int i = 0; i < tabBar->count(); ++i ) {
    if ( tabBar->tabRect( i ).contains( pos ) ) {
      currTabIndex = i;
      break;
    }
  }

  AQMainWindow * mw = qobject_cast<AQMainWindow *> ( tabView_->widget( currTabIndex ) );

  if ( mw ) {
    QMenu menu;
    QAction * a;

    tabView_->setCurrentWidget( mw );

    if ( tabView_->widgets().size() > 1 ) {
      a = new QAction( tr( "Cerrar las otras pestañas" ), &menu );
      connect( a, SIGNAL( triggered() ), this, SLOT( closeOtherTabs() ) );
      menu.addAction( a );
    }

    a = new QAction( tr( "Cerrar pestaña" ), &menu );
    a->setIcon( QIcon( ":/images/close.png" ) );

    connect( a, SIGNAL( triggered() ), this, SLOT( closeActiveMainWindow() ) );

    menu.addAction( a );
    menu.exec( tabBar->mapToGlobal( pos ) );
  }
}

void AQApplication::closeOtherTabs()
{
  AQMainWindow * win = activeMainWindow();
  AQMainWindow * winIt;
  QWidgetList widgets( tabView_->widgets() );

  for ( QWidgetList::const_iterator it = widgets.begin(); it != widgets.end(); ++it ) {
    winIt = qobject_cast<AQMainWindow *> ( *it );
    if ( winIt == win )
      continue;
    closeMainWindow( winIt );
  }
}

void AQApplication::init()
{
  loadTranslation( "sys." );
  loadTranslation( "qt_" );

  setAQStyle();
  setAQStyleSheet();

  tabView_ = new AQTabView;
  tabView_->setWindowTitle( "AbanQ " + AQVERSION );
  tabView_->setObjectName( "AbanQ " + AQVERSION );

  connect( tabView_, SIGNAL( closeTabClicked() ), this, SLOT(
             closeActiveMainWindow() ) );
  connect( tabView_, SIGNAL( newTabClicked() ), this, SLOT( newMainWindow() ) );
  connect( tabView_, SIGNAL( closeRequest() ), this, SLOT( tryExit() ) );

  QTabBar * tabBar = tabView_->findChild<QTabBar *> ();

  if ( tabBar ) {
    tabBar->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( tabBar, SIGNAL( customContextMenuRequested( const QPoint& ) ),
             this, SLOT( openTabMenu( const QPoint& ) ) );
  }

  QSettings settings( QSettings::UserScope, "InfoSiAL" );
  tabView_->restoreGeometry( settings.value( "AbanQ/Geometry/TabView/" + tabView_->objectName() ).toByteArray() );
  newMainWindow();

  if ( !connCount_ && finish() ) {
    QTimer::singleShot( 500, this, SLOT( quit() ) );
    return ;
  }

  AQScriptEngineQSA::registerInterfaceMetaObjects();

  tabView_->show();
}

bool AQApplication::finish()
{
  QSettings settings( QSettings::UserScope, "InfoSiAL" );
  settings.setValue( "AbanQ/Geometry/TabView/" + tabView_->objectName(), tabView_->saveGeometry() );

  tabView_->hide();

  QWidgetList list( tabView_->widgets() );
  AQMainWindow * win;

  for ( QWidgetList::const_iterator it = list.begin(); it != list.end(); ++it ) {
    win = qobject_cast<AQMainWindow *> ( *it );

    if ( win && win->finish() ) {
      tabView_->closeTab( win );
      QTimer::singleShot( 0, win, SLOT( deleteLater() ) );

      QString connName( win->db().connectionName() );

      AQSqlConnections::removeDatabase( connName );
      mapMainWindow_.remove( connName );
    } else
      return false;
  }

  return true;
}

bool AQApplication::openConnectionDB( const QString & connectionName )
{
  AQConnectDB connDialog( connectionName, tabView_ );
  int ret;
  bool error;

  do {
    ret = connDialog.exec();
    error = connDialog.error();
  } while ( error && ret != QDialog::Rejected );

  if ( error || ret == QDialog::Rejected )
    return false;

  AQSqlDatabase db = AQSqlConnections::database( connectionName );

  if ( !db.db().isOpen() )
    return false;

  if ( AQUiManager::updateFromAbanQ2( db ) ) {
    AQSqlConnections::removeDatabase( connectionName );
    AQConnectDB connDialog( connectionName, tabView_ );

    connDialog.tryConnect();
    if ( connDialog.error() )
      return false;
  }

  db = AQSqlConnections::database( connectionName );
  db.managerModules() ->loadResourceImages( AQUiManager::compileResourceImages( db ) );

  return true;
}

void AQApplication::loadTranslation( const QString & prefixFileTs, const QString & connectionName )
{
  QString Language( QLocale::system().name() );
  QString fileTs( prefixFileTs + Language.left( 2 ) + ".ts" );
  QString shaKey, cacheKey;
  QString
  contentTs( AQSqlConnections::database( connectionName ) .managerModules() ->content( fileTs, shaKey, cacheKey ) );

  if ( !contentTs.isEmpty() ) {
    AQTranslator * tor = new AQTranslator( this );
    tor->loadTsContent( cacheKey );
    installTranslator( tor );
  }
}
