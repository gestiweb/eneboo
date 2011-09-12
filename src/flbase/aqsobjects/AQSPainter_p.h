/***************************************************************************
 AQSPainter_p.h
 -------------------
 begin                : 14/02/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
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

#ifndef AQSPAINTER_P_H_
#define AQSPAINTER_P_H_

#include "AQSImage_p.h"
#include "AQSPen_p.h"
#include "AQSBrush_p.h"
#include "AQSFontMetrics_p.h"
#include "AQSPicture_p.h"
#include <qpainter.h>

class AQSPainter : public AQSVoidPtr
{
  Q_OBJECT

  Q_ENUMS(BGMode)
  Q_ENUMS(RasterOp)
  Q_ENUMS(BrushStyle)

  AQ_DECLARE_AQS_VOID(Painter, VoidPtr);

public slots:
  void setNoPen() {
    AQ_CALL_VOID(setPen(Qt::NoPen));
  }

  void setNoBrush() {
    AQ_CALL_VOID(setBrush(Qt::NoBrush));
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool begin(const QPaintDevice *, bool = FALSE);
  bool begin(const QPaintDevice *, const QWidget *, bool = FALSE);
  bool end();
  QPaintDevice *device() const;
  bool isActive() const;
  void flush(QRegion *, uint = QPainter::CoordDevice);
  void flush(AQSRegion *, uint = QPainter::CoordDevice);
  void flush();
  void save();
  void restore();
  QFontMetrics *fontMetrics() const;
  const QFont &font() const;
  void setFont(const QFont &);
  QPen *pen() const;
  void setPen(QPen *);
  void setPen(AQSPen *);
  void setPen(uint);
  void setPen(QColor *);
  void setPen(AQSColor *);
  QBrush *brush() const;
  void setBrush(QBrush *);
  void setBrush(AQSBrush *);
  void setBrush(BrushStyle);
  void setBrush(QColor *);
  void setBrush(AQSColor *);
  QColor *backgroundColor() const;
  void setBackgroundColor(QColor *);
  void setBackgroundColor(AQSColor *);
  void setBackgroundMode(BGMode);
  void setRasterOp(RasterOp);
  QPoint brushOrigin() const;
  void setBrushOrigin(int, int);
  void setBrushOrigin(const QPoint &);
  bool hasViewXForm() const;
  bool hasWorldXForm() const;
  void setViewXForm(bool);
  QRect window() const;
  void setWindow(const QRect &);
  void setWindow(int, int, int, int);
  QRect viewport() const;
  void setViewport(const QRect &);
  void setViewport(int, int, int, int);
  void setWorldXForm(bool);
  QWMatrix *worldMatrix() const;
  void setWorldMatrix(QWMatrix *, bool = FALSE);
  void setWorldMatrix(AQSWMatrix *, bool = FALSE);
  void scale(double, double);
  void shear(double, double);
  void rotate(double);
  void translate(double, double);
  void resetXForm();
  QPoint xForm(const QPoint &) const;
  QRect xForm(const QRect &) const;
  QPointArray *xForm(QPointArray *) const;
  QPointArray *xForm(AQSPointArray *) const;
  QPointArray *xForm(QPointArray *, int, int) const;
  QPointArray *xForm(AQSPointArray *, int, int) const;
  QPoint xFormDev(const QPoint &) const;
  QRect xFormDev(const QRect &) const;
  QPointArray *xFormDev(QPointArray *) const;
  QPointArray *xFormDev(AQSPointArray *) const;
  QPointArray *xFormDev(QPointArray *, int, int) const;
  QPointArray *xFormDev(AQSPointArray *, int, int) const;
  void setClipping(bool);
  bool hasClipping() const;
  QRegion *clipRegion(uint = QPainter::CoordDevice) const;
  void setClipRect(const QRect &, uint = QPainter::CoordDevice);
  void setClipRect(int, int, int, int, uint = QPainter::CoordDevice);
  void setClipRegion(QRegion *, uint = QPainter::CoordDevice);
  void setClipRegion(AQSRegion *, uint = QPainter::CoordDevice);
  void drawPoint(int, int);
  void drawPoint(const QPoint &);
  void drawPoints(QPointArray *, int = 0, int = -1);
  void drawPoints(AQSPointArray *, int = 0, int = -1);
  void drawLine(int, int, int, int);
  void drawLine(const QPoint &, const QPoint &);
  void drawRect(int, int, int, int);
  void drawRect(const QRect &);
  void drawWinFocusRect(int, int, int, int);
  void drawWinFocusRect(int, int, int, int, QColor *);
  void drawWinFocusRect(int, int, int, int, AQSColor *);
  void drawWinFocusRect(const QRect &);
  void drawWinFocusRect(const QRect &, QColor *);
  void drawWinFocusRect(const QRect &, AQSColor *);
  void drawRoundRect(int, int, int, int, int = 25, int = 25);
  void drawRoundRect(const QRect &, int = 25, int = 25);
  void drawEllipse(int, int, int, int);
  void drawEllipse(const QRect &);
  void drawArc(int, int, int, int, int, int);
  void drawArc(const QRect &, int, int);
  void drawPie(int, int, int, int, int, int);
  void drawPie(const QRect &, int, int);
  void drawChord(int, int, int, int, int, int);
  void drawChord(const QRect &, int, int);
  void drawLineSegments(QPointArray *, int = 0, int = -1);
  void drawLineSegments(AQSPointArray *, int = 0, int = -1);
  void drawPolyline(QPointArray *, int = 0, int = -1);
  void drawPolyline(AQSPointArray *, int = 0, int = -1);
  void drawPolygon(QPointArray *, bool = FALSE, int = 0, int = -1);
  void drawPolygon(AQSPointArray *, bool = FALSE, int = 0, int = -1);
  void drawConvexPolygon(QPointArray *, int = 0, int = -1);
  void drawConvexPolygon(AQSPointArray *, int = 0, int = -1);
  void drawCubicBezier(QPointArray *, int = 0);
  void drawCubicBezier(AQSPointArray *, int = 0);
  void drawPixmap(int, int, QPixmap *, int = 0, int = 0, int = -1, int = -1);
  void drawPixmap(int, int, AQSPixmap *, int = 0, int = 0, int = -1, int = -1);
  void drawPixmap(const QPoint &, QPixmap *, const QRect &);
  void drawPixmap(const QPoint &, AQSPixmap *, const QRect &);
  void drawPixmap(const QPoint &, QPixmap *);
  void drawPixmap(const QPoint &, AQSPixmap *);
  void drawPixmap(const QRect &, QPixmap *);
  void drawPixmap(const QRect &, AQSPixmap *);
  void drawImage(int, int, QImage *, int = 0, int = 0, int = -1, int = -1, int = 0);
  void drawImage(int, int, AQSImage *, int = 0, int = 0, int = -1, int = -1, int = 0);
  void drawImage(const QPoint &, QImage *, const QRect &, int = 0);
  void drawImage(const QPoint &, AQSImage *, const QRect &, int = 0);
  void drawImage(const QPoint &, QImage *, int = 0);
  void drawImage(const QPoint &, AQSImage *, int = 0);
  void drawImage(const QRect &, QImage *);
  void drawImage(const QRect &, AQSImage *);
  void drawTiledPixmap(int, int, int, int, QPixmap *, int = 0, int = 0);
  void drawTiledPixmap(int, int, int, int, AQSPixmap *, int = 0, int = 0);
  void drawTiledPixmap(const QRect &, QPixmap *, const QPoint &);
  void drawTiledPixmap(const QRect &, AQSPixmap *, const QPoint &);
  void drawTiledPixmap(const QRect &, QPixmap *);
  void drawTiledPixmap(const QRect &, AQSPixmap *);
  void drawPicture(int, int, QPicture *);
  void drawPicture(int, int, AQSPicture *);
  void drawPicture(const QPoint &, QPicture *);
  void drawPicture(const QPoint &, AQSPicture *);
  void fillRect(int, int, int, int, QBrush *);
  void fillRect(int, int, int, int, AQSBrush *);
  void fillRect(const QRect &, QBrush *);
  void fillRect(const QRect &, AQSBrush *);
  void eraseRect(int, int, int, int);
  void eraseRect(const QRect &);
  void drawText(int, int, const QString &, int = -1, uint = QPainter::Auto);
  void drawText(const QPoint &, const QString &, int = -1, uint = QPainter::Auto);
  void drawText(int, int, const QString &, int, int, uint = QPainter::Auto);
  void drawText(const QPoint &, const QString &, int, int, uint = QPainter::Auto);
  void drawText(int, int, int, int, int, const QString &, int = -1, QRect* = 0, QTextParag ** = 0);
  void drawText(const QRect &, int, const QString &, int = -1, QRect* = 0, QTextParag ** = 0);
  QRect boundingRect(int, int, int, int, int, const QString &, int = -1, QTextParag ** = 0);
  QRect boundingRect(const QRect &, int, const QString &, int = -1, QTextParag ** = 0);
  int tabStops() const;
  void setTabStops(int);
  int *tabArray() const;
  void setTabArray(int *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QPaintDevice*"));
    candidates[2].append(QString::fromLatin1("QPaintDevice*,bool"));
    candidates[2].append(QString::fromLatin1("QPaintDevice*,QWidget*"));
    candidates[3].append(QString::fromLatin1("QPaintDevice*,QWidget*,bool"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QPainter;
    if (sgt == QString::fromLatin1("QPaintDevice*"))
      return new QPainter(argValue<QPaintDevice *>(args[0]));
    if (sgt == QString::fromLatin1("QPaintDevice*,bool"))
      return new QPainter(argValue<QPaintDevice *>(args[0]),
                          args[1].variant().toBool());
    if (sgt == QString::fromLatin1("QPaintDevice*,QWidget*"))
      return new QPainter(argValue<QPaintDevice *>(args[0]),
                          argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QPaintDevice*,QWidget*,bool"))
      return new QPainter(argValue<QPaintDevice *>(args[0]),
                          argValue<QWidget *>(args[1]),
                          args[2].variant().toBool());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QPaintDevice*"));
    candidates[2].append(QString::fromLatin1("QPaintDevice*,bool"));
    candidates[2].append(QString::fromLatin1("QPaintDevice*,QWidget*"));
    candidates[3].append(QString::fromLatin1("QPaintDevice*,QWidget*,bool"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSPainter::begin(const QPaintDevice *arg0,  bool arg1)
{
  AQ_CALL_RET_V(begin(arg0, arg1), bool);
}
inline bool AQSPainter::begin(const QPaintDevice *arg0,  const QWidget *arg1,  bool arg2)
{
  AQ_CALL_RET_V(begin(arg0, arg1, arg2), bool);
}
inline bool AQSPainter::end()
{
  AQ_CALL_RET_V(end(), bool);
}
inline QPaintDevice *AQSPainter::device() const
{
  AQ_CALL_RET(device());
}
inline bool AQSPainter::isActive() const
{
  AQ_CALL_RET_V(isActive(), bool);
}
inline void AQSPainter::flush(QRegion *arg0,  uint arg1)
{
  AQ_CALL_VOID(flush(*arg0, static_cast<QPainter::CoordinateMode>(arg1)));
}
inline void AQSPainter::flush(AQSRegion *arg0,  uint arg1)
{
  AQ_CALL_VOID(flush(*arg0, static_cast<QPainter::CoordinateMode>(arg1)));
}
inline void AQSPainter::flush()
{
  AQ_CALL_VOID(flush());
}
inline void AQSPainter::save()
{
  AQ_CALL_VOID(save());
}
inline void AQSPainter::restore()
{
  AQ_CALL_VOID(restore());
}
inline QFontMetrics *AQSPainter::fontMetrics() const
{
  AQ_CALL_RET_PTR(fontMetrics(), QFontMetrics);
}
inline const QFont &AQSPainter::font() const
{
  AQ_CALL_RET_V(font(), QFont);
}
inline void AQSPainter::setFont(const QFont &arg0)
{
  AQ_CALL_VOID(setFont(arg0));
}
inline QPen *AQSPainter::pen() const
{
  AQ_CALL_RET_PTR(pen(), QPen);
}
inline void AQSPainter::setPen(QPen *arg0)
{
  AQ_CALL_VOID(setPen(*arg0));
}
inline void AQSPainter::setPen(AQSPen *arg0)
{
  AQ_CALL_VOID(setPen(*arg0));
}
inline void AQSPainter::setPen(uint arg0)
{
  AQ_CALL_VOID(setPen(arg0));
}
inline void AQSPainter::setPen(QColor *arg0)
{
  AQ_CALL_VOID(setPen(*arg0));
}
inline void AQSPainter::setPen(AQSColor *arg0)
{
  AQ_CALL_VOID(setPen(*arg0));
}
inline QBrush *AQSPainter::brush() const
{
  AQ_CALL_RET_PTR(brush(), QBrush);
}
inline void AQSPainter::setBrush(QBrush *arg0)
{
  AQ_CALL_VOID(setBrush(*arg0));
}
inline void AQSPainter::setBrush(AQSBrush *arg0)
{
  AQ_CALL_VOID(setBrush(*arg0));
}
inline void AQSPainter::setBrush(BrushStyle arg0)
{
  AQ_CALL_VOID(setBrush(arg0));
}
inline void AQSPainter::setBrush(QColor *arg0)
{
  AQ_CALL_VOID(setBrush(*arg0));
}
inline void AQSPainter::setBrush(AQSColor *arg0)
{
  AQ_CALL_VOID(setBrush(*arg0));
}
inline QColor *AQSPainter::backgroundColor() const
{
  AQ_CALL_RET_PTR(backgroundColor(), QColor);
}
inline void AQSPainter::setBackgroundColor(QColor *arg0)
{
  AQ_CALL_VOID(setBackgroundColor(*arg0));
}
inline void AQSPainter::setBackgroundColor(AQSColor *arg0)
{
  AQ_CALL_VOID(setBackgroundColor(*arg0));
}
inline void AQSPainter::setBackgroundMode(BGMode arg0)
{
  AQ_CALL_VOID(setBackgroundMode(arg0));
}
inline void AQSPainter::setRasterOp(RasterOp arg0)
{
  AQ_CALL_VOID(setRasterOp(arg0));
}
inline QPoint AQSPainter::brushOrigin() const
{
  AQ_CALL_RET_V(brushOrigin(), QPoint);
}
inline void AQSPainter::setBrushOrigin(int arg0,  int arg1)
{
  AQ_CALL_VOID(setBrushOrigin(arg0, arg1));
}
inline void AQSPainter::setBrushOrigin(const QPoint &arg0)
{
  AQ_CALL_VOID(setBrushOrigin(arg0));
}
inline bool AQSPainter::hasViewXForm() const
{
  AQ_CALL_RET_V(hasViewXForm(), bool);
}
inline bool AQSPainter::hasWorldXForm() const
{
  AQ_CALL_RET_V(hasWorldXForm(), bool);
}
inline void AQSPainter::setViewXForm(bool arg0)
{
  AQ_CALL_VOID(setViewXForm(arg0));
}
inline QRect AQSPainter::window() const
{
  AQ_CALL_RET_V(window(), QRect);
}
inline void AQSPainter::setWindow(const QRect &arg0)
{
  AQ_CALL_VOID(setWindow(arg0));
}
inline void AQSPainter::setWindow(int arg0,  int arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(setWindow(arg0, arg1, arg2, arg3));
}
inline QRect AQSPainter::viewport() const
{
  AQ_CALL_RET_V(viewport(), QRect);
}
inline void AQSPainter::setViewport(const QRect &arg0)
{
  AQ_CALL_VOID(setViewport(arg0));
}
inline void AQSPainter::setViewport(int arg0,  int arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(setViewport(arg0, arg1, arg2, arg3));
}
inline void AQSPainter::setWorldXForm(bool arg0)
{
  AQ_CALL_VOID(setWorldXForm(arg0));
}
inline QWMatrix *AQSPainter::worldMatrix() const
{
  AQ_CALL_RET_PTR(worldMatrix(), QWMatrix);
}
inline void AQSPainter::setWorldMatrix(QWMatrix *arg0,  bool arg1)
{
  AQ_CALL_VOID(setWorldMatrix(*arg0, arg1));
}
inline void AQSPainter::setWorldMatrix(AQSWMatrix *arg0,  bool arg1)
{
  AQ_CALL_VOID(setWorldMatrix(*arg0, arg1));
}
inline void AQSPainter::scale(double arg0,  double arg1)
{
  AQ_CALL_VOID(scale(arg0, arg1));
}
inline void AQSPainter::shear(double arg0,  double arg1)
{
  AQ_CALL_VOID(shear(arg0, arg1));
}
inline void AQSPainter::rotate(double arg0)
{
  AQ_CALL_VOID(rotate(arg0));
}
inline void AQSPainter::translate(double arg0,  double arg1)
{
  AQ_CALL_VOID(translate(arg0, arg1));
}
inline void AQSPainter::resetXForm()
{
  AQ_CALL_VOID(resetXForm());
}
inline QPoint AQSPainter::xForm(const QPoint &arg0) const
{
  AQ_CALL_RET_V(xForm(arg0), QPoint);
}
inline QRect AQSPainter::xForm(const QRect &arg0) const
{
  AQ_CALL_RET_V(xForm(arg0), QRect);
}
inline QPointArray *AQSPainter::xForm(QPointArray *arg0) const
{
  AQ_CALL_RET_PTR(xForm(*arg0), QPointArray);
}
inline QPointArray *AQSPainter::xForm(AQSPointArray *arg0) const
{
  AQ_CALL_RET_PTR(xForm(*arg0), QPointArray);
}
inline QPointArray *AQSPainter::xForm(QPointArray *arg0,  int arg1,  int arg2) const
{
  AQ_CALL_RET_PTR(xForm(*arg0, arg1, arg2), QPointArray);
}
inline QPointArray *AQSPainter::xForm(AQSPointArray *arg0,  int arg1,  int arg2) const
{
  AQ_CALL_RET_PTR(xForm(*arg0, arg1, arg2), QPointArray);
}
inline QPoint AQSPainter::xFormDev(const QPoint &arg0) const
{
  AQ_CALL_RET_V(xFormDev(arg0), QPoint);
}
inline QRect AQSPainter::xFormDev(const QRect &arg0) const
{
  AQ_CALL_RET_V(xFormDev(arg0), QRect);
}
inline QPointArray *AQSPainter::xFormDev(QPointArray *arg0) const
{
  AQ_CALL_RET_PTR(xFormDev(*arg0), QPointArray);
}
inline QPointArray *AQSPainter::xFormDev(AQSPointArray *arg0) const
{
  AQ_CALL_RET_PTR(xFormDev(*arg0), QPointArray);
}
inline QPointArray *AQSPainter::xFormDev(QPointArray *arg0,  int arg1,  int arg2) const
{
  AQ_CALL_RET_PTR(xFormDev(*arg0, arg1, arg2), QPointArray);
}
inline QPointArray *AQSPainter::xFormDev(AQSPointArray *arg0,  int arg1,  int arg2) const
{
  AQ_CALL_RET_PTR(xFormDev(*arg0, arg1, arg2), QPointArray);
}
inline void AQSPainter::setClipping(bool arg0)
{
  AQ_CALL_VOID(setClipping(arg0));
}
inline bool AQSPainter::hasClipping() const
{
  AQ_CALL_RET_V(hasClipping(), bool);
}
inline QRegion *AQSPainter::clipRegion(uint arg0) const
{
  AQ_CALL_RET_PTR(clipRegion(static_cast<QPainter::CoordinateMode>(arg0)), QRegion);
}
inline void AQSPainter::setClipRect(const QRect &arg0,  uint arg1)
{
  AQ_CALL_VOID(setClipRect(arg0, static_cast<QPainter::CoordinateMode>(arg1)));
}
inline void AQSPainter::setClipRect(int arg0,  int arg1,  int arg2,  int arg3,  uint arg4)
{
  AQ_CALL_VOID(setClipRect(arg0, arg1, arg2, arg3, static_cast<QPainter::CoordinateMode>(arg4)));
}
inline void AQSPainter::setClipRegion(QRegion *arg0,  uint arg1)
{
  AQ_CALL_VOID(setClipRegion(*arg0, static_cast<QPainter::CoordinateMode>(arg1)));
}
inline void AQSPainter::setClipRegion(AQSRegion *arg0,  uint arg1)
{
  AQ_CALL_VOID(setClipRegion(*arg0, static_cast<QPainter::CoordinateMode>(arg1)));
}
inline void AQSPainter::drawPoint(int arg0,  int arg1)
{
  AQ_CALL_VOID(drawPoint(arg0, arg1));
}
inline void AQSPainter::drawPoint(const QPoint &arg0)
{
  AQ_CALL_VOID(drawPoint(arg0));
}
inline void AQSPainter::drawPoints(QPointArray *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawPoints(*arg0, arg1, arg2));
}
inline void AQSPainter::drawPoints(AQSPointArray *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawPoints(*arg0, arg1, arg2));
}
inline void AQSPainter::drawLine(int arg0,  int arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(drawLine(arg0, arg1, arg2, arg3));
}
inline void AQSPainter::drawLine(const QPoint &arg0,  const QPoint &arg1)
{
  AQ_CALL_VOID(drawLine(arg0, arg1));
}
inline void AQSPainter::drawRect(int arg0,  int arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(drawRect(arg0, arg1, arg2, arg3));
}
inline void AQSPainter::drawRect(const QRect &arg0)
{
  AQ_CALL_VOID(drawRect(arg0));
}
inline void AQSPainter::drawWinFocusRect(int arg0,  int arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(drawWinFocusRect(arg0, arg1, arg2, arg3));
}
inline void AQSPainter::drawWinFocusRect(int arg0,  int arg1,  int arg2,  int arg3,  QColor *arg4)
{
  AQ_CALL_VOID(drawWinFocusRect(arg0, arg1, arg2, arg3, *arg4));
}
inline void AQSPainter::drawWinFocusRect(int arg0,  int arg1,  int arg2,  int arg3, AQSColor *arg4)
{
  AQ_CALL_VOID(drawWinFocusRect(arg0, arg1, arg2, arg3, *arg4));
}
inline void AQSPainter::drawWinFocusRect(const QRect &arg0)
{
  AQ_CALL_VOID(drawWinFocusRect(arg0));
}
inline void AQSPainter::drawWinFocusRect(const QRect &arg0,  QColor *arg1)
{
  AQ_CALL_VOID(drawWinFocusRect(arg0, *arg1));
}
inline void AQSPainter::drawWinFocusRect(const QRect &arg0, AQSColor *arg1)
{
  AQ_CALL_VOID(drawWinFocusRect(arg0, *arg1));
}
inline void AQSPainter::drawRoundRect(int arg0,  int arg1,  int arg2,  int arg3,  int arg4,  int arg5)
{
  AQ_CALL_VOID(drawRoundRect(arg0, arg1, arg2, arg3, arg4, arg5));
}
inline void AQSPainter::drawRoundRect(const QRect &arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawRoundRect(arg0, arg1, arg2));
}
inline void AQSPainter::drawEllipse(int arg0,  int arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(drawEllipse(arg0, arg1, arg2, arg3));
}
inline void AQSPainter::drawEllipse(const QRect &arg0)
{
  AQ_CALL_VOID(drawEllipse(arg0));
}
inline void AQSPainter::drawArc(int arg0,  int arg1,  int arg2,  int arg3,  int arg4,  int arg5)
{
  AQ_CALL_VOID(drawArc(arg0, arg1, arg2, arg3, arg4, arg5));
}
inline void AQSPainter::drawArc(const QRect &arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawArc(arg0, arg1, arg2));
}
inline void AQSPainter::drawPie(int arg0,  int arg1,  int arg2,  int arg3,  int arg4,  int arg5)
{
  AQ_CALL_VOID(drawPie(arg0, arg1, arg2, arg3, arg4, arg5));
}
inline void AQSPainter::drawPie(const QRect &arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawPie(arg0, arg1, arg2));
}
inline void AQSPainter::drawChord(int arg0,  int arg1,  int arg2,  int arg3,  int arg4,  int arg5)
{
  AQ_CALL_VOID(drawChord(arg0, arg1, arg2, arg3, arg4, arg5));
}
inline void AQSPainter::drawChord(const QRect &arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawChord(arg0, arg1, arg2));
}
inline void AQSPainter::drawLineSegments(QPointArray *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawLineSegments(*arg0, arg1, arg2));
}
inline void AQSPainter::drawLineSegments(AQSPointArray *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawLineSegments(*arg0, arg1, arg2));
}
inline void AQSPainter::drawPolyline(QPointArray *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawPolyline(*arg0, arg1, arg2));
}
inline void AQSPainter::drawPolyline(AQSPointArray *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawPolyline(*arg0, arg1, arg2));
}
inline void AQSPainter::drawPolygon(QPointArray *arg0,  bool arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(drawPolygon(*arg0, arg1, arg2, arg3));
}
inline void AQSPainter::drawPolygon(AQSPointArray *arg0,  bool arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(drawPolygon(*arg0, arg1, arg2, arg3));
}
inline void AQSPainter::drawConvexPolygon(QPointArray *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawConvexPolygon(*arg0, arg1, arg2));
}
inline void AQSPainter::drawConvexPolygon(AQSPointArray *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(drawConvexPolygon(*arg0, arg1, arg2));
}
inline void AQSPainter::drawCubicBezier(QPointArray *arg0,  int arg1)
{
  AQ_CALL_VOID(drawCubicBezier(*arg0, arg1));
}
inline void AQSPainter::drawCubicBezier(AQSPointArray *arg0,  int arg1)
{
  AQ_CALL_VOID(drawCubicBezier(*arg0, arg1));
}
inline void AQSPainter::drawPixmap(int arg0,  int arg1,  QPixmap *arg2,  int arg3,  int arg4,  int arg5,  int arg6)
{
  AQ_CALL_VOID(drawPixmap(arg0, arg1, *arg2, arg3, arg4, arg5, arg6));
}
inline void AQSPainter::drawPixmap(int arg0,  int arg1, AQSPixmap *arg2,  int arg3,  int arg4,  int arg5,  int arg6)
{
  AQ_CALL_VOID(drawPixmap(arg0, arg1, *arg2, arg3, arg4, arg5, arg6));
}
inline void AQSPainter::drawPixmap(const QPoint &arg0,  QPixmap *arg1,  const QRect &arg2)
{
  AQ_CALL_VOID(drawPixmap(arg0, *arg1, arg2));
}
inline void AQSPainter::drawPixmap(const QPoint &arg0, AQSPixmap *arg1,  const QRect &arg2)
{
  AQ_CALL_VOID(drawPixmap(arg0, *arg1, arg2));
}
inline void AQSPainter::drawPixmap(const QPoint &arg0,  QPixmap *arg1)
{
  AQ_CALL_VOID(drawPixmap(arg0, *arg1));
}
inline void AQSPainter::drawPixmap(const QPoint &arg0, AQSPixmap *arg1)
{
  AQ_CALL_VOID(drawPixmap(arg0, *arg1));
}
inline void AQSPainter::drawPixmap(const QRect &arg0,  QPixmap *arg1)
{
  AQ_CALL_VOID(drawPixmap(arg0, *arg1));
}
inline void AQSPainter::drawPixmap(const QRect &arg0, AQSPixmap *arg1)
{
  AQ_CALL_VOID(drawPixmap(arg0, *arg1));
}
inline void AQSPainter::drawImage(int arg0,  int arg1,  QImage *arg2,  int arg3,  int arg4,  int arg5,  int arg6,  int arg7)
{
  AQ_CALL_VOID(drawImage(arg0, arg1, *arg2, arg3, arg4, arg5, arg6, arg7));
}
inline void AQSPainter::drawImage(int arg0,  int arg1, AQSImage *arg2,  int arg3,  int arg4,  int arg5,  int arg6,  int arg7)
{
  AQ_CALL_VOID(drawImage(arg0, arg1, *arg2, arg3, arg4, arg5, arg6, arg7));
}
inline void AQSPainter::drawImage(const QPoint &arg0,  QImage *arg1,  const QRect &arg2,  int arg3)
{
  AQ_CALL_VOID(drawImage(arg0, *arg1, arg2, arg3));
}
inline void AQSPainter::drawImage(const QPoint &arg0, AQSImage *arg1,  const QRect &arg2,  int arg3)
{
  AQ_CALL_VOID(drawImage(arg0, *arg1, arg2, arg3));
}
inline void AQSPainter::drawImage(const QPoint &arg0,  QImage *arg1,  int arg2)
{
  AQ_CALL_VOID(drawImage(arg0, *arg1, arg2));
}
inline void AQSPainter::drawImage(const QPoint &arg0, AQSImage *arg1,  int arg2)
{
  AQ_CALL_VOID(drawImage(arg0, *arg1, arg2));
}
inline void AQSPainter::drawImage(const QRect &arg0,  QImage *arg1)
{
  AQ_CALL_VOID(drawImage(arg0, *arg1));
}
inline void AQSPainter::drawImage(const QRect &arg0, AQSImage *arg1)
{
  AQ_CALL_VOID(drawImage(arg0, *arg1));
}
inline void AQSPainter::drawTiledPixmap(int arg0,  int arg1,  int arg2,  int arg3,  QPixmap *arg4,  int arg5,  int arg6)
{
  AQ_CALL_VOID(drawTiledPixmap(arg0, arg1, arg2, arg3, *arg4, arg5, arg6));
}
inline void AQSPainter::drawTiledPixmap(int arg0,  int arg1,  int arg2,  int arg3, AQSPixmap *arg4,  int arg5,  int arg6)
{
  AQ_CALL_VOID(drawTiledPixmap(arg0, arg1, arg2, arg3, *arg4, arg5, arg6));
}
inline void AQSPainter::drawTiledPixmap(const QRect &arg0,  QPixmap *arg1,  const QPoint &arg2)
{
  AQ_CALL_VOID(drawTiledPixmap(arg0, *arg1, arg2));
}
inline void AQSPainter::drawTiledPixmap(const QRect &arg0, AQSPixmap *arg1,  const QPoint &arg2)
{
  AQ_CALL_VOID(drawTiledPixmap(arg0, *arg1, arg2));
}
inline void AQSPainter::drawTiledPixmap(const QRect &arg0,  QPixmap *arg1)
{
  AQ_CALL_VOID(drawTiledPixmap(arg0, *arg1));
}
inline void AQSPainter::drawTiledPixmap(const QRect &arg0, AQSPixmap *arg1)
{
  AQ_CALL_VOID(drawTiledPixmap(arg0, *arg1));
}
inline void AQSPainter::drawPicture(int arg0,  int arg1,  QPicture *arg2)
{
  AQ_CALL_VOID(drawPicture(arg0, arg1, *arg2));
}
inline void AQSPainter::drawPicture(int arg0,  int arg1, AQSPicture *arg2)
{
  AQ_CALL_VOID(drawPicture(arg0, arg1, *arg2));
}
inline void AQSPainter::drawPicture(const QPoint &arg0,  QPicture *arg1)
{
  AQ_CALL_VOID(drawPicture(arg0, *arg1));
}
inline void AQSPainter::drawPicture(const QPoint &arg0, AQSPicture *arg1)
{
  AQ_CALL_VOID(drawPicture(arg0, *arg1));
}
inline void AQSPainter::fillRect(int arg0,  int arg1,  int arg2,  int arg3,  QBrush *arg4)
{
  AQ_CALL_VOID(fillRect(arg0, arg1, arg2, arg3, *arg4));
}
inline void AQSPainter::fillRect(int arg0,  int arg1,  int arg2,  int arg3, AQSBrush *arg4)
{
  AQ_CALL_VOID(fillRect(arg0, arg1, arg2, arg3, *arg4));
}
inline void AQSPainter::fillRect(const QRect &arg0,  QBrush *arg1)
{
  AQ_CALL_VOID(fillRect(arg0, *arg1));
}
inline void AQSPainter::fillRect(const QRect &arg0, AQSBrush *arg1)
{
  AQ_CALL_VOID(fillRect(arg0, *arg1));
}
inline void AQSPainter::eraseRect(int arg0,  int arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(eraseRect(arg0, arg1, arg2, arg3));
}
inline void AQSPainter::eraseRect(const QRect &arg0)
{
  AQ_CALL_VOID(eraseRect(arg0));
}
inline void AQSPainter::drawText(int arg0,  int arg1,  const QString &arg2,  int arg3,  uint arg4)
{
  AQ_CALL_VOID(drawText(arg0, arg1, arg2, arg3, static_cast<QPainter::TextDirection>(arg4)));
}
inline void AQSPainter::drawText(const QPoint &arg0,  const QString &arg1,  int arg2,  uint arg3)
{
  AQ_CALL_VOID(drawText(arg0, arg1, arg2, static_cast<QPainter::TextDirection>(arg3)));
}
inline void AQSPainter::drawText(int arg0,  int arg1,  const QString &arg2,  int arg3,  int arg4,  uint arg5)
{
  AQ_CALL_VOID(drawText(arg0, arg1, arg2, arg3, arg4, static_cast<QPainter::TextDirection>(arg5)));
}
inline void AQSPainter::drawText(const QPoint &arg0,  const QString &arg1,  int arg2,  int arg3,  uint arg4)
{
  AQ_CALL_VOID(drawText(arg0, arg1, arg2, arg3, static_cast<QPainter::TextDirection>(arg4)));
}
inline void AQSPainter::drawText(int arg0,  int arg1,  int arg2,  int arg3,  int arg4,  const QString &arg5,  int arg6,  QRect *arg7,  QTextParag **arg8)
{
  AQ_CALL_VOID(drawText(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}
inline void AQSPainter::drawText(const QRect &arg0,  int arg1,  const QString &arg2,  int arg3,  QRect *arg4,  QTextParag **arg5)
{
  AQ_CALL_VOID(drawText(arg0, arg1, arg2, arg3, arg4, arg5));
}
inline QRect AQSPainter::boundingRect(int arg0,  int arg1,  int arg2,  int arg3,  int arg4,  const QString &arg5,  int arg6,  QTextParag **arg7)
{
  AQ_CALL_RET_V(boundingRect(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7), QRect);
}
inline QRect AQSPainter::boundingRect(const QRect &arg0,  int arg1,  const QString &arg2,  int arg3,  QTextParag **arg4)
{
  AQ_CALL_RET_V(boundingRect(arg0, arg1, arg2, arg3, arg4), QRect);
}
inline int AQSPainter::tabStops() const
{
  AQ_CALL_RET_V(tabStops(), int);
}
inline void AQSPainter::setTabStops(int arg0)
{
  AQ_CALL_VOID(setTabStops(arg0));
}
inline int *AQSPainter::tabArray() const
{
  AQ_CALL_RET(tabArray());
}
inline void AQSPainter::setTabArray(int *arg0)
{
  AQ_CALL_VOID(setTabArray(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPAINTER_P_H_ */
// @AQSVOIDPTR@
