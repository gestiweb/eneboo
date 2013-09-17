/***************************************************************************
AQSqlPSQL.cpp
-------------------
begin                : 23/08/2007
copyright            : (C) 2003-2008 by InfoSiAL S.L.
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

#include "AQSqlPSQL.h"

#include <AQCore/AQCore.h>
#include <AQCore/AQTableMD.h>
#include <AQCore/AQFieldMD.h>
#include <AQCore/AQCoreUtil.h>
#include <AQSql/AQManager.h>
#include <AQSql/AQManagerModules.h>
#include <AQSql/AQSqlDatabase.h>

#include <QQueue>

#include <qcoreapplication.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qregexp.h>
#include <qsqlerror.h>
#include <qsqlfield.h>
#include <qsqlindex.h>
#include <qsqlrecord.h>
#include <qsqlquery.h>
#include <qstringlist.h>
#include <qhash.h>
#include <qdom.h>

#include <aqpq/libpq-fe.h>

#include <stdlib.h>
#include <math.h>

// workaround for postgres defining their OIDs in a private header file
#define QBOOLOID 16
#define QINT8OID 20
#define QINT2OID 21
#define QINT4OID 23
#define QNUMERICOID 1700
#define QFLOAT4OID 700
#define QFLOAT8OID 701
#define QABSTIMEOID 702
#define QRELTIMEOID 703
#define QDATEOID 1082
#define QTIMEOID 1083
#define QTIMETZOID 1266
#define QTIMESTAMPOID 1114
#define QTIMESTAMPTZOID 1184
#define QOIDOID 2278
#define QBYTEAOID 17
#define QREGPROCOID 24
#define QXIDOID 28
#define QCIDOID 29

#define LIMIT_RESULT 99
#define TAM_RECENT 8

Q_DECLARE_METATYPE( PGconn* )
Q_DECLARE_METATYPE( PGresult* )

/* This is a compile time switch - if PQfreemem is declared, the compiler will use that one,
   otherwise it'll run in this template */
template <typename T>
inline void PQfreemem( T *t, int = 0 )
{
  free( t );
}

inline void qPQfreemem( void *buffer )
{
  PQfreemem( buffer );
}

class AQPSQLDriverPrivate
{
public:
  AQPSQLDriverPrivate() :
      connection( 0 ),
      isUtf8( false ),
      pro( AQPSQLDriver::Version6 ), idConn( -1 )
  {}
  PGconn *connection;
  bool isUtf8;
  AQPSQLDriver::Protocol pro;
  QHash<QString, bool> hashIndexes;
  AQSqlDatabase db_;
  int idConn;

  void appendTables( QStringList &tl, QSqlQuery &t, QChar type );
};

void AQPSQLDriverPrivate::appendTables( QStringList &tl, QSqlQuery &t, QChar type )
{
  QString query;
  if ( pro >= AQPSQLDriver::Version73 ) {
    query = QString::fromLatin1( "select pg_class.relname, pg_namespace.nspname from pg_class "
                                 "left join pg_namespace on (pg_class.relnamespace = pg_namespace.oid) "
                                 "where (pg_class.relkind = '%1') and (pg_class.relname !~ '^Inv') "
                                 "and (pg_class.relname !~ '^pg_') "
                                 "and (pg_namespace.nspname != 'information_schema') " ).arg( type );
  } else {
    query = QString::fromLatin1( "select relname, null from pg_class where (relkind = 'r') "
                                 "and (relname !~ '^Inv') "
                                 "and (relname !~ '^pg_') " );
  }
  t.exec( query );
  while ( t.next() ) {
    QString schema = t.value( 1 ).toString();
    if ( schema.isEmpty() || schema == QLatin1String( "public" ) )
      tl.append( t.value( 0 ).toString() );
    else
      tl.append( t.value( 0 ).toString().prepend( QLatin1Char( '.' ) ).prepend( schema ) );
  }
}

class AQPGresult
{
public:
  AQPGresult( PGresult * result = 0 ) :
      result_( result )
  {}
  ~AQPGresult()
  {
    if ( result_ )
      PQclear( result_ );
  }

  PGresult * result_;
};

class AQPSQLResultPrivate
{
public:
  AQPSQLResultPrivate( AQPSQLResult * qq ) :
      q( qq ),
      driver( 0 ),
      result( 0 ),
      currentSize( -1 ),
      closeCursor_( false ),
      currentResult( 0 )
  {}

  AQPSQLResult * q;
  const AQPSQLDriverPrivate * driver;
  PGresult * result;
  int currentSize;

  bool processResults();

  void calcSize();
  bool nextResult( int i );
  void closeCursor();
  bool openCursor();
  void cleanupCache();

  static int cursorCounter;

  QString qry;
  QString idCursor;
  bool closeCursor_;
  QHash<int, AQPGresult *> cachedResults;
  QQueue<int> recent;
  int currentResult;
};

int AQPSQLResultPrivate::cursorCounter = 0;

static QSqlError qMakeError( const QString& err, QSqlError::ErrorType type,
                             const AQPSQLDriverPrivate *p )
{
  const char * s = PQerrorMessage( p->connection );
  QString msg = p->isUtf8 ? QString::fromUtf8( s ) : QString::fromLocal8Bit( s );

  msg = "\n" + msg.trimmed().simplified() + "\n\n";
  msg += AQ_TR( "SqlDriver", "Obtenga ayuda y soporte en" ) +
         " http://www.infosial.com\n(C)2003-2008 InfoSiAL S.L.";

  return QSqlError( QLatin1String( "AQPSQL: " ) + err, msg, type );
}

bool AQPSQLResultPrivate::processResults()
{
  if ( !result )
    return false;

  int status = PQresultStatus( result );
  if ( status == PGRES_TUPLES_OK ) {
    q->setSelect( true );
    q->setActive( true );
    currentSize = PQntuples( result );
    return true;
  } else if ( status == PGRES_COMMAND_OK ) {
    q->setSelect( false );
    q->setActive( true );
    currentSize = -1;
    return true;
  }
  q->setLastError( qMakeError( AQ_TR( "SqlDriver",
                                      "Unable to create query" ), QSqlError::StatementError, driver ) );
  return false;
}

bool AQPSQLResultPrivate::openCursor()
{
  if ( !qry.isEmpty() && !idCursor.isEmpty() ) {
    if ( closeCursor_ )
      closeCursor();

    QString qr;
    if ( q->isForwardOnly() )
      qr = QLatin1String( "DECLARE " ) + idCursor + QLatin1String( " NO SCROLL CURSOR" );
    else
      qr = QLatin1String( "DECLARE " ) + idCursor + QLatin1String( " SCROLL CURSOR" );

    if ( PQtransactionStatus( driver->connection ) != PQTRANS_INTRANS ) {
      qr += QLatin1String( " WITH HOLD FOR " ) + qry;
      closeCursor_ = true;
    } else
      qr += QLatin1String( " WITHOUT HOLD FOR " ) + qry;

    PGresult * res = 0;
    if ( driver->isUtf8 )
      res = PQexec( driver->connection, qr.toUtf8().constData() );
    else
      res = PQexec( driver->connection, qr.toLocal8Bit().constData() );

    int status = PQresultStatus( res );
    PQclear( res );

    if ( q->isForwardOnly() ) {
      qr = QLatin1String( "MOVE FORWARD " ) + QString::number( LIMIT_RESULT ) +
           QLatin1String( " FROM " ) + idCursor;

      if ( driver->isUtf8 ) {
        res = PQexec( driver->connection, qr.toUtf8().constData() );
      } else {
        res = PQexec( driver->connection, qr.toLocal8Bit().constData() );
      }
      status = PQresultStatus( res );
      PQclear( res );
    }

    if ( status != PGRES_COMMAND_OK ) {
      q->setLastError( qMakeError( AQ_TR( "SqlDriver",
                                          "La consulta a la base de datos ha fallado" ),
                                   QSqlError::StatementError, driver ) );
      aqDebug( QString( PQresultErrorMessage( res ) ) + "\n" + qr + "\n" );
    } else {
      cachedResults.insert( 0, new AQPGresult( result ) );
      return true;
    }
  }

  closeCursor_ = false;
  return false;
}

