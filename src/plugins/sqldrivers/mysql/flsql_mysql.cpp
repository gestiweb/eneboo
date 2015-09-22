/***************************************************************************
                      flsql_mysql.cpp  -  description
                            -------------------
   begin                : lun ago 23 2004
   copyright            : (C) 2004-2005 by InfoSiAL, S.L.
   email                : mail@infosial.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/****************************************************************************
**
** Implementation of MYSQL driver classes
**
** Created : 001103
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the sql module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#define QMYSQL_DRIVER_NAME "FLQMYSQL4"

#include "flsql_mysql.h"
#include <private/qsqlextension_p.h>

#include <qapplication.h>
#include <qdom.h>
#include <qprogressdialog.h>
#include <qdatetime.h>
#include <qvaluevector.h>
#include <qsqlrecord.h>
#include <qtextcodec.h>
#include <qcache.h>

#ifdef Q_OS_WIN32
// comment the next line out if you want to use MySQL/embedded on Win32 systems.
// note that it will crash if you don't statically link to the mysql/e library!
# define Q_NO_MYSQL_EMBEDDED
#endif

#ifdef FL_QUICK_CLIENT
#define LIMIT_RESULT 49
#else
#define LIMIT_RESULT 99
#endif

#ifndef FL_QUICK_CLIENT
QDict < bool > * QMYSQLDriver::dictIndexes = 0;
#endif

QPtrDict<QSqlOpenExtension> *qSqlOpenExtDict();

class QMYSQLOpenExtension : public QSqlOpenExtension
{
public:
  QMYSQLOpenExtension(QMYSQLDriver *dri)
    : QSqlOpenExtension(), driver(dri) {}
  ~QMYSQLOpenExtension() {}

  bool open(const QString &db,
            const QString &user,
            const QString &password,
            const QString &host,
            int port,
            const QString &connOpts);

private:
  QMYSQLDriver *driver;
};

bool QMYSQLOpenExtension::open(const QString &db,
                               const QString &user,
                               const QString &password,
                               const QString &host,
                               int port,
                               const QString &connOpts)
{
  return driver->open(db, user, password, host, port, connOpts);
}

class QMYSQLCacheInfoPrivate
{
public:
  QMYSQLCacheInfoPrivate()
    : mngInitCount(0) {}

  void clear() {
    cache.clear();
    cacheIdx.clear();
  }

  int mngInitCount;
  QMap<QString, QSqlRecordInfo> cache;
  QMap<QString, QSqlIndex> cacheIdx;
};

class QMYSQLDriverPrivate
{
public:
  QMYSQLDriverPrivate()
    : mysql(0),
      mysqlSerial(0),
      tc(QTextCodec::codecForLocale()),
      idConn(0),
      activeCreateIndex(false),
      forceChapuza(false) {}
  MYSQL *mysql;
  MYSQL *mysqlSerial;
  QTextCodec *tc;
  unsigned long idConn;
  bool activeCreateIndex;
  bool noInnoDB;
  bool forceChapuza;
};

static inline QString toUnicode(QTextCodec *tc, const char *str)
{
  return tc->toUnicode(str);
}

static inline QString toUnicode(QTextCodec *tc, const char *str, int length)
{
  return tc->toUnicode(str, length);
}

static inline QCString fromUnicode(QTextCodec *tc, const QString &str)
{
  return tc->fromUnicode(str);
}

static QTextCodec *codec(MYSQL *mysql)
{
#if MYSQL_VERSION_ID >= 32321
  QTextCodec *heuristicCodec = QTextCodec::codecForName(mysql_character_set_name(mysql));
  if (heuristicCodec)
    return heuristicCodec;
#endif
  return QTextCodec::codecForLocale();
}

class FLMYresult
{
public:
  FLMYresult(MYSQL_RES *result = 0)
    : result_(result) {}
  ~FLMYresult() {
    if (result_)
      mysql_free_result(result_);
  }

  MYSQL_RES *result_;
};

class QMYSQLResultPrivate : public QMYSQLDriverPrivate
{
public:
  QMYSQLResultPrivate();
  MYSQL_RES *result;
  FLMYresult *flresult;
  MYSQL_ROW row;
  QValueVector<QVariant::Type> fieldTypes;
  QCache < FLMYresult > * cachedResults;
  bool limited;
  my_ulonglong numRows;
  int currentResult;
  QString qry;
  bool invalidate;
};

QMYSQLResultPrivate::QMYSQLResultPrivate()
  : QMYSQLDriverPrivate(),
    result(0),
    flresult(0)
{
  cachedResults = new QCache< FLMYresult  >(20, 41);
  cachedResults->setAutoDelete(true);
}

QSqlError qMakeError(const QString &err, int type, const QMYSQLDriverPrivate *p)
{
  QString msg = QString(mysql_error(p->mysql));
  printf("%s\n", msg.latin1());
  return QSqlError(QMYSQL_DRIVER_NAME ": " + err, msg, type, mysql_errno(p->mysql));
}

QSqlError qMakeErrorSerial(const QString &err, int type, const QMYSQLDriverPrivate *p)
{
  QString msg = QString(mysql_error(p->mysqlSerial));
  printf("%s\n", msg.latin1());
  return QSqlError(QMYSQL_DRIVER_NAME ": " + err, msg, type, mysql_errno(p->mysqlSerial));
}

QVariant::Type qDecodeMYSQLType(MYSQL_FIELD *field)
{
  int mysqltype = field->type;
  

  uint flags = field->flags;
  QVariant::Type type;
    
  switch (mysqltype) {
    case FIELD_TYPE_TINY :
    case FIELD_TYPE_SHORT :
    case FIELD_TYPE_LONG :
    case FIELD_TYPE_INT24 :
      type = (flags & UNSIGNED_FLAG) ? QVariant::UInt : QVariant::Int;
      break;
    case FIELD_TYPE_YEAR :
      type = QVariant::Int;
      break;
    case FIELD_TYPE_LONGLONG :
      type = (flags & UNSIGNED_FLAG) ? QVariant::ULongLong : QVariant::LongLong;
      break;
    case FIELD_TYPE_DECIMAL :
    case FIELD_TYPE_FLOAT :
    case FIELD_TYPE_DOUBLE :
    case FIELD_TYPE_NEWDECIMAL:
      type = QVariant::Double;
      break;
    case FIELD_TYPE_DATE :
      type = QVariant::Date;
      break;
    case FIELD_TYPE_TIME :
      type = QVariant::Time;
      break;
    case FIELD_TYPE_DATETIME :
    case FIELD_TYPE_TIMESTAMP :
      type = QVariant::DateTime;
      break;
    case FIELD_TYPE_BLOB :
    case FIELD_TYPE_TINY_BLOB :
    case FIELD_TYPE_MEDIUM_BLOB :
    case FIELD_TYPE_LONG_BLOB :
      if (field->charsetnr != 63)
        type = QVariant::String;
      else
        type = (flags & BINARY_FLAG) ? QVariant::ByteArray : QVariant::CString;
      break;
    default:
    case FIELD_TYPE_ENUM :
    case FIELD_TYPE_SET :
    case FIELD_TYPE_STRING :
    case FIELD_TYPE_VAR_STRING :
      type = QVariant::String;
      break;
  }
  
  return type;
}

QMYSQLResult::QMYSQLResult(const QMYSQLDriver *db)
  : QSqlResult(db)
{
  d = new QMYSQLResultPrivate();
  d->mysql = db->d->mysql;
  d->tc = db->d->tc;
  d->idConn = db->d->idConn;
  version_ = db->version();
}

QMYSQLResult::~QMYSQLResult()
{
  cleanup();
  delete d->cachedResults;
  delete d;
}

MYSQL_RES *QMYSQLResult::result()
{
  return d->result;
}

void QMYSQLResult::cleanupCache()
{
  d->cachedResults->clear();
  if (d->flresult)
    delete d->flresult;
  d->result = 0;
  d->flresult = 0;
  d->row = NULL;
  d->currentResult = 0;
}

void QMYSQLResult::cleanup()
{
  cleanupCache();
  d->limited = false;
  d->numRows = 0;
  d->qry = QString::null;
  d->invalidate = false;
  setAt(QSql::BeforeFirst);
  setActive(false);
}

bool QMYSQLResult::nextResult(int i)
{
  int currentResult = (int)(i / LIMIT_RESULT);
  if (d->result && (currentResult == d->currentResult || !d->limited))
    return true;

  FLMYresult *flres = d->cachedResults->take(QString::number(currentResult));
  if (flres) {
    if (d->flresult)
      d->cachedResults->insert(QString::number(d->currentResult), d->flresult);
    d->result = flres->result_;
    d->flresult = flres;
    d->currentResult = currentResult;
  } else {
    int offset = currentResult * LIMIT_RESULT;
    QString q(d->qry + " LIMIT " +
              QString::number(offset) + "," + QString::number(LIMIT_RESULT));
    QCString encQuery = fromUnicode(d->tc, q);
    if (mysql_real_query(d->mysql, encQuery.data(), encQuery.length())) {
      setLastError(qMakeError(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
      printf("N1 Err: %s \n", (const char *)encQuery);
      return false;
    }
    MYSQL_RES *result = mysql_store_result(d->mysql);
    if (!result && mysql_field_count(d->mysql) > 0) {
      setLastError(qMakeError(QApplication::tr("Imposible almacenar el resultado de la consulta"), QSqlError::Statement, d));
      printf("N2 Err: %s \n", (const char *)encQuery);
      return false;
    }

    int fetchedRows = mysql_num_rows(result);
    if (fetchedRows < LIMIT_RESULT) {
      if (d->numRows == offset + fetchedRows) {
        if (currentResult == 0)
          cleanupCache();
        if (i > d->numRows) {
          mysql_free_result(result);
          return false;
        }
        if (currentResult == 0) {
          d->result = result;
          d->flresult = new FLMYresult(result);
          return true;
        }
      } else {
        qWarning("\n%s \n\n" +
                 QString("Aviso: El número de registros obtenido ( %1 ) es distinto al esperado. Reiniciando consulta.")
                 .arg(fetchedRows), (const char *)encQuery);
        qWarning("Esto sucede cuando se borran registros de un cursor mientras se recorre en un bucle ( while( cur.next() ) ... ) .\n"
                 "En estos casos casi siempre es posible y se recomienda usar setForwardOnly( true ),\n"
                 "para ejecutar más rápido la consulta y evitar reiniciarla.\n");
        resetF(QString(d->qry));
        d->invalidate = true;
        return true;
      }
    }

    if (d->flresult)
      d->cachedResults->insert(QString::number(d->currentResult), d->flresult);

    d->result = result;
    d->flresult = new FLMYresult(result);
    d->currentResult = currentResult;
  }

  return true;
}

bool QMYSQLResult::fetch(int i)
{
  if (!isActive() || !isSelect() || i >= d->numRows || d->numRows <= 0)
    return false;
  if (i < 0)
    return false;
  if (at() == i)
    return true;
  if (!d->limited)
    return fetchF(i);
  if (!nextResult(i))
    return false;
  if (d->invalidate) {
    d->invalidate = false;
    return fetchF(0);
  }
  if (d->limited)
    mysql_data_seek(d->result, i % LIMIT_RESULT);
  else
    mysql_data_seek(d->result, i);
  d->row = mysql_fetch_row(d->result);
  if (!d->row)
    return false;
  setAt(i);
  return true;
}

bool QMYSQLResult::fetchF(int i)
{
  mysql_data_seek(d->result, i);
  d->row = mysql_fetch_row(d->result);
  if (!d->row)
    return false;
  setAt(i);
  return true;
}

bool QMYSQLResult::fetchNext()
{
  if (!d->limited)
    return fetchNextF();
  return fetch(at() + 1);
}

bool QMYSQLResult::fetchNextF()
{
  d->row = mysql_fetch_row(d->result);
  if (!d->row)
    return false;
  setAt(at() + 1);
  return true;
}

bool QMYSQLResult::fetchLast()
{
  if (at() == d->numRows)
    return true;
  return fetch(d->numRows - 1);
}

bool QMYSQLResult::fetchFirst()
{
  if (at() == 0)
    return true;
  return fetch(0);
}

static inline uchar convertHex(ushort c)
{
  if (c >= '0' && c <= '9')
    return static_cast<uchar>(c - '0');
  else if (c >= 'a' && c <= 'f')
    return static_cast<uchar>(c - 'a' + 10);
  else
    return static_cast<uchar>(c - 'A' + 10);
}

static inline uchar convertHex(ushort c1, ushort c2)
{
  return ((convertHex(c2) << 4) + convertHex(c1));
}

QVariant QMYSQLResult::data(int field)
{
  if (!isSelect() || field >= (int) d->fieldTypes.count()) {
    qWarning("QMYSQLResult::data: column %d out of range", field);
    return QVariant();
  }

  uint fieldLength = 0;
  QString val;
  QVariant::Type fType = d->fieldTypes.at(field);
  fieldLength = mysql_fetch_lengths(d->result)[field];
  if (fType != QVariant::ByteArray)
    val = toUnicode(d->tc, d->row[field], fieldLength);

  switch (fType) {
    case QVariant::LongLong:
      if (val[ 0 ] == '-')
        return QVariant(val.toLongLong());
      else
        return QVariant(val.toULongLong());
    case QVariant::ULongLong:
      return QVariant(val.toULongLong());
    case QVariant::Int:
      return QVariant(val.toInt());
    case QVariant::UInt:
      return QVariant(val.toUInt());
    case QVariant::Double:
      return QVariant(val.toDouble());
    case QVariant::Date:
      if (val.isEmpty()) {
        return QVariant(QDate());
      } else {
        return QVariant(QDate::fromString(val, Qt::ISODate));
      }
    case QVariant::Time:
      if (val.isEmpty()) {
        return QVariant(QTime());
      } else {
        return QVariant(QTime::fromString(val, Qt::ISODate));
      }
    case QVariant::DateTime:
      if (val.isEmpty())
        return QVariant(QDateTime());
      if (val.length() == 14u)
        // TIMESTAMPS have the format yyyyMMddhhmmss
        val.insert(4, "-").insert(7, "-").insert(10, 'T').insert(13, ':').insert(16, ':');
      return QVariant(QDateTime::fromString(val, Qt::ISODate));
    case QVariant::ByteArray: {
      char *p = d->row[field];
      uint pos = 0;
      QByteArray ba;
      QDataStream dts(ba, IO_WriteOnly);
      while (pos < fieldLength) {
        dts << (Q_UINT8)convertHex(*p++, *p++);
        pos += 2;
      }
      return ba;
      //return ba;
      //QByteArray ba;
      //ba.duplicate(d->row[ field ], fieldLength);
      //return QVariant(ba);
    }
    default:
    case QVariant::String:
    case QVariant::CString:
      return QVariant(val);
  }
#ifdef QT_CHECK_RANGE
  qWarning("QMYSQLResult::data: unknown data type");
#endif

  return QVariant();
}

bool QMYSQLResult::isNull(int field)
{
  return d->row[field] == NULL;
}

my_ulonglong QMYSQLResult::calcSize()
{
  QString qry("SELECT FOUND_ROWS()");
  QCString encQuery = fromUnicode(d->tc, qry);
  if (mysql_real_query(d->mysql, encQuery.data(), encQuery.length())) {
    setLastError(qMakeError(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
    printf("C1 Err: %s\n", (const char *)encQuery);
    return 0;
  }
  MYSQL_RES *res = mysql_store_result(d->mysql);
  if (res) {
    MYSQL_ROW r = mysql_fetch_row(res);
    if (r) {
      qry = r[0];
      mysql_free_result(res);
      return qry.toULongLong();
    }
    mysql_free_result(res);
  } else {
    setLastError(qMakeError(QApplication::tr("Imposible almacenar el resultado de la consulta"), QSqlError::Statement, d));
    printf("C2 Err: %s\n", (const char *)encQuery);
    return 0;
  }
}

bool QMYSQLResult::reset(const QString &query)
{
  if (query.isEmpty())
    return false;
  const QSqlDriver *dr = driver();
  if (!dr || !dr->isOpen() || dr->isOpenError())
    return false;

  QString q(query.stripWhiteSpace());
  q.replace("'true'", "'1'");
  q.replace("'false'", "'0'");
  // ###
  //q.replace("=;", "= NULL;");
  while (q.endsWith(";"))
    q.truncate(q.length() - 1);
  if (q.upper().endsWith("NOWAIT"))
    q.truncate(q.length() - 6);

  QString qLimit(q);
  QString qUpper(q.upper());
  bool forUpdate = false;

  if (qUpper.left(7).contains("SELECT")) {
    if (isForwardOnly())
      return resetF(q);
    qLimit.insert(7, "SQL_CALC_FOUND_ROWS ");
    forUpdate = qUpper.endsWith("FOR UPDATE");
    if (!forUpdate && !qUpper.contains(" LIMIT "))
      qLimit += " LIMIT 0," + QString::number(LIMIT_RESULT);
  }

  cleanup();

  QCString encQuery = fromUnicode(d->tc, qLimit);
  if (mysql_real_query(d->mysql, encQuery.data(), encQuery.length())) {
    setLastError(qMakeError(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
    printf("R1 Err: %s\n", (const char *)encQuery);
    return false;
  }
  d->result = mysql_store_result(d->mysql);
  d->flresult = new FLMYresult(d->result);
  if (!d->result && mysql_field_count(d->mysql) > 0) {
    setLastError(qMakeError(QApplication::tr("Imposible almacenar el resultado de la consulta"), QSqlError::Statement, d));
    printf("R1 Err: %s\n", (const char *)encQuery);
    return false;
  }

  int numFields = mysql_field_count(d->mysql);
  setSelect(numFields != 0);
  d->fieldTypes.resize(numFields);

  if (isSelect()) {
    d->numRows = mysql_num_rows(d->result);
    d->qry = q;

    for (int i = 0; i < numFields; ++i) {
      MYSQL_FIELD *field = mysql_fetch_field_direct(d->result, i);
      d->fieldTypes[ i ] = qDecodeMYSQLType(field);
    }

    if (!forUpdate && d->numRows >= LIMIT_RESULT && !qUpper.left(7).contains("SHOW")) {
      d->numRows = calcSize();
      d->limited = true;
    }
  }

  setActive(true);
  return true;
}

bool QMYSQLResult::resetF(const QString &query)
{
  cleanup();

  QCString encQuery = fromUnicode(d->tc, query);
  if (mysql_real_query(d->mysql, encQuery.data(), encQuery.length())) {
    setLastError(qMakeError(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
    printf("F1 Err: %s\n", (const char *)encQuery);
    return false;
  }
  d->result = mysql_store_result(d->mysql);
  d->flresult = new FLMYresult(d->result);
  if (!d->result && mysql_field_count(d->mysql) > 0) {
    setLastError(qMakeError(QApplication::tr("Imposible almacenar el resultado de la consulta"), QSqlError::Statement, d));
    printf("F2 Err: %s\n", (const char *)encQuery);
    return false;
  }

  int numFields = mysql_field_count(d->mysql);
  setSelect(numFields != 0);
  d->fieldTypes.resize(numFields);

  if (isSelect()) {
    d->numRows = mysql_num_rows(d->result);
    d->qry = query;

    for (int i = 0; i < numFields; ++i) {
      MYSQL_FIELD *field = mysql_fetch_field_direct(d->result, i);
      d->fieldTypes[ i ] = qDecodeMYSQLType(field);
    }
  }

  setActive(true);
  return true;
}

int QMYSQLResult::size()
{
  return isSelect() ? (int) d->numRows : -1;
}

int QMYSQLResult::numRowsAffected()
{
  return (int) mysql_affected_rows(d->mysql);
}

/////////////////////////////////////////////////////////

static int qMySqlConnectionCount = 0;
static bool qMySqlInitHandledByUser = false;

static void qLibraryInit()
{
#ifndef Q_NO_MYSQL_EMBEDDED
# if (MYSQL_VERSION_ID >= 40000)
  if (qMySqlInitHandledByUser || qMySqlConnectionCount > 1)
    return;

# if (MYSQL_VERSION_ID >= 40110 && MYSQL_VERSION_ID < 50000) || (MYSQL_VERSION_ID >= 50003)
  if (mysql_library_init(0, 0, 0))
# else
  if (mysql_server_init(0, 0, 0))
# endif
    qWarning("QMYSQLDriver::qServerInit: unable to start server.");
# endif // MYSQL_VERSION_ID
#endif // Q_NO_MYSQL_EMBEDDED
}

static void qLibraryEnd()
{
#ifndef Q_NO_MYSQL_EMBEDDED
# if (MYSQL_VERSION_ID > 40000)
#  if (MYSQL_VERSION_ID >= 40110 && MYSQL_VERSION_ID < 50000) || (MYSQL_VERSION_ID >= 50003)
  mysql_library_end();
#  else
  mysql_server_end();
#  endif
# endif
#endif
}

static bool setTransactionReadCommited(MYSQL *mysql)
{
  if (mysql_query(mysql, "SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED"))
    return false;
  return true;
}

QMYSQLDriver::QMYSQLDriver(QObject *parent, const char *name)
  : FLSqlDriver(parent, name ? name : QMYSQL_DRIVER_NAME)
{
  init();
  qLibraryInit();
}

/*!
    Create a driver instance with an already open connection handle.
*/

