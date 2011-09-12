/***************************************************************************
 AQSHttpRequestHeader_p.h
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

#ifndef AQSHTTPREQUESTHEADER_P_H_
#define AQSHTTPREQUESTHEADER_P_H_

#include "AQSHttpHeader_p.h"

// @AQ_PTR_INDIRECTION@

class AQSHttpRequestHeader : public AQSHttpHeader
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(HttpRequestHeader, HttpHeader);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setRequest(const QString &, const QString &, int = 1, int = 1);
  QString method() const;
  QString path() const;
  virtual int majorVersion() const;
  virtual int minorVersion() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,int"));
    candidates[4].append(QString::fromLatin1("QString,QString,int,int"));
    candidates[1].append(QString::fromLatin1("QHttpRequestHeader*"));
    candidates[1].append(QString::fromLatin1("QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QHttpRequestHeader;
    if (sgt == QString::fromLatin1("QString,QString"))
      return new QHttpRequestHeader(*(argValue<QString *>(args[0])),
                                    *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString,QString,int"))
      return new QHttpRequestHeader(*(argValue<QString *>(args[0])),
                                    *(argValue<QString *>(args[1])),
                                    args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QString,QString,int,int"))
      return new QHttpRequestHeader(*(argValue<QString *>(args[0])),
                                    *(argValue<QString *>(args[1])),
                                    args[2].variant().toInt(),
                                    args[3].variant().toInt());
    if (sgt == QString::fromLatin1("QHttpRequestHeader*"))
      return new QHttpRequestHeader(*argValue<QHttpRequestHeader *>(args[0]));
    if (sgt == QString::fromLatin1("QString"))
      return new QHttpRequestHeader(*(argValue<QString *>(args[0])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,int"));
    candidates[4].append(QString::fromLatin1("QString,QString,int,int"));
    candidates[1].append(QString::fromLatin1("QHttpRequestHeader*"));
    candidates[1].append(QString::fromLatin1("QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSHttpRequestHeader::setRequest(const QString &arg0,  const QString &arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(setRequest(arg0, arg1, arg2, arg3));
}
inline QString AQSHttpRequestHeader::method() const
{
  AQ_CALL_RET_V(method(), QString);
}
inline QString AQSHttpRequestHeader::path() const
{
  AQ_CALL_RET_V(path(), QString);
}
inline int AQSHttpRequestHeader::majorVersion() const
{
  AQ_CALL_RET_V(majorVersion(), int);
}
inline int AQSHttpRequestHeader::minorVersion() const
{
  AQ_CALL_RET_V(minorVersion(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSHTTPREQUESTHEADER_P_H_ */
// @AQSVOIDPTR@