void AQPSQLResultPrivate::closeCursor()
{
  if ( !idCursor.isEmpty() && closeCursor_ ) {
    QString qr( QLatin1String( "CLOSE " ) + idCursor );
    PGresult * res = 0;

    if ( driver->isUtf8 ) {
      res = PQexec( driver->connection, qr.toUtf8().constData() );
    } else {
      res = PQexec( driver->connection, qr.toLocal8Bit().constData() );
    }
    PQclear( res );
  }

  idCursor.clear();
  closeCursor_ = false;
}

void AQPSQLResultPrivate::cleanupCache()
{
  if ( !cachedResults.isEmpty() ) {
    while ( !cachedResults.isEmpty() ) {
      AQPGresult * pgR = cachedResults.begin().value();
      cachedResults.erase( cachedResults.begin() );
      delete pgR;
    }
  } else if ( result )
    PQclear( result );

  recent.clear();
  result = 0;
  currentResult = 0;
}

bool AQPSQLResultPrivate::nextResult( int i )
{
  int curResult = ( int ) ( i / LIMIT_RESULT );
  if ( curResult == currentResult || idCursor.isEmpty() )
    return true;

  PGresult * res = 0;
  AQPGresult * aqres = cachedResults.value( curResult, 0 );
  if ( aqres )
    res = aqres->result_;

  if ( res ) {
    result = res;
    currentResult = curResult;
  } else {
    int status = PGRES_COMMAND_OK, moveRows = 0;
    QString qr;

    if ( !q->isForwardOnly() ) {
      qr = QLatin1String( "MOVE ABSOLUTE " ) +
           QString::number( curResult * LIMIT_RESULT ) +
           QLatin1String( " FROM " ) + idCursor;

      if ( driver->isUtf8 ) {
        res = PQexec( driver->connection, qr.toUtf8().constData() );
      } else {
        res = PQexec( driver->connection, qr.toLocal8Bit().constData() );
      }

      status = PQresultStatus( res );
      moveRows = PQntuples( res );
      PQclear( res );

      if ( status != PGRES_COMMAND_OK )
        return false;
    } else if ( curResult < currentResult ) {
      aqDebug( AQ_TR( "SqlDriver",
                      "Esta consulta sólo puede posicionarse hacia adelante." ) );
      return false;
    }

    if ( status == PGRES_COMMAND_OK ) {
      qr = QLatin1String( "FETCH FORWARD " ) + QString::number( LIMIT_RESULT ) +
           QLatin1String( " FROM " ) + idCursor;

      if ( driver->isUtf8 ) {
        res = PQexec( driver->connection, qr.toUtf8().constData() );
      } else {
        res = PQexec( driver->connection, qr.toLocal8Bit().constData() );
      }

      status = PQresultStatus( res );
      if ( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK ) {
        if ( status == PGRES_TUPLES_OK ) {
          int fetchedRows = PQntuples( res );
          if ( fetchedRows < LIMIT_RESULT ) {
            currentSize = ( curResult * LIMIT_RESULT ) + fetchedRows + moveRows;
            if ( curResult == 0 || q->isForwardOnly() ) {
              closeCursor();
              cleanupCache();
            }
            if ( i >= currentSize ) {
              PQclear( res );
              return false;
            }
            if ( curResult == 0 || q->isForwardOnly() ) {
              result = res;
              return true;
            }
          }
        } else {
          PQclear( res );
          return false;
        }
      } else {
        PQclear( res );
        return false;
      }
    }

    AQPGresult * prevResult = cachedResults.take( curResult );
    if ( prevResult )
      delete prevResult;
    if ( ( curResult > currentResult ) && q->isForwardOnly() ) {
      prevResult = cachedResults.take( currentResult );
      if ( prevResult )
        delete prevResult;
    }

    result = res;
    currentResult = curResult;
    cachedResults.insert( curResult, new AQPGresult( result ) );

    recent.enqueue( curResult );
    if ( recent.size() > TAM_RECENT ) {
      int old = recent.dequeue();
      if ( old != curResult ) {
        AQPGresult * oldRes = cachedResults.take( old );
        if ( oldRes )
          delete oldRes;
      }
    }
  }

  return true;
}

void AQPSQLResultPrivate::calcSize()
{
  if ( q->isSelect() && !qry.isEmpty() ) {
    if ( !idCursor.isEmpty() && !q->isForwardOnly() ) {
      PGresult * res = 0;
      QString qr( QLatin1String( "MOVE ALL FROM " ) + idCursor );

      if ( driver->isUtf8 ) {
        res = PQexec( driver->connection, qr.toUtf8().constData() );
      } else {
        res = PQexec( driver->connection, qr.toLocal8Bit().constData() );
      }

      currentSize = QString( PQcmdTuples( res ) ).toInt();
      PQclear( res );

      qr = QLatin1String( "MOVE ABSOLUTE " ) +
           QString::number( ( currentResult + 1 ) * LIMIT_RESULT ) +
           QLatin1String( " FROM " ) + idCursor;

      if ( driver->isUtf8 ) {
        res = PQexec( driver->connection, qr.toUtf8().constData() );
      } else {
        res = PQexec( driver->connection, qr.toLocal8Bit().constData() );
      }
      PQclear( res );
    } else {
      QString qryNoOrder = qry;
      qryNoOrder.replace( QRegExp( QLatin1String( "ORDER BY.*ASC" ), Qt::CaseInsensitive ), QLatin1String( "" ) );
      qryNoOrder.replace( QRegExp( QLatin1String( "ORDER BY.*DESC" ), Qt::CaseInsensitive ), QLatin1String( "" ) );
      qryNoOrder.replace( QRegExp( QLatin1String( "ORDER BY.*$" ), Qt::CaseInsensitive ), QLatin1String( "" ) );
      qryNoOrder.replace( QRegExp( QLatin1String( "ORDER BY.*;" ), Qt::CaseInsensitive ), QLatin1String( ";" ) );
      qryNoOrder.replace( QRegExp( QLatin1String( "ORDER BY.*LIMIT" ), Qt::CaseInsensitive ), QLatin1String( "limit" ) );
      qryNoOrder.replace( QRegExp( QLatin1String( "ORDER BY.*OFFSET" ), Qt::CaseInsensitive ), QLatin1String( "offset" ) );
      qryNoOrder.replace( QRegExp( QLatin1String( "ORDER BY.*FOR" ), Qt::CaseInsensitive ), QLatin1String( "for" ) );

      PGresult * res = 0;
      QString qr( QLatin1String( "select count(*) from ( " ) + qryNoOrder + QLatin1String( " ) as cursize" ) );
      if ( driver->isUtf8 ) {
        res = PQexec( driver->connection, qr.toUtf8().constData() );
      } else {
        res = PQexec( driver->connection, qr.toLocal8Bit().constData() );
      }
      int status = PQresultStatus( res );
      if ( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK ) {
        QString val( PQgetvalue( res, 0, 0 ) );
        currentSize = val.toInt();
        PQclear( res );
      } else
        PQclear( res );
    }
  } else
    currentSize = -1;
}

static QVariant::Type qDecodePSQLType( int t )
{
  QVariant::Type type = QVariant::Invalid;
  switch ( t ) {
  case QBOOLOID:
    type = QVariant::Bool;
    break;
  case QINT8OID:
    type = QVariant::LongLong;
    break;
  case QINT2OID:
  case QINT4OID:
  case QOIDOID:
  case QREGPROCOID:
  case QXIDOID:
  case QCIDOID:
    type = QVariant::Int;
    break;
  case QNUMERICOID:
  case QFLOAT4OID:
  case QFLOAT8OID:
    type = QVariant::Double;
    break;
  case QABSTIMEOID:
  case QRELTIMEOID:
  case QDATEOID:
    type = QVariant::Date;
    break;
  case QTIMEOID:
  case QTIMETZOID:
    type = QVariant::Time;
    break;
  case QTIMESTAMPOID:
  case QTIMESTAMPTZOID:
    type = QVariant::DateTime;
    break;
  case QBYTEAOID:
    type = QVariant::ByteArray;
    break;
  default:
    type = QVariant::String;
    break;
  }
  return type;
}

