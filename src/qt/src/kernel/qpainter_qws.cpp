/****************************************************************************
**
** Implementation of QPainter class for Qt/Embedded
**
** Created : 991026
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
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

#include "qpainter.h"
#include "qwidget.h"
#include "qbitmap.h"
#include "qpixmapcache.h"
#include "qobjectlist.h"
#include "qfontdata_p.h"
#include "qtextcodec.h"
#include "qmemorymanager_qws.h"
#include "qwsdisplay_qws.h"
#include "qpaintdevicemetrics.h"
#include "qgfx_qws.h"
#include <string.h>
#include "qtextlayout_p.h"
#include "qtextengine_p.h"
#include "qfontengine_p.h"

/* paintevent magic to provide Windows semantics on Qt/E
 */
static QRegion* paintEventClipRegion = 0;
static QRegion* paintEventSaveRegion = 0;
static QPaintDevice* paintEventDevice = 0;

void qt_set_paintevent_clipping( QPaintDevice* dev, const QRegion& region)
{
    if ( !paintEventClipRegion )
	paintEventClipRegion = new QRegion( region );
    else
	*paintEventClipRegion = region;
    paintEventDevice = dev;

#ifdef QWS_EXTRA_DEBUG
    qDebug( "qt_set_paintevent_clipping" );
    QMemArray<QRect> ar = region.rects();
    for ( int i=0; i<int(ar.size()); i++ ) {
	QRect r = ar[i];
        qDebug( "   r[%d]:  %d,%d %dx%d", i,
		r.x(), r.y(), r.width(), r.height() );
    }
#endif

}

void qt_clear_paintevent_clipping()
{
    delete paintEventClipRegion;
    delete paintEventSaveRegion;
    paintEventClipRegion = 0;
    paintEventSaveRegion = 0;
    paintEventDevice = 0;
}

class QPainterPrivate
{
public:
    QPoint curPt;
};

static QList<QPainter> *widgetPainterList = 0;

void qwsUpdateActivePainters()
{
    if ( widgetPainterList ) {
	QPainter *ptr = widgetPainterList->first();
	while ( ptr ) {
	    ptr->save();
	    delete ptr->gfx;
	    ptr->gfx = ptr->device()->graphicsContext();
	    ptr->setf( QPainter::VolatileDC );
	    ptr->restore();
	    ptr = widgetPainterList->next();
	}
    }
}


/*****************************************************************************
  Trigonometric function for QPainter

  We have implemented simple sine and cosine function that are called from
  QPainter::drawPie() and QPainter::drawChord() when drawing the outline of
  pies and chords.
  These functions are slower and less accurate than math.h sin() and cos(),
  but with still around 1/70000th sec. execution time (on a 486DX2-66) and
  8 digits accuracy, it should not be the bottleneck in drawing these shapes.
  The advantage is that you don't have to link in the math library.
 *****************************************************************************/

const double Q_PI   = 3.14159265358979323846;	// pi
const double Q_2PI  = 6.28318530717958647693;	// 2*pi
const double Q_PI2  = 1.57079632679489661923;	// pi/2


#if 0

#if defined(Q_CC_GNU) && defined(__i386__)

inline double qcos( double a )
{
    double r;
    __asm__ (
	"fcos"
	: "=t" (r) : "0" (a) );
    return(r);
}

inline double qsin( double a )
{
    double r;
    __asm__ (
	"fsin"
	: "=t" (r) : "0" (a) );
    return(r);
}

double qsincos( double a, bool calcCos=FALSE )
{
    return calcCos ? qcos(a) : qsin(a);
}

#else

double qsincos( double a, bool calcCos=FALSE )
{
    if ( calcCos )				// calculate cosine
	a -= Q_PI2;
    if ( a >= Q_2PI || a <= -Q_2PI ) {		// fix range: -2*pi < a < 2*pi
	int m = (int)(a/Q_2PI);
	a -= Q_2PI*m;
    }
    if ( a < 0.0 )				// 0 <= a < 2*pi
	a += Q_2PI;
    int sign = a > Q_PI ? -1 : 1;
    if ( a >= Q_PI )
	a = Q_2PI - a;
    if ( a >= Q_PI2 )
	a = Q_PI - a;
    if ( calcCos )
	sign = -sign;
    double a2  = a*a;				// here: 0 <= a < pi/4
    double a3  = a2*a;				// make taylor sin sum
    double a5  = a3*a2;
    double a7  = a5*a2;
    double a9  = a7*a2;
    double a11 = a9*a2;
    return (a-a3/6+a5/120-a7/5040+a9/362880-a11/39916800)*sign;
}

inline double qsin( double a ) { return qsincos(a,FALSE); }
inline double qcos( double a ) { return qsincos(a,TRUE); }

#endif

#endif // 0


/*****************************************************************************
  QPainter member functions
 *****************************************************************************/

void QPainter::initialize()
{
}

void QPainter::cleanup()
{
    QPointArray::cleanBuffers();
}

void QPainter::destroy()
{
    delete d;
}

void QPainter::init()
{
    if ( !widgetPainterList )
	widgetPainterList = new QList<QPainter>;
    d = new QPainterPrivate;
    flags = IsStartingUp;
    bg_col = white;				// default background color
    bg_mode = TransparentMode;			// default background mode
    rop = CopyROP;				// default ROP
    tabstops = 0;				// default tabbing
    tabarray = 0;
    tabarraylen = 0;
    ps_stack = 0;
    wm_stack = 0;
    pdev = 0;
    penRef = brushRef = 0;
#ifndef QT_NO_TRANSFORMATIONS
    txop = txinv = 0;
#else
    xlatex = xlatey = 0;
#endif
    pfont = 0;
    block_ext = FALSE;
}


void QPainter::setFont( const QFont &font )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	qWarning( "QPainter::setFont: Will be reset by begin()" );
#endif

    if ( cfont.d != font.d ) {
	cfont = font;
	setf(DirtyFont);
    }
    updateFont();
}


void QPainter::updateFont()
{
    clearf(DirtyFont);
    if(testf(ExtDev)) {
        if (pdev->devType() == QInternal::Printer) {
            if ( pfont ) delete pfont;
            pfont = new QFont( cfont.d, pdev );
        }
	QPDevCmdParam param[1];
	param[0].font = &cfont;
	if ( !pdev->cmd(QPaintDevice::PdcSetFont, this, param) )
	    return;
    }
}


void QPainter::updatePen()
{
    if(testf(ExtDev)) {
	QPDevCmdParam param[1];
	param[0].pen = &cpen;
	if ( !pdev->cmd(QPaintDevice::PdcSetPen, this, param) )
	    return;
    }
    if ( gfx )
	gfx->setPen(cpen);
}


