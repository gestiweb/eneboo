/****************************************************************************
** $Id: qfontdatabase_win_qt4.cpp,v 1.1.2.1 2005/10/24 18:10:16 chehrlic Exp $
**
** Implementation of platform specific QFontDatabase
**
** Created : 20030709
**
** Copyright (C) 2003 Richard Lärkäng
** Copyright (C) 2003 Ivan de Jesus Deras Tabora
** Copyright (C) 2004 Ralf Habacker
** Copyright (C) 2004 Peter Kuemmel
** Copyright (C) 2004 Christian Ehrlicher
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact kde-cygwin@kde.org if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/


#ifdef QFONTDATABASE_ENABLE_QT4CODE

static HDC shared_dc; 

namespace Qt4
{

	template <typename T>
	inline const T &qMin(const T &a, const T &b) { if (a < b) return a; return b; }


	#define MAKE_TAG(ch1, ch2, ch3, ch4) (\
    (((DWORD)(ch4)) << 24) | \
    (((DWORD)(ch3)) << 16) | \
    (((DWORD)(ch2)) << 8) | \
    ((DWORD)(ch1)) \
    )

	template <typename T>
	class QGlobalStatic
	{
	public:
		T *pointer;
		inline QGlobalStatic(T *p) : pointer(p) { }
		inline ~QGlobalStatic() { pointer = 0; }
	};

	#define Q_GLOBAL_STATIC_BACKPORT(TYPE, NAME)                 \
    static TYPE *NAME()                                          \
    {                                                            \
        static TYPE this_##NAME;                                 \
        static QGlobalStatic<TYPE > global_##NAME(&this_##NAME); \
        return global_##NAME.pointer;                            \
    }

	
	Q_GLOBAL_STATIC_BACKPORT(QFontDatabasePrivate, privateDb);

	struct QFont
	{

		enum Style 
		{
			StyleNormal,
			StyleItalic,
			StyleOblique
		};

		enum Weight 
		{
			Light    = 25,
			Normal   = 50,
			DemiBold = 63,
			Bold     = 75,
			Black    = 87
		};
	};

	struct QFontDatabase
	{
		enum WritingSystem {
			Any,

			Latin,
			Greek,
			Cyrillic,
			Armenian,
			Hebrew,
			Arabic,
			Syriac,
			Thaana,
			Devanagari,
			Bengali,
			Gurmukhi,
			Gujarati,
			Oriya,
			Tamil,
			Telugu,
			Kannada,
			Malayalam,
			Sinhala,
			Thai,
			Lao,
			Tibetan,
			Myanmar,
			Georgian,
			Khmer,
			SimplifiedChinese,
			TraditionalChinese,
			Japanese,
			Korean,
			Vietnamese,

			Other,

			WritingSystemsCount
		};
	};

	struct QtFontStyle
	{
		struct Key 
		{
			Key(const QString &styleString);
			Key() : style(QFont::StyleNormal),
					weight(QFont::Normal), stretch(0) { }
			Key(const Key &o) : style(o.style),
								weight(o.weight), stretch(o.stretch) { }
			uint style : 2;
			signed int  weight : 8;
			signed int stretch : 12;

			bool operator==(const Key & other) {
				return (style == other.style &&
						weight == other.weight &&
						(stretch == 0 || other.stretch == 0 || stretch == other.stretch));
			}
			bool operator!=(const Key &other) {
				return !operator==(other);
			}
			bool operator <(const Key &o) {
				int x = (style << 12) + (weight << 14) + stretch;
				int y = (o.style << 12) + (o.weight << 14) + o.stretch;
				return (x < y);
			}

			// !Qt4
			::QtFontStyle::Key toQt3()
			{
				::QtFontStyle::Key k3;
				if(style==QFont::StyleNormal){k3.italic=0;k3.oblique=0;}
				if(style==QFont::StyleItalic){k3.italic=1;k3.oblique=0;}
				if(style==QFont::StyleOblique){k3.italic=0;k3.oblique=1;}
				k3.weight = weight;
				k3.stretch = stretch;
				return k3;
			}

			// !Qt4
			QtFontStyle::Key(::QtFontStyle::Key& k3)
			{
				if(k3.italic==0 && k3.oblique==0){style=QFont::StyleNormal;}
				if(k3.italic==1 && k3.oblique==0){style=QFont::StyleItalic;}
				if(k3.italic==0 && k3.oblique==1){style=QFont::StyleOblique;}
				weight = k3.weight;
				stretch = k3.stretch;
			}
		};

		QtFontStyle(const Key &k)
			: key(k), bitmapScalable(false), smoothScalable(false),
			fakeOblique(false), count(0), pixelSizes(0)
		{}

		// !Qt4
		QtFontStyle(::QtFontStyle &s3)
		{
			key = Key(s3.key);
			bitmapScalable = s3.bitmapScalable;
			smoothScalable = s3.smoothScalable;
			fakeOblique = s3.fakeOblique;
			count = s3.count;
			pixelSizes = s3.pixelSizes;
		}

		~QtFontStyle() 
		{
			free(pixelSizes);
		}

		Key key;
		bool bitmapScalable : 1;
		bool smoothScalable : 1;
		bool fakeOblique    : 1;
		signed int count    : 29;
		QtFontSize *pixelSizes;


		QtFontSize *pixelSize(unsigned short size, bool = false);
	};

	QtFontStyle::Key::Key(const QString &styleString)
		: style(QFont::StyleNormal), weight(QFont::Normal), stretch(0)
	{
		weight = getFontWeight(styleString);

		if (styleString.contains("Italic"))
			style = QFont::StyleItalic;
		else if (styleString.contains("Oblique"))
			style = QFont::StyleOblique;
	}

	QtFontSize *QtFontStyle::pixelSize(unsigned short size, bool add)
	{
		for (int i = 0; i < count; i++) {
			if (pixelSizes[i].pixelSize == size)
				return pixelSizes + i;
		}
		if (!add)
			return 0;

		if (!(count % 8))
			pixelSizes = (QtFontSize *)
						realloc(pixelSizes,
								(((count+8) >> 3) << 3) * sizeof(QtFontSize));
		pixelSizes[count].pixelSize = size;
	#ifdef Q_WS_X11
		pixelSizes[count].count = 0;
		pixelSizes[count].encodings = 0;
	#endif
	#ifdef Q_WS_QWS
		new (&pixelSizes[count].fileName) QByteArray;
	#endif
		return pixelSizes + (count++);
	}

	struct QtFontFamily4
	{
		enum WritingSystemStatus {
			Unknown         = 0,
			Supported       = 1,
			UnsupportedFT  = 2,
			UnsupportedXLFD = 4,
			Unsupported     = UnsupportedFT | UnsupportedXLFD
		};

		QtFontFamily4(const QString &n)		:
				fixedPitch(false),
				writingSystemCheck(false),
				name(n), count(0), foundries(0) {
				memset(writingSystems, 0, sizeof(writingSystems));
			}
			~QtFontFamily4() {
				while (count--)
					delete foundries[count];
				free(foundries);
			}

			bool fixedPitch : 1;
			bool writingSystemCheck : 1;
		
			QString name;
			QString rawName;
		
			QString english_name;

			int count;
			QtFontFoundry **foundries;

			unsigned char writingSystems[QFontDatabase::WritingSystemsCount];

			QtFontFoundry *foundry(const QString &f, bool = false);
	};


/****************************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file is part of the text module of the Qt Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qt_windows.h"
#include <private/qapplication_p.h>
//#include "qfont_p.h"
#include "qfontengine_p.h"
#include "qpaintdevice.h"

//extern HDC   shared_dc;                // common dc for all fonts
static HDC shared_dc; // !Qt4

static HFONT stock_sysfont  = 0;

// see the Unicode subset bitfields in the MSDN docs
static int requiredUnicodeBits[QFontDatabase::WritingSystemsCount][2] = {
        // Any,
    { 127, 127 },
        // Latin,
    { 0, 127 },
        // Greek,
    { 7, 127 },
        // Cyrillic,
    { 9, 127 },
        // Armenian,
    { 10, 127 },
        // Hebrew,
    { 11, 127 },
        // Arabic,
    { 13, 127 },
        // Syriac,
    { 71, 127 },
    //Thaana,
    { 72, 127 },
    //Devanagari,
    { 15, 127 },
    //Bengali,
    { 16, 127 },
    //Gurmukhi,
    { 17, 127 },
    //Gujarati,
    { 18, 127 },
    //Oriya,
    { 19, 127 },
    //Tamil,
    { 20, 127 },
    //Telugu,
    { 21, 127 },
    //Kannada,
    { 22, 127 },
    //Malayalam,
    { 23, 127 },
    //Sinhala,
    { 73, 127 },
    //Thai,
    { 24, 127 },
    //Lao,
    { 25, 127 },
    //Tibetan,
    { 70, 127 },
    //Myanmar,
    { 74, 127 },
        // Georgian,
    { 26, 127 },
        // Khmer,
    { 80, 127 },
        // SimplifiedChinese,
    { 126, 127 },
        // TraditionalChinese,
    { 126, 127 },
        // Japanese,
    { 126, 127 },
        // Korean,
    { 56, 127 },
        // Vietnamese,
    { 0, 127 }, // same as latin1
        // Other,
    { 126, 127 }
};

#define SimplifiedChineseCsbBit 18
#define TraditionalChineseCsbBit 20
#define JapaneseCsbBit 17
#define KoreanCsbBit 21

static bool localizedName(const QString &name)
{
    const QChar *c = name.unicode();
    for(int i = 0; i < name.length(); ++i) {
        if(c[i].unicode() >= 0x100)
            return true;
    }
    return false;
}

#define MAKE_TAG(ch1, ch2, ch3, ch4) (\
    (((DWORD)(ch4)) << 24) | \
    (((DWORD)(ch3)) << 16) | \
    (((DWORD)(ch2)) << 8) | \
    ((DWORD)(ch1)) \
    )

typedef short qint16;
typedef unsigned short quint16;
typedef unsigned int quint32;

static inline quint16 getUShort(unsigned char *p)
{
    quint16 val;
    val = *p++ << 8;
    val |= *p;

    return val;
}

static QString getEnglishName(const QString &familyName)
{
    QString i18n_name;

    HDC hdc = GetDC( 0 );
    HFONT hfont;
    QT_WA( {
        LOGFONTW lf;
        memset( &lf, 0, sizeof( LOGFONTW ) );
        memcpy( lf.lfFaceName, familyName.ucs2(), qMin<uint>(LF_FACESIZE, familyName.length())*sizeof(QChar) ); // !Qt4
        lf.lfCharSet = DEFAULT_CHARSET;
        hfont = CreateFontIndirectW( &lf );
    }, {
        LOGFONTA lf;
        memset( &lf, 0, sizeof( LOGFONTA ) );
        QByteArray lfam = familyName.local8Bit();						// !Qt4
        memcpy( lf.lfFaceName, lfam, qMin<uint>(LF_FACESIZE, lfam.size()) );
        lf.lfCharSet = DEFAULT_CHARSET;
        hfont = CreateFontIndirectA( &lf );
    } );
    if(!hfont) {
        ReleaseDC(0, hdc);
        return QString();
    }

    HGDIOBJ oldobj = SelectObject( hdc, hfont );

    const DWORD name_tag = MAKE_TAG( 'n', 'a', 'm', 'e' );

    enum {
        NameRecordSize = 12,
        FamilyId = 1,
        MS_LangIdEnglish = 0x009
    };

    // get the name table
    quint16 count;
    quint16 string_offset;
    unsigned char *table = 0;
    unsigned char *names;

    int microsoft_id = -1;
    int apple_id = -1;
    int unicode_id = -1;

    DWORD bytes = GetFontData( hdc, name_tag, 0, 0, 0 );
    if ( bytes == GDI_ERROR ) {
        // ### Unused variable
        /* int err = GetLastError(); */
        goto error;
    }

    table = new unsigned char[bytes];
    GetFontData(hdc, name_tag, 0, table, bytes);
    if ( bytes == GDI_ERROR )
        goto error;

    if(getUShort(table) != 0)
        goto error;

    count = getUShort(table+2);
    string_offset = getUShort(table+4);
    names = table + 6;

    if(string_offset >= bytes || 6 + count*NameRecordSize > string_offset)
        goto error;

    for(int i = 0; i < count; ++i) {
        // search for the correct name entry

        quint16 platform_id = getUShort(names + i*NameRecordSize);
        quint16 encoding_id = getUShort(names + 2 + i*NameRecordSize);
        quint16 language_id = getUShort(names + 4 + i*NameRecordSize);
        quint16 name_id = getUShort(names + 6 + i*NameRecordSize);

        if(name_id != FamilyId)
            continue;

        enum {
            PlatformId_Unicode = 0,
            PlatformId_Apple = 1,
            PlatformId_Microsoft = 3
        };

        quint16 length = getUShort(names + 8 + i*NameRecordSize);
        quint16 offset = getUShort(names + 10 + i*NameRecordSize);
        if(DWORD(string_offset + offset + length) >= bytes)
            continue;

        if ((platform_id == PlatformId_Microsoft
            && (encoding_id == 0 || encoding_id == 1))
            && (language_id & 0x3ff) == MS_LangIdEnglish
            && microsoft_id == -1)
            microsoft_id = i;
            // not sure if encoding id 4 for Unicode is utf16 or ucs4...
        else if(platform_id == PlatformId_Unicode && encoding_id < 4 && unicode_id == -1)
            unicode_id = i;
        else if(platform_id == PlatformId_Apple && encoding_id == 0 && language_id == 0)
            apple_id = i;
    }
    {
        bool unicode = false;
        int id = -1;
        if(microsoft_id != -1) {
            id = microsoft_id;
            unicode = true;
        } else if(apple_id != -1) {
            id = apple_id;
            unicode = false;
        } else if (unicode_id != -1) {
            id = unicode_id;
            unicode = true;
        }
        if(id != -1) {
            quint16 length = getUShort(names + 8 + id*NameRecordSize);
            quint16 offset = getUShort(names + 10 + id*NameRecordSize);
            if(unicode) {
                // utf16

                length /= 2;
                //i18n_name.resize(length);
				i18n_name.setUnicode(0,length);	// !Qt4
                QChar *uc = (QChar *) i18n_name.unicode();
                unsigned char *string = table + string_offset + offset;
                for(int i = 0; i < length; ++i)
                    uc[i] = getUShort(string + 2*i);
            } else {
                // Apple Roman

                //i18n_name.resize(length);
				i18n_name.setUnicode(0,length);	// !Qt4
                QChar *uc = (QChar *) i18n_name.unicode();
                unsigned char *string = table + string_offset + offset;
                for(int i = 0; i < length; ++i)
                    uc[i] = string[i];
            }
        }
    }
  error:
    delete [] table;
    SelectObject( hdc, oldobj );
    DeleteObject( hfont );
    ReleaseDC( 0, hdc );

    //qDebug("got i18n name of '%s' for font '%s'", i18n_name.latin1(), familyName.toLocal8Bit().data());
    return i18n_name;
}


