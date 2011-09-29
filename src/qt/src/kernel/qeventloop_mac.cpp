/****************************************************************************
** $Id: qt/qeventloop_mac.cpp   3.3.8   edited Jan 11 14:38 $
**
** Implementation of timers, socket notifiers, and event handling
**
** Created : 001018
**
** Copyright (C) 1992-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Macintosh may use this file in accordance with the Qt Commercial
** License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qeventloop_p.h"
#include "qeventloop.h"
#include "qapplication.h"
#include "qt_mac.h"

#if defined(QT_THREAD_SUPPORT)
#  include "qmutex.h"
#endif // QT_THREAD_SUPPORT

#ifdef Q_WS_MAC9
#  define QMAC_EVENT_NOWAIT 0.01
#else
#  define QMAC_EVENT_NOWAIT kEventDurationNoWait
#endif

//Externals
void qt_event_request_timer(TimerInfo *); //qapplication_mac.cpp
TimerInfo *qt_event_get_timer(EventRef); //qapplication_mac.cpp
void qt_event_request_select(QEventLoop *); //qapplication_mac.cpp
void qt_event_request_sockact(QEventLoop *); //qapplication_mac.cpp
void qt_event_request_updates(); //qapplication_mac.cpp
void qt_event_request_wakeup(); //qapplication_mac.cpp
bool qt_mac_send_event(QEventLoop::ProcessEventsFlags, EventRef, WindowPtr =NULL); //qapplication_mac.cpp
extern bool qt_is_gui_used; //qapplication.cpp

//Local Statics

//pre/post select callbacks
typedef void (*VFPTR)();
typedef QValueList<VFPTR> QVFuncList;
void qt_install_preselect_handler(VFPTR);
void qt_remove_preselect_handler(VFPTR);
static QVFuncList *qt_preselect_handler = 0;
void qt_install_postselect_handler(VFPTR);
void qt_remove_postselect_handler(VFPTR);
static QVFuncList *qt_postselect_handler = 0;
void qt_install_preselect_handler(VFPTR handler)
{
    if(!qt_preselect_handler)
	qt_preselect_handler = new QVFuncList;
    qt_preselect_handler->append(handler);
}
void qt_remove_preselect_handler(VFPTR handler)
{
    if(qt_preselect_handler) {
	QVFuncList::Iterator it = qt_preselect_handler->find(handler);
	if(it != qt_preselect_handler->end())
	    qt_preselect_handler->remove(it);
    }
}
void qt_install_postselect_handler(VFPTR handler)
{
    if(!qt_postselect_handler)
	qt_postselect_handler = new QVFuncList;
    qt_postselect_handler->prepend(handler);
}
void qt_remove_postselect_handler(VFPTR handler)
{
    if(qt_postselect_handler) {
	QVFuncList::Iterator it = qt_postselect_handler->find(handler);
	if(it != qt_postselect_handler->end())
	    qt_postselect_handler->remove(it);
    }
}

/*****************************************************************************
 Socket notifier type
 *****************************************************************************/
QSockNotType::QSockNotType()  : list(0)
{
    FD_ZERO(&select_fds);
    FD_ZERO(&enabled_fds);
    FD_ZERO(&pending_fds);
}

QSockNotType::~QSockNotType()
{
    if(list)
	delete list;
    list = 0;
}

/*****************************************************************************
  Timers stuff
 *****************************************************************************/
/* timer code */
struct TimerInfo {
    int id;
    QObject *obj;
    bool pending;
    //type switches
    enum TimerType { TIMER_ZERO, TIMER_QT, TIMER_MAC, TIMER_ANY } type;
    union {
	EventLoopTimerRef mac_timer;
	struct {
	    timeval  interval;
	    timeval  timeout;
	} qt_timer;
    } u;
};
static int zero_timer_count = 0;
typedef QPtrList<TimerInfo> TimerList;	// list of TimerInfo structs
static TimerList *timerList	= 0;		// timer list
static EventLoopTimerUPP timerUPP = NULL;       //UPP

