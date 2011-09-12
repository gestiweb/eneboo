/***************************************************************************
 AQSPushButton_p.h
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

#ifndef AQSPUSHBUTTON_P_H_
#define AQSPUSHBUTTON_P_H_

#include "AQSButton_p.h"
#include <qpushbutton.h>

class AQSPushButton : public AQSButton
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(PushButton, Button);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setPopup(QPopupMenu *);
  QPopupMenu *popup() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QIconSet*,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("QIconSet*,QString,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QPushButton(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QPushButton(argValue<QWidget *>(args[0]),
                             *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString,QWidget*"))
      return new QPushButton(*(argValue<QString *>(args[0])),
                             argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QString,QWidget*,QString"))
      return new QPushButton(*(argValue<QString *>(args[0])),
                             argValue<QWidget *>(args[1]),
                             *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QIconSet*,QString,QWidget*"))
      return new QPushButton(*argValue<QIconSet *>(args[0]),
                             *(argValue<QString *>(args[1])),
                             argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("QIconSet*,QString,QWidget*,QString"))
      return new QPushButton(*argValue<QIconSet *>(args[0]),
                             *(argValue<QString *>(args[1])),
                             argValue<QWidget *>(args[2]),
                             *(argValue<QString *>(args[3])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QIconSet*,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("QIconSet*,QString,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSPushButton::setPopup(QPopupMenu *arg0)
{
  AQ_CALL_VOID(setPopup(arg0));
}
inline QPopupMenu *AQSPushButton::popup() const
{
  AQ_CALL_RET(popup());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPUSHBUTTON_P_H_ */
// @AQSWIDGET@
