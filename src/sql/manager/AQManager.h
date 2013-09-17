/***************************************************************************
AQManager.h
-------------------
begin                : 09/08/2007
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

#ifndef AQMANAGER_H_
#define AQMANAGER_H_

#include <QVariant>
#include <QString>
#include <QCache>
#include <QStringList>

class AQManagerPrivate;
class AQTableMD;
class AQFieldMD;
class AQActionMD;
class AQSqlQuery;
class QDomElement;
class AQSqlDatabase;

/**
Esta clase sirve como administrador de la base de datos.
 
@author InfoSiAL S.L.
*/
class AQManager
{
public:

  /**
  Para obtener definicion de una tabla de la base de datos, a partir de un fichero XML.

  El nombre de la tabla corresponde con el nombre del fichero mas la extensión ".mtd"
  que contiene en XML la descripción de la tablas. Este método escanea el fichero
  y construye/devuelve el objeto AQTableMD correspondiente, además
  realiza una copia de estos metadatos en una tabla de la misma base de datos
  para poder determinar cuando ha sido modificados y así, si es necesario, reconstruir
  la tabla para que se adapte a la nuevos metadatos.

  @param  n     Nombre de la tabla de la base de datos de la que obtener los metadatos
  @param  quick Si TRUE no realiza chequeos, usar con cuidado
  @return Un objeto AQTableMD con los metadatos de la tabla solicitada
  */
  AQTableMD * metadata( const QString & n, bool quick = false ) const;

  /**
  Crea un objeto AQTableMD a partir de un elemento XML.

  Dado un elemento XML, que contiene la descripción de una
  tablas, construye y devuelve el objeto AQTableMD correspondiente.

  @param  mtd   Elemento XML con la descripción de la tabla
  @param  quick Si TRUE no realiza chequeos, usar con cuidado
  @return Objeto AQTableMD que contiene la descrición de la relación
  */
  AQTableMD * metadata( QDomElement * mtd, bool quick = false ) const;

  /**
  Para obtener una consulta de la base de datos, a partir de un fichero XML.

  El nombre de la consulta corresponde con el nombre del fichero mas la extensión ".qry"
  que contiene en XML la descripción de la consulta. Este método escanea el fichero
  y construye/devuelve el objeto AQSqlQuery.

  @param  n Nombre de la consulta de la base de datos que se quiere obtener
  */
  AQSqlQuery * query( const QString & n ) const;

  /**
  Obtiene la definición de una acción a partir de su nombre.

  Este método busca en los [id_modulo].xml la acción que se le pasa
  como nombre y construye y devuelve el objeto AQActionMD correspondiente.

  @param  n Nombre de la accion
  @return Un objeto AQActionMD con la descripcion de la accion
  */
  AQActionMD * action( const QString & n ) const;

  /**
  Comprueba si existe la tabla especificada en la base de datos.

  @param  n Nombre de la tabla que se quiere comprobar si existe
  @return TRUE si existe la tabla, FALSE en caso contrario
  */
  bool existsTable( const QString & n ) const;

  /**
  Comprueba si los metadatos definidos actualmente para una tabla en un fichero .mtd,
  difieren de los que tiene actualmente esa tabla en la base de datos (difieren con
  la copia que se tiene actualmente de ellos).

  Cuando se realizan modificaciones en la definición
  de una tabla, es probable que esa tabla ya exista en la base de datos. Con este
  método podemos determinar posteriormente si lo metadatos definidos actualmente
  difieren de los de la tabla existente en la base de datos.

  @param  n Nombre de la tabla a comprobar
  @return TRUE si los metadatos que hemos definido en AQManager::metadata()
          para una tabla (nuevos) son iguales a los metadatos de la tabla que tenemos
          en la base de datos (viejos), y FALSE en caso contrario
  */
  bool checkMetaData( const QString & n ) const;

