/***************************************************************************
 AQSTextEdit_p.h
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

#ifndef AQSTEXTEDIT_P_H_
#define AQSTEXTEDIT_P_H_

#include "AQSScrollView_p.h"
#include <qtextedit.h>

class AQSTextEdit : public AQSScrollView
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(TextEdit, ScrollView);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("QString,QString,QWidget*,QString"));
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString"))
      return new QTextEdit(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QString"))
      return new QTextEdit(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString,QString,QWidget*"))
      return new QTextEdit(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("QString,QString,QWidget*,QString"))
      return new QTextEdit(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           argValue<QWidget *>(args[2]),
                           *(argValue<QString *>(args[3])));
    if (sgt.isEmpty())
      return new QTextEdit;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QTextEdit(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QTextEdit(argValue<QWidget *>(args[0]),
                           *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("QString,QString,QWidget*,QString"));
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTEXTEDIT_P_H_ */
// @AQSWIDGET@
