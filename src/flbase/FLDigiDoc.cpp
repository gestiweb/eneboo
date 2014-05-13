/***************************************************************************
 FLDigiDoc.c
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

#ifdef FL_DIGIDOC
#include <qbuffer.h>
#include <qtextstream.h>

#include "DigiDocLib.h"
#include "DigiDocConfig.h"
#include "DigiDocCert.h"
#include "DigiDocConvert.h"
#endif

#include "FLDigiDoc.h"

#ifdef FL_DIGIDOC
FLDigiDoc::FLDigiDoc(const QString &configfile)
{

  int err = ERR_OK;
  initDigiDocLib();
  if (!configfile.isEmpty()) {
    err = initConfigStore(NULL);
    if (err == ERR_OK)
      err = readConfigFile(configfile.latin1(), ITEM_TYPE_PRIVATE);
    if (err != ERR_OK)
      qWarning("FLDigiDoc:" + QString(getErrorString(err)));
  }
}

FLDigiDoc::~FLDigiDoc()
{
  finalizeDigiDocLib();
}

int FLDigiDoc::dataSignature(const QByteArray &data, QByteArray &signResult,
                             const QString &keyfile, const QString &passwd)
{
  int sigLen = SIGNATURE_LEN;
  byte buf[sigLen * sizeof(char)];
  byte buf1[sigLen * sizeof(char) * 2];
  int err = signData((const byte *)data.data(), data.size() * sizeof(char), buf,
                     &sigLen, DIGEST_SHA1, keyfile.latin1(), passwd.latin1());
  signResult = encodeBase64(buf, sigLen);
  return err;
}

int FLDigiDoc::dataSignature(const QString &data, QByteArray &signResult,
                             const QString &keyfile, const QString &passwd)
{
  QBuffer b;
  b.open(IO_WriteOnly);
  QTextStream t(&b);
  t << data;
  b.close();
  return dataSignature(b.buffer(), signResult, keyfile, passwd);
}

int FLDigiDoc::fileSignature(const QString &fileName, QByteArray &signResult,
                             const QString &keyfile, const QString &passwd)
{
  int sigLen = SIGNATURE_LEN;
  byte buf[sigLen * sizeof(char)];
  byte buf1[sigLen * sizeof(char) * 2];
  int err = calculateFileSignature(fileName.latin1(), DIGEST_SHA1, buf, &sigLen,
                                   keyfile.latin1(), passwd.latin1());
  signResult = encodeBase64(buf, sigLen);
  return err;
}

int FLDigiDoc::dataDigest(const QByteArray &data, QByteArray &digestResult)
{
  int sigLen = SIGNATURE_LEN;
  byte buf[sigLen * sizeof(char)];
  byte buf1[sigLen * sizeof(char) * 2];
  int err = calculateDigest((const byte *)data.data(), data.size() * sizeof(char),
                            DIGEST_SHA1, buf, &sigLen);
  digestResult = encodeBase64(buf, sigLen);
  return err;
}

int FLDigiDoc::dataDigest(const QString &data, QByteArray &digestResult)
{
  QBuffer b;
  b.open(IO_WriteOnly);
  QTextStream t(&b);
  t << data;
  b.close();
  return dataDigest(b.buffer(), digestResult);
}

int FLDigiDoc::fileDigest(const QString &fileName, QByteArray &digestResult)
{
  int sigLen = SIGNATURE_LEN;
  long fileLen;
  byte buf[sigLen * sizeof(char)];
  byte buf1[sigLen * sizeof(char) * 2];
  int err = calculateFileDigest(fileName.latin1(), DIGEST_SHA1, buf,
                                &sigLen, &fileLen);
  digestResult = encodeBase64(buf, sigLen);
  return err;
}

int FLDigiDoc::verifySignatureFile(const QString &fileName, QByteArray &signResult,
                                   const QString &certfile)
{
  int sigLen = SIGNATURE_LEN;
  byte buf[sigLen * sizeof(char)];
  byte buf1[sigLen * sizeof(char) * 2];
  int err = verifyFileSignature(fileName.latin1(), DIGEST_SHA1, buf, sigLen,
                                certfile.latin1());
  signResult = encodeBase64(buf, sigLen);
  return err;
}

int FLDigiDoc::verifySignatureData(const QByteArray &data, QByteArray &signResult,
                                   const QString &certfile)
{
  X509 *pCert = 0;
  int err = ReadCertificate(&pCert, certfile.latin1());
  if (!err && pCert) {
    int sigLen = SIGNATURE_LEN;
    byte buf[sigLen * sizeof(char)];
    byte buf1[sigLen * sizeof(char) * 2];
    err = verifySignature((const char *)data, data.size() * sizeof(char),
                          DIGEST_SHA1, buf, sigLen, pCert);
    signResult = encodeBase64(buf, SIGNATURE_LEN);
  }
  if (pCert) {
    X509_free(pCert);
    pCert = 0;
  }
  return err;
}

int FLDigiDoc::verifyCertByOCSP(const QString &certfile)
{
  X509 *pCert = 0;
  DigiDocMemBuf mbuf;
  mbuf.pMem = 0;
  mbuf.nLen = 0;
  int err = ReadCertificate(&pCert, certfile.latin1());
  if (!err && pCert) {
    err = ddocVerifyCertByOCSP(pCert, NULL);
    err = ddocCertGetSubjectCN(pCert, &mbuf);
    ddocMemBuf_free(&mbuf);
  }
  if (pCert) {
    X509_free(pCert);
    pCert = 0;
  }
  return err;
}

int FLDigiDoc::certDigest(const QString &certfile, QByteArray &digestResult)
{
  int err = ERR_OK, len1;
  char buf1[1024 * sizeof(char)];
  X509 *pCert = 0;
  DigiDocMemBuf mbuf;
  mbuf.pMem = 0;
  mbuf.nLen = 0;
  err = ReadCertificate(&pCert, certfile.latin1());
  if (!err && pCert) {
    len1 = sizeof(buf1);
    buf1[0] = 0;
    err = ddocCertGetDigest(pCert, &mbuf);
    encode((const byte *) mbuf.pMem, mbuf.nLen, (byte *) buf1, &len1);
    digestResult.duplicate((const char *) buf1, len1);
    ddocMemBuf_free(&mbuf);
  }
  if (pCert) {
    X509_free(pCert);
    pCert = 0;
  }
  return err;
}

int FLDigiDoc::certIssuerDN(const QString &certfile, QString &issuerResult)
{
  int err = ERR_OK;
  X509 *pCert = 0;
  DigiDocMemBuf mbuf;
  mbuf.pMem = 0;
  mbuf.nLen = 0;
  err = ReadCertificate(&pCert, certfile.latin1());
  if (!err && pCert) {
    err = ddocCertGetIssuerDN(pCert, &mbuf);
    issuerResult = QString((const char *) mbuf.pMem);
    ddocMemBuf_free(&mbuf);
  }
  if (pCert) {
    X509_free(pCert);
    pCert = 0;
  }
  return err;
}

int FLDigiDoc::certSerialNumber(const QString &certfile, QByteArray &serialResult)
{
  int bLen = 512;
  char buf[bLen * sizeof(char)];
  int err = GetCertSerialNumber(buf, bLen, certfile.latin1());
  serialResult.duplicate((const char *) buf, bLen);
  return err;
}

int FLDigiDoc::certRSAKeyValue(const QString &certfile, QByteArray &modResult,
                               QByteArray &expResult)
{
  int err = ERR_OK;
  unsigned char buf2[1024 * sizeof(unsigned char)], *buf1 = 0;
  unsigned char buf22[1024 * sizeof(unsigned char)], *buf11 = 0;
  int len2, len1;
  EVP_PKEY *pubKey;

  len1 = 512;
  buf1 = (unsigned char *) malloc(len1);
  memset(buf1, 0, len1);

  buf11 = (unsigned char *) malloc(len1);
  memset(buf11, 0, len1);

  err = ReadPublicKey(&pubKey, certfile.latin1());

if (err != ERR_OK )
	{
	qWarning("FLDigiDoc:" + QString(getErrorString(err)));
	return err;
	}
  // FIXME
  // modulus
  len1 = BN_bn2bin(pubKey->pkey.rsa->n, buf1);
  // in version 1.1 we output modulus as it is
  // starting from 1.2 we convert it to big-endian
  //  len2 = sizeof(buf2);
  //  memset(buf2, 0, len2);
  //  encode(buf1, len1, buf2, &len2);
  //  printf("Old modulus: %s\n", buf2);
  //  if (!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_1_VER)) {
  //    swapBytes((byte *)buf1, len1);
  //  }
  len2 = sizeof(buf2);
  memset(buf2, 0, len2);
  encode(buf1, len1, buf2, &len2);
  modResult.duplicate((const char *)buf2, len2);

  // exponent
  memset(buf11, 0, len1);
  len1 = BN_bn2bin(pubKey->pkey.rsa->e, buf11);
  len2 = sizeof(buf22);
  memset(buf22, 0, len2);
  encode(buf11, len1, buf22, &len2);
  expResult.duplicate((const char *)buf22, len2);

  EVP_PKEY_free(pubKey);
  free(buf1);
  free(buf11);
  return err;
}

QByteArray FLDigiDoc::encodeBase64(const byte *data, int size) const
{
  int outsize = (((size + 2) / 3) * 4) + 2;
  byte *buf = (byte *)malloc(outsize * sizeof(char));
  outsize = 1;
  encode(data, size, buf, &outsize);
  QByteArray ret(outsize);
  ret.duplicate((const char *)buf, outsize);
  free(buf);
  return ret;
}

QByteArray FLDigiDoc::encodeBase64(const QByteArray &data) const
{
  return encodeBase64((const byte *)data.data(), data.size());
}

QByteArray FLDigiDoc::decodeBase64(const byte *data, int size) const
{
  int outsize = (((size + 3) / 4) * 3) + 2;
  byte *buf = (byte *)malloc(outsize * sizeof(char));
  outsize = 1;
  decode(data, size, buf, &outsize);
  QByteArray ret(outsize);
  ret.duplicate((const char *)buf, outsize);
  free(buf);
  return ret;
}

QByteArray FLDigiDoc::decodeBase64(const QByteArray &data) const
{
  return decodeBase64((const byte *)data.data(), data.size());
}
#else
FLDigiDoc::FLDigiDoc(const QString &) {}

FLDigiDoc::~FLDigiDoc() {}

int FLDigiDoc::dataSignature(const QByteArray &, QByteArray &,
                             const QString &, const QString &) {}
int FLDigiDoc::dataSignature(const QString &, QByteArray &,
                             const QString &, const QString &) {}
int FLDigiDoc::fileSignature(const QString &, QByteArray &,
                             const QString &, const QString &) {}

int FLDigiDoc::dataDigest(const QByteArray &, QByteArray &) {}
int FLDigiDoc::dataDigest(const QString &, QByteArray &) {}
int FLDigiDoc::fileDigest(const QString &, QByteArray &) {}

int FLDigiDoc::verifySignatureFile(const QString &, QByteArray &,
                                   const QString &) {}
int FLDigiDoc::verifySignatureData(const QByteArray &, QByteArray &,
                                   const QString &) {}
int FLDigiDoc::verifyCertByOCSP(const QString &) {}

int FLDigiDoc::certDigest(const QString &, QByteArray &) {}
int FLDigiDoc::certIssuerDN(const QString &, QString &) {}
int FLDigiDoc::certSerialNumber(const QString &, QByteArray &) {}
int FLDigiDoc::certRSAKeyValue(const QString &, QByteArray &, QByteArray &) {}

QByteArray FLDigiDoc::encodeBase64(const byte *, int) const {}
QByteArray FLDigiDoc::encodeBase64(const QByteArray &) const {}
QByteArray FLDigiDoc::decodeBase64(const byte *, int) const {}
QByteArray FLDigiDoc::decodeBase64(const QByteArray &) const {}
#endif
