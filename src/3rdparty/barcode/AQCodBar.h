/***************************************************************************
AQCodBar.h
-------------------
begin                : 07/08/2007
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

#ifndef AQCODBAR_H_
#define AQCODBAR_H_

#include <QObject>
#include <QString>
#include <QColor>
#include <QPixmap>
#include <QProcess>

class barcodeFormat;

class barcodeData
{
public:

  barcodeData()
  {}
  QString value;
  int type;
  int margin;
  int rotation;
  double scale;
  double cut;
  bool text;
  bool valid;
  QColor fg;
  QColor bg;
  QString caption;
  int x;
  int y;
  int res;
};

class AQCodBar : public QObject
{
  Q_OBJECT

public:

  AQCodBar( const QString & value, int type, int margin, double scale, double cut, int rotation,
            bool text_flag = false, QColor fg = Qt::black, QColor bg = Qt::white, int res = 72 );

  AQCodBar( barcodeData * data );

  AQCodBar();

  ~AQCodBar();

  QPixmap pixmap();

  QPixmap pixmapError();

  QString value()
  {
    return barcode.value;
  }
  int type()
  {
    return barcode.type;
  }
  int margin()
  {
    return barcode.margin;
  }
  double scale()
  {
    return barcode.scale;
  }
  double cut()
  {
    return barcode.cut;
  }
  bool text()
  {
    return barcode.text;
  }
  int rotation()
  {
    return barcode.rotation;
  }
  QColor fg()
  {
    return barcode.fg;
  }
  QColor bg()
  {
    return barcode.bg;
  }
  int res()
  {
    return barcode.res;
  }
  void setData( barcodeData * d )
  {
    barcode = *d;
  }
  bool validBarcode()
  {
    return barcode.valid;
  }

  void setCaption( const QString & caption )
  {
    barcode.caption = caption;
  }
  QString caption()
  {
    return barcode.caption;
  }

  void setValue( const QString & value )
  {
    barcode.value = value;
  }
  void setType( int type )
  {
    barcode.type = type;
  }
  void setMargin( int margin )
  {
    barcode.margin = margin;
  }
  void setScale( double scale )
  {
    barcode.scale = scale;
  }
  void setCut( double cut )
  {
    barcode.cut = cut;
  }
  void setText( bool text )
  {
    barcode.text = text;
  }
  void setRotation( int rotation )
  {
    barcode.rotation = rotation;
  }
  void setFg( const QColor & fg )
  {
    barcode.fg = fg;
  }
  void setBg( const QColor & bg )
  {
    barcode.bg = bg;
  }
  void setRes( int res )
  {
    barcode.res = res;
  }
  barcodeData data()
  {
    return barcode;
  }

  void fillDefault( barcodeData * )
  {}
  void cleanUp()
  {}

  static int nameToType( const QString & )
  {
    return 0;
  }
  static QString typeToName( int )
  {
    return QString();
  }

protected Q_SLOTS:

  void readPixmapStdout()
  {}
  void writingStdoutFinished()
  {}

private:

  void createBarcode()
  {}
  void copyBarcode( barcodeData *, barcodeData * )
  {}

  QPixmap p;
  QPixmap pError;
  barcodeData barcode;
  QProcess * proc;
  bool readingStdout;
  bool writingStdout;
};

#endif /*AQCODBAR_H_*/
