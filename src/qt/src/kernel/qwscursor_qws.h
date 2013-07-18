/****************************************************************************
**
** Definition of QWSCursor class
**
** Created : 000101
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

#ifndef QWSCURSOR_QWS_H
#define QWSCURSOR_QWS_H

#ifndef QT_H
#include "qimage.h"
#endif // QT_H

class QWSCursor : public Qt
{
public:
    QWSCursor() {}
    QWSCursor(const uchar *data, const uchar *mask,
                int width, int height, int hotX, int hotY)
	{ set(data, mask, width, height, hotX, hotY); }

    void set(const uchar *data, const uchar *mask,
		int width, int height, int hotX, int hotY);

    QPoint hotSpot() const { return hot; }
    QImage &image() { return cursor; }
    const QRegion region() const { return rgn; }

    static QWSCursor *systemCursor(int id);

private:
    static void createSystemCursor( int id );	
    void createDropShadow(int dropx, int dropy);

private:
    QPoint hot;
    QImage cursor;
    QRegion rgn;
};

#endif // QWSCURSOR_QWS_H
