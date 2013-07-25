/****************************************************************************
**
** Implementation of QPixmap class for FB
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

#include "qglobal.h"

#include "qbitmap.h"
#include "qpaintdevicemetrics.h"
#include "qimage.h"
#include "qwmatrix.h"
#include "qapplication.h"
#include "qpainter.h"
#include "qptrdict.h"
#include "qwsdisplay_qws.h"
#include "qgfx_qws.h"
#include <stdlib.h>
#include <limits.h>

#include "qmemorymanager_qws.h"

/*****************************************************************************
  Internal functions
 *****************************************************************************/

extern const uchar *qt_get_bitflip_array();		// defined in qimage.cpp

static uchar *flip_bits( const uchar *bits, int len )
{
    register const uchar *p = bits;
    const uchar *end = p + len;
    uchar *newdata = new uchar[len];
    uchar *b = newdata;
    const uchar *f = qt_get_bitflip_array();
    while ( p < end )
	*b++ = f[*p++];
    return newdata;
}

// Returns position of highest bit set or -1 if none
/*
static int highest_bit( uint v )
{
    int i;
    uint b = (uint)1 << 31;
    for ( i=31; ((b & v) == 0) && i>=0;	 i-- )
	b >>= 1;
    return i;
}
*/

// Returns position of lowest set bit in 'v' as an integer (0-31), or -1
/*
static int lowest_bit( uint v )
{
    int i;
    ulong lb;
    lb = 1;
    for (i=0; ((v & lb) == 0) && i<32;  i++, lb<<=1);
    return i==32 ? -1 : i;
}
*/

// Counts the number of bits set in 'v'
/*
static uint n_bits( uint v )
{
    int i = 0;
    while ( v ) {
	v = v & (v - 1);
	i++;
    }
    return i;
}
*/

/*
static uint *red_scale_table   = 0;
static uint *green_scale_table = 0;
static uint *blue_scale_table  = 0;

static void cleanup_scale_tables()
{
    delete[] red_scale_table;
    delete[] green_scale_table;
    delete[] blue_scale_table;
}
*/

/*
  Could do smart bitshifting, but the "obvious" algorithm only works for
  nBits >= 4. This is more robust.
*/
/*
static void build_scale_table( uint **table, uint nBits )
{
    if ( nBits > 7 ) {
#if defined(QT_CHECK_RANGE)
	qWarning( "build_scale_table: internal error, nBits = %i", nBits );
#endif
	return;
    }
    if (!*table) {
	static bool firstTable = TRUE;
	if ( firstTable ) {
	    qAddPostRoutine( cleanup_scale_tables );
	    firstTable = FALSE;
	}
	*table = new uint[256];
    }
    int   maxVal   = (1 << nBits) - 1;
    int   valShift = 8 - nBits;
    int i;
    for( i = 0 ; i < maxVal + 1 ; i++ )
	(*table)[i << valShift] = i*255/maxVal;
}
*/

static QList<QShared> *qws_pixmapData = 0;
static bool qws_trackPixmapData = TRUE;

class QwsPixmap : public QPixmap
{
public:
    QwsPixmap() : QPixmap() {}
    static void mapPixmaps( bool from );
    static QPtrDict<QImage> *images;
};

QPtrDict<QImage> *QwsPixmap::images = 0;

void QwsPixmap::mapPixmaps( bool from )
{
    if ( !qws_pixmapData )
	return;
    if ( !images )
	images = new QPtrDict<QImage>;
    qws_trackPixmapData = FALSE;
    QListIterator<QShared> it( *qws_pixmapData );
    while ( it.current() ) {
	QPixmapData *d = (QPixmapData*)it.current();
	++it;
	if ( d->w && d->h ) {
	    if ( from ) {
		QwsPixmap p;
		QPixmapData *tmp = p.data;
		p.data = d;
		QImage *img = new QImage(p.convertToImage());
		images->insert( d, img );
		p.data = tmp;
	    } else {
		QImage *img = images->take( d );
		if ( img ) {
		    if ( d->clut )
			delete [] d->clut;
		    if ( memorymanager )
			memorymanager->deletePixmap(d->id);
		    QwsPixmap p;
		    p.convertFromImage( *img );
		    int cnt = d->count-1;
		    p.data->mask = d->mask;
		    *d = *p.data;
		    while (cnt > 0) {
			d->ref();
			--cnt;
		    }
		    delete img;
		    delete p.data;
		    p.data = 0;
		}
	    }
	}
    }
    if ( !from )
	images->clear();
    qws_trackPixmapData = TRUE;
}

