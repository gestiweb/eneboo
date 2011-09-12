/***************************************************************************
 AQSDomImplementation_p.h
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

#ifndef AQSDOMIMPLEMENTATION_P_H_
#define AQSDOMIMPLEMENTATION_P_H_

#include "AQSDomDocumentType_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomImplementation : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomImplementation, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual bool hasFeature(const QString &, const QString &);
  virtual QDomDocumentType *createDocumentType(const QString &, const QString &, const QString &);
  virtual QDomDocument *createDocument(const QString &, const QString &, QDomDocumentType *);
  virtual QDomDocument *createDocument(const QString &, const QString &, AQSDomDocumentType *);
  bool isNull();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomImplementation*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomImplementation;
    if (sgt == QString::fromLatin1("QDomImplementation*"))
      return new QDomImplementation(*argValue<QDomImplementation *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomImplementation*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSDomImplementation::hasFeature(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(hasFeature(arg0, arg1), bool);
}
inline QDomDocumentType *AQSDomImplementation::createDocumentType(const QString &arg0,  const QString &arg1,  const QString &arg2)
{
  AQ_CALL_RET_PTR(createDocumentType(arg0, arg1, arg2), QDomDocumentType);
}
inline QDomDocument *AQSDomImplementation::createDocument(const QString &arg0,  const QString &arg1,  QDomDocumentType *arg2)
{
  AQ_CALL_RET_PTR(createDocument(arg0, arg1, *arg2), QDomDocument);
}
inline QDomDocument *AQSDomImplementation::createDocument(const QString &arg0,  const QString &arg1, AQSDomDocumentType *arg2)
{
  AQ_CALL_RET_PTR(createDocument(arg0, arg1, *arg2), QDomDocument);
}
inline bool AQSDomImplementation::isNull()
{
  AQ_CALL_RET_V(isNull(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMIMPLEMENTATION_P_H_ */
// @AQSVOIDPTR@
