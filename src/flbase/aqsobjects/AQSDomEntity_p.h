/***************************************************************************
 AQSDomEntity_p.h
 -------------------
 begin                : 16/02/2011
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

#ifndef AQSDOMENTITY_P_H_
#define AQSDOMENTITY_P_H_

#include "AQSDomNode_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomEntity : public AQSDomNode
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomEntity, DomNode);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual QString publicId() const;
  virtual QString systemId() const;
  virtual QString notationName() const;
  virtual uint nodeType() const;
  virtual bool isEntity() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomEntity*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomEntity;
    if (sgt == QString::fromLatin1("QDomEntity*"))
      return new QDomEntity(*argValue<QDomEntity *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomEntity*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSDomEntity::publicId() const
{
  AQ_CALL_RET_V(publicId(), QString);
}
inline QString AQSDomEntity::systemId() const
{
  AQ_CALL_RET_V(systemId(), QString);
}
inline QString AQSDomEntity::notationName() const
{
  AQ_CALL_RET_V(notationName(), QString);
}
inline uint AQSDomEntity::nodeType() const
{
  AQ_CALL_RET_V(nodeType(), uint);
}
inline bool AQSDomEntity::isEntity() const
{
  AQ_CALL_RET_V(isEntity(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMENTITY_P_H_ */
// @AQSVOIDPTR@
