/***************************************************************************
 FLManager.cpp
 -------------------
 begin                : Sun Jul 15 2001
 copyright            : (C) 2001-2005 by InfoSiAL S.L.
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

#include <qdom.h>
#include <qdict.h>
#include <qstringlist.h>

#include "FLManager.h"
#include "FLFieldMetaData.h"
#include "FLRelationMetaData.h"
#include "FLCompoundKey.h"
#include "FLSqlQuery.h"
#include "FLParameterQuery.h"
#include "FLGroupByQuery.h"
#include "FLUtil.h"
#include "FLAction.h"
#include "FLSqlCursor.h"
#include "FLManagerModules.h"
#include "FLDiskCache.h"
#include "FLAccessControlLists.h"
#include "FLSqlDatabase.h"
#include "FLApplication.h"

#include "AQConfig.h"

FLManager::FLManager(FLSqlDatabase *db) :
  cacheMetaData_(0),
  cacheAction_(0),
  cacheMetaDataSys_(0),
  cacheLargeValues_(0),
  db_(db)
{
#ifndef FL_QUICK_CLIENT
  listTables_ = 0;
  dictKeyMetaData_ = 0;
#endif
}

FLManager::~FLManager()
{
  finish();
}

void FLManager::loadTables()
{
#ifndef FL_QUICK_CLIENT
  if (!db_->dbAux())
    return;
  if (!listTables_)
    listTables_ = new QStringList();
  else
    listTables_->clear();

  *listTables_ = db_->dbAux() ->tables();
#endif
}

void FLManager::init()
{
#ifndef FL_QUICK_CLIENT
  FLTableMetaData *tmpTMD;

  tmpTMD = createSystemTable("flmetadata");
  tmpTMD = createSystemTable("flseqs");

  if (!db_->dbAux())
    return;

  QSqlQuery q(QString::null, db_->dbAux());
  q.setForwardOnly(true);

  tmpTMD = createSystemTable("flsettings");

  if (!q.exec("SELECT * FROM flsettings WHERE flkey = 'sysmodver'")) {
    q.exec("DROP TABLE flsettings CASCADE");
    tmpTMD = createSystemTable("flsettings");
  }

  if (!dictKeyMetaData_) {
    dictKeyMetaData_ = new QDict<QString>(277);
    dictKeyMetaData_->setAutoDelete(true);
  } else
    dictKeyMetaData_->clear();

  q.exec("SELECT tabla,xml FROM flmetadata");
  while (q.next())
    dictKeyMetaData_->insert(q.value(0).toString(), new QString(q.value(1).toString()));

  q.exec("SELECT * FROM flsettings WHERE flkey = 'sysmodver'");
  if (!q.next()) {
    q.exec("DROP TABLE flmetadata CASCADE");
    tmpTMD = createSystemTable("flmetadata");

    QSqlCursor c("flmetadata", true, db_->dbAux());
    QSqlRecord *buffer;
    QDictIterator<QString> it(*dictKeyMetaData_);

    for (; it.current(); ++it) {
      buffer = c.primeInsert();
      buffer->setValue("tabla", it.currentKey());
      buffer->setValue("xml", *(*it));
      c.insert();
    }
  }
#endif

  if (!cacheMetaData_) {
    cacheMetaData_ = new QDict<FLTableMetaData>(277);
    cacheMetaData_->setAutoDelete(true);
  }

  if (!cacheAction_) {
    cacheAction_ = new QDict<FLAction>(277);
    cacheAction_->setAutoDelete(true);
  }

  if (!cacheMetaDataSys_) {
    cacheMetaDataSys_ = new QDict<FLTableMetaData>(277);
    cacheMetaDataSys_->setAutoDelete(true);
  }

  if (!cacheLargeValues_) {
    cacheLargeValues_ = new QDict<QVariant>(277);
    cacheLargeValues_->setAutoDelete(true);
  }
}

bool FLManager::existsTable(const QString &n, bool cache) const
{
#ifndef FL_QUICK_CLIENT
  if (!db_->dbAux() || n.isEmpty())
    return false;
  if (cache && listTables_)
    return (listTables_->contains(n) ? true : (db_->dbAux() ->tables().contains(n)));
  else
    return (db_->dbAux() ->tables().contains(n));
#else
  return true;
#endif
}

void FLManager::finish()
{
#ifndef FL_QUICK_CLIENT
  if (dictKeyMetaData_) {
    delete dictKeyMetaData_;
    dictKeyMetaData_ = 0;
  }

  if (listTables_) {
    delete listTables_;
    listTables_ = 0;
  }
#endif

  if (cacheMetaData_) {
    delete cacheMetaData_;
    cacheMetaData_ = 0;
  }

  if (cacheAction_) {
    delete cacheAction_;
    cacheAction_ = 0;
  }

  if (cacheLargeValues_) {
    delete cacheLargeValues_;
    cacheLargeValues_ = 0;
  }
}

FLTableMetaData *FLManager::createTable(FLTableMetaData *tmd)
{
#ifndef FL_QUICK_CLIENT
  if (!tmd)
    return 0;

  if (tmd->isQuery() || existsTable(tmd->name(), false))
    return tmd;

  if (!db_->createTable(tmd)) {
#ifdef FL_DEBUG
    qWarning("FLManager : " + QApplication::tr("No se ha podido crear la tabla ") + tmd->name());
#endif
    return 0;
  }
#endif //FL_QUICK_CLIENT
  return tmd;
}

FLTableMetaData *FLManager::createTable(const QString &n)
{
  FLTableMetaData *tmd = metadata(n);
#ifndef FL_QUICK_CLIENT
  if (!tmd)
    return 0;
  if (existsTable(tmd->name())) {
    listTables_->append(n);
    return tmd;
  }
  return createTable(tmd);
#else
  return tmd;
#endif
}

bool FLManager::checkMetaData(const QString &n)
{
#ifndef FL_QUICK_CLIENT
  QDomDocument doc(n);
  QDomElement docElem;

  QString mtd1 = db_->managerModules()->contentCached(n + ".mtd");
  if (!db_->dbAux())
    return true;

  QString mtd2;
  QSqlCursor c("flmetadata", true, db_->dbAux());
  c.setForwardOnly(true);
  c.setFilter(QString::fromLatin1("tabla='") + n + QString::fromLatin1("'"));
  c.select();
  if (c.next())
    mtd2 = c.value("xml").toString();
  else
    return true;

  return !!checkMetaData(mtd1, mtd2);
#else
  return true;
#endif
}

bool FLManager::alterTable(const QString &mtd1, const QString &mtd2, const QString &key)
{
  return db_->alterTable(mtd1, mtd2, key);
}

bool FLManager::checkMetaData(FLTableMetaData *tmd1, FLTableMetaData *tmd2)
{
  if (!tmd1 || !tmd2)
    return (tmd1 == tmd2);

  FLTableMetaData::FLFieldMetaDataList *fieldList = tmd1->fieldList();
  FLFieldMetaData *field1;
  FLFieldMetaData *field2;

  QDictIterator<FLFieldMetaData> it(*fieldList);
  while ((field1 = it.current()) != 0) {
    ++it;
    if (field1->isCheck())
      continue;
    field2 = tmd2->field(field1->name());
    if (!field2)
      return false;
    if (field1->type() != field2->type() || field1->allowNull() != field2->allowNull())
      return false;
    if (field1->isUnique() != field2->isUnique() || field1->isIndex() != field2->isIndex())
      return false;
    if (field1->length() != field2->length() || field1->partDecimal() != field2->partDecimal() || field1->partInteger()
        != field2->partInteger())
      return false;
  }

  fieldList = tmd2->fieldList();
  QDictIterator<FLFieldMetaData> it2(*fieldList);
  while ((field1 = it2.current()) != 0) {
    ++it2;
    if (field1->isCheck())
      continue;
    field2 = tmd1->field(field1->name());
    if (!field2)
      return false;
    if (field1->type() != field2->type() || field1->allowNull() != field2->allowNull())
      return false;
    if (field1->isUnique() != field2->isUnique() || field1->isIndex() != field2->isIndex())
      return false;
    if (field1->length() != field2->length() || field1->partDecimal() != field2->partDecimal() || field1->partInteger()
        != field2->partInteger())
      return false;
  }

  return true;
}

bool FLManager::alterTable(const QString &n)
{
#ifndef FL_QUICK_CLIENT
  QString mtd(db_->managerModules()->contentCached(n + QString::fromLatin1(".mtd")));

  if (!db_->dbAux())
    return false;

  QSqlCursor c("flmetadata", true, db_->dbAux());
  c.setForwardOnly(true);
  c.setFilter(QString::fromLatin1("tabla='") + n + QString::fromLatin1("'"));
  c.select();
  if (c.next())
    return alterTable(c.value("xml").toString(), mtd);
  else
    return false;
#else
  return true;
#endif
}

FLFieldMetaData *FLManager::metadataField(QDomElement *field, bool v, bool ed)
{
  if (!field)
    return 0;

  bool ck = false;
  QString n, a, ol, rX, assocBy, assocWith;
  bool aN = true, iPK = true, c = false, iNX = false, uNI = false, coun = false, oT = false, vG = true;
  int t = QVariant::Int, l = 0, pI = 4, pD = 0;
  QVariant dV = QVariant();

  QDomNode no = field->firstChild();

  while (!no.isNull()) {
    QDomElement e = no.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "relation" || e.tagName() == "associated") {
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "name") {
        n = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "alias") {
        a = e.text().mid(30, e.text().length() - 32);
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "null") {
        aN = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "pk") {
        iPK = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "type") {
        if (e.text() == "int")
          t = QVariant::Int;
        else if (e.text() == "uint")
          t = QVariant::UInt;
        else if (e.text() == "bool")
          t = QVariant::Bool;
        else if (e.text() == "double")
          t = QVariant::Double;
        else if (e.text() == "time")
          t = QVariant::Time;
        else if (e.text() == "date")
          t = QVariant::Date;
        else if (e.text() == "pixmap")
          t = QVariant::Pixmap;
        else if (e.text() == "bytearray")
          t = QVariant::ByteArray;
        else if (e.text() == "string")
          t = QVariant::String;
        else if (e.text() == "stringlist")
          t = QVariant::StringList;
        else if (e.text() == "unlock")
          t = FLFieldMetaData::Unlock;
        else if (e.text() == "serial")
          t = FLFieldMetaData::Serial;
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "length") {
        l = e.text().toInt();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "regexp") {
        rX = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "default") {
        if (e.text().contains("QT_TRANSLATE_NOOP"))
          dV = QVariant(e.text().mid(30, e.text().length() - 32));
        else
          dV = QVariant(e.text());
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "outtransaction") {
        oT = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "counter") {
        coun = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "calculated") {
        c = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "visible") {
        v = (e.text() == "true" && v);
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "visiblegrid") {
        vG = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "editable") {
        ed = (e.text() == "true" && ed);
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "partI") {
        pI = e.text().toInt();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "partD") {
        pD = e.text().toInt();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "index") {
        iNX = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "unique") {
        uNI = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "ck") {
        ck = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "optionslist") {
        ol = e.text();
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  FLFieldMetaData *f = new FLFieldMetaData(n, FLUtil::translate("MetaData", a), aN, iPK, t, l, c, v, ed, pI, pD, iNX,
                                           uNI, coun, dV, oT, rX, vG, true, ck);

  if (!ol.isEmpty())
    f->setOptionsList(ol);

  no = field->firstChild();

  while (!no.isNull()) {
    QDomElement e = no.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "relation") {
        f->addRelationMD(metadataRelation(&e));
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "associated") {
        QDomNode noas = e.firstChild();
        while (!noas.isNull()) {
          QDomElement eas = noas.toElement();
          if (!eas.isNull()) {
            if (eas.tagName() == "with") {
              assocWith = eas.text();
              noas = noas.nextSibling();
              continue;
            }
            if (eas.tagName() == "by") {
              assocBy = eas.text();
              noas = noas.nextSibling();
              continue;
            }
          }
          noas = noas.nextSibling();
        }
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  if (!assocWith.isEmpty() && !assocBy.isEmpty())
    f->setAssociatedField(assocWith, assocBy);

  return f;
}

FLRelationMetaData *FLManager::metadataRelation(QDomElement *relation)
{
  if (!relation)
    return 0;

  QString fT, fF;
  int rC = FLRelationMetaData::RELATION_M1;
  bool dC = false, uC = false, cI = true;

  QDomNode no = relation->firstChild();

  while (!no.isNull()) {
    QDomElement e = no.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "table") {
        fT = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "field") {
        fF = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "card") {
        if (e.text() == "1M") {
          rC = FLRelationMetaData::RELATION_1M;
        }
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "delC") {
        dC = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "updC") {
        uC = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "checkIn") {
        cI = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  return new FLRelationMetaData(fT, fF, rC, dC, uC, cI);
}

FLTableMetaData *FLManager::metadata(QDomElement *mtd, bool quick)
{
  if (!mtd)
    return 0;

  QString name, a, q;
  bool v = true, ed = true;

  QDomNode no = mtd->firstChild();

  while (!no.isNull()) {
    QDomElement e = no.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "field") {
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "name") {
        name = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "query") {
        q = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "alias") {
        a = e.text().mid(30, e.text().length() - 32);
        a = FLUtil::translate("MetaData", a);
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "visible") {
        v = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "editable") {
        ed = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  FLTableMetaData *tmd = new FLTableMetaData(name, a, q);
  FLCompoundKey *cK = 0;
  QStringList assocs;

  no = mtd->firstChild();

  while (!no.isNull()) {
    QDomElement e = no.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "field") {
        FLFieldMetaData *f = metadataField(&e, v, ed);
        if (!tmd)
          tmd = new FLTableMetaData(name, a, q);
        tmd->addFieldMD(f);
        if (f->isCompoundKey()) {
          if (!cK)
            cK = new FLCompoundKey();
          cK->addFieldMD(f);
        }
        if (!f->associatedFieldName().isEmpty()) {
          assocs.append(f->associatedFieldName());
          assocs.append(f->associatedFieldFilterTo());
          assocs.append(f->name());
        }
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  tmd->setCompoundKey(cK);
  QString aWith, aBy;

  for (QStringList::Iterator it = assocs.begin(); it != assocs.end(); ++it) {
    aWith = (*it);
    ++it;
    aBy = (*it);
    ++it;
    tmd->field((*it))->setAssociatedField(tmd->field(aWith), aBy);
  }

  if (!q.isEmpty() && !quick) {
    FLSqlQuery *qry = query(q, tmd);

    if (qry) {
      QStringList fL = qry->fieldList();
      QString table, field;
      QString fields = tmd->fieldsNames();

      for (QStringList::Iterator it = fL.begin(); it != fL.end(); ++it) {
        table = (*it).section('.', 0, 0);
        field = (*it).section('.', 1, 1);

        if (table == name || fields.contains(field.lower()))
          continue;

        FLTableMetaData *mtdAux = metadata(table, true);
        if (mtdAux) {
          FLFieldMetaData *fmtdAux = mtdAux->field(field);
          if (fmtdAux) {
            int typeAux = fmtdAux->type();
            if (typeAux == FLFieldMetaData::Serial)
              typeAux = QVariant::UInt;
            tmd->addFieldMD(new FLFieldMetaData(field, fmtdAux->alias(), true, false, typeAux, fmtdAux->length(),
                                                false, fmtdAux->visible(), fmtdAux->editable(), fmtdAux->partInteger(),
                                                fmtdAux->partDecimal(), false, false, false, QVariant(), false,
                                                QString::null, fmtdAux->visibleGrid(), true, false));
          }
        }
      }

      qry->deleteLater();
    }
  }

  FLAccessControlLists *acl = aqApp ->acl();
  if (acl)
    acl->process(tmd);
  return tmd;
}

void FLManager::cleanupMetaData()
{
#ifndef FL_QUICK_CLIENT
  if (!existsTable("flfiles") || !existsTable("flmetadata"))
    return;

  QSqlQuery q(QString::null, db_->dbAux());
  QSqlCursor c("flmetadata", true, db_->dbAux());
  QSqlRecord *buffer;
  QString table;

  q.setForwardOnly(true);
  c.setForwardOnly(true);

  if (!dictKeyMetaData_) {
    dictKeyMetaData_ = new QDict<QString> (277);
    dictKeyMetaData_->setAutoDelete(true);
  } else
    dictKeyMetaData_->clear();

  loadTables();
  db_->managerModules()->loadKeyFiles();
  db_->managerModules()->loadAllIdModules();
  db_->managerModules()->loadIdAreas();

  q.exec("SELECT tabla,xml FROM flmetadata");
  while (q.next())
    dictKeyMetaData_->replace(q.value(0).toString(), new QString(q.value(1).toString()));

  q.exec("SELECT nombre,sha FROM flfiles WHERE nombre LIKE '%.mtd'");
  while (q.next()) {
    table = q.value(0).toString();
    table = table.replace(".mtd", "");
    if (!existsTable(table))
      createTable(table);
    FLTableMetaData *tmd = metadata(table);
    if (!tmd)
      qWarning("FLManager::cleanupMetaData "
               + QApplication::tr("No se ha podido crear los metadatos para la tabla %1").arg(table));
    c.select("tabla='" + table + "'");
    if (c.next()) {
      buffer = c.primeUpdate();
      buffer->setValue("xml", q.value(1).toString());
      c.update();
    }
    dictKeyMetaData_->replace(table, new QString(q.value(1).toString()));
  }
#endif
}

FLTableMetaData *FLManager::metadata(const QString &n, bool quick)
{
  if (n.isEmpty() || !db_->dbAux())
    return 0;

  FLTableMetaData *ret = 0;
  QDomDocument doc(n);
  QDomElement docElem;

  QString key;
  QString *dictKey = 0;
  QString stream = db_->managerModules()->contentCached(n + QString::fromLatin1(".mtd"), &key);
  bool newTable = false;
  bool notSysTable = n.left(3) != "sys" && !isSystemTable(n);

  if (!notSysTable)
    dictKey = new QString(n);

#ifndef FL_QUICK_CLIENT
  if (!quick) {
    if (db_->canRegenTables() && notSysTable) {
      QSqlRecord *buffer = 0;
      QString key2;
      dictKey = dictKeyMetaData_->find(n);
      if (dictKey)
        key2 = *dictKey;
      if (key2.isEmpty()) {
        QSqlCursor c("flmetadata", true, db_->dbAux());
        c.setForwardOnly(true);
        c.setFilter(QString::fromLatin1("tabla='") + n + QString::fromLatin1("'"));
        c.select();
        if (c.next()) {
          c.primeDelete();
          c.del();
        }
        buffer = c.primeInsert();
        buffer->setValue("tabla", n);
        if (key.isEmpty()) {
          buffer->setValue("xml", stream);
          dictKey = new QString(stream);
          dictKeyMetaData_->replace(n, dictKey);
        } else {
          buffer->setValue("xml", key);
          dictKey = new QString(key);
          dictKeyMetaData_->replace(n, dictKey);
        }
        newTable = true;
        c.insert();
      } else {
        if (key != key2) {
          QString s;
          if (key2.left(255).find("<!DOCTYPE TMD>", 0, true) == -1)
            AQ_DISKCACHE_FIND(key2, s);
          else
            s = key2;
          bool mustAlter;
          if (key.isEmpty())
            mustAlter = !checkMetaData(s, stream);
          else
            mustAlter = (key != key2);
          if (mustAlter) {
            if (alterTable(s, stream, key2)) {
              QSqlCursor c("flmetadata", true, db_->dbAux());
              c.setForwardOnly(true);
              c.setFilter(QString::fromLatin1("tabla='") + n + QString::fromLatin1("'"));
              c.select();
              c.next();
              buffer = c.primeUpdate();
              if (key.isEmpty()) {
                buffer->setValue("xml", stream);
                dictKey = new QString(stream);
                dictKeyMetaData_->replace(n, dictKey);
              } else {
                buffer->setValue("xml", key);
                dictKey = new QString(key);
                dictKeyMetaData_->replace(n, dictKey);
              }
              c.update();
            }
          }
        }
      }
    }
  } else {
    if (key.isEmpty())
      dictKey = new QString(n);
    else
      dictKey = new QString(key);
  }
#else
  if (key.isEmpty())
    dictKey = new QString(n);
  else
    dictKey = new QString(key);
#endif //FL_QUICK_CLIENT
  if (dictKey) {
    if (cacheMetaData_ && notSysTable) {
      ret = cacheMetaData_->find(*dictKey);
    } else if (cacheMetaDataSys_) {
      ret = cacheMetaDataSys_->find(*dictKey);
    }
    if (ret) {
      FLAccessControlLists *acl = aqApp ->acl();
      if (acl)
        acl->process(ret);
      return ret;
    }
  }

  if (!FLUtil::domDocumentSetContent(doc, stream)) {
#ifdef FL_DEBUG
    qWarning("FLManager : " + QApplication::tr("Error al cargar los metadatos para la tabla %1").arg(n));
#endif
    return 0;
  }

  docElem = doc.documentElement();
  ret = metadata(&docElem, quick);
  if (dictKey && !ret->isQuery()) {
    if (cacheMetaData_ && notSysTable) {
      cacheMetaData_->insert(*dictKey, ret);
    } else if (cacheMetaDataSys_) {
      cacheMetaDataSys_->insert(*dictKey, ret);
    }
#ifdef FL_QUICK_CLIENT
    delete dictKey;
#endif
  }
  return ret;
}

FLGroupByQuery *FLManager::queryGroup(QDomElement *group)
{
  if (!group)
    return 0;

  QString level = QString::null, field = QString::null;

  QDomNode no = group->firstChild();

  while (!no.isNull()) {
    QDomElement e = no.toElement();

    if (!e.isNull()) {
      if (e.tagName() == "level") {
        level = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "field") {
        field = e.text();
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  return new FLGroupByQuery(level.toInt(), field);
}

FLParameterQuery *FLManager::queryParameter(QDomElement *parameter)
{
  if (!parameter)
    return 0;

  QString name = QString::null, alias = QString::null;
  int t = QVariant::Int;

  QDomNode no = parameter->firstChild();

  while (!no.isNull()) {
    QDomElement e = no.toElement();

    if (!e.isNull()) {
      if (e.tagName() == "name") {
        name = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "alias") {
        QString s = e.text().left(e.text().length() - 2);

        alias = s.right(s.length() - 29);
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "type") {
        if (e.text() == "int")
          t = QVariant::Int;
        if (e.text() == "uint")
          t = QVariant::UInt;
        if (e.text() == "bool")
          t = QVariant::Bool;
        if (e.text() == "double")
          t = QVariant::Double;
        if (e.text() == "time")
          t = QVariant::Time;
        if (e.text() == "date")
          t = QVariant::Date;
        if (e.text() == "pixmap")
          t = QVariant::Pixmap;
        if (e.text() == "string")
          t = QVariant::String;
        if (e.text() == "stringlist")
          t = QVariant::StringList;
        if (e.text() == "serial")
          t = FLFieldMetaData::Serial;
        else if (e.text() == "bytearray")
          t = QVariant::ByteArray;
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  return new FLParameterQuery(name, FLUtil::translate("Queries", alias), t);
}

FLSqlQuery *FLManager::query(const QString &n, QObject *parent)
{
  FLSqlQuery *q;
  QDomDocument doc(n);

  if (!FLUtil::domDocumentSetContent(
        doc, db_->managerModules()->contentCached(n + QString::fromLatin1(".qry"))
      )) {
#ifdef FL_DEBUG
    qWarning("FLManager : " + QApplication::tr("Error al cargar la consulta %1").arg(n));
#endif

    return 0;
  }

  q = new FLSqlQuery(parent, db_->connectionName());

  QDomElement docElem = doc.documentElement();
  QDomNode no = docElem.firstChild();

  while (!no.isNull()) {
    QDomElement e = no.toElement();

    if (!e.isNull()) {
      if (e.tagName() == "name") {
        q->setName(e.text());
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "tables") {
        q->setTablesList(e.text());
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "select") {
        q->setSelect(e.text());
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "from") {
        q->setFrom(e.text());
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "where") {
        q->setWhere(e.text());
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "param") {
        q->addParameter(queryParameter(&e));
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "group") {
        q->addGroup(queryGroup(&e));
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "order") {
        q->setOrderBy(e.text());
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  return q;
}

FLAction *FLManager::action(const QString &n)
{
  FLAction *a = cacheAction_->find(n);
  if (a)
    return a;
  a = new FLAction;

  QDomDocument doc(n);
  QStringList listModules(db_->managerModules()->listAllIdModules());
  QString contentActions;

  for (QStringList::Iterator it = listModules.begin(); it != listModules.end(); ++it) {
    contentActions = db_->managerModules()->contentCached((*it) + QString::fromLatin1(".xml"));
    if (contentActions.find(QString::fromLatin1("<name>") + n +
                            QString::fromLatin1("</name>"), 0, true) != -1)
      break;
  }

  if (!FLUtil::domDocumentSetContent(doc, contentActions)) {
#ifdef FL_DEBUG
    qWarning("FLManager : " + QApplication::tr("Error al cargar la accion %1").arg(n));
#endif

    return 0;
  }

  QDomElement docElem = doc.documentElement();
  QDomNode no = docElem.firstChild();

  a->setName(n);
  a->setTable(n);
  while (!no.isNull()) {
    QDomElement e = no.toElement();

    if (!e.isNull()) {
      if (e.tagName() == "action") {
        QDomNode no2 = e.firstChild();

        while (!no2.isNull()) {
          QDomElement e2 = no2.toElement();

          if (!e2.isNull()) {
            if (e2.tagName() == "name") {
              if (e2.text() != n)
                break;
              a->setName(e2.text());
              no2 = no2.nextSibling();
              continue;
            }
            if (e2.tagName() == "scriptformrecord") {
              a->setScriptFormRecord(e2.text());
              no2 = no2.nextSibling();
              continue;
            }
            if (e2.tagName() == "scriptform") {
              a->setScriptForm(e2.text());
              no2 = no2.nextSibling();
              continue;
            }
            if (e2.tagName() == "table") {
              a->setTable(e2.text());
              no2 = no2.nextSibling();
              continue;
            }
            if (e2.tagName() == "form") {
              a->setForm(e2.text());
              no2 = no2.nextSibling();
              continue;
            }
            if (e2.tagName() == "formrecord") {
              a->setFormRecord(e2.text());
              no2 = no2.nextSibling();
              continue;
            }
            if (e2.tagName() == "caption") {
              a->setCaption(e2.text());
              no2 = no2.nextSibling();
              continue;
            }
            if (e2.tagName() == "description") {
              a->setCaption(e2.text());
              no2 = no2.nextSibling();
              continue;
            }
            if (e2.tagName() == "alias") {
              a->setCaption(e2.text());
              no2 = no2.nextSibling();
              continue;
            }
          }
          no2 = no2.nextSibling();
        }
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  cacheAction_->insert(n, a);
  return a;
}

QString FLManager::formatValueLike(int t, const QVariant &v, const bool upper)
{
  return db_->formatValueLike(t, v, upper);
}

QString FLManager::formatValueLike(FLFieldMetaData *fMD, const QVariant &v, const bool upper)
{
  if (!fMD)
    return QString::null;

  return formatValueLike(fMD->type(), v, upper);
}

QString FLManager::formatAssignValueLike(const QString &fieldName, int t, const QVariant &v, const bool upper)
{
  if (fieldName.isEmpty() || t == QVariant::Invalid)
    return "1 = 1";

  bool isText = (t == QVariant::String || t == QVariant::StringList);
  QString formatV(formatValueLike(t, v, upper));

  if (formatV.isEmpty())
    return "1 = 1";

  QString fName((upper && isText ? QString::fromLatin1("upper(") + fieldName +
                 QString::fromLatin1(")") : fieldName));
  return fName + " " + formatV;
}

QString FLManager::formatAssignValueLike(const QString &fieldName, FLFieldMetaData *fMD, const QVariant &v,
                                         const bool upper)
{
  if (!fMD)
    return "1 = 1";
  return formatAssignValueLike(fieldName, fMD->type(), v, upper);
}

QString FLManager::formatAssignValueLike(FLFieldMetaData *fMD, const QVariant &v, const bool upper)
{
  if (!fMD)
    return "1 = 1";

  FLTableMetaData *mtd = fMD->metadata();
  QString fieldName(fMD->name());

  if (mtd && mtd->isQuery()) {
    QString prefixTable(mtd->name());
    FLSqlQuery *qry = query(mtd->query());

    if (qry) {
      QStringList fL(qry->fieldList());

      for (QStringList::Iterator it = fL.begin(); it != fL.end(); ++it) {
        prefixTable = (*it).section('.', 0, 0);
        if ((*it).section('.', 1, 1) == fieldName)
          break;
      }

      qry->deleteLater();
    }

    fieldName.prepend(prefixTable + QString::fromLatin1("."));
  }

  return formatAssignValueLike(fieldName, fMD->type(), v, upper);
}

QString FLManager::formatValue(int t, const QVariant &v, const bool upper)
{
  return db_->formatValue(t, v, upper);
}

QString FLManager::formatValue(FLFieldMetaData *fMD, const QVariant &v, const bool upper)
{
  if (!fMD)
    return QString::null;

  return formatValue(fMD->type(), v, upper);
}

QString FLManager::formatAssignValue(const QString &fieldName, int t, const QVariant &v, const bool upper)
{
  if (fieldName.isEmpty() || t == QVariant::Invalid)
    return "1 = 1";

  bool isText = (t == QVariant::String || t == QVariant::StringList);
  QString formatV(formatValue(t, v, upper));

  if (formatV.isEmpty())
    return "1 = 1";

  QString fName((upper && isText ? QString::fromLatin1("upper(") + fieldName +
                 QString::fromLatin1(")") : fieldName));
  return fName + "=" + formatV;
}

QString FLManager::formatAssignValue(const QString &fieldName, FLFieldMetaData *fMD, const QVariant &v,
                                     const bool upper)
{
  if (!fMD)
    return "1 = 1";
  return formatAssignValue(fieldName, fMD->type(), v, upper);
}

QString FLManager::formatAssignValue(FLFieldMetaData *fMD, const QVariant &v, const bool upper)
{
  if (!fMD)
    return "1 = 1";

  FLTableMetaData *mtd = fMD->metadata();
  QString fieldName(fMD->name());

  if (mtd && mtd->isQuery()) {
    QString prefixTable(mtd->name());
    FLSqlQuery *qry = query(mtd->query());

    if (qry) {
      QStringList fL(qry->fieldList());

      for (QStringList::Iterator it = fL.begin(); it != fL.end(); ++it) {
        prefixTable = (*it).section('.', 0, 0);
        if ((*it).section('.', 1, 1) == fieldName)
          break;
      }

      qry->deleteLater();
    }

    fieldName.prepend(prefixTable + ".");
  }

  return formatAssignValue(fieldName, fMD->type(), v, upper);
}

FLTableMetaData *FLManager::createSystemTable(const QString &n)
{
#ifndef FL_QUICK_CLIENT
  if (!existsTable(n)) {
    QDomDocument doc(n);
    QDomElement docElem;
    QFile fi(AQ_DATA +
             QString::fromLatin1("/tables/") + n +
             QString::fromLatin1(".mtd"));
    if (!fi.open(IO_ReadOnly)) {
#ifdef FL_DEBUG
      qWarning("FLManager : " + QApplication::tr("Los metadatos para %1 no están definidos").arg(n));
#endif
    } else {
      QTextStream t;
      t.setDevice(&fi);
      t.setEncoding(QTextStream::Latin1);
      QString stream = t.read();

      if (!FLUtil::domDocumentSetContent(doc, stream)) {
#ifdef FL_DEBUG
        qWarning("FLManager::createSystemTable : " + QApplication::tr("Error al cargar los metadatos para la tabla %1").arg(n));
#endif

        return 0;
      } else {
        docElem = doc.documentElement();
        FLTableMetaData *mtd = createTable(metadata(&docElem, true));
        return mtd;
      }
    }
    fi.close();
  }

  return 0;
#else

  return metadata(n, true);
#endif //FL_QUICK_CLIENT
}

bool FLManager::isSystemTable(const QString &n)
{
  if (n.left(2) != "fl")
    return false;
  QString systemTable("flfiles,"
                      "flmetadata,"
                      "flmodules,"
                      "flareas,"
                      "flserial,"
                      "flvar,"
                      "flsettings,"
                      "flseqs,");
  if (n.endsWith(".mtd"))
    return systemTable.contains(n.left(n.length() - 4));
  return systemTable.contains(n);
}

QString FLManager::storeLargeValue(FLTableMetaData *mtd, const QString &largeValue)
{
  if (largeValue.left(3) == "RK@" || !mtd)
    return QString::null;

  QString tableName(mtd->name());
  if (isSystemTable(tableName))
    return QString::null;

  QString tableLarge(QString::fromLatin1("fllarge_") + tableName);
  if (!existsTable(tableLarge)) {
    FLTableMetaData *mtdLarge = new FLTableMetaData(tableLarge, tableLarge);
    FLFieldMetaData *fieldLarge = new FLFieldMetaData("refkey", "refkey", false, true, QVariant::String, 100);
    mtdLarge->addFieldMD(fieldLarge);
    fieldLarge = new FLFieldMetaData("sha", "sha", true, false, QVariant::String, 50);
    mtdLarge->addFieldMD(fieldLarge);
    fieldLarge = new FLFieldMetaData("contenido", "contenido", true, false, QVariant::StringList);
    mtdLarge->addFieldMD(fieldLarge);
    FLTableMetaData *mtdAux = createTable(mtdLarge);
    mtd->insertChild(mtdLarge);
    if (!mtdAux)
      return QString::null;
  }

  QString sha(FLUtil::sha1(largeValue));
  QString refKey(QString::fromLatin1("RK@") + tableName.left(50) + QString::fromLatin1("@") + sha);
  QSqlCursor curLarge(tableLarge, true, db_->dbAux());
  QSqlRecord *bufLarge;
  curLarge.setFilter(QString::fromLatin1("refkey='") + refKey +
                     QString::fromLatin1("'"));
  curLarge.select();
  if (curLarge.next()) {
    if (curLarge.value("sha").toString() != sha) {
      bufLarge = curLarge.primeUpdate();
      bufLarge->setValue("sha", sha);
      bufLarge->setValue("contenido", largeValue);
      curLarge.update();
    }
  } else {
    bufLarge = curLarge.primeInsert();
    bufLarge->setValue("refkey", refKey);
    bufLarge->setValue("sha", sha);
    bufLarge->setValue("contenido", largeValue);
    curLarge.insert();
  }

  return refKey;
}

QVariant FLManager::fetchLargeValue(const QString &refKey) const
{
  if (refKey.left(3) != "RK@")
    return QVariant();

  QString sha(refKey.section('@', 2, 2));
  QVariant *cachedV = cacheLargeValues_->find(sha);
  if (cachedV)
    return QVariant(*cachedV);

  QString tableLarge(QString::fromLatin1("fllarge_") +
                     refKey.section('@', 1, 1));
  if (!existsTable(tableLarge))
    return QVariant();

  QSqlQuery qryLarge(QString::null, db_->db());
  if (qryLarge.exec(QString::fromLatin1("SELECT contenido FROM ") + tableLarge +
                    QString::fromLatin1(" WHERE refkey='") + refKey + QString::fromLatin1("'")) &&
      qryLarge.next()) {
    QVariant v(qryLarge.value(0));
    if (v.isValid())
      cacheLargeValues_->insert(sha, new QVariant(v));
    return v;
  }
  return QVariant();
}
