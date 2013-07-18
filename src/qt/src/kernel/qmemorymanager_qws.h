/****************************************************************************
**
** Definition of QMemoryManager class
**
** Created : 000411
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#ifndef QMEMORYMANAGER_QWS_H
#define QMEMORYMANAGER_QWS_H

#ifndef QT_H
#include "qfontmanager_qws.h"
#include "qstring.h"
#include "qmap.h"
#include <private/qtextengine_p.h>
#endif // QT_H


class QFontDef;
class QMemoryManagerPixmap {
    friend class QMemoryManager;
    uchar* data;
    int xoffset;
};

class QMemoryManager {
public:
    QMemoryManager(
	void* vram, int vramsize,
	void* fontrom
	//, ...
    );

    // Pixmaps
    typedef int PixmapID;
    PixmapID newPixmap(int w, int h, int d, int optim );
    void deletePixmap(PixmapID);
    bool inVRAM(PixmapID) const;
    void findPixmap(PixmapID,
	    int width, int depth, // sames as passed when created
	    uchar** address, int* xoffset, int* linestep);

    // Fonts
    typedef void* FontID;
    FontID refFont(const QFontDef&);
    void derefFont(FontID);
    QRenderedFont* fontRenderer(FontID); // XXX JUST FOR METRICS
    bool inFont(FontID, glyph_t glyph) const;
    QGlyph lockGlyph(FontID, glyph_t glyph);
    QGlyphMetrics* lockGlyphMetrics(FontID, glyph_t glyph);
    void unlockGlyph(FontID, glyph_t glyph);
#ifndef QT_NO_QWS_SAVEFONTS
    void savePrerenderedFont(const QFontDef&, bool all=TRUE);
    void savePrerenderedFont(FontID id, bool all=TRUE);
#endif
    bool fontSmooth(FontID id) const;
    int fontAscent(FontID id) const;
    int fontDescent(FontID id) const;
    int fontMinLeftBearing(FontID id) const;
    int fontMinRightBearing(FontID id) const;
    int fontLeading(FontID id) const;
    int fontMaxWidth(FontID id) const;
    int fontUnderlinePos(FontID id) const;
    int fontLineWidth(FontID id) const;
    int fontLineSpacing(FontID id) const;

private:
    QMap<PixmapID,QMemoryManagerPixmap> pixmap_map;
    int next_pixmap_id;
    QMap<QString,FontID> font_map;
    int next_font_id;
};

extern QMemoryManager* memorymanager;

#endif // QMEMORYMANAGER_QWS_H