static
int CALLBACK
#ifndef Q_OS_TEMP
storeFont(ENUMLOGFONTEX* f, NEWTEXTMETRICEX *textmetric, int type, LPARAM /*p*/)
#else
storeFont(ENUMLOGFONTEX* f, NEWTEXTMETRIC *textmetric, int type, LPARAM /*p*/)
#endif
{
    const int script = -1;
    const QString foundryName;
    const bool smoothScalable = true;
    Q_UNUSED(script);
    Q_UNUSED(smoothScalable);

    bool italic = false;
    QString familyName;
    int weight;
    bool fixed;
    bool ttf;

    // ### make non scalable fonts work

    QT_WA({
        familyName = QString::fromUcs2((ushort*)f->elfLogFont.lfFaceName);	//Qt4
        italic = f->elfLogFont.lfItalic;
        weight = f->elfLogFont.lfWeight;
        TEXTMETRIC *tm = (TEXTMETRIC *)textmetric;
        fixed = !(tm->tmPitchAndFamily & TMPF_FIXED_PITCH);
        ttf = (tm->tmPitchAndFamily & TMPF_TRUETYPE);
    } , {
        ENUMLOGFONTEXA* fa = (ENUMLOGFONTEXA *)f;
        familyName = QString::fromLocal8Bit(fa->elfLogFont.lfFaceName);
        italic = fa->elfLogFont.lfItalic;
        weight = fa->elfLogFont.lfWeight;
        TEXTMETRICA *tm = (TEXTMETRICA *)textmetric;
        fixed = !(tm->tmPitchAndFamily & TMPF_FIXED_PITCH);
        ttf = (tm->tmPitchAndFamily & TMPF_TRUETYPE);
    });
    // the "@family" fonts are just the same as "family". Ignore them.
    if (familyName[0] != '@') {
        QtFontStyle::Key styleKey;
        styleKey.style = italic ? QFont::StyleItalic : QFont::StyleNormal;
        if (weight < 400)
            styleKey.weight = QFont::Light;
        else if (weight < 600)
            styleKey.weight = QFont::Normal;
        else if (weight < 700)
            styleKey.weight = QFont::DemiBold;
        else if (weight < 800)
            styleKey.weight = QFont::Bold;
        else
            styleKey.weight = QFont::Black;

        QString rawName = familyName;
        familyName.replace('-', ' ');
        QtFontFamily *family = privateDb()->family(familyName, true);
        family->rawName = rawName;

        if(ttf && localizedName(familyName) && family->english_name.isEmpty())
            family->english_name = getEnglishName(familyName);

        QtFontFoundry *foundry = family->foundry(foundryName, true);
		QtFontStyle *style = &QtFontStyle(*foundry->style(styleKey.toQt3(), true)); // !Qt4
        style->smoothScalable = true;
        style->pixelSize(SMOOTH_SCALABLE, true);

        // add fonts windows can generate for us:
        if (styleKey.weight <= QFont::DemiBold) {
            QtFontStyle::Key key(styleKey);
            key.weight = QFont::Bold;
            QtFontStyle *style = &QtFontStyle(*foundry->style(key.toQt3(), true)); // !Qt4
            style->smoothScalable = true;
            style->pixelSize(SMOOTH_SCALABLE, true);
        }
        if (styleKey.style != QFont::StyleItalic) {
            QtFontStyle::Key key(styleKey);
            key.style = QFont::StyleItalic;
            QtFontStyle *style = &QtFontStyle(*foundry->style(key.toQt3(), true)); // !Qt4
            style->smoothScalable = true;
            style->pixelSize(SMOOTH_SCALABLE, true);
        }
        if (styleKey.weight <= QFont::DemiBold && styleKey.style != QFont::StyleItalic) {
            QtFontStyle::Key key(styleKey);
            key.weight = QFont::Bold;
            key.style = QFont::StyleItalic;
            QtFontStyle *style = &QtFontStyle(*foundry->style(key.toQt3(), true)); // !Qt4
            style->smoothScalable = true;
            style->pixelSize(SMOOTH_SCALABLE, true);
        }

        family->fixedPitch = fixed;

        if (!family->writingSystemCheck && type & TRUETYPE_FONTTYPE) {
            bool hasScript = false;
            FONTSIGNATURE signature;
#ifndef Q_OS_TEMP
            QT_WA({
                signature = textmetric->ntmFontSig;
            }, {
                // the textmetric structure we get from EnumFontFamiliesEx on Win9x has
                // a FONTSIGNATURE, but that one is uninitialized and doesn't work. Have to go
                // the hard way and load the font to find out.
                HDC hdc = GetDC(0);
                LOGFONTA lf;
                memset(&lf, 0, sizeof(LOGFONTA));
                QByteArray lfam = familyName.local8Bit();	// !Qt4
                memcpy(lf.lfFaceName, lfam.data(), qMin<uint>(LF_FACESIZE, lfam.size())); // !Qt4
                lf.lfCharSet = DEFAULT_CHARSET;
                HFONT hfont = CreateFontIndirectA(&lf);
                HGDIOBJ oldobj = SelectObject(hdc, hfont);
                GetTextCharsetInfo(hdc, &signature, 0);
                SelectObject(hdc, oldobj);
                DeleteObject(hfont);
                ReleaseDC(0, hdc);
            });
#else
            CHARSETINFO csi;
            DWORD charset = textmetric->tmCharSet;
            TranslateCharsetInfo(&charset, &csi, TCI_SRCCHARSET);
            signature = csi.fs;
#endif

            int i;
            for(i = 0; i < QFontDatabase::WritingSystemsCount; i++) {
                int bit = requiredUnicodeBits[i][0];
                int index = bit/32;
                int flag =  1 << (bit&31);
                if (bit != 126 && signature.fsUsb[index] & flag) {
                    bit = requiredUnicodeBits[i][1];
                    index = bit/32;

                    flag =  1 << (bit&31);
                    if (bit == 127 || signature.fsUsb[index] & flag) {
                        family->writingSystems[i] = QtFontFamily::Supported;
                        hasScript = true;
                        // qDebug("font %s: index=%d, flag=%8x supports script %d", familyName.latin1(), index, flag, i);
                    }
                }
            }
            if(signature.fsCsb[0] & (1 << SimplifiedChineseCsbBit)) {
                family->writingSystems[QFontDatabase::SimplifiedChinese] = QtFontFamily::Supported;
                hasScript = true;
                //qDebug("font %s supports Simplified Chinese", familyName.latin1());
            }
            if(signature.fsCsb[0] & (1 << TraditionalChineseCsbBit)) {
                family->writingSystems[QFontDatabase::TraditionalChinese] = QtFontFamily::Supported;
                hasScript = true;
                //qDebug("font %s supports Traditional Chinese", familyName.latin1());
            }
            if(signature.fsCsb[0] & (1 << JapaneseCsbBit)) {
                family->writingSystems[QFontDatabase::Japanese] = QtFontFamily::Supported;
                hasScript = true;
                //qDebug("font %s supports Japanese", familyName.latin1());
            }
            if(signature.fsCsb[0] & (1 << KoreanCsbBit)) {
                family->writingSystems[QFontDatabase::Korean] = QtFontFamily::Supported;
                hasScript = true;
                //qDebug("font %s supports Korean", familyName.latin1());
            }
#ifdef Q_OS_TEMP
            // ##### FIXME
            family->writingSystems[QFontDatabase::Latin] = QtFontFamily::Supported;
#endif
            if (!hasScript)
                family->writingSystems[QFontDatabase::Other] = QtFontFamily::Supported;
            family->writingSystemCheck = true;
            // qDebug("usb=%08x %08x csb=%08x for %s", signature.fsUsb[0], signature.fsUsb[1], signature.fsCsb[0], familyName.latin1());
        } else if (!family->writingSystemCheck) {
            family->writingSystems[QFontDatabase::Other] = QtFontFamily::Supported;
        }
    }

    // keep on enumerating
    return 1;
}