QMYSQLDriver::QMYSQLDriver(MYSQL *con, QObject *parent, const char *name)
  : FLSqlDriver(parent, name ? name : QMYSQL_DRIVER_NAME)
{
  init();
  if (con) {
    d->mysql = (MYSQL *) con;
    d->tc = codec(con);
    setOpen(true);
    setOpenError(false);
    if (qMySqlConnectionCount == 1)
      qMySqlInitHandledByUser = true;
  } else {
    qLibraryInit();
  }
}

void QMYSQLDriver::init()
{
  qSqlOpenExtDict()->insert(this, new QMYSQLOpenExtension(this));
  d = new QMYSQLDriverPrivate();
  cInfo = new QMYSQLCacheInfoPrivate();
  d->noInnoDB = (QString(QObject::name()) == "FLQMYSQL4_NO_INNODB");
  d->mysql = 0;
  qMySqlConnectionCount++;
}

QMYSQLDriver::~QMYSQLDriver()
{
  qMySqlConnectionCount--;
  if (qMySqlConnectionCount == 0 && !qMySqlInitHandledByUser)
    qLibraryEnd();
  delete d;
  delete cInfo;
  if (!qSqlOpenExtDict()->isEmpty()) {
    QSqlOpenExtension *ext = qSqlOpenExtDict()->take(this);
    delete ext;
  }
}

