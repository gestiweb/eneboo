/***************************************************************************
 AQSPointArray_p.h
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

#ifndef AQSPOINTARRAY_P_H_
#define AQSPOINTARRAY_P_H_

#include "AQSAbstractMemArray_p.h"
#include <qpointarray.h>

// @AQ_PTR_INDIRECTION@

class AQSPointArray : public AQSAbstractMemArray
{
  Q_OBJECT

  Q_PROPERTY(QPointArray toVariant READ toVariant);

  AQ_DECLARE_AQS_VOID(PointArray, AbstractMemArray);

public:
  QPointArray toVariant() const {
    if (!o_)
      return QPointArray();
    return *o_;
  }

  virtual void setter(uint index, const QVariant &v) {
    if (!o_)
      return;
    (*o_)[index] = v.toPoint();
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
  bool fill(const QPoint &, int = -1);
  int find(const QPoint &, uint = 0) const;
  int contains(const QPoint &) const;
  void sort();
  int bsearch(const QPoint &) const;
  void translate(int, int);
  QRect boundingRect() const;
  void point(uint, int *, int *) const;
  QPoint point(uint) const;
  void setPoint(uint, int, int);
  void setPoint(uint, const QPoint &);
  bool putPoints(int, int, QPointArray *, int = 0);
  bool putPoints(int, int, AQSPointArray *, int = 0);
  void makeArc(int, int, int, int, int, int);
  void makeEllipse(int, int, int, int);
  QPointArray *cubicBezier() const;
  virtual void detach();
  QPointArray *copy() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("int"));
    candidates[1].append(QString::fromLatin1("QPointArray*"));
    candidates[1].append(QString::fromLatin1("QRect"));
    candidates[2].append(QString::fromLatin1("QRect,bool"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QPointArray;
    if (sgt == QString::fromLatin1("int"))
      return new QPointArray(args[0].variant().toInt());
    if (sgt == QString::fromLatin1("QPointArray*"))
      return new QPointArray(*argValue<QPointArray *>(args[0]));
    if (sgt == QString::fromLatin1("QRect"))
      return new QPointArray(*(argValue<QRect *>(args[0])));
    if (sgt == QString::fromLatin1("QRect,bool"))
      return new QPointArray(*(argValue<QRect *>(args[0])),
                             args[1].variant().toBool());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("int"));
    candidates[1].append(QString::fromLatin1("QPointArray*"));
    candidates[1].append(QString::fromLatin1("QRect"));
    candidates[2].append(QString::fromLatin1("QRect,bool"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QVariant AQSPointArray::at(uint arg0) const
{
  AQ_CALL_RET_V(at(arg0), QVariant);
}
inline uint AQSPointArray::nrefs() const
{
  AQ_CALL_RET_V(nrefs(), uint);
}
inline uint AQSPointArray::size() const
{
  AQ_CALL_RET_V(size(), uint);
}
inline uint AQSPointArray::count() const
{
  AQ_CALL_RET_V(count(), uint);
}
inline bool AQSPointArray::isEmpty() const
{
  AQ_CALL_RET_V(isEmpty(), bool);
}
inline bool AQSPointArray::isNull() const
{
  AQ_CALL_RET_V(isNull(), bool);
}
inline bool AQSPointArray::resize(uint arg0)
{
  AQ_CALL_RET_V(resize(arg0), bool);
}
inline bool AQSPointArray::truncate(uint arg0)
{
  AQ_CALL_RET_V(truncate(arg0), bool);
}
inline bool AQSPointArray::fill(const QPoint &arg0,  int arg1)
{
  AQ_CALL_RET_V(fill(arg0, arg1), bool);
}
inline int AQSPointArray::find(const QPoint &arg0,  uint arg1) const
{
  AQ_CALL_RET_V(find(arg0, arg1), int);
}
inline int AQSPointArray::contains(const QPoint &arg0) const
{
  AQ_CALL_RET_V(contains(arg0), int);
}
inline void AQSPointArray::sort()
{
  AQ_CALL_VOID(sort());
}
inline int AQSPointArray::bsearch(const QPoint &arg0) const
{
  AQ_CALL_RET_V(bsearch(arg0), int);
}
inline void AQSPointArray::translate(int arg0,  int arg1)
{
  AQ_CALL_VOID(translate(arg0, arg1));
}
inline QRect AQSPointArray::boundingRect() const
{
  AQ_CALL_RET_V(boundingRect(), QRect);
}
inline void AQSPointArray::point(uint arg0,  int *arg1,  int *arg2) const
{
  AQ_CALL_VOID(point(arg0, arg1, arg2));
}
inline QPoint AQSPointArray::point(uint arg0) const
{
  AQ_CALL_RET_V(point(arg0), QPoint);
}
inline void AQSPointArray::setPoint(uint arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(setPoint(arg0, arg1, arg2));
}
inline void AQSPointArray::setPoint(uint arg0,  const QPoint &arg1)
{
  AQ_CALL_VOID(setPoint(arg0, arg1));
}
inline bool AQSPointArray::putPoints(int arg0,  int arg1,  QPointArray *arg2,  int arg3)
{
  AQ_CALL_RET_V(putPoints(arg0, arg1, *arg2, arg3), bool);
}
inline bool AQSPointArray::putPoints(int arg0,  int arg1, AQSPointArray *arg2,  int arg3)
{
  AQ_CALL_RET_V(putPoints(arg0, arg1, *arg2, arg3), bool);
}
inline void AQSPointArray::makeArc(int arg0,  int arg1,  int arg2,  int arg3,  int arg4,  int arg5)
{
  AQ_CALL_VOID(makeArc(arg0, arg1, arg2, arg3, arg4, arg5));
}
inline void AQSPointArray::makeEllipse(int arg0,  int arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(makeEllipse(arg0, arg1, arg2, arg3));
}
inline QPointArray *AQSPointArray::cubicBezier() const
{
  AQ_CALL_RET_PTR(cubicBezier(), QPointArray);
}
inline void AQSPointArray::detach()
{
  AQ_CALL_VOID(detach());
}
inline QPointArray *AQSPointArray::copy() const
{
  AQ_CALL_RET_PTR(copy(), QPointArray);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPOINTARRAY_P_H_ */
// @AQSVOIDPTR@