AQPSQLResult::AQPSQLResult( const AQPSQLDriver * db, const AQPSQLDriverPrivate * p ) :
    QSqlResult( db )
{
  d = new AQPSQLResultPrivate( this );
  d->driver = p;
}

AQPSQLResult::~AQPSQLResult()
{
  cleanup();
  delete d;
}

QVariant AQPSQLResult::handle() const
{
  return qVariantFromValue( d->result );
}

void AQPSQLResult::cleanup()
{
  d->closeCursor();
  d->cleanupCache();
  d->qry = QString();
  setAt( QSql::BeforeFirstRow );
  d->currentSize = -1;
  setActive( false );
}

bool AQPSQLResult::fetch( int i )
{
  if ( !isActive() || !isSelect() )
    return false;
  if ( i < 0 )
    return false;
  if ( at() == i )
    return true;
  if ( !d->idCursor.isEmpty() )
    if ( !d->nextResult( i ) )
      return false;
  if ( i >= d->currentSize )
    return false;
  setAt( i );
  return true;
}

bool AQPSQLResult::fetchFirst()
{
  return fetch( 0 );
}

bool AQPSQLResult::fetchLast()
{
  return fetch( d->currentSize - 1 );
}

QVariant AQPSQLResult::data( int i )
{
  if ( i >= PQnfields( d->result ) ) {
    aqDebug( AQ_TR( "SqlDriver",
                    "AQPSQLResult::data: column %1 out of range" ).arg( i ) );
    return QVariant();
  }

  int ptype = PQftype( d->result, i );
  QVariant::Type type = qDecodePSQLType( ptype );
  
  if ( PQgetisnull( d->result, at() % LIMIT_RESULT, i ) )
    return QVariant( type );

  const char *val = PQgetvalue( d->result, at() % LIMIT_RESULT, i );

  switch ( type ) {
  case QVariant::Bool:
    return QVariant( ( bool ) ( val[ 0 ] == 't' ) );
  case QVariant::String:
    return d->driver->isUtf8 ? QString::fromUtf8( val ) : QString::fromAscii( val );
  case QVariant::LongLong:
    if ( val[ 0 ] == '-' )
      return QString::fromLatin1( val ).toLongLong();
    else
      return QString::fromLatin1( val ).toULongLong();
  case QVariant::Int:
    return atoi( val );
  case QVariant::Double:
    if ( ptype == QNUMERICOID )
      return QString::fromAscii( val );
    return strtod( val, 0 );
  case QVariant::Date:
    if ( val[ 0 ] == '\0' ) {
      return QVariant( QDate() );
    } else {
#ifndef QT_NO_DATESTRING
      return QVariant( QDate::fromString( QString::fromLatin1( val ), Qt::ISODate ) );
#else
      return QVariant( QString::fromLatin1( val ) );
#endif

    }
  case QVariant::Time: {
      const QString str = QString::fromLatin1( val );
#ifndef QT_NO_DATESTRING
      if ( str.isEmpty() )
        return QVariant( QTime() );
      if ( str.at( str.length() - 3 ) == QLatin1Char( '+' ) )
        // strip the timezone
        return QVariant( QTime::fromString( str.left( str.length() - 3 ), Qt::ISODate ) );
      return QVariant( QTime::fromString( str, Qt::ISODate ) );
#else
      return QVariant( str );
#endif

    }
  case QVariant::DateTime: {
      QString dtval = QString::fromLatin1( val );
#ifndef QT_NO_DATESTRING
      if ( dtval.length() < 10 )
        return QVariant( QDateTime() );
      // remove the timezone
      if ( dtval.at( dtval.length() - 3 ) == QLatin1Char( '+' ) )
        dtval.chop( 3 );
      // milliseconds are sometimes returned with 2 digits only
      if ( dtval.at( dtval.length() - 3 ).isPunct() )
        dtval += QLatin1Char( '0' );
      if ( dtval.isEmpty() )
        return QVariant( QDateTime() );
      else
        return QVariant( QDateTime::fromString( dtval, Qt::ISODate ) );
#else
      return QVariant( dtval );
#endif

    }
  case QVariant::ByteArray: {
      size_t len;
      unsigned char *data = PQunescapeBytea( ( unsigned char* ) val, &len );
      QByteArray ba( ( const char* ) data, len );
      qPQfreemem( data );
      return QVariant( ba );
    }
  default:
  case QVariant::Invalid:
    aqDebug( "AQPSQLResult::data: unknown data type" );
  }
  return QVariant();
}

bool AQPSQLResult::isNull( int field )
{
  PQgetvalue( d->result, at() % LIMIT_RESULT, field );
  return PQgetisnull( d->result, at() % LIMIT_RESULT, field );
}

bool AQPSQLResult::reset( const QString & query )
{
  if ( query.isEmpty() )
    return false;
  const QSqlDriver * dr = driver();
  if ( !dr || !dr->isOpen() || dr->isOpenError() )
    return false;

  QString qr( query.trimmed() );
  QString qLimit;

  qr.replace( QLatin1String( "=;" ), QLatin1String( "= NULL;" ) );
  while ( qr.endsWith( QLatin1Char( ';' ) ) )
    qr.truncate( qr.length() - 1 );

  qLimit = qr;
  if ( qr.left( 7 ).toUpper().contains( QLatin1String( "SELECT" ) ) &&
       !qLimit.toUpper().contains( QLatin1String( "LIMIT" ) ) )
    qLimit += QLatin1String( " LIMIT " ) + QString::number( LIMIT_RESULT + 1 );

  cleanup();
  if ( d->driver->isUtf8 ) {
    d->result = PQexec( d->driver->connection, qLimit.toUtf8().constData() );
  } else {
    d->result = PQexec( d->driver->connection, qLimit.toLocal8Bit().constData() );
  }

  int status = PQresultStatus( d->result );
  if ( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK ) {
    if ( status == PGRES_TUPLES_OK ) {
      d->currentSize = PQntuples( d->result );
      setSelect( true );
    } else {
      setSelect( false );
      setActive( true );
      return !!d->result;
    }
  } else {
    QString msgError = PQresultErrorMessage( d->result );
    d->q->setLastError( qMakeError( AQ_TR( "SqlDriver", "La consulta a la base de datos ha fallado" ),
                                    QSqlError::StatementError, d->driver ) );
    aqDebug( msgError + "\n" + qr + "\n" );
    if ( d->result ) {
      PQclear( d->result );
      d->result = 0;
    }
    return false;
  }

  if ( d->currentSize >= LIMIT_RESULT ) {
    if ( d->result ) {
      d->qry = qr;
      d->idCursor = "C" + QString::number( d->driver->idConn ) + QString::number( AQPSQLResultPrivate::cursorCounter++ );
      if ( !d->openCursor() )
        return false;
      d->calcSize();
    } else
      return false;
  }

  setActive( true );
  return true;
}

int AQPSQLResult::size()
{
  return d->currentSize;
}

int AQPSQLResult::numRowsAffected()
{
  return QString::fromLatin1( PQcmdTuples( d->result ) ).toInt();
}

QVariant AQPSQLResult::lastInsertId() const
{
  if ( isActive() ) {
    Oid id = PQoidValue( d->result );
    if ( id != InvalidOid )
      return QVariant( id );
  }
  return QVariant();
}

QSqlRecord AQPSQLResult::record() const
{
  QSqlRecord info;
  if ( !isActive() || !isSelect() )
    return info;

  int count = PQnfields( d->result );
  for ( int i = 0; i < count; ++i ) {
    QSqlField f;
    if ( d->driver->isUtf8 )
      f.setName( QString::fromUtf8( PQfname( d->result, i ) ) );
    else
      f.setName( QString::fromLocal8Bit( PQfname( d->result, i ) ) );
    f.setType( qDecodePSQLType( PQftype( d->result, i ) ) );
    int len = PQfsize( d->result, i );
    int precision = PQfmod( d->result, i );
    // swap length and precision if length == -1
    if ( len == -1 && precision > -1 ) {
      len = precision - 4;
      precision = -1;
    }
    f.setLength( len );
    f.setPrecision( precision );
    f.setSqlType( PQftype( d->result, i ) );
    info.append( f );
  }
  return info;
}

