/***************************************************************************
 AQSManager_p.h
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

#ifndef AQSMANAGER_P_H_
#define AQSMANAGER_P_H_

#include "AQSVoidPtr_p.h"
#include "AQObjects.h"

class AQSManager : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(Manager, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  FLTableMetaData *metadata(const QString &, bool = false);
  FLSqlQuery *query(const QString &, QObject* = 0);
  FLAction *action(const QString &);
  bool existsTable(const QString &, bool = true) const;
  bool checkMetaData(const QString &);
  bool checkMetaData(const QString &, const QString &);
  bool checkMetaData(FLTableMetaData *, FLTableMetaData *);
  bool alterTable(const QString &);
  bool alterTable(const QString &, const QString &, const QString& = QString::null);
  FLTableMetaData *createTable(const QString &);
  FLTableMetaData *createTable(FLTableMetaData *);
  QString formatValueLike(FLFieldMetaData *, const QVariant &, bool = false);
  QString formatAssignValueLike(FLFieldMetaData *, const QVariant &, bool = false);
  QString formatAssignValueLike(const QString &, FLFieldMetaData *, const QVariant &, bool = false);
  QString formatAssignValueLike(const QString &, int, const QVariant &, bool = false);
  QString formatValueLike(int, const QVariant &, bool = false);
  QString formatValue(FLFieldMetaData *, const QVariant &, bool = false);
  QString formatAssignValue(FLFieldMetaData *, const QVariant &, bool = false);
  QString formatAssignValue(const QString &, FLFieldMetaData *, const QVariant &, bool = false);
  QString formatAssignValue(const QString &, int, const QVariant &, bool = false);
  QString formatValue(int, const QVariant &, bool = false);
  void loadTables();
  void cleanupMetaData();
  bool isSystemTable(const QString &);
  QString storeLargeValue(FLTableMetaData *, const QString &);
  QVariant fetchLargeValue(const QString &) const;

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline FLTableMetaData *AQSManager::metadata(const QString &arg0,  bool arg1)
{
  AQ_CALL_RET(metadata(arg0, arg1));
}
inline FLSqlQuery *AQSManager::query(const QString &arg0,  QObject *arg1)
{
  AQ_CALL_RET(query(arg0, arg1));
}
inline FLAction *AQSManager::action(const QString &arg0)
{
  AQ_CALL_RET(action(arg0));
}
inline bool AQSManager::existsTable(const QString &arg0,  bool arg1) const
{
  AQ_CALL_RET_V(existsTable(arg0, arg1), bool);
}
inline bool AQSManager::checkMetaData(const QString &arg0)
{
  AQ_CALL_RET_V(checkMetaData(arg0), bool);
}
inline bool AQSManager::checkMetaData(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(checkMetaData(arg0, arg1), bool);
}
inline bool AQSManager::checkMetaData(FLTableMetaData *arg0,  FLTableMetaData *arg1)
{
  AQ_CALL_RET_V(checkMetaData(arg0, arg1), bool);
}
inline bool AQSManager::alterTable(const QString &arg0)
{
  AQ_CALL_RET_V(alterTable(arg0), bool);
}
inline bool AQSManager::alterTable(const QString &arg0,  const QString &arg1,  const QString &arg2)
{
  AQ_CALL_RET_V(alterTable(arg0, arg1, arg2), bool);
}
inline FLTableMetaData *AQSManager::createTable(const QString &arg0)
{
  AQ_CALL_RET(createTable(arg0));
}
inline FLTableMetaData *AQSManager::createTable(FLTableMetaData *arg0)
{
  AQ_CALL_RET(createTable(arg0));
}
inline QString AQSManager::formatValueLike(FLFieldMetaData *arg0,  const QVariant &arg1,  bool arg2)
{
  AQ_CALL_RET_V(formatValueLike(arg0, arg1, arg2), QString);
}
inline QString AQSManager::formatAssignValueLike(FLFieldMetaData *arg0,  const QVariant &arg1,  bool arg2)
{
  AQ_CALL_RET_V(formatAssignValueLike(arg0, arg1, arg2), QString);
}
inline QString AQSManager::formatAssignValueLike(const QString &arg0,  FLFieldMetaData *arg1,  const QVariant &arg2,  bool arg3)
{
  AQ_CALL_RET_V(formatAssignValueLike(arg0, arg1, arg2, arg3), QString);
}
inline QString AQSManager::formatAssignValueLike(const QString &arg0,  int arg1,  const QVariant &arg2,  bool arg3)
{
  AQ_CALL_RET_V(formatAssignValueLike(arg0, arg1, arg2, arg3), QString);
}
inline QString AQSManager::formatValueLike(int arg0,  const QVariant &arg1,  bool arg2)
{
  AQ_CALL_RET_V(formatValueLike(arg0, arg1, arg2), QString);
}
inline QString AQSManager::formatValue(FLFieldMetaData *arg0,  const QVariant &arg1,  bool arg2)
{
  AQ_CALL_RET_V(formatValue(arg0, arg1, arg2), QString);
}
inline QString AQSManager::formatAssignValue(FLFieldMetaData *arg0,  const QVariant &arg1,  bool arg2)
{
  AQ_CALL_RET_V(formatAssignValue(arg0, arg1, arg2), QString);
}
inline QString AQSManager::formatAssignValue(const QString &arg0,  FLFieldMetaData *arg1,  const QVariant &arg2,  bool arg3)
{
  AQ_CALL_RET_V(formatAssignValue(arg0, arg1, arg2, arg3), QString);
}
inline QString AQSManager::formatAssignValue(const QString &arg0,  int arg1,  const QVariant &arg2,  bool arg3)
{
  AQ_CALL_RET_V(formatAssignValue(arg0, arg1, arg2, arg3), QString);
}
inline QString AQSManager::formatValue(int arg0,  const QVariant &arg1,  bool arg2)
{
  AQ_CALL_RET_V(formatValue(arg0, arg1, arg2), QString);
}
inline void AQSManager::loadTables()
{
  AQ_CALL_VOID(loadTables());
}
inline void AQSManager::cleanupMetaData()
{
  AQ_CALL_VOID(cleanupMetaData());
}
inline bool AQSManager::isSystemTable(const QString &arg0)
{
  AQ_CALL_RET_V(isSystemTable(arg0), bool);
}
inline QString AQSManager::storeLargeValue(FLTableMetaData *arg0,  const QString &arg1)
{
  AQ_CALL_RET_V(storeLargeValue(arg0, arg1), QString);
}
inline QVariant AQSManager::fetchLargeValue(const QString &arg0) const
{
  AQ_CALL_RET_V(fetchLargeValue(arg0), QVariant);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSMANAGER_P_H_ */
// @AQOBJECT_VOID@
