/***************************************************************************
                 ssldevice.cpp  -  QSocketDevice with SSL support
                             -------------------
    begin                : Tue Apr 23 2002
    copyright            : (C) 2002 by theKompany.com
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#include <cassert>

#include "ssldevice.h"

// #define SSL_DEBUG

#include <errno.h>

#include <qfile.h>
#include <qdir.h>
#include <qapplication.h>
#include <qeventloop.h>

using namespace tkcSSL;

namespace tkcSSL
{
  SSLQtHandler::~SSLQtHandler()
  {
  }

  void SSLQtHandler::emit_doYouAcceptCertificate(SSLDevice *sslDev, const QString &certInfo)
  {
    emit doYouAcceptCertificate(sslDev, certInfo);
  }

  void SSLQtHandler::emit_certificateVerifyFailed(const QString &certError)
  {
    emit certificateVerifyFailed(certError);
  }

  class SSLDevicePrivate
  {
  public:

    SSLDevicePrivate(SSLDevice::ConnectionType conType = SSLDevice::NOSSL);
    ~SSLDevicePrivate();

    bool isSSL();
    bool isTLS();

    SSLQtHandler *qtHandler;

#ifndef NO_OPENSSL
    SSL *_ssl;
    SSL_CTX *_ctx;
    SSL_METHOD *_method;
    X509 *x509;
#endif

    SSLBase *sslBase;
    SSLDevice::ConnectionType _conType;

    SSLDevice::ConnectionState _state;

    QString certFile;
    QString certFolder;
    int verifyType;
    SSLDevice::SSLPerformance performance;
  };
};

// Private member
SSLDevicePrivate::SSLDevicePrivate(SSLDevice::ConnectionType conType)
  : qtHandler(0),
#ifndef NO_OPENSSL
    _ssl(0L),
    _ctx(0L),
    _method(0L),
#endif
    sslBase(0L),
    _conType(conType),
    _state(SSLDevice::CLOSE),
    verifyType(0),
    performance(SSLDevice::P_DEFAULT)
{
  sslBase = SSLBase::ref();

  qtHandler = new SSLQtHandler();
}

SSLDevicePrivate::~SSLDevicePrivate()
{
  if (qtHandler) {
    delete qtHandler;
    qtHandler = 0;
  }

  _state = SSLDevice::CLOSE;
}

bool SSLDevicePrivate::isSSL()
{
#ifdef SSL_DEBUG
  qDebug("isSSL");
#endif

  if (_conType != SSLDevice::NOSSL)
    return true;

#ifdef SSL_DEBUG
  qDebug("isSSL - very bad");
#endif

  return false;
}

bool SSLDevicePrivate::isTLS()
{
  if (_conType == SSLDevice::TLS)
    return true;

  return false;
}

// SSL Device
SSLDevice::SSLDevice(Type type, SSLDevice::ConnectionType conType)
  : QSocketDevice(type)
{
  d = new SSLDevicePrivate(conType);
#ifdef TKCSSL_LINUX
  setReceiveBufferSize(262144);
#endif
}

SSLDevice::SSLDevice(int socket, Type type, SSLDevice::ConnectionType conType)
  : QSocketDevice(socket, type)
{
  d = new SSLDevicePrivate(conType);
#ifdef TKCSSL_LINUX
  setReceiveBufferSize(262144);
#endif
}

SSLDevice::~SSLDevice()
{
  close();

  if (d) {
    delete d;
    d = 0L;
  }
}

bool SSLDevice::open(int mode)
{
#ifdef SSL_DEBUG
  qDebug("SSLDevice::open");
#endif

  if (QSocketDevice::open(mode)) {
    if (d->isSSL() && d->sslBase->isOpenSSL())
      return initSSL();

    return true;
  }

  return false;
}

void SSLDevice::close()
{
#ifdef SSL_DEBUG
  qDebug("SSLDevice::close");
#endif

  QSocketDevice::close();

  if (d->isSSL() && d->sslBase->isOpenSSL())
    closeSSL();
}

bool SSLDevice::connect(const QHostAddress &addr, Q_UINT16 port)
{
  return QSocketDevice::connect(addr, port);
}


#if QT_VERSION < 300
int SSLDevice::bytesAvailable() const
#else
Q_LONG SSLDevice::bytesAvailable() const
#endif
{
#ifndef NO_OPENSSL
  if (d->_state != SSLDevice::CONNECTED)
    return QSocketDevice::bytesAvailable();

  Q_LONG iRet = QSocketDevice::bytesAvailable();
  if (!iRet) {
    iRet = d->sslBase->SSL_pending(d->_ssl);
  } else {
    // some dirty trick here :(
    if (iRet > 16384) {
      iRet = iRet >> 1;
    }
  }

#ifdef SSL_DEBUG
  qDebug("SSLDevice::bytesAvailable = %d", iRet);
#endif

  return iRet;
#else
  return -1;
#endif
}

#if QT_VERSION < 300
int SSLDevice::readBlock(char *data, uint maxlen)
#else
Q_LONG SSLDevice::readBlock(char *data, Q_ULONG maxlen)
#endif
{

#ifndef NO_OPENSSL
  if (d->_state != SSLDevice::CONNECTED)
    return QSocketDevice::readBlock(data, maxlen);

  Q_ULONG bufSize = 1024;

  QEventLoop *eventLoop = qApp->eventLoop();
  // dirty trick :(
  switch (d->performance) {
    case SSLDevice::P_DEFAULT :
    default :
      bufSize = 8192;
      break;

    case SSLDevice::P_LOW :
      bufSize = 1024;
      break;

    case SSLDevice::P_VERY_LOW :
      if (eventLoop)
        eventLoop->processEvents(QEventLoop::ExcludeSocketNotifiers, 1);
      bufSize = 128;
      break;
  }

#ifdef SSL_DEBUG
  qDebug("SSLDevice::readBlock %u", (unsigned)maxlen);
#endif

  QByteArray buffer;
  if (maxlen >= bufSize) {
    buffer.resize(bufSize);
  } else {
    buffer.resize(maxlen);
  }

  char *ptrBuffer = buffer.data();
  int iSize = buffer.size();

  QByteArray output(maxlen);
  char *ptrOutput = output.data();

  SSLBase *sslbase = d->sslBase;
  SSL *ptrSSL = d->_ssl;

  Q_LONG ret = -1;
  Q_ULONG length = 0;
  while (d && d->_state == SSLDevice::CONNECTED &&
         (ret = sslbase->SSL_read(ptrSSL, ptrBuffer, iSize)) > 0) {
    memcpy(ptrOutput + length, ptrBuffer, ret);
    length += ret;

    if ((length + iSize) > maxlen ||
        sslbase->SSL_pending(ptrSSL) <= 0)
      break;

    // dirty trick :(
    switch (d->performance) {
      case SSLDevice::P_DEFAULT :
      default :
        break;

      case SSLDevice::P_LOW :
        if (eventLoop)
          eventLoop->processEvents(QEventLoop::ExcludeSocketNotifiers, 1);
        break;

      case SSLDevice::P_VERY_LOW :
        if (eventLoop)
          eventLoop->processEvents(QEventLoop::ExcludeSocketNotifiers, 1);
        break;
    }
  }

  // error
  if (ret == -1)
    return -1;

  // slow connection, refresh the UI
  if (length < 4096) {
    if (eventLoop)
      eventLoop->processEvents(QEventLoop::ExcludeSocketNotifiers, 1);
  }

  output.resize(length);
  memcpy(data, output.data(), length);

#ifdef SSL_DEBUG
  qDebug("SSLDevice::readBlock -- %u", (unsigned)length);
#endif

  return length;
#else
  return QSocketDevice::readBlock(data, maxlen);
#endif
}

#if QT_VERSION < 300
int SSLDevice::writeBlock(const char *data, uint len)
#else
Q_LONG SSLDevice::writeBlock(const char *data, Q_ULONG len)
#endif
{
#ifndef NO_OPENSSL
  if (d->_state != SSLDevice::CONNECTED)
    return QSocketDevice::writeBlock(data, len);

#ifdef SSL_DEBUG
  qDebug("SSLDevice::writeBlock : \n%*.*s\n", (int)len, (int)len, data);
#endif

  QEventLoop *eventLoop = qApp->eventLoop();

  Q_ULONG writeSize = 16384;
  Q_ULONG bufSize = writeSize < len ? writeSize : len;
  Q_ULONG lWrite = 0L;
  Q_LONG lRet = 0L;

  d->sslBase->SSL_set_mode(d->_ssl, SSL_MODE_ENABLE_PARTIAL_WRITE);

  while (lWrite < len && d->_state == SSLDevice::CONNECTED) {
    lRet = d->sslBase->SSL_write(d->_ssl, data + lWrite, bufSize);
    if (lRet <= 0L) {
      if (lRet < 0L) {
        return lRet;
      }

      break;
    }

    if (eventLoop) {
      eventLoop->processEvents(QEventLoop::ExcludeSocketNotifiers, 1);
    }

    lWrite += lRet;
    bufSize = writeSize < len - lWrite ? writeSize : len - lWrite;
    //    qDebug("####### WRITE = %ld || %ld", lRet, lWrite);
  }

  //  qDebug("******** WRITE = %ld", lWrite);

  if (d->_state == SSLDevice::CONNECTED)
    return lWrite;
  else
    return -1L;

  //  return d->sslBase->SSL_write(d->_ssl, data, len);
#else
  return QSocketDevice::writeBlock(data, len);
#endif
}

bool SSLDevice::initSSL()
{
#ifndef NO_OPENSSL
#ifdef SSL_DEBUG
  qDebug("initSSL");
#endif

  if (d->_state != SSLDevice::CLOSE) {
    qDebug("initSSL: device state != CLOSE");
    return false;
  }

  switch (d->_conType) {
    case SSLDevice::SSL2 :
      d->_method = d->sslBase->SSLv2_client_method();
      break;
    case SSLDevice::SSL3 :
      d->_method = d->sslBase->SSLv3_client_method();
      break;
    case SSLDevice::SSL23 :
      d->_method = d->sslBase->SSLv23_client_method();
      break;
    case SSLDevice::TLS :
      d->_method = d->sslBase->TLSv1_client_method();
      break;
    default:
      assert(0);
      break;
  }
  if (NULL == d->_method) {
    qDebug("initSSL: no method");
    return false;
  }

  d->_ctx = d->sslBase->SSL_CTX_new(d->_method);
  if (NULL == d->_ctx) {
    qDebug("initSSL: no ctx");
    return false;
  }

  // certificates stuff
  if (d->verifyType) {
    QCString filePath;
    QCString folderPath;
    QFile certFile(d->certFile);
    QDir certFolder(d->certFolder);
    if (certFile.exists()) {
      filePath = QFile::encodeName(certFile.name());
    } else {
      QFile tryFile(d->certFolder + "/" + d->certFile);
      if (tryFile.exists()) {
        filePath = QFile::encodeName(tryFile.name());
      }
    }
    if (certFolder.exists()) {
      folderPath = QFile::encodeName(certFolder.absPath());
    }

    if (!filePath.isEmpty() || !folderPath.isEmpty()) {
      /*      if( -1 == d->sslBase->SSL_CTX_load_verify_locations(d->_ctx, filePath, folderPath) )
            {
              qDebug("SSL_CTX_load_verify_locations error");
            }*/
    } else {
      qDebug("SSL_CTX_load_verify_locations empty names");
    }

    // default should be 9 as well
    /*    d->sslBase->SSL_CTX_set_verify_depth(d->_ctx, 9);*/
  }

  d->_state = SSLDevice::INIT;
  return true;
