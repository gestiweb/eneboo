/***************************************************************************
               AQSqlQuery.h
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

#ifndef AQSQLQUERY_H_
#define AQSQLQUERY_H_

#include <AQCore/AQCore.h>
#include <AQSql/AQSql.h>

#include <QSqlQuery>
#include <QVariant>
#include <QStringList>

class AQSqlDatabase;
class AQSqlQueryPrivate;

/**
Maneja consultas con características específicas para AbanQ, hereda de QSqlQuery.
 
Ofrece la funcionalidad para manejar consultas de QSqlQuery y además ofrece métodos
para trabajar con consultas parametrizadas y niveles de agrupamiento.
 
@author InfoSiAL S.L.
*/
class AQSqlQuery: public QSqlQuery
{
public:

  /**
  constructor
  */
  AQSqlQuery( const QString & connectionName = "default" );

  /**
  destructor
  */
  ~AQSqlQuery();

  /**
  Ejecuta la consulta
  */
  bool exec();

  /**
  Añade la descripción parámetro al diccionario de parámetros.

  @param p Objeto AQParameterQuery con la descripción del parámetro a añadir
  */
  void addParameter( AQParameterQuery * p ) const;

  /**
  Añade la descripción de un grupo al diccionario de grupos.

  @param g Objeto AQGroupByQuery con la descripción del grupo a añadir
  */
  void addGroup( AQGroupByQuery * g ) const;

  /**
  Para establecer el nombre de la consulta.

  @param n Nombre de la consulta
  */
  void setName( const QString & n ) const;

  /**
  Para obtener el nombre de la consulta
  */
  QString name() const;

  /**
  Para obtener la parte SELECT de la sentencia SQL de la consulta
  */
  QString select() const;

  /**
  Para obtener la parte FROM de la sentencia SQL de la consulta
  */
  QString from() const;

  /**
  Para obtener la parte WHERE de la sentencia SQL de la consulta
  */
  QString where() const;

  /**
  Para obtener la parte ORDER BY de la sentencia SQL de la consulta
  */
  QString orderBy() const;

  /**
  Para establecer la parte SELECT de la sentencia SQL de la consulta.

  @param  s Cadena de texto con la parte SELECT de la sentencia SQL que
            genera la consulta. Esta cadena NO debe incluir la palabra reservada
            SELECT, ni tampoco el caracter '*' como comodín. Solo admite la lista
            de campos que deben aparecer en la consulta separados por comas
  */
  void setSelect( const QString & s ) const;

  /**
  Para establecer la parte FROM de la sentencia SQL de la consulta.

  @param  f Cadena de texto con la parte FROM de la sentencia SQL que
            genera la consulta
  */
  void setFrom( const QString & f ) const;

  /**
  Para establecer la parte WHERE de la sentencia SQL de la consulta.

  @param  s Cadena de texto con la parte WHERE de la sentencia SQL que
            genera la consulta
  */
  void setWhere( const QString & w ) const;

  /**
  Para establecer la parte ORDER BY de la sentencia SQL de la consulta.

  @param  s Cadena de texto con la parte ORDER BY de la sentencia SQL que
            genera la consulta
  */
  void setOrderBy( const QString & w ) const;

  /**
  Para obtener la sentencia completa SQL de la consulta.

  Este método une las tres partes de la consulta (SELECT, FROM Y WHERE),
  sustituye los parámetros por el valor que tienen en el diccionario y devuelve
  todo en una cadena de texto.

  @return Cadena de texto con la sentencia completa SQL que genera la consulta
  */
  QString sql();

  /**
  Para obtener los parametros de la consulta.

  @return Diccionario de parámetros
  */
  AQParameterQryHash parameterHash() const;

  /**
  Para obtener los niveles de agrupamiento de la consulta.

  @return Diccionario de niveles de agrupamiento
  */
  AQGroupByQryHash groupHash() const;

