/***************************************************************************
 AQSNetworkOperation_p.h
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

#ifndef AQSNETWORKOPERATION_P_H_
#define AQSNETWORKOPERATION_P_H_

#include "AQSObject_p.h"
#include "AQSByteArray_p.h"
#include <qnetworkprotocol.h>

class AQSNetworkOperation : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(NetworkOperation, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setState(uint);
  void setProtocolDetail(const QString &);
  void setErrorCode(int);
  void setArg(int, const QString &);
  void setRawArg(int, QByteArray *);
  void setRawArg(int, AQSByteArray *);
  uint operation() const;
  uint state() const;
  QString arg(int) const;
  QByteArray *rawArg(int) const;
  QString protocolDetail() const;
  int errorCode() const;
  void free();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[4].append(QString::fromLatin1("QNetworkProtocol::Operation,QString,QString,QString"));
    candidates[4].append(QString::fromLatin1("QNetworkProtocol::Operation,QByteArray*,QByteArray*,QByteArray*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QNetworkProtocol::Operation,QString,QString,QString"))
      return new QNetworkOperation(static_cast<QNetworkProtocol::Operation>(args[0].variant().toUInt()),
                                   *(argValue<QString *>(args[1])),
                                   *(argValue<QString *>(args[2])),
                                   *(argValue<QString *>(args[3])));
    if (sgt == QString::fromLatin1("QNetworkProtocol::Operation,QByteArray*,QByteArray*,QByteArray*"))
      return new QNetworkOperation(static_cast<QNetworkProtocol::Operation>(args[0].variant().toUInt()),
                                   *argValue<QByteArray *>(args[1]),
                                   *argValue<QByteArray *>(args[2]),
                                   *argValue<QByteArray *>(args[3]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[4].append(QString::fromLatin1("QNetworkProtocol::Operation,QString,QString,QString"));
    candidates[4].append(QString::fromLatin1("QNetworkProtocol::Operation,QByteArray*,QByteArray*,QByteArray*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSNetworkOperation::setState(uint arg0)
{
  AQ_CALL_VOID(setState(static_cast<QNetworkProtocol::State>(arg0)));
}
inline void AQSNetworkOperation::setProtocolDetail(const QString &arg0)
{
  AQ_CALL_VOID(setProtocolDetail(arg0));
}
inline void AQSNetworkOperation::setErrorCode(int arg0)
{
  AQ_CALL_VOID(setErrorCode(arg0));
}
inline void AQSNetworkOperation::setArg(int arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setArg(arg0, arg1));
}
inline void AQSNetworkOperation::setRawArg(int arg0,  QByteArray *arg1)
{
  AQ_CALL_VOID(setRawArg(arg0, *arg1));
}
inline void AQSNetworkOperation::setRawArg(int arg0, AQSByteArray *arg1)
{
  AQ_CALL_VOID(setRawArg(arg0, *arg1));
}
inline uint AQSNetworkOperation::operation() const
{
  AQ_CALL_RET_V(operation(), uint);
}
inline uint AQSNetworkOperation::state() const
{
  AQ_CALL_RET_V(state(), uint);
}
inline QString AQSNetworkOperation::arg(int arg0) const
{
  AQ_CALL_RET_V(arg(arg0), QString);
}
inline QByteArray *AQSNetworkOperation::rawArg(int arg0) const
{
  AQ_CALL_RET_PTR(rawArg(arg0), QByteArray);
}
inline QString AQSNetworkOperation::protocolDetail() const
{
  AQ_CALL_RET_V(protocolDetail(), QString);
}
inline int AQSNetworkOperation::errorCode() const
{
  AQ_CALL_RET_V(errorCode(), int);
}
inline void AQSNetworkOperation::free()
{
  AQ_CALL_VOID(free());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSNETWORKOPERATION_P_H_ */
// @AQSOBJECT@
