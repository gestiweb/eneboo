/***************************************************************************
 AQSListBox_p.h
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

#ifndef AQSLISTBOX_P_H_
#define AQSLISTBOX_P_H_

#include "AQSScrollView_p.h"
#include "AQSListBoxItem_p.h"
#include <qlistbox.h>

class AQSListBox : public AQSScrollView
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ListBox, ScrollView);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void insertStringList(const QStringList &, int = -1);
  void insertItem(const QListBoxItem *, int = -1);
  void insertItem(AQSListBoxItem *, int = -1);
  void insertItem(const QListBoxItem *, const QListBoxItem *);
  void insertItem(const QListBoxItem *, AQSListBoxItem *);
  void insertItem(AQSListBoxItem *, const QListBoxItem *);
  void insertItem(AQSListBoxItem *, AQSListBoxItem *);
  void insertItem(const QString &, int = -1);
  void insertItem(QPixmap *, int = -1);
  void insertItem(AQSPixmap *, int = -1);
  void insertItem(QPixmap *, const QString &, int = -1);
  void insertItem(AQSPixmap *, const QString &, int = -1);
  void removeItem(int);
  QString text(int) const;
  const QPixmap *pixmap(int) const;
  void changeItem(const QListBoxItem *, int);
  void changeItem(AQSListBoxItem *, int);
  void changeItem(const QString &, int);
  void changeItem(QPixmap *, int);
  void changeItem(AQSPixmap *, int);
  void changeItem(QPixmap *, const QString &, int);
  void changeItem(AQSPixmap *, const QString &, int);
  void setSelected(int, bool);
  bool isSelected(int) const;
  bool isSelected(const QListBoxItem *) const;
  bool isSelected(AQSListBoxItem *) const;
  QListBoxItem *selectedItem() const;
  QListBoxItem *item(int) const;
  int index(const QListBoxItem *) const;
  int index(AQSListBoxItem *) const;
  QListBoxItem *findItem(const QString &, uint = BeginsWith) const;
  void triggerUpdate(bool);
  bool itemVisible(int);
  bool itemVisible(const QListBoxItem *);
  bool itemVisible(AQSListBoxItem *);
  int itemHeight(int = 0) const;
  QListBoxItem *itemAt(const QPoint &) const;
  QRect itemRect(QListBoxItem *) const;
  QRect itemRect(AQSListBoxItem *) const;
  QListBoxItem *firstItem() const;
  void sort(bool = TRUE);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QListBox;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QListBox(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QListBox(argValue<QWidget *>(args[0]),
                          *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QListBox(argValue<QWidget *>(args[0]),
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
inline void AQSListBox::insertStringList(const QStringList &arg0,  int arg1)
{
  AQ_CALL_VOID(insertStringList(arg0, arg1));
}
inline void AQSListBox::insertItem(const QListBoxItem *arg0,  int arg1)
{
  AQ_CALL_VOID(insertItem(arg0, arg1));
}
inline void AQSListBox::insertItem(AQSListBoxItem *arg0,  int arg1)
{
  AQ_CALL_VOID(insertItem(*arg0, arg1));
}
inline void AQSListBox::insertItem(const QListBoxItem *arg0,  const QListBoxItem *arg1)
{
  AQ_CALL_VOID(insertItem(arg0, arg1));
}
inline void AQSListBox::insertItem(const QListBoxItem *arg0, AQSListBoxItem *arg1)
{
  AQ_CALL_VOID(insertItem(arg0, *arg1));
}
inline void AQSListBox::insertItem(AQSListBoxItem *arg0,  const QListBoxItem *arg1)
{
  AQ_CALL_VOID(insertItem(*arg0, arg1));
}
inline void AQSListBox::insertItem(AQSListBoxItem *arg0, AQSListBoxItem *arg1)
{
  AQ_CALL_VOID(insertItem(*arg0, *arg1));
}
inline void AQSListBox::insertItem(const QString &arg0,  int arg1)
{
  AQ_CALL_VOID(insertItem(arg0, arg1));
}
inline void AQSListBox::insertItem(QPixmap *arg0,  int arg1)
{
  AQ_CALL_VOID(insertItem(*arg0, arg1));
}
inline void AQSListBox::insertItem(AQSPixmap *arg0,  int arg1)
{
  AQ_CALL_VOID(insertItem(*arg0, arg1));
}
inline void AQSListBox::insertItem(QPixmap *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(insertItem(*arg0, arg1, arg2));
}
inline void AQSListBox::insertItem(AQSPixmap *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(insertItem(*arg0, arg1, arg2));
}
inline void AQSListBox::removeItem(int arg0)
{
  AQ_CALL_VOID(removeItem(arg0));
}
inline QString AQSListBox::text(int arg0) const
{
  AQ_CALL_RET_V(text(arg0), QString);
}
inline const QPixmap *AQSListBox::pixmap(int arg0) const
{
  AQ_CALL_RET(pixmap(arg0));
}
inline void AQSListBox::changeItem(const QListBoxItem *arg0,  int arg1)
{
  AQ_CALL_VOID(changeItem(arg0, arg1));
}
inline void AQSListBox::changeItem(AQSListBoxItem *arg0,  int arg1)
{
  AQ_CALL_VOID(changeItem(*arg0, arg1));
}
inline void AQSListBox::changeItem(const QString &arg0,  int arg1)
{
  AQ_CALL_VOID(changeItem(arg0, arg1));
}
inline void AQSListBox::changeItem(QPixmap *arg0,  int arg1)
{
  AQ_CALL_VOID(changeItem(*arg0, arg1));
}
inline void AQSListBox::changeItem(AQSPixmap *arg0,  int arg1)
{
  AQ_CALL_VOID(changeItem(*arg0, arg1));
}
inline void AQSListBox::changeItem(QPixmap *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(changeItem(*arg0, arg1, arg2));
}
inline void AQSListBox::changeItem(AQSPixmap *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(changeItem(*arg0, arg1, arg2));
}
inline void AQSListBox::setSelected(int arg0,  bool arg1)
{
  AQ_CALL_VOID(setSelected(arg0, arg1));
}
inline bool AQSListBox::isSelected(int arg0) const
{
  AQ_CALL_RET_V(isSelected(arg0), bool);
}
inline bool AQSListBox::isSelected(const QListBoxItem *arg0) const
{
  AQ_CALL_RET_V(isSelected(arg0), bool);
}
inline bool AQSListBox::isSelected(AQSListBoxItem *arg0) const
{
  AQ_CALL_RET_V(isSelected(*arg0), bool);
}
inline QListBoxItem *AQSListBox::selectedItem() const
{
  AQ_CALL_RET(selectedItem());
}
inline QListBoxItem *AQSListBox::item(int arg0) const
{
  AQ_CALL_RET(item(arg0));
}
inline int AQSListBox::index(const QListBoxItem *arg0) const
{
  AQ_CALL_RET_V(index(arg0), int);
}
inline int AQSListBox::index(AQSListBoxItem *arg0) const
{
  AQ_CALL_RET_V(index(*arg0), int);
}
inline QListBoxItem *AQSListBox::findItem(const QString &arg0,  uint arg1) const
{
  AQ_CALL_RET(findItem(arg0, arg1));
}
inline void AQSListBox::triggerUpdate(bool arg0)
{
  AQ_CALL_VOID(triggerUpdate(arg0));
}
inline bool AQSListBox::itemVisible(int arg0)
{
  AQ_CALL_RET_V(itemVisible(arg0), bool);
}
inline bool AQSListBox::itemVisible(const QListBoxItem *arg0)
{
  AQ_CALL_RET_V(itemVisible(arg0), bool);
}
inline bool AQSListBox::itemVisible(AQSListBoxItem *arg0)
{
  AQ_CALL_RET_V(itemVisible(*arg0), bool);
}
inline int AQSListBox::itemHeight(int arg0) const
{
  AQ_CALL_RET_V(itemHeight(arg0), int);
}
inline QListBoxItem *AQSListBox::itemAt(const QPoint &arg0) const
{
  AQ_CALL_RET(itemAt(arg0));
}
inline QRect AQSListBox::itemRect(QListBoxItem *arg0) const
{
  AQ_CALL_RET_V(itemRect(arg0), QRect);
}
inline QRect AQSListBox::itemRect(AQSListBoxItem *arg0) const
{
  AQ_CALL_RET_V(itemRect(*arg0), QRect);
}
inline QListBoxItem *AQSListBox::firstItem() const
{
  AQ_CALL_RET(firstItem());
}
inline void AQSListBox::sort(bool arg0)
{
  AQ_CALL_VOID(sort(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSLISTBOX_P_H_ */
// @AQSWIDGET@
