/***************************************************************************
                        vdatepopup.cpp  -  description
                           -------------------
  begin                : Tue Apr 2 2002
  copyright            : (C) 2002 by Bryan Brunton
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

#include <qframe.h>
#include <qpainter.h>
#include <qdialog.h>
#include <qtoolbutton.h>
#include <qfont.h>

#include "vdatepopup.h"
#include "vdatetable.h"

VDatePopup::VDatePopup( QWidget * parent, QDate dt, const char *name ) : QFrame( parent, name ), yearForward( new QToolButton( this ) ), yearBackward( new QToolButton( this ) ), monthForward( new QToolButton( this ) ), monthBackward( new QToolButton( this ) ), selectMonth( new QToolButton( this ) ), selectYear( new QToolButton( this ) ), table( new VDateTable( this ) ) {
	yearForward->setPixmap( QPixmap::fromMimeSource( "2rightarrow.png" ) );
	yearBackward->setPixmap( QPixmap::fromMimeSource( "2leftarrow.png" ) );
	monthForward->setPixmap( QPixmap::fromMimeSource( "1rightarrow.png" ) );
	monthBackward->setPixmap( QPixmap::fromMimeSource( "1leftarrow.png" ) );
	setDate( dt );									// set button texts

	connect( table, SIGNAL( dateChanged( QDate ) ), SLOT( dateChangedSlot( QDate ) ) );
	connect( table, SIGNAL( tableClicked() ), SLOT( tableClickedSlot() ) );
	connect( monthForward, SIGNAL( clicked() ), SLOT( monthForwardClicked() ) );
	connect( monthBackward, SIGNAL( clicked() ), SLOT( monthBackwardClicked() ) );
	connect( yearForward, SIGNAL( clicked() ), SLOT( yearForwardClicked() ) );
	connect( yearBackward, SIGNAL( clicked() ), SLOT( yearBackwardClicked() ) );
	connect( selectMonth, SIGNAL( clicked() ), SLOT( selectMonthClicked() ) );
	connect( selectYear, SIGNAL( clicked() ), SLOT( selectYearClicked() ) );
}

VDatePopup::~VDatePopup() {}

void VDatePopup::resizeEvent( QResizeEvent * ) {
	QWidget * buttons[] = {
	                          yearBackward,
	                          monthBackward,
	                          selectMonth,
	                          selectYear,
	                          monthForward,
	                          yearForward
	                      };
	const int NoOfButtons = sizeof( buttons ) / sizeof( buttons[ 0 ] );
	QSize sizes[ NoOfButtons ];
	int buttonHeight = 0;
	int count;
	int w;
	int x = 0;
	// ----- calculate button row height:
	for ( count = 0; count < NoOfButtons; ++count ) {
		sizes[ count ] = buttons[ count ] ->sizeHint();
		buttonHeight = QMAX( buttonHeight, sizes[ count ].height() );
	}
	// ----- calculate size of the month button:
	w = 0;
	for ( count = 0; count < NoOfButtons; ++count ) {
		if ( buttons[ count ] != selectMonth ) {
			w += sizes[ count ].width();
		} else {
			x = count;
		}
	}
	sizes[ x ].setWidth( width() - w );	// stretch the month button
	// ----- place the buttons:
	x = 0;
	for ( count = 0; count < NoOfButtons; ++count ) {
		w = sizes[ count ].width();
		buttons[ count ] ->setGeometry( x, 0, w, buttonHeight );
		x += w;
	}

	// ----- adjust the table:
	table->setGeometry( 0, buttonHeight, width(), height() - buttonHeight );
}

void VDatePopup::dateChangedSlot( QDate date ) {
	emit( dateChanged( date ) );
}

void VDatePopup::tableClickedSlot() {
	emit( dateSelected( table->getDate() ) );
	emit( tableClicked() );
}

const QDate & VDatePopup::getDate() {
	return table->getDate();
}

const QDate & VDatePopup::date() const {
	return table->getDate();
}

bool VDatePopup::setDate( const QDate & date ) {
	if ( date.isValid() ) {
		QString temp;
		// -----
		table->setDate( date );
		selectMonth->setText( QDate::shortMonthName( date.month() ) );
		temp.setNum( date.year() );
		selectYear->setText( temp );
		return true;
	} else {
		return false;
	}
}

void VDatePopup::monthForwardClicked() {
	QDate temp = table->getDate();
	int day = temp.day();
	// -----
	if ( temp.month() == 12 ) {
		temp.setYMD( temp.year() + 1, 1, 1 );
	} else {
		temp.setYMD( temp.year(), temp.month() + 1, 1 );
	}
	if ( temp.daysInMonth() < day ) {
		temp.setYMD( temp.year(), temp.month(), temp.daysInMonth() );
	} else {
		temp.setYMD( temp.year(), temp.month(), day );
	}
	// assert(temp.isValid());
	setDate( temp );
}

void VDatePopup::monthBackwardClicked() {
	QDate temp = table->getDate();
	int day = temp.day();
	// -----
	if ( temp.month() == 1 ) {
		temp.setYMD( temp.year() - 1, 12, 1 );
	} else {
		temp.setYMD( temp.year(), temp.month() - 1, 1 );
	}
	if ( temp.daysInMonth() < day ) {
		temp.setYMD( temp.year(), temp.month(), temp.daysInMonth() );
	} else {
		temp.setYMD( temp.year(), temp.month(), day );
	}
	// assert(temp.isValid());
	setDate( temp );
}

void VDatePopup::yearForwardClicked() {
	QDate temp = table->getDate();
	int day = temp.day();
	// -----
	temp.setYMD( temp.year() + 1, temp.month(), 1 );
	if ( temp.daysInMonth() < day ) {
		temp.setYMD( temp.year(), temp.month(), temp.daysInMonth() );
	} else {
		temp.setYMD( temp.year(), temp.month(), day );
	}
	// assert(temp.isValid());
	setDate( temp );
}

void VDatePopup::yearBackwardClicked() {
	QDate temp = table->getDate();
	int day = temp.day();
	// -----
	temp.setYMD( temp.year() - 1, temp.month(), 1 );
	if ( temp.daysInMonth() < day ) {
		temp.setYMD( temp.year(), temp.month(), temp.daysInMonth() );
	} else {
		temp.setYMD( temp.year(), temp.month(), day );
	}
	// assert(temp.isValid());
	setDate( temp );
}

void VDatePopup::selectMonthClicked() {
	int month;
	VPopupFrame *popup = new VPopupFrame( this );
	VDateInternalMonthPicker *picker = new VDateInternalMonthPicker( qApp->font().pointSize(), popup );
	// -----
	picker->resize( picker->sizeHint() );
	popup->setMainWidget( picker );
	picker->setFocus();
	connect( picker, SIGNAL( closeMe( int ) ), popup, SLOT( close( int ) ) );
	if ( popup->exec( selectMonth->mapToGlobal( QPoint( 0, selectMonth->height() ) ) ) ) {
		QDate date;
		int day;
		// -----
		month = picker->getResult();
		date = table->getDate();
		day = date.day();
		// ----- construct a valid date in this month:
		date.setYMD( date.year(), month, 1 );
		date.setYMD( date.year(), month, QMIN( day, date.daysInMonth() ) );
		// ----- set this month
		setDate( date );
	}

	delete popup;
}

void VDatePopup::selectYearClicked() {
	int year;
	VPopupFrame *popup = new VPopupFrame( this );
	VDateInternalYearSelector *picker = new VDateInternalYearSelector( qApp->font().pointSize(), popup );
	// -----
	picker->resize( picker->sizeHint() );
	popup->setMainWidget( picker );
	connect( picker, SIGNAL( closeMe( int ) ), popup, SLOT( close( int ) ) );
	picker->setFocus();
	if ( popup->exec( selectYear->mapToGlobal( QPoint( 0, selectMonth->height() ) ) ) ) {
		QDate date;
		int day;

		year = picker->getYear();
		date = table->getDate();
		day = date.day();
		// ----- construct a valid date in this month:
		date.setYMD( year, date.month(), 1 );
		date.setYMD( year, date.month(), QMIN( day, date.daysInMonth() ) );
		// ----- set this month
		setDate( date );
	}

	delete popup;
}

void VDatePopup::setEnabled( bool enable ) {
	QWidget * widgets[] = {
	                          yearForward, yearBackward, monthForward, monthBackward,
	                          selectMonth, selectYear, table
	                      };
	const int Size = sizeof( widgets ) / sizeof( widgets[ 0 ] );
	int count;
	// -----
	for ( count = 0; count < Size; ++count ) {
		widgets[ count ] ->setEnabled( enable );
	}
}

QSize VDatePopup::sizeHint() const {
	QSize tableSize = table->sizeHint();
	QWidget *buttons[] = {
	                         yearBackward,
	                         monthBackward,
	                         selectMonth,
	                         selectYear,
	                         monthForward,
	                         yearForward
	                     };
	const int NoOfButtons = sizeof( buttons ) / sizeof( buttons[ 0 ] );
	QSize sizes[ NoOfButtons ];
	int cx = 0, cy = 0, count;
	// ----- store the size hints:
	for ( count = 0; count < NoOfButtons; ++count ) {
		sizes[ count ] = buttons[ count ] ->sizeHint();
		if ( buttons[ count ] == selectMonth ) {
			cx += maxMonthRect.width();
		} else {
			cx += sizes[ count ].width();
		}
		cy = QMAX( sizes[ count ].height(), cy );
	}
	// ----- calculate width hint:
	cx = QMAX( cx, tableSize.width() );	// line edit ignored
	// ----- calculate height hint:
	cy += tableSize.height();
	return QSize( cx, cy );
}

void VDatePopup::virtual_hook( int id, void *data ) {																/*BASE::virtual_hook( id, data ); */
}
