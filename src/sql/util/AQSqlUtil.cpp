/***************************************************************************
                 AQSqlUtil.cpp
              -------------------
begin                : 09/08/2007
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

#include <math.h>

#include <AQCore/AQTableMD.h>
#include <AQCore/AQFieldMD.h>

#include "AQSqlUtil.h"
#include "../AQSqlDatabase.h"
#include "../AQSqlConnections.h"
#include "../AQSqlCursor.h"
#include "../AQSqlQuery.h"
#include "../qt3support/q3sqlcursor.h"

QStringList AQSqlUtil::fieldsNames( const QString & table, const QString & connName )
{
  QStringList res;
  AQSqlCursor c( table, true, connName );

  res = c.metadata()->fieldList( false ).split( "," );
  res.prepend( QString::number( res.size() ) );

  return res;
}

QVariant AQSqlUtil::nextCounter( const QString & name, AQSqlCursor * cursor_ )
{
  Q_ASSERT( cursor_ );

  AQFieldMD * field = cursor_->metadata()->field( name );

  Q_ASSERT( field );

  int type = field->type();

  if ( type != QVariant::String && type != QVariant::Double )
    return QVariant();

  int len = field->length();
  QString cadena;
  QString tableName( cursor_->metadata()->name() );

  AQSqlQuery q( cursor_->db().connectionName() );
  q.setForwardOnly( true );
  q.setTablesList( tableName );
  q.setSelect( name );
  q.setFrom( tableName );
  q.setWhere( "LENGTH(" + name + ")=" + QString::number( len ) );
  q.setOrderBy( name + " DESC" );

  if ( !q.exec() )
    return QVariant();

  double maxRange = pow( 10, len );
  double numero = maxRange;

  while ( numero >= maxRange ) {
    if ( !q.next() ) {
      numero = 1;
      break;
    }
    numero = q.value( 0 ).toDouble();
    ++numero;
  }

  if ( type == QVariant::String ) {
    cadena = QString::number( numero, 'f', 0 );
    if ( cadena.length() < len ) {
      QString str;
      str.fill( '0', ( len - cadena.length() ) );
      cadena = str + cadena;
    }
    return QVariant( cadena );
  }

  if ( type == QVariant::Double ) {
    return QVariant( numero );
  }

  return QVariant();
}

QVariant AQSqlUtil::readDBSettingEntry( const QString & key, const QString & connName )
{
  QVariant v = sqlSelect( "aqsettings", "valor", "aqkey = '" + key + "'", "aqsettings", 0, connName );
  return v;
}

bool AQSqlUtil::writeDBSettingEntry( const QString & key, const QString & value, const QString & connName )
{
  int size;
  bool result;
  sqlSelect( "aqsettings", "valor", "aqkey = '" + key + "'", "aqsettings", &size, connName );
  if ( size > 0 )
    result = sqlUpdate( "aqsettings", "valor", value, "aqkey = '" + key + "'", connName );
  else
    result = sqlInsert( "aqsettings", "aqkey,valor", key + "," + value, connName );
  return result;
}

QVariant AQSqlUtil::sqlSelect( const QString & f, const QString & s,
                               const QString & w, const QString & tL, int * size, const QString & connName )
{
  AQSqlQuery q_( connName );

  if ( !tL.isNull() )
    q_.setTablesList( tL );
  else
    q_.setTablesList( f );
  q_.setSelect( s );
  q_.setFrom( f );
  q_.setWhere( w );
  q_.setForwardOnly( true );
  if ( !q_.exec() ) {
    if ( size )
      * size = -1;
    return QVariant( false );
  }
  if ( size )
    * size = q_.size();
  if ( q_.next() )
    return q_.value( 0 );

  return QVariant( false );
}

bool AQSqlUtil::sqlInsert( const QString & t, const QString & fL, const QString & vL, const QString & connName )
{
  int fN = fL.count( ',' ), vN = vL.count( ',' );

  if ( fN != vN )
    return false;

  ++fN;
  QString fL2( fL ), v, f;
  fL2.remove( " " );

  AQSqlCursor c( t, true, connName );
  c.setModeAccess( AQSql::INSERT );
  c.refreshBuffer();
  for ( int i = 0; i < fN; ++i ) {
    v = vL.section( ',', i, i );
    f = fL2.section( ',', i, i );
    if ( v == "NULL" )
      c.bufferSetNull( f );
    else
      c.setValueBuffer( f, v );
  }

  return c.commitBuffer();
}

bool AQSqlUtil::sqlUpdate( const QString & t, const QString & fL,
                           const QString & vL, const QString & w, const QString & connName )
{
  int fN = fL.count( ',' ), vN = vL.count( ',' );

  if ( fN != vN )
    return false;

  AQSqlCursor c( t, true, connName );
  if ( !c.select( w ) )
    return false;

  ++fN;
  QString fL2( fL ), v, f;
  fL2.remove( " " );

  c.setForwardOnly( true );
  while ( c.next() ) {
    c.setModeAccess( AQSql::EDIT );
    c.refreshBuffer();
    for ( int i = 0; i < fN; ++i ) {
      v = vL.section( ',', i, i );
      f = fL2.section( ',', i, i );
      if ( v == "NULL" )
        c.bufferSetNull( f );
      else
        c.setValueBuffer( f, v );
    }
    if ( !c.commitBuffer() )
      return false;
  }

  return true;
}

bool AQSqlUtil::sqlDelete( const QString & t, const QString & w, const QString & connName )
{
  AQSqlCursor c( t, true, connName );

  if ( !c.select( w ) )
    return false;

  c.setForwardOnly( true );
  while ( c.next() ) {
    c.setModeAccess( AQSql::DEL );
    c.refreshBuffer();
    if ( !c.commitBuffer() )
      return false;
  }

  return true;
}