  /**
  Esta función es esencialmente igual a la anterior, se proporciona por conveniencia.

  Compara los metadatos de dos tablas,  la definición en XML de esas dos tablas se
  pasan como dos cadenas de caracteres.

  @param  mtd1  Cadena de caracteres con XML que describe la primera tabla
  @param  mtd2  Cadena de caracteres con XML que describe la primera tabla
  @return TRUE si las dos descripciones son iguales, y FALSE en caso contrario
  */
  bool checkMetaData( const QString & mtd1, const QString & mtd2 ) const;

  /**
  Esta función es esencialmente igual a la anterior, se proporciona por conveniencia.
  */
  bool checkMetaData( AQTableMD * tmd1, AQTableMD * tmd2 ) const;

  /**
  Crea una tabla en la base de datos.

  @param  n Nombre de la tabla que se quiere crear
  @return Un objeto AQTableMD con los metadatos de la tabla que se ha creado, o
          0 si no se pudo crear la tabla o ya existía
  */
  AQTableMD * createTable( const QString & n ) const;

  /**
  Crea una tabla en la base de datos.

  @param  tmd Metadatos de la tabla
  @return Un objeto AQTableMD con los metadatos de la tabla que se ha creado, o
          0 si no se pudo crear la tabla o ya existía
  */
  AQTableMD * createTable( AQTableMD * tmd ) const;

  /**
  Devuelve el contenido del valor de de un campo formateado para ser reconocido
  por la base de datos actual en condiciones LIKE, dentro de la clausura WHERE de SQL.

  Este método toma como parametros los metadatos del campo definidos con
  AQFieldMD. Además de TRUE y FALSE como posibles valores de un campo
  lógico también acepta los valores Sí y No (o su traducción al idioma correspondiente).
  Las fechas son adaptadas al forma AAAA-MM-DD, que es el formato reconocido por PostgreSQL .

  @param  fMD   Objeto AQFieldMD que describre los metadatos para el campo
  @param  v     Valor que se quiere formatear para el campo indicado
  @param  upper Si TRUE convierte a mayúsculas el valor (si es de tipo cadena)
  */
  QString formatValueLike( AQFieldMD * fMD, const QVariant & v, bool upper = false ) const;
  QString formatAssignValueLike( AQFieldMD * fMD, const QVariant & v, bool upper = false ) const;
  QString formatAssignValueLike( const QString & fieldName, AQFieldMD * fMD, const QVariant & v,
                                 bool upper = false ) const;
  QString formatAssignValueLike( const QString & fieldName, int t, const QVariant & v,
                                 bool upper = false ) const;

  /**
  Este método hace lo mismo que el anterior, y se suministra por conveniencia.

  @param  t     Tipo de datos del valor
  @param  v     Valor que se quiere formatear para el campo indicado
  @param  upper Si TRUE convierte a mayúsculas el valor (si es de tipo cadena)
  */
  QString formatValueLike( int t, const QVariant & v, bool upper = false ) const;

  /**
  Devuelve el contenido del valor de de un campo formateado para ser reconocido
  por la base de datos actual, dentro de la clausura WHERE de SQL.

  Este método toma como parametros los metadatos del campo definidos con
  AQFieldMD. Además de TRUE y FALSE como posibles valores de un campo
  lógico también acepta los valores Sí y No (o su traducción al idioma correspondiente).
  Las fechas son adaptadas al forma AAAA-MM-DD, que es el formato reconocido por PostgreSQL .

  @param  fMD   Objeto AQFieldMD que describre los metadatos para el campo
  @param  v     Valor que se quiere formatear para el campo indicado
  @param  upper Si TRUE convierte a mayúsculas el valor (si es de tipo cadena)
  */
  QString formatValue( AQFieldMD * fMD, const QVariant & v, bool upper = false ) const;
  QString formatAssignValue( AQFieldMD * fMD, const QVariant & v, bool upper = false ) const;
  QString formatAssignValue( const QString & fieldName, AQFieldMD * fMD, const QVariant & v,
                             bool upper = false ) const;
  QString formatAssignValue( const QString & fieldName, int t, const QVariant & v,
                             bool upper = false ) const;

