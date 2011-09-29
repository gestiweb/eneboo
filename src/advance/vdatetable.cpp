/***************************************************************************
                        vdatetable.cpp  -  description
                           -------------------
  begin                : Tue Apr 2 2002
  copyright            : (C) 2002 by Bryan Brunton
                         (C) 1997 Tim D. Gilman
                         (C) 1998-2001 Mirko Boehm
  email                : bryanbrun@yahoo.com

  Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
            (C) 1998-2001 Mirko Boehm (mirko@kde.org)

***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qdatetime.h>
#include <qstring.h>
#include <qpen.h>
#include <qpainter.h>
#include <qdialog.h>
#include <assert.h>

#include "vdatetable.h"

VDateValidator::VDateValidator( QWidget * parent, const char *name ) : QValidator( parent, name ) {}

QValidator::State VDateValidator::validate( QString & text, int & ) const {
	QDate temp;

	return date( text, temp );
}

QValidator::State VDateValidator::date( const QString & text, QDate & d ) const {
	QDate tmp = QDate::fromString( text );
	if ( !tmp.isNull() ) {
		d = tmp;
		return Acceptable;
	} else
		return Valid;
}

void VDateValidator::fixup( QString & ) const {}

VDateTable::VDateTable( QWidget * parent, QDate date_, const char *name, WFlags f ) : QGridView( parent, name, f ) {
	if ( !date_.isValid() ) {
		date_ = QDate::currentDate();
	}
	setFocusPolicy( QWidget::StrongFocus );
	setNumRows( 7 );								// 6 weeks max + headline
	setNumCols( 7 );								// 7 days a week
	setHScrollBarMode( AlwaysOff );
	setVScrollBarMode( AlwaysOff );
	viewport() ->setEraseColor( white );
	setDate( date_ );								// this initializes firstday, numdays, numDaysPrevMonth
}

VDateTable::~VDateTable() {}

void VDateTable::paintCell( QPainter * painter, int row, int col ) {
	QRect rect;
	QString text;
	QPen pen;
	int w = cellWidth();
	int h = cellHeight();
	int pos;
	QBrush brushBlue( blue );
	QBrush brushLightblue( white );

	QFont font( qApp->font() );
	if ( row == 0 ) {
		font.setBold( true );
		painter->setFont( font );
		bool normalday = true;
		QString daystr;
		if ( true /*AdvanceApp::ref()->locale()->weekStartsMonday()*/ ) {
			daystr = QDate::shortDayName( col + 1 );
			if ( col == 5 || col == 6 )
				normalday = false;
		} else {
			daystr = QDate::shortDayName( col == 0 ? 7 : col );
			if ( col == 0 || col == 6 )
				normalday = false;
		}
		if ( !normalday ) {
			painter->setPen( white );
			painter->setBrush( brushLightblue );
			painter->drawRect( 0, 0, w, h );
			painter->setPen( blue );
		} else {
			painter->setPen( blue );
			painter->setBrush( brushBlue );
			painter->drawRect( 0, 0, w, h );
			painter->setPen( white );
		}
		painter->drawText( 0, 0, w, h - 1, AlignCenter, daystr, -1, &rect );
		painter->setPen( black );
		painter->moveTo( 0, h - 1 );
		painter->lineTo( w - 1, h - 1 );
		// ----- draw the weekday:
	} else {
		painter->setFont( font );
		pos = 7 * ( row - 1 ) + col;
		if ( true /*AdvanceApp::ref()->locale()->weekStartsMonday()*/ )
			pos++;
		if ( pos < firstday || ( firstday + numdays <= pos ) ) {														// we are either
			// ° painting a day of the previous month or
			// ° painting a day of the following month
			if ( pos < firstday ) {													// previous month
				text.setNum( numDaysPrevMonth + pos - firstday + 1 );
			} else {													// following month
				text.setNum( pos - firstday - numdays + 1 );
			}
			painter->setPen( gray );
		} else {														// paint a day of the current month
			text.setNum( pos - firstday + 1 );
			painter->setPen( black );
		}

		pen = painter->pen();
		if ( firstday + date.day() - 1 == pos ) {
			if ( hasFocus() ) {													// draw the currently selected date
				painter->setPen( red );
				painter->setBrush( darkRed );
				pen = white;
			} else {
				painter->setPen( darkGray );
				painter->setBrush( darkGray );
				pen = white;
			}
		} else {
			painter->setBrush( white );
			painter->setPen( white );
		}
		painter->drawRect( 0, 0, w, h );
		painter->setPen( pen );
		painter->drawText( 0, 0, w, h, AlignCenter, text, -1, &rect );
	}
	if ( rect.width() > maxCell.width() )
		maxCell.setWidth( rect.width() );
	if ( rect.height() > maxCell.height() )
		maxCell.setHeight( rect.height() );
}

