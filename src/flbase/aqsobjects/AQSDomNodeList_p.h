/***************************************************************************
 AQSDomNodeList_p.h
 -------------------
 begin                : 15/02/2011
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

#ifndef AQSDOMNODELIST_P_H_
#define AQSDOMNODELIST_P_H_

#include "AQSDomNode_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomNodeList : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomNodeList, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual QDomNode *item(int) const;
  virtual uint length() const;
  uint count() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomNodeList*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomNodeList;
    if (sgt == QString::fromLatin1("QDomNodeList*"))
      return new QDomNodeList(*argValue<QDomNodeList *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomNodeList*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QDomNode *AQSDomNodeList::item(int arg0) const
{
  AQ_CALL_RET_PTR(item(arg0), QDomNode);
}
inline uint AQSDomNodeList::length() const
{
  AQ_CALL_RET_V(length(), uint);
}
inline uint AQSDomNodeList::count() const
{
  AQ_CALL_RET_V(count(), uint);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMNODELIST_P_H_ */
// @AQSVOIDPTR@