  /**
  Para obtener la lista de nombres de los campos.

  @return Lista de cadenas de texto con los nombres de los cmapos de la consulta
  */
  QStringList fieldList() const;

  /**
  Asigna un diccionario de parámetros, al diccionario de parámetros de la consulta.

  El diccionario de parámetros del tipo AQGroupByQueryHash , ya construido,
  es asignado como el nuevo diccionario de grupos de la consulta, en el caso de que
  ya exista un diccionario de grupos, este es destruido y sobreescrito por el nuevo.
  El diccionario pasado a este método pasa a ser propiedad de la consulta, y ella es la
  encargada de borrarlo. Si el diccionario que se pretende asignar es nulo o vacío este
  método no hace nada.

  @param gd Diccionario de parámetros
  */
  void setGroupHash( const AQGroupByQryHash & gd ) const;

  /**
  Asigna un diccionario de grupos, al diccionario de grupos de la consulta.

  El diccionario de grupos del tipo AQParameterQueryHash , ya construido,
  es asignado como el nuevo diccionario de parámetros de la consulta, en el caso de que
  ya exista un diccionario de parámetros, este es destruido y sobreescrito por el nuevo.
  El diccionario pasado a este método pasa a ser propiedad de la consulta, y ella es la
  encargada de borrarlo. Si el diccionario que se pretende asignar es nulo o vacío este
  método no hace nada.

  @param pd Diccionario de parámetros
  */
  void setParameterHash( const AQParameterQryHash & pd ) const;

  /**
  Obtiene el valor de un campo de la consulta.

  Dado un nombre de un campo de la consulta, este método devuelve un objeto QVariant
  con el valor de dicho campo. El nombre debe corresponder con el que se coloco en
  la parte SELECT de la sentenica SQL de la consulta.

  @param n Nombre del campo de la consulta
  */
  QVariant value( const QString & n ) const;

  /**
  Reimplementacion de QSqlQuery::value(int)
  */
  QVariant value( int i ) const;

  /**
  Devuelve el nombre de campo, dada su posicion en la consulta.

  @param  p Posicion del campo en la consulta, empieza en cero y de izquierda a derecha
  @return Nombre del campo correspondiente. Si no existe el campo devuelve QString()
  */
  QString posToFieldName( int p ) const;

  /**
  Devuelve la posición de una campo en la consulta, dado su nombre.

  @param  n Nombre del campo
  @return Posicion del campo en la consulta. Si no existe el campo devuelve -1
  */
  int fieldNameToPos( const QString & n ) const;

  /**
  Para obtener la lista de nombres de las tablas de la consulta.

  @return Lista de nombres de las tablas que entran a formar parte de la
          consulta
  */
  QStringList tablesList() const;

  /**
  Establece la lista de nombres de las tablas de la consulta

  @param  tl  Cadena de texto con los nombres de las tablas
              separados por comas, p.e. "tabla1,tabla2,tabla3"
  */
  void setTablesList( const QString & tl ) const;

  /**
  Establece el valor de un parámetro.

  @param name Nombre del parámetro
  @param v    Valor para el parámetros
  */
  void setValueParam( const QString & name, const QVariant & v ) const;

  /**
  Obtiene el valor de un parámetro.

  @param name Nombre del parámetro.
  */
  QVariant valueParam( const QString & name ) const;

  /**
  Redefinicion del método size() de QSqlQuery
  */
  int size() const;

  /**
  Para obtener la lista de definiciones de campos de la consulta

  @return Objeto con la lista de deficiones de campos de la consulta
  */
  AQFieldMDHash fieldMetaDataList() const;

  /**
  Para obtener la base de datos sobre la que trabaja
  */
  AQSqlDatabase db() const;

#ifdef AQ_WARN
  static long countRefQuery;
#endif

private:

  /**
  Privado
  */
  AQSqlQueryPrivate * d;
};

#endif /*AQSQLQUERY_H_*/
