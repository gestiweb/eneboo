/****************************************************************************
**
** Definition of font rendering infrastructure for Embedded Qt
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

#ifndef QFONTMANAGER_QWS_H
#define QFONTMANAGER_QWS_H

#ifndef QT_H
#include "qfont.h"
#include "qptrlist.h"
#include <private/qtextengine_p.h>
#endif // QT_H

// These are stored in the shared memory segment in front of their
// data, and indexed at the start of the segment

// This needs to be a multiple of 64 bits

class QFontDef;

class Q_PACKED QGlyphMetrics {

public:
    Q_UINT8 linestep;
    Q_UINT8 width;
    Q_UINT8 height;
    Q_UINT8 flags;

    Q_INT8 bearingx;      // Difference from pen position to glyph's left bbox
    Q_UINT8 advance;       // Difference between pen positions
    Q_INT8 bearingy;      // Used for putting characters on baseline

    Q_INT8 reserved;      // Do not use

    // Flags:
    // RendererOwnsData - the renderer is responsible for glyph data
    //                    memory deletion otherwise QGlyphTree must
    //                    delete [] the data when the glyph is deleted.
    enum Flags { RendererOwnsData=0x01 };
};

class QGlyph {
public:
    QGlyph() { metrics=0; data=0; }
    QGlyph(QGlyphMetrics* m, uchar* d) :
	metrics(m), data(d) { }
    ~QGlyph() {}

    QGlyphMetrics* metrics;
    uchar* data;
};



class QFontFactory;
class QDiskFont;

// This is a particular font instance at a particular resolution
// e.g. Truetype Times, 10 point. There's only one of these though;
// we want to share generated glyphs

class QRenderedFont {

public:

    // Normal font-type is monochrome; glyph data is a
    //   bitmap, which doesn't use much memory

    // Initialise for name A, renderer B, font type C, D glyphs

    QRenderedFont(QDiskFont *,const QFontDef&);
    virtual ~QRenderedFont();

    QFontDef fontDef() const;

    int refcount;

    int ptsize;

    bool italic;
    unsigned int weight;

    void ref() { refcount++; }
    bool deref() { refcount--; return refcount==0; }

    bool match(const QFontDef &);

    QDiskFont* diskfont;
    int fascent,fdescent;
    int fleftbearing,frightbearing;
    int fmaxwidth;
    int fleading;
    int funderlinepos;
    int funderlinewidth;
    bool smooth;
    int maxchar;

    int ascent() { return fascent; }
    int descent() { return fdescent; }
    int width(int);
    int width( const QString&, int =-1 );
    int leftBearing(int);
    int rightBearing(int);

    // Calling any of these can trigger a full-font metrics check
    // which can be expensive
    int minLeftBearing();
    int minRightBearing();
    int maxWidth();

    virtual bool inFont(glyph_t g) const=0;
    virtual QGlyph render(glyph_t g)=0;

private:

};

// Keeps track of available renderers and which font is which

class QDiskFontPrivate {};

class QDiskFont {

public:
    QDiskFont(QFontFactory *f, const QString& n, bool i, int w, int s,
	      const QString &fl, const QString& fi) :
	factory(f), name(n), italic(i), weight(w), size(s), flags(fl), file(fi)
    {
	loaded=FALSE;
	p=0;
    }

    QRenderedFont* load(const QFontDef &);

    QFontDef fontDef() const;

    QFontFactory *factory;
    QString name;
    bool italic;
    int weight;
    int size;
    QString flags;
    QString file;
    bool loaded;

    QDiskFontPrivate * p;
};

class QCachePolicy {

public:

    virtual void cache(QRenderedFont *)=0;
    virtual void uncache(QRenderedFont *)=0;

};

// Exposed here so the default policy can be reset

class QDefaultCachePolicy : public QCachePolicy {

public:

    virtual void cache(QRenderedFont *);
    virtual void uncache(QRenderedFont *);

};

class QFontManager {

public:

    QPtrList<QFontFactory> factories;
    QPtrList<QRenderedFont> cachedfonts;
    QPtrList<QDiskFont> diskfonts;

    QFontManager();
    ~QFontManager();

    // Font definition, type and color
    QDiskFont * get(const QFontDef &);

    static int cmpFontDef(const QFontDef & goal, const QFontDef & choice);

    static void initialize();
    static void cleanup();

    void setPolicy(QCachePolicy *);

    void cache(QRenderedFont * f) { policy->cache(f); }
    void uncache(QRenderedFont * f) { policy->uncache(f); }
    QRenderedFont * getCached(const QFontDef &);

private:

    QCachePolicy * policy;

};

class QFontFactory {

public:

    QFontFactory() {}
    virtual ~QFontFactory() {}

    virtual QRenderedFont * get(const QFontDef &,QDiskFont *)=0;
    virtual void load(QDiskFont *) const=0;
    virtual void unload(QDiskFont *) {}
    virtual QString name()=0;
};

void qt_init_fonts();

extern QFontManager * qt_fontmanager;

#endif
