/***************************************************************************
                               FLCodBar.h
                          -------------------
 begin                : Mar Jun 17 2003
 copyright            : (C) 2003-2005 by InfoSiAL S.L.
 email                : mail@infosial.com
***************************************************************************/
/***************************************************************************
                                mybarcode.h
                             -------------------
    begin                : Die Apr 23 2002
    copyright            : (C) 2002,2003 by Dominik Seichter
    email                : domseichter@web.de
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

#ifndef FLCODBAR_H
#define FLCODBAR_H

#include <qapplication.h>
#include <qobject.h>
#include <qfont.h>
#include <qpainter.h>
#include <qstring.h>
#include <qprocess.h>
#include <qpixmapcache.h>

class barcodeFormat;

class barcodeData
{
public:

  barcodeData() {}
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

class FLCodBar : public QObject
{
  Q_OBJECT

public:

  FLCodBar(const QString &value, int type, int margin, double scale, double cut, int rotation,
           bool text_flag = false, QColor fg = Qt::black, QColor bg = Qt::white, int res = 72);

  FLCodBar(barcodeData *data);

  FLCodBar();

  ~FLCodBar();

  QPixmap pixmap();

  QPixmap pixmapError();

  QString value() {
    return barcode.value;
  }
  int type() {
    return barcode.type;
  }
  int margin() {
    return barcode.margin;
  }
  double scale() {
    return barcode.scale;
  }
  double cut() {
    return barcode.cut;
  }
  bool text() {
    return barcode.text;
  }
  int rotation() {
    return barcode.rotation;
  }
  QColor fg() {
    return barcode.fg;
  }
  QColor bg() {
    return barcode.bg;
  }
  int res() {
    return barcode.res;
  }
  void setData(barcodeData *d) {
    barcode = *d;
  }
  bool validBarcode() {
    return barcode.valid;
  }

  void setCaption(const QString &caption) {
    barcode.caption = caption;
  }
  QString caption() {
    return barcode.caption;
  }

  void setValue(const QString &value) {
    barcode.value = value;
  }
  void setType(const int type) {
    barcode.type = type;
  }
  void setMargin(const int margin) {
    barcode.margin = margin;
  }
  void setScale(const double scale) {
    barcode.scale = scale;
  }
  void setCut(const double cut) {
    barcode.cut = cut;
  }
  void setText(const bool text) {
    barcode.text = text;
  }
  void setRotation(const int rotation) {
    barcode.rotation = rotation;
  }
  void setFg(const QColor &fg) {
    barcode.fg = fg;
  }
  void setBg(const QColor &bg) {
    barcode.bg = bg;
  }
  void setRes(const int res) {
    barcode.res = res;
  }
  barcodeData data() {
    return barcode;
  }

  void fillDefault(barcodeData *data);
  void cleanUp();

  static int nameToType(const QString &name);
  static QString typeToName(const int type);

protected slots:

  void readPixmapStdout();
  void writingStdoutFinished();

private:

  void createBarcode();
  void copyBarcode(barcodeData *src, barcodeData *dest);

  QPixmap p;
  QPixmap pError;
  barcodeData barcode;
  QProcess *proc;
  bool readingStdout;
  bool writingStdout;
};

#endif
