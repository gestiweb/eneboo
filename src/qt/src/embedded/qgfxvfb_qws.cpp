/*****************************************************************************
**
** Implementation of QGfxvfb (virtual frame buffer driver)
**
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

#include "qgfxraster_qws.h"

#ifndef QT_NO_QWS_VFB

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <qvfbhdr.h>
#include <qgfxvfb_qws.h>
#include <qwindowsystem_qws.h>
#include <qsocketnotifier.h>
#include <qapplication.h>

//===========================================================================

class QVFbMouseHandler : public QObject {
    Q_OBJECT
public:
    QVFbMouseHandler( int display_id );
    ~QVFbMouseHandler();

private:
    int mouseFD;
    int mouseIdx;
    enum {mouseBufSize = 128};
    uchar mouseBuf[mouseBufSize];

private slots:
    void readMouseData();
};

QVFbMouseHandler::QVFbMouseHandler( int display_id )
{
    mouseFD = -1;
    QString mouseDev = QString(QT_VFB_MOUSE_PIPE).arg(display_id);

    if ((mouseFD = open( mouseDev.local8Bit(), O_RDWR | O_NDELAY)) < 0) {
	qDebug( "Cannot open %s (%s)", mouseDev.ascii(),
		strerror(errno));
    } else {
	// Clear pending input
	char buf[2];
	while (read(mouseFD, buf, 1) > 0) { }

	mouseIdx = 0;

	QSocketNotifier *mouseNotifier;
	mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read, this );
	connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
    }
}

QVFbMouseHandler::~QVFbMouseHandler()
{
    if (mouseFD >= 0)
	close(mouseFD);
}

void QVFbMouseHandler::readMouseData()
{
    int n;
    do {
	n = read(mouseFD, mouseBuf+mouseIdx, mouseBufSize-mouseIdx );
	if ( n > 0 )
	    mouseIdx += n;
    } while ( n > 0 );

    int idx = 0;
    while ( mouseIdx-idx >= int(sizeof( QPoint ) + sizeof( int )) ) {
	uchar *mb = mouseBuf+idx;
	QPoint *p = (QPoint *) mb;
	mb += sizeof( QPoint );
	int *bstate = (int *)mb;
	QPoint mousePos = *p;
//	limitToScreen( mousePos );
	QWSServer::sendMouseEvent( mousePos, *bstate );
	idx += sizeof( QPoint ) + sizeof( int );
    }

    int surplus = mouseIdx - idx;
    for ( int i = 0; i < surplus; i++ )
	mouseBuf[i] = mouseBuf[idx+i];
    mouseIdx = surplus;
}
//===========================================================================

class QVFbKeyboardHandler : public QObject
{
    Q_OBJECT
public:
    QVFbKeyboardHandler( int display_id );
    virtual ~QVFbKeyboardHandler();

private slots:
    void readKeyboardData();

private:
    QString terminalName;
    int kbdFD;
    int kbdIdx;
    int kbdBufferLen;
    unsigned char *kbdBuffer;
    QSocketNotifier *notifier;
};


QVFbKeyboardHandler::QVFbKeyboardHandler( int display_id )
{
    kbdFD = -1;
    kbdIdx = 0;
    kbdBufferLen = sizeof( QVFbKeyData ) * 5;
    kbdBuffer = new unsigned char [kbdBufferLen];

    terminalName = QString(QT_VFB_KEYBOARD_PIPE).arg(display_id);

    if ((kbdFD = open( terminalName.local8Bit(), O_RDWR | O_NDELAY)) < 0) {
	qDebug( "Cannot open %s (%s)", terminalName.latin1(),
	strerror(errno));
    } else {
	// Clear pending input
	char buf[2];
	while (read(kbdFD, buf, 1) > 0) { }

	notifier = new QSocketNotifier( kbdFD, QSocketNotifier::Read, this );
	connect(notifier, SIGNAL(activated(int)),this, SLOT(readKeyboardData()));
    }
}

QVFbKeyboardHandler::~QVFbKeyboardHandler()
{
    if ( kbdFD >= 0 )
	close( kbdFD );
    delete [] kbdBuffer;
}


void QVFbKeyboardHandler::readKeyboardData()
{
    int n;
    do {
	n  = read(kbdFD, kbdBuffer+kbdIdx, kbdBufferLen - kbdIdx );
	if ( n > 0 )
	    kbdIdx += n;
    } while ( n > 0 );

    int idx = 0;
    while ( kbdIdx - idx >= (int)sizeof( QVFbKeyData ) ) {
	QVFbKeyData *kd = (QVFbKeyData *)(kbdBuffer + idx);
	if ( kd->unicode == 0 && kd->modifiers == 0 && kd->press ) {
	    // magic exit key
	    qWarning( "Instructed to quit by Virtual Keyboard" );
	    qApp->quit();
	}
	QWSServer::sendKeyEvent( kd->unicode&0xffff, kd->unicode>>16,
				 kd->modifiers, kd->press, kd->repeat );
	idx += sizeof( QVFbKeyData );
    }

    int surplus = kbdIdx - idx;
    for ( int i = 0; i < surplus; i++ )
	kbdBuffer[i] = kbdBuffer[idx+i];
    kbdIdx = surplus;
}


//===========================================================================

#ifndef QT_NO_QWS_CURSOR
class QVFbScreenCursor : public QScreenCursor
{
public:
    QVFbScreenCursor(QVFbScreen * s);

    virtual void set( const QImage &image, int hotx, int hoty );
    virtual void move( int x, int y );

private:

    QVFbScreen * cursor_screen;

};

QVFbScreenCursor::QVFbScreenCursor(QVFbScreen * s) : QScreenCursor()
{
    cursor_screen=s;
}

void QVFbScreenCursor::set( const QImage &image, int hotx, int hoty )
{
    QWSDisplay::grab( TRUE );
    QRect r( data->x - hotx, data->y - hoty, image.width(), image.height() );
    cursor_screen->setDirty( data->bound | r );
    QScreenCursor::set( image, hotx, hoty );
    QWSDisplay::ungrab();
}

void QVFbScreenCursor::move( int x, int y )
{
    QWSDisplay::grab( TRUE );
    QRect r( x - data->hotx, y - data->hoty, data->width, data->height );
    cursor_screen->setDirty( r | data->bound );
    QScreenCursor::move( x, y );
    QWSDisplay::ungrab();
}
#endif

//===========================================================================

template <const int depth, const int type>
class QGfxVFb : public QGfxRaster<depth,type>
{
public:
    QGfxVFb(unsigned char *b,int w,int h);
    virtual ~QGfxVFb();

    virtual void drawPoint( int,int );
    virtual void drawPoints( const QPointArray &,int,int );
    virtual void drawLine( int,int,int,int );
    virtual void fillRect( int,int,int,int );
    virtual void drawPolyline( const QPointArray &,int,int );
    virtual void drawPolygon( const QPointArray &,bool,int,int );
    virtual void blt( int,int,int,int,int,int );
    virtual void scroll( int,int,int,int,int,int );
#if !defined(QT_NO_MOVIE) || !defined(QT_NO_TRANSFORMATIONS)
    virtual void stretchBlt( int,int,int,int,int,int );
#endif
    virtual void tiledBlt( int,int,int,int );
};

template <const int depth, const int type>
QGfxVFb<depth,type>::QGfxVFb(unsigned char *b,int w,int h)
    : QGfxRaster<depth,type>( b, w, h )
{
}

template <const int depth, const int type>
QGfxVFb<depth,type>::~QGfxVFb()
{
}

template <const int depth, const int type>
void QGfxVFb<depth,type>::drawPoint( int x, int y )
{
    QWSDisplay::grab( TRUE );
    ((QVFbScreen *)this->gfx_screen)->setDirty( QRect( x+this->xoffs, y+this->yoffs, 1, 1 ) );
    QGfxRaster<depth,type>::drawPoint( x, y );
    QWSDisplay::ungrab();
}

template <const int depth, const int type>
void QGfxVFb<depth,type>::drawPoints( const QPointArray &pa,int x,int y )
{
    QWSDisplay::grab( TRUE );
    ((QVFbScreen *)this->gfx_screen)->setDirty( this->clipbounds );
    QGfxRaster<depth,type>::drawPoints( pa, x, y );
    QWSDisplay::ungrab();
}

template <const int depth, const int type>
void QGfxVFb<depth,type>::drawLine( int x1,int y1,int x2,int y2 )
{
    QWSDisplay::grab( TRUE );
    QRect r;
    r.setCoords( x1+this->xoffs, y1+this->yoffs, x2+this->xoffs, y2+this->yoffs );
    ((QVFbScreen *)this->gfx_screen)->setDirty( r.normalize() );
    QGfxRaster<depth,type>::drawLine( x1, y1, x2, y2 );
    QWSDisplay::ungrab();
}

template <const int depth, const int type>
void QGfxVFb<depth,type>::fillRect( int x,int y,int w,int h )
{
    QWSDisplay::grab( TRUE );
    ((QVFbScreen *)this->gfx_screen)->setDirty( QRect( x+this->xoffs, y+this->yoffs, w, h ) );
    QGfxRaster<depth,type>::fillRect( x, y, w, h );
    QWSDisplay::ungrab();
}

template <const int depth, const int type>
void QGfxVFb<depth,type>::drawPolyline( const QPointArray &pa,int x,int y )
{
    QWSDisplay::grab( TRUE );
    ((QVFbScreen *)this->gfx_screen)->setDirty( this->clipbounds );
    QGfxRaster<depth,type>::drawPolyline( pa, x, y );
    QWSDisplay::ungrab();
}

template <const int depth, const int type>
void QGfxVFb<depth,type>::drawPolygon( const QPointArray &pa,bool w,int x,int y )
{
    QWSDisplay::grab( TRUE );
    ((QVFbScreen *)this->gfx_screen)->setDirty( this->clipbounds );
    QGfxRaster<depth,type>::drawPolygon( pa, w, x, y );
    QWSDisplay::ungrab();
}

template <const int depth, const int type>
void QGfxVFb<depth,type>::blt( int x,int y,int w,int h, int sx, int sy )
{
    QWSDisplay::grab( TRUE );
    ((QVFbScreen *)this->gfx_screen)->setDirty( QRect( x+this->xoffs, y+this->yoffs, w, h ) );
    QGfxRaster<depth,type>::blt( x, y, w, h, sx, sy );
    QWSDisplay::ungrab();
}

template <const int depth, const int type>
void QGfxVFb<depth,type>::scroll( int x,int y,int w,int h,int sx,int sy )
{
    QWSDisplay::grab( TRUE );
    int dy = sy - y;
    int dx = sx - x;
    ((QVFbScreen *)this->gfx_screen)->setDirty( QRect(QMIN(x,sx) + this->xoffs, QMIN(y,sy) + this->yoffs,
			   w+abs(dx), h+abs(dy)) );
    QGfxRaster<depth,type>::scroll( x, y, w, h, sx, sy );
    QWSDisplay::ungrab();
}

#if !defined(QT_NO_MOVIE) || !defined(QT_NO_TRANSFORMATIONS)
template <const int depth, const int type>
void QGfxVFb<depth,type>::stretchBlt( int x,int y,int w,int h,int sx,int sy )
{
    QWSDisplay::grab( TRUE );
    ((QVFbScreen *)this->gfx_screen)->setDirty( QRect( x + this->xoffs, y + this->yoffs, w, h) );
    QGfxRaster<depth,type>::stretchBlt( x, y, w, h, sx, sy );
    QWSDisplay::ungrab();
}
#endif

template <const int depth, const int type>
void QGfxVFb<depth,type>::tiledBlt( int x,int y,int w,int h )
{
    QWSDisplay::grab( TRUE );
    ((QVFbScreen *)this->gfx_screen)->setDirty( QRect(x + this->xoffs, y + this->yoffs, w, h) );
    QGfxRaster<depth,type>::tiledBlt( x, y, w, h );
    QWSDisplay::ungrab();
}


//===========================================================================

QVFbScreen::QVFbScreen( int display_id ) : QScreen( display_id )
{
    mouseHandler = 0;
    keyboardHandler = 0;
    shmrgn = 0;
    hdr = 0;
    data = 0;
}

QVFbScreen::~QVFbScreen()
{
}


static int QVFb_dummy;

bool QVFbScreen::connect( const QString &displaySpec )
{
    screen_optype=&QVFb_dummy;
    screen_lastop=&QVFb_dummy;

    if ( displaySpec.find( ":Gray" ) >= 0 )
	grayscale = TRUE;

    key_t key = ftok( QString(QT_VFB_MOUSE_PIPE).arg(displayId).latin1(), 'b' );

    if ( key == -1 )
	return FALSE;

    int shmId = shmget( key, 0, 0 );
    if ( shmId != -1 )
	shmrgn = (unsigned char *)shmat( shmId, 0, 0 );
    else
	return FALSE;

    if ( (int)shmrgn == -1 || shmrgn == 0 ) {
	qDebug("No shmrgn %d", (int)shmrgn);
	return FALSE;
    }

    hdr = (QVFbHeader *) shmrgn;
    data = shmrgn + hdr->dataoffset;

    dw = w = hdr->width;
    dh = h = hdr->height;
    d = hdr->depth;
    lstep = hdr->linestep;

    qDebug( "Connected to VFB server: %d x %d x %d", w, h, d );

    size = lstep * h;
    mapsize = size;
    screencols = hdr->numcols;
    memcpy( screenclut, hdr->clut, sizeof( QRgb ) * screencols );

    if ( qApp->type() == QApplication::GuiServer ) {
	// We handle mouse and keyboard here
	QWSServer::setDefaultMouse( "None" );
	QWSServer::setDefaultKeyboard( "None" );
	mouseHandler = new QVFbMouseHandler( displayId );
	keyboardHandler = new QVFbKeyboardHandler( displayId );
    }

    return TRUE;
}

void QVFbScreen::disconnect()
{
    if ( (int)shmrgn != -1 && shmrgn )
	shmdt( (char*)shmrgn );
    if ( qApp->type() == QApplication::GuiServer ) {
	delete mouseHandler;
	mouseHandler = 0;
	delete keyboardHandler;
	keyboardHandler = 0;
    }
}

bool QVFbScreen::initDevice()
{
    if(d==8) {
	screencols=256;
	if ( grayscale ) {
	    // Build grayscale palette
	    for(int loopc=0;loopc<256;loopc++) {
		screenclut[loopc]=qRgb(loopc,loopc,loopc);
	    }
	} else {
	    // 6x6x6 216 color cube
	    int idx = 0;
	    for( int ir = 0x0; ir <= 0xff; ir+=0x33 ) {
		for( int ig = 0x0; ig <= 0xff; ig+=0x33 ) {
		    for( int ib = 0x0; ib <= 0xff; ib+=0x33 ) {
			screenclut[idx]=qRgb( ir, ig, ib );
			idx++;
		    }
		}
	    }
	    screencols=idx;
	}
	memcpy( hdr->clut, screenclut, sizeof( QRgb ) * screencols );
	hdr->numcols = screencols;
    } else if ( d == 4 ) {
	int val = 0;
	for ( int idx = 0; idx < 16; idx++, val += 17 ) {
	    screenclut[idx] = qRgb( val, val, val );
	}
	screencols = 16;
	memcpy( hdr->clut, screenclut, sizeof( QRgb ) * screencols );
	hdr->numcols = screencols;
    } else if ( d == 1 ) {
	screencols = 2;
	screenclut[1] = qRgb( 0xff, 0xff, 0xff );
	screenclut[0] = qRgb( 0, 0, 0 );
	memcpy( hdr->clut, screenclut, sizeof( QRgb ) * screencols );
	hdr->numcols = screencols;
    }

    return true;
}

void QVFbScreen::shutdownDevice()
{
}

int QVFbScreen::initCursor(void* e, bool init)
{
#ifndef QT_NO_QWS_CURSOR
    qt_sw_cursor=true;
    // ### until QLumpManager works Ok with multiple connected clients,
    // we steal a chunk of shared memory
    SWCursorData *data = (SWCursorData *)e - 1;
    qt_screencursor=new QVFbScreenCursor(this);
    qt_screencursor->init( data, init );
    return sizeof(SWCursorData);
#else
    return 0;
#endif
}

void QVFbScreen::setMode(int ,int ,int)
{
}

// save the state of the graphics card
// This is needed so that e.g. we can restore the palette when switching
// between linux virtual consoles.
void QVFbScreen::save()
{
    // nothing to do.
}

// restore the state of the graphics card.
void QVFbScreen::restore()
{
}

QGfx * QVFbScreen::createGfx(unsigned char * bytes,int w,int h,int d, int linestep)
{
    QGfx* ret = 0;
    if(d==1) {
	if ( bytes == qt_screen->base() )
	    ret = new QGfxVFb<1,0>(bytes,w,h);
	else
	    ret = new QGfxRaster<1,0>(bytes,w,h);
#ifndef QT_NO_QWS_DEPTH_4
    } else if (d==4) {
	if ( bytes == qt_screen->base() )
	    ret = new QGfxVFb<4,0>(bytes,w,h);
	else
	    ret = new QGfxRaster<4,0>(bytes,w,h);
#endif
#ifndef QT_NO_QWS_DEPTH_16
    } else if(d==16) {
	if ( bytes == qt_screen->base() )
	    ret = new QGfxVFb<16,0>(bytes,w,h);
	else
	    ret = new QGfxRaster<16,0>(bytes,w,h);
#endif
#ifndef QT_NO_QWS_DEPTH_8
    } else if (d==8) {
	if ( bytes == qt_screen->base() )
	    ret = new QGfxVFb<8,0>(bytes,w,h);
	else
	    ret = new QGfxRaster<8,0>(bytes,w,h);
#endif
#ifndef QT_NO_QWS_DEPTH_32
    } else if (d==32) {
	if ( bytes == qt_screen->base() )
	    ret = new QGfxVFb<32,0>(bytes,w,h);
	else
	    ret = new QGfxRaster<32,0>(bytes,w,h);
#endif
    } else {
	qFatal("Can't drive depth %d",d);
    }
    ret->setLineStep(linestep);
    return ret;
}

#include "qgfxvfb_qws.moc"

#endif