#else
  qDebug("no openssl");
  return false;
#endif
}

bool SSLDevice::connectSSL()
{
  if (d->isSSL() && d->sslBase->isOpenSSL()) {
#ifndef NO_OPENSSL
    //qDebug("SSLDevice : ssl connect");
    if (d->_state != SSLDevice::INIT) {
      qDebug("SSLDevice : wrong state");
      return false;
    }

    d->_ssl = d->sslBase->SSL_new(d->_ctx);
    if (!d->_ssl) {
      qDebug("SSLDevice : cannot create a new SSL context");
      return false;
    }

    d->sslBase->SSL_set_connect_state(d->_ssl);

#ifdef _OLD_EAY_
    if (d->isTLS())
      SSL_CTX_set_options(d->_ctx, SSL_OP_ALL/*SSL_OP_NO_TLSv1*/);
    else
      SSL_CTX_set_options(d->_ctx, SSL_OP_ALL);
#else
    if (d->isTLS())
      d->sslBase->SSL_set_options(d->_ssl, SSL_OP_NO_TLSv1);
    else
      d->sslBase->SSL_set_options(d->_ssl, SSL_OP_ALL);
#endif

    // Load the (CA) certificates
    if (!d->certFile.isEmpty() || !d->certFolder.isEmpty()) {
      if (!d->sslBase->SSL_CTX_load_verify_locations(
            d->_ctx,
            d->certFile.ascii(),
            d->certFolder.ascii())) {
        qDebug("Failed to load the list of trusted CAs");
        /*        return false;*/
      }
    }

    if (!(d->sslBase->SSL_set_fd(d->_ssl, socket()))) {
      qDebug("SSLDevice : cannot assign the socket for SSL");
      return false;
    }

    if (!(d->sslBase->SSL_connect(d->_ssl))) {
      d->sslBase->ERR_print_errors_fp(stdout);
      qDebug("SSLDevice : SSL handshake failed.");
      return false;
    }

    // set the read ahead flag
    // it's buggy now, don't use it
    //    d->sslBase->SSL_set_read_ahead(d->_ssl, true);
    d->sslBase->SSL_set_mode(d->_ssl, SSL_MODE_ENABLE_PARTIAL_WRITE);

    if (d->verifyType) {
      d->x509 = d->sslBase->SSL_get_peer_certificate(d->_ssl);
      long lRet = d->sslBase->SSL_get_verify_result(d->_ssl);
      if (lRet != X509_V_OK) {
        qDebug("Certificate Error: %s", d->sslBase->verifyErrorString(lRet).latin1());
        if (qtHandler()) {
          qtHandler()->emit_certificateVerifyFailed(d->sslBase->verifyErrorString(lRet));
        }

        if (d->verifyType & SSLDevice::NOT_ACCEPTED) {
          return false;
        }
        if (d->verifyType & SSLDevice::ASKING) {
          if (!acceptCertificate()) {
            return false;
          }
        }
        if (d->verifyType & SSLDevice::SAVE) {
          saveCertificate();
        }
      } else {
        qDebug("The certificate is OK.");
      }
    }
#else
    return false;
#endif
  }

  d->_state = SSLDevice::CONNECTED;

  return true;
}