///////////////////////////////////////////////////////////////////

static bool setEncodingUtf8( PGconn * connection )
{
  PGresult * result = PQexec( connection, "SET CLIENT_ENCODING TO 'UNICODE'" );
  int status = PQresultStatus( result );
  PQclear( result );
  return status == PGRES_COMMAND_OK;
}

static void setDatestyle( PGconn * connection )
{
  PGresult * result = PQexec( connection, "SET DATESTYLE TO 'ISO'" );
  int status = PQresultStatus( result );
  if ( status != PGRES_COMMAND_OK )
    aqDebug( QString( PQerrorMessage( connection ) ) );
  PQclear( result );
}

static bool setTransactionReadCommited( PGconn * connection )
{
  PGresult * result = PQexec( connection,
                              "SET SESSION CHARACTERISTICS AS TRANSACTION ISOLATION LEVEL READ COMMITTED READ WRITE" );
  int status = PQresultStatus( result );
  PQclear( result );
  return status == PGRES_COMMAND_OK;
}

static AQPSQLDriver::Protocol getPSQLVersion( PGconn * connection )
{
  PGresult * result = PQexec( connection, "select version()" );
  int status = PQresultStatus( result );
  if ( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK ) {
    QString val = QString::fromAscii( PQgetvalue( result, 0, 0 ) );
    PQclear( result );
    QRegExp rx( QLatin1String( "(\\d+)\\.(\\d+)" ) );
    rx.setMinimal( true );
    if ( rx.indexIn( val ) != -1 ) {
      int vMaj = rx.cap(1).toInt();
      int vMin = rx.cap(2).toInt();
      if (vMaj < 6) {
        qWarning("This version of PostgreSQL is not supported and may not work.");
        return AQPSQLDriver::Version6;
      }
      if (vMaj == 6) {
        return AQPSQLDriver::Version6;
      } else if (vMaj == 7) {
        if (vMin < 1)
          return AQPSQLDriver::Version7;
        else if (vMin < 3)
          return AQPSQLDriver::Version71;
        else if (vMin < 4)
          return AQPSQLDriver::Version73;
        return AQPSQLDriver::Version74;
      } else if (vMaj == 8) {
        if (vMin < 1)
          return AQPSQLDriver::Version8;
        else if (vMin < 2)
          return AQPSQLDriver::Version81;
        else if (vMin < 3)
          return AQPSQLDriver::Version82;
        else if (vMin < 4)
          return AQPSQLDriver::Version83;
        return AQPSQLDriver::Version84;
      } else if (vMaj == 9) {
        if (vMin < 1)
          return AQPSQLDriver::Version9;
        return AQPSQLDriver::Version91;
      }
      return AQPSQLDriver::Version7;
    }
  } else {
    aqDebug( "This version of PostgreSQL is not supported and may not work." );
    PQclear( result );
  }

  return AQPSQLDriver::Version6;
}

AQPSQLDriver::AQPSQLDriver( QObject * parent )
    : AQSqlDriver( parent )
{
  init();
}

AQPSQLDriver::AQPSQLDriver( PGconn * conn, QObject * parent )
    : AQSqlDriver( parent )
{
  init();
  d->connection = conn;
  if ( conn ) {
    d->pro = getPSQLVersion( d->connection );
    setOpen( true );
    setOpenError( false );
  }
}

void AQPSQLDriver::init()
{
  d = new AQPSQLDriverPrivate();
}

AQPSQLDriver::~AQPSQLDriver()
{
  if ( d->connection )
    PQfinish( d->connection );
  delete d;
}

void AQPSQLDriver::setAQSqlDatabase( AQSqlDatabase db )
{
  db_ = db;
  d->db_ = db_;
}

void AQPSQLDriver::finish()
{
  db_ = AQSqlDatabase();
  d->db_ = AQSqlDatabase();
  d->hashIndexes.clear();
}

QVariant AQPSQLDriver::handle() const
{
  return qVariantFromValue( d->connection );
}

bool AQPSQLDriver::hasFeature( DriverFeature f ) const
{
  switch ( f ) {
  case Transactions:
  case QuerySize:
  case LastInsertId:
    return true;
  case BatchOperations:
  case PreparedQueries:
  case NamedPlaceholders:
  case PositionalPlaceholders:
  case SimpleLocking:
  case LowPrecisionNumbers:
    return false;
  case BLOB:
    return d->pro >= AQPSQLDriver::Version71;
  case Unicode:
    return d->isUtf8;
  default:
    return false;
  }
}

/*
   Quote a string for inclusion into the connection string
   \ -> \\
   ' -> \'
   surround string by single quotes
 */
static QString qQuote( QString s )
{
  s.replace( QLatin1Char( '\\' ), QLatin1String( "\\\\" ) );
  s.replace( QLatin1Char( '\'' ), QLatin1String( "\\'" ) );
  s.append( QLatin1Char( '\'' ) ).prepend( QLatin1Char( '\'' ) );
  return s;
}

bool AQPSQLDriver::open( const QString & db,
                         const QString & user,
                         const QString & password,
                         const QString & host,
                         int port,
                         const QString & connOpts )
{
  if ( isOpen() )
    close();
  QString connectString;
  if ( !host.isEmpty() )
    connectString.append( QLatin1String( "host=" ) ).append( qQuote( host ) );
  if ( !db.isEmpty() )
    connectString.append( QLatin1String( " dbname=" ) ).append( qQuote( db ) );
  if ( !user.isEmpty() )
    connectString.append( QLatin1String( " user=" ) ).append( qQuote( user ) );
  if ( !password.isEmpty() )
    connectString.append( QLatin1String( " password=" ) ).append( qQuote( password ) );
  if ( port != -1 )
    connectString.append( QLatin1String( " port=" ) ).append( qQuote( QString::number( port ) ) );

  // add any connect options - the server will handle error detection
  if ( !connOpts.isEmpty() ) {
    QString opt = connOpts;
    opt.replace( QLatin1Char( ';' ), QLatin1Char( ' ' ), Qt::CaseInsensitive );
    connectString.append( QLatin1Char( ' ' ) ).append( opt );
  }

  d->connection = PQconnectdb( connectString.toLocal8Bit().constData() );
  if ( PQstatus( d->connection ) == CONNECTION_BAD ) {
    setLastError( qMakeError( tr( "No se puede conectar a la base de datos" ),
                              QSqlError::ConnectionError, d ) );
    setOpenError( true );
    PQfinish( d->connection );
    d->connection = 0;
    return false;
  }

  d->idConn = PQbackendPID( d->connection );
#ifdef AQ_DEBUG
  PQsetErrorVerbosity( d->connection, PQERRORS_VERBOSE );
#endif

  d->pro = getPSQLVersion( d->connection );
  d->isUtf8 = setEncodingUtf8( d->connection );
  setDatestyle( d->connection );
  setTransactionReadCommited( d->connection );

  setOpen( true );
  setOpenError( false );
  return true;
}

void AQPSQLDriver::close()
{
  if ( isOpen() ) {
    if ( d->connection )
      PQfinish( d->connection );
    d->connection = 0;
    setOpen( false );
    setOpenError( false );
  }
}

QSqlResult * AQPSQLDriver::createResult() const
{
  return new AQPSQLResult( this, d );
}

bool AQPSQLDriver::beginTransaction()
{
  if ( !isOpen() ) {
    aqDebug( "AQPSQLDriver::beginTransaction: Database not open" );
    return false;
  }
  PGresult* res = PQexec( d->connection, "BEGIN" );
  if ( !res || PQresultStatus( res ) != PGRES_COMMAND_OK ) {
    PQclear( res );
    setLastError( qMakeError( tr( "Could not begin transaction" ),
                              QSqlError::TransactionError, d ) );
    return false;
  }
  PQclear( res );
  return true;
}

