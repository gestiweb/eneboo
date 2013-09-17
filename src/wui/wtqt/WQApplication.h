/***************************************************************************
WQApplication.h
-------------------
begin                : 19/06/2008
copyright            : (C) 2003-2008 by InfoSiAL S.L.
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

#ifndef WQAPPLICATION_H_
#define WQAPPLICATION_H_

#include <WApplication>
#include <WLogger>

#include <QGenericArgument>
#include <QString>
#include <QQueue>
#include <QMutex>

class QThread;
class AQApplication;
class AQApplicationWeb;
class WQEventLoop;
class AQInvocation;

using namespace Wt;

class WQApplication : public WApplication
{
public:

  WQApplication( const WEnvironment& env );

  QString keybase() const
  {
    return keybase_;
  }

  static void invokeMethod( QObject * receiver, const char * member, Qt::ConnectionType type = Qt::AutoConnection,
                            QGenericArgument val0 = QGenericArgument( 0 ), QGenericArgument val1 = QGenericArgument() );

  void releaseInvocationLast();

private:

  void initialize();
  void finalize();
  void notify( const WEvent& ev );

  void createInvocation();
  void waitInvocation();
  void lockInvocation();
  void releaseInvocationHead();

  AQApplication * aqApp_;
  WQEventLoop * eventLoop_;
  QString keybase_;

  QQueue<AQInvocation *> invocations_;
  QMutex queueMutex_;

  friend class WQEventLoop;
  friend class AQApplicationWeb;
};

#endif /*WQAPPLICATION_H_*/
