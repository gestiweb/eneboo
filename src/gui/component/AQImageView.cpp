/***************************************************************************
                         AQImageView.cpp
                     -------------------
begin                : 28/08/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
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

#include <QLabel>
#include <QImage>
#include <QPaintEvent>
#include <Q3Url>
#include <QPainter>

#include "AQImageView.h"
#include "../AQUi.h"
#include "../AQApplication.h"

class AQImageViewPrivate
{

public:

  AQImageViewPrivate();

  QImage image_;
  QLabel * view_;
  QSize viewSize_;
  QString path_;
  bool autoScaled_;
};

AQImageViewPrivate::AQImageViewPrivate() : viewSize_( 0, 0 ), autoScaled_( false )
{}

AQImageView::AQImageView( QWidget * parent ) : QScrollArea( parent ), Q3FilePreview()
{
  d = new AQImageViewPrivate();

  d->view_ = new QLabel( viewport() );
  setWidget( d->view_ );
  setAlignment( Qt::AlignCenter );
}

AQImageView::~AQImageView()
{
  delete d;
}

void AQImageView::setImage( const QImage & img )
{
  AbanQ->setOverrideCursor( Qt::waitCursor );
  d->image_ = img;
  if ( !d->autoScaled_ ) {
    d->view_->show();
    if ( d->image_.isNull() ) {
      d->view_->clear();
      d->view_->adjustSize();
    } else {
      QPixmap pix( QPixmap::fromImage( img ) );
      d->view_->resize( pix.size() );
      d->view_->setPixmap( pix );
    }
  } else {
    d->view_->hide();
    viewport() ->repaint();
  }
  AbanQ->restoreOverrideCursor();
}

void AQImageView::paintEvent ( QPaintEvent * event )
{
  QScrollArea::paintEvent( event );

  if ( d->autoScaled_ ) {
    QPainter p( viewport() );

    p.setClipRegion( event->region() );
    p.fillRect( event->rect(), colorGroup().brush( QColorGroup::Background ) );

    if ( d->image_.isNull() ) {
      d->viewSize_.setHeight( 0 );
      d->viewSize_.setWidth( 0 );
      return ;
    }

    QSize newSize( size() - QSize( 15, 15 ) );

    if ( !d->viewSize_.isEmpty() && d->viewSize_ == newSize )
      return ;

    if ( d->image_.width() > newSize.width() || d->image_.height() > newSize.height() ) {
      QImage imageScaled( d->image_.scaled( newSize, Qt::KeepAspectRatio ) );
      d->viewSize_ = imageScaled.size();
      p.drawImage( ( width() / 2 ) - ( d->viewSize_.width() / 2 ),
                   ( height() / 2 ) - ( d->viewSize_.height() / 2 ),
                   imageScaled );
    } else {
      d->viewSize_ = d->image_.size();
      p.drawImage( ( width() / 2 ) - ( d->viewSize_.width() / 2 ),
                   ( height() / 2 ) - ( d->viewSize_.height() / 2 ),
                   d->image_ );
    }
  }
}

void AQImageView::previewUrl( const Q3Url & u )
{
  if ( u.isLocalFile() ) {
    QString path = u.path();
    if ( path != d->path_ ) {
      d->path_ = path;
      QImage img( d->path_ );
      setImage( img );
    }
  }
}

void AQImageView::clear()
{
  setImage( QImage() );
}

QImage AQImageView::image()
{
  return d->image_;
}

void AQImageView::setAutoScaled( bool autoScaled )
{
  d->autoScaled_ = autoScaled;
}

bool AQImageView::autoScaled() const
{
  return d->autoScaled_;
}
