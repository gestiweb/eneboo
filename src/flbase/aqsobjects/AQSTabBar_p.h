/***************************************************************************
 AQSTabBar_p.h
 -------------------
 begin                : 06/02/2011
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

#ifndef AQSTABBAR_P_H_
#define AQSTABBAR_P_H_

#include "AQSWidget_p.h"
#include "AQSTab_p.h"
#include <qtabbar.h>

class AQSTabBar : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(TabBar, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int addTab(QTab *);
  int addTab(AQSTab *);
  int insertTab(QTab *, int = -1);
  int insertTab(AQSTab *, int = -1);
  void removeTab(QTab *);
  void removeTab(AQSTab *);
  void setTabEnabled(int, bool);
  bool isTabEnabled(int) const;
  QTab *tab(int) const;
  QTab *tabAt(int) const;
  int indexOf(int) const;
  void layoutTabs();
  QTab *selectTab(const QPoint &) const;
  void removeToolTip(int);
  void setToolTip(int, const QString &);
  QString toolTip(int) const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QTabBar;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QTabBar(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QTabBar(argValue<QWidget *>(args[0]),
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
inline int AQSTabBar::addTab(QTab *arg0)
{
  AQ_CALL_RET_V(addTab(arg0), int);
}
inline int AQSTabBar::addTab(AQSTab *arg0)
{
  AQ_CALL_RET_V(addTab(*arg0), int);
}
inline int AQSTabBar::insertTab(QTab *arg0,  int arg1)
{
  AQ_CALL_RET_V(insertTab(arg0, arg1), int);
}
inline int AQSTabBar::insertTab(AQSTab *arg0,  int arg1)
{
  AQ_CALL_RET_V(insertTab(*arg0, arg1), int);
}
inline void AQSTabBar::removeTab(QTab *arg0)
{
  AQ_CALL_VOID(removeTab(arg0));
}
inline void AQSTabBar::removeTab(AQSTab *arg0)
{
  AQ_CALL_VOID(removeTab(*arg0));
}
inline void AQSTabBar::setTabEnabled(int arg0,  bool arg1)
{
  AQ_CALL_VOID(setTabEnabled(arg0, arg1));
}
inline bool AQSTabBar::isTabEnabled(int arg0) const
{
  AQ_CALL_RET_V(isTabEnabled(arg0), bool);
}
inline QTab *AQSTabBar::tab(int arg0) const
{
  AQ_CALL_RET(tab(arg0));
}
inline QTab *AQSTabBar::tabAt(int arg0) const
{
  AQ_CALL_RET(tabAt(arg0));
}
inline int AQSTabBar::indexOf(int arg0) const
{
  AQ_CALL_RET_V(indexOf(arg0), int);
}
inline void AQSTabBar::layoutTabs()
{
  AQ_CALL_VOID(layoutTabs());
}
inline QTab *AQSTabBar::selectTab(const QPoint &arg0) const
{
  AQ_CALL_RET(selectTab(arg0));
}
inline void AQSTabBar::removeToolTip(int arg0)
{
  AQ_CALL_VOID(removeToolTip(arg0));
}
inline void AQSTabBar::setToolTip(int arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setToolTip(arg0, arg1));
}
inline QString AQSTabBar::toolTip(int arg0) const
{
  AQ_CALL_RET_V(toolTip(arg0), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTABBAR_P_H_ */
// @AQSWIDGET@
