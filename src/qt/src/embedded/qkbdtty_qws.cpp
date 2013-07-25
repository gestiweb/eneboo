/****************************************************************************
**
** Implementation of Qt/Embedded keyboard drivers
**
** Created : 991025
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

#include "qkbdtty_qws.h"

#ifndef QT_NO_QWS_KEYBOARD

#include "qgfx_qws.h"

#include "qwindowsystem_qws.h"
#include "qapplication.h"
#include "qsocketnotifier.h"
#include "qnamespace.h"
#include "qtimer.h"

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>

#ifdef Q_OS_QNX6
#include "qwskeyboard_qnx.cpp"
#endif

#ifdef Q_OS_LINUX
#include <sys/kd.h>
#include <sys/vt.h>
#endif

#define VTACQSIG SIGUSR1
#define VTRELSIG SIGUSR2

//static bool vtActive = true;
static int  vtQws = 0;
static int  kbdFD = -1;

volatile sig_atomic_t qt_qws_tty_signal;
static volatile sig_atomic_t vt_is_active = 1;

#if defined(Q_OS_LINUX)
static void vtReleaseHandler(int /*sig*/)
{
    qt_qws_tty_signal = 1;
    vt_is_active = 0;
}

static void vtAcquireHandler(int /*sig*/)
{
    qt_qws_tty_signal = 1;
    vt_is_active = 1;
}


void qt_qws_handle_tty_signal()
{
    //block signals while we process the previous one(s)
    
    sigset_t newset,oldset;
    sigemptyset(&newset);
    sigaddset(&newset, VTRELSIG);
    sigaddset(&newset, VTACQSIG);
    sigprocmask(SIG_BLOCK, &newset, &oldset);

    if (vt_is_active) {
	if (ioctl(kbdFD, VT_RELDISP, VT_ACKACQ) == 0) {
	    qwsServer->enablePainting(true);
	    qt_screen->restore();
	    qwsServer->openMouse();
	    qwsServer->refresh();
	}
    } else {
	qwsServer->enablePainting(false);
	qt_screen->save();
	if (ioctl(kbdFD, VT_RELDISP, 1) == 0) {
	    qwsServer->closeMouse();
	}
	else {
	    qwsServer->enablePainting(true);
	}
    }

    sigprocmask(SIG_SETMASK, &oldset, 0);
}
#endif





//===========================================================================

//
// Tty keyboard
//

#ifndef QT_NO_QWS_KBD_TTY

class QWSTtyKbPrivate : public QObject
{
    Q_OBJECT
public:
    QWSTtyKbPrivate( QWSPC101KeyboardHandler *, const QString &device );
    ~QWSTtyKbPrivate();

private slots:
    void readKeyboardData();

private:
    QWSPC101KeyboardHandler *handler;
    struct termios origTermData;
};

QWSTtyKeyboardHandler::QWSTtyKeyboardHandler( const QString &device )
    : QWSPC101KeyboardHandler( device )
{
    d = new QWSTtyKbPrivate( this, device );
}

QWSTtyKeyboardHandler::~QWSTtyKeyboardHandler()
{
    delete d;
}

void QWSTtyKeyboardHandler::processKeyEvent(int unicode, int keycode,
		    int modifiers, bool isPress, bool autoRepeat)
{
#if defined(Q_OS_LINUX)
    // Virtual console switching
    int term = 0;
    bool ctrl = modifiers & Qt::ControlButton;
    bool alt = modifiers & Qt::AltButton;
    if (ctrl && alt && keycode >= Qt::Key_F1 && keycode <= Qt::Key_F10)
	term = keycode - Qt::Key_F1 + 1;
    else if (ctrl && alt && keycode == Qt::Key_Left)
	term = QMAX(vtQws - 1, 1);
    else if (ctrl && alt && keycode == Qt::Key_Right)
	term = QMIN(vtQws + 1, 10);
    if (term && isPress) {
	ioctl(kbdFD, VT_ACTIVATE, term);
	return;
    }
#endif

    QWSPC101KeyboardHandler::processKeyEvent( unicode, keycode, modifiers,
	isPress, autoRepeat );
}


QWSTtyKbPrivate::QWSTtyKbPrivate( QWSPC101KeyboardHandler *h, const QString &device ) : handler(h)
{
    kbdFD = ::open(device.isEmpty()?"/dev/tty0":device.latin1(), O_RDWR|O_NDELAY, 0);

    if ( kbdFD >= 0 ) {
	QSocketNotifier *notifier;
	notifier = new QSocketNotifier( kbdFD, QSocketNotifier::Read, this );
	connect( notifier, SIGNAL(activated(int)),this,
		 SLOT(readKeyboardData()) );

	// save for restore.
	tcgetattr( kbdFD, &origTermData );

	struct termios termdata;
	tcgetattr( kbdFD, &termdata );

#if defined(Q_OS_LINUX)
	ioctl(kbdFD, KDSKBMODE, K_RAW);
#endif

	termdata.c_iflag = (IGNPAR | IGNBRK) & (~PARMRK) & (~ISTRIP);
	termdata.c_oflag = 0;
	termdata.c_cflag = CREAD | CS8;
	termdata.c_lflag = 0;
	termdata.c_cc[VTIME]=0;
	termdata.c_cc[VMIN]=1;
	cfsetispeed(&termdata, 9600);
	cfsetospeed(&termdata, 9600);
	tcsetattr(kbdFD, TCSANOW, &termdata);

#if defined(Q_OS_LINUX)

	struct sigaction sa;
	sigemptyset(&(sa.sa_mask));
	sigaddset(&(sa.sa_mask), VTRELSIG);
	sigaddset(&(sa.sa_mask), VTACQSIG);

	sa.sa_flags = 0;
	sa.sa_handler = vtReleaseHandler;
	sigaction(VTRELSIG, &sa, 0);
	sa.sa_handler = vtAcquireHandler;
	sigaction(VTACQSIG, &sa, 0);

	struct vt_mode vtMode;
	ioctl(kbdFD, VT_GETMODE, &vtMode);

	// let us control VT switching
	vtMode.mode = VT_PROCESS;
	vtMode.relsig = VTRELSIG;
	vtMode.acqsig = VTACQSIG;
	ioctl(kbdFD, VT_SETMODE, &vtMode);

	struct vt_stat vtStat;
	ioctl(kbdFD, VT_GETSTATE, &vtStat);
	vtQws = vtStat.v_active;
#endif
    } else {
	qDebug( "Cannot open keyboard" );
    }

}

QWSTtyKbPrivate::~QWSTtyKbPrivate()
{
    if (kbdFD >= 0) {
#if defined(Q_OS_LINUX)
	ioctl(kbdFD, KDSKBMODE, K_XLATE);
#endif
	tcsetattr(kbdFD, TCSANOW, &origTermData);
	::close(kbdFD);
	kbdFD = -1;
    }
}

void QWSTtyKbPrivate::readKeyboardData()
{
    unsigned char buf[81];
    int n = read(kbdFD, buf, 80 );
    for ( int loop = 0; loop < n; loop++ )
	handler->doKey(buf[loop]);
}

#endif // QT_NO_QWS_KBD_TTY

#include "qkbdtty_qws.moc"

#endif // QT_NO_QWS_KEYBOARD
