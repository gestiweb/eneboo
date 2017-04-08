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
#include <qbuffer.h>

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

extern QSInterpreter *globalAQSInterpreter;

static inline void throwMsgWarning(FLSqlDatabase *db, const QString &msg)
{
  if (!db->qsaExceptions() || !globalAQSInterpreter) {
    qWarning(msg);
    return;
  }
  if (globalAQSInterpreter->isRunning())
    globalAQSInterpreter->throwError(msg);
  else
    qWarning(msg);
}

FLManager::FLManager(FLSqlDatabase *db) :
  cacheMetaData_(0),
  cacheAction_(0),
  cacheMetaDataSys_(0),
  db_(db),
  initCount_(0)
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

  *listTables_ = db_->dbAux()->tables();
#endif
}

void FLManager::init()
{
  ++initCount_;

#ifndef FL_QUICK_CLIENT
  FLTableMetaData *tmpTMD;

  tmpTMD = createSystemTable("flmetadata");
  tmpTMD = createSystemTable("flseqs");
  tmpTMD = createSystemTable("flreports");

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
}

bool FLManager::existsTable(const QString &n, bool cache) const
{
#ifndef FL_QUICK_CLIENT
  if (!db_ || !db_->dbAux() || n.isEmpty())
    return false;
  if (cache && listTables_) {
    QStringList::const_iterator it(listTables_->find(n));
    return ((it != listTables_->constEnd()) ? true : db_->existsTable(n));
  } else
    return db_->existsTable(n);
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

bool FLManager::alterTable(const QString &mtd1, const QString &mtd2, const QString &key)
{
  return db_->alterTable(mtd1, mtd2, key);
}

bool FLManager::checkMetaData(FLTableMetaData *tmd1, FLTableMetaData *tmd2)
{
  if (!tmd1 || !tmd2)
    return (tmd1 == tmd2);

  const FLTableMetaData::FLFieldMetaDataList *fieldList = tmd1->fieldList();
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
    if (field2->isCheck())
      continue;
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
    if (field2->isCheck())
      continue;
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
  if (!db_->dbAux())
    return false;

  QSqlCursor c("flmetadata", true, db_->dbAux());
  c.setForwardOnly(true);
  c.setFilter(QString::fromLatin1("tabla='") + n + QString::fromLatin1("'"));
  c.select();
  if (c.next()) {
    QString mtd(db_->managerModules()->contentCached(n + QString::fromLatin1(".mtd")));
    return alterTable(c.value("xml").toString(), mtd);
  } else
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
  QString n, a, ol, rX, assocBy, assocWith, so;
  bool aN = true, iPK = true, c = false, iNX = false, uNI = false,
       coun = false, oT = false, vG = true, fullCalc = false, trimm = false;
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
      if (e.tagName() == "fullycalculated") {
        fullCalc = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "trimmed") {
        trimm = (e.text() == "true");
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
      if (e.tagName() == "searchoptions") {
        so = e.text();
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  FLFieldMetaData *f = new FLFieldMetaData(n, FLUtil::translate("MetaData", a), aN, iPK, t, l, c, v, ed, pI, pD, iNX,
                                           uNI, coun, dV, oT, rX, vG, true, ck);
  f->setFullyCalculated(fullCalc);
  f->setTrimed(trimm);

  if (!ol.isEmpty())
    f->setOptionsList(ol);
  if (!so.isEmpty())
    f->setSearchOptions(so);
  
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

  QString name, a, q, ftsfun, archiveWhere, transBehavior = "strict";
  bool v = true, ed = true, cw = false, dl = false;

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
      if (e.tagName() == "concurWarn") {
        cw = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "detectLocks") {
        dl = (e.text() == "true");
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "FTSFunction") {
        ftsfun = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "archiveWhere") {
        archiveWhere = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "transactionBehavior") {
        transBehavior = e.text();
        no = no.nextSibling();
        continue;
      }
      
    }
    no = no.nextSibling();
  }

  FLTableMetaData *tmd = new FLTableMetaData(name, a, q);
  FLCompoundKey *cK = 0;
  QStringList assocs;
  tmd->setFTSFunction(ftsfun);
  tmd->setArchiveWhere(archiveWhere);
  tmd->setConcurWarn(cw);
  tmd->setDetectLocks(dl);
  tmd->setTransactionBehavior(transBehavior);
  
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
      QStringList fL(qry->fieldList());
      QString table, field;
      QStringList fields(QStringList::split(',', tmd->fieldsNames()));
      bool fieldsEmpty = fields.isEmpty();

      for (QStringList::Iterator it = fL.begin(); it != fL.end(); ++it) {
        table = (*it).section('.', 0, 0);
        field = (*it).section('.', 1, 1);

        if (!fieldsEmpty && table == name && fields.find(field.lower()) != fields.end())
          continue;

        FLTableMetaData *mtdAux = metadata(table, true);
        if (mtdAux) {
          FLFieldMetaData *fmtdAux = mtdAux->field(field);
          if (fmtdAux) {
            bool isForeignKey = false;
            if (fmtdAux->isPrimaryKey() && table != name) {
              isForeignKey = true;
              fmtdAux = new FLFieldMetaData(fmtdAux);
              fmtdAux->setIsPrimaryKey(false);
              fmtdAux->setEditable(false);
            }
            // ###
            bool newRef = !isForeignKey;
            QString fmtdAuxName(fmtdAux->name().lower());
            if (!fmtdAuxName.contains(".")) {
              QStringList fieldsAux(QStringList::split(',', tmd->fieldsNames()));
              if (fieldsAux.find(fmtdAuxName) != fieldsAux.end()) {
                if (!isForeignKey)
                  fmtdAux = new FLFieldMetaData(fmtdAux);
                fmtdAux->setName(table + QString::fromLatin1(".") + field);
                newRef = false;
              }
            }
            if (newRef)
              fmtdAux->ref();
            // ###
            tmd->addFieldMD(fmtdAux);
          }
        }
      }
      qry->deleteLater();
    }
  }

  FLAccessControlLists *acl = aqApp->acl();
  if (acl)
    acl->process(tmd);
  return tmd;
}

