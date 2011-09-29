/****************************************************************************
** $Id: qt/qpainter_mac.cpp   3.3.8   edited Jan 11 14:38 $
**
** Implementation of QPainter class for Mac
**
** Created : 001018
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

#include "qpainter.h"
#include "qwidget.h"
#include "qbitmap.h"
#include "qpixmapcache.h"
#include "qptrlist.h"
#include "qfontdata_p.h"
#include "qtextcodec.h"
#include "qpaintdevicemetrics.h"
#include "qpaintdevice.h"
#include "qt_mac.h"
#include <qptrstack.h>
#include <qtextcodec.h>
#include <qprinter.h>
#include <private/qtextengine_p.h>
#include <private/qfontengine_p.h>
#include <private/qtextlayout_p.h>
#ifndef QMAC_NO_QUARTZ
# include <ApplicationServices/ApplicationServices.h>
#endif
#include <string.h>

class paintevent_item;
class QPainterPrivate
{
public:
    QMacSavedPortInfo *saved;
    QPixmap *brush_style_pix;
    uint unclipped : 1, locked : 1;

    //implementation details of the port information
    int offx, offy;
    struct {
	QRegion clippedreg, paintreg;
	uint crgn_dirty : 1,  clip_serial : 15;
	paintevent_item *paintevent;
    } cache;

#ifndef QMAC_NO_QUARTZ
    struct {
	CGContextRef context;
	int off_w, off_h;
    } cg;
#endif
};

/*****************************************************************************
  Current "active" QPainter
 *****************************************************************************/
QPainter *qt_mac_current_painter = 0;

/*****************************************************************************
  QPainter member functions
 *****************************************************************************/
bool qt_recreate_root_win(); //qwidget_mac.cpp
static void drawTile(QPainter *, int, int, int, int, const QPixmap &, int, int);
QPoint posInWindow(QWidget *w);
QRegion make_region(RgnHandle handle);
void unclippedBitBlt(QPaintDevice *dst, int dx, int dy,
		     const QPaintDevice *src, int sx, int sy, int sw, int sh,
		     Qt::RasterOp rop, bool imask, bool set_fore_colour); //qpaintdevice_mac.cpp
RgnHandle qt_mac_get_rgn(); //qregion_mac.cpp
void qt_mac_dispose_rgn(RgnHandle r); //qregion_mac.cpp

/* paintevent magic to provide Windows semantics on Qt/Mac */
class paintevent_item
{
    QPaintDevice* dev;
    QRegion clipRegion;
public:
    paintevent_item(QPaintDevice *d, QRegion r) : dev(d), clipRegion(r) { }
    inline bool operator==(QPaintDevice *rhs) const { return rhs == dev; }
    inline QPaintDevice *device() const { return dev; }
    inline QRegion region() const { return clipRegion; }
};
QPtrStack<paintevent_item> paintevents;

void qt_set_paintevent_clipping(QPaintDevice* dev, const QRegion& region)
{
    QRegion r = region;
    if(dev && dev->devType() == QInternal::Widget) {
	QWidget *w = (QWidget *)dev;
	QPoint mp(posInWindow(w));
	r.translate(mp.x(), mp.y());
    }
    paintevents.push(new paintevent_item(dev, r));
}

void qt_clear_paintevent_clipping(QPaintDevice *dev)
{
    if(paintevents.isEmpty() || !((*paintevents.current()) == dev)) {
	qDebug("Qt: internal: WH0A, qt_clear_paintevent_clipping mismatch.");
	return;
    }
    delete paintevents.pop();
}

//used internally to create a pixmap for tiling (CopyBits is a bit slow, and as such a couple
//medium sized blts is much cheaper than many small ones). I remember tuning this carefully
//when implementing the mac styles as well, so here just use the same number as in the mac styles
//this will additionally pay off by not causing this code to really do much for those pixmaps
static const QPixmap *qt_mac_make_tiled_pixmap(const QPixmap *pixmap, const QColor &color) //size not used right now
{
    if(!pixmap || pixmap->isNull())
        return pixmap;
    const int maxim = 200;
    const int tile_width = maxim - (maxim % pixmap->width()),
             tile_height = maxim - (maxim % pixmap->height());
    if(pixmap->width() >= tile_width || pixmap->height() >= tile_height)
        return pixmap;

    QString key;
    key.sprintf("$qt-tile-pixmap%d-%d-%d-%d", pixmap->serialNumber(), color.red(), color.green(), color.blue());
    if(QPixmap *ret = QPixmapCache::find(key))
        return ret;
    QPixmap *px = new QPixmap(tile_width, tile_height);
    {
        QPainter p(px);
        p.drawTiledPixmap(0, 0, tile_width, tile_height, *pixmap);
    }
    if(pixmap->mask()) {
        QBitmap bitmap(tile_width, tile_height);
        {
            QPainter p(&bitmap);
            p.drawTiledPixmap(0, 0, tile_width, tile_height, *pixmap->mask());
        }
        px->setMask(bitmap);
    }
    QPixmapCache::insert(key, px);
    return px;
}

void QPainter::initialize()
{
}

void QPainter::cleanup()
{
}

void QPainter::destroy()
{
    delete d;
}

void QPainter::init()
{
    flags = IsStartingUp;
    bg_col = white;                             // default background color
    bg_mode = TransparentMode;                  // default background mode
    rop = CopyROP;                              // default ROP
    tabstops = 0;                               // default tabbing
    tabarray = 0;
    tabarraylen = 0;
    ps_stack = 0;
    wm_stack = 0;
    pdev = 0;
    txop = txinv = 0;
    pfont = 0;
    block_ext = FALSE;

    d = new QPainterPrivate;
    d->saved = 0;
    d->cache.paintevent = 0;
    d->cache.clip_serial = 0;
    d->brush_style_pix = 0;
    d->cache.crgn_dirty = d->locked = d->unclipped = FALSE;
#ifndef QMAC_NO_QUARTZ
    d->cg.context = 0;
    d->cg.off_w = d->cg.off_h = 0;
#endif
    d->cache.clippedreg = d->cache.paintreg = QRegion();
    d->offx = d->offy = 0;
}


void QPainter::setFont(const QFont &font)
{
#if defined(QT_CHECK_STATE)
    if(!isActive())
	qWarning("QPainter::setFont: Will be reset by begin()");
#endif
    if(cfont.d != font.d) {
	cfont = font;
	setf(DirtyFont);
    }
}

static int ropCodes[] = {			// ROP translation table
    patCopy, addMax, patXor, subPin, notPatCopy,
    notPatOr, patXor, adMin,666, 666, 666, 666, 666,
    666, 666, 666, 666
};

void QPainter::updateFont()
{
    clearf(DirtyFont);
    if(testf(ExtDev)) {
        if (pdev->devType() == QInternal::Printer) {
            delete pfont;
            pfont = new QFont(cfont.d, pdev);
        }
	QPDevCmdParam param[1];
	param[0].font = &cfont;
	if(!pdev->cmd(QPaintDevice::PdcSetFont,this,param) || !pdev->handle())
	    return;
    }
    updatePen();                            // force a non-cached GC
    cfont.macSetFont(pdev);
}

