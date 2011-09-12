/***************************************************************************
 AQSGroupBox_p.h
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

#ifndef AQSGROUPBOX_P_H_
#define AQSGROUPBOX_P_H_

#include "AQSFrame_p.h"
#include <qgroupbox.h>

class AQSGroupBox : public AQSFrame
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(GroupBox, Frame);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int insideMargin() const;
  int insideSpacing() const;
  void setInsideMargin(int);
  void setInsideSpacing(int);
  void addSpace(int);
  bool isChecked() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("int,Qt::Orientation"));
    candidates[3].append(QString::fromLatin1("int,Qt::Orientation,QWidget*"));
    candidates[4].append(QString::fromLatin1("int,Qt::Orientation,QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("int,Qt::Orientation,QString"));
    candidates[4].append(QString::fromLatin1("int,Qt::Orientation,QString,QWidget*"));
    candidates[5].append(QString::fromLatin1("int,Qt::Orientation,QString,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QGroupBox;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QGroupBox(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QGroupBox(argValue<QWidget *>(args[0]),
                           *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString"))
      return new QGroupBox(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QWidget*"))
      return new QGroupBox(*(argValue<QString *>(args[0])),
                           argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QString,QWidget*,QString"))
      return new QGroupBox(*(argValue<QString *>(args[0])),
                           argValue<QWidget *>(args[1]),
                           *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("int,Qt::Orientation"))
      return new QGroupBox(args[0].variant().toInt(),
                           static_cast<Qt::Orientation>(args[1].variant().toUInt()));
    if (sgt == QString::fromLatin1("int,Qt::Orientation,QWidget*"))
      return new QGroupBox(args[0].variant().toInt(),
                           static_cast<Qt::Orientation>(args[1].variant().toUInt()),
                           argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("int,Qt::Orientation,QWidget*,QString"))
      return new QGroupBox(args[0].variant().toInt(),
                           static_cast<Qt::Orientation>(args[1].variant().toUInt()),
                           argValue<QWidget *>(args[2]),
                           *(argValue<QString *>(args[3])));
    if (sgt == QString::fromLatin1("int,Qt::Orientation,QString"))
      return new QGroupBox(args[0].variant().toInt(),
                           static_cast<Qt::Orientation>(args[1].variant().toUInt()),
                           *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("int,Qt::Orientation,QString,QWidget*"))
      return new QGroupBox(args[0].variant().toInt(),
                           static_cast<Qt::Orientation>(args[1].variant().toUInt()),
                           *(argValue<QString *>(args[2])),
                           argValue<QWidget *>(args[3]));
    if (sgt == QString::fromLatin1("int,Qt::Orientation,QString,QWidget*,QString"))
      return new QGroupBox(args[0].variant().toInt(),
                           static_cast<Qt::Orientation>(args[1].variant().toUInt()),
                           *(argValue<QString *>(args[2])),
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
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("int,Qt::Orientation"));
    candidates[3].append(QString::fromLatin1("int,Qt::Orientation,QWidget*"));
    candidates[4].append(QString::fromLatin1("int,Qt::Orientation,QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("int,Qt::Orientation,QString"));
    candidates[4].append(QString::fromLatin1("int,Qt::Orientation,QString,QWidget*"));
    candidates[5].append(QString::fromLatin1("int,Qt::Orientation,QString,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSGroupBox::insideMargin() const
{
  AQ_CALL_RET_V(insideMargin(), int);
}
inline int AQSGroupBox::insideSpacing() const
{
  AQ_CALL_RET_V(insideSpacing(), int);
}
inline void AQSGroupBox::setInsideMargin(int arg0)
{
  AQ_CALL_VOID(setInsideMargin(arg0));
}
inline void AQSGroupBox::setInsideSpacing(int arg0)
{
  AQ_CALL_VOID(setInsideSpacing(arg0));
}
inline void AQSGroupBox::addSpace(int arg0)
{
  AQ_CALL_VOID(addSpace(arg0));
}
inline bool AQSGroupBox::isChecked() const
{
  AQ_CALL_RET_V(isChecked(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSGROUPBOX_P_H_ */
// @AQSWIDGET@
