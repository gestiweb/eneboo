/***************************************************************************
                             FLSqlDriver.h
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

#ifndef FLSQLDRIVER_H
#define FLSQLDRIVER_H

#include <qsqldriver.h>

class FLTableMetaData;
class FLSqlCursor;
class FLSqlDatabase;

/**
Clase de abstracción para controladores de bases de datos.

Mediante esta clase se definen de forma unificada los distintos controladores para la
gestión de sistemas de gestión de bases de datos específicos.

Esta clase no debería utilizarse directamente, se recomienda utilizar FLSqlDatabase.

@author InfoSiAL S.L.
*/
class FL_EXPORT FLSqlDriver : public QSqlDriver {

  Q_OBJECT

public:

  /**
  constructor
  */
  FLSqlDriver( QObject * parent = 0, const char * name = 0 );

  /**
  destructor
  */
  ~FLSqlDriver();

  /**
  Obtiene el nombre de la base de datos formateado correctamente para realizar una conexión

  @param name Nombre de la base de datos
  @return Cadena con el nombre debidamente formateado
  */
  virtual QString formatDatabaseName( const QString & name );

  /**
  Intentar realizar una conexión a una base de datos.

  Si la base de datos no existe intenta crearla.

  @param database Nombre de la base de datos a la que conectar
  @param user  Usuario
  @param password Contraseña
  @param host  Servidor de la base de datos
  @param port  Puerto TCP de conexión
  @return True si la conexión tuvo éxito, false en caso contrario
  */
  virtual bool tryConnect( const QString & db, const QString & user = QString::null, const QString & password = QString::null, const QString & host = QString::null, int port = -1 );

  /**
  Sentencia SQL específica de la base de datos que soporta el controlador, necesaria para crear
  la tabla solicitada.

  @param tmd Metadatos con la descripción de la tabla que se desea crear
  @return Sentencia SQL debidamente formateada para el tipo de base de datos soportada por el controlador
  */
  virtual QString sqlCreateTable( FLTableMetaData * tmd );

  /** Ver FLSqlDatabase::formatValueLike() */
  virtual QString formatValueLike( int t, const QVariant & v, const bool upper = false );
  /** Ver FLSqlDatabase::formatValue() */
  virtual QString formatValue( int t, const QVariant & v, const bool upper = false );
  /** Ver FLSqlDatabase::nextSerialVal() */
  virtual QVariant nextSerialVal( const QString & table, const QString & field );
  /** Ver FLSqlDatabase::atFrom() */
  virtual int atFrom( FLSqlCursor * cur );
  /** Ver FLSqlDatabase::alterTable() */
  virtual bool alterTable( const QString & mtd1, const QString & mtd2, const QString & key = QString::null );
  /** Ver FLSqlDatabase::canSavePoint() */
  virtual bool canSavePoint();
  /** Ver FLSqlDatabase::savePoint() */
  virtual bool savePoint( const QString & n );
  /** Ver FLSqlDatabase::releaseSavePoint() */
  virtual bool releaseSavePoint( const QString & n );
  /** Ver FLSqlDatabase::rollbackSavePoint() */
  virtual bool rollbackSavePoint( const QString & n );
  /** Ver FLSqlDatabase::Mr_Proper() */
  virtual void Mr_Proper() {}
  /** Ver FLSqlDatabase::locksStatus() */
  virtual QStringList locksStatus();
  /** Ver FLSqlDatabase::detectLocks() */
  virtual QStringList detectLocks();
  /** Ver FLSqlDatabase::detectRisksLocks() */
  virtual QStringList detectRisksLocks( const QString & table = QString::null,
                                        const QString & primaryKeyValue = QString::null );
  /** Ver FLSqlDatabase::regenTable() */
  virtual bool regenTable( const QString & n, FLTableMetaData * tmd );

  /**
  Informa al driver de la base de datos que lo utiliza
  */
  void setFLSqlDatabase( FLSqlDatabase * db );

protected:

  FLSqlDatabase * db_;
};

#endif