#ifdef Q_OS_MACX
static inline bool operator<(const timeval &t1, const timeval &t2)
{
    return t1.tv_sec < t2.tv_sec ||
	  (t1.tv_sec == t2.tv_sec && t1.tv_usec < t2.tv_usec);
}
static inline bool operator==( const timeval &t1, const timeval &t2 )
{
    return t1.tv_sec == t2.tv_sec && t1.tv_usec == t2.tv_usec;
}
static inline timeval &operator+=(timeval &t1, const timeval &t2)
{
    t1.tv_sec += t2.tv_sec;
    if((t1.tv_usec += t2.tv_usec) >= 1000000) {
	t1.tv_sec++;
	t1.tv_usec -= 1000000;
    }
    return t1;
}
static inline timeval operator+(const timeval &t1, const timeval &t2)
{
    timeval tmp;
    tmp.tv_sec = t1.tv_sec + t2.tv_sec;
    if((tmp.tv_usec = t1.tv_usec + t2.tv_usec) >= 1000000) {
	tmp.tv_sec++;
	tmp.tv_usec -= 1000000;
    }
    return tmp;
}
static inline timeval operator-(const timeval &t1, const timeval &t2)
{
    timeval tmp;
    tmp.tv_sec = t1.tv_sec - t2.tv_sec;
    if((tmp.tv_usec = t1.tv_usec - t2.tv_usec) < 0) {
	tmp.tv_sec--;
	tmp.tv_usec += 1000000;
    }
    return tmp;
}
static timeval	watchtime;			// watch if time is turned back
timeval	*qt_wait_timer_max = 0;
static inline void getTime(timeval &t)	// get time of day
{
    gettimeofday(&t, 0);
    while(t.tv_usec >= 1000000) {		// NTP-related fix
	t.tv_usec -= 1000000;
	t.tv_sec++;
    }
    while(t.tv_usec < 0) {
	if(t.tv_sec > 0) {
	    t.tv_usec += 1000000;
	    t.tv_sec--;
	} else {
	    t.tv_usec = 0;
	    break;
	}
    }
}
static void repairTimer(const timeval &time)	// repair broken timer
{
    if(!timerList)				// not initialized
	return;
    timeval diff = watchtime - time;
    register TimerInfo *t = timerList->first();
    while(t) {				// repair all timers
	if(t->type == TimerInfo::TIMER_QT)
	    t->u.qt_timer.timeout = t->u.qt_timer.timeout - diff;
	else
	    qWarning("Qt: internal: %s:%d This can't happen!", __FILE__, __LINE__);
	t = timerList->next();
    }
}
static timeval *qt_wait_timer()
{
    static timeval tm;
    bool first = TRUE;
    timeval currentTime;
    if(timerList && timerList->count()) {	// there are waiting timers
	getTime(currentTime);
	if(first) {
	    if(currentTime < watchtime)	// clock was turned back
		repairTimer(currentTime);
	    first = FALSE;
	    watchtime = currentTime;
	}
	TimerInfo *t = timerList->first();	// first waiting timer
	if(t->type == TimerInfo::TIMER_QT) {
	    if(currentTime < t->u.qt_timer.timeout) {	// time to wait
		tm = t->u.qt_timer.timeout - currentTime;
	    } else {
		tm.tv_sec  = 0;			// no time to wait
		tm.tv_usec = 0;
	    }
	    if(qt_wait_timer_max && *qt_wait_timer_max < tm)
		tm = *qt_wait_timer_max;
	} else {
	    qWarning("Qt: internal: %s:%d Unexpected condition reached.", __FILE__, __LINE__);
	}
	return &tm;
    }
    if(qt_wait_timer_max) {
	tm = *qt_wait_timer_max;
	return &tm;
    }
    return 0;					// no timers
}
#endif

static void insertTimer(const TimerInfo *ti)	// insert timer info into list
{
    int index = 0;
    for(TimerInfo *t = timerList->first(); t; index++) {	// list is sorted by timeout
	if(t->type == TimerInfo::TIMER_QT && ti->u.qt_timer.timeout < t->u.qt_timer.timeout)
	    break;
	t = timerList->next();
    }
    timerList->insert(index, ti);		// inserts sorted
}

