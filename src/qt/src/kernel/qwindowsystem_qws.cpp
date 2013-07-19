/****************************************************************************
**
** Implementation of Qt/FB central server
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

#include "qwindowsystem_qws.h"
#include "qwsevent_qws.h"
#include "qwscommand_qws.h"
#include "qwsutils_qws.h"
#include "qwscursor_qws.h"
#include "qwsdisplay_qws.h"
#include "qmouse_qws.h"
#include "qcopchannel_qws.h"

#include "qapplication.h"
#include "qsocketnotifier.h"
#include "qpointarray.h" //cursor test code
#include "qimage.h"
#include "qcursor.h"
#include "qgfx_qws.h"
#include "qwindowdefs.h"
#include "private/qlock_p.h"
#include "qwsregionmanager_qws.h"
#include "qptrqueue.h"
#include "qfile.h"
#include "qtimer.h"
#include "qpen.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#ifndef QT_NO_QWS_MULTIPROCESS
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#ifndef Q_OS_MACX
# include <sys/sem.h>
#endif
#include <sys/param.h>
#include <sys/mount.h>
#endif
#include <signal.h>
#include <fcntl.h>

#if !defined( QT_NO_SOUND ) && !defined( Q_OS_MACX )
#ifdef QT_USE_OLD_QWS_SOUND
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#else
#include "qsoundqss_qws.h"
#endif
#endif

#include "qgfx_qws.h"
#include "qkbddriverfactory_qws.h"
#include "qmousedriverfactory_qws.h"

extern void qt_setMaxWindowRect(const QRect& r);

QWSServer *qwsServer=0;

class QWSServerData {
public:
    QWSServerData()
    {
	screensaverintervals = 0;
	saver = 0;
	cursorClient = 0;
	mouseState = 0;
	deletedWindows.setAutoDelete( TRUE );
    }
    ~QWSServerData()
    {
	deletedWindows.clear();
	delete [] screensaverintervals;
	delete saver;
    }
    QTime screensavertime;
    QTimer* screensavertimer;
    int* screensaverintervals;
    QWSScreenSaver* saver;
    QWSClient *cursorClient;
    int mouseState;
    bool prevWin;
    QPtrList<QWSWindow> deletedWindows;
};

QWSScreenSaver::~QWSScreenSaver()
{
}

extern char *qws_display_spec;
extern void qt_init_display(); //qapplication_qws.cpp
extern QString qws_qtePipeFilename();

extern void qt_client_enqueue(const QWSEvent *); //qapplication_qws.cpp
typedef void MoveRegionF( const QWSRegionMoveCommand*);
typedef void RequestRegionF( int, QRegion );
typedef void SetAltitudeF( const QWSChangeAltitudeCommand* );
extern QPtrQueue<QWSCommand> *qt_get_server_queue();

static QRect maxwindow_rect;
static const char *defaultMouse =
#if defined(QT_QWS_CASSIOPEIA) || defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX) || defined(QT_QWS_YOPY) || defined(QWS_CUSTOMTOUCHPANEL)
    "TPanel"
#else
    "Auto"
#endif
#if defined(QT_QWS_CASSIOPEIA)
    "/dev/tpanel"
#endif
    ;
static const char *defaultKeyboard = "TTY";
static int qws_keyModifiers = 0;

static QWSWindow *keyboardGrabber;
static bool keyboardGrabbing;

static int get_object_id()
{
    static int next=1000;
    return next++;
}
#ifndef QT_NO_QWS_IM
static QWSInputMethod *current_IM = 0;
#endif



//#define QWS_REGION_DEBUG

/*!
    \class QWSWindow qwindowsystem_qws.h
    \brief The QWSWindow class provides server-specific functionality in Qt/Embedded.

    \ingroup qws

    When you run a Qt/Embedded application, it either runs as a server
    or connects to an existing server. If it runs as a server, some
    additional functionality is provided by the QWSServer class.

    This class maintains information about each window and allows
    operations to be performed on the windows.

    You can get the window's name(), caption() and winId(), along with
    the client() that owns the window.

    The region the window wants to draw on is returned by requested();
    the region that the window is allowed to draw on is returned by
    allocation().

    The visibility of the window can be determined using isVisible(),
    isPartiallyObscured() and isFullyObscured(). Visibility can be
    changed using raise(), lower(), show(), hide() and
    setActiveWindow().
*/


/*!
    \fn int QWSWindow::winId() const

    Returns the window's Id.
*/

/*!
    \fn const QString &QWSWindow::name() const

    Returns the window's name.
*/

/*!
    \fn const QString &QWSWindow::caption() const

    Returns the window's caption.
*/

/*!
    \fn QWSClient* QWSWindow::client() const

    Returns the QWSClient that owns this window.
*/

/*!
    \fn QRegion QWSWindow::requested() const

    Returns the region that the window has requested to draw onto,
    including any window decorations.

    \sa allocation()
*/

/*!
    \fn QRegion QWSWindow::allocation() const

    Returns the region that the window is allowed to draw onto,
    including any window decorations but excluding regions covered by
    other windows.

    \sa requested()
*/

/*!
    \fn bool QWSWindow::isVisible() const

    Returns TRUE if the window is visible; otherwise returns FALSE.
*/

/*!
    \fn bool QWSWindow::isPartiallyObscured() const

    Returns TRUE if the window is partially obsured by another window
    or by the bounds of the screen; otherwise returns FALSE.
*/

/*!
    \fn bool QWSWindow::isFullyObscured() const

    Returns TRUE if the window is completely obsured by another window
    or by the bounds of the screen; otherwise returns FALSE.
*/


QWSWindow::QWSWindow(int i, QWSClient* client)
	: id(i), alloc_region_idx(-1), modified(FALSE), needAck(FALSE),
	    onTop(FALSE), c(client), last_focus_time(0)
{
}

/*!
    Raises the window above all other windows except "Stay on top" windows.
*/
void QWSWindow::raise()
{
    qwsServer->raiseWindow( this );
}

/*!
    Lowers the window below other windows.
*/
void QWSWindow::lower()
{
    qwsServer->lowerWindow( this );
}

/*!
    Shows the window.
*/
void QWSWindow::show()
{
    operation( QWSWindowOperationEvent::Show );
}

/*!
    Hides the window.
*/
void QWSWindow::hide()
{
    operation( QWSWindowOperationEvent::Hide );
}

/*!
    Make this the active window (i.e. sets the keyboard focus to this
    window).
*/
void QWSWindow::setActiveWindow()
{
    qwsServer->setFocus( this, TRUE );
}

void QWSWindow::setName( const QString &n )
{
    rgnName = n;
}

void QWSWindow::setCaption( const QString &c )
{
    rgnCaption = c;
}

/*!
  \internal
  Adds \a r to the window's allocated region.
*/
void QWSWindow::addAllocation( QWSRegionManager *rm, const QRegion &r )
{
    QRegion added = r & requested_region;
    if ( !added.isEmpty() ) {
	allocated_region |= added;
	exposed |= added;
	rm->set( alloc_region_idx, allocated_region );
	modified = TRUE;
    }
}

/*!
  \internal
  Removes \a r from the window's allocated region
*/
void QWSWindow::removeAllocation(QWSRegionManager *rm, const QRegion &r)
{
    QRegion nr = allocated_region - r;
    if ( nr != allocated_region ) {
	allocated_region = nr;
	rm->set( alloc_region_idx, allocated_region );
	modified = TRUE;
    } else if ( needAck ) {
	// set our region dirty anyway
	rm->markUpdated( alloc_region_idx );
    }
}

void QWSWindow::updateAllocation()
{
    if ( modified || needAck) {
	c->sendRegionModifyEvent( id, exposed, needAck );
	exposed = QRegion();
	modified = FALSE;
	needAck = FALSE;
    }
}

static int global_focus_time_counter=100;

void QWSWindow::focus(bool get)
{
    if ( get )
	last_focus_time = global_focus_time_counter++;
    QWSFocusEvent event;
    event.simpleData.window = id;
    event.simpleData.get_focus = get;
    c->sendEvent( &event );
}

void QWSWindow::operation( QWSWindowOperationEvent::Operation o )
{
    QWSWindowOperationEvent event;
    event.simpleData.window = id;
    event.simpleData.op = o;
    c->sendEvent( &event );
}

/*!
    Destructor.
*/
QWSWindow::~QWSWindow()
{
}


/*********************************************************************
 *
 * Class: QWSClient
 *
 *********************************************************************/
//always use frame buffer
QWSClient::QWSClient( QObject* parent, int socket, int id )
    : QObject( parent), s(socket), command(0), cid(id)
{
#ifndef QT_NO_QWS_MULTIPROCESS
    if ( socket == -1 ) {
	csocket = 0;
	isClosed = FALSE;
    } else {
	csocket = new QWSSocket( this );
	csocket->setSocket(socket);
	isClosed = FALSE;

	csocket->flush();

	connect( csocket, SIGNAL(readyRead()), this, SIGNAL(readyRead()) );
	connect( csocket, SIGNAL(connectionClosed()), this, SLOT(closeHandler()) );
	connect( csocket, SIGNAL(error(int)), this, SLOT(errorHandler(int)) );
    }
#else
    isClosed = FALSE;
#endif //QT_NO_QWS_MULTIPROCESS
}

QWSClient::~QWSClient()
{
    while(cursors.begin()!=cursors.end()) {
        cursors.remove(cursors.begin());
    }
}

void QWSClient::setIdentity(const QString& i)
{
    id = i;
}

void QWSClient::closeHandler()
{
    isClosed = TRUE;
    emit connectionClosed();
}

void QWSClient::errorHandler( int err )
{
    QString s = "Unknown";
#ifndef QT_NO_QWS_MULTIPROCESS
    switch( err ) {
    case QWSSocket::ErrConnectionRefused:
	s = "Connection Refused";
	break;
    case QWSSocket::ErrHostNotFound:
	s = "Host Not Found";
	break;
    case QWSSocket::ErrSocketRead:
	s = "Socket Read";
	break;
    }
#endif
    //qDebug( "Client %p error %d (%s)", this, err, s.ascii() );
    isClosed = TRUE;
#ifndef QT_NO_QWS_MULTIPROCESS
    if ( csocket )
	csocket->flush(); //####We need to clean out the pipes, this in not the the way.
#endif
    emit connectionClosed();
}

int QWSClient::socket() const
{
    return s;
}

void QWSClient::sendEvent( QWSEvent* event )
{
#ifndef QT_NO_QWS_MULTIPROCESS
    if ( csocket ) {
	if (csocket->state() == QSocket::Connection) {
	    event->write( csocket );
	    csocket->flush();
	}
    }
    else
#endif
    {
	qt_client_enqueue( event );
    }
}

void QWSClient::sendConnectedEvent( const char *display_spec )
{
    QWSConnectedEvent event;
    event.simpleData.window = 0;
    event.simpleData.len = strlen( display_spec ) + 1;
    event.simpleData.clientId = cid;
    char * tmp=(char *)display_spec;
    event.setData( tmp, event.simpleData.len );
    sendEvent( &event );
}

void QWSClient::sendMaxWindowRectEvent()
{
    QWSMaxWindowRectEvent event;
    event.simpleData.window = 0;
    event.simpleData.rect = maxwindow_rect;
    sendEvent(&event);
}

void QWSClient::sendRegionModifyEvent( int winid, QRegion exposed, bool ack )
{
    QWSRegionModifiedEvent event;
    event.simpleData.window = winid;
    event.simpleData.nrectangles = exposed.rects().count();
    event.simpleData.is_ack = ack;
    event.setData( (char *)exposed.rects().data(),
		    exposed.rects().count() * sizeof(QRect), FALSE );

//    qDebug( "Sending %d %d rects ack: %d", winid, event.simpleData.nrectangles, ack );
    sendEvent( &event );
}

