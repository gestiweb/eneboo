/*****************************************************************************
**
** Implementation of QGfxRaster (unaccelerated graphics context) class for
** Embedded Qt
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

#ifndef QGFXTRANSFORMED_QWS_H
#define QGFXTRANSFORMED_QWS_H

#ifndef QT_H
#include "qgfx_qws.h"
#endif // QT_H

#ifndef QT_NO_QWS_TRANSFORMED

#ifndef Q_OS_QNX6
#define QT_TRANS_SCREEN_BASE    QLinuxFbScreen
#define QT_TRANS_CURSOR_BASE	QScreenCursor
#define QT_TRANS_GFX_BASE	QGfxRaster
//#define QT_TRANS_SCREEN_BASE  QVFbScreen
//#define QT_TRANS_CURSOR_BASE   QVFbScreenCursor
//#define QT_TRANS_GFX_BASE      QGfxVFb
#include "qgfxlinuxfb_qws.h"
#else
#define QT_TRANS_SCREEN_BASE    QQnxScreen
#include "qwsgfx_qnx.h"
#endif

class QTransformedScreen : public QT_TRANS_SCREEN_BASE
{
public:
    QTransformedScreen( int display_id );
    virtual ~QTransformedScreen();

    virtual bool connect( const QString &displaySpec );
    virtual int initCursor(void* e, bool init);
    virtual QGfx * createGfx(unsigned char *,int,int,int,int);

    enum Transformation { None, Rot90, Rot180, Rot270 };
    Transformation transformation() const { return trans; }

    virtual bool isTransformed() const;
    virtual QSize mapToDevice( const QSize & ) const;
    virtual QSize mapFromDevice( const QSize & ) const;
    virtual QPoint mapToDevice( const QPoint &, const QSize & ) const;
    virtual QPoint mapFromDevice( const QPoint &, const QSize & ) const;
    virtual QRect mapToDevice( const QRect &, const QSize & ) const;
    virtual QRect mapFromDevice( const QRect &, const QSize & ) const;
    virtual QImage mapToDevice( const QImage & ) const;
    virtual QImage mapFromDevice( const QImage & ) const;
    virtual QRegion mapToDevice( const QRegion &, const QSize & ) const;
    virtual QRegion mapFromDevice( const QRegion &, const QSize & ) const;
    virtual int transformOrientation() const;

    void setTransformation( Transformation t );

private:
    Transformation trans;
    QScreen *driver;
};

#endif // QT_NO_QWS_TRANSFORMED

#endif // QGFXTRANSFORMED_QWS_H
