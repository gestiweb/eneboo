/****************************************************************************
**
** Implementation of Qt/Embedded Windows-style decorations
**
** Created : 000101
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free Qt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/
#include <qapplication.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include "qwswindowsdecoration_qws.h"

#ifndef QT_NO_QWS_WINDOWS_WM_STYLE

#ifndef QT_NO_IMAGEIO_XPM

// #undef BORDER_WIDTH
// #define BORDER_WIDTH	3

/* XPM */
static const char * const win_menu_xpm[] = {
/* width height ncolors chars_per_pixel */
"16 16 12 1",
/* colors */
"       s None  c None",
"* c #000000",
". c #336600",
"X c #666600",
"o c #99CC00",
"O c #999933",
"+ c #333300",
"@ c #669900",
"# c #999900",
"$ c #336633",
"% c #666633",
"& c #99CC33",
/* pixels */
"oooooooooooooooo",
"oooooooooooooooo",
"ooooo#.++X#ooooo",
"ooooX******Xoooo",
"oooX**XO#%**X&oo",
"oo#**Ooo&@O**Ooo",
"oo.*Xoo#+*@X*Xoo",
"oo+*OoO+*+O#*+oo",
"oo+*#O+**+##*+oo",
"oo.*%@*++*+.*Xoo",
"oo#**O@OO+***#oo",
"oooX**X##$***Ooo",
"ooooX********Xoo",
"oooo&OX++X#OXooo",
"oooooooooooooooo",
"                "};

static const char * const win_close_xpm[] = {
"16 16 4 1",
"       s None  c None",
".      c #000000",
"X      c #FFFFFF",
"Y      c #707070",
"XXXXXXXXXXXXXXXY",
"X              .",
"X              .",
"X  Y.      .Y  .",
"X   ..    ..   .",
"X    ..  ..    .",
"X     .YY.     .",
"X     Y..Y     .",
"X     .YY.     .",
"X    ..  ..    .",
"X   ..    ..   .",
"X  Y.      .Y  .",
"X              .",
"X              .",
"Y...............",
"                "};

static const char * const win_maximize_xpm[] = {
"16 16 4 1",
"       s None  c None",
".      c #000000",
"X      c #FFFFFF",
"Y      c #707070",
"XXXXXXXXXXXXXXXY",
"X              .",
"X              .",
"X  ..........  .",
"X  ..........  .",
"X  .        .  .",
"X  .        .  .",
"X  .        .  .",
"X  .        .  .",
"X  .        .  .",
"X  .        .  .",
"X  ..........  .",
"X              .",
"X              .",
"Y...............",
"                "};

static const char * const win_minimize_xpm[] = {
"16 16 4 1",
"       s None  c None",
".      c #000000",
"X      c #FFFFFF",
"Y      c #707070",
"XXXXXXXXXXXXXXXY",
"X              .",
"X              .",
"X              .",
"X              .",
"X              .",
"X              .",
"X              .",
"X              .",
"X              .",
"X   ........   .",
"X   ........   .",
"X              .",
"X              .",
"Y...............",
"                "};

static const char * const win_normalize_xpm[] = {
"16 16 4 1",
"       s None  c None",
".      c #000000",
"X      c #FFFFFF",
"Y      c #707070",
"XXXXXXXXXXXXXXXY",
"X              .",
"X    ......... .",
"X    ......... .",
"X    .       . .",
"X    .       . .",
"X .........  . .",
"X .........  . .",
"X .       .  . .",
"X .       .... .",
"X .       .    .",
"X .       .    .",
"X .........    .",
"X              .",
"Y...............",
"                "};

#endif // QT_NO_IMAGEIO_XPM


QWSWindowsDecoration::QWSWindowsDecoration()
    : QWSDefaultDecoration()
{
}

QWSWindowsDecoration::~QWSWindowsDecoration()
{
}

const char **QWSWindowsDecoration::menuPixmap()
{
#ifndef QT_NO_IMAGEIO_XPM
    return (const char **)win_menu_xpm;
#else
    return 0;
#endif
}

const char **QWSWindowsDecoration::closePixmap()
{
#ifndef QT_NO_IMAGEIO_XPM
    return (const char **)win_close_xpm;
#else
    return 0;
#endif
}

const char **QWSWindowsDecoration::minimizePixmap()
{
#ifndef QT_NO_IMAGEIO_XPM
    return (const char **)win_minimize_xpm;
#else
    return 0;
#endif
}

const char **QWSWindowsDecoration::maximizePixmap()
{
#ifndef QT_NO_IMAGEIO_XPM
    return (const char **)win_maximize_xpm;
#else
    return 0;
#endif
}

const char **QWSWindowsDecoration::normalizePixmap()
{
#ifndef QT_NO_IMAGEIO_XPM
    return (const char **)win_normalize_xpm;
#else
    return 0;
#endif
}

int QWSWindowsDecoration::getTitleWidth(const QWidget *widget)
{
    return widget->width();
}

