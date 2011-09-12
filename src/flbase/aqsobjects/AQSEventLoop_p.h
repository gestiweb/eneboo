/***************************************************************************
 AQSEventLoop_p.h
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

#ifndef AQSEVENTLOOP_P_H_
#define AQSEVENTLOOP_P_H_

#include "AQSObject_p.h"
#include <qeventloop.h>

class AQSEventLoop : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(EventLoop, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void processEvents(uint, int);
  virtual bool processEvents(uint);
  virtual bool hasPendingEvents() const;
  virtual void registerSocketNotifier(QSocketNotifier *);
  virtual void unregisterSocketNotifier(QSocketNotifier *);
  void setSocketNotifierPending(QSocketNotifier *);
  int activateSocketNotifiers();
  int activateTimers();
  int timeToWait() const;
  virtual int exec();
  virtual void exit(int = 0);
  virtual int enterLoop();
  virtual void exitLoop();
  virtual int loopLevel() const;
  virtual void wakeUp();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QEventLoop;
    if (sgt == QString::fromLatin1("QObject*"))
      return new QEventLoop(argValue<QObject *>(args[0]));
    if (sgt == QString::fromLatin1("QObject*,QString"))
      return new QEventLoop(argValue<QObject *>(args[0]),
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
inline void AQSEventLoop::processEvents(uint arg0,  int arg1)
{
  AQ_CALL_VOID(processEvents(static_cast<QEventLoop::ProcessEventsFlags>(arg0), arg1));
}
inline bool AQSEventLoop::processEvents(uint arg0)
{
  AQ_CALL_RET_V(processEvents(static_cast<QEventLoop::ProcessEventsFlags>(arg0)), bool);
}
inline bool AQSEventLoop::hasPendingEvents() const
{
  AQ_CALL_RET_V(hasPendingEvents(), bool);
}
inline void AQSEventLoop::registerSocketNotifier(QSocketNotifier *arg0)
{
  AQ_CALL_VOID(registerSocketNotifier(arg0));
}
inline void AQSEventLoop::unregisterSocketNotifier(QSocketNotifier *arg0)
{
  AQ_CALL_VOID(unregisterSocketNotifier(arg0));
}
inline void AQSEventLoop::setSocketNotifierPending(QSocketNotifier *arg0)
{
  AQ_CALL_VOID(setSocketNotifierPending(arg0));
}
inline int AQSEventLoop::activateSocketNotifiers()
{
  AQ_CALL_RET_V(activateSocketNotifiers(), int);
}
inline int AQSEventLoop::activateTimers()
{
  AQ_CALL_RET_V(activateTimers(), int);
}
inline int AQSEventLoop::timeToWait() const
{
  AQ_CALL_RET_V(timeToWait(), int);
}
inline int AQSEventLoop::exec()
{
  AQ_CALL_RET_V(exec(), int);
}
inline void AQSEventLoop::exit(int arg0)
{
  AQ_CALL_VOID(exit(arg0));
}
inline int AQSEventLoop::enterLoop()
{
  AQ_CALL_RET_V(enterLoop(), int);
}
inline void AQSEventLoop::exitLoop()
{
  AQ_CALL_VOID(exitLoop());
}
inline int AQSEventLoop::loopLevel() const
{
  AQ_CALL_RET_V(loopLevel(), int);
}
inline void AQSEventLoop::wakeUp()
{
  AQ_CALL_VOID(wakeUp());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSEVENTLOOP_P_H_ */
// @AQSOBJECT@
