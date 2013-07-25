/****************************************************************************
** $Id: smtp.h,v 1.1.1.8 2006/05/07 17:31:13 chehrlic Exp $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef SMTP_H
#define SMTP_H

#include <qobject.h>
#include <qstringlist.h>

#include "aqmailglobal.h"

class SmtpPrivate;

class AQMAIL_EXPORT Smtp : public QObject
{
  Q_OBJECT

public:
  Smtp(const QString &from, const QStringList &rcpts,
       const QString &m, const QString &mS = QString::null,
       int port = 25, QObject *parent = 0, const char *name = 0);
  ~Smtp();

  QString mailServer() const;
  int port() const;

  void setUser(const QString &user);
  QString user() const;
  void setPassword(const QString &password);
  QString password() const;
  void setConnectionType(int ct);
  int connectionType() const;
  void setAuthMethod(int method);
  int authMethod() const;

signals:
  void statusChanged(const QString &, int);

public slots:
  void init();

private slots:
  void connectToMailServer(const QString &mS, int port = 25);
  void dnsLookupHelper();
  void readyRead();
  void connected();
  void changeStatus(const QString &statusMsg, int stateCode);
  void connectionClosed();
  void socketError(int e);

private:
  friend class SmtpPrivate;


  enum State {
    Init,
    Mail,
    Rcpt,
    Data,
    Body,
    Quit,
    Close,
    SmtpError,
    Connecting,
    Connected,
    MxDnsError,
    SendOk,
    SocketError,
    Composing,
    Attach,
    AttachError,
    ServerError,    // 4xx smtp error
    ClientError,    // 5xx smtp error
    StartTTLS,
    WaitingForSTARTTLS,
    SendAuthPlain,
    SendAuthLogin,
    WaitingForAuthPlain,
    WaitingForAuthLogin,
    WaitingForUser,
    WaitingForPass
  };

  enum AuthMethod {
    NoAuth,
    AuthPlain,
    AuthLogin
  };

  enum ConnectionType {
    TcpConnection,
    SslConnection,
    TlsConnection       // STARTTLS
  };

  SmtpPrivate *d;
};

#endif