void SSLDevice::closeSSL()
{
#ifndef NO_OPENSSL
  switch (d->_state) {
    case SSLDevice::CONNECTED :
      if (d->_ssl)
        d->sslBase->SSL_shutdown(d->_ssl);
    case SSLDevice::INIT :
      if (d->_ssl) {
        d->sslBase->SSL_free(d->_ssl);
        d->_ssl = 0L;
      }
      if (d->_ctx) {
        d->sslBase->SSL_CTX_free(d->_ctx);
        d->_ctx = 0L;
      }
    case SSLDevice::CLOSE :
    default:
      break;
  }
  d->_method = 0L;

  d->_state = SSLDevice::CLOSE;
#endif
}

SSLDevice::ConnectionType SSLDevice::connectionType() const
{
  return d->_conType;
}

void SSLDevice::setCertificatesFile(const QString &fileName)
{
  d->certFile = fileName;
}

void SSLDevice::setCertificatesFolder(const QString &folderName)
{
  d->certFolder = folderName;
}

QString SSLDevice::certificatesFile() const
{
  return d->certFile;
}

QString SSLDevice::certificatesFolder() const
{
  return d->certFolder;
}

void SSLDevice::setVerifyType(int verifyType)
{
  d->verifyType = verifyType;
}

int SSLDevice::verifyType() const
{
  return d->verifyType;
}

