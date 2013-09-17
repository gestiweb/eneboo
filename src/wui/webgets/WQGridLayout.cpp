/***************************************************************************
WQGridLayout.cpp
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

#include <WGridLayout>

#include <QGridLayout>

#include "WQGridLayout.h"
#include "WQGridLayout_p.h"

WQGridLayoutPrivate::WQGridLayoutPrivate() :
    WQWidgetPrivate(), qLay_( 0 )
{
  isLayout_ = true;
  isWidget_ = false;
}

WQGridLayoutPrivate::~WQGridLayoutPrivate()
{}

void WQGridLayoutPrivate::init()
{
  if ( !qLay_ )
    qLay_ = qobject_cast<QGridLayout *> ( qLayItem_->layout() );
}

WQGridLayout::WQGridLayout( WQWidget * parent ) :
    WQWidget( *new WQGridLayoutPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQGridLayout );

  d->wl_ = new WGridLayout;
  d->wl_->setVerticalSpacing( 0 );
  d->wl_->setHorizontalSpacing( 0 );
  d->wl_->setContentsMargins( 0, 0, 0, 0 );
}

WLayout * WQGridLayout::wl()
{
  AQ_D( WQGridLayout );
  return d->wl_;
}

void WQGridLayout::addWQWidget( WQWidget * w )
{
  if ( !w )
    return ;

  AQ_D( WQGridLayout );

  aqWarn( "### TODO" );

  //  for ( int r = 0; r < d->qLay_->rowCount(); ++r ) {
  //    for ( int c = 0; c < d->qLay_->columnCount(); ++c ) {
  //      QLayout * itemLay = d->qLay_->itemAtPosition( r, c ) ->layout();
  //      QWidget * itemWid = d->qLay_->itemAtPosition( r, c ) ->widget();
  //
  //      if ( ( itemLay && itemLay == w->qLayItem() ->layout() ) || ( itemWid && itemWid == w->qWidget() ) ) {
  //
  //        if ( w->wl() ) {
  //          d->wl_->addLayout( w->wl(), r, c );
  //        } else {
  //          d->wl_->addWidget( w->ww(), r, c );
  //        }
  //
  //        d->items_.append( w );
  //
  //        if ( !w->isLayout() ) {
  //          WQWidget * wqParent = parentWidget();
  //          if ( wqParent )
  //            wqParent->addChildWidget( w );
  //        }
  //
  //        return ;
  //      }
  //    }
  //  }
}

void WQGridLayout::update()
{
  AQ_D( WQGridLayout );

  d->wl_->setVerticalSpacing( d->qLay_->verticalSpacing() );
  d->wl_->setHorizontalSpacing( d->qLay_->horizontalSpacing() );
}
