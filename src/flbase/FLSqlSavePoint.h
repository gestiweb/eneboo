/***************************************************************************
                           FLSqlSavePoint.h
                          -------------------
 begin                : Fri Nov 26 2004
 copyright            : (C) 2004-2005 by InfoSiAL S.L.
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

#ifndef FLSQLSAVEPOINT_H
#define FLSQLSAVEPOINT_H

#include <qptrstack.h>

#include "FLSqlCursor.h"

/**
Información sobre una operación.

La información de una operación es;
la clave primaria,
operacion realizada (0 = insertar, 1 = editar, 2 = borrar),
buffer con el contenido del registro afectado por la operación,
posición del registro actual del cursor,
orden del cursor,
filtro del cursor,
nombre del cursor (de la tabla),
cursor asociado.
*/
class FL_EXPORT opInfo : public QObject {
  Q_OBJECT
public:
  opInfo() : QObject( 0 ) {}
  opInfo( const QString & pK, const int o, const QSqlRecord & b, const int a, const QSqlIndex & s, const QString & f, const QString & n, FLSqlCursor * c ) : QObject( 0 ), primaryKey( pK ), op( o ), buffer( b ), at( a ), sort( s ), filter( f ), name( n ), cursor( c ) {
    connect( c, SIGNAL( destroyed() ), this, SLOT( cursorDestroyed() ) );
  }
  ~opInfo() { }
  QString primaryKey;
  int op;
  QSqlRecord buffer;
  int at;
  QSqlIndex sort;
  QString filter;
  QString name;
  FLSqlCursor * cursor;
public slots:
  void cursorDestroyed() {
    cursor = 0;
  }
};

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
    FLSqlCursor cur( "articulos" );
    FLSqlSavePoint savePoint();

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
class FL_EXPORT FLSqlSavePoint {

public:

  /**
  constructor.

  @param id Identificador para el punto de salvaguarda.
  */
  FLSqlSavePoint( int id = 0 );

  /**
  destructor.
  */
  ~FLSqlSavePoint();

  /**
  Establece el identificador del punto de salvaguarda.
  */
  void setId( int id ) {
    id_ = id;
  }

  /**
  Obtiene el identificador del punto de salvaguarda.
  */
  int id() {
    return id_;
  }

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
  void saveInsert( const QString & primaryKey, QSqlRecord * buffer, FLSqlCursor * cursor );

  /**
  Guarda el buffer con el contenido del registro a editar.

  @param primaryKey Nombre del campo que es clave primaria.
  @param buffer buffer con el contenido del registro.
  @param cursor Cursor asociado.
  */
  void saveEdit( const QString & primaryKey, QSqlRecord * buffer, FLSqlCursor * cursor );

  /**
  Guarda el buffer con el contenido del registro a borrar.

  @param primaryKey Nombre del campo que es clave primaria.
  @param buffer buffer con el contenido del registro.
  @param cursor Cursor asociado.
  */
  void saveDel( const QString & primaryKey, QSqlRecord * buffer, FLSqlCursor * cursor );

#ifdef FL_DEBUG
  static long countRefSavePoint;
#endif

private:

  /**
  Deshace una operacion de insertar.

  @param opInf Información de la operación.
  */
  void undoInsert( const opInfo * opInf );

  /**
  Deshace una operacion de editar.

  @param opInf Información de la operación.
  */
  void undoEdit( const opInfo * opInf );

  /**
  Deshace una operacion de borrar.

  @param opInf Información de la operación.
  */
  void undoDel( const opInfo * opInf );

  /**
  Pila para almacenar informacion de las operaciones.
  */
  QPtrStack < opInfo > * opInfos;

  /**
  Identificador del punto de salvaguarda
  */
  int id_;
};

#endif
