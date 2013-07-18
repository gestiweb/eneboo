/****************************************************************************
**
** ???
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
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

#include "qfontengine_p.h"
#include "qmemorymanager_qws.h"
#include <private/qunicodetables_p.h>
#include <qpainter.h>
#include <qgfxraster_qws.h>
#include <private/qunicodetables_p.h>
#include <qbitmap.h>

QFontDef qt_qws_get_real_fontdef( /*QMemoryManager::FontID*/ void* id ); //in qmemorymanager_qws.cpp


/*QMemoryManager::FontID*/ void *QFontEngine::handle() const
{
    return id;
}

QFontEngine::QFontEngine( const QFontDef& d, const QPaintDevice *pd )
{
    id = memorymanager->refFont(d);
    fontDef = qt_qws_get_real_fontdef(id);
    scale = pd ? (pd->resolution()<<8)/75 : 1<<8; 
}

QFontEngine::~QFontEngine()
{
    memorymanager->derefFont(id);
}


/* returns 0 as glyph index for non existant glyphs */
QFontEngine::Error QFontEngine::stringToCMap( const QChar *str, int len, glyph_t *glyphs, advance_t *advances, int *nglyphs, bool mirrored ) const
{
    if(*nglyphs < len) {
	*nglyphs = len;
	return OutOfMemory;
    }
    if ( mirrored ) {
	for(int i = 0; i < len; i++ )
	    glyphs[i] = ::mirroredChar(str[i]).unicode();
    } else {
	for(int i = 0; i < len; i++ )
	    glyphs[i] = str[i].unicode();
    }
    *nglyphs = len;
    if(advances) {
	for(int i = 0; i < len; i++)
	    if ( ::category(str[i]) == QChar::Mark_NonSpacing )
		advances[i] = 0;
	    else
		advances[i] = (memorymanager->lockGlyphMetrics(handle(), str[i].unicode() )->advance*scale)>>8;
    }
    return NoError;
}

void QFontEngine::draw( QPainter *p, int x, int y, const QTextEngine *engine, const QScriptItem *si, int textFlags )
{
#ifndef QT_NO_TRANSFORMATIONS
    if ( p->txop >= QPainter::TxScale ) {
	int aw = si->width;
	int ah = si->ascent + si->descent + 1;
	int tx = 0;
	int ty = 0;
	if ( aw == 0 || ah == 0 )
	    return;
	QWMatrix mat1 = p->xmat;
#ifndef QT_NO_PIXMAP_TRANSFORMATION
	QWMatrix mat2 = QPixmap::trueMatrix( mat1, aw, ah );
#endif
	QPixmap *tpm = 0;
	QBitmap *wx_bm = 0;
	if ( memorymanager->fontSmooth(handle()) &&
	     QPaintDevice::qwsDisplay()->supportsDepth(32) )
	{
	    QPixmap pm(aw, ah, 32);
	    QPainter paint(&pm);
	    paint.fillRect(pm.rect(),Qt::black);
	    paint.setPen(QPen(Qt::white));
	    draw( &paint, 0, si->ascent, engine, si, textFlags );
	    paint.end();
	    // Now we have an image with r,g,b gray scale set.
	    // Put this in alpha channel and set pixmap to pen color.
	    QRgb bg = p->cpen.color().rgb() & 0x00FFFFFF;
	    for ( int y = 0; y < ah; y++ ) {
		uint *p = (uint *)pm.scanLine(y);
		for ( int x = 0; x < aw; x++ ) {
		    int a = *p & 0xFF;
		    *p = bg | (a << 24);
		    p++;
		}
	    }
#ifndef QT_NO_PIXMAP_TRANSFORMATION
	    tpm = new QPixmap( pm.xForm( mat2 ) );
#else
	    tpm = new QPixmap( pm );
#endif
	    if ( tpm->isNull() ) {
		delete tpm;
		return;
	    }
	} else {
#if 0
	    bm_key = gen_text_bitmap_key( mat2, dfont, string, len );
	    wx_bm = get_text_bitmap( bm_key );
	    create_new_bm = wx_bm == 0;
	    if ( create_new_bm && !empty )
#endif
	    { // no such cached bitmap
		QBitmap bm( aw, ah, TRUE );	// create bitmap
		QPainter paint;
		paint.begin( &bm );		// draw text in bitmap
		paint.setPen( p->color1 );
		draw( &paint, 0, si->ascent, engine, si, textFlags );
		paint.end();
#ifndef QT_NO_PIXMAP_TRANSFORMATION
		wx_bm = new QBitmap( bm.xForm(mat2) ); // transform bitmap
#else
		wx_bm = new QBitmap( bm );
#endif
		if ( wx_bm->isNull() ) {
		    delete wx_bm;		// nothing to draw
		    return;
		}
	    }
	}
	double fx=x, fy=y - si->ascent, nfx, nfy;
	mat1.map( fx,fy, &nfx,&nfy );
	double tfx=tx, tfy=ty, dx, dy;
#ifndef QT_NO_PIXMAP_TRANSFORMATION
	mat2.map( tfx, tfy, &dx, &dy );     // compute position of bitmap
#endif
	x = qRound(nfx-dx);
	y = qRound(nfy-dy);

	if ( memorymanager->fontSmooth(handle()) &&
	     QPaintDevice::qwsDisplay()->supportsDepth(32) ) {
	    p->gfx->setSource( tpm );
	    p->gfx->setAlphaType(QGfx::InlineAlpha);
	    p->gfx->blt(x, y, tpm->width(),tpm->height(), 0, 0);
	    delete tpm;
	    return;
	} else {
	    p->gfx->setSource(wx_bm);
	    p->gfx->setAlphaType(QGfx::LittleEndianMask);
	    p->gfx->setAlphaSource(wx_bm->scanLine(0), wx_bm->bytesPerLine());
	    p->gfx->blt(x, y, wx_bm->width(),wx_bm->height(), 0, 0);

#if 0
	    if ( create_new_bm )
		ins_text_bitmap( bm_key, wx_bm );
#else
	    delete wx_bm;
#endif
	}
	return;
    }
#endif

#ifndef QT_NO_TRANSFORMATIONS
    if ( p->txop == QPainter::TxTranslate )
#endif
	p->map( x, y, &x, &y );

    if ( textFlags ) {
	int lw = lineThickness();
	p->gfx->setBrush( p->cpen.color() );
	if ( textFlags & Qt::Underline )
	    p->gfx->fillRect( x, y+underlinePosition(), si->width, lw );
	if ( textFlags & Qt::StrikeOut )
	    p->gfx->fillRect( x, y-ascent()/3, si->width, lw );
	if ( textFlags & Qt::Overline )
	    p->gfx->fillRect( x, y-ascent()-1, si->width, lw );
	p->gfx->setBrush( p->cbrush );
    }

    if ( si->isSpace )
	return;

    glyph_t *glyphs = engine->glyphs( si );
    advance_t *advances = engine->advances( si );
    qoffset_t *offsets = engine->offsets( si );

    struct Pos {
	int x;
	int y;
    };
    Pos _positions[64];
    Pos *positions = _positions;
    if ( si->num_glyphs > 64 )
	positions = new Pos[si->num_glyphs];

    if ( si->analysis.bidiLevel % 2 ) {
	int i = si->num_glyphs;
	while( i-- ) {
	    x += advances[i];
	    glyph_metrics_t gi = boundingBox( glyphs[i] );
	    positions[i].x = x-offsets[i].x-gi.xoff;
	    positions[i].y = y+offsets[i].y-gi.yoff;
	}
    } else {
	int i = 0;
	while( i < si->num_glyphs ) {
	    positions[i].x = x+offsets[i].x;
	    positions[i].y = y+offsets[i].y;
	    x += advances[i];
	    i++;
	}
    }
    QConstString cstr( (QChar *)glyphs, si->num_glyphs );
    p->internalGfx()->drawGlyphs(handle(), glyphs, (QPoint *)positions, si->num_glyphs);

    if ( positions != _positions )
	delete [] positions;
}

