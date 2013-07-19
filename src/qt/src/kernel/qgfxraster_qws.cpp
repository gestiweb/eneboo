/*****************************************************************************
**
** Implementation of QGfxRaster (unaccelerated graphics context) class for
** Embedded Qt
**
** Created : 940721
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

#include "qgfxraster_qws.h"
#include "qpen.h"
#include "qpaintdevicemetrics.h"
#include "qmemorymanager_qws.h"
#include "qwsregionmanager_qws.h"
#include "qwsdisplay_qws.h"
#include "qgfxdriverfactory_qws.h"

#include <stdlib.h>
#include <math.h>

extern bool qws_sw_cursor;

#if !defined(_OS_FREEBSD_) && !defined(Q_OS_MAC)
# include <endian.h>
# if __BYTE_ORDER == __BIG_ENDIAN
#  define QT_QWS_REVERSE_BYTE_ENDIANNESS
//#  define QWS_BIG_ENDIAN
# endif
#endif

// #define QT_QWS_REVERSE_BYTE_ENDIANNESS

/*
  Uncomment the following for 1bpp/4bpp displays with a different
  endianness than the processor. This is endianness *within* a byte,
  ie. whether 0x01 is the rightmost or the leftmost pixel on the
  screen.

  This code is still experimental. There are no known bugs, but it has not
  been extensively tested.
*/
//#define QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS

// Pull this private function in from qglobal.cpp
extern unsigned int qt_int_sqrt( unsigned int n );

// VGA16 code does not compile on sparc
#if defined(__sparc__) && !defined(QT_NO_QWS_VGA_16)
#define QT_NO_QWS_VGA_16
#endif

#ifndef QT_NO_QWS_GFX_SPEED
# define QWS_EXPERIMENTAL_FASTPATH
# define GFX_INLINE inline
#else
# define GFX_INLINE
#endif

#if defined(QT_NO_QWS_GFX_SPEED)
#define QWS_NO_WRITE_PACKING
#endif

#define QGfxRaster_Generic 0
#define QGfxRaster_VGA16   1

#define GFX_8BPP_PIXEL(r,g,b)		gfx_screen->alloc(r,g,b)
#define GFX_8BPP_PIXEL_CURSOR(r,g,b)	qt_screen->alloc(r,g,b)

#define MASK4BPP(x) (0xf0 >> (x))

inline void gfxSetRgb24( unsigned char *d, unsigned int p )
{
    *d = p & 0x0000ff;
    *(d+1) = (p & 0x00ff00 ) >> 8;
    *(d+2) = (p & 0xff0000 ) >> 16;
}

inline void gfxSetRgb24( unsigned char *d, int r, int g, int b )
{
    *d = b;
    *(d+1) = g;
    *(d+2) = r;
}

inline unsigned int gfxGetRgb24( unsigned char *d )
{
    return *d | (*(d+1)<<8) | (*(d+2)<<16);
}

static bool simple_8bpp_alloc=FALSE;

// Used for synchronisation with accelerated drivers
static int dummy_optype = 0;
static int dummy_lastop = 0;
volatile int * optype = &dummy_optype;
volatile int * lastop = &dummy_lastop;

#ifndef QT_NO_QWS_CURSOR

/*!
  \class QScreenCursor qgfx_qws.h
  \ingroup qws
  \brief The QScreenCursor class manages the onscreen mouse cursor in
  Qt/Embedded.

  \internal (for now)

  It provides an implementation of a software mouse cursor
  and can be subclassed by hardware drivers which support a hardware mouse
  cursor. There may only be one QScreenCursor at a time; it is constructed
  by QScreen or one of its descendants.

  This class is non-portable. It is available only in Qt/Embedded.
  It is also internal - this documentation is intended for those subclassing
  it in hardware drivers, not for application developers.
*/

/*!
  \internal

  Constructs a screen cursor
*/

QScreenCursor::QScreenCursor() : gfx(0), gfxunder(0), imgunder(0), cursor(0)
{
}

/*!
  \internal

  Initialises a screen cursor - creates a Gfx to draw it with
  and an image to store the part of the screen stored under the cursor.
  Should not be called by hardware cursor descendants. \a da points
  to the location in framebuffer memory where the cursor saves information
  stored under it, \a init is true if the cursor is being initialized
  (i.e. if the program calling this is the Qt/Embedded server), false
  if another application has already initialized it.
*/

void QScreenCursor::init(SWCursorData *da, bool init)
{
    // initialise our gfx
    gfx = (QGfxRasterBase*)qt_screen->screenGfx();
    gfx->setClipRect( 0, 0, qt_screen->width(), qt_screen->height() );

    data = da;
    save_under = FALSE;
    fb_start = qt_screen->base();
    fb_end = fb_start + gfx->pixelHeight() * gfx->linestep();

    if (init) {
        data->x = gfx->pixelWidth()/2;
        data->y = gfx->pixelHeight()/2;
        data->width = 0;
        data->height = 0;
	data->enable = TRUE;
	data->bound = QRect( data->x - data->hotx, data->y - data->hoty,
		       data->width+1, data->height+1 );
    }
    clipWidth = qt_screen->deviceWidth();
    clipHeight = qt_screen->deviceHeight();

    int d = gfx->bitDepth();
    int cols = gfx->bitDepth() == 1 ? 0 : 256;
    if ( d == 4 ) {
	d = 8;
	cols = 16;
    }

    imgunder = new QImage( data->under, 64, 64, d, 0,
		cols, QImage::LittleEndian );
    if ( d <= 8 ) {
	for ( int i = 0; i < cols; i++ )
	    imgunder->setColor( i, qt_screen->clut()[i] );
    }
    gfxunder = (QGfxRasterBase*)imgunder->graphicsContext();
}

/*!
  \internal

  Destroys a screen cursor, deleting its gfxes, cursor image and
  under-cursor storage
*/

QScreenCursor::~QScreenCursor()
{
    delete gfx;
    delete gfxunder;
    delete imgunder;
}

/*!
  \internal

  Returns true if an alpha-blended cursor image is supported.
  This affects the type of QImage passed to the cursor - descendants
  returning true (as QScreenCursor does for bit depths of 8 and above.
  unless QT_NO_QWS_ALPHA_CURSOR is defined in qconfig.h) should be prepared
  to accept QImages with full 8-bit alpha channels
*/

bool QScreenCursor::supportsAlphaCursor()
{
#ifndef QT_NO_QWS_ALPHA_CURSOR
    return gfx->bitDepth() >= 8;
#else
    return FALSE;
#endif
}

/*!
  \internal

  Hide the mouse cursor from the screen.
*/

void QScreenCursor::hide()
{
    if ( data->enable ) {
	if (restoreUnder(data->bound))
	    QWSDisplay::ungrab();
	delete gfx;
	gfx = 0;
	data->enable = FALSE;
    }
}

/*!
  \internal

  Show the mouse cursor again after it has been hidden. Note that hides
  and shows are not nested; show() should always re-display the cursor no
  matter how many hide()s preceded it.
*/

void QScreenCursor::show()
{
    if ( !data->enable ) {
	if (qws_sw_cursor)
	   QWSDisplay::grab(true);
	data->enable = TRUE;
	gfx = (QGfxRasterBase*)qt_screen->screenGfx();
	gfx->setClipRect( 0, 0, qt_screen->width(), qt_screen->height() );
	fb_start = qt_screen->base();
	fb_end = fb_start + qt_screen->deviceHeight() * gfx->linestep();
	clipWidth = qt_screen->deviceWidth();
	clipHeight = qt_screen->deviceHeight();
	saveUnder();
    }
}

/*!
  \internal

  Sets a mouse cursor to \a image. The QImage is 32 bit, with an alpha
  channel containing either only 255 or 0 (that is, display the pixel
  or not) or a full alpha channel, depending on what
  supportsAlphaCursor() returns. \a hotx and \a hoty are the point within
  the QImage where mouse events actually 'come from'.
*/

void QScreenCursor::set(const QImage &image, int hotx, int hoty)
{
#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
    QWSDisplay::grab( TRUE );
#endif
    bool save = restoreUnder(data->bound);
    data->hotx = hotx;
    data->hoty = hoty;
    data->width = image.width();
    data->height = image.height();
    for ( int r = 0; r < image.height(); r++ )
	memcpy(data->cursor+data->width*r, image.scanLine(r), data->width);
    data->colors = image.numColors();
    int depth = qt_screen->depth();
    if ( depth <= 8 ) {
	for (int i = 0; i < image.numColors(); i++) {
	    int r = qRed( image.colorTable()[i] );
	    int g = qGreen( image.colorTable()[i] );
	    int b = qBlue( image.colorTable()[i] );
	    data->translut[i] = QColor(r, g, b).pixel();
	}
    }
    for (int i = 0; i < image.numColors(); i++) {
	data->clut[i] = image.colorTable()[i];
    }
    data->bound = QRect( data->x - data->hotx, data->y - data->hoty,
		   data->width+1, data->height+1 );
    if (save) saveUnder();
#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
    QWSDisplay::ungrab();
#endif
}

/*!
  \internal

  Move the mouse cursor to point (\a x, \a y) on the screen. This should be done
  in such a way that the hotspot of the cursor is at (x,y) - e.g. if the
  hotspot is at 5,5 within the image then the top left of the image should
  be at x-5,y-5
*/

void QScreenCursor::move( int x, int y )
{
    bool save = FALSE;
    if ( qws_sw_cursor ) {
#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
	QWSDisplay::grab( TRUE );
#endif
	save = restoreUnder(data->bound);
    }
    data->x = x;
    data->y = y;
    data->bound = QRect( data->x - data->hotx, data->y - data->hoty,
			data->width+1, data->height+1 );
    if ( qws_sw_cursor ) {
	if ( save )
	    saveUnder();
#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
	QWSDisplay::ungrab();
#endif
    }
}

/*!
  \internal

  This is relevant only to the software mouse cursor and should be
  reimplemented as a null method in hardware cursor drivers. It redraws
  what was under the mouse cursor when the cursor is moved. \a r
  is the rectangle that needs updating, \a g is the QGfx that will be
  used to redraw that rectangle.
*/

bool QScreenCursor::restoreUnder( const QRect &r, QGfxRasterBase *g )
{
    if ( !qws_sw_cursor )
	return FALSE;


    if (!data || !data->enable) {
	return FALSE;
    }

    if (!r.intersects(data->bound)) {
	return FALSE;
    }

    if ( g && !g->is_screen_gfx ) {
	return FALSE;
    }

    if (!save_under) {
#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
	QWSDisplay::grab( TRUE );
#endif
	int depth = gfx->bitDepth();

	int x = data->x - data->hotx;
	int y = data->y - data->hoty;

	if ( depth < 8 ) {
	    if ( data->width && data->height ) {
		gfx->gfx_swcursor = FALSE;   // prevent recursive call from blt
		QSize s( qt_screen->deviceWidth(), qt_screen->deviceHeight() );
		QRect r( x,y,data->width,data->height );
		r = qt_screen->mapFromDevice( r, s );
		gfx->setSource( imgunder );
		gfx->setAlphaType(QGfx::IgnoreAlpha);
		gfx->blt(r.x(), r.y(), r.width(), r.height(),0,0);
		gfx->gfx_swcursor = TRUE;
	    }
	} else {
	    // This is faster than the above - at least until blt is
	    // better optimized.
	    int linestep = gfx->linestep();
	    int startCol = x < 0 ? QABS(x) : 0;
	    int startRow = y < 0 ? QABS(y) : 0;
	    int endRow = y + data->height > clipHeight ? clipHeight - y : data->height;
	    int endCol = x + data->width > clipWidth ? clipWidth - x : data->width;

	    int srcLineStep = data->width * depth/8;
	    unsigned char *dest = fb_start + (y + startRow) * linestep
				    + (x + startCol) * depth/8;
	    unsigned char *src = data->under;

	    if ( endCol > startCol ) {
		int bytes;
		if ( depth < 8 )
		    bytes = (x + endCol - 1)*depth/8 - (x + startCol)*depth/8 + 1;
		else
		    bytes = (endCol - startCol) * depth / 8;
		if ( depth == 1 ) bytes++;
		for (int row = startRow; row < endRow; row++)
		{
		    memcpy(dest, src, bytes);
		    src += srcLineStep;
		    dest += linestep;
		}
	    }
	}
	save_under = TRUE;
	return TRUE;
    }

    return FALSE;
}

/*!
  \internal

  This saves the area under the mouse pointer - it should be reimplemented
  as a null method by hardware drivers.
*/

void QScreenCursor::saveUnder()
{
    if ( !qws_sw_cursor )
	return;

    int depth = gfx->bitDepth();
    int x = data->x - data->hotx;
    int y = data->y - data->hoty;

    if ( depth < 8 ) {
        gfxunder->gfx_swcursor = FALSE;   // prevent recursive call from blt
	gfxunder->setAlphaType(QGfx::IgnoreAlpha);
	gfxunder->srclinestep = gfx->linestep();
	gfxunder->srcdepth = gfx->bitDepth();
	gfxunder->srcbits = gfx->buffer;
	gfxunder->srctype = QGfx::SourceImage;
	gfxunder->srcpixeltype = QGfx::NormalPixel;
	gfxunder->srcwidth = qt_screen->width();
	gfxunder->srcheight = qt_screen->height();
	gfxunder->setSourceWidgetOffset( 0, 0 );
	gfxunder->src_normal_palette = TRUE;
	QSize s( qt_screen->deviceWidth(), qt_screen->deviceHeight() );
	QRect r( x, y, data->width, data->height );
	r = qt_screen->mapFromDevice( r, s );
	gfxunder->blt(0,0,data->width,data->height,r.x(), r.y());
	gfxunder->gfx_swcursor = TRUE;
    } else {
	// This is faster than the above - at least until blt is
	// better optimized.
	int linestep = gfx->linestep();
	int startRow = y < 0 ? QABS(y) : 0;
	int startCol = x < 0 ? QABS(x) : 0;
	int endRow = y + data->height > clipHeight ? clipHeight - y : data->height;
	int endCol = x + data->width > clipWidth ? clipWidth - x : data->width;

	int destLineStep = data->width * depth / 8;

	unsigned char *src = fb_start + (y + startRow) * linestep
				+ (x + startCol) * depth/8;
	unsigned char *dest = data->under;

	if ( endCol > startCol ) {
	    int bytes;
	    if ( depth < 8 )
		bytes = (x + endCol - 1)*depth/8 - (x + startCol)*depth/8 + 1;
	    else
		bytes = (endCol - startCol) * depth / 8;
	    for (int row = startRow; row < endRow; row++)
	    {
		memcpy(dest, src, bytes);
		src += linestep;
		dest += destLineStep;
	    }
	}
    }

    drawCursor();

    save_under = FALSE;

#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
    QWSDisplay::ungrab();
#endif

}

/*!
  \internal

  This draws the software cursor. It should be reimplemented as a null
  method by hardware drivers
*/

void QScreenCursor::drawCursor()
{
    /*
      We could use blt, but since cursor redraw speed is critical it
      is all handled here.  Whether this is significantly faster is
      questionable.
    */
    int x = data->x - data->hotx;
    int y = data->y - data->hoty;

    /* ### experimental
    if ( data->width != cursor->width() || data->height != cursor->height() ) {
	delete cursor;
	cursor = new QImage( data->cursor, data->width, data->height, 8,
			 data->clut, data->colors, QImage::IgnoreEndian );
    }
    if ( data->width && data->height ) {
	qt_sw_cursor = FALSE;   // prevent recursive call from blt
	gfx->setSource( cursor );
	gfx->setAlphaType(QGfx::InlineAlpha);
	gfx->blt(x,y,data->width,data->height,0,0);
	qt_sw_cursor = TRUE;
    }

    return;
    */

    int linestep = gfx->linestep();
    int depth = gfx->bitDepth();

    // clipping
    int startRow = y < 0 ? QABS(y) : 0;
    int startCol = x < 0 ? QABS(x) : 0;
    int endRow = y + data->height > clipHeight ? clipHeight - y : data->height;
    int endCol = x + data->width > clipWidth ? clipWidth - x : data->width;

    unsigned char *dest = fb_start + (y + startRow) * linestep
			    + x * depth/8;
    unsigned char *srcptr = data->cursor + startRow * data->width;

    QRgb *clut = data->clut;

#ifndef QT_NO_QWS_DEPTH_32
    if (depth == 32)
    {
	unsigned int *dptr = (unsigned int *)dest;
	unsigned int srcval;
	int av,r,g,b;
	for (int row = startRow; row < endRow; row++)
	{
	    for (int col = startCol; col < endCol; col++)
	    {
		srcval = clut[*(srcptr+col)];
		av = srcval >> 24;
		if (av == 0xff) {
		    *(dptr+col) = srcval;
		}
# ifndef QT_NO_QWS_ALPHA_CURSOR
		else if (av != 0) {
		    r = (srcval & 0xff0000) >> 16;
		    g = (srcval & 0xff00) >> 8;
		    b = srcval & 0xff;
		    unsigned int hold = *(dptr+col);
		    int sr=(hold & 0xff0000) >> 16;
		    int sg=(hold & 0xff00) >> 8;
		    int sb=(hold & 0xff);

		    r = ((r-sr) * av) / 256 + sr;
		    g = ((g-sg) * av) / 256 + sg;
		    b = ((b-sb) * av) / 256 + sb;

		    *(dptr+col) = (r << 16) | (g << 8) | b;
		}
# endif
	    }
	    srcptr += data->width;
	    dptr += linestep/4;
	}
	return;
    }
#endif
#ifndef QT_NO_QWS_DEPTH_24
    if (depth == 24)
    {
	unsigned int srcval;
	int av,r,g,b;
	for (int row = startRow; row < endRow; row++)
	{
	    unsigned char *dptr = dest + (row-startRow) * linestep + startCol * 3;
	    for (int col = startCol; col < endCol; col++, dptr += 3)
	    {
		srcval = clut[*(srcptr+col)];
		av = srcval >> 24;
		if (av == 0xff) {
		    gfxSetRgb24( dptr, srcval );
		}
# ifndef QT_NO_QWS_ALPHA_CURSOR
		else if (av != 0) {
		    r = (srcval & 0xff0000) >> 16;
		    g = (srcval & 0xff00) >> 8;
		    b = srcval & 0xff;
		    unsigned int hold = gfxGetRgb24( dptr );
		    int sr=(hold & 0xff0000) >> 16;
		    int sg=(hold & 0xff00) >> 8;
		    int sb=(hold & 0xff);

		    r = ((r-sr) * av) / 256 + sr;
		    g = ((g-sg) * av) / 256 + sg;
		    b = ((b-sb) * av) / 256 + sb;

		    gfxSetRgb24( dptr, r, g, b );
		}
# endif
	    }
	    srcptr += data->width;
	}
	return;
    }
#endif
#ifndef QT_NO_QWS_DEPTH_16
    if (depth == 16)
    {
	unsigned short *dptr = (unsigned short *)dest;
	unsigned int srcval;
	int av,r,g,b;
	for (int row = startRow; row < endRow; row++)
	{
	    for (int col = startCol; col < endCol; col++)
	    {
		srcval = clut[*(srcptr+col)];
		av = srcval >> 24;
		if (av == 0xff) {
		    *(dptr+col) = qt_convRgbTo16(srcval);
		}
# ifndef QT_NO_QWS_ALPHA_CURSOR
		else if (av != 0) {
		    // This is absolutely silly - but we can so we do.
		    r = (srcval & 0xff0000) >> 16;
		    g = (srcval & 0xff00) >> 8;
		    b = srcval & 0xff;

		    int sr;
		    int sg;
		    int sb;
		    qt_conv16ToRgb(*(dptr+col),sr,sg,sb);

		    r = ((r-sr) * av) / 256 + sr;
		    g = ((g-sg) * av) / 256 + sg;
		    b = ((b-sb) * av) / 256 + sb;

		    *(dptr+col) = qt_convRgbTo16(r,g,b);
		}
# endif
	    }
	    srcptr += data->width;
	    dptr += linestep/2;
	}
	return;
    }
#endif
#if !defined(QT_NO_QWS_DEPTH_8)
    if (depth == 8) {
	unsigned char *dptr = (unsigned char *)dest;
        unsigned int srcval;
	int av,r,g,b;
	QRgb * screenclut=qt_screen->clut();
	simple_8bpp_alloc=TRUE;
	for (int row = startRow; row < endRow; row++)
	{
	    for (int col = startCol; col < endCol; col++)
	    {
		srcval = clut[*(srcptr+col)];
		av = srcval >> 24;
		if (av == 0xff) {
		    *(dptr+col) = data->translut[*(srcptr+col)];
		}
# ifndef QT_NO_QWS_ALPHA_CURSOR
		else if (av != 0) {
		    // This is absolutely silly - but we can so we do.
		    r = (srcval & 0xff0000) >> 16;
		    g = (srcval & 0xff00) >> 8;
		    b = srcval & 0xff;

		    unsigned char hold = *(dptr+col);
		    int sr,sg,sb;
		    sr=qRed(screenclut[hold]);
		    sg=qGreen(screenclut[hold]);
		    sb=qBlue(screenclut[hold]);

		    r = ((r-sr) * av) / 256 + sr;
		    g = ((g-sg) * av) / 256 + sg;
		    b = ((b-sb) * av) / 256 + sb;

		    *(dptr+col) = GFX_8BPP_PIXEL_CURSOR(r,g,b);
		}
# endif
	    }
	    srcptr += data->width;
	    dptr += linestep;
	}
	simple_8bpp_alloc=FALSE;
    }
#endif
#ifndef QT_NO_QWS_DEPTH_4
    if ( depth == 4 ) {
        unsigned int srcval;
	int av;
	for (int row = startRow; row < endRow; row++)
	{
	    unsigned char *dp = fb_start + (y + row) * linestep;
	    for (int col = startCol; col < endCol; col++)
	    {
		srcval = clut[*(srcptr+col)];
		av = srcval >> 24;
		if (av == 0xff) {
		    int tx = x + col;
		    unsigned char *dptr = dp + (tx>>1);
		    int val = data->translut[*(srcptr+col)];
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
		    int s = (~tx & 1) << 2;
#else
		    int s = (tx & 1) << 2;
#endif
		    *dptr = ( *dptr & MASK4BPP(s) ) | (val << s);
		}
	    }
	    srcptr += data->width;
	}
    }
#endif
#ifndef QT_NO_QWS_DEPTH_1
    if ( depth == 1 ) {
        unsigned int srcval;
	int av;
	for (int row = startRow; row < endRow; row++)
	{
	    unsigned char *dp = fb_start + (y + row) * linestep;
	    int x1 = x+startCol;
	    int x2 = x+endCol-1;
	    dp += x1/8;
	    int skipbits = x1%8;
	    int col = startCol;
	    for ( int b = x1/8; b <= x2/8; b++ ) {
		unsigned char m = *dp;
		for (int i = 0; i < 8 && col < endCol; i++) {
		    if (skipbits)
			skipbits--;
		    else {
			srcval = clut[*(srcptr+col)];
			av = srcval >> 24;
			if (av == 0xff) {
			    unsigned char val = data->translut[*(srcptr+col)];
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
			    if (val)
				m |= 0x80 >> i;
			    else
				m &= ~( 0x80 >> i );
#else
			    if (val)
				m |= 1 << i;
			    else
				m &= ~( 1 << i );
#endif
			}
			col++;
		    }
		}
		*(dp++) = m;
	    }
	    srcptr += data->width;
	}
    }
#endif
}

#endif // QT_NO_QWS_CURSOR

// Define this to ensure the pixels in (1 pixel wide) polyline joins are
// only written once, i.e. XOR polyline joins work correctly.
//#define GFX_CORRECT_POLYLINE_JOIN

#ifdef GFX_CORRECT_POLYLINE_JOIN
static QPoint *gfx_storedLineRd = 0;
static QPoint *gfx_storedLineWr = 0;
static bool gfx_storeLine = FALSE;
static int gfx_storedLineRead = 0;
static int gfx_storedLineWrite = 0;
static int gfx_storedLineDir = 1;
static bool gfx_noLineOverwrite = FALSE;
static int gfx_storedLineBufferSize = 0;
static bool gfx_doDraw = TRUE;
#else
static const bool gfx_storeLine = FALSE;
#endif

/*!
  \class QGfxRasterBase qgfxraster_qws.h
  \ingroup qws
  \brief The QGfxRasterBase class is the base class of the
  QGfxRaster<depth> template and contains the non-depth-dependent code.

  \internal (for now)

  The QGfxRaster class is used for drawing in software on raw
  framebuffers of varying depths and  is subclassed by hardware
  drivers. It handles clipping and a movable origin in order to
  support subwindows. It is available \e only in Qt/Embedded. QWidget
  and QPixmap both return a QGfxRaster via their respective
  graphicsContext() methods, already initialized with the appropriate
  origin, framebuffer and clip region. QGfxRasterBase and its
  template subclasses should effectively be considered as one class;
  a raw QGfxRasterBase is never used, it's simply a handy place to
  put some of the functionality.
*/

