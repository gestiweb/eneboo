/***************************************************************************
AQSqlDatabase.cpp
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

#include <QDateTime>
#include <QSqlError>

#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQTableMD.h>
#include <AQScript/AQScriptEngineQSA.h>

#include "AQSqlDatabase.h"
#include "AQSqlDriver.h"
#include "AQSqlCursor.h"
#include "manager/AQManager.h"
#include "manager/AQManagerModules.h"
#include "qt3support/q3sqlcursor.h"

AQSqlDatabasePrivate::AQSqlDatabasePrivate()
    : dr_( 0 ),
    drAux_( 0 ),
    port_( -1 ),
    connectionName_( "default" )
{}

AQSqlDatabasePrivate::AQSqlDatabasePrivate( const AQSqlDatabasePrivate & other )
    : QSharedData( other )
{
  db_ = other.db_;
  dbConnName_ = other.dbConnName_;
  dr_ = other.dr_;
  drAux_ = other.drAux_;
  dbAux_ = other.dbAux_;
  dbAuxConnName_ = other.dbAuxConnName_;
  database_ = other.database_;
  user_ = other.user_;
  password_ = other.password_;
  host_ = other.host_;
  port_ = other.port_;
  driverName_ = other.driverName_;
  connectionName_ = other.connectionName_;
  manager_ = other.manager_;
  managerModules_ = other.managerModules_;
  scriptEngine_ = other.scriptEngine_;
  idSessionGlobal_ = other.idSessionGlobal_;
}

AQSqlDatabasePrivate::~AQSqlDatabasePrivate()
{}

AQSqlDatabase::AQSqlDatabase()
    : d_( new AQSqlDatabasePrivate )
{}

QStringList AQSqlDatabase::driverAliases()
{
  QStringList ret;
  QStringList drivers( QSqlDatabase::drivers() );
  QString alias;
  for ( QStringList::const_iterator it = drivers.begin(); it != drivers.end(); ++it ) {
    alias = driverNameToDriverAlias( *it );
    if ( !alias.isEmpty() )
      ret.append( alias );
  }
  return ret;
}

QString AQSqlDatabase::defaultAlias()
{
  return "PostgreSQL";
}

QString AQSqlDatabase::driverAliasToDriverName( const QString & alias )
{
  if ( alias == "PostgreSQL" )
    return "AQPSQL";
  else if ( alias == "MySQL" )
    return "AQMYSQL";
  return QString();
}

QString AQSqlDatabase::driverNameToDriverAlias( const QString & name )
{
  if ( name == "AQPSQL" )
    return "PostgreSQL";
  else if ( name == "AQMYSQL" )
    return "MySQL";
  return QString();
}

bool AQSqlDatabase::needConnOption( const QString & alias, const int )
{
  if ( alias == "PostgreSQL" )
    return true;
  else if ( alias == "MySQL" )
    return true;
  return true;
}

bool AQSqlDatabase::loadDriver( const QString & driverName, const QString & connName )
{
  d_->dbConnName_ = connName + QDateTime::currentDateTime().toString( "hhmmsszzz" );
  d_->db_ = QSqlDatabase::addDatabase( driverName, d_->dbConnName_ );
  if ( !d_->db_.lastError().driverText().isEmpty() ) {
    d_->db_.close();
    d_->db_ = QSqlDatabase();
    return false;
  }

  d_->dbAuxConnName_ = connName + "Aux" + QDateTime::currentDateTime().toString( "hhmmsszzz" );
  d_->dbAux_ = QSqlDatabase::addDatabase( driverName, d_->dbAuxConnName_ );
  if ( !d_->dbAux_.lastError().driverText().isEmpty() ) {
    d_->db_.close();
    d_->db_ = QSqlDatabase();
    d_->dbAux_.close();
    d_->dbAux_ = QSqlDatabase();
    return false;
  }

  d_->dr_ = static_cast<AQSqlDriver *>( d_->db_.driver() );
  d_->dr_->setAQSqlDatabase( *this );

  d_->drAux_ = static_cast<AQSqlDriver *>( d_->dbAux_.driver() );
  d_->drAux_->setAQSqlDatabase( *this );

  d_->driverName_ = driverName;

  return true;
}

void AQSqlDatabase::init()
{
  d_->idSessionGlobal_ = AQCoreUtil::sha1( d_->dbConnName_ + d_->dbAuxConnName_ );
  d_->manager_.init( *this );
  d_->managerModules_.init( *this );
  d_->scriptEngine_.init( *this );
}

void AQSqlDatabase::finish()
{
  d_->scriptEngine_.finish();
  d_->manager_.finish();
  d_->managerModules_.finish();

  if ( d_->dr_ ) {
    d_->dr_->finish();
    d_->dr_ = 0;
  }

  if ( d_->drAux_ ) {
    d_->drAux_->finish();
    d_->drAux_ = 0;
  }

  d_->dbAux_.close();
  d_->db_.close();
  d_->dbAux_ = QSqlDatabase();
  d_->db_ = QSqlDatabase();
  d_->idSessionGlobal_.clear();
}

bool AQSqlDatabase::connectDB( const QString & database, const QString & user, const QString & password,
                               const QString & host, int port, const QString & connName )
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() ) {
    closeDB();
    return false;
  }

  d_->database_ = d_->dr_->formatDatabaseName( database );
  d_->user_ = user;
  d_->password_ = password;
  d_->host_ = host;
  d_->port_ = port;

  if ( d_->dr_->tryConnect( d_->database_, d_->user_, d_->password_, d_->host_, d_->port_ ) ) {
    d_->db_.setDatabaseName( d_->database_ );
    d_->db_.setUserName( d_->user_ );
    d_->db_.setPassword( d_->password_ );
    d_->db_.setHostName( d_->host_ );
    d_->db_.setPort( d_->port_ );
    if ( !d_->db_.open() ) {
      closeDB();
      return false;
    }

    d_->dbAux_.setDatabaseName( d_->database_ );
    d_->dbAux_.setUserName( d_->user_ );
    d_->dbAux_.setPassword( d_->password_ );
    d_->dbAux_.setHostName( d_->host_ );
    d_->dbAux_.setPort( d_->port_ );
    if ( !d_->dbAux_.open() ) {
      closeDB();
      return false;
    }

    d_->connectionName_ = connName;
    init();
    return true;
  }

  closeDB();
  return false;
}

bool AQSqlDatabase::connectDB()
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() ) {
    closeDB();
    return false;
  }

  if ( d_->dr_->tryConnect( d_->database_, d_->user_, d_->password_, d_->host_, d_->port_ ) ) {
    d_->db_.setDatabaseName( d_->database_ );
    d_->db_.setUserName( d_->user_ );
    d_->db_.setPassword( d_->password_ );
    d_->db_.setHostName( d_->host_ );
    d_->db_.setPort( d_->port_ );
    if ( !d_->db_.open() ) {
      closeDB();
      return false;
    }

    d_->dbAux_.setDatabaseName( d_->database_ );
    d_->dbAux_.setUserName( d_->user_ );
    d_->dbAux_.setPassword( d_->password_ );
    d_->dbAux_.setHostName( d_->host_ );
    d_->dbAux_.setPort( d_->port_ );
    if ( !d_->dbAux_.open() ) {
      closeDB();
      return false;
    }

    init();
    return true;
  }

  closeDB();
  return false;
}

void AQSqlDatabase::closeDB()
{
  finish();
}

bool AQSqlDatabase::createTable( AQTableMD * tmd ) const
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() || !tmd || !d_->dbAux_.isOpen() )
    return false;

  QString sql( d_->dr_->sqlCreateTable( tmd ) );

  if ( sql.isEmpty() )
    return false;

  QSqlQuery q( QString(), d_->dbAux_ );
  if ( !q.exec( sql ) ) {
    if ( q.lastError().isValid() )
      aqCrit( q.lastError().text() );
    aqCrit( "AQSqlDatabase : SQL - " + sql );
    return false;
  }
  return true;
}

bool AQSqlDatabase::canRegenTables() const
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() || !d_->dbAux_.isOpen() )
    return false;
  else if ( d_->driverName_ == "AQPSQL" )
    return true;
  else if ( d_->driverName_ == "AQMYSQL" )
    return true;
  return false;
}

QString AQSqlDatabase::formatValueLike( int t, const QVariant & v, bool upper ) const
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() || !d_->db_.isOpen() )
    return v.toString();

  return d_->dr_->formatValueLike( t, v, upper );
}

QString AQSqlDatabase::formatValue( int t, const QVariant & v, bool upper ) const
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() || !d_->db_.isOpen() )
    return v.toString();

  return d_->dr_->formatValue( t, v, upper );
}

QVariant AQSqlDatabase::nextSerialVal( const QString & table, const QString & field ) const
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() || !d_->db_.isOpen() )
    return QVariant( 0 );

  return d_->dr_->nextSerialVal( table, field );
}

int AQSqlDatabase::atFrom( AQSqlCursor * cur ) const
{
  if ( !d_->db_.isOpen() || !cur )
    return 0;

  const AQSqlDriver * dr = static_cast<const AQSqlDriver *>( cur->driver() );
  return dr->atFrom( cur );
}

QString AQSqlDatabase::user() const
{
  return d_->user_;
}

QString AQSqlDatabase::password() const
{
  return d_->password_;
}

QString AQSqlDatabase::host() const
{
  return d_->host_;
}

int AQSqlDatabase::port() const
{
  return d_->port_;
}

QString AQSqlDatabase::driverName() const
{
  return d_->driverName_;
}

const AQManager * AQSqlDatabase::manager() const
{
  return & d_->manager_;
}

const AQManagerModules * AQSqlDatabase::managerModules() const
{
  return & d_->managerModules_;
}

const AQScriptEngineQSA * AQSqlDatabase::scriptEngine() const
{
  return & d_->scriptEngine_;
}


bool AQSqlDatabase::canSavePoint() const
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() || !d_->db_.isOpen() )
    return false;

  return d_->dr_->canSavePoint();
}

bool AQSqlDatabase::savePoint( const QString & n ) const
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() || !d_->db_.isOpen() )
    return false;

  return d_->dr_->savePoint( n );
}

bool AQSqlDatabase::releaseSavePoint( const QString & n ) const
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() || !d_->db_.isOpen() )
    return false;

  return d_->dr_->releaseSavePoint( n );
}

bool AQSqlDatabase::rollbackSavePoint( const QString & n ) const
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() || !d_->db_.isOpen() )
    return false;

  return d_->dr_->rollbackSavePoint( n );
}

void AQSqlDatabase::createIndex( const QString & fieldName, const QString & tableName, bool upper ) const
{
  if ( !d_->dr_ || d_->driverName_.isEmpty() || !d_->db_.isOpen() )
    return ;

  d_->dr_->createIndex( fieldName, tableName, upper );
}

QSqlDatabase AQSqlDatabase::db() const
{
  return d_->db_;
}

QSqlDatabase AQSqlDatabase::dbAux() const
{
  return d_->dbAux_;
}

QString AQSqlDatabase::database() const
{
  return d_->database_;
}

QString AQSqlDatabase::connectionName() const
{
  return d_->connectionName_;
}

QString AQSqlDatabase::idSessionGlobal() const
{
  return d_->idSessionGlobal_;
}
