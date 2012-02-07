/***************************************************************************
 AQSApplication_p.h
 -------------------
 begin                : 23/02/2011
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

#ifndef AQSAPPLICATION_P_H_
#define AQSAPPLICATION_P_H_

#include "AQSObject_p.h"
#include "AQSSqlDatabase_p.h"
#include "AQApplication.h"

class AQSApplication : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(Application, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QWidget *mainWidget() const;
  virtual void polish(QWidget *);
  QWidget *focusWidget() const;
  QWidget *activeWindow() const;
  void processEvents();
  void processEvents(int);
  bool hasPendingEvents();
  virtual bool notify(QObject *, QEvent *);
  virtual bool notify(QObject *, AQSEvent *);
  QString translate(const char *, const char *, const char* = 0, uint = QApplication::DefaultCodec) const;
  QString applicationDirPath();
  QString applicationFilePath();
  bool isSessionRestored() const;
  QString sessionId() const;
  QString sessionKey() const;
  void setStyle(QStyle *);
  QStyle *setStyle(const QString &);
  int colorSpec();
  void setColorSpec(int);
  QCursor *overrideCursor();
  void setOverrideCursor(const QCursor &, bool = FALSE);
  void restoreOverrideCursor();
  bool hasGlobalMouseTracking();
  void setGlobalMouseTracking(bool);
  QPalette palette(const QWidget* = 0);
  void setPalette(const QPalette &, bool = FALSE, const char* = 0);
  QFont font(const QWidget* = 0);
  void setFont(const QFont &, bool = FALSE, const char* = 0);
  QFontMetrics *fontMetrics();
  QDesktopWidget *desktop();
  QWidget *activePopupWidget();
  QWidget *activeModalWidget();
  QClipboard *clipboard();
  QWidget *widgetAt(int, int, bool = FALSE);
  QWidget *widgetAt(const QPoint &, bool = FALSE);
  QEventLoop *eventLoop();
  bool sendEvent(QObject *, QEvent *);
  bool sendEvent(QObject *, AQSEvent *);
  void postEvent(QObject *, QEvent *);
  void postEvent(QObject *, AQSEvent *);
  void sendPostedEvents(QObject *, int);
  void sendPostedEvents();
  void removePostedEvents(QObject *);
  bool startingUp();
  bool closingDown();
  void setDesktopSettingsAware(bool);
  bool desktopSettingsAware();
  void setCursorFlashTime(int);
  int cursorFlashTime();
  void setDoubleClickInterval(int);
  int doubleClickInterval();
  void setWheelScrollLines(int);
  int wheelScrollLines();
  void setGlobalStrut(const QSize &);
  QSize globalStrut();
  void setLibraryPaths(const QStringList &);
  QStringList libraryPaths();
  void addLibraryPath(const QString &);
  void removeLibraryPath(const QString &);
  void setStartDragTime(int);
  int startDragTime();
  void setStartDragDistance(int);
  int startDragDistance();
  void setReverseLayout(bool);
  bool reverseLayout();
  int horizontalAlignment(int);
  void quit();
  void closeAllWindows();
  void aboutQt();
  void reinit();
  FLSqlDatabase *db();
  void setMainWidget(QWidget *);
  QSArgument call(const QString &, const QString &) const;
  void activateModule(const QString &);
  void execMainScript(const QString &);
  void aboutAbanQ();
  void chooseFont();
  void setDatabaseLockDetection(bool = true, int = 30000, int = -1, bool = true, const QString& = QString::null, const QString& = QString::null);
  QString commaSeparator() const;

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QWidget *AQSApplication::mainWidget() const
{
  AQ_CALL_RET(mainWidget());
}
inline void AQSApplication::polish(QWidget *arg0)
{
  AQ_CALL_VOID(polish(arg0));
}
inline QWidget *AQSApplication::focusWidget() const
{
  AQ_CALL_RET(focusWidget());
}
inline QWidget *AQSApplication::activeWindow() const
{
  AQ_CALL_RET(activeWindow());
}
inline void AQSApplication::processEvents()
{
  AQ_CALL_VOID(processEvents());
}
inline void AQSApplication::processEvents(int arg0)
{
  AQ_CALL_VOID(processEvents(arg0));
}
inline bool AQSApplication::hasPendingEvents()
{
  AQ_CALL_RET_V(hasPendingEvents(), bool);
}
inline bool AQSApplication::notify(QObject *arg0,  QEvent *arg1)
{
  AQ_CALL_RET_V(notify(arg0, arg1), bool);
}
inline bool AQSApplication::notify(QObject *arg0, AQSEvent *arg1)
{
  AQ_CALL_RET_V(notify(arg0, *arg1), bool);
}
inline QString AQSApplication::translate(const char *arg0,  const char *arg1,  const char *arg2,  uint arg3) const
{
  AQ_CALL_RET_V(translate(arg0, arg1, arg2, static_cast<QApplication::Encoding>(arg3)), QString);
}
inline QString AQSApplication::applicationDirPath()
{
  AQ_CALL_RET_V(applicationDirPath(), QString);
}
inline QString AQSApplication::applicationFilePath()
{
  AQ_CALL_RET_V(applicationFilePath(), QString);
}
inline bool AQSApplication::isSessionRestored() const
{
  AQ_CALL_RET_V(isSessionRestored(), bool);
}
inline QString AQSApplication::sessionId() const
{
  AQ_CALL_RET_V(sessionId(), QString);
}
inline QString AQSApplication::sessionKey() const
{
  AQ_CALL_RET_V(sessionKey(), QString);
}
inline void AQSApplication::setStyle(QStyle *arg0)
{
  AQ_CALL_VOID(setStyle(arg0));
}
inline QStyle *AQSApplication::setStyle(const QString &arg0)
{
  AQ_CALL_RET(setStyle(arg0));
}
inline int AQSApplication::colorSpec()
{
  AQ_CALL_RET_V(colorSpec(), int);
}
inline void AQSApplication::setColorSpec(int arg0)
{
  AQ_CALL_VOID(setColorSpec(arg0));
}
inline QCursor *AQSApplication::overrideCursor()
{
  AQ_CALL_RET(overrideCursor());
}
inline void AQSApplication::setOverrideCursor(const QCursor &arg0,  bool arg1)
{
  AQ_CALL_VOID(setOverrideCursor(arg0, arg1));
}
inline void AQSApplication::restoreOverrideCursor()
{
  AQ_CALL_VOID(restoreOverrideCursor());
}
inline bool AQSApplication::hasGlobalMouseTracking()
{
  AQ_CALL_RET_V(hasGlobalMouseTracking(), bool);
}
inline void AQSApplication::setGlobalMouseTracking(bool arg0)
{
  AQ_CALL_VOID(setGlobalMouseTracking(arg0));
}
inline QPalette AQSApplication::palette(const QWidget *arg0)
{
  AQ_CALL_RET_V(palette(arg0), QPalette);
}
inline void AQSApplication::setPalette(const QPalette &arg0,  bool arg1,  const char *arg2)
{
  AQ_CALL_VOID(setPalette(arg0, arg1, arg2));
}
inline QFont AQSApplication::font(const QWidget *arg0)
{
  AQ_CALL_RET_V(font(arg0), QFont);
}
inline void AQSApplication::setFont(const QFont &arg0,  bool arg1,  const char *arg2)
{
  AQ_CALL_VOID(setFont(arg0, arg1, arg2));
}
inline QFontMetrics *AQSApplication::fontMetrics()
{
  AQ_CALL_RET_PTR(fontMetrics(), QFontMetrics);
}
inline QDesktopWidget *AQSApplication::desktop()
{
  AQ_CALL_RET(desktop());
}
inline QWidget *AQSApplication::activePopupWidget()
{
  AQ_CALL_RET(activePopupWidget());
}
inline QWidget *AQSApplication::activeModalWidget()
{
  AQ_CALL_RET(activeModalWidget());
}
inline QClipboard *AQSApplication::clipboard()
{
  AQ_CALL_RET(clipboard());
}
inline QWidget *AQSApplication::widgetAt(int arg0,  int arg1,  bool arg2)
{
  AQ_CALL_RET(widgetAt(arg0, arg1, arg2));
}
inline QWidget *AQSApplication::widgetAt(const QPoint &arg0,  bool arg1)
{
  AQ_CALL_RET(widgetAt(arg0, arg1));
}
inline QEventLoop *AQSApplication::eventLoop()
{
  AQ_CALL_RET(eventLoop());
}
inline bool AQSApplication::sendEvent(QObject *arg0,  QEvent *arg1)
{
  AQ_CALL_RET_V(sendEvent(arg0, arg1), bool);
}
inline bool AQSApplication::sendEvent(QObject *arg0, AQSEvent *arg1)
{
  AQ_CALL_RET_V(sendEvent(arg0, *arg1), bool);
}
inline void AQSApplication::postEvent(QObject *arg0,  QEvent *arg1)
{
  AQ_CALL_VOID(postEvent(arg0, arg1));
}
inline void AQSApplication::postEvent(QObject *arg0, AQSEvent *arg1)
{
  AQ_CALL_VOID(postEvent(arg0, *arg1));
}
inline void AQSApplication::sendPostedEvents(QObject *arg0,  int arg1)
{
  AQ_CALL_VOID(sendPostedEvents(arg0, arg1));
}
inline void AQSApplication::sendPostedEvents()
{
  AQ_CALL_VOID(sendPostedEvents());
}
inline void AQSApplication::removePostedEvents(QObject *arg0)
{
  AQ_CALL_VOID(removePostedEvents(arg0));
}
inline bool AQSApplication::startingUp()
{
  AQ_CALL_RET_V(startingUp(), bool);
}
inline bool AQSApplication::closingDown()
{
  AQ_CALL_RET_V(closingDown(), bool);
}
inline void AQSApplication::setDesktopSettingsAware(bool arg0)
{
  AQ_CALL_VOID(setDesktopSettingsAware(arg0));
}
inline bool AQSApplication::desktopSettingsAware()
{
  AQ_CALL_RET_V(desktopSettingsAware(), bool);
}
inline void AQSApplication::setCursorFlashTime(int arg0)
{
  AQ_CALL_VOID(setCursorFlashTime(arg0));
}
inline int AQSApplication::cursorFlashTime()
{
  AQ_CALL_RET_V(cursorFlashTime(), int);
}
inline void AQSApplication::setDoubleClickInterval(int arg0)
{
  AQ_CALL_VOID(setDoubleClickInterval(arg0));
}
inline int AQSApplication::doubleClickInterval()
{
  AQ_CALL_RET_V(doubleClickInterval(), int);
}
inline void AQSApplication::setWheelScrollLines(int arg0)
{
  AQ_CALL_VOID(setWheelScrollLines(arg0));
}
inline int AQSApplication::wheelScrollLines()
{
  AQ_CALL_RET_V(wheelScrollLines(), int);
}
inline void AQSApplication::setGlobalStrut(const QSize &arg0)
{
  AQ_CALL_VOID(setGlobalStrut(arg0));
}
inline QSize AQSApplication::globalStrut()
{
  AQ_CALL_RET_V(globalStrut(), QSize);
}
inline void AQSApplication::setLibraryPaths(const QStringList &arg0)
{
  AQ_CALL_VOID(setLibraryPaths(arg0));
}
inline QStringList AQSApplication::libraryPaths()
{
  AQ_CALL_RET_V(libraryPaths(), QStringList);
}
inline void AQSApplication::addLibraryPath(const QString &arg0)
{
  AQ_CALL_VOID(addLibraryPath(arg0));
}
inline void AQSApplication::removeLibraryPath(const QString &arg0)
{
  AQ_CALL_VOID(removeLibraryPath(arg0));
}
inline void AQSApplication::setStartDragTime(int arg0)
{
  AQ_CALL_VOID(setStartDragTime(arg0));
}
inline int AQSApplication::startDragTime()
{
  AQ_CALL_RET_V(startDragTime(), int);
}
inline void AQSApplication::setStartDragDistance(int arg0)
{
  AQ_CALL_VOID(setStartDragDistance(arg0));
}
inline int AQSApplication::startDragDistance()
{
  AQ_CALL_RET_V(startDragDistance(), int);
}
inline void AQSApplication::setReverseLayout(bool arg0)
{
  AQ_CALL_VOID(setReverseLayout(arg0));
}
inline bool AQSApplication::reverseLayout()
{
  AQ_CALL_RET_V(reverseLayout(), bool);
}
inline int AQSApplication::horizontalAlignment(int arg0)
{
  AQ_CALL_RET_V(horizontalAlignment(arg0), int);
}
inline void AQSApplication::quit()
{
  AQ_CALL_VOID(quit());
}
inline void AQSApplication::closeAllWindows()
{
  AQ_CALL_VOID(closeAllWindows());
}
inline void AQSApplication::aboutQt()
{
  AQ_CALL_VOID(aboutQt());
}
inline void AQSApplication::reinit()
{
  AQ_CALL_VOID(reinit());
}
inline FLSqlDatabase *AQSApplication::db()
{
  AQ_CALL_RET(db());
}
inline void AQSApplication::setMainWidget(QWidget *arg0)
{
  AQ_CALL_VOID(setMainWidget(arg0));
}
inline QSArgument AQSApplication::call(const QString &arg0,  const QString &arg1) const
{
  AQ_CALL_RET_V(call(arg0, arg1), QSArgument);
}
inline void AQSApplication::activateModule(const QString &arg0)
{
  AQ_CALL_VOID(activateModule(arg0));
}
inline void AQSApplication::execMainScript(const QString &arg0)
{
  AQ_CALL_VOID(execMainScript(arg0));
}
inline void AQSApplication::aboutAbanQ()
{
  AQ_CALL_VOID(aboutAbanQ());
}
inline void AQSApplication::chooseFont()
{
  AQ_CALL_VOID(chooseFont());
}
inline void AQSApplication::setDatabaseLockDetection(bool arg0,  int arg1,  int arg2,  bool arg3,  const QString &arg4,  const QString &arg5)
{
  AQ_CALL_VOID(setDatabaseLockDetection(arg0, arg1, arg2, arg3, arg4, arg5));
}
inline QString AQSApplication::commaSeparator() const
{
  AQ_CALL_RET_V(commaSeparator(), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSAPPLICATION_P_H_ */
// @AQOBJECT@
