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

// C Libraries ----
#include <stdlib.h>
#include <stdio.h>
// ----

#include <qsplashscreen.h>

#include "../flbase/FLConnectDBDialog.h"
#include "../flbase/FLDiskCache.h"
#include "../flbase/FLSqlDatabase.h"
#include "../flbase/FLSqlConnections.h"
#include "../flbase/FLSettings.h"

#include "../flbase/AQApplication.h"
#include "../../AQConfig.h"

static inline bool silentConnect(const QString &conn)
{
  if (conn.isEmpty())
    return false;

  QString user, namedb, db, host, port, password;
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
    }
  }

  FLSqlDatabase *sqlDb = new FLSqlDatabase();

  if (!sqlDb->loadDriver(FLSqlDatabase::driverAliasToDriverName(db))) {
    delete sqlDb;
    return false;
  }

  if (!sqlDb->connectDB(namedb, user, password, host, port.toInt())) {
    delete sqlDb;
    return false;
  }

  FLSqlConnections::addDatabase(sqlDb);
  return true;
}

void aq_main(int argc, char **argv) 
{
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
    //Buscamos splashScreen Externo
    QImage img(AQ_PREFIX + "/share/eneboo/splashscreen.png");
      if(!img.isNull()) // Load succeded ?
             splash = new QSplashScreen(QPixmap(img)); 
               else
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
  } else {
    if (splash) {
      QTimer::singleShot(0, splash, SLOT(deleteLater()));
    }
  }
    
}

int main(int argc, char **argv)
{
  int retval = 0;
  AQApplication app(argc, argv);
  aq_main(argc, argv);
  retval = app.exec();
  exit(retval);
  return retval;
}
