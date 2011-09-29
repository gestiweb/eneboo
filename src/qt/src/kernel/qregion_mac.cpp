/****************************************************************************
** $Id: qt/qregion_mac.cpp   3.3.8   edited Jan 11 14:38 $
**
** Implementation of QRegion class for mac
**
** Created : 940729
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

#include "qregion.h"
#include "qpointarray.h"
#include "qbuffer.h"
#include "qimage.h"
#include "qbitmap.h"
#include "qt_mac.h"

#if 1
#define RGN_DATA_SIZE 200
static struct qt_mac_rgn_data_cache {
    bool rgndata_init;
    int rgndata_used;
    QRegion::QRegionData *cache[RGN_DATA_SIZE];
} data_cache = { FALSE, 0, { } };
static void qt_mac_cleanup_region_data() {
    data_cache.rgndata_init = FALSE;
    data_cache.rgndata_used = 0;
    for(int i = 0; i < RGN_DATA_SIZE; i++) {
	if(data_cache.cache[i]) {
	    delete data_cache.cache[i];
	    data_cache.cache[i] = 0;
	}
    }
}
QRegion::QRegionData *qt_mac_get_rgn_data() {
    QRegion::QRegionData *data = NULL;
    if(!data_cache.rgndata_init) {
	if(!data_cache.rgndata_init) {
	    data_cache.rgndata_init = TRUE;
	    for(int i = 0; i < RGN_DATA_SIZE; i++)
		data_cache.cache[i] = NULL;
	    qAddPostRoutine(qt_mac_cleanup_region_data);
	}
    } else if(data_cache.rgndata_used) {
	for(int i = 0; i < RGN_DATA_SIZE; i++) {
	    if(data_cache.cache[i]) {
		data = data_cache.cache[i];
		data_cache.cache[i] = NULL;
		data_cache.rgndata_used--;
		data->ref();
		break;
	    }
	}
    }
    if(!data) {
	data = new QRegion::QRegionData;
	Q_CHECK_PTR(data);
    }
    return data;
}
void qt_mac_free_rgn_data(QRegion::QRegionData *data)
{
    if(data_cache.rgndata_used < RGN_DATA_SIZE) {
	for(int i = 0; i < RGN_DATA_SIZE; i++) {
	    if(!data_cache.cache[i]) {
		data_cache.cache[i] = data;
		data_cache.rgndata_used++;
		break;
	    }
	}
    } else {
	delete data;
    }
}
#else
#define qt_mac_get_rgn_data() (new QRegionData)
#define qt_mac_free_rgn_data(x) delete x
#endif

#ifdef Q_WS_MACX
#define RGN_CACHE_SIZE 200
static bool rgncache_init=FALSE;
static int rgncache_used;
static RgnHandle rgncache[RGN_CACHE_SIZE];
static void qt_mac_cleanup_rgncache() {
    rgncache_init = FALSE;
    for(int i = 0; i < RGN_CACHE_SIZE; i++) {
	if(rgncache[i]) {
	    rgncache_used--;
	    DisposeRgn(rgncache[i]);
	    rgncache[i] = 0;
	}
    }
}
RgnHandle qt_mac_get_rgn() {
    RgnHandle ret = NULL;
    if(!rgncache_init) {
	rgncache_used = 0;
	rgncache_init = TRUE;
	for(int i = 0; i < RGN_CACHE_SIZE; i++)
	    rgncache[i] = 0;
	qAddPostRoutine(qt_mac_cleanup_rgncache);
    } else if(rgncache_used) {
	for(int i = 0; i < RGN_CACHE_SIZE; i++) {
	    if(rgncache[i]) {
		ret = rgncache[i];
		SetEmptyRgn(ret);
		rgncache[i] = NULL;
		rgncache_used--;
		break;
	    }
	}
    }
    if(!ret)
	ret = NewRgn();
    return ret;
}
void qt_mac_dispose_rgn(RgnHandle r) {
    if(rgncache_init && rgncache_used < RGN_CACHE_SIZE) {
	for(int i = 0; i < RGN_CACHE_SIZE; i++) {
	    if(!rgncache[i]) {
		rgncache_used++;
		rgncache[i] = r;
		break;
	    }
	}
    } else {
	DisposeRgn(r);
    }
}

#else
RgnHandle qt_mac_get_rgn() { return NewRgn(); }
void qt_mac_dispose_rgn(RgnHandle r) { DisposeRgn(r); }
#endif

// NOT REVISED

static QRegion *empty_region = 0;
static void cleanup_empty_region()
{
    delete empty_region;
    empty_region = 0;
}

QRegion::QRegion()
{
    if(!empty_region) {			// avoid too many allocs
	qAddPostRoutine(cleanup_empty_region);
	empty_region = new QRegion(TRUE);
	Q_CHECK_PTR(empty_region);
    }
    data = empty_region->data;
    data->ref();
}

void
QRegion::rectifyRegion()
{
    if(!data->is_rect)
	return;
    if(data->is_null) {
	detach();
	data->is_null = FALSE;
    }
    data->is_rect = FALSE;
    data->rgn = qt_mac_get_rgn();
    if(!data->rect.isEmpty()) 
	SetRectRgn(data->rgn, data->rect.left(), data->rect.top(), data->rect.right()+1, data->rect.bottom()+1);
}

/*!
    \internal
*/
RgnHandle
QRegion::handle(bool require_rgn) const
{
    if(require_rgn && data->is_rect)
	((QRegion *)this)->rectifyRegion();
    return data->is_rect ? NULL : data->rgn;
}

