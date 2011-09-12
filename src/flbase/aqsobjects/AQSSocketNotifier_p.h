/***************************************************************************
 AQSSocketNotifier_p.h
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

#ifndef AQSSOCKETNOTIFIER_P_H_
#define AQSSOCKETNOTIFIER_P_H_

#include "AQSObject_p.h"
#include <qsocketnotifier.h>

class AQSSocketNotifier : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(SocketNotifier, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int socket() const;
  uint type() const;
  bool isEnabled() const;
  virtual void setEnabled(bool);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("int,QSocketNotifier::Type"));
    candidates[3].append(QString::fromLatin1("int,QSocketNotifier::Type,QObject*"));
    candidates[4].append(QString::fromLatin1("int,QSocketNotifier::Type,QObject*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("int,QSocketNotifier::Type"))
      return new QSocketNotifier(args[0].variant().toInt(),
                                 static_cast<QSocketNotifier::Type>(args[1].variant().toUInt()));
    if (sgt == QString::fromLatin1("int,QSocketNotifier::Type,QObject*"))
      return new QSocketNotifier(args[0].variant().toInt(),
                                 static_cast<QSocketNotifier::Type>(args[1].variant().toUInt()),
                                 argValue<QObject *>(args[2]));
    if (sgt == QString::fromLatin1("int,QSocketNotifier::Type,QObject*,QString"))
      return new QSocketNotifier(args[0].variant().toInt(),
                                 static_cast<QSocketNotifier::Type>(args[1].variant().toUInt()),
                                 argValue<QObject *>(args[2]),
                                 *(argValue<QString *>(args[3])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("int,QSocketNotifier::Type"));
    candidates[3].append(QString::fromLatin1("int,QSocketNotifier::Type,QObject*"));
    candidates[4].append(QString::fromLatin1("int,QSocketNotifier::Type,QObject*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSSocketNotifier::socket() const
{
  AQ_CALL_RET_V(socket(), int);
}
inline uint AQSSocketNotifier::type() const
{
  AQ_CALL_RET_V(type(), uint);
}
inline bool AQSSocketNotifier::isEnabled() const
{
  AQ_CALL_RET_V(isEnabled(), bool);
}
inline void AQSSocketNotifier::setEnabled(bool arg0)
{
  AQ_CALL_VOID(setEnabled(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSOCKETNOTIFIER_P_H_ */
// @AQSOBJECT@
