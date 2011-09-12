/***************************************************************************
 AQSBuffer_p.h
 -------------------
 begin                : 28/02/2011
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

#ifndef AQSBUFFER_P_H_
#define AQSBUFFER_P_H_

#include "AQSIODevice_p.h"
#include "AQSByteArray_p.h"
#include <qbuffer.h>

// @AQ_PTR_INDIRECTION@

class AQSBuffer : public AQSIODevice
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(Buffer, IODevice);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QByteArray *buffer() const;
  bool setBuffer(QByteArray *);
  bool setBuffer(AQSByteArray *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QByteArray*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QBuffer;
    if (sgt == QString::fromLatin1("QByteArray*"))
      return new QBuffer(*argValue<QByteArray *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QByteArray*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QByteArray *AQSBuffer::buffer() const
{
  AQ_CALL_RET_PTR(buffer(), QByteArray);
}
inline bool AQSBuffer::setBuffer(QByteArray *arg0)
{
  AQ_CALL_RET_V(setBuffer(*arg0), bool);
}
inline bool AQSBuffer::setBuffer(AQSByteArray *arg0)
{
  AQ_CALL_RET_V(setBuffer(*arg0), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSBUFFER_P_H_ */
// @AQSVOIDPTR@
