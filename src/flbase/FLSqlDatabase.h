/***************************************************************************
                             FLSqlDatabase.h
                          -------------------
 begin                : Thu Nov 22 2005
 copyright            : (C) 2002-2005 by InfoSiAL S.L.
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

#ifndef FLSQLDATABASE_H
#define FLSQLDATABASE_H

#include <qsqldatabase.h>
#include <qsqlresult.h>

class FLTableMetaData;
class FLSqlCursor;
class FLManager;
class FLManagerModules;

/**
Gestión de bases de datos.

Proporciona una capa de abstracción para manejar distintos tipos de bases de datos.
A través de controladores internos (drivers) específicos se gestiona el acceso a cada
uno de los tipos de la bases de datos, ver FLSqlDriver.

Ofrece métodos para cargar controladores, conectar a la base de datos y realizar operaciones
específicas de forma unificada.

El sistemas de persistencia utiliza esta clase para poder trabajar de forma homogénea sobre cualquier
tipo de base de datos de la que exista un controlador.

@author InfoSiAL S.L.
*/
class FL_EXPORT FLSqlDatabase
{

public:

  /**
  Enumeración de opciones de conexión
  */
  enum ConnOptions {
    /** Usuario */
    USER = 0,
    /** Contraseña */
    PASSWORD = 1,
    /** Puerto */
    PORT = 2,
    /** Servidor */
    HOST = 3
  };

  /**
  constructor
  */
  FLSqlDatabase();

  /**
  destructor
  */
  ~FLSqlDatabase();

  /**
  @return Lista de los alias de los controladores actualmente disponibles.
  */
  static QStringList driverAliases();

  /**
  @return Alias establecido por defecto
  */
  static QString defaultAlias();

  /**
  @param alias Alias de un controlador
  @return Alias el nombre interno de un controlador a partir de su alias
  */
  static QString driverAliasToDriverName(const QString &alias);

  /**
  @param name Nombre interno de un controlador
  @return Alias de un controlador a partir de su nombre interno
  */
  static QString driverNameToDriverAlias(const QString &name);

  /**
  Obtiene si un controlador necesita una opción de conexión, a partir de su alias.

  @param alias Alias del controlador
  @param connOption Tipo de opción a comprobar, del tipo enumeración FLSqlDatabase::ConnOptions
  @return True si la opción es necesaria para que el controlador pueda establecer la conexión, false en caso contrario
  */
  static bool needConnOption(const QString &alias, const int connOption);

  /**
  Obtiene el puerto de conexión usado habitualmente por un controlador

  @param alias Alias del controlador
  @return Numero de puerto
  */
  static QString defaultPort(const QString &alias);

  /**
  Carga un controlador.

  @param driverName Nombre interno del controlador que se desa cargar
  @param connName Nombre de la conexion
  @return True si la carga tuvo éxito, false en caso contrario
  */
  bool loadDriver(const QString &driverName, const QString &connName = "default");

  /**
  Conecta con una base de datos.

  Para poder conectar con una base de datos es imprescindible haber cargado con anterioridad el driver
  correspondiente utilizando FLSqlDatabase::loadDriver().

  A partir de la llamada a este método la base de datos actual para a ser a la que conectamos. Internamente,
  si es posible, crea dos conexiones paralelas a la misma base de datos; FLSqlDatabase::db() y FLSqlDatabase::dbAux().

  @param database Nombre de la base de datos a la que conectar
  @param user  Usuario
  @param password Contraseña
  @param host  Servidor de la base de datos
  @param port  Puerto TCP de conexión
  @param connName Nombre de la conexion
  @return True si la conexión tuvo éxito, false en caso contrario
  */
  bool connectDB(const QString &database, const QString &user = QString::null,
                 const QString &password = QString::null,
                 const QString &host = QString::null,
                 int port = -1, const QString &connName = "default");

  /**
  Conecta con una base de datos utilizando los datos de conexión actuales
  */
  bool connectDB();

  /**
  Cierra la conexión actual de la base de datos
  */
  void closeDB();

  /**
  Crea una tabla en la base de datos actual.

  @param tmd Metadatos con la descripción de la tabla a crear
  @return True si se pudo crear la tabla, false en caso contrario
  */
  bool createTable(FLTableMetaData *tmd);

  /**
  @return True si la base de datos actual es capaz de regenerar tablas de forma dinámica
  */
  bool canRegenTables();

