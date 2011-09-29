/***************************************************************************
 AQSToolButton_p.h
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

#ifndef AQSTOOLBUTTON_P_H_
#define AQSTOOLBUTTON_P_H_

#include "AQSButton_p.h"
#include "AQSIconSet_p.h"
#include <qtoolbutton.h>

class AQSToolButton : public AQSButton
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ToolButton, Button);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setIconSet(QIconSet *);
  virtual void setIconSet(AQSIconSet *);
  QIconSet *iconSet() const;
  void setPopup(QPopupMenu *);
  QPopupMenu *popup() const;
  void setPopupDelay(int);
  void openPopup();
  void setAutoRaise(bool);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[6].append(QString::fromLatin1("QIconSet*,QString,QString,QObject*,QString,QToolBar*"));
    candidates[7].append(QString::fromLatin1("QIconSet*,QString,QString,QObject*,QString,QToolBar*,QString"));
    candidates[2].append(QString::fromLatin1("Qt::ArrowType,QWidget*"));
    candidates[3].append(QString::fromLatin1("Qt::ArrowType,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QToolButton(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QToolButton(argValue<QWidget *>(args[0]),
                             *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QIconSet*,QString,QString,QObject*,QString,QToolBar*"))
      return new QToolButton(*argValue<QIconSet *>(args[0]),
                             *(argValue<QString *>(args[1])),
                             *(argValue<QString *>(args[2])),
                             argValue<QObject *>(args[3]),
                             *(argValue<QString *>(args[4])),
                             argValue<QToolBar *>(args[5]));
    if (sgt == QString::fromLatin1("QIconSet*,QString,QString,QObject*,QString,QToolBar*,QString"))
      return new QToolButton(*argValue<QIconSet *>(args[0]),
                             *(argValue<QString *>(args[1])),
                             *(argValue<QString *>(args[2])),
                             argValue<QObject *>(args[3]),
                             *(argValue<QString *>(args[4])),
                             argValue<QToolBar *>(args[5]),
                             *(argValue<QString *>(args[6])));
    if (sgt == QString::fromLatin1("Qt::ArrowType,QWidget*"))
      return new QToolButton(static_cast<Qt::ArrowType>(args[0].variant().toUInt()),
                             argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("Qt::ArrowType,QWidget*,QString"))
      return new QToolButton(static_cast<Qt::ArrowType>(args[0].variant().toUInt()),
                             argValue<QWidget *>(args[1]),
                             *(argValue<QString *>(args[2])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[6].append(QString::fromLatin1("QIconSet*,QString,QString,QObject*,QString,QToolBar*"));
    candidates[7].append(QString::fromLatin1("QIconSet*,QString,QString,QObject*,QString,QToolBar*,QString"));
    candidates[2].append(QString::fromLatin1("Qt::ArrowType,QWidget*"));
    candidates[3].append(QString::fromLatin1("Qt::ArrowType,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSToolButton::setIconSet(QIconSet *arg0)
{
  AQ_CALL_VOID(setIconSet(*arg0));
}
inline void AQSToolButton::setIconSet(AQSIconSet *arg0)
{
  AQ_CALL_VOID(setIconSet(*arg0));
}
inline QIconSet *AQSToolButton::iconSet() const
{
  AQ_CALL_RET_PTR(iconSet(), QIconSet);
}
inline void AQSToolButton::setPopup(QPopupMenu *arg0)
{
  AQ_CALL_VOID(setPopup(arg0));
}
inline QPopupMenu *AQSToolButton::popup() const
{
  AQ_CALL_RET(popup());
}
inline void AQSToolButton::setPopupDelay(int arg0)
{
  AQ_CALL_VOID(setPopupDelay(arg0));
}
inline void AQSToolButton::openPopup()
{
  AQ_CALL_VOID(openPopup());
}
inline void AQSToolButton::setAutoRaise(bool arg0)
{
  AQ_CALL_VOID(setAutoRaise(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTOOLBUTTON_P_H_ */
// @AQSWIDGET@
