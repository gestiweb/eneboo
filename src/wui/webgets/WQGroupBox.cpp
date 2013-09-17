/***************************************************************************
WQGroupBox.cpp
-------------------
begin                : 05/09/2008
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
#include <WGroupBox>

#include <QGroupBox>

#include "../wtqt/WQApplication.h"
#include "WQGroupBox.h"
#include "WQGroupBox_p.h"

WQGroupBoxPrivate::WQGroupBoxPrivate() :
    WQWidgetPrivate(), qGrp_( 0 )
{}

WQGroupBoxPrivate::~WQGroupBoxPrivate()
{}

void WQGroupBoxPrivate::init()
{
  WQWidgetPrivate::init();

  if ( !qGrp_ )
    qGrp_ = qobject_cast<QGroupBox *> ( qWidget_ );
}

WQGroupBox::WQGroupBox( WQWidget * parent ) :
    WQWidget( *new WQGroupBoxPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQGroupBox );
  d->wc_ = d->ww_ = new WGroupBox;
  d->wc_->setStyleClass( "WQFrame" );
}

WWidget * WQGroupBox::ww()
{
  AQ_D( WQGroupBox );
  return d->ww_;
}

void WQGroupBox::update()
{
  AQ_D( WQGroupBox );
  d->ww_->setTitle( toWString( d->qGrp_->title().remove( "&" ) ) );
}
