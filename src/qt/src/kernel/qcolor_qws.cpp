/****************************************************************************
**
** Implementation of QColor class for FB
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

#include "qcolor.h"
#include "qcolor_p.h"
#include "string.h"
#include "qpaintdevice.h"
#include "qapplication.h"
#include "qgfx_qws.h"
#include "qwsdisplay_qws.h"

/*****************************************************************************
  QColor static member functions
 *****************************************************************************/

int QColor::maxColors()
{
    return 256;
}

int QColor::numBitPlanes()
{
    return 32;
}

void QColor::initialize()
{
    if ( color_init )				// already initialized
	return;
    color_init = TRUE;
    if (QPaintDevice::qwsDisplay()->depth() <= 8)
	colormodel = d8;
    else
	colormodel = d32;
}

void QColor::cleanup()
{
    if ( !color_init )
	return;
    color_init = FALSE;
}

#if 0
// Return a value for how close a is to b
// Lower is better
static inline int match(QRgb a,QRgb b)
{
    int ret;

#ifndef QT_NO_QWS_DEPTH_8
    int h1,s1,v1;
    int h2,s2,v2;
    /*
    QColor tmp1(a);
    QColor tmp2(b);
    tmp1.hsv(&h1,&s1,&v1);
    tmp2.hsv(&h2,&s2,&v2);
    */
    h1=qRed(a);
    s1=qGreen(a);
    v1=qBlue(a);
    h2=qRed(b);
    s2=qGreen(b);
    v2=qBlue(b);
    ret=abs(h1-h2);
    ret+=abs(s1-s2);
    ret+=abs(v1-v2);
#else
    ret=abs(qGray(a)-qGray(b));
#endif

    return ret;
}

inline unsigned int closestMatch(int r,int g,int b)
{
    QRgb * clut=qt_screen->clut();
    int clutcols=qt_screen->numCols();
    if ( r>255 || g>255 || b>255 || r<0 || g<0 || b<0 )
	abort();

    QRgb tomatch=qRgb(r,g,b);
    int loopc;
    unsigned int hold=0xfffff;
    unsigned int tmp;
    int pos=0;
    for(loopc=0;loopc<clutcols;loopc++) {
	tmp=match(clut[loopc],tomatch);
	if(tmp<hold) {
	    hold=tmp;
	    pos=loopc;
	}
    }
    return pos;
}
#endif


/*****************************************************************************
  QColor member functions
 *****************************************************************************/

uint QColor::alloc()
{
// These macros mazimize optimizations even on dumb compilers.

#define GET \
    const int r = qRed(d.argb);\
    const int g = qGreen(d.argb);\
    const int b = qBlue(d.argb);

    switch (qt_screen->depth()) {
      case 1: {
	GET
	return d.d8.pix = qGray(r,g,b) < 128 ? 1 : 0;
#if !defined( QT_NO_IMAGE_16_BIT ) || !defined( QT_NO_QWS_DEPTH_16 )
      } case 16: {
	return d.d32.pix = qt_convRgbTo16(d.argb);
#endif
      } case 24:
        case 32: {
	GET
	const int red_shift = 16;
	const int green_shift = 8;
	const int red_mask   = 0xff0000;
	const int green_mask = 0x00ff00;
	const int blue_mask  = 0x0000ff;
	const int tg = g << green_shift;
#ifndef QT_NO_QWS_DEPTH_32_BGR
	if ( qt_screen->pixelType() == QGfx::BGRPixel ) {
	    const int tb = b << red_shift;
	    d.d32.pix = (r & blue_mask) | (tg & green_mask) | (tb & red_mask);
	} else
#endif
	{
	    const int tr = r << red_shift;
	    d.d32.pix = (b & blue_mask) | (tg & green_mask) | (tr & red_mask);
	}
	return d.d32.pix |= 0xff000000;
     } default: {
	GET
	return d.d8.pix=qt_screen->alloc(r,g,b);
      }
    }
}

void QColor::setSystemNamedColor( const QString& name )
{
    // setSystemNamedColor should look up rgb values from the built in
    // color tables first (see qcolor_p.cpp), and failing that, use
    // the window system's interface for translating names to rgb values...
    // we do this so that things like uic can load an XPM file with named colors
    // and convert it to a png without having to use window system functions...
    d.argb = qt_get_rgb_val( name.latin1() );
    QRgb rgb;
    if ( qt_get_named_rgb( name.latin1(), &rgb ) ) {
	d.argb = rgb;
	if ( colormodel == d8 ) {
	    d.d8.invalid = FALSE;
	    d.d8.dirty = TRUE;
	    d.d8.pix = 0;
	} else {
	    if ( qt_screen )
		alloc();
	    else
		d.d32.pix = d.argb | 0xff000000;
	}
    } else {
	// set to invalid color
	*this = QColor();
    }
}

int QColor::enterAllocContext()
{
    return 0;
}


void QColor::leaveAllocContext()
{
}


int QColor::currentAllocContext()
{
    return 0;
}


void QColor::destroyAllocContext( int )
{
}
