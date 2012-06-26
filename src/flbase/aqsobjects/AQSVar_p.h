/***************************************************************************
 AQSVar_p.h
 -------------------
 begin                : 02/11/2011
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

#ifndef AQSVAR_P_H_
#define AQSVAR_P_H_

#include "AQSVoidPtr_p.h"
#include "AQObjects.h"

class AQSVar : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(Var, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool set(const QString &, const QVariant &);
  QVariant get(const QString &);
  bool del(const QString &);
  bool clean();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQVar;
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
inline bool AQSVar::set(const QString &arg0,  const QVariant &arg1)
{
  AQ_CALL_RET_V(set(arg0, arg1), bool);
}
inline QVariant AQSVar::get(const QString &arg0)
{
  AQ_CALL_RET_V(get(arg0), QVariant);
}
inline bool AQSVar::del(const QString &arg0)
{
  AQ_CALL_RET_V(del(arg0), bool);
}
inline bool AQSVar::clean()
{
  AQ_CALL_RET_V(clean(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSVAR_P_H_ */
// @AQOBJECT_VOID@
