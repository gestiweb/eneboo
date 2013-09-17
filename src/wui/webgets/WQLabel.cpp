/***************************************************************************
WQLabel.cpp
-------------------
begin                : 13/02/2010
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

#include <WLabel>

#include <QLabel>
#include <QTextDocument>

#include "../wtqt/WQApplication.h"
#include "WQLabel.h"
#include "WQLabel_p.h"

WQLabelPrivate::WQLabelPrivate() :
    WQWidgetPrivate(), qLab_( 0 )
{
  canContain_ = false;
}

WQLabelPrivate::~WQLabelPrivate()
{}

void WQLabelPrivate::init()
{
  if ( !qLab_ )
    qLab_ = qobject_cast<QLabel *> ( qWidget_ );

  ww_->setMinimumSize( qLab_->minimumWidth(), WLength() );
  ww_->resize( qLab_->size().width(), WLength() );

  ww_->setHidden( qLab_->testAttribute( Qt::WA_WState_ExplicitShowHide ) &&
                  qLab_->testAttribute( Qt::WA_WState_Hidden ) );
}

WQLabel::WQLabel( WQWidget * parent ) :
    WQWidget( *new WQLabelPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQLabel );
  d->ww_ = new WLabel;
  d->ww_->setMargin( 0 );
}

WWidget * WQLabel::ww()
{
  AQ_D( WQLabel );
  return d->ww_;
}

void WQLabel::update()
{
  AQ_D( WQLabel );
  d->ww_->setWordWrap( d->qLab_->wordWrap() );
  d->ww_->setText( toWString( d->qLab_->text() ) );
}
