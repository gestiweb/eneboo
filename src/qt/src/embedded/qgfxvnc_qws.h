/*****************************************************************************
**
** Implementation of QGfxvnc (remote frame buffer driver)
** Proof of concept driver only.
** 
** Created : 20000703
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

#ifndef QGFXVNC_QWS_H
#define QGFXVNC_QWS_H

#if defined(Q_OS_QNX6)
#define VNCSCREEN_BASE QQnxScreen
#ifndef QT_H
#include "qwsgfx_qnx.h"
#endif // QT_H
#else
#define VNCSCREEN_BASE QLinuxFbScreen
#include "qgfxlinuxfb_qws.h"
#endif

#ifndef QT_NO_QWS_VNC

class QVNCServer;
class QVNCHeader;
class QSharedMemory;

class QVNCScreen : public VNCSCREEN_BASE {
public:
    QVNCScreen( int display_id );
    virtual ~QVNCScreen();
    virtual bool initDevice();
    virtual bool connect( const QString &displaySpec );
    virtual void disconnect();
    virtual int initCursor(void*, bool);
    virtual void shutdownDevice();
    virtual QGfx * createGfx(unsigned char *,int,int,int,int);
    virtual void save();
    virtual void restore();
    virtual void setMode(int nw,int nh,int nd);

    virtual void setDirty( const QRect& r );

    bool success;
    QVNCServer *vncServer;
    unsigned char *shmrgn;
    QSharedMemory *shm;
    QVNCHeader *hdr;
    bool virtualBuffer;
};

#endif // QT_NO_QWS_VNC

#endif // QGFXVNC_QWS_H

