/***************************************************************************
                         qsqlite.h  -  description
                            -------------------
   begin                : lun Nov 24 2003
   copyright            : (C) 2003-2004 by InfoSiAL S.L.
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

#ifndef QSQLITE_H
#define QSQLITE_H

#include <qstringlist.h>
#include <qregexp.h>
#include <qdir.h>
#include <stdlib.h>
#include <qmessagebox.h>
#include <FLManager.h>
#include <FLTableMetaData.h>
#include <FLFieldMetaData.h>
#include <FLManagerModules.h>
#include <FLUtil.h>
#include <sqlitedataset.h>
#include <FLSqlDatabase.h>
#include <FLSqlDriver.h>
#include <FLSqlConnections.h>
#include <FLDiskCache.h>

#if (QT_VERSION-0 >= 0x030000)
typedef QVariant QSqlVariant;
#endif

using namespace dbiplus;

class SqliteDriver : public FLSqlDriver
{
  Q_OBJECT

public:

  SqliteDriver(QObject *parent = 0, const char *name = 0);
  ~SqliteDriver();
  bool open(const QString &db,
            const QString &user = QString::null,
            const QString &password = QString::null,
            const QString &host = QString::null,
            int port = -1);
  bool open(const QString &db, const QString &user,
            const QString &password, const QString &host,
            int port, const QString &connOpts);
  void close();
  bool hasFeature(QSqlDriver::DriverFeature f) const;
  QSqlQuery createQuery() const;
  QStringList tables(const QString &user) const;
  QSqlRecord record(const QString &tablename) const;
  QSqlRecord record(const QSqlQuery &query) const;
  QSqlRecordInfo recordInfo(const QString &tablename) const;
  QSqlRecordInfo recordInfo(const QSqlQuery &query) const;
  QSqlIndex primaryIndex(const QString &tablename) const;
  bool beginTransaction();
  bool commitTransaction();
  bool rollbackTransaction();
  SqliteDatabase *dataBase();

  QString formatDatabaseName(const QString &name);
  bool tryConnect(const QString &db,
                  const QString &user = QString::null,
                  const QString &password = QString::null,
                  const QString &host = QString::null,
                  int port = -1);
  QString sqlCreateTable(const FLTableMetaData *tmd);
  QString formatValueLike(int t, const QVariant &v, const bool upper = false);
  QString formatValue(int t, const QVariant &v, const bool upper = false);
  QVariant nextSerialVal(const QString &table, const QString &field);
  bool alterTable(const QString &mtd1, const QString &mtd2, const QString &key = QString::null);
  void Mr_Proper();

private:

  QSqlIndex primaryIndex2(const QString &tablename) const;
  QSqlRecord record2(const QString &tablename) const;
  QSqlRecord record(const FLTableMetaData *mtd) const;
  QSqlRecordInfo recordInfo2(const QString &tablename) const;
  SqliteDatabase *dataBase_;
};

class SqliteResult : public QSqlResult
{

public:

  SqliteResult(const QSqlDriver *d);
  ~SqliteResult();

protected:

  QVariant data(int);
  bool reset(const QString &query);
  bool fetch(int);
  bool fetchFirst();
  bool fetchLast();
  bool isNull(int);
  int size();
  int numRowsAffected();

private:

  QString query;
  const SqliteDriver *driver;
  Dataset *dataSet;

  friend class SqliteDriver;
};

#endif
