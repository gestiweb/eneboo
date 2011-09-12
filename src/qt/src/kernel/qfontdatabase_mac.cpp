/****************************************************************************
** $Id: qt/qfontdatabase_mac.cpp   3.3.8   edited Jan 11 14:38 $
**
** Implementation of platform specific QFontDatabase
**
** Created : 970521
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

#include "qt_mac.h"
#include <stdlib.h>

void QFontDatabase::createDatabase()
{
    if(db)
	return;
    db = new QFontDatabasePrivate;
    qfontdatabase_cleanup.set(&db);

    FMFontFamilyIterator it;
    QString foundry_name = "Mac";
    if(!FMCreateFontFamilyIterator(NULL, NULL, kFMUseGlobalScopeOption, &it)) {
	FMFontFamily fam;
	QString fam_name;
	while(!FMGetNextFontFamily(&it, &fam)) {
	    static Str255 n;
	    if(FMGetFontFamilyName(fam, n) != noErr)
		qDebug("Qt: internal: WH0A, %s %d", __FILE__, __LINE__);
	    if(!n[0] || n[1] == '.') //throw out ones starting with a .
		continue;
	    {
		short fnum;
		ATSUFontID fond;
		GetFNum(n, &fnum);
		if(ATSUFONDtoFontID(fnum, 0, &fond) != noErr)
		    continue;
	    }
	    TextEncoding encoding;
	    FMGetFontFamilyTextEncoding(fam, &encoding);
	    TextToUnicodeInfo uni_info;
	    CreateTextToUnicodeInfoByEncoding(encoding, &uni_info);

	    unsigned long len = n[0] * 2;
	    unsigned char *buff = (unsigned char *)malloc(len);
	    ConvertFromPStringToUnicode(uni_info, n, len, &len, (UniCharArrayPtr)buff);
	    fam_name = "";
	    for(unsigned long x = 0; x < len; x+=2) {
#if defined(__i386__)
		fam_name += QChar(buff[x], buff[x+1]);
#else
		fam_name += QChar(buff[x+1], buff[x]);
#endif
            }
	    free(buff);
	    DisposeTextToUnicodeInfo(&uni_info);

	    QtFontFamily *family = db->family( fam_name, TRUE );
            family->macFamily = fam;
	    for(int script = 0; script < QFont::LastPrivateScript; ++script)
		family->scripts[script] = QtFontFamily::Supported;
	    QtFontFoundry *foundry = family->foundry( foundry_name, TRUE );

	    FMFontFamilyInstanceIterator fit;
	    if(!FMCreateFontFamilyInstanceIterator(fam, &fit)) {
		FMFont font;
		FMFontStyle font_style;
		FMFontSize font_size;

		while(!FMGetNextFontFamilyInstance(&fit, &font, &font_style, &font_size)) {
		    bool italic = (bool)(font_style & ::italic);
		    int weight = ((font_style & ::bold) ? QFont::Bold : QFont::Normal);
		    QtFontStyle::Key styleKey;
		    styleKey.italic = italic;
		    styleKey.oblique = false;
		    styleKey.weight = weight;

		    QtFontStyle *style = foundry->style( styleKey, TRUE );
		    style->smoothScalable = TRUE;
		    if( !italic ) {
			styleKey.oblique = TRUE;
			style = foundry->style( styleKey, TRUE );
			style->smoothScalable = TRUE;
			styleKey.oblique = FALSE;
		    }
		    if(weight < QFont::DemiBold) {
			// Can make bolder
			styleKey.weight = QFont::Bold;
			if(italic) {
			    style = foundry->style( styleKey, TRUE );
			    style->smoothScalable = TRUE;
			} else {
			    styleKey.oblique = TRUE;
			    style = foundry->style( styleKey, TRUE );
			    style->smoothScalable = TRUE;
			}
		    }
		}
		FMDisposeFontFamilyInstanceIterator(&fit);
	    }
	}
	FMDisposeFontFamilyIterator(&it);
    }
}

static inline void load(const QString & = QString::null,  int = -1 )
{
}

FMFontFamily qt_macFamilyName(const QString &family)
{
    if (!db) {
        QFontDatabase db2;
    }
    QtFontFamily *f = db->family( family, FALSE );
    if (!f)
        f = db->family("Geneva", FALSE);
    return f->macFamily;
}