void QPainter::updateBrush()
{
static uchar dense1_pat[] = { 0xff, 0xbb, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff };
static uchar dense2_pat[] = { 0x77, 0xff, 0xdd, 0xff, 0x77, 0xff, 0xdd, 0xff };
static uchar dense3_pat[] = { 0x55, 0xbb, 0x55, 0xee, 0x55, 0xbb, 0x55, 0xee };
static uchar dense4_pat[] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };
static uchar dense5_pat[] = { 0xaa, 0x44, 0xaa, 0x11, 0xaa, 0x44, 0xaa, 0x11 };
static uchar dense6_pat[] = { 0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00 };
static uchar dense7_pat[] = { 0x00, 0x44, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00 };
static uchar hor_pat[] = {			// horizontal pattern
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uchar ver_pat[] = {			// vertical pattern
    0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
    0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
    0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
    0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
    0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
    0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20 };
static uchar cross_pat[] = {			// cross pattern
    0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0xff, 0xff, 0xff,
    0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
    0x08, 0x82, 0x20, 0xff, 0xff, 0xff, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
    0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0xff, 0xff, 0xff,
    0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20,
    0x08, 0x82, 0x20, 0xff, 0xff, 0xff, 0x08, 0x82, 0x20, 0x08, 0x82, 0x20 };
static uchar bdiag_pat[] = {			// backward diagonal pattern
    0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x01, 0x01,
    0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04,
    0x02, 0x02, 0x01, 0x01, 0x80, 0x80, 0x40, 0x40 };
static uchar fdiag_pat[] = {			// forward diagonal pattern
    0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x40, 0x40,
    0x80, 0x80, 0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10,
    0x20, 0x20, 0x40, 0x40, 0x80, 0x80, 0x01, 0x01 };
static uchar dcross_pat[] = {			// diagonal cross pattern
    0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x14, 0x14, 0x22, 0x22, 0x41, 0x41,
    0x80, 0x80, 0x41, 0x41, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x14, 0x14,
    0x22, 0x22, 0x41, 0x41, 0x80, 0x80, 0x41, 0x41 };
static uchar *pat_tbl[] = {
    dense1_pat, dense2_pat, dense3_pat, dense4_pat, dense5_pat,
    dense6_pat, dense7_pat,
    hor_pat, ver_pat, cross_pat, bdiag_pat, fdiag_pat, dcross_pat };

    if ( testf(ExtDev) ) {
	QPDevCmdParam param[1];
	param[0].brush = &cbrush;
        if ( !pdev->cmd(QPaintDevice::PdcSetBrush, this, param) )
	    return;
    }
    if ( !gfx )
	return;

    uchar * pat=0;
    int bs=cbrush.style();
    int d=0;
    if( bs>=Dense1Pattern && bs <= DiagCrossPattern ) {
	pat=pat_tbl[ bs-Dense1Pattern ];
	if(bs<=Dense7Pattern)
	    d=8;
	else if (bs<=CrossPattern)
	    d=24;
	else
	    d=16;
    }
    if ( bs == CustomPattern || pat ) {
        QPixmap *pm;
        if ( pat ) {
            QString key="$qt-brush$" + QString::number( bs );
            pm = QPixmapCache::find( key );
            bool del = FALSE;
            if ( !pm ) {                        // not already in pm dict
		pm = new QBitmap( d, d, pat, TRUE );
		Q_CHECK_PTR( pm );
		del = !QPixmapCache::insert( key, pm );
            }
	    if ( cbrush.data->pixmap )
		delete cbrush.data->pixmap;
	    cbrush.data->pixmap = new QPixmap( *pm );
	    if (del) delete pm;
	}
	pm = cbrush.data->pixmap;
    }

    gfx->setBrush( cbrush );
    gfx->setBrushPixmap( cbrush.data->pixmap );
}


bool QPainter::begin( const QPaintDevice *pd, bool unclipped )
{
    if ( isActive() ) {				// already active painting
#if defined(QT_CHECK_STATE)
	qWarning( "QPainter::begin: Painter is already active."
		 "\n\tYou must end() the painter before a second begin()" );
#endif
	return FALSE;
    }
    if ( pd == 0 ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QPainter::begin: Paint device cannot be null" );
#endif
	return FALSE;
    }

    const QWidget *copyFrom = 0;
    pdev = redirect( (QPaintDevice*)pd );
    if ( pdev ) {				    // redirected paint device?
	if ( pd->devType() == QInternal::Widget )
	    copyFrom = (const QWidget *)pd;	    // copy widget settings
    } else {
	pdev = (QPaintDevice*)pd;
    }

    if ( pdev->isExtDev() && pdev->paintingActive() ) {
		// somebody else is already painting
#if defined(QT_CHECK_STATE)
	qWarning( "QPainter::begin: Another QPainter is already painting "
		 "this device;\n\tAn extended paint device can only be painted "
	         "by one QPainter at a time." );
#endif
	return FALSE;
    }

    bool reinit = flags != IsStartingUp;	// 2nd or 3rd etc. time called
    flags = IsActive | DirtyFont;		// init flags
    int dt = pdev->devType();			// get the device type

    if ( (pdev->devFlags & QInternal::ExternalDevice) != 0 ) // external device
	setf(ExtDev);
    else if ( dt == QInternal::Pixmap )		// device is a pixmap
	((QPixmap*)pdev)->detach();		// will modify it

    gfx = ((QPaintDevice *)pdev)->graphicsContext();
//  if gfx == 0 we might have an invalid pixmap
    if ( gfx == 0 && !testf(ExtDev) ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QPainter::begin: Unable to get graphics context" );
#endif
	return FALSE;
    }

    if ( testf(ExtDev) ) {			// external device
	if ( !pdev->cmd( QPaintDevice::PdcBegin, this, 0 ) ) {
	    // could not begin painting
	    pdev = 0;
	    return FALSE;
	}
	if ( tabstops )				// update tabstops for device
	    setTabStops( tabstops );
	if ( tabarray )				// update tabarray for device
	    setTabArray( tabarray );
    }

    pdev->painters++;				// also tell paint device
    bro = QPoint( 0, 0 );
    d->curPt = QPoint( 0, 0 );
    if ( reinit ) {
	bg_mode = TransparentMode;		// default background mode
	rop = CopyROP;				// default ROP
#ifndef QT_NO_TRANSFORMATIONS
	wxmat.reset();				// reset world xform matrix
	xmat.reset();
	ixmat.reset();
	txop = txinv = 0;
#else
	xlatex = xlatey = 0;
#endif
	if ( dt != QInternal::Widget ) {
	    QFont  defaultFont;			// default drawing tools
	    QPen   defaultPen;
	    QBrush defaultBrush;
	    cfont  = defaultFont;		// set these drawing tools
	    cpen   = defaultPen;
	    cbrush = defaultBrush;
	    bg_col = white;			// default background color
	}
    }
#ifndef QT_NO_TRANSFORMATIONS
    wx = wy = vx = vy = 0;			// default view origins
#endif

    if ( dt == QInternal::Widget ) {			// device is a widget
	QWidget *w = (QWidget*)pdev;
	cfont = w->font();			// use widget font
	cpen = QPen( w->foregroundColor() );	// use widget fg color
	if ( reinit ) {
	    QBrush defaultBrush;
	    cbrush = defaultBrush;
	}
	bg_col = w->backgroundColor();		// use widget bg color
#ifndef QT_NO_TRANSFORMATIONS
	ww = vw = w->width();			// default view size
	wh = vh = w->height();
#endif
	if ( unclipped || w->testWFlags(WPaintUnclipped) ) { // paint direct on device
	    setf( NoCache );
	    updatePen();
	    updateBrush();
	    if ( !testf(ExtDev) )
		gfx->setWidgetRegion( QRect(w->mapToGlobal(QPoint(0,0)), w->size()) );
	}
	widgetPainterList->prepend( this );
    } else if ( dt == QInternal::Pixmap ) {		// device is a pixmap
	QPixmap *pm = (QPixmap*)pdev;
	if ( pm->isNull() ) {
#if defined(QT_CHECK_NULL)
	    qWarning( "QPainter::begin: Cannot paint null pixmap" );
#endif
	    end();
	    return FALSE;
	}
	bool mono = pm->depth() == 1;		// monochrome bitmap
	if ( mono ) {
	    setf( MonoDev );
	    bg_col = color0;
	    cpen.setColor( color1 );
	}
#ifndef QT_NO_TRANSFORMATIONS
	ww = vw = pm->width();			// default view size
	wh = vh = pm->height();
#endif
    } else if ( testf(ExtDev) ) {		// external device
#ifndef QT_NO_TRANSFORMATIONS
	ww = vw = pdev->metric( QPaintDeviceMetrics::PdmWidth );
	wh = vh = pdev->metric( QPaintDeviceMetrics::PdmHeight );
#endif
    }
#ifndef QT_NO_TRANSFORMATIONS
    if ( ww == 0 )
	ww = wh = vw = vh = 1024;
#endif
    if ( copyFrom ) {				// copy redirected widget
	cfont = copyFrom->font();
	cpen = QPen( copyFrom->foregroundColor() );
	bg_col = copyFrom->backgroundColor();
    }
    setBackgroundColor( bg_col );		// default background color
    if ( testf(ExtDev) ) {			// external device
	setBackgroundMode( TransparentMode );	// default background mode
	setRasterOp( CopyROP );			// default raster operation
    }
    updateBrush();
    updatePen();

    if ( !testf(ExtDev) ) {
	if ( paintEventDevice == device() )
	    gfx->setClipRegion( *paintEventClipRegion );
	else
	    gfx->setClipping( FALSE );
    }

    return TRUE;
}

