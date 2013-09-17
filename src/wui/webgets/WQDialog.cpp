/***************************************************************************
WQDialog.cpp
-------------------
begin                : 24/07/2008
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

#include "../wtqt/WQEventLoop.h"
#include "WQDialog.h"

WQDialog::WQDialog() :
    WObject(), AQObject(), eventLoop_( 0 ), rCode_( 0 )
{}

int WQDialog::result() const
{
  return rCode_;
}

void WQDialog::setResult( int r )
{
  rCode_ = r;
}

int WQDialog::exec()
{
  eventLoop_ = new WQEventLoop( this->widget() );

  eventLoop_->execLoop();

  return rCode_;
}

void WQDialog::done( int r )
{
  setResult( r );

  Q_EMIT finished( r );
  if ( r == Accepted )
    Q_EMIT accepted();
  else if ( r == Rejected )
    Q_EMIT rejected();

  if ( eventLoop_ )
    eventLoop_->exitLoop();
}

void WQDialog::accept()
{
  done( Accepted );
}

void WQDialog::reject()
{
  done( Rejected );
}
