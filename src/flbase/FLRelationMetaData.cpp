/***************************************************************************
                        FLRelationMetaData.cpp
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

#include "FLRelationMetaData.h"

#ifdef FL_DEBUG
AQ_EXPORT long FLRelationMetaData::count_ = 0;
#endif

FLRelationMetaDataPrivate::FLRelationMetaDataPrivate(const QString &fT, const QString &fF, int rC,
                                                     bool dC, bool uC, bool cI)
  : foreignTable_(fT.lower()), foreignField_(fF.lower()), cardinality_(rC),
    deleteCascade_(dC), updateCascade_(uC), checkIn_(cI)
{
}

FLRelationMetaData::FLRelationMetaData(const QString &fT, const QString &fF, int rC, bool dC,
                                       bool uC, bool cI) : QShared()
{
#ifdef FL_DEBUG
  ++count_;
#endif
  d = new FLRelationMetaDataPrivate(fT, fF, rC, dC, uC, cI);
}

FLRelationMetaData::FLRelationMetaData(const FLRelationMetaData *other)
{
#ifdef FL_DEBUG
  ++count_;
#endif
  d = new FLRelationMetaDataPrivate;
  copy(other);
}

FLRelationMetaData::~FLRelationMetaData()
{
#ifdef FL_DEBUG
  --count_;
#endif
  delete d;
}

void FLRelationMetaData::copy(const FLRelationMetaData *other)
{
  if (other == this)
    return;
  d->field_ = other->d->field_;
  d->foreignTable_ = other->d->foreignTable_;
  d->foreignField_ = other->d->foreignField_;
  d->cardinality_ = other->d->cardinality_;
  d->deleteCascade_ = other->d->deleteCascade_;
  d->updateCascade_ = other->d->updateCascade_;
  d->checkIn_ = other->d->checkIn_;
}