bool QMYSQLDriver::hasFeature(DriverFeature f) const
{
  switch (f) {
    case Transactions:
      if (d->noInnoDB)
        return false;
      // CLIENT_TRANSACTION should be defined in all recent mysql client libs > 3.23.34
#ifdef CLIENT_TRANSACTIONS
      if (d->mysql) {
        if ((d->mysql->server_capabilities & CLIENT_TRANSACTIONS) == CLIENT_TRANSACTIONS)
          return true;
      }
#endif
      return false;
    case QuerySize:
      return true;
    case BLOB:
      return true;
    case Unicode:
      return true;
    default:
      return false;
  }
}

bool QMYSQLDriver::open(const QString &,
                        const QString &,
                        const QString &,
                        const QString &,
                        int)
{
  qWarning("QMYSQLDriver::open(): This version of open() is no longer supported.");
  return false;
}

static inline unsigned int parseOptionFlags(const QString &connOpts)
{
  unsigned int optionFlags = 0;

  QStringList raw = QStringList::split(';', connOpts);
  QStringList opts;
  QStringList::ConstIterator it;

  // extract the real options from the string
  for (it = raw.begin(); it != raw.end(); ++it) {
    QString tmp(*it);
    int idx;
    if ((idx = tmp.find('=')) != -1) {
      QString val(tmp.mid(idx + 1));
      val.simplifyWhiteSpace();
      if (val == "true" || val == "1")
        opts << tmp.left(idx);
      else
        qWarning("QMYSQLDriver::open: Illegal connect option value '%s'", tmp.latin1());
    } else {
      opts << tmp;
    }
  }

  for (it = opts.begin(); it != opts.end(); ++it) {
    QString opt((*it).upper());
    if (opt == "CLIENT_COMPRESS")
      optionFlags |= CLIENT_COMPRESS;
    else if (opt == "CLIENT_FOUND_ROWS")
      optionFlags |= CLIENT_FOUND_ROWS;
    else if (opt == "CLIENT_IGNORE_SPACE")
      optionFlags |= CLIENT_IGNORE_SPACE;
    else if (opt == "CLIENT_INTERACTIVE")
      optionFlags |= CLIENT_INTERACTIVE;
    else if (opt == "CLIENT_NO_SCHEMA")
      optionFlags |= CLIENT_NO_SCHEMA;
    else if (opt == "CLIENT_ODBC")
      optionFlags |= CLIENT_ODBC;
    else if (opt == "CLIENT_SSL")
      optionFlags |= CLIENT_SSL;
    else
      qWarning("QMYSQLDriver::open: Unknown connect option '%s'", (*it).latin1());
  }

  return optionFlags;
}

bool QMYSQLDriver::open(const QString &db,
                        const QString &user,
                        const QString &password,
                        const QString &host,
                        int port,
                        const QString &connOpts)
{
  if (isOpen())
    close();

  unsigned int optionFlags = parseOptionFlags(connOpts);

  if (!(d->mysql = mysql_init((MYSQL *) 0))) {
    setLastError(qMakeError(QApplication::tr("No se puede conectar a la base de datos"),
                            QSqlError::Connection, d));
    mysql_close(d->mysql);
    setOpenError(true);
    return false;
  }
  uint protocol = MYSQL_PROTOCOL_TCP;
  mysql_options(d->mysql, MYSQL_OPT_PROTOCOL, (const char *) & protocol);
  if (mysql_real_connect(d->mysql, host, user, password, db.isNull() ? QString("") : db, (port > -1) ? port : 0, NULL, optionFlags)) {
    if (!db.isEmpty() && mysql_select_db(d->mysql, db)) {
      setLastError(qMakeError(QApplication::tr("No se puede abrir la base de datos") + " '" + db + "'", QSqlError::Connection, d));
      mysql_close(d->mysql);
      setOpenError(true);
      return false;
    }
  } else {
    setLastError(qMakeError(QApplication::tr("No se puede conectar a la base de datos"), QSqlError::Connection, d));
    mysql_close(d->mysql);
    setOpenError(true);
    return false;
  }

  if (!(d->mysqlSerial = mysql_init((MYSQL *) 0))) {
    setLastError(qMakeErrorSerial(QApplication::tr("No se puede conectar a la base de datos"), QSqlError::Connection, d));
    mysql_close(d->mysqlSerial);
    mysql_close(d->mysql);
    setOpenError(true);
    return false;
  }
  mysql_options(d->mysqlSerial, MYSQL_OPT_PROTOCOL, (const char *) & protocol);
  if (mysql_real_connect(d->mysqlSerial, host, user, password, db.isNull() ? QString("") : db, (port > -1) ? port : 0, NULL, optionFlags)) {
    if (!db.isEmpty() && mysql_select_db(d->mysqlSerial, db)) {
      setLastError(qMakeErrorSerial(QApplication::tr("No se puede abrir la base de datos") + " '" + db + "'", QSqlError::Connection, d));
      mysql_close(d->mysqlSerial);
      mysql_close(d->mysql);
      setOpenError(true);
      return false;
    }
  } else {
    setLastError(qMakeErrorSerial(QApplication::tr("No se puede conectar a la base de datos"), QSqlError::Connection, d));
    mysql_close(d->mysqlSerial);
    mysql_close(d->mysql);
    setOpenError(true);
    return false;
  }

  d->idConn = mysql_thread_id(d->mysql);

#if (MYSQL_VERSION_ID >= 40113 && MYSQL_VERSION_ID < 50000) || MYSQL_VERSION_ID >= 50007
  mysql_set_character_set(d->mysql, "utf8");
  mysql_set_character_set(d->mysqlSerial, "utf8");
#endif
  d->tc = codec(d->mysql);

  mysql_thread_init();

  setOpen(true);
  setOpenError(false);
  return true;
}

void QMYSQLDriver::close()
{
  if (isOpen()) {
    mysql_close(d->mysql);
#ifndef FL_QUICK_CLIENT
    if (dictIndexes) {
      delete dictIndexes;
      dictIndexes = 0;
    }
#endif
    setOpen(false);
    setOpenError(false);
  }
}

QSqlQuery QMYSQLDriver::createQuery() const
{
  return QSqlQuery(new QMYSQLResult(this));
}

QStringList QMYSQLDriver::tables(const QString &typeName) const
{
  QStringList tl;
  if (!isOpen())
    return tl;
  if (!typeName.isEmpty() && !(typeName.toInt() & (int) QSql::Tables))
    return tl;

  MYSQL_RES *tableRes = mysql_list_tables(d->mysql, NULL);
  MYSQL_ROW row;
  int i = 0;
  while (tableRes && true) {
    mysql_data_seek(tableRes, i);
    row = mysql_fetch_row(tableRes);
    if (!row)
      break;
    tl.append(toUnicode(d->tc, row[0]));
    i++;
  }
  mysql_free_result(tableRes);
  return tl;
}

QSqlIndex QMYSQLDriver::primaryIndex2(const QString &tablename) const
{
  QSqlIndex idx;
  if (!isOpen())
    return idx;
  QSqlQuery i = createQuery();
  i.setForwardOnly(true);
  QString stmt("show index from %1;");
  QSqlRecord fil = record(tablename);
  i.exec(stmt.arg(tablename));
  while (i.isActive() && i.next()) {
    if (i.value(2).toString() == "PRIMARY") {
      idx.append(*fil.field(i.value(4).toString()));
      idx.setCursorName(i.value(0).toString());
      idx.setName(i.value(2).toString());
    }
  }
  return idx;
}

QSqlIndex QMYSQLDriver::primaryIndex(const QString &tablename) const
{
  FLManager *mng = db_->manager();
  if (mng->initCount() == cInfo->mngInitCount) {
    QMap<QString, QSqlIndex>::const_iterator it(cInfo->cacheIdx.find(tablename));
    if (it != cInfo->cacheIdx.end())
      return *it;
  } else {
    cInfo->mngInitCount = mng->initCount();
    cInfo->clear();
  }

  QSqlIndex idx(tablename);
  if (!isOpen())
    return idx;
  QDomDocument doc(tablename);
  QDomElement docElem;
  QString stream = db_->managerModules()->contentCached(tablename + ".mtd");
  if (!FLUtil::domDocumentSetContent(doc, stream)) {
#ifdef FL_DEBUG
    qWarning("FLManager : " + QApplication::tr("Error al cargar los metadatos para la tabla %1").arg(tablename));
#endif

    return primaryIndex2(tablename);
  }
  docElem = doc.documentElement();
  FLTableMetaData *mtd = mng->metadata(&docElem, true);
  if (!mtd)
    return primaryIndex2(tablename);
  idx.append(QSqlField(mtd->primaryKey(), FLFieldMetaData::flDecodeType(mtd->fieldType(mtd->primaryKey()))));
  idx.setName(tablename.lower() + "_pkey");

  delete mtd;
  cInfo->cacheIdx.replace(tablename, idx);
  return idx;
}