/* timer call back */
QMAC_PASCAL static void qt_activate_mac_timer(EventLoopTimerRef, void *data)
{
    TimerInfo *tmr = ((TimerInfo *)data);
    if(tmr->type != TimerInfo::TIMER_MAC) { //can't really happen, can it?
	qWarning("Qt: internal %s: %d WH0A", __FILE__, __LINE__);
	return;
    }
    if(QMacBlockingFunction::blocking()) { //just send it immediately
	/* someday this is going to lead to an infite loop, I just know it. I should be marking the
	   pending here, and unmarking, but of course single shot timers are removed between now
	   and the return (down 4 lines) */
	QTimerEvent e(tmr->id);
	QApplication::sendEvent(tmr->obj, &e);
	QApplication::flush();
	return;
    }
    if(tmr->pending)
	return;
    tmr->pending = TRUE;
    qt_event_request_timer(tmr);
}

//central cleanup
QMAC_PASCAL static Boolean find_timer_event(EventRef event, void *d)
{
    return (qt_event_get_timer(event) == ((TimerInfo *)d));
}

static bool killTimer(TimerInfo *t, bool remove=TRUE)
{
    t->pending = TRUE;
    if(t->type == TimerInfo::TIMER_MAC) {
	RemoveEventLoopTimer(t->u.mac_timer);
	if(t->pending) {
	    EventComparatorUPP fnc = NewEventComparatorUPP(find_timer_event);
	    FlushSpecificEventsFromQueue(GetMainEventQueue(), fnc, (void *)t);
	    DisposeEventComparatorUPP(fnc);
	}
    } else if(t->type == TimerInfo::TIMER_ZERO) {
	zero_timer_count--;
    }
    return remove ? timerList->remove() : TRUE;
}

//
// Timer initialization and cleanup routines
//
static void initTimers()			// initialize timers
{
    timerUPP = NewEventLoopTimerUPP(qt_activate_mac_timer);
    Q_CHECK_PTR(timerUPP);
    timerList = new TimerList;
    Q_CHECK_PTR(timerList);
    timerList->setAutoDelete(TRUE);
    zero_timer_count = 0;
}

static void cleanupTimers()			// cleanup timer data structure
{
    zero_timer_count = 0;
    if(timerList) {
	for(register TimerInfo *t = timerList->first(); t; t = timerList->next())
	    killTimer(t, FALSE);
	delete timerList;
	timerList = 0;
    }
    if(timerUPP) {
	DisposeEventLoopTimerUPP(timerUPP);
	timerUPP = NULL;
    }
}

static int qt_activate_timers(TimerInfo::TimerType types = TimerInfo::TIMER_ANY)
{
    if(types == TimerInfo::TIMER_ZERO) {
	if(!zero_timer_count)
	    return 0;
	int ret = 0;
	for(register TimerInfo *t = timerList->first();
	    ret != zero_timer_count && t; t = timerList->next()) {
	    if(t->type == TimerInfo::TIMER_ZERO) {
		ret++;
		QTimerEvent e(t->id);
		QApplication::sendEvent(t->obj, &e);	// send event
	    }
	}
	return ret;
    }

    if(!timerList || !timerList->count())	// no timers
	return 0;
    int n_act = 0;
#ifdef Q_OS_MACX
    bool first = TRUE;
    int maxCount = timerList->count();
    timeval currentTime;
    TimerInfo *begin = 0;
    register TimerInfo *t;
    for ( ;; ) {
	if ( ! maxCount )
	    break;
	getTime(currentTime);			// get current time
	if(first) {
	    if(currentTime < watchtime)	// clock was turned back
		repairTimer(currentTime);
	    first = FALSE;
	    watchtime = currentTime;
	}
	t = timerList->first();
        if(!t) //out of timers
            break;
	if(t->type == TimerInfo::TIMER_QT) {
	    if(currentTime < t->u.qt_timer.timeout) // no timer has expired
		break;
	    if ( ! begin ) {
		begin = t;
	    } else if ( begin == t ) { // avoid sending the same timer multiple times
		break;
	    } else if ( t->u.qt_timer.interval <  begin->u.qt_timer.interval ||
			t->u.qt_timer.interval == begin->u.qt_timer.interval ) {
		begin = t;
	    }
	    timerList->take();			// unlink from list
	    t->u.qt_timer.timeout += t->u.qt_timer.interval;
	    if(t->u.qt_timer.timeout < currentTime)
		t->u.qt_timer.timeout = currentTime + t->u.qt_timer.interval;
	    insertTimer(t);			// relink timer
	    if(t->u.qt_timer.interval.tv_usec > 0 || t->u.qt_timer.interval.tv_sec > 0)
		n_act++;
	    else
		maxCount--;
	    QTimerEvent e(t->id);
	    QApplication::sendEvent(t->obj, &e);	// send event
	    if ( timerList->findRef( begin ) == -1 )
		begin = 0;
	}
    }
#endif
    return n_act;
}

