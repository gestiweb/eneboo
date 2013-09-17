/***************************************************************************
WQComboBox.cpp
-------------------
begin                : 14/02/2010
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

#include <WComboBox>

#include <QComboBox>

#include "../wtqt/WQApplication.h"
#include "WQComboBox.h"
#include "WQComboBox_p.h"

WQComboBoxPrivate::WQComboBoxPrivate() :
    WQWidgetPrivate(), qCbx_( 0 )
{
  canContain_ = false;
}

WQComboBoxPrivate::~WQComboBoxPrivate()
{}

void WQComboBoxPrivate::init()
{
  if ( !qCbx_ )
    qCbx_ = qobject_cast<QComboBox *> ( qWidget_ );

  ww_->setMinimumSize( qCbx_->minimumWidth(), WLength() );
  ww_->resize( qCbx_->size().width(), WLength() );

  ww_->setDisabled( qCbx_->testAttribute( Qt::WA_Disabled ) );
  ww_->setHidden( qCbx_->testAttribute( Qt::WA_WState_ExplicitShowHide ) &&
                  qCbx_->testAttribute( Qt::WA_WState_Hidden ) );
}

WQComboBox::WQComboBox( WQWidget * parent ) :
    WQWidget( *new WQComboBoxPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQComboBox );
  d->ww_ = new WComboBox;
  d->ww_->setStyleClass( "x-form-field" );
}

WWidget * WQComboBox::ww()
{
  AQ_D( WQComboBox );
  return d->ww_;
}

void WQComboBox::update()
{
  AQ_D( WQComboBox );

  d->ww_->clear();

  for ( int idx = 0; idx < d->qCbx_->count(); ++idx )
    d->ww_->addItem( toWString( d->qCbx_->itemText( idx ) ) );

  d->ww_->setCurrentIndex( d->qCbx_->currentIndex() );
}
