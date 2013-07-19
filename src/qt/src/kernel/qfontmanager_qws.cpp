/****************************************************************************
**
** Definition of QFontFactory for Truetype class for Embedded Qt
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

#include "qfontmanager_qws.h"
#include "qfontfactoryttf_qws.h"
#include "qfontfactorybdf_qws.h"
#include "qfontdata_p.h"
#include "qfile.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//
// INSTALL/lib/fonts/fontdir lists a sequence of:
//
//    <name> <file> <renderer> <italic> <weight> <size> <flags>
//
// eg.
//      <name> = Helvetica
//      <file> = /usr/local/qt-embedded/lib/fonts/helvR0810.bdf or
//                 /usr/local/qt-embedded/lib/fonts/verdana.ttf, etc.
//  <renderer> = BDF or FT
//    <italic> = y or n
//    <weight> = 50 is Normal, 75 is Bold, etc.
//      <size> = 0 for scalable or 10 time pointsize (eg. 120 for 12pt)
//     <flags> = flag characters:
//                 s = smooth (anti-aliased)
//                 u = unicode range when saving (default is Latin 1)
//                 a = ascii range when saving (default is Latin 1)
//
// and of course...
//    # This is a comment
//

QString qws_topdir()
{
    QString ret( qInstallPath() );

    // ### REMOVE ME 3.1
    // we shouldn't use QTDIR to change the behavior of the library at
    // runtime.  the install prefix above should be enough when
    // switching between Qt versions.
    const char *r = getenv( "QTDIR" );
    if ( r ) ret = QString( r );

    return ret;
}

QFontManager * qt_fontmanager=0;

/*!
    \class QFontManager qfontmanager_qws.h
    \brief The QFontManager class implements font management in Qt/Embedded.
    \ingroup qws

    There is one and only one QFontManager per Qt/Embedded
    application. The \c qt_fontmanager global variable points to it. It
    keeps a list of font factories, a cache of rendered fonts and a
    list of fonts available on disk. QFontManager is called when a new
    font needs to be rendered from a Freetype-compatible or BDF font
    on disk; this only happens if there isn't an appropriate QPF font
    already available.
*/

/*!
    \fn void QFontManager::cache(QRenderedFont * f)

    \internal

    Caches the given font, \a f.
*/

/*!
    \fn void QFontManager::uncache(QRenderedFont * f)

    \internal

    Removes font \a f from the font cache.
*/

/*!
    Creates a new QFontManager and points \c qt_fontmanager to it
*/
void QFontManager::initialize()
{
    qt_fontmanager=new QFontManager();
}

/*!
    Destroys the font manager
*/
void QFontManager::cleanup()
{
    delete qt_fontmanager;
    qt_fontmanager = 0;
}

/*!
    \class QRenderedFont qfontmanager_qws.h
    \brief The QRenderedFont class is the rendered version of a particular
    font.

    \internal
    \omit Expected to become public API \endomit

    \ingroup qws

    The font is specified by a QFont, and is rendered using the font's
    weight, family and point size, italic or not, and so on. There is
    one and only one QRenderedFont for each particular QFont
    specification; if you specify two Times 10pt bold italic QFonts
    they will both refer to the same QRenderedFont. QRenderedFonts are
    cached by QFontManager and are reference counted; when there is no
    QGfx referring to a particular QRenderedFont it is deleted. Each
    QRenderedFont renders glyphs (that is, images of characters) on
    demand and caches the rendered glyphs. It can be subclassed by
    individual factories since they create new QRenderedFonts.
*/

/*!
    \fn QRenderedFont::QRenderedFont(QDiskFont * df, const QFontDef &d)

    This constructs a QRenderedFont; the QDiskFont \a df and QFontDef
    \a d are needed by the font factory to render glyphs.
*/

QRenderedFont::QRenderedFont(QDiskFont * df, const QFontDef &d)
{
    diskfont = df;
    ptsize=d.pointSize;
    refcount=0;
    ref();
    fleftbearing=0;
    frightbearing=0;
    fmaxwidth=0;
    smooth = df->flags.contains('s');
    if ( df->flags.contains('u') ) {
	maxchar = 65535;
    } else if ( df->flags.contains('a') ) {
	maxchar = 127;
    } else {
	maxchar = 255;
    }
}

