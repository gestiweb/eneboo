/***************************************************************************
AQSqlPSQL.h
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

#ifndef AQSQLPSQL_H_
#define AQSQLPSQL_H_

#include <AQSql/AQSqlDriver.h>

#include <QtSql/qsqlresult.h>
#include <QtSql/qsqldriver.h>

typedef struct pg_conn PGconn;
typedef struct pg_result PGresult;

class AQPSQLResultPrivate;
class AQPSQLDriverPrivate;
class AQPSQLDriver;
class QSqlRecordInfo;

class AQPSQLResult : public QSqlResult
{
  friend class AQPSQLResultPrivate;

public:

  AQPSQLResult( const AQPSQLDriver * db, const AQPSQLDriverPrivate * p );
  ~AQPSQLResult();

  QVariant handle() const;

protected:

  void cleanup();
  bool fetch( int i );
  bool fetchFirst();
  bool fetchLast();
  QVariant data( int i );
  bool isNull( int field );
  bool reset ( const QString& query );
  int size();
  int numRowsAffected();
  QSqlRecord record() const;
  QVariant lastInsertId() const;

private:

  AQPSQLResultPrivate * d;
};

class AQPSQLDriver : public AQSqlDriver
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
    Version91 = 17
  };
  explicit AQPSQLDriver( QObject * parent = 0 );
  explicit AQPSQLDriver( PGconn * conn, QObject * parent = 0 );
  virtual ~AQPSQLDriver();

  bool hasFeature( DriverFeature f ) const;
  bool open( const QString & db, const QString & user, const QString & password,
             const QString & host, int port, const QString & connOpts );
  bool isOpen() const;
  void close();

  QSqlResult * createResult() const;
  QStringList tables( QSql::TableType ) const;
  QSqlIndex primaryIndex( const QString & tablename ) const;
  QSqlRecord record( const QString & tablename ) const;

  Protocol protocol() const;
  QVariant handle() const;

  QString escapeIdentifier( const QString & identifier, IdentifierType type ) const;
  QString formatValue( const QSqlField &field, bool trimStrings ) const;

  bool tryConnect( const QString & db, const QString & user = QString(),
                   const QString & password = QString(),
                   const QString & host = QString(), int port = -1 );

  QString sqlCreateTable( AQTableMD * tmd ) const;
  QString formatValueLike( int t, const QVariant & v, const bool upper = false ) const;
  QString formatValue( int t, const QVariant & v, const bool upper = false ) const;
  QVariant nextSerialVal( const QString & table, const QString & field ) const;

  bool canSavePoint() const;
  bool savePoint( const QString & n ) const;
  bool releaseSavePoint( const QString & n ) const;
  bool rollbackSavePoint( const QString & n ) const;
  void setAQSqlDatabase( AQSqlDatabase db );
  void createIndex( const QString & fieldName, const QString & tableName, bool upper = true ) const;
  void finish();

protected:

  bool beginTransaction();
  bool commitTransaction();
  bool rollbackTransaction();

private:

  QSqlIndex primaryIndex2( const QString& tablename ) const;
  QSqlRecord record2( const QString& tablename ) const;
  void init();
  AQPSQLDriverPrivate * d;
};

#endif /*AQSQLPSQL_H_*/

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
