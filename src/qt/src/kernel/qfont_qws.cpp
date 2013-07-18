/****************************************************************************
**
** Implementation of QFont, QFontMetrics and QFontInfo classes for FB
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

#include "qwidget.h"
#include "qpainter.h"
#include "qfontdata_p.h"
#include <private/qunicodetables_p.h>
#include "qfontdatabase.h"
#include "qstrlist.h"
#include "qcache.h"
#include "qdict.h"
#include "qtextcodec.h"
#include "qapplication.h"
#include "qfile.h"
#include "qtextstream.h"
#include "qmap.h"
#include "qshared.h"
#include "qfontmanager_qws.h"
#include "qmemorymanager_qws.h"
#include "qgfx_qws.h"
#include "qtextengine_p.h"
#include "qfontengine_p.h"

QFont::Script QFontPrivate::defaultScript = QFont::UnknownScript;

void QFont::initialize()
{
    // create global font cache
    if ( ! QFontCache::instance ) (void) new QFontCache;
}

void QFont::cleanup()
{
    // delete the global font cache
    delete QFontCache::instance;
}


/*****************************************************************************
  QFont member functions
 *****************************************************************************/

Qt::HANDLE QFont::handle() const
{
    QFontEngine *engine = d->engineForScript( QFontPrivate::defaultScript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

    return engine->handle();
}

QString QFont::rawName() const
{
    return "unknown";
}

void QFont::setRawName( const QString & )
{
}


bool QFont::dirty() const
{
    return d->engineData == 0;
}

QString QFont::defaultFamily() const
{
    switch( d->request.styleHint ) {
	case QFont::Times:
	    return QString::fromLatin1("times");
	case QFont::Courier:
	    return QString::fromLatin1("courier");
	case QFont::Decorative:
	    return QString::fromLatin1("old english");
	case QFont::Helvetica:
	case QFont::System:
	default:
	    return QString::fromLatin1("helvetica");
    }
}

QString QFont::lastResortFamily() const
{
    return QString::fromLatin1("helvetica");
}

QString QFont::lastResortFont() const
{
    qFatal( "QFont::lastResortFont: Cannot find any reasonable font" );
    // Shut compiler up
    return QString::null;
}

void QFontPrivate::load( QFont::Script )
{
    QFontDef req = request;

    // 75 dpi on embedded
    if ( req.pixelSize == -1 )
	req.pixelSize = req.pointSize/10;
    if ( req.pointSize == -1 )
	req.pointSize = req.pixelSize*10;

    if ( ! engineData ) {
	QFontCache::Key key( req, QFont::NoScript, (int)paintdevice, paintdevice );

	// look for the requested font in the engine data cache
	engineData = QFontCache::instance->findEngineData( key );

	if ( ! engineData ) {
	    // create a new one
	    engineData = new QFontEngineData;
	    QFontCache::instance->insertEngineData( key, engineData );
	} else {
	    engineData->ref();
	}
    }

    // the cached engineData could have already loaded the engine we want
    if ( engineData->engine ) return;

    // load the font
    engineData->engine = new QFontEngine(req, paintdevice);
    engineData->engine->ref();
}


/*****************************************************************************
  QFontMetrics member functions
 *****************************************************************************/

int QFontMetrics::leftBearing(QChar ch) const
{
    QFontEngine *engine = d->engineForScript( QFont::NoScript );
    return (memorymanager->lockGlyphMetrics( engine->handle(), ch.unicode() )->bearingx*engine->scale)>>8;
}


int QFontMetrics::rightBearing(QChar ch) const
{
    QFontEngine *engine = d->engineForScript( QFont::NoScript );
    QGlyphMetrics *metrics = memorymanager->lockGlyphMetrics( engine->handle(), ch.unicode() );
    return ((metrics->advance - metrics->width - metrics->bearingx)*engine->scale)>>8;
}

int QFontMetrics::charWidth( const QString &str, int pos ) const
{
    if ( pos < 0 || pos > (int)str.length() )
	return 0;

    const QChar &ch = str.unicode()[ pos ];
    if ( ch.unicode() < QFontEngineData::widthCacheSize &&
	 d->engineData && d->engineData->widthCache[ ch.unicode() ] )
	return (d->engineData->widthCache[ ch.unicode() ]*d->engineData->engine->scale)>>8;

    QFont::Script script;
    SCRIPT_FOR_CHAR( script, ch );

    int width;

    if ( script >= QFont::Arabic && script <= QFont::Khmer ) {
	// complex script shaping. Have to do some hard work
	int from = QMAX( 0,  pos - 8 );
	int to = QMIN( (int)str.length(), pos + 8 );
	QConstString cstr( str.unicode()+from, to-from);
	QTextEngine layout( cstr.string(), d );
	layout.itemize( QTextEngine::WidthOnly );
	width = layout.width( pos-from, 1 );
    } else if ( ::category( ch ) == QChar::Mark_NonSpacing || qIsZeroWidthChar(ch.unicode())) {
        width = 0;
    } else {
	QFontEngine *engine = d->engineForScript( script );
#ifdef QT_CHECK_STATE
	Q_ASSERT( engine != 0 );
#endif // QT_CHECK_STATE

	glyph_t glyphs[8];
	advance_t advances[8];
	int nglyphs = 7;
	engine->stringToCMap( &ch, 1, glyphs, advances, &nglyphs, FALSE );
	width = advances[0];
    }
    if ( ch.unicode() < QFontEngineData::widthCacheSize && width > 0 && width < 0x100 )
	d->engineData->widthCache[ ch.unicode() ] = width;
    return width;
}

int QFontMetrics::width( QChar ch ) const
{
    unsigned short uc = ch.unicode();
    QFontEngine *engine = d->engineForScript( QFont::NoScript );
    if ( uc < QFontEngineData::widthCacheSize &&
	 d->engineData && d->engineData->widthCache[ uc ] )
	return (d->engineData->widthCache[ uc ]*engine->scale)>>8;

    if ( ::category( ch ) == QChar::Mark_NonSpacing || qIsZeroWidthChar(ch.unicode()))
	return 0;

    int width = memorymanager->lockGlyphMetrics( engine->handle(), ch.unicode() )->advance;

    if ( ch.unicode() < QFontEngineData::widthCacheSize && width > 0 && width < 0x100 )
	d->engineData->widthCache[ ch.unicode() ] = width;

    return (width*engine->scale)>>8;
}

// ### should maybe use the common method aswell, but since that one is quite a bit slower
// and we currently don't support complex text rendering on embedded, leave it as is.
/*!
    \overload

    Returns the width of the first \a len characters of string \a str.
*/
int QFontMetrics::width( const QString &str, int len ) const
{
    if ( len < 0 )
	len = str.length();
    int ret=0;
    QFontEngine *engine = d->engineForScript( QFont::NoScript );
    for (int i=0; i<len; i++)
	ret += memorymanager->lockGlyphMetrics( engine->handle(), str[i].unicode() )->advance;
    return (ret*engine->scale)>>8;
}

/*!
    \overload

    Returns the bounding rectangle that contains the first \a len
    characters of string \a str.
*/
QRect QFontMetrics::boundingRect( const QString &str, int len ) const
{
    return QRect( 0,-(ascent()),width(str,len),height());
}

/*!
    Saves the glyphs in the font that have previously been accessed as
    a QPF file. If \a all is TRUE (the default), then before saving,
    all glyphs are marked as used.

    If the font is large and you are sure that only a subset of
    characters will ever be required on the target device, passing
    FALSE for the \a all parameter can save a significant amount of
    disk space.

    Note that this function is only applicable on Qt/Embedded.
*/
void QFont::qwsRenderToDisk(bool all)
{
#ifndef QT_NO_QWS_SAVEFONTS
    memorymanager->savePrerenderedFont(handle(), all);
#endif
}
