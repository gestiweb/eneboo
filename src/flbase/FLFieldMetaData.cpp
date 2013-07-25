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

#ifdef FL_DEBUG
AQ_EXPORT long FLFieldMetaData::count_ = 0;
#endif

FLFieldMetaDataPrivate::FLFieldMetaDataPrivate(const QString &n, const QString &a, bool aN,
                                               bool iPK, int t, int l, bool c, bool v, bool ed,
                                               int pI, int pD, bool iNX, bool uNI, bool coun,
                                               const QVariant &defValue, bool oT,
                                               const QString &rX, bool vG, bool gen, bool iCK) :
  fieldName_(n.lower()), alias_(a), allowNull_((!c) ? aN : true), isPrimaryKey_(iPK), type_(t),
  length_(l), calculated_(c), visible_(v), editable_(ed), partInteger_(pI), partDecimal_(pD),
  isIndex_(iNX), isUnique_(uNI), contador_(coun), relationList_(0), relationM1_(0),
  associatedField_(0), defaultValue_(defValue), outTransaction_(oT), regExpValidator_(rX),
  visibleGrid_(vG), generated_(gen), isCompoundKey_(iCK), hasOptionsList_(false), mtd_(0),
  fullyCalculated_(false), trimmed_(false)
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

FLFieldMetaDataPrivate::FLFieldMetaDataPrivate()
  : relationList_(0), relationM1_(0),
    associatedField_(0), mtd_(0)
{
}

FLFieldMetaDataPrivate::~FLFieldMetaDataPrivate()
{
  clearRelationList();
  if (relationM1_ && relationM1_->deref()) {
    delete relationM1_;
    relationM1_ = 0;
  }
}

void FLFieldMetaDataPrivate::clearRelationList()
{
  if (!relationList_)
    return;
  FLRelationMetaData *r;
  QPtrListIterator<FLRelationMetaData> it(*relationList_);
  while ((r = it.current()) != 0) {
    ++it;
    relationList_->remove(r);
    if (r->deref())
      delete r;
  }
  delete relationList_;
  relationList_ = 0;
}

FLFieldMetaData::FLFieldMetaData(const QString &n, const QString &a, bool aN, bool iPK, int t,
                                 int l, bool c, bool v, bool ed, int pI, int pD, bool iNX,
                                 bool uNI, bool coun, const QVariant &defValue, bool oT,
                                 const QString &rX, bool vG, bool gen, bool iCK) : QShared()
{
#ifdef FL_DEBUG
  ++count_;
#endif
  d = new FLFieldMetaDataPrivate(n, a, aN, iPK, t, l, c, v, ed, pI, pD, iNX, uNI, coun, defValue,
                                 oT, rX, vG, gen, iCK);
}

FLFieldMetaData::FLFieldMetaData(const FLFieldMetaData *other)
{
#ifdef FL_DEBUG
  ++count_;
#endif
  d =  new FLFieldMetaDataPrivate;
  copy(other);
}

FLFieldMetaData::~FLFieldMetaData()
{
#ifdef FL_DEBUG
  --count_;
#endif
  delete d;
}

void FLFieldMetaData::setName(const QString &n)
{
  d->fieldName_ = n;
}

void FLFieldMetaData::addRelationMD(FLRelationMetaData *r)
{
  bool isRelM1 = r->cardinality() == FLRelationMetaData::RELATION_M1;
  if (isRelM1 && d->relationM1_) {
#ifdef FL_DEBUG
    qWarning
    ("FLFieldMetaData: Se ha intentado crear más de una relación muchos a uno para el mismo campo");
#endif
    return;
  }
  r->setField(d->fieldName_);
  if (isRelM1) {
    d->relationM1_ = r;
    return;
  }
  if (!d->relationList_)
    d->relationList_ = new FLRelationMetaDataList;
  d->relationList_->append(r);
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

void FLFieldMetaData::copy(const FLFieldMetaData *other)
{
  if (other == this)
    return;
  FLFieldMetaDataPrivate *od = other->d;

  if (od->relationM1_) {
    //d->relationM1_ = new FLRelationMetaData(od->relationM1_);
    od->relationM1_->ref();
    d->relationM1_ = od->relationM1_;
  }

  d->clearRelationList();

  if (od->relationList_) {
    FLRelationMetaData *r;
    QPtrListIterator<FLRelationMetaData> it(*od->relationList_);
    while ((r = it.current()) != 0) {
      ++it;
      if (!d->relationList_)
        d->relationList_ = new FLRelationMetaDataList;
      //d->relationList_->append(new FLRelationMetaData(r));
      r->ref();
      d->relationList_->append(r);
    }
  }

  d->fieldName_ = od->fieldName_;
  d->alias_ = od->alias_;
  d->allowNull_ = od->allowNull_;
  d->isPrimaryKey_ = od->isPrimaryKey_;
  d->type_ = od->type_;
  d->length_ = od->length_;
  d->calculated_ = od->calculated_;
  d->fullyCalculated_ = od->fullyCalculated_;
  d->trimmed_ = od->trimmed_;
  d->visible_ = od->visible_;
  d->editable_ = od->editable_;
  d->partInteger_ = od->partInteger_;
  d->partDecimal_ = od->partDecimal_;
  d->isIndex_ = od->isIndex_;
  d->isUnique_ = od->isUnique_;
  d->contador_ = od->contador_;
  d->associatedField_ = od->associatedField_;
  d->associatedFieldName_ = od->associatedFieldName_;
  d->associatedFieldFilterTo_ = od->associatedFieldFilterTo_;
  d->defaultValue_ = od->defaultValue_;
  d->optionsList_ = od->optionsList_;
  d->outTransaction_ = od->outTransaction_;
  d->regExpValidator_ = od->regExpValidator_;
  d->visibleGrid_ = od->visibleGrid_;
  d->generated_ = od->generated_;
  d->isCompoundKey_ = od->isCompoundKey_;
  d->hasOptionsList_ = od->hasOptionsList_;
}