glyph_metrics_t QFontEngine::boundingBox( const glyph_t *glyphs,
					  const advance_t *advances, const qoffset_t *offsets, int numGlyphs )
{
    Q_UNUSED( glyphs );
    Q_UNUSED( offsets );
    Q_UNUSED( advances );

    if ( numGlyphs == 0 )
	return glyph_metrics_t();

    int w = 0;
    const advance_t *end = advances + numGlyphs;
    while( end > advances )
	w += *(--end);
    w = (w*scale)>>8;
    return glyph_metrics_t(0, -ascent(), w, ascent()+descent()+1, w, 0 );
}

glyph_metrics_t QFontEngine::boundingBox( glyph_t glyph )
{
    QGlyphMetrics *metrics = memorymanager->lockGlyphMetrics( handle(), glyph);
    return glyph_metrics_t( (metrics->bearingx*scale)>>8, (metrics->bearingy*scale)>>8, (metrics->width*scale)>>8, (metrics->height*scale)>>8, (metrics->advance*scale)>>8, 0 );
}

bool QFontEngine::canRender( const QChar *string,  int len )
{
    bool allExist = TRUE;
    while ( len-- )
	if ( !memorymanager->inFont( handle(), string[len].unicode() ) ) {
	    allExist = FALSE;
	    break;
	}

    return allExist;
}


int QFontEngine::ascent() const
{
    return (memorymanager->fontAscent(handle())*scale)>>8;
}

int QFontEngine::descent() const
{
    return (memorymanager->fontDescent(handle())*scale)>>8;
}

int QFontEngine::leading() const
{
    return (memorymanager->fontLeading(handle())*scale)>>8;
}

int QFontEngine::maxCharWidth() const
{
    return (memorymanager->fontMaxWidth(handle())*scale)>>8;
}

int QFontEngine::minLeftBearing() const
{
    return (memorymanager->fontMinLeftBearing(handle())*scale)>>8;
}

int QFontEngine::minRightBearing() const
{
    return (memorymanager->fontMinRightBearing(handle())*scale)>>8;
}

int QFontEngine::underlinePosition() const
{
    return memorymanager->fontUnderlinePos(handle());
}

int QFontEngine::lineThickness() const
{
    return memorymanager->fontLineWidth(handle());
}

