/*
    Copyright (C) 2001-2002 by theKompany.com <www.thekompany.com>
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    By PLUGINS we mean the commercial plug-ins developed by theKompany.com
    for Aethera.

    In addition, as a special exception, theKompany.com gives permission
    to link the code of this program with PLUGINS (or with
    modified versions of PLUGINS that use the same license as PLUGINS),
    and distribute linked combinations including the two.  You must obey
    the GNU General Public License in all respects for all of the code used
    other than PLUGINS.  If you modify this file, you may extend this
    exception to your version of the file, but you are not obligated to do so.
    If you do not wish to do so, delete this exception statement from your
    version.

    This license grants you the ability to use PLUGINS with Aethera only
    and may not be used outside of Aethera.
    See also http://www.thekompany.com/products/license.txt for details.
*/

#include "pop3client.h"

// SSL support
#include <sslsocket.h>
#include <ssldevice.h>

#include <ctime>
#include <pop3uidjar.h>
#include <mailinfo.h>

#include <accounts.h>

#include <qregexp.h>
#include <qapplication.h>
#include <qmessagebox.h>

using namespace tkcSSL;

// #define POP3_DEBUG

// ----------- POP3Private -------------------
class POP3Private
{
public:
  POP3Private(POP3Client *parent, Account *account);
  ~POP3Private();

  Account         *account_;

  SSLSocket       *socket_;

  POP3_UIDJar     *uidJar_;

  //ServerNotifier * notifier_;

  POP3Client::State state_;

  QTimer         idleTimer_;

  UIDMailList    uidlIndex_;
  RetrMailList   retrIndex_;
  ListMailList   listIndex_;

  QCString       retrBuffer_;
  QCString       retrBufferData_;
  unsigned long  retrSize_;
  unsigned long  retrBufferSize_;

  //AppStatus * status;
  int iMessages;

  bool endOfMessage_;
  bool singleLine_;
  bool starttls_;

  // workaround for a bug
  bool bStop;
  bool downloadFinished;

private:

  POP3Client *parent_;
};

POP3Private::POP3Private(POP3Client *parent, Account *account)
  : state_(POP3Client::Idle),
    //status          (0L),
    iMessages(0),
    endOfMessage_(false),
    singleLine_(true),
    starttls_(false),
    bStop(false),
    downloadFinished(false),
    parent_(parent)
{
  account_ = new Account(*account);
  socket_ = new SSLSocket(parent_, (account_->accname + "_Socket").latin1());

  if (account_->ssl) {
    SSLDevice *sslDevice = 0;
    if (account_->tls) {
      qDebug("******** TLS connection **********");
      starttls_ = true;
      sslDevice = new SSLDevice(QSocketDevice::Stream, SSLDevice::TLS);
    } else {
      qDebug("******** SSL connection **********");
      sslDevice = new SSLDevice(QSocketDevice::Stream, SSLDevice::SSL23);
    }

    if (sslDevice) {
      if (account_->useCertificates) {
#if 0
        QObject::connect(
          sslDevice->qtHandler(),
          SIGNAL(doYouAcceptCertificate(SSLDevice *, const QString &)),
          CertificatesManager::instance(),
          SLOT(slotAcceptCertificateDialog(SSLDevice *, const QString &)));
        QObject::connect(
          sslDevice->qtHandler(),
          SIGNAL(certificateVerifyFailed(const QString &)),
          CertificatesManager::instance(),
          SLOT(slotCertificateFailed(const QString &)));

        sslDevice->setVerifyType(SSLDevice::ASKING);
        sslDevice->setCertificatesFolder
        (CertificatesManager::instance()->certificatesFolder());
        sslDevice->setCertificatesFile
        (CertificatesManager::instance()->certificateFile(account_->accname, "POP3"));
#endif
      }

      socket_->setSocketDevice(sslDevice, true);
      if (!sslDevice->initSSL()) {
        qDebug("POP3Client : Error initSSL()");
      }
    } else {
      qDebug("POP3Client : cannot create a new SSL device.");
    }
  }

  //  socket_->socketDevice()->setReadBufferSize(174760);
  //  socket_->socketDevice()->setSendBufferSize(87380);

  uidJar_   =   POP3_UIDJar::ref();
  //notifier_ =   ServerNotifier::thisInstance();

  //status    =   AppStatus::instance();
}