QSqlRecord QMYSQLDriver::record2(const QString &tablename) const
{
  QSqlRecord fil;
  if (!isOpen())
    return fil;
  MYSQL_RES *r = mysql_list_fields(d->mysql, tablename.local8Bit().data(), 0);
  if (!r) {
    return fil;
  }
  MYSQL_FIELD *field;
  while ((field = mysql_fetch_field(r))) {
    QSqlField f(toUnicode(d->tc, field->name) , qDecodeMYSQLType(field));
    fil.append(f);
  }
  mysql_free_result(r);
  return fil;
}

QSqlRecord QMYSQLDriver::record(const FLTableMetaData *mtd) const
{
  QSqlRecord fil;
  const FLTableMetaData::FLFieldMetaDataList *fl = mtd->fieldList();

  if (!fl || fl->isEmpty())
    return record2(mtd->name());

  QStringList fieldsNames = QStringList::split(",", mtd->fieldsNames());
  for (QStringList::Iterator it = fieldsNames.begin(); it != fieldsNames.end(); ++it)
    fil.append(QSqlField((*it), FLFieldMetaData::flDecodeType(mtd->fieldType((*it)))));

  return fil;
}

QSqlRecord QMYSQLDriver::record(const QString &tablename) const
{
  QSqlRecord fil;
  if (!isOpen())
    return fil;
  QDomDocument doc(tablename);
  QDomElement docElem;
  QString stream = db_->managerModules()->contentCached(tablename + ".mtd");
  if (!FLUtil::domDocumentSetContent(doc, stream)) {
#ifdef FL_DEBUG
    qWarning("FLManager : " + QApplication::tr("Error al cargar los metadatos para la tabla %1").arg(tablename));
#endif
    return record2(tablename);
  }
  docElem = doc.documentElement();
  FLTableMetaData *mtd = db_->manager()->metadata(&docElem, true);
  if (!mtd)
    return record2(tablename);

  const FLTableMetaData::FLFieldMetaDataList *fl = mtd->fieldList();
  if (!fl) {
    delete mtd;
    return record2(tablename);
  }

  if (fl->isEmpty()) {
    delete mtd;
    return record2(tablename);
  }

  QStringList fieldsNames = QStringList::split(",", mtd->fieldsNames());
  for (QStringList::Iterator it = fieldsNames.begin(); it != fieldsNames.end(); ++it)
    fil.append(QSqlField((*it), FLFieldMetaData::flDecodeType(mtd->fieldType((*it)))));

  delete mtd;
  return fil;
}

QSqlRecord QMYSQLDriver::record(const QSqlQuery &query) const
{
  QSqlRecord fil;
  if (!isOpen())
    return fil;
  if (query.isActive() && query.isSelect() && query.driver() == this) {
    QMYSQLResult *result = (QMYSQLResult *) query.result();
    QMYSQLResultPrivate *p = result->d;
    if (!mysql_errno(p->mysql)) {
      for (;;) {
        MYSQL_FIELD *f = mysql_fetch_field(p->result);
        if (f) {
          QSqlField fi(toUnicode(d->tc, f->name), qDecodeMYSQLType(f));
          fil.append(fi);
        } else
          break;
      }
    }
    mysql_field_seek(p->result, 0);
  }
  return fil;
}

QSqlRecordInfo QMYSQLDriver::recordInfo2(const QString &tablename) const
{
  QSqlRecordInfo info;
  if (!isOpen())
    return info;
  MYSQL_RES *r = mysql_list_fields(d->mysql, tablename.local8Bit().data(), 0);
  if (!r) {
    return info;
  }
  MYSQL_FIELD *field;
  while ((field = mysql_fetch_field(r))) {
    info.append(QSqlFieldInfo(toUnicode(d->tc, field->name),
                              qDecodeMYSQLType(field),
                              IS_NOT_NULL(field->flags),
                              (int) field->length,
                              (int) field->decimals,
                              QString(field->def),
                              (int) field->type));
  }
  mysql_free_result(r);
  return info;
}

QSqlRecordInfo QMYSQLDriver::recordInfo(const QString &tablename) const
{
  FLManager *mng = db_->manager();
  if (mng->initCount() == cInfo->mngInitCount) {
    QMap<QString, QSqlRecordInfo>::const_iterator it(cInfo->cache.find(tablename));
    if (it != cInfo->cache.end())
      return *it;
  } else {
    cInfo->clear();
    cInfo->mngInitCount = mng->initCount();
  }

  QSqlRecordInfo info;
  if (!isOpen())
    return info;
  QDomDocument doc(tablename);
  QDomElement docElem;
  QString stream = db_->managerModules()->contentCached(tablename + ".mtd");
  if (!FLUtil::domDocumentSetContent(doc, stream)) {
#ifdef FL_DEBUG
    qWarning("FLManager : " + QApplication::tr("Error al cargar los metadatos para la tabla %1").arg(tablename));
#endif

    return recordInfo2(tablename);
  }
  docElem = doc.documentElement();
  FLTableMetaData *mtd = mng->metadata(&docElem, true);
  if (!mtd)
    return recordInfo2(tablename);
  const FLTableMetaData::FLFieldMetaDataList *fl = mtd->fieldList();
  if (!fl) {
    delete mtd;
    return recordInfo2(tablename);
  }
  if (fl->isEmpty()) {
    delete mtd;
    return recordInfo2(tablename);
  }

  QStringList fieldsNames = QStringList::split(",", mtd->fieldsNames());
  for (QStringList::Iterator it = fieldsNames.begin(); it != fieldsNames.end(); ++it) {
    FLFieldMetaData *field = mtd->field((*it));
    info.append(QSqlFieldInfo(field->name(),
                              FLFieldMetaData::flDecodeType(field->type()),
                              !field->allowNull(),
                              field->length(), field->partDecimal(), field->defaultValue()));
#ifndef FL_QUICK_CLIENT
    if (field->relationM1() || field->isPrimaryKey() || field->isCompoundKey())
      createIndex(field->name(), tablename);
    else if (field->type() == QVariant::Date)
      createIndex(field->name(), tablename);
#endif
  }

  delete mtd;
  cInfo->cache.replace(tablename, info);
  return info;
}

QSqlRecordInfo QMYSQLDriver::recordInfo(const QSqlQuery &query) const
{
  QSqlRecordInfo info;
  if (!isOpen())
    return info;
  if (query.isActive() && query.isSelect() && query.driver() == this) {
    QMYSQLResult *result = (QMYSQLResult *) query.result();
    QMYSQLResultPrivate *p = result->d;
    if (!mysql_errno(p->mysql)) {
      for (;;) {
        MYSQL_FIELD *field = mysql_fetch_field(p->result);
        if (field) {
          QString name(toUnicode(d->tc, field->name));
          // ###
          if (!info.find(name).name().isEmpty()) {
            QString tableName(toUnicode(d->tc, field->table));
            if (!tableName.isEmpty())
              name.prepend(tableName + QString::fromLatin1("."));
          }
          // ###
          info.append(QSqlFieldInfo(name,
                                    qDecodeMYSQLType(field),
                                    IS_NOT_NULL(field->flags),
                                    (int) field->length,
                                    (int) field->decimals,
                                    QVariant(),
                                    (int) field->type));

        } else
          break;
      }
    }
    mysql_field_seek(p->result, 0);
  }
  return info;
}

MYSQL *QMYSQLDriver::mysql()
{
  return d->mysql;
}

bool QMYSQLDriver::beginTransaction()
{
#ifndef CLIENT_TRANSACTIONS
  return false;
#endif
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QMYSQLDriver::beginTransaction: Database not open");
#endif
    return false;
  }
  if (d->noInnoDB)
    return true;
  if (mysql_query(d->mysql, "BEGIN WORK")) {
    setLastError(qMakeError(QApplication::tr("No se puede iniciar transacción"), QSqlError::Statement, d));
    return false;
  }
  return true;
}

bool QMYSQLDriver::commitTransaction()
{
#ifndef CLIENT_TRANSACTIONS
  return false;
#endif
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QMYSQLDriver::commitTransaction: Database not open");
#endif
    return false;
  }
  if (d->noInnoDB)
    return true;
  if (mysql_query(d->mysql, "COMMIT")) {
    setLastError(qMakeError(QApplication::tr("No se puede aceptar la transacción"), QSqlError::Statement, d));
    return false;
  }
  return true;
}

bool QMYSQLDriver::rollbackTransaction()
{
#ifndef CLIENT_TRANSACTIONS
  return false;
#endif
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QMYSQLDriver::rollbackTransaction: Database not open");
#endif
    return false;
  }
  if (d->noInnoDB)
    return true;
  if (mysql_query(d->mysql, "ROLLBACK")) {
    setLastError(qMakeError(QApplication::tr("No se puede deshacer la transacción"), QSqlError::Statement, d));
    return false;
  }
  return true;
}

QString QMYSQLDriver::formatValue(const QSqlField *field, bool trimStrings) const
{
  QString r;
  if (field->isNull()) {
    r = nullText();
  } else {
    switch (field->type()) {
        //case QVariant::ByteArray: {
        //  const QByteArray ba = field->value().toByteArray();
        //  // buffer has to be at least length*2+1 bytes
        //  char *buffer = new char[ ba.size() * 2 + 1 ];
        //  uint escapedSize = mysql_real_escape_string(d->mysql, buffer,  ba.data(),  ba.size());
        //  r.reserve(escapedSize + 3);
        //  r.append('\'').append(toUnicode(d->tc, buffer).replace("'", "\\'")).append('\'');
        //  delete[] buffer;
        //}
        //break;
      case QVariant::String:
      case QVariant::CString: {
        // Escape '\' characters
        r = QSqlDriver::formatValue(field);
        r.replace("\\", "\\\\");
        break;
      }
      default:
        r = QSqlDriver::formatValue(field, trimStrings);
    }
  }
  return r;
}

