/***************************************************************************
                          FLSqlConnections.cpp
                          -------------------
 begin                : Mon Sep 18 2006
 copyright            : (C) 2006 by InfoSiAL S.L.
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

#include <qdict.h>

#include "FLSqlConnections.h"
#include "FLSqlDatabase.h"

class FLSqlConnectionsPrivate
{
public:
  FLSqlConnectionsPrivate();
  ~FLSqlConnectionsPrivate();

  QDict < FLSqlDatabase > *dictDB;
};

FL_EXPORT FLSqlConnectionsPrivate *FLSqlConnections::d = 0;

FLSqlConnectionsPrivate::FLSqlConnectionsPrivate()
{
  dictDB = new QDict<FLSqlDatabase>;
  dictDB->setAutoDelete(true);
}

FLSqlConnectionsPrivate::~ FLSqlConnectionsPrivate()
{
  delete dictDB;
}

bool FLSqlConnections::addDatabase(FLSqlDatabase *db, const QString &connectionName)
{
  if (!d)
    d = new FLSqlConnectionsPrivate();
  if (!db)
    return false;
  d->dictDB->replace(connectionName, db);
  return true;
}

FLSqlDatabase *FLSqlConnections::database(const QString &connectionName)
{
  if (!d)
    d = new FLSqlConnectionsPrivate();
  if (!d->dictDB)
    return 0;
  if (d->dictDB->isEmpty())
    d->dictDB->replace("default", new FLSqlDatabase());
  return (*d->dictDB)[connectionName];
}

bool FLSqlConnections::removeDatabase(const QString &connectionName)
{
  if (!d || !d->dictDB || d->dictDB->isEmpty())
    return false;
  return d->dictDB->remove(connectionName);
}

bool FLSqlConnections::addDatabase(const QString &driverAlias, const QString &nameDB, const QString &user,
                                   const QString &password, const QString &host, int port, const QString &connectionName)
{
  FLSqlDatabase *db = new FLSqlDatabase();
  if (!db->loadDriver(FLSqlDatabase::driverAliasToDriverName(driverAlias), connectionName)) {
    delete db;
#ifdef FL_DEBUG
    qWarning("FLSqlConnections::addDatabase : Driver no cargado " + driverAlias);
#endif
    return false;
  }
  if (!db->connectDB(nameDB, user, password, host, port, connectionName)) {
    delete db;
#ifdef FL_DEBUG
    qWarning("FLSqlConnections::addDatabase : No se pudo conectar a " + nameDB);
#endif
    return false;
  }
  return addDatabase(db, connectionName);
}

void FLSqlConnections::finish()
{
  delete d;
}
