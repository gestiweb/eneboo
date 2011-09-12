/***************************************************************************
 AQSFocusEvent_p.h
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

#ifndef AQSFOCUSEVENT_P_H_
#define AQSFOCUSEVENT_P_H_

#include "AQSEvent_p.h"
#include <qevent.h>

class AQSFocusEvent : public AQSEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(FocusEvent, Event);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool gotFocus() const;
  bool lostFocus() const;
  uint reason();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QEvent::Type"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QEvent::Type"))
      return new QFocusEvent(static_cast<QEvent::Type>(args[0].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QEvent::Type"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSFocusEvent::gotFocus() const
{
  AQ_CALL_RET_V(gotFocus(), bool);
}
inline bool AQSFocusEvent::lostFocus() const
{
  AQ_CALL_RET_V(lostFocus(), bool);
}
inline uint AQSFocusEvent::reason()
{
  AQ_CALL_RET_V(reason(), uint);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSFOCUSEVENT_P_H_ */
// @AQSVOIDPTR@
