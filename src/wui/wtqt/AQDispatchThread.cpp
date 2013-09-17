/***************************************************************************
AQDispatchThread.cpp
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

#include "../AQApplication.h"
#include "../AQUi.h"
#include "WQApplication.h"
#include "AQDispatchThread.h"

void AQDispatchObject::methodCreate()
{
  wuiDebugLog( "---INI methodCreate" );

  thread_->doCreate();

  wuiDebugLog( "---FIN methodCreate" );
}

void AQDispatchObject::methodDestroy()
{
  wuiDebugLog( "---INI methodDestroy" );

  thread_->doDestroy();

  thread_->QThread::exit();

  wuiDebugLog( "---FIN methodDestroy" );
}

AQDispatchThread::AQDispatchThread() :
    QThread(), dispatchObject_( 0 ), wqApp_( AbanQWT )
{}

void AQDispatchThread::run()
{
  wqApp_->attachThread();

  wuiDebugLog( "RUN INI" );

  dispatchObject_ = new AQDispatchObject( this );

  WQApplication::invokeMethod( dispatchObject_, "methodCreate", Qt::QueuedConnection );

  QThread::exec();

  delete dispatchObject_;

  wuiDebugLog( "RUN FIN" );
}

void AQDispatchThread::notifyStart()
{
  wuiDebugLog( "START INI" );

  QThread::start();

  wuiDebugLog( "START FIN" );
}

void AQDispatchThread::notifyEnd()
{
  wuiDebugLog( "END INI" );

  WQApplication::invokeMethod( dispatchObject_, "methodDestroy" );

  wuiDebugLog( "END FIN" );

  QThread::wait();
}
