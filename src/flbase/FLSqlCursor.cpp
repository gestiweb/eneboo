/***************************************************************************
                     FLSqlCursor.cpp  -  description
                        -------------------
begin                : Fri Jul 27 2001
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

#include <qsqldriver.h>

#include "FLSqlCursor.h"
#include "FLSqlQuery.h"
#include "FLApplication.h"
#include "FLTableMetaData.h"
#include "FLFieldMetaData.h"
#include "FLRelationMetaData.h"
#include "FLObjectFactory.h"
#include "FLManager.h"
#include "FLFormRecordDB.h"
#include "FLAction.h"
#include "FLUtil.h"
#include "FLSqlSavePoint.h"
#include "FLSqlDatabase.h"
#include "FLSqlConnections.h"
#include "FLAccessControl.h"
#include "FLAccessControlFactory.h"
#include "FLSettings.h"


#ifdef FL_DEBUG
AQ_EXPORT long FLSqlCursor::countRefCursor = 0;
AQ_EXPORT long AQBoolFlagState::count_ = 0;
#endif

extern QSInterpreter *globalAQSInterpreter;

FLSqlCursorPrivate::FLSqlCursorPrivate() :
  buffer_(0), bufferCopy_(0), metadata_(0), cursorRelation_(0), relation_(0),
  edition_(true), browse_(true), mainFilter_(QString::null), action_(0),
  askForCancelChanges_(true), activatedCheckIntegrity_(true), ctxt_(0),
  timer_(0), activatedCommitActions_(true), populated_(false), query_(QString::null),
  inLoopRisksLocks_(false), inRisksLocks_(false), modalRisksLocks_(0), timerRisksLocks_(0),
  acTable_(0), lastAt_(-1), aclDone_(false),
  idAc_(0), idAcos_(0), idCond_(0), id_("000"),
  isQuery_(false), isSysTable_(false), rawValues_(false),
  editionStates_(0), browseStates_(0) {}

FLSqlCursorPrivate::~FLSqlCursorPrivate()
{
  if (metadata_)
    undoAcl();

  if (bufferCopy_)
    delete bufferCopy_;

  if (relation_ && relation_->deref())
    delete relation_;

  if (acTable_)
    delete acTable_;

  if (editionStates_) {
#ifdef FL_DEBUG
    editionStates_->dumpDebug();
#endif
    delete editionStates_;
#ifdef FL_DEBUG
    qWarning("AQBoolFlagState count %d", AQBoolFlagState::count_);
#endif
  }
  if (browseStates_) {
#ifdef FL_DEBUG
    browseStates_->dumpDebug();
#endif
    delete browseStates_;
#ifdef FL_DEBUG
    qWarning("AQBoolFlagState count %d", AQBoolFlagState::count_);
#endif
  }
}

void FLSqlCursorPrivate::doAcl()
{
  if (!acTable_) {
    acTable_ = FLAccessControlFactory::create("table");
    acTable_->setFromObject(metadata_);
    acosBackupTable_ = acTable_->getAcos();
    acPermBackupTable_ = acTable_->perm();
    acTable_->clear();
  }

  if (modeAccess_ == FLSqlCursor::INSERT ||
      (lastAt_ != -1 && lastAt_ == cursor_->at()))
    return;

  lastAt_ = cursor_->at();

  if (!acosCondName_.isEmpty()) {

    bool condTrue_ = false;

    switch (acosCond_) {
      case FLSqlCursor::VALUE:
        condTrue_ = (cursor_->QSqlCursor::value(acosCondName_) == acosCondVal_);
        break;
      case FLSqlCursor::REGEXP:
        condTrue_ = (QRegExp(acosCondVal_.toString()).exactMatch(cursor_->QSqlCursor::value(acosCondName_).toString()));
        break;
      case FLSqlCursor::FUNCTION: {
        buffer_ = cursor_->QSqlCursor::editBuffer(true);
        FLSqlCursorInterface *cI = FLSqlCursorInterface::sqlCursorInterface(cursor_);
        condTrue_ = (aqApp->call(acosCondName_, QSArgumentList(cI), 0).variant() == acosCondVal_);
      }
      break;
    }

    if (condTrue_) {
      if (acTable_->name() != id_) {
        acTable_->clear();
        acTable_->setName(id_);
        acTable_->setPerm(acPermTable_);
        acTable_->setAcos(acosTable_);
        acTable_->processObject(metadata_);
        aclDone_ = true;
      }
      return;
    }
  } else if (cursor_->isLocked() || (cursorRelation_ && cursorRelation_->isLocked())) {
    if (acTable_->name() != id_) {
      acTable_->clear();
      acTable_->setName(id_);
      acTable_->setPerm("r-");
      acTable_->processObject(metadata_);
      aclDone_ = true;
    }
    return;
  }

  undoAcl();
}

void FLSqlCursorPrivate::undoAcl()
{
  if (acTable_ && aclDone_) {
    aclDone_ = false;
    acTable_->clear();
    acTable_->setPerm(acPermBackupTable_);
    acTable_->setAcos(acosBackupTable_);
    acTable_->processObject(metadata_);
  }
}

void FLSqlCursorPrivate::msgBoxWarning(const QString &text,
                                       bool throwException, bool gui)
{
  if (gui && db_->interactiveGUI()) {
    qWarning(text);
    QMessageBox::warning(qApp->focusWidget(), qApp->tr("Aviso"),
                         text, QMessageBox::Ok, 0, 0);
    return;
  }
  if (!throwException || !db_->qsaExceptions() || !globalAQSInterpreter) {
    qWarning(text);
    return;
  }
  if (globalAQSInterpreter->isRunning())
    globalAQSInterpreter->throwError(text);
  else
    qWarning(text);
}

#ifdef AQ_MD5_CHECK
bool FLSqlCursorPrivate::needUpdate()
{
  if (isQuery_)
    return false;
  QString md5Str(db_->md5TuplesStateTable(curName_));
  if (md5Str.isEmpty())
    return false;
  if (md5Tuples_.isEmpty()) {
    md5Tuples_ = md5Str;
    return true;
  }
  bool need = (md5Str != md5Tuples_);
  md5Tuples_ = md5Str;
  return need;
}
#endif


FLSqlCursor::FLSqlCursor(const QString &name, bool autopopulate,
                         const QString &connectionName, FLSqlCursor *cR,
                         FLRelationMetaData *r, QObject *parent) :
  QObject(parent, name + QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz") + "-K"),
  QSqlCursor(QString::null, autopopulate, FLSqlConnections::database(connectionName)->db())
{
  d = new FLSqlCursorPrivate();
  d->cursor_ = this;
  d->db_ = FLSqlConnections::database(connectionName);

  init(name, autopopulate, cR, r);
}

FLSqlCursor::FLSqlCursor(const QString &name, bool autopopulate,
                         QSqlDatabase *db, FLSqlCursor *cR,
                         FLRelationMetaData *r, QObject *parent) :
  QObject(parent, name + QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz") + "-K"),
  QSqlCursor(QString::null, autopopulate, db)
{
  d = new FLSqlCursorPrivate();
  d->cursor_ = this;
  d->db_ = FLSqlConnections::database();

  init(name, autopopulate, cR, r);
}

void FLSqlCursor::init(const QString &name, bool autopopulate,
                       FLSqlCursor *cR, FLRelationMetaData *r)
{
  bool delMtd = d->metadata_ && !d->metadata_->aqWasDeleted() && !d->metadata_->inCache();
  if (delMtd) {
    delete(FLTableMetaData *) d->metadata_;
    d->metadata_ = 0;
  }

  d->curName_ = name;

  if (!name.isEmpty()) {
    if (!d->db_->manager()->existsTable(name))
      d->metadata_ = d->db_->manager()->createTable(name);
    else
      d->metadata_ = d->db_->manager()->metadata(name);
  }

  d->cursorRelation_ = cR;
  if (r) {
    if (d->relation_ && d->relation_->deref())
      delete d->relation_;
    //d->relation_ = new FLRelationMetaData(r->foreignTable(), r->foreignField(), r->cardinality(),
    //                                      r->deleteCascade(), r->updateCascade(), r->checkIn());
    //d->relation_->setField(r->field());
    r->ref();
    d->relation_ = r;
  } else
    d->relation_ = 0;

  if (!d->metadata_)
    return;

  d->fieldsNamesUnlock_ = d->metadata_->fieldsNamesUnlock();
  d->isQuery_ = d->metadata_->isQuery();
  d->isSysTable_ = name.left(3) == "sys" || d->db_->manager()->isSystemTable(name);

  if (d->isQuery_) {
    FLSqlQuery *qry = d->db_->manager()->query(d->metadata_->query(), this);
    d->query_ = qry->sql();
    if (qry && !d->query_.isEmpty())
      exec(d->query_);
    if (qry)
      qry->deleteLater();
  } else
    QSqlCursor::setName(d->metadata_->name(), autopopulate);

  d->modeAccess_ = BROWSE;
  if (cR && r) {
    disconnect(cR, SIGNAL(bufferChanged(const QString &)), this, SLOT(refresh(const QString &)));
    connect(cR, SIGNAL(bufferChanged(const QString &)), this, SLOT(refresh(const QString &)));
    disconnect(cR, SIGNAL(newBuffer()), this, SLOT(clearPersistentFilter()));
    connect(cR, SIGNAL(newBuffer()), this, SLOT(clearPersistentFilter()));
  } else
    seek(0);

  if (d->timer_)
    delete d->timer_;
  d->timer_ = new QTimer(this, QObject::name());
  connect(d->timer_, SIGNAL(timeout()), SLOT(refreshDelayed()));

#ifdef AQ_MD5_CHECK
  d->md5Tuples_ = d->db_->md5TuplesStateTable(d->curName_);
#endif

#ifdef FL_DEBUG
  ++countRefCursor;
#endif
}

FLSqlCursor::~FLSqlCursor()
{
  bool delMtd = d->metadata_ && !d->metadata_->aqWasDeleted() && !d->metadata_->inCache();
  //bool delMtd = d->metadata_ && !d->metadata_->aqWasDeleted() &&
  //              (!d->isSysTable_ && (d->isQuery_ || !d->fieldsNamesUnlock_.isEmpty()));
  if (!d->transactionsOpened_.isEmpty()) {
    QString t(d->metadata_ ? d->metadata_->name() : QString(QObject::name()));
    rollbackOpened(
      -1, tr("Se han detectado transacciones no finalizadas en la última operación.\n"
             "Se van a cancelar las transacciones pendientes.\n"
             "Los últimos datos introducidos no han sido guardados, por favor\n"
             "revise sus últimas acciones y repita las operaciones que no\n"
             "se han guardado.\n") +
      QString("SqlCursor::~SqlCursor: %1\n").arg(t)
    );
  }
  delete d;
  if (delMtd)
    delete(FLTableMetaData *) d->metadata_;
#ifdef FL_DEBUG
  --countRefCursor;
#endif
}

void FLSqlCursor::refreshDelayed(int msec)
{
  if (!d->timer_)
    return;
  const QObject *obj = sender();
  if (obj) {
    if (!obj->inherits("QTimer")) {
      d->timer_->start(msec, true);
      return;
    }
  } else {
    d->timer_->start(msec, true);
    return;
  }
  QString cFilter(QSqlCursor::filter());
  setFilter("");
  if (cFilter == QSqlCursor::filter() && isValid())
    return;
  QSqlCursor::select();
  int pos = atFrom();
  if (pos >= size())
    pos = size() - 1;
  if (!seek(pos, false, true)) {
    d->buffer_ = 0;
    emit newBuffer();
  } else {
    if (d->cursorRelation_ && d->relation_ && d->cursorRelation_->metadata()) {
      QVariant v(valueBuffer(d->relation_->field()));
      if (d->cursorRelation_->valueBuffer(d->relation_->foreignField()) != v)
        d->cursorRelation_->setValueBuffer(d->relation_->foreignField(), v);
    }
  }
}

void FLSqlCursor::refresh(const QString &fN)
{
  if (!d->metadata_)
    return;

  if (d->cursorRelation_ && d->relation_) {
    d->persistentFilter_ = QString::null;
    if (!d->cursorRelation_->metadata())
      return;
    if (d->cursorRelation_->metadata()->primaryKey() == fN && d->cursorRelation_->modeAccess() == INSERT)
      return;
    if (fN.isEmpty() || d->relation_->foreignField() == fN) {
      d->buffer_ = 0;
      refreshDelayed(500);
    }
  } else {
    QSqlCursor::select();
    int pos = atFrom();
    if (pos >= size())
      pos = size() - 1;
    if (!seek(pos, false, true)) {
      d->buffer_ = 0;
      emit newBuffer();
    }
  }
}

bool FLSqlCursor::setEditMode()
{
  if (d->modeAccess_ == INSERT) {
    if (!commitBuffer()) {
      return false;
    }
    refresh();
    setModeAccess(EDIT);
    return refreshBuffer();
  } else if (d->modeAccess_ == EDIT)
    return true;

  return false;
}

bool FLSqlCursor::commitBufferCursorRelation()
{
  bool ok = true;

  switch (d->modeAccess_) {
    case INSERT:
      if (d->cursorRelation_ && d->relation_) {
        if (d->cursorRelation_->metadata() && d->cursorRelation_->modeAccess() == INSERT) {
          QWidget *activeWid = aqApp->activeModalWidget();
          if (!activeWid)
            activeWid = aqApp->activePopupWidget();
          if (!activeWid)
            activeWid = aqApp->activeWindow();
          bool activeWidEnabled = activeWid ? activeWid->isEnabled() : false;

          if (activeWid && activeWidEnabled)
            activeWid->setEnabled(false);

          if (!d->cursorRelation_->commitBuffer()) {
            d->modeAccess_ = BROWSE;
            ok = false;
          } else {
            setFilter("");
            d->cursorRelation_->refresh();
            d->cursorRelation_->setModeAccess(EDIT);
            d->cursorRelation_->refreshBuffer();
          }

          if (activeWid && activeWidEnabled)
            activeWid->setEnabled(true);
        }
      }
      break;

    case BROWSE:
    case EDIT:
      if (d->cursorRelation_ && d->relation_) {
        if (d->cursorRelation_->metadata() && d->cursorRelation_->modeAccess() == INSERT) {
          QWidget *activeWid = aqApp->activeModalWidget();
          if (!activeWid)
            activeWid = aqApp->activePopupWidget();
          if (!activeWid)
            activeWid = aqApp->activeWindow();
          bool activeWidEnabled = activeWid ? activeWid->isEnabled() : false;

          if (activeWid && activeWidEnabled)
            activeWid->setEnabled(false);

          if (!d->cursorRelation_->commitBuffer()) {
            d->modeAccess_ = BROWSE;
            ok = false;
          } else {
            d->cursorRelation_->refresh();
            d->cursorRelation_->setModeAccess(EDIT);
            d->cursorRelation_->refreshBuffer();
          }

          if (activeWid && activeWidEnabled)
            activeWid->setEnabled(true);
        }
      }
      break;
  }

  return ok;
}

bool FLSqlCursor::refreshBuffer()
{
  if (!d->metadata_)
    return false;

  if (!isValid() && d->modeAccess_ != INSERT)
    return false;

  switch (d->modeAccess_) {
    case INSERT: {
      if (!commitBufferCursorRelation())
        return false;

      d->buffer_ = primeInsert();
      setNotGenerateds();

      const FLTableMetaData::FLFieldMetaDataList *fieldList = d->metadata_->fieldList();

      if (fieldList) {
        FLFieldMetaData *field;
        QDictIterator<FLFieldMetaData> it(*fieldList);
        QString fiName;
        int type;
        QVariant::Type fltype;
        QVariant defVal;

        while ((field = it.current()) != 0) {
          ++it;

          fiName = field->name();
          d->buffer_->setNull(fiName);
          if (!d->buffer_->isGenerated(fiName))
            continue;

          type = field->type();
          fltype = FLFieldMetaData::flDecodeType(type);
          defVal = field->defaultValue();
          if (defVal.isValid()) {
            defVal.cast(fltype);
            d->buffer_->setValue(fiName, defVal);
          }

          if (type == FLFieldMetaData::Serial)
            d->buffer_->setValue(fiName, d->db_->nextSerialVal(d->metadata_->name(), fiName).toUInt());

          if (field->isCounter()) {
            if (d->ctxt_) {
              QVariant siguiente = aqApp->call("calculateCounter", QSArgumentList(fiName), d->ctxt_).variant();
              if (siguiente.isValid()) {
                d->buffer_->setValue(fiName, siguiente);
              } else {
                siguiente = FLUtil::nextCounter(fiName, this);
                if (siguiente.isValid())
                  d->buffer_->setValue(fiName, siguiente);
              }
            }
          }
        }
      }

      if (d->cursorRelation_ && d->relation_ && d->cursorRelation_->metadata())
        setValueBuffer(d->relation_->field(), d->cursorRelation_->valueBuffer(d->relation_->foreignField()));

      d->undoAcl();

      updateBufferCopy();
      emit newBuffer();
    }
    break;

    case EDIT: {
      if (!commitBufferCursorRelation())
        return false;

      if (isLocked() && d->acosCondName_.isEmpty())
        d->modeAccess_ = BROWSE;

      d->buffer_ = primeUpdate();
      setNotGenerateds();

      updateBufferCopy();
      emit newBuffer();
    }
    break;

    case DEL:
      if (isLocked()) {
        d->msgBoxWarning(tr("Registro bloqueado, no se puede eliminar"));
        d->modeAccess_ = BROWSE;
        return false;
      }

      d->buffer_ = primeDelete();
      setNotGenerateds();

      updateBufferCopy();
      break;

    case BROWSE:
      d->buffer_ = editBuffer(true);
      setNotGenerateds();
      emit newBuffer();
      break;
  }

  return true;
}

QString FLSqlCursor::msgCheckIntegrity()
{
  QString msg;

  if (!d->buffer_ || !d->metadata_) {
    msg += QString::fromLatin1("\n") + tr("Buffer vacío o no hay metadatos");
    return msg;
  }

  if (d->modeAccess_ == INSERT || d->modeAccess_ == EDIT) {
    if (!isModifiedBuffer() && d->modeAccess_ == EDIT)
      return msg;
    const FLTableMetaData::FLFieldMetaDataList *fieldList = d->metadata_->fieldList();
    bool checkedCK = false;

    if (!fieldList)
      return msg;

    FLFieldMetaData *field;
    QString fiName, fmdName;
    QVariant s, ss;
    QDictIterator<FLFieldMetaData> it(*fieldList);

    while ((field = it.current()) != 0) {
      ++it;

      fiName = field->name();
      if (!d->buffer_->isGenerated(fiName))
        continue;

      s.clear();
      if (!d->buffer_->isNull(fiName)) {
        s = d->buffer_->value(fiName);
        if (s.toString().isEmpty())
          s.clear();
      }

      FLFieldMetaData *fMD = field->associatedField();
      if (fMD && s.isValid() && !s.isNull()) {
        if (!field->relationM1()) {
          msg += QString::fromLatin1("\n") +
                 tr(
                   "FLSqlCursor : Error en metadatos, el campo %1 tiene un campo asociado pero no existe relación muchos a uno"
                 ).arg(d->metadata_->name() + QString::fromLatin1(":") + fiName);
          continue;
        }
        const FLRelationMetaData *r = field->relationM1();
        if (!r->checkIn())
          continue;
        FLTableMetaData *tMD = d->db_->manager()->metadata(field->relationM1()->foreignTable());
        if (!tMD)
          continue;
        fmdName = fMD->name();
        ss.clear();
        if (!d->buffer_->isNull(fmdName)) {
          ss = d->buffer_->value(fmdName);
          if (ss.toString().isEmpty())
            ss.clear();
        }
        if (ss.isValid() && !ss.isNull()) {
          QString filter(d->db_->manager()->formatAssignValue(field->associatedFieldFilterTo(), fMD, ss, true) +
                         QString::fromLatin1(" AND ") +
                         d->db_->manager()->formatAssignValue(field->relationM1()->foreignField(), field, s, true)
                        );
          FLSqlQuery q(0, d->db_->connectionName());

          q.setTablesList(tMD->name());
          q.setSelect(field->associatedFieldFilterTo());
          q.setFrom(tMD->name());
          q.setWhere(filter);
          q.setForwardOnly(true);
          q.exec();
          if (!q.next())
            msg += QString::fromLatin1("\n") + d->metadata_->name() + QString::fromLatin1(":") +
                   field->alias() + tr(" : %1 no pertenece a %2").arg(s.toString(), ss.toString());
          else
            d->buffer_->setValue(fmdName, q.value(0));
        } else
          msg += QString::fromLatin1("\n") + d->metadata_->name() + QString::fromLatin1(":") +
                 field->alias() + tr(" : %1 no se puede asociar a un valor NULO").arg(s.toString());
        if (!tMD->inCache())
          delete tMD;
      }

      if (d->modeAccess_ == EDIT && d->buffer_->value(fiName) == d->bufferCopy_->value(fiName))
        continue;

      if (d->buffer_->isNull(fiName) && !field->allowNull() &&
          field->type() != FLFieldMetaData::Serial) {
        msg += QString::fromLatin1("\n") + d->metadata_->name() + QString::fromLatin1(":") +
               field->alias() + tr(" : No puede ser nulo");
      }

      if (field->isUnique()) {
        QString pK = d->metadata_->primaryKey();
        if (!d->buffer_->isNull(pK) && s.isValid() && !s.isNull()) {
          QVariant pKV(d->buffer_->value(pK));
          FLSqlQuery q(0, d->db_->connectionName());
          q.setTablesList(d->metadata_->name());
          q.setSelect(fiName);
          q.setFrom(d->metadata_->name());
          q.setWhere(
            d->db_->manager()->formatAssignValue(field, s, true) +
            QString::fromLatin1(" AND ") + d->metadata_->primaryKey(d->isQuery_) +
            QString::fromLatin1("<>") + d->db_->manager()->formatValue(d->metadata_->fieldType(pK), pKV)
          );
          q.setForwardOnly(true);
          q.exec();
          if (q.next())
            msg += QString::fromLatin1("\n") + d->metadata_->name() + QString::fromLatin1(":") + field->alias() +
                   tr(
                     " : Requiere valores únicos, y ya hay otro registro con el valor %1 en este campo"
                   ).arg(s.toString());
        }
      }

      if (field->isPrimaryKey() && d->modeAccess_ == INSERT && s.isValid() && !s.isNull()) {
        FLSqlQuery q(0, d->db_->connectionName());
        q.setTablesList(d->metadata_->name());
        q.setSelect(fiName);
        q.setFrom(d->metadata_->name());
        q.setWhere(d->db_->manager()->formatAssignValue(field, s, true));
        q.setForwardOnly(true);
        q.exec();
        if (q.next())
          msg += QString::fromLatin1("\n") + d->metadata_->name() + QString::fromLatin1(":") + field->alias() +
                 tr(
                   " : Es clave primaria y requiere valores únicos, y ya hay otro registro con el valor %1 en este campo"
                 ).arg(s.toString());
      }

      if (field->relationM1() && s.isValid() && !s.isNull()) {
        if (field->relationM1()->checkIn() && field->relationM1()->foreignTable() != d->metadata_->name()) {
          const FLRelationMetaData *r = field->relationM1();
          FLTableMetaData *tMD = d->db_->manager()->metadata(r->foreignTable());
          if (!tMD)
            continue;
          FLSqlQuery q(0, d->db_->connectionName());
          q.setTablesList(tMD->name());
          q.setSelect(r->foreignField());
          q.setFrom(tMD->name());
          q.setWhere(d->db_->manager()->formatAssignValue(r->foreignField(), field, s, true));
          q.setForwardOnly(true);
          q.exec();
          if (!q.next())
            msg += QString::fromLatin1("\n") + d->metadata_->name() + QString::fromLatin1(":") + field->alias() +
                   tr(" : El valor %1 no existe en la tabla %2").arg(s.toString(), r->foreignTable());
          else
            d->buffer_->setValue(fiName, q.value(0));
          if (!tMD->inCache())
            delete tMD;
        }
      }

      const FLTableMetaData::FLFieldMetaDataList *fieldListCK = d->metadata_->fieldListOfCompoundKey(fiName);
      if (fieldListCK && !checkedCK && d->modeAccess_ == INSERT) {
        if (!fieldListCK->isEmpty()) {
          FLFieldMetaData *fieldCK;
          QString filter;
          QString fields;
          QString valuesFields;
          QDictIterator<FLFieldMetaData> it(*fieldListCK);
          while ((fieldCK = it.current()) != 0) {
            ++it;
            QVariant sCK = d->buffer_->value(fieldCK->name());
            if (filter.isEmpty())
              filter += d->db_->manager()->formatAssignValue(fieldCK, sCK, true);
            else
              filter += QString::fromLatin1(" AND ") + d->db_->manager()->formatAssignValue(fieldCK, sCK, true);
            if (fields.isEmpty())
              fields += fieldCK->alias();
            else
              fields += QString::fromLatin1("+") + fieldCK->alias();
            if (valuesFields.isEmpty())
              valuesFields += sCK.toString();
            else
              valuesFields += QString::fromLatin1("+") + sCK.toString();
          }
          FLSqlQuery q(0, d->db_->connectionName());
          q.setTablesList(d->metadata_->name());
          q.setSelect(fiName);
          q.setFrom(d->metadata_->name());
          q.setWhere(filter);
          q.setForwardOnly(true);
          q.exec();
          if (q.next())
            msg += QString::fromLatin1("\n") + fields +
                   tr(" : Requiere valor único, y ya hay otro registro con el valor %1").arg(valuesFields);
          checkedCK = true;
        }
      }
    }
  } else if (d->modeAccess_ == DEL) {
    const FLTableMetaData::FLFieldMetaDataList *fieldList = d->metadata_->fieldList();
    FLFieldMetaData *field;
    QString fiName;
    QVariant s;
    QDictIterator<FLFieldMetaData> it(*fieldList);

    while ((field = it.current()) != 0) {
      ++it;

      fiName = field->name();
      if (!d->buffer_->isGenerated(fiName))
        continue;

      s.clear();
      if (!d->buffer_->isNull(fiName)) {
        s = d->buffer_->value(fiName);
        if (s.toString().isEmpty())
          s.clear();
      }

      if (!s.isValid() || s.isNull())
        continue;

      const FLFieldMetaData::FLRelationMetaDataList *relationList = field->relationList();

      if (!relationList)
        continue;
      if (!relationList->isEmpty()) {
        FLRelationMetaData *r;
        QPtrListIterator<FLRelationMetaData> itR(*relationList);

        while ((r = itR.current()) != 0) {
          ++itR;
          if (!r->checkIn())
            continue;
          FLTableMetaData *mtd = d->db_->manager()->metadata(r->foreignTable());
          if (!mtd)
            continue;
          FLFieldMetaData *f = mtd->field(r->foreignField());
          if (f) {
            if (f->relationM1()) {
              if (f->relationM1()->deleteCascade()) {
                if (!mtd->inCache())
                  delete mtd;
                continue;
              }
              if (!f->relationM1()->checkIn()) {
                if (!mtd->inCache())
                  delete mtd;
                continue;
              }
            } else {
              if (!mtd->inCache())
                delete mtd;
              continue;
            }
          } else {
            msg += QString::fromLatin1("\n") +
                   tr("FLSqlCursor : Error en metadatos, %1.%2 no es válido.\n"
                      "Campo relacionado con %3.%4.")
                   .arg(mtd->name())
                   .arg(r->foreignField())
                   .arg(d->metadata_->name())
                   .arg(field->name());
            if (!mtd->inCache())
              delete mtd;
            continue;
          }

          FLSqlQuery q(0, d->db_->connectionName());
          q.setTablesList(mtd->name());
          q.setSelect(r->foreignField());
          q.setFrom(mtd->name());
          q.setWhere(d->db_->manager()->formatAssignValue(r->foreignField(), field, s, true));
          q.setForwardOnly(true);
          q.exec();
          if (q.next())
            msg += QString::fromLatin1("\n") + d->metadata_->name() + QString::fromLatin1(":") + field->alias() +
                   tr(" : Con el valor %1 hay registros en la tabla %2:%3").arg(s.toString(), mtd->name(), mtd->alias());
          if (!mtd->inCache())
            delete mtd;
        }
      }
    }
  }

  return msg;
}

bool FLSqlCursor::checkIntegrity(bool showError)
{
  if (!d->buffer_ || !d->metadata_)
    return false;
  if (!d->activatedCheckIntegrity_)
    return true;

  QString msg(msgCheckIntegrity());
  if (!msg.isEmpty()) {
    if (showError) {
      if (d->modeAccess_ == INSERT || d->modeAccess_ == EDIT) {
        d->msgBoxWarning(tr("No se puede validar el registro actual:\n") + msg);
      } else if (d->modeAccess_ == DEL) {
        d->msgBoxWarning(tr("No se puede borrar registro:\n") + msg);
      }
    }

    return false;
  }

  return true;
}

void FLSqlCursor::checkRisksLocks(bool terminate)
{
  if (d->timerRisksLocks_)
    d->timerRisksLocks_->stop();
  if (d->inLoopRisksLocks_ && terminate) {
    d->inLoopRisksLocks_ = false;
    d->inRisksLocks_ = !(d->db_->detectRisksLocks(d->metadata_->name(),
                                                  valueBuffer(d->metadata_->primaryKey()).toString()).isEmpty());
    QApplication::eventLoop()->exitLoop();
  } else if (!terminate) {
    QStringList locks(d->db_->detectRisksLocks(d->metadata_->name(),
                                               valueBuffer(d->metadata_->primaryKey()).toString()));

    if (!locks.isEmpty()) {
      if (!d->inLoopRisksLocks_) {
        d->inRisksLocks_ = true;
        d->inLoopRisksLocks_ = true;
        if (!d->timerRisksLocks_)
          d->timerRisksLocks_ = new QTimer(this);
        if (!d->modalRisksLocks_)
          d->modalRisksLocks_ = new QLabel(tr("SISTEMA  ANTIBLOQUEOS  ACTIVADO"),
                                           0, 0,
                                           Qt::WStyle_Customize |
                                           Qt::WStyle_Splash |
                                           Qt::WShowModal);
        QFont lblFont(qApp->font());
        lblFont.setPointSize(20);
        lblFont.setBold(true);
        d->modalRisksLocks_->setFont(lblFont);
        d->modalRisksLocks_->setFrameStyle(QFrame::Box | QFrame::Plain);
        d->modalRisksLocks_->show();

        connect(d->timerRisksLocks_, SIGNAL(timeout()), this, SLOT(checkRisksLocks()));
        connect(QApplication::eventLoop(), SIGNAL(aboutToBlock()), this, SLOT(checkRisksLocks()));
        QApplication::eventLoop()->enterLoop();

        d->timerRisksLocks_->deleteLater();
        d->timerRisksLocks_ = 0;
        d->modalRisksLocks_->deleteLater();
        d->modalRisksLocks_ = 0;

        if (!d->inRisksLocks_) {
          QString msgLock(tr("<p><img source=\"locked.png\" align=\"right\"><b><u>Sin Riesgo de Bloqueo</u></b><br><br>"
                             "Ya no existe riesgo de bloqueo puede continuar con la transacción actual."));
          QMap<QString, QSArgumentList> scrCall;
          aqApp->popupWarn(msgLock, scrCall);
        }
      } else {
        disconnect(QApplication::eventLoop(), SIGNAL(aboutToBlock()), this, SLOT(checkRisksLocks()));

        QString msgLock(tr("<p><img source=\"locked.png\" align=\"right\"><b><u>Riesgo de Bloqueo Detectado</u></b><br><br>"
                           "Va a acceder a datos que están siendo modificados por otro usuario en este momento, "
                           "es muy probable que su conexión y la aplicación queden congeladas hasta que el "
                           "otro usuario termine su transacción.<br><br>Si le es posible postponer la operación "
                           "que está realizando se recomienda cancelar los cambios actuales y esperar "
                           "unos segundos antes de continuar."
                           "<br><br><em><a href=\"sys.terminateChecksLocks\">Cerrar sistema antibloqueos</a></em></p>"
                           "<<br><em><a href=\"sys.statusDbLocksDialog\">Más información</a></em></p>"));

        QMap<QString, QSArgumentList> scrCall;
        scrCall["sys.terminateChecksLocks"] = QSArgumentList(FLSqlCursorInterface::sqlCursorInterface(this));
        scrCall["sys.statusDbLocksDialog"] = QSArgumentList(locks);
        aqApp->popupWarn(msgLock, scrCall);

        if (d->timerRisksLocks_)
          d->timerRisksLocks_->start(10000, true);
      }
    } else if (d->inLoopRisksLocks_)
      checkRisksLocks(true);
  }
}

bool FLSqlCursor::commitBuffer(bool emite, bool checkLocks)
{
  if (!d->buffer_ || !d->metadata_)
    return false;

  if (d->db_->interactiveGUI() && d->db_->canDetectLocks() &&
      (checkLocks || d->metadata_->detectLocks())) {
    checkRisksLocks();
    if (d->inRisksLocks_ &&
        QMessageBox::No ==
        QMessageBox::warning(
          0, tr("Bloqueo inminente"),
          tr("Los registros que va a modificar están bloqueados actualmente,\n"
             "si continua hay riesgo de que su conexión quede congelada hasta "
             "finalizar el bloqueo.\n\n"
             "¿ Desa continuar aunque exista riesgo de bloqueo ?"),
          QMessageBox::Yes,
          QMessageBox::No | QMessageBox::Default | QMessageBox::Escape
        )
       ) {
      return false;
    }
  }

  if (!checkIntegrity())
    return false;

  FLApplication *flapp = aqApp;
  QString fieldNameCheck;

  if (d->modeAccess_ == EDIT || d->modeAccess_ == INSERT) {
    const FLTableMetaData::FLFieldMetaDataList *fieldList = d->metadata_->fieldList();
    FLFieldMetaData *field;

    QDictIterator<FLFieldMetaData> it(*fieldList);
    while ((field = it.current()) != 0) {
      ++it;

      if (field->isCheck()) {
        fieldNameCheck = field->name();
        d->buffer_->setGenerated(fieldNameCheck, false);
        if (d->bufferCopy_)
          d->bufferCopy_->setGenerated(fieldNameCheck, false);
        continue;
      }
      if (!d->buffer_->isGenerated(field->name()))
        continue;

      if (d->ctxt_ && field->calculated()) {
        QVariant v = flapp->call("calculateField", QSArgumentList(field->name()), d->ctxt_).variant();
        if (v.isValid())
          setValueBuffer(field->name(), v);
      }
    }
  }

  QString functionBefore;
  QString functionAfter;
  if (d->modeAccess_ != BROWSE && d->activatedCommitActions_) {
    QString idMod(d->db_->managerModules()->idModuleOfFile(d->metadata_->name() +
                                                           QString::fromLatin1(".mtd")));
    if (!idMod.isEmpty()) {
      functionBefore = idMod + QString::fromLatin1(".beforeCommit_") + d->metadata_->name();
      functionAfter = idMod + QString::fromLatin1(".afterCommit_") + d->metadata_->name();
    } else {
      functionBefore = QString::fromLatin1("sys.beforeCommit_") + d->metadata_->name();
      functionAfter = QString::fromLatin1("sys.afterCommit_") + d->metadata_->name();
    }
    if (!functionBefore.isEmpty()) {
      FLSqlCursorInterface *cI = FLSqlCursorInterface::sqlCursorInterface(this);
      QVariant v = flapp->call(functionBefore, QSArgumentList(cI), 0).variant();
      if (v.isValid() && !v.toBool())
        return false;
    }
  }

  if (!checkIntegrity())
    return false;

  QString pKN(d->metadata_->primaryKey());
  bool updated = false;
  FLSqlSavePoint *savePoint = 0;

  switch (d->modeAccess_) {
    case INSERT: {
      if (d->cursorRelation_ && d->relation_) {
        if (d->cursorRelation_->metadata()) {
          setValueBuffer(d->relation_->field(), d->cursorRelation_->valueBuffer(d->relation_->foreignField()));
          d->cursorRelation_->setAskForCancelChanges(true);
        }
      }
      QString pKWhere(d->db_->manager()->formatAssignValue(d->metadata_->field(pKN), valueBuffer(pKN)));
      insert(false);
      if (!d->db_->canSavePoint()) {
        if (d->db_->currentSavePoint_)
          d->db_->currentSavePoint_->saveInsert(pKN, d->buffer_, this);
      }
      if (!functionAfter.isEmpty() && d->activatedCommitActions_) {
        if (!savePoint)
          savePoint = new FLSqlSavePoint(0);
        savePoint->saveInsert(pKN, d->buffer_, this);
      }
      if (!d->persistentFilter_.contains(pKWhere))
        d->persistentFilter_ = (d->persistentFilter_.isEmpty() ?
                                pKWhere : d->persistentFilter_ + QString::fromLatin1(" OR ") + pKWhere);
      updated = true;
    }
    break;

    case EDIT: {
      if (!d->db_->canSavePoint()) {
        if (d->db_->currentSavePoint_)
          d->db_->currentSavePoint_->saveEdit(pKN, d->bufferCopy_, this);
      }
      if (!functionAfter.isEmpty() && d->activatedCommitActions_) {
        if (!savePoint)
          savePoint = new FLSqlSavePoint(0);
        savePoint->saveEdit(pKN, d->bufferCopy_, this);
      }

      if (d->cursorRelation_ && d->relation_) {
        if (d->cursorRelation_->metadata()) {
          if (isModifiedBuffer())
            d->cursorRelation_->setAskForCancelChanges(true);
        }
      }
      if (isModifiedBuffer()) {
        for (uint i = 0; i < d->buffer_->count(); ++i) {
          if ((d->buffer_->value(i) == d->bufferCopy_->value(i)) &&
              (d->buffer_->isNull(i) == d->bufferCopy_->isNull(i))) {
            d->buffer_->setGenerated(i, false);
          }
        }
        update(false);
        for (uint i = 0; i < d->buffer_->count(); ++i)
          d->buffer_->setGenerated(i, true);
        updated = true;
        setNotGenerateds();
      }
    }
    break;

    case DEL: {
      if (!d->db_->canSavePoint()) {
        if (d->db_->currentSavePoint_)
          d->db_->currentSavePoint_->saveDel(pKN, d->bufferCopy_, this);
      }
      if (!functionAfter.isEmpty() && d->activatedCommitActions_) {
        if (!savePoint)
          savePoint = new FLSqlSavePoint(0);
        savePoint->saveDel(pKN, d->bufferCopy_, this);
      }

      if (d->cursorRelation_ && d->relation_)
        if (d->cursorRelation_->metadata())
          d->cursorRelation_->setAskForCancelChanges(true);
      del(false);
      updated = true;
    }
    break;

    case BROWSE:
      break;
  }

  if (updated && lastError().type() != QSqlError::None) {
    //    QString msg("<p><img source=\"remove.png\" align=\"right\">" +
    //                "<b><u>SQL ERROR</u></b><br><br>" +
    //                QString(lastError().driverText()) + "<br>" +
    //                QString(lastError().databaseText()) + "</p>");
    //    msg.replace("\n", "<br>");
    //    aqApp->popupWarn(msg);

    if (savePoint)
      delete savePoint;

    return false;
  }

  if (d->modeAccess_ != BROWSE && !functionAfter.isEmpty() && d->activatedCommitActions_) {
    FLSqlCursorInterface *cI = FLSqlCursorInterface::sqlCursorInterface(this);
    QVariant v = flapp->call(functionAfter, QSArgumentList(cI), 0).variant();
    if (v.isValid() && !v.toBool()) {
      if (savePoint) {
        savePoint->undo();
        delete savePoint;
      }
      return false;
    }
  }

  if (savePoint)
    delete savePoint;

  d->modeAccess_ = BROWSE;

  if (updated) {
    if (!fieldNameCheck.isEmpty()) {
      d->buffer_->setGenerated(fieldNameCheck, true);
      if (d->bufferCopy_)
        d->bufferCopy_->setGenerated(fieldNameCheck, true);
    }
    setFilter("");
    clearMapCalcFields();
  }

  if (updated && emite)
    emit cursorUpdated();

  emit bufferCommited();

  return true;
}

bool FLSqlCursor::seek(int i, bool relative, bool emite)
{
  if (d->modeAccess_ == DEL)
    return false;

  bool b = QSqlCursor::seek(i, relative);

  if (b && emite)
    emit currentChanged(at());

  if (b)
    return refreshBuffer();

  return b;
}

void FLSqlCursor::afterSeek()
{
  QSqlCursor::afterSeek();
  d->doAcl();
}

bool FLSqlCursor::next(bool emite)
{
  if (d->modeAccess_ == DEL)
    return false;

  bool b = QSqlCursor::next();

  if (b && emite)
    emit currentChanged(at());

  if (b)
    return refreshBuffer();

  return b;
}

bool FLSqlCursor::prev(bool emite)
{
  if (d->modeAccess_ == DEL)
    return false;

  bool b = QSqlCursor::prev();

  if (b && emite)
    emit currentChanged(at());

  if (b)
    return refreshBuffer();

  return b;
}

bool FLSqlCursor::first(bool emite)
{
  if (d->modeAccess_ == DEL)
    return false;

  bool b = QSqlCursor::first();

  if (b && emite)
    emit currentChanged(at());

  if (b)
    return refreshBuffer();

  return b;
}

bool FLSqlCursor::last(bool emite)
{
  if (d->modeAccess_ == DEL)
    return false;

  bool b = QSqlCursor::last();

  if (b && emite)
    emit currentChanged(at());

  if (b)
    return refreshBuffer();

  return b;
}

int FLSqlCursor::del(bool invalidate)
{
  if (!d->metadata_)
    return -1;

  if (d->ctxt_) {
    QString f = "recordDelBefore" + d->metadata_->name();
    aqApp->call(f, QSArgumentList(), d->ctxt_);
  }

  const FLTableMetaData::FLFieldMetaDataList *fieldList = d->metadata_->fieldList();
  FLFieldMetaData *field;
  QString fiName;
  QVariant s;

  QDictIterator<FLFieldMetaData> it(*fieldList);
  while ((field = it.current()) != 0) {
    ++it;

    fiName = field->name();
    if (!d->buffer_->isGenerated(fiName))
      continue;

    s.clear();
    if (!d->buffer_->isNull(fiName)) {
      s = d->buffer_->value(fiName);
      if (s.toString().isEmpty())
        s.clear();
    }

    if (!s.isValid() || s.isNull())
      continue;

    const FLFieldMetaData::FLRelationMetaDataList *relationList = field->relationList();

    if (!relationList)
      continue;

    if (!relationList->isEmpty()) {
      FLRelationMetaData *r;
      QPtrListIterator<FLRelationMetaData> itR(*relationList);

      while ((r = itR.current()) != 0) {
        ++itR;

        FLSqlCursor c(r->foreignTable());
        if (!c.metadata())
          continue;
        FLFieldMetaData *f = c.metadata()->field(r->foreignField());
        if (f && f->relationM1() && f->relationM1()->deleteCascade()) {
          c.setForwardOnly(true);
          c.select(d->db_->manager()->formatAssignValue(r->foreignField(), f, s, true));
          while (c.next()) {
            c.setModeAccess(DEL);
            c.refreshBuffer();
            if (!c.commitBuffer(false))
              return false;
          }
        }
      }
    }
  }

  int r = QSqlCursor::del(invalidate);

  if (lastError().type() != QSqlError::None)
    return -1;

  if (d->ctxt_) {
    QString f = "recordDelAfter" + d->metadata_->name();
    aqApp->call(f, QSArgumentList(), d->ctxt_);
  }

  return r;
}

void FLSqlCursor::setAtomicValueBuffer(const QString &fN, const QString &functionName)
{
  if (!d->buffer_ || fN.isEmpty() || !d->metadata_)
    return;

  FLFieldMetaData *field = d->metadata_->field(fN);
  if (!field) {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor::setAtomicValueBuffer() : No existe el campo ") + d->metadata_->name() + ":" + fN);
#endif
    return;
  }

  if (!d->db_->dbAux())
    return;

  int type = field->type();
  QVariant::Type fltype = FLFieldMetaData::flDecodeType(type);
  QString pK = d->metadata_->primaryKey();
  QVariant v;

  if (d->cursorRelation_ && d->modeAccess_ != BROWSE)
    d->cursorRelation_->commit(false);

  if (!pK.isEmpty() && d->db_->db() != d->db_->dbAux()) {
    QVariant pKV = d->buffer_->value(pK);
    d->db_->dbAux()->transaction();

    QSArgumentList arglist;
    arglist.append(QSArgument(fN));
    arglist.append(QSArgument(d->buffer_->value(fN)));
    v = aqApp->call(functionName, arglist, d->ctxt_).variant();

    QSqlQuery q(QString::null, d->db_->dbAux());
    if (q.exec("UPDATE " + d->metadata_->name() + " SET " + fN + "=" + d->db_->manager()->formatValue(type, v) + " WHERE " + d->db_->manager()->formatAssignValue(d->metadata_->field(pK), pKV)))
      d->db_->dbAux()->commit();
    else
      d->db_->dbAux()->rollback();
  } else {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor : No se puede actualizar el campo de forma atómica, porque no existe clave primaria"));
#endif
  }

  if (v.isValid())
    v.cast(fltype);
  d->buffer_->setValue(fN, v);

  emit bufferChanged(fN);
}

void FLSqlCursor::setValueBuffer(const QString &fN, const QVariant &v)
{
  if (!d->buffer_ || fN.isEmpty() || !d->metadata_)
    return;

  FLFieldMetaData *field = d->metadata_->field(fN);
  if (!field) {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor::setValueBuffer() : No existe el campo ") + d->metadata_->name() + ":" + fN);
#endif
    return;
  }

  int type = field->type();
  QVariant::Type fltype = FLFieldMetaData::flDecodeType(type);
  QVariant vv(v);

  if (vv.type() == QVariant::Bool && fltype != QVariant::Bool)
    vv.clear();
  if (vv.isValid()) {
    vv.cast(fltype);
    if (!vv.isNull() && type == QVariant::Pixmap) {
      QString largeValue(vv.toString());
      largeValue = d->db_->manager()->storeLargeValue(d->metadata_, largeValue);
      if (!largeValue.isEmpty())
        vv = largeValue;
    }
  }

  if (field->outTransaction() && d->db_->dbAux() && d->db_->db() != d->db_->dbAux() && d->modeAccess_ != INSERT) {
    QString pK(d->metadata_->primaryKey());

    if (d->cursorRelation_ && d->modeAccess_ != BROWSE)
      d->cursorRelation_->commit(false);

    if (!pK.isEmpty()) {
      QVariant pKV(d->buffer_->value(pK));
      QSqlQuery q(QString::null, d->db_->dbAux());
      q.exec("UPDATE " + d->metadata_->name() + " SET " + fN + "=" +
             d->db_->manager()->formatValue(type, vv) + " WHERE " +
             d->db_->manager()->formatAssignValue(d->metadata_->field(pK), pKV));
    } else {
#ifdef FL_DEBUG
      qWarning(tr("FLSqlCursor : No se puede actualizar el campo fuera de transaccion, porque no existe clave primaria"));
#endif
    }
  } else
    d->buffer_->setValue(fN, vv);

  emit bufferChanged(fN);
}

QVariant FLSqlCursor::valueBuffer(const QString &fN)
{
  if (d->rawValues_)
    return valueBufferRaw(fN);

  if (!d->buffer_ || fN.isEmpty() || !d->metadata_)
    return QVariant();

  FLFieldMetaData *field = d->metadata_->field(fN);
  if (!field) {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor::valueBuffer() : No existe el campo ") + d->metadata_->name() + ":" + fN);
#endif
    return QVariant();
  }

  int type = field->type();
  QVariant::Type fltype = FLFieldMetaData::flDecodeType(type);
  if (d->buffer_->isNull(fN)) {
    if (type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt)
      return 0;
  }

  QVariant v;
  if (field->outTransaction() && d->db_->dbAux() && d->db_->db() != d->db_->dbAux() && d->modeAccess_ != INSERT) {
    QString pK(d->metadata_->primaryKey());
    if (!pK.isEmpty()) {
      QVariant pKV(d->buffer_->value(pK));
      QSqlQuery q(QString::null, d->db_->dbAux());
      if (q.exec("SELECT " + fN + " FROM " + d->metadata_->name() + " WHERE " +
                 d->db_->manager()->formatAssignValue(d->metadata_->field(pK), pKV))) {
        if (q.isActive() && q.next())
          v = q.value(0);
      }
    } else {
#ifdef FL_DEBUG
      qWarning(tr("FLSqlCursor : No se puede obtener el campo fuera de transaccion, porque no existe clave primaria"));
#endif
    }
  } else
    v = d->buffer_->value(fN);

  if (v.isValid())
    v.cast(fltype);
  if (!v.isNull() && type == QVariant::Pixmap) {
    QVariant vLarge(d->db_->manager()->fetchLargeValue(v.toString()));
    if (vLarge.isValid())
      return vLarge;
  }
  return v;
}


QVariant FLSqlCursor::valueBufferRaw(const QString &fN)
{
  if (fN.isEmpty() || !d->metadata_)
    return QVariant();

  FLFieldMetaData *field = d->metadata_->field(fN);
  if (!field) {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor::valueBuffer() : No existe el campo ") + d->metadata_->name() + ":" + fN);
#endif
    return QVariant();
  }

  int type = field->type();
  QVariant::Type fltype = FLFieldMetaData::flDecodeType(type);
  if (QSqlRecord::isNull(fN)) {
    if (type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt)
      return 0;
  }

  QVariant v(value(fN));

  if (v.isValid())
    v.cast(fltype);
  if (!v.isNull() && type == QVariant::Pixmap) {
    QVariant vLarge(d->db_->manager()->fetchLargeValue(v.toString()));
    if (vLarge.isValid())
      return vLarge;
  }
  return v;
}

QVariant FLSqlCursor::valueBufferCopy(const QString &fN) const
{
  if (!d->bufferCopy_ || fN.isEmpty() || !d->metadata_)
    return QVariant();

  FLFieldMetaData *field = d->metadata_->field(fN);
  if (!field) {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor::valueBufferCopy() : No existe el campo ") + d->metadata_->name() + ":" + fN);
#endif
    return QVariant();
  }

  int type = field->type();
  QVariant::Type fltype = FLFieldMetaData::flDecodeType(type);
  if (d->bufferCopy_->isNull(fN)) {
    if (type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt)
      return 0;
  }

  QVariant v(d->bufferCopy_->value(fN));
  if (v.isValid())
    v.cast(fltype);
  if (!v.isNull() && type == QVariant::Pixmap) {
    QVariant vLarge(d->db_->manager()->fetchLargeValue(v.toString()));
    if (vLarge.isValid())
      return vLarge;
  }
  return v;
}

void FLSqlCursor::deleteRecord()
{
  openFormInMode(DEL);
}

void FLSqlCursor::browseRecord()
{
#ifdef AQ_MD5_CHECK
  // ### Ver refresco buffer dos usuarios ¿es necesario forzar refresco?
  if (d->needUpdate()) {
    QString pKN(d->metadata_->primaryKey());
    QVariant pKValue(valueBuffer(pKN));
    refresh();
    int pos = atFromBinarySearch(pKN, pKValue.toString());
    if (pos != at())
      seek(pos, false, true);
  }
#endif
  openFormInMode(BROWSE);
}

void FLSqlCursor::editRecord()
{
#ifdef AQ_MD5_CHECK
  if (d->needUpdate()) {
    QString pKN(d->metadata_->primaryKey());
    QVariant pKValue(valueBuffer(pKN));
    refresh();
    int pos = atFromBinarySearch(pKN, pKValue.toString());
    if (pos != at())
      seek(pos, false, true);
  }
#endif
  openFormInMode(EDIT);
}

void FLSqlCursor::insertRecord()
{
  openFormInMode(INSERT);
}

void FLSqlCursor::copyRecord()
{
  if (!d->metadata_ || !d->buffer_)
    return;

  if (!isValid() || size() <= 0) {
    QMessageBox::warning(qApp->focusWidget(),
                         tr("Aviso"),
                         tr("No hay ningún registro seleccionado"),
                         QMessageBox::Ok, 0, 0);
    return;
  }

  const FLTableMetaData::FLFieldMetaDataList *fieldList = d->metadata_->fieldList();
  if (!fieldList)
    return;

#ifdef AQ_MD5_CHECK
  if (d->needUpdate()) {
    QString pKN(d->metadata_->primaryKey());
    QVariant pKValue(valueBuffer(pKN));
    refresh();
    int pos = atFromBinarySearch(pKN, pKValue.toString());
    if (pos != at())
      seek(pos, false, true);
  }
#endif

  QSqlRecord *bufferAux = new QSqlRecord(*d->buffer_);

  insertRecord();

  QDictIterator<FLFieldMetaData> it(*fieldList);
  for (; it.current(); ++it) {
    if (!(*it))
      continue;
    if (d->buffer_->isNull((*it)->name()) && !(*it)->isPrimaryKey() &&
        !d->metadata_->fieldListOfCompoundKey((*it)->name()) && !(*it)->calculated())
      d->buffer_->setValue((*it)->name(), bufferAux->value((*it)->name()));
  }

  delete bufferAux;
  emit newBuffer();
}

void FLSqlCursor::openFormInMode(int m, bool cont)
{
  if (!d->metadata_)
    return;

  if ((!isValid() || size() <= 0) && m != INSERT) {
    QMessageBox::warning(qApp->focusWidget(), tr("Aviso"),
                         tr("No hay ningún registro seleccionado"),
                         QMessageBox::Ok, 0, 0);
    return;
  }

  if (m == DEL) {
    int res = QMessageBox::information(
                qApp->focusWidget(), tr("Borrar registro"),
                tr("El registro activo será borrado. ¿ Está seguro ?"),
                QMessageBox::Yes,
                QMessageBox::No | QMessageBox::Default | QMessageBox::Escape
              );
    if (res == QMessageBox::No)
      return;

    transaction();
    d->modeAccess_ = DEL;
    if (!refreshBuffer()) {
      commit();
    } else {
      if (!commitBuffer())
        rollback();
      else
        commit();
    }
    return;
  }

  d->modeAccess_ = m;
  if (d->buffer_)
    d->buffer_->clearValues(true);

  if (!d->action_)
    d->action_ = d->db_->manager()->action(d->metadata_->name());

  if (!d->action_) {
#ifdef FL_DEBUG
    qWarning("FLSqlCursor : Para poder abrir un registro de edición se necesita una acción asociada al cursor, o una acción definida con el mismo nombre que la tabla de la que procede el cursor.");
#endif
    return;
  }

  if (d->action_->formRecord().isEmpty()) {
    QMessageBox::warning(
      qApp->focusWidget(), tr("Aviso"),
      tr("No hay definido ningún formulario para manejar "
         "registros de esta tabla : %1").arg(d->action_->name()),
      QMessageBox::Ok, 0, 0
    );
    return;
  } else {
    QSProject *p = aqApp->project();
    FLFormRecordDBInterface *iface = static_cast<FLFormRecordDBInterface *>(
                                       p->object("formRecord" + d->action_->name())
                                     );
    if (iface && iface->obj()) {
      QMessageBox::warning(
        qApp->focusWidget(), tr("Aviso"),
        tr("Ya hay abierto un formulario de edición de registros para esta "
           "tabla.\nNo se abrirán más para evitar ciclos repetitivos de "
           "edición de registros."),
        QMessageBox::Ok, 0, 0
      );
      return;
    }
  }

  if (!refreshBuffer())
    return;
  FLFormRecordDB *f = new FLFormRecordDB(this, d->action_->name(),
                                         aqApp->mainWidget(), cont);
  if (refreshBuffer()) {
    f->setMainWidget();
    f->setFocus();
    f->show();
    updateBufferCopy();
  } else
    f->close();
}

void FLSqlCursor::chooseRecord()
{
// --> Aulla Desactiva edición con doble click
if (!FLSettings::readBoolEntry("ebcomportamiento/FLTableDoubleClick", false))
	{
	  if (d->edition_)
    		editRecord();
  else
       if (d->browse_)
    		browseRecord();
	}
	else
	{
 if (d->browse_)
    browseRecord();
	}
	
// <-- Aulla Desactiva edición con doble click
  emit recordChoosed();
}

bool FLSqlCursor::fieldDisabled(const QString &fN)
{
  if (d->modeAccess_ == INSERT || d->modeAccess_ == EDIT) {
    if (d->cursorRelation_ && d->relation_) {
      if (!d->cursorRelation_->metadata())
        return false;
      return (d->relation_->field().lower() == fN.lower());
    } else
      return false;
  } else
    return false;
}

bool FLSqlCursor::transaction(bool /*lock*/)
{
  if (!d->db_ && !d->db_->db()) {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor::transaction() : No hay conexión con la base de datos"));
#endif
    return false;
  }

  return d->db_->doTransaction(this);
}