static
void populate_database(const QString& fam)
{
    HDC dummy = GetDC(0);

#ifndef Q_OS_TEMP
    QT_WA({
        LOGFONT lf;
        lf.lfCharSet = DEFAULT_CHARSET;
        if (fam.isNull()) {
            lf.lfFaceName[0] = 0;
        } else {
            memcpy(lf.lfFaceName, fam.ucs2(), sizeof(TCHAR)*qMin<uint>(fam.length()+1,32));  // 32 = Windows hard-coded // !QT4
        }
        lf.lfPitchAndFamily = 0;

        EnumFontFamiliesEx(dummy, &lf,
            (FONTENUMPROC)storeFont, (LPARAM)privateDb(), 0);
    } , {
        LOGFONTA lf;
        lf.lfCharSet = DEFAULT_CHARSET;
        if (fam.isNull()) {
            lf.lfFaceName[0] = 0;
        } else {
            QByteArray lname = fam.local8Bit(); // !QT4
            memcpy(lf.lfFaceName,lname.data(),
                qMin<uint>(lname.size()+1,32));  // 32 = Windows hard-coded
        }
        lf.lfPitchAndFamily = 0;

        EnumFontFamiliesExA(dummy, &lf,
            (FONTENUMPROCA)storeFont, (LPARAM)privateDb(), 0);
    });
#else
        LOGFONT lf;
        lf.lfCharSet = DEFAULT_CHARSET;
        if (fam.isNull()) {
            lf.lfFaceName[0] = 0;
        } else {
            memcpy(lf.lfFaceName, fam.utf16(), sizeof(TCHAR)*qMin(fam.length()+1,32));  // 32 = Windows hard-coded
        }
        lf.lfPitchAndFamily = 0;

        EnumFontFamilies(dummy, lf.lfFaceName,
            (FONTENUMPROC)storeFont, (LPARAM)db);
#endif


    ReleaseDC(0, dummy);
}

