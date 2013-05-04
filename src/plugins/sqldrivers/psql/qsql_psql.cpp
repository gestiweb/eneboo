/***************************************************************************
                             qsql_psql.cpp
                         -------------------
begin                : mie Sep 15 2004
copyright            : (C) 2004-2005 by InfoSiAL S.L.
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
/****************************************************************************
**
** Implementation of PostgreSQL driver classes
**
** Created : 001103
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
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

#include "qsql_psql.h"
#include <private/qsqlextension_p.h>

#include <math.h>

#include <qapplication.h>
#include <qdom.h>
#include <qprogressdialog.h>
#include <qpointarray.h>
#include <qsqlrecord.h>
#include <qregexp.h>
#include <qdatetime.h>
#include <qsqlselectcursor.h>
#include <qtextcodec.h>
#include <qcache.h>
#if defined(DEBUG)
# undef DEBUG
#endif
#include <postgres.h>
#include <libpq/libpq-fs.h>
#if defined(errno)
# undef errno
#endif
#define errno qt_psql_errno
#include <catalog/pg_type.h>
#undef errno

#ifdef FL_QUICK_CLIENT
#define LIMIT_RESULT 49
#else
#define LIMIT_RESULT 99
#endif

int QPSQLResult::cursorCounter = 0;

#ifndef FL_QUICK_CLIENT
QDict < bool > * QPSQLDriver::dictIndexes = 0;
#endif

QPtrDict < QSqlDriverExtension > *qSqlDriverExtDict();
QPtrDict < QSqlOpenExtension > *qSqlOpenExtDict();

/* This is a compile time switch - if PQfreemem is declared, the compiler will use that one,
   otherwise it'll run in this template */
template <typename T>
inline void PQfreemem(T *t, int = 0)
{
  free(t);
}

inline void qPQfreemem(void *buffer)
{
  PQfreemem(buffer);
}

class FLPGresult
{
public:
  FLPGresult(PGresult *result = 0)
    : result_(result) {}
  ~FLPGresult() {
    if (result_)
      PQclear(result_);
  }

  PGresult *result_;
};

class QPSQLPrivate
{

public:

  QPSQLPrivate();

  PGconn *connection;
  PGresult *result;
  FLPGresult *flresult;
  bool isUtf8;
  QString qry;
  QString idCursor;
  int idConn;
  bool closeCursor;
  QCache < FLPGresult > * cachedResults;
  int currentResult;
  bool checkLock;
  bool activeCreateIndex;
#ifdef FL_DEBUG
  QDict < QString > * dictPendCursors;
#endif
};

QPSQLPrivate::QPSQLPrivate()
  : connection(0),
    result(0),
    flresult(0),
    isUtf8(false),
    idConn(0),
    closeCursor(false),
    cachedResults(0),
    currentResult(0),
    checkLock(true),
    activeCreateIndex(false)
#ifdef FL_DEBUG
    , dictPendCursors(0)
#endif
{
}

class QPSQLDriverExtension : public QSqlDriverExtension
{

public:
  QPSQLDriverExtension(QPSQLDriver *dri) : QSqlDriverExtension(), driver(dri) {}
  ~QPSQLDriverExtension() {}

  bool isOpen() const;

private:

  QPSQLDriver *driver;
};

bool QPSQLDriverExtension::isOpen() const
{
  return PQstatus(driver->connection()) == CONNECTION_OK;
}

class QPSQLOpenExtension : public QSqlOpenExtension
{

public:

  QPSQLOpenExtension(QPSQLDriver *dri) : QSqlOpenExtension(), driver(dri) {}
  ~QPSQLOpenExtension() {}

  bool open(const QString &db, const QString &user, const QString &password, const QString &host, int port, const QString &connOpts);

private:

  QPSQLDriver *driver;
};

bool QPSQLOpenExtension::open(const QString &db, const QString &user, const QString &password, const QString &host, int port, const QString &connOpts)
{
  return driver->open(db, user, password, host, port, connOpts);
}

static QSqlError qMakeError(const QString &err, int type, const QPSQLPrivate *p)
{
  const char *s = PQerrorMessage(p->connection);
  QString msg = "\n";
  QTextCodec *tc = QTextCodec::codecForName("ISO8859-15");
  int slen = qstrlen(s);
  if (tc->heuristicContentMatch(s, slen) >= slen)
    msg += QString::fromLatin1(s);
  else
    msg += QString::fromUtf8(s);
  return QSqlError("QPSQL: " + err, msg, type);
}

static QVariant::Type qDecodePSQLType(int t)
{
  QVariant::Type type = QVariant::Invalid;
  switch (t) {
    case BOOLOID:
      type = QVariant::Bool;
      break;
    case INT8OID:
      type = QVariant::LongLong;
      break;
    case INT2OID:
    case INT4OID:
      type = QVariant::Int;
      break;
    case NUMERICOID:
    case FLOAT4OID:
    case FLOAT8OID:
      type = QVariant::Double;
      break;
    case ABSTIMEOID:
    case RELTIMEOID:
    case DATEOID:
      type = QVariant::Date;
      break;
    case TIMEOID:
#ifdef TIMETZOID
    case TIMETZOID:
#endif
      type = QVariant::Time;
      break;
    case TIMESTAMPOID:
#ifdef DATETIMEOID
    case DATETIMEOID:
#endif
#ifdef TIMESTAMPTZOID
    case TIMESTAMPTZOID:
#endif
      type = QVariant::DateTime;
      break;
    case OIDOID:
    case BYTEAOID:
      type = QVariant::ByteArray;
      break;
    case REGPROCOID:
    case TIDOID:
    case XIDOID:
    case CIDOID:
    case UNKNOWNOID:
      //type = QVariant::Invalid;
      type = QVariant::String;
      break;
    default:
    case CHAROID:
    case BPCHAROID:
    case VARCHAROID:
    case TEXTOID:
    case NAMEOID:
    case CASHOID:
    case INETOID:
    case CIDROID:
    case CIRCLEOID:
      type = QVariant::String;
      break;
  }
  return type;
}

QPSQLResult::QPSQLResult(const QPSQLDriver *db, const QPSQLPrivate *p) : QSqlResult(db), currentSize(-1)
{
  d = new QPSQLPrivate();
  d->connection = p->connection;
  d->isUtf8 = p->isUtf8;
  d->idConn = p->idConn;
}

QPSQLResult::~QPSQLResult()
{
  cleanup();
#ifdef FL_DEBUG
  if (d->dictPendCursors)
    delete d->dictPendCursors;
#endif
  delete d;
}

PGresult *QPSQLResult::result()
{
  return d->result;
}

bool QPSQLResult::openCursor()
{
  if (!d->qry.isEmpty() && !d->idCursor.isEmpty()) {
    if (d->closeCursor)
      closeCursor();

    QString q;
    if (isForwardOnly())
      q = "DECLARE " + d->idCursor + " NO SCROLL CURSOR";
    else
      q = "DECLARE " + d->idCursor + " SCROLL CURSOR";

    if (PQtransactionStatus(d->connection) != PQTRANS_INTRANS) {
      q += " WITH HOLD FOR " + d->qry;
      d->closeCursor = true;
#ifdef FL_DEBUG
      if (d->dictPendCursors)
        d->dictPendCursors->replace(d->idCursor, new QString(d->qry));
#endif
    } else
      q += " WITHOUT HOLD FOR " + d->qry;

    PGresult *result = 0;
    if (d->isUtf8) {
      result = PQexec(d->connection, q.utf8().data());
    } else {
      result = PQexec(d->connection, q.local8Bit().data());
    }

    int status = PQresultStatus(result);
    PQclear(result);

    if (status == PGRES_COMMAND_OK) {
      q = "FETCH FORWARD " + QString::number(LIMIT_RESULT) + " FROM " + d->idCursor;
      if (d->isUtf8) {
        result = PQexec(d->connection, q.utf8().data());
      } else {
        result = PQexec(d->connection, q.local8Bit().data());
      }
      status = PQresultStatus(result);
    }

    if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) {
      cleanupCache();
      d->result = result;
      d->flresult = new FLPGresult(result);
      if (!isForwardOnly()) {
        d->cachedResults = new QCache < FLPGresult >(20, 41);
        d->cachedResults->setAutoDelete(true);
      }
      return true;
    } else {
      setLastError(qMakeError(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
      //qWarning(QString(PQresultErrorMessage(result)) + "\n" + q + "\n");
      fprintf(stderr, "%s\n%s\n", PQresultErrorMessage(result), q.latin1());
      PQclear(result);
      cleanupCache();
    }
  }

  d->closeCursor = false;
  return false;
}

void QPSQLResult::closeCursor()
{
  if (!d->idCursor.isEmpty() && d->closeCursor) {
    QString q = "CLOSE " + d->idCursor;
    PGresult *result = 0;
    if (d->isUtf8) {
      result = PQexec(d->connection, q.utf8().data());
    } else {
      result = PQexec(d->connection, q.local8Bit().data());
    }
    PQclear(result);
#ifdef FL_DEBUG
    if (d->dictPendCursors)
      d->dictPendCursors->remove(d->idCursor);
#endif
  }

  d->idCursor = QString::null;
  d->closeCursor = false;
}

void QPSQLResult::cleanupCache()
{
  if (d->cachedResults)
    delete d->cachedResults;
  if (d->flresult)
    delete d->flresult;
  d->result = 0;
  d->flresult = 0;
  d->cachedResults = 0;
  d->currentResult = 0;
}

void QPSQLResult::cleanup()
{
  closeCursor();
  cleanupCache();
  d->qry = QString::null;
  setAt(QSql::BeforeFirst);
  currentSize = -1;
  setActive(false);
}

bool QPSQLResult::nextResult(int i)
{
  int currentResult = (int)(i / LIMIT_RESULT);
  if (d->result && (currentResult == d->currentResult || d->idCursor.isEmpty()))
    return true;

  FLPGresult *flres = (d->cachedResults ? d->cachedResults->take(QString::number(currentResult)) : 0);
  if (flres) {
    if (d->flresult)
      d->cachedResults->insert(QString::number(d->currentResult), d->flresult);
    d->result = flres->result_;
    d->flresult = flres;
    d->currentResult = currentResult;
  } else {
    PGresult *result = 0;
    int status = PGRES_COMMAND_OK, moveRows = 0;
    QString q;
    if (!isForwardOnly()) {
      q = "MOVE ABSOLUTE " + QString::number(currentResult * LIMIT_RESULT) + " FROM " + d->idCursor;
      if (d->isUtf8) {
        result = PQexec(d->connection, q.utf8().data());
      } else {
        result = PQexec(d->connection, q.local8Bit().data());
      }
      status = PQresultStatus(result);
      moveRows = PQntuples(result);
      PQclear(result);
      if (status != PGRES_COMMAND_OK)
        return false;
    } else {
      if (currentResult < d->currentResult) {
#ifdef FL_DEBUG
        qWarning("QPSQLResult : " + QApplication::tr("Esta consulta sólo puede posicionarse hacia adelante."));
#endif
        return false;
      }
    }

    if (status == PGRES_COMMAND_OK) {
      q = "FETCH FORWARD " + QString::number(LIMIT_RESULT) + " FROM " + d->idCursor;
      if (d->isUtf8) {
        result = PQexec(d->connection, q.utf8().data());
      } else {
        result = PQexec(d->connection, q.local8Bit().data());
      }
      status = PQresultStatus(result);
      if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) {
        if (status == PGRES_TUPLES_OK) {
          int fetchedRows = PQntuples(result);
          if (fetchedRows < LIMIT_RESULT) {
            currentSize = (currentResult * LIMIT_RESULT) + fetchedRows + moveRows;
            if (currentResult == 0 || isForwardOnly()) {
              closeCursor();
              cleanupCache();
            }
            if (i >= currentSize) {
              PQclear(result);
              return false;
            }
            if (currentResult == 0 || isForwardOnly()) {
              d->result = result;
              d->flresult = new FLPGresult(result);
              return true;
            }
          }
        } else {
          PQclear(result);
          return false;
        }
      } else {
        PQclear(result);
        return false;
      }
    }

    if (d->cachedResults && d->flresult)
      d->cachedResults->insert(QString::number(d->currentResult), d->flresult);
    else if (!d->cachedResults && d->flresult)
      delete d->flresult;

    d->result = result;
    d->flresult = new FLPGresult(result);
    d->currentResult = currentResult;
  }

  return true;
}