POP3Private::~POP3Private()
{
  retrIndex_.clear();
  listIndex_.clear();
  uidlIndex_.setAutoDelete(true);
  uidlIndex_.clear();

  delete account_;
  account_ = 0;
}
// ----------- POP3Private -------------------


// ----------- POP3Client -------------------
POP3Client::POP3Client(Account *account, QObject *parent, const char *name)
  : QObject(parent, name)
{
  d = new POP3Private(this, account);

  connect
  (
    d->socket_,
    SIGNAL(hostFound()),
    this,
    SLOT(slotHostFound())
  );

  connect
  (
    d->socket_,
    SIGNAL(connected()),
    this,
    SLOT(slotConnected())
  );

  connect
  (
    d->socket_,
    SIGNAL(connectionClosed()),
    this,
    SLOT(slotConnectionClosed())
  );

  connect
  (
    d->socket_,
    SIGNAL(readyRead()),
    this,
    SLOT(slotReadyRead())
  );

  connect
  (
    d->socket_,
    SIGNAL(bytesWritten(int)),
    this,
    SLOT(slotBytesWritten(int))
  );

  connect
  (
    d->socket_,
    SIGNAL(error(int)),
    this,
    SLOT(slotError(int))
  );

  connect
  (
    &d->idleTimer_,
    SIGNAL(timeout()),
    this,
    SLOT(slotIdle())
  );
}

POP3Client::~POP3Client()
{
  delete d;
  d = 0;
}

void
POP3Client::getMail()
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::getMail(%s)", d->account_->accname.latin1());
#endif

  if (Idle != d->state_) {
    qDebug("POP3Client::getMail: Already getting mail");
    return;
  }

  if (d->account_->getdisabled) {
    qDebug("POP3Client::getMail: Account disabled");
    return;
  }

  //d->notifier_->ioProgress(QString("pop3:task:") + d->account_->accname);

  qDebug("POP3Client::getMail: Connecting to %s:%d",
         d->account_->server.latin1(),
         d->account_->port);

  d->socket_->connectToHost(d->account_->server, d->account_->port);

  setState(WaitingForConnection);

  resetIdleTimer();
}

void
POP3Client::slotHostFound()
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::slotHostFound");
#endif

  //d->status->message(AppStatus::Server, AppStatus::Text, AppStatus::Waiting, tr("Host found..."));

  resetIdleTimer();
}

void
POP3Client::slotConnected()
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::slotConnected");
#endif

  //d->status->message(AppStatus::Server, AppStatus::Text, AppStatus::Waiting, tr("Connected..."));

  resetIdleTimer();
}

void
POP3Client::slotConnectionClosed()
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::slotConnectionClosed");

  if (Idle != d->state_) {
    qDebug("Wasn't idle !");
  }
#endif

  if (!d->downloadFinished) {
    //d->status->message(AppStatus::Server, AppStatus::Text, AppStatus::Warning, tr("Connection closed..."));
  }

  stop();
}

void
POP3Client::slotReadyRead()
{
  bool bRet = true;

#ifdef POP3_DEBUG
  qDebug("POP3Client::slotReadyRead");
#endif

  resetIdleTimer();

  if (isSingleLine()) {
    QString line;

    while (d->socket_->canReadLine()) {
      line = d->socket_->readLine();

#ifdef POP3_DEBUG
      // QString replaces in-place. Must copy first.
      QString stripped = line;
      stripped.replace(QRegExp("[\r\n]*$"), "");

      qDebug("Read: `%s'", stripped.latin1());
#endif

      bRet = processLine(line);
      line = QString::null;

      if (!bRet || !isSingleLine())
        break;
    }
  }

  if (bRet && !isSingleLine()) {
    QByteArray block;

    long size = 0;
    while ((size = d->socket_->bytesAvailable())) {
      block.resize(size);
      size = d->socket_->readBlock(block.data(), block.size());

      if (-1 == size)
        bRet = false;
      else
        bRet = processMultiLine(block);

      block.resize(0);
      size = 0;

      if (!bRet)
        break;
    }
  }
}