void QPainter::updatePen()
{
    if(testf(ExtDev)) {
	QPDevCmdParam param[1];
	param[0].pen = &cpen;
	if(!pdev->cmd(QPaintDevice::PdcSetPen, this, param) || !pdev->handle())
	    return;
    }

    //pen size
    int dot = cpen.width();
    if(dot < 1)
	dot = 1;
    PenSize(dot, dot);

    //forecolor
    ::RGBColor f;
    f.red = cpen.color().red()*256;
    f.green = cpen.color().green()*256;
    f.blue = cpen.color().blue()*256;
    Pattern pat;
    GetQDGlobalsBlack(&pat);
    PenPat(&pat);
    RGBForeColor(&f);

    //backcolor
    ::RGBColor b;
    b.red = bg_col.red()*256;
    b.green = bg_col.green()*256;
    b.blue = bg_col.blue()*256;
    RGBBackColor(&b);

    //setup pen mode
#if defined(Q_WS_MACX)
    //Throw away a desktop when you paint into it non copy mode (xor?) I do this because
    //xor doesn't really work on an overlay widget FIXME
    if(rop != CopyROP && pdev->devType() == QInternal::Widget && ((QWidget *)pdev)->isDesktop())
	qt_recreate_root_win();
#endif
    int penmode = ropCodes[rop];
    PenMode(penmode);
    if(penmode == subPin || penmode == addPin)
        OpColor(&b);
}

void QPainter::updateBrush()
{
    if(testf(ExtDev)) {
	QPDevCmdParam param[1];
	param[0].brush = &cbrush;
	if(!pdev->cmd(QPaintDevice::PdcSetBrush,this,param) || !pdev->handle())
	    return;
    }

    //pattern
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
    d->brush_style_pix = 0;
    int bs = cbrush.style();
    if(bs >= Dense1Pattern && bs <= DiagCrossPattern) {
	QString key;
	key.sprintf("$qt-brush$%d", bs);
	d->brush_style_pix = QPixmapCache::find(key);
	if(!d->brush_style_pix) {                        // not already in pm dict
	    uchar *pat=pat_tbl[bs-Dense1Pattern];
	    int size = 16;
	    if(bs<=Dense7Pattern)
		size=8;
	    else if(bs<=CrossPattern)
		size=24;
	    d->brush_style_pix = new QPixmap(size, size);
	    Q_CHECK_PTR(d->brush_style_pix);
	    d->brush_style_pix->setMask(QBitmap(size, size, pat, FALSE));
	    QPixmapCache::insert(key, d->brush_style_pix);
	}
	d->brush_style_pix->fill(cbrush.color());
    } else if(bs == CustomPattern) {
        if(cbrush.data->pixmap->isQBitmap()) {
            QString key;
            key.sprintf("$qt-brush$%d-%d", bs, cbrush.data->pixmap->serialNumber());
            d->brush_style_pix = QPixmapCache::find(key);
            if(!d->brush_style_pix) {                        // not already in pm dict
                const int width = cbrush.data->pixmap->width(),
                         height = cbrush.data->pixmap->height();
                d->brush_style_pix = new QPixmap(width, height);
                d->brush_style_pix->setMask(*((QBitmap*)cbrush.data->pixmap));
                QPixmapCache::insert(key, d->brush_style_pix);
            }
            d->brush_style_pix->fill(cbrush.color());
        }
    }

    //forecolor
    ::RGBColor f;
    f.red = cbrush.color().red()*256;
    f.green = cbrush.color().green()*256;
    f.blue = cbrush.color().blue()*256;
    Pattern pat;
    GetQDGlobalsBlack(&pat);
    PenPat(&pat);
    RGBForeColor(&f);

    //background
    ::RGBColor b;
    b.red = bg_col.red()*256;
    b.green = bg_col.green()*256;
    b.blue = bg_col.blue()*256;
    RGBBackColor(&b);

    //setup pen mode
#if defined(Q_WS_MACX)
    //Throw away a desktop when you paint into it non copy mode (xor?) I do this because
    //xor doesn't really work on an overlay widget FIXME
    if(rop != CopyROP && pdev->devType() == QInternal::Widget && ((QWidget *)pdev)->isDesktop())
	qt_recreate_root_win();
#endif
    int penmode = ropCodes[rop];
    PenMode(penmode);
    if(penmode == subPin || penmode == addPin)
        OpColor(&b);
}

bool QPainter::begin(const QPaintDevice *pd, bool unclipp)
{
    if(isActive()) {                         // already active painting
#if defined(QT_CHECK_STATE)
	qWarning("QPainter::begin: Painter is already active."
		 "\n\tYou must end() the painter before a second begin()");
#endif
	return FALSE;
    }
    if(!pd) {
#if defined(QT_CHECK_NULL)
	qWarning("QPainter::begin: Paint device cannot be null");
#endif
	return FALSE;
    }

    //save the gworld now, we'll reset it in end()
    d->saved = new QMacSavedPortInfo;

    const QWidget *copyFrom = 0;
    pdev = redirect( (QPaintDevice*)pd );
    if ( pdev ) {				    // redirected paint device?
	if ( pd->devType() == QInternal::Widget )
	    copyFrom = (const QWidget *)pd;	    // copy widget settings
    } else {
	pdev = (QPaintDevice*)pd;
    }

    if(pdev->isExtDev() && pdev->paintingActive()) {
	// somebody else is already painting
#if defined(QT_CHECK_STATE)
	qWarning("QPainter::begin: Another QPainter is already painting "
		 "this device;\n\tAn extended paint device can only be painted "
		 "by one QPainter at a time.");
#endif
	return FALSE;
    }

    int dt = pdev->devType();                   // get the device type
    bool reinit = flags != IsStartingUp;        // 2nd or 3rd etc. time called
    flags = IsActive | DirtyFont;               // init flags

    if((pdev->devFlags & QInternal::ExternalDevice) != 0)
	// this is an extended device
	setf(ExtDev);
    else if(dt == QInternal::Pixmap)         // device is a pixmap
	((QPixmap*)pdev)->detach();             // will modify it

    if(testf(ExtDev)) {                      // external device
	if(!pdev->cmd(QPaintDevice::PdcBegin, this, 0)) {   // could not begin painting
	    clearf(IsActive);
	    pdev = 0;
	    return FALSE;
	}
	if(tabstops)                         // update tabstops for device
	    setTabStops(tabstops);
	if(tabarray)                         // update tabarray for device
	    setTabArray(tabarray);
    }
    hd = pdev->handle();                       // get handle to drawable

    pdev->painters++;                           // also tell paint device
    bro = QPoint(0, 0);
    if(reinit) {
	bg_mode = TransparentMode;              // default background mode
	rop = CopyROP;                          // default ROP
	wxmat.reset();                          // reset world xform matrix
	xmat.reset();
	ixmat.reset();
	txop = txinv = 0;
	if(dt != QInternal::Widget) {
	    QFont  defaultFont;                 // default drawing tools
	    QPen   defaultPen;
	    QBrush defaultBrush;
	    cfont  = defaultFont;               // set these drawing tools
	    cpen   = defaultPen;
	    cbrush = defaultBrush;
	    bg_col = white;                     // default background color
	    // was white
	}
    }
    d->cache.clip_serial = 0;
    d->cache.paintevent = 0;
    d->cache.crgn_dirty = FALSE;
#ifndef QMAC_NO_QUARTZ
    d->cg.off_w = d->cg.off_h = 0;
#endif
    d->offx = d->offy = wx = wy = vx = vy = 0;                      // default view origins

    d->unclipped = unclipp;
    if(pdev->devType() == QInternal::Widget) {                    // device is a widget
	QWidget *w = (QWidget*)pdev;
	cfont = w->font();                      // use widget font
	cpen = QPen(w->foregroundColor());    // use widget fg color
	if(reinit) {
	    QBrush defaultBrush;
	    cbrush = defaultBrush;
	}
	bg_col = w->backgroundColor();          // use widget bg color
	ww = vw = w->width();                   // default view size
	wh = vh = w->height();
	if(!d->unclipped)
	    d->unclipped = (bool)w->testWFlags(WPaintUnclipped);
	if(!d->locked) {
	    LockPortBits(GetWindowPort((WindowPtr)w->handle()));
	    d->locked = TRUE;
	}

#ifdef Q_WS_MACX
	if(w->isDesktop()) {
	    if(!d->unclipped)
		qWarning("QPainter::begin: Does not support clipped desktop on MacOSX");
	    ShowWindow((WindowPtr)w->handle());
	}
#endif
    } else if(pdev->devType() == QInternal::Pixmap) {             // device is a pixmap
	QPixmap *pm = (QPixmap*)pdev;
	if(pm->isNull()) {
#if defined(QT_CHECK_NULL)
	    qWarning("QPainter::begin: Cannot paint null pixmap");
#endif
	    end();
	    return FALSE;
	}
#ifndef QMAC_ONE_PIXEL_LOCK
	if(!d->locked) {
	    bool locked = LockPixels(GetGWorldPixMap((GWorldPtr)pm->handle()));
	    Q_ASSERT(locked);
	    d->locked = TRUE;
	}
#endif
	ww = vw = pm->width();                  // default view size
	wh = vh = pm->height();
    }
#ifndef QMAC_NO_QUARTZ
    d->cg.context = pdev->macCGContext(!d->unclipped);
#endif
    initPaintDevice(TRUE); //force setting paint device, this does unclipped fu

    if(testf(ExtDev)) {               // external device
	ww = vw = pdev->metric(QPaintDeviceMetrics::PdmWidth);
	wh = vh = pdev->metric(QPaintDeviceMetrics::PdmHeight);
    }

    if(ww == 0)
	ww = wh = vw = vh = 1024;
    if(copyFrom) {                           // copy redirected widget
	cfont = copyFrom->font();
	cpen = QPen(copyFrom->foregroundColor());
	bg_col = copyFrom->backgroundColor();
    }
    if(testf(ExtDev)) {                      // external device
	setBackgroundColor(bg_col);           // default background color
	setBackgroundMode(TransparentMode);   // default background mode
	setRasterOp(CopyROP);                 // default raster operation
    }

    updateBrush();
    updatePen();
    return TRUE;
}

