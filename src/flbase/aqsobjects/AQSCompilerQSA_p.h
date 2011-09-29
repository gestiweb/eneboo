/***************************************************************************
 AQSCompilerQSA_p.h
 -------------------
 begin                : 18/03/2011
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

#ifndef AQSCOMPILERQSA_P_H_
#define AQSCOMPILERQSA_P_H_

#include "AQSByteArray_p.h"
#include "AQObjects.h"

class AQSCompilerQSA : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(CompilerQSA, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

public slots:
  QValueList<QVariant> errorLines() const {
    QVariantList ret;
    if (!o_)
      return ret;
    QValueList<int> l(o_->errorLines());
    for (QValueList<int>::const_iterator it = l.begin(); it != l.end(); ++it)
      ret << *it;
    return ret;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QByteArray *byteCode(const QString &, int = 0, int = 0);
  int errorType() const;
  QStringList errorMessages() const;
  QString code(QByteArray *);
  QString code(AQSByteArray *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQCompilerQSA;
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
inline QByteArray *AQSCompilerQSA::byteCode(const QString &arg0,  int arg1,  int arg2)
{
  AQ_CALL_RET_PTR(byteCode(arg0, arg1, arg2), QByteArray);
}
inline int AQSCompilerQSA::errorType() const
{
  AQ_CALL_RET_V(errorType(), int);
}
inline QStringList AQSCompilerQSA::errorMessages() const
{
  AQ_CALL_RET_V(errorMessages(), QStringList);
}
inline QString AQSCompilerQSA::code(QByteArray *arg0)
{
  AQ_CALL_RET_V(code(*arg0), QString);
}
inline QString AQSCompilerQSA::code(AQSByteArray *arg0)
{
  AQ_CALL_RET_V(code(*arg0), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCOMPILERQSA_P_H_ */
// @AQOBJECT_VOID@