QRegion::QRegion(bool is_null)
{
    data = qt_mac_get_rgn_data();
    if((data->is_null = is_null)) {
	data->is_rect = TRUE;
	data->rect = QRect();
    } else {
	data->is_rect = FALSE;
	data->rgn = qt_mac_get_rgn();
    }
}

QRegion::QRegion(const QRect &r, RegionType t)
{
    if (r.isEmpty()) {
	if (!empty_region) {			// avoid too many allocs
	    qAddPostRoutine(cleanup_empty_region);
	    empty_region = new QRegion(TRUE);
	    Q_CHECK_PTR(empty_region);
	}
	data = empty_region->data;
	data->ref();
    } else {
	QRect rr = r.normalize();
	data = qt_mac_get_rgn_data();
	data->is_null = FALSE;
	if(t == Rectangle )	{		// rectangular region
	    data->is_rect = TRUE;
	    data->rect = r;
	} else {
	    Rect rect;
	    SetRect(&rect, rr.x(), rr.y(), rr.right()+1, rr.bottom()+1);
	    data->is_rect = FALSE;
	    data->rgn = qt_mac_get_rgn();
	    OpenRgn();
	    FrameOval(&rect);
	    CloseRgn(data->rgn);
	}
    }
}

//### We do not support winding yet, how do we do that?? --SAM
QRegion::QRegion(const QPointArray &a, bool)
{
    if(a.size() > 0) {
	data = qt_mac_get_rgn_data();
	data->is_null = FALSE;
	data->is_rect = FALSE;
	data->rgn = qt_mac_get_rgn();

	OpenRgn();
	MoveTo(a[0].x(), a[0].y());
	for(unsigned int loopc = 1; loopc < a.size(); loopc++) {
	    LineTo(a[loopc].x(), a[loopc].y());
	    MoveTo(a[loopc].x(), a[loopc].y());
	}
	LineTo(a[0].x(), a[0].y());
	CloseRgn(data->rgn);
    } else {
	if (!empty_region) {			// avoid too many allocs
	    qAddPostRoutine(cleanup_empty_region);
	    empty_region = new QRegion(TRUE);
	    Q_CHECK_PTR(empty_region);
	}
	data = empty_region->data;
	data->ref();
    }
}


QRegion::QRegion(const QRegion &r)
{
    data = r.data;
    data->ref();
}