int qStartTimer(int interval, QObject *obj)
{
    if(!timerList)				// initialize timer data
	initTimers();
    TimerInfo *t = new TimerInfo;		// create timer
    t->obj = obj;
    t->pending = TRUE;
    Q_CHECK_PTR(t);
#ifdef Q_OS_MACX
    if(!qt_is_gui_used) {
	t->type = TimerInfo::TIMER_QT;
	t->u.qt_timer.interval.tv_sec  = interval/1000;
	t->u.qt_timer.interval.tv_usec = (interval%1000)*1000;
	timeval currentTime;
	getTime(currentTime);
	t->u.qt_timer.timeout = currentTime + t->u.qt_timer.interval;
    } else
#endif
    if(!interval) {
	t->type = TimerInfo::TIMER_ZERO;
	zero_timer_count++;
    } else {
	t->type = TimerInfo::TIMER_MAC;
	EventTimerInterval mint = (((EventTimerInterval)interval) / 1000);
	if(InstallEventLoopTimer(GetMainEventLoop(), mint, mint,
				 timerUPP, t, &t->u.mac_timer)) {
	    delete t;
	    return 0;
	}
    }
    static int serial_id = 666;
    t->id = serial_id++;
    t->pending = FALSE;
    insertTimer(t);
    return t->id;
}

bool qKillTimer(int id)
{
    if(!timerList || id <= 0)
	return FALSE;				// not init'd or invalid timer
    register TimerInfo *t = timerList->first();
    while(t && (t->id != id)) // find timer info in list
	t = timerList->next();
    if(t)					// id found
	return killTimer(t);
    return FALSE; // id not found
}

bool qKillTimer(QObject *obj)
{
    if(!timerList)				// not initialized
	return FALSE;
    register TimerInfo *t = timerList->first();
    while(t) {				// check all timers
	if(t->obj == obj) {			// object found
	    killTimer(t);
	    t = timerList->current();
	} else {
	    t = timerList->next();
	}
    }
    return TRUE;
}


/*****************************************************************************
  QEventLoop Implementation
 *****************************************************************************/

// Initializations / Cleanup
void QEventLoop::init()
{
#ifdef Q_OS_UNIX
    if(!qt_is_gui_used) {
	pipe(d->thread_pipe);
	fcntl(d->thread_pipe[0], F_SETFD, FD_CLOEXEC);
	fcntl(d->thread_pipe[1], F_SETFD, FD_CLOEXEC);
    }
    d->next_select_timer = 0;
    d->select_timer = NULL;
    d->sn_highest = -1;
#endif
}

void QEventLoop::cleanup()
{
    if(!qt_is_gui_used) {
	close(d->thread_pipe[0]);
	close(d->thread_pipe[1]);
    }
    cleanupTimers();
    if(d->select_timer) {
	RemoveEventLoopTimer(d->select_timer);
	d->select_timer = NULL;
    }
}