void qws_mapPixmaps( bool from )
{
    QwsPixmap::mapPixmaps( from );
}

/*****************************************************************************
  QPixmap member functions
 *****************************************************************************/

void QPixmap::init( int w, int h, int d, bool bitmap, Optimization optim )
{
    static int serial = 0;
    int dd = defaultDepth();

    if ( !qws_pixmapData )
	qws_pixmapData = new QList<QShared>;

    if ( optim == DefaultOptim )		// use default optimization
	optim = defOptim;

    data = new QPixmapData;
    Q_CHECK_PTR( data );

    if ( qws_trackPixmapData )
	qws_pixmapData->append( data );

    memset( data, 0, sizeof(QPixmapData) );
    data->id=0;
    data->count  = 1;
    data->uninit = TRUE;
    data->bitmap = bitmap;
    data->ser_no = ++serial;
    data->optim	 = optim;
    data->clut=0;
    data->numcols = 0;
    data->hasAlpha = FALSE;

    if ( d > 0 && !qwsDisplay()->supportsDepth(d) )
	d = dd; // caller asked for an unsupported depth

    bool make_null = w == 0 || h == 0;		// create null pixmap
    if ( d == 1 )				// monocrome pixmap
	data->d = 1;
    else if ( d < 0 || d == dd )		// def depth pixmap
	data->d = dd;
    else
	data->d = d;
    if ( make_null || w < 0 || h < 0 || data->d == 0 ) {
	data->id = 0;
	data->w = 0;
	data->h = 0;
#if defined(QT_CHECK_RANGE)
	if ( !make_null ) {
	    qWarning( "QPixmap: Invalid pixmap parameters, %d %d %d",w,h,data->d);
	    abort();
	}
#endif
	return;
    }
    data->w = w;
    data->h = h;

    if(data->d<=8) {
	if ( qt_screen->numCols() ) {
	    data->numcols = qt_screen->numCols();
	    data->clut = new QRgb[qt_screen->numCols()];
	    for ( int i = 0; i < qt_screen->numCols(); i++ )
		data->clut[i] = qt_screen->clut()[i];
	}
    }

    data->rw = qt_screen->mapToDevice( QSize(w,h) ).width();
    data->rh = qt_screen->mapToDevice( QSize(w,h) ).height();

    data->id=memorymanager->newPixmap(data->rw, data->rh, data->d, optim);
    if ( data->id == 0 )
	data->w = data->h = 0; // out of memory -- create null pixmap
}


void QPixmap::deref()
{
    if ( data && data->deref() ) {			// last reference lost
	if ( qws_trackPixmapData )
	    qws_pixmapData->removeRef( data );
	if ( data->mask )
	    delete data->mask;
	if ( data->clut )
	    delete[] data->clut;

	if ( memorymanager )
	    memorymanager->deletePixmap(data->id);
	delete data;
	data = 0;
    }
}


