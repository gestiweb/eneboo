/***************************************************************************
 AQSDomProcessingInstruction_p.h
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

#ifndef AQSDOMPROCESSINGINSTRUCTION_P_H_
#define AQSDOMPROCESSINGINSTRUCTION_P_H_

#include "AQSDomNode_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomProcessingInstruction : public AQSDomNode
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomProcessingInstruction, DomNode);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual QString target() const;
  virtual QString data() const;
  virtual void setData(const QString &);
  virtual uint nodeType() const;
  virtual bool isProcessingInstruction() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomProcessingInstruction*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomProcessingInstruction;
    if (sgt == QString::fromLatin1("QDomProcessingInstruction*"))
      return new QDomProcessingInstruction(*argValue<QDomProcessingInstruction *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomProcessingInstruction*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSDomProcessingInstruction::target() const
{
  AQ_CALL_RET_V(target(), QString);
}
inline QString AQSDomProcessingInstruction::data() const
{
  AQ_CALL_RET_V(data(), QString);
}
inline void AQSDomProcessingInstruction::setData(const QString &arg0)
{
  AQ_CALL_VOID(setData(arg0));
}
inline uint AQSDomProcessingInstruction::nodeType() const
{
  AQ_CALL_RET_V(nodeType(), uint);
}
inline bool AQSDomProcessingInstruction::isProcessingInstruction() const
{
  AQ_CALL_RET_V(isProcessingInstruction(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMPROCESSINGINSTRUCTION_P_H_ */
// @AQSVOIDPTR@