bool FLSqlCursor::rollback()
{
  if (!d->db_ && !d->db_->db()) {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor::rollback() : No hay conexión con la base de datos"));
#endif
    return false;
  }

  return d->db_->doRollback(this);
}

bool FLSqlCursor::commit(bool notify)
{
  if (!d->db_ && !d->db_->db()) {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor::commit() : No hay conexión con la base de datos"));
#endif
    return false;
  }
  bool r = d->db_->doCommit(this, notify);
  if (r) {
    emit commited();
  }
  return r;
}

bool FLSqlCursor::select(const QString &filter, const QSqlIndex &sort)
{
  if (!d->metadata_)
    return false;

  clearMapCalcFields();

  QString f(filter);
  QString bFilter(baseFilter());
  QString finalFilter;

  if (bFilter.isEmpty())
    finalFilter = f;
  else {
    if (filter.isEmpty() || bFilter.contains(f))
      finalFilter = bFilter;
    else {
      if (f.contains(bFilter))
        finalFilter = f;
      else
        finalFilter = bFilter + " AND " + f;
    }
  }

  QStringList fields;
  if (d->isQuery_)
    fields = sort.toStringList(d->metadata_->name());
  else
    fields = sort.toStringList();

  finalFilter = finalFilter.simplifyWhiteSpace();
  // ###
  //finalFilter.replace("=;", "= NULL;");

  while (finalFilter.endsWith(";"))
    finalFilter.truncate(finalFilter.length() - 1);

  int posOrderBy = finalFilter.findRev("ORDER BY", -1, false);
  QString fieldsOrderBy;

  if (posOrderBy != -1) {
    fieldsOrderBy = finalFilter.mid(posOrderBy + 8, finalFilter.length() - 8);
    finalFilter.remove(posOrderBy, finalFilter.length() - 8);
    if (!fieldsOrderBy.isEmpty())
      fields += QStringList::split(',', fieldsOrderBy);
  }

  if (d->isQuery_) {
    FLSqlQuery *qry = d->db_->manager()->query(d->metadata_->query(), this);
    FLTableMetaData *mtdAux = d->db_->manager()->metadata(d->metadata_->name(), true);
    QStringList fL(QStringList::split(',', mtdAux->fieldList(false)));
    if (mtdAux && !mtdAux->inCache())
      delete mtdAux;

    if (!finalFilter.isEmpty()) {
      for (QStringList::Iterator it = fL.begin(); it != fL.end(); ++it) {
        if (finalFilter.contains(*it) && !(*it).contains('.')) {
          finalFilter.replace(d->metadata_->name() + '.' + *it, *it);
          finalFilter.replace(QRegExp("([^\\w\\.])" + *it + "([^\\w\\.])"),
                              "\\1" + d->metadata_->name() + '.' + *it + "\\2");
          finalFilter.replace(QRegExp('^' + *it + "([^\\w\\.])"),
                              d->metadata_->name() + '.' + *it + "\\1");
        }
      }
    }

    if (qry) {
      QString where = qry->where();
      if (where.isEmpty())
        qry->setWhere(finalFilter);
      else {
        if (finalFilter.isEmpty() || where.contains(finalFilter))
          qry->setWhere(where);
        else if (finalFilter.contains(where))
          qry->setWhere(finalFilter);
        else
          qry->setWhere(where + " AND " + finalFilter);
      }

      fields = sort.toStringList();
      fL = qry->fieldList();

      if (!sort.field(d->metadata_->primaryKey()) && !fieldsOrderBy.upper().contains(d->metadata_->primaryKey().upper()))
        fields << d->metadata_->primaryKey();

      for (QStringList::Iterator it = fields.begin(); it != fields.end(); ++it) {
        for (QStringList::Iterator it2 = fL.begin(); it2 != fL.end(); ++it2) {
          if ((*it2).section('.', 1, 1) == (*it).section(' ', 0, 0)) {
            if (!fieldsOrderBy.isEmpty())
              fieldsOrderBy += "," + (*it2) + " " + (*it).section(' ', 1, 1);
            else
              fieldsOrderBy += (*it2) + " " + (*it).section(' ', 1, 1);
            break;
          }
        }
      }

      if (!fieldsOrderBy.isEmpty()) {
        qry->setOrderBy(fieldsOrderBy);
        setSort(QSqlIndex::fromStringList(QStringList::split(',', fieldsOrderBy), this));
      }

      d->queryOrderBy_ = fieldsOrderBy;
      d->query_ = qry->sql();
      qry->deleteLater();
    }

    return exec(d->query_);
  }

  if (!sort.field(d->metadata_->primaryKey()) &&
      !fieldsOrderBy.upper().contains(d->metadata_->primaryKey().upper()))
    fields << d->metadata_->primaryKey();

  return QSqlCursor::select(finalFilter, QSqlIndex::fromStringList(fields, this));
}