bool QPSQLResult::fetch(int i)
{
  if (!isActive() || !isSelect())
    return false;

  if (i < 0)
    return false;

  if (at() == i)
    return true;

  if (!d->idCursor.isEmpty())
    if (!nextResult(i))
      return false;

  if (i >= currentSize)
    return false;

  setAt(i);

  return true;
}

bool QPSQLResult::fetchFirst()
{
  return fetch(0);
}

bool QPSQLResult::fetchLast()
{
  return fetch(currentSize - 1);
}

static QPoint pointFromString(const QString &s)
{
  int pivot = s.find(',');
  if (pivot != -1) {
    int x = s.mid(1, pivot - 1).toInt();
    int y = s.mid(pivot + 1, s.length() - pivot - 2).toInt();
    return QPoint(x, y);
  } else
    return QPoint();
}

QVariant QPSQLResult::data(int i)
{
  if (d->result && i >= PQnfields(d->result)) {
#ifdef FL_DEBUG
    qWarning("QPSQLResult::data: column %d out of range", i);
#endif
    return QVariant();
  }

  int ptype = PQftype(d->result, i);
  QVariant::Type type = qDecodePSQLType(ptype);

  if (PQgetisnull(d->result, at() % LIMIT_RESULT, i)) {
    QVariant v;
    v.cast(type);
    return v;
  }

  QString val;
  if (ptype != BYTEAOID) {
    val = d->isUtf8 ? QString::fromUtf8(PQgetvalue(d->result, at() % LIMIT_RESULT, i)) :
          QString::fromLocal8Bit(PQgetvalue(d->result, at() % LIMIT_RESULT, i));
  }

  switch (type) {
    case QVariant::Bool: {
      QVariant b((bool)(val == "t"), 0);
      return (b);
    }
    case QVariant::String:
      return QVariant(val);
    case QVariant::LongLong:
      if (val[ 0 ] == '-')
        return QVariant(val.toLongLong());
      else
        return QVariant(val.toULongLong());
    case QVariant::Int:
      return QVariant(val.toInt());
    case QVariant::Double:
      if (ptype == NUMERICOID)
        return QVariant(val);
      return QVariant(val.toDouble());
    case QVariant::Date:
      if (val.isEmpty()) {
        return QVariant(QDate());
      } else {
        return QVariant(QDate::fromString(val, Qt::ISODate));
      }
    case QVariant::Time:
      if (val.isEmpty())
        return QVariant(QTime());
      if (val.at(val.length() - 3) == '+')
        return QVariant(QTime::fromString(val.left(val.length() - 3), Qt::ISODate));
      return QVariant(QTime::fromString(val, Qt::ISODate));
    case QVariant::DateTime: {
      if (val.length() < 10)
        return QVariant(QDateTime());
      QString dtval = val;
      if (dtval.at(dtval.length() - 3) == '+')
        dtval.truncate(dtval.length() - 3);
      if (dtval.at(dtval.length() - 3).isPunct())
        dtval += '0';
      if (dtval.isEmpty())
        return QVariant(QDateTime());
      else
        return QVariant(QDateTime::fromString(dtval, Qt::ISODate));
    }
    case QVariant::Point:
      return QVariant(pointFromString(val));
    case QVariant::Rect: {
      int pivot = val.find("),(");
      if (pivot != -1)
        return QVariant(QRect(pointFromString(val.mid(pivot + 2, val.length())), pointFromString(val.mid(0, pivot + 1))));
      return QVariant(QRect());
    }
    case QVariant::PointArray: {
      QRegExp pointPattern("\\([0-9-]*,[0-9-]*\\)");
      int points = val.contains(pointPattern);
      QPointArray parray(points);
      int idx = 1;
      for (int i = 0; i < points; i++) {
        int start = val.find(pointPattern, idx);
        int end = -1;
        if (start != -1) {
          end = val.find(')', start + 1);
          if (end != -1) {
            parray.setPoint(i, pointFromString(val.mid(idx, end - idx + 1)));
          } else
            parray.setPoint(i, QPoint());
        } else {
          parray.setPoint(i, QPoint());
          break;
        }
        idx = end + 2;
      }
      return QVariant(parray);
    }
    case QVariant::ByteArray: {
      if (ptype == BYTEAOID) {
        const char *v = PQgetvalue(d->result, at() % LIMIT_RESULT, i);
        size_t len;
        unsigned char *data = PQunescapeBytea((unsigned char *) v, &len);
        QByteArray ba(len);
        ba.duplicate((const char *) data, len);
        qPQfreemem(data);
        return QVariant(ba);
      }

      QByteArray ba;
      ((QSqlDriver *) driver())->beginTransaction();
      Oid oid = val.toInt();
      int fd = lo_open(d->connection, oid, INV_READ);
      if (fd < 0) {
#ifdef QT_CHECK_RANGE
        qWarning("QPSQLResult::data: unable to open large object for read");
#endif
        ((QSqlDriver *)driver())->commitTransaction();
        if (ptype == OIDOID)
          return val;
        return QVariant(ba);
      }

      int size = 0;
      int retval = lo_lseek(d->connection, fd, 0L, SEEK_END);
      if (retval >= 0) {
        size = lo_tell(d->connection, fd);
        lo_lseek(d->connection, fd, 0L, SEEK_SET);
      }
      if (size == 0) {
        lo_close(d->connection, fd);
        ((QSqlDriver *) driver())->commitTransaction();
        return QVariant(ba);
      }
      char *buf = new char[ size ];

#ifdef Q_OS_WIN32
      // ### For some reason lo_read() fails if we try to read more than
      // ### 32760 bytes
      char *p = buf;
      int nread = 0;

      while (size < nread) {
        retval = lo_read(d->connection, fd, p, 32760);
        nread += retval;
        p += retval;
      }
#else
      retval = lo_read(d->connection, fd, buf, size);
#endif

      if (retval < 0) {
        qWarning("QPSQLResult::data: unable to read large object");
      } else {
        ba.duplicate(buf, size);
      }
      delete[] buf;
      lo_close(d->connection, fd);
      ((QSqlDriver *) driver())->commitTransaction();
      return QVariant(ba);
    }
    default:
    case QVariant::Invalid:
#ifdef QT_CHECK_RANGE
      qWarning("QPSQLResult::data: unknown data type");
#endif
      ;
  }
  return QVariant();
}

bool QPSQLResult::isNull(int field)
{
  PQgetvalue(d->result, at() % LIMIT_RESULT, field);
  return PQgetisnull(d->result, at() % LIMIT_RESULT, field);
}

void QPSQLResult::calcSize()
{
  if (isSelect() && !d->qry.isEmpty()) {
    if (!d->idCursor.isEmpty() && !isForwardOnly()) {
      PGresult *result = 0;
      QString qr = "MOVE ALL FROM " + d->idCursor;
      if (d->isUtf8) {
        result = PQexec(d->connection, qr.utf8());
      } else {
        result = PQexec(d->connection, qr.local8Bit());
      }
      currentSize = LIMIT_RESULT + QString(PQcmdTuples(result)).toInt();
      PQclear(result);
      qr = "MOVE ABSOLUTE " + QString::number((d->currentResult + 1) * LIMIT_RESULT) + " FROM " + d->idCursor;
      if (d->isUtf8) {
        result = PQexec(d->connection, qr.utf8());
      } else {
        result = PQexec(d->connection, qr.local8Bit());
      }
      PQclear(result);
    } else {
      QString qryNoOrder = d->qry;
      qryNoOrder.replace(QRegExp("ORDER BY.*ASC"), "");
      qryNoOrder.replace(QRegExp("ORDER BY.*DESC"), "");
      qryNoOrder.replace(QRegExp("ORDER BY.*$"), "");
      qryNoOrder.replace(QRegExp("ORDER BY.*;"), ";");
      qryNoOrder.replace(QRegExp("ORDER BY.*LIMIT"), "LIMIT");
      qryNoOrder.replace(QRegExp("ORDER BY.*OFFSET"), "OFFSET");
      qryNoOrder.replace(QRegExp("ORDER BY.*FOR"), "FOR");

      qryNoOrder.replace(QRegExp("order by.*ASC"), "");
      qryNoOrder.replace(QRegExp("order by.*DESC"), "");
      qryNoOrder.replace(QRegExp("order by.*$"), "");
      qryNoOrder.replace(QRegExp("order by.*;"), ";");
      qryNoOrder.replace(QRegExp("order by.*limit"), "limit");
      qryNoOrder.replace(QRegExp("order by.*offset"), "offset");
      qryNoOrder.replace(QRegExp("order by.*for"), "for");

      PGresult *result = 0;
      QString qr("select count(*) from ( " + qryNoOrder + " ) as cursize");
      if (d->isUtf8) {
        result = PQexec(d->connection, qr.utf8());
      } else {
        result = PQexec(d->connection, qr.local8Bit());
      }
      int status = PQresultStatus(result);
      if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) {
        QString val(PQgetvalue(result, 0, 0));
        currentSize = val.toInt();
        PQclear(result);
      } else
        PQclear(result);
    }
  } else
    currentSize = -1;
}

