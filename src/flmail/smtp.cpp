/****************************************************************************
** $Id: smtp.cpp,v 1.1.1.9 2006/05/07 17:31:13 chehrlic Exp $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "smtp.h"

// SSL support
#include <sslsocket.h>
#include <ssldevice.h>

#include <mimecodec.h>

#include <qtextstream.h>
#include <qsocket.h>
#include <qdns.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qregexp.h>

using namespace tkcSSL;

class AQSmtpSocket : public SSLSocket
{
public:

  AQSmtpSocket(QObject *parent = 0, const char *name = 0)
    : SSLSocket(parent, name), t_(0) {}

  virtual ~AQSmtpSocket() {
    delete t_;
  }

  QTextStream *t_;
};

class SmtpPrivate
{
public:
  SmtpPrivate() {}

  QString message_;
  QString from_;
  QStringList rcpts_;
  QStringList::const_iterator rcpt_;
  AQSmtpSocket *socket_;
  QString response_;
  int responseCode_;
  int state_;
  QDns *mxLookup_;
  QString mailServer_;
  int port_;
  QString user_;
  QString password_;
  Smtp::ConnectionType connectionType_;
  Smtp::AuthMethod authMethod_;
  bool starttls_;
};

Smtp::Smtp(const QString &from, const QStringList &rcpts,
           const QString &m, const QString &mS, int port,
           QObject *parent, const char *name)
  : QObject(parent, name)
{
  d = new SmtpPrivate;

  d->mailServer_ = mS;
  d->port_ = port;
  d->message_ = m;
  d->from_ = from;
  d->rcpts_ = rcpts;
  d->socket_ = 0;
  d->mxLookup_ = 0;
  d->connectionType_ = Smtp::TcpConnection;
  d->authMethod_ = Smtp::NoAuth;
  d->starttls_ = false;
}

Smtp::~Smtp()
{
  if (d->mxLookup_)
    d->mxLookup_->deleteLater();
  delete d;
}

QString Smtp::mailServer() const
{
  return d->mailServer_;
}

int Smtp::port() const
{
  return d->port_;
}

void Smtp::init()
{
  if (d->mailServer_.isEmpty()) {
    QString rcpt(d->rcpts_.first());
    if (d->mxLookup_)
      d->mxLookup_->deleteLater();
    d->mxLookup_ = new QDns(rcpt.mid(rcpt.find('@') + 1), QDns::Mx);
    connect(d->mxLookup_, SIGNAL(resultsReady()),
            this, SLOT(dnsLookupHelper()));
  } else
    connectToMailServer(d->mailServer_, d->port_);
}

void Smtp::connectToMailServer(const QString &mS, int port)
{
  changeStatus(tr("Conectando a %1 %2 %3").arg(mS).arg(port).arg(d->connectionType_), Connecting);

  d->socket_ = new AQSmtpSocket(this);

  SSLDevice *sslDevice = 0;

  switch (d->connectionType_) {
    case TlsConnection:
      d->starttls_ = true;
      sslDevice = new SSLDevice(QSocketDevice::Stream, SSLDevice::TLS);
      break;
    case SslConnection:
      sslDevice = new SSLDevice(QSocketDevice::Stream, SSLDevice::SSL23);
      break;
  }

  if (sslDevice) {
    d->socket_->setSocketDevice(sslDevice, true);
    if (!sslDevice->initSSL())
      changeStatus(tr("Error initSSL()"), ClientError);
  }

  connect(d->socket_, SIGNAL(readyRead()),
          this, SLOT(readyRead()));
  connect(d->socket_, SIGNAL(connected()),
          this, SLOT(connected()));
  connect(d->socket_, SIGNAL(connectionClosed()),
          this, SLOT(connectionClosed()));
  connect(d->socket_, SIGNAL(error(int)),
          this, SLOT(socketError(int)));

  d->socket_->connectToHost(mS, port);
  d->socket_->t_ = new QTextStream(d->socket_);
  d->state_ = Init;
}

void Smtp::dnsLookupHelper()
{
  QValueList<QDns::MailServer> s = d->mxLookup_->mailServers();
  if (s.isEmpty()) {
    if (!d->mxLookup_->isWorking())
      changeStatus(tr("Error en el registro MX del DNS"), MxDnsError);
    return;
  }

  connectToMailServer(s.first().name);
}


void Smtp::connected()
{
  changeStatus(tr("Conectado a %1").arg(d->socket_->peerName()), Connected);
}

void Smtp::readyRead()
{
  if (sender() == d->socket_) {
    d->socket_ = ::qt_cast<AQSmtpSocket *>(sender());

    // SMTP is line-oriented
    if (!d->socket_ || !d->socket_->canReadLine())
      return;

    QString responseLine;
    do {
      responseLine = d->socket_->readLine();
      d->response_ += responseLine;

      d->responseCode_ = responseLine.left(3).toInt();

      if (d->responseCode_ / 100 == 4)
        changeStatus(d->response_, ServerError);

      if (d->responseCode_ / 100 == 5)
        changeStatus(d->response_, ClientError);

      //qWarning("responsecode %d %s", d->responseCode_, d->response_.latin1());

    } while (d->socket_->canReadLine() && responseLine[3] != ' ');
  } else if (!d->socket_)
    return;

  // ** Init
  if (d->state_ == Init && d->responseCode_ == 220) {
    changeStatus("SERVICE OK", d->state_);
    // banner was okay, let's go on
    // *t << "HELO there\r\n";
    // helo should be a full qualified hostname, we can use doaminpart of from
    QString helo = d->from_.mid(d->from_.find('@') + 1);
    *d->socket_->t_ << "HELO " << helo << "\r\n";
    changeStatus("HELO " + helo, d->state_);
    if (d->starttls_) {
      d->state_ = StartTTLS;
    } else if (d->authMethod_ == AuthPlain) {
      d->state_ = SendAuthPlain;
    } else if (d->authMethod_ == AuthLogin) {
      d->state_ = SendAuthLogin;
    } else {
      d->state_ = Mail;
      d->rcpt_ = d->rcpts_.begin();
    }
  }

  // ** SendAuthPlain
  else if (d->state_ == SendAuthPlain && d->responseCode_ == 250) {
    changeStatus("SEND AUTH PLAIN", d->state_);
    QByteArray ba;
    QTextStream ts(ba, IO_WriteOnly);
    ts << '\0' << d->user_ << '\0' << d->password_;
    QString msg(MIMECodec::encodeBase64(ba, false, false));
    *d->socket_->t_ << "AUTH PLAIN " << msg << "\r\n";
    d->state_ = WaitingForAuthPlain;
  }

  // ** WaitingForAuthPlain
  else if (d->state_ == WaitingForAuthPlain && d->responseCode_ == 235) {
    changeStatus("AUTH PLAIN OK", d->state_);
    d->responseCode_ = 250;
    d->state_ = Mail;
    d->rcpt_ = d->rcpts_.begin();
    QTimer::singleShot(0, this, SLOT(readyRead()));
  }

  // ** SendAuthLogin
  else if (d->state_ == SendAuthLogin && d->responseCode_ == 250) {
    changeStatus("SEND AUTH LOGIN", d->state_);
    *d->socket_->t_ << "AUTH LOGIN" << "\r\n";
    d->state_ = WaitingForAuthLogin;
  }

  // ** WaitingForAuthLogin
  else if (d->state_ == WaitingForAuthLogin && d->responseCode_ == 334) {
    changeStatus("AUTH LOGIN OK", d->state_);
    QByteArray ba;
    QTextStream ts(ba, IO_WriteOnly);
    ts << d->user_ ;
    QString msg(MIMECodec::encodeBase64(ba, false, false));
    *d->socket_->t_ << msg << "\r\n";
    d->state_ = WaitingForUser;
  }

  // ** WaitingForUser
  else if (d->state_ == WaitingForUser && d->responseCode_ == 334) {
    changeStatus("USER OK", d->state_);
    QByteArray ba;
    QTextStream ts(ba, IO_WriteOnly);
    ts << d->password_ ;
    QString msg(MIMECodec::encodeBase64(ba, false, false));
    *d->socket_->t_ << msg << "\r\n";
    d->state_ = WaitingForPass;
  }

  // ** WaitingForPass
  else if (d->state_ == WaitingForPass && d->responseCode_ == 235) {
    changeStatus("PASSWORD OK", d->state_);
    d->responseCode_ = 250;
    d->state_ = Mail;
    d->rcpt_ = d->rcpts_.begin();
    QTimer::singleShot(0, this, SLOT(readyRead()));
  }

  // ** StartTTLS
  else if (d->state_ == StartTTLS && d->responseCode_ == 250) {
    changeStatus("SEND STARTTLS", d->state_);
    *d->socket_->t_ << "STARTTLS " << "\r\n";
    d->state_ = WaitingForSTARTTLS;
  }

  // ** WaitingForSTARTTLS
  else if (d->state_ == WaitingForSTARTTLS && d->responseCode_ == 220) {
    changeStatus("STARTTLS OK", d->state_);
    if (d->socket_->startTLS()) {
      d->starttls_ = false;
      d->state_ = Init;
      QTimer::singleShot(0, this, SLOT(readyRead()));
    } else {
      changeStatus(tr("No puedo iniciar TLS:\n\n") +
                   d->response_, ClientError);
      d->starttls_ = false;
      d->state_ = Close;
    }
  }

  // ** Mail
  else if (d->state_ == Mail && d->responseCode_ == 250) {
    // HELO response was okay (well, it has to be)
    *d->socket_->t_ << "MAIL FROM: <" << d->from_ << ">\r\n";
    changeStatus("FROM " + d->from_, d->state_);
    d->state_ = Rcpt;
  }

  // ** Rcpt
  else if (d->state_ == Rcpt && d->responseCode_ == 250) {
    QString rcpt(*d->rcpt_);
    *d->socket_->t_ << "RCPT TO: <" << rcpt << ">\r\n";
    changeStatus("TO " + rcpt, d->state_);
    ++d->rcpt_;
    if (d->rcpt_ == d->rcpts_.end())
      d->state_ = Data;
    else
      d->state_ = Rcpt;
  }

  // ** Data
  else if (d->state_ == Data && d->responseCode_ == 250) {
    *d->socket_->t_ << "DATA\r\n";
    changeStatus("DATA", d->state_);
    d->state_ = Body;
  }

  // ** Body
  else if (d->state_ == Body && d->responseCode_ == 354) {
    *d->socket_->t_ << d->message_ << "\r\n.\r\n";
    changeStatus("BODY", d->state_);
    d->state_ = Quit;
  }

  // ** Quit
  else if (d->state_ == Quit && d->responseCode_ == 250) {
    *d->socket_->t_ << "QUIT\r\n";
    // here, we just close.
    changeStatus(tr("Envío completado"), SendOk);
    changeStatus(tr("Quit"), d->state_);
    d->state_ = Close;
  }

  // ** Close
  else if (d->state_ == Close) {
    return;
  }

  // ** Error
  else {
    // something broke.
    changeStatus(tr("Respuesta inesperada desde el servidor SMTP %1 :\n %2")
                 .arg(d->responseCode_).arg(d->response_), SmtpError);
    d->state_ = Close;
  }

  d->response_ = "";
}

void Smtp::connectionClosed()
{
  d->state_ = Close;
  changeStatus(tr("Conexión cerrada"), d->state_);
}

void Smtp::socketError(int e)
{
  QString msg;
  if (e == QSocket::ErrConnectionRefused)
    msg = "QSocket::ErrConnectionRefused";
  else if (e == QSocket::ErrHostNotFound)
    msg = "QSocket::ErrHostNotFound";
  else
    msg = "QSocket::ErrSocketRead";
  changeStatus(tr("Error número %1: %2").arg(e).arg(msg), SocketError);
}

void Smtp::changeStatus(const QString &statusMsg, int stateCode)
{
  emit statusChanged(statusMsg, stateCode);
}

void Smtp::setUser(const QString &user)
{
  d->user_ = user;
}

QString Smtp::user() const
{
  return d->user_;
}

void Smtp::setPassword(const QString &password)
{
  d->password_ = password;
}

QString Smtp::password() const
{
  return d->password_;
}

void Smtp::setConnectionType(int ct)
{
  d->connectionType_ = (Smtp::ConnectionType) ct;
}

int Smtp::connectionType() const
{
  return d->connectionType_;
}

void Smtp::setAuthMethod(int method)
{
  d->authMethod_ = (Smtp::AuthMethod) method;
}

int Smtp::authMethod() const
{
  return d->authMethod_;
}