bool AQPSQLDriver::commitTransaction()
{
  if ( !isOpen() ) {
    aqDebug( "AQPSQLDriver::commitTransaction: Database not open" );
    return false;
  }
  PGresult* res = PQexec( d->connection, "COMMIT" );
  if ( !res || PQresultStatus( res ) != PGRES_COMMAND_OK ) {
    PQclear( res );
    setLastError( qMakeError( tr( "Could not commit transaction" ),
                              QSqlError::TransactionError, d ) );
    return false;
  }
  PQclear( res );
  return true;
}

bool AQPSQLDriver::rollbackTransaction()
{
  if ( !isOpen() ) {
    aqDebug( "AQPSQLDriver::rollbackTransaction: Database not open" );
    return false;
  }
  PGresult* res = PQexec( d->connection, "ROLLBACK" );
  if ( !res || PQresultStatus( res ) != PGRES_COMMAND_OK ) {
    setLastError( qMakeError( tr( "Could not rollback transaction" ),
                              QSqlError::TransactionError, d ) );
    PQclear( res );
    return false;
  }
  PQclear( res );
  return true;
}

QStringList AQPSQLDriver::tables( QSql::TableType type ) const
{
  QStringList tl;
  if ( !isOpen() )
    return tl;
  QSqlQuery t( createResult() );
  t.setForwardOnly( true );

  if ( type & QSql::Tables )
    d->appendTables( tl, t, QLatin1Char( 'r' ) );
  if ( type & QSql::Views )
    d->appendTables( tl, t, QLatin1Char( 'v' ) );
  if ( type & QSql::SystemTables ) {
    t.exec( QLatin1String( "select relname from pg_class where (relkind = 'r') "
                           "and (relname like 'pg_%') " ) );
    while ( t.next() )
      tl.append( t.value( 0 ).toString() );
  }

  return tl;
}

static void qSplitTableName( QString & tablename, QString & schema )
{
  int dot = tablename.indexOf( QLatin1Char( '.' ) );
  if ( dot == -1 )
    return ;
  schema = tablename.left( dot );
  tablename = tablename.mid( dot + 1 );
}

QSqlIndex AQPSQLDriver::primaryIndex( const QString & tablename ) const
{
  QSqlIndex idx( tablename );

  if ( !isOpen() )
    return idx;

  QDomDocument doc( tablename );
  QDomElement docElem;
  QString shaKey, cacheKey;
  QString stream = d->db_.managerModules() ->content( tablename + ".mtd", shaKey, cacheKey );

  if ( !AQCoreUtil::domDocumentSetContent( doc, stream ) ) {
    aqDebug( "AQManager : " +
             AQ_TR( "SqlDriver",
                    "Error al cargar los metadatos para la tabla %1" ).arg( tablename ) );
    return primaryIndex2( tablename );
  }

  docElem = doc.documentElement();
  AQTableMD * mtd = d->db_.manager() ->metadata( &docElem, true );

  if ( !mtd )
    return primaryIndex2( tablename );

  idx.append( QSqlField( mtd->primaryKey(), AQFieldMD::aqDecodeType( mtd->fieldType( mtd->primaryKey() ) ) ) );
  idx.setName( tablename.toLower() + "_pkey" );

  return idx;
}

QSqlIndex AQPSQLDriver::primaryIndex2( const QString & tablename ) const
{
  QSqlIndex idx( tablename );

  if ( !isOpen() )
    return idx;

  QSqlQuery i( createResult() );
  QString stmt;

  QString tbl = tablename;
  QString schema;
  qSplitTableName( tbl, schema );

  switch ( d->pro ) {
  case AQPSQLDriver::Version6:
    stmt = QLatin1String( "select pg_att1.attname, int(pg_att1.atttypid), pg_cl.relname "
                          "from pg_attribute pg_att1, pg_attribute pg_att2, pg_class pg_cl, pg_index pg_ind "
                          "where lower(pg_cl.relname) = '%1_pkey' "
                          "and pg_cl.oid = pg_ind.indexrelid "
                          "and pg_att2.attrelid = pg_ind.indexrelid "
                          "and pg_att1.attrelid = pg_ind.indrelid "
                          "and pg_att1.attnum = pg_ind.indkey[pg_att2.attnum-1] "
                          "order by pg_att2.attnum" );
    break;
  case AQPSQLDriver::Version7:
  case AQPSQLDriver::Version71:
    stmt = QLatin1String( "select pg_att1.attname, pg_att1.atttypid::int, pg_cl.relname "
                          "from pg_attribute pg_att1, pg_attribute pg_att2, pg_class pg_cl, pg_index pg_ind "
                          "where lower(pg_cl.relname) = '%1_pkey' "
                          "and pg_cl.oid = pg_ind.indexrelid "
                          "and pg_att2.attrelid = pg_ind.indexrelid "
                          "and pg_att1.attrelid = pg_ind.indrelid "
                          "and pg_att1.attnum = pg_ind.indkey[pg_att2.attnum-1] "
                          "order by pg_att2.attnum" );
    break;
  case AQPSQLDriver::Version73:
  case AQPSQLDriver::Version74:
  case AQPSQLDriver::Version8:
  case AQPSQLDriver::Version81:
  case AQPSQLDriver::Version82:
  case AQPSQLDriver::Version83:
    stmt = QLatin1String( "SELECT pg_attribute.attname, pg_attribute.atttypid::int, "
                          "pg_class.relname "
                          "FROM pg_attribute, pg_class "
                          "WHERE %1 pg_class.oid = "
                          "(SELECT indexrelid FROM pg_index WHERE indisprimary = true AND indrelid = "
                          " (SELECT oid FROM pg_class WHERE lower(relname) = '%2')) "
                          "AND pg_attribute.attrelid = pg_class.oid "
                          "AND pg_attribute.attisdropped = false "
                          "ORDER BY pg_attribute.attnum" );
    if ( schema.isEmpty() )
      stmt = stmt.arg( QLatin1String( "pg_table_is_visible(pg_class.oid) AND" ) );
    else
      stmt = stmt.arg( QString::fromLatin1( "pg_class.relnamespace = (select oid from "
                                            "pg_namespace where pg_namespace.nspname = '%1') AND " ).arg( schema.toLower() ) );
    break;
  }

  i.exec( stmt.arg( tbl.toLower() ) );
  while ( i.isActive() && i.next() ) {
    QSqlField f( i.value( 0 ).toString(), qDecodePSQLType( i.value( 1 ).toInt() ) );
    idx.append( f );
    idx.setName( i.value( 2 ).toString() );
  }
  return idx;
}

QSqlRecord AQPSQLDriver::record( const QString & tablename ) const
{
  QSqlRecord fil;

  if ( !isOpen() )
    return fil;

  QDomDocument doc( tablename );
  QDomElement docElem;
  QString shaKey, cacheKey;
  QString stream = d->db_.managerModules() ->content( tablename + ".mtd", shaKey, cacheKey );

  if ( !AQCoreUtil::domDocumentSetContent( doc, stream ) ) {
    aqDebug( "AQManager : " +
             AQ_TR( "SqlDriver",
                    "Error al cargar los metadatos para la tabla %1" ).arg( tablename ) );
    return record2( tablename );
  }

  docElem = doc.documentElement();
  AQTableMD * mtd = d->db_.manager() ->metadata( &docElem, true );

  if ( !mtd )
    return record2( tablename );

  QStringList fieldsNames = mtd->fieldList( false ).split( "," );

  if ( fieldsNames.isEmpty() )
    return record2( tablename );

  for ( QStringList::Iterator it = fieldsNames.begin(); it != fieldsNames.end(); ++it )
    fil.append( QSqlField( ( *it ), AQFieldMD::aqDecodeType( mtd->fieldType( ( *it ) ) ) ) );

  return fil;
}