bool QPSQLResult::reset(const QString &query)
{
  if (query.isEmpty())
    return false;
  const QSqlDriver *dr = driver();
  if (!dr || !dr->isOpen() || dr->isOpenError())
    return false;

  QString q(query.stripWhiteSpace());
  q.replace("=;", "= NULL;");
  while (q.endsWith(";"))
    q.truncate(q.length() - 1);

  QString qLimit(q);
  QString qUpper(q.upper());
  bool forUpdate = false;

  if (qUpper.left(7).contains("SELECT")) {
    forUpdate = qUpper.endsWith("FOR UPDATE") || qUpper.endsWith("NOWAIT");
    if (!forUpdate && !qUpper.contains(" LIMIT "))
      qLimit += " LIMIT " + QString::number(LIMIT_RESULT + 1);
  }

  cleanup();
  if (d->isUtf8) {
    d->result = PQexec(d->connection, qLimit.utf8().data());
    d->flresult = new FLPGresult(d->result);
  } else {
    d->result = PQexec(d->connection, qLimit.local8Bit().data());
    d->flresult = new FLPGresult(d->result);
  }

  int status = PQresultStatus(d->result);
  if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) {
    if (status == PGRES_TUPLES_OK) {
      currentSize = PQntuples(d->result);
      setSelect(true);
    } else {
      currentSize = -1;
      setSelect(false);
      setActive(true);
      return true;
    }
  } else {
    QString msgError = PQresultErrorMessage(d->result);
    setLastError(qMakeError(QApplication::tr("La consulta a la base de datos ha fallado"), QSqlError::Statement, d));
    //qWarning("QPSQLResult::reset: " + msgError + "\n" + q + "\n");
    fprintf(stderr, "%s\n%s\n", msgError.latin1(), q.latin1());
    cleanupCache();
    return false;
  }

  if (!forUpdate && currentSize >= LIMIT_RESULT) {
    d->qry = q;
    d->idCursor = "C" + QString::number(d->idConn) + QString::number(cursorCounter++);
    if (!openCursor())
      return false;
    calcSize();
  }

  setActive(true);
  return true;
}

int QPSQLResult::size()
{
  return currentSize;
}

int QPSQLResult::numRowsAffected()
{
  return QString(PQcmdTuples(d->result)).toInt();
}

///////////////////////////////////////////////////////////////////

static inline bool setEncodingUtf8(PGconn *connection)
{
  PGresult *result = PQexec(connection, "SET CLIENT_ENCODING TO 'UNICODE'");
  int status = PQresultStatus(result);
  PQclear(result);
  return status == PGRES_COMMAND_OK;
}

static inline void setDatestyle(PGconn *connection)
{
  PGresult *result = PQexec(connection, "SET DATESTYLE TO 'ISO'");
#ifdef QT_CHECK_RANGE
  int status = PQresultStatus(result);
  if (status != PGRES_COMMAND_OK)
    //qWarning("%s", PQerrorMessage(connection));
    fprintf(stderr, "%s\n%", PQerrorMessage(connection));
#endif
  PQclear(result);
}

static inline bool setByteaOutputEscape(PGconn *connection)
{
  PGresult *result = PQexec(connection, "SET BYTEA_OUTPUT TO ESCAPE");
  int status = PQresultStatus(result);
  PQclear(result);
  return status == PGRES_COMMAND_OK;
}

static inline bool setTransactionReadCommited(PGconn *connection)
{
  PGresult *result = PQexec(connection, "SET SESSION CHARACTERISTICS AS TRANSACTION ISOLATION LEVEL READ COMMITTED READ WRITE");
  int status = PQresultStatus(result);
  PQclear(result);
  return status == PGRES_COMMAND_OK;
}

static inline QPSQLDriver::Protocol getPSQLVersion(PGconn *connection)
{
  PGresult *result = PQexec(connection, "select version()");
  int status = PQresultStatus(result);
  if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) {
    QString val(PQgetvalue(result, 0, 0));
    PQclear(result);
    QRegExp rx("(\\d+)\\.(\\d+)");
    rx.setMinimal(true);
    if (rx.search(val) != -1) {
      int vMaj = rx.cap(1).toInt();
      int vMin = rx.cap(2).toInt();
      if (vMaj < 6) {
        qWarning("This version of PostgreSQL is not supported and may not work.");
        return QPSQLDriver::Version6;
      }
      if (vMaj == 6) {
        return QPSQLDriver::Version6;
      } else if (vMaj == 7) {
        if (vMin < 1)
          return QPSQLDriver::Version7;
        else if (vMin < 3)
          return QPSQLDriver::Version71;
        else if (vMin < 4)
          return QPSQLDriver::Version73;
        return QPSQLDriver::Version74;
      } else if (vMaj == 8) {
        if (vMin < 1)
          return QPSQLDriver::Version8;
        else if (vMin < 2)
          return QPSQLDriver::Version81;
        else if (vMin < 3)
          return QPSQLDriver::Version82;
        else if (vMin < 4)
          return QPSQLDriver::Version83;
        return QPSQLDriver::Version84;
      } else if (vMaj == 9) {
        if (vMin < 1)
          return QPSQLDriver::Version9;
        return QPSQLDriver::Version91;
      }
      return QPSQLDriver::Version7;
    }
  } else {
    qWarning("This version of PostgreSQL is not supported and may not work.");
    PQclear(result);
  }

  return QPSQLDriver::Version7;
}

QPSQLDriver::QPSQLDriver(QObject *parent, const char *name) :
  FLSqlDriver(parent, name ? name : "QPSQL"),
  pro(QPSQLDriver::Version6)
{
  init();
}

QPSQLDriver::QPSQLDriver(PGconn *conn, QObject *parent, const char *name) :
  FLSqlDriver(parent, name ? name : "QPSQL"),
  pro(QPSQLDriver::Version6)
{
  init();
  d->connection = conn;
  if (conn) {
    pro = getPSQLVersion(d->connection);
    setOpen(true);
    setOpenError(false);
  }
}

void QPSQLDriver::init()
{
  qSqlDriverExtDict()->insert(this, new QPSQLDriverExtension(this));
  qSqlOpenExtDict()->insert(this, new QPSQLOpenExtension(this));

  d = new QPSQLPrivate();
}

QPSQLDriver::~QPSQLDriver()
{
  if (d->connection)
    PQfinish(d->connection);
  delete d;
  if (!qSqlDriverExtDict()->isEmpty()) {
    QSqlDriverExtension *ext = qSqlDriverExtDict()->take(this);
    delete ext;
  }
  if (!qSqlOpenExtDict()->isEmpty()) {
    QSqlOpenExtension *ext = qSqlOpenExtDict()->take(this);
    delete ext;
  }
}

PGconn *QPSQLDriver::connection()
{
  return d->connection;
}

bool QPSQLDriver::hasFeature(DriverFeature f) const
{
  switch (f) {
    case Transactions:
      return true;
    case QuerySize:
      return true;
    case BLOB:
      return pro >= QPSQLDriver::Version71;
    case Unicode:
      return d->isUtf8;
    default:
      return false;
  }
}

bool QPSQLDriver::open(const QString &, const QString &, const QString &, const QString &, int)
{
  qWarning("QPSQLDriver::open(): This version of open() is no longer supported.");
  return false;
}

bool QPSQLDriver::open(const QString &db, const QString &user, const QString &password,
                       const QString &host, int port, const QString &connOpts)
{
  if (isOpen())
    close();
  QString connectString;
  QString value_host = host;
  QString value_db = db;
  QString value_user = user;
  QString value_password = password;
  value_host = value_host.replace("'","\\'");
  value_db = value_db.replace("'","\\'");
  value_user = value_user.replace("'","\\'");
  value_password = value_password.replace("'","\\'");
  
  if (host.length())
    connectString.append("host='").append(value_host).append("'");
  if (db.length())
    connectString.append(" dbname='").append(value_db).append("'");
  if (user.length())
    connectString.append(" user='").append(value_user).append("'");
  if (password.length())
    connectString.append(" password='").append(value_password).append("'");
  if (port > -1)
    connectString.append(" port='").append(QString::number(port)).append("'");

  if (!connOpts.isEmpty())
    connectString += " " + QStringList::split(';', connOpts).join(" ");

  d->connection = PQconnectdb(connectString.local8Bit().data());
  if (PQstatus(d->connection) == CONNECTION_BAD) {
    setLastError(qMakeError(QApplication::tr("No se puede conectar a la base de datos"), QSqlError::Connection, d));
    setOpenError(true);
    return false;
  }

  d->idConn = PQbackendPID(d->connection);

#ifdef FL_DEBUG
  if (!d->dictPendCursors) {
    d->dictPendCursors = new QDict < QString > (101);
    d->dictPendCursors->setAutoDelete(true);
  }
  PQsetErrorVerbosity(d->connection, PQERRORS_VERBOSE);
#endif

  pro = getPSQLVersion(d->connection);
  d->isUtf8 = setEncodingUtf8(d->connection);
  setDatestyle(d->connection);
  setTransactionReadCommited(d->connection);
  if (pro >= QPSQLDriver::Version9)
    setByteaOutputEscape(d->connection);

  setOpen(true);
  setOpenError(false);

  return true;
}

bool QPSQLDriver::tryConnect(const QString &db, const QString &user, const QString &password, const QString &host, int port)
{
  if (!open("template1", user, password, host, port, QString::null)) {
    if (lastError().text().contains("template1"))
      QMessageBox::critical(0, tr("Conexión fallida"),
                            tr("La base de datos template1 no existe."), QMessageBox::Ok, 0, 0);
    else
      QMessageBox::critical(0, tr("Conexión fallida"),
                            tr("No se pudo conectar con la base de datos %1.").arg(db), QMessageBox::Ok, 0, 0);
    QMessageBox::critical(0, tr("Error"), QString(lastError().driverText()) + "\n" +
                          QString(lastError().databaseText()), QMessageBox::Ok, 0, 0);
    return false;
  }
  close();
  setOpenError(false);
  setLastError(QSqlError());

  if (protocol() < QPSQLDriver::Version74) {
    QMessageBox::warning(0, tr("Conexión fallida"),
                         tr("La versión de PostgreSQL es inferior a la 7.4.\nSe necesita PostgreSQL 7.4 o superior."),
                         QMessageBox::Ok, 0, 0);
    close();
    setOpenError(true);
    return false;
  }

  if (!open(db, user, password, host, port, QString::null)) {
    if (lastError().type() == QSqlError::Connection) {
      if (lastError().text().contains(db)) {
        int res = QMessageBox::question(0, tr("Crear base de datos"), tr("La base de datos %1 no existe. ¿Quiere crearla?").arg(db),
                                        QMessageBox::Yes, QMessageBox::No);
        if (res == QMessageBox::Yes) {
          close();
          setOpenError(false);
          setLastError(QSqlError());
          open("template1", user, password, host, port, QString::null);
          QSqlQuery *qry = new QSqlQuery(new QPSQLResult(this, d));
          qry->exec("CREATE DATABASE " + db + " WITH ENCODING = 'UNICODE';");
          if (lastError().type() != QSqlError::None) {
            QMessageBox::critical(0, tr("Creación fallida"), tr("No se pudo crear la base de datos %1.").arg(db), QMessageBox::Ok, 0, 0);
            QMessageBox::critical(0, tr("Error"), QString(lastError().driverText()) + "\n" +
                                  QString(lastError().databaseText()), QMessageBox::Ok, 0, 0);
            delete qry;
            return false;
          } else {
            close();
            setOpenError(false);
            setLastError(QSqlError());
            if (!open(db, user, password, host, port, QString::null)) {
              QMessageBox::critical(0, tr("Conexión fallida"),
                                    tr("La base de datos %1 ha sido creada, pero no se pudo realizar la conexión.").arg(db), QMessageBox::Ok, 0, 0);
              QMessageBox::critical(0, tr("Error"), QString(lastError().driverText()) + "\n" +
                                    QString(lastError().databaseText()), QMessageBox::Ok, 0, 0);
              delete qry;
              return false;
            }
          }
          delete qry;
        }
      }

      if (lastError().type() != QSqlError::None) {
        QMessageBox::critical(0, tr("Conexión fallida"), tr("No se pudo conectar con la base de datos %1.").arg(db), QMessageBox::Ok, 0, 0);
        QMessageBox::critical(0, tr("Error"), QString(lastError().driverText()) + "\n" +
                              QString(lastError().databaseText()), QMessageBox::Ok, 0, 0);
        return false;
      }
    }
  }

  close();
  setOpenError(false);
  setLastError(QSqlError());
  return true;
}

