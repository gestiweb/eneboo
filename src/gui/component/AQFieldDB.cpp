/***************************************************************************
AQFieldDB.cpp
-------------------
begin                : 28/08/2007
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

#include <math.h>

#include <QKeyEvent>
#include <QByteArray>
#include <QPixmap>
#include <QFocusEvent>
#include <QEvent>
#include <QSignalMapper>
#include <QMenu>
#include <QImageWriter>
#include <QComboBox>
#include <QCalendarWidget>
#include <QPixmapCache>
#include <QCheckBox>
#include <QBuffer>
#include <QFileDialog>
#include <QDateEdit>
#include <QTimeEdit>
#include <QTimer>

#include <Q3Accel>
#include <Q3TextEdit>
#include <Q3SqlFieldInfo>

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

#include "AQFieldDB.h"
#include "AQLineEdit.h"
#include "AQValidator.h"
#include "AQImageView.h"
#include "../AQAction.h"
#include "../mainwindow/AQFormDB.h"

AQFieldDB::AQFieldDB( QWidget * parent ) :
    AQComponentDB( parent ), editor_( 0 ), showAlias_( true ), calendar_( 0 ), accel_( 0 ), keepDisabled_( false ),
    editorImg_( 0 ), pbAux_( 0 ), pbAux2_( 0 ), pbAux3_( 0 ), showEditor_( true ), firstRefresh_( true ),
    refreshType_( -1 )
{
  setObjectName( "AQFieldDB" );
  setFocusPolicy( Qt::StrongFocus );
}

void AQFieldDB::setAQAction( AQSharedAction act )
{
  if ( act_ && !act ) {
    disconnect( this, 0, 0, 0 );
    if ( editor_ )
      disconnect( editor_, 0, this, 0 );
  }

  act_ = act;
}

AQSharedAction AQFieldDB::aqAction() const
{
  return act_;
}

void AQFieldDB::setupUi( AQFormDB * form )
{
  AQComponentDB::setupUi( form );
}

void AQFieldDB::linkUi()
{
  initAQAction();
}

void AQFieldDB::initRefreshUi()
{
  firstRefresh_ = false;
}

AQFieldDB::~AQFieldDB()
{}

bool AQFieldDB::eventFilter( QObject * obj, QEvent * ev )
{
  QEvent::Type evType = ev->type();

  if ( evType > AQCoreEvent::MinInternal ) {

    switch ( evType ) {
      case AQCoreEvent::InitNow: {
          if ( firstRefresh_ )
            return false;
          firstRefresh();
        }
        break;
    }

    return true;
  }

  if ( evType == QEvent::KeyPress ) {
    QKeyEvent * k = static_cast<QKeyEvent *> ( ev );

    if ( k->key() == Qt::Key_Enter || k->key() == Qt::Key_Return ) {
      focusNextPrevChild( true );
      return true;
    }

    if ( k->key() == Qt::Key_Up ) {
      focusNextPrevChild( false );
      return true;
    }

    if ( k->key() == Qt::Key_Down ) {
      focusNextPrevChild( true );
      return true;
    }

    if ( k->key() == Qt::Key_F2 ) {
      Q_EMIT keyF2Pressed();
      return true;
    }

    if ( k->key() == Qt::Key_Plus ) {
      Q_EMIT keyF2Pressed();
      return true;
    }
    return false;
  } else
    return false;
}

void AQFieldDB::showEvent( QShowEvent * ev )
{
  if ( !ev->spontaneous() )
    firstRefresh();
  QWidget::showEvent( ev );
}

void AQFieldDB::updateValue( const QDate & d )
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  if ( !cursor_->isValid() )
    return ;

  bool isNull = ( !d.isValid() || d.isNull() );

  if ( !cursor_->bufferIsNull( fieldName_ ) ) {
    if ( d == cursor_->valueBuffer( fieldName_ ).toDate() )
      return ;
  } else if ( isNull )
    return ;

  if ( isNull )
    cursor_->setValueBuffer( fieldName_, QVariant() );
  else
    cursor_->setValueBuffer( fieldName_, d );
}

void AQFieldDB::updateValue( const QTime & t )
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  if ( !cursor_->isValid() )
    return ;

  bool isNull = ( !t.isValid() || t.isNull() );

  if ( !cursor_->bufferIsNull( fieldName_ ) ) {
    if ( t == cursor_->valueBuffer( fieldName_ ).toTime() )
      return ;
  } else if ( isNull )
    return ;

  if ( isNull )
    cursor_->setValueBuffer( fieldName_, QVariant() );
  else
    cursor_->setValueBuffer( fieldName_, t );
}

void AQFieldDB::updateValue( bool b )
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  if ( !cursor_->isValid() )
    return ;

  if ( !cursor_->bufferIsNull( fieldName_ ) && b == cursor_->valueBuffer( fieldName_ ).toBool() )
    return ;

  cursor_->setValueBuffer( fieldName_, QVariant( b ) );
}

void AQFieldDB::updateValue( const QString & t )
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  if ( !cursor_->isValid() )
    return ;

  const AQTableMD * tMD = cursor_->metadata();
  AQFieldMD * field = tMD->field( fieldName_ );

  Q_ASSERT( field );

  QStringList ol( field->optionsList() );
  QString tAux( t );

  if ( !ol.isEmpty() && editor_ )
    tAux = ol.at( static_cast<QComboBox *> ( editor_ ) ->currentItem() );

  if ( !cursor_->bufferIsNull( fieldName_ ) ) {
    if ( tAux == cursor_->valueBuffer( fieldName_ ).toString() )
      return ;
  } else if ( tAux.isEmpty() )
    return ;

  QString s( tAux );

  if ( field->type() == QVariant::String && ol.empty() ) {
    if ( s.startsWith( " " ) ) {
      disconnect( cursor_, SIGNAL( bufferChanged( const QString & ) ),
                  this, SLOT( refreshQuick( const QString & ) ) );
      cursor_->setValueBuffer( fieldName_, s.remove( 0, 1 ) );
      connect( cursor_, SIGNAL( bufferChanged( const QString & ) ),
               this, SLOT( refreshQuick( const QString & ) ) );
      return ;
    }

    s.remove( "\\" );
    s.replace( "'", "\'" );
  }

  if ( s.isEmpty() )
    cursor_->setValueBuffer( fieldName_, QVariant() );
  else
    cursor_->setValueBuffer( fieldName_, s );

  if ( isVisible() && hasFocus() && field->type() == QVariant::String && field->length() == s.length() )
    focusNextPrevChild( true );
}

void AQFieldDB::updateValue()
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  if ( !cursor_->isValid() || !qobject_cast<Q3TextEdit *> ( editor_ ) )
    return ;

  QString t( static_cast<Q3TextEdit *> ( editor_ ) ->text() );

  if ( !cursor_->bufferIsNull( fieldName_ ) ) {
    if ( t == cursor_->valueBuffer( fieldName_ ).toString() )
      return ;
  } else if ( t.isEmpty() )
    return ;

  t.remove( "\\" );
  t.replace( "'", "\'" );

  if ( t.isEmpty() )
    cursor_->setValueBuffer( fieldName_, QVariant() );
  else
    cursor_->setValueBuffer( fieldName_, t );
}

void AQFieldDB::setTextFormat( int f )
{
  if ( editor_ ) {
    Q_ASSERT( act_ );

    AQFieldMD * field = act_->cursor() ->metadata() ->field( fieldName_ );

    if ( field && field->type() == QVariant::StringList )
      static_cast<Q3TextEdit *> ( editor_ ) ->setTextFormat( ( Qt::TextFormat ) f );
  }
}

int AQFieldDB::textFormat() const
{
  if ( editor_ ) {
    Q_ASSERT( act_ );

    AQFieldMD * field = act_->cursor() ->metadata() ->field( fieldName_ );

    if ( field && field->type() == QVariant::StringList )
      return static_cast<Q3TextEdit *> ( editor_ ) ->textFormat();
  }

  return Qt::PlainText;
}

void AQFieldDB::setValue( const QVariant & val )
{
  Q_ASSERT( act_ );

  const AQTableMD * tMD = act_->cursor() ->metadata();
  AQFieldMD * field = tMD->field( fieldName_ );

  Q_ASSERT( field );

  if ( !field->optionsList().empty() ) {
    static_cast<QComboBox *> ( editor_ ) ->setCurrentItem( val.toInt() );
    updateValue( static_cast<QComboBox *> ( editor_ ) ->currentText() );
    return ;
  }

  QVariant v( val );
  int type = field->type();
  QVariant::Type aqtype = AQFieldMD::aqDecodeType( type );
  if ( v.type() == QVariant::Bool && aqtype != QVariant::Bool ) {
    if ( type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt )
      v = 0;
    else
      v.clear();
  }

  if ( v.type() == QVariant::String && v.toString().isEmpty() )
    if ( type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt )
      v.clear();

  bool isNull = ( !v.isValid() || v.isNull() );

  if ( isNull && !field->allowNull() ) {
    QVariant defVal( field->defaultValue() );
    if ( defVal.isValid() && !defVal.isNull() ) {
      v = defVal;
      isNull = false;
    }
  }

  v.cast( aqtype );

  switch ( type ) {
    case QVariant::UInt:
    case QVariant::Int:
    case QVariant::String:
      if ( editor_ ) {
        bool doHome = ( static_cast<AQLineEdit *> ( editor_ ) ->text().isEmpty() );
        static_cast<AQLineEdit *> ( editor_ ) ->setText( isNull ? QString() : v.toString() );
        if ( doHome )
          static_cast<AQLineEdit *> ( editor_ ) ->home( false );
      }
      break;
    case QVariant::StringList:
      if ( !editor_ )
        return ;
      static_cast<Q3TextEdit *> ( editor_ ) ->setText( isNull ? QString() : v.toString() );
      break;
    case QVariant::Double:
      if ( editor_ ) {
        QString s;
        if ( !isNull )
          s.setNum( v.toDouble(), 'f', field->partDecimal() );
        static_cast<AQLineEdit *> ( editor_ ) ->setText( s );
      }
      break;
    case AQCore::Serial:
      if ( !editor_ )
        return ;
      static_cast<AQSpinBox *> ( editor_ ) ->setValue( isNull ? 0 : v.toUInt() );
      break;
    case QVariant::Pixmap:
      if ( editorImg_ ) {
        QByteArray cs;
        if ( !isNull )
          cs = v.toByteArray();
        if ( cs.isEmpty() ) {
          editorImg_->clear();
          return ;
        }
        QPixmap pix;
        if ( !QPixmapCache::find( cs.left( 100 ), pix ) ) {
          pix.loadFromData( cs );
          QPixmapCache::insert( cs.left( 100 ), pix );
        }
        if ( !pix.isNull() )
          editorImg_->setImage( pix );
        else
          editorImg_->clear();
      }
      break;
    case QVariant::Date:
      if ( !editor_ )
        return ;
      static_cast<QDateEdit *> ( editor_ ) ->setDate( isNull ? QDate() : v.toDate() );
      break;
    case QVariant::Time:
      static_cast<QTimeEdit *> ( editor_ ) ->setTime( isNull ? QTime() : v.toTime() );
      break;
    case QVariant::Bool:
      if ( editor_ && !isNull )
        static_cast<QCheckBox *> ( editor_ ) ->setChecked( v.toBool() );
      break;
  }
}

QVariant AQFieldDB::value()
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  if ( !cursor_->isValid() )
    return QVariant();

  const AQTableMD * tMD = cursor_->metadata();
  AQFieldMD * field = tMD->field( fieldName_ );

  Q_ASSERT( field );

  QVariant v;

  if ( !field->optionsList().empty() ) {
    v = static_cast<QComboBox *> ( editor_ ) ->currentItem();
    v.cast( QVariant::Int );
    return v;
  }

  int type = field->type();
  QVariant::Type aqtype = AQFieldMD::aqDecodeType( type );
  if ( cursor_->bufferIsNull( fieldName_ ) )
    if ( type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt )
      return 0;

  switch ( type ) {
    case QVariant::UInt:
    case QVariant::Int:
    case QVariant::Double:
    case QVariant::String:
    case QVariant::StringList:
      if ( editor_ )
        v = static_cast<AQLineEdit *> ( editor_ ) ->text();
      break;
    case AQCore::Serial:
      if ( editor_ )
        v = static_cast<AQSpinBox *> ( editor_ ) ->value();
      break;
    case QVariant::Pixmap:
      v = cursor_->valueBuffer( fieldName_ );
      break;
    case QVariant::Date:
      if ( editor_ )
        v = static_cast<QDateEdit *> ( editor_ ) ->date().toString( Qt::ISODate );
      break;
    case QVariant::Time:
      if ( editor_ )
        v = static_cast<QTimeEdit *> ( editor_ ) ->time().toString( Qt::ISODate );
      break;
    case QVariant::Bool:
      if ( editor_ )
        v = QVariant( static_cast<QCheckBox *> ( editor_ ) ->isChecked(), 0 );
      break;
  }

  v.cast( aqtype );
  return v;
}

void AQFieldDB::selectAll()
{
  if ( editor_ ) {
    Q_ASSERT( act_ );

    AQFieldMD * field = act_->cursor() ->metadata() ->field( fieldName_ );

    Q_ASSERT( field );

    switch ( field->type() ) {
      case QVariant::UInt:
      case QVariant::Int:
      case QVariant::Double:
      case QVariant::String:
        static_cast<AQLineEdit *> ( editor_ ) ->selectAll();
        break;
      case AQCore::Serial:
        static_cast<AQSpinBox *> ( editor_ ) ->selectAll();
        break;
    }
  }
}

void AQFieldDB::initAQAction()
{
  Q_ASSERT( act_ );

  if ( !tableName_.isEmpty() && foreignField_.isEmpty() && fieldRelation_.isEmpty() ) {
    refreshType_ = 1;
    act_ = AQAction::create( tableName_, act_->cursor() ->db().connectionName() );
    return ;
  }

  if ( tableName_.isEmpty() && !foreignField_.isEmpty() && !fieldRelation_.isEmpty() ) {
    refreshType_ = 2;
    return ;
  }

  if ( !tableName_.isEmpty() && !foreignField_.isEmpty() && !fieldRelation_.isEmpty() ) {
    refreshType_ = 3;
    AQRelationMD * relMasterToDetail = new AQRelationMD( tableName_, fieldRelation_, AQCore::RELATION_1M );
    relMasterToDetail->setField( foreignField_ );

    act_ = AQAction::create( act_, relMasterToDetail, act_->cursor() ->db().connectionName() );
  }
}

void AQFieldDB::initEditor()
{
  if ( editor_ ) {
    delete editor_;
    editor_ = 0;
  }

  if ( editorImg_ ) {
    delete editorImg_;
    editorImg_ = 0;
  }

  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  const AQTableMD * tMD = cursor_->metadata();
  AQFieldMD * field = tMD->field( fieldName_ );

  if ( !field )
    return ;

  int type = field->type();
  int len = field->length();
  int partInteger = field->partInteger();
  int partDecimal = field->partDecimal();
  QString rX = field->regExpValidator();
  QStringList ol = field->optionsList();

  QString rt;
  if ( field->relationM1() && field->relationM1() ->foreignTable() != tMD->name() )
    rt = field->relationM1() ->foreignTable();

  bool hasPushButtonDB = false;
  fieldAlias_ = field->alias();

  if ( showAlias_ && type != QVariant::Pixmap && type != QVariant::Bool )
    ui.textLabelDB->setText( fieldAlias_ );
  else
    ui.textLabelDB->hide();

  if ( !rt.isEmpty() ) {
    hasPushButtonDB = true;
    if ( !cursor_->db().manager() ->metadata( rt ) )
      ui.pushButtonDB->setDisabled( true );
  }

  QSizePolicy sizePolicy_( QSizePolicy::Expanding, QSizePolicy::Fixed );
  QSizePolicy sizePolicy2_( QSizePolicy::Fixed, QSizePolicy::Fixed );

  switch ( type ) {
    case QVariant::UInt:
    case QVariant::Int:
    case QVariant::Double:
    case QVariant::String:
      if ( !ol.empty() ) {
        editor_ = new QComboBox( this );

        static_cast<QComboBox *> ( editor_ ) ->setEditable( false );

        QStringList olTranslated;
        for ( QStringList::const_iterator it = ol.begin(); it != ol.end(); ++it )
          olTranslated.append( AQ_TR( "MetaData", *it ) );
        static_cast<QComboBox *> ( editor_ ) ->insertItems( 0, olTranslated );

        editor_->installEventFilter( this );
        connect( editor_, SIGNAL( activated( const QString & ) ),
                 this, SLOT( updateValue( const QString & ) ) );
        sizePolicy_.setControlType( QSizePolicy::ComboBox );
      } else {
        editor_ = new AQLineEdit( this );

        static_cast<AQLineEdit *> ( editor_ ) ->type = type;
        editor_->installEventFilter( this );

        if ( type == QVariant::Double ) {
          static_cast<AQLineEdit *> ( editor_ ) -> setValidator( new AQDoubleValidator( 0, pow( 10, partInteger ) - 1,
              partDecimal, editor_ ) );
          static_cast<AQLineEdit *> ( editor_ ) ->setAlignment( Qt::AlignRight );
        } else {
          if ( type == QVariant::UInt || type == QVariant::Int ) {
            if ( type == QVariant::UInt )
              static_cast<AQLineEdit *> ( editor_ ) -> setValidator( new AQUIntValidator( 0,
                  ( ( int ) pow( 10, partInteger ) - 1 ), editor_ ) );
            else
              static_cast<AQLineEdit *> ( editor_ ) -> setValidator( new AQIntValidator( ( ( int ) ( pow( 10, partInteger ) - 1 ) * ( -1 ) ),
                  ( ( int ) pow( 10, partInteger ) - 1 ), editor_ ) );
            static_cast<AQLineEdit *> ( editor_ ) ->setAlignment( Qt::AlignRight );
          } else {
            static_cast<AQLineEdit *> ( editor_ ) ->setMaxLength( len );
            if ( !rX.isEmpty() ) {
              QRegExp r( rX );
              static_cast<AQLineEdit *> ( editor_ ) -> setValidator( new QRegExpValidator( r, editor_ ) );
            }
            static_cast<AQLineEdit *> ( editor_ ) ->setAlignment( Qt::AlignLeft );
          }
        }

        connect( editor_, SIGNAL( lostFocus() ), this, SLOT( emitLostFocus() ) );
        connect( editor_, SIGNAL( textChanged( const QString & ) ),
                 this, SLOT( updateValue( const QString & ) ) );
        connect( editor_, SIGNAL( textChanged( const QString & ) ),
                 this, SLOT( emitTextChanged( const QString & ) ) );
        if ( hasPushButtonDB )
          connect( this, SIGNAL( keyF2Pressed() ), ui.pushButtonDB, SLOT( animateClick() ) );
        sizePolicy_.setControlType( QSizePolicy::LineEdit );
      }
      editor_->setSizePolicy( sizePolicy_ );
      editor_->setMaximumSize( initMaxSize_.width(), height() );
      editor_->setMinimumSize( initMinSize_ );
      ui.lytMain->addWidget( editor_ );
      break;

    case AQCore::Serial:
      editor_ = new AQSpinBox( this );

      static_cast<AQSpinBox *> ( editor_ ) ->setMaximum( ( ( int ) pow( 10, partInteger ) - 1 ) );
      sizePolicy_.setControlType( QSizePolicy::SpinBox );
      editor_->setSizePolicy( sizePolicy_ );
      editor_->setMaximumSize( initMaxSize_.width(), height() );
      editor_->setMinimumSize( initMinSize_ );
      ui.lytMain->addWidget( editor_ );
      editor_->installEventFilter( this );

      connect( editor_, SIGNAL( valueChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      break;

    case QVariant::Pixmap:
      if ( cursor_->modeAccess() != AQSql::BROWSE ) {
        ui.lytMain->setDirection( QBoxLayout::TopToBottom );
        editorImg_ = new AQImageView( this );
        editorImg_->setFocusPolicy( Qt::NoFocus );
        editorImg_->setSizePolicy( sizePolicy() );
        editorImg_->setMaximumSize( initMaxSize_.width(), height() );
        editorImg_->setMinimumSize( initMinSize_ );
        editorImg_->setAutoScaled( true );
        ui.lytMain->addWidget( editorImg_ );

        if ( !pbAux3_ ) {
          QSpacerItem * spcBut = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
          ui.lytButtons->addItem( spcBut );
          pbAux3_ = new QPushButton( this );
          pbAux3_->setObjectName( "pbAux3" );
          sizePolicy2_.setControlType( QSizePolicy::PushButton );
          pbAux3_->setSizePolicy( sizePolicy2_ );
          pbAux3_->setMinimumSize( QSize( 22, 22 ) );
          pbAux3_->setFocusPolicy( Qt::NoFocus );
          pbAux3_->setIconSet( QIcon( ":/images/folder.png" ) );
          pbAux3_->setText( QString() );
          pbAux3_->setToolTip( tr( "Abrir fichero de imagen" ) );
          ui.lytButtons->addWidget( pbAux3_ );
          connect( pbAux3_, SIGNAL( clicked() ), this, SLOT( searchPixmap() ) );
          if ( !hasPushButtonDB ) {
            connect( this, SIGNAL( keyF2Pressed() ), pbAux3_, SLOT( animateClick() ) );
            pbAux3_->setFocusPolicy( Qt::StrongFocus );
            pbAux3_->installEventFilter( this );
          }
        }

        if ( !pbAux_ ) {
          pbAux_ = new QPushButton( this );
          pbAux_->setObjectName( "pbAux" );
          pbAux_->setFocusPolicy( Qt::NoFocus );
          pbAux_->setIconSet( QIcon( ":/images/no.png" ) );
          pbAux_->setText( QString() );
          pbAux_->setToolTip( tr( "Borrar imagen" ) );
          sizePolicy2_.setControlType( QSizePolicy::PushButton );
          pbAux_->setSizePolicy( sizePolicy2_ );
          pbAux_->setMinimumSize( QSize( 22, 22 ) );
          ui.lytButtons->addWidget( pbAux_ );
          connect( pbAux_, SIGNAL( clicked() ), this, SLOT( clearPixmap() ) );
        }

        if ( !pbAux2_ ) {
          pbAux2_ = new QPushButton( this );
          pbAux2_->setObjectName( "pbAux2" );
          QMenu * menuFormats = new QMenu( this );
          QSignalMapper * mapper = new QSignalMapper( this );
          QAction * a;
          QList<QByteArray> list( QImageWriter::supportedImageFormats() );

          int id = 0;
          for ( QList<QByteArray>::const_iterator it = list.begin(); it != list.end(); ++it, ++id ) {
            a = new QAction( *it, this );
            connect( a, SIGNAL( triggered() ), mapper, SLOT( map() ) );
            mapper->setMapping( a, id );
            menuFormats->addAction( a );
          }

          pbAux2_->setMenu( menuFormats );
          pbAux2_->setFocusPolicy( Qt::NoFocus );
          pbAux2_->setIconSet( QIcon( ":/images/filesaveas.png" ) );
          pbAux2_->setText( QString() );
          pbAux2_->setToolTip( tr( "Guardar imagen como..." ) );
          sizePolicy2_.setControlType( QSizePolicy::PushButton );
          pbAux2_->setSizePolicy( sizePolicy2_ );
          pbAux2_->setMinimumSize( QSize( 22, 22 ) );
          ui.lytButtons->addWidget( pbAux2_ );
          connect ( mapper, SIGNAL( mapped( int ) ), this, SLOT( savePixmap( int ) ) );
        }

        if ( hasPushButtonDB )
          ui.pushButtonDB->installEventFilter( this );
      }
      break;

    case QVariant::Date:
      editor_ = new QDateEdit( this );

      sizePolicy_.setControlType( QSizePolicy::LineEdit );
      editor_->setSizePolicy( sizePolicy_ );
      editor_->setMaximumSize( initMaxSize_.width(), height() );
      editor_->setMinimumSize( initMinSize_ );
      ui.lytMain->insertWidget( 1, editor_ );

      static_cast<QDateEdit *>( editor_ ) ->setDisplayFormat( AQ_FORMAT_D );
      static_cast<QDateEdit *>( editor_ ) ->setCalendarPopup( true );
      editor_->installEventFilter( this );

      disconnect( editor_, SIGNAL( dateChanged( const QDate & ) ), this, SLOT( updateValue( const QDate & ) ) );
      static_cast<QDateEdit *>( editor_ ) ->setDate( QDate::currentDate() );
      connect( editor_, SIGNAL( dateChanged( const QDate & ) ), this, SLOT( updateValue( const QDate & ) ) );
      break;

    case QVariant::Time:
      editor_ = new QTimeEdit( this );

      sizePolicy_.setControlType( QSizePolicy::LineEdit );
      editor_->setSizePolicy( sizePolicy_ );
      editor_->setMaximumSize( initMaxSize_.width(), height() );
      editor_->setMinimumSize( initMinSize_ );
      ui.lytMain->addWidget( editor_ );

      static_cast<QDateEdit *>( editor_ ) ->setDisplayFormat( AQ_FORMAT_T );
      editor_->installEventFilter( this );

      disconnect( editor_, SIGNAL( timeChanged( const QTime & ) ), this, SLOT( updateValue( const QTime & ) ) );
      static_cast<QTimeEdit *>( editor_ ) ->setTime( QTime::currentTime() );
      connect( editor_, SIGNAL( timeChanged( const QTime & ) ), this, SLOT( updateValue( const QTime & ) ) );
      break;

    case QVariant::StringList:
      editor_ = new Q3TextEdit( this );

      editor_->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
      editor_->setMaximumSize( initMaxSize_.width(), height() );
      editor_->setMinimumSize( initMinSize_ );
      static_cast<Q3TextEdit *>( editor_ ) ->setTabChangesFocus( true );
      ui.lytMain->addWidget( editor_ );

      connect( editor_, SIGNAL( textChanged() ), this, SLOT( updateValue() ) );
      break;

    case QVariant::Bool:
      editor_ = new QCheckBox( this );

      static_cast<QCheckBox *>( editor_ ) ->setText( tMD->fieldNameToAlias( fieldName_ ) );
      editor_->installEventFilter( this );

      sizePolicy_.setControlType( QSizePolicy::CheckBox );
      editor_->setSizePolicy( sizePolicy_ );
      editor_->setMaximumSize( initMaxSize_.width(), height() );
      editor_->setMinimumSize( initMinSize_ );
      ui.lytMain->addWidget( editor_ );
      connect( editor_, SIGNAL( toggled( bool ) ), this, SLOT( updateValue( bool ) ) );
      break;
  }

  if ( editor_ ) {
    editor_->hide();
    editor_->setFocusPolicy( Qt::StrongFocus );
    setFocusProxy( editor_ );
    if ( hasPushButtonDB )
      ui.pushButtonDB->setFocusPolicy( Qt::NoFocus );
  } else if ( editorImg_ ) {
    editorImg_->hide();
    editorImg_->setFocusPolicy( Qt::NoFocus );
    if ( hasPushButtonDB )
      ui.pushButtonDB->setFocusPolicy( Qt::StrongFocus );
  }

  if ( hasPushButtonDB && editor_ )
    editor_->setToolTip( tr( "Para buscar un valor en la tabla relacionada pulsar F2" ) );
  else if ( !hasPushButtonDB )
    ui.pushButtonDB->hide();

  if ( initMaxSize_.width() < 80 )
    setShowEditor( false );
  else
    setShowEditor( showEditor_ );
}

void AQFieldDB::searchValue()
{
  Q_ASSERT( act_ );

  firstRefresh();

  AQSharedAction actField = act_->aqActionField( fieldName_ );

  if ( actField ) {
    QVariant v( actField->exec( QString(), actionName_ ) );

    if ( v.isValid() && !v.isNull() ) {
      setValue( "" );
      setValue( v );
    }
  }
}

void AQFieldDB::searchPixmap()
{
  if ( !editorImg_ )
    return ;

  if ( fieldName_.isEmpty() )
    return ;

  Q_ASSERT( act_ );

  const AQTableMD * tMD = act_->cursor() ->metadata();
  AQFieldMD * field = tMD->field( fieldName_ );

  Q_ASSERT( field );

  if ( field->type() == QVariant::Pixmap ) {
    Q3FileDialog * fd = new Q3FileDialog( this, 0, true );
    AQImageView * p = new AQImageView( fd );

    p->setAutoScaled( true );
    fd->setContentsPreviewEnabled( TRUE );
    fd->setContentsPreview( p, p );
    fd->setPreviewMode( Q3FileDialog::Contents );
    fd->setCaption( tr( "Elegir archivo" ) );
    fd->setFilter( "*" );

    QString filename;

    if ( fd->exec() == QDialog::Accepted )
      filename = fd->selectedFile();

    if ( filename.isEmpty() )
      return ;

    QImage img( filename );

    if ( img.isNull() )
      return ;

    AbanQ->setOverrideCursor( Qt::waitCursor );

    QPixmap pix;
    QByteArray s;
    QBuffer buffer( &s, this );

    pix.convertFromImage( img );

    AbanQ->restoreOverrideCursor();

    if ( pix.isNull() )
      return ;

    editorImg_->setImage( pix );

    AbanQ->setOverrideCursor( Qt::waitCursor );

    buffer.open( QIODevice::WriteOnly );
    pix.save( &buffer, "XPM" );
    buffer.close();

    AbanQ->restoreOverrideCursor();

    if ( s.isEmpty() )
      return ;

    if ( !QPixmapCache::find( s.left( 100 ) ) )
      QPixmapCache::insert( s.left( 100 ), pix );

    updateValue( QString( s ) );
  }
}

void AQFieldDB::setFilter( const QString & f )
{
  filter_ = f;
}

QString AQFieldDB::fieldName() const
{
  return fieldName_;
}

QString AQFieldDB::actionName() const
{
  return actionName_;
}

QString AQFieldDB::tableName() const
{
  return tableName_;
}

QString AQFieldDB::foreignField() const
{
  return foreignField_;
}

QString AQFieldDB::fieldRelation() const
{
  return fieldRelation_;
}

void AQFieldDB::refreshQuick( const QString & fN )
{
  if ( fN.isEmpty() || fN != fieldName_ )
    return ;

  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  const AQTableMD * tMD = cursor_->metadata();
  AQFieldMD * field = tMD->field( fieldName_ );

  Q_ASSERT( field );

  if ( field->outTransaction() )
    return ;

  QVariant v( cursor_->valueBuffer( fieldName_ ) );
  bool null = cursor_->bufferIsNull( fieldName_ );
  int type = field->type();
  int partDecimal = field->partDecimal();
  QStringList ol( field->optionsList() );

  switch ( type ) {
    case QVariant::Double:
      if ( !editor_ )
        return ;
      if ( v.toDouble() == static_cast<AQLineEdit *> ( editor_ ) ->text().toDouble() )
        return ;
      disconnect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      {
        QString s;
        if ( !null )
          s.setNum( v.toDouble(), 'f', partDecimal );
        static_cast<AQLineEdit *> ( editor_ ) ->setText( s );
        static_cast<AQLineEdit *> ( editor_ ) ->home( false );
      }
      connect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      break;

    case QVariant::String: {
        if ( !editor_ )
          return ;
        bool doHome = false;
        if ( !ol.empty() ) {
          if ( v.toString() == static_cast<QComboBox *> ( editor_ ) ->currentText() )
            return ;
        } else {
          if ( v.toString() == static_cast<AQLineEdit *> ( editor_ ) ->text() )
            return ;
          doHome = ( static_cast<AQLineEdit *> ( editor_ ) ->text().isEmpty() );
        }
        disconnect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
        if ( v.isValid() && !v.isNull() ) {
          if ( !ol.empty() )
            static_cast<QComboBox *> ( editor_ ) ->setCurrentItem( ol.indexOf( v.toString() ) );
          else
            static_cast<AQLineEdit *> ( editor_ ) ->setText( v.toString() );
        } else {
          if ( !ol.empty() )
            static_cast<QComboBox *> ( editor_ ) ->setCurrentItem( 0 );
          else
            static_cast<AQLineEdit *> ( editor_ ) ->setText( QString() );
        }
        if ( ol.empty() && doHome )
          static_cast<AQLineEdit *> ( editor_ ) ->home( false );
        connect ( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      }
      break;

    case QVariant::UInt:
      if ( !editor_ )
        return ;
      if ( v.toUInt() == static_cast<AQLineEdit *>( editor_ ) ->text().toUInt() )
        return ;
      disconnect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      {
        QString s;
        if ( !null )
          s.setNum( v.toUInt() );
        static_cast<AQLineEdit *>( editor_ ) ->setText( s );
        static_cast<AQLineEdit *>( editor_ ) ->home( false );
      }
      connect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      break;

    case QVariant::Int:
      if ( !editor_ )
        return ;
      if ( v.toInt() == static_cast<AQLineEdit *>( editor_ ) ->text().toInt() )
        return ;
      disconnect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      {
        QString s;
        if ( !null )
          s.setNum( v.toInt() );
        static_cast<AQLineEdit *>( editor_ ) ->setText( s );
        static_cast<AQLineEdit *>( editor_ ) ->home( false );
      }
      connect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      break;

    case AQCore::Serial:
      if ( !editor_ )
        return ;
      if ( v.toInt() == static_cast<AQSpinBox *>( editor_ ) ->value() )
        return ;
      disconnect( editor_, SIGNAL( valueChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      static_cast<AQSpinBox *>( editor_ ) ->setValue( v.toInt() );
      connect( editor_, SIGNAL( valueChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      break;

    case QVariant::Pixmap: {
        if ( !editorImg_ ) {
          editorImg_ = new AQImageView( this );
          editorImg_->setFocusPolicy( Qt::NoFocus );
          editorImg_->setSizePolicy( sizePolicy() );
          editorImg_->setMaximumSize( initMaxSize_.width(), height() );
          editorImg_->setMinimumSize( initMinSize_ );
          editorImg_->setAutoScaled( true );
          ui.lytMain->addWidget( editorImg_ );
        }
        QByteArray cs;
        if ( !null )
          cs = v.toCString();
        if ( cs.isEmpty() ) {
          editorImg_->clear();
          return ;
        }
        QPixmap pix;
        if ( !QPixmapCache::find( cs.left( 100 ), pix ) ) {
          pix.loadFromData( cs );
          QPixmapCache::insert( cs.left( 100 ), pix );
        }
        if ( pix.isNull() )
          editorImg_->clear();
        else
          editorImg_->setImage( pix );
      }
      break;

    case QVariant::Date:
      if ( !editor_ )
        return ;
      if ( v.toDate() == static_cast<QDateEdit *>( editor_ ) ->date() )
        return ;
      disconnect( editor_, SIGNAL( dateChanged( const QDate & ) ), this, SLOT( updateValue( const QDate & ) ) );
      {
        QDate d( v.toDate() );
        if ( d.isValid() )
          static_cast<QDateEdit *>( editor_ ) ->setDate( v.toDate() );
        else
          static_cast<QDateEdit *>( editor_ ) ->setDate( QDate::currentDate() );
      }
      connect( editor_, SIGNAL( dateChanged( const QDate & ) ), this, SLOT( updateValue( const QDate & ) ) );
      break;

    case QVariant::Time:
      if ( !editor_ )
        return ;
      if ( v.toTime() == static_cast<QTimeEdit *>( editor_ ) ->time() )
        return ;
      disconnect( editor_, SIGNAL( timeChanged( const QTime & ) ), this, SLOT( updateValue( const QTime & ) ) );
      static_cast<QTimeEdit *>( editor_ ) ->setTime( v.toTime() );
      connect( editor_, SIGNAL( timeChanged( const QTime & ) ), this, SLOT( updateValue( const QTime & ) ) );
      break;

    case QVariant::StringList:
      if ( !editor_ )
        return ;
      if ( v.toString() == static_cast<Q3TextEdit *>( editor_ ) ->text() )
        return ;
      disconnect( editor_, SIGNAL( textChanged() ), this, SLOT( updateValue() ) );
      static_cast<Q3TextEdit *>( editor_ ) ->setText( v.toString() );
      connect( editor_, SIGNAL( textChanged() ), this, SLOT( updateValue() ) );
      break;

    case QVariant::Bool:
      if ( !editor_ )
        return ;
      if ( v.toBool() == static_cast<QCheckBox *>( editor_ ) ->isChecked() )
        return ;
      disconnect( editor_, SIGNAL( toggled( bool ) ), this, SLOT( updateValue( bool ) ) );
      static_cast<QCheckBox *>( editor_ ) ->setChecked( v.toBool() );
      connect( editor_, SIGNAL( toggled( bool ) ), this, SLOT( updateValue( bool ) ) );
      break;
  }
}

void AQFieldDB::refresh( const QString & fN )
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();
  const AQTableMD * tMD = cursor_->metadata();

  if ( !fN.isEmpty() ) {

    if ( refreshType_ == 2 && fN.lower() == fieldRelation_.lower() ) {

      if ( cursor_->bufferIsNull( fieldRelation_ ) )
        return ;

      AQFieldMD * field = tMD->field( fieldRelation_ );

      if ( !field || !field->relationM1() ) {
        aqWarn( "AQFieldDB : " + tr( "El campo %1 no puede tener una acción porque no se ha "
                                     "definido una relación M1 ( muchos a uno ) para él" ).arg( fieldRelation_ ) );
        return ;
      }

      QVariant v( cursor_->valueBuffer( fieldRelation_ ) );
      AQSqlQuery q( cursor_->db().connectionName() );

      q.setTablesList( field->relationM1() ->foreignTable() );
      q.setSelect( foreignField_ + "," + field->relationM1() ->foreignField() );
      q.setFrom( field->relationM1() ->foreignTable() );
      if ( filter_.isEmpty() )
        q.setWhere( cursor_->db().manager() ->formatAssignValue( field->relationM1() ->foreignField(), field, v, true ) );
      else
        q.setWhere( filter_ + " AND "
                    + cursor_->db().manager() ->formatAssignValue( field->relationM1() ->foreignField(), field, v, true ) );

      if ( q.exec() && q.next() ) {
        QVariant v0( q.value( 0 ) ), v1( q.value( 1 ) );
        if ( v0 != value() )
          setValue( v0 );
        if ( v1 != v )
          cursor_->setValueBuffer( fieldRelation_, v1 );
      }
    }

    return ;
  }

  AQFieldMD * field = cursor_->metadata() ->field( fieldName_ );

  if ( !field )
    return ;

  int modeAccess = cursor_->modeAccess();

  setDisabled( keepDisabled_ || cursor_->fieldDisabled( fieldName_ ) || ( modeAccess == AQSql::EDIT
               && ( field->isPrimaryKey() || !tMD->fieldListOfCompoundKey( fieldName_ ).isEmpty() ) ) || !field->editable()
               || modeAccess == AQSql::BROWSE );

  int type = field->type();
  int partDecimal = field->partDecimal();
  QStringList ol( field->optionsList() );
  QVariant v( cursor_->valueBuffer( fieldName_ ) );
  bool null = cursor_->bufferIsNull( fieldName_ );

  switch ( type ) {
    case QVariant::Double:
      if ( !editor_ )
        return ;
      disconnect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      {
        QString s;
        if ( !null )
          s.setNum( v.toDouble(), 'f', partDecimal );
        static_cast<AQLineEdit *> ( editor_ ) ->setText( s );
        static_cast<AQLineEdit *> ( editor_ ) ->home( false );
      }
      connect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      break;

    case QVariant::String: {
        if ( !editor_ )
          return ;
        bool doHome = false;
        if ( ol.empty() )
          doHome = ( static_cast<AQLineEdit *> ( editor_ ) ->text().isEmpty() );
        disconnect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
        if ( !null ) {
          if ( !ol.empty() )
            static_cast<QComboBox *> ( editor_ ) ->setCurrentItem( ol.indexOf( v.toString() ) );
          else
            static_cast<AQLineEdit *> ( editor_ ) ->setText( v.toString() );
        } else {
          if ( !ol.empty() )
            static_cast<QComboBox *> ( editor_ ) ->setCurrentItem( 0 );
          else
            static_cast<AQLineEdit *> ( editor_ ) ->setText( QString() );
        }
        if ( ol.empty() && doHome )
          static_cast<AQLineEdit *> ( editor_ ) ->home( false );
        connect ( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      }
      break;

    case QVariant::UInt:
      if ( !editor_ )
        return ;
      disconnect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      {
        QString s;
        if ( !null )
          s.setNum( v.toUInt() );
        static_cast<AQLineEdit *>( editor_ ) ->setText( s );
        static_cast<AQLineEdit *>( editor_ ) ->home( false );
      }
      connect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      break;

    case QVariant::Int:
      if ( !editor_ )
        return ;
      disconnect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      {
        QString s;
        if ( !null )
          s.setNum( v.toInt() );
        static_cast<AQLineEdit *>( editor_ ) ->setText( s );
        static_cast<AQLineEdit *>( editor_ ) ->home( false );
      }
      connect( editor_, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      break;

    case AQCore::Serial:
      if ( !editor_ )
        return ;
      disconnect( editor_, SIGNAL( valueChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      static_cast<AQSpinBox *>( editor_ ) ->setValue( 0 );
      connect( editor_, SIGNAL( valueChanged( const QString & ) ), this, SLOT( updateValue( const QString & ) ) );
      break;

    case QVariant::Pixmap:
      if ( !editorImg_ ) {
        editorImg_ = new AQImageView( this );
        editorImg_->setFocusPolicy( Qt::NoFocus );
        editorImg_->setSizePolicy( sizePolicy() );
        editorImg_->setMaximumSize( initMaxSize_.width(), height() );
        editorImg_->setMinimumSize( initMinSize_ );
        editorImg_->setAutoScaled( true );
        ui.lytMain->addWidget( editorImg_ );
      }
      if ( modeAccess == AQSql::BROWSE )
        setDisabled( false );
      if ( field->visible() ) {
        QByteArray cs;
        if ( !null )
          cs = v.toCString();
        if ( cs.isEmpty() ) {
          editorImg_->clear();
          return ;
        }
        QPixmap pix;
        if ( !QPixmapCache::find( cs.left( 100 ), pix ) ) {
          pix.loadFromData( cs );
          QPixmapCache::insert( cs.left( 100 ), pix );
        }
        if ( pix.isNull() )
          editorImg_->clear();
        else
          editorImg_->setImage( pix );
      }
      if ( modeAccess == AQSql::BROWSE )
        ui.pushButtonDB->setDisabled( true );
      break;

    case QVariant::Date:
      if ( !editor_ )
        return ;
      disconnect( editor_, SIGNAL( dateChanged( const QDate & ) ), this, SLOT( updateValue( const QDate & ) ) );
      {
        QDate d( v.toDate() );
        if ( ( null && !field->allowNull() ) || !d.isValid() )
          d = QDate::currentDate();
        static_cast<QDateEdit *>( editor_ ) ->setDate( d );
      }
      connect( editor_, SIGNAL( dateChanged( const QDate & ) ), this, SLOT( updateValue( const QDate & ) ) );
      if ( ( cursor_->modeAccess() == AQSql::INSERT && !field->allowNull() ) )
        static_cast<QDateEdit *>( editor_ ) ->setDate( QDate::currentDate() );
      break;

    case QVariant::Time:
      if ( !editor_ )
        return ;
      disconnect( editor_, SIGNAL( timeChanged( const QTime & ) ), this, SLOT( updateValue( const QTime & ) ) );
      {
        QTime t = v.toTime();
        if ( ( null && !field->allowNull() ) || !t.isValid() )
          t = QTime::currentTime();
        static_cast<QTimeEdit *>( editor_ ) ->setTime( t );
      }
      connect( editor_, SIGNAL( timeChanged( const QTime & ) ), this, SLOT( updateValue( const QTime & ) ) );
      if ( cursor_->modeAccess() == AQSql::INSERT && !field->allowNull() )
        static_cast<QTimeEdit *>( editor_ ) ->setTime( QTime::currentTime() );
      break;

    case QVariant::StringList:
      if ( !editor_ )
        return ;
      disconnect( editor_, SIGNAL( textChanged() ), this, SLOT( updateValue() ) );
      static_cast<Q3TextEdit *>( editor_ ) ->setText( v.toString() );
      connect( editor_, SIGNAL( textChanged() ), this, SLOT( updateValue() ) );
      break;

    case QVariant::Bool:
      if ( !editor_ )
        return ;
      disconnect( editor_, SIGNAL( toggled( bool ) ), this, SLOT( updateValue( bool ) ) );
      static_cast<QCheckBox *>( editor_ ) ->setChecked( v.toBool() );
      connect( editor_, SIGNAL( toggled( bool ) ), this, SLOT( updateValue( bool ) ) );
      break;
  }

  if ( !field->visible() ) {
    if ( editor_ )
      editor_->hide();
    if ( editorImg_ )
      editorImg_->hide();
    setDisabled( true );
  }
}

void AQFieldDB::setTableName( const QString & fT )
{
  tableName_ = fT;
}

void AQFieldDB::setForeignField( const QString & fN )
{
  foreignField_ = fN;
}

void AQFieldDB::setFieldRelation( const QString & fN )
{
  fieldRelation_ = fN;
}

void AQFieldDB::setFieldName( const QString & fN )
{
  fieldName_ = fN;
}

void AQFieldDB::setActionName( const QString & aN )
{
  actionName_ = aN;
}

void AQFieldDB::emitLostFocus()
{
  Q_EMIT lostFocus();
}

bool AQFieldDB::showAlias() const
{
  return showAlias_;
}

void AQFieldDB::setShowAlias( bool value )
{
  showAlias_ = value;
}

void AQFieldDB::setMapValue( const QString & v )
{
  AQFieldDB * s = qobject_cast<AQFieldDB *> ( sender() );
  if ( !s )
    return ;

  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  const AQTableMD * tMD = cursor_->metadata();
  QString fSN = static_cast<AQFieldDB *> ( s ) ->fieldName();
  AQFieldMD * field = tMD->field( fieldName_ );
  AQFieldMD * fieldSender = tMD->field( fSN );

  if ( !field || !fieldSender )
    return ;

  QString rt, fF;
  if ( field->relationM1() ) {
    if ( field->relationM1() ->foreignTable() != tMD->name() ) {
      rt = field->relationM1() ->foreignTable();
      fF = static_cast<AQFieldDB *> ( s ) ->foreignField();

      QVariant vv( v );
      AQSqlQuery q( cursor_->db().connectionName() );

      q.setTablesList( rt );
      q.setSelect( field->relationM1() ->foreignField() + "," + fF );
      q.setFrom( rt );
      q.setForwardOnly( true );

      if ( filter_.isEmpty() )
        q.setWhere( cursor_->db().manager() ->formatAssignValue( fF, fieldSender, v, true ) );
      else
        q.setWhere( filter_ + " AND " + cursor_->db().manager() ->formatAssignValue( fF, fieldSender, v, true ) );

      if ( q.exec() && q.next() ) {
        setValue( QVariant() );
        setValue( q.value( 0 ) );
      }
    }
  }
}

void AQFieldDB::emitKeyF2Pressed()
{
  Q_EMIT keyF2Pressed();
}

void AQFieldDB::emitTextChanged( const QString & t )
{
  Q_EMIT textChanged( t );
}

void AQFieldDB::clearPixmap()
{
  if ( editorImg_ ) {
    editorImg_->clear();

    Q_ASSERT( act_ );

    act_->cursor() ->setValueBuffer( fieldName_, QVariant() );
  }
}

void AQFieldDB::savePixmap( int f )
{
  if ( editorImg_ ) {
    QByteArray fmt( QImageWriter::supportedImageFormats().at( f ) );
    QString ext( QString( fmt ).lower() );
    QString filename( "image." + ext );
    QString
    saveFilename( QFileDialog::getSaveFileName( this, tr( "Guardar imagen como" ), filename.lower(), "*." + ext ) );

    if ( !saveFilename.isEmpty() ) {
      AbanQ->setOverrideCursor( Qt::waitCursor );
      QPixmap pix;
      pix.loadFromData( value().toByteArray() );
      if ( !pix.isNull() )
        if ( !pix.save( saveFilename, fmt ) )
          AQ_MSGBOX_WARN( tr( "Error" ), tr( "Error guardando fichero" ) );
      AbanQ->restoreOverrideCursor();
    }
  }
}

int AQFieldDB::insertAccel( const QString & key )
{
  if ( !accel_ ) {
    accel_ = new Q3Accel( editor_ );
    connect ( accel_, SIGNAL( activated( int ) ), this, SLOT( emitActivatedAccel( int ) ) );
  }

  int id = accel_->findKey( QKeySequence( key ) );

  if ( id != -1 )
    return id;

  id = accel_->insertItem( QKeySequence( key ) );
  return id;
}

void AQFieldDB::removeAccel( int id )
{
  if ( !accel_ )
    return ;
  accel_->removeItem( id );
}

void AQFieldDB::emitActivatedAccel( int id )
{
  if ( editor_ && editor_->hasFocus() )
    Q_EMIT activatedAccel( id );
}

void AQFieldDB::setKeepDisabled( bool keep )
{
  keepDisabled_ = keep;
}

bool AQFieldDB::showEditor() const
{
  return showEditor_;
}

void AQFieldDB::setShowEditor( bool show )
{
  if ( showEditor_ != show ) {
    QWidget * ed = 0;
    if ( editor_ )
      ed = editor_;
    else if ( editorImg_ )
      ed = editorImg_;

    if ( ed ) {
      showEditor_ = show;
      if ( show )
        ed->show();
      else
        ed->hide();
    }
  }
}

QWidget * AQFieldDB::editor() const
{
  return editor_;
}

void AQFieldDB::firstRefresh()
{
  if ( !firstRefresh_ && act_ ) {
    firstRefresh_ = true;

    AQSqlCursor * cursor_ = act_->cursor();
    AQSqlCursor * cursorMaster_ = act_->aqActionMaster() ? act_->aqActionMaster() ->cursor() : 0;

    blockSignals( true );

    if ( cursorMaster_ )
      cursorMaster_->blockSignals( true );

    cursor_->blockSignals( true );
    ui.setupUi( this );
    initMaxSize_ = maximumSize();
    initMinSize_ = minimumSize();

    switch ( refreshType_ ) {
      case 1:
        connect( cursor_, SIGNAL( cursorUpdated() ), this, SLOT( refresh() ) );
        break;

      case 2:
        connect( cursor_, SIGNAL( bufferChanged( const QString & ) ), this, SLOT( refresh( const QString & ) ) );
        break;

      case 3:
        cursor_->setModeAccess( AQSql::BROWSE );
        cursor_->append( Q3SqlFieldInfo( cursor_->db().db().recordInfo( tableName_ ).field( fieldName_ ) ) );
        cursor_->append( Q3SqlFieldInfo( cursor_->db().db().recordInfo( tableName_ ).field( fieldRelation_ ) ) );
        break;

    }

    connect( cursor_, SIGNAL( newBuffer() ), this, SLOT( refresh() ) );
    connect( cursor_, SIGNAL( bufferChanged( const QString & ) ), this, SLOT( refreshQuick( const QString & ) ) );

    initEditor();

    if ( editor_ && showEditor_ )
      editor_->show();
    if ( editorImg_ && showEditor_ )
      editorImg_->show();

    refresh();

    if ( hasFocus() )
      selectAll();

    cursor_->blockSignals( false );
    if ( cursorMaster_ )
      cursorMaster_->blockSignals( false );
    blockSignals( false );
  }
}

FLFieldDB::FLFieldDB( QWidget * parent ) :
    AQFieldDB( parent )
{}

FLFieldDB::~FLFieldDB()
{}