QString SSLDevice::certificateInfo() const
{
  QString info;

#ifndef NO_OPENSSL
  if (d->x509) {
    BIO *bioOut = 0;
    char *bioPtr = 0;
    long bioLen = 0;

    bioOut = d->sslBase->BIO_new(d->sslBase->BIO_s_mem());
    d->sslBase->X509_print(bioOut, d->x509);

    // other printing functions
    // PEM_write_bio_X509(bioOut, d->x509);
    // d->sslBase->X509_print_fp(stdout, d->x509);

    bioLen = d->sslBase->SSL_BIO_get_mem_data(bioOut, &bioPtr);

    if (bioLen > 0 && bioPtr) {
      info = QString::fromLatin1(bioPtr, bioLen);
    }

    d->sslBase->BIO_free(bioOut);
  }
#endif

  return info;
}

bool SSLDevice::acceptCertificate()
{
  // ask, then set the verifyType flags
  if (qtHandler()) {
    qtHandler()->emit_doYouAcceptCertificate(this, certificateInfo());
  }

  if (d->verifyType & SSLDevice::NOT_ACCEPTED) {
    return false;
  }

  return true;
}

bool SSLDevice::saveCertificate(const QString &path)
{
  if (d->x509) {
    FILE *fpTest = 0;
    QString filePath = path;
    if (filePath.isEmpty()) {
      filePath = d->certFile;
    }

    if (!filePath.isEmpty()) {
      fpTest = fopen(filePath.ascii(), "w");
      if (fpTest > 0) {
        int len = 0;

        len = d->sslBase->PEM_write_X509(fpTest, d->x509);
        fclose(fpTest);

        if (len < 0) {
          qDebug("saveCertificate: PEM_write_X509 error.");
        } else {
          return true;
        }
      } else {
        qDebug("saveCertificate: couldn't open the file %s.", filePath.latin1());
      }
    } else {
      qDebug("saveCertificate: File name is empty.");
    }
  }

  return false;
}

SSLQtHandler *SSLDevice::qtHandler()
{
  if (d) {
    return d->qtHandler;
  }

  return 0;
}

void SSLDevice::setPerformance(SSLDevice::SSLPerformance perf)
{
  d->performance = perf;
}

SSLDevice::SSLPerformance SSLDevice::performance() const
{
  return d->performance;
}
