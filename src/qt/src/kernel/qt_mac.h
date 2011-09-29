/****************************************************************************
** $Id: qt/qt_mac.h   3.3.8   edited Jan 11 14:38 $
**
** Definition of ???
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

#ifndef QT_MAC_H
#define QT_MAC_H

#undef OLD_DEBUG
#ifdef DEBUG
#define OLD_DEBUG DEBUG
#undef DEBUG
#endif
#define DEBUG 0

#ifndef __IMAGECAPTURE__
#define __IMAGECAPTURE__
#endif
#include <Carbon/Carbon.h>
#include <QuickTime/Movies.h>
#undef QT_BUILD_KEY
#include <qconfig.h> //We need this to get QT_MACOSX_VERSION
#include "qglobal.h"

#ifdef Q_OS_MAC9
# define QMAC_DEFAULT_STYLE "QPlatinumStyle" //Default style
# include "qt_mac9.h"
#elif defined(Q_OS_MACX)
# define QMAC_DEFAULT_STYLE "QMacStyle" //DefaultStyle
#endif

#if !defined(Q_WS_MACX) || QT_MACOSX_VERSION < 0x1020 || (QT_MACOSX_VERSION >= 0x1030 && QT_MACOSX_VERSION <= 0x1040)
# define QMAC_NO_FAKECURSOR
#endif

/* We don't use the ApplicationEventLoop because it can causes bad behaviour in
   multithreaded applications. I've left the code in however because using the
   ApplicationEventLoop solved other problems (ages ago) - for example the gumdrop
   "hover" effects. */
//#define QMAC_USE_APPLICATION_EVENT_LOOP

#undef DEBUG
#ifdef OLD_DEBUG
#define DEBUG OLD_DEBUG
#endif
#undef OLD_DEBUG

#ifdef Q_WS_MAC
#include "qpainter.h"
#include "qwidget.h"
extern int mac_window_count; //qwidget_mac.cpp
#ifdef QT_THREAD_SUPPORT
#include "qthread.h"
extern QMutex *qt_mac_port_mutex; //qapplication_mac.cpp
#endif

class QMacBlockingFunction : public QObject //done in qapplication_mac.cpp
{
private:
    static int block;
public:
    QMacBlockingFunction();
    ~QMacBlockingFunction() { block--; }
    static bool blocking() { return block != 0; }

protected:
    void timerEvent(QTimerEvent *);
};

class QMacSavedFontInfo 
{
private:
    void init(CGrafPtr);
protected:
    short tfont, tface;
    int tsize;
public:
    inline QMacSavedFontInfo() { GWorldPtr w; GDHandle h; GetGWorld(&w, &h); init(w); }
    inline QMacSavedFontInfo(CGrafPtr w) { init(w); }
    ~QMacSavedFontInfo();
};

inline QMacSavedFontInfo::~QMacSavedFontInfo() 
{
    if(mac_window_count) {
	TextFont(tfont);
	TextFace(tface);
	TextSize(tsize);
    }
}

inline void QMacSavedFontInfo::init(CGrafPtr w) 
{
    if(mac_window_count) {
	tfont = GetPortTextFont(w);
	tface = GetPortTextFace(w);
	tsize = GetPortTextSize(w);
    }
}

class QMacFontInfo
{
public:
    inline QMacFontInfo() : fi_fnum(0), fi_face(0), fi_size(0), fi_enc(0), fi_astyle(0)
	{ }
    inline ~QMacFontInfo() 
	{ if(fi_astyle && fi_astyle->deref()) {
	    ATSUDisposeStyle(fi_astyle->style);
	    delete fi_astyle;
	} }
    inline QMacFontInfo &operator=(const QMacFontInfo &rhs) {
	setEncoding(rhs.encoding());
	setFont(rhs.font());
	setStyle(rhs.style());
	setSize(rhs.size());
	if(rhs.atsuStyle()) {
	    rhs.atsuStyle()->ref();
	    setATSUStyle(rhs.atsuStyle());
	} else {
	    if(fi_astyle && fi_astyle->deref()) {
		ATSUDisposeStyle(fi_astyle->style);
		delete fi_astyle;
	    }
	    setStyle(0);
	}
	return *this;
    }

    inline TextEncoding encoding() const { return fi_enc; }
    inline void setEncoding(TextEncoding f) { fi_enc = f; }

    inline short font() const { return fi_fnum; }
    inline void setFont(short f) { fi_fnum = f; }

    inline short style() const { return fi_face; }
    inline void setStyle(short f) { fi_face = f; }

    inline int size() const { return fi_size; }
    inline void setSize(int f) { fi_size = f; }

