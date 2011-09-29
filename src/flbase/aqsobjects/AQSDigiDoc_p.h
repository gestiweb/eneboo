/***************************************************************************
 AQSDigiDoc_p.h
 -------------------
 begin                : 09/03/2011
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

#ifndef AQSDIGIDOC_P_H_
#define AQSDIGIDOC_P_H_

#include "AQSByteArray_p.h"
#include "AQObjects.h"

class AQSDigiDoc : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(DigiDoc, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int dataSignature(QByteArray *, QByteArray *, const QString &, const QString &);
  int dataSignature(QByteArray *, AQSByteArray *, const QString &, const QString &);
  int dataSignature(AQSByteArray *, QByteArray *, const QString &, const QString &);
  int dataSignature(AQSByteArray *, AQSByteArray *, const QString &, const QString &);
  int dataSignature(const QString &, QByteArray *, const QString &, const QString &);
  int dataSignature(const QString &, AQSByteArray *, const QString &, const QString &);
  int fileSignature(const QString &, QByteArray *, const QString &, const QString &);
  int fileSignature(const QString &, AQSByteArray *, const QString &, const QString &);
  int dataDigest(QByteArray *, QByteArray *);
  int dataDigest(QByteArray *, AQSByteArray *);
  int dataDigest(AQSByteArray *, QByteArray *);
  int dataDigest(AQSByteArray *, AQSByteArray *);
  int dataDigest(const QString &, QByteArray *);
  int dataDigest(const QString &, AQSByteArray *);
  int fileDigest(const QString &, QByteArray *);
  int fileDigest(const QString &, AQSByteArray *);
  int verifySignatureFile(const QString &, QByteArray *, const QString &);
  int verifySignatureFile(const QString &, AQSByteArray *, const QString &);
  int verifySignatureData(QByteArray *, QByteArray *, const QString &);
  int verifySignatureData(QByteArray *, AQSByteArray *, const QString &);
  int verifySignatureData(AQSByteArray *, QByteArray *, const QString &);
  int verifySignatureData(AQSByteArray *, AQSByteArray *, const QString &);
  int verifyCertByOCSP(const QString &);
  int certDigest(const QString &, QByteArray *);
  int certDigest(const QString &, AQSByteArray *);
  int certIssuerDN(const QString &, QString &);
  int certSerialNumber(const QString &, QByteArray *);
  int certSerialNumber(const QString &, AQSByteArray *);
  int certRSAKeyValue(const QString &, QByteArray *, QByteArray *);
  int certRSAKeyValue(const QString &, QByteArray *, AQSByteArray *);
  int certRSAKeyValue(const QString &, AQSByteArray *, QByteArray *);
  int certRSAKeyValue(const QString &, AQSByteArray *, AQSByteArray *);
  QByteArray *encodeBase64(QByteArray *) const;
  QByteArray *encodeBase64(AQSByteArray *) const;
  QByteArray *decodeBase64(QByteArray *) const;
  QByteArray *decodeBase64(AQSByteArray *) const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQDigiDoc;
    if (sgt == QString::fromLatin1("QString"))
      return new AQDigiDoc(*(argValue<QString *>(args[0])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSDigiDoc::dataSignature(QByteArray *arg0,  QByteArray *arg1,  const QString &arg2,  const QString &arg3)
{
  AQ_CALL_RET_V(dataSignature(*arg0, *arg1, arg2, arg3), int);
}
inline int AQSDigiDoc::dataSignature(QByteArray *arg0, AQSByteArray *arg1,  const QString &arg2,  const QString &arg3)
{
  AQ_CALL_RET_V(dataSignature(*arg0, *arg1, arg2, arg3), int);
}
inline int AQSDigiDoc::dataSignature(AQSByteArray *arg0,  QByteArray *arg1,  const QString &arg2,  const QString &arg3)
{
  AQ_CALL_RET_V(dataSignature(*arg0, *arg1, arg2, arg3), int);
}
inline int AQSDigiDoc::dataSignature(AQSByteArray *arg0, AQSByteArray *arg1,  const QString &arg2,  const QString &arg3)
{
  AQ_CALL_RET_V(dataSignature(*arg0, *arg1, arg2, arg3), int);
}
inline int AQSDigiDoc::dataSignature(const QString &arg0,  QByteArray *arg1,  const QString &arg2,  const QString &arg3)
{
  AQ_CALL_RET_V(dataSignature(arg0, *arg1, arg2, arg3), int);
}
inline int AQSDigiDoc::dataSignature(const QString &arg0, AQSByteArray *arg1,  const QString &arg2,  const QString &arg3)
{
  AQ_CALL_RET_V(dataSignature(arg0, *arg1, arg2, arg3), int);
}
inline int AQSDigiDoc::fileSignature(const QString &arg0,  QByteArray *arg1,  const QString &arg2,  const QString &arg3)
{
  AQ_CALL_RET_V(fileSignature(arg0, *arg1, arg2, arg3), int);
}
inline int AQSDigiDoc::fileSignature(const QString &arg0, AQSByteArray *arg1,  const QString &arg2,  const QString &arg3)
{
  AQ_CALL_RET_V(fileSignature(arg0, *arg1, arg2, arg3), int);
}
inline int AQSDigiDoc::dataDigest(QByteArray *arg0,  QByteArray *arg1)
{
  AQ_CALL_RET_V(dataDigest(*arg0, *arg1), int);
}
inline int AQSDigiDoc::dataDigest(QByteArray *arg0, AQSByteArray *arg1)
{
  AQ_CALL_RET_V(dataDigest(*arg0, *arg1), int);
}
inline int AQSDigiDoc::dataDigest(AQSByteArray *arg0,  QByteArray *arg1)
{
  AQ_CALL_RET_V(dataDigest(*arg0, *arg1), int);
}
inline int AQSDigiDoc::dataDigest(AQSByteArray *arg0, AQSByteArray *arg1)
{
  AQ_CALL_RET_V(dataDigest(*arg0, *arg1), int);
}
inline int AQSDigiDoc::dataDigest(const QString &arg0,  QByteArray *arg1)
{
  AQ_CALL_RET_V(dataDigest(arg0, *arg1), int);
}
inline int AQSDigiDoc::dataDigest(const QString &arg0, AQSByteArray *arg1)
{
  AQ_CALL_RET_V(dataDigest(arg0, *arg1), int);
}
inline int AQSDigiDoc::fileDigest(const QString &arg0,  QByteArray *arg1)
{
  AQ_CALL_RET_V(fileDigest(arg0, *arg1), int);
}
inline int AQSDigiDoc::fileDigest(const QString &arg0, AQSByteArray *arg1)
{
  AQ_CALL_RET_V(fileDigest(arg0, *arg1), int);
}
inline int AQSDigiDoc::verifySignatureFile(const QString &arg0,  QByteArray *arg1,  const QString &arg2)
{
  AQ_CALL_RET_V(verifySignatureFile(arg0, *arg1, arg2), int);
}
inline int AQSDigiDoc::verifySignatureFile(const QString &arg0, AQSByteArray *arg1,  const QString &arg2)
{
  AQ_CALL_RET_V(verifySignatureFile(arg0, *arg1, arg2), int);
}
inline int AQSDigiDoc::verifySignatureData(QByteArray *arg0,  QByteArray *arg1,  const QString &arg2)
{
  AQ_CALL_RET_V(verifySignatureData(*arg0, *arg1, arg2), int);
}
inline int AQSDigiDoc::verifySignatureData(QByteArray *arg0, AQSByteArray *arg1,  const QString &arg2)
{
  AQ_CALL_RET_V(verifySignatureData(*arg0, *arg1, arg2), int);
}
inline int AQSDigiDoc::verifySignatureData(AQSByteArray *arg0,  QByteArray *arg1,  const QString &arg2)
{
  AQ_CALL_RET_V(verifySignatureData(*arg0, *arg1, arg2), int);
}
inline int AQSDigiDoc::verifySignatureData(AQSByteArray *arg0, AQSByteArray *arg1,  const QString &arg2)
{
  AQ_CALL_RET_V(verifySignatureData(*arg0, *arg1, arg2), int);
}
inline int AQSDigiDoc::verifyCertByOCSP(const QString &arg0)
{
  AQ_CALL_RET_V(verifyCertByOCSP(arg0), int);
}
inline int AQSDigiDoc::certDigest(const QString &arg0,  QByteArray *arg1)
{
  AQ_CALL_RET_V(certDigest(arg0, *arg1), int);
}
inline int AQSDigiDoc::certDigest(const QString &arg0, AQSByteArray *arg1)
{
  AQ_CALL_RET_V(certDigest(arg0, *arg1), int);
}
inline int AQSDigiDoc::certIssuerDN(const QString &arg0,  QString &arg1)
{
  AQ_CALL_RET_V(certIssuerDN(arg0, arg1), int);
}
inline int AQSDigiDoc::certSerialNumber(const QString &arg0,  QByteArray *arg1)
{
  AQ_CALL_RET_V(certSerialNumber(arg0, *arg1), int);
}
inline int AQSDigiDoc::certSerialNumber(const QString &arg0, AQSByteArray *arg1)
{
  AQ_CALL_RET_V(certSerialNumber(arg0, *arg1), int);
}
inline int AQSDigiDoc::certRSAKeyValue(const QString &arg0,  QByteArray *arg1,  QByteArray *arg2)
{
  AQ_CALL_RET_V(certRSAKeyValue(arg0, *arg1, *arg2), int);
}
inline int AQSDigiDoc::certRSAKeyValue(const QString &arg0,  QByteArray *arg1, AQSByteArray *arg2)
{
  AQ_CALL_RET_V(certRSAKeyValue(arg0, *arg1, *arg2), int);
}
inline int AQSDigiDoc::certRSAKeyValue(const QString &arg0, AQSByteArray *arg1,  QByteArray *arg2)
{
  AQ_CALL_RET_V(certRSAKeyValue(arg0, *arg1, *arg2), int);
}
inline int AQSDigiDoc::certRSAKeyValue(const QString &arg0, AQSByteArray *arg1, AQSByteArray *arg2)
{
  AQ_CALL_RET_V(certRSAKeyValue(arg0, *arg1, *arg2), int);
}
inline QByteArray *AQSDigiDoc::encodeBase64(QByteArray *arg0) const
{
  AQ_CALL_RET_PTR(encodeBase64(*arg0), QByteArray);
}
inline QByteArray *AQSDigiDoc::encodeBase64(AQSByteArray *arg0) const
{
  AQ_CALL_RET_PTR(encodeBase64(*arg0), QByteArray);
}
inline QByteArray *AQSDigiDoc::decodeBase64(QByteArray *arg0) const
{
  AQ_CALL_RET_PTR(decodeBase64(*arg0), QByteArray);
}
inline QByteArray *AQSDigiDoc::decodeBase64(AQSByteArray *arg0) const
{
  AQ_CALL_RET_PTR(decodeBase64(*arg0), QByteArray);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDIGIDOC_P_H_ */
// @AQOBJECT_VOID@
