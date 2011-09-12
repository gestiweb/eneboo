/***************************************************************************
 AQSListView_p.h
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

#ifndef AQSLISTVIEW_P_H_
#define AQSLISTVIEW_P_H_

#include "AQSScrollView_p.h"
#include "AQSListViewItem_p.h"
#include "AQSIconSet_p.h"
#include <qlistview.h>

class AQSListView : public AQSScrollView
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ListView, ScrollView);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QHeader *header() const;
  int addColumn(const QString &, int = -1);
  int addColumn(QIconSet *, const QString &, int = -1);
  int addColumn(AQSIconSet *, const QString &, int = -1);
  void removeColumn(int);
  void setColumnText(int, const QString &);
  void setColumnText(int, QIconSet *, const QString &);
  void setColumnText(int, AQSIconSet *, const QString &);
  QString columnText(int) const;
  void setColumnWidth(int, int);
  int columnWidth(int) const;
  void setColumnWidthMode(int, uint);
  uint columnWidthMode(int) const;
  void setColumnAlignment(int, int);
  int columnAlignment(int) const;
  QListViewItem *itemAt(const QPoint &) const;
  QRect itemRect(const QListViewItem *) const;
  QRect itemRect(AQSListViewItem *) const;
  int itemPos(const QListViewItem *);
  int itemPos(AQSListViewItem *);
  void ensureItemVisible(const QListViewItem *);
  void ensureItemVisible(AQSListViewItem *);
  void repaintItem(const QListViewItem *) const;
  void repaintItem(AQSListViewItem *) const;
  void clearSelection();
  void setSelected(QListViewItem *, bool);
  void setSelected(AQSListViewItem *, bool);
  void setSelectionAnchor(QListViewItem *);
  void setSelectionAnchor(AQSListViewItem *);
  bool isSelected(const QListViewItem *) const;
  bool isSelected(AQSListViewItem *) const;
  QListViewItem *selectedItem() const;
  void setOpen(QListViewItem *, bool);
  void setOpen(AQSListViewItem *, bool);
  bool isOpen(const QListViewItem *) const;
  bool isOpen(AQSListViewItem *) const;
  void setCurrentItem(QListViewItem *);
  void setCurrentItem(AQSListViewItem *);
  QListViewItem *currentItem() const;
  QListViewItem *firstChild() const;
  QListViewItem *lastItem() const;
  void setSorting(int, bool = TRUE);
  int sortColumn() const;
  void setSortColumn(int);
  uint sortOrder() const;
  void setSortOrder(uint);
  void sort();
  bool isRenaming() const;
  void hideColumn(int);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QListView;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QListView(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QListView(argValue<QWidget *>(args[0]),
                           *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QListView(argValue<QWidget *>(args[0]),
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
inline QHeader *AQSListView::header() const
{
  AQ_CALL_RET(header());
}
inline int AQSListView::addColumn(const QString &arg0,  int arg1)
{
  AQ_CALL_RET_V(addColumn(arg0, arg1), int);
}
inline int AQSListView::addColumn(QIconSet *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_RET_V(addColumn(*arg0, arg1, arg2), int);
}
inline int AQSListView::addColumn(AQSIconSet *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_RET_V(addColumn(*arg0, arg1, arg2), int);
}
inline void AQSListView::removeColumn(int arg0)
{
  AQ_CALL_VOID(removeColumn(arg0));
}
inline void AQSListView::setColumnText(int arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setColumnText(arg0, arg1));
}
inline void AQSListView::setColumnText(int arg0,  QIconSet *arg1,  const QString &arg2)
{
  AQ_CALL_VOID(setColumnText(arg0, *arg1, arg2));
}
inline void AQSListView::setColumnText(int arg0, AQSIconSet *arg1,  const QString &arg2)
{
  AQ_CALL_VOID(setColumnText(arg0, *arg1, arg2));
}
inline QString AQSListView::columnText(int arg0) const
{
  AQ_CALL_RET_V(columnText(arg0), QString);
}
inline void AQSListView::setColumnWidth(int arg0,  int arg1)
{
  AQ_CALL_VOID(setColumnWidth(arg0, arg1));
}
inline int AQSListView::columnWidth(int arg0) const
{
  AQ_CALL_RET_V(columnWidth(arg0), int);
}
inline void AQSListView::setColumnWidthMode(int arg0,  uint arg1)
{
  AQ_CALL_VOID(setColumnWidthMode(arg0, static_cast<QListView::WidthMode>(arg1)));
}
inline uint AQSListView::columnWidthMode(int arg0) const
{
  AQ_CALL_RET_V(columnWidthMode(arg0), uint);
}
inline void AQSListView::setColumnAlignment(int arg0,  int arg1)
{
  AQ_CALL_VOID(setColumnAlignment(arg0, arg1));
}
inline int AQSListView::columnAlignment(int arg0) const
{
  AQ_CALL_RET_V(columnAlignment(arg0), int);
}
inline QListViewItem *AQSListView::itemAt(const QPoint &arg0) const
{
  AQ_CALL_RET(itemAt(arg0));
}
inline QRect AQSListView::itemRect(const QListViewItem *arg0) const
{
  AQ_CALL_RET_V(itemRect(arg0), QRect);
}
inline QRect AQSListView::itemRect(AQSListViewItem *arg0) const
{
  AQ_CALL_RET_V(itemRect(*arg0), QRect);
}
inline int AQSListView::itemPos(const QListViewItem *arg0)
{
  AQ_CALL_RET_V(itemPos(arg0), int);
}
inline int AQSListView::itemPos(AQSListViewItem *arg0)
{
  AQ_CALL_RET_V(itemPos(*arg0), int);
}
inline void AQSListView::ensureItemVisible(const QListViewItem *arg0)
{
  AQ_CALL_VOID(ensureItemVisible(arg0));
}
inline void AQSListView::ensureItemVisible(AQSListViewItem *arg0)
{
  AQ_CALL_VOID(ensureItemVisible(*arg0));
}
inline void AQSListView::repaintItem(const QListViewItem *arg0) const
{
  AQ_CALL_VOID(repaintItem(arg0));
}
inline void AQSListView::repaintItem(AQSListViewItem *arg0) const
{
  AQ_CALL_VOID(repaintItem(*arg0));
}
inline void AQSListView::clearSelection()
{
  AQ_CALL_VOID(clearSelection());
}
inline void AQSListView::setSelected(QListViewItem *arg0,  bool arg1)
{
  AQ_CALL_VOID(setSelected(arg0, arg1));
}
inline void AQSListView::setSelected(AQSListViewItem *arg0,  bool arg1)
{
  AQ_CALL_VOID(setSelected(*arg0, arg1));
}
inline void AQSListView::setSelectionAnchor(QListViewItem *arg0)
{
  AQ_CALL_VOID(setSelectionAnchor(arg0));
}
inline void AQSListView::setSelectionAnchor(AQSListViewItem *arg0)
{
  AQ_CALL_VOID(setSelectionAnchor(*arg0));
}
inline bool AQSListView::isSelected(const QListViewItem *arg0) const
{
  AQ_CALL_RET_V(isSelected(arg0), bool);
}
inline bool AQSListView::isSelected(AQSListViewItem *arg0) const
{
  AQ_CALL_RET_V(isSelected(*arg0), bool);
}
inline QListViewItem *AQSListView::selectedItem() const
{
  AQ_CALL_RET(selectedItem());
}
inline void AQSListView::setOpen(QListViewItem *arg0,  bool arg1)
{
  AQ_CALL_VOID(setOpen(arg0, arg1));
}
inline void AQSListView::setOpen(AQSListViewItem *arg0,  bool arg1)
{
  AQ_CALL_VOID(setOpen(*arg0, arg1));
}
inline bool AQSListView::isOpen(const QListViewItem *arg0) const
{
  AQ_CALL_RET_V(isOpen(arg0), bool);
}
inline bool AQSListView::isOpen(AQSListViewItem *arg0) const
{
  AQ_CALL_RET_V(isOpen(*arg0), bool);
}
inline void AQSListView::setCurrentItem(QListViewItem *arg0)
{
  AQ_CALL_VOID(setCurrentItem(arg0));
}
inline void AQSListView::setCurrentItem(AQSListViewItem *arg0)
{
  AQ_CALL_VOID(setCurrentItem(*arg0));
}
inline QListViewItem *AQSListView::currentItem() const
{
  AQ_CALL_RET(currentItem());
}
inline QListViewItem *AQSListView::firstChild() const
{
  AQ_CALL_RET(firstChild());
}
inline QListViewItem *AQSListView::lastItem() const
{
  AQ_CALL_RET(lastItem());
}
inline void AQSListView::setSorting(int arg0,  bool arg1)
{
  AQ_CALL_VOID(setSorting(arg0, arg1));
}
inline int AQSListView::sortColumn() const
{
  AQ_CALL_RET_V(sortColumn(), int);
}
inline void AQSListView::setSortColumn(int arg0)
{
  AQ_CALL_VOID(setSortColumn(arg0));
}
inline uint AQSListView::sortOrder() const
{
  AQ_CALL_RET_V(sortOrder(), uint);
}
inline void AQSListView::setSortOrder(uint arg0)
{
  AQ_CALL_VOID(setSortOrder(static_cast<Qt::SortOrder>(arg0)));
}
inline void AQSListView::sort()
{
  AQ_CALL_VOID(sort());
}
inline bool AQSListView::isRenaming() const
{
  AQ_CALL_RET_V(isRenaming(), bool);
}
inline void AQSListView::hideColumn(int arg0)
{
  AQ_CALL_VOID(hideColumn(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSLISTVIEW_P_H_ */
// @AQSWIDGET@