void QEventLoop::macHandleTimer(TimerInfo *t)
{
    if(!timerList)
	return;
    if(t && t->pending) {
	t->pending = FALSE;
	QTimerEvent e(t->id);
	QApplication::sendEvent(t->obj, &e);	// send event
    }
}

/**************************************************************************
    Socket Notifiers
 *************************************************************************/
static EventLoopTimerUPP mac_select_timerUPP = NULL;
static void qt_mac_select_cleanup()
{
    DisposeEventLoopTimerUPP(mac_select_timerUPP);
    mac_select_timerUPP = NULL;
}
QMAC_PASCAL void qt_mac_select_timer_callbk(EventLoopTimerRef, void *me)
{
    QEventLoop *eloop = (QEventLoop*)me;
    if(QMacBlockingFunction::blocking()) { //just send it immediately
	timeval tm;
	memset(&tm, '\0', sizeof(tm));
	eloop->macHandleSelect(&tm);
    } else {
	qt_event_request_select(eloop);
    }
}

int QEventLoop::macHandleSelect(timeval *tm)
{
    if(qt_preselect_handler) {
	QVFuncList::Iterator end = qt_preselect_handler->end();
	for(QVFuncList::Iterator it = qt_preselect_handler->begin(); it != end; ++it)
	    (**it)();
    }
#ifdef Q_OS_MACX
    int highest = 0;
    if(d->sn_highest >= 0) {			// has socket notifier(s)
	if(d->sn_vec[0].list && ! d->sn_vec[0].list->isEmpty())
	    d->sn_vec[0].select_fds = d->sn_vec[0].enabled_fds;
	else
	    FD_ZERO(&d->sn_vec[0].select_fds);
	if(d->sn_vec[1].list && ! d->sn_vec[1].list->isEmpty())
	    d->sn_vec[1].select_fds = d->sn_vec[1].enabled_fds;
	else
	    FD_ZERO(&d->sn_vec[1].select_fds);
	if(d->sn_vec[2].list && ! d->sn_vec[2].list->isEmpty())
	    d->sn_vec[2].select_fds = d->sn_vec[2].enabled_fds;
	else
	    FD_ZERO(&d->sn_vec[2].select_fds);
    } else {
	FD_ZERO(&d->sn_vec[0].select_fds);
	FD_ZERO(&d->sn_vec[1].select_fds);
	FD_ZERO(&d->sn_vec[2].select_fds);
    }
    highest = d->sn_highest;

    if(!qt_is_gui_used) {
	FD_SET(d->thread_pipe[0], &d->sn_vec[0].select_fds);
	highest = QMAX(highest, d->thread_pipe[0]);
    }
    int nsel;
    do {
	nsel = select(highest + 1, &d->sn_vec[0].select_fds,
		      d->sn_vec[1].list ? &d->sn_vec[1].select_fds : 0,
		      d->sn_vec[2].list ? &d->sn_vec[2].select_fds : 0, tm);
    } while (nsel == -1 && (errno == EINTR || errno == EAGAIN));
#endif
    if(qt_postselect_handler) {
	QVFuncList::Iterator end = qt_postselect_handler->end();
	for(QVFuncList::Iterator it = qt_postselect_handler->begin(); it != end; ++it)
	    (**it)();
    }
#ifdef Q_OS_MACX
    if(nsel == -1) {
	if (errno == EBADF) {
	    // it seems a socket notifier has a bad fd... find out
	    // which one it is and disable it
	    fd_set fdset;
	    timeval zerotm;
	    if (!tm)
		tm = &zerotm;
	    tm->tv_sec = tm->tv_usec = 0l;

	    for (int type = 0; type < 3; ++type) {
		QPtrList<QSockNot> *list = d->sn_vec[type].list;
		if (!list) continue;

		QSockNot *sn = list->first();
		while (sn) {
		    FD_ZERO(&fdset);
		    FD_SET(sn->fd, &fdset);

		    int ret;
		    do {
			switch (type) {
                        default:
                            ret = -1;
			case 0: // read
			    ret = select(sn->fd + 1, &fdset, 0, 0, tm);
			    break;
			case 1: // write
			    ret = select(sn->fd + 1, 0, &fdset, 0, tm);
			    break;
			case 2: // except
			    ret = select(sn->fd + 1, 0, 0, &fdset, tm);
			    break;
			}
		    } while (ret == -1 && (errno == EINTR || errno == EAGAIN));

		    if (ret == -1 && errno == EBADF) {
			// disable the invalid socket notifier
			static const char *t[] = { "Read", "Write", "Exception" };
			qWarning("QSocketNotifier: invalid socket %d and type '%s', disabling...",
				 sn->fd, t[type]);
			sn->obj->setEnabled(false);
		    }

		    sn = list->next();
		}
	    }
	} else {
	    // EINVAL... shouldn't happen, so let's complain to stderr
	    // and hope someone sends us a bug report
	    perror( "select" );
	}
    }
    if(nsel > 0 && highest >= 0) {
	if(qt_is_gui_used) {
	    qt_event_request_updates();
	} else if(FD_ISSET(d->thread_pipe[0], &d->sn_vec[0].select_fds)) {
	    char c;
	    ::read(d->thread_pipe[0], &c, 1);
	}
	// if select says data is ready on any socket, then set the socket notifier
	// to pending
	for(int i=0; i<3; i++) {
	    if(!d->sn_vec[i].list)
		continue;

	    QPtrList<QSockNot> *list = d->sn_vec[i].list;
	    QSockNot *sn = list->first();
	    while(sn) {
		if(FD_ISSET(sn->fd, &d->sn_vec[i].select_fds))
		    setSocketNotifierPending(sn->obj);
		sn = list->next();
	    }
	}
	return activateSocketNotifiers();
    }
    return 0;
#endif
}