QString QPSQLDriver::sqlCreateTable(FLTableMetaData *tmd)
{
#ifndef FL_QUICK_CLIENT
  if (!tmd)
    return QString::null;

  QString primaryKey(QString::null);
  QString sql = "CREATE TABLE " + tmd->name() + " (";
  QString seq;

  FLFieldMetaData *field;
  FLTableMetaData::FLFieldMetaDataList *fieldList = tmd->fieldList();

  unsigned int unlocks = 0;
  QDictIterator<FLFieldMetaData> it(*fieldList);
  while ((field = it.current()) != 0) {
    ++it;
    if (field->type() == FLFieldMetaData::Unlock)
      unlocks++;
  }

  if (unlocks > 1) {
#ifdef FL_DEBUG
    qWarning("FLManager : " + QApplication::tr("No se ha podido crear la tabla ") + tmd->name());
    qWarning("FLManager : " + QApplication::tr("Hay más de un campo tipo unlock. Solo puede haber uno."));
#endif
    return QString::null;
  }

  QDictIterator<FLFieldMetaData> it2(*fieldList);
  while ((field = it2.current()) != 0) {
    ++it2;
    sql += field->name();
    switch (field->type()) {
      case QVariant::Int:
        sql += " INT2";
        break;

      case QVariant::UInt:
        sql += " INT4";
        break;

      case QVariant::Bool:
      case FLFieldMetaData::Unlock:
        sql += " BOOLEAN";
        break;

      case QVariant::Double:
        sql += " FLOAT8";
        break;

      case QVariant::Time:
        sql += " TIME";
        break;

      case QVariant::Date:
        sql += " DATE";
        break;

      case QVariant::Pixmap:
        sql += " TEXT";
        break;

      case QVariant::String:
        sql += " VARCHAR";
        break;

      case QVariant::StringList:
        sql += " TEXT";
        break;

      case QVariant::ByteArray:
        sql += " BYTEA";
        break;

      case FLFieldMetaData::Serial: {
        seq = tmd->name() + "_" + field->name() + "_seq";
        QSqlQuery *q = new QSqlQuery(new QPSQLResult(this, d));
        q->setForwardOnly(true);
        q->exec("SELECT relname FROM pg_class WHERE relname='" + seq + "';");
        if (!q->next())
          q->exec("CREATE SEQUENCE " + seq + ";");
        sql += " INT4 DEFAULT NEXTVAL('" + seq + "')";
        delete q;
      }
      break;
    }

    int longitud = field->length();

    if (longitud > 0)
      sql += "(" + QString::number(longitud) + ")";

    if (field->isPrimaryKey()) {
      if (primaryKey.isEmpty()) {
        sql += " PRIMARY KEY";
        primaryKey = field->name();
      } else {
#ifdef FL_DEBUG
        qWarning(QApplication::tr("FLManager : Tabla-> ") + tmd->name() +
                 QApplication::tr(" . Se ha intentado poner una segunda clave primaria para el campo ") +
                 field->name() + QApplication::tr(" , pero el campo ") + primaryKey +
                 QApplication::tr(" ya es clave primaria. Sólo puede existir una clave primaria en FLTableMetaData, use FLCompoundKey para crear claves compuestas."));
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

  sql += ");";

  return sql;

#endif //FL_QUICK_CLIENT

  return QString::null;
}

QString QPSQLDriver::formatValueLike(int t, const QVariant &v, const bool upper)
{
  QString res("IS NULL");

  switch (t) {
    case QVariant::Bool: {
      QString s(v.toString().left(1).upper());
      if (s == QApplication::tr("Sí").left(1).upper())
        res = "='t'";
      else if (s == QApplication::tr("No").left(1).upper())
        res = "='f'";
    }
    break;
    case QVariant::Date:
      res = "::text LIKE '%%" + FLUtil::dateDMAtoAMD(v.toString()) + "'";
      break;
    case QVariant::Time: {
      QTime t(v.toTime());
      if (t.isValid() && !t.isNull())
        res = "::text LIKE '" + t.toString(Qt::ISODate) + "%%'";
    }
    break;
    default: {
      res = v.toString();
      res.replace(QChar('\''), "''");
      if (upper)
        res = "'" + res.upper();
      else
        res = "'" + res;
      if (protocol() < QPSQLDriver::Version82)
        res.replace("\\", "\\\\");
      else  {
        if (res.contains("\\")) {
          res.replace("\\", "\\\\");
          res.prepend('E');
        }
      }
      res = "::text LIKE " + res + "%%'";
      break;
    }
  }

  return res;
}

QString QPSQLDriver::formatValue(int t, const QVariant &v, const bool upper)
{
  QString res;

  switch (FLFieldMetaData::flDecodeType(t)) {
    case QVariant::Bool: {
      QString s(v.toString().left(1).upper());
      if (s == QApplication::tr("Sí").left(1).upper())
        res = "'t'";
      else if (s == QApplication::tr("No").left(1).upper())
        res = "'f'";
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
      if (upper)
        res = "'" + res.upper() + "'";
      else
        res = "'" + res + "'";
      if (protocol() < QPSQLDriver::Version82)
        res.replace("\\", "\\\\");
      else  {
        if (res.contains("\\")) {
          res.replace("\\", "\\\\");
          res.prepend('E');
        }
      }
      break;
    }

  }

  return res;
}

QVariant QPSQLDriver::nextSerialVal(const QString &table, const QString &field)
{
  QString seq = table + "_" + field + "_seq";
  QSqlQuery *query = new QSqlQuery(new QPSQLResult(this, d));
  query->setForwardOnly(true);
  query->exec("SELECT nextval('" + seq + "')");
  query->next();
  QVariant v = query->value(0);
  delete query;
  return v;
}

int QPSQLDriver::atFrom(FLSqlCursor *cur)
{
#ifndef FL_QUICK_CLIENT
  if (cur && cur->metadata() && !cur->metadata()->isQuery() && cur->sort().count() > 1) {
    d->checkLock = false;
    d->activeCreateIndex = true;
    createIndex(cur->sort().toStringList().join(",").replace(" ASC", "").replace(" DESC", ""),
                cur->metadata()->name(), false);
    d->activeCreateIndex = false;
    d->checkLock = true;
  }
#endif
  return -99;
}

bool QPSQLDriver::alterTable(const QString &mtd1, const QString &mtd2, const QString &key)
{
  return alterTable2(mtd1, mtd2, key);
}

bool QPSQLDriver::constraintExists(const QString &name) const
{
  QString sql = "SELECT constraint_name FROM ";
  sql += "information_schema.table_constraints where ";
  sql += "constraint_name='%1'";

  QSqlQuery q(QString::null, db_->dbAux());
  return q.exec(sql.arg(name)) && q.size() > 0;
}

bool QPSQLDriver::alterTable(FLTableMetaData *newMTD)
{
  FLTableMetaData *oldMTD = newMTD;
  QString renameOld(oldMTD->name().left(6) + "alteredtable" + QDateTime::currentDateTime().toString("ddhhssz"));
  FLTableMetaData::FLFieldMetaDataList *fieldList = oldMTD->fieldList();
  FLFieldMetaData *oldField = 0;

  db_->dbAux()->transaction();

  QSqlQuery q(QString::null, db_->dbAux());

  QString constraintName(oldMTD->name() + "_pkey");
  if (constraintExists(constraintName) &&
      !q.exec("ALTER TABLE " + oldMTD->name() + " DROP CONSTRAINT " + constraintName)) {
    db_->dbAux()->rollback();
    return false;
  }

  QDictIterator<FLFieldMetaData> it(*fieldList);
  while ((oldField = it.current()) != 0) {
    ++it;
    if (oldField->isUnique()) {
      constraintName = oldMTD->name() + "_" + oldField->name() + "_key";
      if (constraintExists(constraintName) &&
          !q.exec("ALTER TABLE " + oldMTD->name() + " DROP CONSTRAINT " + constraintName)) {
        db_->dbAux()->rollback();
        return false;
      }
    }
  }

  if (!q.exec("ALTER TABLE " + oldMTD->name() + " RENAME TO " + renameOld)) {
    db_->dbAux()->rollback();
    return false;
  }

  if (!db_->manager()->createTable(newMTD)) {
    db_->dbAux()->rollback();
    return false;
  }

  QSqlCursor oldCursor(renameOld, true, db_->dbAux());
  oldCursor.setMode(QSqlCursor::ReadOnly);
  oldCursor.select();

  fieldList = newMTD->fieldList();

  if (!fieldList || fieldList->isEmpty()) {
    db_->dbAux()->rollback();
    return false;
  }

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

  bool ok = true;
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
        if (v.isValid() && !v.isNull())
          v.cast(newBuffer->value(newField->name()).type());
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

  if (!ok) {
    db_->dbAux()->rollback();
    return false;
  } else {
    db_->dbAux()->commit();
    q.exec("DROP TABLE " + renameOld + " CASCADE");
  }

  return true;
}

bool QPSQLDriver::alterTable2(const QString &mtd1, const QString &mtd2, const QString &key, bool force)
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

  FLTableMetaData::FLFieldMetaDataList *fieldList = oldMTD->fieldList();
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

  QString renameOld = oldMTD->name().left(6) + "alteredtable" + QDateTime::currentDateTime().toString("ddhhssz");

  if (!db_->dbAux()) {
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

  QSqlQuery q(QString::null, db_->dbAux());
  QString constraintName(oldMTD->name() + "_pkey");

  if (constraintExists(constraintName) &&
      !q.exec("ALTER TABLE " + oldMTD->name() + " DROP CONSTRAINT " + constraintName)) {
#ifdef FL_DEBUG
    qWarning("FLManager : " + QApplication::tr("En método alterTable, no se ha podido borrar el índice %1_pkey de la tabla antigua.").arg(oldMTD->name()));
#endif

    db_->dbAux()->rollback();
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  QDictIterator<FLFieldMetaData> it(*fieldList);
  while ((oldField = it.current()) != 0) {
    ++it;
    if (oldField->isUnique()) {
      constraintName = oldMTD->name() + "_" + oldField->name() + "_key";
      if (constraintExists(constraintName) &&
          !q.exec("ALTER TABLE " + oldMTD->name() + " DROP CONSTRAINT " + constraintName)) {
#ifdef FL_DEBUG
        qWarning("FLManager : " +
                 QApplication::tr("En método alterTable, no se ha podido borrar el índice %1_%2_key de la tabla antigua.")
                 .arg(oldMTD->name(), oldField->name()));
#endif

        db_->dbAux()->rollback();
        if ((oldMTD != newMTD) && oldMTD)
          delete oldMTD;
        if (newMTD)
          delete newMTD;
        return false;
      }
    }
  }

  if (!q.exec("ALTER TABLE " + oldMTD->name() + " RENAME TO " + renameOld)) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("No se ha podido renombrar la tabla antigua."));
#endif

    db_->dbAux()->rollback();
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  if (!db_->manager()->createTable(newMTD)) {
    db_->dbAux()->rollback();
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  QSqlCursor oldCursor(renameOld, true, db_->dbAux());
  oldCursor.setMode(QSqlCursor::ReadOnly);
  oldCursor.select();
  int totalSteps = oldCursor.size();
  QProgressDialog progress(QApplication::tr("Reestructurando registros para %1...").arg(newMTD->alias()), 0, totalSteps, qApp->focusWidget(), 0, true);
  progress.setCaption(QApplication::tr("Tabla modificada"));

  fieldList = newMTD->fieldList();

  if (!fieldList) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("Los nuevos metadatos no tienen campos."));
#endif
    db_->dbAux()->rollback();
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
    db_->dbAux()->rollback();
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

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
  bool ok = true;
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

  if ((oldMTD != newMTD) && oldMTD)
    delete oldMTD;

#if 0
  //#ifndef FL_QUICK_CLIENT
  d->checkLock = false;
  if (dictIndexes) {
    delete dictIndexes;
    dictIndexes = 0;
  }
  QStringList fieldsNames = QStringList::split(",", newMTD->fieldsNames());
  for (QStringList::Iterator it = fieldsNames.begin(); it != fieldsNames.end(); ++it) {
    FLFieldMetaData *field = newMTD->field((*it));
    if (field->relationM1() || field->isPrimaryKey())
      createIndex(field->name(), newMTD->name(), FLFieldMetaData::flDecodeType(field->type()) == QVariant::String,
                  FLFieldMetaData::flDecodeType(field->type()) != QVariant::String);
    else if (field->type() == QVariant::Date)
      createIndex(field->name(), newMTD->name(), false , true);
  }
  d->checkLock = true;
#endif

  if (newMTD)
    delete newMTD;

  if (ok)
    db_->dbAux()->commit();
  else {
    db_->dbAux()->rollback();
    return false;
  }

  if (force && ok)
    q.exec("DROP TABLE " + renameOld + " CASCADE");
#else

  return true;
#endif //FL_QUICK_CLIENT
}

int QPSQLDriver::insertMulti(const QString &tablename, QPtrList<QSqlRecord> * records)
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
    QMessageBox::critical(0, "Error",
                          QString(qry.lastError().driverText()) + "\n" +
                          QString(qry.lastError().databaseText()), QMessageBox::Ok, 0, 0);
    return 0;
  }

  return rows;
}

void QPSQLDriver::close()
{
  if (isOpen()) {
#ifdef FL_DEBUG
    if (d->dictPendCursors) {
      QDictIterator < QString > it(*d->dictPendCursors);
      while (it.current()) {
        qWarning("CURSOR PENDIENTE : " + it.currentKey() + " -- " + * (it.current()));
        ++it;
      }
      d->dictPendCursors->clear();
    }
#endif

    PQfinish(d->connection);
    d->connection = 0;
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

QSqlQuery QPSQLDriver::createQuery() const
{
  return QSqlQuery(new QPSQLResult(this, d));
}

bool QPSQLDriver::beginTransaction()
{
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QPSQLDriver::beginTransaction: Database not open");
#endif

    return false;
  }
  PGresult *res = PQexec(d->connection, "BEGIN");
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    setLastError(qMakeError(QApplication::tr("No se pudo iniciar transacción"), QSqlError::Transaction, d));
    return false;
  }
  PQclear(res);
  return true;
}

bool QPSQLDriver::commitTransaction()
{
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QPSQLDriver::commitTransaction: Database not open");
#endif

    return false;
  }
  PGresult *res = PQexec(d->connection, "COMMIT");
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    setLastError(qMakeError(QApplication::tr("No se pudo aceptar la transacción"), QSqlError::Transaction, d));
    return false;
  }
  PQclear(res);
  return true;
}

