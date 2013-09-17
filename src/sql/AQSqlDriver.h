/***************************************************************************
                        AQSqlDriver.h
                    -------------------
begin                : 07/08/2007
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

#ifndef AQSQLDRIVER_H_
#define AQSQLDRIVER_H_

#include <AQSql/AQSqlDatabase.h>

#include <QObject>
#include <QSqlDriver>
#include <QString>
#include <QVariant>

class AQTableMD;
class AQSqlCursor;

/**
Clase de abstracción para controladores de bases de datos.
 
Mediante esta clase se definen de forma unificada los distintos controladores para la
gestión de sistemas de gestión de bases de datos específicos.
 
Esta clase no debería utilizarse directamente, se recomienda utilizar AQSqlDatabase.
 
@author InfoSiAL S.L.
*/
class AQSqlDriver : public QSqlDriver
{
  Q_OBJECT

public:

  /**
  constructor
  */
  AQSqlDriver( QObject * parent = 0 );

  /**
  destructor
  */
  virtual ~AQSqlDriver();

  /**
  Obtiene el nombre de la base de datos formateado correctamente para realizar una conexión

  @param name Nombre de la base de datos
  @return Cadena con el nombre debidamente formateado
  */
  virtual QString formatDatabaseName( const QString & name ) const;

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
  virtual bool tryConnect( const QString & db, const QString & user = QString(),
                           const QString & password = QString(), const QString & host = QString(),
                           int port = -1 );

  /**
  Sentencia SQL específica de la base de datos que soporta el controlador, necesaria para crear
  la tabla solicitada.

  @param tmd Metadatos con la descripción de la tabla que se desea crear
  @return Sentencia SQL debidamente formateada para el tipo de base de datos soportada por el controlador
  */
  virtual QString sqlCreateTable( AQTableMD * ) const;

  /** Ver AQSqlDatabase::formatValueLike() */
  virtual QString formatValueLike( int, const QVariant & v, bool = false ) const;
  /** Ver AQSqlDatabase::formatValue() */
  virtual QString formatValue( int , const QVariant & v, bool = false ) const;
  /** Ver AQSqlDatabase::nextSerialVal() */
  virtual QVariant nextSerialVal( const QString & table, const QString & field ) const;
  /** Ver AQSqlDatabase::atFrom() */
  virtual int atFrom( AQSqlCursor * cur ) const;
  /** Ver AQSqlDatabase::canSavePoint() */
  virtual bool canSavePoint() const;
  /** Ver AQSqlDatabase::savePoint() */
  virtual bool savePoint( const QString & n ) const;
  /** Ver AQSqlDatabase::releaseSavePoint() */
  virtual bool releaseSavePoint( const QString & n ) const;
  /** Ver AQSqlDatabase::rollbackSavePoint() */
  virtual bool rollbackSavePoint( const QString & n ) const;
  /**
  Informa al driver de la base de datos que lo utiliza
  */
  virtual void setAQSqlDatabase( AQSqlDatabase db );
  virtual void finish();
  virtual void createIndex( const QString &, const QString &, bool = true ) const;

protected:

  AQSqlDatabase db_;
};

#endif /*AQSQLDRIVER_H_*/
