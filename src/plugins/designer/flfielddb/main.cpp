/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sun Jul 1 2001
    copyright            : (C) 2001-2004 by InfoSiAL S.L.
    email                : mail@infosial.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qwidgetplugin.h>

#include "FLFieldDB.h"

// Uso interno
class FLFieldDBWidgetInterface: public QWidgetPlugin
{

public:

    FLFieldDBWidgetInterface();

    QStringList keys() const;
    QWidget *create( const QString & classname, QWidget * parent =
                         0, const char *name = 0 );
    QString group( const QString & ) const;
    QIconSet iconSet( const QString & ) const;
    QString includeFile( const QString & ) const;
    QString toolTip( const QString & ) const;
    QString whatsThis( const QString & ) const;
    bool isContainer( const QString & ) const;
};

FLFieldDBWidgetInterface::FLFieldDBWidgetInterface() {}

QStringList FLFieldDBWidgetInterface::keys() const
{
    QStringList list;

    list << "FLFieldDB";
    return list;
}

QWidget *FLFieldDBWidgetInterface::create( const QString & classname,
        QWidget * parent, const char *name )
{
    QWidget *w = 0;

    if ( classname == "FLFieldDB" )
        w = new FLFieldDB( parent, name );

    return w;
}

QString FLFieldDBWidgetInterface::group( const QString & description ) const
{
    if ( description == "FLFieldDB" )
        return "Database";
    return QString::null;
}

QIconSet FLFieldDBWidgetInterface::iconSet( const QString & ) const
{
    return QIconSet( QPixmap::fromMimeSource( "dataline.png" ) );
}

QString FLFieldDBWidgetInterface::includeFile( const QString & description ) const
{
    return QString::null;
}

QString FLFieldDBWidgetInterface::toolTip( const QString & description ) const
{
    if ( description == "FLFieldDB" )
        return "FLFieldDB Widget";
    return QString::null;
}

QString FLFieldDBWidgetInterface::whatsThis( const QString & description ) const
{
    if ( description == "FLFieldDB" )
        return "A widget for data base fields";
    return QString::null;
}

bool FLFieldDBWidgetInterface::isContainer( const QString & ) const
{
    return FALSE;
}

Q_EXPORT_PLUGIN( FLFieldDBWidgetInterface )