void QEventLoop::registerSocketNotifier(QSocketNotifier *notifier)
{
#ifdef Q_OS_UNIX
    int sockfd = notifier->socket();
    int type = notifier->type();
    if(sockfd < 0 || type < 0 || type > 2 || notifier == 0) {
#if defined(QT_CHECK_RANGE)
	qWarning("Qt: QSocketNotifier: Internal error");
#endif
	return;
    }

    QPtrList<QSockNot>  *list = d->sn_vec[type].list;
    fd_set *fds  = &d->sn_vec[type].enabled_fds;
    QSockNot *sn;

    if(!list) {
	// create new list, the QSockNotType destructor will delete it for us
	list = new QPtrList<QSockNot>;
	Q_CHECK_PTR(list);
	list->setAutoDelete(TRUE);
	d->sn_vec[type].list = list;
    }

    sn = new QSockNot;
    Q_CHECK_PTR(sn);
    sn->obj = notifier;
    sn->fd = sockfd;
    sn->queue = &d->sn_vec[type].pending_fds;

    if(list->isEmpty()) {
	list->insert(0, sn);
    } else {				// sort list by fd, decreasing
	QSockNot *p = list->first();
	while(p && p->fd > sockfd)
	    p = list->next();
#if defined(QT_CHECK_STATE)
	if(p && p->fd == sockfd) {
	    static const char *t[] = { "read", "write", "exception" };
	    qWarning("Qt: QSocketNotifier: Multiple socket notifiers for "
		      "same socket %d and type %s", sockfd, t[type]);
	}
#endif
	if(p)
	    list->insert(list->at(), sn);
	else
	    list->append(sn);
    }

    FD_SET(sockfd, fds);
    d->sn_highest = QMAX(d->sn_highest, sockfd);
    if(qt_is_gui_used && !d->select_timer) {
	if(!mac_select_timerUPP) {
	    mac_select_timerUPP = NewEventLoopTimerUPP(qt_mac_select_timer_callbk);
	    qAddPostRoutine(qt_mac_select_cleanup);
	}
	InstallEventLoopTimer(GetMainEventLoop(), 0.1, 0.1,
			      mac_select_timerUPP, (void *)this, &d->select_timer);
    }
#endif
}

