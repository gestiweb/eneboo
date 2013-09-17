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

#include <iostream>

#include <WFitLayout>
#include <WServer>
#include <WTimer>

#include <Ext/Panel>

#include <QLocale>
#include <QDateTime>
#include <QTextCodec>
#include <QThread>
#include <QDir>
#include <QSettings>

#include <AQCore/AQCore.h>
#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQTranslator.h>
#include <AQCore/AQMemCache.h>
#include <AQUi/AQUiUtil.h>
#include <AQUi/AQUiManager.h>
#include <AQUi/AQAbout.h>
#include <AQScript/AQScriptEngineQSA.h>
#include <AQSql/AQSqlConnections.h>
#include <AQSql/AQSqlDatabase.h>
#include <AQSql/AQManagerModules.h>
#ifdef AQ_DEBUG
#include <AQSql/AQSqlCursor.h>
#include <AQSql/AQSqlQuery.h>
#endif

#include "wtqt/WQApplication.h"
#include "wtqt/AQDispatchThread.h"
#include "mainwindow/AQMainWindow.h"
#include "mainwindow/AQTabView.h"
#include "dialog/AQConnectDB.h"
#include "webgets/WQWidgetDialog.h"
#include "AQApplication.h"
#include "AQApplication_p.h"

static void ( *qt_default_message_handler ) ( QtMsgType, const char *msg );

// Para interceptar y manejar mesajes
void aqMsgHandler( QtMsgType type, const char *msg )
{
  if ( !msg )
    return ;

  QString strMsg( msg );

  if ( strMsg.isEmpty() || strMsg.contains( "QPixmap:" ) )
    return ;

  if ( !AbanQWT )
    return ;

  if ( qt_default_message_handler )
    qt_default_message_handler( type, msg );

  switch ( type ) {
    case QtDebugMsg:
      wuiDebugShortLog( "Debug: " << msg );
      fflush( stderr );
      break;
    case QtWarningMsg:
      wuiShortLog( "Warning: " << msg );
      fflush( stderr );
      break;
    case QtCriticalMsg:
      AQ_MSGBOX_CRIT( AQ_TR( "Critical", "Mensaje del Sistema" ), QString( msg ) );
      break;
    case QtFatalMsg:
      fprintf( stderr, "Fatal: %s\n", msg );
      abort();
  }
}

class AQSignalThread : public QThread
{
public:

  AQSignalThread( AQApplicationWebPrivate * d ) :
      QThread(), d_( d )
  {}

protected:

  void run();

private:

  AQApplicationWebPrivate * d_;
};

class AQApplicationWebPrivate
{
public:

  AQApplicationWebPrivate( sigset_t * set ) :
      wServer_( 0 ), lastWQApp_( 0 ), set_( set ), sigThread_( 0 ), quited_( false ), quitOnLastClosed_( false )
  {}

  void init();
  void finish();
  void interrupt();
  void addSessionId( const QString & sessionId, AQApplication * aqApp );
  void removeSessionId( const QString & sessionId );
  void closeAllSessions();

  void registerMetaTypes();

  WServer * wServer_;
  WQApplication * lastWQApp_;
  sigset_t * set_;
  AQSignalThread * sigThread_;
  QStringList sessionsIds_;
  QHash<QString, AQApplication *> hashAQApps_;
  bool quited_;
  bool quitOnLastClosed_;
};

void AQSignalThread::run()
{
  int sig = 0, err;
  do {
    err = sigwait( d_->set_, &sig );
  } while ( err != 0 );
  if ( err == 0 ) {
    std::cerr << "Shutdown (signal = " << sig << ")" << std::endl;
    d_->interrupt();
  }
  QThread::exec();
}

void AQApplicationWebPrivate::init()
{
  sigThread_ = new AQSignalThread( this );
  sigThread_->start();
}

void AQApplicationWebPrivate::finish()
{
  if ( quited_ )
    return ;
  else
    quited_ = true;

  if ( sigThread_ ) {
    if ( sigThread_->isRunning() ) {
      std::cerr << "Terminated" << std::endl;
      sigThread_->QThread::exit();
    }
    sigThread_->wait();
    delete sigThread_;
    sigThread_ = 0;
  }

  if ( wServer_ ) {
    try {
      std::cerr << "Stopping Web Server" << std::endl;
      qInstallMsgHandler( 0 );
      wServer_->stop();
      delete wServer_;
      wServer_ = 0;
    } catch ( WServer::Exception & e ) {
      std::cerr << e.what() << "\n";
    }
    catch ( std::exception & e ) {
      std::cerr << "exception: " << e.what() << "\n";
    }
  }
}

void AQApplicationWebPrivate::interrupt()
{
  if ( sessionsIds_.size() ) {
    quitOnLastClosed_ = true;
    closeAllSessions();
  } else
    AbanQ->quit();
}

