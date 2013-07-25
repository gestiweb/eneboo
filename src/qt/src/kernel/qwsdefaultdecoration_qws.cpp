/****************************************************************************
**
** Implementation of Qt/Embedded window manager decorations
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
#include "qwsdefaultdecoration_qws.h"

#ifndef QT_NO_QWS_MANAGER

QPixmap * QWSDefaultDecoration::staticMenuPixmap=0;
QPixmap * QWSDefaultDecoration::staticClosePixmap=0;
QPixmap * QWSDefaultDecoration::staticMinimizePixmap=0;
QPixmap * QWSDefaultDecoration::staticMaximizePixmap=0;
QPixmap * QWSDefaultDecoration::staticNormalizePixmap=0;

#ifndef QT_NO_IMAGEIO_XPM

/* XPM */
static const char * const default_menu_xpm[] = {
/* width height ncolors chars_per_pixel */
"16 16 11 1",
/* colors */
"  c #000000",
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
"ooooX      Xoooo",
"oooX  XO#%  X&oo",
"oo#  Ooo&@O  Ooo",
"oo. Xoo#+ @X Xoo",
"oo+ OoO+ +O# +oo",
"oo+ #O+  +## +oo",
"oo. %@ ++ +. Xoo",
"oo#  O@OO+   #oo",
"oooX  X##$   Ooo",
"ooooX        Xoo",
"oooo&OX++X#OXooo",
"oooooooooooooooo",
"oooooooooooooooo"
};

static const char * const default_close_xpm[] = {
"16 16 3 1",
"       s None  c None",
".      c #ffffff",
"X      c #707070",
"                ",
"                ",
"  .X        .X  ",
"  .XX      .XX  ",
"   .XX    .XX   ",
"    .XX  .XX    ",
"     .XX.XX     ",
"      .XXX      ",
"      .XXX      ",
"     .XX.XX     ",
"    .XX  .XX    ",
"   .XX    .XX   ",
"  .XX      .XX  ",
"  .X        .X  ",
"                ",
"                "};

static const char * const default_maximize_xpm[] = {
"16 16 3 1",
"       s None  c None",
".      c #ffffff",
"X      c #707070",
"                ",
"                ",
"  ...........   ",
"  .XXXXXXXXXX   ",
"  .X       .X   ",
"  .X       .X   ",
"  .X       .X   ",
"  .X       .X   ",
"  .X       .X   ",
"  .X       .X   ",
"  .X       .X   ",
"  .X........X   ",
"  .XXXXXXXXXX   ",
"                ",
"                ",
"                "};

static const char * const default_minimize_xpm[] = {
"16 16 3 1",
"       s None  c None",
".      c #ffffff",
"X      c #707070",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"       ...      ",
"       . X      ",
"       .XX      ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                "};

static const char * const default_normalize_xpm[] = {
"16 16 3 1",
"       s None  c None",
".      c #ffffff",
"X      c #707070",
"                ",
"                ",
"     ........   ",
"     .XXXXXXXX  ",
"     .X     .X  ",
"     .X     .X  ",
"  ....X...  .X  ",
"  .XXXXXXXX .X  ",
"  .X     .XXXX  ",
"  .X     .X     ",
"  .X     .X     ",
"  .X......X     ",
"  .XXXXXXXX     ",
"                ",
"                ",
"                "};

#endif // QT_NO_IMAGEIO_XPM

QWSDefaultDecoration::QWSDefaultDecoration()
    : QWSDecoration()
{
}

QWSDefaultDecoration::~QWSDefaultDecoration()
{
    delete staticMenuPixmap;
    delete staticClosePixmap;
    delete staticMinimizePixmap;
    delete staticMaximizePixmap;
    delete staticNormalizePixmap;

    // This makes it safe to delete and then create a QWSDefaultDecoration
    staticMenuPixmap = 0;
    staticClosePixmap = 0;
    staticMinimizePixmap = 0;
    staticMaximizePixmap = 0;
    staticNormalizePixmap = 0;
}

#ifndef QT_NO_IMAGEIO_XPM
const char **QWSDefaultDecoration::menuPixmap()
{
    return (const char **)default_menu_xpm;
}

const char **QWSDefaultDecoration::closePixmap()
{
    return (const char **)default_close_xpm;
}

const char **QWSDefaultDecoration::minimizePixmap()
{
    return (const char **)default_minimize_xpm;
}

const char **QWSDefaultDecoration::maximizePixmap()
{
    return (const char **)default_maximize_xpm;
}

const char **QWSDefaultDecoration::normalizePixmap()
{
    return (const char **)default_normalize_xpm;
}
#endif