static void initializeDb()
{
	shared_dc = GetDC( NULL ); // !Qt4
	::shared_dc = shared_dc;

    //QFontDatabasePrivate *db = privateDb();
	db = privateDb();		// !Qt4
    if (!db || db->count)
        return;

    populate_database(QString());

#ifdef QFONTDATABASE_DEBUG
    // print the database
    for (int f = 0; f < db->count; f++) {
        QtFontFamily *family = db->families[f];
        qDebug("    %s: %p", family->name.latin1(), family);
        populate_database(family->name);

        qDebug("        scripts supported:");
        for (int i = 0; i < QUnicodeTables::ScriptCount; i++)
            if(family->writingSystems[i] & QtFontFamily::Supported)
                qDebug("            %d", i);
        for (int fd = 0; fd < family->count; fd++) {
            QtFontFoundry *foundry = family->foundries[fd];
            qDebug("        %s", foundry->name.latin1());
            for (int s = 0; s < foundry->count; s++) {
                QtFontStyle *style = &QtFontStyle(foundry->styles[s]);
                qDebug("            style: style=%d weight=%d", style->key.style, style->key.weight);
            }
        }
    }
#endif // QFONTDATABASE_DEBUG

}

static inline void load(const QString &family = QString(), int = -1)
{
    populate_database(family);
}