bool
POP3Client::processLine(const QString &line)
{
#ifdef POP3_DEBUG
  qDebug("processLine %s", stateToString(d->state_).latin1());
#endif

  switch (d->state_) {
    case WaitingForConnection:
      if (!handleConnection(line))
        return false;
      break;

    case WaitingForSTARTTLS:
      if (!handleSTARTTLS(line))
        return false;
      break;

    case WaitingForAPOPResponse:
      if (!handleAPOPResponse(line))
        return false;
      break;

    case WaitingForUserResponse:
      if (!handleUserResponse(line))
        return false;
      break;

    case WaitingForPassResponse:
      if (!handlePassResponse(line))
        return false;
      break;

    case WaitingForListResponse:
      if (!handleListResponse(line))
        return false;
      break;

    case WaitingForListItems:
      if (!handleListItem(line))
        return false;
      break;

    case WaitingForUidlResponse:
      if (!handleUidlResponse(line))
        return false;
      break;

    case WaitingForUidlItems:
      if (!handleUidlItem(line))
        return false;
      break;

    case WaitingForRetrResponse:
      if (!handleRetrResponse(line))
        return false;
      break;

    case WaitingForRetrData:
      if (!handleRetrData(line))
        return false;
      break;

    case WaitingForDeleResponse:
      if (!handleDeleResponse(line))
        return false;
      break;

    default:
      qDebug("Not sure what to do with this line: `%s'", line.latin1());
      break;
  }

  return true;
}

bool
POP3Client::processMultiLine(const QByteArray &block)
{
  switch (d->state_) {
    case WaitingForRetrData:
      if (!handleRetrData(block))
        return false;
      break;
    default:
      qDebug("processMultiLine: cannot use the data in %s state",
             stateToString(d->state_).latin1());
      break;
  }

  return true;
}

#ifdef POP3_DEBUG
void
POP3Client::slotBytesWritten(int byteCount)
#else
void
POP3Client::slotBytesWritten(int)
#endif
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::slotBytesWritten(%d)", byteCount);
#endif

  resetIdleTimer();
}

void
POP3Client::slotError(int err)
{
  qDebug("POP3Client::slotError(%d)", err);

  stop();

  switch (err) {
    case QSocket::ErrConnectionRefused:
      qDebug("Connection refused");
      //d->status->message(AppStatus::Server, AppStatus::Text, AppStatus::Error, tr("Connection refused"));
      break;

    case QSocket::ErrHostNotFound:
      qDebug("Host not found");
      //d->status->message(AppStatus::Server, AppStatus::Text, AppStatus::Error, tr("Host not found"));
      break;

    case QSocket::ErrSocketRead:
      qDebug("Read error");
      //d->status->message(AppStatus::Server, AppStatus::Text, AppStatus::Error, tr("Read error"));
      break;

    default:
      qDebug("Protocol error");
      //d->status->message(AppStatus::Server, AppStatus::Text, AppStatus::Error, tr("Protocol error"));
      break;
  }
}

void
POP3Client::slotIdle()
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::slotIdle");
#endif

  if (Idle == d->state_) {
#ifdef POP3_DEBUG
    qDebug("Was already idle");
#endif
    return;
  }

  qDebug("Cancelling current pop"); // XXX
  stop();
}

void
POP3Client::stop()
{
  if (!d->bStop) {
    qDebug("POP3Client::stop");

    d->bStop = true;

    d->uidJar_->saveLists();

    d->retrIndex_.clear();
    d->listIndex_.clear();
    d->uidlIndex_.setAutoDelete(true);
    d->uidlIndex_.clear();

    d->socket_->close();

    d->idleTimer_.stop();
    setState(Idle);

    //IOSyncHandler::ref()->pop3Sync();

    // it's better to be made in events loop
    // see SSLDevice
    QTimer::singleShot(100, this, SLOT(slotEmitFinished()));
  }
}

void
POP3Client::slotEmitFinished()
{
  emit(finished(this));
}

void
POP3Client::sendAPOP(const QString & /* serverAPOPInfo */)
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::sendAPOP");
#endif

  QString digest; // FIXME
  writeLine("APOP " + digest);
  setState(WaitingForAPOPResponse);
}

void
POP3Client::sendSTARTTLS()
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::sendSTARTTLS");
#endif

  writeLine("STLS");
  setState(WaitingForSTARTTLS);
}

void
POP3Client::sendUser()
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::sendUser");
#endif

  writeLine("USER " + d->account_->user);
  setState(WaitingForUserResponse);
}

void
POP3Client::sendPass()
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::sendPass");
#endif

  writeLine("PASS " + d->account_->password);
  setState(WaitingForPassResponse);
}

void
POP3Client::sendList()
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::sendList");
#endif

  writeLine("LIST");
  setState(WaitingForListResponse);
}

