/***************************************************************************
 AQSCustomEvent_p.h
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

#ifndef AQSCUSTOMEVENT_P_H_
#define AQSCUSTOMEVENT_P_H_

#include "AQSEvent_p.h"
#include <qevent.h>

class AQSCustomEvent : public AQSEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CustomEvent, Event);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void *data() const;
  void setData(void *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("int"));
    candidates[2].append(QString::fromLatin1("QEvent::Type,void*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("int"))
      return new QCustomEvent(args[0].variant().toInt());
    if (sgt == QString::fromLatin1("QEvent::Type,void*"))
      return new QCustomEvent(static_cast<QEvent::Type>(args[0].variant().toUInt()),
                              argValue<void *>(args[1]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("int"));
    candidates[2].append(QString::fromLatin1("QEvent::Type,void*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void *AQSCustomEvent::data() const
{
  AQ_CALL_RET(data());
}
inline void AQSCustomEvent::setData(void *arg0)
{
  AQ_CALL_VOID(setData(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCUSTOMEVENT_P_H_ */
// @AQSVOIDPTR@