/*!
    \fn QRenderedFont::~QRenderedFont()

    Destroys a QRenderedFont
*/

QRenderedFont::~QRenderedFont()
{
}

bool QRenderedFont::match(const QFontDef & f)
{
    if(f.family.lower() != diskfont->name )
        return false;
    if(f.italic !=  italic)
        return false;
    if(f.pointSize != ptsize )
        return false;
    if(f.weight != weight )
        return false;
    return true;
}

// Triggering a whole font metrics call is bad, so right now return
// some best guesses

/*!
    Returns the minimum left bearing (distance before the start of a
    character) of any character in the font.

    \warning Unimplemented.
*/

int QRenderedFont::minLeftBearing()
{
    return 0;
}

/*!
    Returns the minimum right bearing (distance from the end of a
    character) of any character in the font.

    \warning Unimplemented.
*/

int QRenderedFont::minRightBearing()
{
    return 0;
}

/*!
    Returns the maximum width in pixels of any character in the font.
*/

int QRenderedFont::maxWidth()
{
    // Actually max advance
    return fmaxwidth;
}

/*!
    Creates a font manager. This method reads in the font definition
    file from \c INSTALL/lib/fonts/fontdir, where \c INSTALL is the
    directory where Qt/Embedded was installed, and creates a list of
    QDiskFonts to hold the information in the file. It also constructs
    any defined font factories.
*/

QFontManager::QFontManager()
{
    factories.setAutoDelete(TRUE);
    diskfonts.setAutoDelete(TRUE);
    diskfonts.setAutoDelete(TRUE);
    cachedfonts.setAutoDelete(TRUE);

#ifndef QT_NO_FREETYPE
    factories.append(new QFontFactoryFT());
#endif
#ifndef QT_NO_BDF
    factories.append(new QFontFactoryBDF());
#endif

    // Load in font definition file
    QString fn = qws_topdir() + "/lib/fonts/fontdir";
    FILE* fontdef=fopen(fn.local8Bit(),"r");
    if(!fontdef) {
	QCString temp=fn.local8Bit();
	qWarning("Cannot find font definition file %s - is Qt installed correctly?",
	       temp.data());
	exit(1);
	//return;
    }
    char buf[200]="";
    char name[200]="";
    char render[200]="";
    char file[200]="";
    char flags[200]="";
    char isitalic[10]="";
    fgets(buf,200,fontdef);
    while(!feof(fontdef)) {
	if ( buf[0] != '#' ) {
	    QFontFactory * factoryp;
	    int weight=50;
	    int size=0;
	    flags[0]=0;
	    sscanf(buf,"%s %s %s %s %d %d %s",name,file,render,isitalic,&weight,&size,flags);
	    QString filename;
	    if ( file[0] != '/' )
		filename = qws_topdir() + "/lib/fonts/";
	    filename += file;
	    if ( QFile::exists(filename) ) {
		for(factoryp=factories.first();factoryp;factoryp=factories.next()) {
		    if( factoryp->name() == render ) {
			QDiskFont * qdf=new QDiskFont(factoryp,name,isitalic[0]=='y',
						      weight,size,flags,filename);
			diskfonts.append(qdf);
			break;
		    }
		}
	    }
	}
	fgets(buf,200,fontdef);
    }
    fclose(fontdef);
    policy = new QDefaultCachePolicy();
}

/*!
    \fn QFontManager::~QFontManager()

    Destroys the QFontManager and sets \c qt_fontmanager to 0.
*/

QFontManager::~QFontManager()
{
    delete policy;

    if ( qt_fontmanager == this )
	qt_fontmanager = 0;
}

extern bool qws_savefonts; //in qapplication_qws.cpp
/*! \internal */
int QFontManager::cmpFontDef(const QFontDef & goal, const QFontDef & choice)
{
    int r = 100;
    if (goal.family.lower() == choice.family.lower())
	r += 1000;
    // Match closest weight
    r -= abs(goal.weight-choice.weight);
    // Favour italicness ahead of weight
    if (goal.italic==choice.italic)
	r += 100;
    if ( choice.pointSize ) {
	// A bit smaller is better than a bit bigger.
	if ( choice.pointSize > goal.pointSize) {
	    r += 1 - (choice.pointSize - goal.pointSize)*2;
	} else {
	    r += 1 - (goal.pointSize - choice.pointSize);
	}
    }
    return r;
}