bool QPSQLDriver::rollbackTransaction()
{
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QPSQLDriver::rollbackTransaction: Database not open");
#endif

    return false;
  }
  PGresult *res = PQexec(d->connection, "ROLLBACK");
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
    setLastError(qMakeError(QApplication::tr("No se pudo deshacer la transacción"), QSqlError::Transaction, d));
    PQclear(res);
    return false;
  }
  PQclear(res);
  return true;
}

QStringList QPSQLDriver::tables(const QString &typeName) const
{
  QStringList tl;
  if (!isOpen())
    return tl;
  int type = typeName.toInt();
  QSqlQuery *t = new QSqlQuery(new QPSQLResult(this, d));
  t->setForwardOnly(true);

  if (typeName.isEmpty() || ((type & (int) QSql::Tables) == (int) QSql::Tables)) {
    t->exec("select relname from pg_class where ( relkind = 'r' ) "
            "and ( relname !~ '^Inv' ) " "and ( relname !~ '^pg_' ) ");
    while (t->next())
      tl.append(t->value(0).toString());
  }
  if ((type & (int) QSql::Views) == (int) QSql::Views) {
    t->exec("select relname from pg_class where ( relkind = 'v' ) "
            "and ( relname !~ '^Inv' ) " "and ( relname !~ '^pg_' ) ");
    while (t->next())
      tl.append(t->value(0).toString());
  }
  if ((type & (int) QSql::SystemTables) == (int) QSql::SystemTables) {
    t->exec("select relname from pg_class where ( relkind = 'r' ) "
            "and ( relname like 'pg_%' ) ");
    while (t->next())
      tl.append(t->value(0).toString());
  }

  delete t;
  return tl;
}

QSqlIndex QPSQLDriver::primaryIndex2(const QString &tablename) const
{
  QSqlIndex idx(tablename);
  if (!isOpen())
    return idx;
  QSqlQuery *i = new QSqlQuery(new QPSQLResult(this, d));
  i->setForwardOnly(true);
  QString stmt;

  switch (pro) {
    case QPSQLDriver::Version6:
      stmt = "select pg_att1.attname, int(pg_att1.atttypid), pg_att2.attnum, pg_cl.relname "
             "from pg_attribute pg_att1, pg_attribute pg_att2, pg_class pg_cl, pg_index pg_ind "
             "where lower(pg_cl.relname) = '%1_pkey' ";
      break;
    case QPSQLDriver::Version7:
    case QPSQLDriver::Version71:
      stmt = "select pg_att1.attname, pg_att1.atttypid::int, pg_cl.relname "
             "from pg_attribute pg_att1, pg_attribute pg_att2, pg_class pg_cl, pg_index pg_ind "
             "where lower(pg_cl.relname) = '%1_pkey' ";
      break;
    case QPSQLDriver::Version73:
    case QPSQLDriver::Version74:
    case QPSQLDriver::Version8:
    case QPSQLDriver::Version81:
    case QPSQLDriver::Version82:
    case QPSQLDriver::Version83:
    case QPSQLDriver::Version84:
    case QPSQLDriver::Version9:
    case QPSQLDriver::Version91:
      stmt = "select pg_att1.attname, pg_att1.atttypid::int, pg_cl.relname "
             "from pg_attribute pg_att1, pg_attribute pg_att2, pg_class pg_cl, pg_index pg_ind "
             "where lower(pg_cl.relname) = '%1_pkey' "
             "and pg_att1.attisdropped = false ";
      break;
  }
  stmt += "and pg_cl.oid = pg_ind.indexrelid "
          "and pg_att2.attrelid = pg_ind.indexrelid "
          "and pg_att1.attrelid = pg_ind.indrelid "
          "and pg_att1.attnum = pg_ind.indkey[pg_att2.attnum-1] "
          "order by pg_att2.attnum";

  i->exec(stmt.arg(tablename.lower()));
  while (i->isActive() && i->next()) {
    QSqlField f(i->value(0).toString(), qDecodePSQLType(i->value(1).toInt()));
    idx.append(f);
    idx.setName(i->value(2).toString());
  }
  delete i;
  return idx;
}

QSqlIndex QPSQLDriver::primaryIndex(const QString &tablename) const
{
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
  FLTableMetaData *mtd = db_->manager()->metadata(&docElem, true);
  if (!mtd)
    return primaryIndex2(tablename);
  idx.append(QSqlField(mtd->primaryKey(), FLFieldMetaData::flDecodeType(mtd->fieldType(mtd->primaryKey()))));
  idx.setName(tablename.lower() + "_pkey");

  delete mtd;
  return idx;
}

QSqlRecord QPSQLDriver::record2(const QString &tablename) const
{
  QSqlRecord fil;
  if (!isOpen())
    return fil;
  QString stmt;
  switch (pro) {
    case QPSQLDriver::Version6:
      stmt = "select pg_attribute.attname, int(pg_attribute.atttypid) "
             "from pg_class, pg_attribute "
             "where lower(pg_class.relname) = '%1' "
             "and pg_attribute.attnum > 0 "
             "and pg_attribute.attrelid = pg_class.oid ";
      break;
    case QPSQLDriver::Version7:
    case QPSQLDriver::Version71:
      stmt = "select pg_attribute.attname, pg_attribute.atttypid::int "
             "from pg_class, pg_attribute "
             "where lower(pg_class.relname) = '%1' "
             "and pg_attribute.attnum > 0 "
             "and pg_attribute.attrelid = pg_class.oid ";
      break;
    case QPSQLDriver::Version73:
    case QPSQLDriver::Version74:
    case QPSQLDriver::Version8:
    case QPSQLDriver::Version81:
    case QPSQLDriver::Version82:
    case QPSQLDriver::Version83:
    case QPSQLDriver::Version84:
    case QPSQLDriver::Version9:
    case QPSQLDriver::Version91:
      stmt = "select pg_attribute.attname, pg_attribute.atttypid::int "
             "from pg_class, pg_attribute "
             "where lower(pg_class.relname) = '%1' "
             "and pg_attribute.attnum > 0 "
             "and pg_attribute.attisdropped = false "
             "and pg_attribute.attrelid = pg_class.oid ";
      break;
  }

  QSqlQuery *fi = new QSqlQuery(new QPSQLResult(this, d));
  fi->setForwardOnly(true);
  fi->exec(stmt.arg(tablename.lower()));
  while (fi->next()) {
    QSqlField f(fi->value(0).toString(), qDecodePSQLType(fi->value(1).toInt()));
    fil.append(f);
  }
  delete fi;
  return fil;
}

