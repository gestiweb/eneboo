/***************************************************************************
WQFrame.cpp
-------------------
begin                : 29/08/2008
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

#include <WContainerWidget>

#include "WQFrame.h"
#include "WQFrame_p.h"

WQFramePrivate::WQFramePrivate() :
    WQWidgetPrivate()
{}

WQFramePrivate::~WQFramePrivate()
{}

WQFrame::WQFrame( WQWidget * parent ) :
    WQWidget( *new WQFramePrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQFrame );
  d->wc_ = new WContainerWidget;
  d->wc_->setStyleClass( "WQFrame" );
}
