/***************************************************************************
                         qsqlite.cpp  -  description
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

#include "qsqlite.h"

#include <qapplication.h>
#include <qdom.h>
#include <qprogressdialog.h>
#include <qsqlcursor.h>
#include <qpixmap.h>

SqliteDriver::SqliteDriver(QObject *parent, const char *name) :
  FLSqlDriver(parent, name), dataBase_(0) {}

SqliteDriver::~SqliteDriver() {}

SqliteDatabase *SqliteDriver::dataBase()
{
  return dataBase_;
}

bool SqliteDriver::open(const QString &db, const QString &, const QString &, const QString &, int)
{
  if (db.isEmpty()) {
#ifdef FL_DEBUG
    qWarning("SqliteDriver::open() : No Database name");
#endif

    return false;
  }

  if (!QFile::exists(db)) {
    QMessageBox msgBox(tr("AVISO IMPORTANTE") ,
                       tr("AbanQ puede comportarse de forma inestable con la base de datos SQLite.\n"
                          "AbanQ utilizando SQLite no tiene ningún tipo de soporte por parte de InfoSiAL S.L.,\n"
                          "esta opción sólo se ofrece para poder probar fácilmente la aplicación\n"
                          "sin necesidad de instalar un servidor de bases de datos.\n\n"
                          "Para asegurar la fiabilidad en entornos en producción utilice un sistema de gestión\n"
                          "de bases de datos como PostgreSQL o MySQL"),
                       QMessageBox::NoIcon, QMessageBox::Ok, QMessageBox::NoButton,  QMessageBox::NoButton);
    msgBox.setIconPixmap(QPixmap::fromMimeSource("bomba.png"));
    msgBox.exec();
  }

  close();
  dataBase_ = new SqliteDatabase();
  dataBase_->setDatabase(db);

  if (dataBase_->connect() != DB_CONNECTION_OK) {
#ifdef FL_DEBUG
    qWarning("SqliteDriver::open() : %s", dataBase_->getErrorMsg());
#endif

    setOpen(false);
    setOpenError(true);
    setLastError(QSqlError(dataBase_->getErrorMsg(), QString::null, QSqlError::Unknown));
    return false;
  } else {
    setOpen(true);
    setOpenError(false);
    return true;
  }
}

bool SqliteDriver::open(const QString &db, const QString &, const QString &, const QString &host, int , const QString &)
{
  return open(db, QString::null, QString::null, QString::null, 0);
}

bool SqliteDriver::tryConnect(const QString &db, const QString &user, const QString &password, const QString &host, int port)
{
  if (!open(db, user, password, host, port, QString::null)) {
    if (lastError().type() == QSqlError::Connection) {
      QMessageBox::critical(0, tr("Conexión fallida"),
                            tr("No se pudo conectar con la base de datos %1.").arg(db),
                            QMessageBox::Ok, 0, 0);
      QMessageBox::critical(0, tr("Error"), QString(lastError().driverText().utf8()) + "\n" +
                            QString(lastError().databaseText().utf8()), QMessageBox::Ok, 0, 0);
      return false;
    }
  }

  return true;
}

QString SqliteDriver::sqlCreateTable(FLTableMetaData *tmd)
{
#ifndef FL_QUICK_CLIENT
  if (!tmd)
    return QString::null;

  QString primaryKey(QString::null);
  QString sql = "CREATE TABLE " + tmd->name() + " (";

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
        sql += " INTEGER";
        break;

      case QVariant::UInt:
        sql += " INTEGER";
        break;

      case QVariant::Bool:
      case FLFieldMetaData::Unlock:
        sql += " BOOLEAN";
        break;

      case QVariant::Double:
        sql += " FLOAT";
        break;

      case QVariant::Time:
        sql += " VARCHAR(20)";
        break;

      case QVariant::Date:
        sql += " VARCHAR(20)";
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
        sql += " CLOB";
        break;

      case FLFieldMetaData::Serial:
        sql += " INTEGER";
        if (!field->isPrimaryKey())
          sql += " PRIMARY KEY";
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
        qWarning(QApplication::tr("FLManager : Tabla -> ") +
                 tmd->name() + QApplication::tr(" . Se ha intentado poner una segunda clave primaria para el campo ") +
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

  QString createIndex = "CREATE INDEX " + tmd->name() + "_pkey ON " + tmd->name() + " (" + tmd->primaryKey() + ");";

  sql += createIndex;

  return sql;

#endif //FL_QUICK_CLIENT

  return QString::null;
}

QString SqliteDriver::formatValueLike(int t, const QVariant &v, const bool upper)
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
    default:
      if (upper)
        res = "LIKE '" + v.toString().upper() + "%%'";
      else
        res = "LIKE '" + v.toString() + "%%'";
  }

  return res;
}

QString SqliteDriver::formatValue(int t, const QVariant &v, const bool upper)
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
      res += v.toString();
      break;
    default:
      if (upper)
        res = "'" + v.toString().upper() + "'";
      else
        res = "'" + v.toString() + "'";
  }

  return res;
}

QVariant SqliteDriver::nextSerialVal(const QString &table, const QString &field)
{
  QSqlQuery query = createQuery();
  query.exec("SELECT max(" + field + ") FROM " + table + ";");
  query.next();
  return query.value(0).toInt() + 1;
}

bool SqliteDriver::alterTable(const QString &mtd1, const QString &mtd2, const QString &key)
{
#ifndef FL_QUICK_CLIENT
  FLTableMetaData *oldMTD = 0;
  FLTableMetaData *newMTD = 0;
  QDomDocument doc("doc");
  QDomElement docElem;

  if (!FLUtil::domDocumentSetContent(doc, mtd1)) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("Error al cargar los metadatos."));
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

  if (db_->manager()->checkMetaData(oldMTD, newMTD)) {
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

  db_->dbAux() ->transaction();

  if (!key.isEmpty() && key.length() == 40) {
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

  if (!q.exec("CREATE TABLE " + renameOld + " AS SELECT * FROM " + oldMTD->name() + ";") || !q.exec("DROP TABLE " + oldMTD->name() + ";")) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("No se ha podido renombrar la tabla antigua."));
#endif

    db_->dbAux() ->rollback();
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  if (!db_->manager()->createTable(newMTD)) {
    db_->dbAux() ->rollback();
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  QSqlCursor oldCursor(renameOld, true, db_->dbAux());
  oldCursor.setMode(QSqlCursor::ReadOnly);
  QSqlCursor newCursor(newMTD->name(), true, db_->dbAux());
  newCursor.setMode(QSqlCursor::Insert);

  oldCursor.select();
  int totalSteps = oldCursor.size();
  QProgressDialog progress(QApplication::tr("Reestructurando registros para %1...").arg(newMTD->alias()), 0, totalSteps, qApp->focusWidget(), 0, true);
  progress.setCaption(QApplication::tr("Tabla modificada"));

  int step = 0;
  QSqlRecord *newBuffer;
  QString sequence;
  fieldList = newMTD->fieldList();
  FLFieldMetaData *newField = 0;

  if (!fieldList) {
#ifdef FL_DEBUG
    qWarning("FLManager::alterTable : " + QApplication::tr("Los nuevos metadatos no tienen campos."));
#endif

    db_->dbAux() ->rollback();
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

    db_->dbAux() ->rollback();
    if ((oldMTD != newMTD) && oldMTD)
      delete oldMTD;
    if (newMTD)
      delete newMTD;
    return false;
  }

  QVariant v;
  bool ok = true;
  while (oldCursor.next()) {
    newBuffer = newCursor.primeInsert();

    QDictIterator<FLFieldMetaData> it(*fieldList);
    while ((newField = it.current()) != 0) {
      ++it;
      oldField = oldMTD->field(newField->name());
      if (!oldField || !oldCursor.field(oldField->name())) {
        if (!oldField)
          oldField = newField;
        v = newField->defaultValue();
        v.cast(FLFieldMetaData::flDecodeType(newField->type()));
      } else {
        v = oldCursor.value(newField->name());
        if ((!oldField->allowNull() || !newField->allowNull()) &&
            (v.isNull() || !v.isValid())) {
          QVariant defVal(newField->defaultValue());
          if (!defVal.isNull() && defVal.isValid())
            v = defVal;
        }
        if (!v.cast(newBuffer->value(newField->name()).type())) {
#ifdef FL_DEBUG
          qWarning("FLManager::alterTable : " +
                   QApplication::tr("Los tipos del campo %1 no son compatibles. Se introducirá un valor nulo.")
                   .arg(newField->name()));
#endif
        }
      }
      if ((!oldField->allowNull() || !newField->allowNull()) && (v.isNull() || !v.isValid())) {
        switch (oldField->type()) {
          case QVariant::Int:
          case FLFieldMetaData::Serial:
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

    if (!newCursor.insert()) {
      ok = false;
      break;
    }
    progress.setProgress(++step);
  }

  progress.setProgress(totalSteps);

  if ((oldMTD != newMTD) && oldMTD)
    delete oldMTD;

  if (newMTD)
    delete newMTD;

  if (ok)
    db_->dbAux() ->commit();
  else {
    db_->dbAux() ->rollback();
    return false;
  }
#else

  return true;
#endif //FL_QUICK_CLIENT
}

void SqliteDriver::close()
{
  if (isOpen() && dataBase_) {
    dataBase_->disconnect();
    delete dataBase_;
    dataBase_ = 0;
  }
  setOpen(FALSE);
  setOpenError(FALSE);
}

bool SqliteDriver::hasFeature(QSqlDriver::DriverFeature feature) const
{
  switch (feature) {
    case QSqlDriver::Transactions:
    case QSqlDriver::QuerySize:
    case QSqlDriver::Unicode:
    case QSqlDriver::BLOB:
      return true;
    case QSqlDriver::PreparedQueries:
    case QSqlDriver::NamedPlaceholders:
    case QSqlDriver::PositionalPlaceholders:
      return false;
  }
}

bool SqliteDriver::beginTransaction()
{
  if (!isOpen() || !dataBase_)
    return false;

  dataBase_->start_transaction();
  return true;
}

bool SqliteDriver::commitTransaction()
{
  if (!isOpen() || !dataBase_)
    return false;

  dataBase_->commit_transaction();
  return true;
}

bool SqliteDriver::rollbackTransaction()
{
  if (!isOpen() || !dataBase_)
    return false;

  dataBase_->rollback_transaction();
  return true;
}

QSqlIndex SqliteDriver::primaryIndex2(const QString &tblname) const
{
  QSqlRecordInfo rec(recordInfo(tblname));     // expensive :(

  if (!isOpen() || !dataBase_)
    return QSqlIndex();

  QSqlQuery q = createQuery();
  q.setForwardOnly(TRUE);
  // finrst find a UNIQUE INDEX
  q.exec("PRAGMA index_list('" + tblname + "');");
  QString indexname;
  while (q.next()) {
    if (q.value(2).toInt() == 1) {
      indexname = q.value(1).toString();
      break;
    }
  }
  if (indexname.isEmpty())
    return QSqlIndex();

  q.exec("PRAGMA index_info('" + indexname + "');");

  QSqlIndex index(tblname, indexname);
  while (q.next()) {
    QString name = q.value(2).toString();
    QSqlVariant::Type type = QSqlVariant::Invalid;
    if (rec.contains(name))
      type = rec.find(name).type();
    index.append(QSqlField(name, type));
  }
  return index;
}

QSqlIndex SqliteDriver::primaryIndex(const QString &tablename) const
{
  QSqlIndex idx(tablename);
  if (!isOpen() || !dataBase_)
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

QSqlQuery SqliteDriver::createQuery() const
{
  return QSqlQuery(new SqliteResult(this));
}

QSqlRecord SqliteDriver::record2(const QString &tblname) const
{
  if (!isOpen() || !dataBase_)
    return QSqlRecord();

  return recordInfo(tblname).toRecord();
}

QSqlRecord SqliteDriver::record(const QString &tablename) const
{
  QSqlRecord fil;
  if (!isOpen() || !dataBase_)
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

QSqlRecord SqliteDriver::record(const QSqlQuery &query) const
{
  return QSqlRecord();
}

QSqlRecordInfo SqliteDriver::recordInfo2(const QString &tbl) const
{
  if (!isOpen() || !dataBase_)
    return QSqlRecordInfo();

  QSqlQuery q = createQuery();
  q.setForwardOnly(TRUE);
  q.exec("SELECT * FROM " + tbl + " LIMIT 1");
  return recordInfo(q);
}

QSqlRecordInfo SqliteDriver::recordInfo(const QString &tablename) const
{
  QSqlRecordInfo info;
  if (!isOpen() || !dataBase_)
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
  }

  delete mtd;
  return info;
}

QSqlRecordInfo SqliteDriver::recordInfo(const QSqlQuery &query) const
{
  return QSqlRecordInfo();
}

QStringList SqliteDriver::tables(const QString &typeName) const
{
  QStringList res;
  if (!isOpen() || !dataBase_)
    return res;
  int type = typeName.toInt();

  QSqlQuery q = createQuery();
  q.setForwardOnly(TRUE);
#if (QT_VERSION-0 >= 0x030000)
  if ((type & (int)QSql::Tables) && (type & (int)QSql::Views))
    q.exec("SELECT name FROM sqlite_master WHERE type='table' OR type='view'");
  else if (typeName.isEmpty() || (type & (int)QSql::Tables))
    q.exec("SELECT name FROM sqlite_master WHERE type='table'");
  else if (type & (int)QSql::Views)
    q.exec("SELECT name FROM sqlite_master WHERE type='view'");
#else
  q.exec("SELECT name FROM sqlite_master WHERE type='table' OR type='view'");
#endif


  if (q.isActive()) {
    while (q.next())
      res.append(q.value(0).toString());
  }

#if (QT_VERSION-0 >= 0x030000)
  if (type & (int)QSql::SystemTables) {
    // there are no internal tables beside this one:
    res.append("sqlite_master");
  }
#endif

  return res;
}

QString SqliteDriver::formatDatabaseName(const QString &name)
{
  return AQ_DISKCACHE_DIRPATH + "/." + name;
}

SqliteResult::SqliteResult(const QSqlDriver *d) : QSqlResult(d), driver(0), dataSet(0)
{
  driver = static_cast <const SqliteDriver *>(d);
}

SqliteResult::~SqliteResult()
{
  if (dataSet)
    delete dataSet;
}

bool SqliteResult::reset(const QString &q)
{
  if (!driver)
    return false;

  if (!driver->isOpen() || driver->isOpenError())
    return false;

  setActive(false);
  setAt(QSql::BeforeFirst);
  query = q.stripWhiteSpace();
  if (q.find("select", 0, false) == 0)
    setSelect(true);
  else
    setSelect(false);
  query.replace("'true'", "'1'");
  query.replace("'false'", "'0'");
  query.replace("=;", "= NULL;");
  while (query.endsWith(";"))
    query.truncate(query.length() - 1);
  if (query.upper().endsWith("NOWAIT"))
    query.truncate(query.length() - 6);
  if (query.upper().endsWith("FOR UPDATE"))
    query.truncate(query.length() - 10);
  if (!isSelect()) {
    if (query.find("CREATE TABLE", 0, false) == 0) {
      Dataset *ds = ((SqliteDriver *) driver) ->dataBase() ->CreateDataset();

      if (!ds)
        return false;

      if (!ds->exec(query.latin1())) {
        delete ds;
        return false;
      }
      delete ds;
    } else {
      if (dataSet)
        delete dataSet;
      dataSet = ((SqliteDriver *) driver) ->dataBase() ->CreateDataset();
      if (!dataSet->exec(query.latin1()))
        return false;
    }

    return true;
  }

  if (dataSet)
    delete dataSet;
  dataSet = ((SqliteDriver *) driver) ->dataBase() ->CreateDataset();
  if (dataSet->query(query.latin1())) {
    setActive(true);
    return true;
  } else
    return false;
}

QVariant SqliteResult::data(int i)
{
  if (!dataSet)
    return QVariant();

  if (i >= dataSet->fieldCount()) {
#ifdef FL_DEBUG
    qWarning("SqliteResult::data: column %d out of range", i);
#endif
    return QVariant();
  }

  if (qstrcmp(dataSet->fieldName(i), "binario") == 0) { // <- esto es un hack para guardar hexadecimal y interpretar binario.
    QString str(dataSet->fv(dataSet->fieldName(i)).get_asString().c_str());
    QByteArray ba;
    QDataStream dts(ba, IO_WriteOnly);
    uint pos = 0;
    while (pos < str.length()) {
      dts << (Q_UINT8) str.mid(pos, 2).toUShort(0, 16);
      pos += 2;
    }
    return ba;
  }

  QVariant v = QVariant(QString(dataSet->fv(dataSet->fieldName(i)).get_asString().c_str()));
  if (v.toString().isEmpty()) {
    QVariant vv;
    fType type = dataSet->fv(dataSet->fieldName(i)).get_fType();
    if (!type)
      vv.cast(QVariant::String);
    else
      vv.cast(QVariant::Double);
    return vv;
  } else
    return v;
}

bool SqliteResult::fetch(int i)
{
  if (!dataSet)
    return false;

  if (!isActive())
    return false;

  if (i < 0)
    return false;

  if (i >= dataSet->num_rows())
    return false;

  if (at() == i)
    return true;

  setAt(i);
  return dataSet->seek(i);
}

bool SqliteResult::isNull(int i)
{
  if (!dataSet)
    return false;
  return dataSet->fv(dataSet->fieldName(i)).get_isNull();
}

bool SqliteResult::fetchFirst()
{
  return fetch(0);
}

bool SqliteResult::fetchLast()
{
  if (!dataSet)
    return false;
  return fetch(dataSet->num_rows() - 1);
}

int SqliteResult::size()
{
  if (!dataSet)
    return 0;
  return dataSet->num_rows();
}

int SqliteResult::numRowsAffected()
{
  if (!dataSet)
    return 0;
  return dataSet->num_rows();
}

void SqliteDriver::Mr_Proper()
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
  db_->dbAux()->transaction();
  QRegExp rx("^.*[\\d][\\d][\\d][\\d].[\\d][\\d].*[\\d][\\d]$");
  QRegExp rx2("^.*alteredtable[\\d][\\d][\\d][\\d].*$");
  QSqlQuery qry(QString::null, db_->dbAux());
  QSqlQuery qry2(QString::null, db_->dbAux());
  int steps = 0;
  QString item;

  QRegExp rx3("^.*\\d{6,9}$");
  QStringList listOldBks(tables("").grep(rx3));

  qry.exec("select nombre from flfiles");
  FLUtil::createProgressDialog(tr("Borrando backups"), listOldBks.size() + qry.size() + 5);
  while (qry.next()) {
    item = qry.value(0).toString();
    if (item.contains(rx) || item.contains(rx2)) {
      FLUtil::setLabelText(tr("Borrando regisro %1").arg(item));
      qry2.exec("delete from flfiles where nombre = '" + item + "'");
#ifdef FL_DEBUG
      qWarning("delete from flfiles where nombre = '" + item + "'");
#endif
      if (item.contains("alteredtable")) {
        if (tables("").contains(item.replace(".mtd", ""))) {
          FLUtil::setLabelText(tr("Borrando tabla %1").arg(item));
          qry2.exec("drop table " + item.replace(".mtd", ""));
#ifdef FL_DEBUG
          qWarning("drop table " + item.replace(".mtd", ""));
#endif
        }
      }
    }
    FLUtil::setProgress(++steps);
  }

  for (QStringList::Iterator it = listOldBks.begin(); it != listOldBks.end(); ++it) {
    item = *it;
    if (tables("").contains(item)) {
      FLUtil::setLabelText(tr("Borrando tabla %1").arg(item));
      qry2.exec("drop table " + item);
#ifdef FL_DEBUG
      qWarning("drop table " + item);
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

  FLUtil::setLabelText(tr("Vacunando base de datos"));
  FLUtil::setProgress(++steps);
  qry2.exec("vacuum");
  FLUtil::setProgress(++steps);
#ifdef FL_DEBUG
  qWarning("vacuum");
#endif
  FLUtil::destroyProgressDialog();
}
