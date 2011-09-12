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

#include "FLTableDB.h"

// Uso interno
class FLTableDBWidgetInterface: public QWidgetPlugin
{

public:

    FLTableDBWidgetInterface();

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

FLTableDBWidgetInterface::FLTableDBWidgetInterface() {}

QStringList FLTableDBWidgetInterface::keys() const
{
    QStringList list;

    list << "FLTableDB";
    return list;
}

QWidget *FLTableDBWidgetInterface::create( const QString & classname,
        QWidget * parent, const char *name )
{
    QWidget *w = 0;

    if ( classname == "FLTableDB" )
        w = new FLTableDB( parent, name );

    return w;
}

QString FLTableDBWidgetInterface::group( const QString & description ) const
{
    if ( description == "FLTableDB" )
        return "Database";
    return QString::null;
}

QIconSet FLTableDBWidgetInterface::iconSet( const QString & ) const
{
    return QIconSet( QPixmap::fromMimeSource( "datatabledb.png" ) );
}

QString FLTableDBWidgetInterface::includeFile( const QString & description ) const
{
    return QString::null;
}

QString FLTableDBWidgetInterface::toolTip( const QString & description ) const
{
    if ( description == "FLTableDB" )
        return "FLTableDB Widget";
    return QString::null;
}

QString FLTableDBWidgetInterface::whatsThis( const QString & description ) const
{
    if ( description == "FLTableDB" )
        return "A widget for data base tables";
    return QString::null;
}

bool FLTableDBWidgetInterface::isContainer( const QString & ) const
{
    return FALSE;
}

Q_EXPORT_PLUGIN( FLTableDBWidgetInterface )
