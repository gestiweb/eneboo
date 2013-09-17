/***************************************************************************
AQSqlSavePoint.cpp
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

#include <QStack>

#include <AQCore/AQTableMD.h>

#include "AQSqlSavePoint.h"
#include "../AQSqlCursor.h"
#include "../AQSqlDatabase.h"

#ifdef AQ_DEBUG
long AQSqlSavePoint::countRefSavePoint = 0;
#endif

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
class opInfo
{
public:

  opInfo() : cursor( 0 )
  {}

  opInfo( const QString & pK,
          int o, const QSqlRecord & b, int a,
          const QSqlIndex & s, const QString & f,
          const QString & n, AQSqlCursor * c ) :
      primaryKey( pK ), op( o ),
      buffer( b ), at( a ),
      sort( s ), filter( f ),
      name( n ), cursor( c )
  {}

  QString primaryKey;
  int op;
  QSqlRecord buffer;
  int at;
  QSqlIndex sort;
  QString filter;
  QString name;
  AQSqlCursor * cursor;
};

class AQSqlSavePointPrivate
{
public:

  /**
  constructor.

  @param connectionName Nombre de la conexion
   */
  AQSqlSavePointPrivate( const QString & connectionName ) :
      connectionName_( connectionName )
  {
#ifdef AQ_DEBUG
    ++AQSqlSavePoint::countRefSavePoint;
#endif
  }

	~AQSqlSavePointPrivate()
	{
#ifdef AQ_DEBUG
  --AQSqlSavePoint::countRefSavePoint;
#endif
	}
	
  /**
  Deshace una operacion de insertar.

  @param opInf Información de la operación.
  */
  void undoInsert( opInfo * opInf );

  /**
  Deshace una operacion de editar.

  @param opInf Información de la operación.
  */
  void undoEdit( opInfo * opInf );

  /**
  Deshace una operacion de borrar.

  @param opInf Información de la operación.
  */
  void undoDel( opInfo * opInf );

  /**
  Pila para almacenar informacion de las operaciones.
  */
  QStack<opInfo> opInfos;

  /**
  Nombre de la conexion de la base de datos a utilizar
  */
  QString connectionName_;
};

void AQSqlSavePointPrivate::undoInsert( opInfo * opInf )
{
  AQSqlCursor * cursor_ = opInf->cursor;
  bool owner = false;

  if ( !cursor_ ) {
    cursor_ = new AQSqlCursor( opInf->name, true, connectionName_ );
    owner = true;
  }

  cursor_->Q3SqlCursor::select(
    cursor_->db().manager()->formatAssignValue
    (
      cursor_->metadata()->field( opInf->primaryKey ),
      opInf->buffer.value( opInf->primaryKey )
    )
  );

  if ( cursor_->Q3SqlCursor::next() ) {
    cursor_->Q3SqlCursor::primeDelete();
    cursor_->Q3SqlCursor::del();
  }

  if ( !owner ) {
    cursor_->Q3SqlCursor::select( opInf->filter, opInf->sort );
    cursor_->Q3SqlCursor::seek( opInf->at );
  } else
    delete cursor_;
}

void AQSqlSavePointPrivate::undoEdit( opInfo * opInf )
{
  AQSqlCursor * cursor_ = opInf->cursor;
  bool owner = false;

  if ( !cursor_ ) {
    cursor_ = new AQSqlCursor( opInf->name, true, connectionName_ );
    owner = true;
  }

  cursor_->Q3SqlCursor::select(
    cursor_->db().manager()->formatAssignValue
    (
      cursor_->metadata()->field( opInf->primaryKey ),
      opInf->buffer.value( opInf->primaryKey )
    )
  );

  if ( cursor_->Q3SqlCursor::next() ) {
    QSqlRecord * buf = cursor_->Q3SqlCursor::primeUpdate();
    *buf = opInf->buffer;
    cursor_->Q3SqlCursor::update();
  }

  if ( !owner ) {
    cursor_->Q3SqlCursor::select( opInf->filter, opInf->sort );
    cursor_->Q3SqlCursor::seek( opInf->at );
  } else
    delete cursor_;
}

void AQSqlSavePointPrivate::undoDel( opInfo * opInf )
{
  AQSqlCursor * cursor_ = opInf->cursor;
  bool owner = false;

  if ( !cursor_ ) {
    cursor_ = new AQSqlCursor( opInf->name, true, connectionName_ );
    owner = true;
  }

  QSqlRecord * buf = cursor_->Q3SqlCursor::primeInsert();
  *buf = opInf->buffer;
  cursor_->Q3SqlCursor::insert();

  if ( !owner ) {
    cursor_->Q3SqlCursor::select( opInf->filter, opInf->sort );
    cursor_->Q3SqlCursor::seek( opInf->at );
  } else
    delete cursor_;
}

AQSqlSavePoint::AQSqlSavePoint( const QString & connectionName )
{
  d = new AQSqlSavePointPrivate( connectionName );
}

AQSqlSavePoint::~AQSqlSavePoint()
{
  delete d;
}

void AQSqlSavePoint::clear()
{
  d->opInfos.clear();
}

void AQSqlSavePoint::undo()
{
  opInfo opInf;
  while ( !d->opInfos.isEmpty() ) {
    opInf = d->opInfos.pop();
    switch ( opInf.op ) {
    case 0:
      d->undoInsert( &opInf );
      break;
    case 1:
      d->undoEdit( &opInf );
      break;
    case 2:
      d->undoDel( &opInf );
      break;
    }
  }
}

void AQSqlSavePoint::saveInsert( const QString & primaryKey, QSqlRecord * buffer, AQSqlCursor * cursor )
{
  Q_ASSERT( cursor );
  Q_ASSERT( buffer );

  d->opInfos.push( opInfo( primaryKey, 0, QSqlRecord( *buffer ),
                           cursor->at(), cursor->sort(), cursor->filter(),
                           static_cast<Q3SqlCursor *>( cursor ) ->name(), cursor
                         )
                 );
}

void AQSqlSavePoint::saveEdit( const QString & primaryKey, QSqlRecord * buffer, AQSqlCursor * cursor )
{
  Q_ASSERT( cursor );
  Q_ASSERT( buffer );

  d->opInfos.push( opInfo( primaryKey, 1, QSqlRecord( *buffer ),
                           cursor->at(), cursor->sort(), cursor->filter(),
                           static_cast<Q3SqlCursor *>( cursor ) ->name(), cursor
                         )
                 );
}

void AQSqlSavePoint::saveDel( const QString & primaryKey, QSqlRecord * buffer, AQSqlCursor * cursor )
{
  Q_ASSERT( cursor );
  Q_ASSERT( buffer );

  d->opInfos.push( opInfo( primaryKey, 2, QSqlRecord( *buffer ),
                           cursor->at(), cursor->sort(), cursor->filter(),
                           static_cast<Q3SqlCursor *>( cursor ) ->name(), cursor
                         )
                 );
}