QPixmap::QPixmap( int w, int h, const uchar *bits, bool isXbitmap )
    : QPaintDevice( QInternal::Pixmap )
{						// for bitmaps only
    init( 0, 0, 0, FALSE, defOptim );
    if ( w <= 0 || h <= 0 )			// create null pixmap
	return;

    data->uninit = FALSE;
    data->w = w;
    data->h = h;
    data->d = 1;
    data->rw = qt_screen->mapToDevice( QSize(w,h) ).width();
    data->rh = qt_screen->mapToDevice( QSize(w,h) ).height();
    data->hasAlpha = FALSE;
    uchar *flipped_bits;
    if ( isXbitmap ) {
	flipped_bits = 0;
    } else {					// not X bitmap -> flip bits
	flipped_bits = flip_bits( bits, ((w+7)/8)*h );
	bits = flipped_bits;
    }

    if ( qt_screen->isTransformed() ) {
	int bpl = isXbitmap ? (w+7)/8 : ((w+31)/32)*4;
	QImage img( (uchar *)bits, w, h, 1, bpl, 0, 0, QImage::LittleEndian );
	convertFromImage( img, MonoOnly );
	if ( flipped_bits )
	    delete [] flipped_bits;
	return;
    }

    data->id=memorymanager->newPixmap(data->rw,data->rh,data->d, optimization());
    if ( data->id == 0 ) {
	// out of memory -- create null pixmap.
	data->w = data->h = 0;
	return;
    }
    uchar *dest;
    int xoffset,linestep;
    memorymanager->findPixmap(data->id,data->rw,data->d,&dest,&xoffset,&linestep);

    Q_ASSERT((xoffset&7) == 0); // if not, we need to fix this to do a bitblt
    dest += xoffset/8;

    uchar *src = (uchar*)bits;
    for (int row = 0; row < h; row++)
    {
	memcpy(dest, src, (w+7)/8);
	dest += linestep;
	src += (w+7)/8;
    }

    if ( flipped_bits )				// Avoid purify complaint
	delete [] flipped_bits;
}


void QPixmap::detach()
{
    if ( data->count != 1 )
        *this = copy();
    data->uninit = FALSE;
}


int QPixmap::defaultDepth()
{
    QWSDisplay *d = qwsDisplay();
    int dd = d ? d->pixmapDepth() : 16;
    return dd;
}


void QPixmap::setOptimization( Optimization )
{
}


void QPixmap::fill( const QColor &fillColor )
{
    if ( isNull() )
	return;
    QPainter p(this);
    p.fillRect(rect(),fillColor);
}


int QPixmap::metric( int m ) const
{
    int val;
    if ( m == QPaintDeviceMetrics::PdmWidth ) {
	val = width();
    } else if ( m == QPaintDeviceMetrics::PdmWidthMM ) {
	// 75 dpi is 3dpmm
	val = (width()*100)/288;
    } else if ( m == QPaintDeviceMetrics::PdmHeight ) {
	val = height();
    } else if ( m == QPaintDeviceMetrics::PdmHeightMM ) {
	val = (height()*100)/288;
    } else if ( m == QPaintDeviceMetrics::PdmDpiX || m == QPaintDeviceMetrics::PdmPhysicalDpiX ) {
	return 72;
    } else if ( m == QPaintDeviceMetrics::PdmDpiY || m == QPaintDeviceMetrics::PdmPhysicalDpiY ) {
	return 72;
    } else if( m ==  QPaintDeviceMetrics::PdmDepth ) {
	val=depth();
    } else {
	// XXX
	val = QPaintDevice::metric(m);
    }
    return val;
}