bool QPainter::end()				// end painting
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QPainter::end: Missing begin() or begin() failed" );
#endif
	return FALSE;
    }
    killPStack();

    if ( testf(ExtDev) )
	pdev->cmd( QPaintDevice::PdcEnd, this, 0 );

    if ( paintEventSaveRegion )
	*paintEventSaveRegion = QRegion();

    if ( pdev && pdev->devType() == QInternal::Widget )
	widgetPainterList->removeRef( this );

    delete gfx;
    gfx = 0;

    if ( pfont ) {
	delete pfont;
	pfont = 0;
    }
    if( testf(ExtDev) )
	QFontCache::instance->cleanupPrinterFonts();

    flags = 0;
    if ( pdev ) {
        pdev->painters--;
        pdev = 0;
    }
    return TRUE;
}

#ifdef QT_QWS_EXPERIMENTAL_SCREENPAINTER
bool QPainter::begin(QScreen *screen)
{
    if ( isActive() ) {				// already active painting
#if defined(QT_CHECK_STATE)
	qWarning( "QPainter::begin: Painter is already active."
                  "\n\tYou must end() the painter before a second begin()" );
#endif
	return FALSE;
    }
    bool reinit = flags != IsStartingUp;	// 2nd or 3rd etc. time called
    flags = IsActive | DirtyFont;		// init flags
    gfx =  screen->screenGfx();
    bro = QPoint( 0, 0 );
    d->curPt = QPoint( 0, 0 );
    if ( reinit ) {
	bg_mode = TransparentMode;		// default background mode
	rop = CopyROP;				// default ROP
#ifndef QT_NO_TRANSFORMATIONS
	wxmat.reset();				// reset world xform matrix
	xmat.reset();
	ixmat.reset();
	txop = txinv = 0;
#else
	xlatex = xlatey = 0;
#endif
	QFont  defaultFont;			// default drawing tools
	QPen   defaultPen;
	QBrush defaultBrush;
	cfont  = defaultFont;		// set these drawing tools
	cpen   = defaultPen;
	cbrush = defaultBrush;
	bg_col = white;			// default background color
    }
#ifndef QT_NO_TRANSFORMATIONS
    wx = wy = vx = vy = 0;			// default view origins
#endif
    ww = vw = screen->width();			// default view size
    wh = vh = screen->height();
    gfx->setWidgetRegion( QRect(0,0,ww,wh) );
    setBackgroundColor( bg_col );
    updateBrush();
    updatePen();
    return TRUE;
}
#endif //QT_QWS_EXPERIMENTAL_SCREENPAINTER

void QPainter::flush(const QRegion &, CoordinateMode)
{
    flush();
}


void QPainter::flush()
{
}


void QPainter::setBackgroundColor( const QColor &c )
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QPainter::setBackgroundColor: Call begin() first" );
#endif
	return;
    }
    bg_col = c;
    if ( testf(ExtDev) ) {
	QPDevCmdParam param[1];
	param[0].color = &bg_col;
	if ( !pdev->cmd(QPaintDevice::PdcSetBkColor,this,param) || !gfx )
	    return;
    }
    gfx->setBackgroundColor( bg_col );
}


void QPainter::setBackgroundMode( BGMode m )
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QPainter::setBackgroundMode: Call begin() first" );
#endif
	return;
    }
    if ( m != TransparentMode && m != OpaqueMode ) {
#if defined(QT_CHECK_RANGE)
	qWarning( "QPainter::setBackgroundMode: Invalid mode" );
#endif
	return;
    }
    bg_mode = m;
    if ( testf(ExtDev) ) {
	QPDevCmdParam param[1];
	param[0].ival = m;
	if ( !pdev->cmd(QPaintDevice::PdcSetBkMode,this,param) || !gfx )
	    return;
    }
    gfx->setOpaqueBackground( bg_mode==OpaqueMode );
}

void QPainter::setRasterOp( RasterOp r )
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QPainter::setRasterOp: Call begin() first" );
#endif
	return;
    }
    if ( (uint)r > LastROP ) {
#if defined(QT_CHECK_RANGE)
	qWarning( "QPainter::setRasterOp: Invalid ROP code" );
#endif
	return;
    }
    rop = r;
    if ( testf(ExtDev) ) {
	QPDevCmdParam param[1];
	param[0].ival = r;
	if ( !pdev->cmd(QPaintDevice::PdcSetROP,this,param) || !gfx )
	    return;
    }
    gfx->setRop( r );
}


void QPainter::setBrushOrigin( int x, int y )
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QPainter::setBrushOrigin: Call begin() first" );
#endif
	return;
    }
    bro = QPoint(x,y);
    if ( testf(ExtDev) ) {
	QPDevCmdParam param[1];
	param[0].point = &bro;
	if ( !pdev->cmd(QPaintDevice::PdcSetBrushOrigin,this,param) || !gfx )
	    return;
    }
}