void FLSqlCursor::setSort(const QSqlIndex &sort)
{
  if (d->metadata_ && !sort.field(d->metadata_->primaryKey())) {
    QSqlIndex srt(sort);
    FLFieldMetaData *field = d->metadata_->field(d->metadata_->primaryKey());
  if (field)
  	{
    srt.append(QSqlField(field->name(), FLFieldMetaData::flDecodeType(field->type())));
    QSqlCursor::setSort(srt);
    	} else
	qWarning(tr("FLSqlCursor : No hay una clave primaria establecida"));    	
  } else
    QSqlCursor::setSort(sort);
}

QString FLSqlCursor::baseFilter()
{
  QString relationFilter, finalFilter;

  if (d->cursorRelation_ && d->relation_ && d->metadata_ && d->cursorRelation_->metadata()) {
    QVariant fgValue(d->cursorRelation_->valueBuffer(d->relation_->foreignField()));
    FLFieldMetaData *field = d->metadata_->field(d->relation_->field());

    if (field) {
      relationFilter = d->db_->manager()->formatAssignValue(field, fgValue, true);
      QString filterAc(d->cursorRelation_->filterAssoc(d->relation_->foreignField(),
                                                       d->metadata_));

      if (!filterAc.isEmpty()) {
        if (relationFilter.isEmpty())
          relationFilter = filterAc;
        else
          relationFilter += QString::fromLatin1(" AND ") + filterAc;
      }
    }
  }

  if (!d->mainFilter_.isEmpty())
    finalFilter = d->mainFilter_;

  if (!relationFilter.isEmpty() && !finalFilter.contains(relationFilter)) {
    if (finalFilter.isEmpty())
      finalFilter = relationFilter;
    else
      finalFilter += " AND " + relationFilter;
  }

  return finalFilter;
}

