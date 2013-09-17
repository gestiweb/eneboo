/***************************************************************************
                       AQFieldDBPlugin.cpp
                   -------------------
begin                : 30/08/2007
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

#include "AQFieldDBPlugin.h"

#include <QtCore/qplugin.h>

#include <AQUi/AQFieldDB.h>

AQFieldDBPlugin::AQFieldDBPlugin( QObject * parent, bool initAsFLFieldDB )
        : QObject( parent ),
        initialized( false ), initAsFLFieldDB_( initAsFLFieldDB )
{}

AQFieldDBPlugin::~AQFieldDBPlugin()
{}

QString AQFieldDBPlugin::name() const
{
    if ( initAsFLFieldDB_ )
        return QLatin1String( "FLFieldDB" );
    else
        return QLatin1String( "AQFieldDB" );
}

QString AQFieldDBPlugin::group() const
{
    return QLatin1String( "AbanQ" );
}

QString AQFieldDBPlugin::toolTip() const
{
    return QString();
}

QString AQFieldDBPlugin::whatsThis() const
{
    return QString();
}

QString AQFieldDBPlugin::includeFile() const
{
    return QLatin1String( "AQUi/AQFieldDB" );
}

QIcon AQFieldDBPlugin::icon() const
{
    if ( initAsFLFieldDB_ )
        return QIcon( ":/images/dataline.png" );
    else
        return QIcon( ":/images/lineedit.png" );
}

bool AQFieldDBPlugin::isContainer() const
{
    return true;
}

QWidget * AQFieldDBPlugin::createWidget( QWidget * parent )
{
    if ( initAsFLFieldDB_ )
        return new FLFieldDB( parent );
    else
        return new AQFieldDB( parent );
}

bool AQFieldDBPlugin::isInitialized() const
{
    return initialized;
}

void AQFieldDBPlugin::initialize( QDesignerFormEditorInterface * core )
{
    Q_UNUSED( core );
    initialized = true;
}

FLFieldDBPlugin::FLFieldDBPlugin( QObject * parent )
        : AQFieldDBPlugin( parent, true )
{}

FLFieldDBPlugin::~FLFieldDBPlugin()
{}
