/***************************************************************************
                        AQSqlDriver.cpp
                    -------------------
begin                : 07/08/2007
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

#include "AQSqlDriver.h"

AQSqlDriver::AQSqlDriver( QObject * parent ) :
    QSqlDriver( parent )
{}

AQSqlDriver::~AQSqlDriver()
{}

QString AQSqlDriver::formatDatabaseName( const QString & name ) const
{
  return name;
}

bool AQSqlDriver::tryConnect( const QString &, const QString &, const QString &,
                              const QString &, int )
{
  return false;
}

QString AQSqlDriver::sqlCreateTable( AQTableMD * ) const
{
  return QString();
}

QString AQSqlDriver::formatValueLike( int, const QVariant & v, bool ) const
{
  return v.toString();
}

QString AQSqlDriver::formatValue( int, const QVariant & v, bool ) const
{
  return v.toString();
}

QVariant AQSqlDriver::nextSerialVal( const QString &, const QString & ) const
{
  return 0;
}

int AQSqlDriver::atFrom( AQSqlCursor * ) const
{
  return -99;
}

void AQSqlDriver::setAQSqlDatabase( AQSqlDatabase db )
{
  db_ = db;
}

bool AQSqlDriver::canSavePoint() const
{
  return false;
}

bool AQSqlDriver::savePoint( const QString & ) const
{
  return false;
}

bool AQSqlDriver::releaseSavePoint( const QString & ) const
{
  return false;
}

bool AQSqlDriver::rollbackSavePoint( const QString & ) const
{
  return false;
}

void AQSqlDriver::finish()
{
  db_ = AQSqlDatabase();
}

void AQSqlDriver::createIndex( const QString &, const QString &, bool ) const
  {}