const QPixmap* QWSDefaultDecoration::pixmapFor(const QWidget* w, QWSDecoration::Region type, bool on, int& xoff, int& /*yoff*/)
{
#ifndef QT_NO_IMAGEIO_XPM

    static const char** staticMenuPixmapXPM=0;
    static const char** staticClosePixmapXPM=0;
    static const char** staticMinimizePixmapXPM=0;
    static const char** staticMaximizePixmapXPM=0;
    static const char** staticNormalizePixmapXPM=0;

    const char** xpm;

    // Why don't we just use/extend the enum type...

    if ( staticMenuPixmapXPM != (xpm=menuPixmap()) ) {
	staticMenuPixmapXPM = xpm;
	staticMenuPixmap = new QPixmap(xpm);
    }
    if ( staticClosePixmapXPM != (xpm=closePixmap()) ) {
	staticClosePixmapXPM = xpm;
	staticClosePixmap = new QPixmap(xpm);
    }
    if ( staticMinimizePixmapXPM != (xpm=minimizePixmap()) ) {
	staticMinimizePixmapXPM = xpm;
	staticMinimizePixmap = new QPixmap(xpm);
    }
    if ( staticMaximizePixmapXPM != (xpm=maximizePixmap()) ) {
	staticMaximizePixmapXPM = xpm;
	staticMaximizePixmap = new QPixmap(xpm);
    }
    if ( staticNormalizePixmapXPM != (xpm=normalizePixmap()) ) {
	staticNormalizePixmapXPM = xpm;
	staticNormalizePixmap = new QPixmap(xpm);
    }

    const QPixmap *pm = 0;

    switch (type) {
	case Menu:
#ifndef QT_NO_WIDGET_TOPEXTRA
	    pm = w->icon();
#endif
	    if ( !pm ) {
		xoff = 1;
		pm = staticMenuPixmap;
	    }
	    break;
	case Close:
	    pm = staticClosePixmap;
	    break;
	case Maximize:
	    if (on)
		pm = staticNormalizePixmap;
	    else
		pm = staticMaximizePixmap;
	    break;
	case Minimize:
	    pm = staticMinimizePixmap;
	    break;
	default:
	    break;
    }
    return pm;
#else
    return 0;
#endif
}

int QWSDefaultDecoration::getTitleWidth(const QWidget *widget)
{
    return widget->width() - 4 * getTitleHeight(widget) - 4;
}

int QWSDefaultDecoration::getTitleHeight(const QWidget *)
{
    return 20;
}

QRegion QWSDefaultDecoration::region(const QWidget *widget, const QRect &rect, QWSDecoration::Region type)
{
//    int titleWidth = getTitleWidth(widget);
    int titleHeight = getTitleHeight(widget);

    QRegion region;

    int bw = BORDER_WIDTH;

    switch (type) {
	case All: {
		if ( widget->isMaximized() ) {
		    QRect r(rect.left(), rect.top() - titleHeight,
			    rect.width(), rect.height() + titleHeight);
		    region = r;
		} else {
		    QRect r(rect.left() - bw,
			    rect.top() - titleHeight - bw,
			    rect.width() + 2 * bw,
			    rect.height() + titleHeight + 2 * bw);
		    region = r;
		}
		region -= rect;
	    }
	    break;

	case Title: {
		    QRect r(rect.left() + titleHeight, rect.top() - titleHeight,
			    rect.width() - 4*titleHeight, titleHeight);
		    if (r.width() > 0)
			region = r;
	    }
	    break;

	case Top: {
		    QRect r(rect.left() + CORNER_GRAB,
			    rect.top() - titleHeight - bw,
			    rect.width() - 2 * CORNER_GRAB,
			    bw);
		    region = r;
	    }
	    break;

	case Left: {
		QRect r(rect.left() - bw,
			rect.top() - titleHeight + CORNER_GRAB,
			bw,
			rect.height() + titleHeight - 2 * CORNER_GRAB);
		region = r;
	    }
	    break;

	case Right: {
		QRect r(rect.right() + 1,
			rect.top() - titleHeight + CORNER_GRAB,
			bw,
			rect.height() + titleHeight - 2 * CORNER_GRAB);
		region = r;
	    }
	    break;

	case Bottom: {
		QRect r(rect.left() + CORNER_GRAB,
			rect.bottom() + 1,
			rect.width() - 2 * CORNER_GRAB,
			bw);
		region = r;
	    }
	    break;

	case TopLeft: {
		QRect r1(rect.left() - bw,
			rect.top() - bw - titleHeight,
			CORNER_GRAB + bw,
			bw);

		QRect r2(rect.left() - bw,
			rect.top() - bw - titleHeight,
			bw,
			CORNER_GRAB + bw);

		region = QRegion(r1) + r2;
	    }
	    break;

	case TopRight: {
		QRect r1(rect.right() - CORNER_GRAB,
			rect.top() - bw - titleHeight,
			CORNER_GRAB + bw,
			bw);

		QRect r2(rect.right() + 1,
			rect.top() - bw - titleHeight,
			bw,
			CORNER_GRAB + bw);

		region = QRegion(r1) + r2;
	    }
	    break;

	case BottomLeft: {
		QRect r1(rect.left() - bw,
			rect.bottom() + 1,
			CORNER_GRAB + bw,
			bw);

		QRect r2(rect.left() - bw,
			rect.bottom() - CORNER_GRAB,
			bw,
			CORNER_GRAB + bw);
		region = QRegion(r1) + r2;
	    }
	    break;

	case BottomRight: {
		QRect r1(rect.right() - CORNER_GRAB,
			rect.bottom() + 1,
			CORNER_GRAB + bw,
			bw);

		QRect r2(rect.right() + 1,
			rect.bottom() - CORNER_GRAB,
			bw,
			CORNER_GRAB + bw);
		region = QRegion(r1) + r2;
	    }
	    break;

	case Menu: {
		    QRect r(rect.left(), rect.top() - titleHeight,
			    titleHeight, titleHeight);
		    region = r;
	    }
	    break;

	case Close: {
		QRect r(rect.right() - titleHeight, rect.top() - titleHeight,
			titleHeight, titleHeight);
		if (r.left() > rect.left() + titleHeight)
		    region = r;
	    }
	    break;

	case Maximize: {
		QRect r(rect.right() - 2*titleHeight, rect.top() - titleHeight,
			titleHeight, titleHeight);
		if (r.left() > rect.left() + titleHeight)
		    region = r;
	    }
	    break;

	case Minimize: {
		QRect r(rect.right() - 3*titleHeight, rect.top() - titleHeight,
			titleHeight, titleHeight);
		if (r.left() > rect.left() + titleHeight)
		    region = r;
	    }
	    break;

	default:
	    break;
    }

    return region;
}