QSqlRecord AQPSQLDriver::record2( const QString & tablename ) const
{
  QSqlRecord info;

  if ( !isOpen() )
    return info;

  QString tbl = tablename;
  QString schema;
  qSplitTableName( tbl, schema );

  QString stmt;
  switch ( d->pro ) {
  case AQPSQLDriver::Version6:
    stmt = QLatin1String( "select pg_attribute.attname, int(pg_attribute.atttypid), "
                          "pg_attribute.attnotnull, pg_attribute.attlen, pg_attribute.atttypmod, "
                          "int(pg_attribute.attrelid), pg_attribute.attnum "
                          "from pg_class, pg_attribute "
                          "where lower(pg_class.relname) = '%1' "
                          "and pg_attribute.attnum > 0 "
                          "and pg_attribute.attrelid = pg_class.oid " );
    break;
  case AQPSQLDriver::Version7:
    stmt = QLatin1String( "select pg_attribute.attname, pg_attribute.atttypid::int, "
                          "pg_attribute.attnotnull, pg_attribute.attlen, pg_attribute.atttypmod, "
                          "pg_attribute.attrelid::int, pg_attribute.attnum "
                          "from pg_class, pg_attribute "
                          "where lower(pg_class.relname) = '%1' "
                          "and pg_attribute.attnum > 0 "
                          "and pg_attribute.attrelid = pg_class.oid " );
    break;
  case AQPSQLDriver::Version71:
    stmt = QLatin1String( "select pg_attribute.attname, pg_attribute.atttypid::int, "
                          "pg_attribute.attnotnull, pg_attribute.attlen, pg_attribute.atttypmod, "
                          "pg_attrdef.adsrc "
                          "from pg_class, pg_attribute "
                          "left join pg_attrdef on (pg_attrdef.adrelid = "
                          "pg_attribute.attrelid and pg_attrdef.adnum = pg_attribute.attnum) "
                          "where lower(pg_class.relname) = '%1' "
                          "and pg_attribute.attnum > 0 "
                          "and pg_attribute.attrelid = pg_class.oid "
                          "order by pg_attribute.attnum " );
    break;
  case AQPSQLDriver::Version73:
  case AQPSQLDriver::Version74:
  case AQPSQLDriver::Version8:
  case AQPSQLDriver::Version81:
  case AQPSQLDriver::Version82:
  case AQPSQLDriver::Version83:
    stmt = QLatin1String( "select pg_attribute.attname, pg_attribute.atttypid::int, "
                          "pg_attribute.attnotnull, pg_attribute.attlen, pg_attribute.atttypmod, "
                          "pg_attrdef.adsrc "
                          "from pg_class, pg_attribute "
                          "left join pg_attrdef on (pg_attrdef.adrelid = "
                          "pg_attribute.attrelid and pg_attrdef.adnum = pg_attribute.attnum) "
                          "where %1 "
                          "and lower(pg_class.relname) = '%2' "
                          "and pg_attribute.attnum > 0 "
                          "and pg_attribute.attrelid = pg_class.oid "
                          "and pg_attribute.attisdropped = false "
                          "order by pg_attribute.attnum " );
    if ( schema.isEmpty() )
      stmt = stmt.arg( QLatin1String( "pg_table_is_visible(pg_class.oid)" ) );
    else
      stmt = stmt.arg( QString::fromLatin1( "pg_class.relnamespace = (select oid from "
                                            "pg_namespace where pg_namespace.nspname = '%1')" ).arg( schema.toLower() ) );
    break;
  }

  QSqlQuery query( createResult() );
  query.exec( stmt.arg( tbl.toLower() ) );
  if ( d->pro >= AQPSQLDriver::Version71 ) {
    while ( query.next() ) {
      int len = query.value( 3 ).toInt();
      int precision = query.value( 4 ).toInt();
      // swap length and precision if length == -1
      if ( len == -1 && precision > -1 ) {
        len = precision - 4;
        precision = -1;
      }
      QString defVal = query.value( 5 ).toString();
      if ( !defVal.isEmpty() && defVal.at( 0 ) == QLatin1Char( '\'' ) )
        defVal = defVal.mid( 1, defVal.length() - 2 );
      QSqlField f( query.value( 0 ).toString(), qDecodePSQLType( query.value( 1 ).toInt() ) );
      f.setRequired( query.value( 2 ).toBool() );
      f.setLength( len );
      f.setPrecision( precision );
      f.setDefaultValue( defVal );
      f.setSqlType( query.value( 1 ).toInt() );
      info.append( f );
    }
  } else {
    // Postgres < 7.1 cannot handle outer joins
    while ( query.next() ) {
      QString defVal;
      QString stmt2 = QLatin1String( "select pg_attrdef.adsrc from pg_attrdef where "
                                     "pg_attrdef.adrelid = %1 and pg_attrdef.adnum = %2 " );
      QSqlQuery query2( createResult() );
      query2.exec( stmt2.arg( query.value( 5 ).toInt() ).arg( query.value( 6 ).toInt() ) );
      if ( query2.isActive() && query2.next() )
        defVal = query2.value( 0 ).toString();
      if ( !defVal.isEmpty() && defVal.at( 0 ) == QLatin1Char( '\'' ) )
        defVal = defVal.mid( 1, defVal.length() - 2 );
      int len = query.value( 3 ).toInt();
      int precision = query.value( 4 ).toInt();
      // swap length and precision if length == -1
      if ( len == -1 && precision > -1 ) {
        len = precision - 4;
        precision = -1;
      }
      QSqlField f( query.value( 0 ).toString(), qDecodePSQLType( query.value( 1 ).toInt() ) );
      f.setRequired( query.value( 2 ).toBool() );
      f.setLength( len );
      f.setPrecision( precision );
      f.setDefaultValue( defVal );
      f.setSqlType( query.value( 1 ).toInt() );
      info.append( f );
    }
  }

  return info;
}

QString AQPSQLDriver::formatValue( const QSqlField & field, bool trimStrings ) const
{
  QString r;
  if ( field.isNull() ) {
    r = QLatin1String( "NULL" );
  } else {
    switch ( field.type() ) {
    case QVariant::DateTime:
#ifndef QT_NO_DATESTRING
      if ( field.value().toDateTime().isValid() ) {
        QDate dt = field.value().toDateTime().date();
        QTime tm = field.value().toDateTime().time();
        // msecs need to be right aligned otherwise psql
        // interpretes them wrong
        r = QLatin1String( "'" ) + QString::number( dt.year() ) + QLatin1String( "-" )
            + QString::number( dt.month() ) + QLatin1String( "-" )
            + QString::number( dt.day() ) + QLatin1String( " " )
            + tm.toString() + QLatin1String( "." )
            + QString::number( tm.msec() ).rightJustified( 3, QLatin1Char( '0' ) )
            + QLatin1String( "'" );
      } else
#else
      {
        r = QLatin1String( "NULL" );
      }
#endif // QT_NO_DATESTRING
      break;
    case QVariant::Time:
#ifndef QT_NO_DATESTRING

      if ( field.value().toTime().isValid() ) {
        r = field.value().toTime().toString( Qt::ISODate );
      } else
#endif
      {
        r = QLatin1String( "NULL" );
      }
    case QVariant::String: {
        // Escape '\' characters
        r = QSqlDriver::formatValue( field, trimStrings );
        if ( protocol() < AQPSQLDriver::Version82 )
          r.replace( QLatin1String( "\\" ), QLatin1String( "\\\\" ) );
        else {
          if ( r.contains( QLatin1String( "\\" ) ) ) {
            r.replace( QLatin1String( "\\" ), QLatin1String( "\\\\" ) );
            r.prepend( QLatin1Char( 'E' ) );
          }
        }
        break;
      }
    case QVariant::Bool:
      if ( field.value().toBool() )
        r = QLatin1String( "TRUE" );
      else
        r = QLatin1String( "FALSE" );
      break;
    case QVariant::ByteArray: {
        QByteArray ba( field.value().toByteArray() );
        QByteArray b = ba.toHex();
        QString res2(b); 
        r = "E'\\\\x" + res2 + "'";
        break;
    
/*        size_t len;
        unsigned char *data = PQescapeBytea( ( unsigned char* ) ba.constData(), ba.size(), &len );
        r += QLatin1Char( '\'' );
        r += QLatin1String( ( const char* ) data );
        r += QLatin1Char( '\'' );
        if ( protocol() >= AQPSQLDriver::Version82 )
          r.prepend( QLatin1Char( 'E' ) );
        qPQfreemem( data );
        break;*/
      }
    default:
      r = QSqlDriver::formatValue( field, trimStrings );
      break;
    }
  }
  return r;
}