/*
    If rect is empty, no frame is added. (a hack, really)
*/
QRegion QWSWindowsDecoration::region(const QWidget *widget, const QRect &rect, QWSDecoration::Region type)
{
    int titleHeight = getTitleHeight(widget);
//    int titleWidth = getTitleWidth(widget);
//    int bw = rect.isEmpty() ? 0 : BORDER_WIDTH;
    QRegion region;

    switch (type) {
	case Maximize: {
	    QRect r(rect.right() - 2*titleHeight + 1, rect.top() - titleHeight,
			    titleHeight, titleHeight);
	    if (r.left() > rect.left() + titleHeight)
		    region = r;
	    break;
	}
	case Minimize: {
	    QRect r(rect.right() - 3*titleHeight + 4, rect.top() - titleHeight,
		    	    titleHeight, titleHeight);
	    if (r.left() > rect.left() + titleHeight)
		    region = r;
	    break;
	}
	case Menu:
	case Close:
	case All:
	case Title:
	case Top:
	case Left:
	case Right:
	case Bottom:
	case TopLeft:
	case TopRight:
	case BottomLeft:
	case BottomRight:
	default:
	    region = QWSDefaultDecoration::region(widget, rect, type);
	    break;
    }

    return region;
}

void QWSWindowsDecoration::paint(QPainter *painter, const QWidget *widget)
{
    int titleWidth = getTitleWidth(widget);
    int titleHeight = getTitleHeight(widget);

    QRect rect(widget->rect());

    // Border rect
    QRect br( rect.left() - BORDER_WIDTH,
                rect.top() - BORDER_WIDTH - titleHeight,
                rect.width() + 2 * BORDER_WIDTH,
                rect.height() + BORDER_WIDTH + BOTTOM_BORDER_WIDTH + titleHeight );

    // title bar rect
    QRect tr( 0, -titleHeight,  titleWidth - 1, titleHeight - 1);

    QRegion oldClip = painter->clipRegion();
    painter->setClipRegion( oldClip - QRegion( tr ) );	// reduce flicker

#ifndef QT_NO_PALETTE
    const QColorGroup &cg = QApplication::palette().active();
//    const QColorGroup &cg = widget->palette().active();

#if !defined(QT_NO_DRAWUTIL)
    qDrawWinPanel(painter, br.x(), br.y(), br.width(),
		  br.height() - 4, cg, FALSE,
		  &cg.brush(QColorGroup::Background));
#endif

    painter->setClipRegion( oldClip );

    if (titleWidth > 0) {
	QBrush titleBrush;
	QPen   titlePen;
	int    titleLeft = titleHeight + 4;

	if (widget == qApp->activeWindow()) {
	    titleBrush = cg.brush(QColorGroup::Highlight);
	    titlePen   = cg.color(QColorGroup::HighlightedText);
	} else {
	    titleBrush = cg.brush(QColorGroup::Background);
	    titlePen   = cg.color(QColorGroup::Text);
	}

#define CLAMP(x, y)	    ( ((x) > (y)) ? (y) : (x) )

	int x = tr.x();
	QColor c1 = titleBrush.color();
	QColor c2 = titleBrush.color();
	
	if (widget == qApp->activeWindow()) {
	    c1 = QColor( 0x10, 0x20, 0x90 ); // '98 Blue
	    // c2 = QColor( 0x50, 0x60, 0xD0 );
	    c2 = QColor( 0x70, 0x70, 0x70 );
	    // c1 = QColor( 0xBA, 0xC4, 0x05 ); // Trolltech Green
	    // c1 = QColor( 0xAA, 0xB4, 0x00 ); // Trolltech Green
	    // c2 = QColor( 0x20, 0x20, 0x20 );
	} else {
	    c1 = QColor( 0x20, 0x30, 0x50 );
	    // c2 = QColor( 0x60, 0x70, 0x90 );
	    // c2 = QColor( 0x40, 0x40, 0x40 );
	    c2 = QColor( 0x70, 0x70, 0x70 );
	    titlePen = QPen( QColor( 0x90, 0x90, 0x90 ) );
	}
	
	int r = c1.red();
	int g = c1.green();
	int b = c1.blue();
	for (int i = 0; i < 100; i++) {
	    int dr = r + (c2.red() * i) / 100;
	    int dg = g + (c2.green() * i) / 100;
	    int db = b + (c2.blue() * i) / 100;
	    x = tr.x() + (tr.width() * i) / 100;
	    int x2 = tr.x() + (tr.width() * (i + 1)) / 100;
	    QBrush tbr = QBrush( QColor( CLAMP(dr,255), CLAMP(dg,255), CLAMP(db,255) ) );
	    painter->fillRect( x, tr.y(), x2 - x, tr.height(), tbr);
	}
	
#ifndef QT_NO_WIDGET_TOPEXTRA
	painter->setPen(titlePen);
	painter->setFont(widget->font());
	painter->drawText( titleLeft, -titleHeight,
	 		rect.width() - titleHeight - 10, titleHeight-1,
			QPainter::AlignVCenter, widget->caption());
#endif
    }

#endif //QT_NO_PALETTE

}

void QWSWindowsDecoration::paintButton(QPainter *painter, const QWidget *w,
			QWSDecoration::Region type, int state)
{
#ifndef QT_NO_PALETTE
    const QColorGroup &cg = QApplication::palette().active();
//    const QColorGroup &cg = w->palette().active();
    QRect brect(region(w, w->rect(), type).boundingRect());
    int xoff=2;
    int yoff=2;
    const QPixmap *pm=pixmapFor(w,type,state & QWSButton::On, xoff, yoff);
    if ((state & QWSButton::MouseOver) && (state & QWSButton::Clicked))
	xoff++, yoff++;
    if (type != Menu)
	painter->fillRect(brect.x()+xoff+1, brect.y()+yoff, 16, 15, cg.brush(QColorGroup::Background));
    if (pm) painter->drawPixmap(brect.x()+xoff+1, brect.y()+yoff, *pm);
#endif
}

#endif // QT_NO_QWS_WINDOWS_WM_STYLE