/*!
  \internal

  This constructed a QGfxRasterBase. \a b is the data buffer pointed to,
  \a w and \a h its width and height in pixels
*/

QGfxRasterBase::QGfxRasterBase(unsigned char * b,int w,int h) :
    buffer(b),backcolor(Qt::black),cpen(Qt::black), cbrush(Qt::black)
{
    // Buffers should always be aligned
    if(((unsigned long)b) & 0x3) {
	qDebug("QGfx buffer unaligned: %lx",(unsigned long)b);
    }

#ifdef QT_PAINTER_LOCKING
    QWSDisplay::grab();
#endif

    gfx_screen=qt_screen;
#ifndef QT_NO_QWS_CURSOR
    gfx_screencursor=qt_screencursor;
    gfx_swcursor=qt_sw_cursor;
#endif
    srcpixeltype = pixeltype = NormalPixel;
    is_screen_gfx = buffer==qt_screen->base();
    width=w;
    height=h;
    xoffs=0;
    yoffs=0;

    srctype=SourcePen;
    dashedLines = FALSE;
    dashes = 0;
    numDashes = 0;

    patternedbrush=FALSE;
    srccol=cbrush.color().pixel();
    cbrushpixmap=0;

    regionClip=FALSE;
    QRect wr(0,0,w,h);
    wr = qt_screen->mapToDevice( wr, QSize( w, h) );
    widgetrgn = wr;
    cliprect = new QRect[1];
    cliprect[0] = wr;
    ncliprect = 1;
    clipbounds = wr;
    clipcursor = 0;
    clipDirty = FALSE;

    penx=0;
    peny=0;
    alphatype=IgnoreAlpha;
    alphabuf = 0;
    ismasking=FALSE;
    srclinestep=0;
    srcbits=0;
    lstep=0;
    calpha=255;
    opaque=FALSE;
    globalRegionRevision = 0;
    src_normal_palette=FALSE;
    clutcols = 0;
    gfx_lastop=lastop;
    gfx_optype=optype;
    myrop=CopyROP;
    stitchedges=QPolygonScanner::Edge(QPolygonScanner::Left+QPolygonScanner::Top);

    src_little_endian=TRUE;
#if !defined(QT_NO_QWS_DEPTH_8)
    // default color map
    setClut( gfx_screen->clut(), gfx_screen->numCols() );
#endif
}

/*!
  \internal

  Destroys a QGfxRaster
*/

QGfxRasterBase::~QGfxRasterBase()
{
#ifdef QT_PAINTER_LOCKING
    QWSDisplay::ungrab();
#endif
    delete [] dashes;
    delete [] cliprect;
}


void* QGfxRasterBase::beginTransaction(const QRect& r)
{
    GFX_START(r);
#ifndef QT_NO_QWS_CURSOR
    return (void*)swc_do_save;
#else
    return (void*)0;
#endif
}

void QGfxRasterBase::endTransaction(void* data)
{
    bool swc_do_save = !!data;
    GFX_END;
}

/*!
  \internal

  This does very little in a purely-software QGfxRasterBase (simply
  records that the last operation was a software one). Hardware drivers
  should reimplement this to wait for graphics engine idle in order to
  allow software and hardware drawing to synchronize properly.
*/

void QGfxRasterBase::sync()
{
    (*gfx_optype)=0;
}

/*!
  \internal

  This corresponds to QPainter::setPen - it tells QGfxRaster
  what line color and style to use, i.e. to use pen \a p.
*/

void QGfxRasterBase::setPen( const QPen & p )
{
    static char dash_line[]         = { 7, 3 };
    static char dot_line[]          = { 1, 3 };
    static char dash_dot_line[]     = { 7, 3, 2, 3 };
    static char dash_dot_dot_line[] = { 7, 3, 2, 3, 2, 3 };

    cpen=p;
    switch (cpen.style()) {
        case DashLine:
            setDashes( dash_line, sizeof(dash_line) );
	    setDashedLines(TRUE);
            break;
        case DotLine:
            setDashes( dot_line, sizeof(dot_line) );
	    setDashedLines(TRUE);
            break;
        case DashDotLine:
            setDashes( dash_dot_line, sizeof(dash_dot_line) );
	    setDashedLines(TRUE);
            break;
        case DashDotDotLine:
            setDashes( dash_dot_dot_line, sizeof(dash_dot_dot_line) );
	    setDashedLines(TRUE);
            break;
        default:
	    setDashedLines(FALSE);
            break;
    }
}

/*!
  \internal

  This is a simplified case of setClipRegion, setting a clip region consisting
  of one rectangle defined by position \a (x, y), width \a w and height
  \a h.
*/

void QGfxRasterBase::setClipRect( int x,int y,int w,int h )
{
    setClipRegion(QRegion(x,y,w,h));
}

/*!
  \internal

  This sets the clipping region for the QGfx to region \a r. All
  drawing outside of the region is not displayed. The clip region is
  defined relative to the QGfx's origin at the time the clip region
  is set, and consists of an array of rectangles stored in the array
  cliprect. Note that changing the origin after the clip region is
  set will not change the position of the clip region within the
  buffer. Hardware drivers should use this to set their clipping
  scissors when drawing. Note also that this is the user clip region
  as set by QPainter; it is combined (via an intersection) with the
  widget clip region to provide the actual clipping region.
*/

void QGfxRasterBase::setClipRegion( const QRegion & r )
{
    regionClip=TRUE;
    cliprgn=r;
    cliprgn.translate(xoffs,yoffs);
    cliprgn = qt_screen->mapToDevice( cliprgn, QSize( width, height ) );
    update_clip();

#ifdef QWS_EXTRA_DEBUG
    qDebug( "QGfxRasterBase::setClipRegion" );
    for (int i=0; i< ncliprect; i++) {
	QRect r = cliprect[i];
	qDebug( "   cliprect[%d] %d,%d %dx%d", i, r.x(), r.y(),
		r.width(), r.height() );
    }
#endif


}

void QGfxRasterBase::setClipDeviceRegion( const QRegion & r )
{
    regionClip=TRUE;
    cliprgn=r;
    update_clip();
}

/*!
  \internal

  If \a b is TRUE then clipping is switched enabled; otherwise clipping
  is disabled.

  If clipping is not enabled then drawing will access the whole buffer.
  This will be reflected in the cliprect array, which will consist of
  one rectangle of buffer width and height. The variable regionClip
  defines whether to clip or not.
*/

void QGfxRasterBase::setClipping(bool b)
{
    if(regionClip!=b) {
	regionClip=b;
	update_clip();
    } else if ( clipDirty ) {
	update_clip();
    }
}

/*!
  \internal

  This defines the (\a x, \a y) origin of the gfx. For instance, if
  the origin is set to (100, 100) and a line is then drawn from (0,
  0) to (10, 10) the line will be (relative to the top left of the
  buffer) from 100,100 to (110, 110). This is used to support windows
  within the buffer.
*/

void QGfxRasterBase::setOffset( int x, int y )
{
    xoffs=x;
    yoffs=y;
}

/*!
  \internal

  This is a special case of setWidgetRegion for widgets which are not
  shaped and not occluded by any other widgets. The rectangle is
  defined by position \a(x, y), width \a w and height \a h.
*/

void QGfxRasterBase::setWidgetRect( int x,int y, int w, int h )
{
    setWidgetRegion(QRegion(x,y,w,h));
}

/*!
  \internal

  This sets the widget's region clip to \a r, which is combined with
  the user clip to determine the widget's drawable region onscreen.
  It's a combination of the widget's shape (if it's a shaped widget)
  and the area not obscured by windows on top of it.
*/

void QGfxRasterBase::setWidgetRegion( const QRegion & r )
{
    widgetrgn = qt_screen->mapToDevice( r, QSize( width, height ) );
    clipDirty = TRUE;
    hsync(r.boundingRect().bottom());
}

void QGfxRasterBase::setWidgetDeviceRegion( const QRegion & r )
{
    widgetrgn = r;
    clipDirty = TRUE;
    hsync(r.boundingRect().bottom());
}

void QGfxRasterBase::setGlobalRegionIndex( int idx )
{
    globalRegionIndex = idx;
    globalRegionRevision = qt_fbdpy->regionManager()->revision( idx );
    currentRegionRevision = *globalRegionRevision;
}

/*!
  \internal

  If \a d is TRUE then the gfx should draw dashed lines; otherwise it
  should draw solid lines. It's called by setPen so there is no need
  to call it directly.
*/

void QGfxRasterBase::setDashedLines(bool d)
{
    dashedLines = d;
}

/*!
  \internal

  This defines the pattern for dashed lines. It's called by setPen
  so there is no need to call it directly. \a dashList is a 1 bit
  per pixel specification of the dashes in the line, \a n is the
  number of bytes in the dashList.
*/

void QGfxRasterBase::setDashes(char *dashList, int n)
{
    if (dashes) delete [] dashes;
    dashes = new char [n];
    memcpy(dashes, dashList, n);
    numDashes = n;
}


void QGfxRasterBase::fixClip()
{
    currentRegionRevision = *globalRegionRevision;
    QRegion rgn = qt_fbdpy->regionManager()->region( globalRegionIndex );
    widgetrgn &= rgn;
    update_clip();
}

struct _XRegion {
    int numRects;
    QMemArray<QRect> rects;
    // ... etc.
};

/*!
  \internal

  This combines the currently set widget and user clips
  and caches the result in an array of QRects, cliprect,
  the size of which is stored in ncliprect. It's called whenever
  the widget or user clips are changed.
*/

void QGfxRasterBase::update_clip()
{
    _XRegion* wr = (_XRegion*) widgetrgn.handle();
    _XRegion* cr = (_XRegion*) cliprgn.handle();

    if ( wr->numRects==0) {
	// Widget not visible
	ncliprect = 0;
	delete [] cliprect;
	cliprect = 0;
	clipbounds = QRect();
    } else if ( wr->numRects==1 && (!regionClip || cr->numRects==1) ) {
	// fastpath: just simple rectangles (90% of cases)
	QRect setrgn;

	if(regionClip) {
	    setrgn=wr->rects[0].intersect(cr->rects[0]);
	} else {
	    setrgn=wr->rects[0];
	}

	if ( setrgn.isEmpty() ) {
	    ncliprect = 0;
	    clipbounds = QRect();
	} else {
	    // cache bounding rect
	    QRect sr( QPoint(0,0), qt_screen->mapToDevice( QSize(width, height) ) );
	    clipbounds = sr.intersect(setrgn);

	    // Convert to simple array for speed
	    if ( ncliprect < 1 ) {
		delete [] cliprect;
		cliprect = new QRect[1];
	    }
	    cliprect[0] = setrgn;
	    ncliprect = 1;
	}
    } else {
#ifdef QWS_EXTRA_DEBUG
	qDebug( "QGfxRasterBase::update_clip" );
#endif
	QRegion setrgn;
	if(regionClip) {
	    setrgn=widgetrgn.intersect(cliprgn);
	} else {
	    setrgn=widgetrgn;
	}

	// cache bounding rect
	QRect sr( QPoint(0,0), gfx_screen->mapToDevice( QSize(width, height) ) );
	clipbounds = sr.intersect(setrgn.boundingRect());

	// Convert to simple array for speed
	_XRegion* cr = (_XRegion*) setrgn.handle();
	const QArray<QRect> &a = cr->rects;
	delete [] cliprect;
	cliprect = new QRect[cr->numRects];
	memcpy( cliprect, a.data(), cr->numRects*sizeof(QRect) );
	ncliprect = cr->numRects;
    }
    clipcursor = 0;
    clipDirty = FALSE;
}

/*!
  \internal

  This is the counterpart to QPainter::moveTo. It simply stores the
  \a x and \a y values passed to it until a lineTo.
*/

void QGfxRasterBase::moveTo( int x,int y )
{
    penx=x;
    peny=y;
}

/*!
  \internal

  This draws a line from the last values passed to moveTo to (\a x,
  \a y). It calls drawLine so there is no need to reimplement it in
  an accelerated driver.
*/

void QGfxRasterBase::lineTo( int x,int y )
{
    drawLine(penx,peny,x,y);
    penx=x;
    peny=y;
}

QPoint QGfxRasterBase::pos() const
{
    return QPoint(penx, peny);
}

/*!
  \internal

  This stores the offset from the screen framebuffer of the widget
  from which a blt() is being performed - this is added to the source
  \a x and \a y coordinates from a bitBlt to produce the source
  screen position of the blt
*/
void QGfxRasterBase::setSourceWidgetOffset(int x ,int y)
{
    srcwidgetoffs = QPoint(x,y);
}

/*!
  \internal

This sets one of several alpha channel types for the next
blt operation to \a a:

\list
\i IgnoreAlpha - Always draw source pixels as-is (no alpha blending)
\i InlineAlpha - An 8-bit alpha value is in the highest byte of the
                       (32-bit) source data
\i SeparateAlpha - A separate 8-bit alpha channel buffer is provided
                       (used for anti-aliased text)
\i LittleEndianMask - A separate little-bit-endian mask is provided
\i BigEndianMask - A separate big-bit-endian mask is provided
\i SolidAlpha - A single 8-bit alpha value is to be applied to
                       all pixels
\endlist

The alpha channel buffer/value is provided by setAlphaSource
*/

void QGfxRasterBase::setAlphaType(AlphaType a)
{
    alphatype=a;
    if(a==LittleEndianMask || a==BigEndianMask) {
	ismasking=TRUE;
    } else {
	ismasking=FALSE;
    }
}

/*!
  \internal

  This is used in conjunction with LittleEndianMask, BigEndianMask or
  SeparateAlpha alpha channels. \a b is a pointer to the bytes
  containing the alpha values, \a l is the linestep (length in bytes
  per horizontal line of data)
*/

void QGfxRasterBase::setAlphaSource(unsigned char * b,int l)
{
    alphabits=b;
    alphalinestep=l;
}

/*!
  \internal
  \overload

  This is used by the SolidAlpha alpha channel type and sets a single
  alpha value to be used in blending all of the source data. The
  value is between 0 (draw nothing) and 255 (draw solid source data)
  - a value of 128 would draw a 50% blend between the source and
  destination data. Normally called with just one argument, where \a
  i is the alpha value to use. If \a i2, \a i3 and \a i4 are used
  they specify different alpha values for the corners of an
  alpha-blended rectangle; this is only used by some experimental
  hardware-accelerated alpha-blending code.
*/

void QGfxRasterBase::setAlphaSource(int i,int i2,int i3,int i4)
{
    calpha=i;
    if(i2==-1)
	i2=i;
    if(i3==-1)
	i3=i;
    if(i4==-1)
	i4=i;
    calpha2=i2;
    calpha3=i3;
    calpha4=i4;
    setAlphaType(SolidAlpha);
}

/*!
  \internal

  Draws a line of Unicode text given in \a s at position \a (x, y)
  using the font set by QFont. It performs a series of blt's using a
  pen source in the current pen color and either an 8-bit alpha
  channel (for anti-aliased text) or a big-endian mask provided by
  the font subsystem.
*/

void QGfxRasterBase::drawGlyphs(QMemoryManager::FontID font, glyph_t *glyphs, QPoint *positions, int num_glyphs )
{
    if (!ncliprect)
	return;
    // Clipping can be handled by blt
    // Offset is handled by blt

    int loopc;

#ifdef DEBUG_LOCKS
    qDebug("unaccelerated drawText grab");
#endif

#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
    QWSDisplay::grab(); // we need it later, and grab-must-precede-lock
#endif

#ifdef DEBUG_LOCKS
    qDebug("unaccelerated drawText lock");
#endif

    setSourcePen();
    if ( memorymanager->fontSmooth(font) ) {
	setAlphaType(SeparateAlpha);
    } else {
	setAlphaType(BigEndianMask);
    }

    for( loopc=0; loopc < num_glyphs; loopc++ ) {
	QGlyph glyph = memorymanager->lockGlyph( font, glyphs[loopc] );
	int myw = glyph.metrics->width;
	srcwidth = myw;
	srcheight = glyph.metrics->height;
	setAlphaSource(glyph.data,glyph.metrics->linestep);
	int myx = positions[loopc].x();
	int myy = positions[loopc].y();
	myx += glyph.metrics->bearingx;
	myy -= glyph.metrics->bearingy;
	if( glyph.metrics->width < 1 || glyph.metrics->height < 1
	    || glyph.metrics->width > 1000 || glyph.metrics->height > 1000
	    || glyph.metrics->linestep == 0)
	{
	    // non-printing characters
	} else {
	    blt(myx,myy,myw,glyph.metrics->height,0,0);
	}
	// ... unlock glyph
    }
#ifdef DEBUG_LOCKS
    qDebug("unaccelerated drawText unlock");
#endif
#ifdef DEBUG_LOCKS
    qDebug("unaccelerated drawText ungrab");
#endif
#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
    QWSDisplay::ungrab();
#endif
    setAlphaType(IgnoreAlpha);
}

/*!
  \internal

  This saves the current brush and pen state to temporary variables.
  This is used internally in QGfxRaster when a temporary pen or brush
  is needed for something. This is not a stack; a save() followed by
  a save() will obliterate the previously saved brush and pen.
*/

void QGfxRasterBase::save()
{
    savepen=cpen;
    savebrush=cbrush;
}

/*!
  \internal

  Restores the brush and pen from a previous save().
*/

void QGfxRasterBase::restore()
{
    setPen(savepen);
    setBrush(savebrush);
}

// inline QRect::setCoords
inline void QRect::setCoords( int xp1, int yp1, int xp2, int yp2 )
{
    x1 = (QCOORD)xp1;
    y1 = (QCOORD)yp1;
    x2 = (QCOORD)xp2;
    y2 = (QCOORD)yp2;
}

/*!
  \internal

  Returns whether the point (\a x, \a y) is in the clip region.

  If \a cr is not null, \c *\cr is set to a rectangle containing
  the point, and within all of which the result does not change.
  If the result is TRUE, \a cr is the widest rectangle for which
  the result remains TRUE (so any point immediately to the left or
  right of \a cr will not be part of the clip region).

  Passing TRUE for the \a known_to_be_outside allows optimizations,
  but the results are not defined it (\a x, \a y) is in the clip region.

  Using this, you can efficiently iterator over the clip region
  using:

  \code
    bool inside = inClip(x,y,&cr);
    while (change y, preferably by +1) {
	while (change x by +1 or -1) {
	    if ( !cr.contains(x,y) )
		inside = inClip(x,y,&cr,inside);
	    if ( inside ) {
		draw stuff
	    }
	}
    }
  \endcode
*/
bool QGfxRasterBase::inClip(int x, int y, QRect* cr, bool known_to_be_outside)
{
    if ( !ncliprect ) {
	// No rectangles.
	if ( cr )
	    *cr = QRect(x-4000,y-4000,8000,8000);
	return FALSE;
    }

//qDebug("Find %d,%d...%s",x,y,known_to_be_outside?" (outside)":"");
    bool search=FALSE;
    const QRect *cursorRect = &cliprect[clipcursor];

//search=TRUE;
    if ( !known_to_be_outside ) {
	if ( cursorRect->contains(x,y) ) {
	    if ( cr )
		*cr = *cursorRect;

//  qDebug("found %d,%d at +0 in %d[%d..%d,%d..%d]",x,y,clipcursor,cliprect[clipcursor].left(),cliprect[clipcursor].right(),cliprect[clipcursor].top(),cliprect[clipcursor].bottom());
	    return TRUE;
	}
	if ( clipcursor > 0 ) {
	    if ( (cursorRect-1)->contains(x,y) ) {
		if ( cr )
		    *cr = cliprect[--clipcursor];

// qDebug("found %d,%d at -1 in %d[%d..%d,%d..%d]",x,y,clipcursor,cliprect[clipcursor].left(),cliprect[clipcursor].right(),cliprect[clipcursor].top(),cliprect[clipcursor].bottom());
		return TRUE;
	    }
	} else if ( clipcursor < (int)ncliprect-1 ) {
	    if ( (cursorRect+1)->contains(x,y) ) {
		if ( cr )
		    *cr = cliprect[++clipcursor];

// qDebug("found %d,%d at +1 in %d[%d..%d,%d..%d]",x,y,clipcursor,cliprect[clipcursor].left(),cliprect[clipcursor].right(),cliprect[clipcursor].top(),cliprect[clipcursor].bottom());
		return TRUE;
	    }
	}
	search=TRUE;
    }

    // Optimize case where (x,y) is in the same band as the clipcursor,
    // and to its right.  eg. left-to-right, same-scanline cases.
    //
    if ( cursorRect->right() < x
	&& cursorRect->top() <= y
	&& cursorRect->bottom() >= y )
    {
	// Move clipcursor right until it is after (x,y)
	for (;;) {
	    if ( clipcursor+1 < ncliprect &&
		 (cursorRect+1)->top()==cursorRect->top() ) {
		// next clip rect is in this band too - move ahead
		clipcursor++;
		cursorRect++;
		if ( cursorRect->left() > x ) {
		    // (x,y) is between clipcursor-1 and clipcursor
		    if ( cr )
			cr->setCoords((cursorRect-1)->right()+1,
				cursorRect->top(),
				cursorRect->left()-1,
				cursorRect->bottom());
		    return FALSE;
		} else if ( cursorRect->right() >= x ) {
		    // (x,y) is in clipcursor
		    if ( cr )
			*cr = *cursorRect;

// qDebug("found %d,%d in %d[%d..%d,%d..%d]",x,y,clipcursor,cliprect[clipcursor].left(),cliprect[clipcursor].right(),cliprect[clipcursor].top(),cliprect[clipcursor].bottom());
		    return TRUE;
		}
	    } else {
		// (x,y) is after last rectangle on band
		if ( cr )
		    cr->setCoords(cursorRect->right()+1,
			    cursorRect->top(),y+4000,
			    cursorRect->bottom());
		return FALSE;
	    }
	}
    } else {
	search=TRUE;
    }

    // The "4000" below are infinitely large rectangles, made small enough
    // to let surrounding alrogithms work of small integers. It means that
    // in rare cases some extra calls may be made to this function, but that
    // will make no measurable difference in performance.

    /*
	(x,y) will be in one of these characteristic places:

	0. In a rectangle of the region
	1. Before the region
	2. To the left of the first rectangle in the first band
	3. To the left of the first rectangle in a non-first band
	4. Between two retcangles in a band
	5. To the right of the last rectangle in a non-last band
	6. Between the last two rectangles
	7. To the right of the last rectangle in the last band
	8. After the region
	9. Between the first two rectangles

                            1
                     2   BBBBBBB
                  3 BB0BBBB 4 BBBBBBBBB 5
                         BBBBBBB   6
                            7
    */


    if ( search ) {
//qDebug("Search for %d,%d",x,y);
	// binary search for rectangle which is before (x,y)
	int a=0;
	int l=ncliprect-1;
	int h;
	int m=-1;
	while ( l>0 ) {
	    h = l/2;
	    m = a + h;
//	    qDebug("l = %d, m = %d", l, m);
	    const QRect& r = cliprect[m];
	    if ( r.bottom() < y || r.top() <= y && r.right() < x ) {
		// m is before (x,y)
		a = m + 1;
		l = l - h - 1;
	    } else
		l = h;
	}
	// Rectangle "a" is the rectangle containing (x,y), or the
	// closest rectangle to the right of (x,y).
	clipcursor = a;
	cursorRect = &cliprect[clipcursor];
	if ( cursorRect->contains(x,y) ) {
	    // PLACE 0
//qDebug("found %d,%d in %d[%d..%d,%d..%d]",x,y,clipcursor,cliprect[clipcursor].left(),cliprect[clipcursor].right(),cliprect[clipcursor].top(),cliprect[clipcursor].bottom());
	    if ( cr )
		*cr = *cursorRect;
// qDebug("Found %d,%d in %d[%d..%d,%d..%d]",x,y,clipcursor,cliprect[clipcursor].left(),cliprect[clipcursor].right(),cliprect[clipcursor].top(),cliprect[clipcursor].bottom());
	    return TRUE;
	}
//qDebug("!found %d,%d in %d[%d..%d,%d..%d]",x,y,clipcursor,cliprect[clipcursor].left(),cliprect[clipcursor].right(),cliprect[clipcursor].top(),cliprect[clipcursor].bottom());
    }

    // At this point, (x,y) is outside the clip region and clipcursor is
    // the rectangle to the right/below of (x,y), or the last rectangle.

    if ( cr ) {
	const QRect &tcr = *cursorRect;
	if ( y < tcr.top() && clipcursor == 0) {
	    // PLACE 1
//qDebug("PLACE 1");
	    cr->setCoords( x-4000,y-4000,x+4000,tcr.top()-1 );
	} else if ( clipcursor == (int)ncliprect-1 && y>tcr.bottom() ) {
	    // PLACE 7
//qDebug("PLACE 7");
	    cr->setCoords( x-4000,tcr.bottom()+1,x+4000,y+4000 );
	} else if ( clipcursor == (int)ncliprect-1 && x > tcr.right() ) {
	    // PLACE 6
//qDebug("PLACE 6");
	    cr->setCoords( tcr.right()+1,tcr.top(),x+4000,y+4000 );
	} else if ( clipcursor == 0 ) {
	    // PLACE 2
//qDebug("PLACE 2");
	    cr->setCoords( x-4000,y-4000,tcr.left()-1,tcr.bottom() );
	} else {
	    const QRect &prev_tcr = *(cursorRect-1);
	    if ( prev_tcr.bottom() < y && tcr.top() > y) {
		// found a new place
//qDebug("PLACE new");
		cr->setCoords( x-4000,prev_tcr.bottom()+1, x+4000,tcr.top()-1 );
	    } else if ( prev_tcr.bottom() < y && tcr.left() > x) {
		// PLACE 3
//qDebug("PLACE 3");
		cr->setCoords( x-4000,tcr.top(), tcr.left()-1,tcr.bottom() );
	    } else {
		if ( prev_tcr.y() == tcr.y() ) {
		    // PLACE 4
//qDebug("PLACE 4");
		    cr->setCoords( prev_tcr.right()+1, tcr.y(),
				       tcr.left()-1, tcr.bottom() );
		} else {
		    // PLACE 5
//qDebug("PLACE 5");
		    cr->setCoords( prev_tcr.right()+1, prev_tcr.y(),
				       prev_tcr.right()+4000, prev_tcr.bottom() );
		}
	    }
	}
    }

//qDebug("!found %d,%d in %d[%d..%d,%d..%d] nor [%d..%d,%d..%d]",x,y, clipcursor, cliprect[clipcursor].left(),cliprect[clipcursor].right(),cliprect[clipcursor].top(),cliprect[clipcursor].bottom(), cr->left(),cr->right(),cr->top(),cr->bottom());
    return FALSE;
}

