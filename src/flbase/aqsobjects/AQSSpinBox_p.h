/***************************************************************************
 AQSSpinBox_p.h
 -------------------
 begin                : 30/01/2011
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

#ifndef AQSSPINBOX_P_H_
#define AQSSPINBOX_P_H_

#include "AQSWidget_p.h"
#include <qspinbox.h>

class AQSSpinBox : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(SpinBox, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setValidator(const QValidator *);
  const QValidator *validator() const;
  QRect upRect() const;
  QRect downRect() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("int,int"));
    candidates[3].append(QString::fromLatin1("int,int,int"));
    candidates[4].append(QString::fromLatin1("int,int,int,QWidget*"));
    candidates[5].append(QString::fromLatin1("int,int,int,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QSpinBox;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QSpinBox(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QSpinBox(argValue<QWidget *>(args[0]),
                          *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("int,int"))
      return new QSpinBox(args[0].variant().toInt(),
                          args[1].variant().toInt());
    if (sgt == QString::fromLatin1("int,int,int"))
      return new QSpinBox(args[0].variant().toInt(),
                          args[1].variant().toInt(),
                          args[2].variant().toInt());
    if (sgt == QString::fromLatin1("int,int,int,QWidget*"))
      return new QSpinBox(args[0].variant().toInt(),
                          args[1].variant().toInt(),
                          args[2].variant().toInt(),
                          argValue<QWidget *>(args[3]));
    if (sgt == QString::fromLatin1("int,int,int,QWidget*,QString"))
      return new QSpinBox(args[0].variant().toInt(),
                          args[1].variant().toInt(),
                          args[2].variant().toInt(),
                          argValue<QWidget *>(args[3]),
                          *(argValue<QString *>(args[4])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("int,int"));
    candidates[3].append(QString::fromLatin1("int,int,int"));
    candidates[4].append(QString::fromLatin1("int,int,int,QWidget*"));
    candidates[5].append(QString::fromLatin1("int,int,int,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSSpinBox::setValidator(const QValidator *arg0)
{
  AQ_CALL_VOID(setValidator(arg0));
}
inline const QValidator *AQSSpinBox::validator() const
{
  AQ_CALL_RET(validator());
}
inline QRect AQSSpinBox::upRect() const
{
  AQ_CALL_RET_V(upRect(), QRect);
}
inline QRect AQSSpinBox::downRect() const
{
  AQ_CALL_RET_V(downRect(), QRect);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSPINBOX_P_H_ */
// @AQSWIDGET@
