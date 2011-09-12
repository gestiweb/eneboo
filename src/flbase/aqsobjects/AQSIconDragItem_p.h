/***************************************************************************
 AQSIconDragItem_p.h
 -------------------
 begin                : 22/02/2011
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

#ifndef AQSICONDRAGITEM_P_H_
#define AQSICONDRAGITEM_P_H_

#include "AQSVoidPtr_p.h"
#include "AQSByteArray_p.h"
#include <qiconview.h>

// @AQ_PTR_INDIRECTION@

class AQSIconDragItem : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(IconDragItem, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual QByteArray *data() const;
  virtual void setData(QByteArray *);
  virtual void setData(AQSByteArray *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QIconDragItem;
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QByteArray *AQSIconDragItem::data() const
{
  AQ_CALL_RET_PTR(data(), QByteArray);
}
inline void AQSIconDragItem::setData(QByteArray *arg0)
{
  AQ_CALL_VOID(setData(*arg0));
}
inline void AQSIconDragItem::setData(AQSByteArray *arg0)
{
  AQ_CALL_VOID(setData(*arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSICONDRAGITEM_P_H_ */
// @AQSVOIDPTR@