  /**
  Devuelve el contenido del valor de de un campo formateado para ser reconocido
  por la base de datos actual en condiciones LIKE, dentro de la clausura WHERE de SQL.

  Este método toma como parametros los metadatos del campo definidos con
  FLFieldMetaData. Además de TRUE y FALSE como posibles valores de un campo
  lógico también acepta los valores Sí y No (o su traducción al idioma correspondiente).
  Las fechas son adaptadas al forma AAAA-MM-DD, que es el formato reconocido por PostgreSQL .

  @param t Tipo de datos del valor
  @param v Valor que se quiere formatear para el campo indicado
  @param upper Si TRUE convierte a mayúsculas el valor (si es de tipo cadena)
  @return Valor del campo debidamente formateado
  */
  QString formatValueLike(int t, const QVariant &v, const bool upper = false);

  /**
  Devuelve el contenido del valor de de un campo formateado para ser reconocido
  por la base de datos actual, dentro de la clausura WHERE de SQL.

  Este método toma como parametros los metadatos del campo definidos con
  FLFieldMetaData. Además de TRUE y FALSE como posibles valores de un campo
  lógico también acepta los valores Sí y No (o su traducción al idioma correspondiente).
  Las fechas son adaptadas al forma AAAA-MM-DD, que es el formato reconocido por PostgreSQL .

  @param t Tipo de datos del valor
  @param v Valor que se quiere formatear para el campo indicado
  @param upper Si TRUE convierte a mayúsculas el valor (si es de tipo cadena)
  @return Valor del campo debidamente formateado
  */
  QString formatValue(int t, const QVariant &v, const bool upper = false);

  /**
  Obtiene el siguiente valor de la secuencia para campos del tipo serial.

  @param table Nombre la tabla del campo serial
  @param field Nombre del campo serial
  @return Siguiente valor de la secuencia
  */
  QVariant nextSerialVal(const QString &table, const QString &field);

  /**
  Obtiene la posición del registro actual.

  La posición del registro actual dentro del cursor se calcula teniendo en cuenta el
  filtro actual ( FLSqlCursor::curFilter() ) y el campo o campos de ordenamiento
  del mismo ( QSqlCursor::sort() ).
  Este método es útil, por ejemplo, para saber en que posición dentro del cursor
  se ha insertado un registro.

  @param cur Cursor sobre el que calcular la posición del registro.
  @return Posición del registro dentro del cursor.
  */
  int atFrom(FLSqlCursor *cur);

  /**
  @return Conexión principal a la base de datos actual
  */
  QSqlDatabase *db() const {
    return db_;
  }

  /**
  @return Conexión auxiliar a la base de datos actual
  */
  QSqlDatabase *dbAux() const {
    return dbAux_;
  }

  /**
  @return Nombre de la base de datos actual
  */
  QString database() const {
    return database_;
  }

  /**
  @return Usuario utilizado para conectar a la base de datos actual
  */
  QString user() const {
    return user_;
  }

  /**
  @return Contraseña utilizada para conectar a la base de datos actual
  */
  QString password() const {
    return password_;
  }

  /**
  @return Dirección del servidor donde se encuentra la base de datos actual
  */
  QString host() const {
    return host_;
  }

  /**
  @return Puerto TCP utlizado para conectar al servidor de la base de datos actual
  */
  int port() const {
    return port_;
  }

  /**
  @return Nombre interno del driver utilizado para conectar a la base de datos actual
  */
  QString driverName() const {
    return driverName_;
  }

  /**
  Modifica la estructura de una tabla dada, preservando los datos. La nueva
  estructura y la vieja se pasan en cadenas de caracteres con la descripcion XML.

  @param n Nombre de la tabla a reconstruir
  @param mtd1 Descripcion en XML de la vieja estructura
  @param mtd2 Descripcion en XML de la nueva estructura
  @param key Clave sha1 de la vieja estructura
  @return TRUE si la modificación tuvo éxito
  */
  bool alterTable(const QString &mtd1, const QString &mtd2, const QString &key = QString::null);

  /**
  @return Manejador general
  */
  FLManager *manager();

  /**
  @return Manejador de módulos
  */
  FLManagerModules *managerModules();

  /**
  @return Nombre de la conexión
  */
  QString connectionName() const {
    return connectionName_;
  }

  /**
  @return Si tiene capacidad para crear puntos de salvaguarda
  */
  bool canSavePoint();

