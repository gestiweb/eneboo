/****************************************************************************
**
** Implementation of QGfx (graphics context) class
**
** Created : 990721
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
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
#include "qgfx_qws.h"

#include <stdio.h>
#ifdef QT_LOADABLE_MODULES
#include <dlfcn.h>
#endif
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

#ifndef QT_NO_QWS_CURSOR
bool qt_sw_cursor=FALSE;
QScreenCursor * qt_screencursor=0;
#endif
QScreen * qt_screen=0;

extern bool qws_screen_is_interlaced; //### hack, from qapplication_qws.cpp

ClearCacheFunc QScreen::clearCacheFunc = 0;

QGfx *QGfx::createGfx( int depth, unsigned char *buffer, int w, int h,
			     int offs )
{
    return qt_screen->createGfx( buffer, w, h, depth, offs );
}

void QGfx::setSource(unsigned char *,int,int,int,int,QRgb *,int)
{
}

void QGfx::setScreen(QScreen *,QScreenCursor *,bool,int *,int *)
{
}

/*!
    \fn virtual int QScreen::sharedRamSize(void *)

    \internal
*/

/*!
    \fn int * QScreen::opType()

    Returns the screen's operation type.
*/

/*!
    \fn int * QScreen::lastOp()

    Returns the screens last operation.
*/

/*!
    \fn QScreen::setDirty( const QRect& )

    Indicates which section of the screen has been altered. Used by
    the VNC and VFB displays; the QScreen version does nothing.
*/

void QScreen::setDirty( const QRect& )
{
}

/*!
    \fn QScreen::isTransformed() const

    Returns TRUE if the screen is transformed (for instance, rotated
    90 degrees); otherwise returns FALSE. QScreen's version always
    returns FALSE.
*/

bool QScreen::isTransformed() const
{
    return FALSE;
}

/*!
    \fn QScreen::isInterlaced() const

    Returns TRUE if the display is interlaced (for instance a
    television screen); otherwise returns FALSE. If TRUE, drawing is
    altered to look better on such displays.
*/

bool QScreen::isInterlaced() const
{
    return qws_screen_is_interlaced;;
}

/*!
    \fn QScreen::mapToDevice( const QSize &s ) const

    Map a user coordinate to the one to actually draw. Used by the
    rotated driver; the QScreen implementation simply returns \a s.
*/

QSize QScreen::mapToDevice( const QSize &s ) const
{
    return s;
}

/*!
    \fn QScreen::mapFromDevice( const QSize &s ) const

    Map a framebuffer coordinate to the coordinate space used by the
    application. Used by the rotated driver; the QScreen
    implementation simply returns \a s.
*/

QSize QScreen::mapFromDevice( const QSize &s ) const
{
    return s;
}

/*!
    \overload QScreen::mapToDevice( const QPoint &, const QSize & ) const

    Map a user coordinate to the one to actually draw. Used by the
    rotated driver; the QScreen implementation simply returns the
    point passed in.
*/

QPoint QScreen::mapToDevice( const QPoint &p, const QSize & ) const
{
    return p;
}

/*!
    \overload QScreen::mapFromDevice( const QPoint &, const QSize & ) const

    Map a framebuffer coordinate to the coordinate space used by the
    application. Used by the rotated driver; the QScreen
    implementation simply returns the point.
*/

QPoint QScreen::mapFromDevice( const QPoint &p, const QSize & ) const
{
    return p;
}

/*!
    \overload QScreen::mapToDevice( const QRect &r, const QSize & ) const

    Map a user coordinate to the one to actually draw. Used by the
    rotated driver; the QScreen implementation simply returns \a r.
*/

QRect QScreen::mapToDevice( const QRect &r, const QSize & ) const
{
    return r;
}

/*!
    \overload QScreen::mapFromDevice( const QRect &r, const QSize & ) const

    Map a framebuffer coordinate to the coordinate space used by the
    application. Used by the rotated driver; the QScreen
    implementation simply returns \a r.
*/

QRect QScreen::mapFromDevice( const QRect &r, const QSize & ) const
{
    return r;
}

/*!
    \overload QScreen::mapToDevice( const QImage &i ) const

    Transforms an image so that it fits the device coordinate space
    (e.g. rotating it 90 degrees clockwise). The QScreen
    implementation simply returns \a i.
*/

QImage QScreen::mapToDevice( const QImage &i ) const
{
    return i;
}

/*!
    \overload QScreen::mapFromDevice( const QImage &i ) const

    Transforms an image so that it matches the application coordinate
    space (e.g. rotating it 90 degrees counter-clockwise). The QScreen
    implementation simply returns \a i.
*/

QImage QScreen::mapFromDevice( const QImage &i ) const
{
    return i;
}

/*!
    \overload QScreen::mapToDevice( const QRegion &r, const QSize & ) const

    Transforms a region so that it fits the device coordinate space
    (e.g. rotating it 90 degrees clockwise). The QScreen
    implementation simply returns \a r.
*/