void QPainter::setClipping( bool enable )
{
    if ( !isActive() ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QPainter::setClipping: Will be reset by begin()" );
#endif
	return;
    }

    if ( enable == testf(ClipOn)
	 && ( paintEventDevice != device() || !enable
	      || !paintEventSaveRegion || paintEventSaveRegion->isNull() ) )
	 return;

    if ( paintEventDevice == device() ) {
	if ( !enable ) {
	    enable = TRUE;
	    if ( !paintEventSaveRegion )
		paintEventSaveRegion = new QRegion( crgn );
	    else
		*paintEventSaveRegion = crgn;
	    crgn = *paintEventClipRegion;
	} else {
	    if ( paintEventSaveRegion && !paintEventSaveRegion->isNull() )
		crgn = *paintEventSaveRegion;
	}
    }

    setf( ClipOn, enable );

    if ( testf(ExtDev) ) {
	QPDevCmdParam param[1];
	param[0].ival = enable;
	if ( !pdev->cmd(QPaintDevice::PdcSetClip, this, param) || !gfx )
	    return;
    }
    if ( enable ) {
	gfx->setClipRegion( crgn );
    } else {
	gfx->setClipping( FALSE );
    }
}


void QPainter::setClipRect( const QRect &r, CoordinateMode m )
{
    QRegion rgn( r );
    setClipRegion( rgn, m );
}

void QPainter::setClipRegion( const QRegion &rgn, CoordinateMode m )
{
#if defined(QT_CHECK_STATE)
    if ( !isActive() )
	qWarning( "QPainter::setClipRegion: Will be reset by begin()" );
#endif
#ifndef QT_NO_TRANSFORMATIONS
    if ( m == CoordDevice )
	crgn = rgn;
    else
	crgn = xmat * rgn;
#else
    crgn = rgn;
    if ( m == CoordPainter )
	crgn.translate( xlatex, xlatey );
#endif

    if ( paintEventDevice == device() ) {
	crgn = crgn.intersect( *paintEventClipRegion );
	if ( paintEventSaveRegion )
	    *paintEventSaveRegion = QRegion();
    }
    if ( testf(ExtDev) ) {
	QPDevCmdParam param[1];
	param[0].rgn = &crgn;
	if ( !pdev->cmd(QPaintDevice::PdcSetClipRegion, this, param) || !gfx )
	    return;
    }
    clearf( ClipOn );
    setClipping( TRUE );
}


void QPainter::drawPolyInternal( const QPointArray &a, bool close )
{
    if ( a.size() < 2 || !gfx )
	return;

    int x1, y1, x2, y2;				// connect last to first point
    a.point( a.size()-1, &x1, &y1 );
    a.point( 0, &x2, &y2 );
    bool do_close = close && !(x1 == x2 && y1 == y2);

    if ( close && cbrush.style() != NoBrush ) {	// draw filled polygon
	gfx->drawPolygon(a,FALSE,0,a.size());
	if ( cpen.style() == NoPen ) {		// draw fake outline
	    gfx->drawPolyline(a,0,a.size());
	    if ( do_close )
		gfx->drawLine(x1,y1,x2,y2);
	}
    }
    if ( cpen.style() != NoPen ) {		// draw outline
	gfx->drawPolyline(a,0,a.size());
	if ( do_close )
	    gfx->drawLine(x1,y1,x2,y2);
    }
}


void QPainter::drawPoint( int x, int y )
{
    if ( !isActive() )
	return;
    if ( cpen.style() != NoPen ) {
	if ( testf(ExtDev|VxF|WxF) ) {
	    QPDevCmdParam param[1];
	    if ( testf(ExtDev) ) {
		QPoint p( x, y );
		param[0].point = &p;
		if ( !pdev->cmd(QPaintDevice::PdcDrawPoint,this,param) || !gfx)
		    return;
	    }
	    map( x, y, &x, &y );
	}
	gfx->drawPoint(x,y);
    }
}


void QPainter::drawPoints( const QPointArray& a, int index, int npoints )
{
    if ( npoints < 0 )
	npoints = a.size() - index;
    if ( index + npoints > (int)a.size() )
	npoints = a.size() - index;
    if ( !isActive() || npoints < 1 || index < 0 || cpen.style() == NoPen )
	return;
    QPointArray pa = a;
    if ( testf(ExtDev|VxF|WxF) ) {
	if ( testf(ExtDev) ) {
	    QPDevCmdParam param[1];
	    for (int i=0; i<npoints; i++) {
		QPoint p( pa[index+i].x(), pa[index+i].y() );
		param[0].point = &p;
		if ( !pdev->cmd(QPaintDevice::PdcDrawPoint, this, param) )
		    return;
	    }
	    if ( !gfx )
		return;
	}
#ifndef QT_NO_TRANSFORMATIONS
	bool tx = (txop != TxNone);
#else
	bool tx = xlatex || xlatey;
#endif
	if ( tx ) {
	    pa = xForm( a, index, npoints );
	    if ( pa.size() != a.size() ) {
		index = 0;
		npoints = pa.size();
	    }
	}
    }
    gfx->drawPoints( pa, index, npoints );
}


void QPainter::moveTo( int x, int y )
{
    if ( !isActive() )
	return;
    if ( testf(ExtDev|VxF|WxF) ) {
	if ( testf(ExtDev) ) {
	    QPDevCmdParam param[1];
	    QPoint p( x, y );
	    param[0].point = &p;
	    if ( !pdev->cmd(QPaintDevice::PdcMoveTo, this, param) || !gfx )
		return;
	}
	map( x, y, &x, &y );
    }
    gfx->moveTo( x, y );
    d->curPt = QPoint( x, y );
}


void QPainter::lineTo( int x, int y )
{
    if ( !isActive() )
	return;
    if ( testf(ExtDev|VxF|WxF) ) {
	if ( testf(ExtDev) ) {
	    QPDevCmdParam param[1];
	    QPoint p( x, y );
	    param[0].point = &p;
	    if ( !pdev->cmd(QPaintDevice::PdcLineTo, this, param) || !gfx )
		return;
	}
	map( x, y, &x, &y );
    }
    gfx->lineTo( x, y );
    d->curPt = QPoint( x, y );
}


void QPainter::drawLine( int x1, int y1, int x2, int y2 )
{
    if ( !isActive() )
	return;
    if ( testf(ExtDev|VxF|WxF) ) {
	if ( testf(ExtDev) ) {
	    QPDevCmdParam param[2];
	    QPoint p1(x1, y1), p2(x2, y2);
	    param[0].point = &p1;
	    param[1].point = &p2;
	    if ( !pdev->cmd(QPaintDevice::PdcDrawLine, this, param) || !gfx )
		return;
	}
	map( x1, y1, &x1, &y1 );
	map( x2, y2, &x2, &y2 );
    }
    if ( cpen.style() != NoPen )
	gfx->drawLine( x1, y1, x2, y2 );
    gfx->moveTo( x2, y2 );
    d->curPt = QPoint( x2, y2 );
}