QImage QPixmap::convertToImage() const
{
    QImage image;
    if ( isNull() ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QPixmap::convertToImage: Cannot convert a null pixmap" );
#if defined(NASTY)
	abort();
#endif
#endif
	return image;
    }

    int w  = qt_screen->mapToDevice( QSize(width(), height()) ).width();
    int h  = qt_screen->mapToDevice( QSize(width(), height()) ).height();
    int	d  = depth();
    bool mono = d == 1;

    const QBitmap* msk = mask();

    if( d == 15 || d == 16 ) {
#ifndef QT_NO_QWS_DEPTH_16
	d = 32;
	// Convert here because we may not have a 32bpp gfx
	image.create( w,h,d,0, QImage::IgnoreEndian );
	for ( int y=0; y < h; y++ ) {     // for each scan line...
	    register uint *p = (uint *)image.scanLine(y);
	    ushort  *s = (ushort*)scanLine(y);
	    uint *end = p + w;
	    if ( msk ) {
		uchar* a = msk->scanLine(y);
		uchar bit = 1; // mask is LittleEndian
		while ( p < end ) {
		    uint rgb = qt_conv16ToRgb( *s++ );
		    if ( !(*a & bit) )
			rgb &= 0x00ffffff;
		    *p++ = rgb;
		    if (!(bit <<= 1)) {
			++a;
			bit = 1;
		    }
		}
	    } else {
		while ( p < end )
		    *p++ = qt_conv16ToRgb( *s++ );
	    }
	}
	if ( msk )
	    image.setAlphaBuffer( TRUE );
#endif
    } else {
	// We can only create little-endian pixmaps
	if ( d == 4 )
	    image.create(w,h,8,0, QImage::IgnoreEndian );
	else if ( d == 24 )
	    image.create(w,h,32,0, QImage::IgnoreEndian );
	else
	    image.create(w,h,d,0, mono ? QImage::LittleEndian : QImage::IgnoreEndian );//####### endianness

	QGfx * mygfx=image.graphicsContext();
	if(mygfx) {
	    mygfx->setSource(this);
	    mygfx->setAlphaType(QGfx::IgnoreAlpha);
	    mygfx->setLineStep(image.bytesPerLine());
	    mygfx->blt(0,0,width(),height(),0,0);
	} else {
	    qWarning("No image gfx for convertToImage!");
	}
	delete mygfx;
	image.setAlphaBuffer(data->hasAlpha);
    }

    if ( mono ) {				// bitmap
	image.setNumColors( 2 );
	image.setColor( 0, qRgb(255,255,255) );
	image.setColor( 1, qRgb(0,0,0) );
    } else if ( d <= 8 ) {
	image.setNumColors( numCols() );
	for ( int i = 0; i < numCols(); i++ )
	    image.setColor( i, clut()[i] );
	if (mask()) {				// which pixels are used?
	    QImage alpha = mask()->convertToImage();
	    alpha = qt_screen->mapToDevice( alpha );
	    bool ale = alpha.bitOrder() == QImage::LittleEndian;
	    register uchar *p;
	    int  used[256];
	    memset( used, 0, sizeof(int)*256 );
	    for ( int i=0; i<h; i++ ) {
		uchar* asrc = alpha.scanLine( i );
		p = image.scanLine( i );
		for ( int x = 0; x < w; x++ ) {
		    if ( ale ) {
			if (asrc[x >> 3] & (1 << (x & 7)))
			    used[*p]++;
		    } else {
			if (asrc[x >> 3] & (1 << (7 -(x & 7))))
			    used[*p]++;
		    }
		    ++p;
		}
	    }

	    int trans=0;
	    int bestn=INT_MAX;
	    for ( int i=0; i<numCols(); i++ ) {
		if ( used[i] < bestn ) {
		    bestn = used[i];
		    trans = i;
		    if ( !bestn )
			break;
		}
	    }

	    image.setColor( trans, image.color(trans)&0x00ffffff );

	    for ( int i=0; i<h; i++ ) {
		uchar* asrc = alpha.scanLine( i );
		p = image.scanLine( i );
		for ( int x = 0; x < w; x++ ) {
		    if ( ale ) {
			if (!(asrc[x >> 3] & (1 << (x & 7))))
			    *p = trans;
		    } else {
			if (!(asrc[x >> 3] & (1 << (7 -(x & 7)))))
			    *p = trans;
		    }
		    ++p;
		}
	    }
	    image.setAlphaBuffer( TRUE );
	}
    } else if ( d == 32 && mask() ) {
	QImage alpha = mask()->convertToImage();
	alpha = qt_screen->mapToDevice( alpha );
	bool ale = alpha.bitOrder() == QImage::LittleEndian;
	for ( int i=0; i<h; i++ ) {
	    uchar* asrc = alpha.scanLine( i );
	    Q_UINT32 *p = (Q_UINT32 *)image.scanLine( i );
	    for ( int x = 0; x < w; x++ ) {
		if ( ale ) {
		    if (!(asrc[x >> 3] & (1 << (x & 7))))
			*p = *p & 0x00ffffff;
		} else {
		    if (!(asrc[x >> 3] & (1 << (7 -(x & 7)))))
			*p = *p & 0x00ffffff;
		}
		++p;
	    }
	}
	image.setAlphaBuffer( TRUE );
    }

    image = qt_screen->mapFromDevice( image );

    return image;
}

