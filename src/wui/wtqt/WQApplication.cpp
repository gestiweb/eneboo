/***************************************************************************
WQApplication.cpp
-------------------
begin                : 19/06/2008
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

#include <WObject>
#include <WEnvironment>
#include <WOverlayLoadingIndicator>

#include <QUuid>
#include <QSemaphore>
#include <QTextStream>

#include <AQCore/AQCoreUtil.h>

#include "../AQApplication.h"
#include "AQLoadingIndicator.h"
#include "AQDispatchThread.h"
#include "WQEventLoop.h"
#include "WQApplication.h"

class AQInvocation : public QSemaphore
{
public:

  AQInvocation() :
      QSemaphore(), locked_( false ), waiting_( false ), thread_( 0 )
  {}

  WString info() const
  {
    QString log;
    QTextStream logStr( &log );

    logStr << thread_ << " :: " << this;
    logStr << " - " << ( locked_ ? "locked" : "no_locked" );
    logStr << " - " << ( waiting_ ? "waiting" : "no_waiting" );

    return toWString( log );
  }

  bool locked_;
  bool waiting_;
  QThread * thread_;
};

WQApplication::WQApplication( const WEnvironment& env ) :
    WApplication( env ), aqApp_( 0 ), eventLoop_( 0 ), keybase_( "AbanQ/" )
{
  useStyleSheet( "abanq_default.css" );
  setCssTheme( "polished" );

  setLoadingIndicator( new AQLoadingIndicator );

  WEnvironment::CookieMap cookies_( env.cookies() );
  WEnvironment::CookieMap::const_iterator it = cookies_.find( "aqkeybase" );

  if ( it == cookies_.end() ) {
    QString uuid_( AQCoreUtil::sha1( QUuid::createUuid().toString() ) );
    setCookie( "aqkeybase", uuid_.toStdString(), 99999 );
    keybase_ += uuid_ + QLatin1String( "/" );
  } else
    keybase_ += QString::fromStdString( it->second ) + QLatin1String( "/" );
}

void WQApplication::invokeMethod( QObject * receiver, const char * member, Qt::ConnectionType type,
                                  QGenericArgument val0, QGenericArgument val1 )
{
  Q_ASSERT( receiver );

  AQDispatchThread * recThread = qobject_cast<AQDispatchThread *> ( receiver->thread() );

  Q_ASSERT( recThread );

  wuiDebugShortLog( "INI invokeMethod: " << receiver << "," << member );
  wuiDebugShortLog( " recThread: " << recThread << " curThread: " << QThread::currentThread() );

  bool livesInCurrentThread = ( recThread == QThread::currentThread() );

  if ( ( type == Qt::AutoConnection && livesInCurrentThread ) || type == Qt::DirectConnection ) {
    wuiDebugShortLog( "!!! invokeMethod : Direct" );

    QMetaObject::invokeMethod( receiver, member, Qt::DirectConnection, val0, val1 );
  } else {

    recThread->wqApp() ->lockInvocation();

    QMetaObject::invokeMethod( receiver, member, Qt::QueuedConnection, val0, val1 );

    if ( livesInCurrentThread )
      QCoreApplication::sendPostedEvents();

    recThread->wqApp() ->waitInvocation();
    recThread->wqApp() ->releaseInvocationLast();
  }

  wuiDebugShortLog( "FIN invokeMethod: " << receiver << "," << member );
}

void WQApplication::initialize()
{
  WString aqTitle( toWString( "AbanQ WUI " + AQVERSION ) );
  wuiDebugLog( aqTitle );
  setTitle( aqTitle );
  wuiDebugLog( "keybase: " << toWString( keybase_ ) );

  aqApp_ = new AQApplication( this );
  eventLoop_ = new WQEventLoop( aqApp_ );

  wuiDebugLog( "CREATE INI " << " curThread: " << qobject_cast<AQDispatchThread *> ( QThread::currentThread() ) );

  eventLoop_->execLoop();

  wuiDebugLog( "CREATE FIN" );
}

void WQApplication::finalize()
{
  eventLoop_->exitLoop();

  releaseInvocationLast();
  releaseInvocationHead();
}

void WQApplication::notify( const WEvent & ev )
{
  wuiDebugShortLog( "NOTIFY INI : " << &ev << " curThread: " << QThread::currentThread() );

  createInvocation();

  WApplication::notify( ev );

  wuiDebugShortLog( "NOTIFY FIN : " << &ev );
}

void WQApplication::createInvocation()
{
  QMutexLocker lock( &queueMutex_ );

  invocations_.enqueue( new AQInvocation );

  wuiDebugShortLog( "   CREATED -> " << invocations_.last() ->info() );
}

void WQApplication::waitInvocation()
{
  QMutexLocker lock( &queueMutex_ );

  if ( !invocations_.isEmpty() ) {
    AQInvocation * invoc = invocations_.last();

    if ( invoc->locked_ && !invoc->waiting_ ) {
      wuiDebugShortLog( "   WAITING -> " << invoc->info() );
      invoc->waiting_ = true;
      lock.unlock();
      invoc->acquire();
      wuiDebugShortLog( "   END WAITING -> " << invoc );
    }
  }
}

void WQApplication::lockInvocation()
{
  QMutexLocker lock( &queueMutex_ );

  if ( !invocations_.isEmpty() ) {
    AQInvocation * invoc = invocations_.last();

    if ( invoc->locked_ ) {
      QThread * prevThread = invoc->thread_;

      invoc = new AQInvocation;
      invoc->waiting_ = true;
      invocations_.enqueue( invoc );

      if ( !prevThread )
        invoc->thread_ = qobject_cast<AQDispatchThread *> ( QThread::currentThread() );

      wuiDebugShortLog( "   AUTO CREATED -> " << invocations_.last() ->info() );
    } else
      invoc->thread_ = qobject_cast<AQDispatchThread *> ( QThread::currentThread() );

    invoc->locked_ = true;
    wuiDebugShortLog( "   LOCKED -> " << invoc->info() );
  }
}

void WQApplication::releaseInvocationHead()
{
  QMutexLocker lock( &queueMutex_ );

  if ( invocations_.isEmpty() )
    return ;

  AQInvocation * invoc = invocations_.head();

  while ( !invoc->locked_ && !invoc->waiting_ ) {
    invoc = invocations_.dequeue();
    wuiDebugShortLog( "   DELETED -> " << invoc->info() );
    delete invoc;
    invoc = 0;

    if ( invocations_.isEmpty() )
      break;

    invoc = invocations_.head();
  }

  if ( !invoc )
    return ;

  if ( !invoc->thread_ || invoc->thread_ == QThread::currentThread() ) {
    invocations_.dequeue();

    lock.unlock();
    releaseInvocationHead();
    lock.relock();

    invoc->release();
    invoc->locked_ = false;
    invoc->waiting_ = false;

    invocations_.enqueue( invoc );

    wuiDebugShortLog( "   RELEASED -> " << invoc->info() << " curThread: " << qobject_cast<AQDispatchThread *> ( QThread::currentThread() ) );
  } else {
    wuiDebugShortLog( "   NO RELEASED -> " << invoc->info() << " curThread: " << qobject_cast<AQDispatchThread *> ( QThread::currentThread() ) );
  }
}

void WQApplication::releaseInvocationLast()
{
  QMutexLocker lock( &queueMutex_ );

  if ( invocations_.isEmpty() )
    return ;

  AQInvocation * invoc = invocations_.takeLast();

  if ( !invoc->thread_ || invoc->thread_ == QThread::currentThread() ) {

    invoc->release();
    invoc->locked_ = false;
    invoc->waiting_ = false;

    if ( invoc->thread_ ) {
      lock.unlock();
      releaseInvocationLast();
      lock.relock();
    }

    invocations_.prepend( invoc );

    wuiDebugShortLog( "   RELEASED LAST -> " << invoc->info() << " curThread: " << qobject_cast<AQDispatchThread *> ( QThread::currentThread() ) );
  } else {
    wuiDebugShortLog( "   NO RELEASED LAST-> " << invoc->info() << " curThread: " << qobject_cast<AQDispatchThread *> ( QThread::currentThread() ) );

    lock.unlock();
    releaseInvocationLast();
    lock.relock();

    invocations_.enqueue( invoc );
  }
}