void QPainter::drawRect( int x, int y, int w, int h )
{
    if ( !isActive() )
	return;
    if ( testf(ExtDev|VxF|WxF) ) {
	if ( testf(ExtDev) ) {
	    QPDevCmdParam param[1];
	    QRect r( x, y, w, h );
	    param[0].rect = &r;
	    if ( !pdev->cmd( QPaintDevice::PdcDrawRect, this, param ) || !gfx )
		return;
	}
#ifndef QT_NO_TRANSFORMATIONS
	if ( txop == TxRotShear ) {		// rotate/shear polygon
	    QPointArray a( QRect(x,y,w,h), TRUE );
	    drawPolyInternal( xForm(a) );
	    return;
	}
#endif
	map( x, y, w, h, &x, &y, &w, &h );
    }
    if ( w <= 0 || h <= 0 )
	fix_neg_rect( &x, &y, &w, &h );

    gfx->setBrushOffset( x-bro.x(), y-bro.y() );

    if ( cpen.style() != NoPen ) {
	if ( cpen.width() > 1 ) {
	    QPointArray a( QRect(x,y,w,h), TRUE );
	    drawPolyInternal( a );
	    return;
	} else {
	    int x1 = x;
	    int y1 = y;
	    int x2 = x + (w-1);
	    int y2 = y + (h-1);
	    gfx->drawLine(x1, y1, x2, y1);
	    gfx->drawLine(x2, y1, x2, y2);
	    gfx->drawLine(x1, y2, x2, y2);
	    gfx->drawLine(x1, y1, x1, y2);
	    x += 1;
	    y += 1;
	    w -= 2;
	    h -= 2;
	}
    }

    gfx->fillRect( x, y, w, h );
}


void QPainter::drawWinFocusRect( int x, int y, int w, int h )
{
    drawWinFocusRect( x, y, w, h, TRUE, color0 );
}

void QPainter::drawWinFocusRect( int x, int y, int w, int h,
				 const QColor &bgColor )
{
    drawWinFocusRect( x, y, w, h, FALSE, bgColor );
}

void QPainter::drawWinFocusRect( int x, int y, int w, int h,
                                 bool xorPaint, const QColor &bgColor )
{
    if ( !isActive() )
        return;
#ifndef QT_NO_TRANSFORMATIONS
    if ( txop == TxRotShear )
        return;
#endif

    if ( testf(ExtDev|VxF|WxF) ) {
        if ( testf(ExtDev) ) {
            QPDevCmdParam param[1];
            QRect r( x, y, w, h );
            param[0].rect = &r;
            if ( !pdev->cmd(QPaintDevice::PdcDrawRect, this, param) || !gfx )
		return;
	}
	map( x, y, w, h, &x, &y, &w, &h );
    }

    if ( w <= 0 || h <= 0 ) {
        if ( w == 0 || h == 0 )
            return;
        fix_neg_rect( &x, &y, &w, &h );
    }

    static char winfocus_line[] = { 1, 1 };
    QPen     old_pen = cpen;
    QBrush   old_brush = cbrush;
    RasterOp old_rop = (RasterOp)rop;

    setBrush( QBrush() );

    if ( xorPaint ) {
        if ( QColor::numBitPlanes() <= 8 )
	    setPen( color1 );
	else
	    setPen( white );
	setRasterOp( XorROP );
    } else {
	if ( qGray( bgColor.rgb() ) < 128 )
	    setPen( white );
	else
	    setPen( black );
    }

    gfx->setDashes(winfocus_line, 2);
    gfx->setDashedLines(TRUE);
    if ( cpen.style() != NoPen ) {
	gfx->drawLine(x,y,x+(w-1),y);
	gfx->drawLine(x+(w-1),y,x+(w-1),y+(h-1));
	gfx->drawLine(x,y+(h-1),x+(w-1),y+(h-1));
	gfx->drawLine(x,y,x,y+(h-1));
	x++;
	y++;
	w -= 2;
	h -= 2;
    }
    gfx->fillRect(x,y,w,h);
    gfx->setDashedLines(FALSE);

    setRasterOp( old_rop );
    setPen( old_pen );
    setBrush( old_brush );
}


void QPainter::drawRoundRect( int x, int y, int w, int h, int xRnd, int yRnd )
{

    if ( !isActive() )
	return;
    if ( xRnd <= 0 || yRnd <= 0 ) {
	drawRect( x, y, w, h );			// draw normal rectangle
	return;
    }
    if ( xRnd >= 100 )				// fix ranges
	xRnd = 99;
    if ( yRnd >= 100 )
	yRnd = 99;
    if ( testf(ExtDev) ) {
	QPDevCmdParam param[3];
	QRect r( x, y, w, h );
	param[0].rect = &r;
	param[1].ival = xRnd;
	param[2].ival = yRnd;
	if ( !pdev->cmd(QPaintDevice::PdcDrawRoundRect, this, param) || !gfx )
	    return;
    }
    QPointArray a;
    if ( w <= 0 || h <= 0 )
	fix_neg_rect( &x, &y, &w, &h );
    w--;
    h--;
    int rxx = w*xRnd/200;
    int ryy = h*yRnd/200;
    int rxx2 = 2*rxx;
    int ryy2 = 2*ryy;
    int xx, yy;

    // ###### WWA: this should use the new makeArc (with xmat)

    a.makeEllipse( x, y, rxx2, ryy2 );
    int s = a.size()/4;
    int i = 0;
    while ( i < s ) {
	a.point( i, &xx, &yy );
	xx += w - rxx2;
	a.setPoint( i++, xx, yy );
    }
    i = 2*s;
    while ( i < 3*s ) {
	a.point( i, &xx, &yy );
	yy += h - ryy2;
	a.setPoint( i++, xx, yy );
    }
    while ( i < 4*s ) {
	a.point( i, &xx, &yy );
	xx += w - rxx2;
	yy += h - ryy2;
	a.setPoint( i++, xx, yy );
    }
    drawPolyInternal( xForm(a) );
}


void QPainter::drawEllipse( int x, int y, int w, int h )
{
    if ( !isActive() )
	return;
    if ( testf(ExtDev) ) {
	QPDevCmdParam param[1];
	QRect r( x, y, w, h );
	param[0].rect = &r;
	if ( !pdev->cmd(QPaintDevice::PdcDrawEllipse, this, param) || !gfx )
	    return;
    }
    QPointArray a;
#ifndef QT_NO_TRANSFORMATIONS
    a.makeArc( x, y, w, h, 0, 360*16, xmat );
#else
    map( x, y, &x, &y );
    a.makeArc( x, y, w, h, 0, 360*16 );
#endif
    QPen oldpen=pen();
    QPen tmppen=oldpen;
    tmppen.setJoinStyle(BevelJoin);
    setPen(tmppen);
    drawPolyInternal( a );
    setPen(oldpen);
}


void QPainter::drawArc( int x, int y, int w, int h, int a, int alen )
{
    if ( !isActive() )
	return;
    if ( testf(ExtDev) ) {
	QPDevCmdParam param[3];
	QRect r( x, y, w, h );
	param[0].rect = &r;
	param[1].ival = a;
	param[2].ival = alen;
	if ( !pdev->cmd(QPaintDevice::PdcDrawArc, this, param) || !gfx )
	    return;
    }
    QPointArray pa;
#ifndef QT_NO_TRANSFORMATIONS
    pa.makeArc( x, y, w, h, a, alen, xmat );	// arc polyline
#else
    map( x, y, &x, &y );
    pa.makeArc( x, y, w, h, a, alen );		// arc polyline
#endif
    drawPolyInternal( pa, FALSE );
}