bool QPainter::end()				// end painting
{
    if(!isActive()) {
#if defined(QT_CHECK_STATE)
	qWarning("QPainter::end: Missing begin() or begin() failed");
#endif
	return FALSE;
    }
    killPStack();

    if(testf(ExtDev))
	pdev->cmd(QPaintDevice::PdcEnd, this, 0);

    if(d->locked) {
	if(pdev->devType() == QInternal::Widget)
	    UnlockPortBits(GetWindowPort((WindowPtr)pdev->handle()));
#ifndef QMAC_ONE_PIXEL_LOCK
	else
	    UnlockPixels(GetGWorldPixMap((GWorldPtr)pdev->handle()));
#endif
	d->locked = FALSE;
    }

    //reset the value we got in begin()
    delete d->saved;
    d->saved = 0;
    if(qt_mac_current_painter == this)
	qt_mac_current_painter = 0;
#ifdef Q_WS_MACX
    if(pdev->painters == 1 &&
       pdev->devType() == QInternal::Widget && ((QWidget*)pdev)->isDesktop())
	HideWindow((WindowPtr)pdev->handle());
#endif
#ifndef QMAC_NO_QUARTZ
    if(d->cg.context)
	CGContextFlush(d->cg.context);
#endif
    if(pfont) {
	delete pfont;
	pfont = 0;
    }

    flags = 0;
    pdev->painters--;
    hd = 0;
    pdev = 0;
    return TRUE;
}

void QPainter::flush(const QRegion &rgn, CoordinateMode m)
{
    if(!isActive())
	return;
    initPaintDevice();

    QRegion b;
    if(m == CoordDevice)
	b = rgn;
    else
	b = xmat * rgn;
    b.translate(d->offx, d->offy);
    QMacSavedPortInfo::flush(pdev, b & d->cache.paintreg, TRUE);
}

void QPainter::flush()
{
    if(!isActive())
	return;
    initPaintDevice();
    QMacSavedPortInfo::flush(pdev, d->cache.paintreg, TRUE);
}

void QPainter::setBackgroundColor(const QColor &c)
{
    if(!isActive()) {
#if defined(QT_CHECK_STATE)
	qWarning("Qt: QPainter::setBackgroundColor: Call begin() first");
#endif
	return;
    }
    bg_col = c;
    if(testf(ExtDev)) {
	QPDevCmdParam param[1];
	param[0].color = &bg_col;
	if(!pdev->cmd(QPaintDevice::PdcSetBkColor, this, param) || !pdev->handle())
	    return;
    }
}

void QPainter::setBackgroundMode(BGMode m)
{
    if(!isActive()) {
#if defined(CHECK_STATE)
	qWarning("Qt: QPainter::setBackgroundMode: Call begin() first");
#endif
	return;
    }
    if(m != TransparentMode && m != OpaqueMode) {
#if defined(CHECK_RANGE)
	qWarning("Qt: QPainter::setBackgroundMode: Invalid mode");
#endif
	return;
    }
    bg_mode = m;
    if(testf(ExtDev)) {
	QPDevCmdParam param[1];
	param[0].ival = m;
	if(!pdev->cmd(QPaintDevice::PdcSetBkMode, this, param) || !pdev->handle())
	    return;
    }
}

void QPainter::setRasterOp(RasterOp r)
{
    if(!isActive()) {
#if defined(CHECK_STATE)
	qWarning("Qt: QPainter::setRasterOp: Call begin() first");
#endif
	return;
    }
    if((uint)r > LastROP) {
#if defined(CHECK_RANGE)
	qWarning("Qt: QPainter::setRasterOp: Invalid ROP code");
#endif
	return;
    }
    if(ropCodes[r] == 666) {
	//qWarning("Woops, we don't have that rasterop, FIXME!!");
	r = XorROP;
    }
    rop = r;
    if(testf(ExtDev)) {
	QPDevCmdParam param[1];
	param[0].ival = r;
	if(!pdev->cmd(QPaintDevice::PdcSetROP, this, param) || !pdev->handle())
	    return;
    }
}

void QPainter::setBrushOrigin(int x, int y)
{
    if(!isActive()) {
#if defined(CHECK_STATE)
	qWarning("Qt: QPainter::setBrushOrigin: Call begin() first");
#endif
	return;
    }
    bro = QPoint(x,y);
    if(testf(ExtDev)) {
	QPDevCmdParam param[1];
	param[0].point = &bro;
	if(!pdev->cmd(QPaintDevice::PdcSetBrushOrigin, this, param) || !pdev->handle())
	    return;
    }
    updateBrush();				// get non-cached brush GC
}

void QPainter::setClipping(bool b)
{
    if(!isActive()) {
#if defined(CHECK_STATE)
	qWarning("Qt: QPainter::setClipping: Call begin() first");
#endif
	return;
    }

    if(b)
	setf(ClipOn);
    else
	clearf(ClipOn);
    d->cache.crgn_dirty = TRUE;
    initPaintDevice(); //reset the clip region
}


void QPainter::setClipRect(const QRect &r, CoordinateMode m)
{
    setClipRegion(QRegion(r), m);
}

void QPainter::setClipRegion(const QRegion &rgn, CoordinateMode m)
{
    if(!isActive()) {
#if defined(CHECK_STATE)
	qWarning("Qt: QPainter::setClipRegion: Call begin() first");
#endif
	return;
    }

    if(m == CoordDevice)
	crgn = rgn;
    else
	crgn = xmat * rgn;

    setf(ClipOn);
    d->cache.crgn_dirty = TRUE;
    initPaintDevice(); //reset clip region
}

