/***************************************************************************
 AQSListViewItem_p.h
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

#ifndef AQSLISTVIEWITEM_P_H_
#define AQSLISTVIEWITEM_P_H_

#include "AQSFontMetrics_p.h"
#include "AQSPixmap_p.h"
#include <qlistview.h>

class AQSListViewItem : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(ListViewItem, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int height() const;
  void invalidateHeight();
  int totalHeight() const;
  int width(QFontMetrics *, const QListView *, int) const;
  int width(AQSFontMetrics *, const QListView *, int) const;
  void widthChanged(int = -1) const;
  int depth() const;
  void setText(int, const QString &);
  QString text(int) const;
  void setPixmap(int, QPixmap *);
  void setPixmap(int, AQSPixmap *);
  const QPixmap *pixmap(int) const;
  QString key(int, bool) const;
  int compare(QListViewItem *, int, bool) const;
  int compare(AQSListViewItem *, int, bool) const;
  void sortChildItems(int, bool);
  int childCount() const;
  bool isOpen() const;
  void setOpen(bool);
  void setup();
  void setSelected(bool);
  bool isSelected() const;
  QListViewItem *firstChild() const;
  QListViewItem *nextSibling() const;
  QListViewItem *parent() const;
  QListViewItem *itemAbove();
  QListViewItem *itemBelow();
  int itemPos() const;
  QListView *listView() const;
  void setSelectable(bool);
  bool isSelectable() const;
  void setExpandable(bool);
  bool isExpandable() const;
  void repaint() const;
  void sort();
  void moveItem(QListViewItem *);
  void moveItem(AQSListViewItem *);
  void setDragEnabled(bool);
  void setDropEnabled(bool);
  bool dragEnabled() const;
  bool dropEnabled() const;
  void setVisible(bool);
  bool isVisible() const;
  void setRenameEnabled(int, bool);
  bool renameEnabled(int) const;
  void startRename(int);
  void setEnabled(bool);
  bool isEnabled() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QListView*"));
    candidates[1].append(QString::fromLatin1("QListViewItem*"));
    candidates[2].append(QString::fromLatin1("QListView*,QListViewItem*"));
    candidates[2].append(QString::fromLatin1("QListViewItem*,QListViewItem*"));
    candidates[2].append(QString::fromLatin1("QListView*,QString"));
    candidates[3].append(QString::fromLatin1("QListView*,QString,QString"));
    candidates[4].append(QString::fromLatin1("QListView*,QString,QString,QString"));
    candidates[5].append(QString::fromLatin1("QListView*,QString,QString,QString,QString"));
    candidates[6].append(QString::fromLatin1("QListView*,QString,QString,QString,QString,QString"));
    candidates[7].append(QString::fromLatin1("QListView*,QString,QString,QString,QString,QString,QString"));
    candidates[8].append(QString::fromLatin1("QListView*,QString,QString,QString,QString,QString,QString,QString"));
    candidates[9].append(QString::fromLatin1("QListView*,QString,QString,QString,QString,QString,QString,QString,QString"));
    candidates[2].append(QString::fromLatin1("QListViewItem*,QString"));
    candidates[3].append(QString::fromLatin1("QListViewItem*,QString,QString"));
    candidates[4].append(QString::fromLatin1("QListViewItem*,QString,QString,QString"));
    candidates[5].append(QString::fromLatin1("QListViewItem*,QString,QString,QString,QString"));
    candidates[6].append(QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString"));
    candidates[7].append(QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString,QString"));
    candidates[8].append(QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString,QString,QString"));
    candidates[9].append(QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString,QString,QString,QString"));
    candidates[3].append(QString::fromLatin1("QListView*,QListViewItem*,QString"));
    candidates[4].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString"));
    candidates[5].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString"));
    candidates[6].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString"));
    candidates[7].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString"));
    candidates[8].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString,QString"));
    candidates[9].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString"));
    candidates[10].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString,QString"));
    candidates[3].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString"));
    candidates[4].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString"));
    candidates[5].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString"));
    candidates[6].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString"));
    candidates[7].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString"));
    candidates[8].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString,QString"));
    candidates[9].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString"));
    candidates[10].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QListView*"))
      return new QListViewItem(argValue<QListView *>(args[0]));
    if (sgt == QString::fromLatin1("QListViewItem*"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]));
    if (sgt == QString::fromLatin1("QListView*,QListViewItem*"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               argValue<QListViewItem *>(args[1]));
    if (sgt == QString::fromLatin1("QListViewItem*,QListViewItem*"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               argValue<QListViewItem *>(args[1]));
    if (sgt == QString::fromLatin1("QListView*,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QListView*,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QListView*,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])));
    if (sgt == QString::fromLatin1("QListView*,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])));
    if (sgt == QString::fromLatin1("QListView*,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])));
    if (sgt == QString::fromLatin1("QListView*,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])));
    if (sgt == QString::fromLatin1("QListView*,QString,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])),
                               *(argValue<QString *>(args[7])));
    if (sgt == QString::fromLatin1("QListView*,QString,QString,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])),
                               *(argValue<QString *>(args[7])),
                               *(argValue<QString *>(args[8])));
    if (sgt == QString::fromLatin1("QListViewItem*,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QListViewItem*,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QListViewItem*,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])));
    if (sgt == QString::fromLatin1("QListViewItem*,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])));
    if (sgt == QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])));
    if (sgt == QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])));
    if (sgt == QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])),
                               *(argValue<QString *>(args[7])));
    if (sgt == QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])),
                               *(argValue<QString *>(args[7])),
                               *(argValue<QString *>(args[8])));
    if (sgt == QString::fromLatin1("QListView*,QListViewItem*,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QListView*,QListViewItem*,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])));
    if (sgt == QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])));
    if (sgt == QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])));
    if (sgt == QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])));
    if (sgt == QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])),
                               *(argValue<QString *>(args[7])));
    if (sgt == QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])),
                               *(argValue<QString *>(args[7])),
                               *(argValue<QString *>(args[8])));
    if (sgt == QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListView *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])),
                               *(argValue<QString *>(args[7])),
                               *(argValue<QString *>(args[8])),
                               *(argValue<QString *>(args[9])));
    if (sgt == QString::fromLatin1("QListViewItem*,QListViewItem*,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])));
    if (sgt == QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])));
    if (sgt == QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])));
    if (sgt == QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])));
    if (sgt == QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])),
                               *(argValue<QString *>(args[7])));
    if (sgt == QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])),
                               *(argValue<QString *>(args[7])),
                               *(argValue<QString *>(args[8])));
    if (sgt == QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString,QString"))
      return new QListViewItem(argValue<QListViewItem *>(args[0]),
                               argValue<QListViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *(argValue<QString *>(args[3])),
                               *(argValue<QString *>(args[4])),
                               *(argValue<QString *>(args[5])),
                               *(argValue<QString *>(args[6])),
                               *(argValue<QString *>(args[7])),
                               *(argValue<QString *>(args[8])),
                               *(argValue<QString *>(args[9])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QListView*"));
    candidates[1].append(QString::fromLatin1("QListViewItem*"));
    candidates[2].append(QString::fromLatin1("QListView*,QListViewItem*"));
    candidates[2].append(QString::fromLatin1("QListViewItem*,QListViewItem*"));
    candidates[2].append(QString::fromLatin1("QListView*,QString"));
    candidates[3].append(QString::fromLatin1("QListView*,QString,QString"));
    candidates[4].append(QString::fromLatin1("QListView*,QString,QString,QString"));
    candidates[5].append(QString::fromLatin1("QListView*,QString,QString,QString,QString"));
    candidates[6].append(QString::fromLatin1("QListView*,QString,QString,QString,QString,QString"));
    candidates[7].append(QString::fromLatin1("QListView*,QString,QString,QString,QString,QString,QString"));
    candidates[8].append(QString::fromLatin1("QListView*,QString,QString,QString,QString,QString,QString,QString"));
    candidates[9].append(QString::fromLatin1("QListView*,QString,QString,QString,QString,QString,QString,QString,QString"));
    candidates[2].append(QString::fromLatin1("QListViewItem*,QString"));
    candidates[3].append(QString::fromLatin1("QListViewItem*,QString,QString"));
    candidates[4].append(QString::fromLatin1("QListViewItem*,QString,QString,QString"));
    candidates[5].append(QString::fromLatin1("QListViewItem*,QString,QString,QString,QString"));
    candidates[6].append(QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString"));
    candidates[7].append(QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString,QString"));
    candidates[8].append(QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString,QString,QString"));
    candidates[9].append(QString::fromLatin1("QListViewItem*,QString,QString,QString,QString,QString,QString,QString,QString"));
    candidates[3].append(QString::fromLatin1("QListView*,QListViewItem*,QString"));
    candidates[4].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString"));
    candidates[5].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString"));
    candidates[6].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString"));
    candidates[7].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString"));
    candidates[8].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString,QString"));
    candidates[9].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString"));
    candidates[10].append(QString::fromLatin1("QListView*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString,QString"));
    candidates[3].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString"));
    candidates[4].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString"));
    candidates[5].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString"));
    candidates[6].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString"));
    candidates[7].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString"));
    candidates[8].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString,QString"));
    candidates[9].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString"));
    candidates[10].append(QString::fromLatin1("QListViewItem*,QListViewItem*,QString,QString,QString,QString,QString,QString,QString,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSListViewItem::height() const
{
  AQ_CALL_RET_V(height(), int);
}
inline void AQSListViewItem::invalidateHeight()
{
  AQ_CALL_VOID(invalidateHeight());
}
inline int AQSListViewItem::totalHeight() const
{
  AQ_CALL_RET_V(totalHeight(), int);
}
inline int AQSListViewItem::width(QFontMetrics *arg0,  const QListView *arg1,  int arg2) const
{
  AQ_CALL_RET_V(width(*arg0, arg1, arg2), int);
}
inline int AQSListViewItem::width(AQSFontMetrics *arg0,  const QListView *arg1,  int arg2) const
{
  AQ_CALL_RET_V(width(*arg0, arg1, arg2), int);
}
inline void AQSListViewItem::widthChanged(int arg0) const
{
  AQ_CALL_VOID(widthChanged(arg0));
}
inline int AQSListViewItem::depth() const
{
  AQ_CALL_RET_V(depth(), int);
}
inline void AQSListViewItem::setText(int arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setText(arg0, arg1));
}
inline QString AQSListViewItem::text(int arg0) const
{
  AQ_CALL_RET_V(text(arg0), QString);
}
inline void AQSListViewItem::setPixmap(int arg0,  QPixmap *arg1)
{
  AQ_CALL_VOID(setPixmap(arg0, *arg1));
}
inline void AQSListViewItem::setPixmap(int arg0, AQSPixmap *arg1)
{
  AQ_CALL_VOID(setPixmap(arg0, *arg1));
}
inline const QPixmap *AQSListViewItem::pixmap(int arg0) const
{
  AQ_CALL_RET(pixmap(arg0));
}
inline QString AQSListViewItem::key(int arg0,  bool arg1) const
{
  AQ_CALL_RET_V(key(arg0, arg1), QString);
}
inline int AQSListViewItem::compare(QListViewItem *arg0,  int arg1,  bool arg2) const
{
  AQ_CALL_RET_V(compare(arg0, arg1, arg2), int);
}
inline int AQSListViewItem::compare(AQSListViewItem *arg0,  int arg1,  bool arg2) const
{
  AQ_CALL_RET_V(compare(*arg0, arg1, arg2), int);
}
inline void AQSListViewItem::sortChildItems(int arg0,  bool arg1)
{
  AQ_CALL_VOID(sortChildItems(arg0, arg1));
}
inline int AQSListViewItem::childCount() const
{
  AQ_CALL_RET_V(childCount(), int);
}
inline bool AQSListViewItem::isOpen() const
{
  AQ_CALL_RET_V(isOpen(), bool);
}
inline void AQSListViewItem::setOpen(bool arg0)
{
  AQ_CALL_VOID(setOpen(arg0));
}
inline void AQSListViewItem::setup()
{
  AQ_CALL_VOID(setup());
}
inline void AQSListViewItem::setSelected(bool arg0)
{
  AQ_CALL_VOID(setSelected(arg0));
}
inline bool AQSListViewItem::isSelected() const
{
  AQ_CALL_RET_V(isSelected(), bool);
}
inline QListViewItem *AQSListViewItem::firstChild() const
{
  AQ_CALL_RET(firstChild());
}
inline QListViewItem *AQSListViewItem::nextSibling() const
{
  AQ_CALL_RET(nextSibling());
}
inline QListViewItem *AQSListViewItem::parent() const
{
  AQ_CALL_RET(parent());
}
inline QListViewItem *AQSListViewItem::itemAbove()
{
  AQ_CALL_RET(itemAbove());
}
inline QListViewItem *AQSListViewItem::itemBelow()
{
  AQ_CALL_RET(itemBelow());
}
inline int AQSListViewItem::itemPos() const
{
  AQ_CALL_RET_V(itemPos(), int);
}
inline QListView *AQSListViewItem::listView() const
{
  AQ_CALL_RET(listView());
}
inline void AQSListViewItem::setSelectable(bool arg0)
{
  AQ_CALL_VOID(setSelectable(arg0));
}
inline bool AQSListViewItem::isSelectable() const
{
  AQ_CALL_RET_V(isSelectable(), bool);
}
inline void AQSListViewItem::setExpandable(bool arg0)
{
  AQ_CALL_VOID(setExpandable(arg0));
}
inline bool AQSListViewItem::isExpandable() const
{
  AQ_CALL_RET_V(isExpandable(), bool);
}
inline void AQSListViewItem::repaint() const
{
  AQ_CALL_VOID(repaint());
}
inline void AQSListViewItem::sort()
{
  AQ_CALL_VOID(sort());
}
inline void AQSListViewItem::moveItem(QListViewItem *arg0)
{
  AQ_CALL_VOID(moveItem(arg0));
}
inline void AQSListViewItem::moveItem(AQSListViewItem *arg0)
{
  AQ_CALL_VOID(moveItem(*arg0));
}
inline void AQSListViewItem::setDragEnabled(bool arg0)
{
  AQ_CALL_VOID(setDragEnabled(arg0));
}
inline void AQSListViewItem::setDropEnabled(bool arg0)
{
  AQ_CALL_VOID(setDropEnabled(arg0));
}
inline bool AQSListViewItem::dragEnabled() const
{
  AQ_CALL_RET_V(dragEnabled(), bool);
}
inline bool AQSListViewItem::dropEnabled() const
{
  AQ_CALL_RET_V(dropEnabled(), bool);
}
inline void AQSListViewItem::setVisible(bool arg0)
{
  AQ_CALL_VOID(setVisible(arg0));
}
inline bool AQSListViewItem::isVisible() const
{
  AQ_CALL_RET_V(isVisible(), bool);
}
inline void AQSListViewItem::setRenameEnabled(int arg0,  bool arg1)
{
  AQ_CALL_VOID(setRenameEnabled(arg0, arg1));
}
inline bool AQSListViewItem::renameEnabled(int arg0) const
{
  AQ_CALL_RET_V(renameEnabled(arg0), bool);
}
inline void AQSListViewItem::startRename(int arg0)
{
  AQ_CALL_VOID(startRename(arg0));
}
inline void AQSListViewItem::setEnabled(bool arg0)
{
  AQ_CALL_VOID(setEnabled(arg0));
}
inline bool AQSListViewItem::isEnabled() const
{
  AQ_CALL_RET_V(isEnabled(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSLISTVIEWITEM_P_H_ */
// @AQSVOIDPTR@
