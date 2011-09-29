/***************************************************************************
 AQSAbstractMemArray_p.h
 -------------------
 begin                : 27/02/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
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

#ifndef AQSABSTRACTMEMARRAY_P_H_
#define AQSABSTRACTMEMARRAY_P_H_

#include "AQSVoidPtr_p.h"

// @AQ_PTR_INDIRECTION@

class AQSAbstractMemArray : public AQSVoidPtr
{
  Q_OBJECT

  Q_PROPERTY(uint size READ size WRITE resize)
  Q_PROPERTY(uint length READ size WRITE resize)
  Q_PROPERTY(QValueList data READ data WRITE setData)

  AQ_SKIP_DECLARE(AbstractMemArray, VoidPtr);

public:
  virtual void setter(uint index, const QVariant &v) = 0;
  virtual QVariant getter(uint index) const = 0;
  virtual uint size() const = 0;
  virtual bool resize(uint size) = 0;

  QValueList<QVariant> data() const {
    QValueList<QVariant> dt;
    if (!isValid())
      return dt;
    for (uint i = 0; i < this->size(); ++i)
      dt.append(this->getter(i));
    return dt;
  }

public slots:
  void set(uint index, const QVariant &v) {
    this->setter(index, v);
  }

  QVariant get(uint index) const {
    return getter(index);
  }

  void setData(const QValueList<QVariant> &dt) {
    if (!isValid())
      return;
    this->resize(dt.size());
    uint idx = 0;
    QValueList<QVariant>::const_iterator it;
    for (it = dt.begin(); it != dt.end(); ++it, ++idx)
      this->setter(idx, *it);
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSABSTRACTMEMARRAY_P_H_ */
// @AQSVOIDPTR@