void
POP3Client::sendUidl()
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::sendUidl");
#endif

  writeLine("UIDL");
  setState(WaitingForUidlResponse);
}

void
POP3Client::sendRetr(uint index)
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::sendRetr");
#endif

  writeLine("RETR " + QString::number(index));
  setState(WaitingForRetrResponse);
}

void
POP3Client::sendDele(uint index)
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::sendDele");
#endif

  writeLine("DELE " + QString::number(index));
  setState(WaitingForDeleResponse);
}

void
POP3Client::sendQuit()
{
#ifdef POP3_DEBUG
  qDebug("POP3Client::sendQuit");
#endif

  // FIXME: deleting messages ?

  writeLine("QUIT");
  //  stop();
}

bool
POP3Client::replyOk(const QString &reply) const
{
  bool ok = "+OK" == reply.left(3);

#ifdef POP3_DEBUG
  if (ok) {
    qDebug("Server said OK");
  } else {
    qDebug("Server said error");
  }
#endif

  return ok;
}

Account *
POP3Client::account()
{
  return d->account_;
}

void
POP3Client::resetIdleTimer()
{
  d->idleTimer_.start(60000 * 10 /* 10 minutes */, true);
}

void
POP3Client::writeLine(const QString &s)
{
  if (!d->socket_)
    return;

#ifdef POP3_DEBUG
  qDebug("Sending: `%s'", s.latin1());
#endif

  QTextStream str(d->socket_);

  str << s << "\r\n";
}

void
POP3Client::setState(POP3Client::State state)
{
#ifdef POP3_DEBUG
  qDebug
  (
    "POP3Client: State change: %s -> %s",
    stateToString(d->state_).latin1(),
    stateToString(state).latin1()
  );
#endif

  d->state_ = state;
}

QString
POP3Client::stateToString(POP3Client::State state) const
{
  QString s = "Unknown";

  switch (state) {
    case Idle:
      s = "Idle";
      break;

    case WaitingForConnection:
      s = "WaitingForConnection";
      break;

    case WaitingForSTARTTLS:
      s = "WaitingForSTARTTLS";
      break;

    case WaitingForAPOPResponse:
      s = "WaitingForAPOPResponse";
      break;

    case WaitingForUserResponse:
      s = "WaitingForUserResponse";
      break;

    case WaitingForPassResponse:
      s = "WaitingForPassResponse";
      break;

    case WaitingForListResponse:
      s = "WaitingForListResponse";
      break;

    case WaitingForListItems:
      s = "WaitingForListItems";
      break;

    case WaitingForUidlResponse:
      s = "WaitingForUidlResponse";
      break;

    case WaitingForUidlItems:
      s = "WaitingForUidlItems";
      break;

    case WaitingForRetrResponse:
      s = "WaitingForRetrResponse";
      break;

    case WaitingForRetrData:
      s = "WaitingForRetrData";
      break;

    case WaitingForDeleResponse:
      s = "WaitingForDeleResponse";
      break;

    default:
      break;
  }

  return s;
}

bool POP3Client::isSingleLine()
{
  return d->singleLine_;
}

void POP3Client::setSingleLine(bool singleLine)
{
  d->singleLine_ = singleLine;
}


bool
POP3Client::handleConnection(const QString &line)
{
  if (!replyOk(line)) {
#ifdef POP3_DEBUG
    qDebug("WaitingForConnection: %s", line.latin1());
#endif

    stop();
    return false;
  } else {
    QRegExp apopInfoRegExp("<\\S@\\S@>");

    // See if we got 1 (and only 1 - more would be quite dodgy) match.

    if (1 == apopInfoRegExp.search(line))
      qDebug("POP3Client::sendAPOP: TODO");

    //  sendAPOP(apopInfoRegExp.cap(0));

    if (d->starttls_) {
      sendSTARTTLS();
    } else {
      sendUser();
    }
  }

  return true;
}

bool
POP3Client::handleSTARTTLS(const QString &line)
{
  if (replyOk(line)) {
    //d->status->message(AppStatus::Server, AppStatus::Text, AppStatus::Waiting,
    //  tr("Starting TLS negotiation..."));

    qDebug("Starting TLS connection...");
    if (d->socket_->startTLS()) {
      // this flag should be set only here
      d->starttls_ = false;

      sendUser();

      return true;
    }
  }

#ifdef POP3_DEBUG
  qDebug("WaitingForSTARTTLS: cannot start TLS : %s", line.latin1());
#endif
  stop();
  return false;
}

