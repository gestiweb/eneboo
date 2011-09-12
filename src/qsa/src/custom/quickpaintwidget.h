/****************************************************************************
** $Id: quickpaintwidget.h  1.1.5   edited 2006-02-23T15:39:57$
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

#ifndef QUICKPAINTWIDGET_H
#define QUICKPAINTWIDGET_H

#include <qwidget.h>
#include <qpixmap.h>

class QPainter;
class QTimer;

class PaintWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY( QString penColor READ penColor WRITE setPenColor )
    Q_PROPERTY( QString brushColor READ brushColor WRITE setBrushColor )
    Q_PROPERTY( QString backgroundColor READ backgroundColorEx WRITE setBackgroundColor )
    Q_PROPERTY( QString fontFamily READ fontFamily WRITE setFontFamily )
    Q_PROPERTY( int pointSize READ pointSize WRITE setPointSize )
    Q_PROPERTY( int penWidth READ penWidth WRITE setPenWidth )
    Q_PROPERTY( QPixmap pixmap READ canvasPixmap )
    Q_PROPERTY( QPixmap erasePixmap READ erasePixmap WRITE setErasePixmap )

public:
    PaintWidget( QWidget *parent=0 , const char *name=0 );
    ~PaintWidget();

    void setPenColor( const QString &color );
    void setBrushColor( const QString &color );
    void setBackgroundColor( const QString &color );
    void setPenWidth( int width );
    QString penColor() const;
    QString brushColor() const;
    QString backgroundColorEx() const;
    int penWidth() const;

    void setFontFamily( const QString &family );
    void setPointSize( int size );
    QString fontFamily() const;
    int pointSize() const;

    void setPaletteBackgroundColor( const QColor &color );

    QPixmap canvasPixmap() const { return buffer; }
    QPixmap erasePixmap() const { return ePixmap; }
    void setErasePixmap( const QPixmap &pm ) { ePixmap = pm; }

public slots:
    void setWindow( int x, int y, int w, int h );
    void setClipRect( int x, int y, int w, int h );
    void drawPoint( int x, int y );
    void moveTo( int x, int y );
    void lineTo( int x, int y );
    void drawLine( int x1, int y1, int x2, int y2 );
    void drawRect( int x, int y, int w, int h );
    void drawRoundRect( int x, int y, int w, int h, int xRnd, int yRnd );
    void drawEllipse( int x, int y, int w, int h );
    void drawArc( int x, int y, int w, int h, int a, int alen );
    void drawPie( int x, int y, int w, int h, int a, int alen );
    void drawChord( int x, int y, int w, int h, int a, int alen );
    void fillRect( int x, int y, int w, int h );
    void eraseRect( int x, int y, int w, int h );
    void drawText( int x, int y, const QString &text );
    int drawText( int x, int y, int w, int h, const QString &text, bool wordWrap );
    void drawPixmap( int x, int y, const QPixmap &pixmap );
    int textWidth( const QString &text );
    int textHeight();
    int textAscent();
    int textDescent();

    void flush();
    void erase();

signals:
    void updateContents();

protected:
    void resizeEvent( QResizeEvent *e );
    void paintEvent( QPaintEvent *e );

private:
    QTimer *updateTimer;
    QPixmap buffer;
    QPainter *painter;
    QRegion updateRegion;
    QPixmap ePixmap;

};

#endif
