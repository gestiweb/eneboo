/***************************************************************************
 AQSToolBox_p.h
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

#ifndef AQSTOOLBOX_P_H_
#define AQSTOOLBOX_P_H_

#include "AQSFrame_p.h"
#include "AQSIconSet_p.h"
#include <qtoolbox.h>

class AQSToolBox : public AQSFrame
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ToolBox, Frame);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int addItem(QWidget *, const QString &);
  int addItem(QWidget *, QIconSet *, const QString &);
  int addItem(QWidget *, AQSIconSet *, const QString &);
  int insertItem(int, QWidget *, const QString &);
  int insertItem(int, QWidget *, QIconSet *, const QString &);
  int insertItem(int, QWidget *, AQSIconSet *, const QString &);
  int removeItem(QWidget *);
  void setItemEnabled(int, bool);
  bool isItemEnabled(int) const;
  void setItemLabel(int, const QString &);
  QString itemLabel(int) const;
  void setItemIconSet(int, QIconSet *);
  void setItemIconSet(int, AQSIconSet *);
  QIconSet *itemIconSet(int) const;
  void setItemToolTip(int, const QString &);
  QString itemToolTip(int) const;
  QWidget *currentItem() const;
  void setCurrentItem(QWidget *);
  QWidget *item(int) const;
  int indexOf(QWidget *) const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QToolBox;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QToolBox(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QToolBox(argValue<QWidget *>(args[0]),
                          *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QToolBox(argValue<QWidget *>(args[0]),
                          *(argValue<QString *>(args[1])),
                          static_cast<WFlags>(args[2].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSToolBox::addItem(QWidget *arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(addItem(arg0, arg1), int);
}
inline int AQSToolBox::addItem(QWidget *arg0,  QIconSet *arg1,  const QString &arg2)
{
  AQ_CALL_RET_V(addItem(arg0, *arg1, arg2), int);
}
inline int AQSToolBox::addItem(QWidget *arg0, AQSIconSet *arg1,  const QString &arg2)
{
  AQ_CALL_RET_V(addItem(arg0, *arg1, arg2), int);
}
inline int AQSToolBox::insertItem(int arg0,  QWidget *arg1,  const QString &arg2)
{
  AQ_CALL_RET_V(insertItem(arg0, arg1, arg2), int);
}
inline int AQSToolBox::insertItem(int arg0,  QWidget *arg1,  QIconSet *arg2,  const QString &arg3)
{
  AQ_CALL_RET_V(insertItem(arg0, arg1, *arg2, arg3), int);
}
inline int AQSToolBox::insertItem(int arg0,  QWidget *arg1, AQSIconSet *arg2,  const QString &arg3)
{
  AQ_CALL_RET_V(insertItem(arg0, arg1, *arg2, arg3), int);
}
inline int AQSToolBox::removeItem(QWidget *arg0)
{
  AQ_CALL_RET_V(removeItem(arg0), int);
}
inline void AQSToolBox::setItemEnabled(int arg0,  bool arg1)
{
  AQ_CALL_VOID(setItemEnabled(arg0, arg1));
}
inline bool AQSToolBox::isItemEnabled(int arg0) const
{
  AQ_CALL_RET_V(isItemEnabled(arg0), bool);
}
inline void AQSToolBox::setItemLabel(int arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setItemLabel(arg0, arg1));
}
inline QString AQSToolBox::itemLabel(int arg0) const
{
  AQ_CALL_RET_V(itemLabel(arg0), QString);
}
inline void AQSToolBox::setItemIconSet(int arg0,  QIconSet *arg1)
{
  AQ_CALL_VOID(setItemIconSet(arg0, *arg1));
}
inline void AQSToolBox::setItemIconSet(int arg0, AQSIconSet *arg1)
{
  AQ_CALL_VOID(setItemIconSet(arg0, *arg1));
}
inline QIconSet *AQSToolBox::itemIconSet(int arg0) const
{
  AQ_CALL_RET_PTR(itemIconSet(arg0), QIconSet);
}
inline void AQSToolBox::setItemToolTip(int arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setItemToolTip(arg0, arg1));
}
inline QString AQSToolBox::itemToolTip(int arg0) const
{
  AQ_CALL_RET_V(itemToolTip(arg0), QString);
}
inline QWidget *AQSToolBox::currentItem() const
{
  AQ_CALL_RET(currentItem());
}
inline void AQSToolBox::setCurrentItem(QWidget *arg0)
{
  AQ_CALL_VOID(setCurrentItem(arg0));
}
inline QWidget *AQSToolBox::item(int arg0) const
{
  AQ_CALL_RET(item(arg0));
}
inline int AQSToolBox::indexOf(QWidget *arg0) const
{
  AQ_CALL_RET_V(indexOf(arg0), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTOOLBOX_P_H_ */
// @AQSWIDGET@