void QPainter::drawPie( int x, int y, int w, int h, int a, int alen )
{
    // Make sure "a" is 0..360*16, as otherwise a*4 may overflow 16 bits.
    if ( a > (360*16) ) {
	a = a % (360*16);
    } else if ( a < 0 ) {
	a = a % (360*16);
	if ( a < 0 ) a += (360*16);
    }
    if ( !isActive() )
	return;
    if ( testf(ExtDev) ) {
	QPDevCmdParam param[3];
	QRect r( x, y, w, h );
	param[0].rect = &r;
	param[1].ival = a;
	param[2].ival = alen;
	if ( !pdev->cmd(QPaintDevice::PdcDrawPie, this, param) || !gfx )
	    return;
    }
    QPointArray pa;
#ifndef QT_NO_TRANSFORMATIONS
    pa.makeArc( x, y, w, h, a, alen, xmat );	// arc polyline
#else
    map( x, y, &x, &y );
    pa.makeArc( x, y, w, h, a, alen );		// arc polyline
#endif
    int n = pa.size();
    int cx, cy;
#ifndef QT_NO_TRANSFORMATIONS
    xmat.map(x+w/2, y+h/2, &cx, &cy);
#else
    cx = x+w/2;
    cy = y+h/2;
#endif
    pa.resize( n+2 );
    pa.setPoint( n, cx, cy );			// add legs
    pa.setPoint( n+1, pa.at(0) );
    drawPolyInternal( pa );
}


void QPainter::drawChord( int x, int y, int w, int h, int a, int alen )
{
    if ( !isActive() )
	return;
    if ( testf(ExtDev) ) {
	QPDevCmdParam param[3];
	QRect r( x, y, w, h );
	param[0].rect = &r;
	param[1].ival = a;
	param[2].ival = alen;
	if ( !pdev->cmd(QPaintDevice::PdcDrawChord,this,param) || !gfx )
	    return;
    }
    QPointArray pa;
#ifndef QT_NO_TRANSFORMATIONS
    pa.makeArc( x, y, w-1, h-1, a, alen, xmat );// arc polygon
#else
    map( x, y, &x, &y );
    pa.makeArc( x, y, w-1, h-1, a, alen );	// arc polygon
#endif
    int n = pa.size();
    pa.resize( n+1 );
    pa.setPoint( n, pa.at(0) );			// connect endpoints
    drawPolyInternal( pa );
}


void QPainter::drawLineSegments( const QPointArray &a, int index, int nlines )
{
    if ( nlines < 0 )
	nlines = a.size()/2 - index/2;
    if ( index + nlines*2 > (int)a.size() )
	nlines = (a.size() - index)/2;
    if ( !isActive() || nlines < 1 || index < 0 )
	return;
    QPointArray pa = a;
    if ( testf(ExtDev) ) {
	if (2*nlines != (int)pa.size()) {
	    pa = QPointArray( nlines*2 );
	    for ( int i=0; i<nlines*2; i++ )
		pa.setPoint( i, a.point(index+i) );
	    index = 0;
	}
	QPDevCmdParam param[1];
	param[0].ptarr = (QPointArray*)&pa;
	if ( !pdev->cmd(QPaintDevice::PdcDrawLineSegments,this,param) || !gfx )
	    return;
    }
#ifndef QT_NO_TRANSFORMATIONS
    bool tx = (txop != TxNone);
#else
    bool tx = xlatex || xlatey;
#endif
    int x1, y1, x2, y2;
    for ( int i=0; i<nlines; i++ ) {
	a.point( index++, &x1, &y1 );
	a.point( index++, &x2, &y2 );
        if ( tx ) {
	    map( x1, y1, &x1, &y1 );
	    map( x2, y2, &x2, &y2 );
	}
	if ( cpen.style() != NoPen )
	    gfx->drawLine( x1, y1, x2, y2 );
    }
    if ( nlines > 0 ) {
	gfx->moveTo( x2, y2 );
	d->curPt = QPoint( x2, y2 );
    }
}


void QPainter::drawPolyline( const QPointArray &a, int index, int npoints )
{
    if ( npoints < 0 )
	npoints = a.size() - index;
    if ( index + npoints > (int)a.size() )
	npoints = a.size() - index;
    if ( !isActive() || npoints < 2 || index < 0 )
	return;
    QPointArray pa = a;
    if ( testf(ExtDev|VxF|WxF) ) {
	if ( testf(ExtDev) ) {
	    if ( npoints != (int)pa.size() ) {
		pa = QPointArray( npoints );
		for ( int i=0; i<npoints; i++ )
		    pa.setPoint( i, a.point(index+i) );
		index = 0;
	    }
	    QPDevCmdParam param[1];
	    param[0].ptarr = (QPointArray*)&pa;
	    if ( !pdev->cmd(QPaintDevice::PdcDrawPolyline,this,param) || !gfx )
		return;
	}
#ifndef QT_NO_TRANSFORMATIONS
	bool tx = (txop != TxNone);
#else
	bool tx = xlatex || xlatey;
#endif
	if ( tx ) {
	    pa = xForm( a, index, npoints );
	    if ( pa.size() != a.size() ) {
		index   = 0;
		npoints = pa.size();
	    }
	}
    }
    if ( cpen.style() != NoPen )
	gfx->drawPolyline( pa, index, npoints );
}


void QPainter::drawConvexPolygon( const QPointArray &pa,
			     int index, int npoints )
{
    // Any efficient way?
    drawPolygon(pa,FALSE,index,npoints);
}


void QPainter::drawPolygon( const QPointArray &a, bool winding,
			    int index, int npoints )
{
    if ( npoints < 0 )
	npoints = a.size() - index;
    if ( index + npoints > (int)a.size() )
	npoints = a.size() - index;
    if ( !isActive() || npoints < 2 || index < 0 )
	return;
    QPointArray pa = a;
    if ( testf(ExtDev|VxF|WxF) ) {
	if ( testf(ExtDev) ) {
	    if ( npoints != (int)a.size() ) {
		pa = QPointArray( npoints );
		for ( int i=0; i<npoints; i++ )
		    pa.setPoint( i, a.point(index+i) );
	    }
	    QPDevCmdParam param[2];
	    param[0].ptarr = (QPointArray*)&pa;
	    param[1].ival = winding;
	    if ( !pdev->cmd(QPaintDevice::PdcDrawPolygon,this,param) || !gfx )
		return;
	}
#ifndef QT_NO_TRANSFORMATIONS
	bool tx = (txop != TxNone);
#else
	bool tx = xlatex || xlatey;
#endif
	if ( tx ) {
	    pa = xForm( a, index, npoints );
	    if ( pa.size() != a.size() ) {
		index   = 0;
		npoints = pa.size();
	    }
	}
    }
    gfx->drawPolygon( pa, winding, index, npoints );
}


