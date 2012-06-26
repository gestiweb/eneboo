/***************************************************************************
 AQSHttp_p.h
 -------------------
 begin                : 28/04/2011
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

#ifndef AQSHTTP_P_H_
#define AQSHTTP_P_H_

#include "AQSNetworkProtocol_p.h"
#include "AQSHttpRequestHeader_p.h"
#include "AQSIODevice_p.h"
#include <qhttp.h>

class AQSHttp : public AQSNetworkProtocol
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Http, NetworkProtocol);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int setHost(const QString &, uint = 80);
  int get(const QString &, QIODevice *);
  int get(const QString &, AQSIODevice *);
  int post(const QString &, QIODevice *, QIODevice *);
  int post(const QString &, QIODevice *, AQSIODevice *);
  int post(const QString &, AQSIODevice *, QIODevice *);
  int post(const QString &, AQSIODevice *, AQSIODevice *);
  int post(const QString &, QByteArray *, QIODevice *);
  int post(const QString &, QByteArray *, AQSIODevice *);
  int post(const QString &, AQSByteArray *, QIODevice *);
  int post(const QString &, AQSByteArray *, AQSIODevice *);
  int head(const QString &);
  int request(QHttpRequestHeader *, QIODevice *, QIODevice *);
  int request(QHttpRequestHeader *, QIODevice *, AQSIODevice *);
  int request(QHttpRequestHeader *, AQSIODevice *, QIODevice *);
  int request(QHttpRequestHeader *, AQSIODevice *, AQSIODevice *);
  int request(AQSHttpRequestHeader *, QIODevice *, QIODevice *);
  int request(AQSHttpRequestHeader *, QIODevice *, AQSIODevice *);
  int request(AQSHttpRequestHeader *, AQSIODevice *, QIODevice *);
  int request(AQSHttpRequestHeader *, AQSIODevice *, AQSIODevice *);
  int request(QHttpRequestHeader *, QByteArray *, QIODevice *);
  int request(QHttpRequestHeader *, QByteArray *, AQSIODevice *);
  int request(QHttpRequestHeader *, AQSByteArray *, QIODevice *);
  int request(QHttpRequestHeader *, AQSByteArray *, AQSIODevice *);
  int request(AQSHttpRequestHeader *, QByteArray *, QIODevice *);
  int request(AQSHttpRequestHeader *, QByteArray *, AQSIODevice *);
  int request(AQSHttpRequestHeader *, AQSByteArray *, QIODevice *);
  int request(AQSHttpRequestHeader *, AQSByteArray *, AQSIODevice *);
  int closeConnection();
  ulong bytesAvailable() const;
  long readBlock(char *, ulong);
  QByteArray readAll();
  int currentId() const;
  QIODevice *currentSourceDevice() const;
  QIODevice *currentDestinationDevice() const;
  QHttpRequestHeader *currentRequest() const;
  bool hasPendingRequests() const;
  void clearPendingRequests();
  uint state() const;
  uint error() const;
  QString errorString() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,uint"));
    candidates[3].append(QString::fromLatin1("QString,uint,QObject*"));
    candidates[4].append(QString::fromLatin1("QString,uint,QObject*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QHttp;
    if (sgt == QString::fromLatin1("QObject*"))
      return new QHttp(argValue<QObject *>(args[0]));
    if (sgt == QString::fromLatin1("QObject*,QString"))
      return new QHttp(argValue<QObject *>(args[0]),
                       *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString"))
      return new QHttp(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,uint"))
      return new QHttp(*(argValue<QString *>(args[0])),
                       args[1].variant().toUInt());
    if (sgt == QString::fromLatin1("QString,uint,QObject*"))
      return new QHttp(*(argValue<QString *>(args[0])),
                       args[1].variant().toUInt(),
                       argValue<QObject *>(args[2]));
    if (sgt == QString::fromLatin1("QString,uint,QObject*,QString"))
      return new QHttp(*(argValue<QString *>(args[0])),
                       args[1].variant().toUInt(),
                       argValue<QObject *>(args[2]),
                       *(argValue<QString *>(args[3])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,uint"));
    candidates[3].append(QString::fromLatin1("QString,uint,QObject*"));
    candidates[4].append(QString::fromLatin1("QString,uint,QObject*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSHttp::setHost(const QString &arg0,  uint arg1)
{
  AQ_CALL_RET_V(setHost(arg0, arg1), int);
}
inline int AQSHttp::get(const QString &arg0,  QIODevice *arg1)
{
  AQ_CALL_RET_V(get(arg0, arg1), int);
}
inline int AQSHttp::get(const QString &arg0, AQSIODevice *arg1)
{
  AQ_CALL_RET_V(get(arg0, *arg1), int);
}
inline int AQSHttp::post(const QString &arg0,  QIODevice *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(post(arg0, arg1, arg2), int);
}
inline int AQSHttp::post(const QString &arg0,  QIODevice *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(post(arg0, arg1, *arg2), int);
}
inline int AQSHttp::post(const QString &arg0, AQSIODevice *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(post(arg0, *arg1, arg2), int);
}
inline int AQSHttp::post(const QString &arg0, AQSIODevice *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(post(arg0, *arg1, *arg2), int);
}
inline int AQSHttp::post(const QString &arg0,  QByteArray *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(post(arg0, *arg1, arg2), int);
}
inline int AQSHttp::post(const QString &arg0,  QByteArray *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(post(arg0, *arg1, *arg2), int);
}
inline int AQSHttp::post(const QString &arg0, AQSByteArray *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(post(arg0, *arg1, arg2), int);
}
inline int AQSHttp::post(const QString &arg0, AQSByteArray *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(post(arg0, *arg1, *arg2), int);
}
inline int AQSHttp::head(const QString &arg0)
{
  AQ_CALL_RET_V(head(arg0), int);
}
inline int AQSHttp::request(QHttpRequestHeader *arg0,  QIODevice *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, arg1, arg2), int);
}
inline int AQSHttp::request(QHttpRequestHeader *arg0,  QIODevice *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, arg1, *arg2), int);
}
inline int AQSHttp::request(QHttpRequestHeader *arg0, AQSIODevice *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, arg2), int);
}
inline int AQSHttp::request(QHttpRequestHeader *arg0, AQSIODevice *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, *arg2), int);
}
inline int AQSHttp::request(AQSHttpRequestHeader *arg0,  QIODevice *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, arg1, arg2), int);
}
inline int AQSHttp::request(AQSHttpRequestHeader *arg0,  QIODevice *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, arg1, *arg2), int);
}
inline int AQSHttp::request(AQSHttpRequestHeader *arg0, AQSIODevice *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, arg2), int);
}
inline int AQSHttp::request(AQSHttpRequestHeader *arg0, AQSIODevice *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, *arg2), int);
}
inline int AQSHttp::request(QHttpRequestHeader *arg0,  QByteArray *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, arg2), int);
}
inline int AQSHttp::request(QHttpRequestHeader *arg0,  QByteArray *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, *arg2), int);
}
inline int AQSHttp::request(QHttpRequestHeader *arg0, AQSByteArray *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, arg2), int);
}
inline int AQSHttp::request(QHttpRequestHeader *arg0, AQSByteArray *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, *arg2), int);
}
inline int AQSHttp::request(AQSHttpRequestHeader *arg0,  QByteArray *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, arg2), int);
}
inline int AQSHttp::request(AQSHttpRequestHeader *arg0,  QByteArray *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, *arg2), int);
}
inline int AQSHttp::request(AQSHttpRequestHeader *arg0, AQSByteArray *arg1,  QIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, arg2), int);
}
inline int AQSHttp::request(AQSHttpRequestHeader *arg0, AQSByteArray *arg1, AQSIODevice *arg2)
{
  AQ_CALL_RET_V(request(*arg0, *arg1, *arg2), int);
}
inline int AQSHttp::closeConnection()
{
  AQ_CALL_RET_V(closeConnection(), int);
}
inline ulong AQSHttp::bytesAvailable() const
{
  AQ_CALL_RET_V(bytesAvailable(), ulong);
}
inline long AQSHttp::readBlock(char *arg0,  ulong arg1)
{
  AQ_CALL_RET_V(readBlock(arg0, arg1), long);
}
inline QByteArray AQSHttp::readAll()
{
  AQ_CALL_RET_V(readAll(), QByteArray);
}
inline int AQSHttp::currentId() const
{
  AQ_CALL_RET_V(currentId(), int);
}
inline QIODevice *AQSHttp::currentSourceDevice() const
{
  AQ_CALL_RET(currentSourceDevice());
}
inline QIODevice *AQSHttp::currentDestinationDevice() const
{
  AQ_CALL_RET(currentDestinationDevice());
}
inline QHttpRequestHeader *AQSHttp::currentRequest() const
{
  AQ_CALL_RET_PTR(currentRequest(), QHttpRequestHeader);
}
inline bool AQSHttp::hasPendingRequests() const
{
  AQ_CALL_RET_V(hasPendingRequests(), bool);
}
inline void AQSHttp::clearPendingRequests()
{
  AQ_CALL_VOID(clearPendingRequests());
}
inline uint AQSHttp::state() const
{
  AQ_CALL_RET_V(state(), uint);
}
inline uint AQSHttp::error() const
{
  AQ_CALL_RET_V(error(), uint);
}
inline QString AQSHttp::errorString() const
{
  AQ_CALL_RET_V(errorString(), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSHTTP_P_H_ */
// @AQSOBJECT@
