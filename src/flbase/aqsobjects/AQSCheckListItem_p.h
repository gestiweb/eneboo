/***************************************************************************
 AQSCheckListItem_p.h
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

#ifndef AQSCHECKLISTITEM_P_H_
#define AQSCHECKLISTITEM_P_H_

#include "AQSListViewItem_p.h"
#include <qlistview.h>

class AQSCheckListItem : public AQSListViewItem
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CheckListItem, ListViewItem);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setOn(bool);
  bool isOn() const;
  uint type() const;
  QString text() const;
  void setTristate(bool);
  bool isTristate() const;
  uint state() const;
  void setState(uint);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QCheckListItem*,QString"));
    candidates[3].append(QString::fromLatin1("QCheckListItem*,QString,QCheckListItem::Type"));
    candidates[3].append(QString::fromLatin1("QCheckListItem*,QListViewItem*,QString"));
    candidates[4].append(QString::fromLatin1("QCheckListItem*,QListViewItem*,QString,QCheckListItem::Type"));
    candidates[2].append(QString::fromLatin1("QListViewItem*,QString"));
    candidates[3].append(QString::fromLatin1("QListViewItem*,QString,QCheckListItem::Type"));
    candidates[3].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString"));
    candidates[4].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QCheckListItem::Type"));
    candidates[2].append(QString::fromLatin1("QListView*,QString"));
    candidates[3].append(QString::fromLatin1("QListView*,QString,QCheckListItem::Type"));
    candidates[3].append(QString::fromLatin1("QListView*,QListViewItem*,QString"));
    candidates[4].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QCheckListItem::Type"));
    candidates[3].append(QString::fromLatin1("QListViewItem*,QString,QPixmap*"));
    candidates[3].append(QString::fromLatin1("QListView*,QString,QPixmap*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QCheckListItem*,QString"))
      return new QCheckListItem(argValue<QCheckListItem *>(args[0]),
                                *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QCheckListItem*,QString,QCheckListItem::Type"))
      return new QCheckListItem(argValue<QCheckListItem *>(args[0]),
                                *(argValue<QString *>(args[1])),
                                static_cast<QCheckListItem::Type>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("QCheckListItem*,QListViewItem*,QString"))
      return new QCheckListItem(argValue<QCheckListItem *>(args[0]),
                                argValue<QListViewItem *>(args[1]),
                                *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QCheckListItem*,QListViewItem*,QString,QCheckListItem::Type"))
      return new QCheckListItem(argValue<QCheckListItem *>(args[0]),
                                argValue<QListViewItem *>(args[1]),
                                *(argValue<QString *>(args[2])),
                                static_cast<QCheckListItem::Type>(args[3].variant().toUInt()));
    if (sgt == QString::fromLatin1("QListViewItem*,QString"))
      return new QCheckListItem(argValue<QListViewItem *>(args[0]),
                                *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QListViewItem*,QString,QCheckListItem::Type"))
      return new QCheckListItem(argValue<QListViewItem *>(args[0]),
                                *(argValue<QString *>(args[1])),
                                static_cast<QCheckListItem::Type>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("QListViewItem*,QListViewItem*,QString"))
      return new QCheckListItem(argValue<QListViewItem *>(args[0]),
                                argValue<QListViewItem *>(args[1]),
                                *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QCheckListItem::Type"))
      return new QCheckListItem(argValue<QListViewItem *>(args[0]),
                                argValue<QListViewItem *>(args[1]),
                                *(argValue<QString *>(args[2])),
                                static_cast<QCheckListItem::Type>(args[3].variant().toUInt()));
    if (sgt == QString::fromLatin1("QListView*,QString"))
      return new QCheckListItem(argValue<QListView *>(args[0]),
                                *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QListView*,QString,QCheckListItem::Type"))
      return new QCheckListItem(argValue<QListView *>(args[0]),
                                *(argValue<QString *>(args[1])),
                                static_cast<QCheckListItem::Type>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("QListView*,QListViewItem*,QString"))
      return new QCheckListItem(argValue<QListView *>(args[0]),
                                argValue<QListViewItem *>(args[1]),
                                *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QListView*,QListViewItem*,QString,QCheckListItem::Type"))
      return new QCheckListItem(argValue<QListView *>(args[0]),
                                argValue<QListViewItem *>(args[1]),
                                *(argValue<QString *>(args[2])),
                                static_cast<QCheckListItem::Type>(args[3].variant().toUInt()));
    if (sgt == QString::fromLatin1("QListViewItem*,QString,QPixmap*"))
      return new QCheckListItem(argValue<QListViewItem *>(args[0]),
                                *(argValue<QString *>(args[1])),
                                *argValue<QPixmap *>(args[2]));
    if (sgt == QString::fromLatin1("QListView*,QString,QPixmap*"))
      return new QCheckListItem(argValue<QListView *>(args[0]),
                                *(argValue<QString *>(args[1])),
                                *argValue<QPixmap *>(args[2]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QCheckListItem*,QString"));
    candidates[3].append(QString::fromLatin1("QCheckListItem*,QString,QCheckListItem::Type"));
    candidates[3].append(QString::fromLatin1("QCheckListItem*,QListViewItem*,QString"));
    candidates[4].append(QString::fromLatin1("QCheckListItem*,QListViewItem*,QString,QCheckListItem::Type"));
    candidates[2].append(QString::fromLatin1("QListViewItem*,QString"));
    candidates[3].append(QString::fromLatin1("QListViewItem*,QString,QCheckListItem::Type"));
    candidates[3].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString"));
    candidates[4].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QCheckListItem::Type"));
    candidates[2].append(QString::fromLatin1("QListView*,QString"));
    candidates[3].append(QString::fromLatin1("QListView*,QString,QCheckListItem::Type"));
    candidates[3].append(QString::fromLatin1("QListView*,QListViewItem*,QString"));
    candidates[4].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QCheckListItem::Type"));
    candidates[3].append(QString::fromLatin1("QListViewItem*,QString,QPixmap*"));
    candidates[3].append(QString::fromLatin1("QListView*,QString,QPixmap*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSCheckListItem::setOn(bool arg0)
{
  AQ_CALL_VOID(setOn(arg0));
}
inline bool AQSCheckListItem::isOn() const
{
  AQ_CALL_RET_V(isOn(), bool);
}
inline uint AQSCheckListItem::type() const
{
  AQ_CALL_RET_V(type(), uint);
}
inline QString AQSCheckListItem::text() const
{
  AQ_CALL_RET_V(text(), QString);
}
inline void AQSCheckListItem::setTristate(bool arg0)
{
  AQ_CALL_VOID(setTristate(arg0));
}
inline bool AQSCheckListItem::isTristate() const
{
  AQ_CALL_RET_V(isTristate(), bool);
}
inline uint AQSCheckListItem::state() const
{
  AQ_CALL_RET_V(state(), uint);
}
inline void AQSCheckListItem::setState(uint arg0)
{
  AQ_CALL_VOID(setState(static_cast<QCheckListItem::ToggleState>(arg0)));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCHECKLISTITEM_P_H_ */
// @AQSVOIDPTR@
