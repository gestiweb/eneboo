/****************************************************************************
** $Id: qt/qfontengine_mac.cpp   3.3.8   edited Jan 11 14:46 $
**
** ???
**
** Copyright (C) 2005-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Macintosh may use this file in accordance with the Qt Commercial
** License Agreement provided with the Software.
**
** This file is not available for use under any other license without
** express written permission from the copyright holder.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qfontengine_p.h"
#include <qglobal.h>
#include "qpixmapcache.h"
#include "qbitmap.h"
#include "qapplication_p.h"

//antialiasing
#ifndef Q_WS_MACX
# define QMAC_FONT_NO_ANTIALIAS
#endif
#ifndef QMAC_FONT_NO_ANTIALIAS
# include "qpixmap.h"
# include "qpaintdevicemetrics.h"
# include <ApplicationServices/ApplicationServices.h>
#endif

//Externals
QCString p2qstring(const unsigned char *c); //qglobal.cpp
unsigned char * p_str(const QString &); //qglobal.cpp
void unclippedBitBlt(QPaintDevice *dst, int dx, int dy,
		     const QPaintDevice *src, int sx, int sy, int sw, int sh,
		     Qt::RasterOp rop, bool imask, bool set_fore_colour); //qpaintdevice_mac.cpp

//Generic engine
QFontEngine::~QFontEngine()
{
}

int QFontEngine::lineThickness() const
{
  // ad hoc algorithm
  int score = fontDef.pixelSize * fontDef.weight;
  int lth = score / 700;

  // looks better with thicker line for small pointsizes
  if(lth < 2 && score >= 1050)
      lth = 2;
  else if(lth == 0)
      lth = 1;
  return lth;
}

int QFontEngine::underlinePosition() const
{
  int pos = ((lineThickness() * 2) + 3) / 6;
  return pos ? pos : 1;
}

//Mac (ATSUI) engine
QFontEngineMac::QFontEngineMac() : QFontEngine(), info(NULL), fmfam(0), internal_fi(NULL),
                                   mTextLayout(0)
{
    memset(widthCache, 0, sizeof(widthCache));
}

QFontEngineMac::~QFontEngineMac()
{
    if (mTextLayout)
        ATSUDisposeTextLayout(mTextLayout);
    delete internal_fi;
    if(info)
        free(info);
}

QFontEngine::Error
QFontEngineMac::stringToCMap(const QChar *str, int len, glyph_t *glyphs, advance_t *advances, int *nglyphs,
			     bool /*mirrored*/) const
{
    if(*nglyphs < len) {
	*nglyphs = len;
	return OutOfMemory;
    }
     memcpy(glyphs, str, len*sizeof(QChar));
    *nglyphs = len;
    if(advances) {
	for(int i = 0; i < len; i++) {
	    if(str[i].unicode() < widthCacheSize && widthCache[str[i].unicode()])
		advances[i] = widthCache[str[i].unicode()];
	    else
		advances[i] = doTextTask(str+i, 0, 1, 1, WIDTH);
	}
    }
    return NoError;
}