    struct QATSUStyle : public QShared {
	ATSUStyle style;
	RGBColor rgb;
    };
    inline QATSUStyle *atsuStyle() const { return fi_astyle; }
    inline void setATSUStyle(QATSUStyle *s) { fi_astyle = s; }

private:
    short fi_fnum, fi_face;
    int fi_size;
    TextEncoding fi_enc;
    QATSUStyle *fi_astyle;
};

class QFontEngine;
class QFontDef;
class QFontPrivate;
class QMacSetFontInfo : public QMacSavedFontInfo, public QMacFontInfo 
{
private:
    static QMacFontInfo *createFontInfo(const QFontEngine *fe, const QFontDef *def, QPaintDevice *pdev);

public:
    //create this for temporary font settting
    inline QMacSetFontInfo(const QFontPrivate *d, QPaintDevice *pdev) : QMacSavedFontInfo(), 
									QMacFontInfo() { setMacFont(d, this, pdev); }
    inline QMacSetFontInfo(const QFontEngine *fe, QPaintDevice *pdev) : QMacSavedFontInfo(), 
									QMacFontInfo() { setMacFont(fe, this, pdev); }

    //you can use these to cause font setting, without restoring old
    static bool setMacFont(const QMacFontInfo *f, QMacSetFontInfo *sfi=NULL);
    static bool setMacFont(const QFontPrivate *d, QMacSetFontInfo *sfi=NULL, QPaintDevice *pdev=NULL);
    static bool setMacFont(const QFontEngine *fe, QMacSetFontInfo *sfi=NULL, QPaintDevice *pdev=NULL);
};


#include "qptrlist.h"
#include "qpaintdevice.h"
extern QPaintDevice *qt_mac_safe_pdev; //qapplication_mac.cpp
extern QPainter *qt_mac_current_painter; //qpainter_mac.cpp
class QMacSavedPortInfo
{
    RgnHandle clip;
    GWorldPtr world;
    GDHandle handle;
    PenState pen; //go pennstate
    RGBColor back, fore;
    QMacSavedFontInfo *fi;
    QPainter *painter;
    bool valid_gworld;
    void init();
    
public:
    inline QMacSavedPortInfo() { init(); }
    inline QMacSavedPortInfo(QPaintDevice *pd) { init(); setPaintDevice(pd); }
    inline QMacSavedPortInfo(QWidget *w, bool set_clip=FALSE) { init(); setPaintDevice(w, set_clip); }
    inline QMacSavedPortInfo(QPaintDevice *pd, const QRect &r) 
	{ init(); setPaintDevice(pd); setClipRegion(r); }
    inline QMacSavedPortInfo(QPaintDevice *pd, const QRegion &r) 
	{ init(); setPaintDevice(pd); setClipRegion(r); }
    ~QMacSavedPortInfo();
    static bool setClipRegion(const QRect &r);
    static bool setClipRegion(const QRegion &r);
    static bool setPaintDevice(QPaintDevice *);
    static bool setPaintDevice(QWidget *, bool set_clip=FALSE, bool with_child=TRUE);
    static bool flush(QPaintDevice *);
    static bool flush(QPaintDevice *, QRegion r, bool force=FALSE);
    static void setWindowAlpha(QWidget *, float);
};

inline bool 
QMacSavedPortInfo::flush(QPaintDevice *pdev) 
{
#ifdef Q_WS_MACX
    if(pdev->devType() == QInternal::Widget) {
	QWidget *w = (QWidget *)pdev;
	if(!w->isHidden() && QDIsPortBuffered(GetWindowPort((WindowPtr)w->handle()))) {
	    QDFlushPortBuffer(GetWindowPort((WindowPtr)w->handle()), NULL);
	    return TRUE;
	}
    } 
#else
    Q_UNUSED(pdev);
#endif
    return FALSE;
}

inline bool 
QMacSavedPortInfo::flush(QPaintDevice *pdev, QRegion r, bool force) 
{
#ifdef Q_WS_MACX
    if(pdev->devType() == QInternal::Widget) {
	QWidget *w = (QWidget *)pdev;
	r.translate(w->topLevelWidget()->geometry().x(), w->topLevelWidget()->geometry().y());
	if(!w->isHidden() || QDIsPortBuffered(GetWindowPort((WindowPtr)w->handle()))) {
	    QDFlushPortBuffer(GetWindowPort((WindowPtr)w->handle()), r.handle(force));
	    return TRUE;
	}
    } 
#else
    Q_UNUSED(pdev);
    Q_UNUSED(r);
    Q_UNUSED(force);
#endif
    return FALSE;
}

