/***************************************************************************
q3sqlindex_p.h
-------------------
begin                : 06/02/2009
copyright            : (C) 2003-2009 by InfoSiAL S.L.
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

#ifndef Q3SQLINDEX_P_H_
#define Q3SQLINDEX_P_H_

/**
Auxiliares para eliminar dependencias con qt3support
*/

static QString sqlIndexCreateField( const QSqlIndex & idx, int i,
                                    const QString& prefix, bool verbose )
{
  QString f;
  if ( !prefix.isEmpty() )
    f += prefix + QLatin1Char( '.' );
  f += idx.field( i ).name();
  if ( verbose )
    f += QLatin1Char( ' ' ) + QString( ( idx.isDescending( i )
                                         ? QLatin1String( "DESC" ) : QLatin1String( "ASC" ) ) );
  return f;
}

static QString sqlIndexToString( const QSqlIndex & idx, const QString &prefix = QString(),
                                 const QString &sep = QLatin1String( "," ),
                                 bool verbose = true )
{
  QString s;
  bool comma = false;
  for ( int i = 0; i < idx.count(); ++i ) {
    if ( comma )
      s += sep + QLatin1Char( ' ' );
    s += sqlIndexCreateField( idx, i, prefix, verbose );
    comma = true;
  }
  return s;
}

#ifdef AQSQLCURSOR_H_
static QStringList sqlIndexToStringList( const QSqlIndex & idx,
    const QString & prefix = QString(), bool verbose = true )
{
  QStringList s;
  for ( int i = 0; i < idx.count(); ++i )
    s += sqlIndexCreateField( idx, i, prefix, verbose );
  return s;
}
#endif

#endif /*Q3SQLINDEX_P_H_*/
