/***************************************************************************
 AQSStatusBar_p.h
 -------------------
 begin                : 02/04/2011
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

#ifndef AQSSTATUSBAR_P_H_
#define AQSSTATUSBAR_P_H_

#include "AQSWidget_p.h"
#include <qstatusbar.h>

class AQSStatusBar : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(StatusBar, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void addWidget(QWidget *, int = 0, bool = FALSE);
  virtual void removeWidget(QWidget *);
  void setSizeGripEnabled(bool);
  bool isSizeGripEnabled() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QStatusBar;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QStatusBar(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QStatusBar(argValue<QWidget *>(args[0]),
                            *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSStatusBar::addWidget(QWidget *arg0,  int arg1,  bool arg2)
{
  AQ_CALL_VOID(addWidget(arg0, arg1, arg2));
}
inline void AQSStatusBar::removeWidget(QWidget *arg0)
{
  AQ_CALL_VOID(removeWidget(arg0));
}
inline void AQSStatusBar::setSizeGripEnabled(bool arg0)
{
  AQ_CALL_VOID(setSizeGripEnabled(arg0));
}
inline bool AQSStatusBar::isSizeGripEnabled() const
{
  AQ_CALL_RET_V(isSizeGripEnabled(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSTATUSBAR_P_H_ */
// @AQSWIDGET@
