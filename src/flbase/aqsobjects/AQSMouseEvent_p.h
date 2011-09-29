/***************************************************************************
 AQSMouseEvent_p.h
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

#ifndef AQSMOUSEEVENT_P_H_
#define AQSMOUSEEVENT_P_H_

#include "AQSEvent_p.h"
#include <qevent.h>

class AQSMouseEvent : public AQSEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(MouseEvent, Event);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QPoint pos() const;
  QPoint globalPos() const;
  int x() const;
  int y() const;
  int globalX() const;
  int globalY() const;
  uint button() const;
  uint state() const;
  uint stateAfter() const;
  bool isAccepted() const;
  void accept();
  void ignore();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[4].append(QString::fromLatin1("QEvent::Type,QPoint,int,int"));
    candidates[5].append(QString::fromLatin1("QEvent::Type,QPoint,QPoint,int,int"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QEvent::Type,QPoint,int,int"))
      return new QMouseEvent(static_cast<QEvent::Type>(args[0].variant().toUInt()),
                             *(argValue<QPoint *>(args[1])),
                             args[2].variant().toInt(),
                             args[3].variant().toInt());
    if (sgt == QString::fromLatin1("QEvent::Type,QPoint,QPoint,int,int"))
      return new QMouseEvent(static_cast<QEvent::Type>(args[0].variant().toUInt()),
                             *(argValue<QPoint *>(args[1])),
                             *(argValue<QPoint *>(args[2])),
                             args[3].variant().toInt(),
                             args[4].variant().toInt());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[4].append(QString::fromLatin1("QEvent::Type,QPoint,int,int"));
    candidates[5].append(QString::fromLatin1("QEvent::Type,QPoint,QPoint,int,int"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QPoint AQSMouseEvent::pos() const
{
  AQ_CALL_RET_V(pos(), QPoint);
}
inline QPoint AQSMouseEvent::globalPos() const
{
  AQ_CALL_RET_V(globalPos(), QPoint);
}
inline int AQSMouseEvent::x() const
{
  AQ_CALL_RET_V(x(), int);
}
inline int AQSMouseEvent::y() const
{
  AQ_CALL_RET_V(y(), int);
}
inline int AQSMouseEvent::globalX() const
{
  AQ_CALL_RET_V(globalX(), int);
}
inline int AQSMouseEvent::globalY() const
{
  AQ_CALL_RET_V(globalY(), int);
}
inline uint AQSMouseEvent::button() const
{
  AQ_CALL_RET_V(button(), uint);
}
inline uint AQSMouseEvent::state() const
{
  AQ_CALL_RET_V(state(), uint);
}
inline uint AQSMouseEvent::stateAfter() const
{
  AQ_CALL_RET_V(stateAfter(), uint);
}
inline bool AQSMouseEvent::isAccepted() const
{
  AQ_CALL_RET_V(isAccepted(), bool);
}
inline void AQSMouseEvent::accept()
{
  AQ_CALL_VOID(accept());
}
inline void AQSMouseEvent::ignore()
{
  AQ_CALL_VOID(ignore());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSMOUSEEVENT_P_H_ */
// @AQSVOIDPTR@