void QPainter::drawPolyInternal(const QPointArray &a, bool close)
{
    if(a.isEmpty())
	return;
    initPaintDevice();
    if(d->cache.paintreg.isEmpty())
	return;

    PolyHandle polyHandle = OpenPoly();
    MoveTo(a[0].x()+d->offx, a[0].y()+d->offy);
    for(uint x = 1; x < a.size(); x++)
	LineTo(a[x].x()+d->offx, a[x].y()+d->offy);
    if(close)
	LineTo(a[0].x()+d->offx, a[0].y()+d->offy);
    ClosePoly();

    if(close && this->brush().style() != NoBrush) {
	updateBrush();
	if(this->brush().style() == SolidPattern) {
	    PaintPoly(polyHandle);
	} else {
	    QPixmap *pm = 0;
	    if(d->brush_style_pix) {
		pm = d->brush_style_pix;
		if(bg_mode == OpaqueMode) {
		    ::RGBColor f;
		    f.red = bg_col.red()*256;
		    f.green = bg_col.green()*256;
		    f.blue = bg_col.blue()*256;
		    RGBForeColor(&f);
		    PaintPoly(polyHandle);
		}
	    } else {
		pm = cbrush.data->pixmap;
	    }

	    if(pm && !pm->isNull()) {
		//save the clip
		bool clipon = testf(ClipOn);
		QRegion clip = crgn;

		//create the region
		QRegion newclip(a);
		if(clipon && !clip.isNull())
		    newclip &= clip;
		setClipRegion(newclip);

		//turn off translation flags
		uint save_flags = flags;
		flags = IsActive | ClipOn;

		//draw the brush
		QRect r(a.boundingRect());
		drawTiledPixmap(r.x(), r.y(), r.width(), r.height(), *qt_mac_make_tiled_pixmap(pm, cbrush.color()),
                                r.x() - bro.x(), r.y() - bro.y());

		//restore translation flags
		flags = save_flags;

		//restore the clip
		if(clipon)
		    setClipRegion(clip);
		else
		    setClipping(FALSE);
	    }
	}
    }
    if(cpen.style() != NoPen) {
	updatePen();
	FramePoly(polyHandle);
    }
    KillPoly(polyHandle);
}

void QPainter::drawPoint(int x, int y)
{
    if(!isActive())
	return;
    if(testf(ExtDev|VxF|WxF)) {
	if(testf(ExtDev)) {
	    QPDevCmdParam param[1];
	    QPoint p(x, y);
	    param[0].point = &p;
	    if(!pdev->cmd(QPaintDevice::PdcDrawPoint, this, param) || !pdev->handle())
		return;
	}
	map(x, y, &x, &y);
    }

    if(cpen.style() != NoPen) {
	initPaintDevice();
	if(d->cache.paintreg.isEmpty())
	    return;
	updatePen();
        MoveTo(x + d->offx, y + d->offy);
        Line(0, 0);
    }
}

void QPainter::drawPoints(const QPointArray& a, int index, int npoints)
{
    if(npoints < 0)
	npoints = a.size() - index;
    if(index + npoints > (int)a.size())
	npoints = a.size() - index;
    if(!isActive() || npoints < 1 || index < 0)
	return;
    QPointArray pa = a;
    if(testf(ExtDev|VxF|WxF)) {
	if(testf(ExtDev)) {
	    QPDevCmdParam param[1];
	    for(int i=0; i<npoints; i++) {
		QPoint p(pa[index+i].x(), pa[index+i].y());
		param[0].point = &p;
		if(!pdev->cmd(QPaintDevice::PdcDrawPoint,this,param) || !pdev->handle())
		    return;
	    }
	}
	if(txop != TxNone) {
	    pa = xForm(a, index, npoints);
	    if(pa.size() != a.size()) {
		index = 0;
		npoints = pa.size();
	    }
	}
    }

    if(cpen.style() != NoPen) {
	initPaintDevice();
	if(d->cache.paintreg.isEmpty())
	    return;
	updatePen();
	for(int i=0; i<npoints; i++) {
	    MoveTo(pa[index+i].x()+d->offx, pa[index+i].y()+d->offy);
	    Line(0,1);
	}
    }
}

void QPainter::moveTo(int x, int y)
{
  if(!isActive())
    return;
  if(testf(ExtDev|VxF|WxF)) {
    if(testf(ExtDev)) {
      QPDevCmdParam param[1];
      QPoint p(x, y);
      param[0].point = &p;
      if(!pdev->cmd(QPaintDevice::PdcMoveTo,this,param) || !pdev->handle())
	return;
    }
    map(x, y, &x, &y);
  }

  initPaintDevice();
  MoveTo(x+d->offx, y+d->offy);
#ifndef QMAC_NO_QUARTZ
  if(d->cg.context)
      CGContextMoveToPoint(d->cg.context, d->cg.off_w - (x+d->offx),
			   d->cg.off_h - (y+d->offy));
#endif
}

void QPainter::lineTo(int x, int y)
{
  if(!isActive())
    return;
  if(testf(ExtDev|VxF|WxF)) {
    if(testf(ExtDev)) {
      QPDevCmdParam param[1];
      QPoint p(x, y);
      param[0].point = &p;
      if(!pdev->cmd(QPaintDevice::PdcLineTo, this, param) || !pdev->handle())
	return;
    }
    map(x, y, &x, &y);
  }

  initPaintDevice();
  if(d->cache.paintreg.isEmpty())
      return;

#ifndef QMAC_NO_QUARTZ
  if(d->cg.context) {
      CGContextAddLineToPoint(d->cg.context, d->cg.off_w - (d->offx+x),
			      d->cg.off_h - (d->offy+y));
  } else
#endif
  {
      updatePen();
      LineTo(x+d->offx,y+d->offy);
  }
}

void QPainter::drawLine(int x1, int y1, int x2, int y2)
{
  if(!isActive())
    return;
  if(testf(ExtDev|VxF|WxF)) {
    if(testf(ExtDev)) {
      QPDevCmdParam param[2];
      QPoint p1(x1, y1), p2(x2, y2);
      param[0].point = &p1;
      param[1].point = &p2;
      if(!pdev->cmd(QPaintDevice::PdcDrawLine, this, param) || !pdev->handle())
	return;
    }
    map(x1, y1, &x1, &y1);
    map(x2, y2, &x2, &y2);
  }

  initPaintDevice();
  if(d->cache.paintreg.isEmpty())
      return;
  updatePen();
  MoveTo(x1+d->offx,y1+d->offy);
  LineTo(x2+d->offx,y2+d->offy);
}

void QPainter::drawRect(int x, int y, int w, int h)
{
    if(!isActive())
	return;
    if(testf(ExtDev|VxF|WxF)) {
	if(testf(ExtDev)) {
	    QPDevCmdParam param[1];
	    QRect r(x, y, w, h);
	    param[0].rect = &r;
	    if(!pdev->cmd(QPaintDevice::PdcDrawRect, this, param) || !pdev->handle())
		return;
	}
	if(txop == TxRotShear) {             // rotate/shear polygon
	    QPointArray a = xmat.mapToPolygon(QRect(x, y, w, h));
	    a.resize(5);
	    a.setPoint(4, a.point(0));
	    drawPolyInternal(a);
	    return;
	}
	map(x, y, w, h, &x, &y, &w, &h);
    }
    if(w <= 0 || h <= 0) {
	if(w == 0 || h == 0)
	    return;
	fix_neg_rect(&x, &y, &w, &h);
    }

    initPaintDevice();
    if(d->cache.paintreg.isEmpty())
	return;

    Rect rect;
    SetRect(&rect, x+d->offx, y+d->offy, x + w+d->offx, y + h+d->offy);
    if(this->brush().style() != NoBrush) {
	updateBrush();
	if(this->brush().style() == SolidPattern) {
	    PaintRect(&rect);
	} else {
	    QPixmap *pm = 0;
	    if(d->brush_style_pix) {
		pm = d->brush_style_pix;
		if(bg_mode == OpaqueMode) {
		    ::RGBColor f;
		    f.red = bg_col.red()*256;
		    f.green = bg_col.green()*256;
		    f.blue = bg_col.blue()*256;
		    RGBForeColor(&f);
		    PaintRect(&rect);
		}
            } else {
		pm = cbrush.data->pixmap;
	    }

	    if(pm && !pm->isNull()) {
		//save the clip
		bool clipon = testf(ClipOn);
		QRegion clip = crgn;

		//create the region
		QRegion newclip(QRect(x, y, w, h));
		if(clipon && !clip.isNull())
		    newclip &= clip;
		setClipRegion(newclip);

		//turn off translation flags
		uint save_flags = flags;
		flags = IsActive | ClipOn;

		//draw the brush
                drawTiledPixmap(x, y, w, h, *qt_mac_make_tiled_pixmap(pm, cbrush.color()), x - bro.x(), y - bro.y());

		//restore translation flags
		flags = save_flags;

		//restore the clip
		if(clipon)
		    setClipRegion(clip);
		else
		    setClipping(FALSE);
	    }
	}
    }
    if(cpen.style() != NoPen) {
	updatePen();
	FrameRect(&rect);
    }
}

