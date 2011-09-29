/***************************************************************************
 AQSRegion_p.h
 -------------------
 begin                : 18/02/2011
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

#ifndef AQSREGION_P_H_
#define AQSREGION_P_H_

#include "AQSPointArray_p.h"
#include <qregion.h>

// @AQ_PTR_INDIRECTION@

class AQSRegion : public AQSVoidPtr
{
  Q_OBJECT

  Q_PROPERTY(QRegion toVariant READ toVariant);

  AQ_DECLARE_AQS_VOID(Region, VoidPtr);

public:
  QRegion toVariant() const {
    if (!o_)
      return QRegion();
    return *o_;
  }

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool isNull() const;
  bool isEmpty() const;
  bool contains(const QPoint &) const;
  bool contains(const QRect &) const;
  void translate(int, int);
  QRegion *unite(QRegion *) const;
  QRegion *unite(AQSRegion *) const;
  QRegion *intersect(QRegion *) const;
  QRegion *intersect(AQSRegion *) const;
  QRegion *subtract(QRegion *) const;
  QRegion *subtract(AQSRegion *) const;
  QRegion *eor(QRegion *) const;
  QRegion *eor(AQSRegion *) const;
  QRect boundingRect() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[4].append(QString::fromLatin1("int,int,int,int"));
    candidates[5].append(QString::fromLatin1("int,int,int,int,QRegion::RegionType"));
    candidates[1].append(QString::fromLatin1("QRect"));
    candidates[2].append(QString::fromLatin1("QRect,QRegion::RegionType"));
    candidates[1].append(QString::fromLatin1("QPointArray*"));
    candidates[2].append(QString::fromLatin1("QPointArray*,bool"));
    candidates[1].append(QString::fromLatin1("QRegion*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QRegion;
    if (sgt == QString::fromLatin1("int,int,int,int"))
      return new QRegion(args[0].variant().toInt(),
                         args[1].variant().toInt(),
                         args[2].variant().toInt(),
                         args[3].variant().toInt());
    if (sgt == QString::fromLatin1("int,int,int,int,QRegion::RegionType"))
      return new QRegion(args[0].variant().toInt(),
                         args[1].variant().toInt(),
                         args[2].variant().toInt(),
                         args[3].variant().toInt(),
                         static_cast<QRegion::RegionType>(args[4].variant().toUInt()));
    if (sgt == QString::fromLatin1("QRect"))
      return new QRegion(*(argValue<QRect *>(args[0])));
    if (sgt == QString::fromLatin1("QRect,QRegion::RegionType"))
      return new QRegion(*(argValue<QRect *>(args[0])),
                         static_cast<QRegion::RegionType>(args[1].variant().toUInt()));
    if (sgt == QString::fromLatin1("QPointArray*"))
      return new QRegion(*argValue<QPointArray *>(args[0]));
    if (sgt == QString::fromLatin1("QPointArray*,bool"))
      return new QRegion(*argValue<QPointArray *>(args[0]),
                         args[1].variant().toBool());
    if (sgt == QString::fromLatin1("QRegion*"))
      return new QRegion(*argValue<QRegion *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[4].append(QString::fromLatin1("int,int,int,int"));
    candidates[5].append(QString::fromLatin1("int,int,int,int,QRegion::RegionType"));
    candidates[1].append(QString::fromLatin1("QRect"));
    candidates[2].append(QString::fromLatin1("QRect,QRegion::RegionType"));
    candidates[1].append(QString::fromLatin1("QPointArray*"));
    candidates[2].append(QString::fromLatin1("QPointArray*,bool"));
    candidates[1].append(QString::fromLatin1("QRegion*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSRegion::isNull() const
{
  AQ_CALL_RET_V(isNull(), bool);
}
inline bool AQSRegion::isEmpty() const
{
  AQ_CALL_RET_V(isEmpty(), bool);
}
inline bool AQSRegion::contains(const QPoint &arg0) const
{
  AQ_CALL_RET_V(contains(arg0), bool);
}
inline bool AQSRegion::contains(const QRect &arg0) const
{
  AQ_CALL_RET_V(contains(arg0), bool);
}
inline void AQSRegion::translate(int arg0,  int arg1)
{
  AQ_CALL_VOID(translate(arg0, arg1));
}
inline QRegion *AQSRegion::unite(QRegion *arg0) const
{
  AQ_CALL_RET_PTR(unite(*arg0), QRegion);
}
inline QRegion *AQSRegion::unite(AQSRegion *arg0) const
{
  AQ_CALL_RET_PTR(unite(*arg0), QRegion);
}
inline QRegion *AQSRegion::intersect(QRegion *arg0) const
{
  AQ_CALL_RET_PTR(intersect(*arg0), QRegion);
}
inline QRegion *AQSRegion::intersect(AQSRegion *arg0) const
{
  AQ_CALL_RET_PTR(intersect(*arg0), QRegion);
}
inline QRegion *AQSRegion::subtract(QRegion *arg0) const
{
  AQ_CALL_RET_PTR(subtract(*arg0), QRegion);
}
inline QRegion *AQSRegion::subtract(AQSRegion *arg0) const
{
  AQ_CALL_RET_PTR(subtract(*arg0), QRegion);
}
inline QRegion *AQSRegion::eor(QRegion *arg0) const
{
  AQ_CALL_RET_PTR(eor(*arg0), QRegion);
}
inline QRegion *AQSRegion::eor(AQSRegion *arg0) const
{
  AQ_CALL_RET_PTR(eor(*arg0), QRegion);
}
inline QRect AQSRegion::boundingRect() const
{
  AQ_CALL_RET_V(boundingRect(), QRect);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSREGION_P_H_ */
// @AQSVOIDPTR@