/*!
  \internal

  This corresponds to QPainter::setBrush, and sets the gfx's brush to
  \a b.
*/

void QGfxRasterBase::setBrush( const QBrush & b )
{
    cbrush=b;
    if((cbrush.style()!=NoBrush) && (cbrush.style()!=SolidPattern)) {
	patternedbrush=TRUE;
    } else {
	patternedbrush=FALSE;
    }
#ifndef QT_NO_QWS_REPEATER
    if ( is_screen_gfx && qt_screen != gfx_screen ) {
	QScreen * tmp=qt_screen;
	qt_screen=gfx_screen;
	QColor tmpcol=b.color();
	srccol=tmpcol.alloc();
	qt_screen=tmp;
    } else
#endif
	srccol=b.color().pixel();


}

/*!
  \internal

  This sets the offset of a pattern when drawing with a patterned
  brush to (\a x, \a y). This is needed when clipping means the start
  position for drawing doesn't correspond with the start position
  requested by QPainter, for example.
*/

void QGfxRasterBase::setBrushOffset( int x, int y )
{
    brushoffs = QPoint( x, y );
}

/*!
  \internal

  This tells blt()s that instead of image data a single solid value
  should be used as the source, taken from the current pen color. You
  could reproduce a fillRect() using a pen source and the IgnoreAlpha
  alpha type, but this would be both pointless and slower than
  fillRect; its normal use is for anti-aliased text, where the text
  color is that of the pen and a separate alpha channel produces the
  shape of the glyphs.
*/

void QGfxRasterBase::setSourcePen()
{
#ifndef QT_NO_QWS_REPEATER
    if ( is_screen_gfx && qt_screen != gfx_screen ) {
	QScreen * tmp=qt_screen;
	qt_screen=gfx_screen;
	QColor tmpcol=cpen.color();
	srccol = tmpcol.alloc();
	qt_screen=tmp;
    } else
#endif
	srccol = cpen.color().pixel();

    src_normal_palette=TRUE;
    srctype=SourcePen;
    setSourceWidgetOffset( 0, 0 );
}

/*!
  \fn QGfxRasterBase::get_value_32( int sdepth, unsigned char **srcdata,
				    bool reverse )

  \internal

  This converts a pixel in an arbitrary source depth (specified by \a
  sdepth, stored at *(*\a srcdata) to a 32 bit value; it's used by
  blt() where the source depth is less than 32 bits and the
  destination depth is 32 bits. *srcdata (the pointer to the data) is
  auto-incremented by the appropriate number of bytes, or decremented
  if \a reverse is true. If the source has a pixel size of less than
  a byte then auto-incrementing or decrementing will happen as
  necessary; the current position within the byte is stored in
  monobitcount (bit within the byte) and monobitval (value of the
  current byte). In the case of 8-bit source data lookups on the
  source's color table are performed.
*/

GFX_INLINE unsigned int QGfxRasterBase::get_value_32(
		       int sdepth, unsigned char **srcdata, bool reverse)
{
// Convert between pixel values for different depths
// reverse can only be true if sdepth == depth
    unsigned int ret;
    if(sdepth==32) {
	ret = *((unsigned int *)(*srcdata));
	if(reverse) {
	    (*srcdata)-=4;
	} else {
	    (*srcdata)+=4;
	}
#if !defined( QT_NO_QWS_DEPTH_24 )
    } else if(sdepth==24) {
	ret = gfxGetRgb24( *srcdata );
	(*srcdata) += 3;
#endif
#if !defined( QT_NO_IMAGE_16_BIT ) || !defined( QT_NO_QWS_DEPTH_16 )
    } else if(sdepth==16) {
	unsigned short int hold=*((unsigned short int *)(*srcdata));
	ret = qt_conv16ToRgb(hold);
	(*srcdata)+=2;
#endif
    } else if(sdepth==8) {
	unsigned char val=*((*srcdata));
	if(src_normal_palette) {
	    ret=((val >> 5) << 16)  | ((val >> 6) << 8) | (val >> 5);
	} else {
	    ret = srcclut[val];
	}
	(*srcdata)++;
    } else if(sdepth==1) {
	if(monobitcount<8) {
	    monobitcount++;
	} else {
	    monobitcount=1;	// yes, 1 is correct
	    (*srcdata)++;
	    monobitval=*((*srcdata));
	}
	if(src_little_endian) {
	    ret=monobitval & 0x1;
	    monobitval=monobitval >> 1;
	} else {
	    ret=(monobitval & 0x80) >> 7;
	    monobitval=monobitval << 1;
	    monobitval=monobitval & 0xff;
	}
	ret=srcclut[ret];
    } else {
	qDebug("Odd source depth %d!",sdepth);
	ret=0;
    }
#ifndef QT_NO_QWS_DEPTH_32_BGR
	if ( pixeltype != srcpixeltype ) {
	    ret = (ret&0x0000ff)<<16 | (ret&0xff00ff00) | (ret&0xff0000)>>16;
	}
#endif

    return ret;
}

/*!
  \fn QGfxRasterBase::get_value_24( int sdepth, unsigned char **srcdata,
				    bool reverse )

  \internal

  This converts a pixel in an arbitrary source depth (specified by \a
  sdepth, stored at *(*\a srcdata) to a 24 bit value; it's used by
  blt() where the source depth is less than 24 bits and the
  destination depth is 24 bits. *srcdata (the pointer to the data) is
  auto-incremented by the appropriate number of bytes, or decremented
  if \a reverse is true. If the source has a pixel size of less than
  a byte then auto-incrementing or decrementing will happen as
  necessary; the current position within the byte is stored in
  monobitcount (bit within the byte) and monobitval (value of the
  current byte). In the case of 8-bit source data lookups on the
  source's color table are performed.

  \sa get_value_32()
*/

GFX_INLINE unsigned int QGfxRasterBase::get_value_24(
		       int sdepth, unsigned char **srcdata, bool reverse)
{
    unsigned int ret;
    if ( sdepth == 24 ) {
	ret = gfxGetRgb24( *srcdata );
	if ( reverse )
	    (*srcdata)-=3;
	else
	    (*srcdata)+=3;
    } else {
	ret = get_value_32( sdepth, srcdata, reverse );
    }

    return ret;
}

/*! \fn QGfxRasterBase::get_value_16(int sdepth, unsigned char **srcdata,
        bool reverse)

  \internal

  This converts a pixel in an arbitrary source depth (specified by \a
  sdepth, stored at *(*\a srcdata) to a 16 bit value; it's used by
  blt() where the source depth is less than 16 bits and the
  destination depth is 16 bits. *srcdata (the pointer to the data) is
  auto-incremented by the appropriate number of bytes, or decremented
  if \a reverse is true. If the source has a pixel size of less than
  a byte then auto-incrementing or decrementing will happen as
  necessary; the current position within the byte is stored in
  monobitcount (bit within the byte) and monobitval (value of the
  current byte). In the case of 8-bit source data lookups on the
  source's color table are performed.

  \sa get_value_32()
*/

// reverse can only be true if sdepth == depth
GFX_INLINE unsigned int QGfxRasterBase::get_value_16(
		       int sdepth, unsigned char **srcdata, bool reverse)
{
#if !defined( QT_NO_IMAGE_16_BIT ) || !defined( QT_NO_QWS_DEPTH_16 )
    unsigned int ret = 0;
    if ( sdepth == 16 ) {
	unsigned short int hold = *((unsigned short int *)(*srcdata));
	if(reverse) {
	    (*srcdata)-=2;
	} else {
	    (*srcdata)+=2;
	}
	ret=hold;
    } else if(sdepth==8) {
	unsigned char val=*((*srcdata));
	QRgb hold;
	if(src_normal_palette) {
	    hold = val*0x010101;
	} else {
	    hold=srcclut[val];
	}
	ret=qt_convRgbTo16(hold);
	(*srcdata)++;
    } else if(sdepth==1) {
	if(monobitcount<8) {
	    monobitcount++;
	} else {
	    monobitcount=1;
	    (*srcdata)++;
	    monobitval=*((*srcdata));
	}
	if(src_little_endian) {
	    ret=monobitval & 0x1;
	    monobitval=monobitval >> 1;
	} else {
	    ret=(monobitval & 0x80) >> 7;
	    monobitval=monobitval << 1;
	    monobitval=monobitval & 0xff;
	}
	ret=srcclut[ret];
    } else if ( sdepth == 32 ) {
	unsigned int hold = *((unsigned int *)(*srcdata));
	ret=qt_convRgbTo16(hold);
	(*srcdata)+=4;
    } else {
	qDebug("Odd source depth %d!",sdepth);
	abort();
	ret=0;
    }

    return ret;
#endif
}

/*! \fn QGfxRasterBase::get_value_8(
        int sdepth, unsigned char **srcdata, bool reverse)

\internal

This is similar to get_value_32(), but returns 8-bit values. Translation
between different color palettes and from 32/24/16 bit data to the nearest
match in the destination's color palette is performed.

This converts a pixel in an arbitrary source depth (specified by \a sdepth,
stored at *(*\a srcdata) to a 8 bit value; it's used by blt() where the
source depth is less than 8 bits and the destination depth is 8 bits.
*srcdata (the pointer to the data) is auto-incremented by the appropriate
number of bytes, or decremented if \a reverse is true. If the source has
a pixel size of less than a byte then auto-incrementing or decrementing
will happen as necessary; the current position within the byte is stored in
monobitcount (bit within the byte) and monobitval (value of the current
byte). In the case of 8-bit source data lookups on the source's color
table are performed.
*/

// reverse can only be true if sdepth == depth
GFX_INLINE unsigned int QGfxRasterBase::get_value_8(
		       int sdepth, unsigned char **srcdata, bool reverse)
{
    unsigned int ret;

    if(sdepth==8) {
	unsigned char val=*((unsigned char *)(*srcdata));
	// If source!=QImage, then the palettes will be the same
	if(src_normal_palette) {
	    ret=val;
	} else {
	    ret=transclut[val];
	}
	if(reverse) {
	    (*srcdata)--;
	} else {
	    (*srcdata)++;
	}
    } else if(sdepth==1) {
	if(monobitcount<8) {
	    monobitcount++;
	} else {
	    monobitcount=1;
	    (*srcdata)++;
	    monobitval=*((*srcdata));
	}
	if(src_little_endian) {
	    ret=monobitval & 0x1;
	    monobitval=monobitval >> 1;
	} else {
	    ret=(monobitval & 0x80) >> 7;
	    monobitval=monobitval << 1;
	    monobitval=monobitval & 0xff;
	}
	ret = transclut[ret];
    } else if(sdepth==32) {
	unsigned int r,g,b;
	unsigned int hold=*((unsigned int *)(*srcdata));
	r=(hold & 0xff0000) >> 16;
	g=(hold & 0x00ff00) >> 8;
	b=(hold & 0x0000ff);
	simple_8bpp_alloc=TRUE;
	ret = GFX_8BPP_PIXEL(r,g,b);
	simple_8bpp_alloc=FALSE;
	(*srcdata)+=4;
    } else if(sdepth==16) {
	unsigned int r,g,b;
	unsigned short int hold=*((unsigned short int *)(*srcdata));
	r=((hold & (0x1f << 11)) >> 11) << 3;
	g=((hold & (0x3f << 5)) >> 5) << 2;
	b=(hold & 0x1f) << 3;
	simple_8bpp_alloc=TRUE;
	ret = GFX_8BPP_PIXEL(r,g,b);
	simple_8bpp_alloc=FALSE;
	(*srcdata)+=2;
    } else if ( sdepth == 4 ) {
	ret = monobitval & 0x0f;
	if ( !monobitcount ) {
	    monobitcount = 1;
	    monobitval >>= 4;
	} else {
	    monobitcount = 0;
	    (*srcdata)++;
	    monobitval = *(*srcdata);
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( !src_little_endian )
		monobitval = (monobitval & 0x0f) << 4 |  (monobitval & 0xf0) >> 4;
#endif
	}
    } else {
	qDebug("Cannot do %d->8!",sdepth);
	ret=0;
    }

    return ret;
}

/*!
\fn unsigned int QGfxRasterBase::get_value_4(int sdepth, unsigned char **srcdata, bool reverse)

\internal

This is similar to get_value_8, but returns 4-bit values.

This converts a pixel in an arbitrary source depth (specified by \a sdepth,
stored at *(*\a srcdata) to a 4 bit value; it's used by blt() where the
source depth is less than 4 bits and the destination depth is 4 bits.
*srcdata (the pointer to the data) is auto-incremented by the appropriate
number of bytes, or decremented if \a reverse is true. If the source has
a pixel size of less than a byte then auto-incrementing or decrementing
will happen as necessary; the current position within the byte is stored in
monobitcount (bit within the byte) and monobitval (value of the current
byte). In the case of 8-bit source data lookups on the source's color
table are performed.
*/

// reverse can only be true if sdepth == depth
GFX_INLINE unsigned int QGfxRasterBase::get_value_4(
		       int sdepth, unsigned char **srcdata, bool reverse)
{
    unsigned int ret;

    if ( sdepth == 4 ) {
	if ( reverse ) {
	    ret = (monobitval & 0xf0) >> 4;
	    if ( !monobitcount ) {
		monobitcount = 1;
		monobitval <<= 4;
	    } else {
		monobitcount = 0;
		(*srcdata)--;
		monobitval = *(*srcdata);
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
		if ( !src_little_endian )
		    monobitval = (monobitval & 0x0f) << 4 |  (monobitval & 0xf0) >> 4;
#endif
	    }
	} else {
	    ret = monobitval & 0x0f;
	    if ( !monobitcount ) {
		monobitcount = 1;
		monobitval >>= 4;
	    } else {
		monobitcount = 0;
		(*srcdata)++;
		monobitval = *(*srcdata);
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
		if ( !src_little_endian )
		    monobitval = (monobitval & 0x0f) << 4 |  (monobitval & 0xf0) >> 4;
#endif
	    }
	}
    } else if ( sdepth == 1 ) {
	if(monobitcount<8) {
	    monobitcount++;
	} else {
	    monobitcount=1;
	    (*srcdata)++;
	    monobitval=*((*srcdata));
	}
	if(src_little_endian) {
	    ret=monobitval & 0x1;
	    monobitval=monobitval >> 1;
	} else {
	    ret=(monobitval & 0x80) >> 7;
	    monobitval=monobitval << 1;
	    monobitval=monobitval & 0xff;
	}
	ret = transclut[ret];
    } else if ( sdepth == 8 ) {
	unsigned char val=*((unsigned char *)(*srcdata));
	ret = transclut[val];
	if(reverse)
	    (*srcdata)--;
	else
	    (*srcdata)++;
    } else if(sdepth==32) {
	unsigned int r,g,b;
	unsigned int hold=*((unsigned int *)(*srcdata));
	r=(hold & 0xff0000) >> 16;
	g=(hold & 0x00ff00) >> 8;
	b=(hold & 0x0000ff);
	ret = qGray( r, g, b ) >> 4;
	(*srcdata)+=4;
    } else {
	qDebug("Cannot do %d->4!",sdepth);
	ret=0;
    }

    return ret;
}

/*!
\fn unsigned int QGfxRasterBase::get_value_1(int sdepth, unsigned char **srcdata, bool reverse)

\internal

This is similar to get_value_8, but returns 1-bit values. The number of depths
that can be blt'd to a monochrome destination are limited - only monochrome
or 32-bit sources are permitted.

This converts a pixel in an arbitrary source depth (specified by \a sdepth,
stored at *(*\a srcdata) to a 1 bit value; it's used by blt() where the
source depth is less than 1 bit and the destination depth is 1 bit.
*srcdata (the pointer to the data) is auto-incremented by the appropriate
number of bytes, or decremented if \a reverse is true. If the source has
a pixel size of less than a byte then auto-incrementing or decrementing
will happen as necessary; the current position within the byte is stored in
monobitcount (bit within the byte) and monobitval (value of the current
byte). In the case of 8-bit source data lookups on the source's color
table are performed.
*/

// reverse can only be true if sdepth == depth
GFX_INLINE unsigned int QGfxRasterBase::get_value_1(
		       int sdepth, unsigned char **srcdata, bool reverse)
{
    unsigned int ret;

    if(sdepth==1) {
	if ( reverse ) {
	    if(monobitcount<8) {
		monobitcount++;
	    } else {
		monobitcount=1;
		(*srcdata)--;
		monobitval=**srcdata;
	    }
	    if(src_little_endian) {
		ret = ( monobitval & 0x80 ) >> 7;
		monobitval=monobitval << 1;
		monobitval=monobitval & 0xff;
	    } else {
		ret=monobitval & 0x1;
		monobitval=monobitval >> 1;
	    }
	} else {
	    if(monobitcount<8) {
		monobitcount++;
	    } else {
		monobitcount=1;
		(*srcdata)++;
		monobitval=**srcdata;
	    }
	    if(src_little_endian) {
		ret=monobitval & 0x1;
		monobitval=monobitval >> 1;
	    } else {
		ret = ( monobitval & 0x80 ) >> 7;
		monobitval=monobitval << 1;
		monobitval=monobitval & 0xff;
	    }
	}
    } else if(sdepth==32) {
	unsigned int hold=*((unsigned int *)(*srcdata));
	unsigned int r,g,b;
	r=(hold & 0xff0000) >> 16;
	g=(hold & 0x00ff00) >> 8;
	b=(hold & 0x0000ff);
	(*srcdata)+=4;
	simple_8bpp_alloc=TRUE;
	ret = GFX_8BPP_PIXEL(r,g,b);
	simple_8bpp_alloc=FALSE;
    } else {
	qDebug("get_value_1(): Unsupported source depth %d!",sdepth);
	ret=0;
    }

    return ret;
}


/*!
 \class QGfxRaster qgfxraster_qws.h
  \ingroup qws
 \brief The QGfxRaster class is QGfxRasterBase specialized for a particular bit
 depth, specified by the depth parameter of the template. The type field
 is currently not used. In future versions, it may be used to specify the
 pixel storage format.

 \internal (for now)

 Many operations with QGfxRaster are specified along the lines of

 if(depth==32) {
   ...
 } else if(depth==16) {
   ...

 The intention is that the compiler will realise when instantiating a template
 for a particular depth that it need only include the code for that depth -
 so if you never use an 8-bit gfx, for example, the 8-bit code will not
 be included in your executable. The actual drawing code for software-only
 rendering is all included in this class; it should be subclassed by hardware
 drivers so that they can fall back to it for anything that hardware can't
 handle.
*/

/*! \fn QGfxRaster::QGfxRaster(unsigned char * b,int w,int h)

  \internal

  Constructs a QGfxRaster for a particular depth with a framebuffer pointed
  to by \a b, with a width and height of \a w and \a h (specified in
  pixels, not bytes)
*/

template <const int depth, const int type>
QGfxRaster<depth,type>::QGfxRaster(unsigned char * b,int w,int h)
    : QGfxRasterBase(b,w,h)
{
    setLineStep((depth*width+7)/8);
    if ( depth == 1 ) {
	setPen( color1 );
	setBrush( color0 );
    }
}

/*! \fn QGfxRaster::~QGfxRaster()

  \internal

  Destroys a QGfxRaster.
*/

template <const int depth, const int type>
QGfxRaster<depth,type>::~QGfxRaster()
{
}


/*!
  \fn void QGfxRaster<depth,type>::calcPacking(
			  void * m,int x1,int x2,
			  int & frontadd,int & backadd,int & count)

  \internal

  This is an internal method used by methods which pack writes to the
  framebuffer for optimisation reasons. It takes longer to write
  80 8-bit values over the PCI or AGP bus to a graphics card than
  it does 20 32-bit values, as long as those 32-bit values are aligned
  on a 32-bit boundary. Therefore the code writes individual pixels
  up to a boundary, writes 32-bit values until it reaches the last boundary
  before the end of the line, and draws then individual pixels again.
  Given a pointer to a start of the line within the framebuffer \a m
  and starting and ending x coordinates \a x1 and \a x2, \a frontadd is filled
  with the number of individual pixels to write at the start of the line,
  \a count with the number of 32-bit values to write and \a backadd with the
  number of individual pixels to write at the end. This optimisation
  yields up to 60% drawing speed performance improvements when Memory
  Type Range Registers are not available, and still gives a few percent
  when write-combining on the framebuffer is enabled using those registers
  (which effectively causes the CPU to do a similar optimisation in hardware)
*/

// Calculate packing values for 32-bit writes

template<const int depth,const int type>
GFX_INLINE void QGfxRaster<depth,type>::calcPacking(
			  void * m,int x1,int x2,
			  int & frontadd,int & backadd,int & count)
{
    int w = x2-x1+1;

#ifndef QWS_NO_WRITE_PACKING
    if ( depth == 16 ) {
	if ( w < 2 )
	    goto unpacked;

	unsigned short int * myptr=(unsigned short int *)m;
	frontadd=(((unsigned long)myptr)+(x1*2)) & 0x3;
	backadd=(((unsigned long)myptr)+((x2+1)*2)) & 0x3;
	if ( frontadd )
	    frontadd = 4 - frontadd;
	frontadd >>= 1;
	backadd >>= 1;
	count=( w-(frontadd+backadd) );
	count >>= 1;
    } else if ( depth == 8 ) {
	if ( w < 4 )
	    goto unpacked;

	unsigned char * myptr=(unsigned char *)m;
	frontadd=(((unsigned long)myptr)+x1) & 0x3;
	backadd=(((unsigned long)myptr)+x2+1) & 0x3;
	if ( frontadd )
	    frontadd = 4 - frontadd;
	count = w-(frontadd+backadd);
	count >>= 2;
    } else {
	goto unpacked;
    }

    if(count<0)
	count=0;
    if(frontadd<0)
	frontadd=0;
    if(backadd<0)
	backadd=0;
    return;
#endif

unpacked:
    frontadd = w;
    backadd = 0;
    count = 0;
    if(frontadd<0)
	frontadd=0;
}

/*!
\fn void QGfxRaster<depth,type>::setSource(const QPaintDevice * p)

\internal
\overload

This sets the gfx to use an arbitrary paintdevice \a p as the source for
future data. It sets up a default alpha-blending value of IgnoreAlpha.
*/

