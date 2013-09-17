/***************************************************************************
WQLineEdit.cpp
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

#include <WLineEdit>

#include <AQUi/AQLineEdit.h>

#include "../wtqt/WQApplication.h"
#include "WQLineEdit.h"
#include "WQLineEdit_p.h"

WQLineEditPrivate::WQLineEditPrivate() :
    WQWidgetPrivate(), qLed_( 0 )
{
  canContain_ = false;
}

WQLineEditPrivate::~WQLineEditPrivate()
{}

void WQLineEditPrivate::init()
{
  AQ_Q( WQLineEdit );

  if ( !qLed_ ) {
    qLed_ = qobject_cast<AQLineEdit *> ( qWidget_ );

    if ( !qLed_ )
      return ;

    ww_->setMinimumSize( qLed_->minimumWidth(), WLength() );
    ww_->resize( qLed_->size().width(), WLength() );
    ww_->setMaxLength( qLed_->maxLength() );

    if ( qLed_->echoMode() == QLineEdit::Password )
      ww_->setEchoMode( WLineEdit::Password );

    ww_->setDisabled( qLed_->testAttribute( Qt::WA_Disabled ) );
    ww_->setHidden( qLed_->testAttribute( Qt::WA_WState_ExplicitShowHide ) &&
                    qLed_->testAttribute( Qt::WA_WState_Hidden ) );

    QObject::connect( q, SIGNAL( textChanged( const QString & ) ),
                      qLed_, SLOT( setText( const QString & ) ) );
    QObject::connect( qLed_, SIGNAL( textChanged( const QString & ) ),
                      q, SLOT( setText( const QString & ) ) );
  }

  AQ_WUI_CONNECT( ww_->changed, q, WQLineEdit, changed );
}

WQLineEdit::WQLineEdit( WQWidget * parent ) :
    WQWidget( *new WQLineEditPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( WQLineEdit );
  d->ww_ = new WLineEdit;
  d->ww_->setStyleClass( "x-form-field" );
  d->ww_->setMargin( 0 );
}

WWidget * WQLineEdit::ww()
{
  AQ_D( WQLineEdit );
  return d->ww_;
}

void WQLineEdit::update()
{
  AQ_D( WQLineEdit );

  if ( !d->qLed_ )
    return ;

  setText( d->qLed_->text() );
}

void WQLineEdit::setText( const QString & text )
{
  AQ_D( WQLineEdit );
  WString wText( toWString( text ) );
  if ( d->ww_->text() != wText )
    d->ww_->setText( wText );
}

void WQLineEdit::changed()
{
  AQ_D( WQLineEdit );
  Q_EMIT textChanged( toQString( d->ww_->text() ) );
}
