/***************************************************************************
 AQSByteArray_p.h
 -------------------
 begin                : 24/02/2011
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

#ifndef AQSBYTEARRAY_P_H_
#define AQSBYTEARRAY_P_H_

#include "AQSAbstractMemArray_p.h"
#include <qcstring.h>

// @AQ_PTR_INDIRECTION@

class AQSByteArray : public AQSAbstractMemArray
{
  Q_OBJECT

  Q_PROPERTY(QString string READ string WRITE setString);
  Q_PROPERTY(QString binaryString READ binaryString);
  Q_PROPERTY(QByteArray toVariant READ toVariant);

  AQ_DECLARE_AQS_VOID(ByteArray, AbstractMemArray);

public:
  QByteArray toVariant() const {
    if (!o_)
      return QByteArray();
    return *o_;
  }

  virtual void setter(uint index, const QVariant &v) {
    if (!o_)
      return;
    (*o_)[index] = (char)v.toInt();
  }
  virtual QVariant getter(uint index) const {
    if (!o_)
      return 0;
    return (*o_)[index];
  }

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

public slots:
  QByteArray toHex() const {
    if (!o_)
      return QByteArray();
    return byteArrayToHex(o_);
  }

  QByteArray fromHex() const {
    if (!o_)
      return QByteArray();
    return byteArrayFromHex(o_);
  }

  // TODO: agregar funciones de hash que devuelvan un QByteArray binario
  // QByteArray md5Digest() const {}
  // QByteArray sha1Digest() const {}
  // QByteArray sha256Digest() const {}

  QByteArray toBase64() const {
    if (!o_)
      return QByteArray();
    return byteArrayToBase64(o_);
  }

  QByteArray fromBase64() const {
    if (!o_)
      return QByteArray();
    return byteArrayFromBase64(o_);
  }

  QByteArray compress() const {
    if (!o_)
      return QByteArray();
    return qCompress(*o_);
  }

  QByteArray uncompress() const {
    if (!o_)
      return QByteArray();
    return qUncompress(*o_);
  }

  QString sha1() const {
    if (!o_)
      return QByteArray();
    return globalAQS->sha1(o_);
  }

  int rand() {
    if (!o_)
      return 0;
    return rand_bytes(o_);
  }

  int rand_pseudo() {
    if (!o_)
      return 0;
    return rand_pseudo_bytes(o_);
  }

  QByteArray aes256Encrypt(const QByteArray &key,
                           const QByteArray &iv) {
    if (!o_)
      return QByteArray();
    return aes_256_encrypt(o_, key, iv);
  }

  QByteArray aes256Decrypt(const QByteArray &key,
                           const QByteArray &iv) {
    if (!o_)
      return QByteArray();
    return aes_256_decrypt(o_, key, iv);
  }

  void setString(const QString &str) {
    if (!o_)
      return;
    QTextStream ts(*o_, IO_WriteOnly);
    ts << str;
  }

  QString string() const {
    if (!o_)
      return QString::null;
    return QString(*o_);
  }
  
  // Esta función asegura que los bytes \0 se pasan correctamente a QString.
  QString binaryString() const {
    if (!o_)
      return QString::null;
    return QString(*o_,'b');
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QVariant at(uint) const;
  uint nrefs() const;
  uint size() const;
  uint count() const;
  bool isEmpty() const;
  bool isNull() const;
  bool resize(uint);
  bool truncate(uint);
  bool fill(char, int = -1);
  QByteArray assign(QByteArray *);
  QByteArray assign(AQSByteArray *);
  QByteArray duplicate(QByteArray *);
  QByteArray duplicate(AQSByteArray *);
  int find(char, uint = 0) const;
  int contains(char) const;
  void sort();
  int bsearch(char) const;
  virtual void detach();
  QByteArray copy() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("int"));
    candidates[1].append(QString::fromLatin1("QByteArray*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QByteArray;
    if (sgt == QString::fromLatin1("int"))
      return new QByteArray(args[0].variant().toInt());
    if (sgt == QString::fromLatin1("QByteArray*"))
      return new QByteArray(*argValue<QByteArray *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("int"));
    candidates[1].append(QString::fromLatin1("QByteArray*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QVariant AQSByteArray::at(uint arg0) const
{
  AQ_CALL_RET_V(at(arg0), QVariant);
}
inline uint AQSByteArray::nrefs() const
{
  AQ_CALL_RET_V(nrefs(), uint);
}
inline uint AQSByteArray::size() const
{
  AQ_CALL_RET_V(size(), uint);
}
inline uint AQSByteArray::count() const
{
  AQ_CALL_RET_V(count(), uint);
}
inline bool AQSByteArray::isEmpty() const
{
  AQ_CALL_RET_V(isEmpty(), bool);
}
inline bool AQSByteArray::isNull() const
{
  AQ_CALL_RET_V(isNull(), bool);
}
inline bool AQSByteArray::resize(uint arg0)
{
  AQ_CALL_RET_V(resize(arg0), bool);
}
inline bool AQSByteArray::truncate(uint arg0)
{
  AQ_CALL_RET_V(truncate(arg0), bool);
}
inline bool AQSByteArray::fill(char arg0,  int arg1)
{
  AQ_CALL_RET_V(fill(arg0, arg1), bool);
}
inline QByteArray AQSByteArray::assign(QByteArray *arg0)
{
  AQ_CALL_RET_V(assign(*arg0), QByteArray);
}
inline QByteArray AQSByteArray::assign(AQSByteArray *arg0)
{
  AQ_CALL_RET_V(assign(*arg0), QByteArray);
}
inline QByteArray AQSByteArray::duplicate(QByteArray *arg0)
{
  AQ_CALL_RET_V(duplicate(*arg0), QByteArray);
}
inline QByteArray AQSByteArray::duplicate(AQSByteArray *arg0)
{
  AQ_CALL_RET_V(duplicate(*arg0), QByteArray);
}
inline int AQSByteArray::find(char arg0,  uint arg1) const
{
  AQ_CALL_RET_V(find(arg0, arg1), int);
}
inline int AQSByteArray::contains(char arg0) const
{
  AQ_CALL_RET_V(contains(arg0), int);
}
inline void AQSByteArray::sort()
{
  AQ_CALL_VOID(sort());
}
inline int AQSByteArray::bsearch(char arg0) const
{
  AQ_CALL_RET_V(bsearch(arg0), int);
}
inline void AQSByteArray::detach()
{
  AQ_CALL_VOID(detach());
}
inline QByteArray AQSByteArray::copy() const
{
  AQ_CALL_RET_V(copy(), QByteArray);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSBYTEARRAY_P_H_ */
// @AQSVOIDPTR@
