/***************************************************************************
 AQSProgressDialog_p.h
 -------------------
 begin                : 18/02/2011
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

#ifndef AQSPROGRESSDIALOG_P_H_
#define AQSPROGRESSDIALOG_P_H_

#include "AQSDialog_p.h"
#include <qprogressdialog.h>

class AQSProgressDialog : public AQSDialog
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ProgressDialog, Dialog);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setLabel(QLabel *);
  void setCancelButton(QPushButton *);
  void setBar(QProgressBar *);
  virtual QSize sizeHint() const;
  QString labelText() const;
  void setAutoReset(bool);
  bool autoReset() const;
  void setAutoClose(bool);
  bool autoClose() const;
  int minimumDuration() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,bool"));
    candidates[4].append(QString::fromLatin1("QWidget*,QString,bool,WFlags"));
    candidates[3].append(QString::fromLatin1("QString,QString,int"));
    candidates[4].append(QString::fromLatin1("QString,QString,int,QWidget*"));
    candidates[5].append(QString::fromLatin1("QString,QString,int,QWidget*,QString"));
    candidates[6].append(QString::fromLatin1("QString,QString,int,QWidget*,QString,bool"));
    candidates[7].append(QString::fromLatin1("QString,QString,int,QWidget*,QString,bool,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QProgressDialog;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QProgressDialog(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QProgressDialog(argValue<QWidget *>(args[0]),
                                 *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,bool"))
      return new QProgressDialog(argValue<QWidget *>(args[0]),
                                 *(argValue<QString *>(args[1])),
                                 args[2].variant().toBool());
    if (sgt == QString::fromLatin1("QWidget*,QString,bool,WFlags"))
      return new QProgressDialog(argValue<QWidget *>(args[0]),
                                 *(argValue<QString *>(args[1])),
                                 args[2].variant().toBool(),
                                 static_cast<WFlags>(args[3].variant().toUInt()));
    if (sgt == QString::fromLatin1("QString,QString,int"))
      return new QProgressDialog(*(argValue<QString *>(args[0])),
                                 *(argValue<QString *>(args[1])),
                                 args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QString,QString,int,QWidget*"))
      return new QProgressDialog(*(argValue<QString *>(args[0])),
                                 *(argValue<QString *>(args[1])),
                                 args[2].variant().toInt(),
                                 argValue<QWidget *>(args[3]));
    if (sgt == QString::fromLatin1("QString,QString,int,QWidget*,QString"))
      return new QProgressDialog(*(argValue<QString *>(args[0])),
                                 *(argValue<QString *>(args[1])),
                                 args[2].variant().toInt(),
                                 argValue<QWidget *>(args[3]),
                                 *(argValue<QString *>(args[4])));
    if (sgt == QString::fromLatin1("QString,QString,int,QWidget*,QString,bool"))
      return new QProgressDialog(*(argValue<QString *>(args[0])),
                                 *(argValue<QString *>(args[1])),
                                 args[2].variant().toInt(),
                                 argValue<QWidget *>(args[3]),
                                 *(argValue<QString *>(args[4])),
                                 args[5].variant().toBool());
    if (sgt == QString::fromLatin1("QString,QString,int,QWidget*,QString,bool,WFlags"))
      return new QProgressDialog(*(argValue<QString *>(args[0])),
                                 *(argValue<QString *>(args[1])),
                                 args[2].variant().toInt(),
                                 argValue<QWidget *>(args[3]),
                                 *(argValue<QString *>(args[4])),
                                 args[5].variant().toBool(),
                                 static_cast<WFlags>(args[6].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,bool"));
    candidates[4].append(QString::fromLatin1("QWidget*,QString,bool,WFlags"));
    candidates[3].append(QString::fromLatin1("QString,QString,int"));
    candidates[4].append(QString::fromLatin1("QString,QString,int,QWidget*"));
    candidates[5].append(QString::fromLatin1("QString,QString,int,QWidget*,QString"));
    candidates[6].append(QString::fromLatin1("QString,QString,int,QWidget*,QString,bool"));
    candidates[7].append(QString::fromLatin1("QString,QString,int,QWidget*,QString,bool,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSProgressDialog::setLabel(QLabel *arg0)
{
  AQ_CALL_VOID(setLabel(arg0));
}
inline void AQSProgressDialog::setCancelButton(QPushButton *arg0)
{
  AQ_CALL_VOID(setCancelButton(arg0));
}
inline void AQSProgressDialog::setBar(QProgressBar *arg0)
{
  AQ_CALL_VOID(setBar(arg0));
}
inline QSize AQSProgressDialog::sizeHint() const
{
  AQ_CALL_RET_V(sizeHint(), QSize);
}
inline QString AQSProgressDialog::labelText() const
{
  AQ_CALL_RET_V(labelText(), QString);
}
inline void AQSProgressDialog::setAutoReset(bool arg0)
{
  AQ_CALL_VOID(setAutoReset(arg0));
}
inline bool AQSProgressDialog::autoReset() const
{
  AQ_CALL_RET_V(autoReset(), bool);
}
inline void AQSProgressDialog::setAutoClose(bool arg0)
{
  AQ_CALL_VOID(setAutoClose(arg0));
}
inline bool AQSProgressDialog::autoClose() const
{
  AQ_CALL_RET_V(autoClose(), bool);
}
inline int AQSProgressDialog::minimumDuration() const
{
  AQ_CALL_RET_V(minimumDuration(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPROGRESSDIALOG_P_H_ */
// @AQSWIDGET@
