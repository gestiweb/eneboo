/***************************************************************************
 AQSTextStream_p.h
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

#ifndef AQSTEXTSTREAM_P_H_
#define AQSTEXTSTREAM_P_H_

#include "AQSIODevice_p.h"
#include "AQSByteArray_p.h"
#include "AQSTextCodec_p.h"
#include <qtextstream.h>

// @AQ_PTR_INDIRECTION@

class AQSTextStream : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(TextStream, VoidPtr);

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

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setEncoding(uint);
  void setCodec(QTextCodec *);
  void setCodec(AQSTextCodec *);
  QTextCodec *codec();
  QIODevice *device() const;
  void setDevice(QIODevice *);
  void setDevice(AQSIODevice *);
  void unsetDevice();
  bool atEnd() const;
  QString readLine();
  QString read();
  void skipWhiteSpace();
  int flags() const;
  int flags(int);
  int setf(int);
  int setf(int, int);
  int unsetf(int);
  void reset();
  int width() const;
  int width(int);
  int fill() const;
  int fill(int);
  int precision() const;
  int precision(int);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QIODevice*"));
    candidates[2].append(QString::fromLatin1("QByteArray*,int"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QTextStream;
    if (sgt == QString::fromLatin1("QIODevice*"))
      return new QTextStream(argValue<QIODevice *>(args[0]));
    if (sgt == QString::fromLatin1("QByteArray*,int"))
      return new QTextStream(*argValue<QByteArray *>(args[0]),
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
inline void AQSTextStream::setEncoding(uint arg0)
{
  AQ_CALL_VOID(setEncoding(static_cast<QTextStream::Encoding>(arg0)));
}
inline void AQSTextStream::setCodec(QTextCodec *arg0)
{
  AQ_CALL_VOID(setCodec(arg0));
}
inline void AQSTextStream::setCodec(AQSTextCodec *arg0)
{
  AQ_CALL_VOID(setCodec(*arg0));
}
inline QTextCodec *AQSTextStream::codec()
{
  AQ_CALL_RET(codec());
}
inline QIODevice *AQSTextStream::device() const
{
  AQ_CALL_RET(device());
}
inline void AQSTextStream::setDevice(QIODevice *arg0)
{
  AQ_CALL_VOID(setDevice(arg0));
}
inline void AQSTextStream::setDevice(AQSIODevice *arg0)
{
  AQ_CALL_VOID(setDevice(*arg0));
}
inline void AQSTextStream::unsetDevice()
{
  AQ_CALL_VOID(unsetDevice());
}
inline bool AQSTextStream::atEnd() const
{
  AQ_CALL_RET_V(atEnd(), bool);
}
inline QString AQSTextStream::readLine()
{
  AQ_CALL_RET_V(readLine(), QString);
}
inline QString AQSTextStream::read()
{
  AQ_CALL_RET_V(read(), QString);
}
inline void AQSTextStream::skipWhiteSpace()
{
  AQ_CALL_VOID(skipWhiteSpace());
}
inline int AQSTextStream::flags() const
{
  AQ_CALL_RET_V(flags(), int);
}
inline int AQSTextStream::flags(int arg0)
{
  AQ_CALL_RET_V(flags(arg0), int);
}
inline int AQSTextStream::setf(int arg0)
{
  AQ_CALL_RET_V(setf(arg0), int);
}
inline int AQSTextStream::setf(int arg0,  int arg1)
{
  AQ_CALL_RET_V(setf(arg0, arg1), int);
}
inline int AQSTextStream::unsetf(int arg0)
{
  AQ_CALL_RET_V(unsetf(arg0), int);
}
inline void AQSTextStream::reset()
{
  AQ_CALL_VOID(reset());
}
inline int AQSTextStream::width() const
{
  AQ_CALL_RET_V(width(), int);
}
inline int AQSTextStream::width(int arg0)
{
  AQ_CALL_RET_V(width(arg0), int);
}
inline int AQSTextStream::fill() const
{
  AQ_CALL_RET_V(fill(), int);
}
inline int AQSTextStream::fill(int arg0)
{
  AQ_CALL_RET_V(fill(arg0), int);
}
inline int AQSTextStream::precision() const
{
  AQ_CALL_RET_V(precision(), int);
}
inline int AQSTextStream::precision(int arg0)
{
  AQ_CALL_RET_V(precision(arg0), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTEXTSTREAM_P_H_ */
// @AQSVOIDPTR@
