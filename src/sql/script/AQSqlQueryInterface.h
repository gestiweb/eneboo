/***************************************************************************
AQSqlQueryInterface.h
-------------------
begin                : 11/10/2007
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

#ifndef AQSQLQUERYINTERFACE_H_
#define AQSQLQUERYINTERFACE_H_

#include <AQScript/AQObjectInterfaceQSA.h>

class AQSqlQuery;

/**
Interfaz para AQSqlQuery
*/
class AQSqlQueryInterface : public AQObjectInterfaceQSA
{
  Q_OBJECT

  Q_PROPERTY( QString select READ select WRITE setSelect )
  Q_PROPERTY( QString from READ from WRITE setFrom )
  Q_PROPERTY( QString where READ where WRITE setWhere )
  Q_PROPERTY( QString orderBy READ orderBy WRITE setOrderBy )
  Q_PROPERTY( QString tablesList READ tablesList WRITE setTablesList )
  Q_PROPERTY( int size READ size )
  Q_PROPERTY( bool valid READ isValid )
  Q_PROPERTY( bool undefined READ isUndefined )

public:

  AQSqlQuery * obj_;

public Q_SLOTS:

  /**
  Establece la sentencia completa SQL de la consulta.

  Este método une las tres partes de la consulta (SELECT, FROM Y WHERE),
  sustituye los parámetros por el valor que tienen en el diccionario y devuelve
  todo en una cadena de texto.

  @return Cadena de texto con la sentencia completa SQL que genera la consulta
  */
  QString sql() const;

  /**
  Reimplementación de QSqlQuery::value(int)
  */
  QVariant value( int i ) const;
  QVariant value( const QString & n ) const;

  /**
  Devuelve si el contenido de un campo de la consulta es nulo.

  @param i Posicion del campo en el buffer
  */
  bool isNull( int i ) const;

  /**
  Devuelve el nombre de campo, dada su posicion en la consulta.

  @param  p Posicion del campo en la consulta, empieza en cero y de izquierda
            a derecha
  @return Nombre del campo correspondiente. Si no existe el campo devuelve una cadena vacía
  */
  QString posToFieldName( int p ) const;

  /**
  Devuelve la posición de una campo en la consulta, dado su nombre.

  @param  n Nombre del campo
  @return Posicion del campo en la consulta. Si no existe el campo devuelve -1
  */
  int fieldNameToPos( const QString & n ) const;

  /**
  Obtiene el valor de un parámetro.

  @param name Nombre del parámetro.
  */
  QVariant valueParam( const QString & name ) const;

  /**
  Establece el valor de un parámetro.

  @param  name  Nombre del parámetro
  @param  v     Valor para el parámetros
  */
  void setValueParam( const QString & name, const QVariant & v ) const;

  /**
  Recupera el siguiente registro del resultado de la consulta asociada con el cursor

  @return TRUE si existe un siguiente registro, FALSE si no existe.
  */
  bool next() const;

  /**
  Recupera el anterior registro del resultado de la consulta asociada con el cursor

  @return TRUE si existe un anterior registro, FALSE si no existe.
  */
  bool prev() const;

  /**
  Recupera el primer registro del resultado de la consulta asociada con el cursor

  @return TRUE si existe un primer registro, FALSE si no existe.
  */
  bool first() const;

  /**
  Recupera el último registro del resultado de la consulta asociada con el cursor

  @return TRUE si existe un último registro, FALSE si no existe.
  */
  bool last() const;

  /**
  Ejecuta la consulta
  */
  bool exec() const;

protected Q_SLOTS:

  /**
  @return Si la interfaz es indefinida, es decir, no tiene objeto subyacente
  */
  bool isUndefined() const;

  /**
  Obtiene la parte SELECT de la sentencia SQL de la consulta
  */
  QString select() const;

  /**
  Obtiene la parte FROM de la sentencia SQL de la consulta
  */
  QString from() const;

  /**
  Obtiene la parte WHERE de la sentencia SQL de la consulta
  */
  QString where() const;

  /**
  Obtiene la parte ORDER BY de la sentencia SQL de la consulta
  */
  QString orderBy() const;

  /**
  Para obtener la lista de nombres de las tablas de la consulta.

  @return Lista de nombres de las tablas que entran a formar parte de la
          consulta. Lista de nombres separados por comas
  */
  QString tablesList() const;

  /**
  Establece la parte SELECT de la sentencia SQL de la consulta.

  @param  s Cadena de texto con la parte SELECT de la sentencia SQL que
            genera la consulta. Esta cadena NO debe incluir la palabra reservada
            SELECT, ni tampoco el caracter '*' como comodín. Solo admite la lista
            de campos que deben aparecer en la consulta separados por comas
  */
  void setSelect( const QString & s ) const;

  /**
  Establece la parte FROM de la sentencia SQL de la consulta.

  @param  f Cadena de texto con la parte FROM de la sentencia SQL que
            genera la consulta
  */
  void setFrom( const QString & f ) const;

  /**
  Establece la parte WHERE de la sentencia SQL de la consulta.

  @param  w Cadena de texto con la parte WHERE de la sentencia SQL que
            genera la consulta
  */
  void setWhere( const QString & w ) const;

  /**
  Establece la parte ORDER BY de la sentencia SQL de la consulta.

  @param oB Cadena de texto con la parte ORDER BY de la sentencia SQL que
            genera la consulta
  */
  void setOrderBy( const QString & oB ) const;

  /**
  Establece la lista de nombres de las tablas de la consulta

  @param  tl  Cadena de texto con los nombres de las tablas
            separados por comas, p.e. "tabla1,tabla2,tabla3"
  */
  void setTablesList( const QString & tl ) const;

  /**
  Devuelve el número de líneas que devuelve la consulta

  @return Número de líneas (-1 si la consulta no está activa o la BD no da información sobre número de filas)
  */
  int size() const;

  /**
  QSqlQuery::isValid()
  */
  bool isValid() const;

protected:

  friend class AQSqlObjectFactoryQSA;

  /**
   Constructor
   */
  AQSqlQueryInterface( const AQScriptEngineQSA * eng,
                       const QString & n = QString(),
                       const QString & connName = "default" );

  /**
  Destructor
  */
  virtual ~AQSqlQueryInterface();
};

inline bool AQSqlQueryInterface::isUndefined() const
{
  return !obj_;
}


/**
Interfaz de soporte para AbanQ v2
*/
class AQSqlQueryInterfaceAQ2 : public AQSqlQueryInterface
{
  Q_OBJECT

public:

  AQSqlQueryInterfaceAQ2( const AQScriptEngineQSA * eng,
                          const QString & n = QString(),
                          const QString & connName = "default" ) :
      AQSqlQueryInterface( eng, n, connName )
  {
    AQ_PORT_AQ2_NOP( FLSqlQuery, AQSqlQuery );
  }

public Q_SLOTS:

  void setForwardOnly ( bool );

  void showDebug() const;

  AQSqlQuery * obj() const;
};

#endif /*AQSQLQUERYINTERFACE_H_*/
