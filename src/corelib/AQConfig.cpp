/***************************************************************************
AQConfig.cpp
-------------------
begin                : 21/08/2008
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
#ifdef AQ_WUI
#include <AQUi/WQApplication.h>
#endif

#include <QCoreApplication>
#include <QDir>

#include "AQConfig.h"

QString AQConfig::aq_dir_mod;
QString AQConfig::aq_home;
QString AQConfig::aqdata;
QString AQConfig::aqprefix;
QString AQConfig::aqqtdir;
QString AQConfig::aqversion;

QString AQConfig::keybase() const
{
#ifdef AQ_WUI
  return static_cast<WQApplication *> ( WApplication::instance() ) ->keybase();
#else
  return "AbanQ/";
#endif
}

void AQConfig::init()
{
  aq_dir_mod = QLatin1String( "/modulos" );
  aq_home = QDir::homePath();
  aqdata = QDir::cleanPath( QCoreApplication::applicationDirPath() + QLatin1String( "/../share/abanq" ) );
  aqprefix = QDir::cleanPath( QCoreApplication::applicationDirPath() + QLatin1String( "/.." ) );
  aqqtdir = aq_home + QLatin1String( "/workspace/abanq/install" );
  aqversion = QLatin1String( "3.0" );
}
