/****************************************************************************
**
** Implementation of Qt/Embedded common PC mouse drivers
**
** Created : 20020220
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

#include "qmousepc_qws.h"

#include "qwindowsystem_qws.h"
#include "qsocketnotifier.h"
#include "qwsevent_qws.h"
#include "qwscommand_qws.h"
#include "qwsutils_qws.h"

#include "qapplication.h"
#include "qpointarray.h"
#include "qtimer.h"
#include "qfile.h"
#include "qtextstream.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#include <qgfx_qws.h>

/*
 * Automatic-detection mouse driver
 */

class QWSPcMouseSubHandler {
protected:
    enum { max_buf=32 };

    int fd;

    uchar buffer[max_buf];
    int nbuf;

    QPoint motion;
    int bstate;

    int goodness;
    int badness;

    virtual int tryData()=0;

public:
    QWSPcMouseSubHandler(int f) : fd(f)
    {
	nbuf = bstate = goodness = badness = 0;
    }

    int file() const { return fd; }

    void closeIfNot(int& f)
    {
	if ( fd != f ) {
	    f = fd;
	    close(fd);
	}
    }

    void worse(int by=1) { badness+=by; }
    bool reliable() const { return goodness >= 5 && badness < 50; }
    int buttonState() const { return bstate; }
    bool motionPending() const { return motion!=QPoint(0,0); }
    QPoint takeMotion() { QPoint r=motion; motion=QPoint(0,0); return r; }

    void appendData(uchar* data, int length)
    {
	memcpy(buffer+nbuf, data, length);
	nbuf += length;
    }

    enum UsageResult { Insufficient, Motion, Button };

    UsageResult useData()
    {
	int pbstate = bstate;
	int n = tryData();
	if ( n > 0 ) {
	    if ( n<nbuf )
		memmove( buffer, buffer+n, nbuf-n );
	    nbuf -= n;
	    return pbstate == bstate ? Motion : Button;
	}
	return Insufficient;
    }
};

class QWSPcMouseSubHandler_intellimouse : public QWSPcMouseSubHandler {
    int packetsize;
public:
    QWSPcMouseSubHandler_intellimouse(int f) : QWSPcMouseSubHandler(f)
    {
	init();
    }

    void init()
    {
	int n;
	uchar reply[20];

	tcflush(fd,TCIOFLUSH);
	static const uchar initseq[] = { 243, 200, 243, 100, 243, 80 };
	static const uchar query[] = { 0xf2 };
	if (write(fd, initseq, sizeof(initseq))!=sizeof(initseq)) {
	    badness = 100;
	    return;
	}
	usleep(10000);
	tcflush(fd,TCIOFLUSH);
	if (write(fd, query, sizeof(query))!=sizeof(query)) {
	    badness = 100;
	    return;
	}
	usleep(10000);
	n = read(fd, reply, 20);
	if ( n > 0 ) {
	    goodness = 10;
	    switch ( reply[n-1] ) {
	      case 3:
	      case 4:
		packetsize = 4;
		break;
	     default:
		packetsize = 3;
	    }
	} else {
	    badness = 100;
	}
    }

    int tryData()
    {
	if ( nbuf >= packetsize ) {
	    //int overflow = (buffer[0]>>6 )& 0x03;

	    if ( /*overflow ||*/ !(buffer[0] & 8) ) {
		badness++;
		return 1;
	    } else {
		QPoint delta((buffer[0] & 0x10) ? buffer[1]-256 : buffer[1],
		       (buffer[0] & 0x20) ? 256-buffer[2] : -buffer[2]);
#ifndef QT_NO_QWS_TRANSFORMED
		delta = qt_screen->mapToDevice( delta, QSize(1,1) );
#endif // QT_NO_QWS_TRANSFORMED
		motion += delta;
		int nbstate = buffer[0] & 0x7;
		int wheel = packetsize > 3 ? (signed char)buffer[3] : 0;
		if ( wheel < -2 || wheel > 2 )
		    wheel = 0;
		if ( motion.x() || motion.y() || bstate != nbstate || wheel) {
		    bstate = nbstate;
		    goodness++;
		} else {
		    badness++;
		    return 1;
		}
	    }
	    return packetsize;
	}
	return 0;
    }
};

class QWSPcMouseSubHandler_mouseman : public QWSPcMouseSubHandler {
    int packetsize;
public:
    QWSPcMouseSubHandler_mouseman(int f) : QWSPcMouseSubHandler(f)
    {
	init();
    }

