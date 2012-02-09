/***************************************************************************
 AQSActionMD_p.h
 -------------------
 begin                : 05/04/2011
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

#ifndef AQSACTIONMD_P_H_
#define AQSACTIONMD_P_H_

#include "AQSVoidPtr_p.h"
#include "AQObjects.h"

class AQSActionMD : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(ActionMD, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setName(const QString &);
  void setScriptFormRecord(const QString &);
  void setScriptForm(const QString &);
  void setTable(const QString &);
  void setForm(const QString &);
  void setFormRecord(const QString &);
  void setCaption(const QString &);
  QString name() const;
  QString scriptFormRecord() const;
  QString scriptForm() const;
  QString table() const;
  QString caption() const;
  QString form() const;
  QString formRecord() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQActionMD;
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
inline void AQSActionMD::setName(const QString &arg0)
{
  AQ_CALL_VOID(setName(arg0));
}
inline void AQSActionMD::setScriptFormRecord(const QString &arg0)
{
  AQ_CALL_VOID(setScriptFormRecord(arg0));
}
inline void AQSActionMD::setScriptForm(const QString &arg0)
{
  AQ_CALL_VOID(setScriptForm(arg0));
}
inline void AQSActionMD::setTable(const QString &arg0)
{
  AQ_CALL_VOID(setTable(arg0));
}
inline void AQSActionMD::setForm(const QString &arg0)
{
  AQ_CALL_VOID(setForm(arg0));
}
inline void AQSActionMD::setFormRecord(const QString &arg0)
{
  AQ_CALL_VOID(setFormRecord(arg0));
}
inline void AQSActionMD::setCaption(const QString &arg0)
{
  AQ_CALL_VOID(setCaption(arg0));
}
inline QString AQSActionMD::name() const
{
  AQ_CALL_RET_V(name(), QString);
}
inline QString AQSActionMD::scriptFormRecord() const
{
  AQ_CALL_RET_V(scriptFormRecord(), QString);
}
inline QString AQSActionMD::scriptForm() const
{
  AQ_CALL_RET_V(scriptForm(), QString);
}
inline QString AQSActionMD::table() const
{
  AQ_CALL_RET_V(table(), QString);
}
inline QString AQSActionMD::caption() const
{
  AQ_CALL_RET_V(caption(), QString);
}
inline QString AQSActionMD::form() const
{
  AQ_CALL_RET_V(form(), QString);
}
inline QString AQSActionMD::formRecord() const
{
  AQ_CALL_RET_V(formRecord(), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSACTIONMD_P_H_ */
// @AQOBJECT_VOID@
