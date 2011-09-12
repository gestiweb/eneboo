/***************************************************************************
 AQSWidgetStack_p.h
 -------------------
 begin                : 05/02/2011
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

#ifndef AQSWIDGETSTACK_P_H_
#define AQSWIDGETSTACK_P_H_

#include "AQSFrame_p.h"
#include <qwidgetstack.h>

class AQSWidgetStack : public AQSFrame
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(WidgetStack, Frame);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int addWidget(QWidget *, int = -1);
  void removeWidget(QWidget *);
  QWidget *widget(int) const;
  int id(QWidget *) const;
  QWidget *visibleWidget() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QWidgetStack;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QWidgetStack(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QWidgetStack(argValue<QWidget *>(args[0]),
                              *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QWidgetStack(argValue<QWidget *>(args[0]),
                              *(argValue<QString *>(args[1])),
                              static_cast<WFlags>(args[2].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSWidgetStack::addWidget(QWidget *arg0,  int arg1)
{
  AQ_CALL_RET_V(addWidget(arg0, arg1), int);
}
inline void AQSWidgetStack::removeWidget(QWidget *arg0)
{
  AQ_CALL_VOID(removeWidget(arg0));
}
inline QWidget *AQSWidgetStack::widget(int arg0) const
{
  AQ_CALL_RET(widget(arg0));
}
inline int AQSWidgetStack::id(QWidget *arg0) const
{
  AQ_CALL_RET_V(id(arg0), int);
}
inline QWidget *AQSWidgetStack::visibleWidget() const
{
  AQ_CALL_RET(visibleWidget());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSWIDGETSTACK_P_H_ */
// @AQSWIDGET@