void AQApplicationWebPrivate::addSessionId( const QString & sessionId, AQApplication * aqApp )
{
  if ( !sessionsIds_.contains( sessionId ) ) {
    sessionsIds_ << sessionId;
    hashAQApps_.insert( sessionId, aqApp );
  }
}

void AQApplicationWebPrivate::removeSessionId( const QString & sessionId )
{
  if ( sessionsIds_.contains( sessionId ) ) {
    sessionsIds_.removeAll( sessionId );
    hashAQApps_.remove( sessionId );
  }

  if ( quitOnLastClosed_ && !sessionsIds_.size() )
    AbanQ->quit();
}

void AQApplicationWebPrivate::closeAllSessions()
{
  QHash<QString, AQApplication *>::iterator it;
  for ( int i = 0; i < sessionsIds_.size(); ++i ) {
    it = hashAQApps_.find( sessionsIds_.at( i ) );
    if ( it != hashAQApps_.end() )
      ( *it ) ->interrupt();
  }
}

void AQApplicationWebPrivate::registerMetaTypes()
{
  qRegisterMetaType<QAction*>();
}

AQApplicationWeb::AQApplicationWeb( int &argc, char **argv, sigset_t * set ) :
    QApplication( argc, argv ), d( new AQApplicationWebPrivate( set ) )
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

  d->registerMetaTypes();

  try {
    d->wServer_ = new WServer( argv[ 0 ] );
    d->wServer_->setServerConfiguration( argc, argv, WTHTTP_CONFIGURATION );
#ifndef WT_SERIES_2

    d->wServer_->addEntryPoint( Wt::Application, &AQApplicationWeb::createWApp );
#else

    d->wServer_->addEntryPoint( WServer::Application, &AQApplicationWeb::createWApp );
#endif

    if ( !d->wServer_->start() ) {
      std::cerr << "Web Server not started. Quitting." << std::endl;
      d->finish();
    } else
      d->init();
  } catch ( WServer::Exception & e ) {
    std::cerr << e.what() << "\n";
  }
  catch ( std::exception & e ) {
    std::cerr << "exception: " << e.what() << "\n";
  }
}

AQApplicationWeb::~AQApplicationWeb()
{
  d->finish();
  delete d;

  AQSqlConnections::close();
  wuiDebugShortLog( toWString( QLatin1String( "AQSqlCursor::countRefCursor-> " ) + QString::number( AQSqlCursor::countRefCursor ) ) );
  wuiDebugShortLog( toWString( QLatin1String( "AQSqlCursor::countRefQuery-> " ) + QString::number( AQSqlQuery::countRefQuery ) ) );
}

bool AQApplicationWeb::notify( QObject * receiver, QEvent * event )
{
  QEvent::Type evType = event->type();

  if ( evType == QEvent::Show )
    return true;

  if ( evType != QEvent::ThreadChange && evType != QEvent::ChildPolished &&
       evType != QEvent::PolishRequest && evType != QEvent::ChildInsertedRequest &&
       evType != QEvent::ChildInserted && evType != QEvent::LayoutRequest ) {

    if ( !receiver->isWidgetType() && evType == QEvent::MetaCall ) {

      AQDispatchThread * recThread = qobject_cast<AQDispatchThread *> ( receiver->thread() );

      if ( recThread ) {

        wuiDebugShortLog( "AQ_NOTIFY INI : " << receiver << "  " << event << "  crThr: " << QThread::currentThread() );

        bool ret = QCoreApplication::notify( receiver, event );

        recThread->wqApp() ->releaseInvocationHead();

        wuiDebugShortLog( "AQ_NOTIFY FIN : " << event );

        return ret;
      }

    } else if ( evType != QEvent::DynamicPropertyChange && WQWidget::isWebget( receiver ) ) {

      WQWidget::webget( receiver ) ->qWidgetEvent( event );

    }

    if ( !receiver->isWidgetType() && evType > AQCoreEvent::MinInternal ) {
      WQWidget * wq = qobject_cast<WQWidget *>( receiver );

      if ( wq )
        wq->qWidgetEvent( event );
    }
  }

  return QCoreApplication::notify( receiver, event );
}

void AQApplicationWeb::aboutAbanQ()
{
  WQWidgetDialog wqAboutDialog( AQAbout::staticMetaObject );
  wqAboutDialog.exec();
}


WApplication * AQApplicationWeb::createWApp( const WEnvironment& env )
{
  WQApplication * wqApp;

  AbanQ->d->lastWQApp_ = wqApp = new WQApplication( env );

  return wqApp;
}

WQApplication * AQApplicationWeb::wqAppInstance()
{

  WQApplication * wqApp = static_cast<WQApplication *> ( WApplication::instance() );

  if ( !wqApp )
    wqApp = AbanQ->d->lastWQApp_;

  Q_ASSERT( wqApp );

  return wqApp;
}

