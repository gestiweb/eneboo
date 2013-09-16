/***************************************************************************
 AQSFtp_p.h
 -------------------
 begin                : 29/07/2013
 copyright            : (C) 2003-2013 by InfoSiAL S.L.
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

#ifndef AQSFTP_P_H_
#define AQSFTP_P_H_

#include "AQSNetworkProtocol_p.h"
#include "AQSIODevice_p.h"
#include <qftp.h>

class AQSFtp : public AQSNetworkProtocol
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Ftp, NetworkProtocol);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int supportedOperations() const;
  int connectToHost(const QString &, uint = 21);
  int login(const QString& = QString::null, const QString& = QString::null);
  int close();
  int list(const QString& = QString::null);
  int cd(const QString &);
  int get(const QString &, QIODevice *);
  int get(const QString &, AQSIODevice *);
  int put(QByteArray *, const QString &);
  int put(AQSByteArray *, const QString &);
  int put(QIODevice *, const QString &);
  int put(AQSIODevice *, const QString &);
  int remove(const QString &);
  int mkdir(const QString &);
  int rmdir(const QString &);
  int rename(const QString &, const QString &);
  int rawCommand(const QString &);
  ulong bytesAvailable() const;
  long readBlock(char *, Q_ULONG);
  QByteArray readAll();
  int currentId() const;
  QIODevice *currentDevice() const;
  uint currentCommand() const;
  bool hasPendingCommands() const;
  void clearPendingCommands();
  uint state() const;
  uint error() const;
  QString errorString() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QFtp;
    if (sgt == QString::fromLatin1("QObject*"))
      return new QFtp(argValue<QObject *>(args[0]));
    if (sgt == QString::fromLatin1("QObject*,QString"))
      return new QFtp(argValue<QObject *>(args[0]),
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
inline int AQSFtp::supportedOperations() const
{
  AQ_CALL_RET_V(supportedOperations(), int);
}
inline int AQSFtp::connectToHost(const QString &arg0,  uint arg1)
{
  AQ_CALL_RET_V(connectToHost(arg0, arg1), int);
}
inline int AQSFtp::login(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(login(arg0, arg1), int);
}
inline int AQSFtp::close()
{
  AQ_CALL_RET_V(close(), int);
}
inline int AQSFtp::list(const QString &arg0)
{
  AQ_CALL_RET_V(list(arg0), int);
}
inline int AQSFtp::cd(const QString &arg0)
{
  AQ_CALL_RET_V(cd(arg0), int);
}
inline int AQSFtp::get(const QString &arg0,  QIODevice *arg1)
{
  AQ_CALL_RET_V(get(arg0, arg1), int);
}
inline int AQSFtp::get(const QString &arg0, AQSIODevice *arg1)
{
  AQ_CALL_RET_V(get(arg0, *arg1), int);
}
inline int AQSFtp::put(QByteArray *arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(put(*arg0, arg1), int);
}
inline int AQSFtp::put(AQSByteArray *arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(put(*arg0, arg1), int);
}
inline int AQSFtp::put(QIODevice *arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(put(arg0, arg1), int);
}
inline int AQSFtp::put(AQSIODevice *arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(put(*arg0, arg1), int);
}
inline int AQSFtp::remove(const QString &arg0)
{
  AQ_CALL_RET_V(remove(arg0), int);
}
inline int AQSFtp::mkdir(const QString &arg0)
{
  AQ_CALL_RET_V(mkdir(arg0), int);
}
inline int AQSFtp::rmdir(const QString &arg0)
{
  AQ_CALL_RET_V(rmdir(arg0), int);
}
inline int AQSFtp::rename(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(rename(arg0, arg1), int);
}
inline int AQSFtp::rawCommand(const QString &arg0)
{
  AQ_CALL_RET_V(rawCommand(arg0), int);
}
inline ulong AQSFtp::bytesAvailable() const
{
  AQ_CALL_RET_V(bytesAvailable(), ulong);
}
inline long AQSFtp::readBlock(char *arg0,  Q_ULONG arg1)
{
  AQ_CALL_RET_V(readBlock(arg0, arg1), long);
}
inline QByteArray AQSFtp::readAll()
{
  AQ_CALL_RET_V(readAll(), QByteArray);
}
inline int AQSFtp::currentId() const
{
  AQ_CALL_RET_V(currentId(), int);
}
inline QIODevice *AQSFtp::currentDevice() const
{
  AQ_CALL_RET(currentDevice());
}
inline uint AQSFtp::currentCommand() const
{
  AQ_CALL_RET_V(currentCommand(), uint);
}
inline bool AQSFtp::hasPendingCommands() const
{
  AQ_CALL_RET_V(hasPendingCommands(), bool);
}
inline void AQSFtp::clearPendingCommands()
{
  AQ_CALL_VOID(clearPendingCommands());
}
inline uint AQSFtp::state() const
{
  AQ_CALL_RET_V(state(), uint);
}
inline uint AQSFtp::error() const
{
  AQ_CALL_RET_V(error(), uint);
}
inline QString AQSFtp::errorString() const
{
  AQ_CALL_RET_V(errorString(), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSFTP_P_H_ */
// @AQSOBJECT@
