/****************************************************************************
**
** Definition of QGfxMatrox (graphics context) class for Matrox MGA cards
**
** Created : 20020214
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

#ifndef QGFXMATROX_QWS_H
#define QGFXMATROX_QWS_H

#ifndef QT_H
#include "qgfxlinuxfb_qws.h"
#endif // QT_H

#ifndef QT_NO_QWS_MATROX

class QMatroxScreen : public QLinuxFbScreen
{
public:
    QMatroxScreen( int display_id );
    virtual ~QMatroxScreen();

    virtual bool connect( const QString &spec );
    virtual bool initDevice();
    virtual void shutdownDevice();
    virtual bool useOffscreen();
    virtual int initCursor(void*, bool);
    virtual QGfx * createGfx(unsigned char *,int,int,int,int);

protected:
    virtual int pixmapOffsetAlignment();
    virtual int pixmapLinestepAlignment();

private:
    unsigned int src_pixel_offset;
};


#endif // QT_NO_QWS_MATROX

#endif // QGFXMATROX_QWS_H