bool
POP3Client::handleAPOPResponse(const QString &line)
{
  if (!replyOk(line)) {
#ifdef POP3_DEBUG
    qDebug("WaitingForAPOPResponse: %s", line.latin1());
#endif

    // If the server didn't like our APOP response, try user/pass.
    // We'll close the connection and re-connect, as some servers
    // get upset if you try USER after a failed APOP.

    // Note that qmail-pop3d seems to pretend to be APOP-compatible,
    // but isn't.

    d->socket_->close();
    d->socket_->connectToHost(d->account_->server, d->account_->port);
    setState(WaitingForConnection);
  }

  sendUidl();

  return true;
}

bool
POP3Client::handleUserResponse(const QString &line)
{
  if (!replyOk(line)) {
#ifdef POP3_DEBUG
    qDebug("WaitingForUserResponse: %s", line.latin1());
#endif

    stop();
    return false;
  }

  sendPass();

  return true;
}

bool
POP3Client::handlePassResponse(const QString &line)
{
  if (!replyOk(line)) {
#ifdef POP3_DEBUG
    qDebug("WaitingForPassResponse: %s", line.latin1());
#endif

    //d->status->message(AppStatus::Server, AppStatus::Text, AppStatus::Error, tr("Authentication failed"));

    QMessageBox::critical(0, d->account_->accname + " - POP3",
                          tr("Authentication failed. \n Please check you user and password."));

    stop();
    return false;
  }

  //d->status->message(AppStatus::Server, AppStatus::Text, AppStatus::Waiting, tr("Authenticated..."));

  sendUidl();

  return true;
}

bool
POP3Client::handleListResponse(const QString &line)
{
  if (!replyOk(line)) {
#ifdef POP3_DEBUG
    qDebug("WaitingForListResponse: %s", line.latin1());
#endif

    stop();
    return false;
  }

  setState(WaitingForListItems);

  return true;
}


bool
POP3Client::handleListItem(const QString &line)
{
  if ('.' == line[0]) {
    retrieveDeleteOrQuit();
  } else {
    QStringList tokens = QStringList::split(' ', line);

    if (2 != tokens.count()) {
      qDebug("Bogus LIST line from server: `%s'", line.latin1());
      qDebug("POP operation cancelled");
      stop();
      return false;
    }

    MailInfo *mailInfo = d->listIndex_[tokens[0].stripWhiteSpace().toULong()];
    if (mailInfo) {
      mailInfo->size = tokens[1].stripWhiteSpace().toULong();
#ifdef POP3_DEBUG
      qDebug("Set size for %u with %u", mailInfo->retr, mailInfo->size);
#endif
    }
  }

  return true;
}


bool
POP3Client::handleUidlResponse(const QString &line)
{
  if (!replyOk(line)) {
#ifdef POP3_DEBUG
    qDebug("WaitingForUidlResponse: %s", line.latin1());
#endif

    stop();
    return false;
  }

  setState(WaitingForUidlItems);

  return true;
}

bool
POP3Client::handleUidlItem(const QString &line)
{
  if ('.' == line[0]) {
#ifdef POP3_DEBUG
    qDebug("%u messages to download", d->retrIndex_.count());
#endif

    d->iMessages = d->retrIndex_.count();
    //d->status->message(AppStatus::Server, AppStatus::Progress, AppStatus::Waiting,
    //  tr("Downloading..."), 0, d->iMessages);

    qApp->processEvents(1);

    // get the message size
    sendList();
  } else {
    QStringList tokens = QStringList::split(' ', line);

    if (2 != tokens.count()) {
      qDebug("Bogus UIDL line from server: `%s'", line.latin1());
      qDebug("POP operation cancelled");
      stop();
      return false;
    }

    ulong   index = tokens[0].stripWhiteSpace().toULong();
    QString uid   = tokens[1].stripWhiteSpace();

    MailInfo *mailInfo = new MailInfo(uid, index);
    d->uidlIndex_.insert(uid, mailInfo);
    d->listIndex_.insert(index, mailInfo);
    if (d->uidJar_->rcvList.find(uid.latin1()) == d->uidJar_->rcvList.end()) {
      d->retrIndex_.append(mailInfo);
    } else if (!d->account_->leave) {
#ifdef POP3_DEBUG
      qDebug("Already seen message: `%s'", uid.latin1());
#endif
      d->uidJar_->markForDeletion(uid.latin1());
    }
  }

  return true;
}


