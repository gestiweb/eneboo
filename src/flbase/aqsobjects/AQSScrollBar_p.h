/***************************************************************************
 AQSScrollBar_p.h
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

#ifndef AQSSCROLLBAR_P_H_
#define AQSSCROLLBAR_P_H_

#include "AQSWidget_p.h"
#include <qscrollbar.h>

class AQSScrollBar : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ScrollBar, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int sliderStart() const;
  QRect sliderRect() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("Qt::Orientation,QWidget*"));
    candidates[3].append(QString::fromLatin1("Qt::Orientation,QWidget*,QString"));
    candidates[7].append(QString::fromLatin1("int,int,int,int,int,Qt::Orientation,QWidget*"));
    candidates[8].append(QString::fromLatin1("int,int,int,int,int,Qt::Orientation,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QScrollBar(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QScrollBar(argValue<QWidget *>(args[0]),
                            *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("Qt::Orientation,QWidget*"))
      return new QScrollBar(static_cast<Qt::Orientation>(args[0].variant().toUInt()),
                            argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("Qt::Orientation,QWidget*,QString"))
      return new QScrollBar(static_cast<Qt::Orientation>(args[0].variant().toUInt()),
                            argValue<QWidget *>(args[1]),
                            *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("int,int,int,int,int,Qt::Orientation,QWidget*"))
      return new QScrollBar(args[0].variant().toInt(),
                            args[1].variant().toInt(),
                            args[2].variant().toInt(),
                            args[3].variant().toInt(),
                            args[4].variant().toInt(),
                            static_cast<Qt::Orientation>(args[5].variant().toUInt()),
                            argValue<QWidget *>(args[6]));
    if (sgt == QString::fromLatin1("int,int,int,int,int,Qt::Orientation,QWidget*,QString"))
      return new QScrollBar(args[0].variant().toInt(),
                            args[1].variant().toInt(),
                            args[2].variant().toInt(),
                            args[3].variant().toInt(),
                            args[4].variant().toInt(),
                            static_cast<Qt::Orientation>(args[5].variant().toUInt()),
                            argValue<QWidget *>(args[6]),
                            *(argValue<QString *>(args[7])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("Qt::Orientation,QWidget*"));
    candidates[3].append(QString::fromLatin1("Qt::Orientation,QWidget*,QString"));
    candidates[7].append(QString::fromLatin1("int,int,int,int,int,Qt::Orientation,QWidget*"));
    candidates[8].append(QString::fromLatin1("int,int,int,int,int,Qt::Orientation,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSScrollBar::sliderStart() const
{
  AQ_CALL_RET_V(sliderStart(), int);
}
inline QRect AQSScrollBar::sliderRect() const
{
  AQ_CALL_RET_V(sliderRect(), QRect);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSCROLLBAR_P_H_ */
// @AQSWIDGET@
