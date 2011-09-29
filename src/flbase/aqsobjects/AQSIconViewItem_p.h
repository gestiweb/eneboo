/***************************************************************************
 AQSIconViewItem_p.h
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

#ifndef AQSICONVIEWITEM_P_H_
#define AQSICONVIEWITEM_P_H_

#include "AQSPixmap_p.h"
#include "AQSPicture_p.h"
#include <qiconview.h>

class AQSIconViewItem : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(IconViewItem, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setRenameEnabled(bool);
  void setDragEnabled(bool);
  void setDropEnabled(bool);
  QString text() const;
  QPixmap *pixmap() const;
  QPicture *picture() const;
  QString key() const;
  bool renameEnabled() const;
  bool dragEnabled() const;
  bool dropEnabled() const;
  QIconView *iconView() const;
  QIconViewItem *prevItem() const;
  QIconViewItem *nextItem() const;
  int index() const;
  void setSelected(bool, bool);
  void setSelected(bool);
  void setSelectable(bool);
  bool isSelected() const;
  bool isSelectable() const;
  void repaint();
  bool move(int, int);
  void moveBy(int, int);
  bool move(const QPoint &);
  void moveBy(const QPoint &);
  QRect rect() const;
  int x() const;
  int y() const;
  int width() const;
  int height() const;
  QSize size() const;
  QPoint pos() const;
  QRect textRect(bool = TRUE) const;
  QRect pixmapRect(bool = TRUE) const;
  bool contains(const QPoint &) const;
  bool intersects(const QRect &) const;
  int compare(QIconViewItem *) const;
  int compare(AQSIconViewItem *) const;
  void setText(const QString &);
  void setPixmap(QPixmap *);
  void setPixmap(AQSPixmap *);
  void setPicture(QPicture *);
  void setPicture(AQSPicture *);
  void setText(const QString &, bool, bool = TRUE);
  void setPixmap(QPixmap *, bool, bool = TRUE);
  void setPixmap(AQSPixmap *, bool, bool = TRUE);
  void setKey(const QString &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QIconView*"));
    candidates[2].append(QString::fromLatin1("QIconView*,QIconViewItem*"));
    candidates[2].append(QString::fromLatin1("QIconView*,QString"));
    candidates[3].append(QString::fromLatin1("QIconView*,QIconViewItem*,QString"));
    candidates[3].append(QString::fromLatin1("QIconView*,QString,QPixmap*"));
    candidates[4].append(QString::fromLatin1("QIconView*,QIconViewItem*,QString,QPixmap*"));
    candidates[3].append(QString::fromLatin1("QIconView*,QString,QPicture*"));
    candidates[4].append(QString::fromLatin1("QIconView*,QIconViewItem*,QString,QPicture*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QIconView*"))
      return new QIconViewItem(argValue<QIconView *>(args[0]));
    if (sgt == QString::fromLatin1("QIconView*,QIconViewItem*"))
      return new QIconViewItem(argValue<QIconView *>(args[0]),
                               argValue<QIconViewItem *>(args[1]));
    if (sgt == QString::fromLatin1("QIconView*,QString"))
      return new QIconViewItem(argValue<QIconView *>(args[0]),
                               *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QIconView*,QIconViewItem*,QString"))
      return new QIconViewItem(argValue<QIconView *>(args[0]),
                               argValue<QIconViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QIconView*,QString,QPixmap*"))
      return new QIconViewItem(argValue<QIconView *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *argValue<QPixmap *>(args[2]));
    if (sgt == QString::fromLatin1("QIconView*,QIconViewItem*,QString,QPixmap*"))
      return new QIconViewItem(argValue<QIconView *>(args[0]),
                               argValue<QIconViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *argValue<QPixmap *>(args[3]));
    if (sgt == QString::fromLatin1("QIconView*,QString,QPicture*"))
      return new QIconViewItem(argValue<QIconView *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               *argValue<QPicture *>(args[2]));
    if (sgt == QString::fromLatin1("QIconView*,QIconViewItem*,QString,QPicture*"))
      return new QIconViewItem(argValue<QIconView *>(args[0]),
                               argValue<QIconViewItem *>(args[1]),
                               *(argValue<QString *>(args[2])),
                               *argValue<QPicture *>(args[3]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QIconView*"));
    candidates[2].append(QString::fromLatin1("QIconView*,QIconViewItem*"));
    candidates[2].append(QString::fromLatin1("QIconView*,QString"));
    candidates[3].append(QString::fromLatin1("QIconView*,QIconViewItem*,QString"));
    candidates[3].append(QString::fromLatin1("QIconView*,QString,QPixmap*"));
    candidates[4].append(QString::fromLatin1("QIconView*,QIconViewItem*,QString,QPixmap*"));
    candidates[3].append(QString::fromLatin1("QIconView*,QString,QPicture*"));
    candidates[4].append(QString::fromLatin1("QIconView*,QIconViewItem*,QString,QPicture*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSIconViewItem::setRenameEnabled(bool arg0)
{
  AQ_CALL_VOID(setRenameEnabled(arg0));
}
inline void AQSIconViewItem::setDragEnabled(bool arg0)
{
  AQ_CALL_VOID(setDragEnabled(arg0));
}
inline void AQSIconViewItem::setDropEnabled(bool arg0)
{
  AQ_CALL_VOID(setDropEnabled(arg0));
}
inline QString AQSIconViewItem::text() const
{
  AQ_CALL_RET_V(text(), QString);
}
inline QPixmap *AQSIconViewItem::pixmap() const
{
  AQ_CALL_RET(pixmap());
}
inline QPicture *AQSIconViewItem::picture() const
{
  AQ_CALL_RET(picture());
}
inline QString AQSIconViewItem::key() const
{
  AQ_CALL_RET_V(key(), QString);
}
inline bool AQSIconViewItem::renameEnabled() const
{
  AQ_CALL_RET_V(renameEnabled(), bool);
}
inline bool AQSIconViewItem::dragEnabled() const
{
  AQ_CALL_RET_V(dragEnabled(), bool);
}
inline bool AQSIconViewItem::dropEnabled() const
{
  AQ_CALL_RET_V(dropEnabled(), bool);
}
inline QIconView *AQSIconViewItem::iconView() const
{
  AQ_CALL_RET(iconView());
}
inline QIconViewItem *AQSIconViewItem::prevItem() const
{
  AQ_CALL_RET(prevItem());
}
inline QIconViewItem *AQSIconViewItem::nextItem() const
{
  AQ_CALL_RET(nextItem());
}
inline int AQSIconViewItem::index() const
{
  AQ_CALL_RET_V(index(), int);
}
inline void AQSIconViewItem::setSelected(bool arg0,  bool arg1)
{
  AQ_CALL_VOID(setSelected(arg0, arg1));
}
inline void AQSIconViewItem::setSelected(bool arg0)
{
  AQ_CALL_VOID(setSelected(arg0));
}
inline void AQSIconViewItem::setSelectable(bool arg0)
{
  AQ_CALL_VOID(setSelectable(arg0));
}
inline bool AQSIconViewItem::isSelected() const
{
  AQ_CALL_RET_V(isSelected(), bool);
}
inline bool AQSIconViewItem::isSelectable() const
{
  AQ_CALL_RET_V(isSelectable(), bool);
}
inline void AQSIconViewItem::repaint()
{
  AQ_CALL_VOID(repaint());
}
inline bool AQSIconViewItem::move(int arg0,  int arg1)
{
  AQ_CALL_RET_V(move(arg0, arg1), bool);
}
inline void AQSIconViewItem::moveBy(int arg0,  int arg1)
{
  AQ_CALL_VOID(moveBy(arg0, arg1));
}
inline bool AQSIconViewItem::move(const QPoint &arg0)
{
  AQ_CALL_RET_V(move(arg0), bool);
}
inline void AQSIconViewItem::moveBy(const QPoint &arg0)
{
  AQ_CALL_VOID(moveBy(arg0));
}
inline QRect AQSIconViewItem::rect() const
{
  AQ_CALL_RET_V(rect(), QRect);
}
inline int AQSIconViewItem::x() const
{
  AQ_CALL_RET_V(x(), int);
}
inline int AQSIconViewItem::y() const
{
  AQ_CALL_RET_V(y(), int);
}
inline int AQSIconViewItem::width() const
{
  AQ_CALL_RET_V(width(), int);
}
inline int AQSIconViewItem::height() const
{
  AQ_CALL_RET_V(height(), int);
}
inline QSize AQSIconViewItem::size() const
{
  AQ_CALL_RET_V(size(), QSize);
}
inline QPoint AQSIconViewItem::pos() const
{
  AQ_CALL_RET_V(pos(), QPoint);
}
inline QRect AQSIconViewItem::textRect(bool arg0) const
{
  AQ_CALL_RET_V(textRect(arg0), QRect);
}
inline QRect AQSIconViewItem::pixmapRect(bool arg0) const
{
  AQ_CALL_RET_V(pixmapRect(arg0), QRect);
}
inline bool AQSIconViewItem::contains(const QPoint &arg0) const
{
  AQ_CALL_RET_V(contains(arg0), bool);
}
inline bool AQSIconViewItem::intersects(const QRect &arg0) const
{
  AQ_CALL_RET_V(intersects(arg0), bool);
}
inline int AQSIconViewItem::compare(QIconViewItem *arg0) const
{
  AQ_CALL_RET_V(compare(arg0), int);
}
inline int AQSIconViewItem::compare(AQSIconViewItem *arg0) const
{
  AQ_CALL_RET_V(compare(*arg0), int);
}
inline void AQSIconViewItem::setText(const QString &arg0)
{
  AQ_CALL_VOID(setText(arg0));
}
inline void AQSIconViewItem::setPixmap(QPixmap *arg0)
{
  AQ_CALL_VOID(setPixmap(*arg0));
}
inline void AQSIconViewItem::setPixmap(AQSPixmap *arg0)
{
  AQ_CALL_VOID(setPixmap(*arg0));
}
inline void AQSIconViewItem::setPicture(QPicture *arg0)
{
  AQ_CALL_VOID(setPicture(*arg0));
}
inline void AQSIconViewItem::setPicture(AQSPicture *arg0)
{
  AQ_CALL_VOID(setPicture(*arg0));
}
inline void AQSIconViewItem::setText(const QString &arg0,  bool arg1,  bool arg2)
{
  AQ_CALL_VOID(setText(arg0, arg1, arg2));
}
inline void AQSIconViewItem::setPixmap(QPixmap *arg0,  bool arg1,  bool arg2)
{
  AQ_CALL_VOID(setPixmap(*arg0, arg1, arg2));
}
inline void AQSIconViewItem::setPixmap(AQSPixmap *arg0,  bool arg1,  bool arg2)
{
  AQ_CALL_VOID(setPixmap(*arg0, arg1, arg2));
}
inline void AQSIconViewItem::setKey(const QString &arg0)
{
  AQ_CALL_VOID(setKey(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSICONVIEWITEM_P_H_ */

// @AQSVOIDPTR@