// --------------------------------------------------------------------------------------
// font loader
// --------------------------------------------------------------------------------------



#if 0
void QFontPrivate::initFontInfo()
{
    lineWidth = 1;
    actual = request;                                // most settings are equal
    QT_WA({
        TCHAR n[64];
        GetTextFaceW(fin->dc(), 64, n);
        actual.family = QString::fromUtf16((ushort*)n);
        actual.fixedPitch = !(fin->tm.w.tmPitchAndFamily & TMPF_FIXED_PITCH);
    } , {
        char an[64];
        GetTextFaceA(fin->dc(), 64, an);
        actual.family = QString::fromLocal8Bit(an);
        actual.fixedPitch = !(fin->tm.a.tmPitchAndFamily & TMPF_FIXED_PITCH);
    });
    if (actual.pointSize < 0) {
        if (paintdevice)
            actual.pointSize = actual.pixelSize * 72. / paintdevice->logicalDpiY();
        else {
            actual.pointSize = actual.pixelSize * 72. / GetDeviceCaps(fin->dc(), LOGPIXELSY);
        }
    } else if (actual.pixelSize == -1) {
        if (paintdevice)
            actual.pixelSize = actual.pointSize * paintdevice->logicalDpiY() / 72.;
        else
            actual.pixelSize = actual.pointSize * GetDeviceCaps(fin->dc(), LOGPIXELSY) / 72.;
    }

    actual.dirty = false;
    exactMatch = (actual.family == request.family &&
                   (request.pointSize < 0 || (actual.pointSize == request.pointSize)) &&
                   (request.pixelSize == -1 || (actual.pixelSize == request.pixelSize)) &&
                   actual.fixedPitch == request.fixedPitch);
}

