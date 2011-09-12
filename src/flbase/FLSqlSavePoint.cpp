/***************************************************************************
                          FLSqlSavePoint.cpp
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

#include "FLSqlSavePoint.h"
#include "FLSqlDatabase.h"

#ifdef FL_DEBUG
FL_EXPORT long FLSqlSavePoint::countRefSavePoint = 0;
#endif

FLSqlSavePoint::FLSqlSavePoint( int id ) : id_( id ), opInfos( 0 ) {
  opInfos = new QPtrStack < opInfo >;
  opInfos->setAutoDelete( true );

#ifdef FL_DEBUG
  countRefSavePoint++;
#endif
}

FLSqlSavePoint::~FLSqlSavePoint() {
  if ( opInfos ) {
    opInfos->clear();
    delete opInfos;
  }

#ifdef FL_DEBUG
  countRefSavePoint--;
#endif
}

void FLSqlSavePoint::clear() {
  opInfos->clear();
}

void FLSqlSavePoint::undo() {
  opInfo * opInf;
  while ( !opInfos->isEmpty() ) {
    opInf = opInfos->pop();
    switch ( opInf->op ) {
      case 0:
        undoInsert( opInf );
        break;
      case 1:
        undoEdit( opInf );
        break;
      case 2:
        undoDel( opInf );
        break;
    }
    delete opInf;
  }
  clear();
}

void FLSqlSavePoint::saveInsert( const QString & primaryKey, QSqlRecord * buffer, FLSqlCursor * cursor ) {
  if ( !cursor || !buffer )
    return ;
  opInfos->push( new opInfo( primaryKey, 0, QSqlRecord( *buffer ), cursor->at(), cursor->sort(), cursor->filter(),
                             static_cast<QSqlCursor *>( cursor ) ->name(), cursor ) );
}

void FLSqlSavePoint::saveEdit( const QString & primaryKey, QSqlRecord * buffer, FLSqlCursor * cursor ) {
  if ( !cursor || !buffer )
    return ;
  opInfos->push( new opInfo( primaryKey, 1, QSqlRecord( *buffer ), cursor->at(), cursor->sort(), cursor->filter(),
                             static_cast<QSqlCursor *>( cursor ) ->name(), cursor ) );
}

void FLSqlSavePoint::saveDel( const QString & primaryKey, QSqlRecord * buffer, FLSqlCursor * cursor ) {
  if ( !cursor || !buffer )
    return ;
  opInfos->push( new opInfo( primaryKey, 2, QSqlRecord( *buffer ), cursor->at(), cursor->sort(), cursor->filter(),
                             static_cast<QSqlCursor *>( cursor ) ->name(), cursor ) );
}

void FLSqlSavePoint::undoInsert( const opInfo * opInf ) {
  FLSqlCursor * cursor_ = opInf->cursor;
  bool owner = false;
  if ( !cursor_ ) {
    cursor_ = new FLSqlCursor( opInf->name );
    cursor_->setForwardOnly( true );
    owner = true;
  }
  if ( !cursor_ )
    return ;
  QString valuePrimaryKey = opInf->buffer.value( opInf->primaryKey ).toString();
  cursor_->QSqlCursor::select( opInf->primaryKey + "='" + valuePrimaryKey + "'" );
  if ( cursor_->QSqlCursor::next() ) {
    cursor_->QSqlCursor::primeDelete();
    cursor_->QSqlCursor::del();
  }
  if ( !owner ) {
    cursor_->QSqlCursor::select( opInf->filter, opInf->sort );
    cursor_->QSqlCursor::seek( opInf->at );
  } else
    delete cursor_;
}

void FLSqlSavePoint::undoEdit( const opInfo * opInf ) {
  FLSqlCursor * cursor_ = opInf->cursor;
  bool owner = false;
  if ( !cursor_ ) {
    cursor_ = new FLSqlCursor( opInf->name );
    cursor_->setForwardOnly( true );
    owner = true;
  }
  if ( !cursor_ )
    return ;
  QString valuePrimaryKey = opInf->buffer.value( opInf->primaryKey ).toString();
  cursor_->QSqlCursor::select( opInf->primaryKey + "='" + valuePrimaryKey + "'" );
  if ( cursor_->QSqlCursor::next() ) {
    QSqlRecord * buf = cursor_->QSqlCursor::primeUpdate();
    *buf = opInf->buffer;
    cursor_->QSqlCursor::update();
  }
  if ( !owner ) {
    cursor_->QSqlCursor::select( opInf->filter, opInf->sort );
    cursor_->QSqlCursor::seek( opInf->at );
  } else
    delete cursor_;
}

void FLSqlSavePoint::undoDel( const opInfo * opInf ) {
  FLSqlCursor * cursor_ = opInf->cursor;
  bool owner = false;
  if ( !cursor_ ) {
    cursor_ = new FLSqlCursor( opInf->name );
    owner = true;
  }
  if ( !cursor_ )
    return ;
  QSqlRecord * buf = cursor_->QSqlCursor::primeInsert();
  *buf = opInf->buffer;
  cursor_->QSqlCursor::insert();
  if ( !owner ) {
    cursor_->QSqlCursor::select( opInf->filter, opInf->sort );
    cursor_->QSqlCursor::seek( opInf->at );
  } else
    delete cursor_;
}