//OPTIMIZATION FIXME, I think quickdraw can do this, this is just to get something going..
static RgnHandle qt_mac_bitmapToRegion(const QBitmap& bitmap)
{
    QImage image = bitmap.convertToImage();
    RgnHandle region = qt_mac_get_rgn(), rr;
#define AddSpan \
	{ \
    	   Rect rect; \
	   SetRect(&rect, prev1, y, (x-1)+1, (y+1)); \
	   rr = qt_mac_get_rgn(); \
    	   OpenRgn(); \
	   FrameRect(&rect); \
	   CloseRgn(rr); \
	   UnionRgn(rr, region, region); \
	   DisposeRgn(rr); \
	}

    int x, y;
    const int zero=0;
    bool little = image.bitOrder() == QImage::LittleEndian;
    for(y=0; y<image.height(); y++) {
	uchar *line = image.scanLine(y);
	int w = image.width();
	uchar all=zero;
	int prev1 = -1;
	for(x=0; x<w; ) {
	    uchar byte = line[x/8];
	    if(x>w-8 || byte!=all) {
		if(little) {
		    for (int b=8; b>0 && x<w; b--) {
			if(!(byte&0x01) == !all) { 			    // More of the same
			} else {
			    // A change.
			    if (all != zero) {
				AddSpan
				all = zero;
			    } else {
				prev1 = x;
				all = ~zero;
			    }
			}
			byte >>= 1;
			x++;
		    }
		} else {
		    for(int b=8; b>0 && x<w; b--) {
			if(!(byte&0x80) == !all) { 			    // More of the same
			} else { 			    // A change.
			    if (all!=zero) {
				AddSpan
				all = zero;
			    } else {
				prev1 = x;
				all = ~zero;
			    }
			}
			byte <<= 1;
			x++;
		    }
		}
	    } else {
		x+=8;
	    }
	}
	if(all != zero) {
	    AddSpan
	}
    }
    return region;
}


QRegion::QRegion(const QBitmap &bm)
{
    if ( bm.isNull() ) {
	if ( !empty_region ) {			// avoid too many allocs
	    qAddPostRoutine( cleanup_empty_region );
	    empty_region = new QRegion( TRUE );
	    Q_CHECK_PTR( empty_region );
	}
	data = empty_region->data;
	data->ref();
    } else {
	data = qt_mac_get_rgn_data();
	data->is_null = FALSE;
	data->is_rect = FALSE;
#if 0 //this should work, but didn't
	data->rgn = qt_mac_get_rgn();
	BitMapToRegion(data->rgn, (BitMap *)*GetGWorldPixMap((GWorldPtr)bm.handle()));
#else
	data->rgn = qt_mac_bitmapToRegion(bm);
#endif
    }
}


QRegion::~QRegion()
{
    if(data->deref()) {
	if(!data->is_rect)
	    qt_mac_dispose_rgn(data->rgn);
	qt_mac_free_rgn_data(data);
    }
}


QRegion &QRegion::operator=(const QRegion &r)
{
    r.data->ref();				// beware of r = r
    if(data->deref()) {
	if(!data->is_rect)
	    qt_mac_dispose_rgn(data->rgn);
	qt_mac_free_rgn_data(data);
    }
    data = r.data;
    return *this;
}


QRegion QRegion::copy() const
{
    if(data->is_null)
	return QRegion(TRUE);
     else if(data->is_rect)
	return QRegion(data->rect);

    QRegion r(FALSE);
    CopyRgn(data->rgn, r.data->rgn);
    return r;
}

bool QRegion::isNull() const
{
    return data->is_null;
}

bool QRegion::isEmpty() const
{
    if(data->is_null)
	return TRUE;
    if(data->is_rect)
	return data->rect.isEmpty();
    return EmptyRgn(data->rgn);
}

bool QRegion::contains(const QPoint &p) const
{
    if(data->is_null)
	return FALSE;
    else if(data->is_rect)
	return data->rect.contains(p);

    Point point;
    point.h = p.x();
    point.v = p.y();
    return PtInRgn(point, data->rgn);
}

bool QRegion::contains(const QRect &r) const
{
    if(data->is_null)
	return FALSE;
    else if(data->is_rect)
	return data->rect.intersects(r);

    Rect rect;
    SetRect(&rect, r.x(), r.y(), r.x() + r.width(), r.y() + r.height());
    return RectInRgn(&rect, data->rgn);
}

void QRegion::translate(int x, int y)
{
    if (data == empty_region->data)
	return;
    detach();
    if(data->is_rect)
	data->rect.moveBy(x, y);
    else
	OffsetRgn(data->rgn, x, y);
}