void QEventLoop::unregisterSocketNotifier(QSocketNotifier *notifier)
{
#ifdef Q_OS_UNIX
    int sockfd = notifier->socket();
    int type = notifier->type();
    if(sockfd < 0 || type < 0 || type > 2 || notifier == 0) {
#if defined(QT_CHECK_RANGE)
	qWarning("Qt: QSocketNotifier: Internal error");
#endif
	return;
    }

    QPtrList<QSockNot> *list = d->sn_vec[type].list;
    fd_set *fds  =  &d->sn_vec[type].enabled_fds;
    QSockNot *sn;
    if(!list)
	return;
    sn = list->first();
    while(sn && !(sn->obj == notifier && sn->fd == sockfd))
	sn = list->next();
    if(!sn) // not found
	return;
    FD_CLR(sockfd, fds);			// clear fd bit
    FD_CLR(sockfd, sn->queue);
    d->sn_pending_list.removeRef(sn);		// remove from activation list
    list->remove();				// remove notifier found above

    if(d->sn_highest == sockfd) {		// find highest fd
	d->sn_highest = -1;
	for(int i=0; i<3; i++) {
	    if(d->sn_vec[i].list && ! d->sn_vec[i].list->isEmpty())
		d->sn_highest = QMAX(d->sn_highest,  // list is fd-sorted
				      d->sn_vec[i].list->getFirst()->fd);
	}
    }
    if(d->sn_highest == -1 && d->select_timer) {
	RemoveEventLoopTimer(d->select_timer);
	d->select_timer = NULL;
    }
#endif
}

int QEventLoop::activateSocketNotifiers()
{
    if(d->sn_pending_list.isEmpty())
	return 0;

    // activate entries
    int n_act = 0;
    QEvent event(QEvent::SockAct);
    QPtrListIterator<QSockNot> it(d->sn_pending_list);
    QSockNot *sn;
    while((sn=it.current())) {
	++it;
	d->sn_pending_list.removeRef(sn);
	if(FD_ISSET(sn->fd, sn->queue)) {
	    FD_CLR(sn->fd, sn->queue);
	    QApplication::sendEvent(sn->obj, &event);
	    n_act++;
	}
    }
    if(d->select_timer && (n_act || d->next_select_timer < 10)) {
	if(n_act)
	    d->next_select_timer = 0;
	else
	    d->next_select_timer++;
	SetEventLoopTimerNextFireTime(d->select_timer, 0);
    }
    return n_act;
}

void QEventLoop::setSocketNotifierPending(QSocketNotifier *notifier)
{
    int sockfd = notifier->socket();
    int type = notifier->type();
    if(sockfd < 0 || type < 0 || type > 2 || notifier == 0) {
#if defined(QT_CHECK_RANGE)
	qWarning("Qt: QSocketNotifier: Internal error");
#endif
	return;
    }

    QSockNot *sn = NULL;
    if(QPtrList<QSockNot> *list = d->sn_vec[type].list)
	for(sn = list->first(); sn && (sn->obj != notifier || sn->fd != sockfd); sn = list->next());
    if(!sn) // not found
	return;

    // We choose a random activation order to be more fair under high load.
    // If a constant order is used and a peer early in the list can
    // saturate the IO, it might grab our attention completely.
    // Also, if we're using a straight list, the callback routines may
    // delete other entries from the list before those other entries are
    // processed.
    if(!FD_ISSET(sn->fd, sn->queue)) {
	d->sn_pending_list.insert((rand() & 0xff) % (d->sn_pending_list.count()+1), sn);
	FD_SET(sn->fd, sn->queue);
    }
}

bool QEventLoop::hasPendingEvents() const
{
    extern uint qGlobalPostedEventsCount();
    return qGlobalPostedEventsCount() || (qt_is_gui_used && GetNumEventsInQueue(GetMainEventQueue()));
}