void QPainter::drawWinFocusRect(int x, int y, int w, int h)
{
    drawWinFocusRect(x, y, w, h, TRUE, color0);
}

void QPainter::drawWinFocusRect(int x, int y, int w, int h,
				 const QColor &bgColor)
{
    drawWinFocusRect(x, y, w, h, FALSE, bgColor);
}

void QPainter::drawWinFocusRect(int x, int y, int w, int h,
				 bool xorPaint, const QColor &bgColor)
{
    if(!isActive())
	return;
#ifndef QT_NO_TRANSFORMATIONS
    if(txop == TxRotShear)
	return;
#endif
    QPen    old_pen = cpen;
    QBrush  old_brush = cbrush;
    RasterOp old_rop = (RasterOp)rop;

    setBrush(QBrush());

    if(xorPaint) {
	if(QColor::numBitPlanes() <= 8)
	    setPen(color1);
	else
	    setPen(white);
	setRasterOp(XorROP);
    } else {
	if(qGray(bgColor.rgb()) < 128)
	    setPen(white);
	else
	    setPen(black);
    }
    if(w <= 0 || h <= 0) {
	if(w == 0 || h == 0)
	    return;
	fix_neg_rect(&x, &y, &w, &h);
    }

    cpen.setStyle(DashLine);
    updatePen();
    if(cpen.style() != NoPen)
	drawRect(x, y, w, h);
    setRasterOp(old_rop);
    setPen(old_pen);
    setBrush(old_brush);
}

void QPainter::drawRoundRect(int x, int y, int w, int h, int xRnd, int yRnd)
{
    if(!isActive())
	return;
    if(xRnd <= 0 || yRnd <= 0) {
	drawRect(x, y, w, h);                 // draw normal rectangle
	return;
    }
    if(xRnd >= 100)                          // fix ranges
	xRnd = 99;
    if(yRnd >= 100)
	yRnd = 99;
    if(testf(ExtDev|VxF|WxF)) {
	if(testf(ExtDev)) {
	    QPDevCmdParam param[3];
	    QRect r(x, y, w, h);
	    param[0].rect = &r;
	    param[1].ival = xRnd;
	    param[2].ival = yRnd;
	    if(!pdev->cmd(QPaintDevice::PdcDrawRoundRect, this, param) || !pdev->handle())
		return;
	}
	if(txop == TxRotShear) {             // rotate/shear polygon
	    if(w <= 0 || h <= 0)
		fix_neg_rect(&x, &y, &w, &h);
	    w--;
	    h--;
	    int rxx = w*xRnd/200;
	    int ryy = h*yRnd/200;
	    // were there overflows?
	    if(rxx < 0)
		rxx = w/200*xRnd;
	    if(ryy < 0)
		ryy = h/200*yRnd;
	    int rxx2 = 2*rxx;
	    int ryy2 = 2*ryy;
	    QPointArray a[4];
	    a[0].makeArc(x, y, rxx2, ryy2, 1*16*90, 16*90, xmat);
	    a[1].makeArc(x, y+h-ryy2, rxx2, ryy2, 2*16*90, 16*90, xmat);
	    a[2].makeArc(x+w-rxx2, y+h-ryy2, rxx2, ryy2, 3*16*90, 16*90, xmat);
	    a[3].makeArc(x+w-rxx2, y, rxx2, ryy2, 0*16*90, 16*90, xmat);
	    // ### is there a better way to join QPointArrays?
	    QPointArray aa;
	    aa.resize(a[0].size() + a[1].size() + a[2].size() + a[3].size());
	    uint j = 0;
	    for(int k=0; k<4; k++) {
		for(uint i=0; i<a[k].size(); i++) {
		    aa.setPoint(j, a[k].point(i));
		    j++;
		}
	    }
	    drawPolyInternal(aa);
	    return;
	}
	map(x, y, w, h, &x, &y, &w, &h);
    }
    if(w <= 0 || h <= 0) {
	if(w == 0 || h == 0)
	    return;
	fix_neg_rect(&x, &y, &w, &h);
    }

    initPaintDevice();
    if(d->cache.paintreg.isEmpty())
	return;

    Rect rect;
    SetRect(&rect, x+d->offx, y+d->offy, x + w+d->offx, y + h+d->offy);
    if(this->brush().style() == SolidPattern) {
	updateBrush();
	PaintRoundRect(&rect, w*xRnd/100, h*yRnd/100);
    }
    if(cpen.style() != NoPen) {
	updatePen();
	FrameRoundRect(&rect, w*xRnd/100, h*yRnd/100);
    }
}

void QPainter::drawEllipse(int x, int y, int w, int h)
{
    if(!isActive()) {
	return;
    }
    if(testf(ExtDev|VxF|WxF)) {
	if(testf(ExtDev)) {
	    QPDevCmdParam param[1];
	    QRect r(x, y, w, h);
	    param[0].rect = &r;
	    if(!pdev->cmd(QPaintDevice::PdcDrawEllipse, this, param) || !pdev->handle())
		return;
	}
	if(txop == TxRotShear) {             // rotate/shear polygon
	    QPointArray a;
	    a.makeArc(x, y, w, h, 0, 360*16, xmat);
	    drawPolyInternal(a);
	    return;
	}
	map(x, y, w, h, &x, &y, &w, &h);
    }
    if(w <= 0 || h <= 0) {
	if(w == 0 || h == 0)
	    return;
	fix_neg_rect(&x, &y, &w, &h);
    }

    initPaintDevice();
    if(d->cache.paintreg.isEmpty())
	return;

    Rect r;
    SetRect(&r, x+d->offx, y+d->offy, x + w+d->offx, y + h+d->offy);
    if(this->brush().style() != NoBrush) {
	updateBrush();
	if(this->brush().style() == SolidPattern) {
	    PaintOval(&r);
	} else {
	    QPixmap *pm = 0;
	    if(d->brush_style_pix) {
		pm = d->brush_style_pix;
		if(bg_mode == OpaqueMode) {
		    ::RGBColor f;
		    f.red = bg_col.red()*256;
		    f.green = bg_col.green()*256;
		    f.blue = bg_col.blue()*256;
		    RGBForeColor(&f);
		    PaintOval(&r);
		}
	    } else {
		pm = cbrush.data->pixmap;
	    }
	    if(pm && !pm->isNull()) {
		//save the clip
		bool clipon = testf(ClipOn);
		QRegion clip = crgn;

		//create the region
		QRegion newclip(QRect(x, y, w, h), QRegion::Ellipse);
		if(clipon && !clip.isNull())
		    newclip &= clip;
		setClipRegion(newclip);

		//turn off translation flags
		uint save_flags = flags;
		flags = IsActive | ClipOn;

		//draw the brush
                drawTiledPixmap(x, y, w, h, *qt_mac_make_tiled_pixmap(pm, cbrush.color()), x - bro.x(), y - bro.y());

		//restore translation flags
		flags = save_flags;

		//restore the clip
		if(clipon)
		    setClipRegion(clip);
		else
		    setClipping(FALSE);
	    }
	}
    }

    if(cpen.style() != NoPen) {
	updatePen();
	FrameOval(&r);
    }
}