bool QPixmap::convertFromImage( const QImage &img, int conversion_flags )
{
    if ( img.isNull() ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QPixmap::convertFromImage: Cannot convert a null image" );
#if defined(NASTY)
	abort();
#endif
#endif
	return FALSE;
    }

    QImage  image = img;
    int	 w   = image.width();
    int	 h   = image.height();
    int	 d   = image.depth();	// source depth
    int	 dd  = defaultDepth();	//destination depth
    bool force_mono = (dd == 1 || isQBitmap() ||
		       (conversion_flags & ColorMode_Mask)==MonoOnly );

    if ( force_mono ) {				// must be monochrome
	if ( d != 1 ) {
	    image = image.convertDepth( 1, conversion_flags );	// dither
	    d = 1;
	}
    } else {					// can be both
	bool conv8 = FALSE;
	if ( d > 8 && dd <= 8 ) {		// convert to 8 bit
	    if ( (conversion_flags & DitherMode_Mask) == AutoDither )
		conversion_flags = (conversion_flags & ~DitherMode_Mask)
					| PreferDither;
	    conv8 = TRUE;
	} else if ( (conversion_flags & ColorMode_Mask) == ColorOnly ) {
	    conv8 = d == 1;			// native depth wanted
	} else if ( d == 1 ) {
	    if ( image.numColors() == 2 ) {
		QRgb c0 = image.color(0);	// Auto: convert to best
		QRgb c1 = image.color(1);
		conv8 = QMIN(c0,c1) != qRgb(0,0,0) || QMAX(c0,c1) != qRgb(255,255,255);
		if ( !conv8 )
		    force_mono = TRUE;
	    } else {
		// eg. 1-color monochrome images (they do exist).
		conv8 = TRUE;
	    }
	}
	if ( conv8 ) {
	    image = image.convertDepth( 8, conversion_flags );
	    d = 8;
	}
    }

    if(image.depth()==1) {
	if(image.bitOrder()==QImage::BigEndian) {
	    image=image.convertBitOrder(QImage::LittleEndian);
	}
    }

    if ( force_mono )
	dd = 1;

    bool manycolors=(qt_screen->depth() > 8);

    bool partialalpha=FALSE;

    QWSDisplay *dpy = qwsDisplay();
    if(image.hasAlphaBuffer() && dpy->supportsDepth(32) && dd>8 && manycolors) {
	if (image.depth()==8) {
	    for (int i=0; i<image.numColors(); i++) {
		int t = qAlpha(image.color(i));
		if ( t>0 && t<255) {
		    partialalpha = TRUE;
		    break;
		}
	    }
	} else if (image.depth()==32) {
	    int loopc,loopc2;
	    for (loopc=0;loopc<image.height();loopc++) {
		QRgb * tmp=(QRgb *)image.scanLine(loopc);
		for(loopc2=0;loopc2<image.width();loopc2++) {
		    int t=qAlpha(*tmp++);
		    if(t>0 && t<255) {
			partialalpha=TRUE;
			loopc2=image.width();
			loopc=image.height();
		    }
		}
	    }
	}
	if ( partialalpha )
	    dd=32;
    }

    // for drivers that do not accelerate alpha blt
    Optimization optim = partialalpha ? NoOptim : defOptim;

    QImage rimg = qt_screen->mapToDevice( image );

    // detach other references and re-init()
    bool ibm = isQBitmap();
    detach();
    deref();
    init( w, h, dd, ibm, optim );

    QGfx * mygfx=graphicsContext();
    if ( mygfx ) {
	mygfx->setAlphaType(QGfx::IgnoreAlpha);
	mygfx->setSource(&rimg);
	mygfx->blt(0,0,data->w,data->h,0,0);
	delete mygfx;
    }
    if ( image.hasAlphaBuffer() ) {
#ifndef QT_NO_IMAGE_DITHER_TO_1
	if ( !partialalpha ) {
	    QBitmap m;
	    m = image.createAlphaMask( conversion_flags );
	    setMask( m );
        } else
#endif
	    data->hasAlpha = TRUE;
    }
    data->uninit = FALSE;

    return TRUE;
}


QPixmap QPixmap::grabWindow( WId window, int x, int y, int w, int h )
{
    QPixmap pm;
    QWidget *widget = QWidget::find( window );
    if ( widget ) {
	if ( w <= 0 || h <= 0 ) {
	    if ( w == 0 || h == 0 )
		return pm;
	    if ( w < 0 )
		w = widget->width() - x;
	    if ( h < 0 )
		h = widget->height() - y;
	}
	pm.resize(w, h);
	QGfx *gfx=pm.graphicsContext();
	if ( gfx ) {
	    gfx->setAlphaType(QGfx::IgnoreAlpha);
	    gfx->setSource(widget);
	    gfx->blt(0,0,w,h,x,y);
	    delete gfx;
	}
    }
    return pm;
}