  /**
  Este método hace lo mismo que el anterior, y se suministra por conveniencia.

  @param  t     Tipo de datos del valor
  @param  v     Valor que se quiere formatear para el campo indicado
  @param  upper Si TRUE convierte a mayúsculas el valor (si es de tipo cadena)
  */
  QString formatValue( int t, const QVariant & v, bool upper = false ) const;

  /**
  Crea una tabla del sistema.

  Este método lee directamente de disco el fichero con la descripción de una tabla
  del sistema y la crea en la base de datos. Su uso normal es para inicializar
  el sistema con tablas iniciales.

  @param  n Nombre de la tabla.
  @return Un objeto AQTableMD con los metadatos de la tabla que se ha creado, o
          0 si no se pudo crear la tabla o ya existía
  */
  AQTableMD * createSystemTable( const QString & n ) const;

  /**
  Limpieza la tabla de metadatos, actualiza el cotenido xml con el de los ficheros .mtd
  actualmente cargados
  */
  void cleanupMetaData();

  /**
  Para saber si la tabla dada es una tabla de sistema.

  @param  n Nombre de la tabla.
  @return TRUE si es una tabla de sistema
  */
  bool isSystemTable( const QString & n ) const;

  /**
  Utilizado para almacenar valores grandes de campos en tablas separadas indexadas
  por claves SHA del contenido del valor.

  Se utiliza para optimizar consultas que incluyen campos con valores grandes,
  como por ejemplo imágenes, para manejar en las consulta SQL la referencia al valor
  que es de tamaño constante en vez del valor en sí. Esto disminuye el tráfico al
  reducir considerablemente el tamaño de los registros obtenidos.

  Las consultas pueden utilizar una referencia y obtener su valor sólo cuando se
  necesita mediante FLManager::fetchLargeValue().

  @param  mtd         Metadatos de la tabla que contiene el campo
  @param  largeValue  Valor de gran tamaño del campo
  @return Clave de referencia al valor
  */
  QString storeLargeValue( AQTableMD * mtd, const QString & largeValue ) const;

  /**
  Obtiene el valor de gran tamaño segun su clave de referencia.

  @param  refKey  Clave de referencia. Esta clave se suele obtener mediante FLManager::storeLargeValue
  @return Valor de gran tamaño almacenado
  */
  QVariant fetchLargeValue( const QString & refKey ) const;

  /**
  @return TRUE si el objeto se ha inicializado correctamente y está listo para trabajar
  */
  bool isReady() const;

  /**
  @return Clave global sha de los módulos cargados en la antigua estructura de tablas 
  de sistema de FacturaLUX. Si no existe la antigua estructura devuelve una cadena
  vacía.
  */
  QString shaGlobalFL() const;

  /**
  @return Clave global sha de módulos de la antigua estructura de tablas de sistema de
  FacturaLUX, correspondiente a la última actualización realizada de dichos módulos a
  la nueva estructura de tablas.
  */
  QString shaLastUpdateFL() const;

private:

  /**
  constructor
  */
  inline AQManager()
  {
    d = 0;
  }

  /**
  constructores de copia
  */
  inline AQManager( const AQManager & other )
  {
    d = other.d;
  }
  inline AQManager & operator=( const AQManager & other )
  {
    d = other.d;
    return *this;
  }

  /**
  destructor
  */
  inline ~AQManager()
  {}

  /**
  Acciones de inicialización.

  @return TRUE si la inicialización tuvo éxito
  */
  bool init( const AQSqlDatabase & db );

  /**
  Acciones de finalización.
  */
  void finish();

  /**
  Privado
  */
  AQManagerPrivate * d;

  friend class AQSqlDatabase;
  friend class AQSqlDatabasePrivate;
};

inline bool AQManager::isReady() const
{
  return !!d;
}

#endif /*AQMANAGER_H_*/