    void init()
    {
	tcflush(fd,TCIOFLUSH);
	write(fd,"",1);
	usleep(50000);
	write(fd,"@EeI!",5);
	usleep(10000);
	char ibuf[] = { 246, 244 };
	write(fd,ibuf,1);
	write(fd,ibuf+1,1);
	tcflush(fd,TCIOFLUSH);
	usleep(10000);

	char buf[100];
	while (read(fd, buf, 100) > 0) { }  // eat unwanted replies
    }

    int tryData()
    {
	if ( nbuf >= 3 ) {
	    int nbstate = 0;
	    if (buffer[0] & 0x01)
		nbstate |= Qt::LeftButton;
	    if (buffer[0] & 0x02)
		nbstate |= Qt::RightButton;
	    if (buffer[0] & 0x04)
		nbstate |= Qt::MidButton;

	    int overflow = (buffer[0]>>6 )& 0x03;
	    if ( overflow ) {
		//### wheel events signalled with overflow bit, ignore for now
		badness++;
		return 1;
	    } else {
		bool xs = buffer[0] & 0x10;
		bool ys = buffer[0] & 0x20;
		int dx = xs ? buffer[1]-256 : buffer[1];
		int dy = ys ? buffer[2]-256 : buffer[2];

		motion += QPoint( dx, -dy );
		if ( motion.x() || motion.y() || bstate != nbstate ) {
		    bstate = nbstate;
		    goodness++;
		} else {
		    badness++;
		    return 1;
		}
	    }
	    return 3;
	}
	return 0;
    }
};

class QWSPcMouseSubHandler_serial : public QWSPcMouseSubHandler {
public:
    QWSPcMouseSubHandler_serial(int f) : QWSPcMouseSubHandler(f)
    {
	initSerial();
    }

protected:
    void setflags(int f)
    {
	termios tty;
	tcgetattr(fd, &tty);
	tty.c_iflag     = IGNBRK | IGNPAR;
	tty.c_oflag     = 0;
	tty.c_lflag     = 0;
	tty.c_cflag     = f | CREAD | CLOCAL | HUPCL;
#if !defined(Q_OS_FREEBSD) && !defined(Q_OS_SOLARIS) && !defined(Q_OS_MACX)
	tty.c_line      = 0;
#endif
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN]  = 1;
	tcsetattr(fd, TCSANOW, &tty);
    }

private:
    void initSerial()
    {
	int speed[4] = { B9600, B4800, B2400, B1200 };

	for (int n = 0; n < 4; n++) {
	    setflags(CSTOPB | speed[n]);
	    write(fd, "*q", 2);
	    usleep(10000);
	}
    }
};

class QWSPcMouseSubHandler_mousesystems : public QWSPcMouseSubHandler_serial {
public:
    // ##### This driver has not been tested

    QWSPcMouseSubHandler_mousesystems(int f) : QWSPcMouseSubHandler_serial(f)
    {
	init();
    }

    void init()
    {
	setflags(B1200|CS8|CSTOPB);
	// 60Hz
	if (write(fd, "R", 1)!=1) {
	    badness = 100;
	    return;
	}
	tcflush(fd,TCIOFLUSH);
    }

    int tryData()
    {
	if ( nbuf >= 5 ) {
	    if ( (buffer[0] & 0xf8) != 0x80 ) {
		badness++;
		return 1;
	    }
	    motion +=
		QPoint((signed char)buffer[1] + (signed char)buffer[3],
		       -(signed char)buffer[2] + (signed char)buffer[4]);
	    int t = ~buffer[0];
	    int nbstate = ((t&3) << 1) | ((t&4) >> 2);
	    if ( motion.x() || motion.y() || bstate != nbstate ) {
		bstate = nbstate;
		goodness++;
	    } else {
		badness++;
		return 1;
	    }
	    return 5;
	}
	return 0;
    }
};

class QWSPcMouseSubHandler_ms : public QWSPcMouseSubHandler_serial {
    int mman;
public:
    QWSPcMouseSubHandler_ms(int f) : QWSPcMouseSubHandler_serial(f)
    {
	mman=0;
	init();
    }

    void init()
    {
	setflags(B1200|CS7);
	// 60Hz
	if (write(fd, "R", 1)!=1) {
	    badness = 100;
	    return;
	}
	tcflush(fd,TCIOFLUSH);
    }

