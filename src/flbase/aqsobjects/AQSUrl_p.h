/***************************************************************************
 AQSUrl_p.h
 -------------------
 begin                : 07/11/2011
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

#ifndef AQSURL_P_H_
#define AQSURL_P_H_

#include "AQSVoidPtr_p.h"
#include <qurl.h>

// @AQ_PTR_INDIRECTION@

class AQSUrl : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(Url, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
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

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QUrl*"));
    candidates[2].append(QString::fromLatin1("QUrl*,QString"));
    candidates[3].append(QString::fromLatin1("QUrl*,QString,bool"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QUrl;
    if (sgt == QString::fromLatin1("QString"))
      return new QUrl(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QUrl*"))
      return new QUrl(*argValue<QUrl *>(args[0]));
    if (sgt == QString::fromLatin1("QUrl*,QString"))
      return new QUrl(*argValue<QUrl *>(args[0]),
                      *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QUrl*,QString,bool"))
      return new QUrl(*argValue<QUrl *>(args[0]),
                      *(argValue<QString *>(args[1])),
                      args[2].variant().toBool());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QUrl*"));
    candidates[2].append(QString::fromLatin1("QUrl*,QString"));
    candidates[3].append(QString::fromLatin1("QUrl*,QString,bool"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSUrl::protocol() const
{
  AQ_CALL_RET_V(protocol(), QString);
}
inline void AQSUrl::setProtocol(const QString &arg0)
{
  AQ_CALL_VOID(setProtocol(arg0));
}
inline QString AQSUrl::user() const
{
  AQ_CALL_RET_V(user(), QString);
}
inline void AQSUrl::setUser(const QString &arg0)
{
  AQ_CALL_VOID(setUser(arg0));
}
inline bool AQSUrl::hasUser() const
{
  AQ_CALL_RET_V(hasUser(), bool);
}
inline QString AQSUrl::password() const
{
  AQ_CALL_RET_V(password(), QString);
}
inline void AQSUrl::setPassword(const QString &arg0)
{
  AQ_CALL_VOID(setPassword(arg0));
}
inline bool AQSUrl::hasPassword() const
{
  AQ_CALL_RET_V(hasPassword(), bool);
}
inline QString AQSUrl::host() const
{
  AQ_CALL_RET_V(host(), QString);
}
inline void AQSUrl::setHost(const QString &arg0)
{
  AQ_CALL_VOID(setHost(arg0));
}
inline bool AQSUrl::hasHost() const
{
  AQ_CALL_RET_V(hasHost(), bool);
}
inline int AQSUrl::port() const
{
  AQ_CALL_RET_V(port(), int);
}
inline void AQSUrl::setPort(int arg0)
{
  AQ_CALL_VOID(setPort(arg0));
}
inline bool AQSUrl::hasPort() const
{
  AQ_CALL_RET_V(hasPort(), bool);
}
inline QString AQSUrl::path(bool arg0) const
{
  AQ_CALL_RET_V(path(arg0), QString);
}
inline void AQSUrl::setPath(const QString &arg0)
{
  AQ_CALL_VOID(setPath(arg0));
}
inline bool AQSUrl::hasPath() const
{
  AQ_CALL_RET_V(hasPath(), bool);
}
inline void AQSUrl::setEncodedPathAndQuery(const QString &arg0)
{
  AQ_CALL_VOID(setEncodedPathAndQuery(arg0));
}
inline QString AQSUrl::encodedPathAndQuery()
{
  AQ_CALL_RET_V(encodedPathAndQuery(), QString);
}
inline void AQSUrl::setQuery(const QString &arg0)
{
  AQ_CALL_VOID(setQuery(arg0));
}
inline QString AQSUrl::query() const
{
  AQ_CALL_RET_V(query(), QString);
}
inline QString AQSUrl::ref() const
{
  AQ_CALL_RET_V(ref(), QString);
}
inline void AQSUrl::setRef(const QString &arg0)
{
  AQ_CALL_VOID(setRef(arg0));
}
inline bool AQSUrl::hasRef() const
{
  AQ_CALL_RET_V(hasRef(), bool);
}
inline bool AQSUrl::isValid() const
{
  AQ_CALL_RET_V(isValid(), bool);
}
inline bool AQSUrl::isLocalFile() const
{
  AQ_CALL_RET_V(isLocalFile(), bool);
}
inline void AQSUrl::addPath(const QString &arg0)
{
  AQ_CALL_VOID(addPath(arg0));
}
inline void AQSUrl::setFileName(const QString &arg0)
{
  AQ_CALL_VOID(setFileName(arg0));
}
inline QString AQSUrl::fileName() const
{
  AQ_CALL_RET_V(fileName(), QString);
}
inline QString AQSUrl::dirPath() const
{
  AQ_CALL_RET_V(dirPath(), QString);
}
inline QString AQSUrl::toString(bool arg0,  bool arg1) const
{
  AQ_CALL_RET_V(toString(arg0, arg1), QString);
}
inline bool AQSUrl::cdUp()
{
  AQ_CALL_RET_V(cdUp(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSURL_P_H_ */
// @AQSVOIDPTR@