#ifndef QT_NO_QWS_PROPERTIES
void QWSClient::sendPropertyNotifyEvent( int property, int state )
{
    QWSPropertyNotifyEvent event;
    event.simpleData.window = 0; // not used yet
    event.simpleData.property = property;
    event.simpleData.state = state;
    sendEvent( &event );
}

void QWSClient::sendPropertyReplyEvent( int property, int len, char *data )
{
    QWSPropertyReplyEvent event;
    event.simpleData.window = 0; // not used yet
    event.simpleData.property = property;
    event.simpleData.len = len;
    event.setData( data, len );
    sendEvent( &event );
}
#endif //QT_NO_QWS_PROPERTIES
void QWSClient::sendSelectionClearEvent( int windowid )
{
    QWSSelectionClearEvent event;
    event.simpleData.window = windowid;
    sendEvent( &event );
}

void QWSClient::sendSelectionRequestEvent( QWSConvertSelectionCommand *cmd, int windowid )
{
    QWSSelectionRequestEvent event;
    event.simpleData.window = windowid;
    event.simpleData.requestor = cmd->simpleData.requestor;
    event.simpleData.property = cmd->simpleData.selection;
    event.simpleData.mimeTypes = cmd->simpleData.mimeTypes;
    sendEvent( &event );
}

#if !defined( QT_NO_SOUND ) && !defined( Q_OS_MACX )
#ifdef QT_USE_OLD_QWS_SOUND

	/*
	***
	****
*************
**********
**********  WARNING:  This code is obsoleted by tests/qsound,
**********            which will soobn be used instead.
*************
	****
	***
	*/

struct QRiffChunk {
    char id[4];
    Q_UINT32 size;
    char data[4/*size*/];
};

//#define QT_QWS_SOUND_8BIT
static const int sound_fragment_size = 8;
static const int sound_stereo = 0;
static const int sound_speed = 11025;

static const int sound_buffer_size=1<<sound_fragment_size;
class QWSSoundServerBucket {
public:
    QWSSoundServerBucket(QIODevice* d)
    {
	dev = d;
	out = 0;
	chunk_remaining = 0;
	available = readSamples(data,sound_buffer_size); // preload some
    }
    ~QWSSoundServerBucket()
    {
	delete dev;
    }
    int max() const
    {
	return available;
    }
    void add(int* mix, int count)
    {
	Q_ASSERT(available>=count);
	available -= count;
	while ( count-- ) {
	    *mix++ += ((int)data[out] - 128)*128;
	    if ( ++out == sound_buffer_size )
		out = 0;
	}
    }
    void rewind(int count)
    {
	Q_ASSERT(count<sound_buffer_size);
	out = (out + (sound_buffer_size-count))%sound_buffer_size;
	available += count;
    }
    bool refill()
    {
	int toread = sound_buffer_size - available;
	int in = (out + available)%sound_buffer_size;
	int a = sound_buffer_size-in; if ( a > toread ) a = toread;
	int rd = a ? readSamples(data+in,a) : 0;
	if ( rd < 0 )
	    return FALSE; // ############
	int b = toread - rd;
	if ( b ) {
	    int r = readSamples(data,b);
	    if ( r > 0 )
		rd += r;
	}
	available += rd;
	return rd > 0;
    }
private:
    int readSamples(uchar* dst, int count)
    {
	if ( chunk_remaining < 0 )
	    return 0; // in error state
	for ( ;; ) {
	    if ( chunk_remaining > 0 ) {
		if ( count > chunk_remaining )
		    count = chunk_remaining;
		chunk_remaining -= count;
		return dev->readBlock((char*)dst, count);
	    } else {
		chunk_remaining = -1;
		// Keep reading chunks...
		const int n = sizeof(chunk)-sizeof(chunk.data);
		if ( dev->readBlock((char*)&chunk,n) != n ) {
		    return 0;
		}
		if ( qstrncmp(chunk.id,"data",4) == 0 ) {
		    chunk_remaining = chunk.size;
		} else if ( qstrncmp(chunk.id,"RIFF",4) == 0 ) {
		    char d[4];
		    dev->readBlock(d,4);
		    if ( qstrncmp(d,"WAVE",4) != 0 ) {
			return 0;
		    }
		} else if ( qstrncmp(chunk.id,"fmt ",4) == 0 ) {
		    struct {
			#define WAVE_FORMAT_PCM 1
			Q_INT16 formatTag;
			Q_INT16 channels;
			Q_INT32 samplesPerSec;
			Q_INT32 avgBytesPerSec;
			Q_INT16 blockAlign;
		    } chunkdata;
		    if ( dev->readBlock((char*)&chunkdata,sizeof(chunkdata)) != sizeof(chunkdata) ) {
			qDebug("WAV file: UNSUPPORTED SIZE");
			return 0;
		    }
		    if ( chunkdata.formatTag != WAVE_FORMAT_PCM ) {
			qDebug("WAV file: UNSUPPORTED FORMAT");
			return 0;
		    }
		    if ( chunkdata.channels != sound_stereo+1 ) {
			qDebug("WAV file: UNSUPPORTED CHANNELS");
			return 0;
		    }
		    /* Ignore
		    if ( chunkdata.samplesPerSec != sound_speed ) {
			return 0;
		    }
		    */
		} else {
		    // ignored chunk
		    if ( !dev->at(dev->at()+chunk.size) ) {
			return 0;
		    }
		}
	    }
	}
    }
    QRiffChunk chunk;
    int chunk_remaining;

    QIODevice* dev;
    uchar data[sound_buffer_size];
    int available,out;
};

class QWSSoundServerData {
public:
    QWSSoundServerData(QWSSoundServer* s)
    {
	active.setAutoDelete(TRUE);
	sn = 0;
	server = s;
    }

    void feedDevice(int fd)
    {
	QWSSoundServerBucket* bucket;
	int available = sound_buffer_size;
	int n = 0;
	QPtrListIterator<QWSSoundServerBucket> it(active);
	for (; (bucket = *it);) {
	    ++it;
	    int m = bucket->max();
	    if ( m ) {
		if ( m < available )
		    available = m;
		n++;
	    } else {
		active.removeRef(bucket);
	    }
	}
	if ( n ) {
	    int data[sound_buffer_size];
	    for (int i=0; i<available; i++)
		data[i]=0;
	    for (bucket = active.first(); bucket; bucket = active.next()) {
		bucket->add(data,available);
	    }
#ifdef QT_QWS_SOUND_8BIT
	    signed char d8[sound_buffer_size];
	    for (int i=0; i<available; i++) {
		int t = data[i] / 1; // ######### configurable
		if ( t > 127 ) t = 127;
		if ( t < -128 ) t = -128;
		d8[i] = (signed char)t;
	    }
	    int w = ::write(fd,d8,available);
#else
	    short d16[sound_buffer_size];
	    for (int i=0; i<available; i++) {
		int t = data[i]; // ######### configurable
		if ( t > 32767 ) t = 32767;
		if ( t < -32768 ) t = -32768;
		d16[i] = (short)t;
		//d16[i] = ((t&0xff)<<8)|((t>>8)&0xff);
	    }
	    int w = ::write(fd,(char*)d16,available*2)/2;
#endif
	    if ( w < 0 )
		return; // ##############
	    int rw = available - w;
	    if ( rw ) {
		for (bucket = active.first(); bucket; bucket = active.next()) {
		    bucket->rewind(rw);
		    bucket->refill();
		}
	    }
	    for (bucket = active.first(); bucket; bucket = active.next()) {
		if ( !bucket->refill() ) {
		    //active.remove(bucket);
		}
	    }
	} else {
	    closeDevice();
	}
    }

    void playFile(const QString& filename)
    {
	QFile* f = new QFile(filename);
	f->open(IO_ReadOnly);
	active.append(new QWSSoundServerBucket(f));
	openDevice();
    }

private:
    void openDevice()
    {
	if ( !sn ) {
	    int fd = ::open("/dev/dsp",O_RDWR);
	    if ( fd < 0 ) {
		// For debugging purposes - defined QT_NO_SOUND if you
		// don't have sound hardware!
		fd = ::open("/tmp/dsp",O_WRONLY);
	    }

	    // Setup soundcard at 16 bit mono
	    int v;
	    v=0x00040000+sound_fragment_size; ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &v);
#ifdef QT_QWS_SOUND_8BIT
	    v=AFMT_U8; ioctl(fd, SNDCTL_DSP_SETFMT, &v);
#else
	    v=AFMT_S16_LE; ioctl(fd, SNDCTL_DSP_SETFMT, &v);
#endif
	    v=sound_stereo; ioctl(fd, SNDCTL_DSP_STEREO, &v);
	    v=sound_speed; ioctl(fd, SNDCTL_DSP_SPEED, &v);

	    sn = new QSocketNotifier(fd,QSocketNotifier::Write,server);
	    QObject::connect(sn,SIGNAL(activated(int)),server,SLOT(feedDevice(int)));
	}
    }

    void closeDevice()
    {
	if ( sn ) {
	    ::close(sn->socket());
	    delete sn;
	    sn = 0;
	}
    }

    QPtrList<QWSSoundServerBucket> active;
    QSocketNotifier* sn;
    QWSSoundServer* server;
};

QWSSoundServer::QWSSoundServer(QObject* parent) :
    QObject(parent)
{
    d = new QWSSoundServerData(this);
}

QWSSoundServer::~QWSSoundServer()
{
    delete d;
}

void QWSSoundServer::playFile(const QString& filename)
{
    d->playFile(filename);
}

void QWSSoundServer::feedDevice(int fd)
{
    d->feedDevice(fd);
}

#endif
#endif

/*********************************************************************
 *
 * Class: QWSServer
 *
 *********************************************************************/


struct QWSCommandStruct
{
    QWSCommandStruct( QWSCommand *c, QWSClient *cl ) :command(c),client(cl){}
    QWSCommand *command;
    QWSClient *client;
};




static void ignoreSignal( int )
{
}

/*!
    \class QWSServer qwindowsystem_qws.h
    \brief The QWSServer class provides server-specific functionality in Qt/Embedded.

    \ingroup qws

    When you run a Qt/Embedded application, it either runs as a server
    or connects to an existing server. If it runs as a server, some
    additional operations are provided by this class.

    This class is instantiated by QApplication for Qt/Embedded server
    processes. You should never construct this class yourself.

    A pointer to the QWSServer instance can be obtained via the global
    \c qwsServer variable.

    The mouse and keyboard devices can be opened with openMouse() and
    openKeyboard(). (Close them with closeMouse() and
    closeKeyboard().)

    The display is refreshed with refresh(), and painting can be
    enabled or disabled with enablePainting().

    Obtain the list of client windows with clientWindows() and find
    out which window is at a particular point with windowAt().

    Many static functions are provided, for example,
    setKeyboardFilter(), setKeyboardHandler(), setDefaultKeyboard()
    and setDefaultMouse().

    The size of the window rectangle can be set with
    setMaxWindowRect(), and the desktop's background can be set with
    setDesktopBackground().

    The screen saver is controlled with setScreenSaverInterval() and
    screenSaverActivate().
*/

/*!
    \enum QWSServer::IMState

    \internal

    \value IMStart  Starting to compose.
    \value IMCompose Composing.
    \value IMEnd Finished composing.
*/

/*!
    \fn const QPtrList<QWSWindow> &QWSServer::clientWindows()

    Returns the list of top-level windows. This list will change as
    applications add and remove wigdets so it should not be stored for
    future use. The windows are sorted in stacking order from
    top-most to bottom-most.
*/

/*!
    \fn void QWSServer::newChannel(const QString& channel)

    This signal is emitted when the QCopChannel \a channel is created.
*/

/*!
    \fn void QWSServer::removedChannel(const QString& channel)

    This signal is emitted immediately after the QCopChannel \a
    channel is destroyed. Note that a channel is not destroyed until
    all its listeners have unregistered.
*/

