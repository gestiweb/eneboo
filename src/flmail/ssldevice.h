/***************************************************************************
                   ssldevice.h  -  QSocketDevice with SSL support
                             -------------------
    begin                : Tue Apr 23 2002
    copyright            : (C) 2002 by theKompany.com
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#ifndef SSLDEVICE_H
#define SSLDEVICE_H

#include <qsocketdevice.h>
#include <qsocket.h>

#include <sslbase.h>

namespace tkcSSL
{
  class SSLDevicePrivate;
  class TKCSSL_API SSLDevice;
  class SSLQtHandler;

  /**Use QSocket with SSLDevice for having a SSL connection.
    *@author Eugen Constantinescu
    */
  class SSLDevice : public QSocketDevice
  {
  public:

    enum ConnectionType { NOSSL = 0, SSL2, SSL3, SSL23, TLS };
    enum ConnectionState { CLOSE = 0, INIT, CONNECTED };
    enum VerifyType { NO_CHECKING = 0, ASKING = 1, SAVE = 2, NOT_ACCEPTED = 4};
    enum SSLPerformance { P_DEFAULT = 0, P_LOW, P_VERY_LOW};

    SSLDevice(Type type = Stream, ConnectionType conType = NOSSL);
    SSLDevice(int socket, Type type, ConnectionType conType = NOSSL);
    virtual ~SSLDevice();

    // from QSocketDevice
    virtual bool open(int mode);
    virtual void close();
    virtual bool connect(const QHostAddress &, Q_UINT16);

#if QT_VERSION < 300
    virtual int  bytesAvailable() const;
    virtual int  readBlock(char *data, uint maxlen);
    virtual int  writeBlock(const char *data, uint len);
#else
    virtual Q_LONG  bytesAvailable() const;
    virtual Q_LONG  readBlock(char *data, Q_ULONG maxlen);
    virtual Q_LONG  writeBlock(const char *data, Q_ULONG len);
#endif

    // SSL related
    virtual bool initSSL();
    virtual void closeSSL();
    virtual bool connectSSL();
    virtual ConnectionType connectionType() const;

    virtual void setPerformance(SSLPerformance perf = P_DEFAULT);
    virtual SSLPerformance performance() const;

    /** Set the file which contains the certificates.*/
    virtual void setCertificatesFile(const QString &);
    /** Set the folder which contains the certificates files.*/
    virtual void setCertificatesFolder(const QString &);
    /** The file which contains the certificates.*/
    virtual QString certificatesFile() const;
    /** The folder which contains the certificates files.*/
    virtual QString certificatesFolder() const;

    virtual void setVerifyType(int);
    virtual int verifyType() const;

    /** Save the server certificate using the path parameter
      * or the certificates file.
      */
    virtual bool saveCertificate(const QString &path = QString::null);

    /** Returns a string with all the server certificate info.*/
    virtual QString certificateInfo() const;

    virtual SSLQtHandler *qtHandler();

  private:

    /** Ask for confirmation.*/
    virtual bool acceptCertificate();

    SSLDevicePrivate *d;
  };

  class SSLQtHandler : public QObject
  {
    Q_OBJECT

  public:

    virtual ~SSLQtHandler();

    virtual void emit_doYouAcceptCertificate(SSLDevice *, const QString &);

    virtual void emit_certificateVerifyFailed(const QString &);

  signals:

    /** Do you accept the certificate ? Set the verifyType.*/
    void doYouAcceptCertificate(SSLDevice *, const QString &);
    /** The certificate verification failed.*/
    void certificateVerifyFailed(const QString &);
  };

};

#endif

