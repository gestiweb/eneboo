/****************************************************************************
** $Id: $
**
** Copyright ( C ) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "scribblescripter.h"

#include <qapplication.h>
#include <qcolordialog.h>
#include <qcursor.h>
#include <qdockwindow.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qimage.h>
#include <qintdict.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qrect.h>
#include <qspinbox.h>
#include <qstrlist.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qvaluelist.h>
#include <qvariant.h>
#include <qmessagebox.h>

#include <qsproject.h>
#include <qsinputdialogfactory.h>

const bool no_writing = FALSE;

Canvas::Canvas( QWidget *parent, const char *name )
    : QWidget( parent, name, WStaticContents ), pen( Qt::red, 3 ), polyline(3),
      mousePressed( FALSE ), buffer( width(), height() )
{

    if ((qApp->argc() > 0) && !buffer.load(qApp->argv()[1]))
	buffer.fill( colorGroup().base() );
    setBackgroundMode( QWidget::PaletteBase );
#ifndef QT_NO_CURSOR
    setCursor( Qt::crossCursor );
#endif
}

void Canvas::save( const QString &filename, const QString &format )
{
    if ( !no_writing )
	buffer.save( filename, format.upper() );
}

void Canvas::clearScreen()
{
    buffer.fill( colorGroup().base() );
    repaint( FALSE );
}

void Canvas::mousePressEvent( QMouseEvent *e )
{
    mousePressed = TRUE;
    polyline[2] = polyline[1] = polyline[0] = e->pos();
}

void Canvas::mouseReleaseEvent( QMouseEvent * )
{
    mousePressed = FALSE;
}

void Canvas::mouseMoveEvent( QMouseEvent *e )
{
    if ( mousePressed ) {
	QPainter painter;
	painter.begin( &buffer );
	painter.setPen( pen );
	polyline[2] = polyline[1];
	polyline[1] = polyline[0];
	polyline[0] = e->pos();
	painter.drawPolyline( polyline );
	painter.end();

	QRect r = polyline.boundingRect();
	r = r.normalize();
	r.setLeft( r.left() - penWidth() );
	r.setTop( r.top() - penWidth() );
	r.setRight( r.right() + penWidth() );
	r.setBottom( r.bottom() + penWidth() );

	bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );
    }
}

void Canvas::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );

    int w = width();
    int h = height();

    QPixmap tmp( buffer );
    buffer.resize( w, h );
    buffer.fill( colorGroup().base() );
    bitBlt( &buffer, 0, 0, &tmp, 0, 0, tmp.width(), tmp.height() );
}

void Canvas::paintEvent( QPaintEvent *e )
{
    QWidget::paintEvent( e );

    QMemArray<QRect> rects = e->region().rects();
    for ( uint i = 0; i < rects.count(); i++ ) {
	QRect r = rects[(int)i];
	bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );
    }
}

//------------------------------------------------------

void ScriptView::updateScripts()
{
    if (view) {
	QListViewItem *tmp = 0, *func;
	while ( (tmp = globals->firstChild()) )
	    globals->takeItem( tmp );
	while ( (tmp = classes->firstChild()) )
	    classes->takeItem( tmp );

	interpreter->project()->commitEditorContents();

	QStringList flist = interpreter->functions();
	for (QStringList::iterator x = flist.begin();
	     x != flist.end(); x++) {
	    func = new QListViewItem( globals, *x );
	    func->setPixmap( 0, *funcPixmap );
	}

	QStringList clist = interpreter->classes( QSInterpreter::GlobalClasses );
	for (QStringList::iterator y = clist.begin();
	     y != clist.end(); y++) {
	    tmp = new QListViewItem( classes, *y );
	    tmp->setSelectable( FALSE );
	    tmp->setPixmap( 0, *varPixmap );
	    QStringList flist = interpreter->functions( *y );
	    for (QStringList::iterator i = flist.begin();
		 i != flist.end(); i++) {
		func = new QListViewItem( tmp, *i );
		func->setPixmap( 0, *funcPixmap );
	    }
	}

	globals->setOpen( TRUE );
	classes->setOpen( TRUE );
    }
}

void ScriptView::runScript( QListViewItem *item )
{
    QValueList<QVariant> none;
    if ( item->parent() == globals ) {
	QString funcName  = item->text(0);
	interpreter->project()->commitEditorContents();
	interpreter->call( funcName, none );
    }
}

void ScriptView::openIDE()
{
#ifndef QSA_NO_IDE
    ide->open();
#else
    QMessageBox::information( this, "Disabled feature",
			      "QSA Workbench, the QSA graphical scripting environent,\n"
			      "has been disabled. Reconfigure to enable",
			      QMessageBox::Ok );
#endif
}

//------------------------------------------------------

Scribble::Scribble( QWidget *parent, const char *name )
    : QMainWindow( parent, name )
{
    canvas = new Canvas( this );
    setCentralWidget( canvas );

    QToolBar *tools = new QToolBar( this );

    bSave = new QToolButton( QPixmap(), "Save", "Save as PNG image", this, SLOT( slotSave() ), tools );
    bSave->setText( "Save As..." );

    tools->addSeparator();

    bPColor = new QToolButton( QPixmap(), "Choose Pen Color", "Choose Pen Color", this, SLOT( slotColor() ), tools );
    bPColor->setText( "Choose Pen Color..." );

    tools->addSeparator();

    bPWidth = new QSpinBox( 1, 20, 1, tools );
    QToolTip::add( bPWidth, "Choose Pen Width" );
    connect( bPWidth, SIGNAL( valueChanged( int ) ), this, SLOT( slotWidth( int ) ) );
    bPWidth->setValue( 3 );

    tools->addSeparator();

    bClear = new QToolButton( QPixmap(), "Clear Screen", "Clear Screen", this, SLOT( slotClear() ), tools );
    bClear->setText( "Clear Screen" );

    project = new QSProject( this, "scribblescript_project" );
    interpreter = project->interpreter();
    QSInputDialogFactory *factory = new QSInputDialogFactory();
    interpreter->addObjectFactory( factory );

    project->addObject( new CanvasInterface( canvas, this, "Canvas" ) );
    project->load( "scribblescript.qsa" );
    connect( project, SIGNAL( projectEvaluated() ), project, SLOT( save() ) );

    selector = new ScriptView( interpreter, ScriptView::InDock, this );
    selector->setVerticallyStretchable( TRUE );
    selector->setCaption( "ScribbleScripts" );
    selector->setResizeEnabled( TRUE );
    addDockWindow( selector, DockLeft );
}

void Scribble::slotSave()
{
    QPopupMenu *menu = new QPopupMenu( 0 );
    QIntDict<QString> formats;
    formats.setAutoDelete( TRUE );

    for ( unsigned int i = 0; i < QImageIO::outputFormats().count(); i++ ) {
	QString str = QString( QImageIO::outputFormats().at( i ) );
	formats.insert( menu->insertItem( QString( "%1..." ).arg( str ) ), new QString( str ) );
    }

    menu->setMouseTracking( TRUE );
    int id = menu->exec( bSave->mapToGlobal( QPoint( 0, bSave->height() + 1 ) ) );

    if ( id != -1 ) {
	QString format = *formats[ id ];

	QString filename = QFileDialog::getSaveFileName( QString::null, QString( "*.%1" ).arg( format.lower() ), this );
	if ( !filename.isEmpty() )
	    canvas->save( filename, format );
    }

    delete menu;
}

void Scribble::slotColor()
{
    QColor c = QColorDialog::getColor( canvas->penColor(), this );
    if ( c.isValid() )
	canvas->setPenColor( c );
}

void Scribble::slotWidth( int w )
{
    canvas->setPenWidth( w );
}

void Scribble::slotClear()
{
    canvas->clearScreen();
}
