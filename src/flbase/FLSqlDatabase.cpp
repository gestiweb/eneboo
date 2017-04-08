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
#include <qsinterpreter.h>

#include "FLApplication.h"
#include "FLSqlSavePoint.h"
#include "FLSqlDatabase.h"
#include "FLSqlDriver.h"
#include "FLSqlCursor.h"
#include "FLManager.h"
#include "FLManagerModules.h"

extern QSInterpreter *globalAQSInterpreter;

FLSqlDatabase::FLSqlDatabase()
  : db_(0), dbAux_(0), database_(QString::null), user_(QString::null),
    password_(QString::null), host_(QString::null), port_(-1),
    driverName_(QString::null), connectionName_("default"),
    manager_(0), managerModules_(0),
    transaction_(0), stackSavePoints_(0), queueSavePoints_(0),
    currentSavePoint_(0), lastActiveCursor_(0)
{
  setInteractiveGUI();
  setQsaExceptions();
}

FLSqlDatabase::~FLSqlDatabase()
{
  if (manager_) {
    manager_->finish();
    delete manager_;
    manager_ = 0;
  }

  if (managerModules_) {
    managerModules_->finish();
    delete managerModules_;
    managerModules_ = 0;
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
  return "PostgreSQL";
}

QString FLSqlDatabase::driverAliasToDriverName(const QString &alias)
{
  if (alias == "PostgreSQL")
    return "FLQPSQL7";
  if (alias == "SQLite3")
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
    return "SQLite3";
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
  if (alias == "SQLite3")
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

static inline QString parseConnOpts(const QString &connOpts, FLSqlDatabase *db)
{
  QStringList raw(QStringList::split(';', connOpts));
  QStringList opts;
  for (QStringList::const_iterator it = raw.begin(); it != raw.end(); ++it) {
    QString tmp(*it);
    if (tmp == "nogui")
      db->setInteractiveGUI(false);
    else if (tmp == "noexceptions")
      db->setQsaExceptions(false);
    else opts << tmp;
  }
  return opts.join(";");
}

bool FLSqlDatabase::connectDB(const QString &database, const QString &user,
                              const QString &password, const QString &host,
                              int port, const QString &connName,
                              const QString &connectOptions)
{
  if (driverName_.isEmpty())
    return false;

  finishInternal();

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  database_ = dr->formatDatabaseName(database);
  user_ = user;
  password_ = password;
  host_ = host;
  port_ = port;

  QString connOpts(parseConnOpts(connectOptions, this));
  db_->setConnectOptions(connOpts);
  dbAux_->setConnectOptions(connOpts);

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
    initInternal();
    return true;
  }

  return false;
}

bool FLSqlDatabase::connectDB()
{
  if (driverName_.isEmpty())
    return false;

  finishInternal();

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

    initInternal();
    return true;
  }

  return false;
}

