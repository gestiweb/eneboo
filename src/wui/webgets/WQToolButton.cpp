/***************************************************************************
WQToolButton.cpp
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

#include <WPushButton>

#include <QToolButton>

#include "../wtqt/WQApplication.h"
#include "WQIcon.h"
#include "WQToolButton.h"
#include "WQToolButton_p.h"

WQToolButtonPrivate::WQToolButtonPrivate() :
    WQWidgetPrivate(), qBut_( 0 )
{
  canContain_ = false;
}

WQToolButtonPrivate::~WQToolButtonPrivate()
{}

void WQToolButtonPrivate::init()
{
  AQ_Q( WQToolButton );

  if ( !qBut_ ) {
    qBut_ = qobject_cast<QToolButton *> ( qWidget_ );

    if ( !qBut_ )
      return ;

    ww_->setMaximumSize( qBut_->maximumWidth(), qBut_->maximumHeight() );
    ww_->setMinimumSize( qBut_->minimumWidth(), qBut_->minimumHeight() );
    ww_->setIcon( WQIcon( qBut_->icon() ).icon() );

    ww_->setDisabled( qBut_->testAttribute( Qt::WA_Disabled ) );
    ww_->setHidden( qBut_->testAttribute( Qt::WA_WState_ExplicitShowHide ) &&
                    qBut_->testAttribute( Qt::WA_WState_Hidden ) );

    QObject::connect( q, SIGNAL( clicked() ), qBut_, SLOT( click() ) );
  }

  AQ_WUI_CONNECT( ww_->clicked, q, WQToolButton, clicked );
}

WQToolButton::WQToolButton( WQWidget * parent ) :
    WQWidget( *new WQToolButtonPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQToolButton );
  d->ww_ = new WPushButton;
  d->ww_->setMargin( 0 );
}

WWidget * WQToolButton::ww()
{
  AQ_D( WQToolButton );
  return d->ww_;
}

void WQToolButton::update()
{
  AQ_D( WQToolButton );

  if ( !d->qBut_ )
    return ;

  d->ww_->setText( toWString( d->qBut_->text().remove( "&" ) ) );
}

void WQToolButton::click()
{
  AQ_D( WQToolButton );
  WT_SIGNAL( d->ww_->clicked ).emit( WMouseEvent() );
}
