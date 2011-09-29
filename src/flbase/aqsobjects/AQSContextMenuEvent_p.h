/***************************************************************************
 AQSContextMenuEvent_p.h
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

#ifndef AQSCONTEXTMENUEVENT_P_H_
#define AQSCONTEXTMENUEVENT_P_H_

#include "AQSEvent_p.h"
#include <qevent.h>

class AQSContextMenuEvent : public AQSEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(ContextMenuEvent, Event);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int x() const;
  int y() const;
  int globalX() const;
  int globalY() const;
  QPoint pos() const;
  QPoint globalPos() const;
  uint state() const;
  bool isAccepted() const;
  bool isConsumed() const;
  void consume();
  void accept();
  void ignore();
  uint reason() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[4].append(QString::fromLatin1("QContextMenuEvent::Reason,QPoint,QPoint,int"));
    candidates[3].append(QString::fromLatin1("QContextMenuEvent::Reason,QPoint,int"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QContextMenuEvent::Reason,QPoint,QPoint,int"))
      return new QContextMenuEvent(static_cast<QContextMenuEvent::Reason>(args[0].variant().toUInt()),
                                   *(argValue<QPoint *>(args[1])),
                                   *(argValue<QPoint *>(args[2])),
                                   args[3].variant().toInt());
    if (sgt == QString::fromLatin1("QContextMenuEvent::Reason,QPoint,int"))
      return new QContextMenuEvent(static_cast<QContextMenuEvent::Reason>(args[0].variant().toUInt()),
                                   *(argValue<QPoint *>(args[1])),
                                   args[2].variant().toInt());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[4].append(QString::fromLatin1("QContextMenuEvent::Reason,QPoint,QPoint,int"));
    candidates[3].append(QString::fromLatin1("QContextMenuEvent::Reason,QPoint,int"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSContextMenuEvent::x() const
{
  AQ_CALL_RET_V(x(), int);
}
inline int AQSContextMenuEvent::y() const
{
  AQ_CALL_RET_V(y(), int);
}
inline int AQSContextMenuEvent::globalX() const
{
  AQ_CALL_RET_V(globalX(), int);
}
inline int AQSContextMenuEvent::globalY() const
{
  AQ_CALL_RET_V(globalY(), int);
}
inline QPoint AQSContextMenuEvent::pos() const
{
  AQ_CALL_RET_V(pos(), QPoint);
}
inline QPoint AQSContextMenuEvent::globalPos() const
{
  AQ_CALL_RET_V(globalPos(), QPoint);
}
inline uint AQSContextMenuEvent::state() const
{
  AQ_CALL_RET_V(state(), uint);
}
inline bool AQSContextMenuEvent::isAccepted() const
{
  AQ_CALL_RET_V(isAccepted(), bool);
}
inline bool AQSContextMenuEvent::isConsumed() const
{
  AQ_CALL_RET_V(isConsumed(), bool);
}
inline void AQSContextMenuEvent::consume()
{
  AQ_CALL_VOID(consume());
}
inline void AQSContextMenuEvent::accept()
{
  AQ_CALL_VOID(accept());
}
inline void AQSContextMenuEvent::ignore()
{
  AQ_CALL_VOID(ignore());
}
inline uint AQSContextMenuEvent::reason() const
{
  AQ_CALL_RET_V(reason(), uint);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCONTEXTMENUEVENT_P_H_ */
// @AQSVOIDPTR@
