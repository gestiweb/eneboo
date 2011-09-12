/***************************************************************************
 AQSIconSet_p.h
 -------------------
 begin                : 11/04/2011
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

#ifndef AQSICONSET_P_H_
#define AQSICONSET_P_H_

#include "AQSPixmap_p.h"
#include <qiconset.h>

// @AQ_PTR_INDIRECTION@

class AQSIconSet : public AQSVoidPtr
{
  Q_OBJECT

  Q_PROPERTY(QIconSet toVariant READ toVariant);

  AQ_DECLARE_AQS_VOID(IconSet, VoidPtr);

public:
  QIconSet toVariant() const {
    if (!o_)
      return QIconSet();
    return *o_;
  }

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void reset(QPixmap *, uint);
  void reset(AQSPixmap *, uint);
  virtual void setPixmap(QPixmap *, uint, uint = QIconSet::Normal, uint = QIconSet::Off);
  virtual void setPixmap(AQSPixmap *, uint, uint = QIconSet::Normal, uint = QIconSet::Off);
  virtual void setPixmap(const QString &, uint, uint = QIconSet::Normal, uint = QIconSet::Off);
  QPixmap *pixmap(uint, uint, uint = QIconSet::Off) const;
  QPixmap *pixmap() const;
  bool isGenerated(uint, uint, uint = QIconSet::Off) const;
  void clearGenerated();
  bool isNull() const;
  void detach();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QPixmap*"));
    candidates[2].append(QString::fromLatin1("QPixmap*,QIconSet::Size"));
    candidates[2].append(QString::fromLatin1("QPixmap*,QPixmap*"));
    candidates[1].append(QString::fromLatin1("QIconSet*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QIconSet;
    if (sgt == QString::fromLatin1("QPixmap*"))
      return new QIconSet(*argValue<QPixmap *>(args[0]));
    if (sgt == QString::fromLatin1("QPixmap*,QIconSet::Size"))
      return new QIconSet(*argValue<QPixmap *>(args[0]),
                          static_cast<QIconSet::Size>(args[1].variant().toUInt()));
    if (sgt == QString::fromLatin1("QPixmap*,QPixmap*"))
      return new QIconSet(*argValue<QPixmap *>(args[0]),
                          *argValue<QPixmap *>(args[1]));
    if (sgt == QString::fromLatin1("QIconSet*"))
      return new QIconSet(*argValue<QIconSet *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QPixmap*"));
    candidates[2].append(QString::fromLatin1("QPixmap*,QIconSet::Size"));
    candidates[2].append(QString::fromLatin1("QPixmap*,QPixmap*"));
    candidates[1].append(QString::fromLatin1("QIconSet*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSIconSet::reset(QPixmap *arg0,  uint arg1)
{
  AQ_CALL_VOID(reset(*arg0, static_cast<QIconSet::Size>(arg1)));
}
inline void AQSIconSet::reset(AQSPixmap *arg0,  uint arg1)
{
  AQ_CALL_VOID(reset(*arg0, static_cast<QIconSet::Size>(arg1)));
}
inline void AQSIconSet::setPixmap(QPixmap *arg0,  uint arg1,  uint arg2,  uint arg3)
{
  AQ_CALL_VOID(setPixmap(*arg0, static_cast<QIconSet::Size>(arg1), static_cast<QIconSet::Mode>(arg2), static_cast<QIconSet::State>(arg3)));
}
inline void AQSIconSet::setPixmap(AQSPixmap *arg0,  uint arg1,  uint arg2,  uint arg3)
{
  AQ_CALL_VOID(setPixmap(*arg0, static_cast<QIconSet::Size>(arg1), static_cast<QIconSet::Mode>(arg2), static_cast<QIconSet::State>(arg3)));
}
inline void AQSIconSet::setPixmap(const QString &arg0,  uint arg1,  uint arg2,  uint arg3)
{
  AQ_CALL_VOID(setPixmap(arg0, static_cast<QIconSet::Size>(arg1), static_cast<QIconSet::Mode>(arg2), static_cast<QIconSet::State>(arg3)));
}
inline QPixmap *AQSIconSet::pixmap(uint arg0,  uint arg1,  uint arg2) const
{
  AQ_CALL_RET_PTR(pixmap(static_cast<QIconSet::Size>(arg0), static_cast<QIconSet::Mode>(arg1), static_cast<QIconSet::State>(arg2)), QPixmap);
}
inline QPixmap *AQSIconSet::pixmap() const
{
  AQ_CALL_RET_PTR(pixmap(), QPixmap);
}
inline bool AQSIconSet::isGenerated(uint arg0,  uint arg1,  uint arg2) const
{
  AQ_CALL_RET_V(isGenerated(static_cast<QIconSet::Size>(arg0), static_cast<QIconSet::Mode>(arg1), static_cast<QIconSet::State>(arg2)), bool);
}
inline void AQSIconSet::clearGenerated()
{
  AQ_CALL_VOID(clearGenerated());
}
inline bool AQSIconSet::isNull() const
{
  AQ_CALL_RET_V(isNull(), bool);
}
inline void AQSIconSet::detach()
{
  AQ_CALL_VOID(detach());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSICONSET_P_H_ */
// @AQSVOIDPTR@
