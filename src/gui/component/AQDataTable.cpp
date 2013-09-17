/***************************************************************************
AQDataTable.cpp
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

#include <QContextMenuEvent>
#include <QFocusEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QPainter>
#include <QSqlField>
#include <QTimer>

#include <Q3SqlCursor>

#include <AQCore/AQTableMD.h>
#include <AQCore/AQFieldMD.h>
#include <AQCore/AQCoreUtil.h>
#include <AQSql/AQSqlCursor.h>
#include <AQSql/AQSqlQuery.h>
#include <AQSql/AQSqlDatabase.h>

#include "AQDataTable.h"
#include "../AQApplication.h"
#include "../AQAction.h"

AQDataTable::AQDataTable( QWidget * parent, const char * name ) :
    Q3DataTable( parent, name ),
    rowSelected( -1 ),
    colSelected( -1 ),
    readonly_( false ),
    editonly_( false ),
    insertonly_( false ),
    okPix( 0 ),
    noPix( 0 ),
    imgPix( 0 ),
    queue( 0 )
{
  setShowGrid( false );
  connect( this, SIGNAL( newAdjustCol( int, int ) ),
           this, SLOT( adjustCol( int, int ) ), Qt::QueuedConnection );
}

AQDataTable::~AQDataTable()
{
  delete okPix;
  delete noPix;
  delete imgPix;
}

void AQDataTable::selectRow( int r, int c )
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  blockSignals( true );

  if ( r < 0 ) {
    if ( cursor_->isValid() ) {
      rowSelected = cursor_->at();
      colSelected = currentColumn();

      QTimer::singleShot( 0, this, SLOT( ensureRowSelectedVisible() ) );
    } else {
      rowSelected = 0;
      colSelected = 0;
    }
  } else {
    rowSelected = r;
    colSelected = c;

    if ( !qobject_cast<AQSqlCursor *>( sender() ) )
      cursor_->seek( rowSelected, false, false );
  }

  setCurrentCell( rowSelected, colSelected );

  blockSignals( false );
}

void AQDataTable::setAQAction( AQSharedAction act )
{
  if ( !act_ )
    connect( this, SIGNAL( currentChanged( int, int ) ), this, SLOT( selectRow( int, int ) ) );

  act_ = act;

  if ( act_ ) {
    AQSqlCursor * cursor_ = act_->cursor();

    if ( cursor_ ) {
      connect( cursor_, SIGNAL( currentChanged( int ) ), this, SLOT( selectRow( int ) ) );
      connect( cursor_, SIGNAL( cursorUpdated() ), this, SLOT( refresh() ) );
      connect( cursor_, SIGNAL( newSelect() ), this, SLOT( refreshData() ) );
    }
  }

  Q3SqlCursor * cursor3_ = static_cast<Q3SqlCursor *>( act_ ? act_->cursor() : 0 );
  Q3DataTable::setSqlCursor( cursor3_, false, false );

  if ( !cursor3_ )
    return ;

  if ( !cursor3_->isActive() )
    refresh();
  else
    refreshData();
}

void AQDataTable::paintCell( QPainter * p, int row, int col, const QRect & cr,
                             bool selected, const QColorGroup & cg )
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  if ( !cursor_ || !cursor_->Q3SqlCursor::seek( row ) )
    return ;

  Q3Table::paintCell( p, row, col, cr, selected, cg );
  if ( !selected && ( row % 2 ) )
    p->fillRect( 0, 0, cr.width() - 2, cr.height() - 2, cg.brush( QPalette::AlternateBase ) );

  const AQTableMD * tMD = cursor_->metadata();
  AQFieldMD * fieldTMD = tMD->field( cursor_->field( indexOf( col ) ).name() );

  Q_ASSERT( fieldTMD );

  if ( fieldTMD->type() == QVariant::Pixmap && row != rowSelected )
    return ;

  p->setPen( selected ? cg.highlightedText() : cg.text() );
  lastTextPainted.clear();
  QSqlField field( cursor_->field( indexOf( col ) ) );
  paintField( p, &field, cr, selected );

  cursor_->Q3SqlCursor::seek( rowSelected );

  if ( !lastTextPainted.isEmpty() ) {
    int wC = adjustedCols.value( col, Q3Table::columnWidth( col ) );
    int wH = fontMetrics().width( horizontalHeader() ->label( col ) + "W" );

    if ( wH < wC )
      wH = wC;

    wC = fontMetrics().width( lastTextPainted ) + fontMetrics().maxWidth();

    if ( wC > wH ) {
      ++queue;
      Q_EMIT newAdjustCol( col, wC );
    }

  }
}

void AQDataTable::paintField( QPainter * p, const QSqlField * field,
                              const QRect & cr, bool selected )
{
  Q_ASSERT( field );
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();
  AQFieldMD * fieldTMD = cursor_->metadata() ->field( field->name() );

  Q_ASSERT( fieldTMD );

  if ( !fieldTMD->visible() ) {
    QBrush bu;
    bu.setColor( Qt::gray );
    bu.setStyle( Qt::DiagCrossPattern );
    p->fillRect( 1, 1, cr.width() - 4, cr.height() - 4, bu );
    return ;
  }

  if ( !field->isNull() ) {
    int type = fieldTMD->type();
    QString text;

    switch ( type ) {
    case QVariant::Double: {
        double fValue = field->value().toDouble();
        int partDecimal = fieldTMD->partDecimal();
        text.setNum( fValue, 'f', partDecimal );
        if ( !selected ) {
          if ( fValue < 0.0 ) {
            QPen curPen = p->pen();
            QPen pen( curPen );
            pen.setColor( Qt::darkRed );
            p->setPen( pen );
          } else {
            QPen curPen = p->pen();
            QPen pen( curPen );
            pen.setColor( Qt::darkBlue );
            p->setPen( pen );
          }
        }
        text = AQCoreUtil::formatoMiles( text );
        p->drawText( 2, 2, cr.width() - 4, cr.height() - 4, Qt::AlignRight, text );
      }
      break;

    case AQCore::Unlock: {
        if ( field->value().toBool() ) {
          if ( !okPix )
            okPix = new QPixmap( ":/images/yes.png" );
          p->drawPixmap( ( cr.width() - okPix->width() ) / 2, 2, *okPix, 0, 0, cr.width() - 4,
                         cr.height() - 4 );
        } else {
          if ( !noPix )
            noPix = new QPixmap( ":/images/no.png" );
          p->drawPixmap( ( cr.width() - noPix->width() ) / 2, 2, *noPix, 0, 0, cr.width() - 4,
                         cr.height() - 4 );
        }
      }
      break;

    case QVariant::String: {
        text = field->value().toString();
        if ( !fieldTMD->optionsList().isEmpty() ) {
          QStringList ol( fieldTMD->optionsList() );
          if ( !ol.contains( text ) ) {
            QVariant defVal( fieldTMD->defaultValue() );
            if ( defVal.isValid() )
              text = defVal.toString();
            else
              text = ol.first();
          }
          text = AQ_TR( "MetaData", text );
        }
        p->drawText( 2, 2, cr.width() - 4, cr.height() - 4, fieldAlignment( field ), text );
      }
      break;

    case AQCore::Serial:
    case QVariant::UInt:
      text = AQCoreUtil::formatoMiles( field->value().toString() );
      p->drawText( 2, 2, cr.width() - 4, cr.height() - 4, Qt::AlignRight, text );
      break;

    case QVariant::Int:
      if ( field->value().toInt() < 0 ) {
        QPen curPen = p->pen();
        QPen pen( curPen );
        pen.setColor( Qt::red );
        p->setPen( pen );
      }
      text = AQCoreUtil::formatoMiles( field->value().toString() );
      p->drawText( 2, 2, cr.width() - 4, cr.height() - 4, Qt::AlignRight, text );
      break;

    case QVariant::Pixmap:
      if ( !imgPix )
        imgPix = new QPixmap( ":/images/image.png" );
      p->drawPixmap( 2, 2, *imgPix, 0, 0, cr.width() - 4, cr.height() - 4 );
      break;

    case QVariant::Date:
      text = field->value().toDate().toString( "dd-MM-yyyy" );
      p->drawText( 2, 2, cr.width() - 4, cr.height() - 4,
                   fieldAlignment( field ), text );
      break;

    case QVariant::Time:
      text = field->value().toTime().toString( "hh:mm:ss" );
      p->drawText( 2, 2, cr.width() - 4, cr.height() - 4,
                   fieldAlignment( field ), text );
      break;

    case QVariant::StringList:
      text = field->value().toString();
      p->drawText( 2, 2, cr.width() - 4, cr.height() - 4,
                   fieldAlignment( field ), text.left( 100 ) + "..." );
      break;

    case QVariant::Bool: {
        if ( field->value().toBool() ) {
          QBrush bu;
          bu.setColor( QColor( 75, 255, 75 ) );
          bu.setStyle( Qt::SolidPattern );
          p->fillRect( 1, 1, cr.width() - 2, cr.height() - 2, bu );
        } else {
          QBrush bu;
          bu.setColor( QColor( 255, 75, 75 ) );
          bu.setStyle( Qt::SolidPattern );
          p->fillRect( 1, 1, cr.width() - 2, cr.height() - 2, bu );
        }
        text = field->value().toBool() ? tr( "Sí" ) : tr( "No" );
        p->drawText( 2, 2, cr.width() - 4, cr.height() - 4,
                     Qt::AlignVCenter | Qt::AlignHCenter, text );
      }
      break;
    }

    lastTextPainted = text;

  }
}

bool AQDataTable::eventFilter( QObject * o, QEvent * e )
{
  int r = currentRow(), c = currentColumn(), nr = numRows(), nc = numCols();

  switch ( e->type() ) {
  case QEvent::KeyPress: {
      QKeyEvent * ke = ( QKeyEvent * ) e;

      if ( ke->key() == Qt::Key_Insert )
        return true;

      if ( ke->key() == Qt::Key_F2 )
        return true;

      if ( ke->key() == Qt::Key_Up && r == 0 )
        return true;

      if ( ke->key() == Qt::Key_Left && c == 0 )
        return true;

      if ( ke->key() == Qt::Key_Down && r == nr - 1 )
        return true;

      if ( ke->key() == Qt::Key_Right && c == nc - 1 )
        return true;

      if ( ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return ) && r > -1 ) {
        Q_EMIT pressEnterOnRecord( readonly_ );
        return true;
      }

      if ( ke->key() == Qt::Key_A )
        return insertRecord();

      if ( ke->key() == Qt::Key_C )
        return copyRecord();

      if ( ke->key() == Qt::Key_M )
        return editRecord();

      if ( ke->key() == Qt::Key_E || ke->key() == Qt::Key_Delete )
        return deleteRecord();

      if ( ke->key() == Qt::Key_V && act_ )
        return browseRecord();

      return false;
    }
    break;

  default:
    break;
  }
  return Q3DataTable::eventFilter( o, e );
}

void AQDataTable::contentsContextMenuEvent( QContextMenuEvent * e )
{
  Q3Table::contentsContextMenuEvent( e );
}

void AQDataTable::contentsMouseDoubleClickEvent( QMouseEvent * e )
{
  if ( e->button() != Qt::LeftButton )
    return ;

  int tmpRow = rowAt( e->pos().y() );
  int tmpCol = columnAt( e->pos().x() );
  Q3TableItem *itm = item( tmpRow, tmpCol );

  if ( itm && !itm->isEnabled() )
    return ;

  Q_EMIT doubleClickOnRecord( readonly_ );
  Q3Table::contentsMouseDoubleClickEvent( e );
}

void AQDataTable::refresh()
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  Q_ASSERT( cursor_ );

  disconnect( cursor_, SIGNAL( cursorUpdated() ), this, SLOT( refresh() ) );

  setFilter( cursor_->curFilter() );
  Q3DataTable::refresh();

  if ( !cursor_->isValid() )
    cursor_->first( false );

  selectRow();

  connect( cursor_, SIGNAL( cursorUpdated() ), this, SLOT( refresh() ) );
}

void AQDataTable::refreshData()
{
  Q_ASSERT( act_ );

  AQSqlCursor * cursor_ = act_->cursor();

  Q_ASSERT( cursor_ );

  disconnect( cursor_, SIGNAL( newSelect() ), this, SLOT( refreshData() ) );

  viewport() ->repaint( false );
  setSize( cursor_ );
  selectRow();

  connect( cursor_, SIGNAL( newSelect() ), this, SLOT( refreshData() ) );
}

void AQDataTable::refreshColumns()
{
  Q3DataTable::refresh( Q3DataTable::RefreshColumns );
}

void AQDataTable::setAQReadOnly( bool mode )
{
  readonly_ = mode;
}

void AQDataTable::setEditOnly( bool mode )
{
  editonly_ = mode;
}

void AQDataTable::setInsertOnly( bool mode )
{
  insertonly_ = mode;
}

AQSharedAction AQDataTable::aqAction() const
{
  return act_;
}

void AQDataTable::adjustCol( int c, int w )
{
  if ( w > adjustedCols.value( c, 0 ) )
    adjustedCols.insert( c, w );
  --queue;
  if ( queue < 1 )
    for ( QHash<int, int>::const_iterator it = adjustedCols.begin(); it != adjustedCols.end(); ++it )
      Q3Table::setColumnWidth( it.key(), *it );
}

bool AQDataTable::deleteRecord()
{
  if ( insertonly_ )
    return false;
  else if ( act_ && !readonly_ && !editonly_ ) {
    act_->deleteRecord();
    return true;
  }
  return false;
}

bool AQDataTable::browseRecord()
{
  if ( act_ ) {
    act_->browseRecord();
    return true;
  }
  return false;
}

bool AQDataTable::editRecord()
{
  if ( insertonly_ )
    return false;
  else if ( act_ && !readonly_ ) {
    act_->editRecord();
    return true;
  }
  return false;
}

bool AQDataTable::insertRecord()
{
  if ( act_ && !readonly_ && !editonly_ ) {
    act_->insertRecord();
    return true;
  }
  return false;
}

bool AQDataTable::copyRecord()
{
  if ( act_ && !readonly_ && !editonly_ ) {
    act_->copyRecord();
    return true;
  }
  return false;
}

void AQDataTable::ensureRowSelectedVisible()
{
  if ( rowSelected > -1 ) {
    if ( !isUpdatesEnabled() || !viewport() ->isUpdatesEnabled() )
      return ;
    int cw = columnWidth( colSelected );
    int margin = visibleHeight() / 2;
    int y = rowPos( rowSelected ) + rowHeight( rowSelected ) / 2;
    if ( cw < visibleWidth() )
      ensureVisible( columnPos( colSelected ) + cw / 2, y, cw / 2, margin );
    else
      ensureVisible( columnPos( colSelected ), y, 0, margin );
  }
}
