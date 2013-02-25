/***************************************************************************
                         FLTableMetaData.cpp
                         -------------------
begin                : Mon Jul 2 2001
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

#include "FLTableMetaData.h"
#include "FLFieldMetaData.h"
#include "FLRelationMetaData.h"
#include "FLCompoundKey.h"

FLTableMetaDataPrivate::FLTableMetaDataPrivate(const QString &n, const QString &a, const QString &q) :
  name_(n.lower()), alias_(a), compoundKey_(0), query_(q)
{
  fieldList_ = new FLTableMetaData::FLFieldMetaDataList(71);
  fieldList_->setAutoDelete(true);
}

FLTableMetaDataPrivate::~FLTableMetaDataPrivate()
{
  delete fieldList_;
  if (compoundKey_)
    delete compoundKey_;
}

FLTableMetaData::FLTableMetaData(const QString &n, const QString &a, const QString &q) : QObject(0, n.lower())
{
  d = new FLTableMetaDataPrivate(n, a, q);
}

FLTableMetaData::~FLTableMetaData()
{
  if (d)
    delete d;
}

void FLTableMetaData::addFieldMD(FLFieldMetaData *f)
{
  if (f) {
    f->setMetadata(this);
    d->fieldList_->insert(f->d->name_.lower(), f);
    d->addFieldName(f->d->name_);
    d->formatAlias(f);
    if (f->d->type_ == FLFieldMetaData::Unlock)
      d->fieldsNamesUnlock_.append(f->d->name_);
    if (f->d->isPrimaryKey_)
      d->primaryKey_ = f->d->name_.lower();
  }
}

void FLTableMetaData::removeFieldMD(const QString &fN)
{
  if (fN.isEmpty())
    return;
  d->fieldList_->remove(fN.lower());
  d->removeFieldName(fN);
}

QString FLTableMetaData::fieldNameToAlias(const QString &fN) const
{
  if (fN.isEmpty())
    return fN;

  QMap<QString, QString>::const_iterator it(d->fieldAliasMap_.find(fN.lower()));
  if (it != d->fieldAliasMap_.end())
    return *it;

  return fN;
}

QString FLTableMetaData::fieldAliasToName(const QString &aN) const
{
  if (aN.isEmpty())
    return aN;

  QMap<QString, QString>::const_iterator it(d->aliasFieldMap_.find(aN));
  if (it != d->aliasFieldMap_.end())
    return *it;

  return aN;
}

int FLTableMetaData::fieldType(const QString &fN) const
{
  if (fN.isEmpty())
    return QVariant::Invalid;
  else
  	{
 	 FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  	if (field)
    	    return field->d->type_;

	return QVariant::Invalid;
	}
}

int FLTableMetaData::fieldLength(const QString &fN) const
{
  if (fN.isEmpty())
    return 0;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field)
    return field->d->length_;

  return 0;
}

int FLTableMetaData::fieldPartInteger(const QString &fN) const
{
  if (fN.isEmpty())
    return 0;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field)
    return field->d->partInteger_;

  return 0;
}

int FLTableMetaData::fieldPartDecimal(const QString &fN) const
{
  if (fN.isEmpty())
    return 0;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field)
    return field->d->partDecimal_;

  return 0;
}

bool FLTableMetaData::fieldIsPrimaryKey(const QString &fN) const
{
  return (d->primaryKey_ == fN.lower());
}

QString FLTableMetaData::primaryKey(bool prefixTable) const
{
  return (prefixTable ? d->name_ + QString::fromLatin1(".") + d->primaryKey_ : d->primaryKey_);
}

bool FLTableMetaData::fieldIsIndex(const QString &fN) const
{
  if (fN.isEmpty())
    return false;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field)
    return field->d->isIndex_;

  return false;
}

bool FLTableMetaData::fieldIsUnique(const QString &fN) const
{
  if (fN.isEmpty())
    return false;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field)
    return field->d->isUnique_;

  return false;
}

bool FLTableMetaData::fieldIsCounter(const QString &fN) const
{
  if (fN.isEmpty())
    return false;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field)
    return field->d->contador_;

  return false;
}

bool FLTableMetaData::fieldAllowNull(const QString &fN) const
{
  if (fN.isEmpty())
    return false;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field)
    return field->d->allowNull_;

  return false;
}

bool FLTableMetaData::fieldCalculated(const QString &fN) const
{
  if (fN.isEmpty())
    return false;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field)
    return field->d->calculated_;

  return false;
}

bool FLTableMetaData::fieldVisible(const QString &fN) const
{
  if (fN.isEmpty())
    return false;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field)
    return field->d->visible_;

  return false;
}

FLFieldMetaData *FLTableMetaData::field(const QString &fN) const
{
  if (fN.isEmpty())
    return 0;

  return d->fieldList_->find(fN.lower());
}

QString FLTableMetaData::fieldTableM1(const QString &fN) const
{
  if (fN.isEmpty())
    return QString::null;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field && field->d->relationM1_)
    return field->d->relationM1_->foreignTable();

  return QString::null;
}

QString FLTableMetaData::fieldForeignFieldM1(const QString &fN) const
{
  if (fN.isEmpty())
    return QString::null;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field && field->d->relationM1_)
    return field->d->relationM1_->foreignField();

  return QString::null;
}

FLRelationMetaData *FLTableMetaData::relation(const QString &fN, const QString &fFN, const QString &fTN) const
{
  if (fN.isEmpty())
    return 0;

  FLFieldMetaData *field = d->fieldList_->find(fN.lower());
  if (field) {
    if (field->d->relationM1_ && field->d->relationM1_->foreignField() == fFN.lower() &&
        field->d->relationM1_->foreignTable() == fTN.lower())
      return field->d->relationM1_;
    FLFieldMetaData::FLRelationMetaDataList *relationList = field->d->relationList_;
    if (!relationList)
      return 0;
    if (relationList->isEmpty())
      return 0;
    FLRelationMetaData *r;
    QPtrListIterator<FLRelationMetaData> itR(*relationList);
    while ((r = itR.current()) != 0) {
      ++itR;
      if (r->foreignField() == fFN.lower() && r->foreignTable() == fTN.lower())
        return r;
    }
  }

  return 0;
}

FLTableMetaData::FLFieldMetaDataList *
FLTableMetaData::fieldListOfCompoundKey(const QString &fN) const
{
  if (d->compoundKey_ && d->compoundKey_->hasField(fN))
    return d->compoundKey_->fieldList();
  return 0;
}

QString FLTableMetaData::fieldList(bool prefixTable) const
{
  if (!prefixTable)
    return d->fieldsNames_;

  QStringList fieldsNames = QStringList::split(',', d->fieldsNames_);
  QString ret;
  for (QStringList::Iterator it = fieldsNames.begin(); it != fieldsNames.end(); ++it) {
    if (!(*it).contains(".")) {
      if (ret.isEmpty())
        ret +=  d->name_ + QString::fromLatin1(".") + (*it);
      else
        ret += QString::fromLatin1(",") +  d->name_ +
               QString::fromLatin1(".") + (*it);
    }
  }

  return ret;
}

void FLTableMetaData::setName(const QString &n)
{
  QObject::setName(n);
  d->name_ = n;
}

void FLTableMetaDataPrivate::formatAlias(FLFieldMetaData *f)
{
  if (!f)
    return;

  QString alias(f->alias());
  QString field(f->name().lower());
  QMap<QString, QString>::const_iterator it(aliasFieldMap_.find(alias));

  if (it != aliasFieldMap_.end()) {
    alias += QString::fromLatin1("(") + QString::number(aliasFieldMap_.size() + 1) +
             QString::fromLatin1(")");
    f->d->alias_ = alias;
  }

  aliasFieldMap_[ alias ] = field;
  fieldAliasMap_[ field ] = alias;
}

void FLTableMetaDataPrivate::addFieldName(const QString &n)
{
  if (fieldsNames_.isEmpty())
    fieldsNames_ += n.lower();
  else
    fieldsNames_ += "," + n.lower();
}

void FLTableMetaDataPrivate::removeFieldName(const QString &n)
{
  if (!fieldsNames_.isEmpty()) {
    QStringList list(QStringList::split(',', fieldsNames_));
    list.remove(n.lower());
    fieldsNames_ = list.join(",");
  }
}
