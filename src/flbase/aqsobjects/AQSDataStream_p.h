/***************************************************************************
 AQSDataStream_p.h
 -------------------
 begin                : 05/04/2011
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

#ifndef AQSDATASTREAM_P_H_
#define AQSDATASTREAM_P_H_

#include "AQSIODevice_p.h"
#include "AQSByteArray_p.h"
#include <qdatastream.h>

// @AQ_PTR_INDIRECTION@

class AQSDataStream : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DataStream, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

public slots:
  QString opOutString() const {
    QString ret;
    if (!o_)
      return ret;
    (*o_) >> ret;
    return ret;
  }

  int opOutInt() const {
    int ret = 0;
    if (!o_)
      return ret;
    (*o_) >> ret;
    return ret;
  }

  float opOutFloat() const {
    float ret = 0;
    if (!o_)
      return ret;
    (*o_) >> ret;
    return ret;
  }

  void opIn(const QString &str) {
    if (o_)
      (*o_) << str;
  }

  void opIn(int i) {
    if (o_)
      (*o_) << i;
  }

  void opIn(float f) {
    if (o_)
      (*o_) << f;
  }

  void opInByteArray(const QByteArray &ba) {
    if (o_)
      (*o_) << ba;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QIODevice *device() const;
  void setDevice(QIODevice *);
  void setDevice(AQSIODevice *);
  void unsetDevice();
  bool atEnd() const;
  int byteOrder() const;
  void setByteOrder(int);
  bool isPrintableData() const;
  void setPrintableData(bool);
  int version() const;
  void setVersion(int);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QIODevice*"));
    candidates[2].append(QString::fromLatin1("QByteArray*,int"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDataStream;
    if (sgt == QString::fromLatin1("QIODevice*"))
      return new QDataStream(argValue<QIODevice *>(args[0]));
    if (sgt == QString::fromLatin1("QByteArray*,int"))
      return new QDataStream(*argValue<QByteArray *>(args[0]),
                             args[1].variant().toInt());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QIODevice*"));
    candidates[2].append(QString::fromLatin1("QByteArray*,int"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QIODevice *AQSDataStream::device() const
{
  AQ_CALL_RET(device());
}
inline void AQSDataStream::setDevice(QIODevice *arg0)
{
  AQ_CALL_VOID(setDevice(arg0));
}
inline void AQSDataStream::setDevice(AQSIODevice *arg0)
{
  AQ_CALL_VOID(setDevice(*arg0));
}
inline void AQSDataStream::unsetDevice()
{
  AQ_CALL_VOID(unsetDevice());
}
inline bool AQSDataStream::atEnd() const
{
  AQ_CALL_RET_V(atEnd(), bool);
}
inline int AQSDataStream::byteOrder() const
{
  AQ_CALL_RET_V(byteOrder(), int);
}
inline void AQSDataStream::setByteOrder(int arg0)
{
  AQ_CALL_VOID(setByteOrder(arg0));
}
inline bool AQSDataStream::isPrintableData() const
{
  AQ_CALL_RET_V(isPrintableData(), bool);
}
inline void AQSDataStream::setPrintableData(bool arg0)
{
  AQ_CALL_VOID(setPrintableData(arg0));
}
inline int AQSDataStream::version() const
{
  AQ_CALL_RET_V(version(), int);
}
inline void AQSDataStream::setVersion(int arg0)
{
  AQ_CALL_VOID(setVersion(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDATASTREAM_P_H_ */
// @AQSVOIDPTR@
