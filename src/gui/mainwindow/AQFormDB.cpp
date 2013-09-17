/***************************************************************************
AQFormDB.cpp
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

#include <QIcon>
#include <QCloseEvent>
#include <QDialog>
#include <QTimer>
#include <QHBoxLayout>
#include <QShortcut>

#include <AQCore/AQCore.h>
#include <AQCore/AQActionMD.h>
#include <AQCore/AQTableMD.h>
#include <AQSql/AQSqlConnections.h>
#include <AQSql/AQSqlCursor.h>
#include <AQSql/AQManager.h>

#include "AQFormDB.h"
#include "../AQApplication.h"
#include "../AQAction.h"
#include "../manager/AQUiManager.h"
#include "../component/AQComponentDB.h"
#include "../util/AQUiUtil.h"

#ifdef AQ_WARN
long AQFormDB::countRefForm = 0;
#endif

AQFormDB::AQFormDB( QWidget * parent ) :
    QMainWindow( parent ),
    detailInit_( false ),
    dialogInit_( false ),
    ignoreCloseEvent_( false ),
    deleteOnClose_( false ),
    uiRecordNeedUpdate_( false ),
    uiMasterNeedUpdate_( false ),
    uiActive_( 0 ),
    savedUiActive_( 0 ),
    uiMaster_( 0 ),
    uiRecord_( 0 ),
    result_( AQUi::NoDialogCode )
{
#ifdef AQ_WARN
  ++AQFormDB::countRefForm;
#endif

  setWindowFlags( 0 );
  ui.setupUi( this );
  setWindowIcon( QIcon( ":/images/icon.png" ) );

  escShortcut_ = new QShortcut( Qt::Key_Escape, this );
  escShortcut_->setEnabled( false );
  connect( escShortcut_, SIGNAL( activated() ), this, SLOT( on_actExit_triggered() ) );
}

AQFormDB::~AQFormDB()
{
#ifdef AQ_WARN
  --AQFormDB::countRefForm;
#endif
}

void AQFormDB::buildUi()
{
  if ( act_ ) {
    switch ( type_ ) {
      case AQUi::FormMaster:
      case AQUi::FormDetail:
        if ( act_->action() ->form().isEmpty() )
          AQ_MSGBOX_WARN( tr( "Aviso" ),
                          tr( "No hay definido ningún formulario para esta acción" ) );

        uiActive_ = AQUiManager::createUI( act_->action() ->form(), this,
                                           act_->cursor() ->db() );
        uiMaster_ = uiActive_;
        uiMasterNeedUpdate_ = true;
        break;
      case AQUi::FormRecord:
        if ( act_->action() ->formRecord().isEmpty() )
          AQ_MSGBOX_WARN( tr( "Aviso" ),
                          tr( "No hay definido ningún formulario para esta acción" ) );

        uiActive_ = AQUiManager::createUI( act_->action() ->formRecord(), this,
                                           act_->cursor() ->db() );
        uiRecord_ = uiActive_;
        uiRecordNeedUpdate_ = true;
        break;
    }
  }
}

void AQFormDB::linkUi()
{
  if ( uiActive_ ) {
    QList<AQComponentDB *> cos = uiActive_->findChildren<AQComponentDB *>();
    AQComponentDB * comp;

    for ( QList<AQComponentDB *>::const_iterator it = cos.begin(); it != cos.end(); ++it ) {
      comp = *it;
      comp->setAQAction( act_ );
      comp->setupUi( this );
      comp->linkUi();
    }

    for ( QList<AQComponentDB *>::const_iterator it = cos.begin(); it != cos.end(); ++it )
      ( *it ) ->initRefreshUi();
  }
}

void AQFormDB::unlinkUi()
{
  if ( uiActive_ ) {
    QList<AQComponentDB *> cos = uiActive_->findChildren<AQComponentDB *>();

    for ( QList<AQComponentDB *>::const_iterator it = cos.begin(); it != cos.end(); ++it )
      ( *it ) ->setAQAction( AQSharedAction() );

    if ( uiActive_ == uiMaster_ )
      uiMasterNeedUpdate_ = true;
    else if ( uiActive_ == uiRecord_ )
      uiRecordNeedUpdate_ = true;
  }
}

void AQFormDB::addUi()
{
  if ( uiActive_ )
    setCentralWidget( uiActive_ );
}

void AQFormDB::setAQAction( AQSharedAction act )
{
  if ( act_ || !act ) {
    uiActive_ = uiMaster_;
    unlinkUi();
    uiActive_ = uiRecord_;
    unlinkUi();
  }

  act_ = act;
}

AQSharedAction AQFormDB::aqAction() const
{
  return act_;
}

QVariant AQFormDB::exec( const QString & fieldName )
{
  QDialog * diag = new QDialog;

  diag->setWindowIcon( QIcon( ":/images/icon.png" ) );
  QVBoxLayout * vboxLayout = new QVBoxLayout( diag );
  vboxLayout->setSpacing( 0 );
  vboxLayout->setContentsMargins( 0, 0, 0, 0 );
  AQFormDB * form = new AQFormDB( diag );

  activateMasterUi();
  act_->requestRecord();
  form->setAQAction( act_ );
  form->dialogInit_ = true;
  form->uiMaster_ = uiMaster_;
  form->activateMasterUi();

  vboxLayout->addWidget( form );
  diag->resize( form->size() );
  form->show();

  connect( form, SIGNAL( accepted() ), diag, SLOT( accept() ) );
  connect( form, SIGNAL( rejected() ), diag, SLOT( reject() ) );
  connect( act_.data(), SIGNAL( accepted() ), form, SLOT( accept() ) );

  setupCaption( diag );

  QVariant v;
  result_ = ( AQUi::DialogCode ) diag->exec();
  if ( result_ == AQUi::Accepted )
    v = act_->cursor() ->valueBuffer( fieldName );

  QTimer::singleShot( 0, diag, SLOT( deleteLater() ) );

  if ( !parent() )
    close();

  return v;
}

void AQFormDB::exec()
{
  QDialog * diag = new QDialog;

  diag->setWindowIcon( QIcon( ":/images/icon.png" ) );
  QVBoxLayout * vboxLayout = new QVBoxLayout( diag );
  vboxLayout->setSpacing( 0 );
  vboxLayout->setContentsMargins( 0, 0, 0, 0 );
  AQFormDB * form = new AQFormDB( diag );

  activateRecordUi();
  form->setAQAction( act_ );
  form->dialogInit_ = true;
  form->uiRecord_ = uiRecord_;
  form->activateRecordUi();

  vboxLayout->addWidget( form );
  diag->resize( form->size() );
  form->show();

  connect( form, SIGNAL( accepted() ), diag, SLOT( accept() ) );
  connect( form, SIGNAL( rejected() ), diag, SLOT( reject() ) );

  setupCaption( diag );

  result_ = ( AQUi::DialogCode ) diag->exec();
  QTimer::singleShot( 0, diag, SLOT( deleteLater() ) );

  if ( !parent() )
    close();
}

void AQFormDB::activateMasterUi()
{
  if ( !detailInit_ ) {
    save();

    uiActive_ = uiMaster_;
    type_ = AQUi::FormMaster;

    if ( !uiActive_ )
      buildUi();

    addUi();

    if ( uiMasterNeedUpdate_ )
      linkUi();

    if ( uiActive_ ) {
      refreshButtons();
      uiMasterNeedUpdate_ = false;
    } else
      restore();

  } else
    activateDetailUi();
}

void AQFormDB::activateDetailUi()
{
  save();

  detailInit_ = true;
  uiActive_ = uiMaster_;
  type_ = AQUi::FormDetail;

  if ( uiActive_ ) {
    addUi();
    refreshButtons();
  } else
    restore();
}

void AQFormDB::activateRecordUi()
{
  save();

  uiActive_ = uiRecord_;
  type_ = AQUi::FormRecord;

  if ( !uiActive_ )
    buildUi();

  addUi();

  if ( uiRecordNeedUpdate_ )
    linkUi();

  if ( uiActive_ ) {
    refreshButtons();
    uiRecordNeedUpdate_ = false;
  } else
    restore();
}

void AQFormDB::closeEvent( QCloseEvent * e )
{
  if ( result_ == AQUi::NoDialogCode )
    reject();

  Q_EMIT closeRequest( this );

  if ( ignoreCloseEvent_ )
    e->ignore();
  else {
    QMainWindow::closeEvent( e );

    if ( deleteOnClose_ && !parent() )
      QTimer::singleShot( 0, this, SLOT( deleteLater() ) );
  }
}

void AQFormDB::showEvent ( QShowEvent * e )
{
  if ( !e->spontaneous() && isWindow() )
    setupCaption( this );
  QWidget::showEvent( e );
}

AQUi::FormType AQFormDB::type() const
{
  return type_;
}

void AQFormDB::accept()
{
  result_ = AQUi::Accepted;
  Q_EMIT accepted();
}

void AQFormDB::reject()
{
  result_ = AQUi::Rejected;
  Q_EMIT rejected();
}

AQUi::DialogCode AQFormDB::result() const
{
  return result_;
}

void AQFormDB::setIgnoreCloseEvent( bool ign )
{
  ignoreCloseEvent_ = ign;
}

bool AQFormDB::ignoreCloseEvent() const
{
  return ignoreCloseEvent_;
}

void AQFormDB::setDeleteOnClose( bool del )
{
  deleteOnClose_ = del;
}

bool AQFormDB::deleteOnClose() const
{
  return deleteOnClose_;
}

void AQFormDB::on_actLast_triggered()
{
  act_->cursor() ->last();
}

void AQFormDB::on_actNext_triggered()
{
  act_->cursor() ->next();
}

void AQFormDB::on_actPrevious_triggered()
{
  act_->cursor() ->prev();
}

void AQFormDB::on_actFirst_triggered()
{
  act_->cursor() ->first();
}

void AQFormDB::on_actRecord_triggered()
{
  exec();
}

void AQFormDB::on_actExit_triggered()
{
  if ( dialogInit_ )
    reject();
  else
    close();
}

void AQFormDB::refreshButtons()
{
  escShortcut_->setEnabled( false );

  switch ( type_ ) {

    case AQUi::FormDetail:
      ui.toolBar->hide();
      break;

    case AQUi::FormMaster:
      if ( dialogInit_ ) {
        ui.toolBar->hide();
        escShortcut_->setEnabled( true );
      } else
        ui.actRecord->setEnabled( true );
      break;

    case AQUi::FormRecord:
      ui.actRecord->setEnabled( false );
      ui.toolBar->show();
      break;

  }
}

void AQFormDB::setUiMaster( QWidget * uiMaster )
{
  save();

  uiActive_ = uiMaster_;
  unlinkUi();

  uiMaster_ = uiMaster;
  uiMaster_->setParent( this );

  restore();
}

QWidget * AQFormDB::uiMaster() const
{
  return uiMaster_;
}

void AQFormDB::setUiRecord( QWidget * uiRecord )
{
  save();

  uiActive_ = uiRecord_;
  unlinkUi();

  uiRecord_ = uiRecord;
  uiRecord_->setParent( this );

  restore();
}

QWidget * AQFormDB::uiRecord() const
{
  return uiRecord_;
}

QWidget * AQFormDB::uiActive() const
{
  return uiActive_;
}

QMenu * AQFormDB::createPopupMenu ()
{
  return 0;
}

void AQFormDB::save()
{
  savedUiActive_ = uiActive_;
  savedType_ = type_;
}

void AQFormDB::restore()
{
  uiActive_ = savedUiActive_;
  type_ = savedType_;
}

void AQFormDB::setupCaption( QWidget * f )
{
  Q_ASSERT( act_ );

  if ( act_->action() ->caption().isEmpty() ) {

    switch ( act_->cursor() ->modeAccess() ) {
      case AQSql::INSERT:
        f->setCaption( tr( "Insertar " ) + act_->cursor() ->metadata() ->alias() );
        break;
      case AQSql::EDIT:
        f->setCaption( tr( "Editar " ) + act_->cursor() ->metadata() ->alias() );
        break;
      case AQSql::BROWSE:
        f->setCaption( tr( "Visualizar " ) + act_->cursor() ->metadata() ->alias() );
        break;
      default:
        break;
    }

  } else
    f->setCaption( act_->action() ->caption() );
}