void QPainter::drawArc(int x, int y, int w, int h, int a, int alen)
{
    if(!isActive())
	return;
    if(testf(ExtDev)) {
	QPDevCmdParam param[3];
	QRect r(x, y, w, h);
	param[0].rect = &r;
	param[1].ival = a;
	param[2].ival = alen;
	if(!pdev->cmd(QPaintDevice::PdcDrawArc, this, param) || !pdev->handle())
	    return;
    }
    if(w <= 0 || h <= 0) {
	if(w == 0 || h == 0)
	    return;
	fix_neg_rect(&x, &y, &w, &h);
    }
    QPointArray pa;
    pa.makeArc(x, y, w, h, a, alen); // arc polyline
    drawPolyline(pa);
}

void QPainter::drawPie(int x, int y, int w, int h, int a, int alen)
{
    if(!isActive())
	return;

    if(a > (360*16)) {
	a = a % (360*16);
    } else if(a < 0) {
	a = a % (360*16);
	if(a < 0)
	    a += (360*16);
    }
    if(testf(ExtDev)) {
	QPDevCmdParam param[3];
	QRect r(x, y, w, h);
	param[0].rect = &r;
	param[1].ival = a;
	param[2].ival = alen;
	if(!pdev->cmd(QPaintDevice::PdcDrawPie, this, param) || !pdev->handle())
	    return;
    }
    if((a + alen) == (360*16)) {
	bool extdev = testf(ExtDev);
	if(extdev)
	    clearf(ExtDev);
	drawEllipse(x, y, w, h);
	if(extdev)
	    setf(ExtDev);
	return;
    }
#if 1
    QPointArray pa;
    pa.makeArc(x, y, w, h, a, alen, xmat); // arc polyline
    int n = pa.size();
    int cx, cy;
    xmat.map(x+w/2, y+h/2, &cx, &cy);
    pa.resize(n+2);
    pa.setPoint(n, cx, cy);	// add legs
    pa.setPoint(n+1, pa.at(0));
    drawPolyInternal(pa, TRUE);
#else
    //There is probably a way to do this with FrameArc/PaintArc, but for now it is
    //very different from Qt and not worth it FIXME
    initPaintDevice();
    if(d->cache.paintreg.isEmpty())
	return;
    Rect r;
    SetRect(&r, d->offx + x, d->offy + y, d->offx + x + w, d->offy + y + h);
    PaintArc(&r, -((a * 16) - 30), -(alen * 16));
#endif
}

void QPainter::drawChord(int x, int y, int w, int h, int a, int alen)
{
    if(!isActive())
	return;
    if(testf(ExtDev|VxF|WxF)) {
        if(testf(ExtDev)) {
            QPDevCmdParam param[3];
            QRect r(x, y, w, h);
            param[0].rect = &r;
            param[1].ival = a;
            param[2].ival = alen;
            if(!pdev->cmd(QPaintDevice::PdcDrawChord, this, param) || !pdev->handle())
                return;
        }
    }
    QPointArray pa;
    pa.makeArc(x, y, w-1, h-1, a, alen);
    int n = pa.size();
    pa.resize(n + 1);
    pa.setPoint(n, pa.at(0));
    drawPolyInternal(pa);
}

void QPainter::drawLineSegments(const QPointArray &a, int index, int nlines)
{
    if(nlines < 0)
	nlines = a.size()/2 - index/2;
    if(index + nlines*2 > (int)a.size())
	nlines = (a.size() - index)/2;
    if(!isActive() || nlines < 1 || index < 0)
	return;
    QPointArray pa = a;
    if(testf(ExtDev|VxF|WxF)) {
	if(testf(ExtDev)) {
	    if (2*nlines != (int)pa.size()) {
		pa = QPointArray(nlines*2);
		for(int i=0; i<nlines*2; i++)
		    pa.setPoint(i, a.point(index+i));
		index = 0;
	    }
	    QPDevCmdParam param[1];
	    param[0].ptarr = (QPointArray*)&pa;
	    if(!pdev->cmd(QPaintDevice::PdcDrawLineSegments,this,param) || !pdev->handle())
		return;
	}
	if(txop != TxNone) {
	    pa = xForm(a, index, nlines*2);
	    if(pa.size() != a.size()) {
		index  = 0;
		nlines = pa.size()/2;
	    }
	}
    }

    int  x1, y1, x2, y2;
    uint i = index;

    initPaintDevice();
    if(d->cache.paintreg.isEmpty())
	return;

    updatePen();
    while(nlines--) {
	pa.point(i++, &x1, &y1);
	pa.point(i++, &x2, &y2);
	MoveTo(x1 + d->offx, y1 + d->offy);
	LineTo(x2 + d->offx, y2 + d->offy);
    }
}

void QPainter::drawPolyline(const QPointArray &a, int index, int npoints)
{
    if(npoints < 0)
	npoints = a.size() - index;
    if(index + npoints > (int)a.size())
	npoints = a.size() - index;
    if(!isActive() || npoints < 2 || index < 0)
	return;
    QPointArray pa = a;
    if(testf(ExtDev|VxF|WxF)) {
	if(testf(ExtDev)) {
	    if(npoints != (int)pa.size()) {
		pa = QPointArray(npoints);
		for(int i=0; i<npoints; i++)
		    pa.setPoint(i, a.point(index+i));
		index = 0;
	    }
	    QPDevCmdParam param[1];
	    param[0].ptarr = (QPointArray*)&pa;
	    if(!pdev->cmd(QPaintDevice::PdcDrawPolyline,this,param) || !pdev->handle())
		return;
	}
	if(txop != TxNone) {
	    pa = xForm(pa, index, npoints);
	    if(pa.size() != a.size()) {
		index   = 0;
		npoints = pa.size();
	    }
	}
    }
    int x1, y1, x2, y2, xsave, ysave;
    pa.point(index+npoints-2, &x1, &y1);      // last line segment
    pa.point(index+npoints-1, &x2, &y2);
    xsave = x2; ysave = y2;
    bool plot_pixel = FALSE;
    if(x1 == x2) {                           // vertical
	if(y1 < y2)
	    y2++;
	else
	    y2--;
    } else if(y1 == y2) {                    // horizontal
	if(x1 < x2)
	    x2++;
	else
	    x2--;
    } else {
	plot_pixel = cpen.style() == SolidLine; // plot last pixel
    }
    initPaintDevice();
    if(d->cache.paintreg.isEmpty())
	return;

    updatePen();
    /* We draw 5000 chunks at a time because of limitations in QD */
    for(uint chunk = (uint)index; chunk < (uint)npoints; ) {
	//make a region of it
	PolyHandle poly = OpenPoly();
	MoveTo(pa[chunk].x()+d->offx, pa[chunk].y()+d->offy);
	for(uint last_chunk = chunk + 5000; chunk < last_chunk; chunk++) {
	    if(chunk == (uint)npoints)
		break;
	    LineTo(pa[chunk].x()+d->offx, pa[chunk].y()+d->offy);
	}
	ClosePoly();
	//now draw it
	FramePoly(poly);
	KillPoly(poly);
    }
}

void QPainter::drawConvexPolygon(const QPointArray &pa,
			     int index, int npoints)
{
    // Any efficient way?
    drawPolygon(pa,FALSE,index,npoints);
}

