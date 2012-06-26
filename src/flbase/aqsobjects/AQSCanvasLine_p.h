/***************************************************************************
 AQSCanvasLine_p.h
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

#ifndef AQSCANVASLINE_P_H_
#define AQSCANVASLINE_P_H_

#include "AQSCanvasPolygonalItem_p.h"

class AQSCanvasLine : public AQSCanvasPolygonalItem
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CanvasLine, CanvasPolygonalItem);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setPoints(int, int, int, int);
  QPoint startPoint() const;
  QPoint endPoint() const;
  virtual int rtti() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QCanvas*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QCanvas*"))
      return new QCanvasLine(argValue<QCanvas *>(args[0]));
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
inline void AQSCanvasLine::setPoints(int arg0,  int arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(setPoints(arg0, arg1, arg2, arg3));
}
inline QPoint AQSCanvasLine::startPoint() const
{
  AQ_CALL_RET_V(startPoint(), QPoint);
}
inline QPoint AQSCanvasLine::endPoint() const
{
  AQ_CALL_RET_V(endPoint(), QPoint);
}
inline int AQSCanvasLine::rtti() const
{
  AQ_CALL_RET_V(rtti(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVASLINE_P_H_ */
// @AQSVOIDPTR@
