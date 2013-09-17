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

#include <QSignalMapper>
#include <QSettings>
#include <QMenu>
#include <QPaintEvent>
#include <QPainter>

#include <AQCore/AQCore.h>
#include <AQSql/AQSql.h>
#include <AQSql/AQSqlDatabase.h>
#include <AQSql/AQSqlConnections.h>

#include "AQConnectDB.h"
#include "../util/AQUiUtil.h"
#include "../AQApplication.h"

AQConnectDB::AQConnectDB( const QString & connectionName, QWidget * parent, Qt::WindowFlags f ) :
    QDialog( parent, f ),
    error_( false ),
    connectionName_( connectionName )
{
  setupUi( this );

  layout() ->setSizeConstraint( QLayout::SetFixedSize );
  frMore->hide();

  QSettings settings( QSettings::UserScope, "InfoSiAL" );

  comboBoxMarks->insertItems( 0, settings.value( AQ_KEYBASE + "DBA/marks" ).toStringList() );
  lineEditUser->setText( settings.value( AQ_KEYBASE + "DBA/username" ).toString() );
  lineEditPassword->setText( settings.value( AQ_KEYBASE + "DBA/password" ).toString() );
  lineEditPort->setText( settings.value( AQ_KEYBASE + "DBA/port", "5432" ).toString() );
  lineEditHost->setText( settings.value( AQ_KEYBASE + "DBA/hostname", "localhost" ).toString() );
  comboBoxNameDB->clear();
  comboBoxNameDB->insertItems( 0, settings.value( AQ_KEYBASE + "DBA/namesDB" ).toStringList() );
  comboBoxNameDB->setCurrentText( settings.value( AQ_KEYBASE + "DBA/lastDB", "abanq" ).toString() );
  lineEditUser->setFocus();

  connect( comboBoxNameDB, SIGNAL( activated( const QString & ) ), this, SLOT( changeDB( const QString & ) ) );

  comboBoxDB->insertItems( 0, AQSqlDatabase::driverAliases() );
  comboBoxDB->setCurrentText( settings.value( AQ_KEYBASE + "DBA/db", AQSqlDatabase::defaultAlias() ).toString() );
  driverChanged( comboBoxDB->currentText() );

  logo.load( ":/images/logo_abanq.png" );

  pbMarks->setEnabled( comboBoxMarks->count() );
}

void AQConnectDB::tryConnect()
{
  if ( !AQSqlConnections::addDatabase( comboBoxDB->currentText(), comboBoxNameDB->currentText(),
                                       lineEditUser->text(), lineEditPassword->text(),
                                       lineEditHost->text(), lineEditPort->text().toInt(),
                                       connectionName_ ) ) {
    error_ = true;
    this->accept();
    return ;
  }

  QSettings settings( QSettings::UserScope, "InfoSiAL" );

  settings.setValue( AQ_KEYBASE + "DBA/username", lineEditUser->text() );
  settings.setValue( AQ_KEYBASE + "DBA/password", lineEditPassword->text() );
  settings.setValue( AQ_KEYBASE + "DBA/port", lineEditPort->text() );
  settings.setValue( AQ_KEYBASE + "DBA/hostname", lineEditHost->text() );
  settings.setValue( AQ_KEYBASE + "DBA/db", comboBoxDB->currentText() );
  settings.setValue( AQ_KEYBASE + "DBA/lastDB", comboBoxNameDB->currentText() );

  QStringList names;
  QString dbN;
  for ( int i = 0; i < comboBoxNameDB->count(); ++i ) {
    dbN = comboBoxNameDB->text( i );
    if ( !dbN.isEmpty() && !names.contains( dbN ) )
      names << dbN;
  }

  dbN = comboBoxNameDB->currentText();
  if ( !dbN.isEmpty() && !names.contains( dbN ) )
    names << dbN;

  comboBoxNameDB->clear();
  comboBoxNameDB->insertItems( 0, names );

  settings.setValue( AQ_KEYBASE + "DBA/namesDB", names );

  error_ = false;

  this->accept();
}

void AQConnectDB::cancel()
{
  this->reject();
}

void AQConnectDB::driverChanged( const QString & driverAlias )
{
  lineEditUser->setEnabled( AQSqlDatabase::needConnOption( driverAlias, AQSql::USER ) );
  lineEditPassword->setEnabled( AQSqlDatabase::needConnOption( driverAlias, AQSql::PASSWORD ) );
  lineEditPort->setEnabled( AQSqlDatabase::needConnOption( driverAlias, AQSql::PORT ) );
  lineEditHost->setEnabled( AQSqlDatabase::needConnOption( driverAlias, AQSql::HOST ) );
}

void AQConnectDB::changeDB( const QString & DB )
{
  int i = 0;
  bool found = false;
  for ( i = 0; i < comboBoxNameDB->count(); ++i ) {
    if ( comboBoxNameDB->text( i ) == DB ) {
      found = true;
      break;
    }
  }

  if ( found )
    comboBoxNameDB->setCurrentItem( i );
  else {
    comboBoxNameDB->insertItem( DB );
    comboBoxNameDB->setCurrentItem( comboBoxNameDB->count() - 1 );
  }
}

