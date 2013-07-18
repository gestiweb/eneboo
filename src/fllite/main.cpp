/***************************************************************************
 main.cpp
 -------------------
 begin                : lun mar 19 13:44:14 CET 2001
 copyright            : (C) 2001-2005 by InfoSiAL, S.L.
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

#include "qplatformdefs.h"

extern "C" {
#include <stdlib.h>
#if !defined(FL_DEBUG) && !defined(Q_OS_MACX)
#include <execinfo.h>
#include <signal.h>
#endif
}

#include <qsplashscreen.h>

#include "FLConnectDBDialog.h"
#include "FLDiskCache.h"
#include "FLSqlDatabase.h"
#include "FLSqlConnections.h"
#include "FLSettings.h"

#include "AQApplication.h"
#include "AQConfig.h"

#if !defined(FL_DEBUG) && !defined(Q_OS_MACX)
bool hasSilentConn = false;
#if defined(Q_OS_WIN32)
HINSTANCE aqkernel32 = NULL;
#endif

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

  static void exitHandler(QT_SIGNAL_ARGS)
  {
    int j, nptrs;
#define SIZE 16
    void *buffer[SIZE];
    char **strings;

    nptrs = backtrace(buffer, SIZE);
    printf("backtrace() returned %d addresses\n", nptrs);

    // The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
    //    would produce similar output to the following:

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
      perror("backtrace_symbols");
      exit(EXIT_FAILURE);
    }

    QString log;
    log = QString("backtrace() returned %1 addresses").arg(nptrs) + '\n';
    for (j = 0; j < nptrs; j++) {
      printf("%s\n", strings[j]);
      log += QString(strings[j]) + '\n';
    }

    free(strings);

    if (!hasSilentConn) {
      QMessageBox::critical(0, "AbanQ",
                            "AbanQ ha detectado un error y debe cerrarse.\n\n" + log,
                            QMessageBox::Ok, QMessageBox::NoButton);
    }
    printf("Terminate AbanQ %p\n", aqApp);
    if (aqApp)
      delete aqApp;
    exit(EXIT_FAILURE);
  }

#if defined(Q_C_CALLBACKS)
}
#endif
#endif

static inline bool silentConnect(const QString &conn)
{
  if (conn.isEmpty())
    return false;

#if !defined(FL_DEBUG) && !defined(Q_OS_MACX)
  hasSilentConn = true;
#endif

  QString user, namedb, db, host, port, password, connOpts;
  QStringList dat = QStringList::split(":", conn, true);
  int i = 0;
  for (QStringList::Iterator it = dat.begin(); it != dat.end(); ++it, ++i) {
    switch (i) {
      case 0:
        namedb = *it;
        break;
      case 1:
        user = *it;
        break;
      case 2:
        db = *it;
        break;
      case 3:
        host = *it;
        break;
      case 4:
        port = *it;
        break;
      case 5:
        password = *it;
        break;
      case 6:
        connOpts = *it;
        break;
    }
  }

  FLSqlDatabase *sqlDb = new FLSqlDatabase();
  QString driverName(FLSqlDatabase::driverAliasToDriverName(db));

  if (!sqlDb->loadDriver(driverName)) {
    delete sqlDb;
    return false;
  }

  if (driverName == "FLQPSQL7") {
    if (connOpts.isEmpty())
      connOpts = "connect_timeout=30";
    else
      connOpts += ";connect_timeout=30";
  }

  if (!sqlDb->connectDB(namedb, user, password, host, port.toInt(),
                        "default", connOpts)) {
    delete sqlDb;
    return false;
  }

  FLSqlConnections::addDatabase(sqlDb);
  return true;
}

AQ_DECL_MAIN {
  QPalette p(QColor(229, 229, 229), QColor(229, 229, 229));
  p.setColor(QPalette::Active, QColorGroup::Dark, QColor(246, 242, 246));
  p.setColor(QPalette::Inactive, QColorGroup::Dark, QColor(246, 242, 246));
  p.setColor(QPalette::Disabled, QColorGroup::Dark, QColor(246, 242, 246));
  p.setColor(QPalette::Active, QColorGroup::Base, QColor(255, 255, 248));
  p.setColor(QPalette::Inactive, QColorGroup::Base, QColor(255, 255, 248));
  p.setColor(QPalette::Disabled, QColorGroup::Base, QColor(255, 255, 248));
  p.setColor(QPalette::Active, QColorGroup::Highlight, QColor(95, 150, 205));
  p.setColor(QPalette::Active, QColorGroup::HighlightedText, Qt::white);
  p.setColor(QPalette::Active, QColorGroup::Highlight, QColor(95, 150, 205));
  p.setColor(QPalette::Inactive, QColorGroup::HighlightedText, Qt::white);
  p.setColor(QPalette::Active, QColorGroup::Highlight, QColor(95, 150, 205));
  p.setColor(QPalette::Disabled, QColorGroup::HighlightedText, Qt::white);
  p.setColor(QPalette::Inactive, QColorGroup::Text, Qt::black);
  p.setColor(QPalette::Disabled, QColorGroup::Text, QColor(100, 100, 100));
  p.setColor(QPalette::Active, QColorGroup::Foreground, QColor(30, 30, 30));
  p.setColor(QPalette::Inactive, QColorGroup::Foreground, QColor(30, 30, 30));
  p.setColor(QPalette::Disabled, QColorGroup::Foreground, QColor(100, 100, 100));
  p.setColor(QPalette::Active, QColorGroup::ButtonText, QColor(30, 30, 30));
  p.setColor(QPalette::Inactive, QColorGroup::ButtonText, QColor(30, 30, 30));
  p.setColor(QPalette::Disabled, QColorGroup::ButtonText, QColor(100, 100, 100));

  QString formAlone, callFunction, arguments, strConn, silentConn;
  bool quitAfterCall = false, autoLogin_ = false, noMax = false;

  for (int i = 1; i < argc; i++) {
    if (qstrcmp(argv[i], "-f") == 0) {
      if (i != argc - 1) {
        i++;
        formAlone = QString::fromLatin1(argv[i]);
      }
    } else if (qstrcmp(argv[i], "-c") == 0) {
      if (i != argc - 1) {
        i++;
        callFunction = QString::fromLatin1(argv[i]);
      }
    } else if (qstrcmp(argv[i], "-a") == 0) {
      if (i != argc - 1) {
        i++;
        arguments = QString::fromLatin1(argv[i]);
      }
    } else if (qstrcmp(argv[i], "-sc") == 0) {
      if (i != argc - 1) {
        i++;
        strConn = QString::fromLatin1(argv[i]);
      }
    } else if (qstrcmp(argv[i], "-silentconn") == 0) {
      if (i != argc - 1) {
        i++;
        silentConn = QString::fromLatin1(argv[i]);
      }
    } else if (qstrcmp(argv[i], "-q") == 0) {
      quitAfterCall = true;
    } else if (qstrcmp(argv[i], "-auto") == 0) {
      autoLogin_ = true;
    } else if (qstrcmp(argv[i], "-nomax") == 0) {
      noMax = true;
    }
  }

#if !defined(FL_DEBUG) && !defined(Q_OS_MACX)
  signal(SIGABRT, exitHandler);
  signal(SIGFPE, exitHandler);
  signal(SIGILL, exitHandler);
  signal(SIGINT, exitHandler);
  signal(SIGSEGV, exitHandler);
  signal(SIGTERM, exitHandler);
#if defined(Q_OS_WIN32)
  QString fileName("Kernel32.dll");
  aqkernel32 = LoadLibrary((wchar_t *)fileName.ucs2());
#endif
#endif

  FLApplication *AbanQ = aqApp;
  QFont appFont;

  QStringList paths(AbanQ->libraryPaths());
  QString pathApp(AQ_PREFIX + "/plugins");
  if (!paths.contains(pathApp))
    paths << pathApp;
  if (!paths.contains(AQ_LIB))
    paths << AQ_LIB;
  AbanQ->setLibraryPaths(paths);

  int pointSize = 8;

#if defined (Q_OS_LINUX)
  appFont.setFamily(FLSettings::readEntry("font/family", "Verdana"));
#endif

#if defined(Q_OS_WIN32)
  appFont.setFamily(FLSettings::readEntry("font/family", "Tahoma"));
#endif

#if defined(Q_OS_MACX)
  appFont.setFamily(FLSettings::readEntry("font/family", "Lucida Grande"));
  pointSize = 12;
  QString envPath(getenv("PATH"));
  envPath = "/sw/sbin:/sw/bin:" + envPath + ":/usr/X11R6/bin";
  setenv("PATH", envPath.local8Bit(), 1);
#endif

  appFont.setPointSize(FLSettings::readNumEntry("font/pointSize", pointSize));
  appFont.setBold(FLSettings::readBoolEntry("font/bold", false));
  appFont.setItalic(FLSettings::readBoolEntry("font/italic", false));
  appFont.setUnderline(FLSettings::readBoolEntry("font/underline", false));
  appFont.setStrikeOut(FLSettings::readBoolEntry("font/strikeOut", false));

  AbanQ->setFont(appFont);
  AbanQ->setStyle(FLSettings::readEntry("style", "QtCurve"));

  AbanQ->installTranslator(AbanQ->createSysTranslator(QString(QTextCodec::locale()).left(2), true));
  AbanQ->installTranslator(AbanQ->createSysTranslator("multilang"));
  AbanQ->setPalette(p, true);

  FLConnectDBDialog *s = 0;
  QSplashScreen *splash = 0;

  if (silentConn.isEmpty()) {
    bool autoLogin = FLSettings::readBoolEntry("application/autoLogin", autoLogin_);
    s = new FLConnectDBDialog(autoLogin, 0, "FLConnectDBDialog", 0, strConn);

    static_cast<QWidget *>(s->child("frMore"))->hide();
    s->adjustSize();
    s->setPalette(p);
    int ret = -1;
    bool connectAttempts = 0;

    do {
      if (!s->isShown()) {
        s->show();
        s->raise();
      }
      if (autoLogin && !connectAttempts) {
        QTimer::singleShot(1000, s, SLOT(tryConnect()));
        connectAttempts++;
      } else
        s->exec();
    } while (s->error());

    ret = s->result();
    QTimer::singleShot(0, s, SLOT(deleteLater()));

    if (ret != QDialog::Accepted) {
      QTimer::singleShot(0, AbanQ, SLOT(quit()));
      return;
    }

    splash = new QSplashScreen(QPixmap::fromMimeSource("splashabanq.png"));
    splash->show();
    splash->message(QT_TR_NOOP("Inicializando..."), Qt::AlignRight, QColor(0, 0, 0));
  } else if (!silentConnect(silentConn)) {
    QTimer::singleShot(0, AbanQ, SLOT(quit()));
    return;
  }

  AbanQ->setNotExit(FLSettings::readBoolEntry("application/notExit", false));
  AbanQ->setPrintProgram(FLSettings::readEntry("printing/printProgram", QString::null));
  AbanQ->flushX();
  AbanQ->syncX();
  AbanQ->processEvents();
  AbanQ->init(
    FLSettings::readEntry("application/formAlone", formAlone),
    FLSettings::readEntry("application/callFunction", callFunction),
    FLSettings::readEntry("application/arguments", arguments),
    FLSettings::readBoolEntry("application/quitAfterCall", quitAfterCall),
    FLSettings::readBoolEntry("application/noMax", noMax)
  );

  if (AbanQ->mainWidget()) {
    AbanQ->mainWidget()->show();
    AbanQ->mainWidget()->raise();
    if (splash) {
      splash->finish(AbanQ->mainWidget());
      delete splash;
    }
  } else if (splash)
    QTimer::singleShot(0, splash, SLOT(deleteLater()));
}

int main(int argc, char **argv)
{
  AQApplication app(argc, argv);
  AQ_IMPL_MAIN(argc, argv);
  return app.exec();
}