QString FLSqlCursor::curFilter()
{
  QString f = QSqlCursor::filter(), bFilter = baseFilter();

  while (f.endsWith(";"))
    f.truncate(f.length() - 1);
  if (bFilter.isEmpty())
    return f;
  else {
    if (f.isEmpty() || bFilter.contains(f))
      return bFilter;
    else {
      if (f.contains(bFilter))
        return f;
      else
        return bFilter + " AND " + f;
    }
  }
}

void FLSqlCursor::setMainFilter(const QString &f, bool doRefresh)
{
  d->mainFilter_ = f;
  if (doRefresh)
  	{
         refresh();
         emit cursorUpdated(); //Fuerza a que actualice el contenido del FLTable
        }
}

void FLSqlCursor::setFilter(const QString &filter)
{
  QString finalFilter = filter, bFilter = baseFilter();

  if (!bFilter.isEmpty()) {
    if (finalFilter.isEmpty() || bFilter.contains(finalFilter))
      finalFilter = bFilter;
    else if (!finalFilter.contains(bFilter))
      finalFilter = bFilter + " AND " + finalFilter;
  }

  if (!finalFilter.isEmpty() && !d->persistentFilter_.isEmpty() &&
      !finalFilter.contains(d->persistentFilter_))
    finalFilter += " OR " + d->persistentFilter_;

  QSqlCursor::setFilter(finalFilter);
}

