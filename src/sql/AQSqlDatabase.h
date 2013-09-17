/***************************************************************************
      AQSqlDatabase.h
  -------------------
begin                : 08/08/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
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

#ifndef AQSQLDATABASE_H_
#define AQSQLDATABASE_H_

#include <QStringList>
#include <QVariant>
#include <QSqlDatabase>

#include <AQSql/AQSqlDatabase_p.h>

class AQSqlCursor;

/**
Gestión de bases de datos.
 
Proporciona una capa de abstracción para manejar distintos tipos de bases de datos.
A través de controladores internos (drivers) específicos se gestiona el acceso a cada
uno de los tipos de la bases de datos, ver AQSqlDriver.
 
Ofrece métodos para cargar controladores, conectar a la base de datos y realizar operaciones
específicas de forma unificada.
 
El sistemas de persistencia utiliza esta clase para poder trabajar de forma homogénea sobre cualquier
tipo de base de datos de la que exista un controlador.
 
@author InfoSiAL S.L.
*/
class AQSqlDatabase
{

public:

  /**
  constructor
  */
  AQSqlDatabase();

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
  static QString driverAliasToDriverName( const QString & alias );

  /**
  @param name Nombre interno de un controlador
  @return Alias de un controlador a partir de su nombre interno
  */
  static QString driverNameToDriverAlias( const QString & name );

  /**
  Obtiene si un controlador necesita una opción de conexión, a partir de su alias.

  @param alias Alias del controlador
  @param connOption Tipo de opción a comprobar, del tipo enumeración AQSqlDatabase::ConnOptions
  @return True si la opción es necesaria para que el controlador pueda establecer la conexión, false en caso contrario
  */
  static bool needConnOption( const QString & alias, int connOption );

  /**
  Carga un controlador.

  @param driverName Nombre interno del controlador que se desa cargar
  @param connName Nombre de la conexion
  @return True si la carga tuvo éxito, false en caso contrario
  */
  bool loadDriver( const QString & driverName, const QString & connName = "default" );

  /**
  Conecta con una base de datos.

  Para poder conectar con una base de datos es imprescindible haber cargado con anterioridad el driver
  correspondiente utilizando AQSqlDatabase::loadDriver().

  A partir de la llamada a este método la base de datos actual para a ser a la que conectamos. Internamente,
  si es posible, crea dos conexiones paralelas a la misma base de datos; AQSqlDatabase::db() y AQSqlDatabase::dbAux().

  @param database Nombre de la base de datos a la que conectar
  @param user  Usuario
  @param password Contraseña
  @param host  Servidor de la base de datos
  @param port  Puerto TCP de conexión
  @param connName Nombre de la conexion
  @return True si la conexión tuvo éxito, false en caso contrario
  */
  bool connectDB( const QString & database, const QString & user = QString(),
                  const QString & password = QString(), const QString & host = QString(), int port = -1,
                  const QString & connName = "default" );

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
  bool createTable( AQTableMD * tmd ) const;

  /**
  @return True si la base de datos actual es capaz de regenerar tablas de forma dinámica
  */
  bool canRegenTables() const;

  /**
  Devuelve el contenido del valor de de un campo formateado para ser reconocido
  por la base de datos actual en condiciones LIKE, dentro de la clausura WHERE de SQL.

  Este método toma como parametros los metadatos del campo definidos con
  AQFieldMD. Además de TRUE y FALSE como posibles valores de un campo
  lógico también acepta los valores Sí y No (o su traducción al idioma correspondiente).
  Las fechas son adaptadas al forma AAAA-MM-DD, que es el formato reconocido por PostgreSQL .

  @param t Tipo de datos del valor
  @param v Valor que se quiere formatear para el campo indicado
  @param upper Si TRUE convierte a mayúsculas el valor (si es de tipo cadena)
  @return Valor del campo debidamente formateado
  */
  QString formatValueLike( int t, const QVariant & v, bool upper = false ) const;