#endif


static const char *other_tryFonts[] = {
    "Arial",
    "MS Mincho",
    "Batang",
    "SimSun",
    "MingLiU",
    "Arial Unicode MS",
    0
};

static const char *jp_tryFonts [] = {
    "MS Mincho",
    "Arial",
    "Batang",
    "SimSun",
    "MingLiU",
    "Arial Unicode MS",
    0
};

static const char *ch_CN_tryFonts [] = {
    "SimSun",
    "Arial",
    "MingLiU",
    "Batang",
    "MS Mincho",
    "Arial Unicode MS",
    0
};

static const char *ch_TW_tryFonts [] = {
    "MingLiU",
    "Arial",
    "SimSun",
    "Batang",
    "MS Mincho",
    "Arial Unicode MS",
    0
};

static const char *kr_tryFonts[] = {
    "Batang",
    "Arial",
    "MingLiU",
    "SimSun",
    "MS Mincho",
    "Arial Unicode MS",
    0
};

static const char **tryFonts = 0;


static inline HFONT systemFont()
{
    if (stock_sysfont == 0)
        stock_sysfont = (HFONT)GetStockObject(SYSTEM_FONT);
    return stock_sysfont;
}

#if !defined(DEFAULT_GUI_FONT)
#define DEFAULT_GUI_FONT 17
#endif