void FLSqlCursor::updateBufferCopy()
{
  if (!d->buffer_)
    return;
  if (d->bufferCopy_)
    delete d->bufferCopy_;
  d->bufferCopy_ = new QSqlRecord(*d->buffer_);
}

void FLSqlCursor::setEdition(bool b, void *m)
{
  if (!m) {
    d->edition_ = b;
    return;
  }

  bool stateChanges = (b != d->edition_);

  if (stateChanges && !d->editionStates_)
    d->editionStates_ = new AQBoolFlagStateList;

  if (!d->editionStates_)
    return;

  AQBoolFlagState *i = d->editionStates_->find(m);
  if (!i && stateChanges) {
    i = new AQBoolFlagState;
    i->modifier_ = m;
    i->prevValue_ = d->edition_;
    d->editionStates_->append(i);
  } else if (i) {
    if (stateChanges) {
      d->editionStates_->pushOnTop(i);
      i->prevValue_ = d->edition_;
    } else
      d->editionStates_->erase(i);
  }

  if (stateChanges)
    d->edition_ = b;
}

void FLSqlCursor::restoreEditionFlag(void *m)
{
  if (!d->editionStates_)
    return;
  AQBoolFlagState *i = d->editionStates_->find(m);
  if (i && i == d->editionStates_->cur_)
    d->edition_ = i->prevValue_;
  if (i)
    d->editionStates_->erase(i);
}

