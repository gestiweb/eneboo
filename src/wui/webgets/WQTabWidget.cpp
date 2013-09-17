/***************************************************************************
WQTabWidget.cpp
-------------------
begin                : 06/09/2008
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

#include <WTabWidget>

#include <QTabWidget>

#include "../wtqt/WQApplication.h"
#include "WQTabWidget.h"
#include "WQTabWidget_p.h"

WQTabWidgetPrivate::WQTabWidgetPrivate() :
    WQWidgetPrivate(), qTabWid_( 0 )
{}

WQTabWidgetPrivate::~WQTabWidgetPrivate()
{}

void WQTabWidgetPrivate::init()
{
  if ( !qTabWid_ )
    qTabWid_ = qobject_cast<QTabWidget *> ( qWidget_ );

  ww_->setMinimumSize( qTabWid_->minimumWidth(), WLength() );
}

WQTabWidget::WQTabWidget( WQWidget * parent ) :
    WQWidget( *new WQTabWidgetPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQTabWidget );
  d->ww_ = d->tbw_ = new WTabWidget;
  d->ww_->setMargin( 0 );
}

WWidget * WQTabWidget::ww()
{
  AQ_D( WQTabWidget );
  return d->ww_;
}

void WQTabWidget::addWQWidget( WQWidget * w )
{
  if ( !w || !w->isWidget() )
    return ;

  AQ_D( WQTabWidget );

  int idx = d->qTabWid_->indexOf( w->qWidget() );

  if ( idx == -1 )
    return ;

  d->tbw_->addTab( w->ww(), toWString( d->qTabWid_->tabText( idx ).remove( "&" ) ) );

  addChildWidget( w );
}