/*!
    Construct a QWSServer object.

    \warning This class is instantiated by QApplication for
    Qt/Embedded server processes. You should never construct this
    class yourself.

    The \a flags are used for keyboard and mouse setting. The server's
    parent is \a parent and it is called \a name.
*/

QWSServer::QWSServer( int flags, QObject *parent, const char *name ) :
#ifndef QT_NO_QWS_MULTIPROCESS
    QWSServerSocket(qws_qtePipeFilename(),16,parent,name),
#else
    QObject( parent, name ),
#endif
    disablePainting(FALSE)
{
    d = new QWSServerData;
    Q_ASSERT( !qwsServer );
    qwsServer = this;

#ifndef QT_NO_QWS_MULTIPROCESS
    QString pipe = qws_qtePipeFilename();

    if ( !ok() ) {
	perror("Error");
	qFatal("Failed to bind to %s", pipe.latin1() );
    } else {
	struct linger tmp;
	tmp.l_onoff=1;
	tmp.l_linger=0;
	setsockopt(socket(),SOL_SOCKET,SO_LINGER,(char *)&tmp,sizeof(tmp));
    }

    signal(SIGPIPE, ignoreSignal); //we get it when we read
#endif
    focusw = 0;
    mouseGrabber = 0;
    mouseGrabbing = FALSE;
    keyboardGrabber = 0;
    keyboardGrabbing = FALSE;
#ifndef QT_NO_QWS_CURSOR
    haveviscurs = FALSE;
    cursor = 0;
    nextCursor = 0;
#endif

#ifndef QT_NO_QWS_MULTIPROCESS

    if ( !geteuid() ) {
#if !defined(Q_OS_FREEBSD) && !defined(Q_OS_SOLARIS) && !defined(Q_OS_MACX)
	if( mount(0,"/var/shm", "shm", 0, 0) ) {
	    /* This just confuses people with 2.2 kernels
	    if ( errno != EBUSY )
		qDebug("Failed mounting shm fs on /var/shm: %s",strerror(errno));
	    */
	}
#endif
    }
#endif

    // no selection yet
    selectionOwner.windowid = -1;
    selectionOwner.time.set( -1, -1, -1, -1 );

    openDisplay();

    d->screensavertimer = new QTimer(this);
    connect( d->screensavertimer, SIGNAL(timeout()), this, SLOT(screenSaverTimeout()) );
    screenSaverWake();

    client[-1] = new QWSClient( this, -1, 0 );

    // input devices
    if ( !(flags&DisableMouse) ) {
	openMouse();
    }
    initializeCursor();

#ifndef QT_NO_QWS_KEYBOARD
    if ( !(flags&DisableKeyboard) ) {
	openKeyboard();
    }
#endif
    if ( !bgColor )
	bgColor = new QColor( 0x20, 0xb0, 0x50 );
    screenRegion = QRegion( 0, 0, swidth, sheight );
    paintBackground( screenRegion );

#if !defined( QT_NO_SOUND ) && !defined( Q_OS_MACX )
    soundserver = new QWSSoundServer(this);
#endif
#ifndef QT_NO_QWS_IM
    microF = FALSE;
#endif
}

/*!
    Destruct QWSServer
*/
QWSServer::~QWSServer()
{
    // destroy all clients
    for (ClientIterator it = client.begin(); it != client.end(); ++it )
	delete *it;

    windows.setAutoDelete(TRUE);
    windows.clear();

    delete bgColor;
    bgColor = 0;
    closeDisplay();
    closeMouse();
#ifndef QT_NO_QWS_KEYBOARD
    closeKeyboard();
#endif
    delete d;
}

/*!
  \internal
*/
void QWSServer::releaseMouse(QWSWindow* w)
{
    if ( w && mouseGrabber == w ) {
	mouseGrabber = 0;
	mouseGrabbing = FALSE;
#ifndef QT_NO_QWS_CURSOR
	if (nextCursor) {
	    // Not grabbing -> set the correct cursor
	    setCursor(nextCursor);
	    nextCursor = 0;
	}
#endif
    }
}

/*!
  \internal
*/
void QWSServer::releaseKeyboard(QWSWindow* w)
{
    if ( keyboardGrabber == w ) {
	keyboardGrabber = 0;
	keyboardGrabbing = FALSE;
    }
}


#ifndef QT_NO_QWS_MULTIPROCESS
/*!
  \internal
*/
void QWSServer::newConnection( int socket )
{
    client[socket] = new QWSClient(this,socket, get_object_id());
    connect( client[socket], SIGNAL(readyRead()),
	     this, SLOT(doClient()) );
    connect( client[socket], SIGNAL(connectionClosed()),
	     this, SLOT(clientClosed()) );

    client[socket]->sendConnectedEvent( qws_display_spec );

    if ( !maxwindow_rect.isEmpty() )
	client[socket]->sendMaxWindowRectEvent();

    // pre-provide some object id's
    for (int i=0; i<20 && client[socket]; i++)
	invokeCreate(0,client[socket]);
}

/*!
  \internal
*/
void QWSServer::clientClosed()
{
    QWSClient* cl = (QWSClient*)sender();

    // Remove any queued commands for this client
    QPtrListIterator<QWSCommandStruct> it(commandQueue);
    QWSCommandStruct *cs;
    while ((cs = *it) != 0) {
	if (cs->client == cl) {
	    commandQueue.removeRef(cs);
	    delete cs;
	} else {
	    ++it;
	}
    }

#ifndef QT_NO_COP
    // Enfore unsubscription from all channels.
    QCopChannel::detach( cl );
#endif

    QRegion exposed;
    {
	// Shut down all windows for this client
	QPtrListIterator<QWSWindow> it( windows );
	QWSWindow* w;
	while (( w = it.current() )) {
	    ++it;
	    if ( w->forClient(cl) )
		w->shuttingDown();
	}
    }
    {
	// Delete all windows for this client
	QPtrListIterator<QWSWindow> it( windows );
	QWSWindow* w;
	while (( w = it.current() )) {
	    ++it;
	    if ( w->forClient(cl) ) {
		releaseMouse(w);
		releaseKeyboard(w);
		exposed += w->allocation();
		rgnMan->remove( w->allocationIndex() );
		if ( focusw == w )
		    setFocus(focusw,0);
		windows.removeRef(w);
#ifndef QT_NO_QWS_PROPERTIES
		manager()->removeProperties( w->winId() );
#endif
		emit windowEvent( w, Destroy );
		d->deletedWindows.append(w);
	    } 
	}
	if ( d->deletedWindows.count() )
	    QTimer::singleShot(0, this, SLOT(deleteWindowsLater()) );
    }
    client.remove( cl->socket() );
    if ( cl == d->cursorClient )
	d->cursorClient = 0;
    if ( qt_screen->clearCacheFunc )
	(qt_screen->clearCacheFunc)( qt_screen, cl->clientId() );  // remove any remaining cache entries.
    cl->deleteLater();
    exposeRegion( exposed );
    syncRegions();
}

void QWSServer::deleteWindowsLater()
{
    d->deletedWindows.clear();
}

#endif //QT_NO_QWS_MULTIPROCESS


QWSCommand* QWSClient::readMoreCommand()
{
#ifndef QT_NO_QWS_MULTIPROCESS
    if ( csocket ) {
	// read next command
	if ( !command ) {
	    int command_type = qws_read_uint( csocket );

	    if ( command_type>=0 ) {
		command = QWSCommand::factory( command_type );
	    }
	}

	if ( command ) {
	    if ( command->read( csocket ) ) {
		// Finished reading a whole command.
		QWSCommand* result = command;
		command = 0;
		return result;
	    }
	}

	// Not finished reading a whole command.
	return 0;
    }
    else
#endif
    {
	return qt_get_server_queue()->dequeue();
    }

}


/*!
  \internal
*/
void QWSServer::processEventQueue()
{
    if ( qwsServer )
	qwsServer->doClient( qwsServer->client[-1] );
}


#ifndef QT_NO_QWS_MULTIPROCESS
void QWSServer::doClient()
{
    static bool active = FALSE;
    if (active) {
	qDebug( "QWSServer::doClient() reentrant call, ignoring" );
	return;
    }
    active = TRUE;
    QWSClient* client = (QWSClient*)sender();
    doClient( client );
    active = FALSE;

#ifndef QT_NO_QWS_IM
    //### Avoid reentrancy problems when the IM tries to
    //do top-level widget operations (eg. move()) as a response to 
    // setMicroFocus()
    //### I hope we can find a cleaner way to do this.
    if ( microF && current_IM ) {
      current_IM->setMicroFocus( microX, microY );
      microF = FALSE;
    }
#endif
}
#endif

void QWSServer::doClient( QWSClient *client )
{
    QWSCommand* command=client->readMoreCommand();

    while ( command ) {
	QWSCommandStruct *cs = new QWSCommandStruct( command, client );
	commandQueue.append( cs );
	// Try for some more...
	command=client->readMoreCommand();
    }


    while ( !commandQueue.isEmpty() ) {
	commandQueue.first();
	QWSCommandStruct *cs = commandQueue.take();
	switch ( cs->command->type ) {
	case QWSCommand::Identify:
	    invokeIdentify( (QWSIdentifyCommand*)cs->command, cs->client );
	    break;
	case QWSCommand::Create:
	    invokeCreate( (QWSCreateCommand*)cs->command, cs->client );
	    break;
	case QWSCommand::RegionName:
	    invokeRegionName( (QWSRegionNameCommand*)cs->command, cs->client );
	    break;
	case QWSCommand::Region:
	    invokeRegion( (QWSRegionCommand*)cs->command, cs->client );
	    break;
	case QWSCommand::RegionMove:
	    invokeRegionMove( (QWSRegionMoveCommand*)cs->command, cs->client );
	    break;
	case QWSCommand::RegionDestroy:
	    invokeRegionDestroy( (QWSRegionDestroyCommand*)cs->command, cs->client );
	    break;
#ifndef QT_NO_QWS_PROPERTIES
	case QWSCommand::AddProperty:
	    invokeAddProperty( (QWSAddPropertyCommand*)cs->command );
	    break;
	case QWSCommand::SetProperty:
	    invokeSetProperty( (QWSSetPropertyCommand*)cs->command );
	    break;
	case QWSCommand::RemoveProperty:
	    invokeRemoveProperty( (QWSRemovePropertyCommand*)cs->command );
	    break;
	case QWSCommand::GetProperty:
	    invokeGetProperty( (QWSGetPropertyCommand*)cs->command, cs->client );
	    break;
#endif
	case QWSCommand::SetSelectionOwner:
	    invokeSetSelectionOwner( (QWSSetSelectionOwnerCommand*)cs->command );
	    break;
	case QWSCommand::RequestFocus:
	    invokeSetFocus( (QWSRequestFocusCommand*)cs->command, cs->client );
	    break;
	case QWSCommand::ChangeAltitude:
	    invokeSetAltitude( (QWSChangeAltitudeCommand*)cs->command,
			       cs->client );
	    break;
#ifndef QT_NO_QWS_CURSOR
	case QWSCommand::DefineCursor:
	    invokeDefineCursor( (QWSDefineCursorCommand*)cs->command, cs->client );
	    break;
	case QWSCommand::SelectCursor:
	    invokeSelectCursor( (QWSSelectCursorCommand*)cs->command, cs->client );
	    break;
#endif
	case QWSCommand::GrabMouse:
	    invokeGrabMouse( (QWSGrabMouseCommand*)cs->command, cs->client );
	    break;
	case QWSCommand::GrabKeyboard:
	    invokeGrabKeyboard( (QWSGrabKeyboardCommand*)cs->command, cs->client );
	    break;
#if !defined( QT_NO_SOUND ) && !defined( Q_OS_MACX )
	case QWSCommand::PlaySound:
	    invokePlaySound( (QWSPlaySoundCommand*)cs->command, cs->client );
	    break;
#endif
#ifndef QT_NO_COP
	case QWSCommand::QCopRegisterChannel:
	    invokeRegisterChannel( (QWSQCopRegisterChannelCommand*)cs->command,
				   cs->client );
	    break;
	case QWSCommand::QCopSend:
	    invokeQCopSend( (QWSQCopSendCommand*)cs->command, cs->client );
	    break;
#endif
#ifndef QT_NO_QWS_IM
	case QWSCommand::ResetIM:
	    invokeResetIM( (QWSResetIMCommand*)cs->command, cs->client );
	    break;
	case QWSCommand::SetMicroFocus:
	    //invokeSetMicroFocus( (QWSSetMicroFocusCommand*)cs->command, cs->client );
	    {
		QWSSetMicroFocusCommand *cmd = (QWSSetMicroFocusCommand*)cs->command;
		microF = TRUE;
		microX = cmd->simpleData.x;
		microY = cmd->simpleData.y;
	    }
	    break;
#endif
	case QWSCommand::RepaintRegion:
	    invokeRepaintRegion((QWSRepaintRegionCommand*)cs->command,
				cs->client);
	    break;
	}
	delete cs->command;
	delete cs;
    }
}


