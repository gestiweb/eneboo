/***************************************************************************
WQCheckBox.cpp
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

#include <WCheckBox>

#include <QCheckBox>

#include "../wtqt/WQApplication.h"
#include "WQCheckBox.h"
#include "WQCheckBox_p.h"

WQCheckBoxPrivate::WQCheckBoxPrivate() :
    WQWidgetPrivate(), qChk_( 0 )
{
  canContain_ = false;
}

WQCheckBoxPrivate::~WQCheckBoxPrivate()
{}

void WQCheckBoxPrivate::init()
{
  if ( !qChk_ )
    qChk_ = qobject_cast<QCheckBox *> ( qWidget_ );

  ww_->setMinimumSize( qChk_->minimumWidth(), WLength() );
  ww_->resize( qChk_->size().width(), WLength() );

  ww_->setDisabled( qChk_->testAttribute( Qt::WA_Disabled ) );
  ww_->setHidden( qChk_->testAttribute( Qt::WA_WState_ExplicitShowHide ) &&
                  qChk_->testAttribute( Qt::WA_WState_Hidden ) );
}

WQCheckBox::WQCheckBox( WQWidget * parent ) :
    WQWidget( *new WQCheckBoxPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQCheckBox );
  d->ww_ = new WCheckBox;
}

WWidget * WQCheckBox::ww()
{
  AQ_D( WQCheckBox );
  return d->ww_;
}

void WQCheckBox::update()
{
  AQ_D( WQCheckBox );
  d->ww_->setText( toWString( d->qChk_->text().remove( "&" ) ) );
  d->ww_->setTristate( d->qChk_->isTristate() );
  d->ww_->setCheckState( ( Wt::CheckState ) int( d->qChk_->checkState() ) );
}
