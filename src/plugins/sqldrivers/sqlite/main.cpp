/***************************************************************************
                         main.cpp  -  description
                            -------------------
   begin                : mie Dic 3 2003
   copyright            : (C) 2003-2004 by InfoSiAL S.L.
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

#include <qsqldriverplugin.h>
#include "qsqlite.h"

class SqlitePlugin : public QSqlDriverPlugin
{
public:

    SqlitePlugin();
    QSqlDriver* create( const QString & );
    QStringList keys() const;
};

SqlitePlugin::SqlitePlugin() : QSqlDriverPlugin() {}

QSqlDriver* SqlitePlugin::create( const QString &name )
{
    if ( name == "FLsqlite" ) {
        SqliteDriver * driver = new SqliteDriver();
        return driver;
    }
    return 0;
}

QStringList SqlitePlugin::keys() const
{
    QStringList l;
    l.append( "FLsqlite" );
    return l;
}

Q_EXPORT_PLUGIN( SqlitePlugin )