// if the source is 1bpp, the pen and brush currently active will be used
template <const int depth, const int type>
void QGfxRaster<depth,type>::setSource(const QPaintDevice * p)
{
    QPaintDeviceMetrics qpdm(p);
    srclinestep=((QPaintDevice *)p)->bytesPerLine();
    srcdepth=qpdm.depth();
    if(srcdepth==0)
	abort();
    srcbits=((QPaintDevice *)p)->scanLine(0);
    srctype=SourceImage;
    srcpixeltype=NormalPixel;
    setAlphaType(IgnoreAlpha);
    if ( p->devType() == QInternal::Widget ) {
	QWidget * w=(QWidget *)p;
	srcwidth=w->width();
	srcheight=w->height();
	QPoint hold;
	hold=w->mapToGlobal(hold);
	setSourceWidgetOffset( hold.x(), hold.y() );
	if ( srcdepth == 1 ) {
	    buildSourceClut(0, 0);
	} else if(srcdepth <= 8) {
	    src_normal_palette=TRUE;
	}
    } else if ( p->devType() == QInternal::Pixmap ) {
	//still a bit ugly
	QPixmap *pix = (QPixmap*)p;
	srcwidth=pix->width();
	srcheight=pix->height();
	setSourceWidgetOffset( 0, 0 );
	if ( srcdepth == 1 ) {
	    buildSourceClut(0, 0);
	} else if(srcdepth <= 8) {
	    src_normal_palette=TRUE;
	}
    } else {
	// This is a bit ugly #### I'll say!
	//### We will have to find another way to do this
	setSourceWidgetOffset( 0, 0 );
	buildSourceClut(0,0);
    }

    src_little_endian=TRUE;
}

/*!
\fn void QGfxRaster<depth,type>::setSource(const QImage * i)

\internal

This sets up future blt's to use a QImage \a i as a source - used by
QPainter::drawImage()
*/

template <const int depth, const int type>
void QGfxRaster<depth,type>::setSource(const QImage * i)
{
    srctype=SourceImage;
    srcpixeltype=NormalPixel;
    srclinestep=i->bytesPerLine();
    srcdepth=i->depth();
    if(srcdepth==0)
	abort();
    srcbits=i->scanLine(0);
    src_little_endian=(i->bitOrder()==QImage::LittleEndian);
    QSize s = gfx_screen->mapToDevice( QSize(i->width(), i->height()) );
    srcwidth=s.width();
    srcheight=s.height();
    setSourceWidgetOffset( 0, 0 );
    src_normal_palette=FALSE;
    if ( srcdepth == 1 )
	buildSourceClut( 0, 0 );
    else  if(srcdepth<=8)
	buildSourceClut(i->colorTable(),i->numColors());
}

template <const int depth, const int type>
void QGfxRaster<depth,type>::setSource(unsigned char * c,int w,int h,int l,
				       int d,QRgb * clut,int numcols)
{
    srctype=SourceImage;
    srcpixeltype=NormalPixel;
    srclinestep=l;
    srcdepth=d;
    srcbits=c;
    src_little_endian=TRUE;
    setSourceWidgetOffset(0,0);
    srcwidth=w;
    srcheight=h;
    src_normal_palette=FALSE;
    if ( srcdepth == 1 )
	buildSourceClut( 0, 0 );
    else  if(srcdepth<=8)
	buildSourceClut(clut,numcols);
}

/*!
\fn void QGfxRaster<depth,type>::buildSourceClut(QRgb * cols,int numcols)

\internal

This is an internal method used to optimise blt's from paletted to paletted
data, where the palettes are different for each. A lookup table
indexed by the source value providing the destination value is
filled in. \a cols is the source data's color lookup table,
\a numcols the number of entries in it. If \a cols is 0 some default
values are put in (this is for 1bpp sources which don't have
a palette).
*/

template <const int depth, const int type>
void QGfxRaster<depth,type>::buildSourceClut(QRgb * cols,int numcols)
{
    if (!cols) {
	useBrush();
#if !defined( QT_NO_IMAGE_16_BIT ) || !defined( QT_NO_QWS_DEPTH_16 )
	if ( qt_screen->depth() == 16 && depth==32 ) {
	    srcclut[0]=qt_conv16ToRgb(pixel);
	    transclut[0]=qt_conv16ToRgb(pixel);
	} else
#endif
	{
	    srcclut[0]=pixel;
	    transclut[0]=pixel;
	}
	usePen();
#if !defined( QT_NO_IMAGE_16_BIT ) || !defined( QT_NO_QWS_DEPTH_16 )
	if ( qt_screen->depth() == 16 && depth==32 ) {
	    srcclut[1]=qt_conv16ToRgb(pixel);
	    transclut[1]=qt_conv16ToRgb(pixel);
	} else
#endif
	{
	    srcclut[1]=pixel;
	    transclut[1]=pixel;
	}
	return;
    }

    int loopc;

    // Copy clut
    for(loopc=0;loopc<numcols;loopc++)
	srcclut[loopc] = cols[loopc];

    if(depth<=8) {
	// Now look for matches
	for(loopc=0;loopc<numcols;loopc++) {
	    int r = qRed(srcclut[loopc]);
	    int g = qGreen(srcclut[loopc]);
	    int b = qBlue(srcclut[loopc]);
	    transclut[loopc] = gfx_screen->alloc(r,g,b);
	}
    }
}

/*!
  \fn void QGfxRaster<depth,type>::drawPointUnclipped( int x, unsigned char* l)

\internal

This draws a point in the scanline pointed to by \a l, at the position \a x,
without taking any notice of clipping. It's an internal method called
by drawPoint()
*/

//screen coordinates
template <const int depth, const int type>
GFX_INLINE void QGfxRaster<depth,type>::drawPointUnclipped( int x, unsigned char* l)
{
    if( (myrop!=XorROP) && (myrop!=NotROP) ) {
	if ( depth == 32 )
	    ((QRgb*)l)[x] = pixel;
	else if ( depth == 24 )
	    gfxSetRgb24( l + x*3, pixel );
	else if ( depth == 16 )
	    ((ushort*)l)[x] = (pixel & 0xffff);
	else if ( depth == 8 )
	    l[x] = (pixel & 0xff);
	else if ( depth == 4 ) {
	    uchar *d = l + (x>>1);
	    int s = (x & 1) << 2;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx )
	        s = (~x & 1) << 2;
#endif
	    *d = ( *d & MASK4BPP(s) ) | (pixel << s);
	} else if ( depth == 1 )
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx ) {
	    if ( pixel )
		l[x/8] |= 0x80 >> (x%8);
	    else
		l[x/8] &= ~(0x80 >> (x%8));
	    } else
#endif
	    if ( pixel )
		l[x/8] |= 1 << (x%8);
	    else
		l[x/8] &= ~(1 << (x%8));
    } else if(myrop==XorROP) {
	if ( depth == 32 )
	    ((QRgb*)l)[x] = ((QRgb*)l)[x] ^ pixel;
	else if ( depth == 24 ) {
	    unsigned char *ptr = l + x*3;
	    unsigned int s = gfxGetRgb24( ptr );
	    gfxSetRgb24( ptr, s ^ pixel );
	} else if ( depth == 16 )
	    ((ushort*)l)[x] = ((ushort*)l)[x] ^ pixel;
	else if ( depth == 8 )
	    l[x] = l[x] ^ pixel;
	else if ( depth == 4 ) {
	    uchar *d = l + (x>>1);
	    int s = (x & 1) << 2;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx )
	        s = (~x & 1) << 2;
#endif
	    unsigned char p = *d;
	    unsigned char e = ( p & MASK4BPP(s) ) |
			      ( (p ^ (pixel << s)) & MASK4BPP(4-s) );
	    *d = e;
	} else if ( depth == 1 )
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx ) {
	    if ( pixel )
		l[x/8] |= 0x80 >> (x%8);
	    else
		l[x/8] &= ~(0x80 >> (x%8));
	    } else
#endif
	    if ( pixel )
		l[x/8] |= 1 << (x%8);
	    else
		l[x/8] &= ~(1 << (x%8));
    } else if(myrop==NotROP) {
	if ( depth == 32 )
	    ((QRgb*)l)[x] = ~(((QRgb*)l)[x]);
	else if ( depth == 24 ) {
	    unsigned char *ptr = l + x*3;
	    unsigned int s = gfxGetRgb24( ptr );
	    gfxSetRgb24( ptr, ~s );
	} else if ( depth == 16 )
	    ((ushort*)l)[x] = ~(((ushort*)l)[x]);
	else if ( depth == 8 )
	    l[x] = ~(l[x]);
	else if ( depth == 4 ) {
	    uchar *d = l + (x>>1);
	    int s = (x & 1) << 2;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx )
	        s = (~x & 1) << 2;
#endif
	    unsigned char p = *d;
	    *d = (p & MASK4BPP(s)) | ((~p) & MASK4BPP(4-s));
	} else if ( depth == 1 )
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx ) {
	    if ( pixel )
		l[x/8] |= 0x80 >> (x%8);
	    else
		l[x/8] &= ~(0x80 >> (x%8));
	    } else
#endif
	    if ( pixel )
		l[x/8] |= 1 << (x%8);
	    else
		l[x/8] &= ~(1 << (x%8));
    } else {
	// ...
    }
}

/*!
\fn void QGfxRaster<depth,type>::drawPoint( int x, int y )

\internal

Draw a point at \a x, \a y in the current pen color. As with most
externally-called dawing methods x and y are relevant to the current gfx
offset, stored in the variables xoffs and yoffs.
*/

template <const int depth, const int type>
void QGfxRaster<depth,type>::drawPoint( int x, int y )
{
    if (!ncliprect)
	return;
    if(cpen.style()==NoPen)
	return;
    x += xoffs;
    y += yoffs;

    GFX_START(QRect(x,y,2,2))
    if((*gfx_optype))
	sync();
    (*gfx_optype)=0;
        usePen();
    if (inClip(x,y)) {
	drawPointUnclipped( x, scanLine(y) );
    }
    GFX_END
}

/*!
  \fn void QGfxRaster<depth,type>::drawPoints( const QPointArray & pa, int index, int npoints )

  \internal

  Draw \a npoints points from position \a index in the array of points \a pa.
*/

template <const int depth, const int type>
void QGfxRaster<depth,type>::drawPoints( const QPointArray & pa, int index, int npoints )
{
    if (!ncliprect)
	return;
    if(cpen.style()==NoPen)
	return;
    usePen();
    QRect cr;
    bool in = FALSE;
    bool foundone=( ((*gfx_optype)==0) ? TRUE : FALSE );

    GFX_START(clipbounds);
    while (npoints--) {
	int x = pa[index].x() + xoffs;
	int y = pa[index].y() + yoffs;
	if ( !cr.contains(x,y) ) {
	    in = inClip(x,y,&cr);
	}
	if ( in ) {
	    if(foundone==FALSE) {
		sync();
		(*gfx_optype)=0;
		foundone=TRUE;
	    }
	    drawPointUnclipped( x, scanLine(y) );
	}
	++index;
    }
    GFX_END
}

/*!
\fn void QGfxRaster<depth,type>::drawLine( int x1, int y1, int x2, int y2 )

\internal

Draw a line in the current pen style from \a x1 \a y1 to \a x2 \a y2
*/

template <const int depth, const int type>
void QGfxRaster<depth,type>::drawLine( int x1, int y1, int x2, int y2 )
{
    if (!ncliprect)
	return;
    if (cpen.style()==NoPen)
	return;

    if (cpen.width() > 1) {
	drawThickLine( x1, y1, x2, y2 );
	return;
    }

    usePen();
    x1+=xoffs;
    y1+=yoffs;
    x2+=xoffs;
    y2+=yoffs;

    if(x1>x2) {
	int x3;
	int y3;
	x3=x2;
	y3=y2;
	x2=x1;
	y2=y1;
	x1=x3;
	y1=y3;
#ifdef GFX_CORRECT_POLYLINE_JOIN
	if ( gfx_noLineOverwrite ) {
	    if ( x2-x1 > QABS(y2-y1) )
		gfx_storedLineRead -= x2-x1;
	    else
		gfx_storedLineRead -= QABS(y2-y1);
	    gfx_storedLineDir = -gfx_storedLineDir;
	}
#endif
    }

    int dx=x2-x1;
    int dy=y2-y1;

    GFX_START(QRect(x1, y1 < y2 ? y1 : y2, dx+1, QABS(dy)+1))

    if((*gfx_optype))
    sync();
    (*gfx_optype)=0;

#ifdef QWS_EXPERIMENTAL_FASTPATH
    // Fast path
    if ( !dashedLines && !gfx_storeLine ) {
	if ( y1 == y2 ) {
	    if ( ncliprect == 1) {
		if ( x1 > cliprect[0].right() || x2 < cliprect[0].left()
			|| y1 < cliprect[0].top() || y1 > cliprect[0].bottom() ) {
		    GFX_END
		    return;
		}
		x1 = x1 > cliprect[0].left() ? x1 : cliprect[0].left();
		x2 = x2 > cliprect[0].right() ? cliprect[0].right() : x2;
		unsigned char *l = scanLine(y1);
		hlineUnclipped(x1,x2,l);
	    } else {
		hline( x1, x2, y1 );
	    }
	    GFX_END
	    return;
	}
# if !defined(_OS_QNX6_)
	else if ( x1 == x2 ) {
	    vline( x1, y1, y2 );
	    GFX_END
	    return;
	}
# endif
    }
#endif
    // Bresenham algorithm from Graphics Gems

    int ax=QABS(dx)*2;
    int ay=QABS(dy)*2;
    int sy=dy>0 ? 1 : -1;
    int x=x1;
    int y=y1;

    int d;
    bool doDraw = TRUE;

    QRect cr;
    bool inside = inClip(x,y,&cr);
    if(ax>ay && !dashedLines && !gfx_storeLine ) {
	unsigned char* l = scanLine(y);
	d=ay-(ax >> 1);
	int px=x;
	#define FLUSH(nx) \
		if ( inside ) \
		    hlineUnclipped(px,nx,l); \
		px = nx+1;
	for(;;) {
	    if(x==x2) {
		FLUSH(x);
		GFX_END
		return;
	    }
	    if(d>=0) {
		FLUSH(x);
		y+=sy;
		d-=ax;
		l = scanLine(y);
		if ( !cr.contains(x+1,y) )
		    inside = inClip(x+1,y, &cr);
	    } else if ( !cr.contains(x+1,y) ) {
		FLUSH(x);
		inside = inClip(x+1,y, &cr);
	    }
	    x++;
	    d+=ay;
	}
    } else if (ax > ay) {
	// cannot use hline for dashed lines
	int di = 0;
	int dc = dashedLines ? dashes[0] : 0;
	d=ay-(ax >> 1);
	for(;;) {
	    if ( !cr.contains(x,y))
		inside = inClip(x,y, &cr);
#ifdef GFX_CORRECT_POLYLINE_JOIN
	    if ( gfx_storeLine ) {
		doDraw = gfx_doDraw;
		QPoint pt( x, y );
		if ( gfx_noLineOverwrite ) {
		    if ( gfx_storedLineRead >= 0 && pt == gfx_storedLineRd[gfx_storedLineRead] ) {
			// we drew this point last time.
			doDraw = FALSE;
		    }
		    gfx_storedLineRead += gfx_storedLineDir;
		    if ( gfx_storedLineRead >= gfx_storedLineBufferSize )
			gfx_noLineOverwrite = FALSE;
		}
		gfx_storedLineWr[gfx_storedLineWrite++] = pt;
		if ( gfx_storedLineWrite >= gfx_storedLineBufferSize )
		    gfx_storeLine = FALSE;
	    }
#endif
	    if ( doDraw && inside && (di&0x01) == 0) {
		drawPointUnclipped( x, scanLine(y) );
	    }
	    if(x==x2) {
		GFX_END
		return;
	    }
	    if (dashedLines && --dc <= 0) {
		if (++di >= numDashes)
		    di = 0;
		dc = dashes[di];
	    }
	    if(d>=0) {
		y+=sy;
		d-=ax;
	    }
	    x++;
	    d+=ay;
	}
    } else {
	int di = 0;
	int dc = dashedLines ? dashes[0] : 0;
	d=ax-(ay >> 1);
	for(;;) {
	    // y is dominant so we can't optimise with hline
	    if ( !cr.contains(x,y))
		inside = inClip(x,y, &cr);
#ifdef GFX_CORRECT_POLYLINE_JOIN
	    if ( gfx_storeLine ) {
		doDraw = gfx_doDraw;
		QPoint pt( x, y );
		if ( gfx_noLineOverwrite ) {
		    if ( gfx_storedLineRead >= 0 && pt == gfx_storedLineRd[gfx_storedLineRead] ) {
			// we drew this point last time.
			doDraw = FALSE;
		    }
		    gfx_storedLineRead += gfx_storedLineDir;
		    if ( gfx_storedLineRead >= gfx_storedLineBufferSize )
			gfx_noLineOverwrite = FALSE;
		}
		gfx_storedLineWr[gfx_storedLineWrite++] = pt;
		if ( gfx_storedLineWrite >= gfx_storedLineBufferSize )
		    gfx_storeLine = FALSE;
	    }
#endif
	    if ( doDraw && inside && (di&0x01) == 0)
		drawPointUnclipped( x, scanLine(y) );
	    if(y==y2) {
		GFX_END
		return;
	    }
	    if (dashedLines && --dc <= 0) {
		if (++di >= numDashes)
		    di = 0;
		dc = dashes[di];
	    }
	    if(d>=0) {
		x++;
		d-=ay;
	    }
	    y+=sy;
	    d+=ax;
	}
    }
    GFX_END
}

/*!
    \fn void QGfxRaster<depth,type>::vline( int x,int y1,int y2 )

    Draw a line at coordinate \a x from \a y1 to \a y2.

    Performs clipping.
*/

//screen coordinates, clipped
template <const int depth, const int type>
void QGfxRaster<depth,type>::vline( int x,int y1,int y2 )
{
    if ( y1 > y2 ) {
	int ty = y2;
	y2 = y1;
	y1 = ty;
    }

    // gross clip.
    if ( y1 > clipbounds.bottom() || y2 < clipbounds.top() )
	return;
    if ( y1 < clipbounds.top() )
	y1 = clipbounds.top();
    if ( y2 > clipbounds.bottom() )
	y2 = clipbounds.bottom();
    /*
    qDebug( "x %d, y1 %d, y2 %d, cliprects %d", x, y1, y2, ncliprect );
    for ( int i = 0; i < ncliprect; i++ ) {
	QRect r = cliprect[i];
	qDebug( "clip: %d, %d, %d, %d", r.left(), r.top(), r.right(), r.bottom() );
    }
    */
    QRect cr;
    bool plot=inClip(x,y1,&cr);
    int y=y1;
    for (;;) {
	int yb = cr.bottom();
	if ( yb >= y2 ) {
	    if (plot) {
		unsigned char *sl = scanLine(y);
		for ( int r = y; r <= y2; r++ ) {
		    drawPointUnclipped( x, sl );
		    sl += lstep;
		}
	    }
	    break;
	} else {
//	    qDebug( "Y = %d, cl %d, cr %d, ct %d, cb %d, clipcursor %d", y, cr.left(), cr.right(), cr.top(), cr.bottom(), clipcursor );
	    if (plot) {
		unsigned char *sl = scanLine(y);
		for ( int r = y; r <= yb; r++ ) {
		    drawPointUnclipped( x, sl );
		    sl += lstep;
		}
	    }
	    y=yb+1;
	    plot=inClip(x,y,&cr,plot);
	}
    }
//    qDebug( "Done" );
}

const double Q_PI   = 3.14159265358979323846;   // pi
const double Q_2PI  = 6.28318530717958647693;   // 2*pi
const double Q_PI2  = 1.57079632679489661923;   // pi/2

double qsincos( double a, bool calcCos=FALSE )
{
    if ( calcCos )                              // calculate cosine
	a -= Q_PI2;
    if ( a >= Q_2PI || a <= -Q_2PI ) {          // fix range: -2*pi < a < 2*pi
	int m = (int)(a/Q_2PI);
	a -= Q_2PI*m;
    }
    if ( a < 0.0 )                              // 0 <= a < 2*pi
	a += Q_2PI;
    int sign = a > Q_PI ? -1 : 1;
    if ( a >= Q_PI )
	a = Q_2PI - a;
    if ( a >= Q_PI2 )
	a = Q_PI - a;
    if ( calcCos )
	sign = -sign;
    double a2  = a*a;                           // here: 0 <= a < pi/4
    double a3  = a2*a;                          // make taylor sin sum
    double a5  = a3*a2;
    double a7  = a5*a2;
    double a9  = a7*a2;
    double a11 = a9*a2;
    return (a-a3/6+a5/120-a7/5040+a9/362880-a11/39916800)*sign;
}

inline double qsin( double a ) { return qsincos(a,FALSE); }
inline double qcos( double a ) { return qsincos(a,TRUE); }

double qatan2( double y, double x )
{
    double r;
    if ( x != 0.0 ) {
	double a = fabs(y/x);
	if ( a <= 1 )
	    r = a/(1+ 0.28*a*a);
	else
	    r = Q_PI2 - a/(a*a + 0.28);
    } else {
	r = Q_PI2;
    }

    if ( y >= 0.0 ) {
	if ( x >= 0.0 )
	    return r;
	else
	    return Q_PI - r;
    } else {
	if ( x >= 0.0 )
	    return Q_2PI - r;
	else
	    return Q_PI + r;
    }
}

/*
  Based on lines_intersect from Graphics Gems II, author: Mukesh Prasad
*/
static QPoint intersection( const QPointArray& pa, const QPoint& p0, int p, int q )
{
    int x1 = p0.x();
    int x2 = pa[p+1].x();
    int y1 = p0.y();
    int y2 = pa[p+1].y();
    int x3 = pa[q].x();
    int x4 = pa[q+1].x();
    int y3 = pa[q].y();
    int y4 = pa[q+1].y();

    int a1 = y2 - y1;
    int b1 = x1 - x2;
    int c1 = x2 * y1 - x1 * y2;

    int a2 = y4 - y3;
    int b2 = x3 - x4;
    int c2 = x4 * y3 - x3 * y4;

    int denom = a1 * b2 - a2 * b1;
    if ( denom == 0 )
	return (p0+pa[q])/2;

    int offset = denom < 0 ? - denom / 2 : denom / 2;
    int num = b1 * c2 - b2 * c1;
    int x = ( num < 0 ? num - offset : num + offset ) / denom;

    num = a2 * c1 - a1 * c2;
    int y = ( num < 0 ? num - offset : num + offset ) / denom;

    return QPoint(x,y);
}

static void fix_mitre(QPointArray& pa, QPoint& pp, int i1, int i2, int i3, int penwidth)
{
    QPoint inter = intersection(pa, pp, i1, i3);
    pp = pa[i3];
    QPoint d2 = inter-pa[i2];
    int l2 = d2.x()*d2.x()+d2.y()*d2.y();
    if ( l2 > penwidth*penwidth*8 ) {
	// Too sharp, leave it square
    } else {
	pa[i2] = inter;
	pa[i3] = inter;
    }
}

/*
  Converts a thick polyline into a polygon which can be painted with
  the winding rule.
*/
static QPointArray convertThickPolylineToPolygon( const QPointArray &points,int index, int npoints, int penwidth, Qt::PenJoinStyle join, bool close )
{
    QPointArray pa(npoints*4+(close?2:-4));

    int cw=0; // clockwise cursor in pa
    int acw=pa.count()-1; // counterclockwise cursor in pa

    for (int i=0; i<npoints-(close?0:1); i++) {
	int x1 = points[index + i].x();
	int y1 = points[index + i].y();
	int x2 = points[index + (i==npoints-1 ? 0 : i+1)].x();
	int y2 = points[index + (i==npoints-1 ? 0 : i+1)].y();

	int dx = x2 - x1;
	int dy = y2 - y1;
	int w = qt_int_sqrt(dx*dx+dy*dy);
	int iy = w ? (penwidth * dy)/ w : dy ? 0 : penwidth;
	int ix = w ? (penwidth * dx)/ w : dx ? 0 : penwidth;

	// rounding dependent on sign
	int nix, niy;
	if ( ix < 0 ) {
	    nix = ix/2;
	    ix = (ix-1)/2;
	} else {
	    nix = (ix+1)/2;
	    ix = ix/2;
	}
	if (dy==0 && penwidth%2) {
	    nix--;
	    ix++;
	}

	if ( iy < 0 ) {
	    niy = iy/2;
	    iy = (iy-1)/2;
	    if (dx==0 && penwidth > 1)
		++iy;
	} else {
	    niy = (iy+1)/2;
	    iy = iy/2;
	    if (dx==0 && penwidth > 1)
		--niy;
	}

	pa.setPoint( cw, x1+iy, y1-nix );
	pa.setPoint( acw, x1-niy, y1+ix );
	cw++; acw--;
	pa.setPoint( cw, x2+iy, y2-nix );
	pa.setPoint( acw, x2-niy, y2+ix );
	cw++; acw--;
    }
    if ( close ) {
	pa[cw] = pa[0];
	pa[acw] = pa[pa.count()-1];
    }
    if ( npoints > 2 && join == Qt::MiterJoin ) {
	if ( close ) {
	    QPoint pp=pa[0];
	    QPoint p1=pa[pa.count()-2];
	    int i;
	    for (i=0; i<cw-2; i+=2)
		fix_mitre(pa, pp, i, i+1, i+2, penwidth);
	    fix_mitre(pa, pp, i, i+1, 0, penwidth);
	    pp=p1;
	    fix_mitre(pa, pp, pa.count()-2, acw, acw+1, penwidth);
	    for (i=acw+1; i<(int)pa.count()-3; i+=2)
		fix_mitre(pa, pp, i, i+1, i+2, penwidth);

	    pa[0] = pa[cw];
	    pa[pa.count()-1] = pa[acw];
	} else {
	    int i;
	    QPoint pp=pa[0];
	    for (i=0; i<cw-2; i+=2)
		fix_mitre(pa, pp, i, i+1, i+2, penwidth);
	    pp=pa[acw+1];
	    for (i=acw+1; i<(int)pa.count()-2; i+=2)
		fix_mitre(pa, pp, i, i+1, i+2, penwidth);
	}
    }

    return pa;
}

