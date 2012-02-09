/***************************************************************************
 AQSCanvasPolygonalItem_p.h
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

#ifndef AQSCANVASPOLYGONALITEM_P_H_
#define AQSCANVASPOLYGONALITEM_P_H_

#include "AQSCanvasItem_p.h"

class AQSCanvasPolygonalItem : public AQSCanvasItem
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CanvasPolygonalItem, CanvasItem);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setPen(QPen *);
  virtual void setPen(AQSPen *);
  virtual void setBrush(QBrush *);
  virtual void setBrush(AQSBrush *);
  QPen *pen() const;
  QBrush *brush() const;
  virtual QPointArray *areaPoints() const;
  virtual QPointArray *areaPointsAdvanced() const;
  virtual QRect boundingRect() const;
  virtual int rtti() const;

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSCanvasPolygonalItem::setPen(QPen *arg0)
{
  AQ_CALL_VOID(setPen(*arg0));
}
inline void AQSCanvasPolygonalItem::setPen(AQSPen *arg0)
{
  AQ_CALL_VOID(setPen(*arg0));
}
inline void AQSCanvasPolygonalItem::setBrush(QBrush *arg0)
{
  AQ_CALL_VOID(setBrush(*arg0));
}
inline void AQSCanvasPolygonalItem::setBrush(AQSBrush *arg0)
{
  AQ_CALL_VOID(setBrush(*arg0));
}
inline QPen *AQSCanvasPolygonalItem::pen() const
{
  AQ_CALL_RET_PTR(pen(), QPen);
}
inline QBrush *AQSCanvasPolygonalItem::brush() const
{
  AQ_CALL_RET_PTR(brush(), QBrush);
}
inline QPointArray *AQSCanvasPolygonalItem::areaPoints() const
{
  AQ_CALL_RET_PTR(areaPoints(), QPointArray);
}
inline QPointArray *AQSCanvasPolygonalItem::areaPointsAdvanced() const
{
  AQ_CALL_RET_PTR(areaPointsAdvanced(), QPointArray);
}
inline QRect AQSCanvasPolygonalItem::boundingRect() const
{
  AQ_CALL_RET_V(boundingRect(), QRect);
}
inline int AQSCanvasPolygonalItem::rtti() const
{
  AQ_CALL_RET_V(rtti(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVASPOLYGONALITEM_P_H_ */
// @AQSVOIDPTR@