void AQConnectDB::addMark()
{
  QString datConn;
  QString driverAlias = comboBoxDB->currentText();

  if ( comboBoxNameDB->currentText().isEmpty() ) {
    AQ_MSGBOX_WARN( tr( "Agregar marcador" ),
                    tr( "Establezca el nombre de la base de datos" ) );
    comboBoxNameDB->setFocus();
    return ;
  }
  datConn = comboBoxNameDB->currentText();

  datConn += ":" + lineEditUser->text();
  datConn += ":" + driverAlias;

  if ( AQSqlDatabase::needConnOption( driverAlias, AQSql::HOST ) && lineEditHost->text().isEmpty() ) {
    AQ_MSGBOX_WARN( tr( "Agregar marcador" ),
                    tr( "Establezca el nombre del servidor" ) );
    lineEditHost->setFocus();
    return ;
  }
  datConn += ":" + lineEditHost->text();

  if ( AQSqlDatabase::needConnOption( driverAlias, AQSql::PORT ) && lineEditPort->text().isEmpty() ) {
    AQ_MSGBOX_WARN( tr( "Agregar marcador" ),
                    tr( "Establezca el número de puerto" ) );
    lineEditPort->setFocus();
    return ;
  }
  datConn += ":" + lineEditPort->text();

  QStringList names;
  QString mkN;
  for ( int i = 0; i < comboBoxMarks->count(); ++i ) {
    mkN = comboBoxMarks->text( i );
    if ( !mkN.isEmpty() && !names.contains( mkN ) )
      names << mkN;
  }

  if ( !names.contains( datConn ) )
    names << datConn;

  comboBoxMarks->clear();
  comboBoxMarks->insertItems( 0, names );
  comboBoxMarks->setCurrentText( datConn );

  QSettings settings( QSettings::UserScope, "InfoSiAL" );

  settings.setValue( AQ_KEYBASE + "DBA/marks", names );

  pbMarks->setEnabled( comboBoxMarks->count() );
}

void AQConnectDB::removeMark()
{
  comboBoxMarks->removeItem( comboBoxMarks->currentItem() );

  QSettings settings( QSettings::UserScope, "InfoSiAL" );

  QStringList names;
  for ( int i = 0; i < comboBoxMarks->count(); ++i )
    names << comboBoxMarks->text( i );

  settings.setValue( AQ_KEYBASE + "DBA/marks", names );

  pbMarks->setEnabled( comboBoxMarks->count() );
}

void AQConnectDB::selectMark( const QString & mark )
{
  if ( mark.isEmpty() )
    return ;

  QStringList dat = mark.split( ":" );
  int i = 0;
  for ( QStringList::const_iterator it = dat.begin(); it != dat.end(); ++it, ++i ) {
    switch ( i ) {
    case 0:
      comboBoxNameDB->setCurrentText( *it );
      break;
    case 1:
      lineEditUser->setText( *it );
      break;
    case 2:
      comboBoxDB->setCurrentText( *it );
      break;
    case 3:
      lineEditHost->setText( *it );
      break;
    case 4:
      lineEditPort->setText( *it );
      break;
    }
  }
}

void AQConnectDB::popupMarks()
{
  if ( !comboBoxMarks->count() )
    return ;

  QMenu * marks = new QMenu( tr( "Marcadores" ), this );
  QActionGroup * ag = new QActionGroup( this );
  QSignalMapper * marksMapper = new QSignalMapper( this );

  QStringList list;
  for ( int i = 0; i < comboBoxMarks->count(); ++i )
    list << comboBoxMarks->text( i );

  for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
    QAction * a = new QAction( *it, ag );
    connect( a, SIGNAL( triggered() ), marksMapper, SLOT( map() ) );
    marksMapper->setMapping( a, *it );
  }

  connect( marksMapper, SIGNAL( mapped( const QString & ) ), this,
           SLOT( tryConnectMark( const QString & ) ) );

  marks->addActions( ag->actions() );
  marks->popup( QCursor::pos() );
}

void AQConnectDB::tryConnectMark( const QString & mark )
{
  selectMark( mark );
  tryConnect();
}

void AQConnectDB::paintEvent( QPaintEvent * pe )
{
  QRect ur = pe->rect();
  QPainter p( this );
  p.fillRect( ur, QColor( logo.pixel( 1, 1 ) ) );

  int dx = width() - logo.width();
  int dy = height() - logo.height();
  p.drawImage ( QPoint( dx, dy ), logo );

  p.setPen( QColor( 0, 0, 32 ) );
  p.setBrush( QColor( 0, 0, 32 ) );
  p.drawRect( dx + 32, 0, 4, dy );

  p.end();

  QWidget::paintEvent( pe );
}

void AQConnectDB::done( int r )
{
  if ( r == Accepted && error_ ) {
    setResult( r );
    Q_EMIT finished( r );
    Q_EMIT accepted();
  } else
    QDialog::done( r );
}
