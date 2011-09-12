/***************************************************************************
                            FLSqlDriver.cpp
                         -------------------
begin                : Thu Nov 22 2005
copyright            : (C) 2002-2005 by InfoSiAL S.L.
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

#include "FLSqlDriver.h"

FLSqlDriver::FLSqlDriver( QObject * parent, const char *name ) : QSqlDriver( parent, name ), db_( 0 ) {}

FLSqlDriver::~FLSqlDriver() {}

QString FLSqlDriver::formatDatabaseName( const QString & name ) {
  return name;
}

bool FLSqlDriver::tryConnect( const QString & db, const QString & user, const QString & password, const QString & host, int port ) {
  return false;
}

QString FLSqlDriver::sqlCreateTable( FLTableMetaData * tmd ) {
  return QString::null;
}

QString FLSqlDriver::formatValueLike( int t, const QVariant & v, const bool upper ) {
  return v.toString();
}

QString FLSqlDriver::formatValue( int t, const QVariant & v, const bool upper ) {
  return v.toString();
}

QVariant FLSqlDriver::nextSerialVal( const QString & table, const QString & field ) {
  return 0;
}

int FLSqlDriver::atFrom( FLSqlCursor * cur ) {
  return -99;
}

bool FLSqlDriver::alterTable( const QString & mtd1, const QString & mtd2, const QString & key ) {
  return false;
}

void FLSqlDriver::setFLSqlDatabase( FLSqlDatabase * db ) {
  db_ = db;
}

bool FLSqlDriver::canSavePoint() {
  return false;
}

bool FLSqlDriver::savePoint( const QString & ) {
  return false;
}

bool FLSqlDriver::releaseSavePoint( const QString & ) {
  return false;
}

bool FLSqlDriver::rollbackSavePoint( const QString & ) {
  return false;
}

QStringList FLSqlDriver::locksStatus() {
  return QStringList();
}

QStringList FLSqlDriver::detectLocks() {
  return QStringList();
}

QStringList FLSqlDriver::detectRisksLocks( const QString & table, const QString & primaryKeyValue ) {
  return QStringList();
}

bool FLSqlDriver::regenTable( const QString & n, FLTableMetaData * tmd ) {
  return false;
}