void QWSServer::showCursor()
{
#ifndef QT_NO_QWS_CURSOR
    qt_screencursor->show();
#endif
}

void QWSServer::hideCursor()
{
#ifndef QT_NO_QWS_CURSOR
    qt_screencursor->hide();
#endif
}

/*!
    If \a e is TRUE, painting on the display is enabled; if \a e is
    FALSE, painting is disabled.
*/

void QWSServer::enablePainting(bool e)
{
// ### don't like this
    if (e)
    {
	disablePainting = FALSE;
	setWindowRegion( 0, QRegion() );
	showCursor();
	syncRegions();
    }
    else
    {
	disablePainting = TRUE;
	hideCursor();
	setWindowRegion( 0, QRegion(0,0,swidth,sheight) );
	syncRegions();
    }
}

/*!
    Refreshes the entire display.
*/
void QWSServer::refresh()
{
    exposeRegion( QRegion(0,0,swidth,sheight) );
    syncRegions();
}
		   
/*!
    \overload

    Refreshes the region \a r.
*/
void QWSServer::refresh(QRegion & r)
{
    exposeRegion(r);
    syncRegions();
}

/*!
    Sets the area of the screen which Qt/Embedded applications will
    consider to be the maximum area to use for windows to \a r.

    \sa QWidget::showMaximized()
*/

void QWSServer::setMaxWindowRect(const QRect& r)
{
    QRect tr = qt_screen->mapToDevice(r,
	QSize(qt_screen->width(),qt_screen->height()));
    if ( maxwindow_rect != tr ) {
	maxwindow_rect = tr;
	qwsServer->sendMaxWindowRectEvents();
    }
}

/*!
  \internal
*/
void QWSServer::sendMaxWindowRectEvents()
{
    for (ClientIterator it = client.begin(); it != client.end(); ++it )
	(*it)->sendMaxWindowRectEvent();
}

/*!
    Set the mouse driver \a m to use if \c $QWS_MOUSE_PROTO is not
    defined. The default is platform-dependent.
*/
void QWSServer::setDefaultMouse( const char *m )
{
    defaultMouse = m;
}

/*!
    Set the keyboard driver to \a k, e.g. if \c $QWS_KEYBOARD is not
    defined. The default is platform-dependent.
*/

void QWSServer::setDefaultKeyboard( const char *k )
{
    defaultKeyboard = k;
}

static bool prevWin;

/*!
    \internal

  Send a mouse event. \a pos is the screen position where the mouse
  event occurred and \a state is a mask indicating which buttons are
  pressed.
*/
void QWSServer::sendMouseEvent(const QPoint& pos, int state)
{
    const int btnMask = LeftButton | RightButton | MidButton;
    qwsServer->showCursor();

    if ( state )
	qwsServer->screenSaverWake();

    mousePosition = pos;
    qwsServer->d->mouseState = state;

    QWSMouseEvent event;

    //If grabbing window disappears, grab is still active until
    //after mouse release.
    QWSWindow *win = qwsServer->mouseGrabber ? qwsServer->mouseGrabber : qwsServer->windowAt( pos );
    event.simpleData.window = win ? win->id : 0;

#ifndef QT_NO_QWS_CURSOR
    qt_screencursor->move(pos.x(),pos.y());

    // Arrow cursor over desktop
    // prevWin remembers if the last event was over a window
    if (!win && prevWin) {
	if ( !qwsServer->mouseGrabber )
	    qwsServer->setCursor(QWSCursor::systemCursor(ArrowCursor));
	else
	    qwsServer->nextCursor = QWSCursor::systemCursor(ArrowCursor);
	prevWin = FALSE;
    }
    // reset prevWin
    if (win && !prevWin)
	prevWin = TRUE;
#endif

    if ( (state&btnMask) && !qwsServer->mouseGrabbing ) {
	qwsServer->mouseGrabber = win;
    }

    event.simpleData.x_root=pos.x();
    event.simpleData.y_root=pos.y();
    event.simpleData.state=state | qws_keyModifiers;
    event.simpleData.time=qwsServer->timer.elapsed();

    QWSClient *serverClient = qwsServer->client[-1];
    QWSClient *winClient = win ? win->client() : 0;

    if ( serverClient )
       serverClient->sendEvent( &event );
    if ( winClient && winClient != serverClient )
       winClient->sendEvent( &event );

    // Make sure that if we leave a window, that window gets one last mouse
    // event so that it knows the mouse has left.
    QWSClient *oldClient = qwsServer->d->cursorClient;
    if ( oldClient && oldClient != winClient && oldClient != serverClient )
	oldClient->sendEvent( &event );

    qwsServer->d->cursorClient = winClient;

    if ( !(state&btnMask) && !qwsServer->mouseGrabbing )
	qwsServer->releaseMouse(qwsServer->mouseGrabber);
}

/*!
    Returns the primary mouse handler.
*/
QWSMouseHandler *QWSServer::mouseHandler()
{
    return qwsServer->mousehandlers.first();
}

// called by QWSMouseHandler constructor, not user code.
/*!
  \internal
*/
void QWSServer::setMouseHandler(QWSMouseHandler* mh)
{
    qwsServer->mousehandlers.prepend(mh);
}

/*!
  \internal
*/
QPtrList<QWSInternalWindowInfo> * QWSServer::windowList()
{
    QPtrList<QWSInternalWindowInfo> * ret=new QPtrList<QWSInternalWindowInfo>;
    ret->setAutoDelete(TRUE);
    QWSWindow * window;
    for(window=qwsServer->windows.first();window!=0;
	window=qwsServer->windows.next()) {
	QWSInternalWindowInfo * qwi=new QWSInternalWindowInfo();
	qwi->winid=window->winId();
	qwi->clientid=window->client()->clientId();
#ifndef QT_NO_QWS_PROPERTIES
	char * name;
	int len;
	qwsServer->propertyManager.getProperty(qwi->winid,
					       QT_QWS_PROPERTY_WINDOWNAME,
					       name,len);
	if(name) {
	    char * buf=(char *)malloc(len+2);
	    strncpy(buf,name,len);
	    buf[len]=0;
	    qwi->name=buf;
	    free(buf);
	} else {
	    qwi->name="unknown";
	}
#else
	qwi->name="unknown";
#endif
	ret->append(qwi);
    }

    return ret;
}

#ifndef QT_NO_COP
/*!
  \internal
*/
void QWSServer::sendQCopEvent( QWSClient *c, const QCString &ch,
			       const QCString &msg, const QByteArray &data,
			       bool response )
{
    Q_ASSERT( c );

    QWSQCopMessageEvent event;
    event.simpleData.is_response = response;
    event.simpleData.lchannel = ch.length();
    event.simpleData.lmessage = msg.length();
    event.simpleData.ldata = data.size();
    int l = event.simpleData.lchannel + event.simpleData.lmessage +
	    event.simpleData.ldata;

    // combine channel, message and data into one block of raw bytes
    QByteArray raw( l );
    char *d = (char*)raw.data();
    memcpy( d, ch.data(), event.simpleData.lchannel );
    d += event.simpleData.lchannel;
    memcpy( d, msg.data(), event.simpleData.lmessage );
    d += event.simpleData.lmessage;
    memcpy( d, data.data(), event.simpleData.ldata );

    event.setData( raw.data(), l );

    c->sendEvent( &event );
}
#endif

/*!
    Returns the window containing the point \a pos or 0 if there is no
    window under the point.
*/

QWSWindow *QWSServer::windowAt( const QPoint& pos )
{
    for (uint i=0; i<windows.count(); i++) {
	QWSWindow* w = windows.at(i);
	if ( w->requested_region.contains( pos ) )
	    return w;
    }
    return 0;
}

static int keyUnicode(int keycode)
{
    int code = 0xffff;

    if ( keycode >= Qt::Key_A && keycode <= Qt::Key_Z )
	code = keycode - Qt::Key_A + 'a';
    else if ( keycode >= Qt::Key_0 && keycode <= Qt::Key_9 )
	code = keycode - Qt::Key_0 + '0';

    return code;
}

/*!
    Send a key event. You can use this to send key events generated by
    "virtual keyboards". \a unicode is the Unicode value of the key to
    send, \a keycode the Qt keycode (e.g. \c Key_Left), \a modifiers
    indicates whether, Shift/Alt/Ctrl keys are pressed, \a isPress is
    TRUE if this is a key down event and FALSE if it's a key up event,
    and \a autoRepeat is TRUE if this is an autorepeat event (i.e. the
    user has held the key down and this is the second or subsequent
    key event being sent).
*/

void QWSServer::sendKeyEvent(int unicode, int keycode, int modifiers, bool isPress,
  bool autoRepeat)
{
#ifndef QT_NO_QWS_IM

    if ( !current_IM || !current_IM->filter( unicode, keycode, modifiers, 
					     isPress, autoRepeat ) )
	sendKeyEventUnfiltered( unicode, keycode, modifiers, 
				isPress, autoRepeat);
}

void QWSServer::sendKeyEventUnfiltered(int unicode, int keycode, int modifiers, bool isPress,
  bool autoRepeat)
{
#endif
    if ( isPress ) {
	if ( keycode != Key_F34 && keycode != Key_F35 )
	    qwsServer->screenSaverWake();
    }

    qws_keyModifiers = modifiers;

    QWSKeyEvent event;
    QWSWindow *win = keyboardGrabber ? keyboardGrabber :
	qwsServer->focusw;

    event.simpleData.window = win ? win->winId() : 0;

    event.simpleData.unicode =
#ifndef QT_NO_QWS_KEYBOARD
	unicode < 0 ? keyUnicode(keycode) :
#endif
	unicode;
    event.simpleData.keycode = keycode;
    event.simpleData.modifiers = modifiers;
    event.simpleData.is_press = isPress;
    event.simpleData.is_auto_repeat = autoRepeat;

    for (ClientIterator it = qwsServer->client.begin(); it != qwsServer->client.end(); ++it ) {
	(*it)->sendEvent(&event);
    }
}

/*!
    \internal
*/
void QWSServer::beginDisplayReconfigure()
{
    qwsServer->enablePainting( FALSE );
#ifndef QT_NO_QWS_CURSOR
    qt_screencursor->hide();
#endif
    QWSDisplay::grab( TRUE );
    qt_screen->disconnect();
}