void FLSqlCursor::setBrowse(bool b, void *m)
{
  if (!m) {
    d->browse_ = b;
    return;
  }

  bool stateChanges = (b != d->browse_);

  if (stateChanges && !d->browseStates_)
    d->browseStates_ = new AQBoolFlagStateList;

  if (!d->browseStates_)
    return;

  AQBoolFlagState *i = d->browseStates_->find(m);
  if (!i && stateChanges) {
    i = new AQBoolFlagState;
    i->modifier_ = m;
    i->prevValue_ = d->browse_;
    d->browseStates_->append(i);
  } else if (i) {
    if (stateChanges) {
      d->browseStates_->pushOnTop(i);
      i->prevValue_ = d->browse_;
    } else
      d->browseStates_->erase(i);
  }

  if (stateChanges)
    d->browse_ = b;
}

void FLSqlCursor::restoreBrowseFlag(void *m)
{
  if (!d->browseStates_)
    return;
  AQBoolFlagState *i = d->browseStates_->find(m);
  if (i && i == d->browseStates_->cur_)
    d->browse_ = i->prevValue_;
  if (i)
    d->browseStates_->erase(i);
}

bool FLSqlCursor::isModifiedBuffer()
{
  if (!d->buffer_ || !d->bufferCopy_)
    return false;

  for (uint i = 0; i < d->buffer_->count(); ++i) {
    if ((d->buffer_->value(i) != d->bufferCopy_->value(i)) ||
        (d->buffer_->isNull(i) != d->bufferCopy_->isNull(i)))
      return true;
  }

  return false;
}