#ifndef QT_NO_PIXMAP_TRANSFORMATION
QPixmap QPixmap::xForm( const QWMatrix &matrix ) const
{
    int	   w, h;				// size of target pixmap
    int	   ws, hs;				// size of source pixmap
    uchar *dptr;				// data in target pixmap
    int	   dbpl, dbytes;			// bytes per line/bytes total
    uchar *sptr;				// data in original pixmap
    int	   sbpl;				// bytes per line in original
    int	   bpp;					// bits per pixel
    bool   depth1 = depth() == 1;
    int	   y;

    if ( isNull() )				// this is a null pixmap
	return copy();

    ws = width();
    hs = height();

    QWMatrix mat( matrix.m11(), matrix.m12(), matrix.m21(), matrix.m22(), 0., 0. );

    if ( matrix.m12() == 0.0F && matrix.m21() == 0.0F ) {
	if ( matrix.m11() == 1.0F && matrix.m22() == 1.0F )
	    return *this;			// identity matrix
	h = qRound( matrix.m22()*hs );
	w = qRound( matrix.m11()*ws );
	h = QABS( h );
	w = QABS( w );
	if ( matrix.m11() >= 0.0F  && matrix.m22() >= 0.0F &&
		depth() == defaultDepth() ) // ### stretchBlt limitation
	{
	     if ( w==0 || h==0 )
		 return *this;

	     QPixmap pm( w, h, depth(), NormalOptim );
	     QGfx * mygfx=pm.graphicsContext();
	     if ( mygfx ) {
		 mygfx->setSource(this);
		 mygfx->setAlphaType(QGfx::IgnoreAlpha);
		 mygfx->stretchBlt(0,0,w,h,ws,hs);
		 delete mygfx;
	     }
	     if ( data->mask ) {
		 QBitmap bm =
		     data->selfmask ? *((QBitmap*)(&pm)) :
		     data->mask->xForm(matrix);
		 pm.setMask( bm );
	     }
	     pm.data->hasAlpha = data->hasAlpha;
	     return pm;
	 }
    } else {					// rotation or shearing
	QPointArray a( QRect(0,0,ws+1,hs+1) );
	a = mat.map( a );
	QRect r = a.boundingRect().normalize();
	w = r.width()-1;
	h = r.height()-1;
    }

    mat = trueMatrix( mat, ws, hs ); // true matrix

    bool invertible;
    mat = mat.invert( &invertible );		// invert matrix

    if ( h == 0 || w == 0 || !invertible ) {	// error, return null pixmap
	QPixmap pm;
	pm.data->bitmap = data->bitmap;
	return pm;
    }

    QImage srcImg;
    if ( qt_screen->isTransformed() ) {
	srcImg = convertToImage();
	sptr=srcImg.scanLine(0);
	sbpl=srcImg.bytesPerLine();
    } else {
	sptr=scanLine(0);
	sbpl=bytesPerLine();
    }
    ws=width();
    hs=height();

    QImage destImg;
    QPixmap pm( 1, 1, depth(), data->bitmap, NormalOptim );
    pm.data->uninit = FALSE;
    if ( qt_screen->isTransformed() ) {
	destImg.create( w, h, srcImg.depth(), srcImg.numColors(), srcImg.bitOrder() );
	dptr=destImg.scanLine(0);
	dbpl=destImg.bytesPerLine();
	bpp=destImg.depth();
    } else {
	pm.resize( w, h );
	dptr=pm.scanLine(0);
	dbpl=pm.bytesPerLine();
	bpp=pm.depth();
    }

    dbytes = dbpl*h;

    if ( depth1 )
	memset( dptr, 0x00, dbytes );
    else if ( bpp == 8 )
	memset( dptr, white.pixel(), dbytes );
    else if ( bpp == 32 ) {
	if ( qt_screen->isTransformed() )
	    destImg.fill( 0x00FFFFFF );
	else
	    pm.fill( QColor(0x00FFFFFF,0x00FFFFFF) );
    } else
	memset( dptr, 0xff, dbytes );

    int xbpl, p_inc;
    if ( depth1 ) {
	xbpl  = (w+7)/8;
	p_inc = dbpl - xbpl;
    } else {
	xbpl  = (w*bpp)/8;
	p_inc = dbpl - xbpl;
    }

    if ( !qt_xForm_helper( mat, 0, QT_XFORM_TYPE_LSBFIRST, bpp, dptr, xbpl, p_inc, h, sptr, sbpl, ws, hs ) ){
#if defined(QT_CHECK_RANGE)
	qWarning( "QPixmap::xForm: display not supported (bpp=%d)",bpp);
#endif
	QPixmap pm;
	return pm;
    }

    if ( qt_screen->isTransformed() ) {
	pm.convertFromImage( destImg );
    }

    if ( data->mask ) {
	if ( depth1 && data->selfmask )               // pixmap == mask
	    pm.setMask( *((QBitmap*)(&pm)) );
	else
	    pm.setMask( data->mask->xForm(matrix) );
    }
    pm.data->hasAlpha = data->hasAlpha;

    return pm;
}
#endif // QT_NO_PIXMAP_TRANSFORMATION