void FLSqlDatabase::closeDB()
{
  finishInternal();

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

void FLSqlDatabase::createSortIndex(FLSqlCursor *cur)
{
  if (!db_ || !cur)
    return;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(const_cast<QSqlDriver *>(cur->driver()));
  dr->createSortIndex(cur);
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

bool FLSqlDatabase::canOverPartition() const
{
  if (!db_)
    return false;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  return dr->canOverPartition();
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

QString FLSqlDatabase::md5TuplesState() const
{
  if (driverName_.isEmpty() || !dbAux_)
    return QString::null;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  return dr->md5TuplesState();
}

QString FLSqlDatabase::md5TuplesStateTable(const QString &table) const
{
  if (driverName_.isEmpty() || !dbAux_)
    return QString::null;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  return dr->md5TuplesStateTable(table);
}

bool FLSqlDatabase::mismatchedTable(const QString &table,
                                    const FLTableMetaData *tmd) const
{
  if (driverName_.isEmpty() || !dbAux_)
    return QString::null;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  return dr->mismatchedTable(table, tmd);
}

bool FLSqlDatabase::existsTable(const QString &n) const
{
  if (driverName_.isEmpty() || !dbAux_)
    return tables().contains(n);

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(dbAux_->driver());
  return dr->existsTable(n);
}

QString FLSqlDatabase::getNotifies() const
{
  if (driverName_.isEmpty() || !dbAux_)
    return QString::null;

  FLSqlDriver *dr = ::qt_cast<FLSqlDriver *>(db_->driver());
  return dr->getNotifies();
}

bool FLSqlDatabase::isOpen() const
{
  return db_ ? db_->isOpen() : false;
}

bool FLSqlDatabase::isOpenError() const
{
  return db_ ? db_->isOpenError() : true;
}

QStringList FLSqlDatabase::tables() const
{
  return db_ ? db_->tables() : QStringList();
}

QStringList FLSqlDatabase::tables(QSql::TableType type) const
{
  return db_ ? db_->tables(type) : QStringList();
}

QSqlError FLSqlDatabase::lastError() const
{
  return db_ ? db_->lastError() : QSqlError();
}

QString FLSqlDatabase::connectOptions() const
{
  return db_ ? db_->connectOptions() : QString();
}

bool FLSqlDatabase::interactiveGUI() const
{
  return interactiveGUI_ && qApp && !qApp->aqWasDeleted();
}

void FLSqlDatabase::setInteractiveGUI(bool on)
{
  interactiveGUI_ = on;
  if (globalAQSInterpreter)
    globalAQSInterpreter->setInteractiveGUI(interactiveGUI_);
}

bool FLSqlDatabase::qsaExceptions() const
{
  return qsaExceptions_ && qApp && !qApp->aqWasDeleted();
}

bool FLSqlDatabase::doTransaction(FLSqlCursor *cur)
{
  if (!cur || !db_ || !aqApp || !qApp)
    return false;

  if (transaction_ == 0 && canTransaction()) {
    aqApp->statusHelpMsg(qApp->tr("Iniciando transacción..."));
    if (db_->transaction()) {
      lastActiveCursor_ = cur;
      aqApp->emitTransactionBegin(cur);

      if (!canSavePoint()) {
        if (currentSavePoint_) {
          delete currentSavePoint_;
          currentSavePoint_ = 0;
        }
        stackSavePoints_->clear();
        queueSavePoints_->clear();
      }
      ++transaction_;
      cur->d->transactionsOpened_.push(transaction_);
      return true;
    } else {
#ifdef FL_DEBUG
      qWarning(qApp->tr("FLSqlDatabase::doTransaction : Fallo al intentar iniciar transacción"));
#endif
      return false;
    }
  } else {
    aqApp->statusHelpMsg(qApp->tr("Creando punto de salvaguarda %1...").arg(transaction_));
    if (!canSavePoint()) {
      if (transaction_ == 0) {
        if (currentSavePoint_) {
          delete currentSavePoint_;
          currentSavePoint_ = 0;
        }
        stackSavePoints_->clear();
        queueSavePoints_->clear();
      }
      if (currentSavePoint_)
        stackSavePoints_->push(currentSavePoint_);
      currentSavePoint_ = new FLSqlSavePoint(transaction_);
    } else
      savePoint(QString::number(transaction_));
    ++transaction_;
    cur->d->transactionsOpened_.push(transaction_);
    return true;
  }
}

bool FLSqlDatabase::doCommit(FLSqlCursor *cur, bool notify)
{
  if (!cur || !db_ || !aqApp || !qApp)
    return false;

  if (!notify)
    emit cur->autoCommit();

  if (transaction_ > 0) {
    if (!cur->d->transactionsOpened_.isEmpty()) {
      int trans = cur->d->transactionsOpened_.pop();
      if (trans != transaction_) {
        qWarning(
          qApp->tr("FLSqlDatabase : El cursor va a terminar la transacción %1 pero la última que inició es la %2")
          .arg(transaction_)
          .arg(trans)
        );
      }
    } else {
      qWarning(
        qApp->tr("FLSqlDatabase : El cursor va a terminar la transacción %1 pero no ha iniciado ninguna")
        .arg(transaction_)
      );
    }
    transaction_--;
  } else
    return true;

  if (transaction_ == 0 && canTransaction()) {
    aqApp->statusHelpMsg(qApp->tr("Terminando transacción..."));
    if (db_->commit()) {
      lastActiveCursor_ = 0;

      if (!canSavePoint()) {
        if (currentSavePoint_) {
          delete currentSavePoint_;
          currentSavePoint_ = 0;
        }
        stackSavePoints_->clear();
        queueSavePoints_->clear();
      }
      if (notify)
        cur->d->modeAccess_ = FLSqlCursor::BROWSE;

      aqApp->emitTransactionEnd(cur);

#ifdef AQ_MD5_CHECK
      cur->d->md5Tuples_ = db_->md5TuplesStateTable(cur->d->curName_);
#endif

      return true;
    } else {
#ifdef FL_DEBUG
      qWarning(qApp->tr("FLSqlDatabase::doCommit : Fallo al intentar terminar transacción"));
#endif
      return false;
    }
  } else {
    aqApp->statusHelpMsg(qApp->tr("Liberando punto de salvaguarda %1...").arg(transaction_));
    if ((transaction_ == 1 && canTransaction()) || (transaction_ == 0 && !canTransaction())) {
      if (!canSavePoint()) {
        if (currentSavePoint_) {
          delete currentSavePoint_;
          currentSavePoint_ = 0;
        }
        stackSavePoints_->clear();
        queueSavePoints_->clear();
      } else
        releaseSavePoint(QString::number(transaction_));
      if (notify)
        cur->d->modeAccess_ = FLSqlCursor::BROWSE;
      return true;
    }
    if (!canSavePoint()) {
      int tamQueue = queueSavePoints_->count();
      FLSqlSavePoint *tempSavePoint;
      for (int i = 0; i < tamQueue; ++i) {
        tempSavePoint = queueSavePoints_->dequeue();
        tempSavePoint->setId(transaction_ - 1);
        queueSavePoints_->enqueue(tempSavePoint);
      }
      if (currentSavePoint_) {
        queueSavePoints_->enqueue(currentSavePoint_);
        currentSavePoint_ = 0;
        if (!stackSavePoints_->isEmpty())
          currentSavePoint_ = stackSavePoints_->pop();
      }
    } else
      releaseSavePoint(QString::number(transaction_));
    if (notify)
      cur->d->modeAccess_ = FLSqlCursor::BROWSE;
    return true;
  }
}

bool FLSqlDatabase::doRollback(FLSqlCursor *cur)
{
  if (!cur || !db_ || !aqApp || !qApp)
    return false;

  bool cancel = false;
  if (interactiveGUI() &&
      (cur->d->modeAccess_ == FLSqlCursor::INSERT || cur->d->modeAccess_ == FLSqlCursor::EDIT)
      && cur->isModifiedBuffer() && cur->d->askForCancelChanges_) {
    int res = QMessageBox::information(
                qApp->focusWidget(), qApp->tr("Cancelar cambios"),
                qApp->tr("Todos los cambios efectuados se cancelarán.¿Está seguro?"),
                QMessageBox::Yes,
                QMessageBox::No | QMessageBox::Default | QMessageBox::Escape
              );
    if (res == QMessageBox::No)
      return false;
    cancel = true;
  }

  if (transaction_ > 0) {
    if (!cur->d->transactionsOpened_.isEmpty()) {
      int trans = cur->d->transactionsOpened_.pop();
      if (trans != transaction_)
        qWarning(
          qApp->tr("FLSqlDatabase : El cursor va a deshacer la transacción %1 pero la última que inició es la %2")
          .arg(transaction_)
          .arg(trans)
        );
    } else {
      qWarning(
        qApp->tr("FLSqlDatabaser : El cursor va a deshacer la transacción %1 pero no ha iniciado ninguna")
        .arg(transaction_)
      );
    }
    transaction_--;
  } else
    return true;

  if (transaction_ == 0 && canTransaction()) {
    aqApp->statusHelpMsg(qApp->tr("Deshaciendo transacción..."));
    if (db_->rollback()) {
      lastActiveCursor_ = 0;

      if (!canSavePoint()) {
        if (currentSavePoint_) {
          delete currentSavePoint_;
          currentSavePoint_ = 0;
        }
        stackSavePoints_->clear();
        queueSavePoints_->clear();
      }
      cur->d->modeAccess_ = FLSqlCursor::BROWSE;
      if (cancel)
        cur->QSqlCursor::select();

      aqApp->emitTransactionRollback(cur);

      return true;
    } else {
#ifdef FL_DEBUG
      qWarning(qApp->tr("FLSqlDatabase::doRollback : Fallo al intentar deshacer transacción"));
#endif
      return false;
    }
  } else {
    aqApp->statusHelpMsg(qApp->tr("Restaurando punto de salvaguarda %1...").arg(transaction_));
    if (!canSavePoint()) {
      int tamQueue = queueSavePoints_->count();
      int tempId;
      FLSqlSavePoint *tempSavePoint;
      for (int i = 0; i < tamQueue; ++i) {
        tempSavePoint = queueSavePoints_->dequeue();
        tempId = tempSavePoint->id();
        if (tempId > transaction_ || transaction_ == 0) {
          tempSavePoint->undo();
          delete tempSavePoint;
        } else
          queueSavePoints_->enqueue(tempSavePoint);
      }
      if (currentSavePoint_) {
        currentSavePoint_->undo();
        delete currentSavePoint_;
        currentSavePoint_ = 0;
        if (!stackSavePoints_->isEmpty())
          currentSavePoint_ = stackSavePoints_->pop();
      }
      if (transaction_ == 0) {
        if (currentSavePoint_) {
          delete currentSavePoint_;
          currentSavePoint_ = 0;
        }
        stackSavePoints_->clear();
        queueSavePoints_->clear();
      }
    } else
      rollbackSavePoint(QString::number(transaction_));
    cur->d->modeAccess_ = FLSqlCursor::BROWSE;
    return true;
  }
}

void FLSqlDatabase::initInternal()
{
  currentSavePoint_ = 0;
  lastActiveCursor_ = 0;
  if (!stackSavePoints_ && !canSavePoint()) {
    stackSavePoints_ = new QPtrStack<FLSqlSavePoint>;
    stackSavePoints_->setAutoDelete(true);
  }
  if (!queueSavePoints_ && !canSavePoint()) {
    queueSavePoints_ = new QPtrQueue<FLSqlSavePoint>;
    queueSavePoints_->setAutoDelete(true);
  }
}

void FLSqlDatabase::finishInternal()
{
  if (transaction_ > 0) {
    if (lastActiveCursor_) {
      lastActiveCursor_->rollbackOpened
      (-1,
       QString("Se han detectado transacciones no finalizadas en la última operación.\n"
               "Se van a cancelar las transacciones pendientes.\n"
               "Los últimos datos introducidos no han sido guardados, por favor\n"
               "revise sus últimas acciones y repita las operaciones que no\n"
               "se han guardado.\n") +
       QString("SqlDatabase::finishInternal: %1\n").arg(lastActiveCursor_->d->curName_)
      );
    }
    if (db_ && transaction_ > 0)
      db_->rollback();
    transaction_ = 0;
  }
  if (stackSavePoints_) {
    stackSavePoints_->clear();
    delete stackSavePoints_;
    stackSavePoints_ = 0;
  }
  if (queueSavePoints_) {
    queueSavePoints_->clear();
    delete queueSavePoints_;
    queueSavePoints_ = 0;
  }
  currentSavePoint_ = 0;
  lastActiveCursor_ = 0;
}