#ifndef QT_NO_BEZIER
void QPainter::drawCubicBezier( const QPointArray &a, int index )
{
    if ( !isActive() )
	return;
    if ( a.size() - index < 4 ) {
#if defined(QT_CHECK_RANGE)
	qWarning( "QPainter::drawCubicBezier: Cubic Bezier needs 4 control "
		 "points" );
#endif
	return;
    }
    QPointArray pa( a );
    if ( index != 0 || a.size() > 4 ) {
	pa = QPointArray( 4 );
	for ( int i=0; i<4; i++ )
	    pa.setPoint( i, a.point(index+i) );
    }
    if ( testf(ExtDev|VxF|WxF) ) {
	if ( testf(ExtDev) ) {
	    QPDevCmdParam param[1];
	    param[0].ptarr = (QPointArray*)&pa;
	    if ( !pdev->cmd(QPaintDevice::PdcDrawCubicBezier,this,param)
		 || !gfx )
		return;
	}
#ifndef QT_NO_TRANSFORMATIONS
	if ( txop != TxNone )
#else
	if ( xlatex || xlatey )
#endif
	    pa = xForm( pa );
    }
    if ( cpen.style() != NoPen ) {
	pa = pa.cubicBezier();
	gfx->drawPolyline( pa, 0, a.size() );
    }
}
#endif //QT_NO_BEZIER


void QPainter::drawPixmap( int x, int y, const QPixmap &pixmap,
			   int sx, int sy, int sw, int sh )
{
    if ( !isActive() || pixmap.isNull() )
	return;

    // right/bottom
    if ( sw < 0 )
	sw = pixmap.width()  - sx;
    if ( sh < 0 )
	sh = pixmap.height() - sy;

    // Sanity-check clipping
    if ( sx < 0 ) {
	x -= sx;
	sw += sx;
	sx = 0;
    }
    if ( sw + sx > pixmap.width() )
	sw = pixmap.width() - sx;
    if ( sy < 0 ) {
	y -= sy;
	sh += sy;
	sy = 0;
    }
    if ( sh + sy > pixmap.height() )
	sh = pixmap.height() - sy;

    if ( sw <= 0 || sh <= 0 )
	return;

#ifndef QT_NO_TRANSFORMATIONS
    if ( testf(ExtDev|VxF|WxF) ) {
	if ( testf(ExtDev) || txop == TxScale || txop == TxRotShear ) {
	    if ( sx != 0 || sy != 0 ||
		 sw != pixmap.width() || sh != pixmap.height() ) {
		QPixmap tmp( sw, sh, pixmap.depth() );
		bitBlt( &tmp, 0, 0, &pixmap, sx, sy, sw, sh, CopyROP, TRUE );
		if ( pixmap.mask() ) {
		    QBitmap mask( sw, sh );
		    bitBlt( &mask, 0, 0, pixmap.mask(), sx, sy, sw, sh,
			    CopyROP, TRUE );
		    tmp.setMask( mask );
		}
		drawPixmap( x, y, tmp );
		return;
	    }
	    if ( testf(ExtDev) ) {
		QPDevCmdParam param[2];
		QRect r( x, y, pixmap.width(), pixmap.height() );
		param[0].rect	= &r;
		param[1].pixmap = &pixmap;
		if ( !pdev->cmd(QPaintDevice::PdcDrawPixmap,this,param) || !gfx )
		    return;
	    }
#ifndef QT_NO_PIXMAP_TRANSFORMATION
	    if ( txop == TxScale || txop == TxRotShear ) {
		QWMatrix mat( m11(), m12(),
			      m21(), m22(),
			      dx(),  dy() );
		mat = QPixmap::trueMatrix( mat, sw, sh );
		QPixmap pm = pixmap.xForm( mat );
		if ( !pm.mask() && txop == TxRotShear ) {
		    QBitmap bm_clip( sw, sh, 1 );
		    bm_clip.fill( color1 );
		    pm.setMask( bm_clip.xForm(mat) );
		}
		map( x, y, &x, &y );		// compute position of pixmap
		int dx, dy;
		mat.map( 0, 0, &dx, &dy );
		uint save_flags = flags;
		flags = IsActive | (save_flags & ClipOn);
		drawPixmap( x-dx, y-dy, pm );
		flags = save_flags;
		return;
	    }
#endif
	}
	map( x, y, &x, &y );
    }
#else
    if ( testf(ExtDev|VxF|WxF) ) {
	if ( testf(ExtDev) ) {
	    QPDevCmdParam param[2];
	    QRect r( x, y, pixmap.width(), pixmap.height() );
	    param[0].rect   = &r;
	    param[1].pixmap = &pixmap;
	    if ( !pdev->cmd(QPaintDevice::PdcDrawPixmap,this,param) || !gfx )
		return;
	}
	map( x, y, &x, &y );
    }
#endif
    //bitBlt( pdev, x, y, &pixmap, sx, sy, sw, sh, CopyROP );
    gfx->setSource(&pixmap);
    if(sw>pixmap.width()) { // ###hanord: isn't this done above?
	sw=pixmap.width();
    }
    if(sh>pixmap.height()) {
	sh=pixmap.height();
    }
    if(pixmap.mask()) {
	QBitmap * mymask=( (QBitmap *)pixmap.mask() );
	unsigned char * thebits=mymask->scanLine(0);
	int ls=mymask->bytesPerLine();
	gfx->setAlphaType(QGfx::LittleEndianMask);
	gfx->setAlphaSource(thebits,ls);
    } else if ( pixmap.data->hasAlpha ){
	gfx->setAlphaType(QGfx::InlineAlpha);
    } else {
	gfx->setAlphaType(QGfx::IgnoreAlpha);
    }
    gfx->blt(x,y,sw,sh,sx,sy);
}


void QPainter::drawTiledPixmap( int x, int y, int w, int h,
				const QPixmap &pixmap, int sx, int sy )
{
    int sw = pixmap.width();
    int sh = pixmap.height();
    if (!sw || !sh || !gfx )
	return;
    if ( sx < 0 )
	sx = sw - -sx % sw;
    else
	sx = sx % sw;
    if ( sy < 0 )
	sy = sh - -sy % sh;
    else
	sy = sy % sh;

    map( x, y, &x, &y );

    gfx->setSource(&pixmap);
    if (pixmap.mask()) {
        QBitmap * mymask=( (QBitmap *)pixmap.mask() );
        unsigned char * thebits=mymask->scanLine(0);
        int ls=mymask->bytesPerLine();
        gfx->setAlphaType(QGfx::LittleEndianMask);
        gfx->setAlphaSource(thebits,ls);
    } else if ( pixmap.data->hasAlpha ) {
	gfx->setAlphaType(QGfx::InlineAlpha);
    } else {
	gfx->setAlphaType(QGfx::IgnoreAlpha);
    }
    gfx->setBrushOffset(sx,sy);
    gfx->tiledBlt(x,y,w,h);
}


#ifndef QT_NO_TRANSFORMATIONS
#if 0
//
// Generate a string that describes a transformed bitmap. This string is used
// to insert and find bitmaps in the global pixmap cache.
//