void QPainter::drawPolygon(const QPointArray &a, bool winding,
			    int index, int npoints)
{
    if(npoints < 0)
	npoints = a.size() - index;
    if(index + npoints > (int)a.size())
	npoints = a.size() - index;
    if(!isActive() || npoints < 2 || index < 0)
	return;
    QPointArray pa;
    if(npoints != (int)a.size()) {
	pa = QPointArray(npoints);
	for(int i=0; i<npoints; i++)
	    pa.setPoint(i, a.point(index+i));
	index = 0;
    } else {
	pa = a;
    }
    if(testf(ExtDev|VxF|WxF)) {
	if(testf(ExtDev)) {
	    QPDevCmdParam param[2];
	    param[0].ptarr = (QPointArray*)&pa;
	    param[1].ival = winding;
	    if(!pdev->cmd(QPaintDevice::PdcDrawPolygon, this, param) || !pdev->handle())
		return;
	}
	if(txop != TxNone) {
	    pa = xForm(a, index, npoints);
	    if(pa.size() != a.size()) {
		index   = 0;
		npoints = pa.size();
	    }
	}
    }
    drawPolyInternal(pa,true);
}

void QPainter::drawCubicBezier(const QPointArray &a, int index)
{
    if(!isActive())
	return;
    if(a.size() - index < 4) {
#if defined(QT_CHECK_RANGE)
	qWarning("Qt: QPainter::drawCubicBezier: Cubic Bezier "
		 "needs 4 control points");
#endif
	return;
    }
    QPointArray pa(a);
    if(index != 0 || a.size() > 4) {
        pa = QPointArray(4);
        for(int i=0; i<4; i++)
            pa.setPoint(i, a.point(index+i));
    }
    if(testf(ExtDev)) {
        if(testf(ExtDev)) {
            QPDevCmdParam param[1];
            param[0].ptarr = (QPointArray*)&pa;
            if(!pdev->cmd(QPaintDevice::PdcDrawCubicBezier, this, param) || !pdev->handle())
                return;
        }
    }
    drawPolyline(pa.cubicBezier());
}

void QPainter::drawPixmap(int x, int y, const QPixmap &pixmap, int sx, int sy, int sw, int sh)
{
    if(!isActive() || pixmap.isNull()) {
	return;
    }
    if(sw < 0)
	sw = pixmap.width() - sx;
    if(sh < 0)
	sh = pixmap.height() - sy;

    // Sanity-check clipping
    if(sx < 0) {
	x -= sx;
	sw += sx;
	sx = 0;
    }
    if(sw + sx > pixmap.width())
	sw = pixmap.width() - sx;
    if(sy < 0) {
	y -= sy;
	sh += sy;
	sy = 0;
    }
    if(sh + sy > pixmap.height())
	sh = pixmap.height() - sy;
    if(sw <= 0 || sh <= 0) {
	return;
    }

    if(testf(ExtDev|VxF|WxF)) {
	if(pdev->devType() != QInternal::Picture && txop == TxScale) {
	    // Plain scaling, then unclippedScaledBitBlt is fastest
	    int w, h;
	    map(x, y, sw, sh, &x, &y, &w, &h);
	    initPaintDevice();
	    if(d->cache.paintreg.isEmpty())
		return;
	    updatePen();
	    unclippedScaledBitBlt(pdev, x, y, w, h, &pixmap, sx, sy, sw, sh, (RasterOp)rop,
				   FALSE, FALSE);
	    return;
	} else if(testf(ExtDev) || txop == TxRotShear) {
	    if(sx != 0 || sy != 0 ||
		 sw != pixmap.width() || sh != pixmap.height()) {
		QPixmap tmp(sw, sh, pixmap.depth());
		updatePen();
		unclippedBitBlt(&tmp, 0, 0, &pixmap, sx, sy, sw, sh, CopyROP, TRUE, FALSE);
		if(pixmap.mask()) {
		    QBitmap mask(sw, sh);
		    unclippedBitBlt(&mask, 0, 0, pixmap.mask(), sx, sy, sw, sh,
				     CopyROP, TRUE, FALSE);
		    tmp.setMask(mask);
		}
		drawPixmap(x, y, tmp);
		return;
	    }
	    if(testf(ExtDev)) {
		QPDevCmdParam param[2];
		QRect r(x, y, pixmap.width(), pixmap.height());
		param[0].rect  = &r;
		param[1].pixmap = &pixmap;
		if(!pdev->cmd(QPaintDevice::PdcDrawPixmap,this,param) || !pdev->handle()) {
		    return;
		}
	    }
	    if(txop == TxScale || txop == TxRotShear) {
		QWMatrix mat(m11(), m12(),
			      m21(), m22(),
			      dx(),  dy());
		mat = QPixmap::trueMatrix(mat, sw, sh);
		QPixmap pm = pixmap.xForm(mat);
		if(!pm.data->alphapm && !pm.mask() && txop == TxRotShear) {
		    QBitmap bm_clip(sw, sh, TRUE);
		    bm_clip.fill(color1);
		    pm.setMask(bm_clip.xForm(mat));
		}
		map(x, y, &x, &y);		// compute position of pixmap
		int dx, dy;
		mat.map(0, 0, &dx, &dy);
		initPaintDevice();
		if(d->cache.paintreg.isEmpty())
		    return;
		updatePen();
		unclippedBitBlt(pdev, x-dx, y-dy, &pm, 0, 0, pm.width(),
				 pm.height(), (RasterOp)rop, FALSE, FALSE);
		return;
	    }
	}

	if(txop == TxTranslate)
	    map(x, y, &x, &y);
    }
    initPaintDevice();
    if(d->cache.paintreg.isEmpty())
	return;
    updatePen();
    unclippedBitBlt(pdev, x, y, &pixmap, sx, sy, sw, sh, (RasterOp)rop, FALSE, FALSE);
}

static void drawTile(QPainter *p, int x, int y, int w, int h,
		      const QPixmap &pixmap, int xOffset, int yOffset)
{
    int yPos, xPos, drawH, drawW, yOff, xOff;
    yPos = y;
    yOff = yOffset;
    while(yPos < y + h) {
	drawH = pixmap.height() - yOff;    // Cropping first row
	if(yPos + drawH > y + h)        // Cropping last row
	    drawH = y + h - yPos;
	xPos = x;
	xOff = xOffset;
	while(xPos < x + w) {
	    drawW = pixmap.width() - xOff; // Cropping first column
	    if(xPos + drawW > x + w)    // Cropping last column
		drawW = x + w - xPos;
	    p->drawPixmap(xPos, yPos, pixmap, xOff, yOff, drawW, drawH);
	    xPos += drawW;
	    xOff = 0;
	}
	yPos += drawH;
	yOff = 0;
    }
}

void QPainter::drawTiledPixmap(int x, int y, int w, int h,
				const QPixmap &pixmap, int sx, int sy)
{
    int sw = pixmap.width();
    int sh = pixmap.height();
    if(!sw || !sh)
	return;

    if(sx < 0)
	sx = sw - -sx % sw;
    else
	sx = sx % sw;
    if(sy < 0)
	sy = sh - -sy % sh;
    else
	sy = sy % sh;
    drawTile(this, x, y, w, h, pixmap, sx, sy);
}

void QPainter::drawText(int x, int y, const QString &str, int len, QPainter::TextDirection dir)
{
    drawText(x, y, str, 0, len, dir);
}

