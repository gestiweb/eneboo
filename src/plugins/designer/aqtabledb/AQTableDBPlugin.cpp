/***************************************************************************
                            AQTableDBPlugin.cpp
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

#include "AQTableDBPlugin.h"

#include <QtCore/qplugin.h>

#include <AQUi/AQTableDB.h>

AQTableDBPlugin::AQTableDBPlugin( QObject * parent, bool initAsFLTableDB )
        : QObject( parent ),
        initialized( false ), initAsFLTableDB_( initAsFLTableDB )
{}

AQTableDBPlugin::~AQTableDBPlugin()
{}

QString AQTableDBPlugin::name() const
{
    if ( initAsFLTableDB_ )
        return QLatin1String( "FLTableDB" );
    else
        return QLatin1String( "AQTableDB" );
}

QString AQTableDBPlugin::group() const
{
    return QLatin1String( "AbanQ" );
}

QString AQTableDBPlugin::toolTip() const
{
    return QString();
}

QString AQTableDBPlugin::whatsThis() const
{
    return QString();
}

QString AQTableDBPlugin::includeFile() const
{
    return QLatin1String( "AQUi/AQTableDB" );
}

QIcon AQTableDBPlugin::icon() const
{
    if ( initAsFLTableDB_ )
        return QIcon( ":/images/datatabledb.png" );
    else
        return QIcon( ":/images/table.png" );
}

bool AQTableDBPlugin::isContainer() const
{
    return true;
}

QWidget * AQTableDBPlugin::createWidget( QWidget * parent )
{
    if ( initAsFLTableDB_ )
        return new FLTableDB( parent );
    else
        return new AQTableDB( parent );
}

bool AQTableDBPlugin::isInitialized() const
{
    return initialized;
}

void AQTableDBPlugin::initialize( QDesignerFormEditorInterface * core )
{
    Q_UNUSED( core );
    initialized = true;
}

FLTableDBPlugin::FLTableDBPlugin( QObject * parent )
        : AQTableDBPlugin( parent, true )
{}

FLTableDBPlugin::~FLTableDBPlugin()
{}
