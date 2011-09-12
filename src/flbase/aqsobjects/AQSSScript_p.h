/***************************************************************************
 AQSSScript_p.h
 -------------------
 begin                : 07/02/2011
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

#ifndef AQSSSCRIPT_P_H_
#define AQSSSCRIPT_P_H_

#include "AQSObject_p.h"
#include <qsscript.h>

class AQSSScript : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(SScript, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString name() const;
  QString code() const;
  QObject *context() const;
  void setCode(const QString &);
  void addCode(const QString &);
  bool addFunction(const QString &, const QString& = QString::null);
  QString fileName() const;
  void setFileName(const QString &);
  QString baseFileName() const;
  void setBaseFileName(const QString &);

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSSScript::name() const
{
  AQ_CALL_RET_V(name(), QString);
}
inline QString AQSSScript::code() const
{
  AQ_CALL_RET_V(code(), QString);
}
inline QObject *AQSSScript::context() const
{
  AQ_CALL_RET(context());
}
inline void AQSSScript::setCode(const QString &arg0)
{
  AQ_CALL_VOID(setCode(arg0));
}
inline void AQSSScript::addCode(const QString &arg0)
{
  AQ_CALL_VOID(addCode(arg0));
}
inline bool AQSSScript::addFunction(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(addFunction(arg0, arg1), bool);
}
inline QString AQSSScript::fileName() const
{
  AQ_CALL_RET_V(fileName(), QString);
}
inline void AQSSScript::setFileName(const QString &arg0)
{
  AQ_CALL_VOID(setFileName(arg0));
}
inline QString AQSSScript::baseFileName() const
{
  AQ_CALL_RET_V(baseFileName(), QString);
}
inline void AQSSScript::setBaseFileName(const QString &arg0)
{
  AQ_CALL_VOID(setBaseFileName(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSSCRIPT_P_H_ */
// @AQSOBJECT@
