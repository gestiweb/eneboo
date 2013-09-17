/***************************************************************************
AQVar.cpp
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

#include "AQVar.h"
#include "AQSqlUtil.h"
#include "../AQSqlQuery.h"
#include "../AQSqlDatabase.h"
#include "../AQSqlConnections.h"

AQVar::AQVar( const QString & connName ) :
    connName_( connName )
{}

bool AQVar::set( const QString & n, const QVariant & v )
{
  QString idSesion( AQSqlConnections::database( connName_ ).idSessionGlobal() );
  QString where( "idvar = '" + n + "' AND idsesion = '" + idSesion + "'" );
  AQSqlQuery q( connName_ );

  q.setTablesList( "aqvar" );
  q.setSelect( "idvar" );
  q.setFrom( "aqvar" );
  q.setWhere( where );
  q.setForwardOnly( true );

  if ( !q.exec() )
    return false;

  if ( q.next() )
    return AQSqlUtil::sqlUpdate( "aqvar", "valor", v.toString(), where, connName_ );
  else {
    QString values( n + "," + idSesion + "," + v.toString() );
    return AQSqlUtil::sqlInsert( "aqvar", "idvar, idsesion, valor", values, connName_ );
  }
}

QVariant AQVar::get( const QString & n, bool * exists ) const
{
  QString idSesion( AQSqlConnections::database( connName_ ).idSessionGlobal() );
  QString where = "idvar = '" + n + "' AND idsesion = '" + idSesion + "'";
  int size;
  QVariant result( AQSqlUtil::sqlSelect( "aqvar", "valor", where, "aqvar", &size, connName_ ) );

  if ( exists ) {
    if ( size == 0 )
      * exists = false;
    else
      *exists = true;
  }

  return result;
}

bool AQVar::del( const QString & n )
{
  QString idSesion( AQSqlConnections::database( connName_ ).idSessionGlobal() );
  QString where = "idvar = '" + n + "' AND idsesion = '" + idSesion + "'";
  return AQSqlUtil::sqlDelete( "aqvar", where, connName_ );
}

bool AQVar::clean()
{
  QString idSesion( AQSqlConnections::database( connName_ ).idSessionGlobal() );
  QString where = "idsesion = '" + idSesion + "'";
  return AQSqlUtil::sqlDelete( "aqvar", where, connName_ );
}