QString AQPSQLDriver::escapeIdentifier( const QString & identifier, IdentifierType ) const
{
  QString res = identifier;
  res.replace( QLatin1Char( '"' ), QLatin1String( "\"\"" ) );
  res.prepend( QLatin1Char( '"' ) ).append( QLatin1Char( '"' ) );
  res.replace( QLatin1Char( '.' ), QLatin1String( "\".\"" ) );
  return res;
}

bool AQPSQLDriver::isOpen() const
{
  return PQstatus( d->connection ) == CONNECTION_OK;
}

bool AQPSQLDriver::tryConnect( const QString & db, const QString & user, const QString & password,
                               const QString & host, int port )
{
  if ( !open( QLatin1String( "template1" ), user, password, host, port, QString() ) ) {
    aqCrit( lastError().driverText().toUtf8() );
    aqCrit( lastError().databaseText().toUtf8() );
    close();
    return false;
  }
  close();
  setOpenError( false );
  setLastError( QSqlError() );

  if ( protocol() < AQPSQLDriver::Version8 ) {
    aqCrit( tr( "La versión de PostgreSQL es inferior a la 8.0.\n"
                "Se necesita PostgreSQL 8.0 o superior." ).toUtf8() );
    close();
    setOpenError( true );
    return false;
  }

  if ( !open( db, user, password, host, port, QString() ) ) {
    aqDebug( tr( "La base de datos %1 no existe" ).arg( db ) );
    close();
    return false;
    //    if ( lastError().type() == QSqlError::ConnectionError ) {
    //      if ( lastError().text().contains( db ) ) {
    //        if ( AQ_Yes == AQ_MSGBOX_QUES( tr( "Crear base de datos" ),
    //                                       tr( "La base de datos %1 no existe. ¿Quiere crearla?" ).arg( db ) ) ) {
    //          close();
    //          setOpenError( false );
    //          setLastError( QSqlError() );
    //          open( QLatin1String( "template1" ), user, password, host, port, QString::null );
    //          QSqlQuery qry( createResult() );
    //          qry.exec( QLatin1String( "CREATE DATABASE " ) + db + QLatin1String( " WITH ENCODING = 'UNICODE'" ) );
    //          if ( lastError().type() != QSqlError::NoError ) {
    //            AQ_MSGBOX_CRIT( tr( "Error" ),
    //                            QString( lastError().driverText().toUtf8() ) + "\n" +
    //                            QString( lastError().databaseText().toUtf8() ) );
    //            close();
    //            return false;
    //          } else {
    //            close();
    //            setOpenError( false );
    //            setLastError( QSqlError() );
    //            if ( !open( db, user, password, host, port, QString::null ) ) {
    //              AQ_MSGBOX_CRIT( tr( "Error" ),
    //                              QString( lastError().driverText().toUtf8() ) + "\n" +
    //                              QString( lastError().databaseText().toUtf8() ) );
    //              close();
    //              return false;
    //            }
    //          }
    //        }
    //      }
    //
    //      if ( lastError().type() != QSqlError::NoError ) {
    //        AQ_MSGBOX_CRIT( tr( "Error" ),
    //                        QString( lastError().driverText().toUtf8() ) + "\n" +
    //                        QString( lastError().databaseText().toUtf8() ) );
    //        close();
    //        return false;
    //      }
    //    }
  }

  close();
  setOpenError( false );
  setLastError( QSqlError() );
  return true;
}

QString AQPSQLDriver::sqlCreateTable( AQTableMD * tmd ) const
{
  Q_ASSERT( tmd );

  QString primaryKey;
  QString sql( QLatin1String( "CREATE TABLE " ) + tmd->name() + QLatin1String( " (" ) );
  QString seq;

  AQFieldMDHash fieldList( tmd->fieldList() );

  unsigned int unlocks = 0;
  for ( AQFieldMDHash::const_iterator it = fieldList.begin(); it != fieldList.end(); ++it )
    if ( ( *it ) ->type() == AQCore::Unlock )
      ++unlocks;

  if ( unlocks > 1 ) {
    aqDebug( "AQManager : " +
             AQ_TR( "SqlDriver", "No se ha podido crear la tabla " ) + tmd->name() );
    aqDebug( "AQManager : " +
             AQ_TR( "SqlDriver", "Hay más de un campo tipo unlock. Solo puede haber uno." ) );
    return QString();
  }

  AQFieldMD * field;

  for ( AQFieldMDHash::const_iterator it = fieldList.begin(); it != fieldList.end(); ) {
    field = *it;
    sql += field->name();

    switch ( field->type() ) {
    case QVariant::Int:
      sql += QLatin1String( " INT2" );
      break;

    case QVariant::UInt:
      sql += QLatin1String( " INT4" );
      break;

    case QVariant::Bool:
    case AQCore::Unlock:
      sql += QLatin1String( " BOOLEAN" );
      break;

    case QVariant::Double:
      sql += QLatin1String( " FLOAT8" );
      break;

    case QVariant::Time:
      sql += QLatin1String( " TIME" );
      break;

    case QVariant::Date:
      sql += QLatin1String( " DATE" );
      break;

    case QVariant::Pixmap:
      sql += QLatin1String( " TEXT" );
      break;

    case QVariant::String:
      sql += QLatin1String( " VARCHAR" );
      break;

    case QVariant::StringList:
      sql += QLatin1String( " TEXT" );
      break;

    case QVariant::ByteArray:
      sql += QLatin1String( " BYTEA" );
      break;

    case AQCore::Serial: {
        seq = tmd->name() + QLatin1Char( '_' ) + field->name() + QLatin1String( "_seq" );
        QSqlQuery q( createResult() );
        q.exec( QLatin1String( "SELECT relname FROM pg_class WHERE relname='" ) + seq + QLatin1String( "';" ) );
        if ( !q.next() )
          q.exec( QLatin1String( "CREATE SEQUENCE " ) + seq + QLatin1Char( ';' ) );
        sql += QLatin1String( " INT4 DEFAULT NEXTVAL('" ) + seq + QLatin1String( "')" );
      }
      break;
    }

    int longitud = field->length();

    if ( longitud > 0 )
      sql += QLatin1Char( '(' ) + QString::number( longitud ) + QLatin1Char( ')' );

    if ( field->isPrimaryKey() ) {
      if ( primaryKey.isEmpty() ) {
        sql += QLatin1String( " PRIMARY KEY" );
        primaryKey = field->name();
      } else {
        aqDebug( AQ_TR( "SqlDriver", "AQManager : Tabla -> " ) +
                 tmd->name() +
                 AQ_TR( "SqlDriver", " . Se ha intentado poner una segunda clave primaria para el campo " ) +
                 field->name() + AQ_TR( "SqlDriver", " , pero el campo " ) +
                 primaryKey +
                 AQ_TR( "SqlDriver",
                        " ya es clave primaria. Sólo puede existir una clave primaria en AQTableMetaData,"
                        "use AQCompoundKey para crear claves compuestas." ) );
        return QString();
      }
    } else {
      if ( field->isUnique() )
        sql += QLatin1String( " UNIQUE" );
      if ( !field->allowNull() )
        sql += QLatin1String( " NOT NULL" );
      else
        sql += QLatin1String( " NULL" );
    }

    ++it;
    if ( it != fieldList.end() )
      sql += QLatin1Char( ',' );
  }

  sql += QLatin1Char( ')' );

  return sql;
}