void VDateTable::keyPressEvent( QKeyEvent * e ) {
	if ( e->key() == Qt::Key_Prior ) {
		if ( date.month() == 1 ) {
			return ;
		}
		int day = date.day();
		if ( day > 27 )
			while ( !QDate::isValid( date.year(), date.month() - 1, day ) )
				day--;
		setDate( QDate( date.year(), date.month() - 1, day ) );
		return ;
	}
	if ( e->key() == Qt::Key_Next ) {
		if ( date.month() == 12 ) {
			return ;
		}
		int day = date.day();
		if ( day > 27 )
			while ( !QDate::isValid( date.year(), date.month() + 1, day ) )
				day--;
		setDate( QDate( date.year(), date.month() + 1, day ) );
		return ;
	}

	int dayoff = true /*AdvanceApp::ref()->locale()->weekStartsMonday()*/ ? 1 : 0;

	int temp = firstday + date.day() - dayoff;
	int pos = temp;

	if ( e->key() == Qt::Key_Up ) {
		pos -= 7;
	}
	if ( e->key() == Qt::Key_Down ) {
		pos += 7;
	}
	if ( e->key() == Qt::Key_Left ) {
		pos--;
	}
	if ( e->key() == Qt::Key_Right ) {
		pos++;
	}

	if ( pos + dayoff <= firstday ) {															// this day is in the previous month
		return ;
	}
	if ( firstday + numdays < pos + dayoff ) {															// this date is in the next month
		return ;
	}

	if ( pos == temp )
		return ;

	setDate( QDate( date.year(), date.month(), pos - firstday + dayoff ) );
	updateCell( temp / 7 + 1, temp % 7 );	// Update the previously selected cell
	updateCell( pos / 7 + 1, pos % 7 );	// Update the selected cell
	assert( QDate( date.year(), date.month(), pos - firstday + dayoff ).isValid() );
}

void VDateTable::viewportResizeEvent( QResizeEvent * e ) {
	QGridView::viewportResizeEvent( e );

	setCellWidth( viewport() ->width() / 7 );
	setCellHeight( viewport() ->height() / 7 );
}

void VDateTable::contentsMousePressEvent( QMouseEvent * e ) {
	if ( e->type() != QEvent::MouseButtonPress ) {															// the KDatePicker only reacts on mouse press events:
		return ;
	}
	if ( !isEnabled() ) {
		return ;
	}

	int dayoff = true /*AdvanceApp::ref()->locale()->weekStartsMonday()*/ ? 1 : 0;
	// -----
	int row, col, pos, temp;
	QPoint mouseCoord;
	// -----
	mouseCoord = e->pos();
	row = rowAt( mouseCoord.y() );
	col = columnAt( mouseCoord.x() );
	if ( row < 0 || col < 0 ) {															// the user clicked on the frame of the table
		return ;
	}
	pos = 7 * ( row - 1 ) + col + 1;
	if ( pos + dayoff <= firstday ) {															// this day is in the previous month
		return ;
	}
	if ( firstday + numdays < pos + dayoff ) {															// this date is in the next month
		return ;
	}
	temp = firstday + date.day() - dayoff - 1;
	setDate( QDate( date.year(), date.month(), pos - firstday + dayoff ) );
	updateCell( temp / 7 + 1, temp % 7 );	// Update the previously selected cell
	updateCell( row, col );					// Update the selected cell
	// assert(QDate(date.year(), date.month(), pos-firstday+dayoff).isValid());
	emit( tableClicked() );
}

bool VDateTable::setDate( const QDate & date_ ) {
	bool changed = false;
	QDate temp;
	// -----
	if ( !date_.isValid() ) {
		return false;
	}
	if ( date != date_ ) {
		date = date_;
		changed = true;
	}
	temp.setYMD( date.year(), date.month(), 1 );
	firstday = temp.dayOfWeek();
	if ( firstday == 1 )
		firstday = 8;
	numdays = date.daysInMonth();
	if ( date.month() == 1 ) {															// set to december of previous year
		temp.setYMD( date.year() - 1, 12, 1 );
	} else {															// set to previous month
		temp.setYMD( date.year(), date.month() - 1, 1 );
	}
	numDaysPrevMonth = temp.daysInMonth();
	if ( changed ) {
		repaintContents( false );
	}
	emit( dateChanged( date ) );
	return true;
}

