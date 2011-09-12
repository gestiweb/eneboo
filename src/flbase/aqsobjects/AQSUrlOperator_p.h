/***************************************************************************
 AQSUrlOperator_p.h
 -------------------
 begin                : 21/03/2011
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

#ifndef AQSURLOPERATOR_P_H_
#define AQSURLOPERATOR_P_H_

#include "AQSNetworkOperation_p.h"
#include <qurloperator.h>

class AQSUrlOperator : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(UrlOperator, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual const QNetworkOperation *listChildren();
  virtual const QNetworkOperation *mkdir(const QString &);
  virtual const QNetworkOperation *remove(const QString &);
  virtual const QNetworkOperation *rename(const QString &, const QString &);
  virtual const QNetworkOperation *get(const QString& = QString::null);
  virtual const QNetworkOperation *put(QByteArray *, const QString& = QString::null);
  virtual const QNetworkOperation *put(AQSByteArray *, const QString& = QString::null);
  virtual void copy(const QStringList &, const QString &, bool = FALSE);
  virtual bool isDir(bool* = 0);
  virtual void setNameFilter(const QString &);
  QString nameFilter() const;
  virtual QUrlInfo info(const QString &) const;
  virtual void stop();
  QString protocol() const;
  virtual void setProtocol(const QString &);
  QString user() const;
  virtual void setUser(const QString &);
  bool hasUser() const;
  QString password() const;
  virtual void setPassword(const QString &);
  bool hasPassword() const;
  QString host() const;
  virtual void setHost(const QString &);
  bool hasHost() const;
  int port() const;
  virtual void setPort(int);
  bool hasPort() const;
  QString path(bool = TRUE) const;
  virtual void setPath(const QString &);
  bool hasPath() const;
  virtual void setEncodedPathAndQuery(const QString &);
  QString encodedPathAndQuery();
  virtual void setQuery(const QString &);
  QString query() const;
  QString ref() const;
  virtual void setRef(const QString &);
  bool hasRef() const;
  bool isValid() const;
  bool isLocalFile() const;
  virtual void addPath(const QString &);
  virtual void setFileName(const QString &);
  QString fileName() const;
  QString dirPath() const;
  virtual QString toString(bool = FALSE, bool = TRUE) const;
  virtual bool cdUp();
  void decode(QString &);
  void encode(QString &);
  bool isRelativeUrl(const QString &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QUrl"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QUrl"));
    candidates[1].append(QString::fromLatin1("QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QUrlOperator;
    if (sgt == QString::fromLatin1("QString"))
      return new QUrlOperator(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QUrl"))
      return new QUrlOperator(*(argValue<QUrl *>(args[0])));
    if (sgt == QString::fromLatin1("QString"))
      return new QUrlOperator(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QUrl"))
      return new QUrlOperator(*(argValue<QUrl *>(args[0])));
    if (sgt == QString::fromLatin1("QString"))
      return new QUrlOperator(*(argValue<QString *>(args[0])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QUrl"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QUrl"));
    candidates[1].append(QString::fromLatin1("QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline const QNetworkOperation *AQSUrlOperator::listChildren()
{
  AQ_CALL_RET(listChildren());
}
inline const QNetworkOperation *AQSUrlOperator::mkdir(const QString &arg0)
{
  AQ_CALL_RET(mkdir(arg0));
}
inline const QNetworkOperation *AQSUrlOperator::remove(const QString &arg0)
{
  AQ_CALL_RET(remove(arg0));
}
inline const QNetworkOperation *AQSUrlOperator::rename(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET(rename(arg0, arg1));
}
inline const QNetworkOperation *AQSUrlOperator::get(const QString &arg0)
{
  AQ_CALL_RET(get(arg0));
}
inline const QNetworkOperation *AQSUrlOperator::put(QByteArray *arg0,  const QString &arg1)
{
  AQ_CALL_RET(put(*arg0, arg1));
}
inline const QNetworkOperation *AQSUrlOperator::put(AQSByteArray *arg0,  const QString &arg1)
{
  AQ_CALL_RET(put(*arg0, arg1));
}
inline void AQSUrlOperator::copy(const QStringList &arg0,  const QString &arg1,  bool arg2)
{
  AQ_CALL_VOID(copy(arg0, arg1, arg2));
}
inline bool AQSUrlOperator::isDir(bool *arg0)
{
  AQ_CALL_RET_V(isDir(arg0), bool);
}
inline void AQSUrlOperator::setNameFilter(const QString &arg0)
{
  AQ_CALL_VOID(setNameFilter(arg0));
}
inline QString AQSUrlOperator::nameFilter() const
{
  AQ_CALL_RET_V(nameFilter(), QString);
}
inline QUrlInfo AQSUrlOperator::info(const QString &arg0) const
{
  AQ_CALL_RET_V(info(arg0), QUrlInfo);
}
inline void AQSUrlOperator::stop()
{
  AQ_CALL_VOID(stop());
}
inline QString AQSUrlOperator::protocol() const
{
  AQ_CALL_RET_V(protocol(), QString);
}
inline void AQSUrlOperator::setProtocol(const QString &arg0)
{
  AQ_CALL_VOID(setProtocol(arg0));
}
inline QString AQSUrlOperator::user() const
{
  AQ_CALL_RET_V(user(), QString);
}
inline void AQSUrlOperator::setUser(const QString &arg0)
{
  AQ_CALL_VOID(setUser(arg0));
}
inline bool AQSUrlOperator::hasUser() const
{
  AQ_CALL_RET_V(hasUser(), bool);
}
inline QString AQSUrlOperator::password() const
{
  AQ_CALL_RET_V(password(), QString);
}
inline void AQSUrlOperator::setPassword(const QString &arg0)
{
  AQ_CALL_VOID(setPassword(arg0));
}
inline bool AQSUrlOperator::hasPassword() const
{
  AQ_CALL_RET_V(hasPassword(), bool);
}
inline QString AQSUrlOperator::host() const
{
  AQ_CALL_RET_V(host(), QString);
}
inline void AQSUrlOperator::setHost(const QString &arg0)
{
  AQ_CALL_VOID(setHost(arg0));
}
inline bool AQSUrlOperator::hasHost() const
{
  AQ_CALL_RET_V(hasHost(), bool);
}
inline int AQSUrlOperator::port() const
{
  AQ_CALL_RET_V(port(), int);
}
inline void AQSUrlOperator::setPort(int arg0)
{
  AQ_CALL_VOID(setPort(arg0));
}
inline bool AQSUrlOperator::hasPort() const
{
  AQ_CALL_RET_V(hasPort(), bool);
}
inline QString AQSUrlOperator::path(bool arg0) const
{
  AQ_CALL_RET_V(path(arg0), QString);
}
inline void AQSUrlOperator::setPath(const QString &arg0)
{
  AQ_CALL_VOID(setPath(arg0));
}
inline bool AQSUrlOperator::hasPath() const
{
  AQ_CALL_RET_V(hasPath(), bool);
}
inline void AQSUrlOperator::setEncodedPathAndQuery(const QString &arg0)
{
  AQ_CALL_VOID(setEncodedPathAndQuery(arg0));
}
inline QString AQSUrlOperator::encodedPathAndQuery()
{
  AQ_CALL_RET_V(encodedPathAndQuery(), QString);
}
inline void AQSUrlOperator::setQuery(const QString &arg0)
{
  AQ_CALL_VOID(setQuery(arg0));
}
inline QString AQSUrlOperator::query() const
{
  AQ_CALL_RET_V(query(), QString);
}
inline QString AQSUrlOperator::ref() const
{
  AQ_CALL_RET_V(ref(), QString);
}
inline void AQSUrlOperator::setRef(const QString &arg0)
{
  AQ_CALL_VOID(setRef(arg0));
}
inline bool AQSUrlOperator::hasRef() const
{
  AQ_CALL_RET_V(hasRef(), bool);
}
inline bool AQSUrlOperator::isValid() const
{
  AQ_CALL_RET_V(isValid(), bool);
}
inline bool AQSUrlOperator::isLocalFile() const
{
  AQ_CALL_RET_V(isLocalFile(), bool);
}
inline void AQSUrlOperator::addPath(const QString &arg0)
{
  AQ_CALL_VOID(addPath(arg0));
}
inline void AQSUrlOperator::setFileName(const QString &arg0)
{
  AQ_CALL_VOID(setFileName(arg0));
}
inline QString AQSUrlOperator::fileName() const
{
  AQ_CALL_RET_V(fileName(), QString);
}
inline QString AQSUrlOperator::dirPath() const
{
  AQ_CALL_RET_V(dirPath(), QString);
}
inline QString AQSUrlOperator::toString(bool arg0,  bool arg1) const
{
  AQ_CALL_RET_V(toString(arg0, arg1), QString);
}
inline bool AQSUrlOperator::cdUp()
{
  AQ_CALL_RET_V(cdUp(), bool);
}
inline void AQSUrlOperator::decode(QString &arg0)
{
  AQ_CALL_VOID(decode(arg0));
}
inline void AQSUrlOperator::encode(QString &arg0)
{
  AQ_CALL_VOID(encode(arg0));
}
inline bool AQSUrlOperator::isRelativeUrl(const QString &arg0)
{
  AQ_CALL_RET_V(isRelativeUrl(arg0), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSURLOPERATOR_P_H_ */
// @AQSOBJECT@
