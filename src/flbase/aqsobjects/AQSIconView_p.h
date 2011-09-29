/***************************************************************************
 AQSIconView_p.h
 -------------------
 begin                : 29/01/2011
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

#ifndef AQSICONVIEW_P_H_
#define AQSICONVIEW_P_H_

#include "AQSScrollView_p.h"
#include "AQSIconViewItem_p.h"
#include <qiconview.h>

class AQSIconView : public AQSScrollView
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(IconView, ScrollView);

public slots:
  void removeCurrentItem() {
    if (!o_)
      return;
    delete o_->currentItem();
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int index(const QIconViewItem *) const;
  int index(AQSIconViewItem *) const;
  QIconViewItem *firstItem() const;
  QIconViewItem *lastItem() const;
  QIconViewItem *currentItem() const;
  void setCurrentItem(QIconViewItem *);
  void setCurrentItem(AQSIconViewItem *);
  void setSelected(QIconViewItem *, bool, bool = FALSE);
  void setSelected(AQSIconViewItem *, bool, bool = FALSE);
  QIconViewItem *findItem(const QPoint &) const;
  QIconViewItem *findItem(const QString &, uint = BeginsWith) const;
  void selectAll(bool);
  void clearSelection();
  void invertSelection();
  void repaintItem(QIconViewItem *);
  void repaintItem(AQSIconViewItem *);
  void repaintSelectedItems();
  void ensureItemVisible(QIconViewItem *);
  void ensureItemVisible(AQSIconViewItem *);
  QIconViewItem *findFirstVisibleItem(const QRect &) const;
  QIconViewItem *findLastVisibleItem(const QRect &) const;
  void clear();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QIconView;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QIconView(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QIconView(argValue<QWidget *>(args[0]),
                           *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QIconView(argValue<QWidget *>(args[0]),
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
inline int AQSIconView::index(const QIconViewItem *arg0) const
{
  AQ_CALL_RET_V(index(arg0), int);
}
inline int AQSIconView::index(AQSIconViewItem *arg0) const
{
  AQ_CALL_RET_V(index(*arg0), int);
}
inline QIconViewItem *AQSIconView::firstItem() const
{
  AQ_CALL_RET(firstItem());
}
inline QIconViewItem *AQSIconView::lastItem() const
{
  AQ_CALL_RET(lastItem());
}
inline QIconViewItem *AQSIconView::currentItem() const
{
  AQ_CALL_RET(currentItem());
}
inline void AQSIconView::setCurrentItem(QIconViewItem *arg0)
{
  AQ_CALL_VOID(setCurrentItem(arg0));
}
inline void AQSIconView::setCurrentItem(AQSIconViewItem *arg0)
{
  AQ_CALL_VOID(setCurrentItem(*arg0));
}
inline void AQSIconView::setSelected(QIconViewItem *arg0,  bool arg1,  bool arg2)
{
  AQ_CALL_VOID(setSelected(arg0, arg1, arg2));
}
inline void AQSIconView::setSelected(AQSIconViewItem *arg0,  bool arg1,  bool arg2)
{
  AQ_CALL_VOID(setSelected(*arg0, arg1, arg2));
}
inline QIconViewItem *AQSIconView::findItem(const QPoint &arg0) const
{
  AQ_CALL_RET(findItem(arg0));
}
inline QIconViewItem *AQSIconView::findItem(const QString &arg0,  uint arg1) const
{
  AQ_CALL_RET(findItem(arg0, arg1));
}
inline void AQSIconView::selectAll(bool arg0)
{
  AQ_CALL_VOID(selectAll(arg0));
}
inline void AQSIconView::clearSelection()
{
  AQ_CALL_VOID(clearSelection());
}
inline void AQSIconView::invertSelection()
{
  AQ_CALL_VOID(invertSelection());
}
inline void AQSIconView::repaintItem(QIconViewItem *arg0)
{
  AQ_CALL_VOID(repaintItem(arg0));
}
inline void AQSIconView::repaintItem(AQSIconViewItem *arg0)
{
  AQ_CALL_VOID(repaintItem(*arg0));
}
inline void AQSIconView::repaintSelectedItems()
{
  AQ_CALL_VOID(repaintSelectedItems());
}
inline void AQSIconView::ensureItemVisible(QIconViewItem *arg0)
{
  AQ_CALL_VOID(ensureItemVisible(arg0));
}
inline void AQSIconView::ensureItemVisible(AQSIconViewItem *arg0)
{
  AQ_CALL_VOID(ensureItemVisible(*arg0));
}
inline QIconViewItem *AQSIconView::findFirstVisibleItem(const QRect &arg0) const
{
  AQ_CALL_RET(findFirstVisibleItem(arg0));
}
inline QIconViewItem *AQSIconView::findLastVisibleItem(const QRect &arg0) const
{
  AQ_CALL_RET(findLastVisibleItem(arg0));
}
inline void AQSIconView::clear()
{
  AQ_CALL_VOID(clear());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSICONVIEW_P_H_ */

// @AQSWIDGET@