const QDate & VDateTable::getDate() const {
	return date;
}

void VDateTable::focusInEvent( QFocusEvent * e ) {
	repaintContents( false );
	QGridView::focusInEvent( e );
}

void VDateTable::focusOutEvent( QFocusEvent * e ) {
	repaintContents( false );
	QGridView::focusOutEvent( e );
}

QSize VDateTable::sizeHint() const {
	if ( maxCell.height() > 0 && maxCell.width() > 0 ) {
		return QSize( maxCell.width() * numCols() + 2 * frameWidth(), ( maxCell.height() + 2 ) * numRows() + 2 * frameWidth() );
	} else {
		return QSize( -1, -1 );
	}
}


VDateInternalMonthPicker::VDateInternalMonthPicker( int fontsize, QWidget * parent, const char *name ) : QGridView( parent, name ), result( 0 ) {
	QRect rect;

	activeCol = -1;
	activeRow = -1;
	QFont font( qApp->font() );
	font.setPointSize( 10 );

	font.setPointSize( fontsize );
	setFont( font );
	setHScrollBarMode( AlwaysOff );
	setVScrollBarMode( AlwaysOff );
	setFrameStyle( QFrame::NoFrame );
	setNumRows( 4 );
	setNumCols( 3 );

	viewport() ->setEraseColor( white );

	QFontMetrics metrics( font );
	for ( int i = 1; i <= 12; ++i ) {
		rect = metrics.boundingRect( QDate::shortMonthName( i ) );
		if ( max.width() < rect.width() )
			max.setWidth( rect.width() );
		if ( max.height() < rect.height() )
			max.setHeight( rect.height() );
	}

}

QSize VDateInternalMonthPicker::sizeHint() const {
	return QSize( ( max.width() + 6 ) * numCols() + 2 * frameWidth(), ( max.height() + 6 ) * numRows() + 2 * frameWidth() );
}

int VDateInternalMonthPicker::getResult() const {
	return result;
}

void VDateInternalMonthPicker::setupPainter( QPainter * p ) {
	p->setPen( black );
}

void VDateInternalMonthPicker::viewportResizeEvent( QResizeEvent * ) {
	setCellWidth( width() / 3 );
	setCellHeight( height() / 4 );
}

void VDateInternalMonthPicker::paintCell( QPainter * painter, int row, int col ) {
	int index;
	QString text;

	index = 3 * row + col + 1;
	text = QDate::shortMonthName( index );
	painter->drawText( 0, 0, cellWidth(), cellHeight(), AlignCenter, text );
	if ( activeCol == col && activeRow == row )
		painter->drawRect( 0, 0, cellWidth(), cellHeight() );
}

void VDateInternalMonthPicker::contentsMousePressEvent( QMouseEvent * e ) {
	if ( !isEnabled() || e->button() != LeftButton ) {
		return ;
	}

	int row, col;
	QPoint mouseCoord;

	mouseCoord = e->pos();
	row = rowAt( mouseCoord.y() );
	col = columnAt( mouseCoord.x() );

	if ( row < 0 || col < 0 ) {															// the user clicked on the frame of the table
		activeCol = -1;
		activeRow = -1;
	} else {
		activeCol = col;
		activeRow = row;
		updateCell( row, col /*, false */ );
	}
}

void VDateInternalMonthPicker::contentsMouseMoveEvent( QMouseEvent * e ) {
	if ( e->state() & LeftButton ) {
		int row, col;
		QPoint mouseCoord;
		// -----
		mouseCoord = e->pos();
		row = rowAt( mouseCoord.y() );
		col = columnAt( mouseCoord.x() );
		int tmpRow = -1, tmpCol = -1;
		if ( row < 0 || col < 0 ) {														// the user clicked on the frame of the table
			if ( activeCol > -1 ) {
				tmpRow = activeRow;
				tmpCol = activeCol;
			}
			activeCol = -1;
			activeRow = -1;
		} else {
			bool differentCell = ( activeRow != row || activeCol != col );
			if ( activeCol > -1 && differentCell ) {
				tmpRow = activeRow;
				tmpCol = activeCol;
			}
			if ( differentCell ) {
				activeRow = row;
				activeCol = col;
				updateCell( row, col /*, false */ );	// mark the new active cell
			}
		}
		if ( tmpRow > -1 )  							// repaint the former active cell
			updateCell( tmpRow, tmpCol /*, true */ );
	}
}

