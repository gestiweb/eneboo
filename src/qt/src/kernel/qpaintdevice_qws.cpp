/****************************************************************************
**
** Implementation of QPaintDevice class for FB
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

#include "qpaintdevice.h"
#include "qpainter.h"
#include "qpaintdevicemetrics.h"
//#include "qimagepaintdevice.h"
#include "qwidget.h"
#include "qbitmap.h"
#include "qapplication.h"
#include "qwsdisplay_qws.h"
#include "qgfx_qws.h"

QPaintDevice::QPaintDevice( uint devflags )
{
    if ( !qApp ) {				// global constructor
#if defined(QT_CHECK_STATE)
	qFatal( "QPaintDevice: Must construct a QApplication before a "
		"QPaintDevice" );
#endif
	return;
    }
    devFlags = devflags;
    painters = 0;
}


QPaintDevice::~QPaintDevice()
{
#if defined(QT_CHECK_STATE)
    if ( paintingActive() )
	qWarning( "QPaintDevice: Cannot destroy paint device that is being "
		  "painted" );
#endif
}


bool QPaintDevice::cmd( int, QPainter *, QPDevCmdParam * )
{
#if defined(QT_CHECK_STATE)
    qWarning( "QPaintDevice::cmd: Not a paintable device" );
#endif
    return FALSE;
}

int QPaintDevice::metric( int m ) const
{
#if defined(QT_CHECK_STATE)
    qWarning( "QPaintDevice::metrics: Device has no metric information" );
#endif
    if ( m == QPaintDeviceMetrics::PdmDpiX ) {
	return 72;
    } else if ( m == QPaintDeviceMetrics::PdmDpiY ) {
	return 72;
    } else if ( m == QPaintDeviceMetrics::PdmNumColors ) {
	// FIXME: does this need to be a real value?
	return 256;
    } else {
	qDebug("Unrecognised metric %d!",m);
	return 0;
    }
}

int QPaintDevice::fontMet( QFont *, int, const char *, int ) const
{
    return 0;
}

int QPaintDevice::fontInf( QFont *, int ) const
{
    return 0;
}


void bitBlt( QPaintDevice *dst, int dx, int dy,
	     const QPaintDevice *src, int sx, int sy, int sw, int sh,
	     Qt::RasterOp rop, bool ignoreMask )
{
    if ( !src || !dst ) {
	/*
#if defined(QT_CHECK_NULL)
	Q_ASSERT( src != 0 );
	Q_ASSERT( dst != 0 );
#endif
	*/
	return;
    }

    if ( src->isExtDev() )
	return;

    QPaintDevice *pdev = QPainter::redirect( dst );
    if ( pdev )
	dst = pdev;

    int ts = src->devType();			// from device type
    int td = dst->devType();			// to device type

    if ( sw <= 0 ) {				// special width
	if ( sw < 0 )
	    sw = src->metric( QPaintDeviceMetrics::PdmWidth ) - sx;
	else
	    return;
    }
    if ( sh <= 0 ) {				// special height
	if ( sh < 0 )
	    sh = src->metric( QPaintDeviceMetrics::PdmHeight ) - sy;
	else
	    return;
    }

    if ( dst->paintingActive() && dst->isExtDev() ) {
	QPixmap *pm;				// output to picture/printer
	bool	 tmp_pm = TRUE;
	if ( ts == QInternal::Pixmap ) {
	    pm = (QPixmap*)src;
	    if ( sx != 0 || sy != 0 ||
		 sw != pm->width() || sh != pm->height() || ignoreMask ) {
		QPixmap *tmp = new QPixmap( sw, sh, pm->depth() );
		bitBlt( tmp, 0, 0, pm, sx, sy, sw, sh, Qt::CopyROP, TRUE );
		if ( pm->mask() && !ignoreMask ) {
		    QBitmap mask( sw, sh );
		    bitBlt( &mask, 0, 0, pm->mask(), sx, sy, sw, sh,
			    Qt::CopyROP, TRUE );
		    tmp->setMask( mask );
		}
		pm = tmp;
	    } else {
		tmp_pm = FALSE;
	    }
	} else if ( ts == QInternal::Widget ) {// bitBlt to temp pixmap
	    pm = new QPixmap( sw, sh );
	    Q_CHECK_PTR( pm );
	    bitBlt( pm, 0, 0, src, sx, sy, sw, sh );
	} else {
#if defined(QT_CHECK_RANGE)
	    qWarning( "bitBlt: Cannot bitBlt from device" );
#endif
	    return;
	}
	QPDevCmdParam param[3];
    QRect r(dx, dy, pm->width(), pm->height());
	param[0].rect	= &r;
	param[1].pixmap = pm;
	dst->cmd( QPaintDevice::PdcDrawPixmap, 0, param );
	if ( tmp_pm )
	    delete pm;
	return;
    }

    switch ( ts ) {
	case QInternal::Widget:
	case QInternal::Pixmap:
	case QInternal::System:			// OK, can blt from these
	    break;
	default:
#if defined(QT_CHECK_RANGE)
	    qWarning( "bitBlt: Cannot bitBlt from device type %x", ts );
#endif
	    return;
    }
    switch ( td ) {
	case QInternal::Widget:
	case QInternal::Pixmap:
	case QInternal::System:			// OK, can blt to these
	    break;
	default:
#if defined(QT_CHECK_RANGE)
	    qWarning( "bitBlt: Cannot bitBlt to device type %x", td );
#endif
	    return;
    }

    if ( rop > Qt::LastROP ) {
#if defined(QT_CHECK_RANGE)
	qWarning( "bitBlt: Invalid ROP code" );
#endif
	return;
    }

    bool mono_src;
    bool mono_dst;
    bool include_inferiors = FALSE;
    QPixmap *src_pm;

    if ( ts == QInternal::Pixmap ) {
	src_pm = (QPixmap*)src;
	mono_src = src_pm->depth() == 1;
    } else {
	src_pm = 0;
	mono_src = FALSE;
	include_inferiors = ((QWidget*)src)->testWFlags(Qt::WPaintUnclipped);
    }
    if ( td == QInternal::Pixmap ) {
	mono_dst = ((QPixmap*)dst)->depth() == 1;
	((QPixmap*)dst)->detach();		// changes shared pixmap
    } else {
	mono_dst = FALSE;
	include_inferiors = include_inferiors ||
	    ((QWidget*)dst)->testWFlags(Qt::WPaintUnclipped);
    }

    if ( mono_dst && !mono_src ) {	// dest is 1-bit pixmap, source is not
#if defined(QT_CHECK_RANGE)
	qWarning( "bitBlt: Incompatible destination pixmap" );
#endif
	return;
    }

    // XXX how much of the above is needed?

    // Temporary, needs fixing

    int ssh = src->metric( QPaintDeviceMetrics::PdmHeight );
    int dsh = dst->metric( QPaintDeviceMetrics::PdmHeight );
    int ssw = src->metric( QPaintDeviceMetrics::PdmWidth );
    int dsw = dst->metric( QPaintDeviceMetrics::PdmWidth );

    if(dy+sh>dsh) {
	sh=(dsh-dy);
    }

    if(sy+sh>ssh) {
	sh=(ssh-sy);
    }

    if(dx+sw>dsw) {
	sw=(dsw-dx);
    }

    if(sx+sw>ssw) {
	sw=(ssw-sx);
    }

    if ( sw <= 0 || sh <= 0 )
	return;

    QGfx * mygfx = dst->graphicsContext();
    if ( dst->devType() == QInternal::Widget )
	mygfx->setClipRegion( ((QWidget *)dst)->rect() );
    QBitmap * mymask=0;
    if(!ignoreMask) {
	if(src->devType()==QInternal::Pixmap) {
	    QPixmap * tmp=(QPixmap *)src;
	    mymask=( (QBitmap *)tmp->mask() );
	}
    }
    mygfx->setSource(src);
    mygfx->setAlphaType(QGfx::IgnoreAlpha);
    mygfx->setRop(rop);
    if(mymask) {
	if(!(mymask->isNull())) {
	    unsigned char * thebits=mymask->scanLine(0);
	    int ls=mymask->bytesPerLine();
	    // Force little-endian for now. Hmm.
	    mygfx->setAlphaType(QGfx::LittleEndianMask);
	    mygfx->setAlphaSource(thebits,ls);
	}
    }
    mygfx->blt(dx,dy,sw,sh,sx,sy);

    delete mygfx;
}

/*!
    \internal
*/
QWSDisplay *QPaintDevice::qwsDisplay()
{
    return qt_fbdpy;
}

Qt::HANDLE QPaintDevice::handle() const
{
    return 0;
}

/*!
    \internal
*/
unsigned char *QPaintDevice::scanLine(int) const
{
    return 0;
}

/*!
    \internal
*/
int QPaintDevice::bytesPerLine() const
{
    return 0;
}

// We should maybe return an extended-device Gfx by default here
// at the moment, it appears to return 0.
/*!
    \internal
*/
QGfx * QPaintDevice::graphicsContext(bool) const
{
    //qFatal("QGfx requested for QPaintDevice");
    return 0;
}

void QPaintDevice::setResolution( int )
{
}

int QPaintDevice::resolution() const
{
    return metric( QPaintDeviceMetrics::PdmDpiY );
}
