/***************************************************************************
AQApplication.h
-------------------
begin                : 06/08/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
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

#ifndef AQAPPLICATION_H_
#define AQAPPLICATION_H_

#include <signal.h>

#include <AQUi/WQWidget.h>

#include <QApplication>

class AQApplicationPrivate;
class AQApplicationWeb;
class AQApplicationWebPrivate;
class WQApplication;
class QDateTime;
class AQMainWindow;
class AQTabView;

namespace Wt
{
  class WApplication;
  class WEnvironment;
}

using namespace Wt;

/**
 Objeto aplicación para AbanQ.

 @author InfoSiAL S.L.
 */
class AQApplication : public WQWidget
{
  Q_OBJECT
  AQ_DECLARE_PRIVATE( AQApplication )

public :

  void addWQWidget( WQWidget * w );

  WQWidget * mainWidget() const;
  AQTabView * tabView() const;
  QDateTime * timeUser() const;

public Q_SLOTS:

  void newMainWindow();
  void closeMainWindow( AQMainWindow * win );
  void closeMainWindow();
  void closeOtherTabs();
  bool tryExit();
  void quit();

private Q_SLOTS:

  void serverExitRequest();

private:

  AQApplication( WQApplication * wqApp, WQWidget * parent = 0 );

  void interrupt();

  friend class WQApplication;
  friend class AQApplicationWeb;
  friend class AQApplicationWebPrivate;
};

class AQApplicationWeb : public QApplication
{
  Q_OBJECT

public:

  AQApplicationWeb( int &argc, char **argv, sigset_t * set );
  ~AQApplicationWeb();

  bool notify ( QObject * receiver, QEvent * event );

  void aboutAbanQ();

  static WApplication * createWApp( const WEnvironment& env );
  static WQApplication * wqAppInstance();

  static QString findIconCache( const QString & key );
  static void insertIconCache( const QString & key, const QString & val );

private:

  AQApplicationWebPrivate * d;

  friend class AQApplicationWebPrivate;
  friend class AQApplicationPrivate;
};

#endif /*AQAPPLICATION_H_*/
