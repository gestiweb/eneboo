/***************************************************************************
                      AQSqlUtil.h
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

#ifndef AQSQLUTIL_H_
#define AQSQLUTIL_H_

#include <QString>
#include <QStringList>
#include <QVariant>

class AQSqlCursor;

/**
Clase con métodos, herramientas y utiles necesarios para ciertas operaciones.
 
Es esta clase se encontrarán métodos genéricos que
realizan operaciones muy específicas pero que
son necesarios para ciertos procesos habituales
en las distintas tareas a desempeñar en la gestión
empresarial.
 
@author InfoSiAL S.L.
*/
class AQSqlUtil
{

public:

  /**
  Obtiene la lista de nombres de campos de la tabla especificada.
  El primer string de la lista contiene el número de campos de la tabla

  @param table Nombre de la tabla
  @param connName Nombre de la conexion
  @return Lista de campos
  */
  static QStringList fieldsNames( const QString & table, const QString & connName = "default" );

  /**
  Este metodo devuelve el siguiente valor de un campo tipo contador de una tabla.

  Este metodo es muy util cuando se insertan registros en los que
  la referencia es secuencial y no nos acordamos de cual fue el ultimo
  numero usado. El valor devuelto es un QVariant del tipo de campo es
  el que se busca la ultima referencia. Lo más aconsejable es que el tipo
  del campo sea 'String' porque así se le puede dar formato y ser
  usado para generar un código de barras. De todas formas la función
  soporta tanto que el campo sea de tipo 'String' como de tipo 'double'.

  @param name Nombre del campo
  @param cursor_ Cursor a la tabla donde se encuentra el campo.
  @return Qvariant con el numero siguiente.
  */
  static QVariant nextCounter( const QString & name, AQSqlCursor * cursor_ );

  /**
  Lee el valor de un setting en la tabla flsettings

  @param key. Clave identificadora del setting
  @param connName Nombre de la conexion
  @return Valor del setting
  */
  static QVariant readDBSettingEntry( const QString & key, const QString & connName = "default" );

  /**
  Establece el valor de un setting en la tabla flsettings

  @param key. Clave identificadora del setting
  @param Valor del setting
  @param connName Nombre de la conexion
  @return Indicador de si la escritura del settings se realiza correctamente
  */
  static bool writeDBSettingEntry( const QString & key, const QString & value, const QString & connName = "default" );

  /**
  Ejecuta una query de tipo select, devolviendo los resultados del primer registro encontrado

  @param f: Sentencia from de la query 
  @param s: Sentencia select de la query, que será el nombre del campo a devolver
  @param w: Sentencia where de la query 
  @param tL: Sentencia tableslist de la query. Necesario cuando en la sentencia from se incluya más de una tabla
  @param size: Número de líneas encontradas. (-1 si hay error)
  @param connName Nombre de la conexion
  @return Valor resultante de la query o falso si no encuentra nada.
  */
  static QVariant sqlSelect( const QString & f, const QString & s, const QString & w,
                             const QString & tL = QString(), int * size = 0,
                             const QString & connName = "default" );

  /**
  Realiza la inserción de un registro en una tabla mediante un objeto AQSqlCursor

  @param t Nombre de la tabla
  @param fL Lista separada con comas de los nombres de los campos
  @param vL Lista separada con comas de los valores correspondientes
  @param connName Nombre de la conexion
  @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
  */
  static bool sqlInsert( const QString & t, const QString & fL, const QString & vL,
                         const QString & connName = "default" );

  /**
  Realiza la modificación de uno o más registros en una tabla mediante un objeto AQSqlCursor

  @param t Nombre de la tabla
  @param fL Lista separada con comas de los nombres de los campos
  @param vL Lista separada con comas de los valores correspondientes
  @param w Sentencia where para identificar los registros a editar.
  @param connName Nombre de la conexion
  @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
  */
  static bool sqlUpdate( const QString & t, const QString & fL, const QString & vL, const QString & w,
                         const QString & connName = "default" );

  /**
  Borra uno o más registros en una tabla mediante un objeto AQSqlCursor

  @param t Nombre de la tabla
  @param w Sentencia where para identificar los registros a borrar.
  @param connName Nombre de la conexion
  @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
  */
  static bool sqlDelete( const QString & t, const QString & w, const QString & connName = "default" );
};

#endif /*AQSQLUTIL_H_*/