void FLSqlCursor::setUnLock(const QString &fN, bool v)
{
  if (!d->metadata_ || d->modeAccess_ != BROWSE)
    return;

  if (d->metadata_->fieldType(fN) != FLFieldMetaData::Unlock) {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor::setUnLock sólo permite modificar campos del tipo Unlock"));
#endif
    return;
  }

  d->buffer_ = primeUpdate();
  d->buffer_->setValue(fN, QVariant(v, 0));
  update();
  refreshBuffer();
}

bool FLSqlCursor::isLocked()
{
  if (d->modeAccess_ != INSERT && !d->fieldsNamesUnlock_.isEmpty()) {
    for (QStringList::const_iterator it = d->fieldsNamesUnlock_.begin();
         it != d->fieldsNamesUnlock_.end(); ++it) {
      if (QSqlCursor::contains(*it) && !QSqlCursor::value(*it).toBool())
        return true;
    }
  }
  return false;
}

void FLSqlCursor::setContext(QObject *c)
{
  d->ctxt_ = c;
}

bool FLSqlCursor::bufferIsNull(int i) const
{
  if (d->rawValues_)
    return QSqlRecord::isNull(i);

  if (d->buffer_)
    return d->buffer_->isNull(i);
  else
    return true;
}

bool FLSqlCursor::bufferIsNull(const QString &name) const
{
  if (d->rawValues_)
    return QSqlRecord::isNull(name);

  if (d->buffer_)
    return d->buffer_->isNull(name);
  else
    return true;
}

void FLSqlCursor::bufferSetNull(int i) const
{
  if (d->buffer_)
    d->buffer_->setNull(i);
}

void FLSqlCursor::bufferSetNull(const QString &name) const
{
  if (d->buffer_)
    d->buffer_->setNull(name);
}

bool FLSqlCursor::bufferCopyIsNull(int i) const
{
  if (d->bufferCopy_)
    return d->bufferCopy_->isNull(i);
  else
    return true;
}

bool FLSqlCursor::bufferCopyIsNull(const QString &name) const
{
  if (d->bufferCopy_)
    return d->bufferCopy_->isNull(name);
  else
    return true;
}

void FLSqlCursor::bufferCopySetNull(int i) const
{
  if (d->bufferCopy_)
    d->bufferCopy_->setNull(i);
}

void FLSqlCursor::bufferCopySetNull(const QString &name) const
{
  if (d->bufferCopy_)
    d->bufferCopy_->setNull(name);
}

bool FLSqlCursor::inTransaction()
{
  return (d->db_ ? d->db_->transaction_ > 0 : false);
}

int FLSqlCursor::atFrom()
{
  if (!d->buffer_ || !d->metadata_)
    return 0;

  QString pKN(d->metadata_->primaryKey());
  QVariant pKValue(valueBuffer(pKN));

#if 0
  //#ifndef FL_QUICK_CLIENT
  int pos = d->db_->atFrom(this);
#else
  int pos = -99;
#endif

  if (pos == -99) {
    QSqlQuery q(QString::null, d->db_->db());
    QString sql, sqlIn, cFilter = curFilter();
    FLFieldMetaData *field = d->metadata_->field(pKN);

    QString sqlPriKey;
    QString sqlFrom;
    QString sqlWhere;
    QString sqlPriKeyValue;
    QString sqlOrderBy;

    if (!d->isQuery_ || pKN.contains(".")) {
      sqlPriKey = pKN;
      sqlFrom = d->metadata_->name();
      sql = "SELECT " + sqlPriKey + " FROM " + sqlFrom;
    } else {
      FLSqlQuery *qry = d->db_->manager()->query(d->metadata_->query(), this);
      if (qry) {
        sqlPriKey = d->metadata_->name() + "." + pKN;
        sqlFrom = qry->from();
        sql = "SELECT " + sqlPriKey + " FROM " + sqlFrom;
        qry->deleteLater();
      } else {
#ifdef FL_DEBUG
        qWarning(tr("FLSqlCursor::atFrom Error al crear la consulta"));
#endif
        QSqlCursor::seek(at());
        pos = isValid() ? at() : 0;
        return pos;
      }
    }
    if (!cFilter.isEmpty()) {
      sqlWhere = cFilter;
      sql += " WHERE " + sqlWhere;
    } else
      sqlWhere = "1=1";

    if (field) {
      sqlPriKeyValue = d->db_->manager()->formatAssignValue(field, pKValue);
      if (!cFilter.isEmpty())
        sqlIn = sql + " AND " + sqlPriKeyValue;
      else
        sqlIn = sql + " WHERE " + sqlPriKeyValue;
      q.exec(sqlIn);
      if (!q.next()) {
        QSqlCursor::seek(at());
        pos = isValid() ? at() : 0;
        return pos;
      }
    }

    if (d->isQuery_ && !d->queryOrderBy_.isEmpty()) {
      sqlOrderBy = d->queryOrderBy_;
      sql += " ORDER BY " + sqlOrderBy;
    } else if (sort().count() > 0) {
      sqlOrderBy = sort().toString();
      sql += " ORDER BY " + sqlOrderBy;
    }

    // ###
    if (!sqlPriKeyValue.isEmpty() && d->db_->canOverPartition()) {
      int posEqual = sqlPriKeyValue.find("=");
      QString leftSqlPriKey(sqlPriKeyValue.left(posEqual));
      QString sqlRowNum;
      sqlRowNum = "SELECT rownum FROM (SELECT row_number() OVER (ORDER BY ";
      sqlRowNum += sqlOrderBy;
      sqlRowNum += ") as rownum,";
      sqlRowNum += sqlPriKey;
      sqlRowNum += " as prikey";
      sqlRowNum += " FROM ";
      sqlRowNum += sqlFrom;
      sqlRowNum += " WHERE ";
      sqlRowNum += sqlWhere;
      sqlRowNum += " ORDER BY ";
      sqlRowNum += sqlOrderBy;
      sqlRowNum += ") as subnumrow where ";
      sqlRowNum += sqlPriKeyValue.replace(leftSqlPriKey, "prikey");
      if (q.exec(sqlRowNum) && q.next()) {
        pos = q.value(0).toInt() - 1;
        if (pos >= 0)
          return pos;
      }
    }

    bool found = false;
    q.exec(sql);

    pos = 0;
    if (q.first()) {
      if (q.value(0) != pKValue) {
        pos = q.size();
        if (q.last() && pos > 1) {
          --pos;
          if (q.value(0) != pKValue) {
            while (q.prev() && pos > 1) {
              --pos;
              if (q.value(0) == pKValue) {
                found = true;
                break;
              }
            }
          } else
            found = true;
        }
      } else
        found = true;
    }

    if (!found) {
      QSqlCursor::seek(at());
      pos = isValid() ? at() : 0;
    }
  }

  return pos;
}

int FLSqlCursor::atFromBinarySearch(const QString &fN, const QString &v, bool orderAsc)
{
  if (!QSqlCursor::contains(fN) || !QSqlCursor::isActive())
    return -1;

  bool isForward = QSqlCursor::isForwardOnly();
  int ret = -1, atBackup = QSqlCursor::at();
  int ini = 0, fin = QSqlCursor::size() - 1, mid, cmp;

  QSqlCursor::setForwardOnly(false);
  while (ini <= fin) {
    mid = (ini + fin) / 2;
    QSqlCursor::seek(mid);
    cmp = (orderAsc ?
           QString::localeAwareCompare(v, QSqlCursor::value(fN).toString()) :
           QString::localeAwareCompare(QSqlCursor::value(fN).toString(), v));
    if (!cmp) {
      ret = mid;
      break;
    } else {
      if (cmp > 0)
        ini = mid + 1;
      else
        fin = mid - 1;
    }
    ret = ini;
  }
  QSqlCursor::seek(atBackup);
  QSqlCursor::setForwardOnly(isForward);

  return ret;
}

bool FLSqlCursor::exec(const QString &query)
{
  bool ret = QSqlCursor::exec(query);
  if (ret && d->isQuery_ && (d->query_ != query || !d->populated_)) {
    d->query_ = query;
    QSqlCursor::clear();
    d->populated_ = false;
    populateCursor();
    setExtraFieldAttributes();
  } else if (ret && !d->isQuery_ && d->query_ != query) {
    d->query_ = query;
    setExtraFieldAttributes();
  }
  if (lastError().type() != QSqlError::None) {
    //    QString msg("<p><img source=\"remove.png\" align=\"right\">" +
    //                "<b><u>SQL ERROR</u></b><br><br>" +
    //                QString(lastError().driverText()) + "<br>" +
    //                QString(lastError().databaseText()) + "</p>");
    //    msg.replace("\n", "<br>");
    //    aqApp->popupWarn(msg);
    return false;
  }
  return ret;
}

