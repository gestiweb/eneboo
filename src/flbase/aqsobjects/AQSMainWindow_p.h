/***************************************************************************
 AQSMainWindow_p.h
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

#ifndef AQSMAINWINDOW_P_H_
#define AQSMAINWINDOW_P_H_

#include "AQSWidget_p.h"
#include <qmainwindow.h>

class AQSMainWindow : public AQSWidget
{
  Q_OBJECT

  Q_ENUMS(Dock)

  AQ_DECLARE_AQS_OBJECT(MainWindow, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QMenuBar *menuBar() const;
  QStatusBar *statusBar() const;
  QToolTipGroup *toolTipGroup() const;
  virtual void setCentralWidget(QWidget *);
  QWidget *centralWidget() const;
  virtual void setDockEnabled(Dock, bool);
  bool isDockEnabled(Dock) const;
  bool isDockEnabled(QDockArea *) const;
  virtual void setDockEnabled(QDockWindow *, Dock, bool);
  bool isDockEnabled(QDockWindow *, Dock) const;
  bool isDockEnabled(QDockWindow *, QDockArea *) const;
  virtual void addDockWindow(QDockWindow *, Dock = DockTop, bool = FALSE);
  virtual void addDockWindow(QDockWindow *, const QString &, Dock = DockTop, bool = FALSE);
  virtual void moveDockWindow(QDockWindow *, Dock = DockTop);
  virtual void moveDockWindow(QDockWindow *, Dock, bool, int, int = -1);
  virtual void removeDockWindow(QDockWindow *);
  bool usesBigPixmaps() const;
  bool usesTextLabel() const;
  bool dockWindowsMovable() const;
  bool opaqueMoving() const;
  bool getLocation(QDockWindow *, Dock &, int &, bool &, int &) const;
  void lineUpDockWindows(bool = FALSE);
  bool isDockMenuEnabled() const;
  bool hasDockWindow(QDockWindow *);
  QDockArea *leftDock() const;
  QDockArea *rightDock() const;
  QDockArea *topDock() const;
  QDockArea *bottomDock() const;
  virtual bool isCustomizable() const;
  bool appropriate(QDockWindow *) const;
  QPopupMenu *createDockWindowMenu(uint = QMainWindow::AllDockWindows) const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QMainWindow;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QMainWindow(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QMainWindow(argValue<QWidget *>(args[0]),
                             *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QMainWindow(argValue<QWidget *>(args[0]),
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
inline QMenuBar *AQSMainWindow::menuBar() const
{
  AQ_CALL_RET(menuBar());
}
inline QStatusBar *AQSMainWindow::statusBar() const
{
  AQ_CALL_RET(statusBar());
}
inline QToolTipGroup *AQSMainWindow::toolTipGroup() const
{
  AQ_CALL_RET(toolTipGroup());
}
inline void AQSMainWindow::setCentralWidget(QWidget *arg0)
{
  AQ_CALL_VOID(setCentralWidget(arg0));
}
inline QWidget *AQSMainWindow::centralWidget() const
{
  AQ_CALL_RET(centralWidget());
}
inline void AQSMainWindow::setDockEnabled(Dock arg0,  bool arg1)
{
  AQ_CALL_VOID(setDockEnabled(arg0, arg1));
}
inline bool AQSMainWindow::isDockEnabled(Dock arg0) const
{
  AQ_CALL_RET_V(isDockEnabled(arg0), bool);
}
inline bool AQSMainWindow::isDockEnabled(QDockArea *arg0) const
{
  AQ_CALL_RET_V(isDockEnabled(arg0), bool);
}
inline void AQSMainWindow::setDockEnabled(QDockWindow *arg0,  Dock arg1,  bool arg2)
{
  AQ_CALL_VOID(setDockEnabled(arg0, arg1, arg2));
}
inline bool AQSMainWindow::isDockEnabled(QDockWindow *arg0,  Dock arg1) const
{
  AQ_CALL_RET_V(isDockEnabled(arg0, arg1), bool);
}
inline bool AQSMainWindow::isDockEnabled(QDockWindow *arg0,  QDockArea *arg1) const
{
  AQ_CALL_RET_V(isDockEnabled(arg0, arg1), bool);
}
inline void AQSMainWindow::addDockWindow(QDockWindow *arg0,  Dock arg1,  bool arg2)
{
  AQ_CALL_VOID(addDockWindow(arg0, arg1, arg2));
}
inline void AQSMainWindow::addDockWindow(QDockWindow *arg0,  const QString &arg1,  Dock arg2,  bool arg3)
{
  AQ_CALL_VOID(addDockWindow(arg0, arg1, arg2, arg3));
}
inline void AQSMainWindow::moveDockWindow(QDockWindow *arg0,  Dock arg1)
{
  AQ_CALL_VOID(moveDockWindow(arg0, arg1));
}
inline void AQSMainWindow::moveDockWindow(QDockWindow *arg0,  Dock arg1,  bool arg2,  int arg3,  int arg4)
{
  AQ_CALL_VOID(moveDockWindow(arg0, arg1, arg2, arg3, arg4));
}
inline void AQSMainWindow::removeDockWindow(QDockWindow *arg0)
{
  AQ_CALL_VOID(removeDockWindow(arg0));
}
inline bool AQSMainWindow::usesBigPixmaps() const
{
  AQ_CALL_RET_V(usesBigPixmaps(), bool);
}
inline bool AQSMainWindow::usesTextLabel() const
{
  AQ_CALL_RET_V(usesTextLabel(), bool);
}
inline bool AQSMainWindow::dockWindowsMovable() const
{
  AQ_CALL_RET_V(dockWindowsMovable(), bool);
}
inline bool AQSMainWindow::opaqueMoving() const
{
  AQ_CALL_RET_V(opaqueMoving(), bool);
}
inline bool AQSMainWindow::getLocation(QDockWindow *arg0,  Dock &arg1,  int &arg2,  bool &arg3,  int &arg4) const
{
  AQ_CALL_RET_V(getLocation(arg0, arg1, arg2, arg3, arg4), bool);
}
inline void AQSMainWindow::lineUpDockWindows(bool arg0)
{
  AQ_CALL_VOID(lineUpDockWindows(arg0));
}
inline bool AQSMainWindow::isDockMenuEnabled() const
{
  AQ_CALL_RET_V(isDockMenuEnabled(), bool);
}
inline bool AQSMainWindow::hasDockWindow(QDockWindow *arg0)
{
  AQ_CALL_RET_V(hasDockWindow(arg0), bool);
}
inline QDockArea *AQSMainWindow::leftDock() const
{
  AQ_CALL_RET(leftDock());
}
inline QDockArea *AQSMainWindow::rightDock() const
{
  AQ_CALL_RET(rightDock());
}
inline QDockArea *AQSMainWindow::topDock() const
{
  AQ_CALL_RET(topDock());
}
inline QDockArea *AQSMainWindow::bottomDock() const
{
  AQ_CALL_RET(bottomDock());
}
inline bool AQSMainWindow::isCustomizable() const
{
  AQ_CALL_RET_V(isCustomizable(), bool);
}
inline bool AQSMainWindow::appropriate(QDockWindow *arg0) const
{
  AQ_CALL_RET_V(appropriate(arg0), bool);
}
inline QPopupMenu *AQSMainWindow::createDockWindowMenu(uint arg0) const
{
  AQ_CALL_RET(createDockWindowMenu(static_cast<QMainWindow::DockWindows>(arg0)));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSMAINWINDOW_P_H_ */
// @AQSWIDGET@