void QPainter::drawText(int x, int y, const QString &str, int pos, int len, QPainter::TextDirection dir)
{
    if(!isActive())
	return;
    if(len < 0 || pos + len > (int)str.length())
	len = str.length() - pos;
    if(len == 0 || pos >= (int)str.length())   // empty string
	return;

    if(testf(DirtyFont))
	updateFont();

    if(testf(ExtDev)) {
	QPDevCmdParam param[2];
	QPoint p(x, y);
	QString newstr = str.mid(pos, len);
	param[0].point = &p;
	param[1].str = &newstr;
	if(!pdev->cmd(QPaintDevice::PdcDrawText2,this,param) || !pdev->handle())
	    return;
    }

    initPaintDevice();
    if(d->cache.paintreg.isEmpty())
	return;

    bool simple = str.simpleText();
    // we can't take the complete string here as we would otherwise
    // get quadratic behaviour when drawing long strings in parts.
    // we do however need some chars around the part we paint to get arabic shaping correct.
    // ### maybe possible to remove after cursor restrictions work in QRT
    int start, end;
    if (simple) {
	start = pos;
	end = pos+len;
    } else {
	start = QMAX(0,  pos - 8);
	end = QMIN((int)str.length(), pos + len + 8);
    }

    QConstString cstr(str.unicode() + start, end - start);
    pos -= start;

    QTextEngine engine(cstr.string(), pfont ? pfont->d : cfont.d);
    QTextLayout layout(&engine);

    // this is actually what beginLayout does. Inlined here, so we can
    // avoid the bidi algorithm if we don't need it.
    engine.itemize(simple ? QTextEngine::NoBidi|QTextEngine::SingleLine : QTextEngine::Full|QTextEngine::SingleLine);
    engine.currentItem = 0;
    engine.firstItemInLine = -1;

    if(dir != Auto) {
	int level = (dir == RTL) ? 1 : 0;
	for(int i = engine.items.size(); i >= 0; i--)
	    engine.items[i].analysis.bidiLevel = level;
    }

    if ( !simple ) {
	layout.setBoundary( pos );
	layout.setBoundary( pos + len );
    }
    // small hack to force skipping of unneeded items
    start = 0;
    while(engine.items[start].position < pos)
	++start;
    engine.currentItem = start;
    layout.beginLine(0xfffffff);
    end = start;
    while (!layout.atEnd() && layout.currentItem().from() < pos + len) {
	layout.addCurrentItem();
	end++;
    }
    int ascent = fontMetrics().ascent();
    layout.endLine(0, 0, Qt::AlignLeft, &ascent, 0);
    // do _not_ call endLayout() here, as it would clean up the shaped items and we would do shaping another time
    // for painting.
    for(int i = start; i < end; i++) {
	QScriptItem *si = &engine.items[i];
	QFontEngine *fe = si->fontEngine;
	Q_ASSERT(fe);

	int textFlags = 0;
	if(cfont.d->underline)
	    textFlags |= Qt::Underline;
	if(cfont.d->overline)
	    textFlags |= Qt::Overline;
	if(cfont.d->strikeOut)
	    textFlags |= Qt::StrikeOut;
	fe->draw(this, x + si->x,  y + si->y - ascent, &engine, si, textFlags);
    }
    layout.d = 0; //this is on the stack, QTextLayout doesn't know that!
}

void QPainter::drawTextItem(int x, int y, const QTextItem &ti, int textFlags)
{
    if(testf(ExtDev)) {
        QPDevCmdParam param[2];
        QPoint p(x, y);
        param[0].point = &p;
        param[1].textItem = &ti;
        bool retval = pdev->cmd(QPaintDevice::PdcDrawTextItem, this, param);
        if(!retval || !hd)
            return;
    }

    QTextEngine *engine = ti.engine;
    QScriptItem &si = engine->items[ti.item];
    engine->shape(ti.item);
    QFontEngine *fe = si.fontEngine;

    Q_ASSERT(fe);
    x += si.x;
    y += si.y;

    fe->draw(this, x,  y, engine, &si, textFlags);
}

QPoint QPainter::pos() const
{
    ((QPainter *)this)->initPaintDevice();
    Point pt;
    GetPen(&pt);
    return xFormDev(QPoint(pt.h - d->offx, pt.v - d->offy));
}

/*!
    \internal
*/
void QPainter::initPaintDevice(bool force, QPoint *off, QRegion *rgn) {
    bool remade_clip = FALSE;
    if(pdev->devType() == QInternal::Printer) {
	if(force && pdev->handle()) {
	    remade_clip = TRUE;
#ifndef QMAC_NO_QUARTZ
	    d->cg.off_w = pdev->metric(QPaintDeviceMetrics::PdmWidth);
	    d->cg.off_h = pdev->metric(QPaintDeviceMetrics::PdmHeight);
#endif
	    d->cache.clippedreg = QRegion(0, 0, pdev->metric(QPaintDeviceMetrics::PdmWidth),
					  pdev->metric(QPaintDeviceMetrics::PdmHeight));
	}
    } else if(pdev->devType() == QInternal::Widget) {                    // device is a widget
	QWidget *w = (QWidget*)pdev;
	paintevent_item *pevent = paintevents.current();
	if(pevent && !((*pevent) == pdev))
	    pevent = 0;
	if(!(remade_clip = force)) {
	    if(pevent != d->cache.paintevent)
		remade_clip = TRUE;
	    else if(!w->isVisible())
		remade_clip = d->cache.clip_serial;
	    else
		remade_clip = (d->cache.clip_serial != w->clippedSerial(!d->unclipped));
	}
	if(remade_clip) {
	    //offset painting in widget relative the tld
	    QPoint wp(posInWindow(w));
	    d->offx = wp.x();
	    d->offy = wp.y();
#ifndef QMAC_NO_QUARTZ
	    QWidget *tlw = w->topLevelWidget();
	    d->cg.off_w = tlw->width();
	    d->cg.off_h = tlw->height();
#endif

	    if(!w->isVisible()) {
		d->cache.clippedreg = QRegion(0, 0, 0, 0); //make the clipped reg empty if its not visible!!!
		d->cache.clip_serial = 0;
	    } else {
		d->cache.clippedreg = w->clippedRegion(!d->unclipped);
		d->cache.clip_serial = w->clippedSerial(!d->unclipped);
	    }
	    if(pevent)
		d->cache.clippedreg &= pevent->region();
	    d->cache.paintevent = pevent;
	}
    } else if(pdev->devType() == QInternal::Pixmap) {             // device is a pixmap
	QPixmap *pm = (QPixmap*)pdev;
	if(force) {//clip out my bounding rect
	    remade_clip = TRUE;
#ifndef QMAC_NO_QUARTZ
	    d->cg.off_w = pm->width();
	    d->cg.off_h = pm->height();
#endif
	    d->cache.clippedreg = QRegion(0, 0, pm->width(), pm->height());
	}
    }
    if(remade_clip || d->cache.crgn_dirty) { 	//update clipped region
	remade_clip = TRUE;
	if(!d->cache.clippedreg.isEmpty() && testf(ClipOn)) {
	    d->cache.paintreg = crgn;
	    d->cache.paintreg.translate(d->offx, d->offy);
	    if(!d->cache.clippedreg.isNull())
		d->cache.paintreg &= d->cache.clippedreg;
	} else {
	    d->cache.paintreg = d->cache.clippedreg;
	}

	CGrafPtr ptr;
	if(pdev->devType() == QInternal::Widget)
	    ptr = GetWindowPort((WindowPtr)pdev->handle());
	else
	    ptr = (GWorldPtr)pdev->handle();
	if(d->cache.paintreg.handle()) {
	    QDAddRegionToDirtyRegion(ptr, d->cache.paintreg.handle());
	} else {
	    QRect qr = d->cache.paintreg.boundingRect();
	    Rect mr; SetRect(&mr, qr.x(), qr.y(), qr.right(), qr.bottom());
	    QDAddRectToDirtyRegion(ptr, &mr);
	}
	d->cache.crgn_dirty = FALSE;
    }
    if(remade_clip || qt_mac_current_painter != this) {
	QMacSavedPortInfo::setPaintDevice(pdev);
	QMacSavedPortInfo::setClipRegion(d->cache.paintreg);
	qt_mac_current_painter = this;
    }
    if(off)
	*off = QPoint(d->offx, d->offy);
    if(rgn)
	*rgn = d->cache.paintreg;
}

