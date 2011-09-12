/***************************************************************************
 AQObjects.h
 -------------------
 begin                : 09/02/2011
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

#ifndef AQOBJECTS_H_
#define AQOBJECTS_H_

#include "AQS_p.h"
#include "AQPackager.h"
#include "AQCompilerQSA.h"

#include "FLSqlDatabase.h"
#include "FLManager.h"
#include "FLManagerModules.h"
#include "FLRelationMetaData.h"
#include "FLFieldMetaData.h"
#include "FLTableMetaData.h"
#include "FLAction.h"
#include "FLSqlCursor.h"
#include "FLSqlQuery.h"
#include "FLSettings.h"

class AQSettings : public FLSettings
{
public:
  AQSettings() : FLSettings() {}
};

class AQSqlDatabase : public FLSqlDatabase
{
public:
  AQSqlDatabase() : FLSqlDatabase() {}
};

class AQManager : public FLManager
{
public:
  AQManager() : FLManager() {}
};

class AQManagerModules : public FLManagerModules
{
public:
  AQManagerModules() : FLManagerModules() {}
};

class AQActionMD: public FLAction
{
public:
  AQActionMD() : FLAction() {}
};

class AQRelationMD : public FLRelationMetaData
{
public:
  AQRelationMD(const QString &fT, const QString &fF, int rC,
               bool dC = false, bool uC = false, bool cI = true) :
    FLRelationMetaData(fT, fF, rC, dC, uC, cI) {}
};

class AQFieldMD : public FLFieldMetaData
{
public:
  AQFieldMD(const QString &n, const QString &a, bool aN, bool iPK, int t,
            int l = 0, bool c = false, bool v = true, bool ed = false,
            int pI = 4, int pD = 0, bool iNX = false, bool uNI = false,
            bool coun = false, const QVariant &defValue = QVariant(), bool oT = false,
            const QString &rX = QString::null, bool vG = true, bool gen = true, bool iCK = false) :
    FLFieldMetaData(n, a, aN, iPK, t,
                    l, c, v, ed,
                    pI, pD, iNX, uNI,
                    coun, defValue, oT,
                    rX, vG, gen, iCK) {}
};

class AQTableMD : public FLTableMetaData
{
public:
  AQTableMD(const QString &n, const QString &a, const QString &q = QString::null) :
    FLTableMetaData(n, a, q) {}
};

class AQSqlCursor : public FLSqlCursor
{
  Q_OBJECT

public:
  AQSqlCursor(const QString &name = QString::null, bool autopopulate = true,
              const QString &connectionName = "default", FLSqlCursor *cR = 0,
              FLRelationMetaData *r = 0, QObject *parent = 0) :
    FLSqlCursor(name, autopopulate, connectionName, cR, r, parent) {}

  AQSqlCursor(const QString &name, bool autopopulate, QSqlDatabase *db, FLSqlCursor *cR = 0,
              FLRelationMetaData *r = 0, QObject *parent = 0) :
    FLSqlCursor(name, autopopulate, db, cR, r, parent) {}

  AQSqlCursor(const QString &name, QObject *parent,
              const QString &connectionName = "default") :
    FLSqlCursor(name, true, connectionName, 0, 0, parent) {}

  AQSqlCursor(const QString &name, const QString &connectionName) :
    FLSqlCursor(name, true, connectionName) {}
};

class AQSqlQuery : public FLSqlQuery
{
  Q_OBJECT

public:
  AQSqlQuery(QObject *parent = 0, const QString &connectionName = "default") :
    FLSqlQuery(parent, connectionName) {}
};

class AQSql : public QObject
{
  Q_OBJECT

  Q_ENUMS(ModeAccess)
  Q_ENUMS(ConnOptions)
  Q_ENUMS(SpecialType)
  Q_ENUMS(Cardinality)

public:
  enum ModeAccess {
    Insert = 0,
    Edit = 1,
    Del = 2,
    Browse = 3
  };

  enum ConnOptions {
    User = 0,
    Password = 1,
    Port = 2,
    Host = 3
  };

  enum SpecialType {
    Serial = 100,
    Unlock = 200,
    Check = 300
  };

  enum Cardinality {
    RELATION_1M = 0,
    RELATION_M1 = 1
  };

  AQSql() : QObject(0, "aqs_aqsql") {}

public slots:
  bool insert(AQSqlCursor *cur, const QStringList &fields,
              const QValueList<QVariant> &values) {
    if (!cur)
      return false;

    int fieldsCount = fields.size();
    int valuesCount = values.size();

    cur->setModeAccess(Insert);
    cur->refreshBuffer();
    for (int i = 0; i < fieldsCount; ++i)
      cur->setValueBuffer(fields[i], (i < valuesCount ? values[i] : QVariant()));

    return cur->commitBuffer();
  }

  bool insert(const QString &table, const QStringList &fields,
              const QValueList<QVariant> &values, const QString &connName = "default") {
    AQSqlCursor cur(table, true, connName);
    return insert(&cur, fields, values);
  }

  bool update(AQSqlCursor *cur, const QStringList &fields,
              const QValueList<QVariant> &values, const QString &where = "") {
    if (!cur || !cur->select(where))
      return false;

    int fieldsCount = fields.size();
    int valuesCount = values.size();

    while (cur->next()) {
      cur->setModeAccess(Edit);
      cur->refreshBuffer();
      for (int i = 0; i < fieldsCount; ++i)
        cur->setValueBuffer(fields[i], (i < valuesCount ? values[i] : QVariant()));
      if (!cur->commitBuffer())
        return false;
    }
    return true;
  }

  bool update(const QString &table, const QStringList &fields,
              const QValueList<QVariant> &values, const QString &where = "",
              const QString &connName = "default") {
    AQSqlCursor cur(table, true, connName);
    cur.setForwardOnly(true);
    return update(&cur, fields, values, where);
  }

  bool del(AQSqlCursor *cur, const QString &where = "") {
    if (!cur || !cur->select(where))
      return false;

    while (cur->next()) {
      cur->setModeAccess(Del);
      cur->refreshBuffer();
      if (!cur->commitBuffer())
        return false;
    }
    return true;
  }

  bool del(const QString &table, const QString &where = "",
           const QString &connName = "default") {
    AQSqlCursor cur(table, true, connName);
    cur.setForwardOnly(true);
    return del(&cur, where);
  }
};

#include "FLFieldDB.h"
#include "FLTableDB.h"
#include "FLDataTable.h"

class AQFieldDB : public FLFieldDB
{
  Q_OBJECT

public:
  AQFieldDB(QWidget *parent = 0, const char *name = 0) :
    FLFieldDB(parent, name) {}
};

class AQTableDB : public FLTableDB
{
  Q_OBJECT

public:
  AQTableDB(QWidget *parent = 0, const char *name = 0) :
    FLTableDB(parent, name) {}
};

class AQDataTableDB : public FLDataTable
{
  Q_OBJECT

public:
  AQDataTableDB(QWidget *parent = 0, const char *name = 0, bool popup = false) :
    FLDataTable(parent, name, popup) {}
};

#include "FLDigiDoc.h"

class AQDigiDoc : public FLDigiDoc
{
public:
  AQDigiDoc(const QString &configfile = QString::null) :
    FLDigiDoc(configfile) {}
};

#include "FLFormDB.h"
#include "FLFormRecordDB.h"
#include "FLFormSearchDB.h"

class AQFormDB : public FLFormDB
{
  Q_OBJECT

public:
  explicit AQFormDB(QWidget *parent = 0, const char *name = 0, WFlags f = 0) :
    FLFormDB(parent, name, f) {}

  AQFormDB(const QString &actionName, QWidget *parent = 0, WFlags f = 0) :
    FLFormDB(actionName, parent, f) {}

  AQFormDB(FLSqlCursor *cursor, const QString &actionName = QString::null,
           QWidget *parent = 0, WFlags f = 0) :
    FLFormDB(cursor, actionName, parent, f) {}
};


class AQFormRecordDB : public FLFormRecordDB
{
  Q_OBJECT

public:
  AQFormRecordDB(FLSqlCursor *cursor, const QString &actionName = QString::null,
                 QWidget *parent = 0, bool showAcceptContinue = true):
    FLFormRecordDB(cursor, actionName, parent) {}
};

class AQFormSearchDB : public FLFormSearchDB
{
  Q_OBJECT

public:
  AQFormSearchDB(const QString &actionName, QWidget *parent = 0) :
    FLFormSearchDB(actionName, parent) {}

  AQFormSearchDB(FLSqlCursor *cursor, const QString &actionName = QString::null,
                 QWidget *parent = 0) :
    FLFormSearchDB(cursor, actionName, parent) {}
};

#endif /* AQOBJECTS_H_ */
