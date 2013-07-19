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
#include "AQZipReader.h"
#include "AQZipWriter.h"
#include "AQOds.h"

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
#include "FLVar.h"
#include "FLReportEngine.h"
#include "FLReportViewer.h"
#include "FLJasperEngine.h"
#include "FLSqlConnections.h"
#include "FLSmtpClient.h"

class AQSmtpClient : public FLSmtpClient
{
  Q_OBJECT

public:
  AQSmtpClient(QObject *parent = 0) : FLSmtpClient(parent) {}
};

class AQJasperEngine : public FLJasperEngine
{
  Q_OBJECT

public:
  AQJasperEngine(QObject *parent = 0) : FLJasperEngine(parent) {}
};

class AQReportViewer : public FLReportViewer
{
  Q_OBJECT

public:
  AQReportViewer(QWidget *parent = 0,
                 const char *name = 0,
                 bool embedInParent = false,
                 FLReportEngine *rptEngine = 0) :
    FLReportViewer(parent, name, embedInParent, rptEngine) {}
};

class AQReportEngine : public FLReportEngine
{
  Q_OBJECT

public:
  AQReportEngine(QObject *parent = 0) : FLReportEngine(parent) {}
};

class AQVar : public FLVar
{
public:
  AQVar() : FLVar() {}
};

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
  Q_ENUMS(TableType)
  Q_ENUMS(SqlErrorType)
  Q_ENUMS(Refresh)

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

  enum TableType {
    Tables = 0x01,
    SystemTables = 0x02,
    Views = 0x04,
    AllTables = 0xff
  };

  enum SqlErrorType {
    SqlErrorNone,
    SqlErrorConnection,
    SqlErrorStatement,
    SqlErrorTransaction,
    SqlErrorUnknown
  };

  enum Refresh {
    RefreshData = 1,
    RefreshColumns = 2,
    RefreshAll = 3
  };

  AQSql() : QObject(0, "aqs_aqsql") {}

  void throwError(const QString &msg, FLSqlDatabase *db) {
    if (!db->qsaExceptions() || !globalAQSInterpreter) {
      qWarning(msg);
      return;
    }
    if (globalAQSInterpreter->isRunning())
      globalAQSInterpreter->throwError(msg);
    else
      qWarning(msg);
  }

