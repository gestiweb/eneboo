/***************************************************************************
 AQSResizeEvent_p.h
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

#ifndef AQSRESIZEEVENT_P_H_
#define AQSRESIZEEVENT_P_H_

#include "AQSEvent_p.h"
#include <qevent.h>

class AQSResizeEvent : public AQSEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(ResizeEvent, Event);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QSize size() const;
  QSize oldSize() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QSize,QSize"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QSize,QSize"))
      return new QResizeEvent(*(argValue<QSize *>(args[0])),
                              *(argValue<QSize *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QSize,QSize"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QSize AQSResizeEvent::size() const
{
  AQ_CALL_RET_V(size(), QSize);
}
inline QSize AQSResizeEvent::oldSize() const
{
  AQ_CALL_RET_V(oldSize(), QSize);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSRESIZEEVENT_P_H_ */
// @AQSVOIDPTR@
