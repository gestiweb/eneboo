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

#include "FLSqlConnections.h"
#include "FLSqlDatabase.h"

class FLSqlConnectionsPrivate
{
public:
  FLSqlConnectionsPrivate();
  ~FLSqlConnectionsPrivate();

  QDict<FLSqlDatabase> *dictDB;
  FLSqlDatabase *defaultDB;
};

AQ_EXPORT FLSqlConnectionsPrivate *FLSqlConnections::d = 0;

FLSqlConnectionsPrivate::FLSqlConnectionsPrivate()
{
  defaultDB = 0;
  dictDB = new QDict<FLSqlDatabase>(101);
  dictDB->setAutoDelete(true);
}

FLSqlConnectionsPrivate::~FLSqlConnectionsPrivate()
{
  defaultDB = 0;
  delete dictDB;
}

bool FLSqlConnections::addDatabase(FLSqlDatabase *db, const QString &connectionName)
{
  if (!d)
    d = new FLSqlConnectionsPrivate();
  if (!db)
    return false;
  FLSqlDatabase *newDb = d->dictDB->find(connectionName);
  if (newDb == db)
    return true;
  d->dictDB->insert(connectionName, db);
  if (d->defaultDB != db && connectionName == "default")
    d->defaultDB = db;
  return true;
}

FLSqlDatabase *FLSqlConnections::database(const QString &connectionName)
{
  if (!d)
    d = new FLSqlConnectionsPrivate();
  if (connectionName == "default") {
    if (!d->defaultDB)
      addDatabase(new FLSqlDatabase());
    return d->defaultDB;
  }
  if (d->dictDB->isEmpty()) {
    addDatabase(new FLSqlDatabase());
    return d->defaultDB;
  }
  FLSqlDatabase *ret = d->dictDB->find(connectionName);
  if (!ret) {
#ifdef FL_DEBUG
    qWarning(QString("FLSqlConnections::database : No existe la conexión '%1',"
                     " se devuelve la conexión por defecto 'default'").arg(connectionName));
#endif
    if (!d->defaultDB)
      addDatabase(new FLSqlDatabase());
    ret = d->defaultDB;
  }
  return ret;
}

bool FLSqlConnections::removeDatabase(const QString &connectionName)
{
  if (!d || d->dictDB->isEmpty())
    return false;
  if (connectionName == "default")
    d->defaultDB = 0;
  return d->dictDB->remove(connectionName);
}

bool FLSqlConnections::addDatabase(const QString &driverAlias, const QString &nameDB,
                                   const QString &user, const QString &password, const QString &host,
                                   int port, const QString &connectionName, const QString &connectOptions)
{
  FLSqlDatabase *db = new FLSqlDatabase();
  if (!db->loadDriver(FLSqlDatabase::driverAliasToDriverName(driverAlias), connectionName)) {
    delete db;
#ifdef FL_DEBUG
    qWarning("FLSqlConnections::addDatabase : Driver no cargado " + driverAlias);
#endif
    return false;
  }
  if (!db->connectDB(nameDB, user, password, host, port, connectionName, connectOptions)) {
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
  d = 0;
}

QDict<FLSqlDatabase> *FLSqlConnections::dictDatabases()
{
  if (d && d->dictDB)
    return d->dictDB;
  return 0;
}