bool QMYSQLDriver::existsDatabase(const QString &db, const QString &user, const QString &password,
                                  const QString &host, int port)
{
  MYSQL *conn;
  if (!(conn = mysql_init((MYSQL *) 0))) {
    setLastError(QSqlError(QMYSQL_DRIVER_NAME ": Unable to connect", QString(mysql_error(conn)), QSqlError::Connection, mysql_errno(conn)));
    mysql_close(conn);
    setOpenError(true);
    return false;
  }
  uint protocol = MYSQL_PROTOCOL_TCP;
  mysql_options(conn, MYSQL_OPT_PROTOCOL, (const char *) & protocol);
  if (mysql_real_connect(conn, host, user, password, QString(""), port,
                         NULL, parseOptionFlags(db_->connectOptions()))) {
    MYSQL_RES *dbs = mysql_list_dbs(conn, NULL);
    MYSQL_ROW row;
    int i = 0, found = false;
    while (dbs) {
      mysql_data_seek(dbs, i);
      row = mysql_fetch_row(dbs);
      if (!row)
        break;
      QString dbCur(row[ 0 ]);
      if (dbCur == db) {
        found = true;
        break;
      }
      i++;
    }
    mysql_free_result(dbs);
    mysql_close(conn);
    setOpenError(false);
    return found;
  } else {
    setLastError(QSqlError(QMYSQL_DRIVER_NAME ": Unable to connect", QString(mysql_error(conn)), QSqlError::Connection, mysql_errno(conn)));
    mysql_close(conn);
    setOpenError(true);
    return false;
  }
}

bool QMYSQLDriver::tryConnect(const QString &db, const QString &user, const QString &password,
                              const QString &host, int port)
{
  bool newDatabase = !existsDatabase(db, user, password, host, port);
  if (newDatabase) {
    if (isOpenError()) {
      msgBoxCritical(tr("Conexión fallida"), tr("No se pudo conectar al servidor."));
      msgBoxCritical(tr("Error"), QString(lastError().driverText().utf8()) + "\n" +
                     QString(lastError().databaseText().utf8()));
      return false;
    }
    if (db_->interactiveGUI()) {
      int res = QMessageBox::question(0, tr("Crear base de datos"),
                                      tr("La base de datos %1 no existe. ¿Quiere crearla?").arg(db),
                                      QMessageBox::Yes, QMessageBox::No);
      if (res == QMessageBox::Yes) {
        MYSQL *conn;
        if (!(conn = mysql_init((MYSQL *) 0))) {
          setLastError(QSqlError(QMYSQL_DRIVER_NAME ": Unable to connect", QString(mysql_error(conn)),
                                 QSqlError::Connection, mysql_errno(conn)));
          mysql_close(conn);
          setOpenError(true);
          return false;
        }
        uint protocol = MYSQL_PROTOCOL_TCP;
        mysql_options(conn, MYSQL_OPT_PROTOCOL, (const char *) & protocol);
        if (mysql_real_connect(conn, host, user, password, QString(""), port,
                               NULL, parseOptionFlags(db_->connectOptions()))) {
          if (mysql_query(conn, "CREATE DATABASE IF NOT EXISTS " + db +
                          " DEFAULT CHARACTER SET = utf8  COLLATE = utf8_bin")) {
            setLastError(QSqlError(QMYSQL_DRIVER_NAME ": Unable create database", QString(mysql_error(conn)),
                                   QSqlError::Statement, mysql_errno(conn)));
            mysql_close(conn);
            QMessageBox::critical(0, tr("Error"), QString(lastError().driverText().utf8()) + "\n" +
                                  QString(lastError().databaseText().utf8()), QMessageBox::Ok, 0, 0);
            return false;
          }
          mysql_close(conn);
          setOpenError(false);
        } else {
          setLastError(QSqlError(QMYSQL_DRIVER_NAME ": Unable to connect", QString(mysql_error(conn)),
                                 QSqlError::Connection, mysql_errno(conn)));
          mysql_close(conn);
          setOpenError(true);
          QMessageBox::critical(0, tr("Error"), QString(lastError().driverText().utf8()) + "\n" +
                                QString(lastError().databaseText().utf8()), QMessageBox::Ok, 0, 0);
          return false;
        }
      }
    }
  }

  QString connOpts("CLIENT_COMPRESS");
  if (!db_->connectOptions().isEmpty())
    connOpts.append(';' + db_->connectOptions());
  if (!open(db, user, password, host, port, connOpts)) {
    msgBoxCritical(tr("Conexión fallida"), tr("No se pudo conectar con la base de datos %1.").arg(db));
    msgBoxCritical(tr("Error"), QString(lastError().driverText().utf8()) + "\n" +
                   QString(lastError().databaseText().utf8()));
    close();
    setOpenError(true);
    return false;
  }

  QSqlQuery q(createQuery());

  if (!d->noInnoDB) {
    setTransactionReadCommited(d->mysql);

    q.setForwardOnly(true);
    q.exec("SHOW VARIABLES LIKE 'have_innodb'");
    q.next();
    if (q.value(1).toString().upper() != "YES") {
      q.exec("SHOW VARIABLES LIKE 'version'");
      q.next();
      msgBoxCritical(tr("Conexión fallida"),
                     tr("La versión MySQL %1 de la base de datos a la que se\nintenta conectar no soporta tablas del tipo InnoDB.").
                     arg(q.value(1).toString()));
      close();
      setOpenError(true);
      return false;
    }
  }

  d->forceChapuza = tables("").contains("forzarchapuzasqlnoformal");

  q.exec("SHOW VARIABLES LIKE 'version'");
  q.next();
  QString val = q.value(1).toString();
  QRegExp rx("(\\d+)\\.(\\d+)");
  rx.setMinimal(true);
  if (rx.search(val) != -1) {
    int vMaj = rx.cap(1).toInt();
    int vMin = rx.cap(2).toInt();
    if (vMaj < 4) {
      msgBoxCritical(tr("Conexión fallida"),
                     tr("La versión MySQL %1 no está soportada por este controlador.\n\n"
                        "La versiones soportada son MySQL mayor o igual a 4.x.").arg(val));
      close();
      setOpenError(true);
      return false;
    } else if (vMaj >= 5) {
      version_ = QMYSQLDriver::Version5;
    } else
      version_ = QMYSQLDriver::Version4;
  }

  close();
  setOpenError(false);
  setLastError(QSqlError());
  return true;
}

QString QMYSQLDriver::sqlCreateTable(const FLTableMetaData *tmd)
{
#ifndef FL_QUICK_CLIENT
  if (!tmd)
    return QString::null;

  QString primaryKey(QString::null);
  QString sql = "CREATE TABLE " + tmd->name() + " (";

  FLFieldMetaData *field;
  const FLTableMetaData::FLFieldMetaDataList *fieldList = tmd->fieldList();

  unsigned int unlocks = 0;
  QDictIterator<FLFieldMetaData> it(*fieldList);
  while ((field = it.current()) != 0) {
    ++it;
    if (field->type() == FLFieldMetaData::Unlock)
      unlocks++;
  }
  if (unlocks > 1) {
#ifdef FL_DEBUG
    qWarning("QMYSQLDriver : " + QApplication::tr("No se ha podido crear la tabla ") + tmd->name());
    qWarning("QMYSQLDriver : " + QApplication::tr("Hay más de un campo tipo unlock. Solo puede haber uno."));
#endif

    return QString::null;
  }

  QDictIterator<FLFieldMetaData> it2(*fieldList);
  while ((field = it2.current()) != 0) {
    ++it2;
    sql += field->name();
    switch (field->type()) {
      case QVariant::Int:
        sql += " INT";
        break;

      case FLFieldMetaData::Serial:
      case QVariant::UInt:
        sql += " INT UNSIGNED";
        break;

      case QVariant::Bool:
      case FLFieldMetaData::Unlock:
        sql += " BOOL";
        break;

      case QVariant::Double:
        sql += " DECIMAL(" +
               QString::number(field->partInteger() + field->partDecimal() + 5) + "," +
               QString::number(field->partDecimal() + 5) + ")";

        break;

      case QVariant::Time:
        sql += " TIME";
        break;

      case QVariant::Date:
        sql += " DATE";
        break;

      case QVariant::String:
        if (field->length() > 0) {
          if (field->length() > 255)
            sql += " VARCHAR";
          else
            sql += " CHAR";
          sql += "(" + QString::number(field->length()) + ")";
        } else
          sql += " CHAR(255)";
        break;

      case QVariant::Pixmap:
      case QVariant::StringList:
        sql += " MEDIUMTEXT";
        break;

      case QVariant::ByteArray:
        sql += " LONGBLOB";
        break;
    }

    if (field->isPrimaryKey()) {
      if (primaryKey.isEmpty()) {
        sql += " PRIMARY KEY";
        primaryKey = field->name();
      } else {
#ifdef FL_DEBUG
        qWarning(QApplication::tr("FLManager : Tabla-> " + tmd->name() +
                                  ". Se ha intentado poner una segunda clave primaria para el campo %1, pero el campo %2 ya es clave primaria."
                                  "Sólo puede existir una clave primaria en FLTableMetaData, use FLCompoundKey para crear claves compuestas.").
                 arg(field->name(), primaryKey));
#endif
        return QString::null;
      }
    } else {
      if (field->isUnique())
        sql += " UNIQUE";
      if (!field->allowNull())
        sql += " NOT NULL";
      else
        sql += " NULL";
    }

    if (it2.current())
      sql += ",";
  }

  QString engine(d->noInnoDB ? ") ENGINE=MyISAM" : ") ENGINE=INNODB");
  sql += engine;
  if (!d->forceChapuza)
    sql += " DEFAULT CHARACTER SET = utf8  COLLATE = utf8_bin";
  else
    sql += " DEFAULT CHARACTER SET = utf8";

  qWarning("NOTICE: CREATE TABLE (" + tmd->name() + engine);

  return sql;

#endif //FL_QUICK_CLIENT

  return QString::null;
}

QString QMYSQLDriver::formatValueLike(int t, const QVariant &v, const bool upper)
{
  QString res("IS NULL");

  switch (t) {
    case QVariant::Bool: {
      QString s(v.toString().left(1).upper());
      if (s == QApplication::tr("Sí").left(1).upper())
        res = "=1";
      else if (s == QApplication::tr("No").left(1).upper())
        res = "=0";
    }
    break;
    case QVariant::Date:
      res = "LIKE '%%" + FLUtil::dateDMAtoAMD(v.toString()) + "'";
      break;
    case QVariant::Time: {
      QTime t(v.toTime());
      if (t.isValid() && !t.isNull())
        res = "LIKE '" + t.toString(Qt::ISODate) + "%%'";
    }
    break;
    default: {
      res = v.toString();
      res.replace(QChar('\''), "''");
      res.replace("\\", "\\\\");
      if (upper)
        res = "LIKE '" + res.upper() + "%%'";
      else
        res = "LIKE '" + res + "%%'";
      break;
    }
  }

  return res;
}