/*!
\fn void QGfxRaster<depth, type>::drawThickLine( int x1, int y1, int x2, int y2 )

\internal

Draw a line with a thickness greater than one pixel from \a x1, \a y1
to \a x2, \a y2. Called from drawLine when necessary.
*/

template <const int depth, const int type>
void QGfxRaster<depth, type>::drawThickLine( int x1, int y1, int x2, int y2 )
{
    QPointArray pa(2);
    pa.setPoint(0,x1,y1);
    pa.setPoint(1,x2,y2);
    drawThickPolyline(pa,0,2);
}


/*!
\fn void QGfxRaster<depth,type>::drawThickPolyline( const QPointArray &points,int index, int npoints )

\internal

Draw a series of lines of a thickness greater than one pixel - called
from drawPolyline as necessary. \a points is the array of points to
use, \a index is the offset at which to start in that array, \a npoints
is the number of points to use.
*/

template <const int depth, const int type>
void QGfxRaster<depth,type>::drawThickPolyline( const QPointArray &points,int index, int npoints )
{
    if ( npoints < 2 )
	return;
    bool close = points[index] == points[index+npoints-1];
    QPointArray pa = convertThickPolylineToPolygon( points, index,
	close ? npoints-1 : npoints,
	cpen.width(), cpen.joinStyle(), close );

    usePen();
    GFX_START(clipbounds)
    if((*gfx_optype)!=0) {
	sync();
    }
    (*gfx_optype)=0;
    scan(pa, TRUE, 0, pa.count(), FALSE);
    GFX_END
}

/*!
\fn void QGfxRaster<depth,type>::hline( int x1,int x2,int y)

\internal

Draw a line at coordinate \a y from \a x1 to \a x2 - used by the polygon
drawing code. Performs clipping.
*/

//screen coordinates, clipped, x1<=x2
template <const int depth, const int type>
void QGfxRaster<depth,type>::hline( int x1,int x2,int y)
{
    // gross clip.
    if ( x1 > clipbounds.right() || x2 < clipbounds.left() )
	return;
    if ( x1 < clipbounds.left() )
	x1 = clipbounds.left();
    if ( x2 > clipbounds.right() )
	x2 = clipbounds.right();

    QRect cr;
    unsigned char *l=scanLine(y);
    bool plot=inClip(x1,y,&cr);
    int x=x1;
    for (;;) {
	int xr = cr.right();
	if ( xr >= x2 ) {
	    if (plot) {
#if defined(_OS_QNX6_) // Qnx-style hline takes x,y not pointer
		if (isScreenGfx())
		    hlineUnclipped(x,x2,y);
		else
#endif
		    hlineUnclipped(x,x2,l); // only call made for non-QNX
	    }
	    break;
	} else {
	    if (plot) {
#if defined(_OS_QNX6_)
		if (isScreenGfx())
		    hlineUnclipped(x,xr,y);
		else
#endif
		    hlineUnclipped(x,xr,l); // only call made for non-QNX
	    }
	    x=xr+1;
	    plot=inClip(x,y,&cr,plot);
	}
    }
}

/*!
\fn void QGfxRaster<depth,type>::hlineUnclipped( int x1,int x2,unsigned char* l)

\internal

Draws a line in the current pen color from \a x1 to \a x2 on scanline \a l,
ignoring clipping. Used by anything that draws in solid colors - drawLine,
fillRect, and drawPolygon.
*/

//screen coordinates, unclipped, x1<=x2, x1>=0
template <const int depth, const int type>
GFX_INLINE void QGfxRaster<depth,type>::hlineUnclipped( int x1,int x2,unsigned char* l)
{
    int w = x2-x1+1;
    if( (myrop!=XorROP) && (myrop!=NotROP) ) {
	if ( depth == 32 ) {
	    unsigned int *myptr=(unsigned int *)l + x1;
	    while ( w-- )
		*(myptr++) = pixel;
	} else if ( depth == 24 ) {
	    unsigned char *myptr = l + x1*3;
	    while ( w-- ) {
		gfxSetRgb24( myptr, pixel );
		myptr += 3;
	    }
	} else if ( depth == 16 ) {
	    unsigned short int *myptr=(unsigned short int *)l;
#ifdef QWS_NO_WRITE_PACKING
	    myptr+=x1;
	    while ( w-- )
		*(myptr++) = pixel;
#else
	    int frontadd;
	    int backadd;
	    int count;
	    calcPacking(myptr,x1,x2,frontadd,backadd,count);

	    myptr+=x1;

	    PackType put = pixel | ( pixel << 16 );

	    while ( frontadd-- )
		*(myptr++)=pixel;
	    // Duffs device.
	    PackType *myptr2 = (PackType*)myptr;
	    myptr += count * 2;
	    PackType *end2 = (PackType*)myptr;
	    switch(count%8){
		case 0:
		    while ( myptr2 != end2 ) {
			*myptr2++ = put;
		case 7: *myptr2++ = put;
		case 6: *myptr2++ = put;
		case 5: *myptr2++ = put;
		case 4: *myptr2++ = put;
		case 3: *myptr2++ = put;
		case 2: *myptr2++ = put;
		case 1: *myptr2++ = put;
		    }
	    }
	    while ( backadd-- )
		*(myptr++)=pixel;
#endif
	} else if ( depth == 8 ) {
	    unsigned char *myptr=l;
#ifdef QWS_NO_WRITE_PACKING
	    myptr+=x1;
	    while ( w-- )
		*(myptr++) = pixel;
#else
	    int frontadd,backadd,count;
	    calcPacking(myptr,x1,x2,frontadd,backadd,count);

	    myptr+=x1;

	    PackType put = pixel | ( pixel << 8 )
		| ( pixel << 16 ) | (pixel << 24 );

	    while ( frontadd-- )
		*(myptr++)=pixel;
	    // Duffs device.
	    PackType *myptr2 = (PackType*)myptr;
	    myptr += count * 4;
	    PackType *end2 = (PackType*)myptr;
	    switch(count%8){
		case 0:
		    while ( myptr2 != end2 ) {
			*myptr2++ = put;
			case 7: *myptr2++ = put;
			case 6: *myptr2++ = put;
			case 5: *myptr2++ = put;
			case 4: *myptr2++ = put;
			case 3: *myptr2++ = put;
			case 2: *myptr2++ = put;
			case 1: *myptr2++ = put;
		    }
	    }
	    while ( backadd-- )
		*(myptr++)=pixel;
#endif
	} else if ( depth == 4 ) {
	    unsigned char *myptr=l;
	    unsigned char *dptr = myptr + x1/2;
	    if ( x1&1 ) {
		drawPointUnclipped( x1, myptr);
		w--;
		dptr++;
	    }

	    unsigned char val = pixel | (pixel << 4);
	    while ( w > 1 ) {
		*dptr++ = val;
		w -= 2;
	    }

	    if ( !(x2&1))
		drawPointUnclipped( x2, myptr);
	} else if ( depth == 1 ) {
	    //#### we need to use semaphore
	    l += x1/8;
	    if ( x1/8 == x2/8 ) {
		// Same byte

		uchar mask = (0xff << (x1 % 8)) & (0xff >> (7 - x2 % 8));
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
		if ( is_screen_gfx )
		mask = (0xff >> (x1 % 8)) & (0xff << (7 - x2 % 8));
#endif
		if ( pixel )
		    *l |= mask;
		else
		    *l &= ~mask;
	    } else {
		volatile unsigned char *last = l + (x2/8-x1/8);
		uchar mask = 0xff << (x1 % 8);
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
		if ( is_screen_gfx )
		    mask = 0xff >> (x1 % 8);
#endif
		if ( pixel )
		    *l++ |= mask;
		else
		    *l++ &= ~mask;
		unsigned char byte = pixel ? 0xff : 0x00;
		while (l < last)
		    *l++ = byte;

		mask = 0xff >> (7 - x2 % 8);
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
		if ( is_screen_gfx )
		    mask = 0xff << (7 - x2 % 8);
#endif
		if ( pixel )
		    *l |= mask;
		else
		    *l &= ~mask;
	    }
	}
    } else if(myrop==XorROP) {
	if ( depth == 32 ) {
	    unsigned int *myptr=(unsigned int *)l + x1;
	    while ( w-- ) {
		(*myptr) = (*myptr) ^ pixel;
		myptr++;
	    }
	} else if ( depth == 16 ) {
	    unsigned short int *myptr=(unsigned short int *)l;
	    myptr+=x1;
	    while ( w-- ) {
		(*myptr) = (*myptr) ^ pixel;
		myptr++;
	    }
	} else if ( depth == 8 ) {
	    unsigned char *myptr=l;
	    myptr+=x1;
	    while ( w-- ) {
		(*myptr) = (*myptr) ^ pixel;
		myptr++;
	    }
	} else if ( depth == 4 || depth == 24 ) {
	    unsigned char *myptr=l;
	    while ( w-- )
		drawPointUnclipped( x1++, myptr);
	} else if ( depth == 1 ) {
	    // ...
	}
    } else if(myrop==NotROP) {
	if ( depth == 32 ) {
	    unsigned int *myptr=(unsigned int *)l + x1;
	    while ( w-- ) {
		(*myptr) = ~(*myptr);
		myptr++;
	    }
	} else if ( depth == 16 ) {
	    unsigned short int *myptr=(unsigned short int *)l;
	    myptr+=x1;
	    while ( w-- ) {
		(*myptr) = ~(*myptr);
		myptr++;
	    }
	} else if ( depth == 8 ) {
	    unsigned char *myptr=l;
	    myptr+=x1;
	    while ( w-- ) {
		(*myptr) = ~(*myptr);
		myptr++;
	    }
	} else if ( depth == 4 || depth == 24 ) {
	    unsigned char *myptr=l;
	    while ( w-- )
		drawPointUnclipped( x1++, myptr);
	} else if ( depth == 1 ) {
	    // ...
	}
    } else {
	// ... - will probably go in above clause
    }
}

/*!
\fn void QGfxRaster<depth,type>::hImageLineUnclipped( int x1,int x2,
						    unsigned char *l,
						    unsigned char *srcdata,
						    bool reverse)

\internal

 \a l points to the start of the destination line's data.
 \a x1 and \a x2 are the start and end pixels.
 \a srcdata points to the source's left pixel start byte if \a reverse is
 false. srcdata points to the source's right pixels's start byte if reverse
 is true. reverse will only be true if the source and destination are the same
 buffer and a mask is set.
 Image data comes from of the setSource calls (in which case the
 variable srcdata points to it) or as a solid value stored in srccol
 (if setSourcePen is used). This method is internal and called from blt and
 stretchBlt. Its complexity is caused by its need to deal with masks, copying
 from right to left or left to right (for overlapping blt's), packing writes
 for performance reasons and arbitrary source and destination depths.
*/

template <const int depth, const int type>
GFX_INLINE void QGfxRaster<depth,type>::hImageLineUnclipped( int x1,int x2,
						    unsigned char *l,
						    unsigned char *srcdata,
						    bool reverse)
{
    int w = x2-x1+1;
    if ( depth == 32 ) {
	unsigned int *myptr=(unsigned int *)l;
	int inc = 1;
	if(!reverse) {
	    myptr+=x1;
	} else {
	    myptr+=x2;
	    inc = -1;
	}
	if ( !ismasking ) {
	    uint gv = srccol;
	    while ( w-- ) {
		if (srctype==SourceImage)
		    gv = get_value_32(srcdepth,&srcdata);
		if(myrop==XorROP) {
		  *(myptr)^=gv;
		  myptr++;
		} else if(myrop==NotROP) {
		  *(myptr)=~(*myptr);
		  myptr++;
		} else {
		  *(myptr++) = gv;
		}
	    }
	} else {
	    //masked 32bpp blt...
	    unsigned int gv = srccol;
	    while ( w-- ) {
		if ( srctype == SourceImage )
		    gv = get_value_32( srcdepth, &srcdata, reverse );
		bool masked = TRUE;
		GET_MASKED(reverse, w);
		if ( !masked ) {
		  if(myrop==XorROP) {
		    *(myptr)^=gv;
		  } else if(myrop==NotROP) {
		    *(myptr)=~(*myptr);
		  } else {
		    *(myptr) = gv;
		  }
		}
		myptr += inc;
	    }
	}
    } else if ( depth == 24 ) {
	unsigned char *myptr = l;
	int inc = 3;
	if(!reverse) {
	    myptr += x1*3;
	} else {
	    myptr += x2*3;
	    inc = -3;
	}
	if ( !ismasking ) {
	    uint gv = srccol;
	    while ( w-- ) {
		if (srctype==SourceImage)
		    gv = get_value_24(srcdepth,&srcdata);
		gfxSetRgb24( myptr, gv );
		myptr += 3;
	    }
	} else {
	    //masked 32bpp blt...
	    unsigned int gv = srccol;
	    while ( w-- ) {
		if ( srctype == SourceImage )
		    gv = get_value_24( srcdepth, &srcdata, reverse );
		bool masked = TRUE;
		GET_MASKED(reverse, w);
		if ( !masked )
		    gfxSetRgb24( myptr, gv );
		myptr += inc;
	    }
	}
    } else if ( depth == 16 ) {
	unsigned short int *myptr=(unsigned short int *)l;
	int inc = 1;
	if(!reverse) {
	    myptr+=x1;
	} else {
	    myptr+=x2;
	    inc = -1;
	}
	if(!ismasking) {
#ifdef QWS_NO_WRITE_PACKING
	    while ( w-- ) {
		if(myrop==XorROP) {
		    *(myptr)^=get_value_16(srcdepth,&srcdata);
		    myptr++;
		} else if(myrop==NotROP) {
		    *(myptr)=~(*myptr);
		    myptr++;
		} else {
		    *(myptr++)=get_value_16(srcdepth,&srcdata);
		}
	    }
#else
	    // 32-bit writes
	    int frontadd;
	    int backadd;
	    int count;

	    calcPacking(myptr-x1,x1,x2,frontadd,backadd,count);

	    PackType dput;
	    if(myrop==XorROP) {
		while ( frontadd-- )
		    *(myptr++)^=get_value_16(srcdepth,&srcdata);
		PackType *myptr2 = (PackType*)myptr;
		myptr += count * 2;
		while ( count-- ) {
#ifdef QT_QWS_REVERSE_BYTE_ENDIANNESS
			dput = (get_value_16(srcdepth,&srcdata) << 16);
			dput |= get_value_16(srcdepth,&srcdata);
#else
			dput = get_value_16(srcdepth,&srcdata);
			dput |= (get_value_16(srcdepth,&srcdata) << 16);
#endif
		    *myptr2++ ^= dput;
		}
		while ( backadd-- )
		    *(myptr++)^=get_value_16(srcdepth,&srcdata);
	    } else if(myrop==NotROP) {
		while ( frontadd-- ) {
		    *(myptr)=~(*myptr);
		    myptr++;
		}
		PackType *myptr2 = (PackType*)myptr;
		myptr += count * 2;
		while ( count-- ) {
		    *myptr2 = ~*myptr2;
		    myptr2++;
		}
		while ( backadd-- ) {
		    *(myptr)=~(*myptr);
		    myptr++;
		}
	    } else {
		while ( frontadd-- )
		    *(myptr++)=get_value_16(srcdepth,&srcdata);
		PackType *myptr2 = (PackType*)myptr;
		myptr += count * 2;
		while ( count-- ) {
# ifdef QT_QWS_REVERSE_BYTE_ENDIANNESS
		    dput = (get_value_16(srcdepth,&srcdata) << 16);
		    dput |= get_value_16(srcdepth,&srcdata);
# else
		    dput = get_value_16(srcdepth,&srcdata);
		    dput |= (get_value_16(srcdepth,&srcdata) << 16);
# endif
		    *myptr2++ = dput;
		}
		while ( backadd-- )
		    *(myptr++)=get_value_16(srcdepth,&srcdata);
	    }
#endif
	} else {
	    // Probably not worth trying to pack writes if there's a mask
	    unsigned short int gv = srccol;
	    if (myrop == XorROP || myrop == NotROP) {
		while ( w-- ) {
		    if ( srctype==SourceImage )
			gv = get_value_16( srcdepth, &srcdata, reverse );
		    bool masked = TRUE;
		    GET_MASKED(reverse, w);
		    if ( !masked ) {
			if (myrop == XorROP)
			    *(myptr) ^= gv;
			else
			    *(myptr) = ~(*myptr);
		    }
		    myptr += inc;
		}
	    } else {
		while ( w-- ) {
		    if ( srctype==SourceImage )
			gv = get_value_16( srcdepth, &srcdata, reverse );
		    bool masked = TRUE;
		    GET_MASKED(reverse, w);
		    if ( !masked )
			*(myptr) = gv;
		    myptr += inc;
		}
	    }
	}
    } else if ( depth == 8 ) {
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	// cursor code uses 8bpp backing store
	if (srcdepth == 4 && srcbits==qt_screen->base())
	    src_little_endian = !src_little_endian;
#endif
	unsigned char *myptr=(unsigned char *)l;
	int inc = 1;
	if(!reverse) {
	    myptr+=x1;
	} else {
	    myptr+=x2;
	    inc = -1;
	}
	if(!ismasking) {
#ifdef QWS_NO_WRITE_PACKING
	  while ( w-- ) {
	    if(myrop==XorROP) {
	      *(myptr)^=get_value_8(srcdepth,&srcdata);
	      myptr++;
	    } else if(myrop==NotROP) {
	      *(myptr)=~(*myptr);
	      myptr++;
	    } else {
	      *(myptr++)=get_value_8(srcdepth,&srcdata);
	    }
	  }
#else
	    // 32-bit writes
	    int frontadd;
	    int backadd;
	    int count;

	    calcPacking(myptr-x1,x1,x2,frontadd,backadd,count);

	    PackType dput;
	    while ( frontadd-- ) {
	      if(myrop==XorROP) {
		*(myptr)^=get_value_8(srcdepth,&srcdata);
		myptr++;
	      } else if(myrop==NotROP) {
		*(myptr)=~(*myptr);
		myptr++;
	      } else {
		*(myptr++)=get_value_8(srcdepth,&srcdata);
	      }
	    }
	    while ( count-- ) {
#ifdef QT_QWS_REVERSE_BYTE_ENDIANNESS
		dput = (get_value_8(srcdepth,&srcdata) << 24);
		dput |= (get_value_8(srcdepth,&srcdata) << 16);
		dput |= (get_value_8(srcdepth,&srcdata) << 8);
		dput |= get_value_8(srcdepth,&srcdata);
#else
		dput = get_value_8(srcdepth,&srcdata);
		dput |= (get_value_8(srcdepth,&srcdata) << 8);
		dput |= (get_value_8(srcdepth,&srcdata) << 16);
		dput |= (get_value_8(srcdepth,&srcdata) << 24);
#endif
		if(myrop==XorROP) {
		  *((PackType*)myptr) ^= dput;
		} else if(myrop==NotROP) {
                  *((PackType*)myptr) = ~*((PackType*)myptr);
		} else {
		  *((PackType*)myptr) = dput;
		}
		myptr += 4;
	    }
	    while ( backadd-- ) {
	      if(myrop==XorROP) {
		*(myptr)^=get_value_8(srcdepth,&srcdata);
		myptr++;
	      } else if(myrop==NotROP) {
		*(myptr)=~(*myptr);
		myptr++;
	      } else {
		*(myptr++)=get_value_8(srcdepth,&srcdata);
	      }
	    }
#endif
	} else {
	    // Probably not worth trying to pack writes if there's a mask
	    unsigned char gv = srccol;
	    while ( w-- ) {
		if ( srctype==SourceImage )
		    gv = get_value_8( srcdepth, &srcdata, reverse);
		bool masked = TRUE;
		GET_MASKED(reverse, w);
		if ( !masked ) {
		  if(myrop==XorROP) {
		    *(myptr) ^= gv;
		  } else if(myrop==NotROP) {
		    *(myptr) = ~(*myptr);
		  } else {
		    *(myptr) = gv;
		  }
		}
		myptr += inc;
	    }
	}
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	//restore
	if (srcdepth == 4 && srcbits==qt_screen->base())
	    src_little_endian = !src_little_endian;
#endif
    } else if ( depth == 4 ) {
	unsigned char *dp = l;
	unsigned int gv = srccol;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	//really ugly hack: screen is opposite endianness to everything else
	if (srcbits==qt_screen->base())
	    src_little_endian = !src_little_endian;
#endif
	if ( reverse ) {
	    dp += (x2/2);
	    int x = x2;
	    while ( w-- ) {
		if ( srctype==SourceImage )
		    gv = get_value_4( srcdepth, &srcdata, reverse);
		bool masked = TRUE;
		if ( ismasking ) {
		    GET_MASKED(reverse, w);
		}
		if ( !masked || !ismasking ) {
		    int s = (x&1) << 2;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
		    if ( is_screen_gfx )
		        s = (~x&1) << 2;
#endif
		    *dp = ( *dp & MASK4BPP(s) ) | (gv << s);
		}
		if ( !(x&1) )
		    dp--;
		x--;
	    }
	} else {
	    dp += (x1/2);
	    int x = x1;
	    while ( w-- ) {
		if ( srctype==SourceImage )
		    gv = get_value_4( srcdepth, &srcdata, reverse);
		bool masked = TRUE;
		if ( ismasking ) {
		    GET_MASKED(reverse, w);
		}
		if ( !masked || !ismasking ) {
		    int s = (x&1) << 2;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
		    if ( is_screen_gfx )
		        s = (~x&1) << 2;
#endif
		    *dp = ( *dp & MASK4BPP(s) ) | (gv << s);
		}
		if ( x&1 )
		    dp++;
		x++;
	    }
	}
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	//end of really ugly hack; undo the damage
	if (srcbits==qt_screen->base())
	    src_little_endian = !src_little_endian;
#endif
    } else if ( depth == 1 ) {
	// General case only implemented.
	// Lots of optimisation can be performed for special cases.
	unsigned char * dp=l;
	unsigned int gv = srccol;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	//really ugly hack: screen is opposite endianness to everything else
	if (srcbits==qt_screen->base())
	    src_little_endian = !src_little_endian;
#endif
	if ( reverse ) {
	    dp+=(x2/8);
	    int skipbits = 7 - (x2%8);
	    for ( int b = x1/8; b <= x2/8; b++ ) {
		unsigned char m = *dp;
		for (int i = 0; i < 8 && w; i++) {
		    if (skipbits)
			skipbits--;
		    else {
			w--;
			if ( srctype == SourceImage )
			    gv = get_value_1( srcdepth, &srcdata, TRUE);
			bool masked = TRUE;
			if ( ismasking ) {
			    GET_MASKED(TRUE, w);
			}
			if ( !masked || !ismasking ) {
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
			    if ( is_screen_gfx ) {
				if (gv)
				    m |= 0x01 << i;
				else
				    m &= ~( 0x01 << i );
			    } else
#endif
				if (gv)
				    m |= 0x80 >> i;
				else
				    m &= ~( 0x80 >> i );
			}
		    }
		}
		*(dp--) = m;
	    }
	} else {
	    dp+=(x1/8);
	    int skipbits = x1%8;
	    for ( int b = x1/8; b <= x2/8; b++ ) {
		unsigned char m = *dp;
		for (int i = 0; i < 8 && w; i++) {
		    if (skipbits)
			skipbits--;
		    else {
			w--;
			if ( srctype == SourceImage )
			    gv = get_value_1( srcdepth, &srcdata, FALSE);
			bool masked = TRUE;
			if ( ismasking ) {
			    GET_MASKED(FALSE, w);
			}
			if ( !masked || !ismasking ) {
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
			    if ( is_screen_gfx ) {
				if (gv)
				    m |= 0x80 >> i;
				else
				    m &= ~( 0x80 >> i );
			    } else
#endif
				if (gv)
				    m |= 1 << i;
				else
				    m &= ~( 1 << i );
			}
		    }
		}
		*(dp++) = m;
	    }
	}
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	//end of really ugly hack; undo the damage
	if (srcbits==qt_screen->base())
	    src_little_endian = !src_little_endian;
#endif
    }
}