QVariant AQPSQLDriver::nextSerialVal( const QString & table, const QString & field ) const
{
  QString seq( table + QLatin1Char( '_' ) + field + QLatin1String( "_seq" ) );
  QSqlQuery query( createResult() );
  query.exec( QLatin1String( "SELECT nextval('" ) + seq + QLatin1String( "')" ) );
  query.next();
  return query.value( 0 );
}

QString AQPSQLDriver::formatValueLike( int t, const QVariant & v, const bool upper ) const
{
  QString res( "IS NULL" );

  switch ( t ) {
  case QVariant::Bool: {
      QString s( v.toString().left( 1 ).toUpper() );
      if ( s == AQ_TR( "SqlDriver", "Sí" ).left( 1 ).toUpper() )
        res = QLatin1String( "='t'" );
      else if ( s == AQ_TR( "SqlDriver", "No" ).left( 1 ).toUpper() )
        res = QLatin1String( "='f'" );
    }
    break;
  case QVariant::Date:
    res = QLatin1String( "::text LIKE '%%" ) + AQCoreUtil::dateDMAtoAMD( v.toString() ) + QLatin1Char( '\'' );
    break;
  case QVariant::Time: {
      QTime t( v.toTime() );
      if ( t.isValid() && !t.isNull() )
        res = QLatin1String( "::text LIKE '" ) + t.toString( Qt::ISODate ) + QLatin1String( "%%'" );
    }
    break;
  default: {
      res = v.toString();
      res.replace( QLatin1Char( '\'' ), QLatin1String( "''" ) );

      if ( upper )
        res = QLatin1Char( '\'' ) + res.toUpper();
      else
        res = QLatin1Char( '\'' ) + res;

      if ( protocol() < AQPSQLDriver::Version82 )
        res.replace( QLatin1String( "\\" ), QLatin1String( "\\\\" ) );
      else {
        if ( res.contains( QLatin1String( "\\" ) ) ) {
          res.replace( QLatin1String( "\\" ), QLatin1String( "\\\\" ) );
          res.prepend( QLatin1Char( 'E' ) );
        }
      }

      res = QLatin1String( "::text LIKE " ) + res + QLatin1String( "%%'" );
      break;
    }
  }

  return res;
}

QString AQPSQLDriver::formatValue( int t, const QVariant & v, const bool upper ) const
{
  QString res;

  switch ( AQFieldMD::aqDecodeType( t ) ) {
  case QVariant::Bool: {
      QString s( v.toString().left( 1 ).toUpper() );
      if ( s == AQ_TR( "SqlDriver", "Sí" ).left( 1 ).toUpper() )
        res = "'t'";
      else if ( s == AQ_TR( "SqlDriver", "No" ).left( 1 ).toUpper() )
        res = "'f'";
      else
        res = tr( "NULL" );
    }
    break;
  case QVariant::Date:
    res = QLatin1Char( '\'' ) + AQCoreUtil::dateDMAtoAMD( v.toString() ) + QLatin1Char( '\'' );
    break;
  case QVariant::Time: {
      QTime t( v.toTime() );
      if ( t.isValid() && !t.isNull() )
        res = QLatin1Char( '\'' ) + t.toString( Qt::ISODate ) + QLatin1Char( '\'' );
      else
        res = tr( "NULL" );
    }
    break;
  case QVariant::Int:
  case QVariant::UInt:
  case QVariant::Double:
    res = v.toString();
    break;
  default: {
      res = v.toString();
      res.replace( QLatin1Char( '\'' ), QLatin1String( "''" ) );

      if ( upper )
        res = QLatin1Char( '\'' ) + res.toUpper() + QLatin1Char( '\'' );
      else
        res = QLatin1Char( '\'' ) + res + QLatin1Char( '\'' );

      if ( protocol() < AQPSQLDriver::Version82 )
        res.replace( QLatin1String( "\\" ), QLatin1String( "\\\\" ) );
      else {
        if ( res.contains( QLatin1String( "\\" ) ) ) {
          res.replace( QLatin1String( "\\" ), QLatin1String( "\\\\" ) );
          res.prepend( QLatin1Char( 'E' ) );
        }
      }
      break;
    }
  }

  return res;
}

void AQPSQLDriver::createIndex( const QString & fieldName, const QString & tableName, bool upper ) const
{
  if ( !isOpen() || fieldName.isEmpty() || tableName.isEmpty() )
    return ;

  if ( d->hashIndexes.isEmpty() ) {
    QSqlQuery idxs( createResult() );
    idxs.exec( "select indexname from pg_indexes where indexname like '%_m1_idx'" );
    while ( idxs.next() )
      d->hashIndexes.insert( idxs.value( 0 ).toString(), true );
  }

  QString indexName = tableName + "_" + fieldName + "_m1_idx";
  indexName.replace( " ", "" ).replace( ",", "" );

  if ( d->hashIndexes.contains( indexName ) )
    return ;

  QString indexNameUp = tableName + "_" + fieldName + "up_m1_idx";
  indexNameUp.replace( " ", "" ).replace( ",", "" );

  PGconn * conn = d->connection;
  PGresult * result = PQexec( conn,
                              QString( "select relname from pg_class where relname = '" +
                                       tableName + "'" ).toUtf8().constData() );
  int status = PQresultStatus( result );

  if ( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK ) {
    int size = PQntuples( result );
    PQclear( result );

    if ( size ) {
      result = PQexec( conn,
                       QString( "select indexname from pg_indexes where indexname = '" +
                                indexName + "'" ).toUtf8().constData() );
      status = PQresultStatus( result );

      if ( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK ) {
        size = PQntuples( result );
        PQclear( result );

        if ( !size ) {
          result = PQexec( conn,
                           QString( "create index " + indexName + " on " +
                                    tableName + " (" + fieldName + ")" ).toUtf8().constData() );
          status = PQresultStatus( result );

          if ( status == PGRES_COMMAND_OK ) {
            PQclear( result );
            d->hashIndexes.insert( indexName, true );
          } else {
            PQclear( result );
            aqDebug( QString( "ERROR : create index %1 on %2 ( %3 ) " )
                     .arg( indexName )
                     .arg( tableName )
                     .arg( fieldName ) );
          }
        }
      } else
        PQclear( result );

      if ( upper && !fieldName.contains( "," ) ) {
        result = PQexec( conn,
                         QString( "select indexname from pg_indexes where indexname = '" +
                                  indexNameUp + "'" ).toUtf8().constData() );
        status = PQresultStatus( result );

        if ( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK ) {
          size = PQntuples( result );
          PQclear( result );

          if ( !size ) {
            result = PQexec( conn,
                             QString( "create index " + indexNameUp + " on " +
                                      tableName + " (upper(" + fieldName + "))" ).toUtf8().constData() );
            status = PQresultStatus( result );

            if ( status == PGRES_COMMAND_OK ) {
              PQclear( result );
              d->hashIndexes.insert( indexNameUp, true );
            } else {
              PQclear( result );
              aqDebug( QString( "ERROR : create index %1 on %2 ( %3 ) " )
                       .arg( indexNameUp )
                       .arg( tableName )
                       .arg( fieldName ) );
            }
          }
        } else
          PQclear( result );
      }
    }
  } else
    PQclear( result );
}

AQPSQLDriver::Protocol AQPSQLDriver::protocol() const
{
  return d->pro;
}

bool AQPSQLDriver::canSavePoint() const
{
  return ( protocol() >= AQPSQLDriver::Version8 );
}

bool AQPSQLDriver::savePoint( const QString & n ) const
{
  return ( canSavePoint() ?
           QSqlQuery( createResult() ).exec( QLatin1String( "savepoint sv_" ) + n ) :
           false );
}

bool AQPSQLDriver::releaseSavePoint( const QString & n ) const
{
  return ( canSavePoint() ?
           QSqlQuery( createResult() ).exec( QLatin1String( "release savepoint sv_" ) + n ) :
           false );
}

bool AQPSQLDriver::rollbackSavePoint( const QString & n ) const
{
  return ( canSavePoint() ?
           QSqlQuery( createResult() ).exec( QLatin1String( "rollback to savepoint sv_" ) + n ) :
           false );
}

/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the QtSql module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
