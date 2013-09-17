/***************************************************************************
                          main.cpp
                      -------------------
begin                : 23/08/2007
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

#include <qsqldriverplugin.h>
#include <qstringlist.h>
#include "../../../sql/drivers/psql/AQSqlPSQL.h"

class QPSQLDriverPlugin : public QSqlDriverPlugin
{
public:
    QPSQLDriverPlugin();

    QSqlDriver * create( const QString & );
    QStringList keys() const;
};

QPSQLDriverPlugin::QPSQLDriverPlugin()
        : QSqlDriverPlugin()
{}

QSqlDriver * QPSQLDriverPlugin::create( const QString &name )
{
    if ( name == QLatin1String( "AQPSQL" ) ) {
        AQPSQLDriver * driver = new AQPSQLDriver();
        return driver;
    }
    return 0;
}

QStringList QPSQLDriverPlugin::keys() const
{
    QStringList l;
    l.append( QLatin1String( "AQPSQL" ) );
    return l;
}

Q_EXPORT_PLUGIN2( AQPSQLPlugin, QPSQLDriverPlugin )
