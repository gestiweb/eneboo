/***************************************************************************
                               FLDigiDoc.h
                          -------------------
 begin                : lun dic 14 2009
 copyright            : (C) 2004-2009 by InfoSiAL, S.L.
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

#ifndef FLDIGIDOC_H
#define FLDIGIDOC_H

#include <qobject.h>
#include <qstring.h>

#ifndef byte
typedef unsigned char byte;
#endif

class FLDigiDoc
{
public:

  FLDigiDoc(const QString &configfile = QString::null);

  ~FLDigiDoc();

  int dataSignature(const QByteArray &data, QByteArray &signResult,
                    const QString &keyfile, const QString &passwd);
  int dataSignature(const QString &data, QByteArray &signResult,
                    const QString &keyfile, const QString &passwd);
  int fileSignature(const QString &fileName, QByteArray &signResult,
                    const QString &keyfile, const QString &passwd);

  int dataDigest(const QByteArray &data, QByteArray &digestResult);
  int dataDigest(const QString &data, QByteArray &digestResult);
  int fileDigest(const QString &fileName, QByteArray &digestResult);

  int verifySignatureFile(const QString &fileName, QByteArray &signResult,
                          const QString &certfile);
  int verifySignatureData(const QByteArray &data, QByteArray &signResult,
                          const QString &certfile);
  int verifyCertByOCSP(const QString &certfile);

  int certDigest(const QString &certfile, QByteArray &digestResult);
  int certIssuerDN(const QString &certfile, QString &issuerResult);
  int certSerialNumber(const QString &certfile, QByteArray &serialResult);
  int certRSAKeyValue(const QString &certfile, QByteArray &modResult,
                      QByteArray &expResult);

  QByteArray encodeBase64(const byte *data, int size) const;
  QByteArray encodeBase64(const QByteArray &data) const;
  QByteArray decodeBase64(const byte *data, int size) const;
  QByteArray decodeBase64(const QByteArray &data) const;
};

class FLDigiDocInterface : public QObject
{
  Q_OBJECT

public:

  /**
  Constructor
  */
  FLDigiDocInterface(const QString &configfile = QString::null) : QObject(0) {
    obj_ = new FLDigiDoc(configfile);
  }

  /**
  Destructor
  */
  ~FLDigiDocInterface() {
    if (obj_)
      delete obj_;
  }

public slots:

  QString dataSignature(const QByteArray &data, const QString &keyfile, const QString &passwd) {
    QByteArray res;
    obj_->dataSignature(data, res, keyfile, passwd);
    return res;
  }

  QString dataSignature(const QString &data, const QString &keyfile, const QString &passwd) {
    QByteArray res;
    obj_->dataSignature(data, res, keyfile, passwd);
    return res;
  }

  QString fileSignature(const QString &fileName, const QString &keyfile, const QString &passwd) {
    QByteArray res;
    obj_->fileSignature(fileName, res, keyfile, passwd);
    return res;
  }

  QString dataDigest(const QByteArray &data) {
    QByteArray res;
    obj_->dataDigest(data, res);
    return res;
  }

  QString dataDigest(const QString &data) {
    QByteArray res;
    obj_->dataDigest(data, res);
    return res;
  }

  QString fileDigest(const QString &fileName) {
    QByteArray res;
    obj_->fileDigest(fileName, res);
    return res;
  }

  QString verifySignature(const QString &fileName, const QString &certfile) {
    QByteArray res;
    obj_->verifySignatureFile(fileName, res, certfile);
    return res;
  }

  int verifyCertByOCSP(const QString &certfile) {
    return obj_->verifyCertByOCSP(certfile);
  }

  QString certDigest(const QString &certfile) {
    QByteArray res;
    obj_->certDigest(certfile, res);
    return res;
  }

  QString certIssuerDN(const QString &certfile) {
    QString res;
    obj_->certIssuerDN(certfile, res);
    return res;
  }

  QString certSerialNumber(const QString &certfile) {
    QByteArray res;
    obj_->certSerialNumber(certfile, res);
    return res;
  }

  QString certRSAKeyValue(const QString &certfile) {
    QByteArray res1, res2;
    obj_->certRSAKeyValue(certfile, res1, res2);
    return QString(res1) + "@@@@" + QString(res2);
  }

private:

  FLDigiDoc *obj_;
};

#endif
