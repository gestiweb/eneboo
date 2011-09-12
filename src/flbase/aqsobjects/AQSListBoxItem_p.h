/***************************************************************************
 AQSListBoxItem_p.h
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

#ifndef AQSLISTBOXITEM_P_H_
#define AQSLISTBOXITEM_P_H_

#include "AQSPixmap_p.h"
#include <qlistbox.h>

class AQSListBoxItem : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(ListBoxItem, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString text() const;
  const QPixmap *pixmap() const;
  int height(const QListBox *) const;
  int width(const QListBox *) const;
  bool isSelected() const;
  bool isCurrent() const;
  QListBox *listBox() const;
  void setSelectable(bool);
  bool isSelectable() const;
  QListBoxItem *next() const;
  QListBoxItem *prev() const;

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSListBoxItem::text() const
{
  AQ_CALL_RET_V(text(), QString);
}
inline const QPixmap *AQSListBoxItem::pixmap() const
{
  AQ_CALL_RET(pixmap());
}
inline int AQSListBoxItem::height(const QListBox *arg0) const
{
  AQ_CALL_RET_V(height(arg0), int);
}
inline int AQSListBoxItem::width(const QListBox *arg0) const
{
  AQ_CALL_RET_V(width(arg0), int);
}
inline bool AQSListBoxItem::isSelected() const
{
  AQ_CALL_RET_V(isSelected(), bool);
}
inline bool AQSListBoxItem::isCurrent() const
{
  AQ_CALL_RET_V(isCurrent(), bool);
}
inline QListBox *AQSListBoxItem::listBox() const
{
  AQ_CALL_RET(listBox());
}
inline void AQSListBoxItem::setSelectable(bool arg0)
{
  AQ_CALL_VOID(setSelectable(arg0));
}
inline bool AQSListBoxItem::isSelectable() const
{
  AQ_CALL_RET_V(isSelectable(), bool);
}
inline QListBoxItem *AQSListBoxItem::next() const
{
  AQ_CALL_RET(next());
}
inline QListBoxItem *AQSListBoxItem::prev() const
{
  AQ_CALL_RET(prev());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSLISTBOXITEM_P_H_ */
// @AQSVOIDPTR@