bool
POP3Client::handleRetrResponse(const QString &line)
{
  if (!replyOk(line)) {
#ifdef POP3_DEBUG
    qDebug("WaitingForRetrResponse: %s", line.latin1());
#endif

    stop();
    return false;
  }

  setState(WaitingForRetrData);

  MailInfo *mailInfo = d->retrIndex_.getFirst();
  if (mailInfo) {
    if (mailInfo->size) {
#ifdef POP3_DEBUG
      qDebug("Try reading %u bytes", mailInfo->size);
#endif

      d->retrSize_ = mailInfo->size;
      d->retrBufferSize_ = 0;
      d->retrBuffer_.resize(mailInfo->size + 10);
      d->retrBuffer_[0] = 0;

      // use block reading for safe
      setSingleLine(false);
    } else {
      // use line reading
      setSingleLine(true);
    }
  } else {
    stop();
    return false;
  }

  return true;
}


unsigned POP3Client::toMail(const QByteArray &bufferData)
{
  char lastChar = d->retrBuffer_.length() ? d->retrBuffer_[(int)d->retrBuffer_.length() - 1 ]
                  : 0;
  char currentChar = 0;
  unsigned uPrepend = 0;

  QCString theEnd("\r\n.\r\n");
  QByteArray data;
  QCString output;

  if (d->retrBufferData_.isEmpty() ||
      !d->retrBufferData_.size() ||
      !d->retrBufferData_.length()) {
    data.resize(bufferData.size());
    memcpy(data.data(), bufferData.data(), bufferData.size());
  } else {
    uPrepend = d->retrBufferData_.size() - 1;

    data.resize(uPrepend + bufferData.size());
    memcpy(data.data(), d->retrBufferData_.data(), uPrepend);
    memcpy(data.data() + uPrepend, bufferData.data(), bufferData.size());
  }

  unsigned dataLength = data.size();
  output.resize(data.size());

  d->retrBufferData_.resize(6);
  d->retrBufferData_.data()[0] = 0;

  unsigned j = 0;
  for (unsigned i = 0; i < dataLength; ++i) {
    currentChar = data.data()[i];

    switch (currentChar) {
      case '\0':
#ifdef POP3_DEBUG
        qDebug("toMail() - Null char detected!");
#endif
        break;
      case '\r':
        // read the next 5 chars (if possible)
        if (dataLength - i >= 5) {
          memcpy(d->retrBufferData_.data(), &data.data()[i], 5);
          if (theEnd == d->retrBufferData_) {
#ifdef POP3_DEBUG
            qDebug("End of message detected!");
#endif
            d->endOfMessage_ = true;

            output.data()[j] = '\n';
            output.data()[j + 1] = 0;

            d->retrBuffer_ += output;

            return (i + 5 - uPrepend);
          } else
            d->retrBufferData_.data()[0] = 0;
        } else {
          uPrepend = dataLength - i;

          d->retrBufferData_.resize(uPrepend + 1);
          memcpy(d->retrBufferData_.data(), data.data() + i, uPrepend);
          d->retrBufferData_.data()[uPrepend] = 0;

          output.data()[j] = 0;
          d->retrBuffer_ += output;

          return bufferData.size();
        }
        break;
      case '.':
        if ('\n' == lastChar) {
          lastChar = currentChar;
          break;
        }
      default :
        lastChar = currentChar;
        output.data()[j] = currentChar;
        j++;
        break;
    }
  }

  output.data()[j] = 0;
  d->retrBuffer_ += output;

  return bufferData.size();
}

