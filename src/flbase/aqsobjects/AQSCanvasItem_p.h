/***************************************************************************
 AQSCanvasItem_p.h
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

#ifndef AQSCANVASITEM_P_H_
#define AQSCANVASITEM_P_H_

#include "AQSVoidPtr_p.h"
#include "AQSPainter_p.h"
#include <qcanvas.h>

class AQSCanvasItem : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CanvasItem, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  double x() const;
  double y() const;
  double z() const;
  virtual void moveBy(double, double);
  void move(double, double);
  void setX(double);
  void setY(double);
  void setZ(double);
  bool animated() const;
  virtual void setAnimated(bool);
  virtual void setVelocity(double, double);
  void setXVelocity(double);
  void setYVelocity(double);
  double xVelocity() const;
  double yVelocity() const;
  virtual void advance(int);
  virtual bool collidesWith(QCanvasItem *) const;
  virtual bool collidesWith(AQSCanvasItem *) const;
  QCanvasItemList collisions(bool) const;
  virtual void setCanvas(QCanvas *);
  virtual void draw(QPainter *);
  virtual void draw(AQSPainter *);
  void show();
  void hide();
  virtual void setVisible(bool);
  bool isVisible() const;
  virtual void setSelected(bool);
  bool isSelected() const;
  virtual void setEnabled(bool);
  bool isEnabled() const;
  virtual void setActive(bool);
  bool isActive() const;
  virtual int rtti() const;
  virtual QRect boundingRect() const;
  virtual QRect boundingRectAdvanced() const;
  QCanvas *canvas() const;

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline double AQSCanvasItem::x() const
{
  AQ_CALL_RET_V(x(), double);
}
inline double AQSCanvasItem::y() const
{
  AQ_CALL_RET_V(y(), double);
}
inline double AQSCanvasItem::z() const
{
  AQ_CALL_RET_V(z(), double);
}
inline void AQSCanvasItem::moveBy(double arg0,  double arg1)
{
  AQ_CALL_VOID(moveBy(arg0, arg1));
}
inline void AQSCanvasItem::move(double arg0,  double arg1)
{
  AQ_CALL_VOID(move(arg0, arg1));
}
inline void AQSCanvasItem::setX(double arg0)
{
  AQ_CALL_VOID(setX(arg0));
}
inline void AQSCanvasItem::setY(double arg0)
{
  AQ_CALL_VOID(setY(arg0));
}
inline void AQSCanvasItem::setZ(double arg0)
{
  AQ_CALL_VOID(setZ(arg0));
}
inline bool AQSCanvasItem::animated() const
{
  AQ_CALL_RET_V(animated(), bool);
}
inline void AQSCanvasItem::setAnimated(bool arg0)
{
  AQ_CALL_VOID(setAnimated(arg0));
}
inline void AQSCanvasItem::setVelocity(double arg0,  double arg1)
{
  AQ_CALL_VOID(setVelocity(arg0, arg1));
}
inline void AQSCanvasItem::setXVelocity(double arg0)
{
  AQ_CALL_VOID(setXVelocity(arg0));
}
inline void AQSCanvasItem::setYVelocity(double arg0)
{
  AQ_CALL_VOID(setYVelocity(arg0));
}
inline double AQSCanvasItem::xVelocity() const
{
  AQ_CALL_RET_V(xVelocity(), double);
}
inline double AQSCanvasItem::yVelocity() const
{
  AQ_CALL_RET_V(yVelocity(), double);
}
inline void AQSCanvasItem::advance(int arg0)
{
  AQ_CALL_VOID(advance(arg0));
}
inline bool AQSCanvasItem::collidesWith(QCanvasItem *arg0) const
{
  AQ_CALL_RET_V(collidesWith(arg0), bool);
}
inline bool AQSCanvasItem::collidesWith(AQSCanvasItem *arg0) const
{
  AQ_CALL_RET_V(collidesWith(*arg0), bool);
}
inline QCanvasItemList AQSCanvasItem::collisions(bool arg0) const
{
  AQ_CALL_RET_V(collisions(arg0), QCanvasItemList);
}
inline void AQSCanvasItem::setCanvas(QCanvas *arg0)
{
  AQ_CALL_VOID(setCanvas(arg0));
}
inline void AQSCanvasItem::draw(QPainter *arg0)
{
  AQ_CALL_VOID(draw(*arg0));
}
inline void AQSCanvasItem::draw(AQSPainter *arg0)
{
  AQ_CALL_VOID(draw(*arg0));
}
inline void AQSCanvasItem::show()
{
  AQ_CALL_VOID(show());
}
inline void AQSCanvasItem::hide()
{
  AQ_CALL_VOID(hide());
}
inline void AQSCanvasItem::setVisible(bool arg0)
{
  AQ_CALL_VOID(setVisible(arg0));
}
inline bool AQSCanvasItem::isVisible() const
{
  AQ_CALL_RET_V(isVisible(), bool);
}
inline void AQSCanvasItem::setSelected(bool arg0)
{
  AQ_CALL_VOID(setSelected(arg0));
}
inline bool AQSCanvasItem::isSelected() const
{
  AQ_CALL_RET_V(isSelected(), bool);
}
inline void AQSCanvasItem::setEnabled(bool arg0)
{
  AQ_CALL_VOID(setEnabled(arg0));
}
inline bool AQSCanvasItem::isEnabled() const
{
  AQ_CALL_RET_V(isEnabled(), bool);
}
inline void AQSCanvasItem::setActive(bool arg0)
{
  AQ_CALL_VOID(setActive(arg0));
}
inline bool AQSCanvasItem::isActive() const
{
  AQ_CALL_RET_V(isActive(), bool);
}
inline int AQSCanvasItem::rtti() const
{
  AQ_CALL_RET_V(rtti(), int);
}
inline QRect AQSCanvasItem::boundingRect() const
{
  AQ_CALL_RET_V(boundingRect(), QRect);
}
inline QRect AQSCanvasItem::boundingRectAdvanced() const
{
  AQ_CALL_RET_V(boundingRectAdvanced(), QRect);
}
inline QCanvas *AQSCanvasItem::canvas() const
{
  AQ_CALL_RET(canvas());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVASITEM_P_H_ */
// @AQSVOIDPTR@
