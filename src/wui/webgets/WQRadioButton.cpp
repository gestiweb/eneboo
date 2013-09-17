/***************************************************************************
WQRadioButton.cpp
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

#include <WRadioButton>

#include <QRadioButton>

#include "../wtqt/WQApplication.h"
#include "WQRadioButton.h"
#include "WQRadioButton_p.h"

WQRadioButtonPrivate::WQRadioButtonPrivate() :
    WQWidgetPrivate(), qBut_( 0 )
{
  canContain_ = false;
}

WQRadioButtonPrivate::~WQRadioButtonPrivate()
{}

void WQRadioButtonPrivate::init()
{
  if ( !qBut_ )
    qBut_ = qobject_cast<QRadioButton *> ( qWidget_ );

  ww_->setMaximumSize( qBut_->maximumWidth(), qBut_->maximumHeight() );
  ww_->setMinimumSize( qBut_->minimumWidth(), qBut_->minimumHeight() );

  ww_->setDisabled( qBut_->testAttribute( Qt::WA_Disabled ) );
  ww_->setHidden( qBut_->testAttribute( Qt::WA_WState_ExplicitShowHide ) &&
                  qBut_->testAttribute( Qt::WA_WState_Hidden ) );
}

WQRadioButton::WQRadioButton( WQWidget * parent ) :
    WQWidget( *new WQRadioButtonPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQRadioButton );
  d->ww_ = new WRadioButton;
  d->ww_->setMargin( 0 );
}

WWidget * WQRadioButton::ww()
{
  AQ_D( WQRadioButton );
  return d->ww_;
}

void WQRadioButton::update()
{
  AQ_D( WQRadioButton );
  d->ww_->setText( toWString( d->qBut_->text().remove( "&" ) ) );
}
