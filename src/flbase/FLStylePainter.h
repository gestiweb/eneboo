/***************************************************************************
                     FLStylePainter.h
                    -------------------
begin                : 24/12/2008
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

#ifndef FLSTYLEPAINTER_H
#define FLSTYLEPAINTER_H

#include <qobject.h>
#include <qpainter.h>

#include "AQGlobal.h"

class FLStylePainterPrivate;

class AQ_EXPORT FLStylePainter
{

public:

  enum ErrCode {
    NoError = 0,
    IdNotFound
  };

  FLStylePainter();
  ~FLStylePainter();

  QPainter *painter() const;

  QString styleName() const;
  void setStyleName(const QString &style);

  float relDpi() const;
  void setRelDpi(float relDpi);

  void beginMark(int x, int y, QObject *obj);
  void endMark();
  void beginSection(int x, int y, int w, int h, QObject *obj);
  void endSection();

  bool drawPixmap(const QPixmap &pixmap, int sx = 0,
                  int sy = 0, int sw = -1, int sh = -1, QObject *obj = 0);
  bool drawText(const QString &text, int tf, QObject *obj);
  bool drawLine(QObject *obj);
  bool drawRect(QObject *obj);
  bool setStyle(QObject *obj);

  void applyTransforms();

  ErrCode errCode() const;

  static void normalizeSVGFile(const QString &fileName, const QStringList &fileNames);
  static void setSVGMode(bool mode);

private:

  FLStylePainterPrivate *d;
};

#endif