QSqlRecord QPSQLDriver::record(FLTableMetaData *mtd) const
{
  QSqlRecord fil;
  FLTableMetaData::FLFieldMetaDataList *fl = mtd->fieldList();

  if (!fl || fl->isEmpty())
    return record2(mtd->name());

  QStringList fieldsNames = QStringList::split(",", mtd->fieldsNames());
  for (QStringList::Iterator it = fieldsNames.begin(); it != fieldsNames.end(); ++it)
    fil.append(QSqlField((*it), FLFieldMetaData::flDecodeType(mtd->fieldType((*it)))));

  return fil;
}

QSqlRecord QPSQLDriver::record(const QString &tablename) const
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

  FLTableMetaData::FLFieldMetaDataList *fl = mtd->fieldList();
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

QSqlRecord QPSQLDriver::record(const QSqlQuery &query) const
{
  QSqlRecord fil;
  if (!isOpen())
    return fil;
  if (query.isActive() && query.driver() == this) {
    QPSQLResult *result = (QPSQLResult *) query.result();
    int count = PQnfields(result->d->result);
    for (int i = 0; i < count; ++i) {
      QString name = PQfname(result->d->result, i);
      QVariant::Type type = qDecodePSQLType(PQftype(result->d->result, i));
      QSqlField rf(name, type);
      fil.append(rf);
    }
  }
  return fil;
}

QSqlRecordInfo QPSQLDriver::recordInfo2(const QString &tablename) const
{
  QSqlRecordInfo info;
  if (!isOpen())
    return info;

  QString stmt;
  switch (pro) {
    case QPSQLDriver::Version6:
      stmt = "select pg_attribute.attname, int(pg_attribute.atttypid), pg_attribute.attnotnull, "
             "pg_attribute.attlen, pg_attribute.atttypmod, int(pg_attribute.attrelid), pg_attribute.attnum "
             "from pg_class, pg_attribute "
             "where lower(pg_class.relname) = '%1' "
             "and pg_attribute.attnum > 0 "
             "and pg_attribute.attrelid = pg_class.oid ";
      break;
    case QPSQLDriver::Version7:
      stmt = "select pg_attribute.attname, pg_attribute.atttypid::int, pg_attribute.attnotnull, "
             "pg_attribute.attlen, pg_attribute.atttypmod, pg_attribute.attrelid::int, pg_attribute.attnum "
             "from pg_class, pg_attribute "
             "where lower(pg_class.relname) = '%1' "
             "and pg_attribute.attnum > 0 "
             "and pg_attribute.attrelid = pg_class.oid ";
      break;
    case QPSQLDriver::Version71:
      stmt = "select pg_attribute.attname, pg_attribute.atttypid::int, pg_attribute.attnotnull, "
             "pg_attribute.attlen, pg_attribute.atttypmod, pg_attrdef.adsrc "
             "from pg_class, pg_attribute "
             "left join pg_attrdef on (pg_attrdef.adrelid = pg_attribute.attrelid and pg_attrdef.adnum = pg_attribute.attnum) "
             "where lower(pg_class.relname) = '%1' "
             "and pg_attribute.attnum > 0 "
             "and pg_attribute.attrelid = pg_class.oid "
             "order by pg_attribute.attnum ";
      break;
    case QPSQLDriver::Version73:
    case QPSQLDriver::Version74:
    case QPSQLDriver::Version8:
    case QPSQLDriver::Version81:
    case QPSQLDriver::Version82:
    case QPSQLDriver::Version83:
    case QPSQLDriver::Version84:
    case QPSQLDriver::Version9:
    case QPSQLDriver::Version91:
      stmt = "select pg_attribute.attname, pg_attribute.atttypid::int, pg_attribute.attnotnull, "
             "pg_attribute.attlen, pg_attribute.atttypmod, pg_attrdef.adsrc "
             "from pg_class, pg_attribute "
             "left join pg_attrdef on (pg_attrdef.adrelid = pg_attribute.attrelid and pg_attrdef.adnum = pg_attribute.attnum) "
             "where lower(pg_class.relname) = '%1' "
             "and pg_attribute.attnum > 0 "
             "and pg_attribute.attrelid = pg_class.oid "
             "and pg_attribute.attisdropped = false "
             "order by pg_attribute.attnum ";
      break;
  }

  QSqlQuery *query = new QSqlQuery(new QPSQLResult(this, d));
  query->setForwardOnly(true);
  query->exec(stmt.arg(tablename.lower()));
  if (pro >= QPSQLDriver::Version71) {
    while (query->next()) {
      int len = query->value(3).toInt();
      int precision = query->value(4).toInt();
      // swap length and precision if length == -1
      if (len == -1 && precision > -1) {
        len = precision - 4;
        precision = -1;
      }
      QString defVal = query->value(5).toString();
      if (!defVal.isEmpty() && defVal.startsWith("'"))
        defVal = defVal.mid(1, defVal.length() - 2);
      info.append(QSqlFieldInfo(query->value(0).toString(),
                                qDecodePSQLType(query->value(1).toInt()),
                                query->value(2).toBool(),
                                len,
                                precision,
                                defVal,
                                query->value(1).toInt()));
    }
  } else {
    // Postgres < 7.1 cannot handle outer joins
    while (query->next()) {
      QString defVal;
      QString stmt2 = "select pg_attrdef.adsrc from pg_attrdef where "
                      "pg_attrdef.adrelid = %1 and pg_attrdef.adnum = %2 ";
      QSqlQuery *query2 = new QSqlQuery(new QPSQLResult(this, d));
      query2->setForwardOnly(true);
      query2->exec(stmt2.arg(query->value(5).toInt()).arg(query->value(6).toInt()));
      if (query2->isActive() && query2->next())
        defVal = query2->value(0).toString();
      if (!defVal.isEmpty() && defVal.startsWith("'"))
        defVal = defVal.mid(1, defVal.length() - 2);
      int len = query->value(3).toInt();
      int precision = query->value(4).toInt();
      if (len == -1 && precision > -1) {
        len = precision - 4;
        precision = -1;
      }
      info.append(QSqlFieldInfo(query->value(0).toString(),
                                qDecodePSQLType(query->value(1).toInt()),
                                query->value(2).toBool(),
                                len,
                                precision,
                                defVal,
                                query->value(1).toInt()));
      delete query2;
    }
  }

  delete query;
  return info;
}

QSqlRecordInfo QPSQLDriver::recordInfo(const QString &tablename) const
{
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
  FLTableMetaData *mtd = db_->manager()->metadata(&docElem, true);
  if (!mtd)
    return recordInfo2(tablename);
  FLTableMetaData::FLFieldMetaDataList *fl = mtd->fieldList();
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
    info.append(QSqlFieldInfo(field->name(), FLFieldMetaData::flDecodeType(field->type())));
#ifndef FL_QUICK_CLIENT
    if (field->relationM1() || field->isPrimaryKey())
      createIndex(field->name(), tablename, FLFieldMetaData::flDecodeType(field->type()) == QVariant::String,
                  FLFieldMetaData::flDecodeType(field->type()) != QVariant::String);
    else if (field->type() == QVariant::Date)
      createIndex(field->name(), tablename, false , true);
#endif
  }

  delete mtd;
  return info;
}

QSqlRecordInfo QPSQLDriver::recordInfo(const QSqlQuery &query) const
{
  QSqlRecordInfo info;
  if (!isOpen())
    return info;
  if (query.isActive() && query.driver() == this) {
    QPSQLResult *result = (QPSQLResult *) query.result();
    int count = PQnfields(result->d->result);
    for (int i = 0; i < count; ++i) {
      QString name = PQfname(result->d->result, i);
      int len = PQfsize(result->d->result, i);
      int precision = PQfmod(result->d->result, i);
      // swap length and precision if length == -1
      if (len == -1 && precision > -1) {
        len = precision - 4;
        precision = -1;
      }
      info.append(QSqlFieldInfo(name,
                                qDecodePSQLType(PQftype
                                                (result->d->result, i)),
                                -1, len, precision, QVariant(),
                                PQftype(result->d->result, i)));
    }
  }
  return info;
}

QString QPSQLDriver::formatValue(const QSqlField *field, bool) const
{
  QString r;
  if (field->isNull()) {
    r = nullText();
  } else {
    switch (field->type()) {
      case QVariant::DateTime:
        if (field->value().toDateTime().isValid()) {
          QDate dt = field->value().toDateTime().date();
          QTime tm = field->value().toDateTime().time();
          r = "'" + QString::number(dt.year()) + "-" +
              QString::number(dt.month()) + "-" +
              QString::number(dt.day()) + " " +
              tm.toString() + "." +
              QString::number(tm.msec()).rightJustify(3, '0') + "'";
        } else {
          r = nullText();
        }
        break;
      case QVariant::Time:
        if (field->value().toTime().isValid()) {
          r = field->value().toTime().toString(Qt::ISODate);
        } else {
          r = nullText();
        }
      case QVariant::String:
      case QVariant::CString: {
        switch (field->value().type()) {
          case QVariant::Rect: {
            QRect rec = field->value().toRect();
            r = "'(" + QString::number(rec.right()) +
                "," + QString::number(rec.bottom()) +
                "),(" + QString::number(rec.left()) +
                "," + QString::number(rec.top()) + ")'";
            break;
          }
          case QVariant::Point: {
            QPoint p = field->value().toPoint();
            r = "'(" + QString::number(p.x()) +
                "," + QString::number(p.y()) + ")'";
            break;
          }
          case QVariant::PointArray: {
            QPointArray pa = field->value().toPointArray();
            r = "' ";
            for (int i = 0; i < (int) pa.size(); ++i) {
              r += "(" + QString::number(pa[ i ].x()) +
                   "," + QString::number(pa[ i ].y()) + "),";
            }
            r.truncate(r.length() - 1);
            r += "'";
            break;
          }
          default:
            r = QSqlDriver::formatValue(field);
            if (protocol() < QPSQLDriver::Version82)
              r.replace("\\", "\\\\");
            else  {
              if (r.contains("\\")) {
                r.replace("\\", "\\\\");
                r.prepend('E');
              }
            }
            break;
        }
        break;
      }
      case QVariant::Bool:
        if (field->value().toBool())
          r = "true";
        else
          r = "false";
        break;
      case QVariant::ByteArray: {
        QByteArray ba(field->value().toByteArray());
        size_t len;
        unsigned char *data = PQescapeBytea((unsigned char *)(const char *)ba, ba.size(), &len);
        r += QChar('\'');
        r += QString::fromLatin1((const char *) data);
        r += QChar('\'');
        if (protocol() >= QPSQLDriver::Version82 &&
            protocol() < QPSQLDriver::Version9)
          r.prepend('E');
        qPQfreemem(data);
        break;
      }
      default:
        r = QSqlDriver::formatValue(field);
        break;
    }
  }
  return r;
}