QRegion QRegion::unite(const QRegion &r) const
{
    if(data->is_null || r.data->is_null)
	return (!data->is_null ? this : &r)->copy();

    if(data->is_rect && r.data->is_rect && data->rect.contains(r.data->rect))
	return copy();

    if(data->is_rect)
	((QRegion *)this)->rectifyRegion();
    if(r.data->is_rect)
	((QRegion *)&r)->rectifyRegion();
    QRegion result(FALSE);
    UnionRgn(data->rgn, r.data->rgn, result.data->rgn);
    return result;
}

QRegion QRegion::intersect(const QRegion &r) const
{
    if(data->is_null || r.data->is_null)
	return QRegion();

    if(data->is_rect && r.data->is_rect)
	return QRegion(data->rect & r.data->rect);

    if(data->is_rect)
	((QRegion *)this)->rectifyRegion();
    if(r.data->is_rect)
	((QRegion *)&r)->rectifyRegion();
    QRegion result(FALSE);
    SectRgn(data->rgn, r.data->rgn, result.data->rgn);
    return result;
}

QRegion QRegion::subtract(const QRegion &r) const
{
    if(data->is_null || r.data->is_null )
	return copy();

    if(data->is_rect)
	((QRegion *)this)->rectifyRegion();
    if(r.data->is_rect)
	((QRegion *)&r)->rectifyRegion();
    QRegion result(FALSE);
    DiffRgn(data->rgn, r.data->rgn, result.data->rgn);
    return result;
}

QRegion QRegion::eor(const QRegion &r) const
{
    if(data->is_null || r.data->is_null)
	return (!data->is_null ? this : &r)->copy();

    if(data->is_rect)
	((QRegion *)this)->rectifyRegion();
    if(r.data->is_rect)
	((QRegion *)&r)->rectifyRegion();
    QRegion result(FALSE);
    XorRgn(data->rgn, r.data->rgn, result.data->rgn);
    return result;
}

QRect QRegion::boundingRect() const
{
    if(data->is_rect)
	return data->rect;
    Rect r;
    GetRegionBounds(data->rgn, &r);
    return QRect(r.left, r.top, (r.right - r.left), (r.bottom - r.top));
}

typedef QValueList<QRect> RectList;
static OSStatus qt_mac_get_rgn_rect(UInt16 msg, RgnHandle, const Rect *rect, void *myd)
{
    if(msg == kQDRegionToRectsMsgParse) {
	RectList *rl = (RectList *)myd;
	QRect rct(rect->left, rect->top, (rect->right - rect->left), (rect->bottom - rect->top));
	if(!rct.isEmpty())
	    rl->append(rct);
    }
    return noErr;
}

QMemArray<QRect> QRegion::rects() const
{
    if(data->is_rect) {
	if(data->rect.isEmpty())
	    return QMemArray<QRect>(0);
	QMemArray<QRect> ret(1);
	ret[0] = data->rect;
	return ret;
    }

    //get list
    RectList rl;
    OSStatus oss;
    RegionToRectsUPP cbk = NewRegionToRectsUPP(qt_mac_get_rgn_rect);
    oss = QDRegionToRects(data->rgn, kQDParseRegionFromTopLeft, cbk, (void *)&rl);
    DisposeRegionToRectsUPP(cbk);

    //check for error
    if(oss != noErr)
	return QMemArray<QRect>(0);

    //turn list into array
    QMemArray<QRect> ret(rl.count());
    int cnt = 0;
    for(RectList::Iterator it = rl.begin(); it != rl.end(); ++it) 
	ret[cnt++] = (*it);
    return ret; //done
}

void QRegion::setRects(const QRect *rects, int num)
{
    // Could be optimized
    if(num == 1) {
	*this = QRegion(*rects);
	return;
    }
    *this = QRegion();
    for (int i=0; i<num; i++)
	*this |= rects[i];
}

bool QRegion::operator==(const QRegion &r) const
{
    if(data == r.data)
	return TRUE;
    if(data->is_rect && r.data->is_rect)
	return data->rect == r.data->rect;
    if(data->is_rect)
	((QRegion *)this)->rectifyRegion();
    if(r.data->is_rect)
	((QRegion *)&r)->rectifyRegion();
    return EqualRgn(data->rgn, r.data->rgn);
}