QString QMYSQLDriver::formatValue(int t, const QVariant &v, const bool upper)
{
  QString res;

  switch (FLFieldMetaData::flDecodeType(t)) {
    case QVariant::Bool: {
      QString s(v.toString().left(1).upper());
      if (s == QApplication::tr("Sí").left(1).upper())
        res = "0";
      else if (s == QApplication::tr("No").left(1).upper())
        res = "1";
      else
        res = nullText();
    }
    break;
    case QVariant::Date:
      res = "'" + FLUtil::dateDMAtoAMD(v.toString()) + "'";
      break;
    case QVariant::Time: {
      QTime t(v.toTime());
      if (t.isValid() && !t.isNull())
        res = "'" + t.toString(Qt::ISODate) + "'";
      else
        res = nullText();
    }
    break;
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::Double:
      res = v.toString();
      break;
    default: {
      res = v.toString();
      res.replace(QChar('\''), "''");
      res.replace("\\", "\\\\");
      if (upper)
        res = "'" + res.upper() + "'";
      else
        res = "'" + res + "'";
      break;
    }
  }

  return res;
}

QVariant QMYSQLDriver::nextSerialVal(const QString &table, const QString &field)
{
#ifndef CLIENT_TRANSACTIONS
  return QVariant();
#endif
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QMYSQLDriver::beginTransaction: Database not open");
#endif
    return QVariant();
  }
  if (!d->noInnoDB && mysql_query(d->mysqlSerial, "BEGIN WORK")) {
    setLastError(qMakeErrorSerial(QApplication::tr("No se puede iniciar transacción"), QSqlError::Statement, d));
    return QVariant();
  }

  MYSQL_RES *res;
  MYSQL_ROW row;
  QString strQry;
  unsigned int max = 0;
  unsigned int curMax = 0;
  bool updateQry = false;

  strQry = "SELECT MAX(" + field + ") FROM " + table;
  const char *encQuery = strQry.ascii();
  if (mysql_real_query(d->mysqlSerial, encQuery, qstrlen(encQuery))) {
    setLastError(qMakeErrorSerial(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
    if (!d->noInnoDB)
      mysql_query(d->mysqlSerial, "ROLLBACK");
    return QVariant();
  }
  res = mysql_store_result(d->mysqlSerial);
  if (res) {
    row = mysql_fetch_row(res);
    if (row)
      max = QString(row[0]).toUInt();
    mysql_free_result(res);
  }

  strQry = "SELECT seq FROM flseqs WHERE tabla = '" + table + "' AND campo = '" + field + "'";
  encQuery = strQry.ascii();
  if (mysql_real_query(d->mysqlSerial, encQuery, qstrlen(encQuery))) {
    setLastError(qMakeErrorSerial(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
    if (!d->noInnoDB)
      mysql_query(d->mysqlSerial, "ROLLBACK");
    return QVariant();
  }
  res = mysql_store_result(d->mysqlSerial);
  if (res) {
    updateQry = (mysql_num_rows(res) > 0);
    if (updateQry) {
      row = mysql_fetch_row(res);
      if (row)
        curMax = QString(row[0]).toUInt();
    }
    mysql_free_result(res);
  }

  strQry = "";
  if (updateQry) {
    if (max > curMax)
      strQry = "UPDATE flseqs SET seq=" + QString::number(max + 1) + " WHERE tabla = '" + table + "' AND campo = '" + field + "'";
  } else
    strQry = "INSERT INTO flseqs (tabla,campo,seq) VALUES('" + table + "','" + field + "'," +  QString::number(max + 1) + ")";

  if (!strQry.isEmpty()) {
    encQuery = strQry.ascii();
    if (mysql_real_query(d->mysqlSerial, encQuery, qstrlen(encQuery))) {
      setLastError(qMakeErrorSerial(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
      if (!d->noInnoDB)
        mysql_query(d->mysqlSerial, "ROLLBACK");
      return QVariant();
    }
  }

  strQry = "UPDATE flseqs SET seq=LAST_INSERT_ID(seq+1) WHERE tabla = '" + table + "' AND campo = '" + field + "'";
  encQuery = strQry.ascii();
  if (mysql_real_query(d->mysqlSerial, encQuery, qstrlen(encQuery))) {
    setLastError(qMakeErrorSerial(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
    if (!d->noInnoDB)
      mysql_query(d->mysqlSerial, "ROLLBACK");
    return QVariant();
  }

  strQry = "SELECT LAST_INSERT_ID()";
  encQuery = strQry.ascii();
  if (mysql_real_query(d->mysqlSerial, encQuery, qstrlen(encQuery))) {
    setLastError(qMakeErrorSerial(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
    if (!d->noInnoDB)
      mysql_query(d->mysqlSerial, "ROLLBACK");
    return QVariant();
  }
  QVariant ret;
  res = mysql_store_result(d->mysqlSerial);
  if (res) {
    row = mysql_fetch_row(res);
    if (row)
      ret = QString(row[0]).toUInt();
    mysql_free_result(res);
  }

  if (!d->noInnoDB && mysql_query(d->mysqlSerial, "COMMIT")) {
    setLastError(qMakeErrorSerial(QApplication::tr("No se puede aceptar la transacción"), QSqlError::Statement, d));
    return QVariant();
  }

  return ret;
}

int QMYSQLDriver::atFrom(FLSqlCursor *cur)
{
#if 0
  //#ifndef FL_QUICK_CLIENT
  if (cur && cur->metadata() && !cur->metadata()->isQuery() && cur->sort().count() > 1) {
    d->activeCreateIndex = true;
    createIndex(cur->sort().toStringList().join(",").replace(" ASC", "").replace(" DESC", ""), cur->metadata()->name());
    d->activeCreateIndex = false;
  }
#endif
  return -99;
}

bool QMYSQLDriver::alterTable(const QString &mtd1, const QString &mtd2, const QString &key)
{
  return alterTable2(mtd1, mtd2, key);
}

static inline bool hasCheckColumn(FLTableMetaData *mtd)
{
  const FLTableMetaData::FLFieldMetaDataList *fieldList = mtd->fieldList();
  if (!fieldList)
    return false;
  FLFieldMetaData *field = 0;
  QDictIterator<FLFieldMetaData> it(*fieldList);
  while ((field = it.current()) != 0) {
    if (field->isCheck() || field->name().endsWith("_check_column"))
      return true;
    ++it;
  }
  return false;
}

bool QMYSQLDriver::alterTable2(const QString &mtd1, const QString &mtd2, const QString &key, bool force)
{
#ifndef FL_QUICK_CLIENT
  FLTableMetaData *oldMTD = 0;
  FLTableMetaData *newMTD = 0;
  QDomDocument doc("doc");
  QDomElement docElem;

  if (!FLUtil::domDocumentSetContent(doc, mtd1)) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("Error al cargar los metadatos."));
    qWarning("FLManager::alterTable : " + key);
#endif
  } else {
    docElem = doc.documentElement();
    oldMTD = db_->manager()->metadata(&docElem, true);
  }

  if (oldMTD && oldMTD->isQuery())
    return true;

  if (oldMTD && hasCheckColumn(oldMTD))
    return false;

  if (!FLUtil::domDocumentSetContent(doc, mtd2)) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("Error al cargar los metadatos."));
    qWarning("FLManager::alterTable : " + key);
#endif
    return false;
  } else {
    docElem = doc.documentElement();
    newMTD = db_->manager()->metadata(&docElem, true);
  }

  if (newMTD && hasCheckColumn(newMTD))
    return false;

  if (!oldMTD)
    oldMTD = newMTD;

  if (oldMTD->name() != newMTD->name()) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("Los nombres de las tablas nueva y vieja difieren."));
#endif

    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  QString oldPK = oldMTD->primaryKey(), newPK = newMTD->primaryKey();
  if (oldPK != newPK) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("Los nombres de las claves primarias difieren."));
#endif

    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  if (oldMTD->fieldType(oldPK) != newMTD->fieldType(newPK)) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("Los tipos de las claves primarias difieren."));
#endif

    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  if (!force && db_->manager()->checkMetaData(oldMTD, newMTD)) {
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return true;
  }

  if (!db_->manager()->existsTable(oldMTD->name())) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("La tabla %1 antigua de donde importar los registros no existe.").arg(oldMTD->name()));
#endif

    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  const FLTableMetaData::FLFieldMetaDataList *fieldList = oldMTD->fieldList();
  FLFieldMetaData *oldField = 0;

  if (!fieldList) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("Los antiguos metadatos no tienen campos."));
#endif
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  QStringList fieldsNamesOld;
  if (!force) {
    QDictIterator<FLFieldMetaData> it(*fieldList);
    while ((oldField = it.current()) != 0) {
      ++it;
      if (newMTD->field(oldField->name()))
        fieldsNamesOld.append(oldField->name());
    }
  }

  QString renameOld = oldMTD->name().left(6) + "alteredtable" + QDateTime::currentDateTime().toString("ddhhssz");

  if (!db_->dbAux()) {
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  fieldList = newMTD->fieldList();

  if (!fieldList) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("Los nuevos metadatos no tienen campos."));
#endif
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  if (fieldList->isEmpty()) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("Los nuevos metadatos no tienen campos."));
#endif
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  QSqlQuery q(QString::null, db_->dbAux());

  if (!q.exec("ALTER TABLE " + oldMTD->name() + " RENAME TO " + renameOld)) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("No se ha podido renombrar la tabla antigua."));
