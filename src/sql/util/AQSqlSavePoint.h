/***************************************************************************
AQSqlSavePoint.h
-------------------
 begin                : 21/01/2009
 copyright            : (C) 2003-2009 by InfoSiAL S.L.
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

#ifndef AQSQLSAVEPOINT_H_
#define AQSQLSAVEPOINT_H_

#include <QString>

class QSqlRecord;
class AQSqlCursor;
class AQSqlSavePointPrivate;

/**
Punto de salvaguarda de un conjunto de operaciones básicas
sobre cursores (insertar, editar y borrar).
 
Mediante esta clase se puede guardar un grupo de operaciones básicas
sobre cursores (insertar, editar y borrar).
Deshacer un punto de salvaguarda, significa que todas las operaciones
almacenadas son canceladas realizando las acciones necesarias para que
no tengan efecto.
 
Para el correcto funcionamiento hay que ir guardando los buffer's (QSqlRecord)
con el contenido de los registros a modificar o modificados por una operación,
indicando el nombre de la clave primaria y el cursor al que pertenece.
 
Ejemplo:
    \code
    AQSqlCursor cur( "articulos" );
    AQSqlSavePoint savePoint();
 
    QSqlRecord * buffer = cur.primeInsert();
    buffer->setValue( "id",    53981 );
    buffer->setValue( "name",  "Thingy" );
    buffer->setValue( "price", 105.75 );
    cur.insert();
    savePoint.saveInsert( "id", buffer, &cur );
 
    cur.first();
    buffer = cur.primeUpdate();
    savePoint.saveEdit( "id", buffer, &cur );
    buffer->setValue( "name",  "Pepe" );
    cur.update();
 
    cur.last();
    buffer = cur.primeDelete();
    savePoint.saveDel( "id", buffer, &cur );
    cur.del();
 
    savePoint.undo(); // Deshace todas las operaciones anteriores
    \endcode
 
@author InfoSiAL S.L.
*/
class AQSqlSavePoint
{

public:

  /**
  constructor.

	@param connectionName Nombre de la conexion
  */
  AQSqlSavePoint( const QString & connectionName = "default" );

  /**
  destructor.
  */
  ~AQSqlSavePoint();

  /**
  Limpia el punto de salvaguarda.

  Todas las operaciones almacenadas son eliminadas, por lo tanto, despues de
  invocar a este método ya no se podrán deshacer.
  */
  void clear();

  /**
  Deshace el punto de salvaguarda.
  */
  void undo();

  /**
  Guarda el buffer con el contenido del registro insertado.

  @param primaryKey Nombre del campo que es clave primaria.
  @param buffer buffer con el contenido del registro.
  @param cursor Cursor asociado.
  */
  void saveInsert( const QString & primaryKey, QSqlRecord * buffer, AQSqlCursor * cursor );

  /**
  Guarda el buffer con el contenido del registro a editar.

  @param primaryKey Nombre del campo que es clave primaria.
  @param buffer buffer con el contenido del registro.
  @param cursor Cursor asociado.
  */
  void saveEdit( const QString & primaryKey, QSqlRecord * buffer, AQSqlCursor * cursor );

  /**
  Guarda el buffer con el contenido del registro a borrar.

  @param primaryKey Nombre del campo que es clave primaria.
  @param buffer buffer con el contenido del registro.
  @param cursor Cursor asociado.
  */
  void saveDel( const QString & primaryKey, QSqlRecord * buffer, AQSqlCursor * cursor );

#ifdef AQ_DEBUG
  static long countRefSavePoint;
#endif

private:

  /**
  Privado
  */
  AQSqlSavePointPrivate * d;
};

#endif /*AQSQLSAVEPOINT_H_*/
