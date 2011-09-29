/***************************************************************************
 AQSDateEdit_p.h
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

#ifndef AQSDATEEDIT_P_H_
#define AQSDATEEDIT_P_H_

#include "AQSWidget_p.h"
#include <qdatetimeedit.h>

class AQSDateEdit : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(DateEdit, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setRange(const QDate &, const QDate &);
  QString separator() const;
  void setSeparator(const QString &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("QDate"));
    candidates[2].append(QString::fromLatin1("QDate,QWidget*"));
    candidates[3].append(QString::fromLatin1("QDate,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDateEdit;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QDateEdit(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QDateEdit(argValue<QWidget *>(args[0]),
                           *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QDate"))
      return new QDateEdit(*(argValue<QDate *>(args[0])));
    if (sgt == QString::fromLatin1("QDate,QWidget*"))
      return new QDateEdit(*(argValue<QDate *>(args[0])),
                           argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QDate,QWidget*,QString"))
      return new QDateEdit(*(argValue<QDate *>(args[0])),
                           argValue<QWidget *>(args[1]),
                           *(argValue<QString *>(args[2])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("QDate"));
    candidates[2].append(QString::fromLatin1("QDate,QWidget*"));
    candidates[3].append(QString::fromLatin1("QDate,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSDateEdit::setRange(const QDate &arg0,  const QDate &arg1)
{
  AQ_CALL_VOID(setRange(arg0, arg1));
}
inline QString AQSDateEdit::separator() const
{
  AQ_CALL_RET_V(separator(), QString);
}
inline void AQSDateEdit::setSeparator(const QString &arg0)
{
  AQ_CALL_VOID(setSeparator(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDATEEDIT_P_H_ */
// @AQSWIDGET@
