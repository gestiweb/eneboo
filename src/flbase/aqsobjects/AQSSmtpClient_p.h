/***************************************************************************
 AQSSmtpClient_p.h
 -------------------
 begin                : 28/10/2012
 copyright            : (C) 2003-2012 by InfoSiAL S.L.
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

#ifndef AQSSMTPCLIENT_P_H_
#define AQSSMTPCLIENT_P_H_

#include "AQSObject_p.h"
#include "AQObjects.h"

class AQSSmtpClient : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(SmtpClient, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setFrom(const QString &);
  QString from() const;
  void setReplyTo(const QString &);
  QString replyTo() const;
  void setTo(const QString &);
  QString to() const;
  void setCC(const QString &);
  QString cc() const;
  void setBCC(const QString &);
  QString bcc() const;
  void setOrganization(const QString &);
  QString organization() const;
  void setPriority(int);
  int priority() const;
  void setSubject(const QString &);
  QString subject() const;
  void setBody(const QString &);
  QString body() const;
  void addAttachment(const QString &, const QString& = QString::null);
  void addTextPart(const QString &);
  void addTextPart(const QString &, const QString &);
  void setMailServer(const QString &);
  QString mailServer() const;
  void setMimeType(const QString &);
  QString mimeType() const;
  void setPort(int);
  int port() const;
  QString lastStatusMsg() const;
  int lastStateCode() const;
  void setUser(const QString &);
  QString user() const;
  void setPassword(const QString &);
  QString password() const;
  void setConnectionType(int);
  int connectionType() const;
  void setAuthMethod(int);
  int authMethod() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQSmtpClient;
    if (sgt == QString::fromLatin1("QObject*"))
      return new AQSmtpClient(argValue<QObject *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSSmtpClient::setFrom(const QString &arg0)
{
  AQ_CALL_VOID(setFrom(arg0));
}
inline QString AQSSmtpClient::from() const
{
  AQ_CALL_RET_V(from(), QString);
}
inline void AQSSmtpClient::setReplyTo(const QString &arg0)
{
  AQ_CALL_VOID(setReplyTo(arg0));
}
inline QString AQSSmtpClient::replyTo() const
{
  AQ_CALL_RET_V(replyTo(), QString);
}
inline void AQSSmtpClient::setTo(const QString &arg0)
{
  AQ_CALL_VOID(setTo(arg0));
}
inline QString AQSSmtpClient::to() const
{
  AQ_CALL_RET_V(to(), QString);
}
inline void AQSSmtpClient::setCC(const QString &arg0)
{
  AQ_CALL_VOID(setCC(arg0));
}
inline QString AQSSmtpClient::cc() const
{
  AQ_CALL_RET_V(cc(), QString);
}
inline void AQSSmtpClient::setBCC(const QString &arg0)
{
  AQ_CALL_VOID(setBCC(arg0));
}
inline QString AQSSmtpClient::bcc() const
{
  AQ_CALL_RET_V(bcc(), QString);
}
inline void AQSSmtpClient::setOrganization(const QString &arg0)
{
  AQ_CALL_VOID(setOrganization(arg0));
}
inline QString AQSSmtpClient::organization() const
{
  AQ_CALL_RET_V(organization(), QString);
}
inline void AQSSmtpClient::setPriority(int arg0)
{
  AQ_CALL_VOID(setPriority(arg0));
}
inline int AQSSmtpClient::priority() const
{
  AQ_CALL_RET_V(priority(), int);
}
inline void AQSSmtpClient::setSubject(const QString &arg0)
{
  AQ_CALL_VOID(setSubject(arg0));
}
inline QString AQSSmtpClient::subject() const
{
  AQ_CALL_RET_V(subject(), QString);
}
inline void AQSSmtpClient::setBody(const QString &arg0)
{
  AQ_CALL_VOID(setBody(arg0));
}
inline QString AQSSmtpClient::body() const
{
  AQ_CALL_RET_V(body(), QString);
}
inline void AQSSmtpClient::addAttachment(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_VOID(addAttachment(arg0, arg1));
}
inline void AQSSmtpClient::addTextPart(const QString &arg0)
{
  AQ_CALL_VOID(addTextPart(arg0));
}
inline void AQSSmtpClient::addTextPart(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_VOID(addTextPart(arg0, arg1));
}
inline void AQSSmtpClient::setMailServer(const QString &arg0)
{
  AQ_CALL_VOID(setMailServer(arg0));
}
inline QString AQSSmtpClient::mailServer() const
{
  AQ_CALL_RET_V(mailServer(), QString);
}
inline void AQSSmtpClient::setMimeType(const QString &arg0)
{
  AQ_CALL_VOID(setMimeType(arg0));
}
inline QString AQSSmtpClient::mimeType() const
{
  AQ_CALL_RET_V(mimeType(), QString);
}
inline void AQSSmtpClient::setPort(int arg0)
{
  AQ_CALL_VOID(setPort(arg0));
}
inline int AQSSmtpClient::port() const
{
  AQ_CALL_RET_V(port(), int);
}
inline QString AQSSmtpClient::lastStatusMsg() const
{
  AQ_CALL_RET_V(lastStatusMsg(), QString);
}
inline int AQSSmtpClient::lastStateCode() const
{
  AQ_CALL_RET_V(lastStateCode(), int);
}
inline void AQSSmtpClient::setUser(const QString &arg0)
{
  AQ_CALL_VOID(setUser(arg0));
}
inline QString AQSSmtpClient::user() const
{
  AQ_CALL_RET_V(user(), QString);
}
inline void AQSSmtpClient::setPassword(const QString &arg0)
{
  AQ_CALL_VOID(setPassword(arg0));
}
inline QString AQSSmtpClient::password() const
{
  AQ_CALL_RET_V(password(), QString);
}
inline void AQSSmtpClient::setConnectionType(int arg0)
{
  AQ_CALL_VOID(setConnectionType(arg0));
}
inline int AQSSmtpClient::connectionType() const
{
  AQ_CALL_RET_V(connectionType(), int);
}
inline void AQSSmtpClient::setAuthMethod(int arg0)
{
  AQ_CALL_VOID(setAuthMethod(arg0));
}
inline int AQSSmtpClient::authMethod() const
{
  AQ_CALL_RET_V(authMethod(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSMTPCLIENT_P_H_ */
// @AQOBJECT@
