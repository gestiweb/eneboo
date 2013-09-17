/***************************************************************************
                 AQSqlConnections.h
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

#ifndef AQSQLCONNECTIONS_H_
#define AQSQLCONNECTIONS_H_

#include <QString>

class AQSqlConnectionsPrivate;
class AQSqlDatabase;

/**
Clase para manejar distintas conexiones a bases de datos.
 
Cada conexión a una base de datos tendrá una cadena de caracteres
como nombre que la identifica. Se podrán añadir conexiones con AQSqlConnections::addDatabase,
eliminar con AQSqlConnections::removeDatabase y obtener con AQSqlConnections::database.
 
La conexión por defecto tendrá el nombre "default".
 
@author InfoSiAL S.L.
*/
class AQSqlConnections
{
public:

  /**
  Añade una base de datos a las conexiones disponibles.

  La base de datos será abierta. Si ya existiera una conexión con el mismo nombre
  la base datos correspondiente será cerrada y borrada, sustituyéndola por la nueva.

  @param driverAlias Alias del driver ( PostgreSQL, MySQL, SQLite, ... ), ver AQSqlDatabase.
  @param nameDB  Nombre de la base de datos a la que conectar
  @param user  Usuario de la conexión
  @param password Contraseña para el usuario
  @param host  Nombre o dirección del servidor de la base de datos
  @param port  Puerto TCP de conexion
  @param connectionName Nombre de la conexion
  @return TRUE si se pudo realizar la conexión, FALSE en caso contrario
  */
  static bool addDatabase( const QString & driverAlias, const QString & nameDB, const QString & user,
                           const QString & password, const QString & host, int port,
                           const QString & connectionName = "default" );

  /**
  Elimina una base de datos de las conexiones disponibles.

  Cierra la base de datos correspondiente y la elimina.

  @param connectionName Nombre de la conexion
  @return TRUE si se pudo eliminar la base de datos, FALSE en caso contrario
  */
  static bool removeDatabase( const QString & connectionName );

  /**
  Obtiene la base de datos de una conexion.

  @param connectionNmae Nombre de la conexion
  @return La base de datos correspondiente al nombre de conexion indicado
  */
  static AQSqlDatabase database( const QString & connectionName = "default" );

  /**
     Inicializar el gestor de conexiones
     */
  static void init();

  /**
  Cierra todas las conexiones a las bases de datos
  */
  static void close();

private:

  /**
  constructor
  */
  AQSqlConnections()
  {}

  /**
  Privado
  */
  static AQSqlConnectionsPrivate * d_;
};

#endif /*AQSQLCONNECTIONS_H_*/
