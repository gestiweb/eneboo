/***************************************************************************
              AQSqlQuery.cpp
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

#include <QRegExp>

#include <AQCore/AQTableMD.h>

#include "AQSqlQuery.h"
#include "AQSqlDatabase.h"
#include "AQSqlConnections.h"
#include "util/AQParameterQuery.h"
#include "util/AQGroupByQuery.h"
#include "manager/AQManager.h"

#ifdef AQ_WARN
long AQSqlQuery::countRefQuery = 0;
#endif

class AQSqlQueryPrivate
{
public:

  AQSqlQueryPrivate()
  {

#ifdef AQ_WARN
    ++AQSqlQuery::countRefQuery;
#endif

  }

  ~AQSqlQueryPrivate();

  /**
  Limpiar
  */
  void clear();

  /**
  Nombre de la consulta
  */
  QString name_;

  /**
  Parte SELECT de la consulta
  */
  QString select_;

  /**
  Parte FROM de la consulta
  */
  QString from_;

  /**
  Parte WHERE de la consulta
  */
  QString where_;

  /**
  Parte ORDER BY de la consulta
  */
  QString orderBy_;

  /**
  Lista de nombres de los campos
  */
  QStringList fieldList_;

  /**
  Lista de parámetros
  */
  AQParameterQryHash parameterHash_;

  /**
  Lista de grupos
  */
  AQGroupByQryHash groupHash_;

  /**
  Lista de nombres de las tablas que entran a formar
  parte en la consulta
  */
  QStringList tablesList_;

  /**
  Lista de con los metadatos de los campos de la consulta
  */
  AQFieldMDHash fieldMetaDataList_;

  /**
  Base de datos sobre la que trabaja
  */
  AQSqlDatabase db_;
};

AQSqlQueryPrivate::~AQSqlQueryPrivate()
{
  clear();
  fieldMetaDataList_.clear();
  fieldList_.clear();
  tablesList_.clear();

#ifdef AQ_WARN
  --AQSqlQuery::countRefQuery;
#endif
}

void AQSqlQueryPrivate::clear()
{
  while ( !parameterHash_.isEmpty() ) {
    AQParameterQuery * v = parameterHash_.begin().value();
    parameterHash_.erase( parameterHash_.begin() );
    delete v;
  }

  while ( !groupHash_.isEmpty() ) {
    AQGroupByQuery * v = groupHash_.begin().value();
    groupHash_.erase( groupHash_.begin() );
    delete v;
  }
}

AQSqlQuery::AQSqlQuery( const QString & connectionName ) :
    QSqlQuery( AQSqlConnections::database( connectionName ).db() )
{
  d = new AQSqlQueryPrivate();
  d->db_ = AQSqlConnections::database( connectionName );
}

AQSqlQuery::~AQSqlQuery()
{
  delete d;
}

void AQSqlQuery::setSelect( const QString & s ) const
{
  d->select_ = s.trimmed();
  d->select_ = d->select_.simplified();

  QStringList fieldListAux;
  fieldListAux = s.split( ',' );
  for ( QStringList::iterator it = fieldListAux.begin(); it != fieldListAux.end(); ++it )
    ( *it ) = ( *it ).trimmed();

  QString table, field;
  d->fieldList_.clear();
  for ( QStringList::iterator it = fieldListAux.begin(); it != fieldListAux.end(); ++it ) {
    table = ( *it ).section( '.', 0, 0 );
    field = ( *it ).section( '.', 1, 1 );
    if ( d->db_.manager() ->isReady() && field == "*" ) {
      AQTableMD * mtd = d->db_.manager() ->metadata( table, true );
      if ( mtd )
        d->fieldList_ += mtd->fieldList( true ).split( ',' );
    } else
      d->fieldList_.append( *it );
  }

  d->select_ = d->fieldList_.join( "," );
}

QString AQSqlQuery::sql()
{
  if ( !d->db_.manager() ->isReady() )
    return QString();

  for ( QStringList::const_iterator it = d->tablesList_.begin(); it != d->tablesList_.end(); ++it )
    if ( !d->db_.manager() ->existsTable( *it ) )
      if ( !d->db_.manager() ->createTable( *it ) )
        return QString();

  QString res;
  if ( d->from_.isEmpty() )
    res = "SELECT " + d->select_;
  else if ( d->where_.isEmpty() )
    res = "SELECT " + d->select_ + " FROM " + d->from_;
  else
    res = "SELECT " + d->select_ + " FROM " + d->from_ + " WHERE " + d->where_;

  if ( !d->groupHash_.isEmpty() && d->orderBy_.isEmpty() ) {
    res += " ORDER BY ";
    for ( int i = 0; i < d->groupHash_.size(); ++i ) {
      if ( i == 0 )
        res += d->groupHash_.value( QString::number( i ) ) ->field();
      else
        res += ", " + d->groupHash_.value( QString::number( i ) ) ->field();
    }
  } else {
    if ( !d->orderBy_.isEmpty() )
      res += " ORDER BY " + d->orderBy_;
  }

  if ( !d->parameterHash_.isEmpty() ) {
    AQParameterQryHash::const_iterator it = d->parameterHash_.begin();
    while ( it != d->parameterHash_.end() ) {
      QVariant v = it.value() ->value();
      if ( v.isValid() )
        res = res.replace( QRegExp( "\\[" + it.key() + "\\]" ),
                           d->db_.manager() ->formatValue( it.value() ->type(), v ) );
      ++it;
    }
  }

  return res;
}

