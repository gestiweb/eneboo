/***************************************************************************
AQSqlConnections.cpp
-------------------
begin                : 08/08/2007
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

#include <QHash>
#include <QMutex>

#include <AQCore/AQCore.h>

#include "AQSqlConnections.h"
#include "AQSqlDatabase.h"

class AQSqlConnectionsPrivate
{
public:

  AQSqlConnectionsPrivate()
  {}

  void replace( const QString & connectionName, AQSqlDatabase db );
  void clearHashDB();
  AQSqlDatabase value( const QString & connectionName );

  AQSqlDatabase database( const QString & connectionName );
  bool removeDatabase( const QString & connectionName );

  QHash<QString, AQSqlDatabase> hashDB;
  QMutex mutexDB;
};

AQSqlConnectionsPrivate * AQSqlConnections::d_ = 0;

void AQSqlConnectionsPrivate::clearHashDB()
{
  QMutexLocker lock( &mutexDB );
  while ( !hashDB.isEmpty() ) {
    hashDB.begin().value().closeDB();
    hashDB.erase( hashDB.begin() );
  }
}

AQSqlDatabase AQSqlConnectionsPrivate::value( const QString & connectionName )
{
  QMutexLocker lock( &mutexDB );
  return hashDB.value( connectionName, AQSqlDatabase() );
}

void AQSqlConnectionsPrivate::replace( const QString & connectionName, AQSqlDatabase db )
{
  QMutexLocker lock( &mutexDB );
  QHash<QString, AQSqlDatabase>::iterator it = hashDB.find( connectionName );
  if ( it != hashDB.end() ) {
    if ( it.value() != db ) {
      it.value().closeDB();
      hashDB.erase( it );
      hashDB.insert( connectionName, db );
    }
  } else
    hashDB.insert( connectionName, db );
}

AQSqlDatabase AQSqlConnectionsPrivate::database( const QString & connectionName )
{
  QMutexLocker lock( &mutexDB );
  QHash<QString, AQSqlDatabase>::iterator it = hashDB.find( connectionName );
  if ( it == hashDB.end() )
    it = hashDB.insert( connectionName, AQSqlDatabase() );
  return it.value();
}

bool AQSqlConnectionsPrivate::removeDatabase( const QString & connectionName )
{
  QMutexLocker lock( &mutexDB );
  QHash<QString, AQSqlDatabase>::iterator it = hashDB.find( connectionName );
  if ( it != hashDB.end() ) {
    it.value().closeDB();
    hashDB.erase( it );
    return true;
  }
  return false;
}

AQSqlDatabase AQSqlConnections::database( const QString & connectionName )
{
  return d_ ? d_->database( connectionName ) : AQSqlDatabase();
}

bool AQSqlConnections::removeDatabase( const QString & connectionName )
{
  return d_ ? d_->removeDatabase( connectionName ) : false;
}

bool AQSqlConnections::addDatabase( const QString & driverAlias, const QString & nameDB,
                                    const QString & user, const QString & password,
                                    const QString & host, int port, const QString & connectionName )
{
  if ( !d_ )
    return false;
  AQSqlDatabase db;
  if ( !db.loadDriver( AQSqlDatabase::driverAliasToDriverName( driverAlias ), connectionName ) ) {
    aqCrit( "AQSqlConnections::addDatabase : Driver no cargado " + driverAlias );
    return false;
  }
  if ( !db.connectDB( nameDB, user, password, host, port, connectionName ) ) {
    aqCrit( "AQSqlConnections::addDatabase : No se pudo conectar a " + nameDB );
    return false;
  }
  d_->replace( connectionName, db );
  return true;
}

void AQSqlConnections::init()
{
  if ( !d_ )
    d_ = new AQSqlConnectionsPrivate;
}

void AQSqlConnections::close()
{
  if ( d_ ) {
    d_->clearHashDB();
    delete d_;
    d_ = 0;
  }
}