/*!
\fn void QGfxRaster<depth,type>::hAlphaLineUnclipped( int x1,int x2,
						    unsigned char* l,
						    unsigned char * srcdata,
						    unsigned char * alphas)

\internal

This is similar to hImageLineUnclipped but handles the more complex
alpha blending modes (InlineAlpha, SeparateAlpha, SolidAlpha).
Blending is a simple averaging between the source and destination r, g and b
values using the 8-bit source alpha value -
that is, for each of r, g and b the result is

(source - destination * alpha) / 256 + destination

Note that since blending requires some per-pixel computation and a read-write
access on the destination it tends to be slower than the simpler alpha
blending modes. \a x1 and \a x2 specify where to draw in the destination,
\a l is the pointer to the scanline to draw into, \a srcdata is the source
pixel data and \a alphas is the alpha channel for the SeparateAlpha mode.
*/

template <const int depth, const int type>
GFX_INLINE void QGfxRaster<depth,type>::hAlphaLineUnclipped( int x1,int x2,
						    unsigned char* l,
						    unsigned char * srcdata,
						    unsigned char * alphas)
{
    int w=x2-x1+1;
    if ( depth == 32 ) {
	// First read in the destination line
	unsigned int *myptr = (unsigned int *)l;
	unsigned int *alphaptr = (unsigned int *)alphabuf;
	unsigned char * avp=alphas;
	int loopc;

	unsigned int *temppos = myptr+x1;
	for ( int i = 0; i < w; i++ )
	    *(alphaptr++) = *(temppos++);

	// Now blend with source data
	unsigned char * srcptr=srcdata;
	unsigned int srcval;

	if(srctype==SourceImage) {
	    srcptr=srcdata;
	    srcval=0; // Shut up compiler
	} else {
	    // SourcePen
	    srcval=srccol;
	}

	alphaptr = (unsigned int *)alphabuf;
	for(loopc=0;loopc<w;loopc++) {
	    int r,g,b;
	    if(srctype==SourceImage)
		srcval=get_value_32(srcdepth,&srcptr);

	    int av;
	    if(alphatype==InlineAlpha) {
		av = srcval >> 24;
	    } else if(alphatype==SolidAlpha) {
		av=calpha;
	    } else {
		av=*(avp++);
	    }

	    r = (srcval & 0xff0000) >> 16;
	    g = (srcval & 0xff00) >> 8;
	    b = srcval & 0xff;

	    unsigned int hold = alphabuf[loopc];
	    if(av==255) {
	        // Do nothing - we already have source values in r,g,b
	    } else if(av==0) {
		r = (hold >> 16) & 0xff;
		g = (hold >> 8) & 0xff;
		b = hold & 0xff;
	    } else {
		int tmp = (hold >> 16) & 0xff;
		r = ((r-tmp) * av) / 256 + tmp;
		tmp = (hold >> 8) & 0xff;
		g = ((g-tmp) * av) / 256 + tmp;
		tmp = hold & 0xff;
		b = ((b-tmp) * av) / 256 + tmp;
	    }
	    *(alphaptr++) = (r << 16) | (g << 8) | b;
	}

	// Now write it all out
	alphaptr = (unsigned int *)alphabuf;

	myptr += x1;
	while ( w-- ) {
	    // NotROP really doesn't make sense for an alpha blt
	  if(myrop==XorROP) {
	    *(myptr++)^=(*(alphaptr++));
	  } else {
	    *(myptr++)=*(alphaptr++);
	  }
	}
    } else if ( depth == 24 ) {
	// First read in the destination line
	unsigned char *myptr = l;
	unsigned char *alphaptr = (unsigned char *)alphabuf;
	unsigned char *avp = alphas;
	int loopc;

	memcpy( alphabuf, myptr+x1*3, w*3 );

	// Now blend with source data
	unsigned char * srcptr=srcdata;
	unsigned int srcval;

	if(srctype==SourceImage) {
	    srcptr=srcdata;
	    srcval=0; // Shut up compiler
	} else {
	    // SourcePen
	    srcval=srccol;
	}

	alphaptr = (unsigned char *)alphabuf;
	for(loopc=0;loopc<w;loopc++) {
	    int r,g,b;
	    if(srctype==SourceImage)
		srcval=get_value_32(srcdepth,&srcptr);

	    int av;
	    if(alphatype==InlineAlpha) {
		av = srcval >> 24;
	    } else if(alphatype==SolidAlpha) {
		av=calpha;
	    } else {
		av=*(avp++);
	    }

	    r = (srcval & 0xff0000) >> 16;
	    g = (srcval & 0xff00) >> 8;
	    b = srcval & 0xff;

	    unsigned char *tmp = alphaptr;
	    if ( av == 255 ) {
	        // Do nothing - we already have source values in r,g,b
	    } else if ( av == 0 ) {
	        r = *(tmp+2);
	        g = *(tmp+1);
	        b = *(tmp+0);
	    } else {
		r = ((r-*(tmp+2)) * av) / 256 + *(tmp+2);
		g = ((g-*(tmp+1)) * av) / 256 + *(tmp+1);
		b = ((b-*(tmp+0)) * av) / 256 + *(tmp+0);
	    }
	    gfxSetRgb24( alphaptr, r, g, b );
	    alphaptr += 3;
	}

	// Now write it all out
	memcpy( myptr+x1*3, alphabuf, w*3 );
#if !defined( QT_NO_IMAGE_16_BIT ) || !defined( QT_NO_QWS_DEPTH_16 )
    } else if ( depth == 16 ) {
        // First read in the destination line
	unsigned short int *myptr = (unsigned short int *)l;
	unsigned int *alphaptr = (unsigned int *)alphabuf;
	int loopc;

#ifdef QWS_NO_WRITE_PACKING
	unsigned short int *temppos = myptr + x1;
	for ( int i = 0; i < w; i++ )
	    *(alphaptr++) = get_value_32(16,(unsigned char **)&temppos);
#else
	int frontadd, backadd, count;
	calcPacking(myptr,x1,x2,frontadd,backadd,count);
	myptr+=x1;

	unsigned short int *temppos = myptr;

	int loopc2;
	for( loopc2=0;loopc2<frontadd;loopc2++ )
	    *(alphaptr++)=get_value_32(16,(unsigned char **)&temppos);

	volatile PackType temp2;
	volatile unsigned short int * cp;
	for( loopc2=0;loopc2<count;loopc2++ ) {
	    temp2=*(PackType *)temppos;
	    cp=(volatile unsigned short int *)&temp2;
	    *(alphaptr++)=qt_conv16ToRgb(*cp);
	    cp++;
	    *(alphaptr++)=qt_conv16ToRgb(*cp);
	    temppos += 2;
	}

	for( loopc2=0;loopc2<backadd;loopc2++ )
	    *(alphaptr++)=get_value_32(16,(unsigned char **)&temppos);
#endif

	// Now blend with source data
	unsigned char *srcptr=srcdata;
	unsigned int srcval;

	if(srctype==SourceImage) {
	    srcptr=srcdata;
	    srcval=0; // Shut up compiler
	} else {
	    // SourcePen
	    srcval=qt_conv16ToRgb(srccol);
	}

	int astype = 3;
	if ( srctype==SourceImage && alphatype==InlineAlpha )
	    astype = 0;
	else if ( srctype==SourceImage && alphatype != SolidAlpha )
	    astype = 1;
	else if ( alphatype != SolidAlpha )
	    astype = 2;

	int av = calpha;
	alphaptr = (unsigned int *)alphabuf;
	unsigned char *avp = alphas;
	for (loopc=0;loopc<w;loopc++) {
	    switch ( astype ) {
		case 0:
		    srcval=get_value_32(srcdepth,&srcptr);
		    av = srcval >> 24;
		    break;
		case 1:
		    srcval=get_value_32(srcdepth,&srcptr);
		    // FALLTHROUGH
		case 2:
		    av=*(avp++);
		    break;
	    }

	    int r,g,b;
	    if (av == 255) {
		unsigned char *stmp = (unsigned char *)&srcval;
#ifdef QT_QWS_REVERSE_BYTE_ENDIANNESS
		stmp++;
		r = *stmp++;
		g = *stmp++;
		b = *stmp;
#else
		b = *stmp++;
		g = *stmp++;
		r = *stmp;
#endif
	    } else {
		unsigned char *tmp=(unsigned char *)alphaptr;
#ifdef QT_QWS_REVERSE_BYTE_ENDIANNESS
		tmp++;
		r = *tmp++;
		g = *tmp++;
		b = *tmp;
#else
		b = *tmp++;
		g = *tmp++;
		r = *tmp;
#endif
		if (av) {
		    unsigned char *stmp = (unsigned char *)&srcval;
#ifdef QT_QWS_REVERSE_BYTE_ENDIANNESS
		    stmp++;
		    r += ((*stmp++-r) * av) >> 8;
		    g += (((*stmp++)-g) * av) >> 8;
		    b += (((*stmp)-b) * av) >> 8;
#else
		    b += (((*stmp++)-b) * av) >> 8;
		    g += (((*stmp++)-g) * av) >> 8;
		    r += ((*stmp-r) * av) >> 8;
#endif
		}
	    }
	    *(alphaptr++) = qt_convRgbTo16(r,g,b);
	}

	// Now write it all out
	alphaptr = (unsigned int *)alphabuf;

#ifdef QWS_NO_WRITE_PACKING
	myptr += x1;
	while ( w-- ) {
	  if(myrop==XorROP) {
	    *(myptr++)^=(*(alphaptr++));
	  } else {
	    *(myptr++)=*(alphaptr++);
	  }
	}
#else
	myptr=(unsigned short int *)l;
	calcPacking(myptr,x1,x2,frontadd,backadd,count);
	myptr+=x1;

	for ( loopc2=0;loopc2<frontadd;loopc2++ ) {
	    if (myrop==XorROP)
		*(myptr++)^=(*(alphaptr++));
	    else
		*(myptr++)=*(alphaptr++);
	}

	PackType put;
	if ( myrop==XorROP ) {
	    PackType *myptr2 = (PackType*)myptr;
	    myptr += count * 2;
	    for ( loopc2=0;loopc2<count;loopc2++ ) {
		put = *(alphaptr++);
		put |= (*(alphaptr++) << 16);
		*myptr2++ ^= put;
	    }
	} else {
	    // Duffs device.
#ifdef QT_QWS_REVERSE_BYTE_ENDIANNESS
	    #define DUFF_WRITE_WORD put=(*(alphaptr++) << 16); put|=(*alphaptr++); \
				*myptr2++ = put;
#else
	    #define DUFF_WRITE_WORD put=*(alphaptr++); put|=(*(alphaptr++) << 16); \
	                       *myptr2++ = put;
#endif
	    PackType *myptr2 = (PackType*)myptr;
	    myptr += count * 2;
	    PackType *end2 = (PackType*)myptr;
	    switch(count%8){
		case 0:
		    while ( myptr2 != end2 ) {
			DUFF_WRITE_WORD
		case 7: DUFF_WRITE_WORD
		case 6: DUFF_WRITE_WORD
		case 5: DUFF_WRITE_WORD
		case 4: DUFF_WRITE_WORD
		case 3: DUFF_WRITE_WORD
		case 2: DUFF_WRITE_WORD
		case 1: DUFF_WRITE_WORD
		    }
	    }
	    #undef DUFF_WRITE_WORD
	}

	for ( loopc2=0;loopc2<backadd;loopc2++ ) {
	    if (myrop==XorROP)
		*(myptr++)^=(*(alphaptr++));
	    else
		*(myptr++)=*(alphaptr++);
	}
#endif
#endif
    } else if ( depth == 8 ) {
        // First read in the destination line
	unsigned char * myptr;
	myptr=l;
	myptr+=x1;
	int loopc;

	unsigned char * avp=alphas;

	unsigned char * tempptr=myptr;

        for(loopc=0;loopc<w;loopc++) {
	    int val = *tempptr++;
	    alphabuf[loopc] = clut[val];
	}

	// Now blend with source data

	unsigned char * srcptr;
	unsigned int srcval = 0;
	if(srctype==SourceImage) {
	    srcptr=srcdata;
	} else {
	    // SourcePen
	    QRgb mytmp=clut[srccol];
	    srcval=qRed(mytmp) << 16 | qGreen(mytmp) << 8 | qBlue(mytmp);
	}
	simple_8bpp_alloc=TRUE;
	for(loopc=0;loopc<w;loopc++) {
	    int r,g,b;
	    if(srctype==SourceImage)
		srcval=get_value_32(srcdepth,&srcptr);

	    int av;
	    if(alphatype==InlineAlpha) {
		av=srcval >> 24;
	    } else if(alphatype==SolidAlpha) {
		av=calpha;
	    } else {
		av=*(avp++);
	    }

	    r = (srcval & 0xff0000) >> 16;
	    g = (srcval & 0xff00) >> 8;
	    b = srcval & 0xff;

	    unsigned char * tmp=(unsigned char *)&alphabuf[loopc];

	    if(myrop==XorROP) {
	      if(av==255) {
		*myptr ^= GFX_8BPP_PIXEL(r,g,b);
	      } else if ( av > 0 ) {
		r = ((r-*(tmp+2)) * av) / 256 + *(tmp+2);
		g = ((g-*(tmp+1)) * av) / 256 + *(tmp+1);
		b = ((b-*(tmp+0)) * av) / 256 + *(tmp+0);
		*myptr ^= GFX_8BPP_PIXEL(r,g,b);
	      }
	    } else {
	      if(av==255) {
		*myptr = GFX_8BPP_PIXEL(r,g,b);
	      } else if ( av > 0 ) {
		r = ((r-*(tmp+2)) * av) / 256 + *(tmp+2);
		g = ((g-*(tmp+1)) * av) / 256 + *(tmp+1);
		b = ((b-*(tmp+0)) * av) / 256 + *(tmp+0);
		*myptr = GFX_8BPP_PIXEL(r,g,b);
	      }
	    }
	    myptr++;
	}
	simple_8bpp_alloc=FALSE;
    } else if ( depth == 4 ) {
        // First read in the destination line
	unsigned char *myptr = l;
	myptr+=x1/2;

	unsigned char *avp=alphas;
	unsigned char *tempptr=myptr;

	int loopc = 0;
	if ( x1&1 ) {
	    int val = *tempptr++;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx )
		alphabuf[loopc++] = clut[(val & 0x0f)];
	    else
#endif
	    alphabuf[loopc++] = clut[(val & 0xf0) >> 4];
	}

        for ( ;loopc < w-1; loopc += 2 ) {
	    int val = *tempptr++;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx ) {
		alphabuf[loopc] = clut[val >> 4];
		alphabuf[loopc+1] = clut[val & 0x0f];
	    } else
#endif
	    {
	    alphabuf[loopc] = clut[val & 0x0f];
	    alphabuf[loopc+1] = clut[val >> 4];
	    }
	}

	if ( !(x2&1) ) {
	    int val = *tempptr;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx )
		alphabuf[w-1] = clut[(val & 0xf0) >> 4];
	    else
#endif
	    alphabuf[w-1] = clut[val & 0x0f];
	}

	// Now blend with source data

	unsigned char * srcptr;
	unsigned int srcval = 0;
	if(srctype==SourceImage) {
	    srcptr=srcdata;
	} else {
	    // SourcePen
	    QRgb mytmp=clut[srccol];
	    srcval=qRed(mytmp) << 16 | qGreen(mytmp) << 8 | qBlue(mytmp);
	}
	for(loopc=0;loopc<w;loopc++) {
	    int r,g,b;
	    if(srctype==SourceImage)
		srcval=get_value_32(srcdepth,&srcptr);

	    int av;
	    if(alphatype==InlineAlpha) {
		av=srcval >> 24;
	    } else if(alphatype==SolidAlpha) {
		av=calpha;
	    } else {
		av=*(avp++);
	    }

	    r = (srcval & 0xff0000) >> 16;
	    g = (srcval & 0xff00) >> 8;
	    b = srcval & 0xff;

	    unsigned char *tmp = (unsigned char *)&alphabuf[loopc];

	    if(av==255) {
		alphabuf[loopc] = qRgb(r,g,b);
	    } else {
		r = ((r-*(tmp+2)) * av) / 256 + *(tmp+2);
		g = ((g-*(tmp+1)) * av) / 256 + *(tmp+1);
		b = ((b-*(tmp+0)) * av) / 256 + *(tmp+0);
		alphabuf[loopc] = qRgb(r,g,b);
	    }
	}

	loopc = 0;
	if ( x1&1 ) {
	    QRgb rgb = alphabuf[loopc++];
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx )
		*myptr++ = (*myptr & 0xf0) |
		(gfx_screen->alloc( qRed(rgb), qGreen(rgb), qBlue(rgb) ) & 0x0f);
	    else
#endif
	    *myptr++ = (*myptr & 0x0f) |
		(gfx_screen->alloc( qRed(rgb), qGreen(rgb), qBlue(rgb) ) << 4);
	}

        for ( ;loopc < w-1; loopc += 2 ) {
	    QRgb rgb1 = alphabuf[loopc];
	    QRgb rgb2 = alphabuf[loopc+1];
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx ) {
		rgb2 = alphabuf[loopc];
		rgb1 = alphabuf[loopc+1];
	    }
#endif
	    *myptr++ = gfx_screen->alloc( qRed(rgb1), qGreen(rgb1), qBlue(rgb1) ) |
		(gfx_screen->alloc( qRed(rgb2), qGreen(rgb2), qBlue(rgb2) ) << 4);
	}

	if ( !(x2&1) ) {
	    QRgb rgb = alphabuf[w-1];
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
	    if ( is_screen_gfx )
		*myptr++ = (*myptr & 0x0f) |
		     (gfx_screen->alloc( qRed(rgb), qGreen(rgb), qBlue(rgb) ) << 4);
	    else
#endif
	    *myptr = (*myptr & 0xf0) |
		gfx_screen->alloc( qRed(rgb), qGreen(rgb), qBlue(rgb) );
	}

    } else if ( depth == 1 ) {
	if (srctype==SourceImage) {
	    static int warn;
	    if ( warn++ < 5 )
		qDebug( "bitmap alpha-image not implemented" );
	    hImageLineUnclipped( x1, x2, l, srcdata, FALSE );
	} else {
	    bool black = qGray(clut[srccol]) < 128;
	    for (int x=x1; x<=x2; x++) {
		if ( *alphas++ >= 64 ) { // ### could be configurable (monoContrast)
		    uchar* lx = l+(x>>3);
		    uchar b = 1<<(x&0x7);
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
		    if ( is_screen_gfx )
			b = 0x80>>(x&0x7);
#endif
		    if ( !(*lx&b) != black )
			*lx ^= b;
		}
	    }
	}
    }
}

/*!
\fn void QGfxRaster<depth,type>::fillRect( int rx,int ry,int w,int h )

\internal

Draw a filled rectangle in the current brush color from \a rx,\a ry to \a w,
\a h.
*/

//widget coordinates
template <const int depth, const int type>
void QGfxRaster<depth,type>::fillRect( int rx,int ry,int w,int h )
{
    if (!ncliprect)
	return;
    GFX_START(QRect(rx+xoffs, ry+yoffs, w+1, h+1))

    if((*gfx_optype))
	sync();
    (*gfx_optype)=0;
    setAlphaType(IgnoreAlpha);
    if ( w <= 0 || h <= 0 ) {
	GFX_END
	return;
    }

#ifdef QWS_EXPERIMENTAL_FASTPATH
    // ### fix for 8bpp
    // This seems to be reliable now, at least for 16bpp

    if (ncliprect == 1 && cbrush.style()==SolidPattern) {
	// Fast path
	if(depth==16) {
	    useBrush();
	    int x1,y1,x2,y2;
	    rx+=xoffs;
	    ry+=yoffs;
	    x2=rx+w-1;
	    y2=ry+h-1;
	    if(rx>cliprect[0].right() || ry>cliprect[0].bottom() ||
	       x2<cliprect[0].left() || y2<cliprect[0].top()) {
		GFX_END
	        return;
	    }
	    x1=cliprect[0].left() > rx ? cliprect[0].left() : rx;
	    y1=cliprect[0].top() > ry ? cliprect[0].top() : ry;
	    x2=cliprect[0].right() > x2 ? x2 : cliprect[0].right();
	    y2=cliprect[0].bottom() > y2 ? y2 : cliprect[0].bottom();
	    w=(x2-x1)+1;
	    h=(y2-y1)+1;

	    if(w<1 || h<1) {
		GFX_END
		return;
	    }

	    unsigned short int * myptr=(unsigned short int *)scanLine(y1);

	// 64-bit writes make a /big/ difference from 32-bit ones,
	// at least on my (fast AGP) hardware - 856 rects/second as opposed
	// to 550, although MTRR makes this difference much less
	    int frontadd;
	    int backadd;
	    int count;
	    calcPacking(myptr,x1,x2,frontadd,backadd,count);

	    int loopc,loopc2;

	    PackType put;
	    put = pixel | (pixel << 16);
	    int add=linestep()/2;
	    add-=(frontadd+(count * 2)+backadd);

	    myptr=((unsigned short int *)scanLine(y1))+x1;

	    if(myrop==XorROP) {
	      for(loopc=0;loopc<h;loopc++) {
		for(loopc2=0;loopc2<frontadd;loopc2++) {
		  *(myptr)^=pixel;
		  myptr++;
		}
		for(loopc2=0;loopc2<count;loopc2++) {
		  *((PackType *)myptr)^=put;
		  myptr+=2;
		}
		for(loopc2=0;loopc2<backadd;loopc2++) {
		  *(myptr)^=pixel;
		  myptr++;
		}
		myptr+=add;
	      }
	    } else if(myrop==NotROP) {
	      for(loopc=0;loopc<h;loopc++) {
		for(loopc2=0;loopc2<frontadd;loopc2++) {
		  *(myptr)=~(*myptr);
		  myptr++;
		}
		for(loopc2=0;loopc2<count;loopc2++) {
		  *((PackType *)myptr)=~*((PackType *)myptr);
		  myptr+=2;
		}
		for(loopc2=0;loopc2<backadd;loopc2++) {
		  *(myptr)=~(*myptr);
		  myptr++;
		}
		myptr+=add;
	      }
	    } else {
	      for(loopc=0;loopc<h;loopc++) {
		for(loopc2=0;loopc2<frontadd;loopc2++)
		  *(myptr++)=pixel;
		// Duffs device.
		PackType *myptr2 = (PackType*)myptr;
		myptr += count * 2;
		PackType *end2 = (PackType*)myptr;
		switch(count%8){
		    case 0:
			while ( myptr2 != end2 ) {
			    *myptr2++ = put;
		    case 7: *myptr2++ = put;
		    case 6: *myptr2++ = put;
		    case 5: *myptr2++ = put;
		    case 4: *myptr2++ = put;
		    case 3: *myptr2++ = put;
		    case 2: *myptr2++ = put;
		    case 1: *myptr2++ = put;
			}
		}
		for(loopc2=0;loopc2<backadd;loopc2++)
		  *(myptr++)=pixel;
		myptr+=add;
	      }
	    }
	    GFX_END
	    return;
	} else if(depth==32) {
	    useBrush();
	    int x1,y1,x2,y2;
	    rx+=xoffs;
	    ry+=yoffs;
	    x2=rx+w-1;
	    y2=ry+h-1;
	    if(rx>cliprect[0].right() || ry>cliprect[0].bottom() ||
	       x2<cliprect[0].left() || y2<cliprect[0].top()) {
		GFX_END
		return;
	    }
	    x1=cliprect[0].left() > rx ? cliprect[0].left() : rx;
	    y1=cliprect[0].top() > ry ? cliprect[0].top() : ry;
	    x2=cliprect[0].right() > x2 ? x2 : cliprect[0].right();
	    y2=cliprect[0].bottom() > y2 ? y2 : cliprect[0].bottom();
	    w=(x2-x1)+1;
	    h=(y2-y1)+1;

	    if(w<1 || h<1) {
		GFX_END
		return;
	    }

	    unsigned int * myptr=(unsigned int *)scanLine(y1);

	    int frontadd;
	    int backadd;
	    int count;
	    calcPacking(myptr,x1,x2,frontadd,backadd,count);

	    int loopc,loopc2;
	    PackType put;
	    unsigned int * sp=(unsigned int *)&put;
	    *sp=pixel;
	    int add=linestep()/4;
	    add-=(frontadd+(count * 2)+backadd);

	    myptr=((unsigned int *)scanLine(y1))+x1;

	    if(myrop==XorROP) {
	      for(loopc=0;loopc<h;loopc++) {
		for(loopc2=0;loopc2<frontadd;loopc2++)
		  *(myptr++)^=pixel;
		for(loopc2=0;loopc2<count;loopc2++) {
		  *((PackType *)myptr)^=put;
		  myptr+=2;
		}
		for(loopc2=0;loopc2<backadd;loopc2++)
		  *(myptr++)^=pixel;
		myptr+=add;
	      }
	    } else if(myrop==NotROP) {
	      for(loopc=0;loopc<h;loopc++) {
		for(loopc2=0;loopc2<frontadd;loopc2++) {
		  *(myptr)=~(*myptr);
		}
		  myptr++;
		for(loopc2=0;loopc2<count;loopc2++) {
		  *((PackType *)myptr)=~( *((PackType *)myptr));
		  myptr+=2;
		}
		for(loopc2=0;loopc2<backadd;loopc2++) {
		  *(myptr)=~(*myptr);
		}
		myptr+=add;
	      }
	    } else {
	      for(loopc=0;loopc<h;loopc++) {
		for(loopc2=0;loopc2<frontadd;loopc2++)
		  *(myptr++)=pixel;
		for(loopc2=0;loopc2<count;loopc2++) {
		  *((PackType *)myptr)=put;
		  myptr+=2;
		}
		for(loopc2=0;loopc2<backadd;loopc2++)
		  *(myptr++)=pixel;
		myptr+=add;
	      }
	    }
	    GFX_END
	    return;
	} else if(depth==8) {
	    useBrush();
	    int x1,y1,x2,y2;
	    rx+=xoffs;
	    ry+=yoffs;
	    x2=rx+w-1;
	    y2=ry+h-1;
	    if(rx>cliprect[0].right() || ry>cliprect[0].bottom() ||
	       x2<cliprect[0].left() || y2<cliprect[0].top()) {
		GFX_END
	        return;
	    }
	    x1=cliprect[0].left() > rx ? cliprect[0].left() : rx;
	    y1=cliprect[0].top() > ry ? cliprect[0].top() : ry;
	    x2=cliprect[0].right() > x2 ? x2 : cliprect[0].right();
	    y2=cliprect[0].bottom() > y2 ? y2 : cliprect[0].bottom();
	    w=(x2-x1)+1;
	    h=(y2-y1)+1;

	    if(w<1 || h<1) {
		GFX_END
		return;
	    }

	    unsigned char * myptr=(unsigned char *)scanLine(y1);

	    int frontadd;
	    int backadd;
	    int count;
	    calcPacking(myptr,x1,x2,frontadd,backadd,count);

	    int loopc,loopc2;
	    PackType put;
	    if ( count )
		put = pixel | (pixel<<8) | (pixel<<16) | (pixel<<24);

	    int add=linestep();
	    add-=(frontadd+(count * 4)+backadd);

	    myptr=((unsigned char *)scanLine(y1))+x1;


	    if(myrop==XorROP) {
	      for(loopc=0;loopc<h;loopc++) {
		for(loopc2=0;loopc2<frontadd;loopc2++)
		  *(myptr++)^=pixel;
		for(loopc2=0;loopc2<count;loopc2++) {
		  *((PackType *)myptr)^=put;
		    myptr+=4;
		}
		for(loopc2=0;loopc2<backadd;loopc2++)
		  *(myptr++)^=pixel;
		myptr+=add;
	      }
	    } else if(myrop==NotROP) {
	      for(loopc=0;loopc<h;loopc++) {
		for(loopc2=0;loopc2<frontadd;loopc2++) {
		  *(myptr)=~(*myptr);
		}
		  myptr++;
		for(loopc2=0;loopc2<count;loopc2++) {
		  *((PackType *)myptr)=~( *((PackType *)myptr));
		    myptr+=4;
		}
		for(loopc2=0;loopc2<backadd;loopc2++) {
		  *(myptr)=~(*myptr);
		}
		myptr+=add;
	      }
	    } else {
	      for(loopc=0;loopc<h;loopc++) {
		for(loopc2=0;loopc2<frontadd;loopc2++)
		  *(myptr++)=pixel;
		for(loopc2=0;loopc2<count;loopc2++) {
		  *((PackType *)myptr)=put;
		    myptr+=4;
		}
		for(loopc2=0;loopc2<backadd;loopc2++)
		  *(myptr++)=pixel;
		myptr+=add;
	      }
	    }
	    GFX_END
	    return;
	} else {

	}
    }
#endif // QWS_EXPERIMENTAL_FASTPATH
    if( (cbrush.style()!=NoBrush) &&
	(cbrush.style()!=SolidPattern) ) {
	srcwidth=cbrushpixmap->width();
	srcheight=cbrushpixmap->height();
	if(cbrushpixmap->depth()==1) {
	    if(opaque) {
		setSource(cbrushpixmap);
		setAlphaType(IgnoreAlpha);
		useBrush();
		srcclut[1]=pixel;
		transclut[1]=pixel;
		QBrush tmp=cbrush;
		cbrush=QBrush(backcolor);
		useBrush();
		srcclut[0]=pixel;
		transclut[0]=pixel;
		cbrush=tmp;
	    } else {
		useBrush();
		srccol=pixel;
		srctype=SourcePen;
		setAlphaType(LittleEndianMask);
		setAlphaSource(cbrushpixmap->scanLine(0),
			       cbrushpixmap->bytesPerLine());
	    }
	} else {
	    setSource(cbrushpixmap);
	    setAlphaType(IgnoreAlpha);
	}
	tiledBlt(rx,ry,w,h);
    } else if(cbrush.style()!=NoBrush) {
	useBrush();
	rx += xoffs;
	ry += yoffs;
	// Gross clip
	if ( rx < clipbounds.left() ) {
	    w -= clipbounds.left()-rx;
	    rx = clipbounds.left();
	}
	if ( ry < clipbounds.top() ) {
	    h -= clipbounds.top()-ry;
	    ry = clipbounds.top();
	}
	if ( rx+w-1 > clipbounds.right() )
	    w = clipbounds.right()-rx+1;
	if ( ry+h-1 > clipbounds.bottom() )
	    h = clipbounds.bottom()-ry+1;
	if ( w > 0 && h > 0 )
	    for (int j=0; j<h; j++,ry++) {
		hline(rx,rx+w-1,ry); }
    }
    GFX_END
}

