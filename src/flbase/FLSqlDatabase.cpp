/***************************************************************************
                            FLSqlDatabase.cpp
                         -------------------
begin                : Thu Nov 22 2005
copyright            : (C) 2002-2005 by InfoSiAL S.L.
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

#include <qapplication.h>

#include "FLSqlDatabase.h"
#include "FLSqlDriver.h"
#include "FLSqlCursor.h"
#include "FLManager.h"
#include "FLManagerModules.h"

FLSqlDatabase::FLSqlDatabase()
  : db_(0), dbAux_(0), database_(QString::null), user_(QString::null), password_(QString::null),
    host_(QString::null), port_(-1), driverName_(QString::null), connectionName_("default"), manager_(0),
    managerModules_(0) {}

FLSqlDatabase::~FLSqlDatabase()
{
  if (manager_) {
    manager_->finish();
    delete manager_;
  }

  if (managerModules_) {
    managerModules_->finish();
    delete managerModules_;
  }

  closeDB();
}

QStringList FLSqlDatabase::driverAliases()
{
  QStringList ret;
  QStringList list = QSqlDatabase::drivers();
  QStringList::Iterator it = list.begin();
  while (it != list.end()) {
    ret << driverNameToDriverAlias(*it);
    ++it;
  }
  return ret;
}

QString FLSqlDatabase::defaultAlias()
{
  return "SQLite";
}

QString FLSqlDatabase::driverAliasToDriverName(const QString &alias)
{
  if (alias == "PostgreSQL")
    return "FLQPSQL7";
  if (alias == "SQLite")
    return "FLsqlite";
  if (alias == "MySQL")
    return "FLQMYSQL4";
  if (alias == "MySQL_NO_INNODB")
    return "FLQMYSQL4_NO_INNODB";
  return alias;
}

QString FLSqlDatabase::driverNameToDriverAlias(const QString &name)
{
  if (name == "FLQPSQL7")
    return "PostgreSQL";
  if (name == "FLsqlite")
    return "SQLite";
  if (name == "FLQMYSQL4")
    return "MySQL";
  if (name == "FLQMYSQL4_NO_INNODB")
    return "MySQL_NO_INNODB";
  return name;
}

bool FLSqlDatabase::needConnOption(const QString &alias, const int connOption)
{
  if (alias == "PostgreSQL")
    return true;
  if (alias == "SQLite")
    return false;
  if (alias == "MySQL" || alias == "MySQL_NO_INNODB")
    return true;
  return true;
}

QString FLSqlDatabase::defaultPort(const QString &alias)
{
  if (alias == "PostgreSQL")
    return "5432";
  if (alias == "MySQL" || alias == "MySQL_NO_INNODB")
    return "3306";
  return QString::null;
}

bool FLSqlDatabase::loadDriver(const QString &driverName, const QString &connName)
{
  db_ = QSqlDatabase::addDatabase(driverName, connName + QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz"));
  if (!db_->lastError().driverText().isEmpty())
    return false;

  dbAux_ = db_;

  if (db_->isOpen())
    db_->close();

  if (driverName == "FLQPSQL7" || driverName == "FLQMYSQL3" || driverName == "FLQMYSQL4" || driverName == "FLQMYSQL4_NO_INNODB") {
    dbAux_ = QSqlDatabase::addDatabase(driverName, connName + "Aux" + QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz"));
    if (!dbAux_->lastError().driverText().isEmpty())
      return false;
    if (dbAux_->isOpen())
      dbAux_->close();
  }

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(db_->driver());
  dr->setFLSqlDatabase(this);
  dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  dr->setFLSqlDatabase(this);
  driverName_ = driverName;
  return true;
}

bool FLSqlDatabase::connectDB(const QString &database, const QString &user, const QString &password, const QString &host, int port, const QString &connName)
{
  if (driverName_.isEmpty())
    return false;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  database_ = dr->formatDatabaseName(database);
  user_ = user;
  password_ = password;
  host_ = host;
  port_ = port;

  if (dr->tryConnect(database_, user_, password_, host_, port_)) {
    db_->setDatabaseName(database_);
    db_->setUserName(user_);
    db_->setPassword(password_);
    db_->setHostName(host_);
    db_->setPort(port_);
    if (!db_->open())
      return false;

    dbAux_->setDatabaseName(database_);
    dbAux_->setUserName(user_);
    dbAux_->setPassword(password_);
    dbAux_->setHostName(host_);
    dbAux_->setPort(port_);
    if (!dbAux_->open())
      return false;

    connectionName_ = connName;
    return true;
  }

  return false;
}

bool FLSqlDatabase::connectDB()
{
  if (driverName_.isEmpty())
    return false;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());

  if (dr->tryConnect(database_, user_, password_, host_, port_)) {
    db_->setDatabaseName(database_);
    db_->setUserName(user_);
    db_->setPassword(password_);
    db_->setHostName(host_);
    db_->setPort(port_);
    if (!db_->open())
      return false;

    dbAux_->setDatabaseName(database_);
    dbAux_->setUserName(user_);
    dbAux_->setPassword(password_);
    dbAux_->setHostName(host_);
    dbAux_->setPort(port_);
    if (!dbAux_->open())
      return false;

    return true;
  }

  return false;
}

void FLSqlDatabase::closeDB()
{
  if (driverName_.isEmpty())
    return;

  if (dbAux_)
    dbAux_->close();
  if (db_)
    db_->close();
}

bool FLSqlDatabase::createTable(FLTableMetaData *tmd)
{
#ifndef FL_QUICK_CLIENT
  if (driverName_.isEmpty() || !tmd || !dbAux_)
    return false;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  QString sql = dr->sqlCreateTable(tmd);

  if (sql.isEmpty())
    return false;

  QSqlQuery q(QString::null, dbAux_);
  if (!q.exec(sql)) {
#ifdef FL_DEBUG
    qWarning("FLManager : SQL - " + sql);
#endif

    return false;
  }
#endif
  return true;
}

bool FLSqlDatabase::canRegenTables()
{
  if (driverName_.isEmpty() || !dbAux_)
    return false;
  if (driverName_ == "FLQPSQL7")
    return true;
  if (driverName_ == "FLsqlite")
    return true;
  if (driverName_ == "FLQMYSQL4" || driverName_ == "FLQMYSQL4_NO_INNODB")
    return true;
  return false;
}

QString FLSqlDatabase::formatValueLike(int t, const QVariant &v, const bool upper)
{
  if (!db_)
    return v.toString();

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(db_->driver());
  return dr->formatValueLike(t, v, upper);
}

QString FLSqlDatabase::formatValue(int t, const QVariant &v, const bool upper)
{
  if (!db_)
    return v.toString();

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(db_->driver());
  return dr->formatValue(t, v, upper);
}

QVariant FLSqlDatabase::nextSerialVal(const QString &table, const QString &field)
{
  if (!db_)
    return QVariant(0);

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  return dr->nextSerialVal(table, field);
}

int FLSqlDatabase::atFrom(FLSqlCursor *cur)
{
  if (!db_ || !cur)
    return 0;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(const_cast<QSqlDriver *>(cur->driver()));
  return dr->atFrom(cur);
}

bool FLSqlDatabase::alterTable(const QString &mtd1, const QString &mtd2, const QString &key)
{
  if (!db_)
    return false;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  return dr->alterTable(mtd1, mtd2, key);
}

FLManager *FLSqlDatabase::manager()
{
  if (!manager_) {
    manager_ = new FLManager(this);
    manager_->init();
  }
  return manager_;
}

FLManagerModules *FLSqlDatabase::managerModules()
{
  if (!manager_) {
    manager_ = new FLManager(this);
    manager_->init();
  }

  if (!managerModules_) {
    managerModules_ = new FLManagerModules(this);
    managerModules_->init();
  }
  return managerModules_;
}

bool FLSqlDatabase::canSavePoint()
{
  if (!db_)
    return false;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  return dr->canSavePoint();
}

bool FLSqlDatabase::savePoint(const QString &n)
{
  if (!db_)
    return false;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(db_->driver());
  return dr->savePoint(n);
}

bool FLSqlDatabase::releaseSavePoint(const QString &n)
{
  if (!db_)
    return false;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(db_->driver());
  return dr->releaseSavePoint(n);
}

bool FLSqlDatabase::rollbackSavePoint(const QString &n)
{
  if (!db_)
    return false;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(db_->driver());
  return dr->rollbackSavePoint(n);
}

bool FLSqlDatabase::canTransaction() const
{
  if (!db_)
    return false;
  return db_->driver()->hasFeature(QSqlDriver::Transactions);
}

void FLSqlDatabase::Mr_Proper()
{
  if (db_) {
    FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
    dr->Mr_Proper();
  }
}

bool FLSqlDatabase::canDetectLocks() const
{
  if (driverName_ == "FLQPSQL7")
    return true;
  return false;
}

QStringList FLSqlDatabase::locksStatus()
{
  if (!db_)
    return QStringList();

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(db_->driver());
  return dr->detectLocks();
}

QStringList FLSqlDatabase::detectLocks()
{
  if (!db_)
    return QStringList();

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(db_->driver());
  return dr->detectLocks();
}

QStringList FLSqlDatabase::detectRisksLocks(const QString &table, const QString &primaryKeyValue)
{
  if (!db_)
    return QStringList();

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(db_->driver());
  return dr->detectRisksLocks(table, primaryKeyValue);
}

bool FLSqlDatabase::regenTable(const QString &n, FLTableMetaData *tmd)
{
  if (driverName_.isEmpty() || !tmd || !dbAux_)
    return false;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  return dr->regenTable(n, tmd);
}
