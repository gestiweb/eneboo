/***************************************************************************
 AQSDragObject_p.h
 -------------------
 begin                : 22/02/2011
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

#ifndef AQSDRAGOBJECT_P_H_
#define AQSDRAGOBJECT_P_H_

#include "AQSObject_p.h"
#include "AQSPixmap_p.h"
#include <qdragobject.h>

class AQSDragObject : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(DragObject, Object);

public slots:
  QMimeSource *mimeSource() const {
    return static_cast<QMimeSource *>(o_);
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool drag();
  bool dragMove();
  void dragCopy();
  void dragLink();
  virtual void setPixmap(QPixmap *);
  virtual void setPixmap(AQSPixmap *);
  virtual void setPixmap(QPixmap *, const QPoint &);
  virtual void setPixmap(AQSPixmap *, const QPoint &);
  QPixmap *pixmap() const;
  QPoint pixmapHotSpot() const;
  QWidget *source();
  virtual const char *format(int = 0) const;
  virtual QByteArray *encodedData(const char *) const;
  virtual bool provides(const char *) const;
  int serialNumber() const;

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSDragObject::drag()
{
  AQ_CALL_RET_V(drag(), bool);
}
inline bool AQSDragObject::dragMove()
{
  AQ_CALL_RET_V(dragMove(), bool);
}
inline void AQSDragObject::dragCopy()
{
  AQ_CALL_VOID(dragCopy());
}
inline void AQSDragObject::dragLink()
{
  AQ_CALL_VOID(dragLink());
}
inline void AQSDragObject::setPixmap(QPixmap *arg0)
{
  AQ_CALL_VOID(setPixmap(*arg0));
}
inline void AQSDragObject::setPixmap(AQSPixmap *arg0)
{
  AQ_CALL_VOID(setPixmap(*arg0));
}
inline void AQSDragObject::setPixmap(QPixmap *arg0,  const QPoint &arg1)
{
  AQ_CALL_VOID(setPixmap(*arg0, arg1));
}
inline void AQSDragObject::setPixmap(AQSPixmap *arg0,  const QPoint &arg1)
{
  AQ_CALL_VOID(setPixmap(*arg0, arg1));
}
inline QPixmap *AQSDragObject::pixmap() const
{
  AQ_CALL_RET_PTR(pixmap(), QPixmap);
}
inline QPoint AQSDragObject::pixmapHotSpot() const
{
  AQ_CALL_RET_V(pixmapHotSpot(), QPoint);
}
inline QWidget *AQSDragObject::source()
{
  AQ_CALL_RET(source());
}
inline const char *AQSDragObject::format(int arg0) const
{
  AQ_CALL_RET(format(arg0));
}
inline QByteArray *AQSDragObject::encodedData(const char *arg0) const
{
  AQ_CALL_RET_PTR(encodedData(arg0), QByteArray);
}
inline bool AQSDragObject::provides(const char *arg0) const
{
  AQ_CALL_RET_V(provides(arg0), bool);
}
inline int AQSDragObject::serialNumber() const
{
  AQ_CALL_RET_V(serialNumber(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDRAGOBJECT_P_H_ */
// @AQSOBJECT@
