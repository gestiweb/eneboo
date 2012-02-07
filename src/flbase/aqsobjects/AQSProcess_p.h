/***************************************************************************
 AQSProcess_p.h
 -------------------
 begin                : 20/11/2011
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

#ifndef AQSPROCESS_P_H_
#define AQSPROCESS_P_H_

#include "AQSObject_p.h"
#include "AQSDir_p.h"
#include <qprocess.h>

class AQSProcess : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Process, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QStringList arguments() const;
  void clearArguments();
  virtual void setArguments(const QStringList &);
  virtual void addArgument(const QString &);
  QDir *workingDirectory() const;
  virtual void setWorkingDirectory(QDir *);
  virtual void setWorkingDirectory(AQSDir *);
  void setCommunication(int);
  uint communication() const;
  virtual bool start(QStringList* = 0);
  virtual bool launch(const QString &, QStringList* = 0);
  virtual bool launch(const QByteArray &, QStringList* = 0);
  bool isRunning() const;
  bool normalExit() const;
  int exitStatus() const;
  virtual QByteArray readStdout();
  virtual QByteArray readStderr();
  bool canReadLineStdout() const;
  bool canReadLineStderr() const;
  virtual QString readLineStdout();
  virtual QString readLineStderr();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QObject*"));
    candidates[3].append(QString::fromLatin1("QString,QObject*,QString"));
    candidates[1].append(QString::fromLatin1("QStringList"));
    candidates[2].append(QString::fromLatin1("QStringList,QObject*"));
    candidates[3].append(QString::fromLatin1("QStringList,QObject*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QProcess;
    if (sgt == QString::fromLatin1("QObject*"))
      return new QProcess(argValue<QObject *>(args[0]));
    if (sgt == QString::fromLatin1("QObject*,QString"))
      return new QProcess(argValue<QObject *>(args[0]),
                          *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString"))
      return new QProcess(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QObject*"))
      return new QProcess(*(argValue<QString *>(args[0])),
                          argValue<QObject *>(args[1]));
    if (sgt == QString::fromLatin1("QString,QObject*,QString"))
      return new QProcess(*(argValue<QString *>(args[0])),
                          argValue<QObject *>(args[1]),
                          *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QStringList"))
      return new QProcess(*(argValue<QStringList *>(args[0])));
    if (sgt == QString::fromLatin1("QStringList,QObject*"))
      return new QProcess(*(argValue<QStringList *>(args[0])),
                          argValue<QObject *>(args[1]));
    if (sgt == QString::fromLatin1("QStringList,QObject*,QString"))
      return new QProcess(*(argValue<QStringList *>(args[0])),
                          argValue<QObject *>(args[1]),
                          *(argValue<QString *>(args[2])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QObject*"));
    candidates[3].append(QString::fromLatin1("QString,QObject*,QString"));
    candidates[1].append(QString::fromLatin1("QStringList"));
    candidates[2].append(QString::fromLatin1("QStringList,QObject*"));
    candidates[3].append(QString::fromLatin1("QStringList,QObject*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QStringList AQSProcess::arguments() const
{
  AQ_CALL_RET_V(arguments(), QStringList);
}
inline void AQSProcess::clearArguments()
{
  AQ_CALL_VOID(clearArguments());
}
inline void AQSProcess::setArguments(const QStringList &arg0)
{
  AQ_CALL_VOID(setArguments(arg0));
}
inline void AQSProcess::addArgument(const QString &arg0)
{
  AQ_CALL_VOID(addArgument(arg0));
}
inline QDir *AQSProcess::workingDirectory() const
{
  AQ_CALL_RET_PTR(workingDirectory(), QDir);
}
inline void AQSProcess::setWorkingDirectory(QDir *arg0)
{
  AQ_CALL_VOID(setWorkingDirectory(*arg0));
}
inline void AQSProcess::setWorkingDirectory(AQSDir *arg0)
{
  AQ_CALL_VOID(setWorkingDirectory(*arg0));
}
inline void AQSProcess::setCommunication(int arg0)
{
  AQ_CALL_VOID(setCommunication(arg0));
}
inline uint AQSProcess::communication() const
{
  AQ_CALL_RET_V(communication(), uint);
}
inline bool AQSProcess::start(QStringList *arg0)
{
  AQ_CALL_RET_V(start(arg0), bool);
}
inline bool AQSProcess::launch(const QString &arg0,  QStringList *arg1)
{
  AQ_CALL_RET_V(launch(arg0, arg1), bool);
}
inline bool AQSProcess::launch(const QByteArray &arg0,  QStringList *arg1)
{
  AQ_CALL_RET_V(launch(arg0, arg1), bool);
}
inline bool AQSProcess::isRunning() const
{
  AQ_CALL_RET_V(isRunning(), bool);
}
inline bool AQSProcess::normalExit() const
{
  AQ_CALL_RET_V(normalExit(), bool);
}
inline int AQSProcess::exitStatus() const
{
  AQ_CALL_RET_V(exitStatus(), int);
}
inline QByteArray AQSProcess::readStdout()
{
  AQ_CALL_RET_V(readStdout(), QByteArray);
}
inline QByteArray AQSProcess::readStderr()
{
  AQ_CALL_RET_V(readStderr(), QByteArray);
}
inline bool AQSProcess::canReadLineStdout() const
{
  AQ_CALL_RET_V(canReadLineStdout(), bool);
}
inline bool AQSProcess::canReadLineStderr() const
{
  AQ_CALL_RET_V(canReadLineStderr(), bool);
}
inline QString AQSProcess::readLineStdout()
{
  AQ_CALL_RET_V(readLineStdout(), QString);
}
inline QString AQSProcess::readLineStderr()
{
  AQ_CALL_RET_V(readLineStderr(), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPROCESS_P_H_ */
// @AQSOBJECT@
