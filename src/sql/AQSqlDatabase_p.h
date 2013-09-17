/***************************************************************************
AQSqlDatabase_p.h
-------------------
begin                : 18/08/2008
copyright            : (C) 2003-2008 by InfoSiAL S.L.
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

#ifndef AQSQLDATABASE_P_H_
#define AQSQLDATABASE_P_H_

#include <QSharedData>

#include <AQScript/AQScriptEngineQSA.h>
#include <AQSql/AQManager.h>
#include <AQSql/AQManagerModules.h>

class AQSqlDriver;

class AQSqlDatabasePrivate : public QSharedData
{
public:

  AQSqlDatabasePrivate();
  AQSqlDatabasePrivate( const AQSqlDatabasePrivate & other );
  ~AQSqlDatabasePrivate();

  /** Conexión interna principal a la base de datos actual */
  QSqlDatabase db_;

  /** Nombre de la conexión interna principal */
  QString dbConnName_;

  /** Driver */
  AQSqlDriver * dr_;
  AQSqlDriver * drAux_;

  /** Conexión interna auxiliar a la base de datos actual */
  QSqlDatabase dbAux_;

  /** Nombre de la conexión interna auxiliar */
  QString dbAuxConnName_;

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
  AQManager manager_;

  /** Manejador de módulos */
  AQManagerModules managerModules_;

  /** Motor de scripts */
  AQScriptEngineQSA scriptEngine_;

  /** Nombre de la conexion, ver AQSqlConnections */
  QString connectionName_;

  /** Identificador único y global para la sesion abierta de la base de datos */
  QString idSessionGlobal_;
};

#endif /*AQSQLDATABASE_P_H_*/