#ifdef GFX_CORRECT_POLYLINE_JOIN

static inline bool qt_inside_edge( const QPoint &p, const QRect &r, int edge )
{
    switch ( edge ) {
	case 0:
	    return p.x() > r.left();
	case 1:
	    return p.y() > r.top();
	case 2:
	    return p.x() < r.right();
	case 3:
	    return p.y() < r.bottom();
    }

    return FALSE;
}

static inline QPoint qt_intersect_edge( const QPoint &p1, const QPoint &p2, const QRect &r, int edge )
{
    int x=0, y=0;
    int dy = p2.y() - p1.y();
    int dx = p2.x() - p1.x();

    switch ( edge ) {
	case 0:
	    x = r.left();
	    y = p1.y() + dy * QABS(p1.x() - x) / QABS(dx);
	    break;
	case 1:
	    y = r.top();
	    x = p1.x() + dx * QABS(p1.y() - y) / QABS(dy);
	    break;
	case 2:
	    x = r.right();
	    y = p1.y() + dy * QABS(p1.x() - x) / QABS(dx);
	    break;
	case 3:
	    y = r.bottom();
	    x = p1.x() + dx * QABS(p1.y() - y) / QABS(dy);
	    break;
    }

    return QPoint(x,y);
}

static bool qt_clipLine( int &x1, int &y1, int &x2, int &y2, const QRect &clip )
{
    if ( clip.contains(x1, y1) && clip.contains(x2, y2) )
	return TRUE;

    for ( int e = 0; e < 4; e++ ) {
	if ( !qt_inside_edge( QPoint(x1, y1), clip, e ) &&
		qt_inside_edge( QPoint(x2, y2), clip, e ) ) {
	    QPoint i = qt_intersect_edge( QPoint(x1, y1), QPoint(x2, y2), clip, e );
	    x1 = i.x();
	    y1 = i.y();
	} else if ( !qt_inside_edge( QPoint(x2, y2), clip, e ) &&
		qt_inside_edge( QPoint(x1, y1), clip, e ) ) {
	    QPoint i = qt_intersect_edge( QPoint(x1, y1), QPoint(x2, y2), clip, e );
	    x2 = i.x();
	    y2 = i.y();
	} else {
	    return FALSE;
	}
    }

    return TRUE;
}

#endif // GFX_CORRECT_POLYLINE_JOIN

/*!
\fn void QGfxRaster<depth,type>::drawPolyline( const QPointArray &a,int index, int npoints )

\internal

Draw a series of lines specified by \a npoints coordinates from array \a a,
starting from \a index in the array.
*/

template <const int depth, const int type>
void QGfxRaster<depth,type>::drawPolyline( const QPointArray &a,int index, int npoints )
{
    if (!ncliprect)
	return;
    if(cpen.style()==NoPen)
	return;
    if (cpen.width() > 1) {
	drawThickPolyline( a, index, npoints );
	return;
    }

#ifndef QT_NO_QWS_CURSOR
    GFX_START(a.boundingRect())
#else
    GFX_START(clipbounds)
#endif

    if((*gfx_optype))
	sync();
    (*gfx_optype)=0;
    //int m=QMIN( index+npoints-1, int(a.size())-1 );

    int loopc;
    int end;
    end=(index+npoints) > (int)a.size() ? a.size() : index+npoints;
#ifdef GFX_CORRECT_POLYLINE_JOIN
    if ( myrop != CopyROP && npoints > 1 ) {
	gfx_storedLineBufferSize = QMAX(clipbounds.height(),clipbounds.width());
	gfx_storedLineBufferSize = QMAX(gfx_storedLineBufferSize,10);
	gfx_storedLineRd = new QPoint [gfx_storedLineBufferSize];
	gfx_storedLineWr = new QPoint [gfx_storedLineBufferSize];
	gfx_storeLine = TRUE;
	gfx_storedLineWrite = 0;
	gfx_storedLineRead = 0;
	gfx_noLineOverwrite = FALSE;
	if ( a[index] == a[end-1] ) {
	    // initialize rd buffer
	    gfx_doDraw = FALSE;
	    int x1 = a[end-2].x();
	    int y1 = a[end-2].y();
	    int x2 = a[end-1].x();
	    int y2 = a[end-1].y();
	    QRect cr = clipbounds;
	    cr.moveBy( -xoffs, -yoffs );
	    qt_clipLine( x1, y1, x2, y2, cr );
	    drawLine(x1, y1, x2, y2);
	    gfx_storedLineDir = x1 > x2 ? 1 : -1;
	    gfx_storedLineRead = gfx_storedLineDir > 0 ? 0 : gfx_storedLineWrite - 1;
	    gfx_noLineOverwrite = TRUE;
	    QPoint *tmp = gfx_storedLineWr;
	    gfx_storedLineWr = gfx_storedLineRd;
	    gfx_storedLineRd = tmp;
	    gfx_doDraw = TRUE;
	}
    }
    for(loopc=index+1;loopc<end;loopc++) {
	int x1 = a[loopc-1].x();
	int y1 = a[loopc-1].y();
	int x2 = a[loopc].x();
	int y2 = a[loopc].y();
	if ( gfx_storeLine ) {
	    QRect cr = clipbounds;
	    cr.moveBy( -xoffs, -yoffs );
	    qt_clipLine( x1, y1, x2, y2, cr );
	    gfx_storedLineWrite = 0;
	}
	drawLine(x1, y1, x2, y2);
	if ( gfx_storeLine ) {
	    gfx_storedLineDir = x1 > x2 ? 1 : -1;
	    gfx_storedLineRead = gfx_storedLineDir > 0 ? 0 : gfx_storedLineWrite - 1;
	    gfx_noLineOverwrite = TRUE;
	    QPoint *tmp = gfx_storedLineWr;
	    gfx_storedLineWr = gfx_storedLineRd;
	    gfx_storedLineRd = tmp;
	}
    }
    if ( gfx_storedLineRd )
	delete [] gfx_storedLineRd;
    if ( gfx_storedLineWr )
	delete [] gfx_storedLineWr;
    gfx_storedLineRd = 0;
    gfx_storedLineWr = 0;
    gfx_storeLine = FALSE;
#else
    for(loopc=index+1;loopc<end;loopc++) {
	drawLine(a[loopc-1].x(),a[loopc-1].y(),
		 a[loopc].x(),a[loopc].y());
    }
#endif
    GFX_END
}

/*!
\fn void QGfxRaster<depth,type>::drawPolygon( const QPointArray &pa, bool winding, int index, int npoints )

\internal

Draw a filled polygon in the current brush style, with a border in the current
pen style. The polygon is specified in array \a pa by \a npoints points from
\a index. \a winding specifies whether to use the winding fill algorithm
or the even-odd (alternative) fill algorithm.
*/

template <const int depth, const int type>
void QGfxRaster<depth,type>::drawPolygon( const QPointArray &pa, bool winding, int index, int npoints )
{
    if (!ncliprect)
	return;
    useBrush();
    GFX_START(clipbounds)
    if((*gfx_optype)!=0) {
	sync();
    }
    (*gfx_optype)=0;
    if ( cbrush.style()!=NoBrush ) {
	if ( cbrush.style()!=SolidPattern ) {
	    srcwidth=cbrushpixmap->width();
	    srcheight=cbrushpixmap->height();
	    if(cbrushpixmap->depth()==1) {
		if(opaque) {
		    setSource(cbrushpixmap);
		    setAlphaType(IgnoreAlpha);
		    useBrush();
		    srcclut[1]=pixel;
		    transclut[1]=pixel;
		    QBrush tmp=cbrush;
		    cbrush=QBrush(backcolor);
		    useBrush();
		    srcclut[0]=pixel;
		    transclut[0]=pixel;
		    cbrush=tmp;
		} else {
		    useBrush();
		    srccol=pixel;
		    srctype=SourcePen;
		    setAlphaType(LittleEndianMask);
		    setAlphaSource(cbrushpixmap->scanLine(0),
				   cbrushpixmap->bytesPerLine());
		}
	    } else {
		setSource(cbrushpixmap);
		setAlphaType(IgnoreAlpha);
	    }
	}
	scan(pa,winding,index,npoints,stitchedges);
    }
    drawPolyline(pa, index, npoints);
    if (pa[index] != pa[index+npoints-1]) {
	drawLine(pa[index].x(), pa[index].y(),
		pa[index+npoints-1].x(),pa[index+npoints-1].y());
    }
    GFX_END
}

/*!
\fn void QGfxRaster<depth,type>::processSpans( int n, QPoint* point, int* width )

\internal

This is used internally by drawPolygon (via scan()) to draw the individual
scanlines of a polygon by calling hline. \a point is an array of points
describing the horizontal lines in this scanline of the polygon,
\a n the array of points to draw, \a width the width of the scanline.
*/

// widget coords
template <const int depth, const int type>
void QGfxRaster<depth,type>::processSpans( int n, QPoint* point, int* width )
{
    while (n--) {
	if ( *width > 0 ) {
	    if ( patternedbrush && srcwidth != 0 && srcheight != 0 ) {
		unsigned char * savealphabits=alphabits;
		int offx = srcwidgetoffs.x() + brushoffs.x() + point->x();
		int offy = srcwidgetoffs.y() + brushoffs.y() + point->y();

		// from qpainter_qws.cpp
		if ( offx < 0 )
		    offx = srcwidth - -offx % srcwidth;
		else
		    offx = offx % srcwidth;
		if ( offy < 0 )
		    offy = srcheight - -offy % srcheight;
		else
		    offy = offy % srcheight;

		int rx = point->x();
		int w = *width;
		int xPos = rx;
		while ( xPos < rx + w - 1 ) {
		    int drawW = srcwidth - offx; // Cropping first column
		    if ( xPos + drawW > rx + w )    // Cropping last column
			drawW = rx + w - xPos;
		    blt( xPos, point->y(), drawW, 1, offx, offy );
		    alphabits=savealphabits;
		    xPos += drawW;
		    offx = 0;
		}
	    } else {
		int x=point->x()+xoffs;
		hline(x,x+*width-1,point->y()+yoffs);
	    }
	}
	point++;
	width++;
    }
}


static GFX_INLINE unsigned char *find_pointer_4( unsigned char * base,int x,int y,
						int w, int linestep, int &astat,
						unsigned char &ahold, bool rev
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
						 , bool reverse_endian
#endif
    )
{
    int nbits;
    int nbytes;

    if ( rev ) {
	nbits = 1 - (x+w) % 2;
	nbytes = (x+w) / 2;
    } else {
	nbits = x % 2;
	nbytes = x / 2;
    }

    unsigned char *ret = base + (y*linestep) + nbytes;
    astat = nbits;
    ahold = *ret;
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
    if ( reverse_endian )
	ahold = (ahold & 0x0f) << 4 |  (ahold & 0xf0) >> 4;
#endif

    if ( rev )
	ahold = ahold << (nbits*4);
    else
	ahold = ahold >> (nbits*4);

    return ret;
}

/*!
\fn void QGfxRaster<depth,type>::scroll( int rx,int ry,int w,int h,int sx, int sy )

\internal

This is intended for hardware optimisation - it handles the common case
of blting a rectangle a small distance within the same drawing surface
(for example when scrolling a listbox). \a rx and \a ry are the X and Y
coordinates to which the rectangle should be moved, \a sx and \a sy
are its source coordinates and \a w and \a h are its width and height.
*/

template <const int depth, const int type>
void QGfxRaster<depth,type>::scroll( int rx,int ry,int w,int h,int sx, int sy )
{
    if (!w || !h || !ncliprect)
	return;

    int dy = sy - ry;
    int dx = sx - rx;

    if (dx == 0 && dy == 0)
	return;

    GFX_START(QRect(QMIN(rx+xoffs,sx+xoffs), QMIN(ry+yoffs,sy+yoffs), w+QABS(dx)+1, h+QABS(dy)+1))

    srcbits=buffer;
    srclinestep=linestep();
    srcdepth=depth;
    srcwidth=w;
    srcheight=h;
    if(srcdepth==0)
	abort();
    srctype=SourceImage;
    setAlphaType(IgnoreAlpha);
    setSourceWidgetOffset( xoffs, yoffs );
    src_normal_palette = TRUE;
    blt(rx,ry,w,h,sx,sy);

    GFX_END
}

/*!
\fn void QGfxRaster<depth,type>::blt( int rx,int ry,int w,int h, int sx, int sy )

\internal

This corresponds to QPixmap::drawPixmap (into a QPainter with no transformation
other than a translation) or bitBlt. The source is set up using
setSource and setSourceWidgetOffset before the blt. \a rx and \a ry are the
destination coordinates, \a w and \a h the size of the rectangle to blt,
\a sx and \a sy the source coordinates relative to the source's widget offset.
In the case of a pen source sx and sy are ignored. Source and destination
can overlap and can be of arbitrary (different) depths.
*/

template <const int depth, const int type>
void QGfxRaster<depth,type>::blt( int rx,int ry,int w,int h, int sx, int sy )
{
    if ( !w || !h ) return;
    int osrcdepth=srcdepth;
    if(srctype==SourcePen) {
	srclinestep=0;//w;
	srcdepth=0;
	usePen();
    }

    rx += xoffs;
    ry += yoffs;

    QRect cursRect(rx, ry, w+1, h+1);
    GFX_START(cursRect&clipbounds);

    // Very gross clip
    if ( !clipbounds.intersects(QRect(rx,ry,w,h)) ) {
	GFX_END
	return;
    }

    //slightly tighter clip
    int leftd = clipbounds.x() - rx;
    if ( leftd > 0 ) {
	rx += leftd;
	sx += leftd;
	w -= leftd;
    }
    int topd =  clipbounds.y() - ry;
    if ( topd > 0 ) {
	ry += topd;
	sy += topd;
	h -= topd;
    }
    int rightd = rx + w - 1 - clipbounds.right();
    if ( rightd > 0 )
	w -= rightd;
    int botd = ry + h - 1 - clipbounds.bottom();
    if ( botd > 0 )
	h -= botd;

    // have we already clipped away everything necessary
    bool mustclip = ncliprect != 1;

    if((*gfx_optype)!=0)
	sync();
    (*gfx_optype)=0;

    QPoint srcoffs = srcwidgetoffs + QPoint( sx, sy );

    int dl = linestep();
    int sl = srclinestep;
    int dj = 1;
    int dry = 1;
    int tj;
    int j;
    if ( srcbits == buffer && srcoffs.y() < ry ) {
	// Back-to-front
	dj = -dj;
	dl = -dl;
	sl = -sl;
	dry = -dry;
	j = h-1;
	ry=(ry+h)-1;
	tj = -1;
    } else {
	j = 0;
	tj = h;
    }

    bool xrev = (srcbits == buffer && srcoffs.x() < rx);

    QRect cr( rx, ry, w, h );

    unsigned char *l = scanLine(ry);
    unsigned char *srcline = srcScanLine(j+srcoffs.y());
    int right = rx+w-1;

    // Fast path for 8/16/32 bit same-depth opaque blit. (ie. the common case)
    if ( srcdepth == depth && alphatype == IgnoreAlpha &&
	 pixeltype == srcpixeltype &&
	 (depth > 8 || (depth == 8 && src_normal_palette)) &&
         myrop == CopyROP ) {
	int bytesPerPixel = depth/8;
	if ( mustclip ) {
	    if ( xrev ) {
		for (; j!=tj; j+=dj,ry+=dry,l+=dl,srcline+=sl) {
		    bool plot = inClip(right,ry,&cr);
		    int x2=right;
		    for (;;) {
			int x = cr.left();
			if ( x < rx ) {
			    x = rx;
			    if ( x2 < x ) break;
			}
			if (plot) {
			    unsigned char *srcptr=srcline+(x-rx+srcoffs.x())*bytesPerPixel;
			    unsigned char *destptr = l + x*bytesPerPixel;
			    memmove(destptr, srcptr, (x2-x+1) * bytesPerPixel );
			}
			if ( x <= rx )
			    break;
			x2=x-1;
			plot=inClip(x2,ry,&cr,plot);
		    }
		}
	    } else {
		for (; j!=tj; j+=dj,ry+=dry,l+=dl,srcline+=sl) {
		    bool plot = inClip(rx,ry,&cr);
		    int x=rx;
		    for (;;) {
			int x2 = cr.right();
			if ( x2 > right ) {
			    x2 = right;
			    if ( x2 < x ) break;
			}
			if (plot) {
			    unsigned char *srcptr=srcline+(x-rx+srcoffs.x())*bytesPerPixel;
			    unsigned char *destptr = l + x*bytesPerPixel;
			    memmove(destptr, srcptr, (x2-x+1) * bytesPerPixel );
			}
			x=x2+1;
			if ( x > right )
			    break;
			plot=inClip(x,ry,&cr,plot);
		    }
		}
	    }
	} else {
	    unsigned char *srcptr = srcline + srcoffs.x()*bytesPerPixel;
	    unsigned char *destptr = l + rx*bytesPerPixel;
	    int bytes = w * bytesPerPixel;
	    for (; j!=tj; j+=dj,destptr+=dl,srcptr+=sl) {
		memmove( destptr, srcptr, bytes );
	    }
	}
    } else {
	if ( alphatype == InlineAlpha || alphatype == SolidAlpha ||
	     alphatype == SeparateAlpha ) {
	    alphabuf = new unsigned int[w];
	}

	// reverse will only ever be true if the source and destination
	// are the same buffer.
	bool reverse = srcoffs.y()==ry && rx>srcoffs.x() &&
			srctype==SourceImage && srcbits == buffer;

	if ( alphatype == LittleEndianMask || alphatype == BigEndianMask ) {
	    // allows us to optimise GET_MASK a little
	    amonolittletest = FALSE;
	    if( (alphatype==LittleEndianMask && !reverse) ||
		(alphatype==BigEndianMask && reverse) ) {
		amonolittletest = TRUE;
	    }
	}

	unsigned char *srcptr = 0;
	for (; j!=tj; j+=dj,ry+=dry,l+=dl,srcline+=sl) {
	    bool plot = mustclip ? inClip(rx,ry,&cr) : TRUE;
	    int x=rx;
	    for (;;) {
		int x2 = cr.right();
		if ( x2 > right ) {
		    x2 = right;
		    if ( x2 < x ) break;
		}
		if (plot) {
		    if ( srctype == SourceImage ) {
			if ( srcdepth == 1) {
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
			if  ( srcbits == qt_screen->base() )
				 src_little_endian =  !src_little_endian;
#endif
			    srcptr=find_pointer(srcbits,(x-rx)+srcoffs.x(),
					 j+srcoffs.y(), x2-x, srclinestep,
					 monobitcount, monobitval,
					 !src_little_endian, reverse);
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
			if  ( srcbits == qt_screen->base() )
				 src_little_endian =  !src_little_endian;
#endif
			} else if ( srcdepth == 4) {
			    srcptr = find_pointer_4(srcbits,(x-rx)+srcoffs.x(),
					 j+srcoffs.y(), x2-x, srclinestep,
					 monobitcount, monobitval, reverse
#ifdef QT_QWS_EXPERIMENTAL_REVERSE_BIT_ENDIANNESS
						    , srcbits == qt_screen->base()
#endif
				);
			} else if ( reverse )
			    srcptr = srcline + (x2-rx+srcoffs.x())*srcdepth/8;
			else
			    srcptr = srcline + (x-rx+srcoffs.x())*srcdepth/8;
		    }
		    switch ( alphatype ) {
		      case LittleEndianMask:
		      case BigEndianMask:
			maskp=find_pointer(alphabits,(x-rx)+srcoffs.x(),
					   j+srcoffs.y(), x2-x, alphalinestep,
					   amonobitcount,amonobitval,
					   alphatype==BigEndianMask, reverse);
			// Fall through
		      case IgnoreAlpha:
			hImageLineUnclipped(x,x2,l,srcptr,reverse);
			break;
		      case InlineAlpha:
		      case SolidAlpha:
			hAlphaLineUnclipped(x,x2,l,srcptr,0);
			break;
		      case SeparateAlpha:
			// Separate alpha table
			unsigned char * alphap=alphabits
						+((j+srcoffs.y())*alphalinestep)
						+(x-rx)+srcoffs.x();
			hAlphaLineUnclipped(x,x2,l,srcptr,alphap);
		    }
		}
		x=x2+1;
		if ( x > right )
		    break;
		if ( mustclip )
		    plot=inClip(x,ry,&cr,plot);
	    }
	}
	if ( alphabuf ) {
	    delete [] alphabuf;
	    alphabuf = 0;
	}
    }

    srcdepth=osrcdepth;
    GFX_END
}

