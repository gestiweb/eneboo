/***************************************************************************
 AQSSqlDatabase_p.h
 -------------------
 begin                : 29/03/2011
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

#ifndef AQSSQLDATABASE_P_H_
#define AQSSQLDATABASE_P_H_

#include "AQSVoidPtr_p.h"
#include "AQObjects.h"

class AQSSqlDatabase : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(SqlDatabase, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool createTable(FLTableMetaData *);
  bool canRegenTables();
  QString formatValueLike(int, const QVariant &, bool = false);
  QString formatValue(int, const QVariant &, bool = false);
  QVariant nextSerialVal(const QString &, const QString &);
  int atFrom(FLSqlCursor *);
  QString database() const;
  QString user() const;
  QString password() const;
  QString host() const;
  int port() const;
  QString driverName() const;
  bool alterTable(const QString &, const QString &, const QString& = QString::null);
  FLManager *manager();
  FLManagerModules *managerModules();
  QString connectionName() const;
  bool canSavePoint();
  bool savePoint(const QString &);
  bool releaseSavePoint(const QString &);
  bool rollbackSavePoint(const QString &);
  bool canTransaction() const;
  void Mr_Proper();
  bool canDetectLocks() const;
  QStringList locksStatus();
  QStringList detectLocks();
  QStringList detectRisksLocks(const QString& = QString::null, const QString& = QString::null);
  bool regenTable(const QString &, FLTableMetaData *);
  QStringList driverAliases();
  QString defaultAlias();
  QString driverAliasToDriverName(const QString &);
  QString driverNameToDriverAlias(const QString &);
  bool needConnOption(const QString &, int);
  QString defaultPort(const QString &);

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSSqlDatabase::createTable(FLTableMetaData *arg0)
{
  AQ_CALL_RET_V(createTable(arg0), bool);
}
inline bool AQSSqlDatabase::canRegenTables()
{
  AQ_CALL_RET_V(canRegenTables(), bool);
}
inline QString AQSSqlDatabase::formatValueLike(int arg0,  const QVariant &arg1,  bool arg2)
{
  AQ_CALL_RET_V(formatValueLike(arg0, arg1, arg2), QString);
}
inline QString AQSSqlDatabase::formatValue(int arg0,  const QVariant &arg1,  bool arg2)
{
  AQ_CALL_RET_V(formatValue(arg0, arg1, arg2), QString);
}
inline QVariant AQSSqlDatabase::nextSerialVal(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(nextSerialVal(arg0, arg1), QVariant);
}
inline int AQSSqlDatabase::atFrom(FLSqlCursor *arg0)
{
  AQ_CALL_RET_V(atFrom(arg0), int);
}
inline QString AQSSqlDatabase::database() const
{
  AQ_CALL_RET_V(database(), QString);
}
inline QString AQSSqlDatabase::user() const
{
  AQ_CALL_RET_V(user(), QString);
}
inline QString AQSSqlDatabase::password() const
{
  AQ_CALL_RET_V(password(), QString);
}
inline QString AQSSqlDatabase::host() const
{
  AQ_CALL_RET_V(host(), QString);
}
inline int AQSSqlDatabase::port() const
{
  AQ_CALL_RET_V(port(), int);
}
inline QString AQSSqlDatabase::driverName() const
{
  AQ_CALL_RET_V(driverName(), QString);
}
inline bool AQSSqlDatabase::alterTable(const QString &arg0,  const QString &arg1,  const QString &arg2)
{
  AQ_CALL_RET_V(alterTable(arg0, arg1, arg2), bool);
}
inline FLManager *AQSSqlDatabase::manager()
{
  AQ_CALL_RET(manager());
}
inline FLManagerModules *AQSSqlDatabase::managerModules()
{
  AQ_CALL_RET(managerModules());
}
inline QString AQSSqlDatabase::connectionName() const
{
  AQ_CALL_RET_V(connectionName(), QString);
}
inline bool AQSSqlDatabase::canSavePoint()
{
  AQ_CALL_RET_V(canSavePoint(), bool);
}
inline bool AQSSqlDatabase::savePoint(const QString &arg0)
{
  AQ_CALL_RET_V(savePoint(arg0), bool);
}
inline bool AQSSqlDatabase::releaseSavePoint(const QString &arg0)
{
  AQ_CALL_RET_V(releaseSavePoint(arg0), bool);
}
inline bool AQSSqlDatabase::rollbackSavePoint(const QString &arg0)
{
  AQ_CALL_RET_V(rollbackSavePoint(arg0), bool);
}
inline bool AQSSqlDatabase::canTransaction() const
{
  AQ_CALL_RET_V(canTransaction(), bool);
}
inline void AQSSqlDatabase::Mr_Proper()
{
  AQ_CALL_VOID(Mr_Proper());
}
inline bool AQSSqlDatabase::canDetectLocks() const
{
  AQ_CALL_RET_V(canDetectLocks(), bool);
}
inline QStringList AQSSqlDatabase::locksStatus()
{
  AQ_CALL_RET_V(locksStatus(), QStringList);
}
inline QStringList AQSSqlDatabase::detectLocks()
{
  AQ_CALL_RET_V(detectLocks(), QStringList);
}
inline QStringList AQSSqlDatabase::detectRisksLocks(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(detectRisksLocks(arg0, arg1), QStringList);
}
inline bool AQSSqlDatabase::regenTable(const QString &arg0,  FLTableMetaData *arg1)
{
  AQ_CALL_RET_V(regenTable(arg0, arg1), bool);
}
inline QStringList AQSSqlDatabase::driverAliases()
{
  AQ_CALL_RET_V(driverAliases(), QStringList);
}
inline QString AQSSqlDatabase::defaultAlias()
{
  AQ_CALL_RET_V(defaultAlias(), QString);
}
inline QString AQSSqlDatabase::driverAliasToDriverName(const QString &arg0)
{
  AQ_CALL_RET_V(driverAliasToDriverName(arg0), QString);
}
inline QString AQSSqlDatabase::driverNameToDriverAlias(const QString &arg0)
{
  AQ_CALL_RET_V(driverNameToDriverAlias(arg0), QString);
}
inline bool AQSSqlDatabase::needConnOption(const QString &arg0,  int arg1)
{
  AQ_CALL_RET_V(needConnOption(arg0, arg1), bool);
}
inline QString AQSSqlDatabase::defaultPort(const QString &arg0)
{
  AQ_CALL_RET_V(defaultPort(arg0), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSQLDATABASE_P_H_ */
// @AQOBJECT_VOID@
