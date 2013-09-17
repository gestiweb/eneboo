/***************************************************************************
WQWidgetDialog.cpp
-------------------
begin                : 26/02/2010
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
#include <WLayout>

#include <Ext/Dialog>

#include <QWidget>

#include "../wtqt/WQApplication.h"
#include "WQWidget.h"
#include "WQWidgetDialog.h"

class WQWidgetDialogPrivate : public WQWidget
{
public:

  WQWidgetDialogPrivate( const QMetaObject & metaObject ) :
      WQWidget( static_cast<WObject *> ( 0 ) ),
      metaObject_( metaObject ), widget_( 0 ), diag_( 0 )
  {}

  void init()
  {
    widget_ = qobject_cast<QWidget *>( metaObject_.newInstance() );

    Q_ASSERT( widget_ );

    diag_ = new Ext::Dialog;

    diag_->setWindowTitle( toWString( widget_->windowTitle() ) );
    diag_->resize( widget_->size().width(), widget_->size().height() );
    diag_->layout() ->addWidget( create( widget_ ) ->ww() );
    diag_->show();
  }

  void finish()
  {
    if ( diag_ ) {
      diag_->WWidget::hide();
      delete diag_;
      delete widget_;
    }
  }

  WWidget * ww()
  {
    return diag_;
  }

  const QMetaObject & metaObject_;
  QWidget * widget_;
  Ext::Dialog * diag_;
};

WQWidgetDialog::WQWidgetDialog( const QMetaObject & metaObject ) :
    metaObject_( metaObject )
{}

WQWidget * WQWidgetDialog::widget()
{
  return new WQWidgetDialogPrivate( metaObject_ );
}
