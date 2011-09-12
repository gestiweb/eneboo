/***************************************************************************
 AQSKeyEvent_p.h
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

#ifndef AQSKEYEVENT_P_H_
#define AQSKEYEVENT_P_H_

#include "AQSEvent_p.h"
#include <qevent.h>

class AQSKeyEvent : public AQSEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(KeyEvent, Event);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int key() const;
  int ascii() const;
  uint state() const;
  uint stateAfter() const;
  bool isAccepted() const;
  QString text() const;
  bool isAutoRepeat() const;
  int count() const;
  void accept();
  void ignore();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[4].append(QString::fromLatin1("QEvent::Type,int,int,int"));
    candidates[5].append(QString::fromLatin1("QEvent::Type,int,int,int,QString"));
    candidates[6].append(QString::fromLatin1("QEvent::Type,int,int,int,QString,bool"));
    candidates[7].append(QString::fromLatin1("QEvent::Type,int,int,int,QString,bool,ushort"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QEvent::Type,int,int,int"))
      return new QKeyEvent(static_cast<QEvent::Type>(args[0].variant().toUInt()),
                           args[1].variant().toInt(),
                           args[2].variant().toInt(),
                           args[3].variant().toInt());
    if (sgt == QString::fromLatin1("QEvent::Type,int,int,int,QString"))
      return new QKeyEvent(static_cast<QEvent::Type>(args[0].variant().toUInt()),
                           args[1].variant().toInt(),
                           args[2].variant().toInt(),
                           args[3].variant().toInt(),
                           *(argValue<QString *>(args[4])));
    if (sgt == QString::fromLatin1("QEvent::Type,int,int,int,QString,bool"))
      return new QKeyEvent(static_cast<QEvent::Type>(args[0].variant().toUInt()),
                           args[1].variant().toInt(),
                           args[2].variant().toInt(),
                           args[3].variant().toInt(),
                           *(argValue<QString *>(args[4])),
                           args[5].variant().toBool());
    if (sgt == QString::fromLatin1("QEvent::Type,int,int,int,QString,bool,ushort"))
      return new QKeyEvent(static_cast<QEvent::Type>(args[0].variant().toUInt()),
                           args[1].variant().toInt(),
                           args[2].variant().toInt(),
                           args[3].variant().toInt(),
                           *(argValue<QString *>(args[4])),
                           args[5].variant().toBool(),
                           *(argValue<ushort *>(args[6])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[4].append(QString::fromLatin1("QEvent::Type,int,int,int"));
    candidates[5].append(QString::fromLatin1("QEvent::Type,int,int,int,QString"));
    candidates[6].append(QString::fromLatin1("QEvent::Type,int,int,int,QString,bool"));
    candidates[7].append(QString::fromLatin1("QEvent::Type,int,int,int,QString,bool,ushort"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSKeyEvent::key() const
{
  AQ_CALL_RET_V(key(), int);
}
inline int AQSKeyEvent::ascii() const
{
  AQ_CALL_RET_V(ascii(), int);
}
inline uint AQSKeyEvent::state() const
{
  AQ_CALL_RET_V(state(), uint);
}
inline uint AQSKeyEvent::stateAfter() const
{
  AQ_CALL_RET_V(stateAfter(), uint);
}
inline bool AQSKeyEvent::isAccepted() const
{
  AQ_CALL_RET_V(isAccepted(), bool);
}
inline QString AQSKeyEvent::text() const
{
  AQ_CALL_RET_V(text(), QString);
}
inline bool AQSKeyEvent::isAutoRepeat() const
{
  AQ_CALL_RET_V(isAutoRepeat(), bool);
}
inline int AQSKeyEvent::count() const
{
  AQ_CALL_RET_V(count(), int);
}
inline void AQSKeyEvent::accept()
{
  AQ_CALL_VOID(accept());
}
inline void AQSKeyEvent::ignore()
{
  AQ_CALL_VOID(ignore());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSKEYEVENT_P_H_ */
// @AQSVOIDPTR@
