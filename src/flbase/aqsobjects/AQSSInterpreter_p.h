/***************************************************************************
 AQSSInterpreter_p.h
 -------------------
 begin                : 30/12/2011
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

#ifndef AQSSINTERPRETER_P_H_
#define AQSSINTERPRETER_P_H_

#include "AQSObject_p.h"
#include <qsinterpreter.h>

class AQSSInterpreter : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(SInterpreter, Object);

public slots:
  QObject *self() const {
    return o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool checkSyntax(const QString &);
  QStringList functions(uint = QSInterpreter::FunctionNames) const;
  QStringList functions(const QString &, uint = QSInterpreter::FunctionNames) const;
  QStringList functions(QObject *, uint = QSInterpreter::FunctionNames) const;
  QStringList classes(uint = QSInterpreter::AllClasses) const;
  QStringList classes(const QString &) const;
  QStringList classes(QObject *) const;
  QStringList variables(bool = false, bool = false, bool = false) const;
  QStringList variables(const QString &, bool = false, bool = false, bool = false) const;
  QStringList variables(QObject *, bool = false, bool = false, bool = false) const;
  bool hasFunction(const QString &) const;
  bool hasClass(const QString &) const;
  bool hasVariable(const QString &) const;
  QObjectList presentObjects() const;
  bool isRunning() const;
  bool hadError() const;
  bool interactiveGUI() const;
  void setInteractiveGUI(bool = true);

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSSInterpreter::checkSyntax(const QString &arg0)
{
  AQ_CALL_RET_V(checkSyntax(arg0), bool);
}
inline QStringList AQSSInterpreter::functions(uint arg0) const
{
  AQ_CALL_RET_V(functions(static_cast<QSInterpreter::FunctionFlags>(arg0)), QStringList);
}
inline QStringList AQSSInterpreter::functions(const QString &arg0,  uint arg1) const
{
  AQ_CALL_RET_V(functions(arg0, arg1), QStringList);
}
inline QStringList AQSSInterpreter::functions(QObject *arg0,  uint arg1) const
{
  AQ_CALL_RET_V(functions(arg0, static_cast<QSInterpreter::FunctionFlags>(arg1)), QStringList);
}
inline QStringList AQSSInterpreter::classes(uint arg0) const
{
  AQ_CALL_RET_V(classes(static_cast<QSInterpreter::ClassFlags>(arg0)), QStringList);
}
inline QStringList AQSSInterpreter::classes(const QString &arg0) const
{
  AQ_CALL_RET_V(classes(arg0), QStringList);
}
inline QStringList AQSSInterpreter::classes(QObject *arg0) const
{
  AQ_CALL_RET_V(classes(arg0), QStringList);
}
inline QStringList AQSSInterpreter::variables(bool arg0,  bool arg1,  bool arg2) const
{
  AQ_CALL_RET_V(variables(arg0, arg1, arg2), QStringList);
}
inline QStringList AQSSInterpreter::variables(const QString &arg0,  bool arg1,  bool arg2,  bool arg3) const
{
  AQ_CALL_RET_V(variables(arg0, arg1, arg2, arg3), QStringList);
}
inline QStringList AQSSInterpreter::variables(QObject *arg0,  bool arg1,  bool arg2,  bool arg3) const
{
  AQ_CALL_RET_V(variables(arg0, arg1, arg2, arg3), QStringList);
}
inline bool AQSSInterpreter::hasFunction(const QString &arg0) const
{
  AQ_CALL_RET_V(hasFunction(arg0), bool);
}
inline bool AQSSInterpreter::hasClass(const QString &arg0) const
{
  AQ_CALL_RET_V(hasClass(arg0), bool);
}
inline bool AQSSInterpreter::hasVariable(const QString &arg0) const
{
  AQ_CALL_RET_V(hasVariable(arg0), bool);
}
inline QObjectList AQSSInterpreter::presentObjects() const
{
  AQ_CALL_RET_V(presentObjects(), QObjectList);
}
inline bool AQSSInterpreter::isRunning() const
{
  AQ_CALL_RET_V(isRunning(), bool);
}
inline bool AQSSInterpreter::hadError() const
{
  AQ_CALL_RET_V(hadError(), bool);
}
inline bool AQSSInterpreter::interactiveGUI() const
{
  AQ_CALL_RET_V(interactiveGUI(), bool);
}
inline void AQSSInterpreter::setInteractiveGUI(bool arg0)
{
  AQ_CALL_VOID(setInteractiveGUI(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSINTERPRETER_P_H_ */
// @AQSOBJECT@
