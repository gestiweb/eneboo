/***************************************************************************
                              qsql_psql.h
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
** Definition of PostgreSQL driver classes
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

#ifndef QSQL_PSQL_H
#define QSQL_PSQL_H

#include <qdict.h>
#include <qmessagebox.h>
#include <qptrvector.h>
#include <libpq-fe.h>
#include <FLManager.h>
#include <FLManagerModules.h>
#include <FLTableMetaData.h>
#include <FLFieldMetaData.h>
#include <FLRelationMetaData.h>
#include <FLUtil.h>
#include <FLSqlDatabase.h>
#include <FLSqlDriver.h>
#include <FLSqlCursor.h>
#include <FLDiskCache.h>

#ifdef QT_PLUGIN
#define Q_EXPORT_SQLDRIVER_PSQL
#else
#define Q_EXPORT_SQLDRIVER_PSQL Q_EXPORT
#endif

class QPSQLCacheInfoPrivate;
class QPSQLPrivate;
class QPSQLResultPrivate;
class QPSQLDriver;
class QSqlRecordInfo;

class QPSQLDriver : public FLSqlDriver
{

  Q_OBJECT

public:

  enum Protocol {
    Version6  = 6,
    Version7  = 7,
    Version71 = 8,
    Version73 = 9,
    Version74 = 10,
    Version8  = 11,
    Version81 = 12,
    Version82 = 13,
    Version83 = 14,
    Version84 = 15,
    Version9  = 16,
    Version91 = 17,
    Version92 = 18
  };

  QPSQLDriver(QObject *parent = 0, const char *name = 0);
  QPSQLDriver(PGconn *conn, QObject *parent = 0, const char *name = 0);
  ~QPSQLDriver();
  bool hasFeature(DriverFeature f) const;
  bool open(const QString &db, const QString &user = QString::null,
            const QString &password = QString::null, const QString &host = QString::null, int port = -1);
  void close();
  QSqlQuery createQuery() const;
  QStringList tables(const QString &user) const;
  bool existsTable(const QString &n) const;
  QSqlIndex primaryIndex(const QString &tablename) const;
  QSqlRecord record(const QString &tablename) const;
  QSqlRecord record(const QSqlQuery &query) const;
  QSqlRecordInfo recordInfo(const QString &tablename) const;
  QSqlRecordInfo recordInfo(const QSqlQuery &query) const;

  Protocol protocol() const {
    return pro;
  }
  PGconn *connection();
  QString formatValue(const QSqlField *field, bool trimStrings) const;

  // ### remove me for 4.0
  bool open(const QString &db, const QString &user, const QString &password,
            const QString &host, int port, const QString &connOpts);

  bool tryConnect(const QString &db, const QString &user = QString::null,
                  const QString &password = QString::null, const QString &host = QString::null, int port = -1);
  QString sqlCreateTable(const FLTableMetaData *tmd);
  QString formatValueLike(int t, const QVariant &v, const bool upper = false);
  QString formatValue(int t, const QVariant &v, const bool upper = false);
  QVariant nextSerialVal(const QString &table, const QString &field);
  int atFrom(FLSqlCursor *cur);
  void createSortIndex(FLSqlCursor *cur);
  bool alterTable(const QString &mtd1, const QString &mtd2, const QString &key = QString::null);
  bool canSavePoint();
  bool savePoint(const QString &n);
  bool releaseSavePoint(const QString &n);
  bool rollbackSavePoint(const QString &n);
  bool canOverPartition();
  QStringList rowsLockeds(const QString &table, const QString &primaryKeyValue = QString::null);
  QStringList locksStatus();
  QStringList detectLocks();
  QStringList detectRisksLocks(const QString &table = QString::null, const QString &primaryKeyValue = QString::null);
  bool regenTable(const QString &n, FLTableMetaData *tmd);
  QString md5TuplesState() const;
  QString md5TuplesStateTable(const QString &table) const;
  bool mismatchedTable(const QString &table,
                       const FLTableMetaData *tmd) const;
  int backendId() const;

private slots:

  void Mr_Proper();

protected:

  bool beginTransaction();
  bool commitTransaction();
  bool rollbackTransaction();

private:

  QSqlIndex primaryIndex2(const QString &tablename) const;
  QSqlRecord record2(const QString &tablename) const;
  QSqlRecord record(const FLTableMetaData *mtd) const;
  bool constraintExists(const QString &name) const;
  bool alterTable(FLTableMetaData *newMTD);
  QSqlRecordInfo recordInfo2(const QString &tablename) const;
  bool alterTable2(const QString &mtd1, const QString &mtd2, const QString &key = QString::null, bool force = false);
  int insertMulti(const QString &tablename, QPtrList<QSqlRecord> * records);
#ifndef FL_QUICK_CLIENT
  void createIndex(const QString &fieldName, const QString &tableName, bool textOp, bool noUpper = false) const;
  static QDict < bool > * dictIndexes;
#endif
  void init();
  bool mismatchedTable(const QString &table1, const QString &table2) const;

  Protocol pro;
  QPSQLPrivate *d;
  QPSQLCacheInfoPrivate *cInfo;
};

class QPSQLResult : public FLSqlResult
{

  friend class QPSQLDriver;

public:

  QPSQLResult(const QPSQLDriver *db, const QPSQLPrivate *p);
  ~QPSQLResult();
  PGresult *result();

protected:

  void cleanup();
  bool fetch(int i);
  bool fetchFirst();
  bool fetchLast();
  QVariant data(int i);
  bool isNull(int field);
  bool reset(const QString &query);
  int size();
  int numRowsAffected();

private:

  void calcSize();
  bool nextResult(int i);
  void closeCursor();
  bool openCursor();
  void cleanupCache();
  bool isCursorValid() const;
  int currentSize;
  QPSQLResultPrivate *d;

  static int cursorCounter;
};

#endif
