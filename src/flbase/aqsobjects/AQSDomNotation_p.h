/***************************************************************************
 AQSDomNotation_p.h
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

#ifndef AQSDOMNOTATION_P_H_
#define AQSDOMNOTATION_P_H_

#include "AQSDomNode_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomNotation : public AQSDomNode
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomNotation, DomNode);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString publicId() const;
  QString systemId() const;
  virtual uint nodeType() const;
  virtual bool isNotation() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomNotation*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomNotation;
    if (sgt == QString::fromLatin1("QDomNotation*"))
      return new QDomNotation(*argValue<QDomNotation *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomNotation*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSDomNotation::publicId() const
{
  AQ_CALL_RET_V(publicId(), QString);
}
inline QString AQSDomNotation::systemId() const
{
  AQ_CALL_RET_V(systemId(), QString);
}
inline uint AQSDomNotation::nodeType() const
{
  AQ_CALL_RET_V(nodeType(), uint);
}
inline bool AQSDomNotation::isNotation() const
{
  AQ_CALL_RET_V(isNotation(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMNOTATION_P_H_ */
// @AQSVOIDPTR@
