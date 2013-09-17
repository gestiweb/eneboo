/***************************************************************************
WQBoxLayout.cpp
-------------------
begin                : 21/02/2010
copyright            : (C) 2003-2010 by InfoSiAL S.L.
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

#include <WWidget>
#include <WBoxLayout>

#include <QWidget>
#include <QBoxLayout>

#include "../AQUi.h"
#include "WQBoxLayout.h"
#include "WQBoxLayout_p.h"

WQBoxLayoutPrivate::WQBoxLayoutPrivate() :
    WQWidgetPrivate(), qLay_( 0 ), vertical_( true )
{
  isLayout_ = true;
  isWidget_ = false;
}

WQBoxLayoutPrivate::~WQBoxLayoutPrivate()
{}

void WQBoxLayoutPrivate::init()
{
  if ( !qLay_ )
    qLay_ = qobject_cast<QBoxLayout *> ( qLayItem_->layout() );

  if ( qLay_ ) {
    int left, top, right, bottom;
    qLay_->getContentsMargins( &left, &top, &right, &bottom );
    wl_->setContentsMargins( left, top, right, bottom );
    wl_->setSpacing( qLay_->spacing() );
  }
}

int WQBoxLayoutPrivate::indexOf( QLayoutItem * item ) const
{
  if ( qLay_ ) {
    for ( int i = 0; i < qLay_->count(); ++i ) {
      if ( item == qLay_->itemAt( i ) )
        return i;
    }
  }
  return -1;
}

WQBoxLayout::WQBoxLayout( int direction, WQWidget * parent ) :
    WQWidget( *new WQBoxLayoutPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQBoxLayout );
  d->wl_ = new WBoxLayout( ( WBoxLayout::Direction ) direction );
  d->vertical_ = ( direction == WBoxLayout::TopToBottom || direction == WBoxLayout::BottomToTop );
  d->wl_->setSpacing( 0 );
  d->wl_->setContentsMargins( 0, 0, 0, 0 );
}

WLayout * WQBoxLayout::wl()
{
  AQ_D( WQBoxLayout );
  return d->wl_;
}

void WQBoxLayout::addWQWidget( WQWidget * w )
{
  if ( !w || w->isProxy() )
    return ;

  AQ_D( WQBoxLayout );

  d->items_.append( w );

  if ( w->isWidget() && w->ww() ) {

    if ( d->vertical_ )
      d->wl_->addWidget( w->ww(), w->qWidget() ->sizePolicy().verticalStretch(), d->calcAlign( w ) );
    else
      d->wl_->addWidget( w->ww(), w->qWidget() ->sizePolicy().horizontalStretch(), d->calcAlign( w ) );

    WQWidget * wqParent = parentWidget();
    if ( wqParent )
      wqParent->addChildWidget( w );

  } else if ( w->isLayout() && w->wl() ) {
    int stretch = ( d->qLay_ ? d->qLay_->stretch( d->indexOf( w->qLayItem() ) ) : 0 );
    d->wl_->addLayout( w->wl(), stretch, d->calcAlign( w ) );
  } else if ( w->isSpacer() ) {
    int stretch = ( d->qLay_ ? d->qLay_->stretch( d->indexOf( w->qLayItem() ) ) : 0 );
    d->wl_->addStretch( stretch );
  }
}

