/***************************************************************************
 FLFieldMetaData.cpp
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

#include "FLFieldMetaData.h"
#include "FLRelationMetaData.h"

FLFieldMetaDataPrivate::FLFieldMetaDataPrivate(const QString &n, const QString &a, bool aN,
                                               bool iPK, int t, int l, bool c, bool v, bool ed,
                                               int pI, int pD, bool iNX, bool uNI, bool coun,
                                               const QVariant &defValue, bool oT,
                                               const QString &rX, bool vG, bool gen, bool iCK) :
  name_(n.lower()), alias_(a), allowNull_((!c) ? aN : true), isPrimaryKey_(iPK), type_(t),
  length_(l), calculated_(c), visible_(v), editable_(ed), partInteger_(pI), partDecimal_(pD),
  isIndex_(iNX), isUnique_(uNI), contador_(coun), relationList_(0), relationM1_(0),
  associatedField_(0), defaultValue_(defValue), outTransaction_(oT), regExpValidator_(rX),
  visibleGrid_(vG), generated_(gen), isCompoundKey_(iCK), hasOptionsList_(false), mtd_(0)
{
  if (l < 0)
    length_ = 0;
  if (pI < 0)
    partInteger_ = 0;
  if (pD < 0)
    partDecimal_ = 0;

  if (t != QVariant::String && l != 0)
    length_ = 0;

  if (t != QVariant::Int && t != QVariant::UInt && t != QVariant::Double && pI != 0)
    partInteger_ = 0;

  if (t != QVariant::Double && pD != 0)
    partDecimal_ = 0;
}

FLFieldMetaDataPrivate::~FLFieldMetaDataPrivate()
{
  if (relationList_) {
    relationList_->clear();
    delete relationList_;
  }

  if (relationM1_)
    delete relationM1_;

  optionsList_.clear();
}

FLFieldMetaData::FLFieldMetaData(const QString &n, const QString &a, bool aN, bool iPK, int t,
                                 int l, bool c, bool v, bool ed, int pI, int pD, bool iNX,
                                 bool uNI, bool coun, const QVariant &defValue, bool oT,
                                 const QString &rX, bool vG, bool gen, bool iCK)
{

  d = new FLFieldMetaDataPrivate(n, a, aN, iPK, t, l, c, v, ed, pI, pD, iNX, uNI, coun, defValue,
                                 oT, rX, vG, gen, iCK);
}

FLFieldMetaData::~FLFieldMetaData()
{
  delete d;
}

void FLFieldMetaData::setName(const QString &n)
{
  d->name_ = n;
}

void FLFieldMetaData::addRelationMD(FLRelationMetaData *r)
{
  if (r->cardinality() == FLRelationMetaData::RELATION_M1 && d->relationM1_) {
#ifdef FL_DEBUG
    qWarning
    ("FLFieldMetaData: Se ha intentado crear más de una relación muchos a uno para el mismo campo");
#endif
    return;
  }

  r->setField(d->name_);
  if (r->cardinality() == FLRelationMetaData::RELATION_M1) {
    if (d->relationM1_)
      delete d->relationM1_;
    d->relationM1_ = r;
    return;
  }

  if (!d->relationList_)
    d->relationList_ = new FLRelationMetaDataList;

  d->relationList_->setAutoDelete(true);
  d->relationList_->append(r);
}

void FLFieldMetaData::setRelationList(FLRelationMetaDataList *rl)
{
  if (!rl)
    return;

  rl->setAutoDelete(true);

  if (rl->isEmpty())
    return;

  if (d->relationList_) {
    d->relationList_->clear();
    delete d->relationList_;
  }

  if (d->relationM1_) {
    delete d->relationM1_;
    d->relationM1_ = 0;
  }

  FLRelationMetaData *relation;

  for (relation = rl->first(); relation; relation = rl->next()) {
    if (relation->cardinality() == FLRelationMetaData::RELATION_M1 && d->relationM1_) {
#ifdef FL_DEBUG
      qWarning("FLFieldMetaData: Se ha intentado crear más de una relación muchos a uno para el mismo campo");
#endif

      continue;
    }

    relation->setField(d->name_);
    if (relation->cardinality() == FLRelationMetaData::RELATION_M1) {
      d->relationM1_ = new FLRelationMetaData(relation->foreignTable(), relation->foreignField(),
                                              relation->cardinality(), relation->deleteCascade(),
                                              relation->updateCascade(), relation->checkIn());
      d->relationM1_->setField(relation->field());
      rl->remove(relation);
      break;
    }
  }

  d->relationList_ = rl;
}

void FLFieldMetaData::setOptionsList(const QString &ol)
{
  d->optionsList_.clear();
  QString olTranslated = ol;
  if (ol.contains("QT_TRANSLATE_NOOP")) {
    QStringList components = QStringList::split(';', olTranslated);
    QString component;

    olTranslated = "";
    for (int i = 0; i < components.count(); i++) {
      component = components[i];
      component = component.mid(30, component.length() - 32);
      if (i > 0)
        olTranslated += ",";
      olTranslated += component;
    }
  }
  d->optionsList_ = QStringList::split(',', olTranslated);
  d->hasOptionsList_ = (!d->optionsList_.empty());
}

QVariant::Type FLFieldMetaData::flDecodeType(int fltype)
{
  QVariant::Type type;
  switch (fltype) {
    case QVariant::Int:
      type = QVariant::Int;
      break;
    case FLFieldMetaData::Serial:
    case QVariant::UInt:
      type = QVariant::UInt;
      break;
    case QVariant::Bool:
    case FLFieldMetaData::Unlock:
      type = QVariant::Bool;
      break;
    case QVariant::Double:
      type = QVariant::Double;
      break;
    case QVariant::Time:
      type = QVariant::Time;
      break;
    case QVariant::Date:
      type = QVariant::Date;
      break;
    case QVariant::String:
    case QVariant::Pixmap:
    case QVariant::StringList:
      type = QVariant::String;
      break;
    case QVariant::ByteArray:
      type = QVariant::ByteArray;
      break;
  }
  return type;
}