QString AQApplicationWeb::findIconCache( const QString & key )
{
  QString ret;
  if ( wqAppInstance() ->aqApp_->d_func() ->iconsCache_.find( key, ret ) ) {
    aqDebug( "Hit IconCache : " << key );
    return ret;
  }
  return QString();
}

void AQApplicationWeb::insertIconCache( const QString & key, const QString & val )
{
  wqAppInstance() ->aqApp_->d_func() ->iconsCache_.insert( key, val );
}

AQApplicationPrivate::AQApplicationPrivate( WQApplication * wqApp ) :
    WQWidgetPrivate(), wqApp_( wqApp ), tabView_( 0 ), containerPanel_( 0 ), connCount( 0 ), timeUser_( 0 ),
    quited_( false ), interrupt_( false )
{}

void AQApplicationPrivate::init()
{
  AQ_Q( AQApplication );

  sessionId_ = QString::fromStdString( wqApp_->sessionId() );
  wc_ = wqApp_->root();
  containerPanel_ = new Ext::Panel( wc_ );
  containerPanel_->setLayout( new WFitLayout );
  timeUser_ = new QDateTime( QDateTime::currentDateTime() );

  AbanQ->d->addSessionId( sessionId_, q );

  //loadTranslation( "sys." );
  //loadTranslation( "qt_" );

  q->newMainWindow();
}

void AQApplicationPrivate::finish()
{
  if ( tabView_ ) {
    WQWidgetList list( tabView_->widgets() );
    AQMainWindow * win;
    for ( WQWidgetList::const_iterator it = list.begin(); it != list.end(); ++it ) {
      win = static_cast<AQMainWindow *> ( *it );
      if ( win ) {
        win->finish();
        tabView_->closeTab( win );
        closeConnectionDB( win->db().connectionName() );
        delete win;
      }
    }
  }

  if ( timeUser_ )
    delete timeUser_;
  AbanQ->d->removeSessionId( sessionId_ );
}

void AQApplicationPrivate::initTabView()
{
  if ( !tabView_ ) {
    AQ_Q( AQApplication );
    tabView_ = new AQTabView( q, true, false );
    tabView_->QObject::setObjectName( "AbanQ " + AQVERSION );
    q->addWQWidget( tabView_ );
    QObject::connect( tabView_, SIGNAL( closeTabClicked() ), q, SLOT( closeMainWindow() ) );
    QObject::connect( tabView_, SIGNAL( newTabClicked() ), q, SLOT( newMainWindow() ) );
  }
}

bool AQApplicationPrivate::execDialogConnectDB( const QString & connectionName )
{
  AQConnectDB connDialog( connectionName );
  int ret;
  bool error;

  do {
    ret = connDialog.exec();
    if ( quited_ || interrupt_ )
      return false;
    error = connDialog.error();
  } while ( error && ret != AQUi::Rejected );

  bool connOk = ( !error && AQSqlConnections::database( connectionName ).db().isOpen() && ret != AQUi::Rejected );

  if ( connOk ) {
    AQSqlDatabase db = AQSqlConnections::database( connectionName );
    db.managerModules() ->loadResourceImages( AQUiManager::compileResourceImages( db ) );
  }

  return connOk;
}

bool AQApplicationPrivate::openDefaultConnectionDB( const QString & connectionName )
{
  QSettings settings( QSettings::UserScope, "InfoSiAL" );

  bool connOk =
    AQSqlConnections::addDatabase( settings.value( "AbanQ/DBA/db", AQSqlDatabase::defaultAlias() ).toString(),
                                   settings.value( "AbanQ/DBA/lastDB", "abanq" ).toString(),
                                   settings.value( "AbanQ/DBA/username" ).toString(),
                                   settings.value( "AbanQ/DBA/password" ).toString(),
                                   settings.value( "AbanQ/DBA/hostname", "localhost" ).toString(),
                                   settings.value( "AbanQ/DBA/port", "5432" ).toInt(), connectionName );

  if ( connOk ) {
    AQSqlDatabase db = AQSqlConnections::database( connectionName );
    db.managerModules() ->loadResourceImages( AQUiManager::compileResourceImages( db ) );
  }

  return connOk;
}

bool AQApplicationPrivate::closeConnectionDB( const QString & connectionName )
{
  bool ret = AQSqlConnections::removeDatabase( connectionName );
  wuiShortLog(
    toWString(
      QString( "Close Connection ID: %1-> %2 !!" ).arg( connectionName ).arg( ret ? "Success" : "Failed" ) ) );
  return ret;
}