    int tryData()
    {
	if ( !(buffer[0] & 0x40) ) {
	    if ( buffer[0] == 0x20 && (bstate & Qt::MidButton) ) {
		mman=1; // mouseman extension
	    }
	    return 1;
	}
	int extra = mman&&(bstate & Qt::MidButton);
	if ( nbuf >= 3+extra ) {
	    int nbstate = 0;
	    if ( buffer[0] == 0x40 && !bstate && !buffer[1] && !buffer[2] ) {
		nbstate = Qt::MidButton;
	    } else {
		nbstate = ((buffer[0] & 0x20) >> 5)
			| ((buffer[0] & 0x10) >> 3);
		if ( extra && buffer[3] == 0x20 )
		    nbstate = Qt::MidButton;
	    }

	    if ( buffer[1] & 0x40 ) {
		badness++;
		return 1;
	    } else {
		motion +=
		    QPoint((signed char)((buffer[0]&0x3)<<6)
			    |(signed char)(buffer[1]&0x3f),
			   (signed char)((buffer[0]&0xc)<<4)
			    |(signed char)(buffer[2]&0x3f));
		if ( motion.x() || motion.y() || bstate != nbstate ) {
		    bstate = nbstate;
		    goodness++;
		} else {
		    badness++;
		    return 1;
		}
		return 3+extra;
	    }
	}
	return 0;
    }
};

//===========================================================================

class QWSPcMouseHandlerPrivate : public QObject
{
    Q_OBJECT
public:
    QWSPcMouseHandlerPrivate( QWSPcMouseHandler *h, const QString &, const QString & );
    ~QWSPcMouseHandlerPrivate();

private:
    enum { max_dev=32 };
    QWSPcMouseSubHandler *sub[max_dev];
    QPtrList<QSocketNotifier> notifiers;
    int nsub;
    int retries;

private slots:
    void readMouseData(int);

private:
    void openDevices();
    void closeDevices();
    void notify(int fd);
    bool sendEvent(QWSPcMouseSubHandler& h);

private:
    QWSPcMouseHandler *handler;
    QString driver;
    QString device;
};

QWSPcMouseHandler::QWSPcMouseHandler( const QString &driver, const QString &device )
{
    d = new QWSPcMouseHandlerPrivate( this, driver, device );
}

QWSPcMouseHandler::~QWSPcMouseHandler()
{
    delete d;
}


QWSPcMouseHandlerPrivate::QWSPcMouseHandlerPrivate( QWSPcMouseHandler *h,
    const QString &drv, const QString &dev )
    : handler( h ), driver( drv ), device( dev )
{
    notifiers.setAutoDelete( TRUE );
    retries = 0;
    openDevices();
}

QWSPcMouseHandlerPrivate::~QWSPcMouseHandlerPrivate()
{
    closeDevices();
}

/*
QWSPcMouseHandler::UsageResult QWSPcMouseHandler::useDev(Dev& d)
{
    if ( d.nbuf >= mouseData[d.protocol].bytesPerPacket ) {
	uchar *mb = d.buf;
	int bstate = 0;
	int dx = 0;
	int dy = 0;

	switch (mouseProtocol) {
	    case MouseMan:
	    case IntelliMouse:
	    {
		bstate = mb[0] & 0x7; // assuming Qt::*Button order

		int overflow = (mb[0]>>6 )& 0x03;
		if (mouseProtocol == MouseMan && overflow) {
		    //### wheel events signalled with overflow bit, ignore for now
		}
		else {
		    bool xs = mb[0] & 0x10;
		    bool ys = mb[0] & 0x20;
		    dx = xs ? mb[1]-256 : mb[1];
		    dy = ys ? mb[2]-256 : mb[2];
		}
		break;
	    }
	    case Microsoft:
		if ( ((mb[0] & 0x20) >> 3) ) {
		    bstate |= Qt::LeftButton;
		}
		if ( ((mb[0] & 0x10) >> 4) ) {
		    bstate |= Qt::RightButton;
		}

		dx=(signed char)(((mb[0] & 0x03) << 6) | (mb[1] & 0x3f));
		dy=-(signed char)(((mb[0] & 0x0c) << 4) | (mb[2] & 0x3f));

		break;
	}
    }
    }
*/


bool QWSPcMouseHandlerPrivate::sendEvent(QWSPcMouseSubHandler& h)
{
    static const int accel_limit = 5;
    static const int accel = 2;

    if ( h.reliable() ) {
	QPoint motion = h.takeMotion();
	if ( QABS(motion.x()) > accel_limit || QABS(motion.y()) > accel_limit )
	    motion *= accel;
	QPoint newPos = handler->pos() + motion;
	handler->limitToScreen( newPos );
/*
        qDebug("%d,%d %c%c%c",
	    newPos.x(),newPos.y(),
	    (h.buttonState()&Qt::LeftButton)?'L':'.',
	    (h.buttonState()&Qt::MidButton)?'M':'.',
	    (h.buttonState()&Qt::RightButton)?'R':'.');
 */
	handler->mouseChanged(newPos,h.buttonState());
	return TRUE;
    } else {
	h.takeMotion();
	if ( h.buttonState() & (Qt::RightButton|Qt::MidButton) ) {
	    // Strange for the user to press right or middle without
	    // a moving mouse!
	    h.worse();
	}
	return FALSE;
    }
}

