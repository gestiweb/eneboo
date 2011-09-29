/****************************************************************************
** $Id: quickpaintwidget.cpp  1.1.5   edited 2006-02-23T15:39:57$
**
** Copyright (C) 2001-2006 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt Script for Applications framework (QSA).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding a valid QSA Beta Evaluation Version license may use
** this file in accordance with the QSA Beta Evaluation Version License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about QSA Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
*****************************************************************************/

#include "quickpaintwidget.h"
#include <qpainter.h>
#include <qtimer.h>
#include <qimage.h>
#include <qbuffer.h>

PaintWidget::PaintWidget( QWidget *parent, const char *name )
    : QWidget( parent, name, WRepaintNoErase | WResizeNoErase ), buffer( size() )
{
    updateTimer = new QTimer( this );
    connect( updateTimer, SIGNAL( timeout() ),
	     this, SLOT( flush() ) );
    painter = new QPainter;
    painter->begin( &buffer );
    setBackgroundColor( "white" );
    buffer.fill( paletteBackgroundColor() );
    updateRegion = QRegion();
}

PaintWidget::~PaintWidget()
{
    delete painter;
}

void PaintWidget::setPenColor( const QString &color )
{
    QPen pen = painter->pen();
    pen.setColor( QColor( color ) );
    painter->setPen( pen );
}

void PaintWidget::setBrushColor( const QString &color )
{
    QBrush brush = painter->brush();
    brush.setColor( QColor( color ) );
    brush.setStyle( SolidPattern );
    painter->setBrush( brush );
}

void PaintWidget::setBackgroundColor( const QString &color )
{
    setPaletteBackgroundColor( QColor( color ) );
    painter->fillRect( 0, 0, width(), height(), paletteBackgroundColor() );
}

void PaintWidget::setPaletteBackgroundColor( const QColor &color )
{
    painter->setBackgroundColor( color );
    QWidget::setPaletteBackgroundColor( color );
    painter->fillRect( 0, 0, width(), height(), paletteBackgroundColor() );
}

void PaintWidget::setPenWidth( int width )
{
    QPen pen = painter->pen();
    pen.setWidth( width );
    painter->setPen( pen );
}

QString PaintWidget::penColor() const
{
    return painter->pen().color().name();
}

QString PaintWidget::brushColor() const
{
    return painter->brush().color().name();
}

QString PaintWidget::backgroundColorEx() const
{
    return paletteBackgroundColor().name();
}

int PaintWidget::penWidth() const
{
    return painter->pen().width();
}

void PaintWidget::setFontFamily( const QString &family )
{
    QFont f( painter->font() );
    f.setFamily( family );
    painter->setFont( f );
}

void PaintWidget::setPointSize( int size )
{
    QFont f( painter->font() );
    f.setPointSize( size );
    painter->setFont( f );
}

QString PaintWidget::fontFamily() const
{
    return painter->font().family();
}

int PaintWidget::pointSize() const
{
    return painter->font().pointSize();
}

#define START_PAINTER updateTimer->stop(); int pw = penWidth(); Q_UNUSED( pw )
#define END_PAINTER updateTimer->start( 0, TRUE )
#define UNITE( r ) updateRegion = updateRegion.unite( r )

void PaintWidget::setClipRect( int x, int y, int w, int h )
{
    painter->setClipRect( QRect( x, y, w, h ) );
}

void PaintWidget::setWindow( int x, int y, int w, int h )
{
    painter->setWindow( x, y, w, h );
}

void PaintWidget::drawPoint( int x, int y )
{
    START_PAINTER;
    painter->drawPoint( x, y );
    UNITE( painter->xForm( QRect( x - pw / 2, y - pw / 2, pw, pw ) ) );
    END_PAINTER;
}

void PaintWidget::moveTo( int x, int y )
{
    START_PAINTER;
    painter->moveTo( x, y );
    END_PAINTER;
}

void PaintWidget::lineTo( int x, int y )
{
    START_PAINTER;
    painter->lineTo( x, y );
    UNITE( painter->xForm( QRect( 0, 0, width(), height() ) ) );
    END_PAINTER;
}

void PaintWidget::drawLine( int x1, int y1, int x2, int y2 )
{
    START_PAINTER;
    painter->drawLine( x1, y1, x2, y2 );
    QRect r( x1, y1, x2 - x1 + 1, y2 - y1 + 1 );
    r = r.normalize();
    r.setLeft( r.left() - pw );
    r.setTop( r.top() - pw );
    r.setBottom( r.bottom() + pw );
    r.setRight( r.right() + pw );
    UNITE( painter->xForm( r ) );
    END_PAINTER;
}

void PaintWidget::drawRect( int x, int y, int w, int h )
{
    START_PAINTER;
    painter->drawRect( x, y, w, h );
    UNITE( painter->xForm( QRect( x - pw / 2, y - pw / 2, w + pw / 2, h + pw / 2 ) ) );
    END_PAINTER;
}

void PaintWidget::drawRoundRect( int x, int y, int w, int h, int xRnd, int yRnd )
{
    START_PAINTER;
    painter->drawRoundRect( x, y, w, h, xRnd, yRnd );
    UNITE( painter->xForm( QRect( x - pw / 2, y - pw / 2, w + pw / 2, h + pw / 2 ) ) );
    END_PAINTER;
}