/*!
    \internal
*/
void QWSServer::endDisplayReconfigure()
{
    delete qwsServer->gfx;
    qt_screen->connect( QString::null );
    qwsServer->swidth = qt_screen->deviceWidth();
    qwsServer->sheight = qt_screen->deviceHeight();
    qwsServer->screenRegion = QRegion( 0, 0, qwsServer->swidth, qwsServer->sheight );
    qwsServer->gfx = qt_screen->screenGfx();
    QWSDisplay::ungrab();
#ifndef QT_NO_QWS_CURSOR
    qt_screencursor->show();
#endif
    qt_setMaxWindowRect( QRect(0, 0, qt_screen->deviceWidth(), qt_screen->deviceHeight()) );
    QSize olds = qApp->desktop()->size();
    qApp->desktop()->resize( qt_screen->width(), qt_screen->height() );
    qApp->postEvent( qApp->desktop(), new QResizeEvent( qApp->desktop()->size(), olds ) );
    qwsServer->enablePainting( TRUE );
    qwsServer->refresh();
    qDebug( "Desktop size: %dx%d", qApp->desktop()->width(), qApp->desktop()->height() );
}

void QWSServer::resetGfx()
{
#ifndef QT_NO_QWS_CURSOR
    qt_screencursor->hide();
    qt_screencursor->show();
#endif
    delete qwsServer->gfx;
    qwsServer->gfx = qt_screen->screenGfx();
}

#ifndef QT_NO_QWS_CURSOR
/*!
    If \a vis is TRUE, makes the cursor visible; if \a vis is FALSE,
    makes the cursor invisible.

    \sa isCursorVisible()
*/
void QWSServer::setCursorVisible( bool vis )
{
    if ( qwsServer && qwsServer->haveviscurs != vis ) {
	QWSCursor* c = qwsServer->cursor;
	qwsServer->setCursor(QWSCursor::systemCursor(BlankCursor));
	qwsServer->haveviscurs = vis;
	qwsServer->setCursor(c);
    }
}

/*!
    Returns TRUE if the cursor is visible; otherwise returns FALSE.

    \sa setCursorVisible()
*/
bool QWSServer::isCursorVisible()
{
    return qwsServer ? qwsServer->haveviscurs : TRUE;
}
#endif

#ifndef QT_NO_QWS_IM

/*!
    This function sends an input method event to the server. The
    current state is passed in \a state and the current text in \a
    txt. The cursor's position in the text is given by \a cpos, and
    the selection length (which could be 0) is given in \a selLen.
*/
void QWSServer::sendIMEvent( IMState state, const QString& txt, int cpos, int selLen )
{
    QWSIMEvent event;

    QWSWindow *win = keyboardGrabber ? keyboardGrabber :
	qwsServer->focusw;

    event.simpleData.window = win ? win->winId() : 0;
    event.simpleData.type = state;
    event.simpleData.cpos = cpos;
    event.simpleData.selLen = selLen;
    event.simpleData.textLen = txt.length();

    char * tmp=(char *)txt.unicode();
    event.setData( tmp, event.simpleData.textLen*2 );

    QWSClient *serverClient = qwsServer->client[-1];
    if ( serverClient )
       serverClient->sendEvent( &event );
    if ( win && win->client() && win->client() != serverClient )
       win->client()->sendEvent( &event );
}


/*!
    \internal

    Sets the current input method to \a im.
*/
void QWSServer::setCurrentInputMethod( QWSInputMethod *im )
{
    current_IM = im;
}

/*!
    \fn static void QWSServer::resetInputMethod()

    \internal
*/

/*!
    \fn static void QWSServer::setMicroFocus( int x, int y )

    \internal
*/

#endif //QT_NO_QWS_IM

#ifndef QT_NO_QWS_PROPERTIES
/*!
  \internal
*/
void QWSServer::sendPropertyNotifyEvent( int property, int state )
{
    ClientIterator it = client.begin();
    while ( it != client.end() ) {
        QWSClient *cl = *it;
        ++it;
        cl->sendPropertyNotifyEvent( property, state );
    }
}
#endif
void QWSServer::invokeIdentify( const QWSIdentifyCommand *cmd, QWSClient *client )
{
    client->setIdentity(cmd->id);
}

void QWSServer::invokeCreate( QWSCreateCommand *, QWSClient *client )
{
    QWSCreationEvent event;
    event.simpleData.objectid = get_object_id();
    client->sendEvent( &event );
}

void QWSServer::invokeRegionName( const QWSRegionNameCommand *cmd, QWSClient *client )
{
    QWSWindow* changingw = findWindow(cmd->simpleData.windowid, client);
    if ( changingw ) {
	changingw->setName( cmd->name );
	changingw->setCaption( cmd->caption );
	emit windowEvent( changingw, Name );
    }
}

void QWSServer::invokeRegion( QWSRegionCommand *cmd, QWSClient *client )
{
#ifdef QWS_REGION_DEBUG
    qDebug( "QWSServer::invokeRegion %d rects (%d)",
	    cmd->simpleData.nrectangles, cmd->simpleData.windowid );
#endif

    QWSWindow* changingw = findWindow(cmd->simpleData.windowid, 0);
    if ( !changingw ) {
	qWarning("Invalid window handle %08x",cmd->simpleData.windowid);
	client->sendRegionModifyEvent( cmd->simpleData.windowid, QRegion(), TRUE );
	return;
    } 
    if ( !changingw->forClient(client) ) {
	qWarning("Disabled: clients changing other client's window region");
	return;
    }

    bool containsMouse = changingw->allocation().contains(mousePosition);

    QRegion region;
    region.setRects(cmd->rectangles, cmd->simpleData.nrectangles);

    if ( !region.isEmpty() )
	changingw->setNeedAck( TRUE );
    bool isShow = !changingw->isVisible() && !region.isEmpty();
    setWindowRegion( changingw, region );
    syncRegions( changingw );
    if ( isShow )
	emit windowEvent( changingw, Show );
    if ( !region.isEmpty() )
	emit windowEvent( changingw, Geometry );
    else
	emit windowEvent( changingw, Hide );
    if ( focusw == changingw && region.isEmpty() )
	setFocus(changingw,FALSE);

    // if the window under our mouse changes, send update.
    if (containsMouse != changingw->allocation().contains(mousePosition))
	updateClientCursorPos();
}

void QWSServer::invokeRegionMove( const QWSRegionMoveCommand *cmd, QWSClient *client )
{
    QWSWindow* changingw = findWindow(cmd->simpleData.windowid, 0);
    if ( !changingw ) {
	qWarning("invokeRegionMove: Invalid window handle %d",cmd->simpleData.windowid);
	client->sendRegionModifyEvent( cmd->simpleData.windowid, QRegion(), TRUE );
	return;
    }
    if ( !changingw->forClient(client) ) {
	qWarning("Disabled: clients changing other client's window region");
	return;
    }

    changingw->setNeedAck( TRUE );
    moveWindowRegion( changingw, cmd->simpleData.dx, cmd->simpleData.dy );
    emit windowEvent( changingw, Geometry );
}

void QWSServer::invokeRegionDestroy( const QWSRegionDestroyCommand *cmd, QWSClient *client )
{
    QWSWindow* changingw = findWindow(cmd->simpleData.windowid, 0);
    if ( !changingw ) {
	qWarning("invokeRegionDestroy: Invalid window handle %d",cmd->simpleData.windowid);
	return;
    }
    if ( !changingw->forClient(client) ) {
	qWarning("Disabled: clients changing other client's window region");
	return;
    }

    setWindowRegion( changingw, QRegion() );
    rgnMan->remove( changingw->allocationIndex() );
    QWSWindow *w = windows.first();
    while ( w ) {
	if ( w == changingw ) {
	    windows.take();
	    break;
	}
	w = windows.next();
    }
    syncRegions();
    if ( focusw == changingw ) {
	changingw->shuttingDown();
	setFocus(changingw,FALSE);
    }
#ifndef QT_NO_QWS_PROPERTIES
    manager()->removeProperties( changingw->winId() );
#endif
    emit windowEvent( changingw, Destroy );
    delete changingw;
}


void QWSServer::invokeSetFocus( const QWSRequestFocusCommand *cmd, QWSClient *client )
{
    int winId = cmd->simpleData.windowid;
    int gain = cmd->simpleData.flag;

    if ( gain != 0 && gain != 1 ) {
	qWarning( "Only 0(lose) and 1(gain) supported" );
	return;
    }

    QWSWindow* changingw = findWindow(winId, 0);
    if ( !changingw )
	return;

    if ( !changingw->forClient(client) ) {
       qWarning("Disabled: clients changing other client's focus");
	return;
    }

    setFocus(changingw, gain);
}

void QWSServer::setFocus( QWSWindow* changingw, bool gain )
{
    if ( gain ) {
	if ( focusw != changingw ) {
	    if ( focusw ) focusw->focus(0);
	    focusw = changingw;
	    focusw->focus(1);
	    emit windowEvent( focusw, Active );
	}
    } else if ( focusw == changingw ) {
	changingw->focus(0);
	focusw = 0;
	// pass focus to window which most recently got it...
	QWSWindow* bestw=0;
	for (uint i=0; i<windows.count(); i++) {
	    QWSWindow* w = windows.at(i);
	    if ( w != changingw && !w->hidden() &&
		    (!bestw || bestw->focusPriority() < w->focusPriority()) )
		bestw = w;
	}
	if ( !bestw && changingw->focusPriority() ) { // accept focus back?
	    bestw = changingw; // must be the only one
	}
	focusw = bestw;
	if ( focusw )
	    focusw->focus(1);
	emit windowEvent( focusw, Active );
    }
}

void QWSServer::invokeSetAltitude( const QWSChangeAltitudeCommand *cmd,
				   QWSClient *client )
{
    int winId = cmd->simpleData.windowid;
    int alt = cmd->simpleData.altitude;
    bool fixed = cmd->simpleData.fixed;
#if 0
    qDebug( "QWSServer::invokeSetAltitude winId %d alt %d)", winId, alt );
#endif

    if ( alt < -1 || alt > 1 ) {
	qWarning( "QWSServer::invokeSetAltitude Only lower, raise and stays-on-top supported" );
	return;
    }

    QWSWindow* changingw = findWindow(winId, 0);
    if ( !changingw ) {
	qWarning("invokeSetAltitude: Invalid window handle %d", winId);
	client->sendRegionModifyEvent( winId, QRegion(), TRUE );
	return;
    }

    changingw->setNeedAck( TRUE );

    if ( fixed && alt >= 1) {
	changingw->onTop = TRUE;
    }
    if ( alt < 0 )
	lowerWindow( changingw, alt );
    else
	raiseWindow( changingw, alt );

    if ( !changingw->forClient(client) ) {
	refresh();
    }

}
#ifndef QT_NO_QWS_PROPERTIES
void QWSServer::invokeAddProperty( QWSAddPropertyCommand *cmd )
{
    manager()->addProperty( cmd->simpleData.windowid, cmd->simpleData.property );
}

void QWSServer::invokeSetProperty( QWSSetPropertyCommand *cmd )
{
    if ( manager()->setProperty( cmd->simpleData.windowid,
				    cmd->simpleData.property,
				    cmd->simpleData.mode,
				    cmd->data,
				    cmd->rawLen ) ) {
	sendPropertyNotifyEvent( cmd->simpleData.property,
				 QWSPropertyNotifyEvent::PropertyNewValue );
   }
}

void QWSServer::invokeRemoveProperty( QWSRemovePropertyCommand *cmd )
{
    if ( manager()->removeProperty( cmd->simpleData.windowid,
				       cmd->simpleData.property ) ) {
	sendPropertyNotifyEvent( cmd->simpleData.property,
				 QWSPropertyNotifyEvent::PropertyDeleted );
    }
}

void QWSServer::invokeGetProperty( QWSGetPropertyCommand *cmd, QWSClient *client )
{
    char *data;
    int len;

    if ( manager()->getProperty( cmd->simpleData.windowid,
				    cmd->simpleData.property,
				    data, len ) ) {
	client->sendPropertyReplyEvent( cmd->simpleData.property, len, data );
    } else {
	client->sendPropertyReplyEvent( cmd->simpleData.property, -1, 0 );
    }
}
#endif //QT_NO_QWS_PROPERTIES

