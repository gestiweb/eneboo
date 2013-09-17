/***************************************************************************
                           AQImageView.h
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

#ifndef AQIMAGEVIEW_H_
#define AQIMAGEVIEW_H_

#include <QScrollArea>
#include <Q3FilePreview>

class AQImageViewPrivate;
class QPaintEvent;
class QImage;
class Q3Url;

class AQImageView : public QScrollArea, public Q3FilePreview
{

public:

  AQImageView( QWidget * parent = 0 );
  ~AQImageView();
  void setImage( const QImage & img );
  void previewUrl( const Q3Url & u );
  void clear();
  QImage image();
  void setAutoScaled( bool autoScaled );
  bool autoScaled() const;

protected:

  void paintEvent ( QPaintEvent * event );

private:

  /**
  Privado
  */
  AQImageViewPrivate * d;
};

#endif /*AQIMAGEVIEW_H_*/
