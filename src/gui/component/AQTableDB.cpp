/***************************************************************************
AQTableDB.cpp
-------------------
begin                : 30/08/2007
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

#include <QKeyEvent>
#include <QEvent>
#include <QTimer>
#include <QEvent>
#include <Q3SqlRecordInfo>
#include <QSqlDriver>

#include <AQCore/AQCore.h>
#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQTableMD.h>
#include <AQCore/AQRelationMD.h>
#include <AQCore/AQFieldMD.h>
#include <AQCore/AQActionMD.h>
#include <AQSql/AQSql.h>
#include <AQSql/AQSqlCursor.h>
#include <AQSql/AQSqlQuery.h>
#include <AQSql/AQSqlConnections.h>
#include <AQSql/AQSqlDatabase.h>
#include <AQSql/AQManager.h>
#include <AQUi/AQUiUtil.h>
#include <AQUi/AQApplication.h>

#include "AQTableDB.h"
#include "AQDataTable.h"
#include "../mainwindow/AQFormDB.h"
#include "../AQAction.h"

extern void qt_set_table_clipper_enabled( bool enabled );

AQTableDB::AQTableDB( QWidget * parent ) :
    AQComponentDB( parent ), sortField_( 0 ), timer( 0 ), tableRecords_( 0 ), firstRefresh_( true )
{
  setObjectName( "AQTableDB" );
  setFocusPolicy( Qt::StrongFocus );
  setFrameStyle( QFrame::StyledPanel );

  timer = new QTimer( this );
  connect( timer, SIGNAL( timeout() ), SLOT( refreshDelayed() ) );
}

void AQTableDB::setAQAction( AQSharedAction act )
{
  if ( act_ && !act ) {
    disconnect( this, 0, 0, 0 );
    if ( tableRecords_ )
      disconnect( tableRecords_, 0, this, 0 );
  }

  act_ = act;

  if ( tableRecords_ )
    tableRecords_->setAQAction( act_ );
}

AQSharedAction AQTableDB::aqAction() const
{
  return act_;
}

void AQTableDB::setupUi( AQFormDB * form )
{
  qt_set_table_clipper_enabled( false );
  AQComponentDB::setupUi( form );
}

void AQTableDB::linkUi()
{
  initAQAction();
}

void AQTableDB::initRefreshUi()
{
  firstRefresh_ = false;
}

AQTableDB::~AQTableDB()
{}

bool AQTableDB::eventFilter( QObject * obj, QEvent * ev )
{
  QEvent::Type evType = ev->type();

  if ( evType > AQCoreEvent::MinInternal ) {

    switch ( evType ) {
      case AQCoreEvent::InitNow: {
          if ( firstRefresh_ )
            return false;
          firstRefresh();
          refresh( true, true );
        }
        break;
    }

    return true;
  }

  if ( !tableRecords_ || !act_ )
    return QFrame::eventFilter( obj, ev );

  if ( evType == QEvent::KeyPress && obj == tableRecords_ ) {
    QKeyEvent * k = static_cast<QKeyEvent *> ( ev );

    if ( k->key() == Qt::Key_F2 ) {
      ui.comboBoxFieldToSearch->popup();
      return true;
    }
  }

  if ( evType == QEvent::KeyPress && obj == ui.lineEditSearch ) {
    QKeyEvent * k = static_cast<QKeyEvent *> ( ev );

    if ( k->key() == Qt::Key_Enter || k->key() == Qt::Key_Return ) {
      tableRecords_->setFocus();
      return true;
    }

    if ( k->key() == Qt::Key_Up ) {
      ui.comboBoxFieldToSearch->setFocus();
      return true;
    }

    if ( k->key() == Qt::Key_Down ) {
      tableRecords_->setFocus();
      return true;
    }

    if ( k->key() == Qt::Key_F2 ) {
      ui.comboBoxFieldToSearch->popup();
      return true;
    }

    if ( k->text() == "'" || k->text() == "\\" )
      return true;
  }

  if ( obj == tableRecords_ || obj == ui.lineEditSearch )
    return false;
  else
    return QFrame::eventFilter( obj, ev );
}

void AQTableDB::showEvent( QShowEvent * e )
{
  firstRefresh();
  QFrame::showEvent( e );
}

void AQTableDB::putFirstCol( const QString & c )
{
  if ( !tableRecords_ )
    return ;

  Q_ASSERT( act_ );

  const AQTableMD * tMD = act_->cursor() ->metadata();
  Q3Header * horizHeader = tableRecords_->horizontalHeader();

  int i;
  for ( i = 0; horizHeader->label( i ) != tMD->fieldNameToAlias( c ) && i < horizHeader->count(); ++i )
    ;

  if ( i < horizHeader->count() )
    putFirstCol( i );
}

void AQTableDB::putFirstCol( int c )
{
  if ( !tableRecords_ || ui.comboBoxFieldToSearch->text( c ) == "*" )
    return ;

  Q_ASSERT( act_ );

  const AQTableMD * tMD = act_->cursor() ->metadata();
  Q3Header * horizHeader = tableRecords_->horizontalHeader();
  QString aliasfN = horizHeader->label( c );
  QString fN = tMD->fieldAliasToName( aliasfN );
  QString aliasfN2 = horizHeader->label( 0 );
  QString fN2 = tMD->fieldAliasToName( aliasfN2 );

  ui.comboBoxFieldToSearch->setCurrentIndex( 0 );
  ui.comboBoxFieldToSearch->setItemText( 0, aliasfN );
  tableRecords_->setColumn( 0, fN, aliasfN );
  ui.comboBoxFieldToSearch->setItemText( c, aliasfN2 );
  tableRecords_->setColumn( c, fN2, aliasfN2 );
  tableRecords_->refreshColumns();
  refresh( true );

  if ( !ui.lineEditSearch->text().isEmpty() )
    filterRecords( ui.lineEditSearch->text() );
}

void AQTableDB::setReadOnly( bool mode )
{
  if ( tableRecords_ )
    tableRecords_->setAQReadOnly( mode );

  Q_EMIT readOnlyChanged( mode );
}

void AQTableDB::setEditOnly( bool mode )
{
  if ( tableRecords_ )
    tableRecords_->setEditOnly( mode );

  Q_EMIT editOnlyChanged( mode );
}

void AQTableDB::setInsertOnly( bool mode )
{
  if ( tableRecords_ )
    tableRecords_->setInsertOnly( mode );

  Q_EMIT insertOnlyChanged( mode );
}

void AQTableDB::setFilter( const QString & f )
{
  filter_ = f;
}

QString AQTableDB::filter() const
{
  return filter_;
}

void AQTableDB::refreshDelayed( int msec )
{
  if ( !timer || !act_ || act_->cursor() ->modeAccess() != AQSql::BROWSE || !tableRecords_ )
    return ;

  if ( msec > 0 ) {
    QObject * obj = sender();
    if ( obj ) {
      if ( !qobject_cast<QTimer *> ( obj ) ) {
        timer->start( msec, true );
        return ;
      }
    } else {
      timer->start( msec, true );
      return ;
    }
  }

  refresh( false, true );
}

void AQTableDB::refresh( bool refreshHead, bool refreshData )
{
  if ( !tableRecords_ )
    return ;

  if ( timer )
    timer->stop();

  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();
  const AQTableMD * tMD = cursor_->metadata();

  if ( tableName_.isEmpty() )
    tableName_ = tMD->name();

  cursor_->setFilter( filter_ );

  if ( refreshHead ) {
    Q3Header * horizHeader = tableRecords_->horizontalHeader();
    QString firstField = tMD->fieldAliasToName( horizHeader->label( 0 ) );
    sortField_ = tMD->field( firstField );
    QStringList s( QStringList() << firstField );
    tableRecords_->setSort( s );
  }

  if ( refreshData )
    tableRecords_->refresh();
}

void AQTableDB::filterRecords( const QString & p )
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  if ( !sortField_ ) {
    const AQTableMD * tMD = cursor_->metadata();
    sortField_ = tMD->field( tMD->primaryKey() );
  }

  if ( sortField_->type() == AQCore::Unlock || !tableRecords_ || !ui.lineEditSearch || !ui.comboBoxFieldToSearch )
    return ;

  filter_ = "";
  if ( !p.isEmpty() ) {

    if ( ui.comboBoxFieldToSearch->text( ui.comboBoxFieldToSearch->currentItem() ) == "*" ) {
      AQFieldMDHash fieldList( cursor_->metadata() ->fieldList() );

      if ( !fieldList.isEmpty() ) {
        AQFieldMD * field;

        for ( AQFieldMDHash::const_iterator it = fieldList.begin(); it != fieldList.end(); ++it ) {
          field = *it;

          if ( !field->visibleGrid() )
            continue;

          if ( !p.contains( "'" ) && !p.contains( "\\" ) && field->type() == QVariant::String ) {
            if ( !filter_.isEmpty() )
              filter_ += " OR ";
            filter_ += cursor_->db().manager() ->formatAssignValueLike( field, p, true );
          }
        }

      }

    } else if ( !p.contains( "'" ) && !p.contains( "\\" ) )
      filter_ = cursor_->db().manager() ->formatAssignValueLike( sortField_, p, true );

  }

  if ( initSearch_.isEmpty() )
    refreshDelayed( 300 );
  else {
    initSearch_ = QString();
    refreshDelayed( 300 );
    ui.lineEditSearch->setFocus();
    ui.lineEditSearch->selectAll();
  }
}

QString AQTableDB::tableName() const
{
  return tableName_;
}

QString AQTableDB::foreignField() const
{
  return foreignField_;
}

QString AQTableDB::fieldRelation() const
{
  return fieldRelation_;
}

void AQTableDB::setTableName( const QString & fT )
{
  tableName_ = fT;
}

void AQTableDB::setForeignField( const QString & fN )
{
  foreignField_ = fN;
}

void AQTableDB::setFieldRelation( const QString & fN )
{
  fieldRelation_ = fN;
}

void AQTableDB::deleteRecord()
{
  if ( tableRecords_ )
    tableRecords_->deleteRecord();
}

void AQTableDB::browseRecord()
{
  if ( tableRecords_ )
    tableRecords_->browseRecord();
}

void AQTableDB::editRecord()
{
  if ( tableRecords_ )
    tableRecords_->editRecord();
}

void AQTableDB::insertRecord()
{
  if ( tableRecords_ )
    tableRecords_->insertRecord();
}

void AQTableDB::copyRecord()
{
  if ( tableRecords_ )
    tableRecords_->copyRecord();
}

void AQTableDB::initAQAction()
{
  if ( !tableName_.isEmpty() || ( !foreignField_.isEmpty() && !fieldRelation_.isEmpty() ) ) {
    Q_ASSERT( act_ );

    QString table_( tableName_ );

    if ( table_.isEmpty() )
      table_ = act_->action() ->table();
    else if ( !act_->cursor() ->db().manager() ->metadata( table_ ) ) {
      firstRefresh_ = true;
      return ;
    }

    if ( !foreignField_.isEmpty() && !fieldRelation_.isEmpty() ) {

      AQSqlCursor * cursorMaster_ = act_->cursor();
      AQRelationMD * relMasterToDetail = new AQRelationMD( table_, fieldRelation_, AQCore::RELATION_1M );
      relMasterToDetail->setField( foreignField_ );

      act_ = AQAction::create( act_, relMasterToDetail, cursorMaster_->db().connectionName() );

      if ( cursorMaster_->modeAccess() == AQSql::BROWSE )
        setReadOnly( true );

    } else if ( !act_ || ( act_ && act_->action() ->table() != table_ ) )
      act_ = AQAction::create( table_, act_ ? act_->cursor() ->db().connectionName() : "default" );

  }

}

AQDataTable * AQTableDB::tableRecords()
{
  firstRefresh();
  return tableRecords_;
}

void AQTableDB::setOrderCols( const QStringList & fields )
{
  if ( !tableRecords_ )
    return ;

  Q_ASSERT( act_ );

  const AQTableMD * tMD = act_->cursor() ->metadata();
  Q3Header * horizHeader = tableRecords_->horizontalHeader();

  QString fieldName;
  int i = fields.count(), c;
  QStringList::const_iterator it( fields.end() );

  do {
    --it;
    --i;
    for ( c = 0; horizHeader->label( c ) != tMD->fieldNameToAlias( *it ) && c < horizHeader->count(); ++c )
      ;
    if ( c < horizHeader->count() ) {
      fieldName = tMD->fieldAliasToName( horizHeader->label( i ) );
      horizHeader->setLabel( i, tMD->fieldNameToAlias( *it ) );
      tableRecords_->setColumn( i, *it, tMD->fieldNameToAlias( *it ) );
      horizHeader->setLabel( i, tMD->fieldNameToAlias( fieldName ) );
      tableRecords_->setColumn( c, fieldName, tMD->fieldNameToAlias( fieldName ) );
      tableRecords_->refreshColumns();
    }
  } while ( it != fields.begin() );

  refresh( true );
  if ( !ui.lineEditSearch->text().isEmpty() )
    filterRecords( ui.lineEditSearch->text() );
}

void AQTableDB::emitCurrentChanged( QSqlRecord * )
{
  Q_EMIT currentChanged();
}

void AQTableDB::setInitSearch( const QString & iS )
{
  initSearch_ = iS;
}

void AQTableDB::addColumns()
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();
  AQTableMD * mtd = cursor_->metadata();
  QSqlIndex index( cursor_->sort() );
  AQFieldMD * field;

  for ( int i = 0; i < index.count(); ++i ) {
    field = mtd->field( index.fieldName( i ) );

    if ( field->visibleGrid() ) {
      tableRecords_->addColumn( field->name(), field->alias() );
      ui.comboBoxFieldToSearch->addItem( field->alias() );
    }
  }

  Q3SqlRecordInfo inf = cursor_->driver() ->recordInfo( *( static_cast<QSqlQuery *> ( cursor_ ) ) );

  for ( Q3SqlRecordInfo::const_iterator it = inf.begin(); it != inf.end(); ++it ) {
    field = cursor_->metadata() ->field( ( *it ).name() );

    if ( field->visibleGrid() && index.indexOf( field->name() ) == -1 ) {
      tableRecords_->addColumn( field->name(), field->alias() );
      ui.comboBoxFieldToSearch->addItem( field->alias() );
    }
  }

  ui.comboBoxFieldToSearch->addItem( "*" );
}

void AQTableDB::firstRefresh()
{
  if ( !firstRefresh_ && act_ && form_ ) {
    firstRefresh_ = true;

    AQSqlCursor * cursor_ = act_->cursor();
    AQSqlCursor * cursorMaster_ = act_->aqActionMaster() ? act_->aqActionMaster() ->cursor() : 0;

    AbanQ->setOverrideCursor( Qt::waitCursor );
    blockSignals( true );

    if ( cursorMaster_ )
      cursorMaster_->blockSignals( true );
    cursor_->blockSignals( true );

    if ( form_->type() == AQUi::FormRecord )
      setupRecord();
    else
      setupMaster();

    cursor_->blockSignals( false );
    if ( cursorMaster_ )
      cursorMaster_->blockSignals( false );

    blockSignals( false );
    AbanQ->restoreOverrideCursor();
  }
}

void AQTableDB::setupMaster()
{
  ui.setupUi( this );
  setupTableRecords();
}

void AQTableDB::setupRecord()
{
  QVBoxLayout * vboxLayout = new QVBoxLayout( this );

  vboxLayout->setSpacing( 0 );
  vboxLayout->setMargin( 0 );
  vboxLayout->setContentsMargins( 0, 0, 0, 0 );

  AQFormDB * form = new AQFormDB( this );

  form->setSizePolicy( sizePolicy() );
  form->setMinimumSize( minimumSize() );
  form->setMaximumSize( maximumSize() );
  form->centralWidget() ->setSizePolicy( sizePolicy() );
  form->centralWidget() ->setMinimumSize( minimumSize() );
  form->centralWidget() ->setMaximumSize( maximumSize() );

  ui.setupUi( form->centralWidget() );
  setupTableRecords();

  form->setAQAction( act_ );
  form->setUiMaster( form->centralWidget() );
  form->activateDetailUi();

  vboxLayout->addWidget( form );
  form->show();
  updateGeometry();
}

void AQTableDB::setupTableRecords()
{
  Q_ASSERT( act_ );

  connect( ui.comboBoxFieldToSearch, SIGNAL( activated( int ) ), ui.lineEditSearch, SLOT( setFocus() ) );
  connect( ui.comboBoxFieldToSearch, SIGNAL( activated( int ) ), ui.lineEditSearch, SLOT( selectAll() ) );
  connect( ui.lineEditSearch, SIGNAL( textChanged( QString ) ), this, SLOT( filterRecords( QString ) ) );
  connect( ui.comboBoxFieldToSearch, SIGNAL( activated( int ) ), this, SLOT( putFirstCol( int ) ) );

  tableRecords_ = new AQDataTable( ui.FrameTable );
  tableRecords_->blockSignals( true );
  tableRecords_->setFocusPolicy( Qt::StrongFocus );
  setFocusProxy( tableRecords_ );
  tableRecords_->setAQAction( act_ );
  tableRecords_->setReadOnly( true );
  addColumns();
  tableRecords_->refreshColumns();

  if ( initSearch_.isEmpty() )
    refresh( true );
  else
    ui.lineEditSearch->setText( initSearch_ );

  connect( tableRecords_, SIGNAL( currentChanged( QSqlRecord * ) ), this, SLOT( emitCurrentChanged( QSqlRecord * ) ) );
  connect( tableRecords_, SIGNAL( doubleClickOnRecord( bool ) ), act_.data(), SLOT( activate( bool ) ) );
  connect( tableRecords_, SIGNAL( pressEnterOnRecord( bool ) ), act_.data(), SLOT( activate( bool ) ) );

  ui.vboxLayout1->addWidget( tableRecords_ );
  setTabOrder( tableRecords_, ui.lineEditSearch );
  setTabOrder( ui.lineEditSearch, ui.comboBoxFieldToSearch );

  ui.lineEditSearch->installEventFilter( this );
  tableRecords_->installEventFilter( this );
  tableRecords_->blockSignals( false );
}

FLTableDB::FLTableDB( QWidget * parent ) :
    AQTableDB( parent )
{}

FLTableDB::~FLTableDB()
{}