bool AQSqlQuery::exec()
{
  return QSqlQuery::exec( sql() );
}

void AQSqlQuery::setGroupHash( const AQGroupByQryHash & gd ) const
{
  if ( gd.isEmpty() )
    return ;

  d->clear();
  d->groupHash_ = gd;
}

void AQSqlQuery::setParameterHash( const AQParameterQryHash & pd ) const
{
  if ( pd.isEmpty() )
    return ;

  d->clear();
  d->parameterHash_ = pd;
}

void AQSqlQuery::addParameter( AQParameterQuery * p ) const
{
  if ( p )
    d->parameterHash_.insert( p->name(), p );
}

void AQSqlQuery::addGroup( AQGroupByQuery * g ) const
{
  if ( g )
    d->groupHash_.insert( QString::number( g->level() ), g );
}

QVariant AQSqlQuery::value( const QString & n ) const
{
  if ( !isValid() )
    return QVariant();
  int i = d->fieldList_.indexOf( n );
  return ( i >= 0 ? value( i ) : QVariant() );
}

QVariant AQSqlQuery::value( int i ) const
{
  if ( !isValid() && i >= 0 )
    return QVariant();

  QVariant v( QSqlQuery::value( i ) );

  if ( v.type() == QVariant::String ) {
    if ( !v.isNull() ) {
      QVariant vLarge( d->db_.manager() ->fetchLargeValue( v.toString() ) );
      if ( vLarge.isValid() )
        return vLarge;
    }
  } else if ( v.type() == QVariant::ULongLong )
    return v.toDouble();

  return v;
}

QString AQSqlQuery::posToFieldName( int p ) const
{
  if ( p < 0 || p >= d->fieldList_.size() )
    return QString();
  QStringList::const_iterator it( d->fieldList_.begin() + p );
  return ( it != d->fieldList_.end() ? *it : QString() );
}

int AQSqlQuery::fieldNameToPos( const QString & n ) const
{
  return d->fieldList_.indexOf( n );
}

void AQSqlQuery::setTablesList( const QString & tl ) const
{
  d->tablesList_.clear();
  d->tablesList_ = tl.split( ',' );
}

void AQSqlQuery::setValueParam( const QString & name, const QVariant & v ) const
{
  d->parameterHash_.value( name ) ->setValue( v );
}

QVariant AQSqlQuery::valueParam( const QString & name ) const
{
  return d->parameterHash_.value( name ) ->value();
}

int AQSqlQuery::size() const
{
  return QSqlQuery::size();
}

AQFieldMDHash AQSqlQuery::fieldMetaDataList() const
{
  if ( d->db_.manager() ->isReady() && d->fieldMetaDataList_.isEmpty() ) {
    QString table, field;
    for ( QStringList::const_iterator it = d->fieldList_.begin(); it != d->fieldList_.end(); ++it ) {
      table = ( *it ).section( '.', 0, 0 );
      field = ( *it ).section( '.', 1, 1 );
      AQTableMD * mtd = d->db_.manager() ->metadata( table, true );
      d->fieldMetaDataList_.insert( field.toLower(), mtd->field( field ) );
    }
  }

  return d->fieldMetaDataList_;
}

AQSqlDatabase AQSqlQuery::db() const
{
  return d->db_;
}

void AQSqlQuery::setName( const QString & n ) const
{
  d->name_ = n;
}

QString AQSqlQuery::name() const
{
  return d->name_;
}

QString AQSqlQuery::select() const
{
  return d->select_;
}

QString AQSqlQuery::from() const
{
  return d->from_;
}

QString AQSqlQuery::where() const
{
  return d->where_;
}

QString AQSqlQuery::orderBy() const
{
  return d->orderBy_;
}

void AQSqlQuery::setFrom( const QString & f ) const
{
  d->from_ = f.trimmed();
  d->from_ = d->from_.simplified();
}

void AQSqlQuery::setWhere( const QString & w ) const
{
  d->where_ = w.trimmed();
  d->where_ = d->where_.simplified();
}

void AQSqlQuery::setOrderBy( const QString & w ) const
{
  d->orderBy_ = w.trimmed();
  d->orderBy_ = d->orderBy_.simplified();
}

AQParameterQryHash AQSqlQuery::parameterHash() const
{
  return d->parameterHash_;
}

AQGroupByQryHash AQSqlQuery::groupHash() const
{
  return d->groupHash_;
}

QStringList AQSqlQuery::fieldList() const
{
  return d->fieldList_;
}

QStringList AQSqlQuery::tablesList() const
{
  return d->tablesList_;
}