QRegion QScreen::mapToDevice( const QRegion &r, const QSize & ) const
{
    return r;
}

/*!
    \overload QScreen::mapFromDevice( const QRegion &r, const QSize & ) const

    Transforms a region so that it matches the application coordinate
    space (e.g. rotating it 90 degrees counter-clockwise). The QScreen
    implementation simply returns \a r.
*/

QRegion QScreen::mapFromDevice( const QRegion &r, const QSize & ) const
{
    return r;
}

/*!
    \fn QScreen::transformOrientation() const

    Used by the rotated server. The QScreeen implementation returns 0.
*/

int QScreen::transformOrientation() const
{
    return 0;
}

int QScreen::pixmapDepth() const
{
    return depth();
}

/*!
    \internal
*/
int QScreen::memoryNeeded(const QString&)
{
    return 0;
}

/*!
    \internal
*/
void QScreen::haltUpdates()
{
}

/*!
    \internal
*/
void QScreen::resumeUpdates()
{
}

#ifdef QT_LOADABLE_MODULES

// ### needs update after driver init changes

static QScreen * qt_dodriver(char * driver,char * a,unsigned char * b)

{
    char buf[200];
    strcpy(buf,"/etc/qws/drivers/");
    qstrcpy(buf+17,driver);
    qDebug("Attempting driver %s",driver);

    void * handle;
    handle=dlopen(buf,RTLD_LAZY);
    if(handle==0) {
	qFatal("Module load error");
    }
    QScreen *(*qt_get_screen_func)(char *,unsigned char *);
    qt_get_screen_func=dlsym(handle,"qt_get_screen");
    if(qt_get_screen_func==0) {
	qFatal("Couldn't get symbol");
    }
    QScreen * ret=qt_get_screen_func(a,b);
    return ret;
}

static QScreen * qt_do_entry(char * entry)
{
    unsigned char config[256];

    FILE * f=fopen(entry,"r");
    if(!f) {
	return 0;
    }

    int r=fread(config,256,1,f);
    if(r<1)
	return 0;

    fclose(f);

    unsigned short vendorid=*((unsigned short int *)config);
    unsigned short deviceid=*(((unsigned short int *)config)+1);
    if(config[0xb]!=3)
	return 0;

    if(vendorid==0x1002) {
	if(deviceid==0x4c4d) {
	    qDebug("Compaq Armada/IBM Thinkpad's Mach64 card");
	    return qt_dodriver("mach64.so",entry,config);
	} else if(deviceid==0x4742) {
	    qDebug("Desktop Rage Pro Mach64 card");
	    return qt_dodriver("mach64.so",entry,config);
	} else {
	    qDebug("Unrecognised ATI card id %x",deviceid);
	    return 0;
	}
    } else {
	qDebug("Unrecognised vendor");
    }
    return 0;
}

extern bool qws_accel;

/// ** NOT SUPPPORTED **

QScreen * qt_probe_bus()
{
    if(!qws_accel) {
	return qt_dodriver("unaccel.so",0,0);
    }

    DIR * dirptr=opendir("/proc/bus/pci");
    if(!dirptr)
	return qt_dodriver("unaccel.so",0,0);
    DIR * dirptr2;
    dirent * cards;

    dirent * busses=readdir(dirptr);

    while(busses) {
	if(busses->d_name[0]!='.') {
	    char buf[100];
	    strcpy(buf,"/proc/bus/pci/");
	    qstrcpy(buf+14,busses->d_name);
	    int p=strlen(buf);
	    dirptr2=opendir(buf);
	    if(dirptr2) {
		cards=readdir(dirptr2);
		while(cards) {
		    if(cards->d_name[0]!='.') {
			buf[p]='/';
			qstrcpy(buf+p+1,cards->d_name);
			QScreen * ret=qt_do_entry(buf);
			if(ret)
			    return ret;
		    }
		    cards=readdir(dirptr2);
		}
		closedir(dirptr2);
	    }
	}
	busses=readdir(dirptr);
    }
    closedir(dirptr);

    return qt_dodriver("unaccel.so",0,0);
}

#else

const char * qt_qws_hardcoded_slot="/proc/bus/pci/01/00.0";

const unsigned char* qt_probe_bus()
{
    const char * slot;
    slot=getenv("QWS_CARD_SLOT");
    if(!slot)
	slot=qt_qws_hardcoded_slot;
    if ( slot ) {
	static unsigned char config[256];
	FILE * f=fopen(slot,"r");
	if(!f) {
	    qDebug("Open failure for %s",slot);
	    slot=0;
	} else {
	    int r=fread((char*)config,256,1,f);
	    fclose(f);
	    if(r<1) {
		qDebug("Read failure");
		return 0;
	    } else {
		return config;
	    }
	}
    }
    return 0;
}

#endif