void AQApplicationPrivate::loadTranslation( const QString & prefixFileTs, const QString & connectionName )
{
  QString Language( QLocale::system().name() );
  QString fileTs = prefixFileTs + Language.left( 2 ) + ".ts", shaKey, cacheKey;
  QString contentTs = AQSqlConnections::database( connectionName ).managerModules() ->content( fileTs, shaKey, cacheKey );
  AQApplicationWeb * app = AbanQ;

  if ( !contentTs.isEmpty() ) {
    AQTranslator * tor = new AQTranslator( app );
    tor->loadTsContent( cacheKey );
    app->installTranslator( tor );
  }
}

AQApplication::AQApplication( WQApplication * wqApp, WQWidget * parent ) :
    WQWidget( *new AQApplicationPrivate( wqApp ), static_cast<WObject *> ( parent ) )
{}

void AQApplication::addWQWidget( WQWidget * w )
{
  if ( !w )
    return ;

  if ( !w->isWidget() ) {
    aqWarn( "AQApplication::addWQWidget !w->isWidget " << this << w );
    return ;
  }

  AQ_D( AQApplication );
  d->containerPanel_->layout() ->addWidget( w->ww() );
}

WQWidget * AQApplication::mainWidget() const
{
  AQ_D( const AQApplication );
  return d->tabView_->currentWidget();
}

AQTabView * AQApplication::tabView() const
{
  AQ_D( const AQApplication );
  return d->tabView_;
}

QDateTime * AQApplication::timeUser() const
{
  AQ_D( const AQApplication );
  return d->timeUser_;
}

void AQApplication::newMainWindow()
{
  AQ_D( AQApplication );

  QString connName( d->connCount ? "conn" + QString::number( d->connCount ) : "default" );
  QString connNameSession( connName + d->sessionId_ );

  if ( d->execDialogConnectDB( connNameSession ) ) {
    AQSqlDatabase db = AQSqlConnections::database( connNameSession );
    QString dbName( db.database() );
    AQMainWindow * win = new AQMainWindow( 0, db );

    win->QObject::setObjectName( dbName );
    connect( win, SIGNAL( closeRequest( AQMainWindow * ) ), this, SLOT( closeMainWindow( AQMainWindow * ) ) );
    win->init();

    d->initTabView();
    d->tabView_->addTab( win, dbName.toUpper() + " (" + connName + ")" );
    d->tabView_->setCurrentWidget( win );
    d->connCount++;

    wuiShortLog( toWString( QString( "Open Connection DB: %1 ID: %2-> Success !!" ) .arg( dbName ) .arg( connNameSession ) ) );
  } else
    wuiShortLog( toWString( QString( "Open Connection ID: %1-> Failed !!" ) .arg( connNameSession ) ) );

  if ( !d->connCount )
    quit();
}

void AQApplication::closeMainWindow( AQMainWindow * win )
{
  AQ_D( AQApplication );

  if ( d->tabView_->count() > 1 && win ) {
    win->finish();
    d->tabView_->closeTab( win );
    d->closeConnectionDB( win->db().connectionName() );
    delete win;
  } else
    tryExit();
}

void AQApplication::closeMainWindow()
{
  AQMainWindow * win = static_cast<AQMainWindow *> ( d_func() ->tabView_->currentWidget() );

  aqWarn( win );

  if ( win )
    closeMainWindow( win );
  else
    tryExit();
}

void AQApplication::closeOtherTabs()
{
  //    AQMainWindow * win = static_cast<AQMainWindow *>( tabView_->currentWidget() );
  //    AQMainWindow * winIt;
  //    QWidgetList widgets = tabView_->widgets();
  //    for ( QWidgetList::const_iterator it = widgets.begin(); it != widgets.end(); ++it ) {
  //        winIt = static_cast<AQMainWindow *>( *it );
  //        if ( winIt == win )
  //            continue;
  //        closeMainWindow( winIt );
  //    }
}

bool AQApplication::tryExit()
{
  if ( AQ_No == AQ_MSGBOX_QUES( tr( "Salir..." ), tr( "¿ Quiere salir de AbanQ ?" ) ) )
    return false;
  quit();
  return true;
}

void AQApplication::quit()
{
  AQ_D( AQApplication );
  if ( d->quited_ )
    return ;
  d->quited_ = true;
  d->wqApp_->redirect( "http://abanq.org/" );
  d->wqApp_->quit();
}

void AQApplication::interrupt()
{
  AQ_D( AQApplication );
  if ( d->quited_ || d->interrupt_ )
    return ;
  d->interrupt_ = true;
  WQApplication::invokeMethod( this, "serverExitRequest" );
}

void AQApplication::serverExitRequest()
{
  QString msg( tr( "El servidor ha iniciado el proceso de parada." ) );
  msg += tr( "\nSu conexión se cerrará automáticamente en 30 segundos." );
  msg += tr( "\nPor favor termine su sesión ahora para evitar pérdida de datos." );
  aqCrit( msg );
  WTimer::singleShot( 30000, this, &AQApplication::quit );
}
