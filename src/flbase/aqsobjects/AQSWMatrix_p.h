/***************************************************************************
 AQSWMatrix_p.h
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

#ifndef AQSWMATRIX_P_H_
#define AQSWMATRIX_P_H_

#include "AQSRegion_p.h"
#include <qwmatrix.h>

// @AQ_PTR_INDIRECTION@

class AQSWMatrix : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(WMatrix, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setMatrix(double, double, double, double, double, double);
  double m11() const;
  double m12() const;
  double m21() const;
  double m22() const;
  double dx() const;
  double dy() const;
  void map(int, int, int *, int *) const;
  void map(double, double, double *, double *) const;
  QRect mapRect(const QRect &) const;
  QPoint map(const QPoint &) const;
  QPointArray *map(QPointArray *) const;
  QPointArray *map(AQSPointArray *) const;
  QRegion *map(QRegion *) const;
  QRegion *map(AQSRegion *) const;
  QRegion *mapToRegion(const QRect &) const;
  QPointArray *mapToPolygon(const QRect &) const;
  void reset();
  bool isIdentity() const;
  QWMatrix *translate(double, double);
  QWMatrix *scale(double, double);
  QWMatrix *shear(double, double);
  QWMatrix *rotate(double);
  bool isInvertible() const;
  double det() const;
  QWMatrix *invert(bool* = 0) const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[6].append(QString::fromLatin1("double,double,double,double,double,double"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QWMatrix;
    if (sgt == QString::fromLatin1("double,double,double,double,double,double"))
      return new QWMatrix(args[0].variant().toDouble(),
                          args[1].variant().toDouble(),
                          args[2].variant().toDouble(),
                          args[3].variant().toDouble(),
                          args[4].variant().toDouble(),
                          args[5].variant().toDouble());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[6].append(QString::fromLatin1("double,double,double,double,double,double"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSWMatrix::setMatrix(double arg0,  double arg1,  double arg2,  double arg3,  double arg4,  double arg5)
{
  AQ_CALL_VOID(setMatrix(arg0, arg1, arg2, arg3, arg4, arg5));
}
inline double AQSWMatrix::m11() const
{
  AQ_CALL_RET_V(m11(), double);
}
inline double AQSWMatrix::m12() const
{
  AQ_CALL_RET_V(m12(), double);
}
inline double AQSWMatrix::m21() const
{
  AQ_CALL_RET_V(m21(), double);
}
inline double AQSWMatrix::m22() const
{
  AQ_CALL_RET_V(m22(), double);
}
inline double AQSWMatrix::dx() const
{
  AQ_CALL_RET_V(dx(), double);
}
inline double AQSWMatrix::dy() const
{
  AQ_CALL_RET_V(dy(), double);
}
inline void AQSWMatrix::map(int arg0,  int arg1,  int *arg2,  int *arg3) const
{
  AQ_CALL_VOID(map(arg0, arg1, arg2, arg3));
}
inline void AQSWMatrix::map(double arg0,  double arg1,  double *arg2,  double *arg3) const
{
  AQ_CALL_VOID(map(arg0, arg1, arg2, arg3));
}
inline QRect AQSWMatrix::mapRect(const QRect &arg0) const
{
  AQ_CALL_RET_V(mapRect(arg0), QRect);
}
inline QPoint AQSWMatrix::map(const QPoint &arg0) const
{
  AQ_CALL_RET_V(map(arg0), QPoint);
}
inline QPointArray *AQSWMatrix::map(QPointArray *arg0) const
{
  AQ_CALL_RET_PTR(map(*arg0), QPointArray);
}
inline QPointArray *AQSWMatrix::map(AQSPointArray *arg0) const
{
  AQ_CALL_RET_PTR(map(*arg0), QPointArray);
}
inline QRegion *AQSWMatrix::map(QRegion *arg0) const
{
  AQ_CALL_RET_PTR(map(*arg0), QRegion);
}
inline QRegion *AQSWMatrix::map(AQSRegion *arg0) const
{
  AQ_CALL_RET_PTR(map(*arg0), QRegion);
}
inline QRegion *AQSWMatrix::mapToRegion(const QRect &arg0) const
{
  AQ_CALL_RET_PTR(mapToRegion(arg0), QRegion);
}
inline QPointArray *AQSWMatrix::mapToPolygon(const QRect &arg0) const
{
  AQ_CALL_RET_PTR(mapToPolygon(arg0), QPointArray);
}
inline void AQSWMatrix::reset()
{
  AQ_CALL_VOID(reset());
}
inline bool AQSWMatrix::isIdentity() const
{
  AQ_CALL_RET_V(isIdentity(), bool);
}
inline QWMatrix *AQSWMatrix::translate(double arg0,  double arg1)
{
  AQ_CALL_RET_PTR(translate(arg0, arg1), QWMatrix);
}
inline QWMatrix *AQSWMatrix::scale(double arg0,  double arg1)
{
  AQ_CALL_RET_PTR(scale(arg0, arg1), QWMatrix);
}
inline QWMatrix *AQSWMatrix::shear(double arg0,  double arg1)
{
  AQ_CALL_RET_PTR(shear(arg0, arg1), QWMatrix);
}
inline QWMatrix *AQSWMatrix::rotate(double arg0)
{
  AQ_CALL_RET_PTR(rotate(arg0), QWMatrix);
}
inline bool AQSWMatrix::isInvertible() const
{
  AQ_CALL_RET_V(isInvertible(), bool);
}
inline double AQSWMatrix::det() const
{
  AQ_CALL_RET_V(det(), double);
}
inline QWMatrix *AQSWMatrix::invert(bool *arg0) const
{
  AQ_CALL_RET_PTR(invert(arg0), QWMatrix);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSWMATRIX_P_H_ */
// @AQSVOIDPTR@
