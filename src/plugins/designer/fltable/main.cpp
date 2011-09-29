/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : jue abr 23 2009
    copyright            : (C) 2001-2009 by InfoSiAL S.L.
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

#include "FLTable.h"

// Uso interno
class FLTableWidgetInterface: public QWidgetPlugin
{

public:

    FLTableWidgetInterface();

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

FLTableWidgetInterface::FLTableWidgetInterface() {}

QStringList FLTableWidgetInterface::keys() const
{
    QStringList list;

    list << "FLTable";
    return list;
}

QWidget *FLTableWidgetInterface::create( const QString & classname,
        QWidget * parent, const char *name )
{
    QWidget *w = 0;

    if ( classname == "FLTable" )
        w = new FLTable( parent, name );

    return w;
}

QString FLTableWidgetInterface::group( const QString & description ) const
{
    if ( description == "FLTable" )
        return "Database";
    return QString::null;
}

QIconSet FLTableWidgetInterface::iconSet( const QString & ) const
{
    return QIconSet( QPixmap::fromMimeSource( "designer_table.png" ) );
}

QString FLTableWidgetInterface::includeFile( const QString & description ) const
{
    return QString::null;
}

QString FLTableWidgetInterface::toolTip( const QString & description ) const
{
    if ( description == "FLTable" )
        return "FLTable Widget";
    return QString::null;
}

QString FLTableWidgetInterface::whatsThis( const QString & description ) const
{
    if ( description == "FLTable" )
        return "A widget for tables";
    return QString::null;
}

bool FLTableWidgetInterface::isContainer( const QString & ) const
{
    return FALSE;
}

Q_EXPORT_PLUGIN( FLTableWidgetInterface )
