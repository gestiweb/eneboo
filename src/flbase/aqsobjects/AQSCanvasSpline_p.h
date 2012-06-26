/***************************************************************************
 AQSCanvasSpline_p.h
 -------------------
 begin                : 06/10/2011
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

#ifndef AQSCANVASSPLINE_P_H_
#define AQSCANVASSPLINE_P_H_

#include "AQSCanvasPolygon_p.h"

class AQSCanvasSpline : public AQSCanvasPolygon
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CanvasSpline, CanvasPolygon);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setControlPoints(QPointArray *, bool = TRUE);
  void setControlPoints(AQSPointArray *, bool = TRUE);
  QPointArray *controlPoints() const;
  bool closed() const;
  virtual int rtti() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QCanvas*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QCanvas*"))
      return new QCanvasSpline(argValue<QCanvas *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QCanvas*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSCanvasSpline::setControlPoints(QPointArray *arg0,  bool arg1)
{
  AQ_CALL_VOID(setControlPoints(*arg0, arg1));
}
inline void AQSCanvasSpline::setControlPoints(AQSPointArray *arg0,  bool arg1)
{
  AQ_CALL_VOID(setControlPoints(*arg0, arg1));
}
inline QPointArray *AQSCanvasSpline::controlPoints() const
{
  AQ_CALL_RET_PTR(controlPoints(), QPointArray);
}
inline bool AQSCanvasSpline::closed() const
{
  AQ_CALL_RET_V(closed(), bool);
}
inline int AQSCanvasSpline::rtti() const
{
  AQ_CALL_RET_V(rtti(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVASSPLINE_P_H_ */
// @AQSVOIDPTR@