void QWSDefaultDecoration::paint(QPainter *painter, const QWidget *widget)
{
    int titleWidth = getTitleWidth(widget);
    int titleHeight = getTitleHeight(widget);

    QRect rect(widget->rect());

    // title bar rect
    QRect tr(titleHeight, -titleHeight,  titleWidth, titleHeight - 1);

    QRegion oldClip = painter->clipRegion();
    painter->setClipRegion( oldClip - QRegion( tr ) );	// reduce flicker

#ifndef QT_NO_PALETTE
    const QColorGroup &cg = QApplication::palette().active();
//    const QColorGroup &cg = widget->palette().active();

#if !defined(QT_NO_DRAWUTIL)
    // Border rect
    QRect br( rect.left() - BORDER_WIDTH,
                rect.top() - BORDER_WIDTH - titleHeight,
                rect.width() + 2 * BORDER_WIDTH,
                rect.height() + BORDER_WIDTH + BOTTOM_BORDER_WIDTH + titleHeight );

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

	{

#if !defined(QT_NO_DRAWUTIL)
	    qDrawShadePanel(painter, tr.x(), tr.y(), tr.width(), tr.height(),
			    cg, TRUE, 1, &titleBrush);
#endif

#ifndef QT_NO_WIDGET_TOPEXTRA
	    painter->setPen(titlePen);
	    painter->setFont(widget->font());
	    painter->drawText( titleLeft, -titleHeight,
			    titleWidth-5, titleHeight - 1,
			    QPainter::AlignVCenter, widget->caption());
#endif
	    return;
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

void QWSDefaultDecoration::paintButton(QPainter *painter, const QWidget *w,
			QWSDecoration::Region type, int state)
{
#ifndef QT_NO_PALETTE
    const QColorGroup &cg = QApplication::palette().active();
//    const QColorGroup &cg = w->palette().active();

    QRect brect(region(w, w->rect(), type).boundingRect());

    int xoff=2;
    int yoff=2;

    const QPixmap *pm=pixmapFor(w,type,state & QWSButton::On, xoff, yoff);

    {

	if ((state & QWSButton::MouseOver) && (state & QWSButton::Clicked)) {
#if !defined(QT_NO_DRAWUTIL)
	    qDrawWinPanel(painter, brect.x(), brect.y(), brect.width()-1,
			brect.height()-1, cg, TRUE,
			&cg.brush(QColorGroup::Background));
#endif	
	    if (pm) painter->drawPixmap(brect.x()+xoff+1, brect.y()+yoff+1, *pm);
	} else {
	    painter->fillRect(brect.x(), brect.y(), brect.width()-1,
			brect.height()-1, cg.brush(QColorGroup::Background));
	    if (pm) painter->drawPixmap(brect.x()+xoff, brect.y()+yoff, *pm);
	}
    }

#endif

}

#endif // QT_NO_QWS_MANAGER
