/***************************************************************************
 AQSCanvasEllipse_p.h
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

#ifndef AQSCANVASELLIPSE_P_H_
#define AQSCANVASELLIPSE_P_H_

#include "AQSCanvasPolygonalItem_p.h"

class AQSCanvasEllipse : public AQSCanvasPolygonalItem
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CanvasEllipse, CanvasPolygonalItem);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int width() const;
  int height() const;
  void setSize(int, int);
  void setAngles(int, int);
  int angleStart() const;
  int angleLength() const;
  virtual int rtti() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QCanvas*"));
    candidates[3].append(QString::fromLatin1("int,int,QCanvas*"));
    candidates[5].append(QString::fromLatin1("int,int,int,int,QCanvas*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QCanvas*"))
      return new QCanvasEllipse(argValue<QCanvas *>(args[0]));
    if (sgt == QString::fromLatin1("int,int,QCanvas*"))
      return new QCanvasEllipse(args[0].variant().toInt(),
                                args[1].variant().toInt(),
                                argValue<QCanvas *>(args[2]));
    if (sgt == QString::fromLatin1("int,int,int,int,QCanvas*"))
      return new QCanvasEllipse(args[0].variant().toInt(),
                                args[1].variant().toInt(),
                                args[2].variant().toInt(),
                                args[3].variant().toInt(),
                                argValue<QCanvas *>(args[4]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QCanvas*"));
    candidates[3].append(QString::fromLatin1("int,int,QCanvas*"));
    candidates[5].append(QString::fromLatin1("int,int,int,int,QCanvas*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSCanvasEllipse::width() const
{
  AQ_CALL_RET_V(width(), int);
}
inline int AQSCanvasEllipse::height() const
{
  AQ_CALL_RET_V(height(), int);
}
inline void AQSCanvasEllipse::setSize(int arg0,  int arg1)
{
  AQ_CALL_VOID(setSize(arg0, arg1));
}
inline void AQSCanvasEllipse::setAngles(int arg0,  int arg1)
{
  AQ_CALL_VOID(setAngles(arg0, arg1));
}
inline int AQSCanvasEllipse::angleStart() const
{
  AQ_CALL_RET_V(angleStart(), int);
}
inline int AQSCanvasEllipse::angleLength() const
{
  AQ_CALL_RET_V(angleLength(), int);
}
inline int AQSCanvasEllipse::rtti() const
{
  AQ_CALL_RET_V(rtti(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVASELLIPSE_P_H_ */
// @AQSVOIDPTR@