void PaintWidget::drawEllipse( int x, int y, int w, int h )
{
    START_PAINTER;
    painter->drawEllipse( x, y, w, h );
    UNITE( painter->xForm( QRect( x - pw / 2, y - pw / 2, w + pw / 2, h + pw / 2 ) ) );
    END_PAINTER;
}

void PaintWidget::drawArc( int x, int y, int w, int h, int a, int alen )
{
    START_PAINTER;
    painter->drawArc( x, y, w, h, a, alen );
    UNITE( painter->xForm( QRect( x - pw / 2, y - pw / 2, w + pw / 2, h + pw / 2 ) ) );
    END_PAINTER;
}

void PaintWidget::drawPie( int x, int y, int w, int h, int a, int alen )
{
    START_PAINTER;
    painter->drawPie( x, y, w, h ,a, alen );
    UNITE( painter->xForm( QRect( x - pw / 2, y - pw / 2, w + pw / 2, h + pw / 2 ) ) );
    END_PAINTER;
}

void PaintWidget::drawChord( int x, int y, int w, int h, int a, int alen )
{
    START_PAINTER;
    painter->drawChord( x, y, w, h ,a, alen );
    UNITE( painter->xForm( QRect( x - pw / 2, y - pw / 2, w + pw / 2, h + pw / 2 ) ) );
    END_PAINTER;
}

void PaintWidget::fillRect( int x, int y, int w, int h )
{
    START_PAINTER;
    painter->fillRect( x, y, w, h, painter->brush() );
    UNITE( painter->xForm( QRect( x - pw / 2, y - pw / 2, w + pw / 2, h + pw / 2 ) ) );
    END_PAINTER;
}

void PaintWidget::eraseRect( int x, int y, int w, int h )
{
    START_PAINTER;
    if ( ePixmap.isNull() )
	painter->eraseRect( x, y, w, h );
    else
	painter->drawTiledPixmap( x, y, w, h, ePixmap, x, y );
    UNITE( painter->xForm( QRect( x - pw / 2, y - pw / 2, w + pw / 2, h + pw / 2 ) ) );
    END_PAINTER;
}

void PaintWidget::erase()
{
    START_PAINTER;
    if ( ePixmap.isNull() )
	painter->eraseRect( 0, 0, width(), height() );
    else
	painter->drawTiledPixmap( 0, 0, width(), height(), ePixmap, 0, 0 );
    UNITE( painter->xForm( QRect( 0, 0, width(), height() ) ) );
    END_PAINTER;
}

void PaintWidget::drawText( int x, int y, const QString &text )
{
    START_PAINTER;
    painter->drawText( x, y, text );
    updateRegion = painter->xForm( QRect( 0, 0, width(), height() ) );
    END_PAINTER;
}

int PaintWidget::drawText( int x, int y, int w, int h, const QString &text, bool wordWrap )
{
    START_PAINTER;
    QRect br;
    painter->drawText( x, y, w, h, wordWrap ? Qt::WordBreak : 0, text, -1, &br );
    UNITE( painter->xForm( QRect( x - pw / 2, y - pw / 2, w + pw / 2, QMAX( h + pw / 2, br.height() ) ) ) );
    END_PAINTER;
    return br.height();
}

void PaintWidget::drawPixmap( int x, int y, const QPixmap &pixmap )
{
    START_PAINTER;
    painter->drawPixmap( x, y, pixmap );
    UNITE( painter->xForm( QRect( x, y, pixmap.width(), pixmap.height() ) ) );
    END_PAINTER;
}

static bool useRegion = FALSE;

void PaintWidget::flush()
{
    useRegion = TRUE;
    repaint( updateRegion.boundingRect(), FALSE );
    useRegion = FALSE;
    updateRegion = QRegion();
}

void PaintWidget::resizeEvent( QResizeEvent *e )
{
    // ### save painter state and restore on begin
    painter->end();
    buffer.resize( size() );
    painter->begin( &buffer );

    if ( width() > e->oldSize().width() ) {
	if ( ePixmap.isNull() )
	    painter->fillRect( e->oldSize().width(), 0, width() - e->oldSize().width() + 1,
			       height(), paletteBackgroundColor() );
	else
	    painter->drawTiledPixmap( 0, 0, width(), height(), ePixmap, 0, 0 );
    }
    if ( height() > e->oldSize().height() ) {
	if ( ePixmap.isNull() )
	    painter->fillRect( 0, e->oldSize().height(), width(), height() - e->oldSize().height() + 1,
			       paletteBackgroundColor() );
	else
	    painter->drawTiledPixmap( 0, 0, width(), height(), ePixmap, 0, 0 );
    }
    updateRegion = QRegion();
    emit updateContents();
}

void PaintWidget::paintEvent( QPaintEvent *e )
{
    QRegion reg;
    if ( useRegion )
	reg = updateRegion;
    else
	reg = e->region();
    for ( int i = 0; i < (int)reg.rects().count(); ++i ) {
	QRect r = reg.rects()[ i ];
	bitBlt( this, r.x(), r.y(), &buffer, r.x(), r.y(), r.width(), r.height() );
    }
}

int PaintWidget::textWidth( const QString &text )
{
    return painter->fontMetrics().width( text );
}

int PaintWidget::textHeight()
{
    return painter->fontMetrics().height();
}

int PaintWidget::textAscent()
{
    return painter->fontMetrics().ascent();
}

int PaintWidget::textDescent()
{
    return painter->fontMetrics().descent();
}