void
QFontEngineMac::draw(QPainter *p, int x, int y, const QTextEngine *engine,
		     const QScriptItem *si, int textFlags)
{
    if(p->txop >= QPainter::TxScale) {
	int aw = si->width, ah = si->ascent + si->descent + 1;
	if(aw == 0 || ah == 0)
	    return;
	QWMatrix mat1 = p->xmat, mat2 = QPixmap::trueMatrix(mat1, aw, ah);
	QPixmap pm(aw, ah);
        if(p->backgroundMode() == QPainter::OpaqueMode) {
            QPainter paint;

            //fill the foreground
            paint.begin(&pm);
            paint.fillRect(0, 0, pm.width(), pm.height(), p->backgroundColor());
	    paint.setPen(p->pen().color());
	    draw(&paint, 0, si->ascent, engine, si, textFlags);
            paint.end();

            // create mask
	    QBitmap bm(pm.width(), pm.height());
	    paint.begin(&bm);
            paint.fillRect(0, 0, pm.width(), pm.height(), Qt::color1);
	    paint.end();
            pm.setMask(bm);
        } else {
	    QPainter paint;

            //fill the foreground
            paint.begin(&pm);
	    paint.fillRect(0, 0, pm.width(), pm.height(), p->pen().color());
            paint.end();

            // create mask
	    QBitmap bm(pm.width(), pm.height(), TRUE);
	    paint.begin(&bm);
	    paint.setPen(Qt::color1);
	    draw(&paint, 0, si->ascent, engine, si, textFlags);
	    paint.end();
            pm.setMask(bm);
	}
        pm = pm.xForm(mat2);
        if(pm.isNull())
            return;

	double nfx, nfy;
	mat1.map(x, y - si->ascent, &nfx, &nfy);
	double dx, dy;
	mat2.map(0, 0, &dx, &dy);     // compute position of bitmap
	unclippedBitBlt(p->device(), qRound(nfx-dx), qRound(nfy-dy), &pm, 0, 0, -1, -1, Qt::CopyROP, FALSE, FALSE );
	return;
    } else if(p->txop == QPainter::TxTranslate) {
	p->map(x, y, &x, &y);
    }

    QPoint off;
    QRegion rgn;
    p->initPaintDevice(FALSE, &off, &rgn);
    if(rgn.isEmpty())
	return;
    x += off.x();
    y += off.y();

    uchar task = DRAW;
    if(textFlags != 0)
	task |= WIDTH; //I need the width for these..

    glyph_t *glyphs = engine->glyphs(si);
    advance_t *advances = engine->advances(si);
    qoffset_t *offsets = engine->offsets(si);

    p->updateBrush();
    if(p->backgroundMode() == Qt::OpaqueMode) {
	glyph_metrics_t br = boundingBox(glyphs, advances, offsets, si->num_glyphs);
	Rect r;
	r.left = x + br.x;
	r.top = y + br.y;
	r.right = r.left + br.width;
	r.bottom = r.top + br.height;
	::RGBColor f;
	QColor qf = p->backgroundColor();
	f.red = qf.red()*256;
	f.green = qf.green()*256;
	f.blue = qf.blue()*256;
	RGBForeColor(&f);
	PaintRect(&r);
    }
    p->updatePen();
    if(p->testf(QPainter::DirtyFont))
	p->updateFont();

#if 0
    short textmode;
    switch(p->rasterOp()) {
    default:
    case Qt::CopyROP:   textmode = srcCopy; break;
    case Qt::OrROP:     textmode = notSrcBic; break;
    case Qt::XorROP:    textmode = srcXor; break;
    case Qt::NotAndROP: textmode = srcBic; break;
    case Qt::NotCopyROP:textmode = notSrcCopy; break;
    case Qt::NotOrROP:  textmode = notSrcOr; break;
    case Qt::NotXorROP: textmode = notSrcXor; break;
    case Qt::AndROP:    textmode = notSrcBic; break;
/*
  case NotROP:      dst = NOT dst
  case ClearROP:    dst = 0
  case SetROP:      dst = 1
  case NopROP:      dst = dst
  case AndNotROP:   dst = src AND (NOT dst)
  case OrNotROP:    dst = src OR (NOT dst)
  case NandROP:     dst = NOT (src AND dst)
  case NorROP:      dst = NOT (src OR dst)
*/
    }
    TextMode(textmode);
#endif

    int w = 0;
    if(si->analysis.bidiLevel % 2 ) {
	offsets += si->num_glyphs;
	advances += si->num_glyphs;
	glyphs += si->num_glyphs;
	for(int i = 0; i < si->num_glyphs; i++) {
	    glyphs--;
	    offsets--;
	    advances--;
	    MoveTo(x, y);
	    w += doTextTask((QChar*)glyphs, 0, 1, 1, task, x, y, p->device(), &rgn);
	    x += *advances;
	}
    } else {
	MoveTo(x, y);
	w = doTextTask((QChar*)glyphs, 0, si->num_glyphs, si->num_glyphs, task, x, y, p->device(), &rgn);
    }
    if(w && textFlags != 0) {
	int lw = lineThickness();
	if(textFlags & Qt::Underline) {
	    int spos = underlinePosition();
	    Rect r;
	    int start_x = x, end_x = x + w;
	    if(si->analysis.bidiLevel % 2) {
		start_x = x - w;
		end_x = x;
	    }
	    SetRect(&r, start_x, y+spos, end_x, y+spos+lw);
	    PaintRect(&r);
	}
	if(textFlags & Qt::Overline) {
	    int spos = ascent() + 1;
	    Rect r;
	    int start_x = x, end_x = x + w;
	    if(si->analysis.bidiLevel % 2) {
		start_x = x - w;
		end_x = x;
	    }
	    SetRect(&r, start_x, (y - spos) - (lw / 2), end_x, (y - spos) + (lw / 2));
	    if(!(r.bottom - r.top))
		r.bottom++;
	    PaintRect(&r);
	}
	if(textFlags & Qt::StrikeOut) {
	    int spos = ascent() / 3;
	    Rect r;
	    int start_x = x, end_x = x + w;
	    if(si->analysis.bidiLevel % 2) {
		start_x = x - w;
		end_x = x;
	    }
	    SetRect(&r, start_x, (y - spos) - (lw / 2), end_x, (y - spos) + (lw / 2));
	    if(!(r.bottom - r.top))
		r.bottom++;
	    PaintRect(&r);
	}
    }
}