void QWSServer::invokeSetSelectionOwner( QWSSetSelectionOwnerCommand *cmd )
{
    qDebug( "QWSServer::invokeSetSelectionOwner" );

    SelectionOwner so;
    so.windowid = cmd->simpleData.windowid;
    so.time.set( cmd->simpleData.hour, cmd->simpleData.minute,
		 cmd->simpleData.sec, cmd->simpleData.ms );

    if ( selectionOwner.windowid != -1 ) {
	QWSWindow *win = findWindow( selectionOwner.windowid, 0 );
	if ( win )
	    win->client()->sendSelectionClearEvent( selectionOwner.windowid );
	else
	    qDebug( "couldn't find window %d", selectionOwner.windowid );
    }

    selectionOwner = so;
}

void QWSServer::invokeConvertSelection( QWSConvertSelectionCommand *cmd )
{
    qDebug( "QWSServer::invokeConvertSelection" );

    if ( selectionOwner.windowid != -1 ) {
	QWSWindow *win = findWindow( selectionOwner.windowid, 0 );
	if ( win )
	    win->client()->sendSelectionRequestEvent( cmd, selectionOwner.windowid );
	else
	    qDebug( "couldn't find window %d", selectionOwner.windowid );
    }
}

#ifndef QT_NO_QWS_CURSOR
void QWSServer::invokeDefineCursor( QWSDefineCursorCommand *cmd, QWSClient *client )
{
    if (cmd->simpleData.height > 64 || cmd->simpleData.width > 64) {
	qDebug("Cannot define cursor size > 64x64");
	return;
    }

    int dataLen = cmd->simpleData.height * ((cmd->simpleData.width+7) / 8);

    QWSCursor *curs = new QWSCursor( cmd->data, cmd->data + dataLen,
				cmd->simpleData.width, cmd->simpleData.height,
				cmd->simpleData.hotX, cmd->simpleData.hotY);

    client->cursors.insert(cmd->simpleData.id, curs);
}

void QWSServer::invokeSelectCursor( QWSSelectCursorCommand *cmd, QWSClient *client )
{
    int id = cmd->simpleData.id;
    QWSCursor *curs = 0;
    if (id <= LastCursor) {
	curs = QWSCursor::systemCursor(id);
    }
    else {
	QWSCursorMap cursMap = client->cursors;
	QWSCursorMap::Iterator it = cursMap.find(id);
	if (it != cursMap.end()) {
	    curs = it.data();
	}
    }
    if (curs == 0) {
	curs = QWSCursor::systemCursor(ArrowCursor);
    }

    QWSWindow* win = findWindow(cmd->simpleData.windowid, 0);
    if (mouseGrabber) {
	// If the mouse is being grabbed, we don't want just anyone to
	// be able to change the cursor.  We do want the cursor to be set
	// correctly once mouse grabbing is stopped though.
	if (win != mouseGrabber)
	    nextCursor = curs;
	else
	    setCursor(curs);
    } else if (win && win->allocation().contains(mousePosition) ) {
	// A non-grabbing window can only set the cursor shape if the
	// cursor is within its allocated region.
	setCursor(curs);
    }
}
#endif

void QWSServer::invokeGrabMouse( QWSGrabMouseCommand *cmd, QWSClient *client )
{
    QWSWindow* win = findWindow(cmd->simpleData.windowid, 0);
    if ( !win )
	return;

    if ( cmd->simpleData.grab ) {
	if ( !mouseGrabber || mouseGrabber->client() == client ) {
	    mouseGrabbing = TRUE;
	    mouseGrabber = win;
	}
    } else {
	releaseMouse(mouseGrabber);
    }
}

void QWSServer::invokeGrabKeyboard( QWSGrabKeyboardCommand *cmd, QWSClient *client )
{
    QWSWindow* win = findWindow(cmd->simpleData.windowid, 0);
    if ( !win )
	return;

    if ( cmd->simpleData.grab ) {
	if ( !keyboardGrabber || ( keyboardGrabber->client() == client ) ) {
	    keyboardGrabbing = TRUE;
	    keyboardGrabber = win;
	}
    } else {
	releaseKeyboard(keyboardGrabber);
    }
}

#if !defined( QT_NO_SOUND ) 
void QWSServer::invokePlaySound( QWSPlaySoundCommand *cmd, QWSClient * )
{
#if !defined( Q_OS_MACX )
    soundserver->playFile(cmd->filename);
#endif
}
#endif

#ifndef QT_NO_COP
void QWSServer::invokeRegisterChannel( QWSQCopRegisterChannelCommand *cmd,
				       QWSClient *client )
{
  // QCopChannel will force us to emit the newChannel signal if this channel
  // didn't already exist.
  QCopChannel::registerChannel( cmd->channel, client );
}

void QWSServer::invokeQCopSend( QWSQCopSendCommand *cmd, QWSClient *client )
{
    QCopChannel::answer( client, cmd->channel, cmd->message, cmd->data );
}

#endif


#ifndef QT_NO_QWS_IM
void QWSServer::invokeSetMicroFocus( const QWSSetMicroFocusCommand *cmd,
                                  QWSClient * )
{
    if ( current_IM )
      current_IM->setMicroFocus( cmd->simpleData.x, cmd->simpleData.y );

}

void QWSServer::invokeResetIM( const QWSResetIMCommand *cmd,
                             QWSClient * )
{
    if ( current_IM )
      current_IM->reset();
}
#endif


void QWSServer::invokeRepaintRegion(QWSRepaintRegionCommand * cmd,
				    QWSClient * )
{
    QRegion r1;
    r1.setRects(cmd->rectangles,cmd->simpleData.numrects);
    refresh(r1);
}


QWSWindow* QWSServer::newWindow(int id, QWSClient* client)
{
    // Make a new window, put it on top.
    QWSWindow* w = new QWSWindow(id,client);
    int idx = rgnMan->add( id, QRegion() );
    if ( idx < 0 ) {
	qWarning( "Exceeded maximum top-level windows" );
	disconnectClient( client );
	return 0;
    }
    w->setAllocationIndex( idx );
    // insert after "stays on top" windows
    QWSWindow *win = windows.first();

    bool added = FALSE;
    while ( win ) {
	if ( !win->onTop ) {
	    windows.insert( windows.at(), w );
	    added = TRUE;
	    break;
	}
	win = windows.next();
    }
    if ( !added )
	windows.append( w );
    emit windowEvent( w, Create );
    return w;
}

QWSWindow* QWSServer::findWindow(int windowid, QWSClient* client)
{
    for (uint i=0; i<windows.count(); i++) {
	QWSWindow* w = windows.at(i);
	if ( w->winId() == windowid )
	    return w;
    }
    if ( client )
	return newWindow(windowid,client);
    else
	return 0;
}


void QWSServer::raiseWindow( QWSWindow *changingw, int /*alt*/ )
{
    if ( changingw == windows.first() ) {
	rgnMan->commit();
	changingw->updateAllocation(); // still need ack
	return;
    }

    int windowPos = 0;

    //change position in list:
    QWSWindow *w = windows.first();
    while ( w ) {
	if ( w == changingw ) {
	    windowPos = windows.at();
	    windows.take();
	    break;
	}
	w = windows.next();
    }

    if ( changingw->onTop )
	windows.prepend( changingw );
    else {
	// insert after "stays on top" windows
	bool in = FALSE;
	w = windows.first();
	while ( w ) {
	    if ( !w->onTop ) {
		windows.insert( windows.at(), changingw );
		in = TRUE;
		break;
	    }
	    w = windows.next();
	}
	if ( !in )
	    windows.append( changingw );
    }

    if ( windowPos != windows.at() ) {
	// window changed position
	setWindowRegion( changingw, changingw->requested_region );
    }
    syncRegions( changingw );
    emit windowEvent( changingw, Raise );
}

void QWSServer::lowerWindow( QWSWindow *changingw, int /*alt*/ )
{
    if ( changingw == windows.last() ) {
	rgnMan->commit();
	changingw->updateAllocation(); // still need ack
	return;
    }

    //lower: must remove region from window first.
    QRegion visible;
    visible = changingw->allocation();
    for (uint i=0; i<windows.count(); i++) {
	QWSWindow* w = windows.at(i);
	if ( w != changingw )
	    visible = visible - w->requested_region;
	if ( visible.isEmpty() )
	    break; //widget will be totally hidden;
    }
    QRegion exposed = changingw->allocation() - visible;

    //change position in list:
    QWSWindow *w = windows.first();
    while ( w ) {
	if ( w == changingw ) {
	    windows.take();
	    windows.append( changingw );
	    break;
	}
	w = windows.next();
    }

    changingw->removeAllocation( rgnMan, exposed );
    exposeRegion( exposed, 0 );
    syncRegions( changingw );
    emit windowEvent( changingw, Lower );
}

void QWSServer::moveWindowRegion( QWSWindow *changingw, int dx, int dy )
{
    if ( !changingw ) return;

    QRegion oldAlloc( changingw->allocation() );
    oldAlloc.translate( dx, dy );
    QRegion newRegion( changingw->requested_region );
    newRegion.translate( dx, dy );
/*
    for ( int i = 0; i < oldAlloc.rects().count(); i++ )
	qDebug( "oldAlloc %d, %d %dx%d",
	    oldAlloc.rects()[i].x(),
	    oldAlloc.rects()[i].y(),
	    oldAlloc.rects()[i].width(),
	    oldAlloc.rects()[i].height() );
*/
    QWSDisplay::grab( TRUE );
    QRegion exposed = setWindowRegion( changingw, newRegion );
/*
    for ( int i = 0; i < changingw->allocation().rects().count(); i++ )
	qDebug( "newAlloc %d, %d %dx%d",
	    changingw->allocation().rects()[i].x(),
	    changingw->allocation().rects()[i].y(),
	    changingw->allocation().rects()[i].width(),
	    changingw->allocation().rects()[i].height() );
*/
    // add exposed areas
    changingw->exposed = changingw->allocation() - oldAlloc;

    rgnMan->commit();

    // safe to blt now
    QRegion cr( changingw->allocation() );
    cr &= oldAlloc;

    QSize s = QSize(swidth, sheight);
    QPoint p1 = qt_screen->mapFromDevice( QPoint(0, 0), s );
    QPoint p2 = qt_screen->mapFromDevice( QPoint(dx, dy), s );

    QRect br( cr.boundingRect() );
    br = qt_screen->mapFromDevice( br, s );
    gfx->setClipDeviceRegion( cr );
    gfx->scroll( br.x(), br.y(), br.width(), br.height(),
		 br.x() - (p2.x() - p1.x()), br.y() - (p2.y() - p1.y()) );
    gfx->setClipDeviceRegion( screenRegion );
#ifndef QT_NO_PALETTE
    clearRegion( exposed, qApp->palette().color( QPalette::Active, QColorGroup::Background ) );
#endif
    QWSDisplay::ungrab();
/*
    for ( int i = 0; i < changingw->exposed.rects().count(); i++ )
	qDebug( "svr exposed: %d, %d %dx%d",
	    changingw->exposed.rects()[i].x(),
	    changingw->exposed.rects()[i].y(),
	    changingw->exposed.rects()[i].width(),
	    changingw->exposed.rects()[i].height() );
*/
    notifyModified( changingw );
    paintBackground( dirtyBackground );
    dirtyBackground = QRegion();
}

