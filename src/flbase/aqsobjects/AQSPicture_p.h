/***************************************************************************
 AQSPicture_p.h
 -------------------
 begin                : 14/02/2011
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

#ifndef AQSPICTURE_P_H_
#define AQSPICTURE_P_H_

#include "AQSVoidPtr_p.h"
#include <qpicture.h>

// @AQ_PTR_INDIRECTION@

class AQSPicture : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(Picture, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

public slots:
  QPaintDevice *paintDevice() const {
    return static_cast<QPaintDevice *>(o_);
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool isNull() const;
  uint size() const;
  bool load(const QString &, const char* = 0);
  bool save(const QString &, const char* = 0);
  QRect boundingRect() const;
  void setBoundingRect(const QRect &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("int"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QPicture;
    if (sgt == QString::fromLatin1("int"))
      return new QPicture(args[0].variant().toInt());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("int"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSPicture::isNull() const
{
  AQ_CALL_RET_V(isNull(), bool);
}
inline uint AQSPicture::size() const
{
  AQ_CALL_RET_V(size(), uint);
}
inline bool AQSPicture::load(const QString &arg0,  const char *arg1)
{
  AQ_CALL_RET_V(load(arg0, arg1), bool);
}
inline bool AQSPicture::save(const QString &arg0,  const char *arg1)
{
  AQ_CALL_RET_V(save(arg0, arg1), bool);
}
inline QRect AQSPicture::boundingRect() const
{
  AQ_CALL_RET_V(boundingRect(), QRect);
}
inline void AQSPicture::setBoundingRect(const QRect &arg0)
{
  AQ_CALL_VOID(setBoundingRect(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPICTURE_P_H_ */
// @AQSVOIDPTR@
