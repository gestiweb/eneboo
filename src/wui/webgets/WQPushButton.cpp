/***************************************************************************
WQPushButton.cpp
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

#include <QPushButton>

#include "../wtqt/WQApplication.h"
#include "WQIcon.h"
#include "WQPushButton.h"
#include "WQPushButton_p.h"

WQPushButtonPrivate::WQPushButtonPrivate() :
    WQWidgetPrivate(), qBut_( 0 )
{
  canContain_ = false;
}

WQPushButtonPrivate::~WQPushButtonPrivate()
{}

void WQPushButtonPrivate::init()
{
  AQ_Q( WQPushButton );

  if ( !qBut_ ) {
    qBut_ = qobject_cast<QPushButton *> ( qWidget_ );

    if ( !qBut_ )
      return ;

    ww_->setMaximumSize( qBut_->maximumWidth(), qBut_->maximumHeight() );
    ww_->setMinimumSize( qBut_->minimumWidth(), qBut_->minimumHeight() );
    ww_->setIcon( WQIcon( qBut_ ).icon() );

    ww_->setDisabled( qBut_->testAttribute( Qt::WA_Disabled ) );
    ww_->setHidden( qBut_->testAttribute( Qt::WA_WState_ExplicitShowHide ) &&
                    qBut_->testAttribute( Qt::WA_WState_Hidden ) );

    QObject::connect( q, SIGNAL( clicked() ), qBut_, SLOT( click() ) );
  }

  AQ_WUI_CONNECT( ww_->clicked, q, WQPushButton, clicked );
}

WQPushButton::WQPushButton( WQWidget * parent ) :
    WQWidget( *new WQPushButtonPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQPushButton );
  d->ww_ = new WPushButton;
  d->ww_->setMargin( 0 );
}

WWidget * WQPushButton::ww()
{
  AQ_D( WQPushButton );
  return d->ww_;
}

void WQPushButton::update()
{
  AQ_D( WQPushButton );

  if ( !d->qBut_ )
    return ;

  d->ww_->setText( toWString( d->qBut_->text().remove( "&" ) ) );
}

void WQPushButton::click()
{
  AQ_D( WQPushButton );
  WT_SIGNAL( d->ww_->clicked ).emit( WMouseEvent() );
}
