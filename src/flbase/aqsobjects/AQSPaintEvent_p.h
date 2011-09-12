/***************************************************************************
 AQSPaintEvent_p.h
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

#ifndef AQSPAINTEVENT_P_H_
#define AQSPAINTEVENT_P_H_

#include "AQSEvent_p.h"
#include <qevent.h>

class AQSPaintEvent : public AQSEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(PaintEvent, Event);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  const QRect &rect() const;
  QRegion *region() const;
  bool erased() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QRegion*"));
    candidates[2].append(QString::fromLatin1("QRegion*,bool"));
    candidates[1].append(QString::fromLatin1("QRect"));
    candidates[2].append(QString::fromLatin1("QRect,bool"));
    candidates[2].append(QString::fromLatin1("QRegion*,QRect"));
    candidates[3].append(QString::fromLatin1("QRegion*,QRect,bool"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QRegion*"))
      return new QPaintEvent(*argValue<QRegion *>(args[0]));
    if (sgt == QString::fromLatin1("QRegion*,bool"))
      return new QPaintEvent(*argValue<QRegion *>(args[0]),
                             args[1].variant().toBool());
    if (sgt == QString::fromLatin1("QRect"))
      return new QPaintEvent(*(argValue<QRect *>(args[0])));
    if (sgt == QString::fromLatin1("QRect,bool"))
      return new QPaintEvent(*(argValue<QRect *>(args[0])),
                             args[1].variant().toBool());
    if (sgt == QString::fromLatin1("QRegion*,QRect"))
      return new QPaintEvent(*argValue<QRegion *>(args[0]),
                             *(argValue<QRect *>(args[1])));
    if (sgt == QString::fromLatin1("QRegion*,QRect,bool"))
      return new QPaintEvent(*argValue<QRegion *>(args[0]),
                             *(argValue<QRect *>(args[1])),
                             args[2].variant().toBool());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QRegion*"));
    candidates[2].append(QString::fromLatin1("QRegion*,bool"));
    candidates[1].append(QString::fromLatin1("QRect"));
    candidates[2].append(QString::fromLatin1("QRect,bool"));
    candidates[2].append(QString::fromLatin1("QRegion*,QRect"));
    candidates[3].append(QString::fromLatin1("QRegion*,QRect,bool"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline const QRect &AQSPaintEvent::rect() const
{
  AQ_CALL_RET_V(rect(), QRect);
}
inline QRegion *AQSPaintEvent::region() const
{
  AQ_CALL_RET_PTR(region(), QRegion);
}
inline bool AQSPaintEvent::erased() const
{
  AQ_CALL_RET_V(erased(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPAINTEVENT_P_H_ */
// @AQSVOIDPTR@