#endif
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  if (!db_->manager()->createTable(newMTD)) {
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  db_->dbAux()->transaction();

  if (!force && !key.isEmpty() && key.length() == 40) {
    QSqlCursor c("flfiles", true, db_->dbAux());
    c.setForwardOnly(true);
    QSqlRecord *buffer;
    c.setFilter("nombre='" + renameOld + ".mtd'");
    c.select();
    if (!c.next()) {
      buffer = c.primeInsert();
      buffer->setValue("nombre", renameOld + ".mtd");
      buffer->setValue("contenido", mtd1);
      buffer->setValue("sha", key);
      c.insert();
    }
  }

  bool ok = false;
  if (!force && !fieldsNamesOld.isEmpty()) {
     QString sel(fieldsNamesOld.join(","));
     QString inSql("INSERT INTO " + newMTD->name() + "(" + sel + ")" +
                   " SELECT " + sel + " FROM " + renameOld);
     qWarning(inSql);
     ok = q.exec(inSql);
    if (!ok) {
      db_->dbAux()->rollback();
      if ((oldMTD != newMTD) && oldMTD)
        delete oldMTD;
      if (newMTD)
        delete newMTD;
      return alterTable2(mtd1, mtd2, key, true);
    }
  }

  if (!ok) {
    QSqlCursor oldCursor(renameOld, true, db_->dbAux());
    oldCursor.setMode(QSqlCursor::ReadOnly);
    oldCursor.setForwardOnly(true);
    oldCursor.select();
    int totalSteps = oldCursor.size();
    QProgressDialog progress(QApplication::tr("Reestructurando registros para %1...").arg(newMTD->alias()), 0,
                             totalSteps, qApp->focusWidget(), 0, true);
    progress.setCaption(QApplication::tr("Tabla modificada"));

    int step = 0;
    QSqlRecord *newBuffer;
    FLFieldMetaData *newField = 0;
    QPtrList<QSqlRecord> listRecords;
    listRecords.setAutoDelete(true);
    QSqlRecord newBufferInfo(recordInfo2(newMTD->name()).toRecord());
    QPtrVector<FLFieldMetaData> vectorFields(fieldList->count() * 2);
    QMap<int, QVariant> defValues;
    QVariant v;

    QDictIterator<FLFieldMetaData> it2(*fieldList);
    while ((newField = it2.current()) != 0) {
      ++it2;
      oldField = oldMTD->field(newField->name());
      if (!oldField || !oldCursor.field(oldField->name())) {
        if (!oldField)
          oldField = newField;
        if (newField->type() != FLFieldMetaData::Serial) {
          v = newField->defaultValue();
          v.cast(FLFieldMetaData::flDecodeType(newField->type()));
          defValues.insert(step, v);
        }
      }
      vectorFields.insert(step++, newField);
      vectorFields.insert(step++, oldField);
    }

    step = 0;
    ok = true;
    while (oldCursor.next()) {
      newBuffer = new QSqlRecord(newBufferInfo);

      for (uint i = 0; i < vectorFields.size();) {
        if (defValues.contains(i)) {
          v = defValues[i];
          newField = vectorFields[i++];
          oldField = vectorFields[i++];
        } else {
          newField = vectorFields[i++];
          oldField = vectorFields[i++];
          v = oldCursor.value(newField->name());
          if ((!oldField->allowNull() || !newField->allowNull()) &&
              (v.isNull() || !v.isValid()) && newField->type() != FLFieldMetaData::Serial) {
            QVariant defVal(newField->defaultValue());
            if (!defVal.isNull() && defVal.isValid())
              v = defVal;
          }
          if (v.isValid() && !v.isNull() && !v.cast(newBuffer->value(newField->name()).type())) {
#ifdef FL_DEBUG
            qWarning("FLManager::alterTable : " +
                     QApplication::tr("Los tipos del campo %1 no son compatibles. Se introducirá un valor nulo.")
                     .arg(newField->name()));
#endif
          }
        }

        if (!v.isNull() && newField->type() == QVariant::String && newField->length() > 0)
          v = v.toString().left(newField->length());

        if ((!oldField->allowNull() || !newField->allowNull()) && (v.isNull() || !v.isValid())) {
          switch (oldField->type()) {
            case FLFieldMetaData::Serial:
              v = nextSerialVal(newMTD->name(), newField->name()).toUInt();
              break;
            case QVariant::Int:
            case QVariant::UInt:
            case QVariant::Bool:
            case FLFieldMetaData::Unlock:
              v =  int(0);
              break;
            case QVariant::Double:
              v = double(0.0);
              break;
            case QVariant::Time:
              v = QTime::currentTime();
              break;
            case QVariant::Date:
              v = QDate::currentDate();
              break;
            default:
              v = QString("NULL").left(newField->length());
              break;
          }
        }
        newBuffer->setValue(newField->name(), v);
      }

      listRecords.append(newBuffer);
      if (listRecords.count() >= LIMIT_RESULT) {
        step += LIMIT_RESULT;
        progress.setProgress(step);

        if (!insertMulti(newMTD->name(), &listRecords)) {
          ok = false;
          listRecords.clear();
          break;
        }
        listRecords.clear();
      }
    }

    if (listRecords.count() > 0) {
      if (!insertMulti(newMTD->name(), &listRecords))
        ok = false;
      listRecords.clear();
    }

    progress.setProgress(totalSteps);
  }

  if (ok) {
    db_->dbAux()->commit();
#if 0
    //#ifndef FL_QUICK_CLIENT
    if (dictIndexes) {
      delete dictIndexes;
      dictIndexes = 0;
    }
    QStringList fieldsNames = QStringList::split(",", newMTD->fieldsNames());
    for (QStringList::Iterator it = fieldsNames.begin(); it != fieldsNames.end(); ++it) {
      FLFieldMetaData *field = newMTD->field((*it));
      if (field->relationM1() || field->isPrimaryKey())
        createIndex(field->name(), newMTD->name());
      else if (field->type() == QVariant::Date)
        createIndex(field->name(), newMTD->name());
    }
#endif

    if (force)
      q.exec("DROP TABLE " + renameOld + " CASCADE");
  } else {
    db_->dbAux()->rollback();

    q.exec("DROP TABLE " + oldMTD->name() + " CASCADE");
    q.exec("ALTER TABLE " + renameOld + " RENAME TO " + oldMTD->name());

    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  if ((oldMTD != newMTD) && oldMTD)
    delete oldMTD;
  if (newMTD)
    delete newMTD;

  return true;
#else
  return true;
#endif //FL_QUICK_CLIENT
}

int QMYSQLDriver::insertMulti(const QString &tablename, QPtrList<QSqlRecord> * records)
{
  QSqlRecord *editBuffer = records->at(0);
  int k = editBuffer->count();
  if (k == 0)
    return 0;

  QString fList;
  QString vList;
  bool comma = false;
  int j;

  for (j = 0; j < k; ++j) {
    QSqlField *f = editBuffer->field(j);
    if (editBuffer->isGenerated(j)) {
      if (comma) {
        fList += ",";
        vList += ",";
      }
      fList += f->name();
      vList += formatValue(f, false);
      comma = true;
    }
  }

  if (!comma)
    return 0;

  QString str;
  str.append("insert into ").append(tablename).append("(").append(fList).append(") values (").append(vList).append(")");

  int rows = records->count();

  for (int row = 1; row < rows; ++row) {
    editBuffer = records->at(row);
    vList = QString::null;
    comma = false;

    for (j = 0; j < k; ++j) {
      QSqlField *f = editBuffer->field(j);
      if (editBuffer->isGenerated(j)) {
        if (comma)
          vList += ",";
        vList += formatValue(f, false);
        comma = true;
      }
    }

    str.append(",(").append(vList).append(")");
  }

  QSqlQuery qry(QString::null, db_->dbAux());
  if (!qry.exec(str) || qry.lastError().type() != QSqlError::None) {
    msgBoxCritical(tr("Error"),
                   QString(qry.lastError().driverText().utf8()) + "\n" +
                   QString(qry.lastError().databaseText().utf8()));
    return 0;
  }

  return rows;
}

#ifndef FL_QUICK_CLIENT
void QMYSQLDriver::createIndex(const QString &fieldName, const QString &tableName) const
{
  if (!d->activeCreateIndex || !isOpen() || fieldName.isEmpty() || tableName.isEmpty())
    return;

  if (tableName.contains("alteredtable"))
    return;

  QStringList listTables(tables(""));

  if (!listTables.contains(tableName))
    return;

  if (!dictIndexes) {
    dictIndexes = new QDict < bool > (277);
    dictIndexes->setAutoDelete(true);

    QSqlQuery *idxs = new QSqlQuery(new QMYSQLResult(this));
    idxs->setForwardOnly(true);
    idxs->exec("select index_name from information_schema.statistics where table_schema='" + db_->database() +
               "' and index_name like '%_m1_idx' and table_name not like '%%alteredtable%'");
    while (idxs->next())
      dictIndexes->insert(idxs->value(0).toString(), new bool(true));
    delete idxs;
  }

  QString indexName = tableName.left(25) + "_" + fieldName.left(25) + "_m1_idx";
  indexName.replace(" ", "").replace(",", "");

  if (dictIndexes->find(indexName))
    return;

  QString strQry = "create index " + indexName + " on " + tableName + " (" + fieldName + ")";
  const char *encQuery = strQry.ascii();
  if (mysql_real_query(d->mysql, encQuery, qstrlen(encQuery))) {
#ifdef FL_DEBUG
    qWarning(QApplication::tr("La consulta a la base de datos ha fallado : ") + strQry);
#endif
    return;
  }
  dictIndexes->replace(indexName, new bool(true));
}
#endif

bool QMYSQLDriver::canSavePoint()
{
  return !d->noInnoDB;
}

bool QMYSQLDriver::savePoint(const QString &n)
{
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QMYSQLDriver::savePoint: Database not open");
#endif
    return false;
  }
  if (d->noInnoDB)
    return true;
  QString strQry = "savepoint sv_" + n;
  const char *encQuery = strQry.ascii();
  if (mysql_real_query(d->mysql, encQuery, qstrlen(encQuery))) {
    setLastError(qMakeError(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
    return false;
  }
  return true;
}

bool QMYSQLDriver::releaseSavePoint(const QString &n)
{
  return true;
}

bool QMYSQLDriver::rollbackSavePoint(const QString &n)
{
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QMYSQLDriver::rollbackSavePoint: Database not open");
#endif
    return false;
  }
  if (d->noInnoDB)
    return true;
  QString strQry = "rollback to savepoint sv_" + n;
  const char *encQuery = strQry.ascii();
  if (mysql_real_query(d->mysql, encQuery, qstrlen(encQuery))) {
    setLastError(qMakeError(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
    return false;
  }
  return true;
}

static inline bool notEqualsFields(QSqlField *fieldBd,
                                   QSqlField *fieldMtd,
                                   const QSqlFieldInfo &fieldInfoBd,
                                   const QSqlFieldInfo &fieldInfoMtd)
{
  if (!fieldBd)
    return true;
  if (!fieldMtd)
    return false;

  if (!fieldBd->value().canCast(FLFieldMetaData::flDecodeType(fieldMtd->type()))) {
#ifdef FL_DEBUG
    qWarning("!= name " + fieldMtd->name() + " " + fieldBd->name());
    qWarning("!= type " +
             QString::number(FLFieldMetaData::flDecodeType(fieldMtd->type())) + " " +
             QString::number(fieldBd->type()));
#endif
    return true;
  }

  if (fieldBd->isNull() != fieldMtd->isNull()) {
#ifdef FL_DEBUG
    qWarning("!= name " + fieldMtd->name() + " " + fieldBd->name());
    qWarning("!= null " + QString::number(fieldMtd->isNull()) + " " +
             QString::number(fieldBd->isNull()));
#endif
    return true;
  }

  if (fieldBd->isReadOnly() != fieldMtd->isReadOnly()) {
#ifdef FL_DEBUG
    qWarning("!= name " + fieldMtd->name() + " " + fieldBd->name());
    qWarning("!= readOnly " + QString::number(fieldMtd->isReadOnly()) + " " +
             QString::number(fieldBd->isReadOnly()));
#endif
    return true;
  }

  if (fieldInfoBd.typeID() == FIELD_TYPE_STRING) {
    int lenBd = fieldInfoBd.length();
    int lenMtd = fieldInfoMtd.length();
    if (lenBd > 0 && lenMtd > 0  &&
        lenBd != lenMtd && (lenBd / 3) != lenMtd) {
#ifdef FL_DEBUG
      qWarning("!= name " + fieldMtd->name() + " " + fieldBd->name());
      qWarning("!= length " + QString::number(lenMtd) + " " +
               QString::number(lenBd));
#endif
      return true;
    }
  }

  return false;
}

void QMYSQLDriver::Mr_Proper()
{
#if 0
  QString mproperMsg(tr("Este proceso puede tener una larga duración, dependiendo\n"
                        "del tamaño de la base de datos.\n"
                        "Antes de empezar debe asegurarse que durante todo el proceso\n"
                        "no habrá otros usuarios conectados a esta base de datos, de lo\n"
                        "contrario los resultados serán impredecibles. Asegúrese también\n"
                        "de tener una COPIA DE SEGURIDAD actualizada de esta base de datos\n"
                        "antes de empezar.\n\n¿ Quiere continuar ?"));
  int res = QMessageBox::question(0, tr("Mr. Proper"), mproperMsg, QMessageBox::Yes, QMessageBox::No);
  if (res != QMessageBox::Yes)
    return;
#endif
  d->activeCreateIndex = false;
  db_->dbAux()->transaction();
  QSqlQuery qry(QString::null, db_->dbAux());
  QSqlQuery qry2(QString::null, db_->dbAux());
  int steps = 0;
  QString item;

  qry.setForwardOnly(true);
  qry2.setForwardOnly(true);
  QRegExp rx("^.*\\d{6,9}$");
  QStringList listOldBks(tables("").grep(rx));

  qry.exec("select nombre from flfiles where nombre regexp"
           "'.*[[:digit:]][[:digit:]][[:digit:]][[:digit:]]-[[:digit:]][[:digit:]].*:[[:digit:]][[:digit:]]$' or nombre regexp"
           "'.*alteredtable[[:digit:]][[:digit:]][[:digit:]][[:digit:]].*' or (bloqueo=0 and nombre like '%.mtd')");
  FLUtil::createProgressDialog(tr("Borrando backups"), listOldBks.size() + qry.size() + 2);
  while (qry.next()) {
    item = qry.value(0).toString();
    FLUtil::setLabelText(tr("Borrando registro %1").arg(item));
    qry2.exec("delete from flfiles where nombre = '" + item + "'");
#ifdef FL_DEBUG
    qWarning("delete from flfiles where nombre = '" + item + "'");
#endif
    if (item.contains("alteredtable")) {
      if (tables("").contains(item.replace(".mtd", ""))) {
        FLUtil::setLabelText(tr("Borrando tabla %1").arg(item));
        qry2.exec("drop table " + item.replace(".mtd", "") + " cascade");
#ifdef FL_DEBUG
        qWarning("drop table " + item.replace(".mtd", "") + " cascade");
#endif
      }
    }
    FLUtil::setProgress(++steps);
  }

  for (QStringList::Iterator it = listOldBks.begin(); it != listOldBks.end(); ++it) {
    item = *it;
    if (tables("").contains(item)) {
      FLUtil::setLabelText(tr("Borrando tabla %1").arg(item));
      qry2.exec("drop table " + item + " cascade");
#ifdef FL_DEBUG
      qWarning("drop table " + item + " cascade");
#endif
    }
    FLUtil::setProgress(++steps);
  }

  FLUtil::setLabelText(tr("Inicializando cachés"));
  FLUtil::setProgress(++steps);
  qry.exec("delete from flmetadata");
  qry.exec("delete from flvar");
  AQ_DISKCACHE_CLR();
  db_->manager()->cleanupMetaData();
  db_->dbAux()->commit();
  FLUtil::destroyProgressDialog();

  qry.exec("show tables");
  int qsize = qry.size();
  steps = 0;
  FLUtil::createProgressDialog(tr("Comprobando base de datos"), qsize);
#ifndef FL_QUICK_CLIENT
  if (dictIndexes) {
    delete dictIndexes;
    dictIndexes = 0;
  }
#endif
  while (qsize && qry.next()) {
    item = qry.value(0).toString();
    FLUtil::setLabelText(tr("Comprobando tabla %1").arg(item));

    if (!d->forceChapuza)
      qry2.exec("alter table " + item + " convert to character set utf8 collate utf8_bin");

    bool mustAlter = mismatchedTable(item, item);
    if (mustAlter) {
      QString conte(db_->managerModules()->content(item + ".mtd"));
      if (!conte.isEmpty()) {
        QString msg(QApplication::tr(
                      "La estructura de los metadatos de la tabla '%1' y su "
                      "estructura interna en la base de datos no coinciden. "
                      "Intentando regenerarla."
                    ));
        qWarning(msg.arg(item));
        alterTable2(conte, conte, QString::null, true);
      }
    }

    FLUtil::setProgress(++steps);
    --qsize;
  }

  db_->dbAux()->transaction();
  d->activeCreateIndex = true;
  steps = 0;

  QString engine(d->noInnoDB ? "MyISAM" : "INNODB");
  bool convertToEngine = false;
  bool doQues = true;

  QSqlCursor sqlCursor(QString::null, true, db_->dbAux());
  sqlCursor.setForwardOnly(true);
  QSqlQuery sqlQuery(QString::null, db_->dbAux());
  sqlQuery.setForwardOnly(true);

  if (sqlQuery.exec("show tables")) {
    cInfo->clear();
    FLUtil::setTotalSteps(qry.size());
    while (sqlQuery.next()) {
      item = sqlQuery.value(0).toString();
      FLUtil::setProgress(++steps);
      FLUtil::setLabelText(tr("Creando índices para %1").arg(item));

      FLTableMetaData *mtd = db_->manager()->metadata(item);
      const FLTableMetaData::FLFieldMetaDataList *fl;
      if (!mtd || !(fl = mtd->fieldList()))
        continue;
      QDictIterator<FLFieldMetaData> it(*fl);
      for (; it.current(); ++it) {
        if (!(*it) || (*it)->type() != QVariant::Pixmap)
          continue;
        QString v;
        QSqlCursor cur(item, true, db_->dbAux());
        cur.setForwardOnly(true);
        QSqlRecord *buf;
        cur.select((*it)->name() + QString::fromLatin1(" not like 'RK@%'"));
        while (cur.next()) {
          v = cur.value((*it)->name()).toString();
          if (v.isEmpty())
            continue;
          v = db_->manager()->storeLargeValue(mtd, v);
          if (!v.isEmpty()) {
            buf = cur.primeUpdate();
            buf->setValue((*it)->name(), v);
            cur.update(false);
          }
        }
      }

      qry2.exec("show table status where Engine='" + engine + "' and Name='" + item + "'");
      if (!qry2.next()) {
        if (doQues &&
            QMessageBox::Yes == QMessageBox::question(0, tr("Mr. Proper"),
                                                      tr("Existen tablas que no son del tipo %1 utilizado por el driver de la conexión actual.\n"
                                                         "Ahora es posible convertirlas, pero asegurése de tener una COPIA DE SEGURIDAD,\n"
                                                         "se pueden peder datos en la conversión de forma definitiva.\n\n"
                                                         "¿ Quiere convertirlas ?").arg(engine), QMessageBox::Yes, QMessageBox::No)) {
          convertToEngine = true;
        }
        doQues = false;
        if (convertToEngine) {
          QString conte = db_->managerModules()->content(item + ".mtd");
          alterTable2(conte, conte, QString::null, true);
        }
      }

      sqlCursor.setName(item, true);
    }
  }

  d->activeCreateIndex = false;
  db_->dbAux()->commit();

  FLUtil::destroyProgressDialog();
}

QMYSQLDriver::MySQLVersion QMYSQLDriver::version() const
{
  return version_;
}

bool QMYSQLDriver::mismatchedTable(const QString &table,
                                   const FLTableMetaData *tmd) const
{
  return mismatchedTable(table, tmd->name());
}

bool QMYSQLDriver::mismatchedTable(const QString &table1,
                                   const QString &table2) const
{
  FLTableMetaData *mtd = db_->manager()->metadata(table2, true);
  if (!mtd)
    return false;

  QSqlRecordInfo recInfoMtd = recordInfo(table2);
  QSqlRecordInfo recInfoBd = recordInfo2(table1);
  QSqlRecord recMtd = recInfoMtd.toRecord();
  QSqlRecord recBd = recInfoBd.toRecord();
  QSqlField *fieldMtd = 0;
  QSqlField *fieldBd = 0;
  bool mismatch = false;

  for (int i = 0; i < recMtd.count(); ++i) {
    fieldMtd = recMtd.field(i);
    fieldBd = recBd.field(fieldMtd->name());
    if (fieldBd) {
      if (notEqualsFields(fieldBd, fieldMtd,
                          recInfoBd.find(fieldMtd->name()),
                          recInfoMtd.find(fieldMtd->name()))) {
        mismatch = true;
        break;
      }
    } else {
      mismatch = true;
      break;
    }
  }

  return mismatch;
}
