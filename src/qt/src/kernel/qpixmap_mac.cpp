/****************************************************************************
** $Id: qt/qpixmap_mac.cpp   3.3.8   edited Jan 23 17:19 $
**
** Implementation of QFileInfo class
**
** Copyright (C) 1992-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Macintosh may use this file in accordance with the Qt Commercial
** License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qpixmap.h"
#include "qimage.h"
#include "qpaintdevicemetrics.h"
#include "qapplication.h"
#include "qbitmap.h"
#include "qwmatrix.h"
#include "qt_mac.h"

#include <limits.h>
#include <string.h>

extern const uchar *qt_get_bitflip_array();		// defined in qimage.cpp

QPixmap::QPixmap(int w, int h, const uchar *bits, bool isXbitmap)
    : QPaintDevice(QInternal::Pixmap)
{
    init(w, h, 1, TRUE, DefaultOptim);
    if(!hd)
	qDebug("Qt: internal: No hd! %s %d", __FILE__, __LINE__);

#ifndef QMAC_ONE_PIXEL_LOCK
    bool locked = LockPixels(GetGWorldPixMap((GWorldPtr)hd));
    Q_ASSERT(locked);
#endif
    long *dptr = (long *)GetPixBaseAddr(GetGWorldPixMap((GWorldPtr)hd)), *drow, q;
    unsigned short dbpr = GetPixRowBytes(GetGWorldPixMap((GWorldPtr)hd));
    for(int yy=0;yy<h;yy++) {
	drow = (long *)((char *)dptr + (yy * dbpr));
	int sy = yy * ((w+7)/8);
	for(int xx=0;xx<w;xx++) {
	    char one_bit = *(bits + (sy + (xx / 8)));
	    if(!isXbitmap)
		one_bit = one_bit >> (7 - (xx % 8));
	    else
		one_bit = one_bit >> (xx % 8);
	    q = 0;
	    if(!(one_bit & 0x01))
		q = (255 << 16) | (255 << 8) | 255;
	    *(drow + xx) = q;
	}
    }
#ifndef QMAC_ONE_PIXEL_LOCK
    UnlockPixels(GetGWorldPixMap((GWorldPtr)hd));
#endif
}

static inline QRgb qt_conv16ToRgb(ushort c) {
    static const int qt_rbits = (565/100);
    static const int qt_gbits = (565/10%10);
    static const int qt_bbits = (565%10);
    static const int qt_red_shift = qt_bbits+qt_gbits-(8-qt_rbits);
    static const int qt_green_shift = qt_bbits-(8-qt_gbits);
    static const int qt_neg_blue_shift = 8-qt_bbits;
    static const int qt_blue_mask = (1<<qt_bbits)-1;
    static const int qt_green_mask = (1<<(qt_gbits+qt_bbits))-((1<<qt_bbits)-1);
    static const int qt_red_mask = (1<<(qt_rbits+qt_gbits+qt_bbits))-(1<<(qt_gbits+qt_bbits));

    const int r=(c & qt_red_mask);
    const int g=(c & qt_green_mask);
    const int b=(c & qt_blue_mask);
    const int tr = r >> qt_red_shift;
    const int tg = g >> qt_green_shift;
    const int tb = b << qt_neg_blue_shift;

    return qRgb(tr,tg,tb);
}

bool QPixmap::convertFromImage(const QImage &img, int conversion_flags)
{
    if(img.isNull()) {
#if defined(QT_CHECK_NULL)
	qWarning("QPixmap::convertFromImage: Cannot convert a null image");
#endif
	return FALSE;
    }

    QImage image = img;
    int    d     = image.depth();
    int    dd    = defaultDepth();
    bool force_mono = (dd == 1 || isQBitmap() ||
		       (conversion_flags & ColorMode_Mask)==MonoOnly);
    if(force_mono) {                         // must be monochrome
	if(d != 1) {
	    image = image.convertDepth(1, conversion_flags);  // dither
	    d = 1;
	}
    } else {                                    // can be both
	bool conv8 = FALSE;
	if(d > 8 && dd <= 8) {               // convert to 8 bit
	    if((conversion_flags & DitherMode_Mask) == AutoDither)
		conversion_flags = (conversion_flags & ~DitherMode_Mask)
				   | PreferDither;
	    conv8 = TRUE;
	} else if((conversion_flags & ColorMode_Mask) == ColorOnly) {
	    conv8 = d == 1;                     // native depth wanted
	} else if(d == 1) {
	    if(image.numColors() == 2) {
		QRgb c0 = image.color(0);       // Auto: convert to best
		QRgb c1 = image.color(1);
#if 0
		conv8 = QMIN(c0,c1) != qRgb(0,0,0) || QMAX(c0,c1) != qRgb(255,255,255);
#else
		conv8 = ((c0 == qRgb(0,0,0)) && c1 == qRgb(255,255,255));
#endif
	    } else {
		// eg. 1-color monochrome images (they do exist).
		conv8 = TRUE;
	    }
	}
	if(conv8) {
	    image = image.convertDepth(8, conversion_flags);
	    d = 8;
	}
    }

    if(image.depth()==1) {
	image.setColor(0, qRgba(255,255,255,0));
	image.setColor(1, qRgba(0,0,0,0));
    }

    int w = image.width();
    int h = image.height();

    if(width() == w && height() == h && ((d == 1 && depth() == 1) ||
					    (d != 1 && depth() != 1))) {
	// same size etc., use the existing pixmap
	detach();

	if(data->mask) {                     // get rid of the mask
	    delete data->mask;
	    data->mask = 0;
	}
	if(data->alphapm) {                     // get rid of the alpha
	    delete data->alphapm;
	    data->alphapm = 0;
	}
    } else {
	// different size or depth, make a new pixmap
	QPixmap pm(w, h, d == 1 ? 1 : -1);
	pm.data->bitmap = data->bitmap;         // keep is-a flag
	pm.data->optim  = data->optim;          // keep optimization flag
	*this = pm;
    }

    if(!hd)
	qDebug("Qt: internal: No hd! %s %d", __FILE__, __LINE__);

#ifndef QMAC_ONE_PIXEL_LOCK
    bool locked = LockPixels(GetGWorldPixMap((GWorldPtr)hd));
    Q_ASSERT(locked);
#endif
    long *dptr = (long *)GetPixBaseAddr(GetGWorldPixMap((GWorldPtr)hd)), *drow;
    unsigned short dbpr = GetPixRowBytes(GetGWorldPixMap((GWorldPtr)hd));

    QRgb q=0;
    int sdpt = image.depth();
    unsigned short sbpr = image.bytesPerLine();
    uchar *sptr = image.bits(), *srow;
    QImage::Endian sord = image.bitOrder();

    for(int yy=0;yy<h;yy++) {
 	drow = (long *)((char *)dptr + (yy * dbpr));
	srow = sptr + (yy * sbpr);
	switch(sdpt) {
	case 1: {
	    for(int xx=0;xx<w;xx++) {
		char one_bit = *(srow + (xx / 8));
		if(sord==QImage::BigEndian)
		    one_bit = one_bit >> (7 - (xx % 8));
		else
		    one_bit = one_bit >> (xx % 8);
		q = 0;
		if(!(one_bit & 0x01))
		    q = (255 << 16) | (255 << 8) | 255;
		*(drow + xx) = q;
	    }
	    break; }
	case 8: {
	    for(int xx=0;xx<w;xx++) {
		q = image.color(*(srow + xx));
		*(drow + xx) = q;
	    }
	    break; }
	case 16: {
	    for(int xx=0;xx<w;xx++) {
		q = qt_conv16ToRgb(*(((ushort *)srow) + xx));
		*(drow + xx) = q;
	    }
	    break; }
	case 32: {
	    for(int xx=0;xx<w;xx++) {
		q = *(((QRgb *)srow) + xx);
		*(drow + xx) = q;
	    }
	    break; }
	default:
	    qDebug("Qt: internal: Oops: Forgot a depth %s:%d", __FILE__, __LINE__);
	    break;
	}
    }
#ifndef QMAC_ONE_PIXEL_LOCK
    UnlockPixels(GetGWorldPixMap((GWorldPtr)hd));
#endif

    data->uninit = FALSE;

    // get rid of alpha pixmap
    delete data->alphapm;
    data->alphapm = 0;
    if(image.hasAlphaBuffer()) {
	{
	    QBitmap m;
	    m = image.createAlphaMask(conversion_flags);
	    setMask(m);
	}
        bool alphamap = image.depth() == 32;
	if (image.depth() == 8) {
	    const QRgb * const rgb = image.colorTable();
	    for (int i = 0, count = image.numColors(); i < count; ++i) {
		const int alpha = qAlpha(rgb[i]);
		if (alpha != 0 && alpha != 0xff) {
		    alphamap = true;
		    break;
		}
	    }
	}
	if (alphamap) {
	    data->alphapm = new QPixmap(w, h, 32);
#ifndef QMAC_ONE_PIXEL_LOCK
	    bool locked = LockPixels(GetGWorldPixMap((GWorldPtr)data->alphapm->hd));
	    Q_ASSERT(locked);
#endif
	    long *dptr = (long *)GetPixBaseAddr(GetGWorldPixMap((GWorldPtr)data->alphapm->hd)), *drow;
	    unsigned short dbpr = GetPixRowBytes(GetGWorldPixMap((GWorldPtr)hd));
            if (image.depth() == 32) {
                unsigned short sbpr = image.bytesPerLine();
                long *sptr = (long*)image.bits(), *srow;
                uchar clr;
                for(int yy=0; yy < h; yy++) {
                    drow = (long *)((char *)dptr + (yy * dbpr));
                    srow = (long *)((char *)sptr + (yy * sbpr));
                    for (int xx=0; xx < w; xx++) {
                        clr = ~(((*(srow + xx)) >> 24) & 0xFF);
                        *(drow + xx) = qRgba(clr, clr, clr, 0);
                    }
                }
            } else {
                const QRgb *const rgb = image.colorTable();
                for (int y = 0; y < h; ++y) {
                    const uchar *iptr = image.scanLine(y);
                    drow = (long *)((char *)dptr + (y * dbpr));
                    for (int x = 0; x < w; ++x) {
                        const int alpha = ~qAlpha(rgb[*iptr++]);
                        *(drow + x) = qRgba(alpha, alpha, alpha, 0);
                    }
                }
            }
#ifndef QMAC_ONE_PIXEL_LOCK
	    UnlockPixels(GetGWorldPixMap((GWorldPtr)data->alphapm->hd));
#endif
	}
    }
    return TRUE;
}

int get_index(QImage * qi,QRgb mycol)
{
    int loopc;
    for(loopc=0;loopc<qi->numColors();loopc++) {
	if(qi->color(loopc)==mycol)
	    return loopc;
    }
    qi->setNumColors(qi->numColors()+1);
    qi->setColor(qi->numColors(),mycol);
    return qi->numColors();
}

QImage QPixmap::convertToImage() const
{
    if(!data->w || !data->h || !hd)
	return QImage(); // null image

    int w = data->w;
    int h = data->h;
    int d = isQBitmap() ? 1 : data->d;
    int ncols = 2;

#if 0
    if(d > 1 && d <= 8) {                    // set to nearest valid depth
        d = 8;                                  //   2..7 ==> 8
        ncols = 256;
    } else if(d > 8) {
        d = 32;                                 //   > 8  ==> 32
        ncols = 0;
    }
#else
    if(d != 1) { //do we want to FIXME??? Might want to support indexed color modes?
	d = 32;
	ncols = 0;
    }
#endif

    QImage image(w, h, d, ncols, QImage::BigEndian);
    //first we copy the clut
    //handle bitmap case, what about other indexed depths?
    if(d == 1) {
	image.setNumColors(2);
	image.setColor(0, qRgba(255, 255, 255, 0));
	image.setColor(1, qRgba(0, 0, 0, 0));
    } else if(d == 8) {
	//figure out how to copy clut into image FIXME???
    }

    if(!hd)
	qDebug("Qt: internal: No hd! %s %d", __FILE__, __LINE__);

#ifndef QMAC_ONE_PIXEL_LOCK
    bool locked = LockPixels(GetGWorldPixMap((GWorldPtr)hd));
    Q_ASSERT(locked);
#endif
    QRgb q;
    long *sptr = (long *)GetPixBaseAddr(GetGWorldPixMap((GWorldPtr)hd)), *srow, r;
    unsigned short sbpr = GetPixRowBytes(GetGWorldPixMap((GWorldPtr)hd));
    long *aptr = NULL, *arow = NULL;
    unsigned short abpr = 0;
    if(data->alphapm) {
	image.setAlphaBuffer(TRUE);
	aptr = (long *)GetPixBaseAddr(GetGWorldPixMap((GWorldPtr)data->alphapm->hd));
	abpr = GetPixRowBytes(GetGWorldPixMap((GWorldPtr)data->alphapm->hd));
    }

    for(int yy=0;yy<h;yy++) {
	srow = (long *)((char *)sptr + (yy * sbpr));
	if(aptr)
	    arow = (long *)((char *)aptr + (yy * abpr));
	for(int xx=0;xx<w;xx++) {
	    r = *(srow + xx);
	    q=qRgba((r >> 16) & 0xFF, (r >> 8) & 0xFF, r & 0xFF, (arow ? ~(*(arow + xx) & 0xFF): 0));
	    if(d == 1)
		image.setPixel(xx, yy, q ? 0 : 1);
	    else if(ncols)
		image.setPixel(xx, yy, get_index(&image,q));
	    else
		image.setPixel(xx,yy,q);
	}
    }

#ifndef QMAC_ONE_PIXEL_LOCK
    UnlockPixels(GetGWorldPixMap((GWorldPtr)hd));
#endif
    if(data->mask && !data->alphapm) {
	QImage alpha = data->mask->convertToImage();
	image.setAlphaBuffer(TRUE);
	switch(d) {
	case 8: {
	    int used[256];
	    memset(used, 0, sizeof(int)*256);
	    uchar* p = image.bits();
	    int l = image.numBytes();
	    while(l--)
		used[*p++]++;
	    int trans=0;
	    int bestn=INT_MAX;
	    for(int i=0; i<256; i++) {
		if(used[i] < bestn) {
		    bestn = used[i];
		    trans = i;
		    if(!bestn)
			break;
		}
	    }
	    image.setColor(trans, image.color(trans)&0x00ffffff);
	    for(int y=0; y<image.height(); y++) {
		uchar* mb = alpha.scanLine(y);
		uchar* ib = image.scanLine(y);
		uchar bit = 0x80;
		int i=image.width();
		while(i--) {
		    if(!(*mb & bit))
			*ib = trans;
		    bit /= 2;
		    if(!bit)
			mb++,bit = 0x80; // ROL
		    ib++;
		}
	    }
	} break;
	case 32: {
	    for(int y=0; y<image.height(); y++) {
		uchar* mb = alpha.scanLine(y);
		QRgb* ib = (QRgb*)image.scanLine(y);
		uchar bit = 0x80;
		int i=image.width();
		while(i--) {
		    if(*mb & bit)
			*ib |= 0xff000000;
		    else
			*ib &= 0x00ffffff;
		    bit /= 2;
		    if(!bit)
			mb++, bit = 0x80; // ROL
		    ib++;
		}
	    }
	} break;
	}
    }

#ifndef QMAC_ONE_PIXEL_LOCK
    UnlockPixels(GetGWorldPixMap((GWorldPtr)hd));
#endif
    return image;
}

void QPixmap::fill(const QColor &fillColor)
{
    if(!width() || !height())
	return;
    if(!hd)
	qDebug("Qt: internal: No hd! %s %d", __FILE__, __LINE__);

    //at the end of this function this will go out of scope and the destructor will restore the state
    QMacSavedPortInfo saveportstate(this);
#ifndef QMAC_ONE_PIXEL_LOCK
    bool locked = LockPixels(GetGWorldPixMap((GWorldPtr)hd));
    Q_ASSERT(locked);
#endif
    detach();					// detach other references
    if(depth() == 1 || depth() == 32) { //small optimization over QD
	ulong *dptr = (ulong *)GetPixBaseAddr(GetGWorldPixMap((GWorldPtr)hd));
	int dbytes = GetPixRowBytes(GetGWorldPixMap((GWorldPtr)hd))*height();
	if(!dptr || !dbytes)
	    qDebug("Qt: internal: No dptr or no dbytes! %s %d", __FILE__, __LINE__);
	QRgb colr = qRgba(fillColor.red(),fillColor.green(), fillColor.blue(), 0);
	if(depth() == 1 || !colr) {
	    memset(dptr, colr ? 0xFF : 0x00, dbytes);
	} else if(depth() == 32) {
	    for(int i = 0; i < (int)(dbytes/sizeof(ulong)); i++)
		*(dptr + i) = colr;
	}
    } else {
	Rect r;
	RGBColor rc;
	rc.red=fillColor.red()*256;
	rc.green=fillColor.green()*256;
	rc.blue=fillColor.blue()*256;
	RGBForeColor(&rc);
	SetRect(&r,0,0,width(),height());
	PaintRect(&r);
    }
#ifndef QMAC_ONE_PIXEL_LOCK
    UnlockPixels(GetGWorldPixMap((GWorldPtr)hd));
#endif
}

void QPixmap::detach()
{
    if ( data->count != 1 )
        *this = copy();
    data->uninit = FALSE;
}

int QPixmap::metric(int m) const
{
    int val=0;
    switch(m) {
	case QPaintDeviceMetrics::PdmWidth:
	    val = width();
	    break;
	case QPaintDeviceMetrics::PdmHeight:
	    val = height();
	    break;
	case QPaintDeviceMetrics::PdmWidthMM:
	case QPaintDeviceMetrics::PdmHeightMM:
	    break;
	case QPaintDeviceMetrics::PdmNumColors:
	    val = 1 << depth();
	    break;
	case QPaintDeviceMetrics::PdmDpiX: // ### fill with real values!
	case QPaintDeviceMetrics::PdmPhysicalDpiX:
	case QPaintDeviceMetrics::PdmDpiY:
	case QPaintDeviceMetrics::PdmPhysicalDpiY:
	    val = 72;
	    break;
	case QPaintDeviceMetrics::PdmDepth:
	    val = depth();
	    break;
	default:
	    val = 0;
#if defined(QT_CHECK_RANGE)
	    qWarning("QPixmap::metric: Invalid metric command");
#endif
    }
    return val;
}

void QPixmap::deref()
{
    if(data && data->deref()) {     // Destroy image if last ref
        if(data->mask) {
            delete data->mask;
            data->mask = 0;
        }
	if(data->alphapm) {
	    delete data->alphapm;
	    data->alphapm = 0;
	}

        if(hd && qApp) {
#ifdef QMAC_ONE_PIXEL_LOCK
	    UnlockPixels(GetGWorldPixMap((GWorldPtr)hd));
#endif
	    DisposeGWorld((GWorldPtr)hd);
        }
#ifndef QMAC_NO_QUARTZ
	if(ctx)
	    CGContextRelease(ctx);
	ctx = NULL;
#endif
        delete data;
	data = NULL;
    }
}

void scaledBitBlt(QPaintDevice *dst, int dx, int dy, int dw, int dh,
		   const QPaintDevice *src, int sx, int sy, int sw, int sh,
		   Qt::RasterOp rop, bool imask);

QPixmap QPixmap::xForm(const QWMatrix &matrix) const
{
    int	   w, h;				// size of target pixmap
    int	   ws, hs;				// size of source pixmap
    uchar *dptr;				// data in target pixmap
    int	   dbpl, dbytes;			// bytes per line/bytes total
    uchar *sptr;				// data in original pixmap
    int	   sbpl;				// bytes per line in original
    int	   bpp;					// bits per pixel

    if(isNull())				// this is a null pixmap
	return copy();

    ws = width();
    hs = height();

    QWMatrix mat(matrix.m11(), matrix.m12(), matrix.m21(), matrix.m22(), 0., 0.);

    if(matrix.m12() == 0.0F  && matrix.m21() == 0.0F &&
	 matrix.m11() >= 0.0F  && matrix.m22() >= 0.0F) {
	if(mat.m11() == 1.0F && mat.m22() == 1.0F)
	    return *this;			// identity matrix

	h = qRound(mat.m22()*hs);
	w = qRound(mat.m11()*ws);
	h = QABS(h);
	w = QABS(w);

	if(w==0 || h==0)
	    return *this;

	QPixmap* save_alpha = data->alphapm;
	data->alphapm = 0;
	QPixmap pm(w, h, depth(), NormalOptim);
	scaledBitBlt(&pm, 0, 0, w, h, this, 0, 0, width(), height(), Qt::CopyROP, TRUE);
	if(data->mask) {
	    QBitmap bm = data->selfmask ? *((QBitmap*)(&pm)) : data->mask->xForm(matrix);
	    pm.setMask(bm);
	}
	if(save_alpha) {
	    data->alphapm = save_alpha;
	    pm.data->alphapm = new QPixmap(w, h, save_alpha->depth(), NormalOptim);
	    scaledBitBlt(pm.data->alphapm, 0, 0, w, h, save_alpha, 0, 0, width(), height(),
			 Qt::CopyROP, TRUE);
	}
	return pm;
    } else {					// rotation or shearing
	QPointArray a(QRect(0,0,ws+1,hs+1));
	a = mat.map(a);
	QRect r = a.boundingRect().normalize();
	w = r.width()-1;
	h = r.height()-1;
    }

    mat = trueMatrix(mat, ws, hs); // true matrix

    bool invertible;
    mat = mat.invert(&invertible);		// invert matrix

    if(h == 0 || w == 0 || !invertible) {	// error, return null pixmap
	QPixmap pm;
	pm.data->bitmap = data->bitmap;
	pm.data->alphapm = data->alphapm;
	return pm;
    }

#ifndef QMAC_ONE_PIXEL_LOCK
    bool locked = LockPixels(GetGWorldPixMap((GWorldPtr)hd));
    Q_ASSERT(locked);
#endif
    sptr = (uchar *)GetPixBaseAddr(GetGWorldPixMap((GWorldPtr)hd));
    sbpl = GetPixRowBytes(GetGWorldPixMap((GWorldPtr)hd));
    ws=width();
    hs=height();

    QPixmap pm(w, h, depth(), optimization());
#ifndef QMAC_ONE_PIXEL_LOCK
    locked = LockPixels(GetGWorldPixMap((GWorldPtr)pm.handle()));
    Q_ASSERT(locked);
#endif
    dptr = (uchar *)GetPixBaseAddr(GetGWorldPixMap((GWorldPtr)pm.handle()));
    dbpl = GetPixRowBytes(GetGWorldPixMap((GWorldPtr)pm.handle()));
    bpp = 32;
    dbytes = dbpl*h;

    if(bpp == 1)
	memset(dptr, 0x00, dbytes);
    else if(bpp == 8)
	memset(dptr, white.pixel(), dbytes);
    else if(bpp == 32)
	pm.fill(0x00FFFFFF);
    else
	memset(dptr, 0xff, dbytes);

    int	xbpl = bpp == 1 ? ((w+7)/8) : ((w*bpp)/8);
    if(!qt_xForm_helper(mat, 0, QT_XFORM_TYPE_MSBFIRST, bpp,
			dptr, xbpl, dbpl - xbpl, h, sptr, sbpl, ws, hs)){
#if defined(QT_CHECK_RANGE)
	qWarning("Qt: QPixmap::xForm: display not supported (bpp=%d)",bpp);
#endif
	QPixmap pm;
	return pm;
    }
#ifndef QMAC_ONE_PIXEL_LOCK
    UnlockPixels(GetGWorldPixMap((GWorldPtr)hd));
    UnlockPixels(GetGWorldPixMap((GWorldPtr)pm.handle()));
#endif

    if(depth() == 1) {
	if(data->mask) {
	    if(data->selfmask)               // pixmap == mask
		pm.setMask(*((QBitmap*)(&pm)));
	    else
		pm.setMask(data->mask->xForm(matrix));
	}
    } else if(data->mask) {
	pm.setMask(data->mask->xForm(matrix));
    }
    if(data->alphapm)
	pm.data->alphapm = new QPixmap(data->alphapm->xForm(matrix));
    return pm;
}

void QPixmap::init(int w, int h, int d, bool bitmap, Optimization optim)
{
#if defined(QT_CHECK_STATE)
    if ( qApp->type() == QApplication::Tty ) {
	qWarning( "QPixmap: Cannot create a QPixmap when no GUI "
		  "is being used" );
    }
#endif

    if(d != 32 && d != 1)
	d = 32; //magic number.. we always use a 32 bit depth for non-bitmaps

    static int serial = 0;

    hd = 0;
    data = new QPixmapData;
    Q_CHECK_PTR(data);
    memset(data, 0, sizeof(QPixmapData));
    data->count=1;
    data->uninit=TRUE;
    data->bitmap=bitmap;
    data->clut = NULL;
    data->ser_no=++serial;
    data->optim=optim;

    int dd = 32; //magic number? 32 seems to be default?
    bool make_null = w == 0 || h == 0;		// create null pixmap
    if(d == 1)				// monocrome pixmap
	data->d = 1;
    else if(d < 0 || d == dd)		// def depth pixmap
	data->d = dd;
    if(make_null || w < 0 || h < 0 || data->d == 0) {
	hd = 0;
#if defined(QT_CHECK_RANGE)
	if(!make_null)
	    qWarning("Qt: QPixmap: Invalid pixmap parameters");
#endif
	return;
    }

    if(w<1 || h<1)
	return;
    data->w=w;
    data->h=h;

    Rect rect;
    SetRect(&rect,0,0,w,h);
    QDErr e = -1;
    int params = alignPix | stretchPix | newDepth;
#if QT_MACOSX_VERSION >= 0x1040
    params |= kNativeEndianPixMap;
#endif

#if 1
    if(optim == BestOptim) //try to get it into distant memory
	e = NewGWorld((GWorldPtr *)&hd, 32, &rect,
		      data->clut ? &data->clut : NULL, 0, useDistantHdwrMem | params);
    if(e != noErr) //oh well I tried
#endif
	e = NewGWorld((GWorldPtr *)&hd, 32, &rect,
		      data->clut ? &data->clut : NULL, 0, params);

    /* error? */
    if(e != noErr) {
	data->w = data->h = 0;
	hd=0; //just to be sure
	qDebug("Qt: internal: QPixmap::init error (%d) (%d %d %d %d)", e, rect.left, rect.top, rect.right, rect.bottom);
	Q_ASSERT(0);
    } else {
#ifdef QMAC_ONE_PIXEL_LOCK
	bool locked = LockPixels(GetGWorldPixMap((GWorldPtr)hd));
	Q_ASSERT(locked);
#endif
	data->w=w;
	data->h=h;
    }
}