  /**
  Devuelve el contenido del valor de de un campo formateado para ser reconocido
  por la base de datos actual, dentro de la clausura WHERE de SQL.

  Este método toma como parametros los metadatos del campo definidos con
  AQFieldMD. Además de TRUE y FALSE como posibles valores de un campo
  lógico también acepta los valores Sí y No (o su traducción al idioma correspondiente).
  Las fechas son adaptadas al forma AAAA-MM-DD, que es el formato reconocido por PostgreSQL .

  @param t Tipo de datos del valor
  @param v Valor que se quiere formatear para el campo indicado
  @param upper Si TRUE convierte a mayúsculas el valor (si es de tipo cadena)
  @return Valor del campo debidamente formateado
  */
  QString formatValue( int t, const QVariant & v, bool upper = false ) const;

  /**
  Obtiene el siguiente valor de la secuencia para campos del tipo serial.

  @param table Nombre la tabla del campo serial
  @param field Nombre del campo serial
  @return Siguiente valor de la secuencia
  */
  QVariant nextSerialVal( const QString & table, const QString & field ) const;

  /**
  Obtiene la posición del registro actual.

  La posición del registro actual dentro del cursor se calcula teniendo en cuenta el
  filtro actual ( AQSqlCursor::curFilter() ) y el campo o campos de ordenamiento
  del mismo ( QSqlCursor::sort() ).
  Este método es útil, por ejemplo, para saber en que posición dentro del cursor 
  se ha insertado un registro.

  @param cur Cursor sobre el que calcular la posición del registro.
  @return Posición del registro dentro del cursor.
  */
  int atFrom( AQSqlCursor * cur ) const;

  /**
  @return Conexión principal a la base de datos actual
  */
  QSqlDatabase db() const;

  /**
  @return Conexión auxiliar a la base de datos actual
  */
  QSqlDatabase dbAux() const;

  /**
  @return Nombre de la base de datos actual
  */
  QString database() const;

  /**
  @return Usuario utilizado para conectar a la base de datos actual
  */
  QString user() const;

  /**
  @return Contraseña utilizada para conectar a la base de datos actual
  */
  QString password() const;

  /**
  @return Dirección del servidor donde se encuentra la base de datos actual
  */
  QString host() const;

  /**
  @return Puerto TCP utlizado para conectar al servidor de la base de datos actual
  */
  int port() const;

  /**
  @return Nombre interno del driver utilizado para conectar a la base de datos actual
  */
  QString driverName() const;

  /**
  @return Manejador general
  */
  const AQManager * manager() const;

  /**
  @return Manejador de módulos
  */
  const AQManagerModules * managerModules() const;

  /**
  @return Motor de scripts
  */
  const AQScriptEngineQSA * scriptEngine() const;

  /**
  @return Nombre de la conexión
  */
  QString connectionName() const;

  /**
  @return Si tiene capacidad para crear puntos de salvaguarda
  */
  bool canSavePoint() const;

  /**
  Crea un punto de salvaguarda

  @param n Nombre que se le asignará al punto de salvaguarda
  @return TRUE si la acción tuvo éxito
  */
  bool savePoint( const QString & n ) const;

  /**
  Libera un punto de salvaguarda

  @param n Nombre del punto de salvaguarda a liberar
  @return TRUE si la acción tuvo éxito
  */
  bool releaseSavePoint( const QString & n ) const;

  /**
  Deshace operaciones hasta el punto de salvaguarda

  @param n Nombre del punto de salvaguarda
  @return TRUE si la acción tuvo éxito
  */
  bool rollbackSavePoint( const QString & n ) const;

  /**
  Crea un indice de bajo nivel en la base de datos para el campo y la tabla indicada.

  @param fieldName Nombre del campo
  @param tableName Nombre de la tabla
  @param upper Indica que se cree un índice para valores en mayúsculas.
         Sólo es efectivo en algunos tipos de base de datos.
  */
  void createIndex( const QString & fieldName, const QString & tableName, bool upper = true ) const;

  /**
  @return Identificador único y global para la sessión actualmente abierta en la base de datos
  */
  QString idSessionGlobal() const;

  /**
  Sobrecarga de operadores
  */
  inline bool operator==( const AQSqlDatabase & other ) const
  {
    return d_ == other.d_;
  }
  inline bool operator!=( const AQSqlDatabase & other ) const
  {
    return d_ != other.d_;
  }

private:

  /**
  Uso interno
  */
  void init();
  void finish();

  QExplicitlySharedDataPointer<AQSqlDatabasePrivate> d_;
};

#endif /*AQSQLDATABASE_H_*/
