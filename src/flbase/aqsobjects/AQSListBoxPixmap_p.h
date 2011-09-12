/***************************************************************************
 AQSListBoxPixmap_p.h
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

#ifndef AQSLISTBOXPIXMAP_P_H_
#define AQSLISTBOXPIXMAP_P_H_

#include "AQSListBoxItem_p.h"
#include <qlistbox.h>

class AQSListBoxPixmap : public AQSListBoxItem
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(ListBoxPixmap, ListBoxItem);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QListBox*,QPixmap*"));
    candidates[1].append(QString::fromLatin1("QPixmap*"));
    candidates[3].append(QString::fromLatin1("QListBox*,QPixmap*,QListBoxItem*"));
    candidates[3].append(QString::fromLatin1("QListBox*,QPixmap*,QString"));
    candidates[2].append(QString::fromLatin1("QPixmap*,QString"));
    candidates[4].append(QString::fromLatin1("QListBox*,QPixmap*,QString,QListBoxItem*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QListBox*,QPixmap*"))
      return new QListBoxPixmap(argValue<QListBox *>(args[0]),
                                *argValue<QPixmap *>(args[1]));
    if (sgt == QString::fromLatin1("QPixmap*"))
      return new QListBoxPixmap(*argValue<QPixmap *>(args[0]));
    if (sgt == QString::fromLatin1("QListBox*,QPixmap*,QListBoxItem*"))
      return new QListBoxPixmap(argValue<QListBox *>(args[0]),
                                *argValue<QPixmap *>(args[1]),
                                argValue<QListBoxItem *>(args[2]));
    if (sgt == QString::fromLatin1("QListBox*,QPixmap*,QString"))
      return new QListBoxPixmap(argValue<QListBox *>(args[0]),
                                *argValue<QPixmap *>(args[1]),
                                *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QPixmap*,QString"))
      return new QListBoxPixmap(*argValue<QPixmap *>(args[0]),
                                *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QListBox*,QPixmap*,QString,QListBoxItem*"))
      return new QListBoxPixmap(argValue<QListBox *>(args[0]),
                                *argValue<QPixmap *>(args[1]),
                                *(argValue<QString *>(args[2])),
                                argValue<QListBoxItem *>(args[3]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QListBox*,QPixmap*"));
    candidates[1].append(QString::fromLatin1("QPixmap*"));
    candidates[3].append(QString::fromLatin1("QListBox*,QPixmap*,QListBoxItem*"));
    candidates[3].append(QString::fromLatin1("QListBox*,QPixmap*,QString"));
    candidates[2].append(QString::fromLatin1("QPixmap*,QString"));
    candidates[4].append(QString::fromLatin1("QListBox*,QPixmap*,QString,QListBoxItem*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSLISTBOXPIXMAP_P_H_ */
// @AQSVOIDPTR@