int QPixmap::defaultDepth()
{
    GDHandle gd;
    gd=GetMainDevice();
    int wug=(**gd).gdCCDepth;
    if(wug) {
	return wug;
    } else {
	return 16;
    }
}

void QPixmap::setOptimization(Optimization)
{
}

QPixmap QPixmap::grabWindow(WId window, int x, int y, int w, int h)
{
    QPixmap pm;
    QWidget *widget = QWidget::find(window);
    if(widget) {
	if(w == -1)
	    w = widget->width() - x;
	if(h == -1)
	    h = widget->height() - y;
	pm = QPixmap(w, h, 32);
	bitBlt(&pm, 0, 0, widget, x, y, w, h);
    }
    return pm;
}


bool QPixmap::hasAlpha() const
{
    return data->alphapm || data->mask;
}

bool QPixmap::hasAlphaChannel() const
{
    return data->alphapm != 0;
}

IconRef qt_mac_create_iconref(const QPixmap &px)
{
    QMacSavedPortInfo pi; //save the current state
    //create icon
    IconFamilyHandle iconFamily = (IconFamilyHandle)NewHandle(0);
    //create data
    {
	struct {
	    OSType mac_type;
	    int width, height, depth;
	    bool mask;
	} images[] = {
	    { kThumbnail32BitData, 128, 128, 32, FALSE },
//	    { kHuge32BitData,       48,  48, 32, FALSE },
//	    { kLarge32BitData,      32,  32, 32, FALSE },
//	    { kSmall32BitData,      16,  16, 32, FALSE },
	    //masks
	    { kThumbnail8BitMask, 128, 128, 8, TRUE },
//	    { kHuge1BitMask,       48,  48, 1, TRUE },
//	    { kLarge1BitMask,      32,  32, 1, TRUE },
//	    { kSmall1BitMask,      16, 16,  1, TRUE },
	    { 0, 0, 0, 0, FALSE } };
	for(int i = 0; images[i].mac_type; i++) {
	    const QPixmap *in_pix = NULL;
	    if(images[i].mask)
		in_pix = px.mask();
	    else if(!px.isNull())
		in_pix = &px;

            const int x_rows = images[i].width * (images[i].depth/8), y_rows = images[i].height;
            Handle hdl = NewHandle(x_rows*y_rows);
            if(in_pix) {
		//make the image
		QImage im;
		im = *in_pix;
		im = im.smoothScale(images[i].width, images[i].height).convertDepth(images[i].depth);
		//set handle bits
		if(images[i].mac_type == kThumbnail8BitMask) {
		    for(int y = 0, h = 0; y < im.height(); y++) {
			for(int x = 0; x < im.width(); x++)
			    *((*hdl)+(h++)) = im.pixel(x, y) ? 0 : 255;
		    }
		} else {
		    char *dest = (*hdl);
#if defined(__i386__)
		    if(images[i].depth == 32) {
			for(int y = 0; y < im.height(); ++y) {
			    uint *source = (uint*)im.scanLine(y);
			    for(int x = 0; x < im.width(); ++x, dest += 4)
				*((uint*)dest) = CFSwapInt32(*(source + x));
			}
		    } else
#endif
                    {
                        for(int y = 0; y < y_rows; ++y)
                            memcpy(dest+(y*x_rows), im.scanLine(y), x_rows);
                    }

		}
	    } else {
                memset((*hdl), 0xFF, x_rows*y_rows);
            }
            OSStatus set = SetIconFamilyData(iconFamily, images[i].mac_type, hdl);
            if(set != noErr)
                qWarning("%s: %d -- Something went very wrong!! %ld", __FILE__, __LINE__, set);
            DisposeHandle(hdl);
	}
    }

    //acquire and cleanup
    IconRef ret;
    const OSType kQtCreator = 'CUTE';
    static int counter = 0;
    RegisterIconRefFromIconFamily(kQtCreator, (OSType)counter, iconFamily, &ret);
    AcquireIconRef(ret);
    UnregisterIconRef(kQtCreator, (OSType)counter);
    DisposeHandle((Handle)iconFamily);
    counter++;
    return ret;

}