static
QFontEngine *loadEngine(int script, const QFontPrivate *fp,
                        const QFontDef &request,
                        QtFontFamily *family, QtFontFoundry *foundry,
                        QtFontStyle *style)
{
    Q_UNUSED(script);
    Q_UNUSED(foundry);
    Q_UNUSED(style);

    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT));

    HDC hdc = shared_dc;

    bool stockFont = false;

    HFONT hfont = 0;

    if (fp->rawMode) {                        // will choose a stock font
        int f, deffnt;
        // ### why different?
		// !QT4 if ((QSysInfo::WindowsVersion & QSysInfo::WV_NT_based) || QSysInfo::WindowsVersion == QSysInfo::WV_32s)
            deffnt = SYSTEM_FONT;
		// !QT4 else
        // !QT4    deffnt = DEFAULT_GUI_FONT;
        QString fam = family->rawName.lower(); // !QT4
        if (fam == "default")
            f = deffnt;
        else if (fam == "system")
            f = SYSTEM_FONT;
#ifndef Q_OS_TEMP
        else if (fam == "system_fixed")
            f = SYSTEM_FIXED_FONT;
        else if (fam == "ansi_fixed")
            f = ANSI_FIXED_FONT;
        else if (fam == "ansi_var")
            f = ANSI_VAR_FONT;
        else if (fam == "device_default")
            f = DEVICE_DEFAULT_FONT;
        else if (fam == "oem_fixed")
            f = OEM_FIXED_FONT;
#endif
        else if (fam[0] == '#')
            f = fam.right(fam.length()-1).toInt();
        else
            f = deffnt;
        hfont = (HFONT)GetStockObject(f);
        if (!hfont) {
            qWarning("QFontEngine::loadEngine: GetStockObject failed"); // !QT4
            hfont = systemFont();
        }
        stockFont = true;
    } else {

        int hint = FF_DONTCARE;
        switch (request.styleHint) {
			case ::QFont::Helvetica:	// !QT4 
                hint = FF_SWISS;
                break;
			case ::QFont::Times:		// !Qt4
                hint = FF_ROMAN;
                break;
			case ::QFont::Courier:		// !Qt4
                hint = FF_MODERN;
                break;
			case ::QFont::OldEnglish:	// !Qt4
                hint = FF_DECORATIVE;
                break;
			case ::QFont::System:		// !Qt4
                hint = FF_MODERN;
                break;
            default:
                break;
        }

        lf.lfHeight = -request.pixelSize;
#ifdef Q_OS_TEMP
        lf.lfHeight                += 3;
#endif
        lf.lfWidth                = 0;
        lf.lfEscapement        = 0;
        lf.lfOrientation        = 0;
        if (style->key.weight == 50)
            lf.lfWeight = FW_DONTCARE;
        else
            lf.lfWeight = (style->key.weight*900)/99;
        lf.lfItalic                = (style->key.style != QFont::StyleNormal);
        lf.lfCharSet        = DEFAULT_CHARSET;

        int strat = OUT_DEFAULT_PRECIS;
		if ( request.styleStrategy & ::QFont::PreferBitmap) {			// !Qt4
            strat = OUT_RASTER_PRECIS;
#ifndef Q_OS_TEMP
		} else if (request.styleStrategy & ::QFont::PreferDevice) {		// !Qt4
            strat = OUT_DEVICE_PRECIS;
		} else if (request.styleStrategy & ::QFont::PreferOutline) {	// !Qt4
            QT_WA({
                strat = OUT_OUTLINE_PRECIS;
            } , {
                strat = OUT_TT_PRECIS;
            });
		} else if (request.styleStrategy & ::QFont::ForceOutline) {		// !Qt4
            strat = OUT_TT_ONLY_PRECIS;
#endif
        }

        lf.lfOutPrecision   = strat;

        int qual = DEFAULT_QUALITY;

		if (request.styleStrategy & ::QFont::PreferMatch)			// !Qt4
            qual = DRAFT_QUALITY;
#ifndef Q_OS_TEMP
		else if (request.styleStrategy & ::QFont::PreferQuality)		// !Qt4
            qual = PROOF_QUALITY;
#endif

		if (request.styleStrategy & ::QFont::PreferAntialias) {			// !Qt4
            // !Qt4 if (QSysInfo::WindowsVersion >= QSysInfo::WV_XP)			
                // !Qt4 qual = 5; // == CLEARTYPE_QUALITY;
            // !Qt4 else
                qual = ANTIALIASED_QUALITY;
		} else if (request.styleStrategy & ::QFont::NoAntialias) {		// !Qt4
            qual = NONANTIALIASED_QUALITY;
        }

        lf.lfQuality        = qual;

        lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
        lf.lfPitchAndFamily = DEFAULT_PITCH | hint;

        QString fam = family->rawName;
        if (fam.isEmpty()) {
            switch ( request.styleHint ) {
		case ::QFont::SansSerif:	// !Qt4
		fam = "MS Sans Serif";
		break;
		case ::QFont::Serif:		// !Qt4
		fam = "Times New Roman";
		break;
		case ::QFont::TypeWriter:	// !Qt4
		fam = "Courier New";
		break;
	    default:
		break;
            }
        }

        if ((fam == "MS Sans Serif")
            && (request.style == QFont::StyleItalic || (-lf.lfHeight > 18 && -lf.lfHeight != 24))) {
            fam = "Arial"; // MS Sans Serif has bearing problems in italic, and does not scale
        }
        QT_WA({
            memcpy(lf.lfFaceName, fam.ucs2(), sizeof(TCHAR)*qMin<uint>(fam.length()+1,32));  // 32 = Windows hard-coded // !Qt4
            hfont = CreateFontIndirect(&lf);
        } , {
            // LOGFONTA and LOGFONTW are binary compatible
            QByteArray lname = fam.local8Bit();		// !Qt4
            memcpy(lf.lfFaceName,lname.data(),
                qMin<uint>(lname.size()+1,32));  // 32 = Windows hard-coded  // !Qt4
            hfont = CreateFontIndirectA((LOGFONTA*)&lf);
        });
        if (!hfont)
            qWarning("QFontEngine::loadEngine: CreateFontIndirect failed");		// !Qt4

        stockFont = (hfont == 0);

        if (hfont && request.stretch != 100) {
            HGDIOBJ oldObj = SelectObject(hdc, hfont);
            BOOL res;
            int avWidth = 0;
            QT_WA({
                TEXTMETRICW tm;
                res = GetTextMetricsW(hdc, &tm);
                avWidth = tm.tmAveCharWidth;
            } , {
                TEXTMETRICA tm;
                res = GetTextMetricsA(hdc, &tm);
                avWidth = tm.tmAveCharWidth;
            });
            if (!res)
                qWarning("QFontEngine::loadEngine: GetTextMetrics failed");	// !Qt4

            SelectObject(hdc, oldObj);
            DeleteObject(hfont);

            lf.lfWidth = avWidth * request.stretch/100;
            QT_WA({
                hfont = CreateFontIndirect(&lf);
            } , {
                hfont = CreateFontIndirectA((LOGFONTA*)&lf);
            });
            if (!hfont)
                qWarning("QFontEngine::loadEngine: CreateFontIndirect with stretch failed");	// !Qt4
        }

#ifndef Q_OS_TEMP
        if (hfont == 0) {
            hfont = (HFONT)GetStockObject(ANSI_VAR_FONT);
            stockFont = true;
        }
#endif

    }
    QFontEngine *fe = new QFontEngineWin(family->name.latin1(), shared_dc, hfont, stockFont, lf);	// !Qt4
