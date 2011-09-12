/***************************************************************************
 AQSWheelEvent_p.h
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

#ifndef AQSWHEELEVENT_P_H_
#define AQSWHEELEVENT_P_H_

#include "AQSEvent_p.h"
#include <qevent.h>

class AQSWheelEvent : public AQSEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(WheelEvent, Event);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int delta() const;
  QPoint pos() const;
  QPoint globalPos() const;
  int x() const;
  int y() const;
  int globalX() const;
  int globalY() const;
  uint state() const;
  uint orientation() const;
  bool isAccepted() const;
  void accept();
  void ignore();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[3].append(QString::fromLatin1("QPoint,int,int"));
    candidates[4].append(QString::fromLatin1("QPoint,int,int,Qt::Orientation"));
    candidates[4].append(QString::fromLatin1("QPoint,QPoint,int,int"));
    candidates[5].append(QString::fromLatin1("QPoint,QPoint,int,int,Qt::Orientation"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QPoint,int,int"))
      return new QWheelEvent(*(argValue<QPoint *>(args[0])),
                             args[1].variant().toInt(),
                             args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QPoint,int,int,Qt::Orientation"))
      return new QWheelEvent(*(argValue<QPoint *>(args[0])),
                             args[1].variant().toInt(),
                             args[2].variant().toInt(),
                             static_cast<Qt::Orientation>(args[3].variant().toUInt()));
    if (sgt == QString::fromLatin1("QPoint,QPoint,int,int"))
      return new QWheelEvent(*(argValue<QPoint *>(args[0])),
                             *(argValue<QPoint *>(args[1])),
                             args[2].variant().toInt(),
                             args[3].variant().toInt());
    if (sgt == QString::fromLatin1("QPoint,QPoint,int,int,Qt::Orientation"))
      return new QWheelEvent(*(argValue<QPoint *>(args[0])),
                             *(argValue<QPoint *>(args[1])),
                             args[2].variant().toInt(),
                             args[3].variant().toInt(),
                             static_cast<Qt::Orientation>(args[4].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[3].append(QString::fromLatin1("QPoint,int,int"));
    candidates[4].append(QString::fromLatin1("QPoint,int,int,Qt::Orientation"));
    candidates[4].append(QString::fromLatin1("QPoint,QPoint,int,int"));
    candidates[5].append(QString::fromLatin1("QPoint,QPoint,int,int,Qt::Orientation"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSWheelEvent::delta() const
{
  AQ_CALL_RET_V(delta(), int);
}
inline QPoint AQSWheelEvent::pos() const
{
  AQ_CALL_RET_V(pos(), QPoint);
}
inline QPoint AQSWheelEvent::globalPos() const
{
  AQ_CALL_RET_V(globalPos(), QPoint);
}
inline int AQSWheelEvent::x() const
{
  AQ_CALL_RET_V(x(), int);
}
inline int AQSWheelEvent::y() const
{
  AQ_CALL_RET_V(y(), int);
}
inline int AQSWheelEvent::globalX() const
{
  AQ_CALL_RET_V(globalX(), int);
}
inline int AQSWheelEvent::globalY() const
{
  AQ_CALL_RET_V(globalY(), int);
}
inline uint AQSWheelEvent::state() const
{
  AQ_CALL_RET_V(state(), uint);
}
inline uint AQSWheelEvent::orientation() const
{
  AQ_CALL_RET_V(orientation(), uint);
}
inline bool AQSWheelEvent::isAccepted() const
{
  AQ_CALL_RET_V(isAccepted(), bool);
}
inline void AQSWheelEvent::accept()
{
  AQ_CALL_VOID(accept());
}
inline void AQSWheelEvent::ignore()
{
  AQ_CALL_VOID(ignore());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSWHEELEVENT_P_H_ */
// @AQSVOIDPTR@
