/***************************************************************************
AQConnectDB.cpp
-------------------
begin                : 25/08/2007
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

#include <WContainerWidget>
#include <WHBoxLayout>
#include <WVBoxLayout>
#include <WLabel>
#include <WLineEdit>
#include <WComboBox>

#include <Ext/Panel>
#include <Ext/Dialog>
#include <Ext/Button>
#include <Ext/ComboBox>

#include <QSettings>

#include <AQCore/AQCoreUtil.h>
#include <AQSql/AQSql.h>
#include <AQSql/AQSqlDatabase.h>
#include <AQSql/AQSqlConnections.h>
#include <AQUi/AQUiUtil.h>
#include <AQUi/AQInvokerThreadContext.h>

#include "../wtqt/WQApplication.h"
#include "../webgets/WQMenu.h"
#include "../webgets/WQAction.h"
#include "../AQApplication.h"
#include "AQConnectDB.h"

#define tr(T) toWString( AQCoreUtil::translate( "ConnectDB", T ) )

class AQConnectDBWidget : public WQWidget
{
public:

  AQConnectDBWidget( AQConnectDB * qq, const QString & connectionName = "default" ) :
      WQWidget( static_cast<WObject *> ( 0 ) ), connectionName_( connectionName ), diag_( 0 ), q( qq )
  {}

  void init();

  void finish()
  {
    if ( diag_ ) {
      diag_->WWidget::hide();
      delete diag_;
    }
  }

  WWidget * ww()
  {
    return diag_;
  }

  void initMenuMarks();
  void addActionMark( const QString & mark );
  void removeActionMark( const QString & mark );
  bool addDatabase();
  void driverChanged( const QString & driverAlias );
  void tryConnect();
  void addMark();
  void removeMark();
  void selectMark( const QString & mark );
  void selectMarkByIndex( int index );
  void tryConnectByAction( QAction * action );

  QString connectionName_;

  WLineEdit * lineEditUser;
  WLineEdit * lineEditPassword;
  WLineEdit * lineEditPort;
  WLineEdit * lineEditHost;

  Ext::ComboBox * comboBoxNameDB;
  WComboBox * comboBoxDB;
  WComboBox * comboBoxMarks;

  Ext::Button * pbMarks;
  Ext::Button * pbAddMark;
  Ext::Button * pbRemoveMark;

  WQMenu * menuMarks_;
  QStringList marks_;
  QList<WQAction *> actionsMarks_;
  QHash<QString, WQAction *> hashActionsMarks_;

  Ext::Dialog * diag_;
  AQConnectDB * q;
};

void AQConnectDBWidget::init()
{
  diag_ = new Ext::Dialog;

  diag_->setWindowTitle( tr( "Conectar" ) );
  diag_->resize( 350, WLength() );

  WContainerWidget * frMain = new WContainerWidget;
  WVBoxLayout * lay_ = new WVBoxLayout( frMain );
  lay_->setSpacing( 4 );
  lay_->setContentsMargins( 4, 4, 4, 4 );

  WLabel * label;
  WVBoxLayout * vbox;
  WHBoxLayout * hbox;

  hbox = new WHBoxLayout;
  hbox->setSpacing( 4 );
  label = new WLabel( tr( "Base de datos" ) );
  comboBoxNameDB = new Ext::ComboBox( frMain );
  comboBoxNameDB->setTextSize( 28 );
  comboBoxNameDB->resize( WLength(), 18 );
  comboBoxNameDB->setEditable( true );
  hbox->addWidget( label, 40, AlignMiddle );
  hbox->addWidget( comboBoxNameDB, 120, AlignMiddle );
  lay_->addLayout( hbox );

  lay_->addStretch( 5 );

  hbox = new WHBoxLayout;
  hbox->setSpacing( 4 );
  label = new WLabel( tr( "Usuario" ) );
  lineEditUser = new WLineEdit( frMain );
  lineEditUser->setStyleClass( "x-form-field" );
  hbox->addWidget( label, 40, AlignMiddle );
  hbox->addWidget( lineEditUser, 120, AlignMiddle );
  lay_->addLayout( hbox );

  hbox = new WHBoxLayout;
  hbox->setSpacing( 4 );
  label = new WLabel( tr( "Contraseña" ) );
  lineEditPassword = new WLineEdit( frMain );
  lineEditPassword->setStyleClass( "x-form-field" );
  lineEditPassword->setEchoMode( WLineEdit::Password );
  hbox->addWidget( label, 40, AlignMiddle );
  hbox->addWidget( lineEditPassword, 120, AlignMiddle );
  lay_->addLayout( hbox );

  lay_->addStretch( 5 );

  diag_->layout() ->addWidget( frMain );

  Ext::Panel * panelMore = new Ext::Panel;
  panelMore->setBorder( false );
  panelMore->resize( WLength(), 180 );
  panelMore->setTitle( "" );
  panelMore->setCollapsible( true );
  panelMore->collapse();

  WContainerWidget * frMore = new WContainerWidget;
  frMore->resize( WLength(), 160 );
  vbox = new WVBoxLayout( frMore );
  vbox->setSpacing( 4 );
  vbox->setContentsMargins( 4, 4, 4, 4 );

  hbox = new WHBoxLayout;
  hbox->setSpacing( 1 );
  comboBoxMarks = new WComboBox( frMore );
  comboBoxMarks->setStyleClass( "x-form-field" );
  WT_SIGNAL( comboBoxMarks->activated ).connect( this, &AQConnectDBWidget::selectMarkByIndex );
  pbAddMark = new Ext::Button( frMore );
  pbAddMark->setIcon( "images/plus.png" );
  WT_SIGNAL( pbAddMark->clicked ).connect( this, &AQConnectDBWidget::addMark );
  pbRemoveMark = new Ext::Button( frMore );
  pbRemoveMark->setIcon( "images/minus.png" );
  WT_SIGNAL( pbRemoveMark->clicked ).connect( this, &AQConnectDBWidget::removeMark );
  hbox->addWidget( comboBoxMarks, 120, AlignMiddle );
  hbox->addWidget( pbAddMark, 0, AlignMiddle );
  hbox->addWidget( pbRemoveMark, 0, AlignMiddle );
  vbox->addLayout( hbox );

  hbox = new WHBoxLayout;
  hbox->setSpacing( 4 );
  label = new WLabel( tr( "Controlador" ) );
  comboBoxDB = new WComboBox( frMore );
  comboBoxDB->setStyleClass( "x-form-field" );
  hbox->addWidget( label, 30, AlignMiddle );
  hbox->addWidget( comboBoxDB, 120, AlignMiddle );
  vbox->addLayout( hbox );

  hbox = new WHBoxLayout;
  hbox->setSpacing( 4 );
  label = new WLabel( tr( "Servidor" ), frMore );
  lineEditHost = new WLineEdit;
  lineEditHost->setStyleClass( "x-form-field" );
  hbox->addWidget( label, 30, AlignMiddle );
  hbox->addWidget( lineEditHost, 120, AlignMiddle );
  vbox->addLayout( hbox );

  hbox = new WHBoxLayout;
  hbox->setSpacing( 4 );
  label = new WLabel( tr( "Puerto" ), frMore );
  lineEditPort = new WLineEdit( frMore );
  lineEditPort->setStyleClass( "x-form-field" );
  hbox->addWidget( label, 30, AlignMiddle );
  hbox->addWidget( lineEditPort, 120, AlignMiddle );
  vbox->addLayout( hbox );

  panelMore->layout() ->addWidget( frMore );

  diag_->layout() ->addWidget( panelMore );

  pbMarks = new Ext::Button;
  pbMarks->setIcon( "images/down.png" );

  initMenuMarks();

  pbMarks->setMenu( menuMarks_->wmenu() );
  pbMarks->setEnabled( !marks_.isEmpty() );
  diag_->addButton( pbMarks );

  Ext::Button * pbConnect = new Ext::Button( tr( "Conectar" ) );
  pbConnect->setIcon( "images/yes.png" );
  WT_SIGNAL( pbConnect->clicked ).connect( this, &AQConnectDBWidget::tryConnect );
  diag_->addButton( pbConnect );

  Ext::Button * pbCancel = new Ext::Button( tr( "Cancelar" ) );
  pbCancel->setIcon( "images/no.png" );
  WT_SIGNAL( pbCancel->clicked ).connect( q, &AQConnectDB::reject );
  diag_->addButton( pbCancel );

  QSettings settings( QSettings::UserScope, "InfoSiAL" );

  lineEditUser->setText( toWString( settings.value( AQ_KEYBASE + "DBA/username" ).toString() ) );
  lineEditPassword->setText( toWString( settings.value( AQ_KEYBASE + "DBA/password" ).toString() ) );
  lineEditPort->setText( toWString( settings.value( AQ_KEYBASE + "DBA/port", "5432" ).toString() ) );
  lineEditHost->setText( toWString( settings.value( AQ_KEYBASE + "DBA/hostname", "localhost" ).toString() ) );

  QStringList list( settings.value( AQ_KEYBASE + "DBA/namesDB" ).toStringList() );
  QStringList::const_iterator it;
  for ( it = list.begin(); it != list.end(); ++it )
    comboBoxNameDB->insertItem( 0, toWString( *it ) );
  comboBoxNameDB->setText( toWString( settings.value( AQ_KEYBASE + "DBA/lastDB", "abanq" ).toString() ) );

  list = AQSqlDatabase::driverAliases();
  for ( it = list.begin(); it != list.end(); ++it )
    comboBoxDB->insertItem( 0, toWString( *it ) );
  WString driverAlias( toWString( settings.value( AQ_KEYBASE + "DBA/db", AQSqlDatabase::defaultAlias() ).toString() ) );
  for ( int i = 0; i < comboBoxDB->count(); ++i ) {
    if ( comboBoxDB->itemText( i ) == driverAlias ) {
      comboBoxDB->setCurrentIndex( i );
      break;
    }
  }

  driverChanged( toQString( comboBoxDB->currentText() ) );

  diag_->show();
}

#undef tr

void AQConnectDBWidget::initMenuMarks()
{
  menuMarks_ = new WQMenu( AQCoreUtil::translate( "ConnectDB", "Marcadores" ), this );
  menuMarks_->triggered().connect( this, &AQConnectDBWidget::tryConnectByAction );

  QSettings settings( QSettings::UserScope, "InfoSiAL" );
  QStringList list( settings.value( AQ_KEYBASE + "DBA/marks" ).toStringList() );
  for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it )
    addActionMark( *it );
}

void AQConnectDBWidget::addActionMark( const QString & mark )
{
  if ( mark.isEmpty() )
    return ;

  WQAction * action = hashActionsMarks_.value( mark );
  if ( !action ) {
    action = new WQAction( mark, this );
    marks_ << mark;
    actionsMarks_ << action;
    hashActionsMarks_.insert( mark, action );
    comboBoxMarks->insertItem( 0, toWString( mark ) );
  } else if ( !marks_.contains( mark ) )
    marks_ << mark;

  menuMarks_->addAction( action );
  pbMarks->setEnabled( !marks_.isEmpty() );
}

void AQConnectDBWidget::removeActionMark( const QString & mark )
{
  if ( mark.isEmpty() )
    return ;

  WQAction * action = hashActionsMarks_.value( mark );
  int i;
  if ( action && ( i = marks_.indexOf( mark ) ) != -1 ) {
    WString wmark( toWString( mark ) );
    for ( int j = 0; j < comboBoxMarks->count(); ++j ) {
      if ( comboBoxMarks->itemText( j ) == wmark ) {
        comboBoxMarks->removeItem( j );
        break;
      }
    }
    menuMarks_->removeAction( action );
    marks_.removeAt( i );
  }

  pbMarks->setEnabled( !marks_.isEmpty() );
}

bool AQConnectDBWidget::addDatabase()
{
  return AQSqlConnections::addDatabase( toQString( comboBoxDB->currentText() ),
                                        toQString( comboBoxNameDB->currentText() ), toQString( lineEditUser->text() ),
                                        toQString( lineEditPassword->text() ), toQString( lineEditHost->text() ),
                                        toQString( lineEditPort->text() ).toInt(), connectionName_ );
}

void AQConnectDBWidget::driverChanged( const QString & driverAlias )
{
  lineEditUser->setEnabled( AQSqlDatabase::needConnOption( driverAlias, AQSql::USER ) );
  lineEditPassword->setEnabled( AQSqlDatabase::needConnOption( driverAlias, AQSql::PASSWORD ) );
  lineEditPort->setEnabled( AQSqlDatabase::needConnOption( driverAlias, AQSql::PORT ) );
  lineEditHost->setEnabled( AQSqlDatabase::needConnOption( driverAlias, AQSql::HOST ) );
}

void AQConnectDBWidget::tryConnect()
{
  if ( !addDatabase() ) {
    q->error_ = true;
    q->accept();
    return ;
  }

  QSettings settings( QSettings::UserScope, "InfoSiAL" );

  settings.setValue( AQ_KEYBASE + "DBA/username", toQString( lineEditUser->text() ) );
  settings.setValue( AQ_KEYBASE + "DBA/password", toQString( lineEditPassword->text() ) );
  settings.setValue( AQ_KEYBASE + "DBA/port", toQString( lineEditPort->text() ) );
  settings.setValue( AQ_KEYBASE + "DBA/hostname", toQString( lineEditHost->text() ) );
  settings.setValue( AQ_KEYBASE + "DBA/db", toQString( comboBoxDB->currentText() ) );
  settings.setValue( AQ_KEYBASE + "DBA/lastDB", toQString( comboBoxNameDB->currentText() ) );

  QStringList names;
  QString dbN;
  for ( int i = 0; i < comboBoxNameDB->count(); ++i ) {
    dbN = toQString( comboBoxNameDB->itemText( i ) );
    if ( !dbN.isEmpty() && !names.contains( dbN ) )
      names << dbN;
  }

  dbN = toQString( comboBoxNameDB->currentText() );
  if ( !dbN.isEmpty() && !names.contains( dbN ) )
    names << dbN;

  settings.setValue( AQ_KEYBASE + "DBA/namesDB", names );

  q->error_ = false;

  q->accept();
}

void AQConnectDBWidget::addMark()
{
  QString datConn;
  QString driverAlias = toQString( comboBoxDB->currentText() );

  if ( comboBoxNameDB->currentText().empty() ) {
    AQ_MSGBOX_WARN( AQCoreUtil::translate( "ConnectDB", "Agregar marcador" ),
                    AQCoreUtil::translate( "ConnectDB", "Establezca el nombre de la base de datos" ) );
    return ;
  }
  datConn = toQString( comboBoxNameDB->currentText() );

  datConn += ":" + toQString( lineEditUser->text() );
  datConn += ":" + driverAlias;

  if ( AQSqlDatabase::needConnOption( driverAlias, AQSql::HOST ) && lineEditHost->text().empty() ) {
    AQ_MSGBOX_WARN( AQCoreUtil::translate( "ConnectDB", "Agregar marcador" ),
                    AQCoreUtil::translate( "ConnectDB", "Establezca el nombre del servidor" ) );
    lineEditHost->setFocus();
    return ;
  }
  datConn += ":" + toQString( lineEditHost->text() );

  if ( AQSqlDatabase::needConnOption( driverAlias, AQSql::PORT ) && lineEditPort->text().empty() ) {
    AQ_MSGBOX_WARN( AQCoreUtil::translate( "ConnectDB", "Agregar marcador" ),
                    AQCoreUtil::translate( "ConnectDB", "Establezca el número de puerto" ) );
    lineEditPort->setFocus();
    return ;
  }
  datConn += ":" + toQString( lineEditPort->text() );
  AQ_INVOKER<AQConnectDBWidget, AQNoClass, QString> ivk( this->thread() );
  ivk.invoke( this, &AQConnectDBWidget::addActionMark, datConn );

  QSettings settings( QSettings::UserScope, "InfoSiAL" );
  settings.setValue( AQ_KEYBASE + "DBA/marks", marks_ );
}

void AQConnectDBWidget::removeMark()
{
  if ( comboBoxMarks->count() )
    removeActionMark( toQString( comboBoxMarks->currentText() ) );

  QSettings settings( QSettings::UserScope, "InfoSiAL" );
  settings.setValue( AQ_KEYBASE + "DBA/marks", marks_ );
}

void AQConnectDBWidget::selectMark( const QString & mark )
{
  if ( mark.isEmpty() )
    return ;

  QStringList dat = mark.split( ":" );
  int i = 0;
  for ( QStringList::const_iterator it = dat.begin(); it != dat.end(); ++it, ++i ) {
    switch ( i ) {
      case 0:
        comboBoxNameDB->setText( toWString( *it ) );
        break;
      case 1:
        lineEditUser->setText( toWString( *it ) );
        break;
      case 2: {
          WString txt( toWString( *it ) );
          for ( int i = 0; i < comboBoxDB->count(); ++i ) {
            if ( comboBoxDB->itemText( i ) == txt ) {
              comboBoxDB->setCurrentIndex( i );
              break;
            }
          }
        }
        break;
      case 3:
        lineEditHost->setText( toWString( *it ) );
        break;
      case 4:
        lineEditPort->setText( toWString( *it ) );
        break;
    }
  }
}

void AQConnectDBWidget::selectMarkByIndex( int index )
{
  selectMark( toQString( comboBoxMarks->itemText( index ) ) );
}

void AQConnectDBWidget::tryConnectByAction( QAction * action )
{
  selectMark( action->text() );
  tryConnect();
}

AQConnectDB::AQConnectDB( const QString & connectionName ) :
    WQDialog(), widget_( 0 ), connectionName_( connectionName ), error_( false )
{}

WQWidget * AQConnectDB::widget()
{
  widget_ = new AQConnectDBWidget( this, connectionName_ );
  return widget_;
}

void AQConnectDB::tryConnect()
{
  Q_ASSERT( widget_ );

  widget_->tryConnect();
}
