/***************************************************************************
                          AQ3MainWindowPlugin.cpp
                      -------------------
begin                : 11/09/2007
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

#include "AQ3MainWindowPlugin.h"

#include <QtCore/qplugin.h>

#include <AQUi/AQ3MainWindow.h>

AQ3MainWindowPlugin::AQ3MainWindowPlugin( QObject * parent )
        : QObject( parent ), initialized( false )
{}

AQ3MainWindowPlugin::~AQ3MainWindowPlugin()
{}

QString AQ3MainWindowPlugin::name() const
{
    return QLatin1String( "AQ3MainWindow" );
}

QString AQ3MainWindowPlugin::group() const
{
    return QLatin1String( "AbanQ" );
}

QString AQ3MainWindowPlugin::toolTip() const
{
    return QString();
}

QString AQ3MainWindowPlugin::whatsThis() const
{
    return QString();
}

QString AQ3MainWindowPlugin::includeFile() const
{
    return QLatin1String( "AQUi/AQ3MainWindow" );
}

QIcon AQ3MainWindowPlugin::icon() const
{
    return QIcon();
}

bool AQ3MainWindowPlugin::isContainer() const
{
    return true;
}

QWidget * AQ3MainWindowPlugin::createWidget( QWidget * parent )
{
    return new AQ3MainWindow( parent );
}

bool AQ3MainWindowPlugin::isInitialized() const
{
    return initialized;
}

void AQ3MainWindowPlugin::initialize( QDesignerFormEditorInterface * core )
{
    Q_UNUSED( core );
    initialized = true;
}