FLTableMetaData *FLManager::metadata(const QString &n, bool quick)
{
#ifdef QSDEBUGGER
  FLTableMetaData *ret = metadataDev(n, quick);
  if (!quick && ret && aqApp->consoleShown() &&
      !ret->isQuery() && db_->mismatchedTable(n, ret)) {
    QString msg(QApplication::tr(
                  "La estructura de los metadatos de la tabla '%1' y su "
                  "estructura interna en la base de datos no coinciden. "
                  "Debe regenerar la base de datos."
                ));
    throwMsgWarning(db_, msg.arg(n));
  }
  return ret;
#else
  if (n.isEmpty() || !db_->dbAux())
    return 0;

  FLTableMetaData *ret = 0;
  FLAccessControlLists *acl = 0;
  QString key(n);
  QString stream;
  bool isSysTable = (n.left(3) == "sys" || isSystemTable(n));

  if (!isSysTable) {
    stream = db_->managerModules()->contentCached(
               n + QString::fromLatin1(".mtd"), &key
             );
    if (stream.isEmpty()) {
#ifdef FL_DEBUG
      qWarning("FLManager : " +
               QApplication::tr("Error al cargar los metadatos para la tabla %1")
               .arg(n));
#endif
      return 0;
    }
    if (key.isEmpty())
      key = n;
  }

  if (cacheMetaData_ && !isSysTable)
    ret = cacheMetaData_->find(key);
  else if (cacheMetaDataSys_ && isSysTable)
    ret = cacheMetaDataSys_->find(key);

  if (!ret) {
    if (isSysTable) {
      stream = db_->managerModules()->contentCached(
                 n + QString::fromLatin1(".mtd")
               );
      if (stream.isEmpty()) {
#ifdef FL_DEBUG
        qWarning("FLManager : " +
                 QApplication::tr("Error al cargar los metadatos para la tabla %1")
                 .arg(n));
#endif
        return 0;
      }
    }

    QDomDocument doc(n);
    if (!FLUtil::domDocumentSetContent(doc, stream)) {
#ifdef FL_DEBUG
      qWarning("FLManager : " +
               QApplication::tr("Error al cargar los metadatos para la tabla %1")
               .arg(n));
#endif
      return 0;
    }

    QDomElement docElem = doc.documentElement();
    ret = metadata(&docElem, quick);
    if (!ret)
      return 0;
    if (cacheMetaData_ && !isSysTable && !ret->isQuery()) {
      ret->setInCache();
      cacheMetaData_->insert(key, ret);
    } else if (cacheMetaDataSys_ && isSysTable) {
      ret->setInCache();
      cacheMetaDataSys_->insert(key, ret);
    }
      //qWarning(QString("FLManager : Inicializada la tabla %1").arg(n));    
      QString idMod(db_->managerModules()->idModuleOfFile(n + QString::fromLatin1(".mtd")));
      QString functionQSA = idMod + QString::fromLatin1(".metadata_afterInit") ;
      
      QValueList<QVariant> vargs = QValueList<QVariant>();
      vargs.append(n);
      QSArgumentList args = QSArgumentList(vargs);
      QVariant v = aqApp->call(functionQSA,args, 0).variant();
      QStringList ret = v.asStringList();
      
      
  } else
    acl = aqApp->acl();

  if (!ret->fieldsNamesUnlock().isEmpty())
    ret = new FLTableMetaData(ret);

  if (acl)
    acl->process(ret);

  if (!quick && !isSysTable && aqApp->consoleShown() &&
      !ret->isQuery() && db_->mismatchedTable(n, ret)) {
    QString msg(QApplication::tr(
                  "La estructura de los metadatos de la tabla '%1' y su "
                  "estructura interna en la base de datos no coinciden. "
                  "Debe regenerar la base de datos."
                ));
    throwMsgWarning(db_, msg.arg(n));
  }

  return ret;
#endif
}