/*!
    Changes the requested region of window \a changingw to \a r, sends
    appropriate region change events to all appropriate clients, and
    waits for all required acknowledgements.

    If \a changingw is 0, the server's reserved region is changed.

    Returns the exposed region.
*/
QRegion QWSServer::setWindowRegion( QWSWindow* changingw, QRegion r )
{
#ifdef QWS_REGION_DEBUG
    qDebug("setWindowRegion %d", changingw ? changingw->winId() : -1 );
#endif

    QRegion exposed;
    if (changingw) {
	changingw->requested_region = r;
	r = r - serverRegion;
	exposed = changingw->allocation() - r;
    } else {
	exposed = serverRegion-r;
	serverRegion = r;
    }
    QRegion extra_allocation;
    int windex = -1;


    if ( changingw )
	changingw->removeAllocation( rgnMan, exposed );

    // Go through the higher windows and calculate the reqion that we will
    // end up with.
    // Then continue with the deeper windows, taking the requested region
    bool deeper = changingw == 0;
    for (uint i=0; i<windows.count(); i++) {
	QWSWindow* w = windows.at(i);
	if ( w == changingw ) {
	    windex = i;
	    extra_allocation = r - w->allocation();
	    deeper = TRUE;
	} else if ( deeper ) {
	    w->removeAllocation(rgnMan, r);
	    r -= w->allocation();
	} else {
	    //higher windows
	    r -= w->allocation();
	}
	if ( r.isEmpty() ) {
	    break; // Nothing left for deeper windows
	}
    }

    if ( r.isEmpty() ) {
	// Invisible! Release grabs.
	releaseMouse(changingw);
	releaseKeyboard(changingw);
    }

    if ( changingw && !changingw->requested_region.isEmpty() )
	changingw->addAllocation( rgnMan, extra_allocation & screenRegion );
    else if ( !disablePainting )
	paintServerRegion();

    exposeRegion( exposed, windex+1 );
    return exposed;
}

void QWSServer::exposeRegion( QRegion r, int start )
{
    r &= screenRegion;

    for (uint i=start; i<windows.count(); i++) {
	if ( r.isEmpty() )
	    break; // Nothing left for deeper windows
	QWSWindow* w = windows.at(i);
	w->addAllocation( rgnMan, r );
	r -= w->allocation();
    }
    dirtyBackground |= r;
}

void QWSServer::notifyModified( QWSWindow *active )
{
    // notify active window first
    if ( active )
	active->updateAllocation();

    // now the rest
    for (uint i=0; i<windows.count(); i++) {
	QWSWindow* w = windows.at(i);
	w->updateAllocation();
    }
}

void QWSServer::syncRegions( QWSWindow *active )
{
    rgnMan->commit();
    notifyModified( active );
    paintBackground( dirtyBackground );
    dirtyBackground = QRegion();
}

/*!
    Closes the pointer device(s).
*/
void QWSServer::closeMouse()
{
    mousehandlers.setAutoDelete(TRUE);
    mousehandlers.clear();
}

