/***************************************************************************
                        vdatetable.h  -  description
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

#ifndef VDATETABLE_H
#define VDATETABLE_H

#include <qapplication.h>
#include <qwidget.h>
#include <qvalidator.h>
#include <qgridview.h>
#include <qlineedit.h>
#include <qdatetime.h>

/**
* A table containing month names. It is used to pick a month directly.
* @internal
* @version $Id: vdatetable.h,v 1.2 2002/04/04 11:01:45 bryanbr Exp $
* @author Tim Gilman, Mirko Boehm
*/
class VDateInternalMonthPicker: public QGridView {
	Q_OBJECT

protected:
	/**
	  * Store the month that has been clicked [1..12].
	  */
	int result;
	/**
	  * the cell under mouse cursor when LBM is pressed
	  */
	short int activeCol;
	short int activeRow;
	/**
	  * Contains the largest rectangle needed by the month names.
	  */
	QRect max;

signals:
	/**
	  * This is send from the mouse click event handler.
	  */
	void closeMe( int );

public:
	/**
	  * The constructor.
	  */
	VDateInternalMonthPicker( int fontsize, QWidget * parent, const char *name = 0 );
	/**
	  * The size hint.
	  */
	QSize sizeHint() const;
	/**
	  * Return the result. 0 means no selection (reject()), 1..12 are the
	  * months.
	  */
	int getResult() const;

protected:
	/**
	  * Set up the painter.
	  */
	void setupPainter( QPainter * p );
	/**
	  * The resize event.
	  */
	void viewportResizeEvent( QResizeEvent * );
	/**
	  * Paint a cell. This simply draws the month names in it.
	  */
	virtual void paintCell( QPainter * painter, int row, int col );
	/**
	  * Catch mouse click and move events to paint a rectangle around the item.
	  */
	void contentsMousePressEvent( QMouseEvent * e );
	void contentsMouseMoveEvent( QMouseEvent * e );
	/**
	  * Emit monthSelected(int) when a cell has been released.
	  */
	void contentsMouseReleaseEvent( QMouseEvent * e );

private:
	class VDateInternalMonthPrivate;
	VDateInternalMonthPrivate *d;
};

/** Year selection widget.
* @internal
* @version $Id: vdatetable.h,v 1.2 2002/04/04 11:01:45 bryanbr Exp $
* @author Tim Gilman, Mirko Boehm
*/
class VDateInternalYearSelector: public QLineEdit {
	Q_OBJECT

protected:
	QIntValidator * val;
	int result;

	public slots:
	void yearEnteredSlot();

signals:
	void closeMe( int );

public:
	VDateInternalYearSelector( int fontsize, QWidget * parent = 0, const char *name = 0 );
	int getYear();
	void setYear( int year );

private:
	class VDateInternalYearPrivate;
	VDateInternalYearPrivate *d;
};

/**
 * Frame with popup menu behaviour.
 * @author Tim Gilman, Mirko Boehm
 * @version $Id: vdatetable.h,v 1.2 2002/04/04 11:01:45 bryanbr Exp $
 */
class VPopupFrame: public QFrame {
	Q_OBJECT

protected:
	/**
	  * The result. It is returned from exec() when the popup window closes.
	  */
	int result;
	/**
	  * Catch key press events.
	  */
	void keyPressEvent( QKeyEvent * e );
	/**
	  * The only subwidget that uses the whole dialog window.
	  */
	QWidget *main;

	public slots:
	/**
	  * Close the popup window. This is called from the main widget, usually.
	  * @p r is the result returned from exec().
	  */
	void close( int r );

public:
	/**
	  * The contructor. Creates a dialog without buttons.
	  */
	VPopupFrame( QWidget * parent = 0, const char *name = 0 );
	/**
	  * Set the main widget. You cannot set the main widget from the constructor,
	  * since it must be a child of the frame itselfes.
	  * Be careful: the size is set to the main widgets size. It is up to you to
	  * set the main widgets correct size before setting it as the main
	  * widget.
	  */
	void setMainWidget( QWidget * m );
	/**
	  * The resize event. Simply resizes the main widget to the whole
	  * widgets client size.
	  */
	void resizeEvent( QResizeEvent * );
	/**
	  * Open the popup window at position pos.
	  */
	void popup( const QPoint & pos );
	/**
	  * Execute the popup window.
	  */
	int exec( QPoint p );
	/**
	  * Dito.
	  */
	int exec( int x, int y );

private:

	virtual bool close( bool alsoDelete ) {
		return QFrame::close( alsoDelete );
	}
protected:
	virtual void virtual_hook( int id, void *data );
private:
	class VPopupFramePrivate;
	VPopupFramePrivate *d;
};

class VDateValidator: public QValidator {
public:
	VDateValidator( QWidget * parent = 0, const char *name = 0 );
	virtual State validate( QString &, int & ) const;
	virtual void fixup( QString & input ) const;
	State date( const QString &, QDate & ) const;
};

class VDateTable: public QGridView {
	Q_OBJECT

public:
	VDateTable( QWidget * parent = 0, QDate date = QDate::currentDate(), const char *name = 0, WFlags f = 0 );
	~VDateTable();
	virtual QSize sizeHint() const;

	/**
	   * Select and display this date.
	   */
	bool setDate( const QDate & );
	const QDate & getDate() const;


protected:
	/**
	   * Paint a cell.
	   */
	virtual void paintCell( QPainter *, int, int );
	/**
	   * Handle the resize events.
	   */
	virtual void viewportResizeEvent( QResizeEvent * );
	/**
	   * React on mouse clicks that select a date.
	   */
	virtual void contentsMousePressEvent( QMouseEvent * );
	virtual void keyPressEvent( QKeyEvent * e );
	virtual void focusInEvent( QFocusEvent * e );
	virtual void focusOutEvent( QFocusEvent * e );

	/**
	   * The currently selected date.
	   */
	QDate date;
	/**
	   * The day of the first day in the month [1..7].
	   */
	int firstday;
	/**
	   * The number of days in the current month.
	   */
	int numdays;
	/**
	   * The number of days in the previous month.
	   */
	int numDaysPrevMonth;
	/**
	   * unused
	   */
	bool unused_hasSelection;
	/**
	   * Save the size of the largest used cell content.
	   */
	QRect maxCell;

signals:
	/**
	   * The selected date changed.
	   */
	void dateChanged( QDate );
	/**
	   * A date has been selected by clicking on the table.
	   */
	void tableClicked();

protected:
	virtual void virtual_hook( int id, void *data );

private:
	class VDateTablePrivate;
	VDateTablePrivate *d;
};

#endif
