/***************************************************************************
AQAction.cpp
-------------------
begin                : 29/01/2009
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

#include <QTimer>
#include <QSqlField>

#include <AQCore/AQActionMD.h>
#include <AQCore/AQTableMD.h>
#include <AQCore/AQFieldMD.h>
#include <AQCore/AQRelationMD.h>
#include <AQSql/AQSqlCursor.h>
#include <AQSql/AQManager.h>
#include <AQSql/AQSqlDatabase.h>
#include <AQSql/AQSqlConnections.h>

#include "AQAction.h"
#include "AQAction_p.h"
#include "util/AQUiUtil.h"
#include "mainwindow/AQFormDB.h"

#ifdef AQ_WARN
long AQAction::countRefAction = 0;
#endif

AQActionPrivate::AQActionPrivate() :
    AQObjectPrivate(),
    actMaster_( 0 ),
    relMasterToDetail_( 0 ),
    action_( 0 ),
    cursor_( 0 ),
    cursorField_( 0 ),
    modal_( false )
{
#ifdef AQ_WARN
  ++AQAction::countRefAction;
#endif
}

AQActionPrivate::~AQActionPrivate()
{
  delete relMasterToDetail_;

#ifdef AQ_WARN
  --AQAction::countRefAction;
#endif
}

void AQActionPrivate::init()
{
  AQ_Q( AQAction );

  action_ = AQSqlConnections::database( connName_ ).
            manager() ->action( actName_.isEmpty() ? "aqvoid" : actName_ );

  Q_ASSERT( action_ );

  if ( !actMaster_ || !relMasterToDetail_ || !actMaster_->cursor() ) {
    cursor_ = new AQSqlCursor( action_->table(), true, connName_, 0, 0, q );
    cursor_->setAQActionHandler( q );
    return ;
  }

  QString table_( relMasterToDetail_->foreignTable() );
  QString fieldRelation_( relMasterToDetail_->foreignField() );
  QString foreignField_( relMasterToDetail_->field() );

  const AQTableMD * mtdMaster = actMaster_->cursor() ->metadata();
  const AQTableMD * mtdDetail = AQSqlConnections::database( connName_ ).manager() ->metadata( table_ );

  AQRelationMD * relMasterToDetail = mtdMaster ? mtdMaster->relation( foreignField_, fieldRelation_, table_ ) : 0;
  AQRelationMD * relDetailToMaster = mtdDetail ? mtdDetail->relation( fieldRelation_, foreignField_, mtdMaster->name() ) : 0;

  bool checkIntegrity = false;

  if ( !relMasterToDetail && mtdMaster ) {

    if ( relDetailToMaster )
      checkIntegrity = ( relDetailToMaster->cardinality() == AQCore::RELATION_M1 );

    AQFieldMD * fieldMaster = mtdMaster->field( foreignField_ );

    if ( fieldMaster ) {

      relMasterToDetail = new AQRelationMD( table_, fieldRelation_, AQCore::RELATION_1M, false, false, checkIntegrity );
      fieldMaster->addRelationMD( relMasterToDetail );

      aqDebug( q->tr( "AQAction : La relación entre la tabla maestra %1 y la tabla detalle %2 "
                      "no existe, pero sin embargo se han indicado los campos de relación( %3, %4 )" )
               .arg( mtdMaster->name() ).arg( table_ ).arg( fieldRelation_ ).arg( foreignField_ ) );

      aqDebug( q->tr( "AQAction : Creando automáticamente %1.%2 --1M--> %3.%4" )
               .arg( mtdMaster->name() ).arg( foreignField_ ).arg( table_ ).arg( fieldRelation_ ) );

    } else
      aqDebug( q->tr( "AQAction : El campo ( %1 ) de la tabla detalle ( %2 ) indicado en la propiedad foreignField,"
                      "no se encuentra en la tabla maestra ( %3 )" )
               .arg( foreignField_ ).arg( table_ ).arg( mtdMaster->name() ) );

  }

  if ( !relDetailToMaster && mtdDetail ) {

    AQFieldMD * fieldDetail = mtdDetail->field( fieldRelation_ );

    if ( fieldDetail ) {

      relDetailToMaster = new AQRelationMD( mtdMaster->name(), foreignField_, AQCore::RELATION_1M, false, false, false );
      fieldDetail->addRelationMD( relDetailToMaster );

      aqDebug( q->tr( "AQAction : Creando automáticamente %1.%2 --1M--> %3.%4" )
               .arg( table_ ).arg( fieldRelation_ ).arg( mtdMaster->name() ).arg( foreignField_ ) );

    } else
      aqDebug( q->tr( "AQAction : El campo ( %1 ) de la tabla maestra ( %2 ) indicado en la propiedad fieldRelation,"
                      "no se encuentra en la tabla detalle ( %3 )" )
               .arg( fieldRelation_ ).arg( mtdMaster->name() ).arg( table_ ) );

  }

  cursor_ = new AQSqlCursor( table_, true, connName_, actMaster_->cursor(), relDetailToMaster, q );
  cursor_->setAQActionHandler( q );
}

bool AQActionPrivate::showWarnings( int cursorModeAccess ) const
{
  AQ_Q( const AQAction );

  switch ( cursorModeAccess ) {

  case AQSql::DEL:
    if ( cursor_->isLocked() ) {
      AQ_MSGBOX_WARN( q->tr( "Aviso" ),
                      q->tr( "Registro bloqueado, no se puede eliminar" ) );
      return true;
    }

  case AQSql::INSERTCOPY:
  case AQSql::EDIT:
  case AQSql::BROWSE:
    if ( !cursor_->isValid() || cursor_->size() <= 0 ) {
      AQ_MSGBOX_WARN( q->tr( "Aviso" ),
                      q->tr( "No hay ningún registro seleccionado" ) );
      return true;
    }

  };

  return false;
}

AQAction::AQAction( const QString & actionName,
                    const QString & connName,
                    bool doInit ) :
    AQObject( *new AQActionPrivate )
{
  AQ_D( AQAction );

  d->actName_ = actionName;
  d->connName_ = connName;

  if ( doInit )
    d->init();
}

AQAction::AQAction( AQSharedAction actMaster,
                    AQRelationMD * relMasterToDetail,
                    const QString & connName ) :
    AQObject( *new AQActionPrivate )
{
  AQ_D( AQAction );

  d->actMaster_ = actMaster;
  d->relMasterToDetail_ = relMasterToDetail;
  d->actName_ = relMasterToDetail->foreignTable();
  d->connName_ = connName;

  d->init();
}

AQSharedAction AQAction::create( const QString & actionName,
                                 const QString & connName )
{
  AQAction * act = new AQAction( actionName, connName );
  AQSharedAction shrAct( act );

  act->d_func() ->weakRef_ = shrAct.toWeakRef();

  return shrAct;
}

AQSharedAction AQAction::create( AQSharedAction actMaster,
                                 AQRelationMD * relMasterToDetail,
                                 const QString & connName )
{
  AQAction * act = new AQAction( actMaster, relMasterToDetail, connName );
  AQSharedAction shrAct( act );

  act->d_func() ->weakRef_ = shrAct.toWeakRef();

  return shrAct;
}

AQFormDB * AQAction::form()
{
  AQ_D( AQAction );

  AQFormDB * form = new AQFormDB;

  form->setAQAction( d->weakRef_.toStrongRef() );

  return form;
}

QVariant AQAction::exec( const QString & fieldName, const QString & actionNameUi )
{
  AQ_D( AQAction );

  Q_ASSERT( d->cursor_ );

  QVariant v;
  AQFormDB * form = new AQFormDB;
  AQActionMD * actBackup = d->action_;

  if ( !actionNameUi.isEmpty() ) {
    d->action_ = AQSqlConnections::database( d->connName_ ).manager() ->action( actionNameUi );

    Q_ASSERT( d->action_ );

    d->action_->setTable( actBackup->table() );
  }

  form->setAQAction( d->weakRef_.toStrongRef() );

  if ( !d->modal_ ) {
    QString searchField( fieldName.isEmpty() ? d->defaultSearchFieldName_ : fieldName );

    if ( searchField.isEmpty() )
      form->exec();
    else {

      if ( d->cursorField_ && searchField != d->defaultSearchFieldName_ ) {
        d->defaultSearchFieldName_ = searchField;

        QSqlIndex indexSort( d->cursor_->sort() );
        QSqlField f( d->cursor_->field( searchField ) );

        indexSort.remove( indexSort.indexOf( searchField ) );
        indexSort.insert( 0, f );

        d->cursor_->setSort( indexSort );
        d->cursor_->select( "" );

        syncPos( d->fieldName_ );
      }

      d->modal_ = true;
      v = form->exec( searchField );
      d->modal_ = false;
    }
  } else
    form->exec();

  d->action_ = actBackup;

  QTimer::singleShot( 0, form, SLOT( deleteLater() ) );

  return v;
}

AQActionMD * AQAction::action() const
{
  return d_func() ->action_;
}

AQSqlCursor * AQAction::cursor() const
{
  return d_func() ->cursor_;
}

AQSharedAction AQAction::aqActionMaster() const
{
  return d_func() ->actMaster_;
}

AQSharedAction AQAction::aqActionField( const QString & fieldName )
{
  AQ_D( AQAction );

  AQSharedAction actField = d->hashActionsField_.value( fieldName );

  if ( !actField ) {

    Q_ASSERT( d->cursor_ );

    const AQTableMD * mtd = d->cursor_->metadata();
    AQFieldMD * field = mtd->field( fieldName );

    if ( !field || !field->relationM1() ) {
      aqDebug( "AQAction : " + tr( "El campo %1 no puede tener una acción porque no se ha "
                                   "definido una relación M1 ( muchos a uno ) para él" ).arg( fieldName ) );
      return AQSharedAction();
    }

    AQFieldMD * fieldAssoc = field->associatedField();

    if ( fieldAssoc ) {

      AQSharedAction actAssoc = aqActionField( fieldAssoc->name() );

      if ( !actAssoc ) {
        aqDebug( "AQAction : " +
                 tr( "Para el campo %1 se ha indicado el campo asociado %2 "
                     "pero no se ha podido obtener la acción asociada" ).arg( fieldName ).arg( fieldAssoc->name() ) );
        return AQSharedAction();
      }

      AQRelationMD * relMasterToDetail = new AQRelationMD( field->relationM1() ->foreignTable(),
                                         field->associatedFieldFilterTo(), AQCore::RELATION_1M );
      relMasterToDetail->setField( fieldAssoc->name() );

      actField = create( actAssoc, relMasterToDetail, d->connName_ );
    } else
      actField = create( field->relationM1() ->foreignTable(), d->connName_ );

    QString searchField( field->relationM1() ->foreignField() );
    AQActionPrivate * df = actField->d_func();

    df->cursorField_ = d->cursor_;
    df->fieldName_ = fieldName;
    df->defaultSearchFieldName_ = searchField;

    d->hashActionsField_.insert( fieldName, actField );

    QSqlIndex indexSort( df->cursor_->sort() );
    QSqlField f( df->cursor_->field( searchField ) );

    indexSort.remove( indexSort.indexOf( searchField ) );
    indexSort.insert( 0, f );

    df->cursor_->setSort( indexSort );
    df->cursor_->select( "" );

    connect( df->cursorField_, SIGNAL( bufferChanged( const QString & ) ), actField.data(), SLOT( syncPos( const QString & ) ) );
    connect( df->cursorField_, SIGNAL( newBuffer() ), actField.data(), SLOT( syncPos() ) );

    actField->syncPos();
  }

  return actField;
}

QString AQAction::actName() const
{
  return d_func() ->actName_;
}

QString AQAction::connName() const
{
  return d_func() ->connName_;
}

void AQAction::activate( bool readonly )
{
  AQ_D( AQAction );

  if ( d->modal_ )
    Q_EMIT accepted();
  else if ( readonly )
    browseRecord();
  else
    editRecord();
}

void AQAction::insertRecord( const QString & actionNameUi )
{
  AQ_D( AQAction );

  Q_ASSERT( d->cursor_ );

  d->cursor_->setModeAccess( AQSql::INSERT );
  if ( d->cursor_->refreshBuffer() )
    exec( QString(), actionNameUi );
}

void AQAction::editRecord( const QString & actionNameUi )
{
  AQ_D( AQAction );

  Q_ASSERT( d->cursor_ );

  if ( d->showWarnings( AQSql::EDIT ) )
    return ;

  d->cursor_->setModeAccess( AQSql::EDIT );
  if ( d->cursor_->refreshBuffer() )
    exec( QString(), actionNameUi );
}

void AQAction::browseRecord( const QString & actionNameUi )
{
  AQ_D( AQAction );

  Q_ASSERT( d->cursor_ );

  if ( d->showWarnings( AQSql::BROWSE ) )
    return ;

  d->cursor_->setModeAccess( AQSql::BROWSE );
  if ( d->cursor_->refreshBuffer() )
    exec( QString(), actionNameUi );
}

void AQAction::copyRecord( const QString & actionNameUi )
{
  AQ_D( AQAction );

  Q_ASSERT( d->cursor_ );

  if ( d->showWarnings( AQSql::INSERTCOPY ) )
    return ;

  d->cursor_->setModeAccess( AQSql::INSERTCOPY );
  if ( d->cursor_->refreshBuffer() )
    exec( QString(), actionNameUi );
}

void AQAction::deleteRecord()
{
  AQ_D( AQAction );

  Q_ASSERT( d->cursor_ );

  if ( d->showWarnings( AQSql::DEL ) )
    return ;

  if ( AQ_No == AQ_MSGBOX_QUES( tr( "Borrar registro" ),
                                tr( "El registro activo será borrado. ¿ Está seguro ?" ) ) )
    return ;

  d->cursor_->setModeAccess( AQSql::DEL );
  d->cursor_->refreshBuffer();

  d->cursor_->db().db().transaction();

  if ( d->cursor_->commitBuffer() )
    d->cursor_->db().db().commit();
  else
    d->cursor_->db().db().rollback();
}

void AQAction::requestRecord()
{
  d_func() ->modal_ = true;
}

void AQAction::syncPos( const QString & fieldName )
{
  AQ_D( AQAction );

  if ( ( fieldName.isEmpty() || d->fieldName_ == fieldName ) && d->cursorField_ ) {
    d->cursor_->seek( d->cursor_->atFromBinarySearch( d->defaultSearchFieldName_,
                      d->cursorField_->valueBuffer( d->fieldName_ ).toString() ),
                      false, false );
  }
}

AQSharedAction AQAction::fromSqlCursor( AQSqlCursor * cur )
{
  Q_ASSERT( cur );

  AQAction * act = qobject_cast<AQAction *>( cur->aqActionHandler() );

  if ( act )
    return act->d_func() ->weakRef_.toStrongRef();

  QString actName_( cur->metadata() ->name() );
  QString connName_( cur->db().connectionName() );

  act = new AQAction( actName_, connName_, false );

  AQActionPrivate * const d = act->d_func();
  AQSharedAction shrAct( act );

  d->action_ = AQSqlConnections::database( connName_ ).
               manager() ->action( actName_.isEmpty() ? "aqvoid" : actName_ );

  Q_ASSERT( d->action_ );

  cur->setParent( act );
  cur->setAQActionHandler( act );
  d->cursor_ = cur;
  d->weakRef_ = shrAct.toWeakRef();

  return shrAct;
}
