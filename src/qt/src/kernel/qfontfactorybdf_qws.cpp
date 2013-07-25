/****************************************************************************
**
** Definition of QFontFactory for BDF fonts for QT/Embedded
**
** Created : 000427
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
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

#include "qfontfactorybdf_qws.h"

#ifndef QT_NO_BDF

#include "qfontdata_p.h"
#include "qtextcodec.h"
#include "qtextstream.h"
#include "qfile.h"
#include "qrect.h"
#include "qstringlist.h"
#include <string.h>
#include <stdio.h>
#include <private/qtextengine_p.h>

QString QFontFactoryBDF::name()
{
    return "BDF";
}

#ifndef QT_NO_TEXTCODEC
static
QTextCodec* calc_mapper(const QString& charset_registry, const QString& charset_encoding)
{
    if ( !charset_registry.isNull() ) {
	QString e = charset_registry+"-"+charset_encoding;
	if ( e != "10646-1" ) // don't map Unicode, we do it faster directly.
	    return QTextCodec::codecForName(e);
    }
    return 0;
}
#endif

class QRenderedFontBDF : public QRenderedFont {
    QDiskFont* df;
    int ptsize;
    QGlyph *glyph;
    glyph_t default_glyph;

public:

    QRenderedFontBDF(QDiskFont* d, const QFontDef &fd) :
	QRenderedFont(d,fd)
    {
	df = d;

	glyph = 0;
	fleading = 0;
	fascent = -1;
	fdescent = -1;
	QFile in(df->file);
	in.open(IO_ReadOnly);
	QTextStream s(&in);
	QString charset_registry, charset_encoding;
	int advance=0;
	bool bitmap_mode = FALSE;
	uchar* data=0;
	int datasize = 0;
	int glyph_index=0;
	int num_glyphs=0;
	QRect bbox;
#ifndef QT_NO_TEXTCODEC
	QTextCodec* mapper=0;
#endif
	int default_char = -1;
	default_glyph = 0;

	do {
	    QString line = s.readLine();
	    if ( bitmap_mode ) {
		if ( line == "ENDCHAR" ) {
		    // Crop character (some BDF files are poor)
		    QGlyph* g = glyph+glyph_index;
		    uchar* t;
		    int linestep = g->metrics->linestep;
		    int h = g->metrics->height, j;
		    int croptop=0,cropbot=0;
		    t = g->data;
		    for (j=0; j<h; j++) {
			int i;
			for (i=0; i<linestep; i++)
			    if ( *t++ )
				goto donetop;
			if ( i==linestep )
			    croptop++;
		    }
		    donetop:
		    t = g->data+h*linestep;
		    for (j=h-1; j>croptop; j--) {
			int i;
			for (i=0; i<linestep; i++)
			    if ( *--t )
				goto donebot;
			if ( i==linestep )
			    cropbot++;
		    }
		    donebot:
		    if ( croptop || cropbot ) {
			g->metrics->height -= croptop+cropbot;
			g->metrics->bearingy -= croptop;
			int datasize = linestep*g->metrics->height;
			uchar* data = new uchar[datasize];
			memcpy(data,g->data+linestep*croptop,datasize);
			delete [] g->data;
			g->data = data;
		    }
		    bitmap_mode = FALSE;
		    glyph_index++;
		} else {
		    for (int i=0; i<(int)line.length(); i+=2)
			if ( datasize ) {
			    --datasize;
			    *data++ = line.mid(i,2).toInt(0,16);
			}
		}
	    } else {
		QStringList token = QStringList::split(QChar(' '),line);
		QString tag = token[0];
		if ( tag == "BITMAP" ) {
		    if ( glyph_index >= 0 ) {
			int linestep = (bbox.width()+7)/8;
			datasize=linestep*bbox.height();
			data = new uchar[datasize];
			QGlyph* g = glyph+glyph_index;
			g->data = data;
			g->metrics = new QGlyphMetrics;
			memset((char*)g->metrics, 0, sizeof(QGlyphMetrics));
			g->metrics->advance=advance;
			g->metrics->bearingx=bbox.x();
			g->metrics->bearingy=bbox.y()+bbox.height();
			g->metrics->linestep=linestep;
			g->metrics->width=bbox.width();
			g->metrics->height=bbox.height();
			bitmap_mode = TRUE;
		    }
		} else if ( tag == "ENCODING" ) {
		    int encoding = token[1].toInt();
		    if ( encoding < 0 ) {
			glyph_index = -1;
		    } else {
#ifndef QT_NO_TEXTCODEC
			if ( mapper ) {
			    QCString c;
			    int e = encoding;
			    while (e) {
				c += char(e&0xff);
				e >>= 8;
			    }
			    glyph_index = mapper->toUnicode(c)[0].unicode();
			} else
#endif
			{
			    // No mapping. Assume Unicode/Latin1/ASCII7
			    glyph_index = encoding;
			}
			if ( encoding == default_char || default_char == -1 )
			    default_glyph = glyph_index;
		    }
		} else if ( tag == "DWIDTH" ) {
		    advance = token[1].toInt();
		} else if ( tag == "BBX" ) {
		    bbox = QRect(token[3].toInt(),token[4].toInt(),
				 token[1].toInt(),token[2].toInt());
		} else if ( tag == "CHARSET_REGISTRY" ) {
		    charset_registry = token[1];
#ifndef QT_NO_TEXTCODEC
		    mapper = calc_mapper(charset_registry,charset_encoding);
#endif
		} else if ( tag == "CHARSET_ENCODING" ) {
		    charset_encoding = token[1];
#ifndef QT_NO_TEXTCODEC
		    mapper = calc_mapper(charset_registry,charset_encoding);
#endif
		} else if ( tag == "FONTBOUNDINGBOX" ) {
		    bbox = QRect(token[3].toInt(),token[4].toInt(),
				 token[1].toInt(),token[2].toInt());
		    fascent = bbox.bottom()+1;
		    fdescent = -bbox.y()-1;
		    fmaxwidth = bbox.width();
		} else if ( tag == "FONT_ASCENT" ) {
		    if ( fascent < 0 ) fascent = token[1].toInt();
		} else if ( tag == "FONT_DESCENT" ) {
		    if ( fdescent < 0 ) fdescent = token[1].toInt();
		} else if ( tag == "DEFAULT_CHAR" ) {
		    default_char = token[1].toInt();
		} else if ( tag == "CHARS" ) {
		    num_glyphs = token[1].toInt();
		    glyph = new QGlyph[0x10000];
		    for (int i=0; i<0x10000; i++) {
			glyph[i].data = 0;
			glyph[i].metrics = 0;
		    }
		} else if ( tag == "POINT_SIZE" ) {
		    ptsize = token[1].toInt();
		}
	    }
	} while (!s.atEnd());
	funderlinepos = ptsize/200+1;
	funderlinewidth = ptsize/200+1;
    }

    ~QRenderedFontBDF()
    {
	if ( glyph ) {
	    for (int i=0; i<0x10000; i++) {
		if ( glyph[i].data )
		    delete [] glyph[i].data;
		delete glyph[i].metrics;
	    }
	}
    }

    bool inFont(glyph_t g) const
    {
	return glyph[g].data ? TRUE : FALSE;
    }

    QGlyph render(glyph_t gl)
    {
	// Return a copy
	QGlyph* g = &glyph[gl];
	if ( !g->data )
	    g = &glyph[default_glyph];
	QGlyph ng;
	ng.metrics = new QGlyphMetrics;
	memcpy(ng.metrics,g->metrics,sizeof(*g->metrics));
	int n=ng.metrics->linestep*ng.metrics->height;
	ng.data = new uchar[n];
	memcpy(ng.data,g->data,n);
	return ng;
    }
};

QFontFactoryBDF::QFontFactoryBDF()
{
}

QFontFactoryBDF::~QFontFactoryBDF()
{
}


QRenderedFont * QFontFactoryBDF::get(const QFontDef & f,QDiskFont * f2)
{
    QRenderedFontBDF * qrf=new QRenderedFontBDF(f2, f);
    return qrf;
}

void QFontFactoryBDF::load(QDiskFont * qdf) const
{
    if(qdf->loaded)
	return;
    qdf->p=0;
    qdf->loaded=TRUE;
}

bool QFontFactoryBDF::unicode(QRenderedFont *,int &)
{
    qFatal("Why is this still used");
    return TRUE;
}

#endif // QT_NO_BDF
