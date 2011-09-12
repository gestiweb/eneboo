/***************************************************************************
                        vdatepopup.h  -  description
                           -------------------
  begin                : Tue Apr 2 2002
  copyright            : (C) 2002 by Bryan Brunton
  email                : bryanbrun@yahoo.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VDATEPOPUP_H
#define VDATEPOPUP_H

#include <qwidget.h>
#include <qframe.h>
#include <qdatetime.h>
#include <qrect.h>

class QToolButton;
class VDateTable;

class VDatePopup: public QFrame {
	Q_OBJECT

public:
	VDatePopup( QWidget * parent = 0, QDate = QDate::currentDate(), const char *name = 0 );
	~VDatePopup();

	QSize sizeHint() const;

	bool setDate( const QDate & );


	const QDate & getDate();
	const QDate & date() const;

	void setEnabled( bool );


protected:
	void resizeEvent( QResizeEvent * );

	QToolButton *yearForward;

	QToolButton *yearBackward;

	QToolButton *monthForward;

	QToolButton *monthBackward;

	QToolButton *selectMonth;

	QToolButton *selectYear;

	VDateTable *table;

	QSize maxMonthRect;

	protected slots:
	void dateChangedSlot( QDate );
	void tableClickedSlot();
	void monthForwardClicked();
	void monthBackwardClicked();
	void yearForwardClicked();
	void yearBackwardClicked();
	void selectMonthClicked();
	void selectYearClicked();

signals:
	void dateChanged( const QDate & );
	void dateSelected( const QDate & );
	void dateEntered( const QDate & );
	void tableClicked();

protected:
	virtual void virtual_hook( int id, void *data );

private:

	class VDatePopupPrivate;
	VDatePopupPrivate *d;
};

#endif
