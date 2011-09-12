/****************************************************************************
** $Id: qt/qfont_mac.cpp   3.3.8   edited Jan 11 14:38 $
**
** Implementation of QFont/QFontMetrics class for mac
**
** Created : 001019
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

#include "qstring.h"
#include "qfont.h"
#include "qfontdata_p.h"
#include "qfontengine_p.h"
#include "qpaintdevicemetrics.h"
#include "qfontmetrics.h"
#include "qfontinfo.h"
#include "qt_mac.h"
#include "qpaintdevice.h"
#include <qdict.h>
#include <qapplication.h>
#include <qpainter.h>
#include <private/qunicodetables_p.h>
#include <stdlib.h>

FMFontFamily qt_macFamilyName(const QString &); // qfontdatabase_mac.cpp
QFont::Script QFontPrivate::defaultScript = QFont::UnknownScript;

static inline int qt_mac_pixelsize(const QFontDef &def, QPaintDevice *pdev)
{
    float ret;
    if(def.pixelSize == -1) {
	if(pdev) {
	    ret = def.pointSize *  QPaintDeviceMetrics(pdev).logicalDpiY() / 720.;
	} else {
	    short vr, hr;
	    ScreenRes(&hr, &vr);
	    ret = def.pointSize * vr / 720.;
	}
    } else {
	ret = def.pixelSize;
    }
    return (int)(ret + .5);
}
static inline int qt_mac_pointsize(const QFontDef &def, QPaintDevice *pdev)
{
    float ret;
    if(def.pointSize == -1) {
	if(pdev) {
	    ret = def.pixelSize * 720. / QPaintDeviceMetrics(pdev).logicalDpiY();
	} else {
	    short vr, hr;
	    ScreenRes(&hr, &vr);
	    ret = def.pixelSize * 720. / vr;
	}
    } else {
	ret = def.pointSize;
    }
    return (int)(ret + .5);
}

QMacFontInfo *
QMacSetFontInfo::createFontInfo(const QFontEngine *fe, const QFontDef *def, QPaintDevice *pdev)
{
    Q_ASSERT(fe->type() == QFontEngine::Mac);
    const QFontEngineMac *mac_fe = (QFontEngineMac*)fe;

    QMacFontInfo *ret = new QMacFontInfo();
    //face
    ret->setFont(mac_fe->fmfam);

    //style
    short face = normal;
    if(def->italic)
	face |= italic;
    if(def->weight == QFont::Bold)
	face |= bold;
    ret->setStyle(face);

    //size
    int psize = qt_mac_pixelsize(*def, pdev);
    ret->setSize(psize);

    //encoding
    TextEncoding enc;
    UpgradeScriptInfoToTextEncoding(FontToScript(mac_fe->fmfam), kTextLanguageDontCare,
				    kTextRegionDontCare, NULL, &enc);
    ret->setEncoding(enc);

    //Create a cacheable ATSUStyle
    const int arr_guess = 7;
    int arr = 0;
    ATSUAttributeTag tags[arr_guess];
    ByteCount valueSizes[arr_guess];
    ATSUAttributeValuePtr values[arr_guess];
    tags[arr] = kATSUSizeTag; //font size
    Fixed fsize = FixRatio(psize, 1);
    valueSizes[arr] = sizeof(fsize);
    values[arr] = &fsize;
    arr++;
    tags[arr] = kATSUFontTag;  //font
    Str255 tmpFont;
    FMGetFontFamilyName(mac_fe->fmfam, tmpFont);
    short fnum;
    GetFNum(tmpFont, &fnum);
    ATSUFontID fond;
    ATSUFONDtoFontID(fnum, 0, &fond);
    valueSizes[arr] = sizeof(fond);
    values[arr] = &fond;
    arr++;
    tags[arr] = kATSUQDItalicTag;
    valueSizes[arr] = sizeof(Boolean);
    Boolean italicBool = def->italic ? true : false;
    values[arr] = &italicBool;
    arr++;
    tags[arr] = kATSUQDBoldfaceTag;
    valueSizes[arr] = sizeof(Boolean);
    Boolean boldBool = ((def->weight == QFont::Bold) ? true : false);
    values[arr] = &boldBool;
    arr++;
#if QT_MACOSX_VERSION >= 0x1020
    CGAffineTransform tf = CGAffineTransformIdentity;
    if(def->stretch != 100) {
	tf = CGAffineTransformMakeScale(double(def->stretch)/100., 1);
	tags[arr] = kATSUFontMatrixTag;
	valueSizes[arr] = sizeof(tf);
	values[arr] = &tf;
	arr++;
    }
#endif
    if(arr > arr_guess) //this won't really happen, just so I will not miss the case
	qDebug("Qt: internal: %d, WH0A %d: arr_guess overflow", __LINE__, arr);

    //create style
    QATSUStyle *st = new QATSUStyle;
    st->rgb.red = st->rgb.green = st->rgb.blue = 0;
    ATSUCreateStyle(&st->style);
    if(OSStatus e = ATSUSetAttributes(st->style, arr, tags, valueSizes, values)) {
	qDebug("Qt: internal: %ld: unexpected condition reached %s:%d", e, __FILE__, __LINE__);
	delete st;
	st = NULL;
    } else {
	const int feat_guess=5;
	int feats=0;
	ATSUFontFeatureType feat_types[feat_guess];
	ATSUFontFeatureSelector feat_values[feat_guess];
	feat_types[feats] = kLigaturesType;
	feat_values[feats] = kRareLigaturesOffSelector;
	feats++;
	feat_types[feats] = kLigaturesType;
	feat_values[feats] = kCommonLigaturesOffSelector;
	feats++;
	if(feats > feat_guess) //this won't really happen, just so I will not miss the case
	    qDebug("Qt: internal: %d: WH0A feat_guess underflow %d", __LINE__, feats);
	if(OSStatus e = ATSUSetFontFeatures(st->style, feats, feat_types, feat_values))
	    qDebug("Qt: internal: %ld: unexpected condition reached %s:%d", e, __FILE__, __LINE__);
    }
    ret->setATSUStyle(st);
    return ret;
}

bool
QMacSetFontInfo::setMacFont(const QFontEngine *fe, QMacSetFontInfo *sfi, QPaintDevice *pdev)
{
    Q_ASSERT(fe->type() == QFontEngine::Mac);
    QFontEngineMac *mac_fe = (QFontEngineMac*)fe;
    if(!mac_fe->internal_fi) {
	mac_fe->internal_fi = createFontInfo(fe, &fe->fontDef, pdev);
	mac_fe->calculateCost();
    }
    return setMacFont(mac_fe->internal_fi, sfi);
}

bool
QMacSetFontInfo::setMacFont(const QFontPrivate *d, QMacSetFontInfo *sfi, QPaintDevice *pdev)
{
    QFontEngine *eng = d->engineForScript( QFont::NoScript );
    Q_ASSERT( eng->type() == QFontEngine::Mac );
    QFontEngineMac *engine = (QFontEngineMac *) eng;
    QMacFontInfo *fi = engine->internal_fi;
    if(!fi) {
	engine->internal_fi = fi = createFontInfo(engine, &eng->fontDef, pdev);
	engine->calculateCost();
    }
    return setMacFont(fi, sfi);
}

bool
QMacSetFontInfo::setMacFont(const QMacFontInfo *fi, QMacSetFontInfo *sfi)
{
    if(!sfi || fi->font() != sfi->tfont)
	TextFont(fi->font());
    if(!sfi || fi->style() != sfi->tface)
	TextFace(fi->style());
    if(!sfi || fi->size() != sfi->tsize)
	TextSize(fi->size());
    if(sfi)
	*((QMacFontInfo*)sfi) = *(fi);
    return TRUE;
}

/* utility functions */
static inline void qstring_to_pstring(QString s, int len, Str255 str, TextEncoding encoding)
{
    UnicodeMapping mapping;
    UnicodeToTextInfo info;
    mapping.unicodeEncoding = CreateTextEncoding(kTextEncodingUnicodeDefault,
						  kTextEncodingDefaultVariant,
						 kUnicode16BitFormat);
    mapping.otherEncoding = encoding;
    mapping.mappingVersion = kUnicodeUseLatestMapping;

    if(CreateUnicodeToTextInfo(&mapping, &info) != noErr) {
	qDebug("Qt: internal: Unexpected condition reached %s:%d", __FILE__, __LINE__);
	return;
    }
    const int unilen = len * 2;
    const UniChar *unibuf = (UniChar *)s.unicode();
    ConvertFromUnicodeToPString(info, unilen, unibuf, str);
    DisposeUnicodeToTextInfo(&info);
}