void QWSPcMouseHandlerPrivate::openDevices()
{
    nsub=0;
    int fd = -1;

    if ( !driver.isEmpty() && driver != "Auto" ) {
	// Manually specified mouse
	if ( driver == "IntelliMouse" ) {
	    QString dev = device.isEmpty() ? QString("/dev/psaux") : device;
	    fd = open( dev.latin1(), O_RDWR | O_NDELAY );
	    if ( fd >= 0 )
		sub[nsub++] = new QWSPcMouseSubHandler_intellimouse(fd);
	} else if ( driver == "Microsoft" ) {
	    QString dev = device.isEmpty() ? QString("/dev/ttyS0") : device;
	    fd = open( dev.latin1(), O_RDWR | O_NDELAY );
	    if ( fd >= 0 )
		sub[nsub++] = new QWSPcMouseSubHandler_ms(fd);
	} else if ( driver == "MouseSystems" ) {
	    QString dev = device.isEmpty() ? QString("/dev/ttyS0") : device;
	    fd = open( dev.latin1(), O_RDWR | O_NDELAY );
	    if ( fd >= 0 )
		sub[nsub++] = new QWSPcMouseSubHandler_mousesystems(fd);
	} else if ( driver == "MouseMan" ) {
	    QString dev = device.isEmpty() ? QString("/dev/psaux") : device;
	    fd = open( dev.latin1(), O_RDWR | O_NDELAY );
	    if ( fd >= 0 )
		sub[nsub++] = new QWSPcMouseSubHandler_mouseman(fd);
	}
	if ( fd >= 0 )
	    notify(fd);
    } else {
	// Try automatically
	fd = open( "/dev/psaux", O_RDWR | O_NDELAY );
	if ( fd >= 0 ) {
	    sub[nsub++] = new QWSPcMouseSubHandler_intellimouse(fd);
	    notify(fd);
	}
	fd = open( "/dev/input/mice", O_RDWR | O_NDELAY );
	if ( fd >= 0 ) {
	    sub[nsub++] = new QWSPcMouseSubHandler_intellimouse(fd);
	    notify(fd);
	    //qDebug( "/dev/input/mice fd %d #%d", fd, nsub-1 );
	}

	char fn[] = "/dev/ttyS?";
	for (int ch='0'; ch<='3'; ch++) {
	    fn[9] = ch;
	    fd = open( fn, O_RDWR | O_NDELAY );
	    if ( fd >= 0 ) {
		//sub[nsub++] = new QWSPcMouseSubHandler_intellimouse(fd);
		sub[nsub++] = new QWSPcMouseSubHandler_mousesystems(fd);
		sub[nsub++] = new QWSPcMouseSubHandler_ms(fd);
		notify(fd);
	    }
	}
    }
}

void QWSPcMouseHandlerPrivate::closeDevices()
{
    int pfd=-1;
    for (int i=0; i<nsub; i++) {
	sub[i]->closeIfNot(pfd);
	delete sub[i];
    }
    notifiers.clear();
}

void QWSPcMouseHandlerPrivate::notify(int fd)
{
    QSocketNotifier *mouseNotifier
	= new QSocketNotifier( fd, QSocketNotifier::Read, this );
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData(int)));
    notifiers.append( mouseNotifier );
}

void QWSPcMouseHandlerPrivate::readMouseData(int fd)
{
    for (;;) {
	uchar buf[8];
	int n = read(fd, buf, 8);
	if ( n<=0 )
	    break;
	for (int i=0; i<nsub; i++) {
	    QWSPcMouseSubHandler& h = *sub[i];
	    if ( h.file() == fd ) {
		h.appendData(buf,n);
		for (;;) {
		    switch ( h.useData() ) {
		      case QWSPcMouseSubHandler::Button:
			sendEvent(h);
			break;
		      case QWSPcMouseSubHandler::Insufficient:
			goto breakbreak;
		      case QWSPcMouseSubHandler::Motion:
			break;
		    }
		}
		breakbreak:
		    ;
	    }
	}
    }
    bool any_reliable=FALSE;
    for (int i=0; i<nsub; i++) {
	QWSPcMouseSubHandler& h = *sub[i];
	if ( h.motionPending() )
	    sendEvent(h);
	any_reliable = any_reliable || h.reliable();
    }
    if ( any_reliable ) {
	// ... get rid of all unreliable ones?  All bad ones?
    } else if ( retries < 2 ) {
	// Try again - maybe the mouse was being moved when we tried to init.
	closeDevices();
	openDevices();
	retries++;
    }
}

#include "qmousepc_qws.moc"