/*!
    Returns the QDiskFont that best matches \a f, based on family,
    weight, italicity and font size.
*/

QDiskFont * QFontManager::get(const QFontDef & f)
{
    QDiskFont * qdf;
    QDiskFont * bestmatch=diskfonts.first();
    int bestmatchval=0;

    for(qdf=diskfonts.first();qdf;qdf=diskfonts.next()) {
	QFontDef def = qdf->fontDef();
	int mymatchval = cmpFontDef(f,def);
	if ( mymatchval>bestmatchval) {
	    bestmatchval=mymatchval;
	    bestmatch=qdf;
	}
    }

    return bestmatch;
}

/*!
    \internal

    Deletes the existing font cache policy and sets the cache policy
    to \a p.
*/
void QFontManager::setPolicy(QCachePolicy * p)
{
    delete policy;
    policy=p;
}

/*!
    \internal

    Returns the cached font that matches font definition \a f, or 0 if
    there is no matching font in the font cache.
*/
QRenderedFont* QFontManager::getCached(const QFontDef & f)
{
    QRenderedFont * it;
    for(it=cachedfonts.first();it;it=cachedfonts.next()) {
        if(it->match(f)) {
            return it;
        }
    }
    return 0;
}

void QDefaultCachePolicy::cache(QRenderedFont * f)
{
    if( !qt_fontmanager->cachedfonts.findRef( f ) ) {
        qt_fontmanager->cachedfonts.append(f);
    }
}

void QDefaultCachePolicy::uncache(QRenderedFont * f)
{
    qt_fontmanager->cachedfonts.removeRef( f );
    delete f;
}

/*!
    Loads the disk font, \a f as a rendered font.
*/
QRenderedFont* QDiskFont::load(const QFontDef & f)
{
    // First check cache
    QRenderedFont * ret;
    ret=qt_fontmanager->getCached(f);
    if( ret ) {
        ret->ref();
        return ret;
    }
    factory->load(this);
    ret=factory->get(f,this);
    qt_fontmanager->cache(ret);
    return ret;
}

/*!
    Returns a QFontDef equivalent to this disk font. The pointSize
    may be 0 which indicates that this is a scalable font.
*/
QFontDef QDiskFont::fontDef() const
{
    QFontDef r;
    r.family = name;
    r.italic = italic;
    r.weight = weight;
    r.pointSize = size;
    return r;
}

/*!
    \fn int QRenderedFont::ascent()

    Returns the font's ascent (the distance from the baseline to the
    top of the tallest character)
*/

/*!
    \fn int QRenderedFont::descent()

    Returns the font's descent (the distance from the baseline to the
    bottom of the lowest character)
*/

/*!
    \overload int QRenderedFont::width(int)

    Returns the width in pixels of the specified Unicode character.
*/

/*!
    \fn int QRenderedFont::width(const QString & s,int l=-1 );

    Returns the width in pixels of the first \a l characters of the
    string \a s, or the whole string if the value for \a l is not
    specified. This should be used in preference to adding up the
    widths of each character in the string since it can take account
    of kerning and inter-character spacing
*/

/*!
    \fn int QRenderedFont::leftBearing(int)

    Returns the left bearing (distance in pixels before the start of
    the letter) of the character specified.
*/

/*!
    \fn int QRenderedFont::rightBearing(int)

    Returns the right bearing (distance in pixels after the end of the
    letter) of the character specified.
*/

/*!
    \fn bool QRenderedFont::inFont( glyph_t g) const

    \internal

    Returns true if the Unicode character \a g is in the font;
    otherwise returns FALSE.
*/

/*!
    \fn virtual QGlyph QRenderedFont::render( glyph_t g)

    \internal

    Renders the Unicode character \a g, returning a QGlyph. A QGlyph
    has two members, metrics and data. Metrics contains information on
    the size, advance width and so forth of the character, data a
    pointer to the raw data for the character -- either a 1-bit per
    pixel bitmap or an 8-bit per pixel alpha map of the character, the
    linestep of which is specified in QGlyphMetrics.
*/
