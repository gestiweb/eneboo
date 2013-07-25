/***************************************************************************
 AQSPoint_p.h
 -------------------
 begin                : 19/06/2013
 copyright            : (C) 2003-2013 by InfoSiAL S.L.
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

#ifndef AQSPOINT_P_H_
#define AQSPOINT_P_H_

#include "AQSVoidPtr_p.h"
#include <qpoint.h>

// @AQ_PTR_INDIRECTION@

class AQSPoint : public AQSVoidPtr
{
  Q_OBJECT

  Q_PROPERTY(QPoint toVariant READ toVariant);

  AQ_DECLARE_AQS_VOID(Point, VoidPtr);

public:
  QPoint toVariant() const {
    if (!o_)
      return QPoint();
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
  int x() const;
  int y() const;
  void setX(int);
  void setY(int);
  int manhattanLength() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[2].append(QString::fromLatin1("int,int"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QPoint;
    if (sgt == QString::fromLatin1("int,int"))
      return new QPoint(args[0].variant().toInt(),
                        args[1].variant().toInt());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[2].append(QString::fromLatin1("int,int"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSPoint::isNull() const
{
  AQ_CALL_RET_V(isNull(), bool);
}
inline int AQSPoint::x() const
{
  AQ_CALL_RET_V(x(), int);
}
inline int AQSPoint::y() const
{
  AQ_CALL_RET_V(y(), int);
}
inline void AQSPoint::setX(int arg0)
{
  AQ_CALL_VOID(setX(arg0));
}
inline void AQSPoint::setY(int arg0)
{
  AQ_CALL_VOID(setY(arg0));
}
inline int AQSPoint::manhattanLength() const
{
  AQ_CALL_RET_V(manhattanLength(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPOINT_P_H_ */
// @AQSVOIDPTR@
