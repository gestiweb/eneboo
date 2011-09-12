/***************************************************************************
 AQSTabWidget_p.h
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

#ifndef AQSTABWIDGET_P_H_
#define AQSTABWIDGET_P_H_

#include "AQSWidget_p.h"
#include "AQSTab_p.h"
#include <qtabwidget.h>

class AQSTabWidget : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(TabWidget, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void addTab(QWidget *, const QString &);
  void addTab(QWidget *, QIconSet *, const QString &);
  void addTab(QWidget *, AQSIconSet *, const QString &);
  void addTab(QWidget *, QTab *);
  void addTab(QWidget *, AQSTab *);
  void insertTab(QWidget *, const QString &, int = -1);
  void insertTab(QWidget *, QIconSet *, const QString &, int = -1);
  void insertTab(QWidget *, AQSIconSet *, const QString &, int = -1);
  void insertTab(QWidget *, QTab *, int = -1);
  void insertTab(QWidget *, AQSTab *, int = -1);
  void changeTab(QWidget *, const QString &);
  void changeTab(QWidget *, QIconSet *, const QString &);
  void changeTab(QWidget *, AQSIconSet *, const QString &);
  bool isTabEnabled(QWidget *) const;
  void setTabEnabled(QWidget *, bool);
  void setCornerWidget(QWidget *, uint = TopRight);
  QWidget *cornerWidget(uint = TopRight) const;
  QString tabLabel(QWidget *) const;
  void setTabLabel(QWidget *, const QString &);
  QIconSet *tabIconSet(QWidget *) const;
  void setTabIconSet(QWidget *, QIconSet *);
  void setTabIconSet(QWidget *, AQSIconSet *);
  void removeTabToolTip(QWidget *);
  void setTabToolTip(QWidget *, const QString &);
  QString tabToolTip(QWidget *) const;
  QWidget *currentPage() const;
  QWidget *page(int) const;
  QString label(int) const;
  int indexOf(QWidget *) const;
  int currentPageIndex() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QTabWidget;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QTabWidget(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QTabWidget(argValue<QWidget *>(args[0]),
                            *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QTabWidget(argValue<QWidget *>(args[0]),
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
inline void AQSTabWidget::addTab(QWidget *arg0,  const QString &arg1)
{
  AQ_CALL_VOID(addTab(arg0, arg1));
}
inline void AQSTabWidget::addTab(QWidget *arg0,  QIconSet *arg1,  const QString &arg2)
{
  AQ_CALL_VOID(addTab(arg0, *arg1, arg2));
}
inline void AQSTabWidget::addTab(QWidget *arg0, AQSIconSet *arg1,  const QString &arg2)
{
  AQ_CALL_VOID(addTab(arg0, *arg1, arg2));
}
inline void AQSTabWidget::addTab(QWidget *arg0,  QTab *arg1)
{
  AQ_CALL_VOID(addTab(arg0, arg1));
}
inline void AQSTabWidget::addTab(QWidget *arg0, AQSTab *arg1)
{
  AQ_CALL_VOID(addTab(arg0, *arg1));
}
inline void AQSTabWidget::insertTab(QWidget *arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(insertTab(arg0, arg1, arg2));
}
inline void AQSTabWidget::insertTab(QWidget *arg0,  QIconSet *arg1,  const QString &arg2,  int arg3)
{
  AQ_CALL_VOID(insertTab(arg0, *arg1, arg2, arg3));
}
inline void AQSTabWidget::insertTab(QWidget *arg0, AQSIconSet *arg1,  const QString &arg2,  int arg3)
{
  AQ_CALL_VOID(insertTab(arg0, *arg1, arg2, arg3));
}
inline void AQSTabWidget::insertTab(QWidget *arg0,  QTab *arg1,  int arg2)
{
  AQ_CALL_VOID(insertTab(arg0, arg1, arg2));
}
inline void AQSTabWidget::insertTab(QWidget *arg0, AQSTab *arg1,  int arg2)
{
  AQ_CALL_VOID(insertTab(arg0, *arg1, arg2));
}
inline void AQSTabWidget::changeTab(QWidget *arg0,  const QString &arg1)
{
  AQ_CALL_VOID(changeTab(arg0, arg1));
}
inline void AQSTabWidget::changeTab(QWidget *arg0,  QIconSet *arg1,  const QString &arg2)
{
  AQ_CALL_VOID(changeTab(arg0, *arg1, arg2));
}
inline void AQSTabWidget::changeTab(QWidget *arg0, AQSIconSet *arg1,  const QString &arg2)
{
  AQ_CALL_VOID(changeTab(arg0, *arg1, arg2));
}
inline bool AQSTabWidget::isTabEnabled(QWidget *arg0) const
{
  AQ_CALL_RET_V(isTabEnabled(arg0), bool);
}
inline void AQSTabWidget::setTabEnabled(QWidget *arg0,  bool arg1)
{
  AQ_CALL_VOID(setTabEnabled(arg0, arg1));
}
inline void AQSTabWidget::setCornerWidget(QWidget *arg0,  uint arg1)
{
  AQ_CALL_VOID(setCornerWidget(arg0, static_cast<Qt::Corner>(arg1)));
}
inline QWidget *AQSTabWidget::cornerWidget(uint arg0) const
{
  AQ_CALL_RET(cornerWidget(static_cast<Qt::Corner>(arg0)));
}
inline QString AQSTabWidget::tabLabel(QWidget *arg0) const
{
  AQ_CALL_RET_V(tabLabel(arg0), QString);
}
inline void AQSTabWidget::setTabLabel(QWidget *arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setTabLabel(arg0, arg1));
}
inline QIconSet *AQSTabWidget::tabIconSet(QWidget *arg0) const
{
  AQ_CALL_RET_PTR(tabIconSet(arg0), QIconSet);
}
inline void AQSTabWidget::setTabIconSet(QWidget *arg0,  QIconSet *arg1)
{
  AQ_CALL_VOID(setTabIconSet(arg0, *arg1));
}
inline void AQSTabWidget::setTabIconSet(QWidget *arg0, AQSIconSet *arg1)
{
  AQ_CALL_VOID(setTabIconSet(arg0, *arg1));
}
inline void AQSTabWidget::removeTabToolTip(QWidget *arg0)
{
  AQ_CALL_VOID(removeTabToolTip(arg0));
}
inline void AQSTabWidget::setTabToolTip(QWidget *arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setTabToolTip(arg0, arg1));
}
inline QString AQSTabWidget::tabToolTip(QWidget *arg0) const
{
  AQ_CALL_RET_V(tabToolTip(arg0), QString);
}
inline QWidget *AQSTabWidget::currentPage() const
{
  AQ_CALL_RET(currentPage());
}
inline QWidget *AQSTabWidget::page(int arg0) const
{
  AQ_CALL_RET(page(arg0));
}
inline QString AQSTabWidget::label(int arg0) const
{
  AQ_CALL_RET_V(label(arg0), QString);
}
inline int AQSTabWidget::indexOf(QWidget *arg0) const
{
  AQ_CALL_RET_V(indexOf(arg0), int);
}
inline int AQSTabWidget::currentPageIndex() const
{
  AQ_CALL_RET_V(currentPageIndex(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTABWIDGET_P_H_ */
// @AQSWIDGET@
