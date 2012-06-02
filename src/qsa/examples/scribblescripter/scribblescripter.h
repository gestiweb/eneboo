/****************************************************************************
** $Id: scribblescripter.h  1.1.5   edited 2003-06-18T18:37:16$
**
** Copyright ( C ) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef SCRIBBLE_H
#define SCRIBBLE_H

#include <qmainwindow.h>
#include <qsinterpreter.h>
#include <qsworkbench.h>
#include <qpen.h>
#include <qpoint.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qstring.h>
#include <qpointarray.h>
#include <qdockwindow.h>
#include <qlistview.h>
#include <qheader.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <qpainter.h>

class QMouseEvent;
class QResizeEvent;
class QPaintEvent;
class QToolButton;
class QSpinBox;

class Canvas : public QWidget
{
    Q_OBJECT

public:
    Canvas( QWidget *parent = 0, const char *name = 0 );

    void setPenColor( const QColor &c )
    { pen.setColor( c ); }

    void setPenWidth( int w )
    { pen.setWidth( w ); }

    QColor penColor()
    { return pen.color(); }

    int penWidth()
    { return pen.width(); }

    QPixmap& pixmap()
    { return buffer; }

    void setPixmap(QPixmap &pixmap)
    { buffer = pixmap; }

    void save( const QString &filename, const QString &format );
    void clearScreen();

protected:
    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void resizeEvent( QResizeEvent *e );
    void paintEvent( QPaintEvent *e );

    QPen pen;
    QPointArray polyline;

    bool mousePressed;
    QPixmap buffer;
};


static const char * function_xpm[] = {
"16 16 14 1",
"       c None",
".      c #000000",
"+      c #FCFC80",
"@      c #E0BC38",
"#      c #F0DC5C",
"$      c #585858",
"%      c #FFFF85",
"&      c #FFFF00",
"*      c #FFFFDE",
"=      c #C0C000",
"-      c #DEDEB6",
";      c #C1C10A",
">      c #FFFFC0",
",      c #FFFFFF",
"         ..     ",
"        .++..   ",
"       .+++++.  ",
"      .+++++@.  ",
"   .. .##++@@.  ",
"  .++..###@@@.  ",
" .+++++.##@@.   ",
".+++++@..#$%    ",
".##++@@$%.$&*$% ",
".###@@@.$&=&=&- ",
".###@@.  ;>%>=$.",
" ..#@. %&&%,%&&%",
"   ..  ..=>%>=..",
"        .&=&=&  ",
"       .% .& .% ",
"          .%    "};

static const char * class_xpm[] = {
"16 16 19 1",
"       c None",
".      c #000000",
"+      c #A4E8FC",
"@      c #24D0FC",
"#      c #001CD0",
"$      c #0080E8",
"%      c #C0FFFF",
"&      c #00FFFF",
"*      c #008080",
"=      c #00C0C0",
"-      c #585858",
";      c #FFFF85",
">      c #FFFF00",
",      c #FFFFDE",
"'      c #C0C000",
")      c #DEDEB6",
"!      c #C1C10A",
"~      c #FFFFC0",
"{      c #FFFFFF",
"     ..         ",
"    .++..       ",
"   .+++@@.      ",
"  .@@@@@#...    ",
"  .$$@@##.%%..  ",
"  .$$$##.%%%&&. ",
"  .$$$#.&&&&&*. ",
"   ...#.==-;**. ",
"   .++.-;=->,-; ",
"  .+++@@->'>'>) ",
" .@@@@@#.!~;~'-.",
" .$$@@#;>>;{;>>;",
" .$$$##..'~;~'..",
" .$$$##..>'>'>  ",
"  ..$#..; .> .; ",
"    ..    .;    "};

static const char * variable_xpm[] = {
"16 16 14 1",
"       c None",
".      c #000000",
"+      c #ACB4C0",
"@      c #8C748C",
"#      c #9C94A4",
"$      c #585858",
"%      c #FFFF85",
"&      c #FFFF00",
"*      c #FFFFDE",
"=      c #C0C000",
"-      c #DEDEB6",
";      c #C1C10A",
">      c #FFFFC0",
",      c #FFFFFF",
"                ",
"                ",
"                ",
"      ..        ",
"     .++..      ",
"    .+++++.     ",
"   .+++++@.     ",
"   .##++@@$%    ",
"   .###$%@$&*$% ",
"   .###@$&=&=&- ",
"    ..#@.;>%>=$.",
"      .%&&%,%&&%",
"       ..=>%>=..",
"        .&=&=&  ",
"       .% .& .% ",
"          .%    "};

class ScriptView : public QDockWindow
{
    Q_OBJECT

public:
    ScriptView( QSInterpreter *ip, Place p = InDock,
		QWidget * parent = 0, const char * name = 0, WFlags f = 0 )
	: QDockWindow( p, parent, name, f )
    {
	interpreter = ip;
#ifndef QSA_NO_IDE
	ide = new QSWorkbench( interpreter->project(), this );
#endif
	classPixmap = new QPixmap( class_xpm );
	varPixmap = new QPixmap( variable_xpm );
	funcPixmap = new QPixmap( function_xpm );

	QVBox *l = new QVBox( this );
	view     = new QListView( l, "Script List" );
	globals  = new QListViewItem( view, "Global" );
	classes  = new QListViewItem( view, "Classes" );

	globals->setSelectable( FALSE );
	globals->setPixmap( 0, *classPixmap );

	classes->setSelectable( FALSE );
	classes->setPixmap( 0, *classPixmap );

	view->addColumn( QString::null );	// We don't care
	view->header()->hide();			// since we hide it...
	view->setRootIsDecorated( TRUE );
	view->setMinimumSize( QSize(130, 10) );
	view->adjustSize();

	edit     = new QPushButton( "Edit Scripts", l );
	update   = new QPushButton( "Update", l );
	connect( edit, SIGNAL(clicked()), this, SLOT( openIDE() ) );
	connect( update, SIGNAL(clicked()), this, SLOT(updateScripts()) );
	connect( view, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(runScript(QListViewItem*)) );
	updateScripts();
	setWidget( l );
    }

    ~ScriptView()
    {
	delete classPixmap;
	delete varPixmap;
	delete funcPixmap;
    }

private slots:
    void updateScripts();
    void runScript( QListViewItem *item );
    void openIDE();

private:
    QSInterpreter *interpreter;
#ifndef QSA_NO_IDE
    QSWorkbench *ide;
#endif
    QListView *view;
    QPushButton *update, *edit;
    QListViewItem *globals, *classes;
    QPixmap *varPixmap;
    QPixmap *classPixmap;
    QPixmap *funcPixmap;
};


class Scribble : public QMainWindow
{
    Q_OBJECT

public:
    Scribble( QWidget *parent = 0, const char *name = 0 );

protected:
    QSProject *project;
    QSInterpreter *interpreter;
    Canvas* canvas;

    QSpinBox *bPWidth;
    QToolButton *bPColor, *bSave, *bClear;
    ScriptView *selector;

protected slots:
    void slotSave();
    void slotColor();
    void slotWidth( int );
    void slotClear();
};


// ------------------------------------------------------------------------------


class PixmapInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QColor color READ penColor WRITE setPenColor )
    Q_PROPERTY( QColor background READ backgroundColor WRITE setBackgroundColor )
    Q_PROPERTY( int penWidth READ penWidth WRITE setPenWidth )

public:
    PixmapInterface( QPixmap &pixmap, QObject *parent = 0, const char *name = 0 )
	: QObject( parent, name )
    {
	setBuffer( pixmap );
	pen.setColor( black );
    }

    // Not slots, since scriptuser should not access these...
    QPixmap& buffer()
    {
	p.end();
	return pix;
    }

    void setBuffer( QPixmap &pixmap )
    {
	pix = pixmap;
	p.begin( &pix );
	p.setPen( pen );
    }

    const QColor& penColor() const
    { return pen.color(); }

    void setPenColor( const QColor& colo )
    {
	color = colo;
	pen.setColor( color );
    }

    const QColor& backgroundColor() const
    { return p.backgroundColor(); }

    void setBackgroundColor( const QColor& colo  )
    { p.setBackgroundColor( colo ); }

    void setPenWidth( int w )
    { pen.setWidth( (uint)w ); }

    int penWidth() const
    { return pen.width(); }

public slots:
    PixmapInterface* pixmap()
    { return new PixmapInterface( pix ); }

    void setPixmap( PixmapInterface *Pixmap )
    { pix = Pixmap->buffer(); }

    void clear()
    { p.fillRect( 0, 0, pix.width(), pix.height(), p.backgroundColor() ); }

    void fillRect( int x, int y, int w, int h )
    { p.fillRect( x, y, w, h, color ); }

    void scale( double sx, double sy )
    { p.scale( sx, sy ); }

    void shear( double sh, double sv )
    { p.shear( sh, sv ); }

    void rotate( double val )
    { p.rotate( val ); }

    void translate( double dx, double dy )
    { p.translate( dx, dy ); }

    void drawPoint( int x, int y )
    { p.drawPoint( x, y ); }

    void moveTo( int x, int y )
    { p.moveTo( x, y ); }

    void lineTo( int x, int y )
    { p.lineTo( x, y ); }

    void drawLine( int x1, int y1, int x2, int y2 )
    { p.drawLine( x1, y1, x2, y2 ); }

    void drawRect( int x, int y, int w, int h )
    { p.drawRect( x, y, w, h ); }

    void drawWinFocusRect( int x, int y, int w, int h )
    { p.drawWinFocusRect( x, y, w, h ); }

    void drawRoundRect( int x, int y, int w, int h, int xRnd = 25, int yRnd = 25)
    { p.drawRoundRect( x, y, w, h, xRnd, yRnd ); }

    void drawEllipse( int x, int y, int w, int h )
    { p.drawEllipse( x, y, w, h ); }

    void drawArc( int x, int y, int w, int h, int a, int alen )
    { p.drawArc( x, y, w, h, a, alen ); }

    void drawPie( int x, int y, int w, int h, int a, int alen )
    {
	p.setPen( NoPen );
	p.setBrush( color );
	p.drawPie( x, y, w, h, a, alen );
	p.setBrush( NoBrush );
	p.setPen( color );
    }

    void drawChord( int x, int y, int w, int h, int a, int alen )
    { p.drawChord( x, y, w, h, a, alen ); }

    void drawText( QString text, int x, int y )
    { p.drawText( x, y, text); }

    void eraseRect( int x, int y, int w, int h )
    { p.eraseRect( x, y, w, h ); }

    void setWindow( int x, int y, int w, int h )
    { p.setWindow( x, y, w, h ); }

    void setViewport( int x, int y, int w, int h )
    { p.setWindow( x, y, w, h ); }

    void resize( int w, int h )
    {
	p.end();
	pix.resize( w, h );
	p.begin( &pix );
    }

private:
    QPixmap pix;
    QPainter p;
    QPen pen;
    QColor color;
};



class CanvasInterface : public QObject
{
    Q_OBJECT

public:
    CanvasInterface( Canvas *mainCanvas, QObject *parent = 0, const char *name = 0 )
	: QObject( parent, name )
    {
	kanwaz = mainCanvas;
    }

public slots:
    PixmapInterface* pixmap() const
    {
	return new PixmapInterface( kanwaz->pixmap() );
    }

    void setPixmap( PixmapInterface *Pixmap )
    {
	kanwaz->setPixmap( Pixmap->buffer() );
	kanwaz->repaint( FALSE );
    }

    int width()
    {
	return kanwaz->pixmap().width();
    }

    int height()
    {
	return kanwaz->pixmap().height();
    }

    int visibleWidth()
    {
	return kanwaz->width();
    }

    int visibleHeight()
    {
	return kanwaz->height();
    }

private:
    Canvas *kanwaz;
};

#endif