// CALLER DELETES
/*!
    \internal
*/
QGfx * QPixmap::graphicsContext(bool) const
{
    if(isNull()) {
	qDebug("Can't make QGfx for null pixmap\n");
	return 0;
    }
    uchar * mydata;
    int xoffset,linestep;
    memorymanager->findPixmap(data->id,data->rw,data->d,&mydata,&xoffset,&linestep);

    QGfx * ret=QGfx::createGfx( depth(), mydata, data->w,data->h, linestep );
    if(data->d<=8) {
	if(data->d==1 && !(data->clut)) {
	    data->clut=new QRgb[2];
	    data->clut[0]=qRgb(255,255,255);
	    data->clut[1]=qRgb(0,0,0);
	    data->numcols = 2;
	}
	if ( data->numcols )
	    ret->setClut(data->clut,data->numcols);
    }
    return ret;
}

/*!
    \internal
*/
unsigned char * QPixmap::scanLine(int i) const
{
    uchar * p;
    int xoffset,linestep;
    memorymanager->findPixmap(data->id,data->rw,data->d,&p,&xoffset,&linestep);
    p+=i*linestep;
    return p;
}

/*!
    \internal
*/
int QPixmap::bytesPerLine() const
{
    uchar * p;
    int xoffset,linestep;
    memorymanager->findPixmap(data->id,data->rw,data->d,&p,&xoffset,&linestep);
    return linestep;
}

/*!
    \internal
*/
QRgb * QPixmap::clut() const
{
    return data->clut;
}

/*!
    \internal
*/
int QPixmap::numCols() const
{
    return data->numcols;
}

bool QPixmap::hasAlpha() const
{
    return data->hasAlpha || data->mask;
}

bool QPixmap::hasAlphaChannel() const
{
    return data->hasAlpha;
}

Q_EXPORT void copyBlt( QPixmap *dst, int dx, int dy,
		       const QPixmap *src, int sx, int sy, int sw, int sh )
{
    if ( ! dst || ! src || sw == 0 || sh == 0 || dst->depth() != src->depth() ) {
#ifdef QT_CHECK_NULL
	Q_ASSERT( dst != 0 );
	Q_ASSERT( src != 0 );
#endif
	return;
    }

    // copy pixel data
    bitBlt( dst, dx, dy, src, sx, sy, sw, sh, Qt::CopyROP, TRUE );

    // copy mask data
    if ( src->data->mask ) {
	if ( ! dst->data->mask ) {
	    dst->data->mask = new QBitmap( dst->width(), dst->height() );

	    // new masks are fully opaque by default
	    dst->data->mask->fill( Qt::color1 );
	}

	bitBlt( dst->data->mask, dx, dy,
		src->data->mask, sx, sy, sw, sh, Qt::CopyROP, TRUE );
    }

    dst->data->hasAlpha = src->data->hasAlpha;
}
