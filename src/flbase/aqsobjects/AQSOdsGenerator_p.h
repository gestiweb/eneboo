/***************************************************************************
 AQSOdsGenerator_p.h
 -------------------
 begin                : 16/12/2011
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

#ifndef AQSODSGENERATOR_P_H_
#define AQSODSGENERATOR_P_H_

#include "AQSVoidPtr_p.h"
#include "AQObjects.h"

// @AQ_PTR_INDIRECTION@

class AQSOdsGenerator : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(OdsGenerator, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void clear();
  QString str() const;
  bool generateOds(const QString &) const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQOdsGenerator;
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
inline void AQSOdsGenerator::clear()
{
  AQ_CALL_VOID(clear());
}
inline QString AQSOdsGenerator::str() const
{
  AQ_CALL_RET_V(str(), QString);
}
inline bool AQSOdsGenerator::generateOds(const QString &arg0) const
{
  AQ_CALL_RET_V(generateOds(arg0), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSODSGENERATOR_P_H_ */
// @AQOBJECT_VOID@