glyph_metrics_t
QFontEngineMac::boundingBox(const glyph_t *, const advance_t *advances, const qoffset_t *, int numGlyphs)
{
    int w = 0;
    const advance_t *end = advances + numGlyphs;
    while(end > advances)
	w += *(--end);
    return glyph_metrics_t(0, -(ascent()), w, ascent()+descent()+1, w, 0);
}

glyph_metrics_t
QFontEngineMac::boundingBox(glyph_t glyph)
{
    int w = doTextTask((QChar*)&glyph, 0, 1, 1, WIDTH);
    return glyph_metrics_t(0, -(ascent()), w, ascent()+descent()+1, w, 0 );
}

bool
QFontEngineMac::canRender(const QChar *string,  int len)
{
    return doTextTask(string, 0, len, len, EXISTS);
}

void
QFontEngineMac::calculateCost()
{
    // ### don't know how to get the number of glyphs from the font,
    // ### so default to 1024
    cache_cost = (ascent() + descent() + 1) * maxCharWidth() * 1024;
}

int
QFontEngineMac::doTextTask(const QChar *s, int pos, int use_len, int len, uchar task, int, int y,
			   QPaintDevice *dev, const QRegion *rgn) const
{
#if defined(QMAC_FONT_NO_ANTIALIAS)
    Q_UNUSED(dev);
    Q_UNUSED(rgn);
#endif

    int ret = 0;
    QMacSetFontInfo fi(this, dev);
    QMacFontInfo::QATSUStyle *st = fi.atsuStyle();
    if(!st)
	return 0;

    if(task & DRAW) {
	RGBColor fcolor;
	GetForeColor(&fcolor);
	if(st->rgb.red != fcolor.red || st->rgb.green != fcolor.green ||
	   st->rgb.blue != fcolor.blue) {
	    st->rgb = fcolor;
	    const ATSUAttributeTag tag = kATSUColorTag;
	    ByteCount size = sizeof(fcolor);
            RGBColor cg_color = fcolor;
	    ATSUAttributeValuePtr value = &cg_color;
	    if(OSStatus e = ATSUSetAttributes(st->style, 1, &tag, &size, &value)) {
		qDebug("Qt: internal: %ld: This shouldn't happen %s:%d", e, __FILE__, __LINE__);
		return 0;
	    }
	}
    }
    if((task & WIDTH)) {
 	bool use_cached_width = TRUE;
 	for(int i = 0; i < use_len; i++) {
 	    if(s[i].unicode() >= widthCacheSize || !widthCache[s[i].unicode()]) {
 		use_cached_width = FALSE;
 		break;
 	    }
 	    ret += widthCache[s[i].unicode()];
 	}
 	if(use_cached_width && task == WIDTH)
	    return ret;
    }

    Q_UNUSED(len);

    OSStatus e;
    if (!mTextLayout) {
        e = ATSUCreateTextLayout(&mTextLayout);
        if (e != noErr) {
            qDebug("Qt: internal: %ld: Unexpected condition reached %s:%d", e, __FILE__, __LINE__);
            return 0;
        }
    }
    const UniCharCount count = use_len;
    e = ATSUSetTextPointerLocation(mTextLayout, (UniChar *)(s) + pos, 0, count, use_len);
    if (e != noErr) {
        qDebug("Qt: internal: %ld: Error ATSUSetTextPointerLocation %s: %d", e, __FILE__, __LINE__);
        return 0;
    }
    e = ATSUSetRunStyle(mTextLayout, st->style, 0, count);
    if (e != noErr) {
        qDebug("Qt: internal: %ld: Error ATSUSetRunStyle %s: %d", e, __FILE__, __LINE__);
        return 0;
    }

    const int arr_guess = 5;
    int arr = 0;
    ATSUAttributeTag tags[arr_guess];
    ByteCount valueSizes[arr_guess];
    ATSUAttributeValuePtr values[arr_guess];
    tags[arr] = kATSULineLayoutOptionsTag;
    ATSLineLayoutOptions layopts = kATSLineHasNoOpticalAlignment | kATSLineIgnoreFontLeading | kATSLineFractDisable;
    if(fontDef.styleStrategy & QFont::NoAntialias)
	layopts |= kATSLineNoAntiAliasing;

#if QT_MACOSX_VERSION >= 0x1020
    if(qMacVersion() == Qt::MV_10_DOT_1)
	layopts |= kATSLineIsDisplayOnly;
    else
	layopts |= kATSLineDisableAutoAdjustDisplayPos | kATSLineDisableAllLayoutOperations |
		   kATSLineUseDeviceMetrics;
#else
    layopts |= kATSLineIsDisplayOnly;
#endif
    valueSizes[arr] = sizeof(layopts);
    values[arr] = &layopts;
    arr++;
#if !defined(QMAC_FONT_NO_ANTIALIAS)
    tags[arr] = kATSUCGContextTag; //cgcontext
    CGrafPtr port = NULL;
    CGContextRef ctx = NULL;
    if(dev) {
	if(dev->devType() == QInternal::Widget)
	    port = GetWindowPort((WindowPtr)dev->handle());
	else
	    port = (CGrafPtr)dev->handle();
    } else {
	static QPixmap *p = NULL;
	if(!p)
	    p = new QPixmap(1, 1, 32);
	port = (CGrafPtr)p->handle();
    }
    RgnHandle rgnh = NULL;
    if(rgn && !rgn->isNull() && !rgn->isEmpty())
	rgnh = rgn->handle(TRUE);
    if(OSStatus err = QDBeginCGContext(port, &ctx)) {
	qDebug("Qt: internal: WH0A, QDBeginCGContext(%ld) failed. %s:%d", err, __FILE__, __LINE__);
	return 0;
    }
    Rect clipr;
    GetPortBounds(port, &clipr);
    if(rgnh)
	ClipCGContextToRegion(ctx, &clipr, rgnh);
    valueSizes[arr] = sizeof(ctx);
    values[arr] = &ctx;
    arr++;
#endif
    if(arr > arr_guess) //this won't really happen, just so I will not miss the case
	qDebug("Qt: internal: %d: WH0A, arr_guess underflow %d", __LINE__, arr);
    if (OSStatus e = ATSUSetLayoutControls(mTextLayout, arr, tags, valueSizes, values)) {
	qDebug("Qt: internal: %ld: Unexpected condition reached %s:%d", e, __FILE__, __LINE__);
#if !defined(QMAC_FONT_NO_ANTIALIAS)
	QDEndCGContext(port, &ctx);
#endif
	return 0;
    }

    ATSUSetTransientFontMatching(mTextLayout, true);
    //do required task now
    if(task & EXISTS) {
	if(task != EXISTS)
	    qWarning("Qt: EXISTS must appear by itself!");
	ATSUFontID fid;
	UniCharArrayOffset off;
	UniCharCount off_len;
	if(ATSUMatchFontsToText(mTextLayout, kATSUFromTextBeginning, kATSUToTextEnd, &fid, &off, &off_len) != kATSUFontsNotMatched)
	    ret = 1;
    } else if((task & WIDTH) && !ret) {
	ATSUTextMeasurement left, right, bottom, top;
#if QT_MACOSX_VERSION >= 0x1020
	if(qMacVersion() >= Qt::MV_10_DOT_2)
	    ATSUGetUnjustifiedBounds(mTextLayout, kATSUFromTextBeginning, kATSUToTextEnd,
				     &left, &right, &bottom, &top);
	else
#endif
	    ATSUMeasureText(mTextLayout, kATSUFromTextBeginning, kATSUToTextEnd,
			    &left, &right, &bottom, &top);
	ret = FixRound(right-left);
	if(use_len == 1 && s->unicode() < widthCacheSize && ret > 0 && ret < 0x100)
 	    widthCache[s->unicode()] = ret;
    }
    if(task & DRAW) {
	ATSUDrawText(mTextLayout, kATSUFromTextBeginning, kATSUToTextEnd,
#if !defined(QMAC_FONT_NO_ANTIALIAS)
		     kATSUUseGrafPortPenLoc, FixRatio((clipr.bottom-clipr.top)-y, 1)
#else
		     kATSUUseGrafPortPenLoc, kATSUUseGrafPortPenLoc
#endif
	    );
    }
#if !defined(QMAC_FONT_NO_ANTIALIAS)
    QDEndCGContext(port, &ctx);
#endif
    return ret;
}