#ifndef FL_QUICK_CLIENT
void QPSQLDriver::createIndex(const QString &fieldName, const QString &tableName, bool textOp, bool noUpper) const
{
  if (!d->activeCreateIndex || !isOpen() || fieldName.isEmpty() || tableName.isEmpty())
    return;

  if (tableName.contains("alteredtable"))
    return;

  if (!dictIndexes) {
    dictIndexes = new QDict < bool > (277);
    dictIndexes->setAutoDelete(true);

    QSqlQuery *idxs = new QSqlQuery(new QPSQLResult(this, d));
    idxs->setForwardOnly(true);
    idxs->exec("select indexname from pg_indexes where indexname like '%_m1_idx' and tablename not like '%%alteredtable%'");
    while (idxs->next())
      dictIndexes->insert(idxs->value(0).toString(), new bool(true));
    delete idxs;
  }

  QString indexName(tableName.left(25) + "_" + fieldName.left(25) + "_m1_idx");
  indexName.replace(" ", "").replace(",", "");

  if (dictIndexes->find(indexName))
    return;

  if (d->checkLock) {
    QSqlQuery *idxs = new QSqlQuery(new QPSQLResult(this, d));
    idxs->setForwardOnly(true);
    idxs->exec("select pg_class.relname,pg_locks.* from pg_class,pg_locks where "
               "pg_class.relfilenode=pg_locks.relation and pg_locks.mode like '%Exclusive%' "
               "and pg_class.relname='" + tableName + "'");
    if (idxs->next()) {
#ifdef FL_DEBUG
      qWarning("No se puede crear índice, tabla bloqueada : " + tableName);
#endif
      delete idxs;
      return;
    } else
      delete idxs;
  }

  QString indexNameUp = tableName.left(25) + "_" + fieldName.left(25) + "up_m1_idx";
  indexNameUp.replace(" ", "").replace(",", "");

  QString suffixOp((textOp ? " ( %1 text_pattern_ops )" : " ( %1 )"));
  QString suffixOpUp((textOp ? " ( upper(%1) text_pattern_ops )" : " ( upper(%1) )"));

  PGconn *conn = d->connection;
  PGresult *result = PQexec(conn, "select relname from pg_class where relname = '" + tableName + "'");
  int status = PQresultStatus(result);
  if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) {
    int size = PQntuples(result);
    PQclear(result);
    if (size) {
      result = PQexec(conn, "select indexname from pg_indexes where indexname = '" + indexName + "'");
      status = PQresultStatus(result);
      if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) {
        size = PQntuples(result);
        PQclear(result);
        if (!size) {
          result = PQexec(conn, "create index " + indexName + " on " + tableName + suffixOp.arg(fieldName));
          status = PQresultStatus(result);
          if (status == PGRES_COMMAND_OK) {
            PQclear(result);
            dictIndexes->replace(indexName, new bool(true));
          } else {
            PQclear(result);
            qWarning("ERROR : " + QString(PQresultErrorMessage(d->result)) + "\n");
            qWarning("create index " + indexName + " on " + tableName + suffixOp.arg(fieldName));
          }
        }
      } else
        PQclear(result);

      if (!fieldName.contains(",") && !noUpper) {
        result = PQexec(conn, "select indexname from pg_indexes where indexname = '" + indexNameUp + "'");
        status = PQresultStatus(result);
        if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) {
          size = PQntuples(result);
          PQclear(result);
          if (!size) {
            result = PQexec(conn, "create index " + indexNameUp + " on " + tableName + suffixOpUp.arg(fieldName));
            status = PQresultStatus(result);
            if (status == PGRES_COMMAND_OK) {
              PQclear(result);
              dictIndexes->replace(indexNameUp, new bool(true));
            } else {
              PQclear(result);
              qWarning("ERROR : " + QString(PQresultErrorMessage(d->result)) + "\n");
              qWarning("create index " + indexNameUp + " on " + tableName + suffixOpUp.arg(fieldName));
            }
          }
        } else
          PQclear(result);
      }
    }
  } else
    PQclear(result);
}
#endif

bool QPSQLDriver::canSavePoint()
{
  return (protocol() >= QPSQLDriver::Version8);
}

bool QPSQLDriver::savePoint(const QString &n)
{
  if (!canSavePoint())
    return false;
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QPSQLDriver::savePoint: Database not open");
#endif
    return false;
  }
  PGresult *res = PQexec(d->connection, "savepoint sv_" + n);
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    setLastError(qMakeError(QApplication::tr("No se pudo crear punto de salvaguarda"), QSqlError::Transaction, d));
    return false;
  }
  PQclear(res);
  return true;
}

bool QPSQLDriver::releaseSavePoint(const QString &n)
{
  if (!canSavePoint())
    return false;
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QPSQLDriver::releaseSavePoint: Database not open");
#endif
    return false;
  }
  PGresult *res = PQexec(d->connection, "release savepoint sv_" + n);
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    setLastError(qMakeError(QApplication::tr("No se pudo liberar punto de salvaguarda"), QSqlError::Transaction, d));
    return false;
  }
  PQclear(res);
  return true;
}

bool QPSQLDriver::rollbackSavePoint(const QString &n)
{
  if (!canSavePoint())
    return false;
  if (!isOpen()) {
#ifdef QT_CHECK_RANGE
    qWarning("QPSQLDriver::rollbackSavePoint: Database not open");
#endif
    return false;
  }
  PGresult *res = PQexec(d->connection, "rollback to savepoint sv_" + n);
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    setLastError(qMakeError(QApplication::tr("No se pudo deshacer punto de salvaguarda"), QSqlError::Transaction, d));
    return false;
  }
  PQclear(res);
  return true;
}

