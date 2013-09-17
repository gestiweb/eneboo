/***************************************************************************
AQSqlQueryInterface.cpp
-------------------
begin                : 11/10/2007
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

#include <AQScript/AQScriptEngineQSA.h>

#include "AQSqlQueryInterface.h"
#include "../AQSqlQuery.h"
#include "../AQSqlDatabase.h"

AQSqlQueryInterface::AQSqlQueryInterface( const AQScriptEngineQSA * eng, const QString & n, const QString & connName ) :
    AQObjectInterfaceQSA( eng )
{
  if ( n.isEmpty() )
    obj_ = new AQSqlQuery( dbConName( connName ) );
  else
    obj_ = dbCon( connName ).manager() ->query( n );
}

AQSqlQueryInterface::~AQSqlQueryInterface()
{
  delete obj_;
}

QString AQSqlQueryInterface::select() const
{
  Q_ASSERT( obj_ );
  return obj_->select();
}

QString AQSqlQueryInterface::from() const
{
  Q_ASSERT( obj_ );
  return obj_->from();
}

QString AQSqlQueryInterface::where() const
{
  Q_ASSERT( obj_ );
  return obj_->where();
}

QString AQSqlQueryInterface::orderBy() const
{
  Q_ASSERT( obj_ );
  return obj_->orderBy();
}

QString AQSqlQueryInterface::tablesList() const
{
  Q_ASSERT( obj_ );
  return obj_->tablesList().join( QLatin1String( "," ) );
}

void AQSqlQueryInterface::setSelect( const QString & s ) const
{
  Q_ASSERT( obj_ );
  obj_->setSelect( s );
}

void AQSqlQueryInterface::setFrom( const QString & f ) const
{
  Q_ASSERT( obj_ );
  QString s = f;
  s.replace( "NaN", "" );
  s.replace( "nan", "" );
  obj_->setFrom( s );
}

void AQSqlQueryInterface::setWhere( const QString & w ) const
{
  Q_ASSERT( obj_ );
  QString s = w;
  s.replace( "NaN", "" );
  s.replace( "nan", "" );
  obj_->setWhere( s );
}

void AQSqlQueryInterface::setOrderBy( const QString & oB ) const
{
  Q_ASSERT( obj_ );
  QString s = oB;
  s.replace( "NaN", "" );
  s.replace( "nan", "" );
  obj_->setOrderBy( s );
}

void AQSqlQueryInterface::setTablesList( const QString & tl ) const
{
  Q_ASSERT( obj_ );
  obj_->setTablesList( tl );
}

QString AQSqlQueryInterface::sql() const
{
  Q_ASSERT( obj_ );
  return obj_->sql();
}

QVariant AQSqlQueryInterface::value( int i ) const
{
  Q_ASSERT( obj_ );
  return obj_->value( i );
}

QVariant AQSqlQueryInterface::value( const QString & n ) const
{
  Q_ASSERT( obj_ );
  return obj_->value( n );
}

bool AQSqlQueryInterface::isNull( int i ) const
{
  Q_ASSERT( obj_ );
  return obj_->isNull( i );
}

QString AQSqlQueryInterface::posToFieldName( int p ) const
{
  Q_ASSERT( obj_ );
  return obj_->posToFieldName( p );
}

int AQSqlQueryInterface::fieldNameToPos( const QString & n ) const
{
  Q_ASSERT( obj_ );
  return obj_->fieldNameToPos( n );
}

QVariant AQSqlQueryInterface::valueParam( const QString & name ) const
{
  Q_ASSERT( obj_ );
  return obj_->valueParam( name );
}

void AQSqlQueryInterface::setValueParam( const QString & name, const QVariant & v ) const
{
  Q_ASSERT( obj_ );
  obj_->setValueParam( name, v );
}

bool AQSqlQueryInterface::next() const
{
  Q_ASSERT( obj_ );
  return obj_->next();
}

bool AQSqlQueryInterface::prev() const
{
  Q_ASSERT( obj_ );
  return obj_->prev();
}

bool AQSqlQueryInterface::first() const
{
  Q_ASSERT( obj_ );
  return obj_->first();
}

bool AQSqlQueryInterface::last() const
{
  Q_ASSERT( obj_ );
  return obj_->last();
}

bool AQSqlQueryInterface::exec() const
{
  Q_ASSERT( obj_ );
  return obj_->exec();
}

int AQSqlQueryInterface::size() const
{
  Q_ASSERT( obj_ );
  return obj_->size();
}

bool AQSqlQueryInterface::isValid() const
{
  Q_ASSERT( obj_ );
  return obj_->isValid();
}


void AQSqlQueryInterfaceAQ2::setForwardOnly ( bool )
{
  AQ_VOID_PORT_AQ2( AQSqlQuery::setForwardOnly() );
}

void AQSqlQueryInterfaceAQ2::showDebug() const
{
  AQ_VOID_PORT_AQ2( AQSqlQuery::showDebug() );
}

AQSqlQuery * AQSqlQueryInterfaceAQ2::obj() const
{
  AQ_VOID_PORT_AQ2( AQSqlQuery::obj() );
  return 0;
}

#include "moc_AQSqlQueryInterface.cpp"