public slots:
  /**
  Añade una base de datos a las conexiones disponibles.

  La base de datos será abierta. Si ya existiera una conexión con el mismo nombre
  la base datos correspondiente será cerrada y borrada, sustituyéndola por la nueva.

  @param driverAlias Alias del driver ( PostgreSQL, MySQL, SQLite, ... ), ver FLSqlDatabase.
  @param nameDB  Nombre de la base de datos a la que conectar
  @param user  Usuario de la conexión
  @param password Contraseña para el usuario
  @param host  Nombre o dirección del servidor de la base de datos
  @param port  Puerto TCP de conexion
  @param connectionName Nombre de la conexion
    @param connectOptions Contiene opciones auxiliares de conexión a la base de datos.
                        El formato de la cadena de opciones es una lista separada por punto y coma
                        de nombres de opción o la opción = valor. Las opciones dependen del uso del
                        driver de base de datos.
  @return TRUE si se pudo realizar la conexión, FALSE en caso contrario
  */
  bool addDatabase(const QString &driverAlias, const QString &nameDB, const QString &user,
                   const QString &password, const QString &host, int port,
                   const QString &connectionName, const QString &connectOptions = QString::null) {
    return FLSqlConnections::addDatabase(driverAlias, nameDB, user, password, host, port,
                                         connectionName, connectOptions);
  }

  /**
  Sobrecargada por conveniencia

  Practicamente hace lo mismo que el método anterior pero utilizando una base de datos ya construida

  @param db  Base datos a añadir a las conexiones disponibles, ver FLSqlDatabase.
  @param connectionName Nombre de la conexion
  @return TRUE si se pudo realizar la conexión, FALSE en caso contrario
  */
  bool addDatabase(FLSqlDatabase *db, const QString &connectionName = "default") {
    return FLSqlConnections::addDatabase(db, connectionName);
  }

  /**
  Sobrecargada por conveniencia

  Añade una base de datos a las conexiones disponibles utilizando los datos de otra conexión

  @param newConnName    Nombre a utilizar para la nueva conexion
  @param sourceConnName Nombre de una conexión existente a utilizar como origen de los datos de conexión
  @return TRUE si se pudo realizar la conexión, FALSE en caso contrario
  */
  bool addDatabase(const QString &newConnName, const QString &sourceConnName = "default") {
    FLSqlDatabase *srcDb = FLSqlConnections::database(sourceConnName);
    if (!srcDb)
      return false;
    return FLSqlConnections::addDatabase(srcDb->driverName(), srcDb->database(),
                                         srcDb->user(), srcDb->password(), srcDb->host(),
                                         srcDb->port(), newConnName);
  }

  /**
  Elimina una base de datos de las conexiones disponibles.

  Cierra la base de datos correspondiente y la elimina.

  @param connectionName Nombre de la conexion
  @return TRUE si se pudo eliminar la base de datos, FALSE en caso contrario
  */
  bool removeDatabase(const QString &connectionName) {
    return FLSqlConnections::removeDatabase(connectionName);
  }

  /**
  Obtiene la base de datos de una conexion.

  @param connectionNmae Nombre de la conexion
  @return La base de datos correspondiente al nombre de conexion indicado
  */
  FLSqlDatabase *database(const QString &connectionName = "default") {
    return FLSqlConnections::database(connectionName);
  }

  /**
  Finalizar todas las conexiones
  */
  void finish() {
    FLSqlConnections::finish();
  }

  /**
  Inserta un registro en un cursor

  Si hay un error SQL, eleva una excepción con el mensaje de error

  Ejemplo:

  var cur = new AQSqlCursor("clientes");
  try {
    AQSql.insert(cur,
                 ["codcliente","nombre","cifnif","codserie"],
                 ["1","pepe","XYZ","A"]);
  } catch (e) {
    sys.errorMsgBox("Error SQL: " + e);
  }
  */
  bool insert(FLSqlCursor *cur, const QStringList &fields,
              const QValueList<QVariant> &values) {
    if (!cur)
      return false;

    if (!cur->metadata()) {
      throwError(
        tr("No hay metadatos para '%1'").arg(cur->QObject::name()), cur->db()
      );
      return false;
    }

    int fieldsCount = fields.size();
    int valuesCount = values.size();

    cur->setModeAccess(Insert);
    if (!cur->refreshBuffer())
      return false;
    for (int i = 0; i < fieldsCount; ++i)
      cur->setValueBuffer(fields[i], (i < valuesCount ? values[i] : QVariant()));

    QString msgCheck(cur->msgCheckIntegrity());
    if (!msgCheck.isEmpty()) {
      throwError(msgCheck, cur->db());
      return false;
    }

    bool actCheck = cur->activatedCheckIntegrity();
    cur->setActivatedCheckIntegrity(false);
    bool ok = cur->commitBuffer();
    cur->setActivatedCheckIntegrity(actCheck);
    return ok;
  }

  bool insert(const QString &table, const QStringList &fields,
              const QValueList<QVariant> &values,
              const QString &connName = "default") {
    AQSqlCursor cur(table, true, connName);
    return insert(&cur, fields, values);
  }

  /**
  Actualiza un conjunto de registros de un cursor con nuevos valores

  Si hay un error SQL, eleva una excepción con el mensaje de error

  Ejemplo:

  var cur = new AQSqlCursor("clientes");
  try {
    AQSql.update(cur,
                 ["nombre","cifnif","codserie"],
                 ["juan","ZYX","A"],
                 "codcliente='1'");
  } catch (e) {
    sys.errorMsgBox("Error SQL: " + e);
  }
  */
  bool update(FLSqlCursor *cur, const QStringList &fields,
              const QValueList<QVariant> &values, const QString &where = "") {
    if (!cur)
      return false;

    if (!cur->metadata()) {
      throwError(
        tr("No hay metadatos para '%1'").arg(cur->QObject::name()), cur->db()
      );
      return false;
    }

    if (!cur->select(where))
      return false;

    bool ok = true;
    QString msgCheck;
    bool actCheck = cur->activatedCheckIntegrity();
    int fieldsCount = fields.size();
    int valuesCount = values.size();

    while (ok && cur->next()) {
      cur->setModeAccess(Edit);
      if (!cur->refreshBuffer()) {
        ok = false;
        break;
      }
      for (int i = 0; i < fieldsCount; ++i)
        cur->setValueBuffer(fields[i], (i < valuesCount ? values[i] : QVariant()));

      msgCheck = cur->msgCheckIntegrity();
      if (!msgCheck.isEmpty()) {
        ok = false;
        throwError(msgCheck, cur->db());
        break;
      }

      cur->setActivatedCheckIntegrity(false);
      ok = cur->commitBuffer();
      cur->setActivatedCheckIntegrity(actCheck);
    }
    return ok;
  }

  bool update(const QString &table, const QStringList &fields,
              const QValueList<QVariant> &values, const QString &where = "",
              const QString &connName = "default") {
    AQSqlCursor cur(table, true, connName);
    cur.setForwardOnly(true);
    return update(&cur, fields, values, where);
  }

  /**
  Elimina un conjunto de registros de un cursor

  Si hay un error SQL, eleva una excepción con el mensaje de error

  Ejemplo:

  var cur = new AQSqlCursor("clientes");
  try {
    AQSql.del(cur, "codcliente='1'");
  } catch (e) {
    sys.errorMsgBox("Error SQL: " + e);
  }
  */
  bool del(FLSqlCursor *cur, const QString &where = "") {
    if (!cur)
      return false;

    if (!cur->metadata()) {
      throwError(
        tr("No hay metadatos para '%1'").arg(cur->QObject::name()), cur->db()
      );
      return false;
    }

    if (!cur->select(where))
      return false;

    bool ok = true;
    QString msgCheck;
    bool actCheck = cur->activatedCheckIntegrity();

    while (ok && cur->next()) {
      cur->setModeAccess(Del);
      if (!cur->refreshBuffer()) {
        ok = false;
        break;
      }

      msgCheck = cur->msgCheckIntegrity();
      if (!msgCheck.isEmpty()) {
        ok = false;
        throwError(msgCheck, cur->db());
        break;
      }

      cur->setActivatedCheckIntegrity(false);
      ok = cur->commitBuffer();
      cur->setActivatedCheckIntegrity(actCheck);
    }
    return ok;
  }

  bool del(const QString &table, const QString &where = "",
           const QString &connName = "default") {
    AQSqlCursor cur(table, true, connName);
    cur.setForwardOnly(true);
    return del(&cur, where);
  }

  /**
  Ejecuta una consulta y devuelve información de la misma y el conjunto de
  registros obtenidos

  Si hay un error SQL, eleva una excepción con el mensaje de error

  Devuelve un array 'A' donde:

  -A[0] contiene el número de registros
  -A[1] contiene el número de campos
  -A[2] contiene otro array con los nombres de lo campos, en orden correlativo
        al que aparecen en la consulta
  -A[3..fin] los valores de los campos, en grupos de tamaño A[1]

  Ejemplo:

  var records;
  try {
    records = AQSql.select("bancos.*", "bancos");
    if (!records.length)
      return;

    var size = records[0];
    var nFields = records[1];
    var fieldNames = records[2];

    var rec = "";
    for (var i = 0; i < nFields; ++i)
      rec += fieldNames[i] + "    | ";
    print(rec);
    print("==============================================");

    for (var i = 3; i <= size * nFields;  i += nFields) {
      rec = "";
      for (var j = 0; j < nFields; ++j) {
        rec += records[i + j] + " | ";
      }
      print(rec);
    }
  } catch (e) {
    sys.errorMsgBox("Error SQL: " + e);
  }
  */
  QValueList<QVariant> select(const QString &select,
                              const QString &from,
                              const QString &where = QString::null,
                              const QString &orderBy = QString::null,
                              const QString &connName = "default") {
    AQSqlQuery qry(0, connName);
    qry.setTablesList(from);
    qry.setSelect(select);
    qry.setFrom(from);
    qry.setWhere(where);
    qry.setOrderBy(orderBy);
    qry.setForwardOnly(true);
    if (!qry.exec())
      return QVariantList();

    QVariantList ret;
    int countFields = qry.fieldList().count();

    ret.append(qry.size());
    ret.append(countFields);
    ret.append(qry.fieldList());

    while (qry.next()) {
      for (int i = 0; i < countFields; ++i)
        ret.append(qry.value(i));
    }

    return ret;
  }

  /**
  Esencialmente hace lo mismo que AQSql::select(), pero con la diferencia que reservará
  un bloqueo sobre los registros que devuelve la consulta.

  Si hay otro selectForUpdate anterior sobre los mismos o algunos registros que obtiene la
  consulta esta llamanda quedará en espera, bloqueada sobre esos registros, hasta que termine
  la transacción que inició el otro selectForUpdate.

  En PostgreSQL se puede utizar el parámetro 'nowait'. Si es TRUE y si al ejecutar la
  consulta se detecta que se va a caer en un bloqueo, es decir hay otro selectForUpdate anterior,
  no se bloqueará la llamada, y se elevará una excepción, terminando la transacción en curso.
  El parámetro 'nowait' no tiene efecto para cualquier otra base de datos distinta a PostgreSQL.
  */
  QValueList<QVariant> selectForUpdate(const QString &select,
                                       const QString &from,
                                       const QString &where = QString::null,
                                       bool nowait = false,
                                       const QString &connName = "default") {
    AQSqlQuery qry(0, connName);
    qry.setTablesList(from);
    qry.setSelect(select);
    qry.setFrom(from);
    QString w(where.isEmpty() ? "1=1" : w);
    qry.setWhere(nowait ?
                 w + QString::fromLatin1(" FOR UPDATE NOWAIT") :
                 w + QString::fromLatin1(" FOR UPDATE"));
    qry.setForwardOnly(true);
    if (!qry.exec())
      return QVariantList();

    QVariantList ret;
    int countFields = qry.fieldList().count();

    ret.append(qry.size());
    ret.append(countFields);
    ret.append(qry.fieldList());

    while (qry.next()) {
      for (int i = 0; i < countFields; ++i)
        ret.append(qry.value(i));
    }

    return ret;
  }
};

#include "FLFieldDB.h"
#include "FLTableDB.h"
#include "FLDataTable.h"

class AQFieldDB : public FLFieldDB
{
public:
  AQFieldDB(QWidget *parent = 0, const char *name = 0) :
    FLFieldDB(parent, name) {}
};

class AQTableDB : public FLTableDB
{
public:
  AQTableDB(QWidget *parent = 0, const char *name = 0) :
    FLTableDB(parent, name) {}
};

class AQDataTableDB : public FLDataTable
{
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
public:
  AQFormRecordDB(FLSqlCursor *cursor, const QString &actionName = QString::null,
                 QWidget *parent = 0, bool showAcceptContinue = true):
    FLFormRecordDB(cursor, actionName, parent) {}
};

class AQFormSearchDB : public FLFormSearchDB
{
public:
  AQFormSearchDB(const QString &actionName, QWidget *parent = 0) :
    FLFormSearchDB(actionName, parent) {}

  AQFormSearchDB(FLSqlCursor *cursor, const QString &actionName = QString::null,
                 QWidget *parent = 0) :
    FLFormSearchDB(cursor, actionName, parent) {}
};

#endif /* AQOBJECTS_H_ */
