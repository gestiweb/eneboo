/***************************************************************************
 AQSTableItem_p.h
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

#ifndef AQSTABLEITEM_P_H_
#define AQSTABLEITEM_P_H_

#include "AQSPixmap_p.h"
#include <qtable.h>

class AQSTableItem : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(TableItem, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QPixmap *pixmap() const;
  QString text() const;
  void setPixmap(QPixmap *);
  void setPixmap(AQSPixmap *);
  void setText(const QString &);
  QTable *table() const;
  int alignment() const;
  void setWordWrap(bool);
  bool wordWrap() const;
  uint editType() const;
  QWidget *createEditor() const;
  void setContentFromEditor(QWidget *);
  void setReplaceable(bool);
  bool isReplaceable() const;
  QString key() const;
  QSize sizeHint() const;
  void setSpan(int, int);
  int rowSpan() const;
  int colSpan() const;
  void setRow(int);
  void setCol(int);
  int row() const;
  int col() const;
  void setEnabled(bool);
  bool isEnabled() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QTable*,QTableItem::EditType"));
    candidates[3].append(QString::fromLatin1("QTable*,QTableItem::EditType,QString"));
    candidates[4].append(QString::fromLatin1("QTable*,QTableItem::EditType,QString,QPixmap*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QTable*,QTableItem::EditType"))
      return new QTableItem(argValue<QTable *>(args[0]),
                            static_cast<QTableItem::EditType>(args[1].variant().toUInt()));
    if (sgt == QString::fromLatin1("QTable*,QTableItem::EditType,QString"))
      return new QTableItem(argValue<QTable *>(args[0]),
                            static_cast<QTableItem::EditType>(args[1].variant().toUInt()),
                            *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QTable*,QTableItem::EditType,QString,QPixmap*"))
      return new QTableItem(argValue<QTable *>(args[0]),
                            static_cast<QTableItem::EditType>(args[1].variant().toUInt()),
                            *(argValue<QString *>(args[2])),
                            *argValue<QPixmap *>(args[3]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QTable*,QTableItem::EditType"));
    candidates[3].append(QString::fromLatin1("QTable*,QTableItem::EditType,QString"));
    candidates[4].append(QString::fromLatin1("QTable*,QTableItem::EditType,QString,QPixmap*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QPixmap *AQSTableItem::pixmap() const
{
  AQ_CALL_RET_PTR(pixmap(), QPixmap);
}
inline QString AQSTableItem::text() const
{
  AQ_CALL_RET_V(text(), QString);
}
inline void AQSTableItem::setPixmap(QPixmap *arg0)
{
  AQ_CALL_VOID(setPixmap(*arg0));
}
inline void AQSTableItem::setPixmap(AQSPixmap *arg0)
{
  AQ_CALL_VOID(setPixmap(*arg0));
}
inline void AQSTableItem::setText(const QString &arg0)
{
  AQ_CALL_VOID(setText(arg0));
}
inline QTable *AQSTableItem::table() const
{
  AQ_CALL_RET(table());
}
inline int AQSTableItem::alignment() const
{
  AQ_CALL_RET_V(alignment(), int);
}
inline void AQSTableItem::setWordWrap(bool arg0)
{
  AQ_CALL_VOID(setWordWrap(arg0));
}
inline bool AQSTableItem::wordWrap() const
{
  AQ_CALL_RET_V(wordWrap(), bool);
}
inline uint AQSTableItem::editType() const
{
  AQ_CALL_RET_V(editType(), uint);
}
inline QWidget *AQSTableItem::createEditor() const
{
  AQ_CALL_RET(createEditor());
}
inline void AQSTableItem::setContentFromEditor(QWidget *arg0)
{
  AQ_CALL_VOID(setContentFromEditor(arg0));
}
inline void AQSTableItem::setReplaceable(bool arg0)
{
  AQ_CALL_VOID(setReplaceable(arg0));
}
inline bool AQSTableItem::isReplaceable() const
{
  AQ_CALL_RET_V(isReplaceable(), bool);
}
inline QString AQSTableItem::key() const
{
  AQ_CALL_RET_V(key(), QString);
}
inline QSize AQSTableItem::sizeHint() const
{
  AQ_CALL_RET_V(sizeHint(), QSize);
}
inline void AQSTableItem::setSpan(int arg0,  int arg1)
{
  AQ_CALL_VOID(setSpan(arg0, arg1));
}
inline int AQSTableItem::rowSpan() const
{
  AQ_CALL_RET_V(rowSpan(), int);
}
inline int AQSTableItem::colSpan() const
{
  AQ_CALL_RET_V(colSpan(), int);
}
inline void AQSTableItem::setRow(int arg0)
{
  AQ_CALL_VOID(setRow(arg0));
}
inline void AQSTableItem::setCol(int arg0)
{
  AQ_CALL_VOID(setCol(arg0));
}
inline int AQSTableItem::row() const
{
  AQ_CALL_RET_V(row(), int);
}
inline int AQSTableItem::col() const
{
  AQ_CALL_RET_V(col(), int);
}
inline void AQSTableItem::setEnabled(bool arg0)
{
  AQ_CALL_VOID(setEnabled(arg0));
}
inline bool AQSTableItem::isEnabled() const
{
  AQ_CALL_RET_V(isEnabled(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTABLEITEM_P_H_ */
// @AQSVOIDPTR@
