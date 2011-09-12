/***************************************************************************
 AQSDomAttr_p.h
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

#ifndef AQSDOMATTR_P_H_
#define AQSDOMATTR_P_H_

#include "AQSDomNode_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomAttr : public AQSDomNode
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomAttr, DomNode);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual QString name() const;
  virtual bool specified() const;
  virtual QDomElement *ownerElement() const;
  virtual QString value() const;
  virtual void setValue(const QString &);
  virtual uint nodeType() const;
  virtual bool isAttr() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomAttr*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomAttr;
    if (sgt == QString::fromLatin1("QDomAttr*"))
      return new QDomAttr(*argValue<QDomAttr *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomAttr*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSDomAttr::name() const
{
  AQ_CALL_RET_V(name(), QString);
}
inline bool AQSDomAttr::specified() const
{
  AQ_CALL_RET_V(specified(), bool);
}
inline QDomElement *AQSDomAttr::ownerElement() const
{
  AQ_CALL_RET_PTR(ownerElement(), QDomElement);
}
inline QString AQSDomAttr::value() const
{
  AQ_CALL_RET_V(value(), QString);
}
inline void AQSDomAttr::setValue(const QString &arg0)
{
  AQ_CALL_VOID(setValue(arg0));
}
inline uint AQSDomAttr::nodeType() const
{
  AQ_CALL_RET_V(nodeType(), uint);
}
inline bool AQSDomAttr::isAttr() const
{
  AQ_CALL_RET_V(isAttr(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMATTR_P_H_ */
// @AQSVOIDPTR@