static QString gen_text_bitmap_key( const QWMatrix &m, const QFont &font,
				    const QString &str, int len )
{
    QString fk = font.key();
    int sz = 4*2 + len*2 + fk.length()*2 + sizeof(double)*6;
    QByteArray buf(sz);
    uchar *p = (uchar *)buf.data();
    *((double*)p)=m.m11();  p+=sizeof(double);
    *((double*)p)=m.m12();  p+=sizeof(double);
    *((double*)p)=m.m21();  p+=sizeof(double);
    *((double*)p)=m.m22();  p+=sizeof(double);
    *((double*)p)=m.dx();   p+=sizeof(double);
    *((double*)p)=m.dy();   p+=sizeof(double);
    QChar h1( '$' );
    QChar h2( 'q' );
    QChar h3( 't' );
    QChar h4( '$' );
    *((QChar*)p)=h1;  p+=2;
    *((QChar*)p)=h2;  p+=2;
    *((QChar*)p)=h3;  p+=2;
    *((QChar*)p)=h4;  p+=2;
    memcpy( (char*)p, (char*)str.unicode(), len*2 );  p += len*2;
    memcpy( (char*)p, (char*)fk.unicode(), fk.length()*2 ); p += fk.length()*2;
    return QString( (QChar*)buf.data(), buf.size()/2 );
}

static QBitmap *get_text_bitmap( const QString &key )
{
    return (QBitmap*)QPixmapCache::find( key );
}

static void ins_text_bitmap( const QString &key, QBitmap *bm )
{
    if ( !QPixmapCache::insert(key,bm) )	// cannot insert pixmap
	delete bm;
}
#endif
#endif // QT_NO_TRANSFORMATIONS

void qt_draw_transformed_rect( QPainter *p,  int x, int y, int w,  int h, bool fill )
{
    QPointArray points(5);
    int xp = x,  yp = y;
    p->map( xp, yp, &xp, &yp );
    points[0] = QPoint( xp,  yp );
    xp = x + w; yp = y;
    p->map( xp, yp, &xp, &yp );
    points[1] = QPoint( xp,  yp );
    xp = x + w; yp = y + h;
    p->map( xp, yp, &xp, &yp );
    points[2] = QPoint( xp,  yp );
    xp = x; yp = y + h;
    p->map( xp, yp, &xp, &yp );
    points[3] = QPoint( xp,  yp );
    points[4] = points[0];

    if ( fill )
	p->gfx->drawPolygon(points, FALSE, 0, 4);
    else
	p->gfx->drawPolyline(points,0, 5);
}

void qt_draw_background( QPainter *p, int x, int y, int w,  int h )
{
    if (p->testf(QPainter::ExtDev)) {
	if (p->pdev->devType() == QInternal::Printer)
	    p->fillRect(x, y, w, h, p->bg_col);
	return;
    }
    p->gfx->setPen( QPen::NoPen );
    p->gfx->setBrush( QBrush(p->backgroundColor()) );
    qt_draw_transformed_rect( p, x, y, w, h, TRUE);
    p->gfx->setBrush( p->cbrush );
    p->gfx->setPen( p->cpen );
}

void QPainter::drawText( int x, int y, const QString &str, int len,
			 QPainter::TextDirection dir )
{
    drawText( x, y, str, 0, len, dir );
}

void QPainter::drawText( int x, int y, const QString &str, int from, int len,
			 QPainter::TextDirection dir)
{
    if ( !isActive() )
	return;

    if (len < 0)
        len = str.length() - from;
    if ( len <= 0 || from >= (int)str.length() ) // empty string
        return;
    if ( from + len > (int)str.length() )
        len = str.length() - from;

    if ( testf(DirtyFont) )
	updateFont();

    if ( testf(ExtDev) ) {
	QPDevCmdParam param[3];
	QPoint p(x, y);
	QString string = str.mid( from, len );
	param[0].point = &p;
	param[1].str = &string;
	param[2].ival = QFont::Latin;// #######
	bool retval = pdev->cmd(QPaintDevice::PdcDrawText2, this, param);
	if ( !retval )
	    return;
    }

    if ( memorymanager->fontAscent(cfont.handle()) == 0 )
	return;

    bool simple = str.simpleText();
    // we can't take the complete string here as we would otherwise
    // get quadratic behaviour when drawing long strings in parts.
    // we do however need some chars around the part we paint to get arabic shaping correct.
    // ### maybe possible to remove after cursor restrictions work in QRT
    int start;
    int end;
    if ( simple ) {
	start = from;
	end = from+len;
    } else {
	start = QMAX( 0,  from - 8 );
	end = QMIN( (int)str.length(), from + len + 8 );
    }
    QConstString cstr( str.unicode() + start, end - start );
    from -= start;

    QTextEngine engine( cstr.string(), pfont ? pfont->d : cfont.d );
    QTextLayout layout( &engine );

    // this is actually what beginLayout does. Inlined here, so we can
    // avoid the bidi algorithm if we don't need it.
    engine.itemize( simple ? QTextEngine::NoBidi|QTextEngine::SingleLine : QTextEngine::SingleLine );
    engine.currentItem = 0;
    engine.firstItemInLine = -1;

    if ( !simple ) {
	layout.setBoundary( from );
	layout.setBoundary( from + len );
    }

    if ( dir != Auto ) {
	int level = dir == RTL ? 1 : 0;
	for ( int i = engine.items.size(); i >= 0; i-- )
	    engine.items[i].analysis.bidiLevel = level;
    }

    // small hack to force skipping of unneeded items
    start = 0;
    while ( engine.items[start].position < from )
	++start;
    engine.currentItem = start;
    layout.beginLine( 0xfffffff );
    end = start;
    while ( !layout.atEnd() && layout.currentItem().from() < from + len ) {
	layout.addCurrentItem();
	end++;
    }
    QFontMetrics fm(fontMetrics());
    int ascent = fm.ascent(), descent = fm.descent();
    int left, right;
    layout.endLine( 0, 0, Qt::SingleLine|Qt::AlignLeft, &ascent, &descent, &left, &right );

    // do _not_ call endLayout() here, as it would clean up the shaped items and we would do shaping another time
    // for painting.

    int textFlags = 0;
    if ( cfont.d->underline ) textFlags |= Qt::Underline;
    if ( cfont.d->overline ) textFlags |= Qt::Overline;
    if ( cfont.d->strikeOut ) textFlags |= Qt::StrikeOut;

    if ( bg_mode == OpaqueMode )
	qt_draw_background( this, x, y-ascent, right-left, ascent+descent+1);

    for ( int i = start; i < end; i++ ) {
	QTextItem ti;
	ti.item = i;
	ti.engine = &engine;

	drawTextItem( x, y - ascent, ti, textFlags );
    }
    layout.d = 0;
}


void QPainter::drawTextItem( int x,  int y, const QTextItem &ti, int textFlags )
{
    if ( testf(ExtDev) ) {
	QPDevCmdParam param[2];
	QPoint p(x, y);
	param[0].point = &p;
	param[1].textItem = &ti;
	bool retval = pdev->cmd(QPaintDevice::PdcDrawTextItem, this, param);
	if ( !retval || !gfx )
	    return;
    }

    QTextEngine *engine = ti.engine;
    QScriptItem *si = &engine->items[ti.item];

    engine->shape( ti.item );
    QFontEngine *fe = si->fontEngine;
    assert( fe );

    x += si->x;
    y += si->y;

    fe->draw( this, x,  y, engine, si, textFlags );
}

QPoint QPainter::pos() const
{
    if ( !gfx )
	return xFormDev( d->curPt );
    return gfx->pos();
}
