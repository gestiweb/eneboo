/***************************************************************************
 AQSComboTableItem_p.h
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

#ifndef AQSCOMBOTABLEITEM_P_H_
#define AQSCOMBOTABLEITEM_P_H_

#include "AQSTableItem_p.h"
#include <qtable.h>

class AQSComboTableItem : public AQSTableItem
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(ComboTableItem, TableItem);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setCurrentItem(int);
  void setCurrentItem(const QString &);
  int currentItem() const;
  QString currentText() const;
  int count() const;
  QString text(int) const;
  void setEditable(bool);
  bool isEditable() const;
  void setStringList(const QStringList &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QTable*,QStringList"));
    candidates[3].append(QString::fromLatin1("QTable*,QStringList,bool"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QTable*,QStringList"))
      return new QComboTableItem(argValue<QTable *>(args[0]),
                                 *(argValue<QStringList *>(args[1])));
    if (sgt == QString::fromLatin1("QTable*,QStringList,bool"))
      return new QComboTableItem(argValue<QTable *>(args[0]),
                                 *(argValue<QStringList *>(args[1])),
                                 args[2].variant().toBool());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QTable*,QStringList"));
    candidates[3].append(QString::fromLatin1("QTable*,QStringList,bool"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSComboTableItem::setCurrentItem(int arg0)
{
  AQ_CALL_VOID(setCurrentItem(arg0));
}
inline void AQSComboTableItem::setCurrentItem(const QString &arg0)
{
  AQ_CALL_VOID(setCurrentItem(arg0));
}
inline int AQSComboTableItem::currentItem() const
{
  AQ_CALL_RET_V(currentItem(), int);
}
inline QString AQSComboTableItem::currentText() const
{
  AQ_CALL_RET_V(currentText(), QString);
}
inline int AQSComboTableItem::count() const
{
  AQ_CALL_RET_V(count(), int);
}
inline QString AQSComboTableItem::text(int arg0) const
{
  AQ_CALL_RET_V(text(arg0), QString);
}
inline void AQSComboTableItem::setEditable(bool arg0)
{
  AQ_CALL_VOID(setEditable(arg0));
}
inline bool AQSComboTableItem::isEditable() const
{
  AQ_CALL_RET_V(isEditable(), bool);
}
inline void AQSComboTableItem::setStringList(const QStringList &arg0)
{
  AQ_CALL_VOID(setStringList(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCOMBOTABLEITEM_P_H_ */
// @AQSVOIDPTR@
