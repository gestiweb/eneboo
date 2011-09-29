/***************************************************************************
 AQSTimer_p.h
 -------------------
 begin                : 30/01/2011
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

#ifndef AQSTIMER_P_H_
#define AQSTIMER_P_H_

#include "AQSObject_p.h"
#include <qtimer.h>

class AQSTimer : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Timer, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool isActive() const;
  int start(int, bool = FALSE);
  void changeInterval(int);
  void stop();
  int timerId() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QTimer;
    if (sgt == QString::fromLatin1("QObject*"))
      return new QTimer(argValue<QObject *>(args[0]));
    if (sgt == QString::fromLatin1("QObject*,QString"))
      return new QTimer(argValue<QObject *>(args[0]),
                        *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSTimer::isActive() const
{
  AQ_CALL_RET_V(isActive(), bool);
}
inline int AQSTimer::start(int arg0,  bool arg1)
{
  AQ_CALL_RET_V(start(arg0, arg1), int);
}
inline void AQSTimer::changeInterval(int arg0)
{
  AQ_CALL_VOID(changeInterval(arg0));
}
inline void AQSTimer::stop()
{
  AQ_CALL_VOID(stop());
}
inline int AQSTimer::timerId() const
{
  AQ_CALL_RET_V(timerId(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTIMER_P_H_ */
// @AQSOBJECT@