void QPSQLDriver::Mr_Proper()
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

  QRegExp rx("^.*\\d{6,9}$");
  QStringList listOldBks(tables("").grep(rx));

  qry.exec("select nombre from flfiles where nombre similar to"
           "'%[[:digit:]][[:digit:]][[:digit:]][[:digit:]]-[[:digit:]][[:digit:]]%:[[:digit:]][[:digit:]]%' or nombre similar to"
           "'%alteredtable[[:digit:]][[:digit:]][[:digit:]][[:digit:]]%' or (bloqueo='f' and nombre like '%.mtd')");
  FLUtil::createProgressDialog(tr("Borrando backups"), listOldBks.size() + qry.size() + 2);
  while (qry.next()) {
    item = qry.value(0).toString();
    FLUtil::setLabelText(tr("Borrando regisro %1").arg(item));
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

#ifndef FL_QUICK_CLIENT
  steps = 0;
  qry.exec("select tablename,indexname from pg_indexes where indexname like '%_m1_idx'");
  FLUtil::createProgressDialog(tr("Regenerando índices"), qry.size());
  if (dictIndexes) {
    delete dictIndexes;
    dictIndexes = 0;
  }
  QString tableIdx, fieldIdx;
  FLTableMetaData *mtdIdx = 0;
  while (qry.next()) {
    tableIdx = qry.value(0).toString();
    fieldIdx = qry.value(1).toString();
    fieldIdx.remove(tableIdx + "_");
    if (fieldIdx.contains("up_m1_idx"))
      fieldIdx.remove("up_m1_idx");
    else
      fieldIdx.remove("_m1_idx");
    FLUtil::setLabelText(tr("Regenerando índice %1.%2").arg(tableIdx).arg(fieldIdx));
#ifdef FL_DEBUG
    qWarning(tr("Regenerando índice %1.%2").arg(tableIdx).arg(fieldIdx));
#endif
    FLUtil::setProgress(++steps);
    mtdIdx = db_->manager()->metadata(tableIdx);
    if (!mtdIdx)
      continue;
    if (!mtdIdx->field(fieldIdx))
      continue;
    qry2.exec("drop index " + qry.value(1).toString());
  }
  FLUtil::destroyProgressDialog();
#endif

  steps = 0;
  qry.exec("select tablename from pg_tables where schemaname='public'");
  FLUtil::createProgressDialog(tr("Vacunando base de datos"), qry.size());
  while (qry.next()) {
    item = qry.value(0).toString();
    FLUtil::setLabelText(tr("Vacunando tabla %1").arg(item));
    qry2.exec("vacuum " + item);
#ifdef FL_DEBUG
    qWarning("vacuum " + item);
#endif
    QSqlRecord recMtd = record(item);
    QSqlRecord recBd = record2(item);
    QSqlField *fieldMtd = 0;
    QSqlField *fieldBd = 0;
    bool mustAlter = false;
    for (int i = 0; i < recMtd.count(); ++i) {
      fieldMtd = recMtd.field(i);
      fieldBd = recBd.field(fieldMtd->name());
      if (fieldBd) {
        if (!fieldBd->value().canCast(FLFieldMetaData::flDecodeType(fieldMtd->type())) ||
            fieldBd->isNull() != fieldMtd->isNull() || fieldBd->isReadOnly() != fieldMtd->isReadOnly()) {
#ifdef FL_DEBUG
          qWarning("!= name " + fieldMtd->name() + " " + fieldBd->name());
          qWarning("!= type " + QString::number(FLFieldMetaData::flDecodeType(fieldMtd->type())) + " " +
                   QString::number(fieldBd->type()));
          qWarning("!= null " + QString::number(fieldMtd->isNull()) + " " + QString::number(fieldBd->isNull()));
          qWarning("!= readOnly " + QString::number(fieldMtd->isReadOnly()) + " " + QString::number(fieldBd->isReadOnly()));
#endif
          mustAlter = true;
          break;
        }
      } else {
#ifdef FL_DEBUG
        qWarning("No fieldBd " +  fieldMtd->name());
#endif
        mustAlter = true;
        break;
      }
    }

#if 0
    for (int i = 0; i < recBd.count(); ++i) {
      fieldBd = recBd.field(i);
      fieldMtd = recMtd.field(fieldBd->name());
      if (!fieldMtd) {
#ifdef FL_DEBUG
        qWarning("No fieldMtd " +  fieldBd->name());
#endif
        mustAlter = true;
        break;
      }
    }
#endif

    if (mustAlter) {
#ifdef FL_DEBUG
      qWarning("mustAlter " + item);
#endif
      QString conte = db_->managerModules()->content(item + ".mtd");
      alterTable2(conte, conte, QString::null, true);
    }
    FLUtil::setProgress(++steps);
  }

  db_->dbAux()->transaction();
  d->activeCreateIndex = true;
  d->checkLock = false;
  steps = 0;
  QSqlCursor sqlCursor(QString::null, true, db_->dbAux());
  QSqlQuery sqlQuery(QString::null, db_->dbAux());
  sqlQuery.setForwardOnly(true);
  if (sqlQuery.exec("select relname from pg_class where ( relkind = 'r' ) "
                    "and ( relname !~ '^Inv' ) " "and ( relname !~ '^pg_' ) and ( relname !~ '^sql_' )")) {
    FLUtil::setTotalSteps(sqlQuery.size());
    while (sqlQuery.next()) {
      item = sqlQuery.value(0).toString();
      FLUtil::setProgress(++steps);
      FLUtil::setLabelText(tr("Creando índices para %1").arg(item));

      FLTableMetaData *mtd = db_->manager()->metadata(item);
      FLTableMetaData::FLFieldMetaDataList *fl;
      if (!mtd || !(fl = mtd->fieldList()))
        continue;
      QDictIterator<FLFieldMetaData> it(*fl);
      for (; it.current(); ++it) {
        if (!(*it) || (*it)->type() != QVariant::Pixmap)
          continue;
        QString v;
        QSqlCursor cur(item, true, db_->dbAux());
        QSqlRecord *buf;
        cur.select();
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

      sqlCursor.setName(item, true);
    }
  }
  d->checkLock = true;
  d->activeCreateIndex = false;
  db_->dbAux()->commit();

  FLUtil::destroyProgressDialog();
}

QStringList QPSQLDriver::rowsLockeds(const QString &table, const QString &primaryKeyValue)
{
  QSqlIndex pri(primaryIndex2(table));
  if (pri.isEmpty())
    return QStringList();

  QStringList rows, ret;
  QString item, pk(pri.fieldName(0));
  QSqlQuery qry("select " + pk + ",xmax from " + table, db_->dbAux());
  bool skipPk = primaryKeyValue.isEmpty();

  while (qry.next()) {
    item = qry.value(1).toString();
    if (!item.isEmpty() && item != "0") {
      if (skipPk) {
        item += "@##" + pk + "=" + qry.value(0).toString();
        rows.append(item);
      } else if (primaryKeyValue == qry.value(0).toString()) {
        item += "@##" + pk + "=" + qry.value(0).toString();
        rows.append(item);
      }
    }
  }

  if (!rows.isEmpty()) {
    QString sql(pro >= QPSQLDriver::Version83
                ?
                QString("select transactionid from pg_locks where locktype='transactionid' and "
                        "virtualtransaction in (select pg_locks.virtualtransaction from pg_class,pg_locks where "
                        "pg_class.relkind='r' and pg_class.relfilenode=pg_locks.relation "
                        "and pg_locks.mode='RowExclusiveLock' and pg_class.relname='" + table + "')")
                :
                QString("select pg_locks.transaction from pg_class,pg_locks where "
                        "pg_class.relkind='r' and pg_class.relfilenode=pg_locks.relation "
                        "and pg_locks.mode='RowExclusiveLock' and pg_class.relname='" + table + "'"));

    qry.exec(sql);

    while (qry.next()) {
      item = qry.value(0).toString();
      for (QStringList::Iterator it = rows.begin(); it != rows.end(); ++it) {
        if ((*it).section('@', 0, 0) > item)
          ret.append((*it).section('@', 1, 1));
      }
    }
  }

  return ret;
}

QStringList QPSQLDriver::locksStatus()
{
  QString sql("select pg_class.relname,pg_stat_activity.*,pg_locks.* from "
              "pg_class,pg_stat_activity,pg_locks where pg_class.relkind='r' "
              "and pg_class.relfilenode=pg_locks.relation and pg_stat_activity.procpid=pg_locks.pid");

  QSqlSelectCursor qry(sql, db_->dbAux());
  QStringList ret;

  if (qry.next() && qry.count()) {
    QStringList rowsLock;
    QString item;

    do {
      item = qry.value(0).toString();
      rowsLock = rowsLockeds(item);
      if (!rowsLock.isEmpty()) {
        for (int i = 1; i < qry.count(); ++i)
          item += "@" + qry.value(i).toString();
        ret.append(item);
        ret += rowsLock;
      }
    } while (qry.next());

    if (!ret.isEmpty()) {
      item = qry.fieldName(0);
      for (int i = 1; i < qry.count(); ++i)
        item += "@" + qry.fieldName(i);
      ret.prepend(item);
    }
  }

  return ret;
}

QStringList QPSQLDriver::detectLocks()
{
  QString fieldTransactionName(pro >= QPSQLDriver::Version83 ? "virtualtransaction" : "transaction");
  QString sql("select pg_locks.%1,pg_locks.pid from pg_stat_activity,pg_locks where "
              "pg_stat_activity.procpid=pg_locks.pid and pg_locks.mode='ExclusiveLock' "
              "and pg_locks.locktype='tuple' and pg_locks.pid<>'%2' and pg_stat_activity.waiting='t'");
  QSqlQuery qry(QString::null, db_->dbAux());

  if (qry.exec(sql.arg(fieldTransactionName).arg(d->idConn)) && qry.next()) {
    QString holdTransaction(qry.value(0).toString());
    QString holdPid(qry.value(1).toString());

    sql =  "select transactionid from pg_locks where locktype='transactionid' and %1='%2' ";
    sql += "and mode='ShareLock' and pid='%3' and granted='f'";

    if (qry.exec(sql.arg(fieldTransactionName).arg(holdTransaction).arg(holdPid)) && qry.next()) {
      QString transactionId(qry.value(0).toString());

      sql = "select pid from pg_locks where locktype='transactionid' and transactionid='%1' and pid='%2'";
      if (!qry.exec(sql.arg(transactionId).arg(d->idConn)) || !qry.next())
        return QStringList();
    } else
      return QStringList();
  } else
    return QStringList();

  sql = QString("select pg_class.relname,pg_stat_activity.*,pg_locks.* from "
                "pg_class,pg_stat_activity,pg_locks where pg_class.relkind='r' "
                "and pg_class.relfilenode=pg_locks.relation and pg_stat_activity.procpid=pg_locks.pid "
                "and pg_locks.mode='ExclusiveLock' and pg_locks.locktype='tuple' "
                "and pg_locks.pid<>'%1' and pg_stat_activity.waiting='t'");

  QSqlSelectCursor qryCur(sql.arg(d->idConn), db_->dbAux());
  QStringList ret;

  if (qryCur.next() && qryCur.count()) {
    QStringList rowsLock;
    QString item;

    do {
      item = qryCur.value(0).toString();
      rowsLock = rowsLockeds(item);
      if (!rowsLock.isEmpty()) {
        for (int i = 1; i < qryCur.count(); ++i)
          item += "@" + qryCur.value(i).toString();
        ret.append(item);
        ret += rowsLock;
      }
    } while (qryCur.next());

    if (!ret.isEmpty()) {
      item = qryCur.fieldName(0);
      for (int i = 1; i < qryCur.count(); ++i)
        item += "@" + qryCur.fieldName(i);
      ret.prepend(item);
    }
  }

  return ret;
}

QStringList QPSQLDriver::detectRisksLocks(const QString &table, const QString &primaryKeyValue)
{
  QString sql("select pg_class.relname,pg_stat_activity.*,pg_locks.* from "
              "pg_class,pg_stat_activity,pg_locks where pg_class.relkind='r' "
              "and pg_class.relfilenode=pg_locks.relation and pg_stat_activity.procpid=pg_locks.pid "
              "and pg_locks.mode like '%%ExclusiveLock' and pg_locks.pid<>'%1'");

  if (!table.isEmpty())
    sql += " and pg_class.relname='" + table + "'";

  QSqlSelectCursor qry(sql.arg(d->idConn), db_->dbAux());
  QStringList ret;

  if (qry.next() && qry.count()) {
    QStringList rowsLock;
    QString item;

    do {
      item = qry.value(0).toString();
      rowsLock = rowsLockeds(item, primaryKeyValue);
      if (!rowsLock.isEmpty()) {
        for (int i = 1; i < qry.count(); ++i)
          item += "@" + qry.value(i).toString();
        ret.append(item);
        ret += rowsLock;
      }
    } while (qry.next());

    if (!ret.isEmpty()) {
      item = qry.fieldName(0);
      for (int i = 1; i < qry.count(); ++i)
        item += "@" + qry.fieldName(i);
      ret.prepend(item);
    }
  }

  return ret;
}

bool QPSQLDriver::regenTable(const QString &n, FLTableMetaData *tmd)
{
  QSqlRecord recMtd = record(tmd);
  QSqlRecord recBd = record2(n);
  QSqlField *fieldMtd = 0;
  QSqlField *fieldBd = 0;
  bool mustAlter = false;

  for (int i = 0; i < recMtd.count(); ++i) {
    fieldMtd = recMtd.field(i);
    fieldBd = recBd.field(fieldMtd->name());
    if (fieldBd) {
      if (!fieldBd->value().canCast(FLFieldMetaData::flDecodeType(fieldMtd->type())) ||
          fieldBd->isNull() != fieldMtd->isNull() || fieldBd->isReadOnly() != fieldMtd->isReadOnly()) {
        mustAlter = true;
        break;
      }
    } else {
      mustAlter = true;
      break;
    }
  }

  if (mustAlter) {
    mustAlter = alterTable(tmd);

    if (mustAlter) {
      d->activeCreateIndex = false;
      db_->dbAux()->transaction();
      QSqlQuery qry(QString::null, db_->dbAux());
      QSqlQuery qry2(QString::null, db_->dbAux());
      QString item;

      QRegExp rx("^.*\\d{6,9}$");
      QStringList listOldBks(tables("").grep(rx));

      qry.exec("select nombre from flfiles where nombre similar to"
               "'%[[:digit:]][[:digit:]][[:digit:]][[:digit:]]-[[:digit:]][[:digit:]]%:[[:digit:]][[:digit:]]$' or nombre similar to"
               "'%alteredtable[[:digit:]][[:digit:]][[:digit:]][[:digit:]]%' or (bloqueo='f' and nombre like '%.mtd')");

      while (qry.next()) {
        item = qry.value(0).toString();
        qry2.exec("delete from flfiles where nombre = '" + item + "'");
        if (item.contains("alteredtable") && tables("").contains(item.replace(".mtd", "")))
          qry2.exec("drop table " + item.replace(".mtd", "") + " cascade");
      }

      for (QStringList::Iterator it = listOldBks.begin(); it != listOldBks.end(); ++it) {
        item = *it;
        if (tables("").contains(item))
          qry2.exec("drop table " + item + " cascade");
      }
      db_->dbAux()->commit();
    }
  }

  return mustAlter;
}

int QPSQLDriver::backendId() const
{
  return d->idConn;
}
