/***************************************************************************
                            FLSqlQuery.cpp
                         -------------------
begin                : sáb jun 22 2002
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
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qdict.h>
#include <qstringlist.h>

#include "FLApplication.h"
#include "FLSqlQuery.h"
#include "FLParameterQuery.h"
#include "FLGroupByQuery.h"
#include "FLManager.h"
#include "FLSqlDatabase.h"
#include "FLSqlConnections.h"
#include "FLFieldMetaData.h"

#ifdef FL_DEBUG
AQ_EXPORT long FLSqlQuery::countRefQuery = 0;
#endif

FLSqlQueryPrivate::FLSqlQueryPrivate() :
  name_(QString::null), select_(QString::null), from_(QString::null),
  where_(QString::null), orderBy_(QString::null), parameterDict_(0),
  groupDict_(0), fieldMetaDataList_(0), db_(0) {}

FLSqlQueryPrivate::~FLSqlQueryPrivate()
{
  delete parameterDict_;
  delete groupDict_;
  if (fieldMetaDataList_) {
    FLFieldMetaData *field;
    QDictIterator<FLFieldMetaData> it(*fieldMetaDataList_);
    while ((field = it.current()) != 0) {
      ++it;
      if (field->deref())
        delete field;
    }
    delete fieldMetaDataList_;
  }
}

FLSqlQuery::FLSqlQuery(QObject *parent, const QString &connectionName)
  : QObject(parent),
    qry_(QSqlQuery(FLSqlConnections::database(connectionName)->db()))
{
#ifdef FL_DEBUG
  countRefQuery++;
#endif
  d = new FLSqlQueryPrivate();
  d->db_ = FLSqlConnections::database(connectionName);
}

FLSqlQuery::~FLSqlQuery()
{
  delete d;
#ifdef FL_DEBUG
  countRefQuery--;
#endif
}

void FLSqlQuery::setSelect(const QString &s, const QString &sep)
{
  d->select_ = s.stripWhiteSpace();
  d->select_ = d->select_.simplifyWhiteSpace();

  if (s.find(sep) == -1 && s.find('*') == -1) {
    d->fieldList_.clear();
    d->fieldList_.append(s);
    return;
  }

  QStringList fieldListAux(QStringList::split(sep, s));
  for (QStringList::Iterator it = fieldListAux.begin(); it != fieldListAux.end(); ++it)
    (*it) = (*it).stripWhiteSpace();

  QString table, field;
  d->fieldList_.clear();
  for (QStringList::Iterator it = fieldListAux.begin(); it != fieldListAux.end(); ++it) {
    table = (*it).section('.', 0, 0);
    field = (*it).section('.', 1, 1);
    if (field == "*") {
      FLTableMetaData *mtd = d->db_->manager()->metadata(table, true);
      if (mtd) {
        d->fieldList_ += QStringList::split(',', mtd->fieldList(true));
        if (!mtd->inCache())
          delete mtd;
      }
    } else
      d->fieldList_.append(*it);
  }

  d->select_ = d->fieldList_.join(",");
}

QString FLSqlQuery::sql()
{
  for (QStringList::Iterator it = d->tablesList_.begin(); it != d->tablesList_.end(); ++it) {
    if (!d->db_->manager()->existsTable(*it) && !d->db_->manager()->createTable(*it))
      return QString::null;
  }

  QString res;
  if (d->from_.isEmpty())
    res = QString::fromLatin1("SELECT ") + d->select_;
  else if (d->where_.isEmpty())
    res = QString::fromLatin1("SELECT ") + d->select_ +
          QString::fromLatin1(" FROM ") + d->from_;
  else
    res = QString::fromLatin1("SELECT ") + d->select_ +
          QString::fromLatin1(" FROM ") + d->from_ +
          QString::fromLatin1(" WHERE ") + d->where_;

  if (d->groupDict_ && d->orderBy_.isEmpty()) {
    res += QString::fromLatin1(" ORDER BY ");
    QDictIterator < FLGroupByQuery > itg(*d->groupDict_);
    for (uint i = 0; i < d->groupDict_->count(); i++) {
      if (i == 0)
        res += d->groupDict_->find(QString::number(i)) ->field();
      else
        res += QString::fromLatin1(", ") +
               d->groupDict_->find(QString::number(i)) ->field();
    }
  } else if (!d->orderBy_.isEmpty())
    res += QString::fromLatin1(" ORDER BY ") + d->orderBy_;

  if (d->parameterDict_) {
    QDictIterator < FLParameterQuery > it(*d->parameterDict_);
    for (; it.current(); ++it) {
      QVariant v = it.current() ->value();

      if (!v.isValid()) {
        bool ok = true;
        v = QVariant(QInputDialog::getText(QApplication::tr
                                           ("Entrada de parámetros de la consulta"),
                                           it.current() ->alias(), QLineEdit::Normal,
                                           QString::null, &ok, qApp->mainWidget()));
      }
      res = res.replace(QRegExp(QString::fromLatin1("\\[") + it.currentKey() +
                                QString::fromLatin1("\\]")),
                        d->db_->manager()->formatValue(it.current() ->type(), v));
    }
  }

  return res;
}

bool FLSqlQuery::exec()
{
  return exec(sql());
}

void FLSqlQuery::setGroupDict(FLGroupByQueryDict *gd)
{
  if (!gd)
    return ;

  gd->setAutoDelete(true);

  if (gd->isEmpty())
    return ;

  if (d->groupDict_)
    delete d->groupDict_;

  d->groupDict_ = gd;
}

void FLSqlQuery::setParameterDict(FLParameterQueryDict *pd)
{
  if (!pd)
    return ;

  pd->setAutoDelete(true);

  if (pd->isEmpty())
    return ;

  if (d->parameterDict_)
    delete d->parameterDict_;

  d->parameterDict_ = pd;
}

void FLSqlQuery::addParameter(const FLParameterQuery *p)
{
  if (!d->parameterDict_)
    d->parameterDict_ = new FLParameterQueryDict;

  d->parameterDict_->setAutoDelete(true);

  if (p)
    d->parameterDict_->insert(p->name(), p);
}

void FLSqlQuery::addGroup(const FLGroupByQuery *g)
{
  if (!d->groupDict_)
    d->groupDict_ = new FLGroupByQueryDict;

  d->groupDict_->setAutoDelete(true);

  if (g)
    d->groupDict_->insert(QString::number(g->level()), g);
}

QVariant FLSqlQuery::value(const QString &n, bool raw) const
{
  if (!isValid())
    return QVariant();
  int i = d->fieldList_.findIndex(n);
  return (i >= 0 ? value(i, raw) : QVariant());
}

bool FLSqlQuery::isNull(const QString &n) const
{
  if (!isValid())
    return true;
  int i = d->fieldList_.findIndex(n);
  return (i >= 0 ? isNull(i) : true);
}


QVariant FLSqlQuery::value(int i, bool raw) const
{
  if (!isValid() && i >= 0)
    return QVariant();
  QVariant v(QSqlQuery_value(i));
  QVariant::Type type = v.type();
  if (!raw  && type == QVariant::String) {
    if (!v.isNull()) {
      QVariant vLarge(d->db_->manager()->fetchLargeValue(v.toString()));
      if (vLarge.isValid())
        return vLarge;
    }
  } else if (type == QVariant::ULongLong || type == QVariant::LongLong)
    return v.toDouble();
  return v;
}

QString FLSqlQuery::posToFieldName(const int p) const
{
  if (p < 0 || p >=  d->fieldList_.size())
    return QString::null;
  QStringList::ConstIterator it(d->fieldList_.at(p));
  return (it != d->fieldList_.end() ? *it : QString::null);
}

int FLSqlQuery::fieldNameToPos(const QString &n) const
{
  return d->fieldList_.findIndex(n);
}


void FLSqlQuery::setTablesList(const QString &tl)
{
  d->tablesList_.clear();
  d->tablesList_ = QStringList::split(',', tl);
  if (d->tablesList_.count() > 0) {
    QString firstTable = d->tablesList_.first();
    FLTableMetaData *metadata_ = d->db_->manager()->metadata(firstTable);
    QSqlDatabase *db = NULL;
  
    if (metadata_->transactionBehavior() == "autocommit") {
      db = d->db_->dbAux();
    }
    if (db) {
      qry_ = QSqlQuery(db);
    }
    
  }
    
}

void FLSqlQuery::setValueParam(const QString &name, const QVariant &v)
{
  if (d->parameterDict_)
    (*d->parameterDict_)[ name ] ->setValue(v);
}

QVariant FLSqlQuery::valueParam(const QString &name) const
{
  return d->parameterDict_ ? (*d->parameterDict_)[ name ] ->value() : QVariant();
}

int FLSqlQuery::size() const
{
  return QSqlQuery_size();
}

FLTableMetaData::FLFieldMetaDataList *FLSqlQuery::fieldMetaDataList()
{
  if (!d->fieldMetaDataList_) {
    d->fieldMetaDataList_ = new FLTableMetaData::FLFieldMetaDataList(71);
    QString table, field;
    for (QStringList::Iterator it = d->fieldList_.begin(); it != d->fieldList_.end(); ++it) {
      table = (*it).section('.', 0, 0);
      field = (*it).section('.', 1, 1);
      FLTableMetaData *mtd = d->db_->manager()->metadata(table, true);
      if (!mtd)
        continue;
      FLFieldMetaData *fd = mtd->field(field);
      if (fd) {
        fd->ref();
        d->fieldMetaDataList_->insert(field.lower(), fd);
      }
      if (!mtd->inCache())
        delete mtd;
    }
  }
  return d->fieldMetaDataList_;
}

void FLSqlQuery::showDebug()
{
#ifdef FL_DEBUG
  if (!isActive())
    qWarning
    ("DEBUG : La consulta no está activa : No se ha ejecutado exec() o la sentencia SQL no es válida");

  qWarning("DEBUG : Nombre de la consulta :  " + d->name_);
  qWarning("DEBUG : Niveles de agrupamiento : ");

  if (d->groupDict_) {
    QDictIterator < FLGroupByQuery > it(*d->groupDict_);
    for (; it.current(); ++it) {
      qWarning("**Nivel : " + QString::number(it.current() ->level()));
      qWarning("**Campo : " + it.current() ->field());
    }
  } else
    qWarning("**No hay niveles de agrupamiento");

  qWarning("DEBUG : Parámetros : ");
  if (d->parameterDict_) {
    QDictIterator < FLParameterQuery > it2(*d->parameterDict_);
    for (; it2.current(); ++it2) {
      qWarning("**Nombre : " + it2.current() ->name());
      qWarning("Alias : " + it2.current() ->alias());
      qWarning("Tipo : " + QString::number(it2.current() ->type()));
      qWarning("**Valor : " + it2.current() ->value().toString());
    }
  } else
    qWarning("**No hay parametros");

  qWarning("DEBUG : Sentencia SQL : ");
  qWarning(sql());

  if (d->fieldList_.empty()) {
    qWarning("DEBUG ERROR : No hay campos en la consulta.");
    return ;
  }

  qWarning("DEBUG : Campos de la consulta : ");
  for (QStringList::Iterator it = d->fieldList_.begin(); it != d->fieldList_.end(); ++it)
    qWarning("**" + (*it));


  qWarning("DEBUG : Contenido de la consulta: ");
  while (next()) {
    QString linea = QString::null;

    QStringList::Iterator it;
    for (uint i = 0; i < d->fieldList_.count(); i++) {
      it = d->fieldList_.at(i);
      linea += "__" + value((*it)).toString();
    }
    qWarning(linea);
  }
#endif
}

void FLSqlQuery::setFrom(const QString &f)
{
  d->from_ = f.stripWhiteSpace();
  d->from_ = d->from_.simplifyWhiteSpace();
}

void FLSqlQuery::setWhere(const QString &w)
{
  d->where_ = w.stripWhiteSpace();
  d->where_ = d->where_.simplifyWhiteSpace();
}

void FLSqlQuery::setOrderBy(const QString &w)
{
  d->orderBy_ = w.stripWhiteSpace();
  d->orderBy_ = d->orderBy_.simplifyWhiteSpace();
}
