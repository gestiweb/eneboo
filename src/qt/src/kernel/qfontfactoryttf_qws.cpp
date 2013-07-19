/****************************************************************************
**
** Implementation of QFontFactory for Truetype class for Embedded Qt
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

#include "qfontfactoryttf_qws.h"

#ifndef QT_NO_FREETYPE

#include "qfontdata_p.h"
#include <string.h>
#include <stdio.h>

#define FLOOR(x)  ((x) & -64)
#define CEIL(x)   (((x)+63) & -64)
#define TRUNC(x)  ((x) >> 6)

#if 1 //new freetype version
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BBOX_H
#else
extern "C" {
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/ftbbox.h>
}
#endif

class QDiskFontFT : public QDiskFontPrivate {
public:
    FT_Face face;
};

class QRenderedFontFT : public QRenderedFont {
public:
    QRenderedFontFT(QDiskFont* f, const QFontDef &d) :
	QRenderedFont(f,d)
    {
	QDiskFontFT *df = (QDiskFontFT*)(f->p);
	myface=df->face;
	selectThisSize();
	// A 1-pixel baseline is excluded in Qt/Windows/X11 fontmetrics
	// (see QFontMetrics::height())
	//
	fascent=CEIL(myface->size->metrics.ascender)/64;
	fdescent=-FLOOR(myface->size->metrics.descender)/64-1;
	fmaxwidth=CEIL(myface->size->metrics.max_advance)/64;
	fleading=CEIL(myface->size->metrics.height)/64
	    - fascent - fdescent - 1;

	// FT has these in font units
	funderlinepos = ptsize/200+1;
	funderlinewidth = ptsize/200+1;
    }

    ~QRenderedFontFT()
    {
    }

    bool unicode(int & i) const
    {
	int ret;

	FT_Face theface=myface;

	ret=FT_Get_Char_Index(theface,i);

	if (ret==0) {
	    return FALSE;
	} else {
	    i=ret;
	    return TRUE;
	}
    }

    bool inFont(glyph_t g) const
    {
	int index = g;
	return unicode(index);
    }

    QGlyph render(glyph_t g)
    {
	selectThisSize();

	int index = g;
	if ( !unicode(index) )
	    index = 0;
	QGlyph result;

	FT_Error err;

	err=FT_Load_Glyph(myface,index,FT_LOAD_NO_BITMAP);
	if(err)
	    qFatal("Load glyph error %x",err);

	int width=0,height=0,pitch=0,size=0;
	FT_Glyph glyph;
	err=FT_Get_Glyph( myface->glyph, &glyph );
	if(err)
	    qFatal("Get glyph error %x",err);

	FT_BBox bbox;
	FT_Glyph_Get_CBox(glyph, ft_glyph_bbox_gridfit, &bbox);

	FT_Vector origin;
	origin.x = -bbox.xMin;
	origin.y = -bbox.yMin;

	if ( FT_IS_SCALABLE(myface) ) {
	    err=FT_Glyph_To_Bitmap(&glyph,
		smooth ? ft_render_mode_normal : ft_render_mode_mono,
		&origin, 1); // destroy original glyph
	    if(err)
		qWarning("Get bitmap error %d",err);
	}

	if ( !err ) {
	    FT_Bitmap bm = ((FT_BitmapGlyph)glyph)->bitmap;
	    pitch = bm.pitch;
	    size=pitch*bm.rows;
	    result.data = new uchar[size]; // XXX memory manage me
	    width=bm.width;
	    height=bm.rows;
	    if ( size ) {
		memcpy( result.data, bm.buffer, size );
	    } else {
		result.data = new uchar[0]; // XXX memory manage me
	    }
	} else {
	    result.data = new uchar[0]; // XXX memory manage me
	}

	result.metrics = new QGlyphMetrics;
	memset((char*)result.metrics, 0, sizeof(QGlyphMetrics));
	result.metrics->bearingx=myface->glyph->metrics.horiBearingX/64;
	result.metrics->advance=myface->glyph->metrics.horiAdvance/64;
	result.metrics->bearingy=myface->glyph->metrics.horiBearingY/64;

	result.metrics->linestep=pitch;
	result.metrics->width=width;
	result.metrics->height=height;

	FT_Done_Glyph( glyph );

	return result;
    }

    FT_Face myface;

private:
    void selectThisSize()
    {
	int psize=(ptsize<<6)/10;

	// Assume 72 dpi for now
	const int dpi=72;
	FT_Error err;
	err=FT_Set_Char_Size(myface, psize,psize,dpi,dpi);
	if (err) {
	    if (FT_IS_SCALABLE(myface) ) {
		qWarning("Set char size error %x for size %d",err,ptsize);
	    } else {
		int best=-1;
		int bdh=99;
		for (int i=0; i<myface->num_fixed_sizes; i++) {
		    FT_Bitmap_Size& sz=myface->available_sizes[i];
		    int dh = sz.height - ptsize*dpi/72/10;
		    dh = QABS(dh);
		    if ( dh < bdh ) {
			bdh=dh;
			best=i;
		    }
		}
		if ( best >= 0 )
		    err=FT_Set_Pixel_Sizes(myface,
			myface->available_sizes[best].width,
			myface->available_sizes[best].height);
		if ( err )
		    qWarning("Set char size error %x for size %d",err,ptsize);
	    }
	}
    }
};

QFontFactoryFT::QFontFactoryFT()
{
    FT_Error err;
    err=FT_Init_FreeType(&library);
    if(err) {
	qFatal("Couldn't initialise Freetype library");
    }
}

QFontFactoryFT::~QFontFactoryFT()
{
    FT_Error err;
    err=FT_Done_FreeType(library);
    if(err) {
	qFatal("Couldn't uninitialise Freetype library");
    }
}

QString QFontFactoryFT::name()
{
    return "FT";
}

QRenderedFont * QFontFactoryFT::get(const QFontDef & f,QDiskFont * f2)
{
    return new QRenderedFontFT(f2, f);
}

void QFontFactoryFT::load(QDiskFont * qdf) const
{
    if(qdf->loaded)
	return;
    QDiskFontFT *f = new QDiskFontFT;
    qdf->p=f;
    FT_Error err;
    err=FT_New_Face(library,qdf->file.ascii(),0,&(f->face));
    if(err) {
	qFatal("Error %d opening face",err);
    }
    qdf->loaded=TRUE;
}

void QFontFactoryFT::unload(QDiskFont * qdf)
{
    QDiskFontFT *f = (QDiskFontFT *) qdf->p;
    delete f;
}

#endif // QT_NO_FREETYPE