/*!
\fn void QGfxRaster<depth,type>::stretchBlt( int rx,int ry,int w,int h,
					 int sw,int sh )

\internal

This is similar to blt() but allows the source rectangle to be a different
size to the destination - the source is expanded or shrunk as necessary
to fit the destination. The source and destination cannot overlap.
Note that since the software implementation uses floating point it will
be slow on embedded processors without an FPU. Qt/Embedded uses
stretchBlt to speed up QPixmap::xForm. \a rx, \a ry, \a w and \a h
specify the destination rectangle, \a sw and \a sh specify the size
of the source; its x and y position are assumed to be 0.
*/

#if !defined(QT_NO_MOVIE) || !defined(QT_NO_TRANSFORMATIONS) || !defined(QT_NO_PIXMAP_TRANSFORMATION)
template <const int depth, const int type>
void QGfxRaster<depth,type>::stretchBlt( int rx,int ry,int w,int h,
					 int sw,int sh )
{
    QRect cr;
    unsigned char * srcptr;
    unsigned char * data = new unsigned char [(w*depth)/8];
    rx+=xoffs;
    ry+=yoffs;
    //int sy=0;
    unsigned char * l=scanLine(ry);
    unsigned char * sl=data;
    double xfac=sw;
    xfac=xfac/((double)w);
    double yfac=sh;
    yfac=yfac/((double)h);

    int loopc;

    // We don't allow overlapping stretchblt src and destination

    int mulfac;
    if(srcdepth==32) {
	mulfac=4;
    } else if(srcdepth==24) {
	mulfac=3;
    } else if(srcdepth==16) {
	mulfac=2;
    } else if(srcdepth==8) {
	mulfac=1;
    } else {
	mulfac=0;
	qDebug("Can't cope with stretchblt source depth %d",mulfac);
	return;
    }

    QPoint srcoffs = srcwidgetoffs; // + QPoint( sx, sy );

    QRect cursRect(rx, ry, w+1, h+1);
    /* ???
    if (buffer_offset >= 0 && src_buffer_offset >= 0) {
	cursRect = QRect( QMIN(rx,srcoffs.x()), QMIN(ry,srcoffs.y()),
			QMAX(w, sw)+QABS(rx - srcoffs.x())+1,
			QMAX(h, sh)+QABS(ry - srcoffs.y())+1 );
    } else if (src_buffer_offset >= 0) {
	cursRect = QRect(srcoffs.x(), srcoffs.y(), sw+1, sh+1);
    }
    */

    GFX_START(cursRect);
    if((*gfx_optype))
	sync();
    (*gfx_optype)=0;
    int osrcdepth=srcdepth;
    int pyp=-1;

    for(int j=0;j<h;j++,ry++,l+=linestep()) {
	bool plot=inClip(rx,ry,&cr);
	int x=rx;

	int yp=(int) ( ( (double) j )*yfac );

	if(yp!=pyp) {
	    for(loopc=0;loopc<w;loopc++) {
		int sp=(int) ( ( (double) loopc )*xfac );
		unsigned char * p=srcScanLine(yp)+(sp*mulfac);
		if(depth==32) {
		    unsigned int val=get_value_32(srcdepth,&p);
		    unsigned int * dp=(unsigned int *)data;
		    *(dp+loopc)=val;
		} else if(depth==24) {
		    unsigned int val=get_value_32(srcdepth,&p);
		    unsigned char* dp=(unsigned char *)data;
		    gfxSetRgb24( dp+loopc*3, val);
		} else if(depth==16) {
		    unsigned int val=get_value_16(srcdepth,&p);
		    unsigned short int * dp=(unsigned short int *)data;
		    *(dp+loopc)=val;
		} else if(depth==8) {
		    unsigned int val=get_value_8(srcdepth,&p);
		    *(data+loopc)=val;
		} else {
		    qDebug("Can't cope with stretchblt depth %d",depth);
		    GFX_END
		    delete [] data;
		    return;
		}
	    }
	    pyp=yp;
	}

	srcdepth=depth;
	for (;;) {
	    int x2 = cr.right();
	    if ( x2 >= rx+w-1 ) {
		srcptr=sl;
		srcptr+=(((x-rx)+srcoffs.x())*mulfac);
		if (plot) {
		    hImageLineUnclipped(x,rx+w-1,l,srcptr,FALSE);
		}
		break;
	    } else {
		srcptr=sl;
		srcptr+=(((x-rx)+(srcoffs.x()))*mulfac);
		if (plot) {
			hImageLineUnclipped(x,x2,l,srcptr,FALSE);
		}
		x=x2+1;
		plot=inClip(x,ry,&cr,plot);
	    }
	}
	srcdepth=osrcdepth;
    }
    delete [] data;
    GFX_END
}
#endif

/*!
\fn void QGfxRaster<depth,type>::tiledBlt( int rx,int ry,int w,int h )

\internal

Like scroll(), this is intended as a candidate for hardware acceleration
- it's a special case of blt where the source can be a different size
to the destination and is tiled across the destination. \a rx and \a ry
specify the x and y position of the rectangle to fill, \a w and \a h
its size.
*/

template <const int depth, const int type>
void QGfxRaster<depth,type>::tiledBlt( int rx,int ry,int w,int h )
{
    if ( srcwidth == 0 || srcheight == 0 )
	return;
    GFX_START(QRect(rx+xoffs, ry+yoffs, w+1, h+1))

    useBrush();
    unsigned char * savealphabits=alphabits;

    int offx = srcwidgetoffs.x() + brushoffs.x();
    int offy = srcwidgetoffs.y() + brushoffs.y();

    // from qpainter_qws.cpp
    if ( offx < 0 )
        offx = srcwidth - -offx % srcwidth;
    else
        offx = offx % srcwidth;
    if ( offy < 0 )
        offy = srcheight - -offy % srcheight;
    else
        offy = offy % srcheight;

    int yPos, xPos, drawH, drawW, yOff, xOff;
    yPos = ry;
    yOff = offy;
    while( yPos < ry + h ) {
        drawH = srcheight - yOff;    // Cropping first row
        if ( yPos + drawH > ry + h )        // Cropping last row
            drawH = ry + h - yPos;
        xPos = rx;
        xOff = offx;
        while( xPos < rx + w ) {
            drawW = srcwidth - xOff; // Cropping first column
            if ( xPos + drawW > rx + w )    // Cropping last column
                drawW = rx + w - xPos;
	    blt(xPos, yPos, drawW, drawH,xOff,yOff);
	    alphabits=savealphabits;
            xPos += drawW;
            xOff = 0;
        }
        yPos += drawH;
        yOff = 0;
    }
    GFX_END
}

/*
This takes the currently-set brush and stores its color value in the
variable pixel for drawing points, lines and rectangles.
*/

template <const int depth, const int type>
GFX_INLINE void QGfxRaster<depth,type>::useBrush()
{
#ifndef QT_NO_QWS_REPEATER
    if (isScreenGfx()) {
	if(depth==32||depth==24) {
	    pixel=(cbrush.color().red() << 16) | (cbrush.color().green() << 8)
		  | (cbrush.color().blue());
	} else if(depth==16) {
	    pixel=((cbrush.color().red() >> 3) << 11) |
		  ((cbrush.color().green() >> 2) << 5) |
		  (cbrush.color().blue() >> 3);
	} else if(depth==1) {
	    pixel=qGray(cbrush.color().red(),cbrush.color().green(),
			cbrush.color().blue()) < 128 ? 1 : 0;
	} else {
	    pixel=gfx_screen->alloc(cbrush.color().red(),
				    cbrush.color().green(),
				    cbrush.color().blue());
	}
    } else
#endif
	pixel = cbrush.color().pixel();
}

/*
This takes the currently-set pen and stores its color value in the
variable pixel for drawing points, lines and rectangles.
*/

template <const int depth, const int type>
GFX_INLINE void QGfxRaster<depth,type>::usePen()
{
#ifndef QT_NO_QWS_REPEATER
    if (isScreenGfx()) {
	if(depth==32||depth==24) {
	    pixel=(cpen.color().red() << 16) | (cpen.color().green() << 8)
		  | (cpen.color().blue());
	} else if(depth==16) {
	    pixel=((cpen.color().red() >> 3) << 11) |
		  ((cpen.color().green() >> 2) << 5) |
		  (cpen.color().blue() >> 3);
	} else if(depth==1) {
	    pixel=qGray(cpen.color().red(),cpen.color().green(),
			cpen.color().blue()) < 128 ? 1 : 0;
	} else {
	    pixel=gfx_screen->alloc(cpen.color().red(),
				    cpen.color().green(),
				    cpen.color().blue());
	}
    } else
#endif
	pixel = cpen.color().pixel();
}

/*!
  \class QScreen qgfx_qws.h
  \ingroup qws
  \brief The QScreen class and its descendants manage the framebuffer and
  palette.

  QScreens act as factories for the screen cursor and QGfx's. QLinuxFbScreen
  manages a Linux framebuffer; accelerated drivers subclass QLinuxFbScreen.
  There can only be one screen in a Qt/Embedded application.
*/

/*!
\fn QScreen::initDevice()
This function is called by the Qt/Embedded server when initializing
the framebuffer. Accelerated drivers use it to set up the graphics card.
*/

/*!
\fn QScreen::connect( const QString &displaySpec )
This function is called by every Qt/Embedded application on startup.
It maps in the framebuffer and in the accelerated drivers the graphics
card control registers. \a displaySpec has the following syntax:
<p>
<tt>[gfx driver][:driver specific options][:display number]</tt>
<p>
for example if you want to use the mach64 driver on fb1 as display 2:
<p>
<tt>Mach64:/dev/fb1:2</tt>
<p>
\a displaySpec is passed in via the QWS_DISPLAY environment variable
or the -display command line parameter.
*/

/*!
\fn QScreen::disconnect()
This function is called by every Qt/Embedded application just
before exitting; it's normally used to unmap the framebuffer.
*/

/*!
\fn QScreen::setMode(int, int, int)
This function can be used to set the framebuffer width, height and
depth. It's currently unused.
*/

/*!
\fn QScreen::blank(bool on)
If \a on is true, blank the screen. Otherwise unblank it.
*/

/*!
\fn QScreen::pixmapOffsetAlignment()
Returns the value in bits to which the start address of pixmaps held in
graphics card memory should be aligned. This is only useful for accelerated
drivers. By default the value returned is 64 but it can be overridden
by individual accelerated drivers.
*/

/*!
\fn QScreen::pixmapLinestepAlignment()
Returns the value in bits to which individual scanlines of pixmaps held in
graphics card memory should be aligned. This is only useful for accelerated
drivers. By default the value returned is 64 but it can be overridden
by individual accelerated drivers.
*/

/*!
\fn QScreen::width() const
Gives the width in pixels of the framebuffer.
*/

/*!
\fn QScreen::height() const
Gives the height in pixels of the framebuffer.
*/

/*!
\fn QScreen::depth() const
Gives the depth in bits per pixel of the framebuffer. This is the number
of bits each pixel takes up rather than the number of significant bits,
so 24bpp and 32bpp express the same range of colors (8 bits of
red, green and blue)
*/

/*!
\fn QScreen::pixmapDepth() const
Gives the preferred depth for pixmaps. By default this is the same
as the screen depth, but for the VGA16 driver it's 8bpp.
*/

/*!
\fn QScreen::linestep() const
Returns the length in bytes of each scanline of the framebuffer.
*/

/*!
  \fn QScreen::deviceWidth() const
Gives the full width of the framebuffer device, as opposed to the
width which Qt/Embedded will actually use. These can differ if the
display is centered within the framebuffer.
*/

/*!
  \fn QScreen::deviceHeight() const
Gives the full height of the framebuffer device, as opposed to the
height which Qt/Embedded will actually use. These can differ if the
display is centered within the framebuffer.
*/

/*!
  \fn QScreen::base() const
Returns a pointer to the start of the framebuffer.
*/

/*!
  \fn QScreen::cache(int,int)
This function is used to store pixmaps in graphics memory for the
use of the accelerated drivers. See QLinuxFbScreen (where the cacheing
is implemented) for more information.
*/

/*!
  \fn QScreen::uncache(uchar *)
This function is called on pixmap destruction to remove them from
graphics card memory.
*/

/*!
  \fn QScreen::screenSize() const
Returns the size in bytes of the screen. This is always located at
the beginning of framebuffer memory (i.e. at base()).
*/

/*!
  \fn QScreen::totalSize() const
Returns the size in bytes of available graphics card memory, including the
screen. Offscreen memory is only used by the accelerated drivers.
*/

// Unaccelerated screen/driver setup. Can be overridden by accelerated
// drivers

/*!
  \fn QScreen::QScreen( int display_id )
  Create a screen; the \a display_id is the number of the Qt/Embedded server
  to connect to.
*/

/*!
  \fn QScreen::clut()
  Returns the screen's color lookup table (color palette). This is only
  valid in paletted modes (8bpp and lower).
*/

/*!
  \fn QScreen::numCols()
  Returns the number of entries in the color table returned by clut().
*/

QScreen::QScreen( int display_id )
{
    pixeltype=QGfx::NormalPixel;
    data = 0;
    displayId = display_id;
    initted=FALSE;
    entryp=0;
    clearCacheFunc = 0;
    grayscale = FALSE;
    screencols = 0;
}

/*!
  Destroys a QScreen
*/

QScreen::~QScreen()
{
}

/*!
  Called by the Qt/Embedded server on shutdown; never called by
  a Qt/Embedded client. This is intended to support graphics card specific
  shutdown; the unaccelerated implementation simply hides the mouse cursor.
*/

void QScreen::shutdownDevice()
{
#ifndef QT_NO_QWS_CURSOR
    qt_screencursor->hide();
#endif
}

extern bool qws_accel; //in qapplication_qws.cpp

/*!
  Returns a QGfx (normally a QGfxRaster) initialized to point to the screen,
  with an origin at 0,0 and a clip region covering the whole screen.
*/

QGfx * QScreen::screenGfx()
{
    QGfx * ret=createGfx(data,w,h,d,lstep);
    if(d<=8) {
	ret->setClut(clut(),numCols());
    }
    return ret;
}

/*!
  \fn int QScreen::pixelType() const
  Returns an integer (taking the same values as QGfx::PixelType)
  that specifies the pixel storage format of the screen.
 */

/*!
  Given an RGB value \a r \a g \a b, return an index which is the closest
  match to it in the screen's palette. Used in paletted modes only.
*/

int QScreen::alloc(unsigned int r,unsigned int g,unsigned int b)
{
    int ret = 0;
    if ( d == 8 ) {
	if ( grayscale )
	    return qGray( r, g, b );
	// First we look to see if we match a default color
	QRgb myrgb=qRgb(r,g,b);
	int pos= (r + 25) / 51 * 36 + (g + 25) / 51 * 6 + (b + 25) / 51;
	if ( simple_8bpp_alloc || screenclut[pos] == myrgb || !initted ) {
	    return pos;
	}

	// search for nearest color
	unsigned int mindiff = 0xffffffff;
	unsigned int diff;
	int dr,dg,db;

	for ( int loopc = 0; loopc < 256; loopc++ ) {
	    dr = qRed(screenclut[loopc]) - r;
	    dg = qGreen(screenclut[loopc]) - g;
	    db = qBlue(screenclut[loopc]) - b;
	    diff = dr*dr + dg*dg + db*db;

	    if ( diff < mindiff ) {
		ret = loopc;
		if ( !diff )
		    break;
		mindiff = diff;
	    }
	}
    } else if ( d == 4 ) {
	ret = qGray( r, g, b ) >> 4;
    } else if ( d == 1 && simple_8bpp_alloc ) {
	ret = qGray( r, g, b ) > 128;
    } else {
	qFatal( "cannot alloc %dbpp color", d );
    }

    return ret;
}

/*!
This is used to initialize the software cursor - \a end_of_location
points to the address after the area where the cursor image can be stored.
\a init is true for the first application this method is called from
(the Qt/Embedded server), false otherwise.
*/

int QScreen::initCursor(void* end_of_location, bool init)
{
    /*
      The end_of_location parameter is unusual: it's the address
      after the cursor data.
    */
#ifndef QT_NO_QWS_CURSOR
    qt_sw_cursor=TRUE;
    // ### until QLumpManager works Ok with multiple connected clients,
    // we steal a chunk of shared memory
    SWCursorData *data = (SWCursorData *)end_of_location - 1;
    qt_screencursor=new QScreenCursor();
    qt_screencursor->init( data, init );
    return sizeof(SWCursorData);
#else
    return 0;
#endif
}

/*!
  Saves the state of the graphics card - used so that, for instance,
  the palette can be restored when switching between linux virtual
  consoles. Hardware QScreen descendants should save register state
  here if necessary if switching between virtual consoles (for
  example to/from X) is to be permitted.
*/

void QScreen::save()
{
}

/*!
  Restores the state of the graphics card from a previous save()
*/

void QScreen::restore()
{
}

void QScreen::blank(bool)
{
}

/*!
  Sets an entry in the color palette.
*/

void QScreen::set(unsigned int, unsigned int, unsigned int, unsigned int)
{
}

/*!
\fn bool QScreen::supportsDepth(int d) const
Returns true if the screen supports a particular color depth \a d.
Possible values are 1,4,8,16 and 32.
*/

bool QScreen::supportsDepth(int d) const
{
    if ( FALSE ) {
	//Just to simplify the ifdeffery
#ifndef QT_NO_QWS_DEPTH_1
    } else if(d==1) {
	return TRUE;
#endif
#ifndef QT_NO_QWS_DEPTH_4
    } else if(d==4) {
	return TRUE;
#endif
#ifndef QT_NO_QWS_DEPTH_8
    } else if(d==8) {
	return TRUE;
#endif
#ifndef QT_NO_QWS_DEPTH_16
    } else if(d==16) {
	return TRUE;
#endif
#ifndef QT_NO_QWS_DEPTH_24
    } else if(d==24) {
	return TRUE;
#endif
#ifndef QT_NO_QWS_DEPTH_32
    } else if(d==32) {
	return TRUE;
#endif
    }
    return FALSE;
}

// explicit template instaniation
#ifndef QT_NO_QWS_DEPTH_1
template class QGfxRaster<1,0>;
#endif
#ifndef QT_NO_QWS_DEPTH_4
template class QGfxRaster<4,0>;
#endif
#ifndef QT_NO_QWS_DEPTH_8
template class QGfxRaster<8,0>;
#endif
#ifndef QT_NO_QWS_DEPTH_16
template class QGfxRaster<16,0>;
#endif
#ifndef QT_NO_QWS_DEPTH_24
template class QGfxRaster<24,0>;
#endif
#ifndef QT_NO_QWS_DEPTH_32
template class QGfxRaster<32,0>;
#endif

/*!
\fn Qfx * QScreen::createGfx(unsigned char * bytes,int w,int h,int d, int linestep)
Creates a gfx on an arbitrary buffer \a bytes, width \a w and height \a h in
pixels, depth \a d and \a linestep (length in bytes of each line in the
buffer). Accelerated drivers can check to see if \a bytes points into
graphics memory and create an accelerated Gfx.
*/

QGfx * QScreen::createGfx(unsigned char * bytes,int w,int h,int d, int linestep)
{
    QGfx* ret;
    if ( FALSE ) {
	//Just to simplify the ifdeffery
#ifndef QT_NO_QWS_DEPTH_1
    } else if(d==1) {
	ret = new QGfxRaster<1,0>(bytes,w,h);
#endif
#ifndef QT_NO_QWS_DEPTH_4
    } else if(d==4) {
	ret = new QGfxRaster<4,0>(bytes,w,h);
#endif
#ifndef QT_NO_QWS_DEPTH_8
    } else if(d==8) {
	ret = new QGfxRaster<8,0>(bytes,w,h);
#endif
#ifndef QT_NO_QWS_DEPTH_16
    } else if(d==16) {
	ret = new QGfxRaster<16,0>(bytes,w,h);
#endif
#ifndef QT_NO_QWS_DEPTH_24
    } else if(d==24) {
	ret = new QGfxRaster<24,0>(bytes,w,h);
#endif
#ifndef QT_NO_QWS_DEPTH_32
    } else if(d==32) {
	ret = new QGfxRaster<32,0>(bytes,w,h);
#endif
    } else {
	qFatal("Can't drive depth %d",d);
	ret = 0; // silence gcc
    }
    ret->setLineStep(linestep);
    return ret;
}

/*!
\fn bool QScreen::onCard(unsigned char * p) const
Returns true if the buffer pointed to by \a p is within graphics card
memory, false if it's in main RAM.
*/

bool QScreen::onCard(unsigned char * p) const
{
    long t=(unsigned long)p;
    long bmin=(unsigned long)data;
    if ( t < bmin )
	return FALSE;
    if( t >= bmin+mapsize )
	return FALSE;
    return TRUE;
}

/*!
\fn bool QScreen::onCard(unsigned char * p, ulong& offset) const
\overload
This checks whether the buffer specified by \a p is on the card
(as per the other version of onCard) and returns an offset in bytes
from the start of graphics card memory in \a offset if it is.
*/

bool QScreen::onCard(unsigned char * p, ulong& offset) const
{
    long t=(unsigned long)p;
    long bmin=(unsigned long)data;
    if ( t < bmin )
	return FALSE;
    long o = t - bmin;
    if ( o >= mapsize )
	return FALSE;
    offset = o;
    return TRUE;
}

/*
#if !defined(QT_NO_QWS_REPEATER)
    { "Repeater", qt_get_screen_repeater, 0 },
#endif
#if defined(QT_QWS_EE)
    { "EE", qt_get_screen_ee, 0 },
#endif

*/

/*
Given a display_id (number of the Qt/Embedded server to connect to)
and a spec (e.g. Mach64:/dev/fb0) return a QScreen-descendant.
The QGfxDriverFactory is queried for a suitable driver and, if found,
asked to create a driver.
People writing new graphics drivers should either hook their own
QScreen-descendant into QGfxDriverFactory or use the QGfxDriverPlugin
to make a dynamically loadable driver.
*/

QScreen *qt_get_screen( int display_id, const char *spec )
{
    QString displaySpec( spec );
    QString driver = displaySpec;
    int colon = displaySpec.find( ':' );
    if ( colon >= 0 )
	driver.truncate( colon );

    bool foundDriver = FALSE;
    QString driverName = driver;

#ifndef QT_NO_STRINGLIST
    QStringList driverList = QGfxDriverFactory::keys();
    QStringList::Iterator it;
    for ( it = driverList.begin(); it != driverList.end(); ++it ) {
	if ( driver.isEmpty() || QString( *it ) == driver ) {
	    driverName = *it;
#endif
	    qt_screen = QGfxDriverFactory::create( driverName, display_id );
	    if ( qt_screen ) {
		foundDriver = TRUE;
		if ( qt_screen->connect( spec ) ) {
		    return qt_screen;
		} else {
		    delete qt_screen;
		    qt_screen = 0;
		}
	    }
#ifndef QT_NO_STRINGLIST
	}
    }
#endif

    if ( driver.isNull() )
	qFatal( "No suitable driver found" );
    else if ( foundDriver )
	qFatal( "%s: driver cannot connect", driver.latin1() );
    else
	qFatal( "%s: driver not found", driver.latin1() );

    return 0;
}
