/*
    Copyright (C) 2001-2002 by theKompany.com <www.thekompany.com>
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    By PLUGINS we mean the commercial plug-ins developed by theKompany.com
    for Aethera.

    In addition, as a special exception, theKompany.com gives permission
    to link the code of this program with PLUGINS (or with
    modified versions of PLUGINS that use the same license as PLUGINS),
    and distribute linked combinations including the two.  You must obey
    the GNU General Public License in all respects for all of the code used
    other than PLUGINS.  If you modify this file, you may extend this
    exception to your version of the file, but you are not obligated to do so.
    If you do not wish to do so, delete this exception statement from your
    version.

    This license grants you the ability to use PLUGINS with Aethera only
    and may not be used outside of Aethera.
    See also http://www.thekompany.com/products/license.txt for details.
*/


// These arrays are from KDE, kcharsets.cpp
// I couldn't use the original files, so I am using only the arrays

#include "charsets.h"

#include <qregexp.h>
#include <qtextcodec.h>

#define CHARSETS_COUNT    32

CharSets* CharSets::instance = 0;

static const char * const charsetsStr[CHARSETS_COUNT] = {
    "unicode",
    "iso-8859-1",
    "iso-8859-2",
    "iso-8859-3",
    "iso-8859-4",
    "iso-8859-5",
    "iso-8859-6",
    "iso-8859-7",
    "iso-8859-8",
    "iso-8859-9",
    "iso-8859-10",
    "iso-8859-11",
    "iso-8859-12",
    "iso-8859-13",
    "iso-8859-14",
    "iso-8859-15",
    "koi8r",
    "koi8u",
    "cp 1251",
    "pt 154",
    "eucjp",
    "euckr",
    "set-th-th",
    "set-gbk",
    "set-zh",
    "set-zh-tw",
    "big5",
    "tscii",
    "utf-8",
    "utf-16",
    "iso-8859-11",
    "Any"
};

static const char * const xNames[CHARSETS_COUNT] = {
    "iso10646-1",
    "iso8859-1",
    "iso8859-2",
    "iso8859-3",
    "iso8859-4",
    "iso8859-5",
    "iso8859-6",
    "iso8859-7",
    "iso8859-8",
    "iso8859-9",
    "iso8859-10",
    "iso8859-11",
    "iso8859-12",
    "iso8859-13",
    "iso8859-14",
    "iso8859-15",
    "koi8-r",
    "koi8-u",
    "microsoft-cp1251",
    "paratype-cp154",
    "jisx0208.1983-0",
    "ksc5601.1987-0",
    "tis620.2533-1",
    "gb2312.1980-0",
    "gb2312.1980-0",
    "cns11643.1986-",
    "big5-0",
    "tscii-0",
    "utf8",
    "utf16",
    "tis620-0",
    ""
};

CharSets::CharSets() {}

CharSets::~CharSets() {}

CharSets* CharSets::thisInstance()
{
    return instance ? instance : instance = new CharSets();
}

QString CharSets::fromQTCodec(const QString &qtCodec)
{
    CodecsMap::ConstIterator found = codecs.find(qtCodec);
    if ( found!=codecs.end() )
        return found.data();

    for (int i=0; i < CHARSETS_COUNT; i++) {
        if ( qtCodec==charsetsStr[i] ) {
            QString codec = charsetsStr[i];
            codecs[qtCodec] = codec;
            return codec;
        }
    }

    {
        for (int i=0; i < CHARSETS_COUNT-1; i++) {
            if ( qtCodec==xNames[i] ) {
                QString codec = charsetsStr[i];
                codecs[qtCodec]=codec;
                return codec;
            }
        }
    }

    return qtCodec;
}

QString CharSets::getDefaultCharset()
{
    QTextCodec * codec = QTextCodec::codecForLocale();
    if (codec) {
        QCString mimeName = codec->mimeName();

        // fix for french people by fabien armand <far@atolcd.com>
        // it appears only on Windows, more fixes Eugen C.
        // but the safest will be to use utf-8 !!
        if ( "System" == mimeName ) {
            QCString locale = codec->locale();
            locale = locale.left(2);
            if ( "en" == locale ||
                    "da" == locale ||
                    "de" == locale ||
                    "fr" == locale ||
                    "fi" == locale ||
                    "it" == locale ||
                    "is" == locale ||
                    "nb" == locale ||
                    "nl" == locale ||
                    "pt" == locale ||
                    "es" == locale ||
                    "sv" == locale ) {
                return "ISO-8859-1";
            } else if ( "ro" == locale ||
                        "sk" == locale ||
                        "hr" == locale ||
                        "hu" == locale ||
                        "cs" == locale ||
                        "pl" == locale ||
                        "sl" == locale ) {
                return "ISO-8859-2";
            }
        }
        // fix end

        return mimeName;
    } else {
        return QString("us-ascii");
    }

    //  QString mainName = QTextCodec::codecForLocale()->name();
    //
    //  // remove all white spaces
    //  mainName.replace(QRegExp("\\s"), "");
    //  // to lower
    //  mainName = mainName.lower();
    //  // fix the name for codec
    //  mainName = fromQTCodec(mainName);
    //
    //  return mainName;
}

QTextCodec * CharSets::getCodec(const QString & text)
{
    QTextCodec * codec = QTextCodec::codecForLocale();
    if (codec && codec->canEncode(text)) {
        return codec;
    }

    //  QT 3.2.3 is broken :(
    //  for(int i=0; (codec = QTextCodec::codecForIndex(i)); i++)
    //  {
    //    qDebug("codec = %d", i);
    //
    //    if( codec->canEncode(text) )
    //    {
    //      return codec;
    //    }
    //  }

    return 0;
}

bool CharSets::isRFC822(const QCString & text)
{
    // ASCII 7 bit
    const char * data = text.data();
    unsigned uLength = text.length();

    for (unsigned i=0; i<uLength; i++) {
        if ( data[i] & 128 ) {
            return false;
        }
    }

    return true;
}
