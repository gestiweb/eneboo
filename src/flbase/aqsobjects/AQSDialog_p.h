/***************************************************************************
 AQSDialog_p.h
 -------------------
 begin                : 09/02/2011
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

#ifndef AQSDIALOG_P_H_
#define AQSDIALOG_P_H_

#include "AQSWidget_p.h"
#include <qdialog.h>

class AQSDialog : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Dialog, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int result() const;
  void show();
  void setOrientation(uint);
  void setExtension(QWidget *);
  QWidget *extension() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,bool"));
    candidates[4].append(QString::fromLatin1("QWidget*,QString,bool,WFlags"));
    candidates[0].append(QString::null);
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDialog;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QDialog(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QDialog(argValue<QWidget *>(args[0]),
                         *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,bool"))
      return new QDialog(argValue<QWidget *>(args[0]),
                         *(argValue<QString *>(args[1])),
                         args[2].variant().toBool());
    if (sgt == QString::fromLatin1("QWidget*,QString,bool,WFlags"))
      return new QDialog(argValue<QWidget *>(args[0]),
                         *(argValue<QString *>(args[1])),
                         args[2].variant().toBool(),
                         static_cast<WFlags>(args[3].variant().toUInt()));
    if (sgt.isEmpty())
      return new QDialog;
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
    candidates[0].append(QString::null);
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSDialog::result() const
{
  AQ_CALL_RET_V(result(), int);
}
inline void AQSDialog::show()
{
  AQ_CALL_VOID(show());
}
inline void AQSDialog::setOrientation(uint arg0)
{
  AQ_CALL_VOID(setOrientation(static_cast<Qt::Orientation>(arg0)));
}
inline void AQSDialog::setExtension(QWidget *arg0)
{
  AQ_CALL_VOID(setExtension(arg0));
}
inline QWidget *AQSDialog::extension() const
{
  AQ_CALL_RET(extension());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDIALOG_P_H_ */
// @AQSWIDGET@
