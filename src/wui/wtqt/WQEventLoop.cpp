/***************************************************************************
WQEventLoop.cpp
-------------------
begin                : 17/07/2008
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

#include <QSemaphore>

#include "../webgets/WQWidget.h"
#include "AQDispatchThread.h"
#include "WQApplication.h"
#include "WQEventLoop.h"

class WQEventLoopThread : public AQDispatchThread
{
public:

  WQEventLoopThread( WQWidget * widget ) :
      AQDispatchThread(), widget_( widget )
  {
    widget_->moveToThread( this );
  }

  ~WQEventLoopThread()
  {
    delete widget_;
  }

  WQWidget * widget_;
  QSemaphore waitExitLoop_;

protected:

  void doCreate()
  {
    widget_->init();
  }

  void doDestroy()
  {
    widget_->finish();
  }
};

WQEventLoop::WQEventLoop( WQWidget * widget ) :
    WObject(), thread_( 0 ), widget_( widget )
{}

WQEventLoop::~WQEventLoop()
{
  if ( thread_ )
    delete thread_;
}

void WQEventLoop::execLoop()
{
  Q_ASSERT( widget_ );

  thread_ = new WQEventLoopThread( widget_ );

  AQDispatchThread * curThread = qobject_cast<AQDispatchThread *> ( QThread::currentThread() );

  if ( !curThread ) {
    thread_->wqApp() ->createInvocation();
    thread_->wqApp() ->lockInvocation();
  }

  thread_->notifyStart();

  if ( !curThread ) {
    thread_->wqApp() ->waitInvocation();
    thread_->wqApp() ->releaseInvocationHead();
  } else
    thread_->waitExitLoop_.acquire();
}

void WQEventLoop::exitLoop()
{
  Q_ASSERT( thread_ );

  thread_->waitExitLoop_.release();
  thread_->notifyEnd();

  delete this;
}
