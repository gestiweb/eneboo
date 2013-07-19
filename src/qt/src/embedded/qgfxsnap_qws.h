/****************************************************************************
**
** Definition of QSNAPScreen and QGfxSNAP classes for Embedded Qt
**
** Copyright (C) SciTech Software, Inc.  All rights reserved.
**
** Author:  Kendall Bennett
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
** This module requires the SciTech SNAP SDK to build and also requires
** the SciTech SNAP runtime device driver modules to be present in order
** to run.
**
** See http://www.scitechsoft.com or email sales@scitechsoft.com for
** information about SciTech SNAP Commercial License Agreements.
**
**********************************************************************/

#ifndef QGFXSNAP_QWS_H
#define QGFXSNAP_QWS_H

#ifndef QT_NO_QWS_SNAP

#include "qgfx_qws.h"
#include "snap/graphics.h"
#include "snap/ref2d.h"

/* Forward reference */
struct QGfxSNAP_State;

/*!
\class QSNAPScreen
\ingroup qws
\brief The QSNAPScreen class manages the SciTech SNAP Graphics device driver.

QSNAPScreen is a descendant of QScreen; there is precisely one per Qt/Embedded
application.
*/

class QSNAPScreen : public QScreen
{
public:
    QSNAPScreen( int display_id );
    virtual ~QSNAPScreen();

    virtual void setMode(int,int,int);
    virtual bool connect( const QString &displaySpec );
    virtual int sharedRamSize(void *);
    virtual bool initDevice();
    virtual int initCursor(void *end_of_location,bool init);
    virtual void disconnect();
    virtual void shutdownDevice();
    virtual QGfx * createGfx(unsigned char *,int,int,int,int);
    virtual void save();
    virtual void restore();
    virtual void blank(bool on);
    virtual void set(unsigned int,unsigned int,unsigned int,unsigned int);
    virtual uchar * cache(int,int);
    virtual void uncache(uchar *);

protected:
    // Note: These values are reported in *bits* rather than the expected *bytes*!!
    virtual int pixmapOffsetAlignment() { return modeInfo.BitmapStartAlign * 8; }
    virtual int pixmapLinestepAlignment() { return modeInfo.BitmapStridePad * 8; }

private:
    void fatalCleanup();
    N_uint16 findMode(int x,int y,int bits);
    int initSoftwareRasterizer();
    void delete_entry(int);
    void insert_entry(int,uint,uint);
    void setupOffScreen();

private:
    PM_HWND             hwndConsole;
    void                *stateBuf;
    int                 isServer;
    int                 useOffscreen;
    ulong               cacheStart;
    GA_devCtx           *dc;
    GA_initFuncs        init;
    GA_driverFuncs      driver;
    GA_DPMSFuncs        dpms;
    GA_2DStateFuncs     hwState2d;
    GA_2DRenderFuncs    hwDraw2d;
    GA_2DStateFuncs     state2d;
    GA_2DRenderFuncs    draw2d;
    QGfxSNAP_State      *cntState;
    REF2D_driver        *ref2d;
    int                 unloadRef2d;
    N_uint16            prevMode;
    N_uint16            cntMode;
    N_int32             maxMem;
    GA_modeInfo         modeInfo;
    N_int16             DPMSStates;
};

#endif // QT_NO_QWS_SNAP

#endif // QGFXSNAP_QWS_H