/* Qt platform dependent functions */

int QFontMetrics::width(QChar c) const
{
    QFontEngine *engine = d->engineForScript( (QFont::Script) fscript );
#ifdef QT_CHECK_STATE
    Q_ASSERT( engine != 0 );
    Q_ASSERT( engine->type() == QFontEngine::Mac );
#endif // QT_CHECK_STATE

    return ((QFontEngineMac*) engine)->doTextTask(&c, 0, 1, 1,
						  QFontEngineMac::WIDTH);
}

int QFontMetrics::charWidth(const QString &str, int pos) const
{
    if ( pos < 0 || pos > (int)str.length() )
	return 0;

    const QChar &ch = str.unicode()[ pos ];

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
    } else if ( ::category( ch ) == QChar::Mark_NonSpacing || qIsZeroWidthChar(ch.unicode()) ) {
        width = 0;
    } else {
        width = this->width(ch);
    }
    return width;
}

QString QFont::rawName() const
{
    return family();
}

void QFont::setRawName(const QString &name)
{
    setFamily(name);
}

void QFont::cleanup()
{
    delete QFontCache::instance;
}

// Return a font family ID
Qt::HANDLE QFont::handle() const
{
    if ( ! d->engineData )
        d->load( QFont::NoScript );
    if (d->engineData && d->engineData->engine)
	return (Qt::HANDLE)((UInt32)((QFontEngineMac*)d->engineData->engine)->fmfam);
    return 0;
}