#if 0 // !Qt4
	if(script == QUnicodeTables::Common) 
	{
        if(!tryFonts) 
		{
	    LANGID lid = GetUserDefaultLangID();
	    switch( lid&0xff ) 
		{
			case LANG_CHINESE: // Chinese (Taiwan)
				if ( lid == 0x0804 ) // Taiwan
				tryFonts = ch_TW_tryFonts;
				else
	    			tryFonts = ch_CN_tryFonts;
				break;
			case LANG_JAPANESE:
	    		tryFonts = jp_tryFonts;
					break;
				case LANG_KOREAN:
					tryFonts = kr_tryFonts;
					break;
			default:
					tryFonts = other_tryFonts;
				break;
		}
		}
        QStringList fm = QFontDatabase().families();
        QStringList list;
        const char **tf = tryFonts;
        while(tf && *tf) {
            if(fm.contains(QLatin1String(*tf)))
                list << QLatin1String(*tf);
            ++tf;
        }
        fe = new QFontEngineMultiWin(static_cast<QFontEngineWin *>(fe), list);
    }
	#endif 0 // !Qt4
    return fe;
}

}// namespace Qt4



static inline void load(const QString &family = QString(), int = -1)
{
	Qt4::populate_database(family);
}

void initializeDb()
{
	Qt4::initializeDb();
}


QFontEngine *loadEngine(int script, const QFontPrivate *fp,
                        const QFontDef &request,
                        QtFontFamily *family, QtFontFoundry *foundry,
                        QtFontStyle *style)
{
	return Qt4::loadEngine(script, fp,
                        request,
                        family, foundry,
						&Qt4::QtFontStyle(*style));
}

void QFontDatabase::createDatabase()
{
    if ( db )
        return ;
    initializeDb();
}

#endif
