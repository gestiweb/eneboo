/***************************************************************************
 AQSCheckTableItem_p.h
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

#ifndef AQSCHECKTABLEITEM_P_H_
#define AQSCHECKTABLEITEM_P_H_

#include "AQSTableItem_p.h"
#include <qtable.h>

class AQSCheckTableItem : public AQSTableItem
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CheckTableItem, TableItem);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setChecked(bool);
  bool isChecked() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QTable*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QTable*,QString"))
      return new QCheckTableItem(argValue<QTable *>(args[0]),
                                 *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QTable*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSCheckTableItem::setChecked(bool arg0)
{
  AQ_CALL_VOID(setChecked(arg0));
}
inline bool AQSCheckTableItem::isChecked() const
{
  AQ_CALL_RET_V(isChecked(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCHECKTABLEITEM_P_H_ */
// @AQSVOIDPTR@
