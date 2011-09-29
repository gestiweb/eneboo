/***************************************************************************
 AQSDomDocumentType_p.h
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

#ifndef AQSDOMDOCUMENTTYPE_P_H_
#define AQSDOMDOCUMENTTYPE_P_H_

#include "AQSDomNode_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomDocumentType : public AQSDomNode
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomDocumentType, DomNode);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual QString name() const;
  virtual QDomNamedNodeMap *entities() const;
  virtual QDomNamedNodeMap *notations() const;
  virtual QString publicId() const;
  virtual QString systemId() const;
  virtual QString internalSubset() const;
  virtual uint nodeType() const;
  virtual bool isDocumentType() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomDocumentType*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomDocumentType;
    if (sgt == QString::fromLatin1("QDomDocumentType*"))
      return new QDomDocumentType(*argValue<QDomDocumentType *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomDocumentType*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSDomDocumentType::name() const
{
  AQ_CALL_RET_V(name(), QString);
}
inline QDomNamedNodeMap *AQSDomDocumentType::entities() const
{
  AQ_CALL_RET_PTR(entities(), QDomNamedNodeMap);
}
inline QDomNamedNodeMap *AQSDomDocumentType::notations() const
{
  AQ_CALL_RET_PTR(notations(), QDomNamedNodeMap);
}
inline QString AQSDomDocumentType::publicId() const
{
  AQ_CALL_RET_V(publicId(), QString);
}
inline QString AQSDomDocumentType::systemId() const
{
  AQ_CALL_RET_V(systemId(), QString);
}
inline QString AQSDomDocumentType::internalSubset() const
{
  AQ_CALL_RET_V(internalSubset(), QString);
}
inline uint AQSDomDocumentType::nodeType() const
{
  AQ_CALL_RET_V(nodeType(), uint);
}
inline bool AQSDomDocumentType::isDocumentType() const
{
  AQ_CALL_RET_V(isDocumentType(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMDOCUMENTTYPE_P_H_ */
// @AQSVOIDPTR@