bool
POP3Client::handleRetrData(const QByteArray &block)
{

#ifdef POP3_DEBUG
  qDebug("handleRetrData(const QByteArray & block)");
#endif

  unsigned uSize = block.size();

  // count the received bytes
  d->retrBufferSize_ += uSize;

#ifdef POP3_DEBUG
  qDebug("Received bytes = %u\n", d->retrBufferSize_);
#endif

  toMail(block);

  // end of message is detected
  if (d->endOfMessage_) {
    d->endOfMessage_ = false;

    // back to single line
    setSingleLine(true);

    MailInfo *mailInfo = d->retrIndex_.getFirst();
    if (mailInfo && !mailInfo->uid.isEmpty()) {
#ifdef POP3_DEBUG
      qDebug("IOSyncHandler::ref()->dispatchToInbox");
#endif

      //IOSyncHandler::ref()->dispatchToInbox
      //  (
      //    d->retrBuffer_,
      //    mailInfo->uid.latin1(),
      //    d->account_->accname
      //  );

      //d->status->message(AppStatus::Server, AppStatus::Progress, AppStatus::Waiting,
      //  tr("Downloading..."), d->iMessages - d->retrIndex_.count());

      if (!d->account_->leave) {
#ifdef POP3_DEBUG
        qDebug("Scheduled to be deleted !");
#endif

        d->uidJar_->markForDeletion(mailInfo->uid.latin1());
      } else {
        d->uidJar_->rcvInsert(mailInfo->uid.latin1(), ::time(0));
      }

      d->retrIndex_.removeFirst();
      d->retrBufferData_.resize(0);
      d->retrBufferSize_ = 0;
      d->retrBuffer_.resize(0);

      retrieveDeleteOrQuit();
    }
  }

  return true;
}

bool
POP3Client::handleRetrData(const QString &line)
{
  // QString replaces in-place. Must copy first.
  QString stripped = line;

  if ('.' == line[0] &&
      "." == stripped.replace(QRegExp("[\r\n]*$"), "")) {
#ifdef POP3_DEBUG
    qDebug("Have message data - calling dispatchToInbox");
#endif

    MailInfo *mailInfo = d->retrIndex_.getFirst();
    if (mailInfo && !mailInfo->uid.isEmpty()) {
      //IOSyncHandler::ref()->dispatchToInbox
      //  (
      //    d->retrBuffer_,
      //    mailInfo->uid.latin1(),
      //    d->account_->accname
      //  );

      if (!d->account_->leave) {
        qDebug("Scheduled to be deleted !");

        d->uidJar_->markForDeletion(mailInfo->uid.latin1());
      } else {
        d->uidJar_->rcvInsert(mailInfo->uid.latin1(), ::time(0));
      }
    } else {
      qDebug("handleRetrData: That's bad.");
      return false;
    }

    d->retrIndex_.removeFirst();
    d->retrBuffer_.resize(0);


    retrieveDeleteOrQuit();
  } else {
    // Unescape.
    if (line[0] == '.') {
      d->retrBuffer_ += line.mid(1).utf8();
    } else {
      d->retrBuffer_ += line.utf8();
    }
  }

  return true;
}

bool
POP3Client::handleDeleResponse(const QString &line)
{
  if (!replyOk(line)) {
#ifdef POP3_DEBUG
    qDebug("WaitingForDeleResponse: %s", line.latin1());
#endif

    stop();
    return false;
  } else {
    retrieveDeleteOrQuit();
  }

  return true;
}

void
POP3Client::retrieveDeleteOrQuit()
{
  bool bEnd = true;

  // get back to single line
  setSingleLine(true);

  if (!d->retrIndex_.isEmpty()) { // More to retrieve ?
#ifdef POP3_DEBUG
    qDebug("POP3Client::retrieveDeleteOrQuit: More to retrieve");
#endif

    MailInfo *mailInfo = d->retrIndex_.getFirst();
    if (mailInfo) {
      sendRetr(mailInfo->retr);
      bEnd = false;
    }
  } else if (d->uidJar_->trashList.size()) {
    if (!d->account_->leave) {
#ifdef POP3_DEBUG
      qDebug("POP3Client::retrieveDeleteOrQuit: More to delete");
#endif

      MailInfo *mailInfo = 0;
      for (UIDMap::iterator it = d->uidJar_->trashList.begin(); it != d->uidJar_->trashList.end(); ++it) {
        mailInfo = d->uidlIndex_[(*it).first.c_str() ];
        if (mailInfo) {
          sendDele(mailInfo->retr);
          d->uidJar_->trashList.erase(it);
          bEnd = false;
          break;
        } else {
          d->uidJar_->trashList.erase(it);
        }
      }
    } else {
#ifdef POP3_DEBUG
      qDebug("retrieveDeleteOrQuit: More to delete but you have to leave the messages.");
#endif

      d->uidJar_->trashList.clear();
    }
  }

  // nothing to send, then quit
  if (bEnd) {
    qDebug("POP3Client::retrieveDeleteOrQuit: Quitting");

    //d->status->message(AppStatus::Server, AppStatus::Text, AppStatus::Good,
    //  tr("Downloading is finished"));
    d->downloadFinished = true;

    sendQuit();
  }
}

// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