#ifdef Q_WS_MACX
extern "C" {
    typedef struct CGSConnection *CGSConnectionRef;
    typedef struct CGSWindow *CGSWindowRef;
    extern OSStatus CGSSetWindowAlpha(CGSConnectionRef, CGSWindowRef, float);
    extern CGSWindowRef GetNativeWindowFromWindowRef(WindowRef);
    extern CGSConnectionRef _CGSDefaultConnection();
}
#endif
inline void 
QMacSavedPortInfo::setWindowAlpha(QWidget *w, float l)
{
#ifdef Q_WS_MACX
    CGSSetWindowAlpha(_CGSDefaultConnection(), 
		      GetNativeWindowFromWindowRef((WindowRef)w->handle()), l);
#else
    Q_UNUSED(w);
    Q_UNUSED(l);
#endif
}

inline bool 
QMacSavedPortInfo::setClipRegion(const QRect &rect)
{
    Rect r;
    SetRect(&r, rect.x(), rect.y(), rect.right()+1, rect.bottom()+1);
#if defined(QT_THREAD_SUPPORT)
    if(qt_mac_port_mutex)
	qt_mac_port_mutex->lock();
#endif
    qt_mac_current_painter = NULL;
    ClipRect(&r);
#if defined(QT_THREAD_SUPPORT)
    if(qt_mac_port_mutex)
	qt_mac_port_mutex->unlock();
#endif
    return TRUE;
}

inline bool 
QMacSavedPortInfo::setClipRegion(const QRegion &r)
{
    if(r.isNull())
	return setClipRegion(QRect());
    else if(!r.handle())
	return setClipRegion(r.boundingRect());
#if defined(QT_THREAD_SUPPORT)
    if(qt_mac_port_mutex)
	qt_mac_port_mutex->lock();
#endif
    qt_mac_current_painter = NULL;
    SetClip(r.handle());
#if defined(QT_THREAD_SUPPORT)
    if(qt_mac_port_mutex)
	qt_mac_port_mutex->unlock();
#endif
    return TRUE;
}

inline bool
QMacSavedPortInfo::setPaintDevice(QWidget *w, bool set_clip, bool with_child) 
{
    if(!w)
	return FALSE;
    if(!setPaintDevice((QPaintDevice *)w))
	return FALSE;
    if(set_clip) 
	return setClipRegion(w->clippedRegion(with_child));
    return TRUE;
}

inline bool
QMacSavedPortInfo::setPaintDevice(QPaintDevice *pd)
{
    if(!pd)
	return FALSE;
#if 0
    if(qt_mac_current_painter && qt_mac_current_painter->handle() == pd->handle())
	return TRUE;
#endif
    bool ret = TRUE;
#if defined(QT_THREAD_SUPPORT)
    if(qt_mac_port_mutex)
	qt_mac_port_mutex->lock();
#endif
//    if(qt_mac_current_painter && qt_mac_current_painter->handle() != pd->handle())
	qt_mac_current_painter = NULL;
    if(pd->devType() == QInternal::Widget)
	SetPortWindowPort((WindowPtr)pd->handle());
    else if(pd->devType() == QInternal::Pixmap || pd->devType() == QInternal::Printer)
	SetGWorld((GrafPtr)pd->handle(), 0); //set the gworld
    else
	ret = FALSE;
#if defined(QT_THREAD_SUPPORT)
    if(qt_mac_port_mutex)
	qt_mac_port_mutex->unlock();
#endif
    return ret;
}
    

inline void 
QMacSavedPortInfo::init()
{
#if defined(QT_THREAD_SUPPORT)
    if(qt_mac_port_mutex)
	qt_mac_port_mutex->lock();
#endif
    fi = NULL;
    painter = qt_mac_current_painter;
    if(mac_window_count) {
   	GetBackColor(&back);
	GetForeColor(&fore);
	GetGWorld(&world, &handle);
	valid_gworld = TRUE;
	fi = new QMacSavedFontInfo(world);
	clip = NewRgn();
	GetClip(clip);
	GetPenState(&pen);
    }
}

inline QMacSavedPortInfo::~QMacSavedPortInfo()
{
    if(mac_window_count) {
        bool set_state = false;
        if(valid_gworld) {
            set_state = IsValidPort(world);
            if(set_state)
                SetGWorld(world,handle); //always do this one first
        } else {
            setPaintDevice(qt_mac_safe_pdev);
        }
        if(set_state) {
            SetClip(clip);
            SetPenState(&pen);
            RGBForeColor(&fore);
            RGBBackColor(&back);
        }
        DisposeRgn(clip);
    }
    if(fi)
	delete fi;
    qt_mac_current_painter = painter;
#if defined(QT_THREAD_SUPPORT)
    if(qt_mac_port_mutex)
        qt_mac_port_mutex->unlock();
#endif
}

#endif //Q_WS_MAC
#endif // QT_MAC_H