FLTableMetaData *FLManager::metadataDev(const QString &n, bool quick)
{
  if (n.isEmpty() || !db_->dbAux())
    return 0;

  FLTableMetaData *ret = 0;
  QDomDocument doc(n);
  QDomElement docElem;
  QString key;
  QString *dictKey = 0;
  QString stream;
  bool newTable = false;
  bool notSysTable = n.left(3) != "sys" && !isSystemTable(n);
  bool readStream = (notSysTable && !quick);

  if (readStream)
    stream = db_->managerModules()->contentCached(n + QString::fromLatin1(".mtd"), &key);
  if (!notSysTable)
    dictKey = new QString(n);

#ifndef FL_QUICK_CLIENT
  if (!quick) {
    if (db_->canRegenTables() && notSysTable) {
      QSqlRecord *buffer = 0;
      QString key2;
      delete dictKey;
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
    } else if (cacheMetaDataSys_ && !notSysTable) {
      ret = cacheMetaDataSys_->find(*dictKey);
    }
    if (ret) {
      if (!ret->fieldsNamesUnlock().isEmpty())
        ret = new FLTableMetaData(ret);
      FLAccessControlLists *acl = aqApp->acl();
      if (acl)
        acl->process(ret);
      if (quick)
        delete dictKey;
      return ret;
    }
  }

  if (!readStream)
    stream = db_->managerModules()->contentCached(n + QString::fromLatin1(".mtd"), &key);

  if (!FLUtil::domDocumentSetContent(doc, stream)) {
#ifdef FL_DEBUG
    qWarning("FLManager : " + QApplication::tr("Error al cargar los metadatos para la tabla %1").arg(n));
#endif
    if (quick)
      delete dictKey;
    return 0;
  }

  docElem = doc.documentElement();
  ret = metadata(&docElem, quick);
  if (!ret)
    return 0;
  if (dictKey && (!notSysTable || !ret->isQuery())) {
    if (cacheMetaData_ && notSysTable) {
      ret->setInCache();
      cacheMetaData_->insert(*dictKey, ret);
    } else if (cacheMetaDataSys_ && !notSysTable) {
      ret->setInCache();
      cacheMetaDataSys_->insert(*dictKey, ret);
    }
    if (quick)
      delete dictKey;
    if (!ret->fieldsNamesUnlock().isEmpty())
      ret = new FLTableMetaData(ret);
  }
  return ret;
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

        QDomNodeList nl(e.elementsByTagName("name"));
        if (nl.count() == 0) {
          qWarning("Debe indicar la etiqueta <name> en acción '%s'", n.latin1());
          no = no.nextSibling();
          continue;
        } else {
          QDomElement it(nl.item(0).toElement());
          if (it.text() != n) {
            no = no.nextSibling();
            continue;
          }
        }

        QDomNode no2 = e.firstChild();
        QDomElement e2 = no2.toElement();
        
        bool isAValidName = false;
                
        while (!no2.isNull()) {
            e2 = no2.toElement();
            if (!e2.isNull()) {
                if (e2.tagName() == "name") {
                    isAValidName = (e2.text() == n);
                    break;
                }
            }
            no2 = no2.nextSibling();            
        }
        no2 = e.firstChild();
        e2 = no2.toElement();
        if (isAValidName) {
            if (!e2.isNull()) {
                if (e2.tagName() == "name") {
                    // Primer tag es name, correcto.
                } else {
                    qDebug("WARN: El primer tag de la accion '" + n + "' no es name, se encontro '" + e2.tagName() + "'");
                }
            } else {
                qDebug("WARN: Se encontro una accion vacia para '" + n + "'.");
            }
            
        }
        while (isAValidName && !no2.isNull()) {
          QDomElement e2 = no2.toElement();

          if (!e2.isNull()) {
            if (e2.tagName() == "name") {
              // if (e2.text() != n) break;
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
              QString txt(e2.text());
              if (txt.contains("QT_TRANSLATE_NOOP")) {
                txt = txt.mid(30, txt.length() - 32);
                txt = FLUtil::translate("MetaData", txt);
              }
              a->setCaption(txt);
              no2 = no2.nextSibling();
              continue;
            }
            if (e2.tagName() == "description") {
              QString txt(e2.text());
              if (txt.contains("QT_TRANSLATE_NOOP")) {
                txt = txt.mid(30, txt.length() - 32);
                txt = FLUtil::translate("MetaData", txt);
              }
              if (a->caption() =="") a->setDescription(txt);
              no2 = no2.nextSibling();
              continue;
            }
            if (e2.tagName() == "alias") {
              QString txt(e2.text());
              if (txt.contains("QT_TRANSLATE_NOOP")) {
                txt = txt.mid(30, txt.length() - 32);
                txt = FLUtil::translate("MetaData", txt);
              }
              a->setCaption(txt);
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
  if (!mtd)
    return formatAssignValueLike(fMD->name(), fMD->type(), v, upper);

  if (fMD->isPrimaryKey())
    return formatAssignValueLike(mtd->primaryKey(true), fMD->type(), v, upper);

  QString fieldName(fMD->name());
  if (mtd->isQuery() && !fieldName.contains(".")) {
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
  if (!mtd)
    return formatAssignValue(fMD->name(), fMD->type(), v, upper);

  if (fMD->isPrimaryKey())
    return formatAssignValue(mtd->primaryKey(true), fMD->type(), v, upper);

  QString fieldName(fMD->name());
  if (mtd->isQuery() && !fieldName.contains(".")) {
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
                      "flseqs,"
                      "flupdates");
//--> FLLarge único           
  if (aqApp->singleFLLarge())
               {
               systemTable = systemTable + QString::fromLatin1(",fllarge");       
               }
               
//<-- FLLarge único                      
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

// --> FLLarge único
QString tableLarge;

  if (aqApp->singleFLLarge())
	tableLarge = QString::fromLatin1("fllarge");
         else
  	{
  	tableLarge = QString::fromLatin1("fllarge_") + tableName;
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
  	}




//<-- FLLarge único

  
  /*if (!existsTable(tableLarge)) {
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
*/
  QString sha(FLUtil::sha1(largeValue));
  QString refKey(QString::fromLatin1("RK@") + tableName.left(50) + QString::fromLatin1("@") + sha);
  QSqlCursor curLarge(tableLarge, true, db_->dbAux());
  QSqlRecord *bufLarge;
  curLarge.setFilter(QString::fromLatin1("refkey='") + refKey +
                     QString::fromLatin1("'"));
  curLarge.select();
  QByteArray valor(largeValue.utf8());
     QPixmap pixmap(valor);
     QByteArray bytes;
     QBuffer buffer(bytes);
     buffer.open(IO_WriteOnly);
     pixmap.save(&buffer, "PNG"); // writes pixmap into bytes in PNG format
    if (curLarge.next()) {
    if (curLarge.value("sha").toString() != sha) {
      bufLarge = curLarge.primeUpdate();
      bufLarge->setValue("sha", sha);
      bufLarge->setValue("contenido", largeValue);
      bufLarge->setValue("imagen",bytes.data());
      curLarge.update();
    }
  } else {
    bufLarge = curLarge.primeInsert();
    bufLarge->setValue("refkey", refKey);
    bufLarge->setValue("sha", sha);
    bufLarge->setValue("contenido", largeValue);
    bufLarge->setValue("imagen",bytes.data());
    curLarge.insert();
  }

  return refKey;
}

QVariant FLManager::fetchLargeValue(const QString &refKey) const
{
  if (refKey.left(3) != "RK@")
    return QVariant();
// --> FLLarge único
QString tableLarge;

  if (aqApp->singleFLLarge())
  tableLarge = QString::fromLatin1("fllarge");
  else
  tableLarge = QString::fromLatin1("fllarge_") + refKey.section('@', 1, 1);

  
//<-- FLLarge único

  if (!existsTable(tableLarge))
    return QVariant();

  QSqlQuery qryLarge(QString::null, db_->db());
  if (qryLarge.exec(QString::fromLatin1("SELECT contenido FROM ") + tableLarge +
                    QString::fromLatin1(" WHERE refkey='") + refKey + QString::fromLatin1("'")) &&
      qryLarge.next()) {
    QVariant v(qryLarge.value(0));
    return v;
  }
  return QVariant();
}