void FLSqlCursor::populateCursor()
{
  if (d->metadata_ && d->isQuery_ && driver()) {
    QSqlCursor::setName(d->metadata_->name(), false);
    QSqlRecordInfo inf = driver()->recordInfo(*(QSqlQuery *)this);
    for (QSqlRecordInfo::const_iterator it = inf.begin(); it != inf.end(); ++it)
      QSqlCursor::append(*it);
    QSqlCursor::setPrimaryIndex(driver()->primaryIndex(d->metadata_->name()));
    d->populated_ = true;
  }
}

void FLSqlCursor::setNotGenerateds()
{
  if (!d->metadata_ || !d->isQuery_ || !d->buffer_)
    return;
  QSqlRecordInfo inf = driver()->recordInfo(*(QSqlQuery *)this);
  for (QSqlRecordInfo::const_iterator it = inf.begin(); it != inf.end(); ++it) {
    FLFieldMetaData *field = d->metadata_->field((*it).name());
    if (!field) {
      d->buffer_->setGenerated((*it).name(), false);
      continue;
    }
    if (!field->generated()) {
      d->buffer_->setGenerated((*it).name(), false);
      continue;
    }
    FLTableMetaData *mtd = field->metadata();
    if (mtd && mtd->name() != d->metadata_->name())
      d->buffer_->setGenerated((*it).name(), false);
  }
}

void FLSqlCursor::setExtraFieldAttributes()
{
  if (!d->metadata_)
    return;

  clearMapCalcFields();

  const FLTableMetaData::FLFieldMetaDataList *fieldList = d->metadata_->fieldList();
  if (!fieldList)
    return;

  FLFieldMetaData *field;
  QDictIterator<FLFieldMetaData> it(*fieldList);
  while ((field = it.current()) != 0) {
    ++it;
    if (field->fullyCalculated())
      QSqlCursor::setCalculated(field->name(), true);
    if (field->trimed())
      QSqlCursor::setTrimmed(field->name(), true);
    if (field->generated())
      QSqlCursor::setGenerated(field->name(), true);
  }
}

void FLSqlCursor::clearMapCalcFields()
{
  d->mapCalcFields_.clear();
}

QVariant FLSqlCursor::calculateField(const QString &name)
{
  if (!d->metadata_ || !d->ctxt_)
    return QVariant();

  int pos = at();
  QString fKey(QString::number(pos) + '@' + name);
  QMap<QString, QVariant>::const_iterator it(d->mapCalcFields_.find(fKey));
  if (it != d->mapCalcFields_.end())
    return *it;

  QSArgumentList arglist;
  QSArgument ret;

  arglist.append(QSArgument(name));
  arglist.append(QSArgument(this));
  arglist.append(QSArgument(d->metadata_->name()));
  arglist.append(QSArgument(d->metadata_->query()));
  if (d->isQuery_ && !d->queryOrderBy_.isEmpty())
    arglist.append(QSArgument(d->queryOrderBy_));
  else if (sort().count() > 0)
    arglist.append(QSArgument(sort().toString()));
  else
    arglist.append(QSArgument(QString::null));
  arglist.append(QSArgument(pos));

  QString idMod(d->db_->managerModules()->idModuleOfFile(d->metadata_->name() +
                                                         QString::fromLatin1(".mtd")));
  QString functionFullyCalc;
  if (!idMod.isEmpty())
    functionFullyCalc = idMod + QString::fromLatin1(".fullyCalculateField");
  else
    functionFullyCalc = QString::fromLatin1("flfactppal.fullyCalculateField");
  ret = aqApp->call(functionFullyCalc, arglist, 0);

  if (ret.type() == QSArgument::Variant) {
    QVariant vRet(ret.variant());
    d->mapCalcFields_.insert(fKey, vRet);
    return vRet;
  }
  return QVariant();
}

int FLSqlCursor::transactionLevel()
{
  return (d->db_ ? d->db_->transaction_ : 0);
}

QStringList FLSqlCursor::transactionsOpened()
{
  QStringList list;
  for (QValueStack<int>::const_iterator it = d->transactionsOpened_.begin();
       it != d->transactionsOpened_.end(); ++it)
    list << QString::number(*it);
  return list;
}

void FLSqlCursor::rollbackOpened(int count, const QString &msg)
{
  int ct = (count < 0 ? d->transactionsOpened_.count() : count);
  if (ct > 0 && !msg.isEmpty()) {
    QString t(d->metadata_ ? d->metadata_->name() : QString(QObject::name()));
    QString m(msg + QString("SqlCursor::rollbackOpened: %1 %1").arg(count).arg(t));
    d->msgBoxWarning(m, false);
#ifndef QSDEBUGGER
    printf("%s\n", m.latin1());
#endif
  } else if (ct > 0) {
    qWarning("SqlCursor::rollbackOpened: %d %s", count, QObject::name());
#ifndef QSDEBUGGER
    printf("SqlCursor::rollbackOpened: %d %s\n", count, QObject::name());
#endif
  }
  for (int i = 0; i < ct; ++i) {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor : Deshaciendo transacción abierta %1").arg(transactionLevel()));
#endif
    rollback();
  }
}

void FLSqlCursor::commitOpened(int count, const QString &msg)
{
  int ct = (count < 0 ? d->transactionsOpened_.count() : count);
  if (ct > 0 && !msg.isEmpty()) {
    QString t(d->metadata_ ? d->metadata_->name() : QString(QObject::name()));
    QString m(msg + QString("SqlCursor::commitOpened: %1 %1").arg(count).arg(t));
    d->msgBoxWarning(m, false);
#ifndef QSDEBUGGER
    printf("%s\n", m.latin1());
#endif
  } else if (ct > 0) {
    qWarning("SqlCursor::commitOpened: %d %s", count, QObject::name());
#ifndef QSDEBUGGER
    printf("SqlCursor::commitOpened: %d %s\n", count, QObject::name());
#endif
  }
  for (int i = 0; i < ct; ++i) {
#ifdef FL_DEBUG
    qWarning(tr("FLSqlCursor : Terminando transacción abierta %1").arg(transactionLevel()));
#endif
    commit();
  }
}

void FLSqlCursor::setAcTable(const QString &ac)
{
  d->idAc_++;
  d->id_ = QString::number(d->idAc_) + QString::number(d->idAcos_) + QString::number(d->idCond_);
  d->acPermTable_ = ac;
}

void FLSqlCursor::setAcosTable(const QStringList &acos)
{
  d->idAcos_++;
  d->id_ = QString::number(d->idAc_) + QString::number(d->idAcos_) + QString::number(d->idCond_);
  d->acosTable_ = acos;
}

void FLSqlCursor::setAcosCondition(const QString &condName, AcosConditionEval cond, const QVariant &condVal)
{
  d->idCond_++;
  d->id_ = QString::number(d->idAc_) + QString::number(d->idAcos_) + QString::number(d->idCond_);
  d->acosCondName_ = condName;
  d->acosCond_ = cond;
  d->acosCondVal_ = condVal;
}

void FLSqlCursor::clearPersistentFilter()
{
  d->persistentFilter_ = QString::null;
}

QString FLSqlCursor::connectionName() const
{
  return d->db_->connectionName();
}

QStringList FLSqlCursor::concurrencyFields()
{
  if (!d->buffer_ || !d->bufferCopy_ || !d->metadata_)
    return QStringList();

  QStringList modFields;
  for (uint i = 0; i < d->buffer_->count(); ++i) {
    if ((d->buffer_->value(i) != d->bufferCopy_->value(i)) ||
        (d->buffer_->isNull(i) != d->bufferCopy_->isNull(i)))
      modFields << d->buffer_->fieldName(i);
  }
  if (modFields.isEmpty())
    return QStringList();

  QString pKN(d->metadata_->primaryKey());
  QString pKWhere(
    d->db_->manager()->formatAssignValue(
      d->metadata_->field(pKN), valueBuffer(pKN)
    )
  );

  FLSqlQuery q(0, d->db_->connectionName());
  q.setTablesList(d->metadata_->name());
  q.setSelect(modFields.join(","));
  q.setFrom(d->metadata_->name());
  q.setWhere(pKWhere);
  q.setForwardOnly(true);
  if (!q.exec() || !q.next())
    return QStringList();

  QStringList colFields;
  for (QStringList::const_iterator it = modFields.begin(); it != modFields.end(); ++it) {
    QVariant val(q.value(*it));
    if (val != d->bufferCopy_->value(*it) && val != d->buffer_->value(*it))
      colFields << *it;
  }
  return colFields;
}

void FLSqlCursor::changeConnection(const QString &connName)
{
  QString curConnName(connectionName());
  if (curConnName == connName)
    return;

  FLSqlDatabase *newDb = FLSqlConnections::database(connName);
  if (curConnName == newDb->connectionName())
    return;

  if (!d->transactionsOpened_.isEmpty()) {
    FLTableMetaData *mtd = d->metadata_;
    QString t(mtd ? mtd->name() : QString(QObject::name()));
    rollbackOpened(
      -1, tr("Se han detectado transacciones no finalizadas en la última operación.\n"
             "Se van a cancelar las transacciones pendientes.\n"
             "Los últimos datos introducidos no han sido guardados, por favor\n"
             "revise sus últimas acciones y repita las operaciones que no\n"
             "se han guardado.\n") +
      QString("SqlCursor::changeConnection: %1\n").arg(t)
    );
  }

  bool bufferNoEmpty = (d->buffer_ != 0);
  QSqlRecord bufferBackup;

  if (bufferNoEmpty) {
    bufferBackup = *d->buffer_;
    d->buffer_ = 0;
  }

  QSqlCursor c(QString::null, true, newDb->db());
  *(static_cast<QSqlCursor *>(this)) = c;
  d->db_ = newDb;
  init(d->curName_, true, d->cursorRelation_, d->relation_);

  if (bufferNoEmpty) {
    d->buffer_ = QSqlCursor::editBuffer();
    *d->buffer_ = bufferBackup;
  }

  emit connectionChanged();
}

QString FLSqlCursor::filterAssoc(const QString &fieldName, FLTableMetaData *tableMD)
{
  FLTableMetaData *mtd = d->metadata_;
  if (!mtd)
    return QString::null;

  FLFieldMetaData *field = mtd->field(fieldName);
  if (!field)
    return QString::null;

  bool ownTMD = false;
  if (!tableMD) {
    ownTMD = true;
    tableMD = d->db_->manager()->metadata(field->relationM1()->foreignTable());
  }
  if (!tableMD)
    return QString::null;

  FLFieldMetaData *fieldAc = field->associatedField();
  if (!fieldAc) {
    if (ownTMD && !tableMD->inCache())
      delete tableMD;
    return QString::null;
  }

  QString fieldBy(field->associatedFieldFilterTo());
  if (!tableMD->field(fieldBy) || d->buffer_->isNull(fieldAc->name())) {
    if (ownTMD && !tableMD->inCache())
      delete tableMD;
    return QString::null;
  }

  QVariant vv(d->buffer_->value(fieldAc->name()));
  if (vv.isValid() && !vv.isNull()) {
    if (ownTMD && !tableMD->inCache())
      delete tableMD;
    return d->db_->manager()->formatAssignValue(fieldBy, fieldAc, vv, true);
  }
  if (ownTMD && !tableMD->inCache())
    delete tableMD;
  return QString::null;
}
