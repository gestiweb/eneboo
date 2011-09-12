/***************************************************************************
 AQSDragMoveEvent_p.h
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

#ifndef AQSDRAGMOVEEVENT_P_H_
#define AQSDRAGMOVEEVENT_P_H_

#include "AQSDropEvent_p.h"
#include <qevent.h>

class AQSDragMoveEvent : public AQSDropEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DragMoveEvent, DropEvent);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QRect answerRect() const;
  void accept(const QRect &);
  void ignore(const QRect &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QPoint"));
    candidates[2].append(QString::fromLatin1("QPoint,QEvent::Type"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QPoint"))
      return new QDragMoveEvent(*(argValue<QPoint *>(args[0])));
    if (sgt == QString::fromLatin1("QPoint,QEvent::Type"))
      return new QDragMoveEvent(*(argValue<QPoint *>(args[0])),
                                static_cast<QEvent::Type>(args[1].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QPoint"));
    candidates[2].append(QString::fromLatin1("QPoint,QEvent::Type"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QRect AQSDragMoveEvent::answerRect() const
{
  AQ_CALL_RET_V(answerRect(), QRect);
}
inline void AQSDragMoveEvent::accept(const QRect &arg0)
{
  AQ_CALL_VOID(accept(arg0));
}
inline void AQSDragMoveEvent::ignore(const QRect &arg0)
{
  AQ_CALL_VOID(ignore(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDRAGMOVEEVENT_P_H_ */
// @AQSVOIDPTR@
