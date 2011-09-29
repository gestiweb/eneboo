/***************************************************************************
                      flsql_mysql.h  -  description
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
** Definition of MySQL driver classes
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

#ifndef QSQL_MYSQL_H
#define QSQL_MYSQL_H

#include <qsqldriver.h>
#include <qsqlresult.h>
#include <qsqlfield.h>
#include <qsqlindex.h>
#include <qmessagebox.h>
#include <qptrvector.h>
#include <FLManager.h>
#include <FLManagerModules.h>
#include <FLTableMetaData.h>
#include <FLFieldMetaData.h>
#include <FLSqlDatabase.h>
#include <FLSqlDriver.h>
#include <FLSqlCursor.h>
#include <FLUtil.h>
#include <FLDiskCache.h>

#if defined (Q_OS_WIN32)
#include <qt_windows.h>
#endif

#include <mysql.h>

#ifdef QT_PLUGIN
#define Q_EXPORT_SQLDRIVER_MYSQL
#else
#define Q_EXPORT_SQLDRIVER_MYSQL Q_EXPORT
#endif

class QMYSQLDriverPrivate;
class QMYSQLResultPrivate;
class QMYSQLDriver;
class QSqlRecordInfo;

class Q_EXPORT_SQLDRIVER_MYSQL QMYSQLDriver : public FLSqlDriver
{

    Q_OBJECT

    friend class QMYSQLResult;

public:

    enum MySQLVersion {
        Version4 = 4,
        Version5 = 5
    };

    QMYSQLDriver( QObject * parent = 0, const char * name = 0 );
    QMYSQLDriver( MYSQL * con, QObject * parent = 0, const char * name = 0 );
    ~QMYSQLDriver();
    bool hasFeature( DriverFeature f ) const;
    bool open( const QString & db, const QString & user = QString::null,
               const QString & password = QString::null, const QString & host = QString::null, int port = -1 );
    void close();
    QSqlQuery createQuery() const;
    QStringList tables( const QString & user ) const;
    QSqlIndex primaryIndex( const QString & tablename ) const;
    QSqlRecord record( const QString & tablename ) const;
    QSqlRecord record( const QSqlQuery & query ) const;
    QSqlRecordInfo recordInfo( const QString & tablename ) const;
    QSqlRecordInfo recordInfo( const QSqlQuery & query ) const;
    QString formatValue( const QSqlField * field, bool trimStrings ) const;
    MYSQL * mysql();
    // ### remove me for 4.0
    bool open( const QString& db, const QString& user, const QString& password,
               const QString& host, int port, const QString& connOpts );

    bool tryConnect( const QString & db, const QString & user = QString::null,
                     const QString & password = QString::null, const QString & host = QString::null, int port = -1 );
    QString sqlCreateTable( FLTableMetaData * tmd );
    QString formatValueLike( int t, const QVariant & v, const bool upper = false );
    QString formatValue( int t, const QVariant & v, const bool upper = false );
    QVariant nextSerialVal( const QString & table, const QString & field );
    int atFrom( FLSqlCursor * cur );
    bool alterTable( const QString & mtd1, const QString & mtd2, const QString & key = QString::null );
    bool canSavePoint();
    bool savePoint( const QString & n );
    bool releaseSavePoint( const QString & n );
    bool rollbackSavePoint( const QString & n );
    MySQLVersion version() const;

private slots:

    void Mr_Proper();

protected:

    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

private:

    QSqlIndex primaryIndex2( const QString& tablename ) const;
    QSqlRecord record2( const QString& tablename ) const;
    QSqlRecordInfo recordInfo2( const QString& tablename ) const;
    bool alterTable2( const QString & mtd1, const QString & mtd2,
                      const QString & key = QString::null, bool force = false );
#ifndef FL_QUICK_CLIENT
    void createIndex( const QString & fieldName, const QString & tableName ) const;
    static QDict < bool > * dictIndexes;
#endif

    void init();
    MySQLVersion version_;
    QMYSQLDriverPrivate* d;

    bool existsDatabase( const QString & db, const QString & user,
                         const QString & password, const QString & host, int port );

    int insertMulti( const QString & tablename, QPtrList<QSqlRecord> * records );
};

class QMYSQLResult : public QSqlResult
{
    friend class QMYSQLDriver;

public:

    QMYSQLResult( const QMYSQLDriver * db );
    ~QMYSQLResult();
    MYSQL_RES * result();

protected:

    void cleanup();
    bool fetch( int i );
    bool fetchNext();
    bool fetchLast();
    bool fetchFirst();
    QVariant data( int field );
    bool isNull( int field );
    bool reset( const QString & query );
    int size();
    int numRowsAffected();

private:

    my_ulonglong calcSize();
    bool fetchF( int i );
    bool fetchNextF();
    bool resetF( const QString & query );
    bool nextResult( int i );
    void cleanupCache();
    QMYSQLDriver::MySQLVersion version_;
    QMYSQLResultPrivate* d;
};

#endif