  /**
  Crea un punto de salvaguarda

  @param n Nombre que se le asignará al punto de salvaguarda
  @return TRUE si la acción tuvo éxito
  */
  bool savePoint(const QString &n);

  /**
  Libera un punto de salvaguarda

  @param n Nombre del punto de salvaguarda a liberar
  @return TRUE si la acción tuvo éxito
  */
  bool releaseSavePoint(const QString &n);

  /**
  Deshace operaciones hasta el punto de salvaguarda

  @param n Nombre del punto de salvaguarda
  @return TRUE si la acción tuvo éxito
  */
  bool rollbackSavePoint(const QString &n);

  /**
  @return Si soporta transacciones
  */
  bool canTransaction() const;

  /**
  Ejecuta tareas de limpieza y optimización de la base de datos
  */
  void Mr_Proper();

  /**
  @return True si la base de datos actual puede detectar si sus transacciones están bloqueando a las de otra conexión
  */
  bool canDetectLocks() const;

  /**
  Para obtener información sobre el estado de los bloqueos existentes en la base de datos.

  Si hay bloqueos devuelve una lista de cadenas de texto en forma de registros de información. En esta lista
  la primera cadena de texto contiene los nombres de los campos de información incluidos y separados con "@",
  las siguientes cadenas son una por cada bloqueo con la información correspondiente.
  Si hay registros bloqueados produciendo situaciones de espera, se incluye información de los mismos cuando
  la cadena empieza por "##", indicando el nombre del campo clave primaria y el valor para el registro bloqueado.

  Ejemplo:

  "relation@locktype@pid"
  "stocks@RowExclusiveLock@8229"
  "##idstock=203"
  "secuencias@ExclusiveLock@8393"

  @return Lista con información de los bloqueos, si es vacia no hay bloqueos.
  */
  QStringList locksStatus();

  /**
  Comprueba si las transacciones de la base de datos actual están bloqueando a otras conexiones.

  Si hay bloqueos devuelve una lista de los mismos con el formato descrito en FLSqlDatabase::locksStatus()

  @return Lista con información de los bloqueos, si es vacia no hay bloqueos.
  */
  QStringList detectLocks();

  /**
  Comprueba si hay riesgo de caer en un bloqueo en espera con otras conexiones.

  Si hay riesgo devuelve una lista de los bloqueos candidatos con el mismo formato descrito en FLSqlDatabase::locksStatus()

  @param  table           El nombre de una tabla para solo comprobar los riesgos sobre ella, o vacio
                          para comprobarlos en todas las tablas.
  @param  primaryKeyValue El valor de la clave primaria de un registro para solo comprobar los riesgos sobre el,
                          o vacio para comprobarlos en todos. ( No funciona con claves compuestas ).
  @return Lista con información de los bloqueos, si es vacia no hay bloqueos.
  */
  QStringList detectRisksLocks(const QString &table = QString::null, const QString &primaryKeyValue = QString::null);

  /**
  Regenera una tabla si su estructura actual en la base de datos difiere de la estructura definida en los metadatos
  pasados como parámetro.

  @param  n   Nombre de la tabla de la base de datos
  @param  tmd Metadatos con la descripción de la tabla
  @return True si se necesitaba regenerar la tabla y la regenación tuvo éxito
  */
  bool regenTable(const QString &n, FLTableMetaData *tmd);

private:

  /** Conexión principal a la base de datos actual */
  QSqlDatabase *db_;

  /** Usuario utilizado para conectar a la base de datos actual */
  QSqlDatabase *dbAux_;

  /** Nombre de la base de datos actual */
  QString database_;

  /** Usuario utilizado para conectar a la base de datos actual */
  QString user_;

  /** Contraseña utilizada para conectar a la base de datos actual */
  QString password_;

  /** Dirección del servidor donde se encuentra la base de datos actual */
  QString host_;

  /** Puerto TCP utlizado para conectar al servidor de la base de datos actual */
  int port_;

  /**  Nombre interno del driver utilizado para conectar a la base de datos actual */
  QString driverName_;

  /** Manejador general */
  FLManager *manager_;

  /** Manejador de módulos */
  FLManagerModules *managerModules_;

  /** Nombre de la conexion, ver FLSqlConnections */
  QString connectionName_;

  /** Indica si esta activado la detección de bloqueos */
  bool lockDetection_;
};

#endif