/*!
    Opens the mouse device(s).
*/
void QWSServer::openMouse()
{
    QString mice = getenv("QWS_MOUSE_PROTO");
    if ( mice.isEmpty() ) {
#if defined(QT_QWS_CASSIOPEIA)
	mice = "TPanel:/dev/tpanel";
#endif
	if ( mice.isEmpty() )
	    mice = defaultMouse;
    }
    closeMouse();
    bool needviscurs = TRUE;
    if ( mice != "None" ) {
#ifndef QT_NO_STRINGLIST
	QStringList mouse = QStringList::split(" ",mice);
	for (QStringList::Iterator m=mouse.begin(); m!=mouse.end(); ++m) {
	    QString ms = *m;
#else
	    QString ms = mice; // Assume only one
	    {
#endif
		QWSMouseHandler* h = newMouseHandler(ms);
		/* XXX handle mouse cursor visibility sensibly
		   if ( !h->inherits("QCalibratedMouseHandler") )
		   needviscurs = TRUE;
		 */
	    }
    }
#ifndef QT_NO_QWS_CURSOR
    setCursorVisible( needviscurs );
#endif
}

QWSMouseHandler* QWSServer::newMouseHandler(const QString& spec)
{
    static int init=0;
    if ( !init && qt_screen ) {
	init = 1;
    }

    int c = spec.find(':');
    QString mouseProto;
    QString mouseDev;
    if ( c >= 0 ) {
	mouseProto = spec.left(c);
	mouseDev = spec.mid(c+1);
    } else {
	mouseProto = spec;
    }

    QWSMouseHandler *handler = 0;
    handler = QMouseDriverFactory::create(mouseProto, mouseDev);
    return handler;
}

#ifndef QT_NO_QWS_KEYBOARD

/*!
    Closes keyboard device(s).
*/
void QWSServer::closeKeyboard()
{
    keyboardhandlers.setAutoDelete(TRUE);
    keyboardhandlers.clear();
}

/*!
    Returns the primary keyboard handler.
*/
QWSKeyboardHandler* QWSServer::keyboardHandler()
{
    return qwsServer->keyboardhandlers.first();
}

/*!
    Sets the primary keyboard handler to \a kh.
*/
void QWSServer::setKeyboardHandler(QWSKeyboardHandler* kh)
{
    qwsServer->keyboardhandlers.prepend(kh);
}

/*!
    Opens the keyboard device(s).
*/
void QWSServer::openKeyboard()
{
    QString keyboards = getenv("QWS_KEYBOARD");
    if ( keyboards.isEmpty() ) {
#if defined( QT_QWS_CASSIOPEIA )
	keyboards = "Buttons";
#endif
	if ( keyboards.isEmpty() ) {
	    keyboards = defaultKeyboard;	// last resort
	}
    }
    closeKeyboard();
    if ( keyboards == "None" )
	return;
    QString device;
    QString type;
#ifndef QT_NO_STRINGLIST
    QStringList keyboard = QStringList::split(" ",keyboards);
    for (QStringList::Iterator k=keyboard.begin(); k!=keyboard.end(); ++k) {
	QString spec = *k;
	int colon=spec.find(':');
	if ( colon>=0 ) {
	    type = spec.left(colon);
	    device = spec.mid(colon+1);
	} else {
	    type = spec;
	}
	QWSKeyboardHandler* kh = QKbdDriverFactory::create( type, device );
	keyboardhandlers.append(kh);
    }
#else
    int colon=keyboards.find(':');
    if ( colon>=0 ) {
	type = keyboards.left(colon);
	device = keyboards.mid(colon+1);
    } else {
	type = keyboards;
    }
    QWSKeyboardHandler* kh = QKbdDriverFactory::create(type, device); //assume only one
    keyboardhandlers.append(kh);
#endif
}

#endif //QT_NO_QWS_KEYBOARD

QPoint QWSServer::mousePosition;
QColor *QWSServer::bgColor = 0;
QImage *QWSServer::bgImage = 0;

void QWSServer::move_region( const QWSRegionMoveCommand *cmd )
{
    QWSClient *serverClient = client[-1];
    invokeRegionMove( cmd, serverClient );
}

void QWSServer::set_altitude( const QWSChangeAltitudeCommand *cmd )
{
    QWSClient *serverClient = client[-1];
    invokeSetAltitude( cmd, serverClient );
}

void QWSServer::request_focus( const QWSRequestFocusCommand *cmd )
{
    invokeSetFocus( cmd, client[-1] );
}

void QWSServer::set_identity( const QWSIdentifyCommand *cmd )
{
    invokeIdentify( cmd, client[-1] );
}

void QWSServer::request_region( int wid, QRegion region )
{
    QWSClient *serverClient = client[-1];
    QWSWindow* changingw = findWindow( wid, 0 );
    if ( !changingw ) {
	if ( !region.isEmpty() )
	    serverClient->sendRegionModifyEvent( wid, QRegion(), TRUE );
	return;
    }
    if ( !region.isEmpty() )
	changingw->setNeedAck( TRUE );
    bool isShow = !changingw->isVisible() && !region.isEmpty();
    setWindowRegion( changingw, region );
    syncRegions( changingw );
    if ( isShow )
	emit windowEvent( changingw, Show );
    if ( !region.isEmpty() )
	emit windowEvent( changingw, Geometry );
    else
	emit windowEvent( changingw, Hide );
    if ( focusw == changingw && region.isEmpty() )
	setFocus(changingw,FALSE);
}

void QWSServer::destroy_region( const QWSRegionDestroyCommand *cmd )
{
    invokeRegionDestroy( cmd, client[-1] );
}

void QWSServer::name_region( const QWSRegionNameCommand *cmd )
{
    invokeRegionName( cmd, client[-1] );
}

#ifndef QT_NO_QWS_IM
void QWSServer::set_micro_focus( const QWSSetMicroFocusCommand *cmd )
{
    invokeSetMicroFocus( cmd, client[-1] );
}

void QWSServer::reset_im( const QWSResetIMCommand *cmd )
{
    invokeResetIM( cmd, client[-1] );
}
#endif


void QWSServer::openDisplay()
{
    qt_init_display();

    rgnMan = qt_fbdpy->regionManager();
    swidth = qt_screen->deviceWidth();
    sheight = qt_screen->deviceHeight();
    gfx = qt_screen->screenGfx();
}


void QWSServer::closeDisplay()
{
    delete gfx;
    qt_screen->shutdownDevice();
}


void QWSServer::paintServerRegion()
{
}

void QWSServer::paintBackground( const QRegion &rr )
{
    if ( bgImage && bgImage->isNull() )
	return;
    QRegion r = rr;
    if ( !r.isEmpty() ) {
	Q_ASSERT ( qt_fbdpy );

	r = qt_screen->mapFromDevice( r, QSize(swidth, sheight) );

	gfx->setClipRegion( r );
	QRect br( r.boundingRect() );
	if ( !bgImage ) {
	    gfx->setBrush(QBrush( *bgColor ));
	    gfx->fillRect( br.x(), br.y(), br.width(), br.height() );
	} else {
	    gfx->setSource( bgImage );
	    gfx->setBrushOffset( br.x(), br.y() );
	    gfx->tiledBlt( br.x(), br.y(), br.width(), br.height() );
	}
	gfx->setClipDeviceRegion( screenRegion );
    }
}

void QWSServer::clearRegion( const QRegion &r, const QColor &c )
{
    if ( !r.isEmpty() ) {
	ASSERT ( qt_fbdpy );
	gfx->setBrush( QBrush(c) );
	QSize s( swidth, sheight );
	QArray<QRect> a = r.rects();
	for ( int i = 0; i < (int)a.count(); i++ ) {
	    QRect r = qt_screen->mapFromDevice( a[i], s );
	    gfx->fillRect( r.x(), r.y(), r.width(), r.height() );
	}
    }
}


void QWSServer::refreshBackground()
{
    QRegion r(0, 0, swidth, sheight);
    for (uint i=0; i<windows.count(); i++) {
	if ( r.isEmpty() )
	    return; // Nothing left for deeper windows
	QWSWindow* w = windows.at(i);
	r -= w->allocation();
    }
    paintBackground( r );
}


/*!
    Sets the image \a img to be used as the background in the absence
    of obscuring windows.
*/

void QWSServer::setDesktopBackground( const QImage &img )
{

    if ( !bgImage )
	bgImage = new QImage( img );
    else
	*bgImage = img;

    if ( qwsServer )
	qwsServer->refreshBackground();
}

/*!
    \overload

    Sets the color \a c to be used as the background in the absence of
    obscuring windows.
*/

void QWSServer::setDesktopBackground( const QColor &c )
{
    if ( !bgColor )
	bgColor = new QColor( c );
    else
	*bgColor = c;

    if ( bgImage ) {
	delete bgImage;
	bgImage = 0;
    }

    if ( qwsServer )
	qwsServer->refreshBackground();
}

/*!
  \internal
 */
void QWSServer::startup(int flags)
{
    if ( qwsServer )
	return;
    unlink( qws_qtePipeFilename().latin1() );
    (void)new QWSServer(flags);
}


/*!
  \internal
*/

void QWSServer::closedown()
{
    unlink( qws_qtePipeFilename().latin1() );
    delete qwsServer;
    qwsServer = 0;
}


void QWSServer::emergency_cleanup()
{
#ifndef QT_NO_QWS_KEYBOARD
    if ( qwsServer )
	qwsServer->closeKeyboard();
#endif
}

#ifndef QT_NO_QWS_KEYBOARD
static QPtrList<QWSServer::KeyboardFilter> *keyFilters = 0;

/*!
  \internal
*/
void QWSServer::processKeyEvent(int unicode, int keycode, int modifiers, bool isPress,
  bool autoRepeat)
{
    if ( keyFilters ) {
        QListIterator<QWSServer::KeyboardFilter> it(*keyFilters);
	QWSServer::KeyboardFilter *keyFilter;
	while ((keyFilter=it.current())) {
            if ( keyFilter->filter( unicode, keycode, modifiers, isPress, autoRepeat ) )
	        return;
	    ++it;
	}
    }
    sendKeyEvent( unicode, keycode, modifiers, isPress, autoRepeat );
}

/*!
    Adds a filter \a f to be invoked for all key events from physical
    keyboard drivers (events sent via processKeyEvent()).

    The filter is not invoked for keys generated by virtual keyboard
    drivers (events sent via sendKeyEvent()).

    If \a f is 0, the most-recently added filter is removed and deleted.
    The caller is responsible for matching each addition with a
    corresponding removal.
*/
void QWSServer::setKeyboardFilter( KeyboardFilter *f )
{
     if ( !keyFilters )
        keyFilters = new QList<QWSServer::KeyboardFilter>;
     if ( f ) {
        keyFilters->prepend(f);
     } else {
        delete keyFilters->take(0);
     }
}
#endif

/*!
    Sets an array of timeouts for the screensaver to a list of \a ms
    milliseconds. A setting of zero turns off the screensaver. The
    array must be 0-terminated.
*/
void QWSServer::setScreenSaverIntervals(int* ms)
{
    if ( !qwsServer )
	return;
    delete [] qwsServer->d->screensaverintervals;
    if ( ms ) {
	int* t=ms;
	int n=0;
	while (*t++) n++;
	if ( n ) {
	    n++; // the 0
	    qwsServer->d->screensaverintervals = new int[n];
	    memcpy( qwsServer->d->screensaverintervals, ms, n*sizeof(int));
	} else {
	    qwsServer->d->screensaverintervals = 0;
	}
    } else {
	qwsServer->d->screensaverintervals = 0;
    }
    qwsServer->screensaverinterval = 0;

    qwsServer->d->screensavertimer->stop();
    qt_screen->blank(FALSE);
    qwsServer->screenSaverWake();
}

/*!
    Sets the timeout for the screensaver to \a ms milliseconds. A
    setting of zero turns off the screensaver.
*/
void QWSServer::setScreenSaverInterval(int ms)
{
    int v[2];
    v[0] = ms;
    v[1] = 0;
    setScreenSaverIntervals(v);
}

extern bool qt_disable_lowpriority_timers;

void QWSServer::screenSaverWake()
{
    if ( d->screensaverintervals ) {
	if ( screensaverinterval != d->screensaverintervals ) {
	    if ( d->saver ) d->saver->restore();
	    screensaverinterval = d->screensaverintervals;
	} else {
	    if ( !d->screensavertimer->isActive() ) {
		qt_screen->blank(FALSE);
		if ( d->saver ) d->saver->restore();
	    }
	}
	d->screensavertimer->start(*screensaverinterval,TRUE);
	d->screensavertime.start();
    }
    qt_disable_lowpriority_timers=FALSE;
}

void QWSServer::screenSaverSleep()
{
    qt_screen->blank(TRUE);
#if !defined(QT_QWS_IPAQ) && !defined(QT_QWS_EBX)
    d->screensavertimer->stop();
#else
    if ( screensaverinterval ) {
	d->screensavertimer->start(*screensaverinterval,TRUE);
	d->screensavertime.start();
    } else {
	d->screensavertimer->stop();
    }
#endif
    qt_disable_lowpriority_timers=TRUE;
}

/*!
    \internal

    Deletes the current screen saver and sets the screen saver to be
    \a ss.
*/
void QWSServer::setScreenSaver(QWSScreenSaver* ss)
{
    delete qwsServer->d->saver;
    qwsServer->d->saver = ss;
}

void QWSServer::screenSave(int level)
{
    if ( d->saver ) {
	if ( d->saver->save(level) ) {
	    if ( screensaverinterval && screensaverinterval[1] ) {
		d->screensavertimer->start(*++screensaverinterval,TRUE);
		d->screensavertime.start();
	    } else {
		screensaverinterval = 0;
	    }
	} else {
	    // for some reason, the saver don't want us to change to the
	    // next level, so we'll stay at this level for another interval
	    if ( screensaverinterval && *screensaverinterval ) {
		d->screensavertimer->start(*screensaverinterval,TRUE);
		d->screensavertime.start();
	    }
	}
    } else {
	screensaverinterval = 0;//d->screensaverintervals;
	screenSaverSleep();
    }
}

void QWSServer::screenSaverTimeout()
{
    if ( screensaverinterval ) {
	if ( d->screensavertime.elapsed() > *screensaverinterval*2 ) {
	    // bogus (eg. unsuspend, system time changed)
	    screenSaverWake(); // try again
	    return;
	}
	screenSave(screensaverinterval-d->screensaverintervals);
    }
}

/*!
    Returns TRUE if the screensaver is active (i.e. the screen is
    blanked); otherwise returns FALSE.
*/
bool QWSServer::screenSaverActive()
{
    return qwsServer->screensaverinterval
	&& !qwsServer->d->screensavertimer->isActive();
}

/*!
    If \a activate is TRUE the screensaver is activated immediately;
    if \a activate is FALSE the screensaver is deactivated.
*/
void QWSServer::screenSaverActivate(bool activate)
{
    if ( activate )
	qwsServer->screenSaverSleep();
    else
	qwsServer->screenSaverWake();
}

void QWSServer::disconnectClient( QWSClient *c )
{
    QTimer::singleShot( 0, c, SLOT(closeHandler()) );
}

void QWSServer::updateClientCursorPos()
{
    QWSWindow *win = qwsServer->mouseGrabber ? qwsServer->mouseGrabber : qwsServer->windowAt( mousePosition );
    QWSClient *winClient = win ? win->client() : 0;
    if ( winClient && winClient != d->cursorClient )
	sendMouseEvent( mousePosition, d->mouseState );
}


#ifndef QT_NO_QWS_IM

/*!
    \class QWSInputMethod
    \brief The QWSInputMethod class provides international input methods
    for Qt/Embedded.

    \ingroup qws

    Subclass this class to implement your own input method.

    An input methods consists of a keyboard filter and optionally a
    graphical interface. The keyboard filter intercepts key events
    from physical or virtual keyboards by implementing the filter()
    function.

    Use sendIMEvent() to send composition events. Composition starts
    with the input method sending an \c IMStart event, followed by a
    number of \c IMCompose events and ending with an \c IMEnd event or
    when the virtual reset() function is called.

    The functions setMicroFocus() and setFont() can be reimplemented
    to receive more information about the state of the focus widget.

    Use QWSServer::setCurrentInputMethod() to install an input method.

*/

/*!
  Constructs a new input method
*/

QWSInputMethod::QWSInputMethod()
{
    
}

/*!
  Destructs the input method uninstalling it if it is currently installed.
*/
QWSInputMethod::~QWSInputMethod()
{
    if ( current_IM == this )
	current_IM = 0;
}



/*!
    \fn bool QWSInputMethod::filter(int unicode, int keycode, int modifiers, bool isPress, bool autoRepeat)

    This function must be implemented in subclasses to handle key
    input from physical or virtual keyboards. Returning TRUE will
    block the event from further processing.

    The Unicode value is given in \a unicode and the key code in \a
    keycode. Keyboard modifiers are OR-ed together in \a modifiers.
    If \a isPress is TRUE this is a key press; otherwise it is a key
    release. If \a autoRepeat is TRUE this is an auto-repeated key
    press.

    All normal key events should be blocked while in compose mode
    (i.e., between \c IMStart and \c IMEnd).

*/


/*!
    Implemented in subclasses to reset the state of the input method.
*/

void QWSInputMethod::reset()
{
    
}


/*!
    \fn void QWSInputMethod::setMicroFocus( int x, int y )

    Implemented in subclasses to handle microFocusHint changes in the
    focus widget. \a x and \a y are the global coordinates of the
    text cursor.

*/

void QWSInputMethod::setMicroFocus( int, int )
{
    
}


/*!
    Implemented in subclasses to handle font changes in the focus
    widget.

    This functionality is provided for future expansion; it is not
    used in this version of Qt/Embedded.
*/

void QWSInputMethod::setFont( const QFont& )
{
    
}


/*!
    \fn QWSInputMethod::sendIMEvent( QWSServer::IMState state, const QString &txt, int cpos, int selLen )

    Causes a QIMEvent to be sent to the focus widget. \a state may be
    one of \c QWSServer::IMStart, \c QWSServer::IMCompose or \c
    QWSServer::IMEnd.

    \a txt is the text being composed (or the finished text if state
    is \c IMEnd). \a cpos is the current cursor position.

    If state is \c IMCompose, \a selLen is the number of characters in
    the composition string (starting at \a cpos) that should be
    marked as selected by the input widget receiving the event.
*/

#endif

/*!
    \fn  QWSWindow::QWSWindow(int i, QWSClient * client)

    Constructs a new top-level window, associated with the client \a
    client and giving it the id \a i.
*/

/*!
    \fn QWSServer::manager()

    Returns the QWSPropertyManager, which is used for implementing
    X11-style window properties.
*/

/*!
    \fn QWSServer::windowEvent(QWSWindow * w, QWSServer::WindowEvent e)

    This signal is emitted whenever something happens to a top-level
    window (e.g. it's created or destroyed). \a w is the window to
    which the event of type \a e has occurred.
*/

/*!
    \fn QWSServer::keyMap()

    Returns the keyboard mapping table used to convert keyboard
    scancodes to Qt keycodes and Unicode values. It's used by the
    keyboard driver in \c qkeyboard_qws.cpp.
*/

/*!
    \enum QWSServer::ServerFlags

    This enum is used to pass various options to the window system
    server.

    \value DisableKeyboard Ignore all keyboard input.
    \value DisableMouse Ignore all mouse input.
*/

/*!
    \enum QWSServer::GUIMode

    This determines what sort of QWS server to create:

    \value NoGui This is used for non-graphical Qt applications.
    \value NormalGUI A normal Qt/Embedded application (not the server).
    \value Server A Qt/Embedded server (e.g. if \c -qws has been specified
		    on the command line.
*/

/*
    \class QWSServer::KeyMap
    \brief The QWSServer::KeyMap class is used for mapping scancodes.

    \ingroup qws

    The KeyMap structure records an individual KeyMap entry in the
    array used to map keyboard scancodes to Qt key codes and Unicode
    values.
*/

/*!
    \class QWSServer::KeyboardFilter
    \brief The QWSServer::KeyboardFilter class provides a global keyboard
    event filter.

    \ingroup qws

    The KeyboardFilter class is used to implement a global, low-level
    filter on key events in the Qt/Embedded server application; this
    can be used to implement things like APM (advanced power
    management) suspend from a button without having to filter for it
    in all applications.
*/

/*!
    \enum QWSServer::WindowEvent

    This specifies what sort of event has occurred to a top-level window:

    \value Create A new window has been created (QWidget constructor).
    \value Destroy The window has been closed and deleted (QWidget destructor).
    \value Hide The window has been hidden with QWidget::hide().
    \value Show The window has been shown with QWidget::show() or similar.
    \value Raise The window has been raised to the top of the desktop.
    \value Lower The window has been lowered.
    \value Geometry The window has changed size or position.
    \value Active The window has become the active window (has keyboard focus).
    \value Name The window has been named.
*/