QPixmap qt_mac_bigEndianCopy(const QPixmap &pixmap)
{
#if defined(__i386__)
    QPixmap fake_pm = pixmap;
    fake_pm.detach();

    GWorldPtr gworld = (GWorldPtr)fake_pm.handle();
#ifndef QMAC_ONE_PIXEL_LOCK
    bool locked = LockPixels(GetGWorldPixMap(gworld);
    Q_ASSERT(locked);
#endif
    long *sptr = (long *)GetPixBaseAddr(GetGWorldPixMap(gworld)), *srow;
    unsigned short sbpr = GetPixRowBytes(GetGWorldPixMap(gworld));

    for(int y=0; y < fake_pm.height(); y++) {
	srow = (long *)((char *)sptr + (y * sbpr));
	for(int x=0; x < fake_pm.width(); x++) {
            long tmp = *(srow + x);
            *(srow + x) = ((tmp << 8) & 0xff000000)
                          | ((tmp >> 8) & 0x000000ff)
                          | ((tmp << 8) & 0x00ff0000)
                          | ((tmp >> 8) & 0x0000ff00);

        }
    }
#ifndef QMAC_ONE_PIXEL_LOCK
    UnlockPixels(GetGWorldPixMap(gworld));
#endif
    return fake_pm;
#else
    return pixmap;
#endif

}