void QFont::macSetFont(QPaintDevice *v)
{
    QMacSavedPortInfo::setPaintDevice(v);
    QMacSetFontInfo::setMacFont(d, NULL, v);
}

void QFontPrivate::load(QFont::Script script)
{
#if defined(QT_CHECK_STATE)
    // sanity checks
    if (!QFontCache::instance)
	qWarning("Must construct a QApplication before a QFont");
    Q_ASSERT(script >= 0 && script < QFont::LastPrivateScript);
#endif // QT_CHECK_STATE

    QFontEngineMac *engine = NULL;

    QFontDef req = request;
    req.pixelSize = qt_mac_pixelsize(request, paintdevice);
    req.pointSize = 0;
    QFontCache::Key key = QFontCache::Key(req, QFont::NoScript, screen, paintdevice);

    if(!(engineData = QFontCache::instance->findEngineData(key))) {
	engineData = new QFontEngineData;
	QFontCache::instance->insertEngineData(key, engineData);
    } else {
	engineData->ref();
    }

    if ( engineData->engine ) // already loaded
	return;

    if(QFontEngine *e = QFontCache::instance->findEngine(key)) {
	Q_ASSERT(e->type() == QFontEngine::Mac);
	e->ref();
	engineData->engine = e;
	engine = (QFontEngineMac*)e;
	return; // the font info and fontdef should already be filled
    }

    engineData->engine = engine = new QFontEngineMac;
    //find the font
    QStringList family_list = QStringList::split( ',', request.family );
    // append the substitute list for each family in family_list
    {
        QStringList subs_list;
        QStringList::ConstIterator it = family_list.begin(), end = family_list.end();
        for ( ; it != end; ++it )
            subs_list += QFont::substitutes( *it );
        family_list += subs_list;
    }
    // add QFont::defaultFamily() to the list, for compatibility with
    // previous versions
    family_list << QApplication::font().defaultFamily();
    QString fam_name;
    for(QStringList::ConstIterator it = family_list.begin(); it !=  family_list.end(); ++it) {
        Str255 actual_str;

        FMFontFamily fmfam = qt_macFamilyName(*it);
        FMGetFontFamilyName(fmfam, actual_str);
        TextEncoding encoding;
        FMGetFontFamilyTextEncoding(fmfam, &encoding);
        TextToUnicodeInfo uni_info;
        CreateTextToUnicodeInfoByEncoding(encoding, &uni_info);

        unsigned long len = actual_str[0] * 2;
        unsigned char *buff = (unsigned char *)malloc(len);
        ConvertFromPStringToUnicode(uni_info, actual_str, len, &len, (UniCharArrayPtr)buff);
        for(unsigned long x = 0; x < len; x+=2) {
#if defined(__i386__)
            fam_name += QChar(buff[x], buff[x+1]);
#else
            fam_name += QChar(buff[x+1], buff[x]);
#endif
        }
        free(buff);
        DisposeTextToUnicodeInfo(&uni_info);

        if(*it == fam_name) {
            engine->fmfam = fmfam;
            break;
        } else if(engine->fmfam == 0) {
            engine->fmfam = fmfam; //take the first one for now..
        }
    }
    { //fill in the engine's font definition
	engineData->engine->fontDef = request; //copy..
	if(engineData->engine->fontDef.pointSize == -1)
	    engineData->engine->fontDef.pointSize = qt_mac_pointsize(engineData->engine->fontDef, paintdevice);
	else
	    engineData->engine->fontDef.pixelSize = qt_mac_pixelsize(engineData->engine->fontDef, paintdevice);
	Q_ASSERT(engineData->engine->type() == QFontEngine::Mac);
	engineData->engine->fontDef.family = fam_name;
    }
    if(!engine->info) {
	engine->info = (FontInfo *)malloc(sizeof(FontInfo));
	QMacSetFontInfo fi(this, paintdevice);
	GetFontInfo(engine->info);
	engine->calculateCost();
    }

    engine->ref();
    QFontCache::instance->insertEngine(key, engine);
}

void QFont::initialize()
{
    if(!QFontCache::instance)
        new QFontCache();
    Q_CHECK_PTR(QFontCache::instance);
}

bool QFont::dirty() const
{
    return d->engineData == 0;
}

QString QFont::defaultFamily() const
{
    switch(d->request.styleHint) {
	case QFont::Times:
	    return QString::fromLatin1("Times New Roman");
	case QFont::Courier:
	    return QString::fromLatin1("Courier New");
	case QFont::Decorative:
	    return QString::fromLatin1("Bookman Old Style");
	case QFont::Helvetica:
	case QFont::System:
	default:
	    return QString::fromLatin1("Helvetica");
    }
}

QString QFont::lastResortFamily() const
{
    return QString::fromLatin1("Helvetica");
}

QString QFont::lastResortFont() const
{
    return QString::fromLatin1("Geneva");
}