void VDateInternalMonthPicker::contentsMouseReleaseEvent( QMouseEvent * e ) {
	if ( !isEnabled() ) {
		return ;
	}
	// -----
	int row, col, pos;
	QPoint mouseCoord;
	// -----
	mouseCoord = e->pos();
	row = rowAt( mouseCoord.y() );
	col = columnAt( mouseCoord.x() );
	if ( row < 0 || col < 0 ) {															// the user clicked on the frame of the table
		emit( closeMe( 0 ) );
	}
	pos = 3 * row + col + 1;
	result = pos;
	emit( closeMe( 1 ) );
}



VDateInternalYearSelector::VDateInternalYearSelector( int fontsize, QWidget * parent, const char *name ) : QLineEdit( parent, name ), val( new QIntValidator( this ) ), result( 0 ) {
	QFont f( font() );
	f.setPointSize( 10 );
	setFont( f );
	setFrameStyle( QFrame::NoFrame );
	// we have to respect the limits of QDate here, I fear:
	val->setRange( 0, 8000 );
	setValidator( val );
	connect( this, SIGNAL( returnPressed() ), SLOT( yearEnteredSlot() ) );
}

void VDateInternalYearSelector::yearEnteredSlot() {
	bool ok;
	int year;
	QDate date;
	// ----- check if this is a valid year:
	year = text().toInt( &ok );
	if ( !ok ) {
		return ;
	}
	date.setYMD( year, 1, 1 );
	if ( !date.isValid() ) {
		return ;
	}
	result = year;
	emit( closeMe( 1 ) );
}

int VDateInternalYearSelector::getYear() {
	return result;
}

void VDateInternalYearSelector::setYear( int year ) {
	QString temp;
	// -----
	temp.setNum( year );
	setText( temp );
}

VPopupFrame::VPopupFrame( QWidget * parent, const char *name ) : QFrame( parent, name, WType_Popup ), result( 0 ),  	// rejected
main( 0 ) {
	setFrameStyle( QFrame::Box | QFrame::Raised );
	setMidLineWidth( 2 );
}

void VPopupFrame::keyPressEvent( QKeyEvent * e ) {
	if ( e->key() == Key_Escape ) {
		result = 0;									// rejected
		qApp->exit_loop();
	}
}

void VPopupFrame::close( int r ) {
	result = r;
	qApp->exit_loop();
}

void VPopupFrame::setMainWidget( QWidget * m ) {
	main = m;
	if ( main != 0 ) {
		resize( main->width() + 2 * frameWidth(), main->height() + 2 * frameWidth() );
	}
}

void VPopupFrame::resizeEvent( QResizeEvent * ) {
	if ( main != 0 ) {
		main->setGeometry( frameWidth(), frameWidth(), width() - 2 * frameWidth(), height() - 2 * frameWidth() );
	}
}

void VPopupFrame::popup( const QPoint & pos ) {
	// Make sure the whole popup is visible.
	QRect d = QApplication::desktop() ->screenGeometry( QApplication::desktop() ->screenNumber( pos ) );
	int x = pos.x();
	int y = pos.y();
	int w = width();
	int h = height();
	if ( x + w > d.x() + d.width() )
		x = d.width() - w;
	if ( y + h > d.y() + d.height() )
		y = d.height() - h;
	if ( x < d.x() )
		x = 0;
	if ( y < d.y() )
		y = 0;

	// Pop the thingy up.
	move( x, y );
	show();
}

int VPopupFrame::exec( QPoint pos ) {
	popup( pos );
	repaint();
	qApp->enter_loop();
	hide();
	return result;
}

int VPopupFrame::exec( int x, int y ) {
	return exec( QPoint( x, y ) );
}

void VPopupFrame::virtual_hook( int, void * ) {																/*BASE::virtual_hook( id, data ); */
}

void VDateTable::virtual_hook( int, void * ) {																/*BASE::virtual_hook( id, data ); */
}
