/***************************************************************************
 AQSCanvas_p.h
 -------------------
 begin                : 06/10/2011
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

#ifndef AQSCANVAS_P_H_
#define AQSCANVAS_P_H_

#include "AQSObject_p.h"
#include "AQSPainter_p.h"
#include "AQSCanvasItem_p.h"
#include <qcanvas.h>

class AQSCanvas : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Canvas, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setTiles(QPixmap *, int, int, int, int);
  virtual void setTiles(AQSPixmap *, int, int, int, int);
  virtual void setBackgroundPixmap(QPixmap *);
  virtual void setBackgroundPixmap(AQSPixmap *);
  QPixmap *backgroundPixmap() const;
  virtual void setBackgroundColor(QColor *);
  virtual void setBackgroundColor(AQSColor *);
  QColor *backgroundColor() const;
  virtual void setTile(int, int, int);
  int tile(int, int) const;
  int tilesHorizontally() const;
  int tilesVertically() const;
  int tileWidth() const;
  int tileHeight() const;
  virtual void resize(int, int);
  int width() const;
  int height() const;
  QSize size() const;
  QRect rect() const;
  bool onCanvas(int, int) const;
  bool onCanvas(const QPoint &) const;
  bool validChunk(int, int) const;
  bool validChunk(const QPoint &) const;
  int chunkSize() const;
  virtual void retune(int, int = 100);
  virtual void setAllChanged();
  virtual void setChanged(const QRect &);
  virtual void setUnchanged(const QRect &);
  QCanvasItemList *allItems();
  QCanvasItemList *collisions(const QPoint &) const;
  QCanvasItemList *collisions(const QRect &) const;
  QCanvasItemList *collisions(QPointArray *, QCanvasItem *, bool) const;
  QCanvasItemList *collisions(QPointArray *, AQSCanvasItem *, bool) const;
  QCanvasItemList *collisions(AQSPointArray *, QCanvasItem *, bool) const;
  QCanvasItemList *collisions(AQSPointArray *, AQSCanvasItem *, bool) const;
  void drawArea(const QRect &, QPainter *, bool = FALSE);
  void drawArea(const QRect &, AQSPainter *, bool = FALSE);
  virtual void setAdvancePeriod(int);
  virtual void setUpdatePeriod(int);
  virtual void setDoubleBuffering(bool);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    candidates[2].append(QString::fromLatin1("int,int"));
    candidates[5].append(QString::fromLatin1("QPixmap*,int,int,int,int"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QCanvas;
    if (sgt == QString::fromLatin1("QObject*"))
      return new QCanvas(argValue<QObject *>(args[0]));
    if (sgt == QString::fromLatin1("QObject*,QString"))
      return new QCanvas(argValue<QObject *>(args[0]),
                         *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("int,int"))
      return new QCanvas(args[0].variant().toInt(),
                         args[1].variant().toInt());
    if (sgt == QString::fromLatin1("QPixmap*,int,int,int,int"))
      return new QCanvas(*argValue<QPixmap *>(args[0]),
                         args[1].variant().toInt(),
                         args[2].variant().toInt(),
                         args[3].variant().toInt(),
                         args[4].variant().toInt());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    candidates[2].append(QString::fromLatin1("int,int"));
    candidates[5].append(QString::fromLatin1("QPixmap*,int,int,int,int"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSCanvas::setTiles(QPixmap *arg0,  int arg1,  int arg2,  int arg3,  int arg4)
{
  AQ_CALL_VOID(setTiles(*arg0, arg1, arg2, arg3, arg4));
}
inline void AQSCanvas::setTiles(AQSPixmap *arg0,  int arg1,  int arg2,  int arg3,  int arg4)
{
  AQ_CALL_VOID(setTiles(*arg0, arg1, arg2, arg3, arg4));
}
inline void AQSCanvas::setBackgroundPixmap(QPixmap *arg0)
{
  AQ_CALL_VOID(setBackgroundPixmap(*arg0));
}
inline void AQSCanvas::setBackgroundPixmap(AQSPixmap *arg0)
{
  AQ_CALL_VOID(setBackgroundPixmap(*arg0));
}
inline QPixmap *AQSCanvas::backgroundPixmap() const
{
  AQ_CALL_RET_PTR(backgroundPixmap(), QPixmap);
}
inline void AQSCanvas::setBackgroundColor(QColor *arg0)
{
  AQ_CALL_VOID(setBackgroundColor(*arg0));
}
inline void AQSCanvas::setBackgroundColor(AQSColor *arg0)
{
  AQ_CALL_VOID(setBackgroundColor(*arg0));
}
inline QColor *AQSCanvas::backgroundColor() const
{
  AQ_CALL_RET_PTR(backgroundColor(), QColor);
}
inline void AQSCanvas::setTile(int arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(setTile(arg0, arg1, arg2));
}
inline int AQSCanvas::tile(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(tile(arg0, arg1), int);
}
inline int AQSCanvas::tilesHorizontally() const
{
  AQ_CALL_RET_V(tilesHorizontally(), int);
}
inline int AQSCanvas::tilesVertically() const
{
  AQ_CALL_RET_V(tilesVertically(), int);
}
inline int AQSCanvas::tileWidth() const
{
  AQ_CALL_RET_V(tileWidth(), int);
}
inline int AQSCanvas::tileHeight() const
{
  AQ_CALL_RET_V(tileHeight(), int);
}
inline void AQSCanvas::resize(int arg0,  int arg1)
{
  AQ_CALL_VOID(resize(arg0, arg1));
}
inline int AQSCanvas::width() const
{
  AQ_CALL_RET_V(width(), int);
}
inline int AQSCanvas::height() const
{
  AQ_CALL_RET_V(height(), int);
}
inline QSize AQSCanvas::size() const
{
  AQ_CALL_RET_V(size(), QSize);
}
inline QRect AQSCanvas::rect() const
{
  AQ_CALL_RET_V(rect(), QRect);
}
inline bool AQSCanvas::onCanvas(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(onCanvas(arg0, arg1), bool);
}
inline bool AQSCanvas::onCanvas(const QPoint &arg0) const
{
  AQ_CALL_RET_V(onCanvas(arg0), bool);
}
inline bool AQSCanvas::validChunk(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(validChunk(arg0, arg1), bool);
}
inline bool AQSCanvas::validChunk(const QPoint &arg0) const
{
  AQ_CALL_RET_V(validChunk(arg0), bool);
}
inline int AQSCanvas::chunkSize() const
{
  AQ_CALL_RET_V(chunkSize(), int);
}
inline void AQSCanvas::retune(int arg0,  int arg1)
{
  AQ_CALL_VOID(retune(arg0, arg1));
}
inline void AQSCanvas::setAllChanged()
{
  AQ_CALL_VOID(setAllChanged());
}
inline void AQSCanvas::setChanged(const QRect &arg0)
{
  AQ_CALL_VOID(setChanged(arg0));
}
inline void AQSCanvas::setUnchanged(const QRect &arg0)
{
  AQ_CALL_VOID(setUnchanged(arg0));
}
inline QCanvasItemList *AQSCanvas::allItems()
{
  AQ_CALL_RET_PTR(allItems(), QCanvasItemList);
}
inline QCanvasItemList *AQSCanvas::collisions(const QPoint &arg0) const
{
  AQ_CALL_RET_PTR(collisions(arg0), QCanvasItemList);
}
inline QCanvasItemList *AQSCanvas::collisions(const QRect &arg0) const
{
  AQ_CALL_RET_PTR(collisions(arg0), QCanvasItemList);
}
inline QCanvasItemList *AQSCanvas::collisions(QPointArray *arg0,  QCanvasItem *arg1,  bool arg2) const
{
  AQ_CALL_RET_PTR(collisions(*arg0, arg1, arg2), QCanvasItemList);
}
inline QCanvasItemList *AQSCanvas::collisions(QPointArray *arg0, AQSCanvasItem *arg1,  bool arg2) const
{
  AQ_CALL_RET_PTR(collisions(*arg0, *arg1, arg2), QCanvasItemList);
}
inline QCanvasItemList *AQSCanvas::collisions(AQSPointArray *arg0,  QCanvasItem *arg1,  bool arg2) const
{
  AQ_CALL_RET_PTR(collisions(*arg0, arg1, arg2), QCanvasItemList);
}
inline QCanvasItemList *AQSCanvas::collisions(AQSPointArray *arg0, AQSCanvasItem *arg1,  bool arg2) const
{
  AQ_CALL_RET_PTR(collisions(*arg0, *arg1, arg2), QCanvasItemList);
}
inline void AQSCanvas::drawArea(const QRect &arg0, QPainter *arg1,  bool arg2)
{
  AQ_CALL_VOID(drawArea(arg0, arg1, arg2));
}
inline void AQSCanvas::drawArea(const QRect &arg0, AQSPainter *arg1,  bool arg2)
{
  AQ_CALL_VOID(drawArea(arg0, *arg1, arg2));
}
inline void AQSCanvas::setAdvancePeriod(int arg0)
{
  AQ_CALL_VOID(setAdvancePeriod(arg0));
}
inline void AQSCanvas::setUpdatePeriod(int arg0)
{
  AQ_CALL_VOID(setUpdatePeriod(arg0));
}
inline void AQSCanvas::setDoubleBuffering(bool arg0)
{
  AQ_CALL_VOID(setDoubleBuffering(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVAS_P_H_ */
// @AQSOBJECT@
