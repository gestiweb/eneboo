/***************************************************************************
 AQSButtonGroup_p.h
 -------------------
 begin                : 06/02/2011
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

#ifndef AQSBUTTONGROUP_P_H_
#define AQSBUTTONGROUP_P_H_

#include "AQSGroupBox_p.h"
#include <qbuttongroup.h>

class AQSButtonGroup : public AQSGroupBox
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ButtonGroup, GroupBox);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int insert(QButton *, int = -1);
  void remove(QButton *);
  QButton *find(int) const;
  int id(QButton *) const;
  int count() const;
  void moveFocus(int);
  QButton *selected() const;

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
      return new QButtonGroup;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QButtonGroup(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QButtonGroup(argValue<QWidget *>(args[0]),
                              *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString"))
      return new QButtonGroup(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QWidget*"))
      return new QButtonGroup(*(argValue<QString *>(args[0])),
                              argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QString,QWidget*,QString"))
      return new QButtonGroup(*(argValue<QString *>(args[0])),
                              argValue<QWidget *>(args[1]),
                              *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("int,Qt::Orientation"))
      return new QButtonGroup(args[0].variant().toInt(),
                              static_cast<Qt::Orientation>(args[1].variant().toUInt()));
    if (sgt == QString::fromLatin1("int,Qt::Orientation,QWidget*"))
      return new QButtonGroup(args[0].variant().toInt(),
                              static_cast<Qt::Orientation>(args[1].variant().toUInt()),
                              argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("int,Qt::Orientation,QWidget*,QString"))
      return new QButtonGroup(args[0].variant().toInt(),
                              static_cast<Qt::Orientation>(args[1].variant().toUInt()),
                              argValue<QWidget *>(args[2]),
                              *(argValue<QString *>(args[3])));
    if (sgt == QString::fromLatin1("int,Qt::Orientation,QString"))
      return new QButtonGroup(args[0].variant().toInt(),
                              static_cast<Qt::Orientation>(args[1].variant().toUInt()),
                              *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("int,Qt::Orientation,QString,QWidget*"))
      return new QButtonGroup(args[0].variant().toInt(),
                              static_cast<Qt::Orientation>(args[1].variant().toUInt()),
                              *(argValue<QString *>(args[2])),
                              argValue<QWidget *>(args[3]));
    if (sgt == QString::fromLatin1("int,Qt::Orientation,QString,QWidget*,QString"))
      return new QButtonGroup(args[0].variant().toInt(),
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
inline int AQSButtonGroup::insert(QButton *arg0,  int arg1)
{
  AQ_CALL_RET_V(insert(arg0, arg1), int);
}
inline void AQSButtonGroup::remove(QButton *arg0)
{
  AQ_CALL_VOID(remove(arg0));
}
inline QButton *AQSButtonGroup::find(int arg0) const
{
  AQ_CALL_RET(find(arg0));
}
inline int AQSButtonGroup::id(QButton *arg0) const
{
  AQ_CALL_RET_V(id(arg0), int);
}
inline int AQSButtonGroup::count() const
{
  AQ_CALL_RET_V(count(), int);
}
inline void AQSButtonGroup::moveFocus(int arg0)
{
  AQ_CALL_VOID(moveFocus(arg0));
}
inline QButton *AQSButtonGroup::selected() const
{
  AQ_CALL_RET(selected());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSBUTTONGROUP_P_H_ */
// @AQSWIDGET@