bool QEventLoop::processEvents(ProcessEventsFlags flags)
{
#if 0
    //TrackDrag says you may not use the EventManager things..
    if(qt_mac_in_drag) {
	qWarning("Qt: Cannot process events whilst dragging!");
	return FALSE;
    }
#endif
    int	   nevents = 0;
#if defined(QT_THREAD_SUPPORT)
    QMutexLocker locker(QApplication::qt_mutex);
#endif
    d->exitloop = FALSE;

    if(qt_is_gui_used) {
	if(!qt_mac_safe_pdev) { //create an empty widget and this can be used for a port anytime
	    QWidget *tlw = new QWidget(NULL, "empty_widget", Qt::WDestructiveClose);
	    tlw->hide();
	    qt_mac_safe_pdev = tlw;
	}

	QApplication::sendPostedEvents();
	qt_activate_timers(TimerInfo::TIMER_ZERO); //try to send null timers..

	EventRef event;
	do {
            if(d->exitloop)
                break;
	    do {
		if(ReceiveNextEvent(0, 0, QMAC_EVENT_NOWAIT, TRUE, &event) != noErr)
		    break;
		if(qt_mac_send_event(flags, event))
		    nevents++;
		ReleaseEvent(event);
	    } while(!d->exitloop && GetNumEventsInQueue(GetMainEventQueue()));
	    QApplication::sendPostedEvents();
	} while(GetNumEventsInQueue(GetMainEventQueue()));
    }
    if(d->quitnow || d->exitloop)
	return FALSE;

    QApplication::sendPostedEvents();
    bool canWait = (d->exitloop || d->quitnow) ? FALSE : (flags & WaitForMore);

    if(!qt_is_gui_used) {
	timeval *tm = NULL;
	if(!canWait) { 		// no time to wait
	    static timeval zerotm;
	    if(!tm)
		tm = &zerotm;
	    tm->tv_sec  = 0;
	    tm->tv_usec = 0;
	} else {
	    tm = qt_wait_timer();
	}
	if(!(flags & ExcludeSocketNotifiers)) {
	    emit aboutToBlock();
#if defined(QT_THREAD_SUPPORT)
	    locker.mutex()->unlock();
#endif
	    nevents += macHandleSelect(tm);
#if defined(QT_THREAD_SUPPORT)
	    locker.mutex()->lock();
#endif
	}

	// we are awake, broadcast it
	emit awake();
	emit qApp->guiThreadAwake();

	nevents += qt_activate_timers();
    } else if(canWait && !zero_timer_count) {
	emit aboutToBlock();
#if defined(QT_THREAD_SUPPORT)
	locker.mutex()->unlock();
#endif
#if defined( QMAC_USE_APPLICATION_EVENT_LOOP )
	RunApplicationEventLoop();
#else
	while(CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1.0e20, true) == kCFRunLoopRunTimedOut);
#endif
#if defined(QT_THREAD_SUPPORT)
	locker.mutex()->lock();
#endif

	// we are awake, broadcast it
	emit awake();
	emit qApp->guiThreadAwake();
    }
    return nevents > 0;
}

int QEventLoop::timeToWait() const
{
    if(qt_is_gui_used)
	return -1;
    if(timeval *tm = qt_wait_timer())
	return (tm->tv_sec*1000) + (tm->tv_usec/1000);
    return -1;
}

int QEventLoop::activateTimers()
{
    if (qt_is_gui_used)
	return 0;
    return qt_activate_timers();
}

void QEventLoop::wakeUp()
{
    if(qt_is_gui_used) {
	qt_event_request_wakeup();
	return;
    }
    size_t nbytes = 0;
    char c = 0;
    if ( ::ioctl( d->thread_pipe[0], FIONREAD, (char*)&nbytes ) >= 0 && nbytes == 0 )
        ::write(d->thread_pipe[1], &c, 1);
}



/* This allows the eventloop to block, and will keep things going - including keeping away
   the evil spinning cursor */
int QMacBlockingFunction::block = 0;
QMacBlockingFunction::QMacBlockingFunction()
{
    if(!block)
	startTimer(100);
    block++;
}

void QMacBlockingFunction::timerEvent(QTimerEvent *)
{
    if(qt_activate_timers(TimerInfo::TIMER_ZERO))
	QApplication::flush();
}
