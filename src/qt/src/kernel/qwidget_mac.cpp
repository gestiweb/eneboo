/****************************************************************************
** $Id: qt/qwidget_mac.cpp   3.3.8   edited Jan 16 10:54 $
**
** Implementation of QWidget and QWindow classes for mac
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

#include "qt_mac.h"

#include "qimage.h"
#include "qapplication.h"
#include "qapplication_p.h"
#include "qpaintdevicemetrics.h"
#include "qpainter.h"
#include "qbitmap.h"
#include "qwidgetlist.h"
#include "qwidgetintdict.h"
#include "qobjectlist.h"
#include "qaccel.h"
#include "qdragobject.h"
#include "qfocusdata.h"
#include "qlayout.h"
#include "qtextcodec.h"
#include "qcursor.h"
#include "qtimer.h"
#include "qstyle.h"
#ifdef Q_WS_MACX
# include <ApplicationServices/ApplicationServices.h>
#endif
#include <limits.h>

/*****************************************************************************
  QWidget debug facilities
 *****************************************************************************/
//#define DEBUG_WINDOW_RGNS
//#define DEBUG_WINDOW_CREATE

/*****************************************************************************
  QWidget globals
 *****************************************************************************/
static bool no_move_blt = FALSE;
static WId serial_id = 0;
static WindowGroupRef qt_mac_stays_on_top_group = 0;
QWidget *mac_mouse_grabber = 0;
QWidget *mac_keyboard_grabber = 0;
int mac_window_count = 0;

/*****************************************************************************
  Externals
 *****************************************************************************/
QSize qt_naturalWidgetSize(QWidget *); //qwidget.cpp
QString cfstring2qstring(CFStringRef); //qglobal.cpp
void qt_mac_unicode_reset_input(QWidget *); //qapplication_mac.cpp
void qt_mac_unicode_init(QWidget *); //qapplication_mac.cpp
void qt_mac_unicode_cleanup(QWidget *); //qapplication_mac.cpp
void qt_event_request_updates(); //qapplication_mac.cpp
void qt_event_request_activate(QWidget *); //qapplication_mac.cpp
bool qt_event_remove_activate(); //qapplication_mac.cpp
void qt_mac_event_release(QWidget *w); //qapplication_mac.cpp
void qt_event_request_showsheet(QWidget *); //qapplication_mac.cpp
IconRef qt_mac_create_iconref(const QPixmap &); //qpixmap_mac.cpp
extern void qt_mac_set_cursor(const QCursor *, const Point *); //qcursor_mac.cpp
bool qt_nograb();
RgnHandle qt_mac_get_rgn(); //qregion_mac.cpp
void qt_mac_dispose_rgn(RgnHandle r); //qregion_mac.cpp
void qt_macdnd_reregister(QWidget *, QWExtra *); //qdnd_mac
void qt_macdnd_unregister(QWidget *, QWExtra *); //dnd_mac
void qt_macdnd_register(QWidget *, QWExtra *); //dnd_mac
void unclippedBitBlt(QPaintDevice *, int, int, const QPaintDevice *, int, int,
		      int, int, Qt::RasterOp, bool, bool); //qpaintdevice_mac.cpp

/*****************************************************************************
  QWidget utility functions
 *****************************************************************************/
QPoint posInWindow(QWidget *w)
{
    if(w->isTopLevel())
	return QPoint(0, 0);
    int x = 0, y = 0;
    if(QWidget *par = w->parentWidget(TRUE)) {
	QPoint p = posInWindow(par);
	x = p.x() + w->geometry().x();
	y = p.y() + w->geometry().y();
    }
    return QPoint(x, y);
}

static void qt_mac_release_stays_on_top_group()
{
    ReleaseWindowGroup(qt_mac_stays_on_top_group);
    if(GetWindowGroupRetainCount(qt_mac_stays_on_top_group) == 1) { //only the global pointer exists
	ReleaseWindowGroup(qt_mac_stays_on_top_group);
	qt_mac_stays_on_top_group = 0;
    }
}

/* Use this function instead of ReleaseWindowGroup, this will be sure to release the
   stays on top window group (created with qt_mac_get_stays_on_top_group below) */
static void qt_mac_release_window_group(WindowGroupRef group)
{
    if(group == qt_mac_stays_on_top_group)
	qt_mac_release_stays_on_top_group();
    else
	ReleaseWindowGroup(group);
}
#define ReleaseWindowGroup(x) Are you sure you wanted to do that? (you wanted qt_mac_release_window_group)

/* We create one static stays on top window group so that all stays on top (aka popups) will
   fall into the same group and be able to be raise()'d with releation to one another (from
   within the same window group). */
static WindowGroupRef qt_mac_get_stays_on_top_group()
{
    if(!qt_mac_stays_on_top_group) {
	CreateWindowGroup(kWindowActivationScopeNone, &qt_mac_stays_on_top_group);
	SetWindowGroupLevel(qt_mac_stays_on_top_group, kCGOverlayWindowLevelKey);
	SetWindowGroupParent(qt_mac_stays_on_top_group, GetWindowGroupOfClass(kAllWindowClasses));
    }
    RetainWindowGroup(qt_mac_stays_on_top_group);
    return qt_mac_stays_on_top_group;
}

static inline const Rect *mac_rect(const QRect &qr)
{
    static Rect r;
    SetRect(&r, qr.left(), qr.top(), qr.right()+1, qr.bottom()+1); //qt says be inclusive!
    return &r;
}
static inline const Rect *mac_rect(const QPoint &qp, const QSize &qs) { return mac_rect(QRect(qp, qs)); }

#ifdef DEBUG_WINDOW_RGNS
static inline void debug_wndw_rgn(const char *where, QWidget *w, const QRegion &r,
				  bool clean=FALSE, bool translate=FALSE) {
    QPoint mp(posInWindow(w));
    QRect wrect(mp.x(), mp.y(), w->width(), w->height());
    qDebug("Qt: internal: %s %s %s (%s) [ %d %d %d %d ]", where, clean ? "clean" : "dirty",
	   w->className(), w->name(), wrect.x(), wrect.y(), wrect.width(), wrect.height());
    QMemArray<QRect> rs = r.rects();
    int offx = 0, offy = 0;
    if(translate) {
	offx = mp.x();
	offy = mp.y();
    }
    for(uint i = 0; i < rs.count(); i++) {
	QRect srect(rs[i].x()+offx, rs[i].y()+offy, rs[i].width(), rs[i].height());
	// * == Completely inside the widget, - == intersects, ? == completely unrelated
	qDebug("Qt: internal: %c(%c) %d %d %d %d",
	       !wrect.intersects(srect) ? '?' : (wrect.contains(srect) ? '*' : '-'),
	       !w->clippedRegion().contains(srect) ? '?' :
	       (!QRegion(w->clippedRegion() ^ srect).isEmpty() ? '*' : '-'),
	       srect.x(), srect.y(), srect.width(), srect.height());
    }
    qDebug("Qt: internal: *****End debug..");
}
static inline void debug_wndw_rgn(const char *where, QWidget *w, const Rect *r, bool clean=FALSE) {
    debug_wndw_rgn(where + QString(" (rect)"), w,
		   QRegion(r->left, r->top, r->right - r->left, r->bottom - r->top), clean);
}
#define qt_dirty_wndw_rgn(x, who, where) do { if(qt_dirty_wndw_rgn_internal(who, where)) \
                                                 debug_wndw_rgn(x, who, where); } while(0);

#define qt_clean_wndw_rgn(x, who, where) do { if(qt_dirty_wndw_rgn_internal(who, where, TRUE)) \
                                                 debug_wndw_rgn(x, who, where, TRUE); } while(0);
#define qt_dirty_wndw_rgn(x, who, where) do { if(qt_dirty_wndw_rgn_internal(who, where)) \
                                                 debug_wndw_rgn(x, who, where); } while(0);
#else
#define clean_wndw_rgn(w, x, y)
#define debug_wndw_rgn(w, x, y)
#define qt_clean_wndw_rgn(x, who, where) qt_dirty_wndw_rgn_internal(who, where, TRUE);
#define qt_dirty_wndw_rgn(x, who, where) qt_dirty_wndw_rgn_internal(who, where);
#endif
static inline bool qt_dirty_wndw_rgn_internal(const QWidget *p, const Rect *r, bool clean=FALSE)
{
    if(qApp->closingDown())
	return FALSE;
    else if(r->right < 0 || r->bottom < 0 || r->left > p->topLevelWidget()->width() ||
	    r->top > p->topLevelWidget()->height())
	return FALSE;
    if(clean) {
	ValidWindowRect((WindowPtr)p->handle(), r);
    } else {
	InvalWindowRect((WindowPtr)p->handle(), r);
	qt_event_request_updates();
    }
    return TRUE;
}
inline bool qt_dirty_wndw_rgn_internal(const QWidget *p, const QRect &r, bool clean=FALSE)
{
    if(qApp->closingDown() || r.isNull())
	return FALSE;
    return qt_dirty_wndw_rgn_internal(p, mac_rect(r), clean);
}
inline bool qt_dirty_wndw_rgn_internal(const QWidget *p, const QRegion &r, bool clean=FALSE)
{
    if(qApp->closingDown())
	return FALSE;
    else if(r.isNull())
	return FALSE;
    else if(!r.handle())
	return qt_dirty_wndw_rgn_internal(p, r.boundingRect(), clean);
    if(clean) {
	ValidWindowRgn((WindowPtr)p->handle(), r.handle());
    } else {
	InvalWindowRgn((WindowPtr)p->handle(), r.handle());
	qt_event_request_updates();
    }
    return TRUE;
}

static OSStatus qt_mac_create_window(WindowClass wclass, WindowAttributes wattr,
				     Rect *geo, WindowPtr *w)
{
    OSStatus ret;
    if(geo->right == geo->left)
	geo->right++;
    if(geo->bottom == geo->top)
	geo->bottom++;
    if(qMacVersion() >= Qt::MV_PANTHER) {
	Rect null_rect; SetRect(&null_rect, 0, 0, 0, 0);
	ret = CreateNewWindow(wclass, wattr, &null_rect, w);
	if(ret == noErr) {
	    ret = SetWindowBounds(*w, kWindowContentRgn, geo);
	    if(ret != noErr)
		qWarning("%s:%d This error shouldn't really ever happen!!!", __FILE__, __LINE__);
	}
    } else {
	ret = CreateNewWindow(wclass, wattr, geo, w);
    }
    return ret;
}

bool qt_mac_update_sizer(QWidget *w, int up=0)
{
    if(!w || !w->isTopLevel())
	return FALSE;

    w->createTLExtra();
    w->extra->topextra->resizer += up;
    {
	WindowClass wclass;
	GetWindowClass((WindowPtr)w->handle(), &wclass);
	if(!(GetAvailableWindowAttributes(wclass) & kWindowResizableAttribute))
	    return TRUE;
    }
    bool remove_grip = (w->extra->topextra->resizer ||
			(w->extra->maxw && w->extra->maxh && w->extra->maxw == w->extra->minw && w->extra->maxh == w->extra->minh));

    WindowAttributes attr;
    GetWindowAttributes((WindowRef)w->handle(), &attr);
    if(remove_grip) {
	if(attr & kWindowResizableAttribute) {
	    ChangeWindowAttributes((WindowRef)w->handle(), kWindowNoAttributes,
				    kWindowResizableAttribute);
	    w->dirtyClippedRegion(TRUE);
	    ReshapeCustomWindow((WindowPtr)w->handle());
	    qt_dirty_wndw_rgn("Remove size grip", w, w->rect());
	}
    } else if(!(attr & kWindowResizableAttribute)) {
	ChangeWindowAttributes((WindowRef)w->handle(), kWindowResizableAttribute,
			       kWindowNoAttributes);
	w->dirtyClippedRegion(TRUE);
	ReshapeCustomWindow((WindowPtr)w->handle());
	qt_dirty_wndw_rgn("Add size grip", w, w->rect());
    }
    return TRUE;
}

// Paint event clipping magic
extern void qt_set_paintevent_clipping(QPaintDevice* dev, const QRegion& region);
extern void qt_clear_paintevent_clipping(QPaintDevice *dev);

enum paint_children_ops {
    PC_None = 0x00,
    PC_Now = 0x01,
    PC_ForceErase = 0x02,
    PC_NoPaint = 0x04,
    PC_NoErase = 0x08,
    PC_Later = 0x10
};
bool qt_paint_children(QWidget *p, QRegion &r, uchar ops = PC_None)
{
    if(qApp->closingDown() || qApp->startingUp() || !p || !p->isVisible() || r.isEmpty())
	return FALSE;
    QPoint point(posInWindow(p));
    r.translate(point.x(), point.y());
    r &= p->clippedRegion(FALSE); //at least sanity check the bounds
    if(r.isEmpty())
	return FALSE;

    if(QObjectList * childObjects=(QObjectList*)p->children()) {
	QObjectListIt it(*childObjects);
	for(it.toLast(); it.current(); --it) {
	    if((*it)->isWidgetType()) {
		QWidget *w = (QWidget *)(*it);
		QRegion clpr = w->clippedRegion(FALSE);
		if(!clpr.isNull() && !w->isTopLevel() &&
		    w->isVisible() && clpr.contains(r.boundingRect())) {
		    QRegion wr = clpr & r;
		    r -= wr;
		    wr.translate(-(point.x() + w->x()), -(point.y() + w->y()));
		    qt_paint_children(w, wr, ops);
		    if(r.isEmpty())
			return TRUE;
		}
	    }
	}
    }

    r.translate(-point.x(), -point.y());
    bool erase = !(ops & PC_NoErase) &&
		 ((ops & PC_ForceErase) || !p->testWFlags(QWidget::WRepaintNoErase));
    if((ops & PC_NoPaint)) {
	if(ops & PC_Later)
	   qDebug("Qt: internal: Cannot use PC_NoPaint with PC_Later!");
	if(erase) {
#ifdef DEBUG_WINDOW_RGN
	    debug_wndw_rgn("**paint_children1", p, r, TRUE, TRUE);
#endif
	    p->erase(r);
	}
    } else {
	if(ops & PC_Now) {
#ifdef DEBUG_WINDOW_RGN
	    debug_wndw_rgn("**paint_children2", p, r, TRUE, TRUE);
#endif
	    p->repaint(r, erase);
	} else {
	    bool painted = FALSE;
	    if(ops & PC_Later); //do nothing
            else if(!p->testWState(QWidget::WState_BlockUpdates)) {
		painted = TRUE;
#ifdef DEBUG_WINDOW_RGN
                debug_wndw_rgn("**paint_children3", p, r, TRUE, TRUE);
#endif
		p->repaint(r, erase);
	    } else if(erase) {
		erase = FALSE;
		p->erase(r);
	    }
	    if(!painted) {
#ifdef DEBUG_WINDOW_RGN
                debug_wndw_rgn("**paint_children4", p, r, TRUE, TRUE);
#endif
		p->update(r); //last try
	    } else if(p->extra && p->extra->has_dirty_area) {
#ifdef DEBUG_WINDOW_RGN
                debug_wndw_rgn("**paint_children5", p, r, TRUE, TRUE);
#endif
		qt_event_request_updates(p, r, TRUE);
	    }
	}
    }
    return FALSE;
}

static QPtrList<QWidget> qt_root_win_widgets;
static WindowPtr qt_root_win = NULL;
void qt_clean_root_win() {
    if(!qt_root_win)
        return;
    for(QPtrListIterator<QWidget> it(qt_root_win_widgets); it.current(); ++it) {
	if((*it)->hd == qt_root_win) {
#if 0
	    warning("%s:%d: %s (%s) had his handle taken away!", __FILE__, __LINE__,
		     (*it)->name(), (*it)->className());
#endif
	    (*it)->hd = NULL; //at least now we'll just crash
	}
    }
    qt_root_win_widgets.clear();
    ReleaseWindow(qt_root_win);
    qt_root_win = NULL;
}
static bool qt_create_root_win() {
    if(qt_root_win)
	return FALSE;
#ifdef Q_WS_MAC9
    //FIXME NEED TO FIGURE OUT HOW TO GET DESKTOP
    //GetCWMgrPort(ret);
#elif defined(Q_WS_MACX)
    Rect r;
    int w = 0, h = 0;
    for(GDHandle g = GetMainDevice(); g; g = GetNextDevice(g)) {
	w = QMAX(w, (*g)->gdRect.right);
	h = QMAX(h, (*g)->gdRect.bottom);
    }
    SetRect(&r, 0, 0, w, h);
    qt_mac_create_window(kOverlayWindowClass, kWindowNoAttributes, &r, &qt_root_win);
#endif //MACX
    if(!qt_root_win)
	return FALSE;
    qAddPostRoutine(qt_clean_root_win);
    return TRUE;
}
bool qt_recreate_root_win() {
    if(!qt_root_win)
	return FALSE;
    WindowPtr old_root_win = qt_root_win;
    qt_root_win = NULL;
    qt_create_root_win();
    for(QPtrListIterator<QWidget> it(qt_root_win_widgets); it.current(); ++it) {
	if((*it)->hd == old_root_win) {
	    (*it)->macWidgetChangedWindow();
	    (*it)->hd = qt_root_win;
	}
    }
    //cleanup old window
    ReleaseWindow(old_root_win);
    return TRUE;
}

bool qt_window_rgn(WId id, short wcode, RgnHandle rgn, bool force = FALSE)
{
    QWidget *widget = QWidget::find((WId)id);
    if(qMacVersion() == Qt::MV_10_DOT_1) {
	switch(wcode) {
	case kWindowOpaqueRgn:
	case kWindowStructureRgn: {
	    if(widget) {
		int x, y;
		{ //lookup the x and y, don't use qwidget because this callback can be called before its updated
		    Point px = { 0, 0 };
		    QMacSavedPortInfo si(widget);
		    LocalToGlobal(&px);
		    x = px.h;
		    y = px.v;
		}

		if(widget->extra && !widget->extra->mask.isNull()) {
		    QRegion titlebar;
		    {
			RgnHandle rgn = qt_mac_get_rgn();
			GetWindowRegion((WindowPtr)widget->handle(), kWindowTitleBarRgn, rgn);
			CopyRgn(rgn, titlebar.handle(TRUE));
			qt_mac_dispose_rgn(rgn);
		    }
		    QRegion rpm = widget->extra->mask;
		    /* This is a gross hack, something is weird with how the Mac is handling this region.
		       clearly the first paintable pixel is becoming 0,0 of this region, so to compensate
		       I just force 0,0 to be on - that way I know the region is offset like I want. Of
		       course it also means another pixel is showing that the user didn't mean to :( FIXME */
		    if(!rpm.contains(QPoint(0, 0)) && rpm.boundingRect().topLeft() != QPoint(0, 0))
			rpm |= QRegion(0, 0, 1, 1);
		    rpm.translate(x, (y + titlebar.boundingRect().height()));
		    titlebar += rpm;
		    CopyRgn(titlebar.handle(TRUE), rgn);
		} else if(force) {
		    QRegion cr(x, y, widget->width(), widget->height());
		    CopyRgn(cr.handle(TRUE), rgn);
		}
	    }
	    return TRUE; }
	case kWindowContentRgn: {
	    if(widget) {
		int x, y;
		{ //lookup the x and y, don't use qwidget because this callback can be called before its updated
		    Point px = { 0, 0 };
		    QMacSavedPortInfo si(widget);
		    LocalToGlobal(&px);
		    x = px.h;
		    y = px.v;
		}

		if(widget->extra && !widget->extra->mask.isNull()) {
		    QRegion rpm = widget->extra->mask;
		    rpm.translate(x, y);
		    CopyRgn(rpm.handle(TRUE), rgn);
		} else if(force) {
		    QRegion cr(x, y, widget->width(), widget->height());
		    CopyRgn(cr.handle(TRUE), rgn);
		}
	    }
	    return TRUE; }
	default: break;
	}
    } else {
	switch(wcode) {
	case kWindowStructureRgn: {
	    bool ret = FALSE;
	    if(widget) {
		if(widget->extra && !widget->extra->mask.isNull()) {
		    QRegion rin;
		    CopyRgn(rgn, rin.handle(TRUE));
		    if(!rin.isEmpty()) {
			QPoint rin_tl = rin.boundingRect().topLeft(); //in offset
			rin.translate(-rin_tl.x(), -rin_tl.y()); //bring into same space as below

			QRegion mask = widget->extra->mask;
//			mask.translate(1, 1);
			if(!widget->testWFlags(Qt::WStyle_Customize) || !widget->testWFlags(Qt::WStyle_NoBorder)) {
			    QRegion title;
			    GetWindowRegion((WindowPtr)widget->handle(), kWindowTitleBarRgn, title.handle(TRUE));
			    QRect br = title.boundingRect();
			    mask.translate(0, br.height()); //put the mask 'under' the titlebar..
			    title.translate(-br.x(), -br.y());
			    mask += title;
			}

			QRegion cr = rin & mask;
			cr.translate(rin_tl.x(), rin_tl.y()); //translate back to incoming space
			CopyRgn(cr.handle(TRUE), rgn);
		    }
		    ret = TRUE;
		} else if(force) {
		    QRegion cr(widget->geometry());
		    CopyRgn(cr.handle(TRUE), rgn);
		    ret = TRUE;
		}
	    }
	    return ret; }
	default: break;
	}
    }
    return FALSE;
}

QMAC_PASCAL OSStatus qt_window_event(EventHandlerCallRef er, EventRef event, void *)
{
    UInt32 ekind = GetEventKind(event), eclass = GetEventClass(event);
    if(eclass == kEventClassWindow) {
	WindowRef wid;
	GetEventParameter(event, kEventParamDirectObject, typeWindowRef, NULL,
			  sizeof(WindowRef), NULL, &wid);
	switch(ekind) {
	case kEventWindowGetRegion: {
	    CallNextEventHandler(er, event);
	    WindowRegionCode wcode;
	    GetEventParameter(event, kEventParamWindowRegionCode, typeWindowRegionCode, NULL,
			      sizeof(wcode), NULL, &wcode);
	    RgnHandle rgn;
	    GetEventParameter(event, kEventParamRgnHandle, typeQDRgnHandle, NULL,
			      sizeof(rgn), NULL, &rgn);
	    if(qt_window_rgn((WId)wid, wcode, rgn, FALSE))
		SetEventParameter(event, kEventParamRgnHandle, typeQDRgnHandle, sizeof(rgn), &rgn);
	    return noErr; }
	case kEventWindowDrawContent: {
	    if(QWidget *widget = QWidget::find((WId)wid)) {
		widget->propagateUpdates(FALSE);
		return noErr;
	    }
	    break; }
	}
    } else if(eclass == kEventClassMouse) {
        return SendEventToApplication(event);
    }
    return eventNotHandledErr;
}
static EventTypeSpec window_events[] = {
    { kEventClassWindow, kEventWindowGetRegion },
//    { kEventClassMouse, kEventMouseDown },
//    { kEventClassMouse, kEventMouseUp }
};
static EventHandlerUPP mac_win_eventUPP = NULL;
static void cleanup_win_eventUPP()
{
    DisposeEventHandlerUPP(mac_win_eventUPP);
    mac_win_eventUPP = NULL;
}
static const EventHandlerUPP make_win_eventUPP()
{
    if(mac_win_eventUPP)
	return mac_win_eventUPP;
    qAddPostRoutine(cleanup_win_eventUPP);
    return mac_win_eventUPP = NewEventHandlerUPP(qt_window_event);
}

//#define QMAC_USE_WDEF
#ifdef QMAC_USE_WDEF
static QMAC_PASCAL long qt_wdef(short, WindowRef window, short message, long param)
{
    long result = 0;
    switch (message) {
    case kWindowMsgHitTest:
	result = wInContent;
	break;
    case kWindowMsgStateChanged:
    case kWindowMsgCleanUp:
    case kWindowMsgInitialize:
    case kWindowMsgDrawInCurrentPort:
    case kWindowMsgDraw:
	result = 0;
	break;
    case kWindowMsgGetFeatures: {
	SInt32 *s = (SInt32*)param;
	*s = kWindowCanGetWindowRegion;
	result = 1;
	break; }
    case kWindowMsgGetRegion: {
	GetWindowRegionRec *s = (GetWindowRegionRec *)param;
	if(qt_window_rgn((WId)window, s->regionCode, s->winRgn, TRUE))
	    result = 0;
	else
	    result = errWindowRegionCodeInvalid;
	break; }
    default:
	qDebug("Qt: internal: Shouldn't happen %s:%d %d", __FILE__, __LINE__, message);
	break;
    }
    return result;
}
#endif

QMAC_PASCAL OSStatus qt_erase(GDHandle, GrafPtr, WindowRef window, RgnHandle rgn,
			 RgnHandle outRgn, void *w)
{
    QWidget *widget = (QWidget *)w;
    if(!widget)
	widget = QWidget::find((WId)window);
    if(widget) {
	QRegion reg;
	CopyRgn(rgn, reg.handle(TRUE));
	{ //lookup the x and y, don't use qwidget because this callback can be called before its updated
	    Point px = { 0, 0 };
	    QMacSavedPortInfo si(widget);
	    LocalToGlobal(&px);
	    reg.translate(-px.h, -px.v);
	}
	//Clear a nobackground widget to make it transparent
	if(widget->backgroundMode() == Qt::NoBackground) {
	    CGContextRef ctx;
	    CGRect r2 = CGRectMake(0, 0, widget->width(), widget->height());
	    CreateCGContextForPort(GetWindowPort((WindowPtr)widget->handle()), &ctx);
	    CGContextClearRect(ctx, r2);
	    CGContextFlush(ctx);
	    CGContextRelease(ctx);
	}
	qt_paint_children(widget, reg, PC_NoPaint | PC_ForceErase);
	QMacSavedPortInfo::flush(widget);
    } else {
	CopyRgn(rgn, outRgn);  //We don't know the widget, so let the Mac do the erasing..
    }
    return 0;
}

bool qt_mac_is_macdrawer(QWidget *w)
{
#if defined(Q_WS_MACX) && 0
    if(w && w->isTopLevel() && w->parentWidget() && w->testWFlags(Qt::WMacDrawer))
	return TRUE;
#else
    Q_UNUSED(w);
#endif
    return FALSE;
}

bool qt_mac_set_drawer_preferred_edge(QWidget *w, Qt::Dock where) //users of Qt/Mac can use this..
{
#if QT_MACOSX_VERSION >= 0x1020
    if(!qt_mac_is_macdrawer(w))
	return FALSE;
    OptionBits bits;
    if(where == Qt::DockTop)
	bits = kWindowEdgeTop;
    else if(where == Qt::DockLeft)
	bits = kWindowEdgeLeft;
    else if(where == Qt::DockRight)
	bits = kWindowEdgeRight;
    else if(where == Qt::DockBottom)
	bits = kWindowEdgeBottom;
    else
	return FALSE;
    SetDrawerPreferredEdge((WindowRef)w->handle(), bits);
    return TRUE;
#else
    return FALSE;
#endif
}

bool qt_mac_is_macsheet(QWidget *w, bool ignore_exclusion=FALSE)
{
#if defined(Q_WS_MACX) && 0
    if(w && w->isTopLevel() && w->testWFlags(Qt::WStyle_DialogBorder) &&
       (ignore_exclusion || !w->testWFlags(Qt::WMacNotSheet)) &&
       w->parentWidget() && !w->parentWidget()->topLevelWidget()->isDesktop() &&
       w->parentWidget()->topLevelWidget()->isVisible()
       && (::qt_cast<QMacStyle *>(&w->style()) || ::qt_cast<QAquaStyle *>(&w->style())))
	return TRUE;
#else
    Q_UNUSED(w);
    Q_UNUSED(ignore_exclusion);
#endif
    return FALSE;
}

/*****************************************************************************
  QWidget member functions
 *****************************************************************************/
void QWidget::create(WId window, bool initializeWindow, bool destroyOldWindow)
{
    window_event = NULL;
    own_id = 0;
    HANDLE destroyw = 0;
    setWState(WState_Created);                        // set created flag

    if(!parentWidget() || parentWidget()->isDesktop())
	setWFlags(WType_TopLevel);            // top-level widget

    QRect dskr;
    if(isDesktop()) {
	int w = 0, h = 0;
	for(GDHandle g = GetMainDevice(); g; g = GetNextDevice(g)) {
	    w = QMAX(w, (*g)->gdRect.right);
	    h = QMAX(h, (*g)->gdRect.bottom);
	}
	dskr = QRect(0, 0, w, h);
    } else {
	if(QDesktopWidget *dsk = QApplication::desktop()) {
	    int d = dsk->primaryScreen();
	    if(parentWidget() && !parentWidget()->isDesktop())
		d = dsk->screenNumber(parentWidget());
	    dskr = dsk->screenGeometry(d);
	}
    }
    int sw = dskr.width(), sh = dskr.height();                // screen size
    bool topLevel = testWFlags(WType_TopLevel);
    bool popup = testWFlags(WType_Popup);
    bool dialog = testWFlags(WType_Dialog);
    bool desktop = testWFlags(WType_Desktop);
    WId    id;

    if(!window)                              // always initialize
	initializeWindow=TRUE;

    if(dialog || popup || desktop) {          // these are top-level, too
	topLevel = TRUE;
	setWFlags(WType_TopLevel);
	if(popup)
	    setWFlags(WStyle_Tool|WStyle_StaysOnTop); // a popup is a tool window
    }
    if(topLevel && parentWidget()) {
	// if our parent has WStyle_StaysOnTop, so must we
	QWidget *ptl = parentWidget()->topLevelWidget();
	if(ptl && ptl->testWFlags(WStyle_StaysOnTop))
	    setWFlags(WStyle_StaysOnTop);
    }
    if(!testWFlags(WStyle_Customize) && !(desktop || popup) && !testWFlags(WShowModal))
	setWFlags(WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_MinMax | WStyle_SysMenu);

    if(desktop) {                            // desktop widget
	dialog = popup = FALSE;                  // force these flags off
	crect.setRect(0, 0, sw, sh);
    } else if(topLevel) {                    // calc pos/size from screen
	crect.setRect(sw/4, 3*sh/10, sw/2, 4*sh/10);
    } else {                                    // child widget
	crect.setRect(0, 0, 100, 30);
    }

    if(window) {				// override the old window
	if(destroyOldWindow && own_id)
	    destroyw = hd;
	own_id = 0; //it has become mine!
	id = window;
	hd = (void *)id;
	macWidgetChangedWindow();
	setWinId(id);
    } else if(desktop) {			// desktop widget
	if(!qt_root_win)
	    qt_create_root_win();
	qt_root_win_widgets.append(this);
	hd = (void *)qt_root_win;
	macWidgetChangedWindow();
	id = (WId)hd;
	own_id = 0;
	setWinId(id);
    } else if(isTopLevel()) {
	own_id = 1; //I created it, I own it

	Rect r;
	SetRect(&r, crect.left(), crect.top(), crect.left(), crect.top());
	WindowClass wclass = kSheetWindowClass;
	if(popup || testWFlags(WStyle_Splash) == WStyle_Splash)
	    wclass = kModalWindowClass;
	else if(testWFlags(WShowModal))
	    wclass = kMovableModalWindowClass;
#if QT_MACOSX_VERSION >= 0x1020
	else if(qt_mac_is_macdrawer(this))
	    wclass = kDrawerWindowClass;
#endif
	else if(testWFlags(WStyle_Tool) && qstrcmp(name(), "toolTipTip") == 0) // Tool tips
	    wclass = kHelpWindowClass;
	else if(testWFlags(WStyle_Tool)
                || (dialog && parentWidget() && !parentWidget()->topLevelWidget()->isDesktop()))
	    wclass = kFloatingWindowClass;
	else if(dialog)
	    wclass = kToolbarWindowClass;
	else
	    wclass = kDocumentWindowClass;

	WindowGroupRef grp = NULL;
	WindowAttributes wattr = kWindowNoAttributes;
	if(testWFlags(WStyle_Customize)) {
	    if(qt_mac_is_macsheet(this)) {
		grp = GetWindowGroupOfClass(kMovableModalWindowClass);
		wclass = kSheetWindowClass;
	    } else {
		grp = GetWindowGroupOfClass(wclass);
                // Shift things around a bit to get the correct window class based on the presence
                // (or lack) of the border.
		if(testWFlags(WStyle_NoBorder)) {
		    if(wclass == kDocumentWindowClass)
			wclass = kPlainWindowClass;
		    else if(wclass == kFloatingWindowClass)
			wclass = kToolbarWindowClass;
		} else {
		    if(wclass != kModalWindowClass)
			wattr |= kWindowResizableAttribute;
                    if(wclass == kToolbarWindowClass) {
                        if(!parentWidget() || parentWidget()->isDesktop())
                            wclass = kDocumentWindowClass;
                        else
                            wclass = kFloatingWindowClass;
                    }
		}
		// Only add extra decorations (well, buttons) for widgets that can have them
		// and have an actual border we can put them on.
                if(wclass != kModalWindowClass && wclass != kMovableModalWindowClass
                    && wclass != kSheetWindowClass && wclass != kPlainWindowClass
                    && !testWFlags(WStyle_NoBorder)) {
		    if(testWFlags(WStyle_Maximize))
			wattr |= kWindowFullZoomAttribute;
		    if(testWFlags(WStyle_Minimize))
			wattr |= kWindowCollapseBoxAttribute;
		    if(testWFlags(WStyle_Title) || testWFlags(WStyle_SysMenu))
		       wattr |= kWindowCloseBoxAttribute;
		}
	    }
	}
	wattr |= kWindowLiveResizeAttribute;
	if(testWFlags(WStyle_Tool) && testWFlags(WStyle_Splash) != WStyle_Splash)
	    wattr |= kWindowHideOnSuspendAttribute;

#ifdef DEBUG_WINDOW_CREATE
#define ADD_DEBUG_WINDOW_NAME(x) { x, #x }
	struct {
	    UInt32 tag;
	    const char *name;
	} known_attribs[] = {
	    ADD_DEBUG_WINDOW_NAME(kWindowStandardHandlerAttribute),
	    ADD_DEBUG_WINDOW_NAME(kWindowCollapseBoxAttribute),
	    ADD_DEBUG_WINDOW_NAME(kWindowHorizontalZoomAttribute),
	    ADD_DEBUG_WINDOW_NAME(kWindowVerticalZoomAttribute),
	    ADD_DEBUG_WINDOW_NAME(kWindowResizableAttribute),
	    ADD_DEBUG_WINDOW_NAME(kWindowNoActivatesAttribute),
	    ADD_DEBUG_WINDOW_NAME(kWindowLiveResizeAttribute),
	    ADD_DEBUG_WINDOW_NAME(kWindowCloseBoxAttribute),
	    ADD_DEBUG_WINDOW_NAME(kWindowHideOnSuspendAttribute),
	    { 0, NULL }
	}, known_classes[] = {
	    ADD_DEBUG_WINDOW_NAME(kPlainWindowClass),
	    ADD_DEBUG_WINDOW_NAME(kHelpWindowClass),
	    ADD_DEBUG_WINDOW_NAME(kDrawerWindowClass),
	    ADD_DEBUG_WINDOW_NAME(kUtilityWindowClass),
	    ADD_DEBUG_WINDOW_NAME(kToolbarWindowClass),
	    ADD_DEBUG_WINDOW_NAME(kSheetWindowClass),
	    ADD_DEBUG_WINDOW_NAME(kFloatingWindowClass),
	    ADD_DEBUG_WINDOW_NAME(kDocumentWindowClass),
	    ADD_DEBUG_WINDOW_NAME(kToolbarWindowClass),
	    ADD_DEBUG_WINDOW_NAME(kMovableModalWindowClass),
	    ADD_DEBUG_WINDOW_NAME(kModalWindowClass),
	    { 0, NULL }
	};
#undef ADD_DEBUG_WINDOW_NAME
	qDebug("Qt: internal: ************* Creating new window (%s::%s)", className(), name());
	bool found_class = FALSE;
	for(int i = 0; known_classes[i].name; i++) {
	    if(wclass == known_classes[i].tag) {
		found_class = TRUE;
		qDebug("Qt: internal: ** Class: %s", known_classes[i].name);
		break;
	    }
	}
	if(!found_class)
	    qDebug("Qt: internal: !! Class: Unknown! (%d)", (int)wclass);
	if(wattr) {
	    WindowAttributes tmp_wattr = wattr;
	    qDebug("Qt: internal: ** Attributes:");
	    for(int i = 0; tmp_wattr && known_attribs[i].name; i++) {
		if((tmp_wattr & known_attribs[i].tag) == known_attribs[i].tag) {
		    tmp_wattr ^= known_attribs[i].tag;
		    qDebug("Qt: internal: * %s %s", known_attribs[i].name,
			   (GetAvailableWindowAttributes(wclass) & known_attribs[i].tag) ? "" : "(*)");
		}
	    }
	    if(tmp_wattr)
		qDebug("Qt: internal: !! Attributes: Unknown (%d)", (int)tmp_wattr);
	}
#endif

	/* Just to be extra careful we will change to the kUtilityWindowClass if the
	   requested attributes cannot be used */
	if((GetAvailableWindowAttributes(wclass) & wattr) != wattr) {
	    WindowClass tmp_class = wclass;
	    if(wclass == kToolbarWindowClass || wclass == kUtilityWindowClass)
		wclass = kFloatingWindowClass;
	    if(tmp_class != wclass) {
		if(!grp)
		    grp = GetWindowGroupOfClass(wclass);
		wclass = tmp_class;
	    }
	}

#ifdef QMAC_USE_WDEF
	if((wclass == kPlainWindowClass && wattr == kWindowNoAttributes) || testWFlags(WStyle_Tool)) {
	    WindowDefSpec wds;
	    wds.defType = kWindowDefProcPtr;
	    wds.u.defProc = NewWindowDefUPP(qt_wdef);
	    CreateCustomWindow(&wds, wclass, wattr, &r, (WindowRef *)&id);
	} else
#endif
	{
	    if(OSStatus ret = qt_mac_create_window(wclass, wattr, &r, (WindowRef *)&id))
		qDebug("Qt: internal: %s:%d If you reach this error please contact Trolltech and include the\n"
		       "      WidgetFlags used in creating the widget (%ld)", __FILE__, __LINE__, ret);
	    if(!desktop) { 	//setup an event callback handler on the window
		InstallWindowEventHandler((WindowRef)id, make_win_eventUPP(),
					  GetEventTypeCount(window_events),
					  window_events, (void *)qApp, &window_event);
	    }
	}
	if(wclass == kFloatingWindowClass) //these dialogs don't hide
	    ChangeWindowAttributes((WindowRef)id, kWindowNoAttributes, kWindowNoActivatesAttribute);
	if(testWFlags(WStyle_StaysOnTop))
	    ChangeWindowAttributes((WindowRef)id, kWindowNoAttributes, kWindowHideOnSuspendAttribute);

#if QT_MACOSX_VERSION >= 0x1020
	if(qt_mac_is_macdrawer(this))
	    SetDrawerParent((WindowRef)id, (WindowRef)parentWidget()->handle());
#endif
        if(dialog && !testWFlags(WShowModal) && parentWidget() && parentWidget()->testWFlags(WShowModal))
	    grp = GetWindowGroupOfClass(kMovableModalWindowClass);
	else if(dialog && !parentWidget() && !testWFlags(WShowModal))
	    grp = GetWindowGroupOfClass(kDocumentWindowClass);
#ifdef Q_WS_MACX
	if(testWFlags(WStyle_StaysOnTop)) {
	    createTLExtra();
	    if(extra->topextra->group)
		qt_mac_release_window_group(extra->topextra->group);
	    extra->topextra->group = qt_mac_get_stays_on_top_group();
	    SetWindowGroup((WindowPtr)id, extra->topextra->group);
	} else if(grp) {
	    SetWindowGroup((WindowPtr)id, grp);
	}
#ifdef DEBUG_WINDOW_CREATE
	if(WindowGroupRef grpf = GetWindowGroup((WindowPtr)id)) {
	    CFStringRef cfname;
	    CopyWindowGroupName(grpf, &cfname);
	    SInt32 lvl;
	    GetWindowGroupLevel(grpf, &lvl);
	    const char *from = "Default";
	    if(extra && extra->topextra && grpf == extra->topextra->group)
		from = "Created";
	    else if(grpf == grp)
		from = "Copied";
	    qDebug("Qt: internal: With window group '%s' [%p] @ %d: %s",
		   cfstring2qstring(cfname).latin1(), grpf, (int)lvl, from);
	} else {
	    qDebug("Qt: internal: No window group!!!");
	}
#endif
#endif
#if 0
	//We cannot use a window content paint proc because it causes problems on 10.2 (it
	//is buggy). We have an outstanding issue with Apple right now.
	InstallWindowContentPaintProc((WindowPtr)id, NewWindowPaintUPP(qt_erase), 0, this);
#endif
	if(testWFlags(WType_Popup) || testWFlags(WStyle_Tool))
	    SetWindowModality((WindowPtr)id, kWindowModalityNone, NULL);
	fstrut_dirty = TRUE; // when we create a toplevel widget, the frame strut should be dirty
	hd = (void *)id;
	if(!mac_window_count++)
	    QMacSavedPortInfo::setPaintDevice(this);
	macWidgetChangedWindow();
	setWinId(id);
	if(extra && !extra->mask.isNull())
	   ReshapeCustomWindow((WindowPtr)hd);
	if(qt_mac_is_macsheet(this))
	    setWindowOpacity(0.70);
#if QT_MACOSX_VERSION >= 0x1020
	else if(qt_mac_is_macdrawer(this))
	    SetDrawerOffsets((WindowPtr)hd, 0.0, 25.0);
#endif
    } else {
	while(QWidget::find(++serial_id));
	setWinId(serial_id);
	id = serial_id;
	hd = topLevelWidget()->hd;
	macWidgetChangedWindow();
	fstrut_dirty = FALSE; // non-toplevel widgets don't have a frame, so no need to update the strut
	setWinId(id);
    }

    setWState(WState_MouseTracking);
    setMouseTracking(FALSE);                  // also sets event mask
    if(desktop) { //immediately "show" a "desktop"
	setWState(WState_Visible);
    } else {
	clearWState(WState_Visible);
	dirtyClippedRegion(TRUE);
    }
    macDropEnabled = false;

    if(destroyw) {
	mac_window_count--;
	ReleaseWindow((WindowPtr)destroyw);
    }
    qt_mac_unicode_init(this);
#ifndef QMAC_NO_QUARTZ
    if(ctx)
	CGContextRelease(ctx);
    ctx = NULL;
#endif
}

void QWidget::destroy(bool destroyWindow, bool destroySubWindows)
{
    deactivateWidgetCleanup();
    qt_mac_event_release(this);
    qt_mac_unicode_cleanup(this);
    if(isDesktop() && hd == qt_root_win && destroyWindow && own_id)
	qt_root_win_widgets.removeRef(this);
    if(testWState(WState_Created)) {
	dirtyClippedRegion(TRUE);
	if(isVisible())
	    qt_dirty_wndw_rgn("destroy",this, mac_rect(posInWindow(this), geometry().size()));
        clearWState(WState_Created);
        if(children()) {
            QObjectListIt it(*children());
            register QObject *obj;
            while((obj=it.current())) {      // destroy all widget children
                ++it;
                if(obj->isWidgetType())
                    ((QWidget*)obj)->destroy(destroySubWindows, destroySubWindows);
            }
        }
	if(mac_mouse_grabber == this)
	    releaseMouse();
	if(mac_keyboard_grabber == this)
	    releaseKeyboard();
	if(acceptDrops())
	    setAcceptDrops(FALSE);

        if(testWFlags(WShowModal))          // just be sure we leave modal
            qt_leave_modal(this);
        else if(testWFlags(WType_Popup))
            qApp->closePopup(this);
#ifndef QMAC_NO_QUARTZ
	if(ctx)
	    CGContextRelease(ctx);
	ctx = NULL;
#endif
	if(destroyWindow && isTopLevel() && hd && own_id) {
	    mac_window_count--;
	    if(window_event) {
		RemoveEventHandler(window_event);
		window_event = NULL;
	    }
	    ReleaseWindow((WindowPtr)hd);
	}
    }
    hd=0;
    setWinId(0);
}

void QWidget::reparentSys(QWidget *parent, WFlags f, const QPoint &p,
			bool showIt)
{
    if(isVisible() && !isTopLevel())
	qt_dirty_wndw_rgn("reparent1", parentWidget() ? parentWidget() : this,
			  mac_rect(posInWindow(this), geometry().size()));
    dirtyClippedRegion(TRUE);

    QCursor oldcurs;
    bool setcurs=testWState(WState_OwnCursor);
    if(setcurs) {
	oldcurs = cursor();
	unsetCursor();
    }

    EventHandlerRef old_window_event = NULL;
    WindowPtr old_hd = NULL;
    if(!isDesktop()) {
	old_hd = (WindowPtr)hd;
	old_window_event = window_event;
    }
    QWidget* oldtlw = topLevelWidget();

    setWinId(0);
    if(parentObj) // remove from parent
	parentObj->removeChild(this);
    if(parent) // insert into new parent
	parent->insertChild(this);
    bool     dropable = acceptDrops();
    bool     enable = isEnabled();
    bool     owned = own_id;
    FocusPolicy fp = focusPolicy();
    QSize    s	    = size();
    QString capt= caption();
    widget_flags = f;
    clearWState(WState_Created | WState_Visible | WState_ForceHide);
    create();
    if(isTopLevel() || (!parent || parent->isVisible()))
	setWState(WState_ForceHide);	// new widgets do not show up in already visible parents
    if(isTopLevel() && extraData())
        qt_macdnd_reregister(this, extraData());

    //get new hd, now move
    no_move_blt = TRUE;
    setGeometry(p.x(), p.y(), s.width(), s.height());
    no_move_blt = FALSE;

    //reset flags and show (if neccesary)
    setEnabled(enable);
    setFocusPolicy(fp);
    setAcceptDrops(dropable);
    if(!capt.isNull()) {
	extra->topextra->caption = QString::null;
	setCaption(capt);
    }
    if(showIt)
	show();
    if(setcurs)
	setCursor(oldcurs);

    //reparent children
    if(QObjectList	*chldn = queryList()) {
	QObjectListIt it(*chldn);
	for(QObject *obj; (obj=it.current()); ++it) {
            QAccel *accel = ::qt_cast<QAccel *>(obj);
	    if(accel)
		accel->repairEventFilter();
	    if(obj->isWidgetType()) {
		QWidget *w = (QWidget *)obj;
		if(((WindowPtr)w->hd) == old_hd) {
		    w->hd = hd; //all my children hd's are now mine!
		    w->macWidgetChangedWindow();
		}
	    }
	}
	delete chldn;
    }
    reparentFocusWidgets(oldtlw);

    //repaint the new area, on the window parent
    if(isVisible()) //finally paint my new area
	qt_dirty_wndw_rgn("reparent2",this, mac_rect(posInWindow(this), geometry().size()));

    //send the reparent event
    if(old_hd && owned) { //don't need old window anymore
	mac_window_count--;
	if(old_window_event) {
	    RemoveEventHandler(old_window_event);
	    old_window_event = NULL;
	}
	ReleaseWindow(old_hd);
    }
}

QPoint QWidget::mapToGlobal(const QPoint &pos) const
{
    Point mac_p;
    QPoint mp(posInWindow(((QWidget *)this)));
    mac_p.h = mp.x() + pos.x();
    mac_p.v = mp.y() + pos.y();
    if(handle()) {
	QMacSavedPortInfo savedInfo(((QWidget *)this));
	LocalToGlobal(&mac_p);
    }
    return QPoint(mac_p.h, mac_p.v);
}

QPoint QWidget::mapFromGlobal(const QPoint &pos) const
{
    Point mac_p;
    mac_p.h = pos.x();
    mac_p.v = pos.y();
    if(handle()) {
	QMacSavedPortInfo savedInfo(((QWidget *)this));
	GlobalToLocal(&mac_p);
    }
    for(const QWidget *p = this; p && !p->isTopLevel(); p = p->parentWidget(TRUE)) {
	mac_p.h -= p->geometry().x();
	mac_p.v -= p->geometry().y();
    }
    return QPoint(mac_p.h, mac_p.v);
}

void QWidget::setMicroFocusHint(int x, int y, int width, int height, bool text, QFont *)
{
    if(!width)
	width = 1;
    if(!height)
	height = 1;
    if(text && QRect(x, y, width, height) != microFocusHint()) {
	createExtra();
	extraData()->micro_focus_hint.setRect(x, y, width, height);
    }
}

void QWidget::setFontSys(QFont *)
{
}

void QWidget::setBackgroundColorDirect(const QColor &color)
{
    bg_col = color;
    if(extra && extra->bg_pix) {		// kill the background pixmap
	delete extra->bg_pix;
	extra->bg_pix = 0;
    }
    if(isTopLevel()) {
	QMacSavedPortInfo savedInfo(this);
	RGBColor f;
	f.red = bg_col.red() * 256;
	f.green = bg_col.green() * 256;;
	f.blue = bg_col.blue() * 256;
	RGBBackColor(&f);
    }
    update();
}

static int allow_null_pixmaps = 0;
void QWidget::setBackgroundPixmapDirect(const QPixmap &pixmap)
{
    QPixmap old;
    if(extra && extra->bg_pix)
	old = *extra->bg_pix;
    if(!allow_null_pixmaps && pixmap.isNull()) {
	if(extra && extra->bg_pix) {
	    delete extra->bg_pix;
	    extra->bg_pix = 0;
	}
    } else {
	QPixmap pm = pixmap;
//	pm.setMask(QBitmap());
	if(!pixmap.isNull()) {
	    if(pm.depth() == 1 && QPixmap::defaultDepth() > 1) {
		pm = QPixmap(pixmap.size());
		bitBlt(&pm, 0, 0, &pixmap, 0, 0, pm.width(), pm.height());
	    }
	}
	if(extra && extra->bg_pix)
	    delete extra->bg_pix;
	else
	    createExtra();
	extra->bg_pix = new QPixmap(pm);
    }
    update();
}

void QWidget::setBackgroundEmpty()
{
    allow_null_pixmaps++;
    setErasePixmap(QPixmap());
    allow_null_pixmaps--;
    if(isTopLevel())
	ReshapeCustomWindow((WindowPtr)hd);
}

void QWidget::setCursor(const QCursor &cursor)
{
    createExtra();
    delete extra->curs;
    extra->curs = new QCursor(cursor);
    setWState(WState_OwnCursor);

    if(qApp && qApp->activeWindow() &&
       QApplication::widgetAt(QCursor::pos(), TRUE) == this) {
	Point mouse_pos;
	QPoint qmp(QCursor::pos());
	mouse_pos.h = qmp.x();
	mouse_pos.v = qmp.y();

	const QCursor *n = &cursor;
	if(QApplication::overrideCursor())
	    n = QApplication::overrideCursor();
	qt_mac_set_cursor(n, &mouse_pos);
    }
}

void QWidget::unsetCursor()
{
    if(!isTopLevel()) {
	if(extra) {
	    delete extra->curs;
	    extra->curs = 0;
	}
	clearWState(WState_OwnCursor);
    }

    if(qApp && qApp->activeWindow() &&
       QApplication::widgetAt(QCursor::pos(), TRUE) == this) {
	Point mouse_pos;
	QPoint qmp(QCursor::pos());
	mouse_pos.h = qmp.x();
	mouse_pos.v = qmp.y();

	const QCursor *n = NULL;
	if(QApplication::overrideCursor()) {
	    n = QApplication::overrideCursor();
	} else {
	    for(QWidget *p = this; p; p = p->parentWidget()) {
		if(p->extra && p->extra->curs) {
		    n = p->extra->curs;
		    break;
		}
	    }
	}
	if(!n)
	    n = &arrowCursor; //I give up..
	qt_mac_set_cursor(n, &mouse_pos);
    }
}

void QWidget::setCaption(const QString &cap)
{
    if(extra && extra->topextra && extra->topextra->caption == cap)
	return; // for less flicker
    createTLExtra();
    extra->topextra->caption = cap;
    if(isTopLevel()) {
	CFStringRef str = CFStringCreateWithCharacters(NULL, (UniChar *)cap.unicode(), cap.length());
	SetWindowTitleWithCFString((WindowPtr)hd, str);
	CFRelease(str);
    }
    QEvent e(QEvent::CaptionChange);
    QApplication::sendEvent(this, &e);
}

void QWidget::setIcon(const QPixmap &pixmap)
{
    if(extra && extra->topextra) {
	delete extra->topextra->icon;
	extra->topextra->icon = 0;
    } else {
	createTLExtra();
    }
    if(!pixmap.isNull())
	extra->topextra->icon = new QPixmap(pixmap);
#ifdef Q_WS_MACX
    if(isTopLevel()) {
	if(qApp && qApp->mainWidget() == this) {
	    if(pixmap.isNull()) {
		RestoreApplicationDockTileImage();
	    } else {
		QImage i = pixmap.convertToImage().convertDepth(32).smoothScale(128, 128);
		CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
		CGDataProviderRef dp = CGDataProviderCreateWithData(NULL, i.bits(), i.numBytes(), NULL);
#if QT_MACOSX_VERSION >= 0x1040
                uint cgflags = i.hasAlphaBuffer() ? kCGImageAlphaFirst : kCGImageAlphaNoneSkipFirst;
#ifdef kCGBitmapByteOrder32Host //only needed because CGImage.h added symbols in the minor version
                if(qMacVersion() >= Qt::MV_10_DOT_4)
                    cgflags |= kCGBitmapByteOrder32Host;
#endif
#else
                CGImageAlphaInfo cgflags = kCGImageAlphaPremultipliedFirst;
#endif
		CGImageRef ir = CGImageCreate(i.width(), i.height(), 8, 32, i.bytesPerLine(), cs,
					      cgflags, dp, 0, 0, kCGRenderingIntentDefault);
		//cleanup
		SetApplicationDockTileImage(ir);
		CGImageRelease(ir);
		CGColorSpaceRelease(cs);
		CGDataProviderRelease(dp);
	    }
	}
        if(pixmap.isNull()) {
            RemoveWindowProxy((WindowRef)handle());
        } else {
            WindowClass wclass;
            GetWindowClass((WindowPtr)handle(), &wclass);
            if(wclass == kDocumentWindowClass)
                SetWindowProxyIcon((WindowRef)handle(), qt_mac_create_iconref(pixmap));
        }
    }
#endif
}

void QWidget::setIconText(const QString &iconText)
{
    createTLExtra();
    extra->topextra->iconText = iconText;
}

void QWidget::grabMouse()
{
    if(isVisible() && !qt_nograb()) {
	if(mac_mouse_grabber)
	    mac_mouse_grabber->releaseMouse();
	mac_mouse_grabber=this;
    }
}

void QWidget::grabMouse(const QCursor &)
{
    if(isVisible() && !qt_nograb()) {
	if(mac_mouse_grabber)
	    mac_mouse_grabber->releaseMouse();
	mac_mouse_grabber=this;
    }
}

void QWidget::releaseMouse()
{
    if(!qt_nograb() && mac_mouse_grabber == this)
	mac_mouse_grabber = NULL;
}

void QWidget::grabKeyboard()
{
    if(!qt_nograb()) {
	if(mac_keyboard_grabber)
	    mac_keyboard_grabber->releaseKeyboard();
	mac_keyboard_grabber = this;
    }
}

void QWidget::releaseKeyboard()
{
    if(!qt_nograb() && mac_keyboard_grabber == this)
	mac_keyboard_grabber = NULL;
}

QWidget *QWidget::mouseGrabber()
{
    return mac_mouse_grabber;
}

QWidget *QWidget::keyboardGrabber()
{
    return mac_keyboard_grabber;
}

void QWidget::setActiveWindow()
{
    QWidget *tlw = topLevelWidget();
    if(!tlw->isVisible() || !tlw->isTopLevel() || tlw->isDesktop())
	return;
    qt_event_remove_activate();
    if(IsWindowActive((WindowPtr)tlw->handle()) ||
       tlw->isPopup() || tlw->testWFlags(WStyle_Tool)) {
	ActivateWindow((WindowPtr)tlw->handle(), true);
        if(qstrcmp(tlw->name(), "toolTipTip"))
            qApp->setActiveWindow(tlw);
    } else if(!isMinimized()){
        SelectWindow((WindowPtr)tlw->handle());
    }
    SetUserFocusWindow((WindowPtr)tlw->handle());
}

void QWidget::update()
{
    update(0, 0, width(), height());
}

void QWidget::update(int x, int y, int w, int h)
{
    if(!testWState(WState_BlockUpdates) && isVisible() && !clippedRegion().isNull()) {
	if(w < 0)
	    w = crect.width()  - x;
	if(h < 0)
	    h = crect.height() - y;
	if(w && h) {
	    QRegion r(x, y, w, h);
	    qt_event_request_updates(this, r, FALSE);
#ifdef DEBUG_WINDOW_RGN
	    debug_wndw_rgn("update1", this, r, FALSE, TRUE);
#endif
	}
    }
}

void QWidget::update(const QRegion &rgn)
{
    if(!testWState(WState_BlockUpdates) && isVisible() && !clippedRegion().isNull()) {
	qt_event_request_updates(this, rgn, FALSE);
#ifdef DEBUG_WINDOW_RGN
	debug_wndw_rgn("update2", this, rgn, FALSE, TRUE);
#endif
    }
}

void QWidget::repaint(int x, int y, int w, int h, bool erase)
{
    if(w < 0)
	w = crect.width()  - x;
    if(h < 0)
	h = crect.height() - y;
    QRect r(x,y,w,h);
    if(r.isEmpty())
	return; // nothing to do
    repaint(QRegion(r), erase); //general function..
}

void QWidget::repaint(const QRegion &reg , bool erase)
{
    if(!testWState(WState_BlockUpdates) && isVisible()) {
	setWState(WState_InPaintEvent);
	qt_set_paintevent_clipping(this, reg);
	if(erase)
	    this->erase(reg);

	QPaintEvent e(reg);
	QApplication::sendEvent(this, &e);
	qt_clear_paintevent_clipping(this);
	clearWState(WState_InPaintEvent);
    }
}

void QWidget::showWindow()
{
    if(isDesktop()) //desktop is always visible
	return;

    if(isTopLevel()) {
	createTLExtra();
	QDesktopWidget *dsk = QApplication::desktop();
	if(!extra->topextra->is_moved && dsk) {
	    int movex = x(), movey = y();
	    QRect r = frameGeometry();
	    QRect avail = dsk->availableGeometry(dsk->screenNumber(this));
	    if(r.bottom() > avail.bottom())
		movey = avail.bottom() - r.height();
	    if(r.right() > avail.right())
		movex = avail.right() - r.width();
	    // +2 to prevent going under the menu bar
	    move(QMAX(avail.left(), movex), QMAX(avail.top() + 2, movey));
	}
    }
    fstrut_dirty = TRUE;
    dirtyClippedRegion(TRUE);
    if(isTopLevel()) {
	SizeWindow((WindowPtr)hd, width(), height(), true);
	if(qt_mac_is_macsheet(this))
	    qt_event_request_showsheet(this);
#if QT_MACOSX_VERSION >= 0x1020
	else if(qt_mac_is_macdrawer(this))
	    OpenDrawer((WindowPtr)hd, kWindowEdgeDefault, true);
#endif
	else {
	    ShowHide((WindowPtr)hd, true);	//now actually show it
            // it seems that collapse window doesn't work unless the window is actually shown,
            // so catch it again.
            if(windowState() & WindowMinimized)
                CollapseWindow((WindowPtr)hd, true);
        }
#ifndef QMAC_NO_FAKECURSOR
	if(qstrcmp(name(), "fake_cursor") != 0)
#endif
	    qt_event_request_activate(this);
    } else if(!parentWidget(TRUE) || parentWidget(TRUE)->isVisible()) {
	qt_dirty_wndw_rgn("show",this, mac_rect(posInWindow(this), geometry().size()));
    }
}

void QWidget::hideWindow()
{
    if(isDesktop()) //you can't hide the desktop!
	return;

    dirtyClippedRegion(TRUE);
    if(isTopLevel()) {
#if QT_MACOSX_VERSION >= 0x1020
	if(qt_mac_is_macdrawer(this))
	    CloseDrawer((WindowPtr)hd, true);
	else
#endif
       if(qt_mac_is_macsheet(this))
           HideSheetWindow((WindowPtr)hd);
	else
	    ShowHide((WindowPtr)hd, false); //now we hide
	SizeWindow((WindowPtr)hd, 0, 0, true);
	if(isActiveWindow()) {
	    QWidget *w = NULL;
	    if(parentWidget())
		w = parentWidget()->topLevelWidget();
	    if(!w || !w->isVisible()) {
		for(WindowPtr wp = GetFrontWindowOfClass(kDocumentWindowClass, true);
		    wp; wp = GetNextWindowOfClass(wp, kDocumentWindowClass, true)) {
		    if((w = QWidget::find((WId)wp)))
			break;
		}
	    }
	    if(w && w->isVisible())
		qt_event_request_activate(w);
	}
    } else if(!parentWidget(TRUE) || parentWidget(TRUE)->isVisible()) { //strange!! ###
	qt_dirty_wndw_rgn("hide",this, mac_rect(posInWindow(this), geometry().size()));
    }
    deactivateWidgetCleanup();
    qt_mac_event_release(this);
}

void QWidget::setWindowState(uint newstate)
{
    uint oldstate = windowState();

    bool needShow = FALSE;
    if(isTopLevel()) {
	if((oldstate & WindowMinimized) != (newstate & WindowMinimized))
	    CollapseWindow((WindowPtr)hd, (newstate & WindowMinimized) ? true : false);

	if((oldstate & WindowFullScreen) != (newstate & WindowFullScreen)) {
	    if(newstate & WindowFullScreen) {
		if(QTLWExtra *tlextra = topData()) {
		    if(tlextra->normalGeometry.width() < 0) {
			if(testWState(WState_Resized))
			    tlextra->normalGeometry = geometry();
			else
			    tlextra->normalGeometry = QRect(pos(), qt_naturalWidgetSize(this));
		    }
		    topData()->savedFlags = getWFlags();
		}
                // This is a bit evil, but it keeps things like toolbars from being
                // obscured by the menubar as listed in task 41205.
               int finalScreen = qApp->desktop()->screenNumber(this);
               QRect screen = qApp->desktop()->availableGeometry(finalScreen);
               GDHandle g = GetMainDevice();
               int i = 0;
               while (i < finalScreen) {
                   g = GetNextDevice(g);
                   ++i;
               }
	       RgnHandle rgn = qt_mac_get_rgn();
	       // Give us the region that basically is the screen minus menubar and rectangle
               // the dock has -- a tad bit more than GetAvailableWindowPositioningBounds.
               if (GetAvailableWindowPositioningRegion(g, rgn) == noErr) {
                   Rect r;
                   GetRegionBounds(rgn, &r);
                   screen = QRect(r.left, r.top, r.right - r.left, r.bottom - r.top);
               }
	       qt_mac_dispose_rgn(rgn);
	       reparent(0, WType_TopLevel | WStyle_Customize | WStyle_NoBorder | (getWFlags() & 0xffff0000), //preserve some flags
		       screen.topLeft());
		resize(screen.size());
	    } else {
		reparent(0, topData()->savedFlags, topData()->normalGeometry.topLeft());
		resize(topData()->normalGeometry.size());
                topData()->normalGeometry = QRect(0, 0, -1, -1);
	    }
	}

	if((oldstate & WindowMaximized) != (newstate & WindowMaximized)) {
	    if(newstate & WindowMaximized) {
		Rect bounds;
		fstrut_dirty = TRUE;
		QDesktopWidget *dsk = QApplication::desktop();
		QRect avail = dsk->availableGeometry(dsk->screenNumber(this));
		SetRect(&bounds, avail.x(), avail.y(), avail.x() + avail.width(), avail.y() + avail.height());
		if(QWExtra   *extra = extraData()) {
		    if(bounds.right - bounds.left > extra->maxw)
			bounds.right = bounds.left + extra->maxw;
		    if(bounds.bottom - bounds.top > extra->maxh)
			bounds.bottom = bounds.top + extra->maxh;
		}
		if(QTLWExtra *tlextra = topData()) {
		    if(tlextra->normalGeometry.width() < 0) {
			if(testWState(WState_Resized))
			    tlextra->normalGeometry = geometry();
			else
			    tlextra->normalGeometry = QRect(pos(), qt_naturalWidgetSize(this));
		    }
		    if(fstrut_dirty)
			updateFrameStrut();
		    bounds.left += tlextra->fleft;
                    if(bounds.right < avail.x()+avail.width())
                        bounds.right = QMIN((uint)avail.x()+avail.width(), bounds.right+tlextra->fleft);
		    bounds.top += tlextra->ftop;
                    if(bounds.bottom < avail.y()+avail.height())
                        bounds.bottom = QMIN((uint)avail.y()+avail.height(), bounds.bottom+tlextra->ftop);
		    bounds.right -= tlextra->fright;
		    bounds.bottom -= tlextra->fbottom;
		}
		QRect orect(geometry().x(), geometry().y(), width(), height()),
		    nrect(bounds.left, bounds.top, bounds.right - bounds.left, bounds.bottom - bounds.top);
		if(orect != nrect) { // no real point..
		    Rect oldr;
		    if(QTLWExtra *tlextra = topData())
			SetRect(&oldr, tlextra->normalGeometry.left(), tlextra->normalGeometry.top(),
				tlextra->normalGeometry.right()+1, tlextra->normalGeometry.bottom()+1);
		    else
			SetRect(&oldr, orect.x(), orect.y(), orect.right(), orect.bottom());
		    SetWindowUserState((WindowPtr)hd, &oldr); //store the old area
		    qt_dirty_wndw_rgn("maximize",this, mac_rect(rect())); //dirty the old area

		    SetWindowStandardState((WindowPtr)hd, &bounds);
		    ZoomWindow((WindowPtr)hd, inZoomOut, FALSE);

		    crect = nrect;
		    if(isVisible()) {
			dirtyClippedRegion(TRUE);
			//issue a resize
			QResizeEvent qre(size(), orect.size());
			QApplication::sendEvent(this, &qre);
			//issue a move
			QMoveEvent qme(pos(), orect.topLeft());
			QApplication::sendEvent(this, &qme);
		    }
		}
	    } else {
		ZoomWindow((WindowPtr)hd, inZoomIn, false);
		if(QTLWExtra *tlextra = topData()) {
		    if(tlextra->normalGeometry.width() < 0)
			clearWState(WState_Resized);
		}
		Rect bounds;
		GetPortBounds(GetWindowPort((WindowPtr)hd), &bounds);
		qt_dirty_wndw_rgn("un-maximize",this, &bounds);
                topData()->normalGeometry = QRect(0, 0, -1, -1);
	    }
	}
    }


    widget_state &= ~(WState_Minimized | WState_Maximized | WState_FullScreen);
    if(newstate & WindowMinimized)
	widget_state |= WState_Minimized;
    if(newstate & WindowMaximized)
	widget_state |= WState_Maximized;
    if(newstate & WindowFullScreen)
	widget_state |= WState_FullScreen;

    if(needShow)
	show();

    if(newstate & WindowActive)
	setActiveWindow();

    QEvent e(QEvent::WindowStateChange);
    QApplication::sendEvent(this, &e);
}

void QWidget::raise()
{
    if(isDesktop())
	return;
    if(isTopLevel()) {
	//raise this window
	BringToFront((WindowPtr)hd);
	//we get to be the active process now
	ProcessSerialNumber psn;
	GetCurrentProcess(&psn);
	SetFrontProcessWithOptions(&psn, kSetFrontProcessFrontWindowOnly);
    } else if(QWidget *p = parentWidget(TRUE)) {
	QRegion clp;
	if(isVisible())
	    clp = clippedRegion(FALSE);
	if(p->childObjects && p->childObjects->findRef(this) >= 0)
	    p->childObjects->append(p->childObjects->take());
	if(isVisible()) {
	    dirtyClippedRegion(TRUE);
	    clp ^= clippedRegion(FALSE);
	    qt_dirty_wndw_rgn("raise",this, clp);
	}
    }
}

void QWidget::lower()
{
    if(isDesktop())
	return;

    if(isTopLevel()) {
	SendBehind((WindowPtr)handle(), NULL);
    } else if(QWidget *p = parentWidget(TRUE)) {
	QRegion clp;
	if(isVisible())
	    clp = clippedRegion(FALSE);
	if(p->childObjects && p->childObjects->findRef(this) >= 0)
	    p->childObjects->insert(0, p->childObjects->take());
	if(isVisible()) {
	    dirtyClippedRegion(TRUE);
	    clp ^= clippedRegion(FALSE);
	    qt_dirty_wndw_rgn("lower",this, clp);
	}
    }
}


void QWidget::stackUnder(QWidget *w)
{
    if(!w || isTopLevel() || isDesktop())
	return;

    QWidget *p = parentWidget();
    if(!p || p != w->parentWidget())
	return;
    int loc = p->childObjects->findRef(w);
    QRegion clp;
    if(isVisible())
	clp = clippedRegion(FALSE);
    if(loc >= 0 && p->childObjects && p->childObjects->findRef(this) >= 0)
	p->childObjects->insert(loc, p->childObjects->take());
    if(isVisible()) {
	dirtyClippedRegion(TRUE);
	clp ^= clippedRegion(FALSE);
	qt_dirty_wndw_rgn("stackUnder",this, clp);
    }
}

void QWidget::internalSetGeometry(int x, int y, int w, int h, bool isMove)
{
    if(isTopLevel() && isMove) {
	createTLExtra();
	extra->topextra->is_moved = 1;
    }
    if(isDesktop())
	return;
    if(extra) {				// any size restrictions?
	if(isTopLevel()) {
	    qt_mac_update_sizer(this);
	    if(testWFlags(WStyle_Maximize)) {
		if(extra->maxw && extra->maxh && extra->maxw == extra->minw
			&& extra->maxh == extra->minh)
		    ChangeWindowAttributes((WindowRef)handle(), kWindowNoAttributes,
					   kWindowFullZoomAttribute);
		else
		    ChangeWindowAttributes((WindowRef)handle(), kWindowFullZoomAttribute,
					   kWindowNoAttributes);
	    }
	}
	w = QMIN(w,extra->maxw);
	h = QMIN(h,extra->maxh);
	w = QMAX(w,extra->minw);
	h = QMAX(h,extra->minh);

	// Deal with size increment
	if(extra->topextra) {
	    if(extra->topextra->incw) {
		w = w/extra->topextra->incw;
		w *= extra->topextra->incw;
	    }
	    if(extra->topextra->inch) {
		h = h/extra->topextra->inch;
		h *= extra->topextra->inch;
	    }
	}
    }
    if(w < 1)                                // invalid size
	w = 1;
    if(h < 1)
	h = 1;

    QPoint oldp = pos();
    QSize  olds = size();
    if(!isTopLevel() && QSize(w, h) == olds && QPoint(x, y) == oldp)
	return;
    const bool visible = isVisible();
    QRegion oldregion, clpreg;
    if(visible) {
	oldregion = clippedRegion(FALSE);
	dirtyClippedRegion(FALSE);
	crect = QRect(x, y, w, h);
	dirtyClippedRegion(TRUE);
    } else {
	crect = QRect(x, y, w, h);
    }

    bool isResize = (olds != size());
    if(isResize && isMaximized())
        clearWState(WState_Maximized);
    if(isTopLevel() && winid && own_id) {
	if(isResize)
	    SizeWindow((WindowPtr)hd, w, h, true);
	if(isMove)
	    MoveWindow((WindowPtr)hd, x, y, true);
	dirtyClippedRegion(TRUE);
    }

    if(isMove || isResize) {
	if(!visible) {
	    if(isResize)
		QApplication::postEvent(this, new QResizeEvent(size(), olds));
	    if(isMove && oldp != pos())
		QApplication::postEvent(this, new QMoveEvent(pos(), oldp));
	} else {
	    QRegion bltregion, clpreg = clippedRegion(FALSE);
	    const bool oldreg_empty=oldregion.isNull(), newreg_empty = clpreg.isNull();
	    if(!oldreg_empty) {
		//setup the old clipped region..
		bltregion = oldregion;
		if(isMove && !isTopLevel())
		    bltregion.translate(pos().x() - oldp.x(), pos().y() - oldp.y());
		bltregion &= clpreg;
		{   //can't blt that which is dirty
		    RgnHandle r = qt_mac_get_rgn();
		    GetWindowRegion((WindowPtr)hd, kWindowUpdateRgn, r);
		    if(!EmptyRgn(r)) {
			QRegion dirty; //the dirty region
			CopyRgn(r, dirty.handle(TRUE));
			dirty.translate(-topLevelWidget()->geometry().x(),
					-topLevelWidget()->geometry().y());
			if(isMove && !isTopLevel()) //need to be in new coords
			    dirty.translate(pos().x() - oldp.x(), pos().y() - oldp.y());
			bltregion -= dirty;
		    }
		    qt_mac_dispose_rgn(r);
		}

		if(isMove && !no_move_blt && !isTopLevel()) {
		    QWidget *p = parentWidget(TRUE);
		    if(!p)
			p = this;
		    QMacSavedPortInfo pi(p, bltregion);
		    unclippedBitBlt(p, pos().x(), pos().y(), p, oldp.x(), oldp.y(),
				    olds.width(), olds.height(), Qt::CopyROP, TRUE, TRUE);
		}
	    }
	    if((!newreg_empty || !oldreg_empty) &&
	       (isResize || !isTopLevel() || !QDIsPortBuffered(GetWindowPort((WindowPtr)hd)))) {
		//finally issue "expose" event
		QRegion upd((oldregion + clpreg) - bltregion);
		if(isResize && !testWFlags(WStaticContents))
		    upd += clippedRegion();
		qt_dirty_wndw_rgn("internalSetGeometry",this, upd);
		//and force the update
		if(isResize || 1)
		    qt_event_request_updates();
	    }
	    //Do these last, as they may cause an event which paints, and messes up
	    //what we blt above
	    if(isResize) { //send the resize event..
		QResizeEvent e(size(), olds);
		QApplication::sendEvent(this, &e);
	    }
	    if(isMove && pos() != oldp) { //send the move event..
		QMoveEvent e(pos(), oldp);
		QApplication::sendEvent(this, &e);
	    }
	}
    }
}

void QWidget::setMinimumSize(int minw, int minh)
{
#if defined(QT_CHECK_RANGE)
    if(minw < 0 || minh < 0)
	qWarning("Qt: QWidget::setMinimumSize: The smallest allowed size is (0,0)");
#endif
    createExtra();
    if(extra->minw == minw && extra->minh == minh)
	return;
    extra->minw = minw;
    extra->minh = minh;
    if(minw > width() || minh > height()) {
	bool resized = testWState(WState_Resized);
	resize(QMAX(minw,width()), QMAX(minh,height()));
	if(!resized)
	    clearWState(WState_Resized); //not a user resize
    }
    updateGeometry();
}

void QWidget::setMaximumSize(int maxw, int maxh)
{
#if defined(QT_CHECK_RANGE)
    if(maxw > QWIDGETSIZE_MAX || maxh > QWIDGETSIZE_MAX) {
	qWarning("Qt: QWidget::setMaximumSize: (%s/%s) "
		"The largest allowed size is (%d,%d)",
		 name("unnamed"), className(), QWIDGETSIZE_MAX,
		QWIDGETSIZE_MAX);
	maxw = QMIN(maxw, QWIDGETSIZE_MAX);
	maxh = QMIN(maxh, QWIDGETSIZE_MAX);
    }
    if(maxw < 0 || maxh < 0) {
	qWarning("Qt: QWidget::setMaximumSize: (%s/%s) Negative sizes (%d,%d) "
		"are not possible",
		name("unnamed"), className(), maxw, maxh);
	maxw = QMAX(maxw, 0);
	maxh = QMAX(maxh, 0);
    }
#endif
    createExtra();
    if(extra->maxw == maxw && extra->maxh == maxh)
	return;
    extra->maxw = maxw;
    extra->maxh = maxh;
    if(maxw < width() || maxh < height()) {
	bool resized = testWState(WState_Resized);
	resize(QMIN(maxw,width()), QMIN(maxh,height()));
	if(!resized)
	    clearWState(WState_Resized); //not a user resize
    }
    updateGeometry();
}


void QWidget::setSizeIncrement(int w, int h)
{
    createTLExtra();
    extra->topextra->incw = w;
    extra->topextra->inch = h;
}

void QWidget::setBaseSize(int w, int h)
{
    createTLExtra();
    extra->topextra->basew = w;
    extra->topextra->baseh = h;
}

void QWidget::erase(int x, int y, int w, int h)
{
    erase(QRegion(x, y, w, h));
}

void QWidget::erase(const QRegion& reg)
{
    if(backgroundMode() == NoBackground || isDesktop() || !isVisible())
	return;
    QRect rr(reg.boundingRect());
    bool unclipped = testWFlags(WPaintUnclipped);
    clearWFlags(WPaintUnclipped);
    QPainter p(this);
    if(unclipped)
	setWFlags(WPaintUnclipped);
    p.setClipRegion(reg);
    if(extra && extra->bg_pix) {
	if(!extra->bg_pix->isNull()) {
	    QPoint offset = backgroundOffset();
	    p.drawTiledPixmap(rr,*extra->bg_pix,
			      QPoint(rr.x()+(offset.x()%extra->bg_pix->width()),
				     rr.y()+(offset.y()%extra->bg_pix->height())));
	}
    } else {
	p.fillRect(rr, bg_col);
    }
    p.end();
}


void QWidget::scroll(int dx, int dy)
{
    scroll(dx, dy, QRect());
}

void QWidget::scroll(int dx, int dy, const QRect& r)
{
    bool valid_rect = r.isValid();
    if(testWState(WState_BlockUpdates) &&  (valid_rect || !children()))
	return;

    QRect sr = valid_rect ? r : rect();
    if(dx == 0 && dy == 0)
	return;
    int x1, y1, x2, y2, w=sr.width(), h=sr.height();
    if(dx > 0) {
	x1 = sr.x();
	x2 = x1+dx;
	w -= dx;
    } else {
	x2 = sr.x();
	x1 = x2-dx;
	w += dx;
    }
    if(dy > 0) {
	y1 = sr.y();
	y2 = y1+dy;
	h -= dy;
    } else {
	y2 = sr.y();
	y1 = y2-dy;
	h += dy;
    }

    bool just_update = QABS(dx) > width() || QABS(dy) > height();
    if(just_update)
	update();

    QRegion bltd;
    QPoint p(posInWindow(this));
    if(w > 0 && h > 0) {
	bltd = clippedRegion(valid_rect); //offset the clip
	bltd.translate(dx, dy);
	QRegion requested(x2, y2, w, h); //only that which I blt to
	requested.translate(p.x(), p.y());
	bltd &= requested;
	bltd &= clippedRegion(valid_rect); //finally clip to clipping region
	{   //can't blt that which is dirty
	    RgnHandle r = qt_mac_get_rgn();
	    GetWindowRegion((WindowPtr)hd, kWindowUpdateRgn, r);
	    if(!EmptyRgn(r)) {
		QRegion dirty; //the dirty region
		CopyRgn(r, dirty.handle(TRUE));
		dirty.translate(-topLevelWidget()->geometry().x(),
				-topLevelWidget()->geometry().y());
		bltd -= dirty;
		debug_wndw_rgn("this is the dirty area", this, dirty);
	    }
	    qt_mac_dispose_rgn(r);
	}
    }
    dirtyClippedRegion(TRUE);
    if(!valid_rect && children()) {	// scroll children
	QPoint pd(dx, dy);
	QWidgetList moved;
	register QObject *o;
	for(QObjectListIt it(*children()); (o=it.current()); ++it) { //first move all children
	    if(o->isWidgetType()) {
		QWidget *w = (QWidget*)o;
		w->crect = QRect(w->pos() + pd, w->size());
		moved.append(w);
	    }
	}
        //now send move events (do not do this in the above loop, breaks QAquaFocusWidget)
	for(QWidget *w = moved.first(); w; w = moved.next()) {
	    w->dirtyClippedRegion(TRUE);
	    QMoveEvent e(w->pos(), w->pos() - pd);
	    QApplication::sendEvent(w, &e);
	}
    }
    {
	QMacSavedPortInfo pi(this, bltd);
	unclippedBitBlt(this,x2,y2,this,x1,y1,w,h,Qt::CopyROP,TRUE,TRUE);
    }

    if(just_update)
	return;
    QRegion newarea(sr);
    newarea.translate(p.x(), p.y());
    newarea &= (clippedRegion(valid_rect) - bltd);
    qt_clean_wndw_rgn("scroll", this, newarea);
    newarea.translate(-p.x(), -p.y());
    qt_paint_children(this, newarea, PC_None);
#if 0
    if(QDIsPortBuffered(GetWindowPort((WindowPtr)hd)))
	QMacSavedPortInfo::flush(this);
#endif
}

void QWidget::drawText(int x, int y, const QString &str)
{
    if(!testWState(WState_Visible))
	return;
    QPainter paint(this);
    paint.drawText(x, y, str);
}

int QWidget::metric(int m) const
{
    switch(m) {
    case QPaintDeviceMetrics::PdmHeightMM: // 75 dpi is 3dpmm
	return (metric(QPaintDeviceMetrics::PdmHeight)*100)/288;
    case QPaintDeviceMetrics::PdmWidthMM: // 75 dpi is 3dpmm
	return (metric(QPaintDeviceMetrics::PdmWidth)*100)/288;
    case QPaintDeviceMetrics::PdmWidth:
    {
	if(!isTopLevel())
	    return crect.width();
	Rect windowBounds;
	GetPortBounds(GetWindowPort(((WindowPtr)hd)), &windowBounds);
	return windowBounds.right;
    }

    case QPaintDeviceMetrics::PdmHeight:
    {
	if(!isTopLevel())
	    return crect.height();
	Rect windowBounds;
	GetPortBounds(GetWindowPort(((WindowPtr)hd)), &windowBounds);
	return windowBounds.bottom;
    }
    case QPaintDeviceMetrics::PdmDepth:// FIXME : this is a lie in most cases
	return 16;
    case QPaintDeviceMetrics::PdmDpiX: // FIXME : this is a lie in most cases
    case QPaintDeviceMetrics::PdmPhysicalDpiX:
	return 80;
    case QPaintDeviceMetrics::PdmDpiY: // FIXME : this is a lie in most cases
    case QPaintDeviceMetrics::PdmPhysicalDpiY:
	return 80;
    default: //leave this so the compiler complains when new ones are added
	qWarning("Qt: QWidget::metric unhandled parameter %d",m);
	return QPaintDevice::metric(m);// XXX
    }
    return 0;
}

void QWidget::createSysExtra()
{
    extra->has_dirty_area = FALSE;
    extra->child_serial = extra->clip_serial = 1;
    extra->child_dirty = extra->clip_dirty = TRUE;
    extra->macDndExtra = 0;
}

void QWidget::deleteSysExtra()
{
}

void QWidget::createTLSysExtra()
{
    extra->topextra->group = NULL;
    extra->topextra->is_moved = 0;
    extra->topextra->resizer = 0;
}

void QWidget::deleteTLSysExtra()
{
    if(extra->topextra->group)
	qt_mac_release_window_group(extra->topextra->group);
}

bool QWidget::acceptDrops() const
{
    return macDropEnabled;
}

void QWidget::updateFrameStrut() const
{
    QWidget *that = (QWidget *) this; //mutable
    if(!fstrut_dirty) {
	that->fstrut_dirty = isVisible();
	return;
    }
    that->fstrut_dirty = FALSE;
    QTLWExtra *top = that->topData();
    top->fleft = top->fright = top->ftop = top->fbottom = 0;
    if(!isDesktop() && isTopLevel()) {
	Rect window_r, content_r;
	//get bounding rects
	RgnHandle rgn = qt_mac_get_rgn();
	GetWindowRegion((WindowPtr)hd, kWindowStructureRgn, rgn);
	GetRegionBounds(rgn, &window_r);
	GetWindowRegion((WindowPtr)hd, kWindowContentRgn, rgn);
	GetRegionBounds(rgn, &content_r);
	qt_mac_dispose_rgn(rgn);
	//put into qt structure
	top->fleft = content_r.left - window_r.left;
	top->ftop = content_r.top - window_r.top;
	top->fright = window_r.right - content_r.right;
	top->fbottom = window_r.bottom - window_r.bottom;
    }
}

void QWidget::setAcceptDrops(bool on)
{
    if((on && macDropEnabled) || (!on && !macDropEnabled))
	return;
    macDropEnabled = on;
    if(!on && !topLevelWidget()->extraData()) //short circuit
	return;
    topLevelWidget()->createExtra();
    if(on)
	qt_macdnd_register(topLevelWidget(),  topLevelWidget()->extraData());
    else
	qt_macdnd_unregister(topLevelWidget(), topLevelWidget()->extraData());
}

void QWidget::setMask(const QRegion &region)
{
    createExtra();
    if(region.isNull() && extra->mask.isNull())
	return;

    QRegion clp;
    if(isVisible())
	clp = clippedRegion(FALSE);
#if 0
    if(qMacVersion() >= Qt::MV_10_DOT_2) {
	QRect r = region.boundingRect();
	region.translate(r.x(), r.y());
    }
#endif
    extra->mask = region;
    if(isVisible()) {
	dirtyClippedRegion(TRUE);
	clp ^= clippedRegion(FALSE);
	qt_dirty_wndw_rgn("setMask", this, clp);
    }
    if(isTopLevel())
	ReshapeCustomWindow((WindowPtr)hd); //now let the wdef take it
}

void QWidget::setMask(const QBitmap &bitmap)
{
    setMask(QRegion(bitmap));
}


void QWidget::clearMask()
{
    setMask(QRegion());
}

void QWidget::setName(const char *name)
{
    QObject::setName(name);
}

void QWidget::propagateUpdates(bool update_rgn)
{
    QRegion rgn;
    QWidget *widg = this;
    if(update_rgn) {
	widg = topLevelWidget();
	QMacSavedPortInfo savedInfo(this);
	GetWindowRegion((WindowPtr)hd, kWindowUpdateRgn, rgn.handle(TRUE));
	if(rgn.isEmpty())
	    return;
	rgn.translate(-widg->geometry().x(), -widg->geometry().y());
	BeginUpdate((WindowPtr)hd);
    } else {
	rgn = QRegion(rect());
    }
#ifdef DEBUG_WINDOW_RGNS
    debug_wndw_rgn("*****propagatUpdates", widg, rgn, TRUE);
#endif
    qt_paint_children(widg, rgn, PC_ForceErase);
    if(update_rgn)
	EndUpdate((WindowPtr)hd);
}

/*!
    \internal
*/
void QWidget::setRegionDirty(bool child)
{
    if(!extra)
	return;
    if(child) {
	extra->clip_serial++;
	extra->clip_dirty = TRUE;
    } else {
	extra->child_serial++;
	extra->child_dirty = TRUE;
    }
}

/*!
    \internal
*/
void QWidget::dirtyClippedRegion(bool dirty_myself)
{
    if(qApp->closingDown())
	return;
    if(dirty_myself && !wasDeleted) {
	//dirty myself
	if(extra) {
	    setRegionDirty(FALSE);
	    setRegionDirty(TRUE);
	}
	//when I get dirty so do my children
	if(QObjectList *chldn = queryList()) {
	    QObjectListIt it(*chldn);
	    for(QObject *obj; (obj = it.current()); ++it) {
		if(obj->isWidgetType() && !obj->wasDeleted) {
		    QWidget *w = (QWidget *)(*it);
		    if(!w->isTopLevel() && w->isVisible())
			w->setRegionDirty(TRUE);
		}
	    }
	    delete chldn;
	}
    }

    if(!isTopLevel()) { //short circuit, there is nothing to dirty here..
	int ox = x(), oy = y(), ow = width(), oh = height();
	for(QWidget *par=this; (par = par->parentWidget(TRUE)); ) {
	    if(ox + ow < 0 || oy + oh < 0 || ox > par->width() || oy > par->height())
		return;
	    ox += par->x();
	    oy += par->y();
	}
    }
    //handle the rest of the widgets
    const QPoint myp(posInWindow(this));
    QRect myr(myp.x(), myp.y(), width(), height());
    QWidget *last = this, *w, *parent=parentWidget();
    int px = myp.x() - x(), py = myp.y() - y();
    for(QWidget *widg = parent; widg; last = widg, widg = widg->parentWidget()) {
	if(widg->wasDeleted) //no point in dirting
	    continue;
	myr = myr.intersect(QRect(px, py, widg->width(), widg->height()));
	widg->setRegionDirty(FALSE);
	if(widg == parent)
	    widg->setRegionDirty(TRUE);

	if(const QObjectList *chldn = widg->children()) {
	    for(QObjectListIt it(*chldn); it.current() && it.current() != last; ++it) {
		if((*it)->isWidgetType() && !(*it)->wasDeleted) {
		    w = (QWidget *)(*it);
		    if(!w->isTopLevel() && w->isVisible()) {
			QPoint wp(px + w->x(), py + w->y());
			if(myr.intersects(QRect(wp.x(), wp.y(), w->width(), w->height()))) {
			    w->setRegionDirty(TRUE);
			    if(QObjectList *chldn2 = w->queryList()) {
				QObjectListIt it2(*chldn2);
				for(QObject *obj; (obj = it2.current()); ++it2) {
				    if(obj->isWidgetType() && !obj->wasDeleted) {
					QWidget *w = (QWidget *)(*it2);
					/* this relies on something that may change in the future
					   if hd for all sub widgets != toplevel widget's hd, then
					   this function will not work any longer */
					if(w->hd == hd &&
					   !w->isTopLevel() && w->isVisible())
					    w->setRegionDirty(TRUE);
				    }
				}
				delete chldn2;
			    }
			}
		    }
		}
	    }
	}
	px -= widg->x();
	py -= widg->y();
    }
}

/*!
    \internal
*/
bool QWidget::isClippedRegionDirty()
{
    if(!extra || extra->clip_dirty)
	return TRUE;
    if(/*!isTopLevel() && */(parentWidget(TRUE) && parentWidget(TRUE)->isClippedRegionDirty()))
	return TRUE;
    return FALSE;
}

#ifndef QMAC_NO_QUARTZ
/*!
    \internal
*/
CGContextRef QWidget::macCGContext(bool do_children) const
{
    QWidget *that = (QWidget*)this;
    if(!extra)
	that->createExtra();
    bool dirty = !ctx ||
		 extra->clip_dirty || (do_children && extra->child_dirty) ||
		 (do_children && !extra->ctx_children_clipped);
    if(!ctx)
	CreateCGContextForPort(GetWindowPort((WindowPtr)hd), &that->ctx);
    if(dirty) {
	Rect r;
	ValidWindowRect((WindowPtr)hd, &r);
	extra->ctx_children_clipped = do_children;
	QRegion reg = that->clippedRegion(do_children);
	ClipCGContextToRegion(ctx, &r, reg.handle(TRUE));
    }
    return ctx;
}
#endif

/*!
    \internal
*/
uint QWidget::clippedSerial(bool do_children)
{
    createExtra();
    return do_children ? extra->clip_serial : extra->child_serial;
}

/*!
    \internal
*/
QRegion QWidget::clippedRegion(bool do_children)
{
    //the desktop doesn't participate in our clipping games
    if(isDesktop()) {
	createExtra();
	if(!extra->clip_dirty && (!do_children || !extra->child_dirty)) {
	    if(!do_children)
		return extra->clip_sibs;
	    return extra->clip_saved;
	}
	extra->child_dirty = (extra->clip_dirty = FALSE);
	return extra->clip_sibs = extra->clip_children = QRegion(0, 0, width(), height());
    }

    if(wasDeleted || !isVisible() ||  qApp->closingDown() || qApp->startingUp())
	return QRegion();

    createExtra();
    if(!extra->clip_dirty && (!do_children || !extra->child_dirty)) {
	if(!do_children)
	    return extra->clip_sibs;
	return extra->clip_saved;
    }

    bool no_children = !children() || !children()->count();
    /* If we have no children, and we are clearly off the screen we just get an automatic
       null region. This is to allow isNull() to be a cheap test of "off-screen" plus it
       prevents all the below calculations (specifically posInWindow() is pointless). */
    QPoint mp; //My position in the window (posInWindow(this))
    if(!isTopLevel() && no_children) { //short-circuit case
	int px = x(), py = y();
	for(QWidget *par = parentWidget(TRUE); par; par = par->parentWidget(TRUE)) {
	    if((px + width() < 0) || (py + height() < 0) ||
	       px > par->width() || py > par->height()) {
		extra->child_dirty = (extra->clip_dirty = FALSE);
		return extra->clip_saved = extra->clip_children = extra->clip_sibs = QRegion();
	    }
	    if(par->isTopLevel())
		break;
	    px += par->x();
	    py += par->y();
	}
	mp = QPoint(px, py);
    } else {
	mp = posInWindow(this);
    }

    /* This whole vis_width / vis_height is to prevent creating very large regions,
       as RgnHandle's just use Rect's SHRT_MAX is the maximum value, which causes strange
       problems when widgets are placed further onto a window. It should be quite unlikely
       that a top level window >SHRT_MAX in either width or height. As when these change they
       should be dirtied this optimization should mean nothing otherwise */
    int vis_width = width(), vis_height = height(), vis_x = 0, vis_y = 0;
    if(QWidget *par = parentWidget(TRUE)) {
	int px = mp.x() - x(), py = mp.y() - y();
	int min_x = 0, min_y = 0,
	    max_x = mp.x() + vis_width, max_y = mp.y() + vis_height;
	for(; par; par = par->parentWidget(TRUE)) {
	    if(px > mp.x() && px - mp.x() > min_x)
		min_x = px - mp.x();
	    if(py > mp.y() && py - mp.y() > min_y)
		min_y = py - mp.y();
	    if(px + par->width() < max_x)
		max_x = px + par->width();
	    if(py + par->height() < max_y)
		max_y = py + par->height();
	    px -= par->x();
	    py -= par->y();
	}
	vis_x = min_x;
	vis_y = min_y;
	vis_width =  max_x > mp.x() ? (max_x - mp.x()) - min_x : 0;
	vis_height = max_y > mp.y() ? (max_y - mp.y()) - min_y : 0;
    }

    //clip out my children
    QRegion mask;
    if(isClippedRegionDirty() || (do_children && extra->child_dirty)) {
	extra->child_dirty = FALSE;
	extra->clip_children = QRegion(vis_x, vis_y, vis_width, vis_height);
	if(!no_children) {
	    const QObjectList *chldnlst=children();
	    QRect sr(vis_x, vis_y, vis_width, vis_height);
	    for(QObjectListIt it(*chldnlst); it.current(); ++it) {
		if((*it)->isWidgetType() && !(*it)->wasDeleted) {
		    QWidget *cw = (QWidget *)(*it);
		    if(cw->isVisible() && !cw->isTopLevel() && sr.intersects(cw->geometry())) {
			QRegion childrgn(cw->x(), cw->y(), cw->width(), cw->height());
			if(cw->extra && !cw->extra->mask.isNull()) {
			    mask = cw->extra->mask;
			    mask.translate(cw->x(), cw->y());
			    childrgn &= mask;
			}
			extra->clip_children -= childrgn;
		    }
		}
	    }
	}
    }

    if(isClippedRegionDirty()) {
	extra->clip_dirty = FALSE;
	extra->clip_sibs = QRegion(mp.x()+vis_x, mp.y()+vis_y, vis_width, vis_height);
	//clip my rect with my mask
	if(extra && !extra->mask.isNull() && (vis_width || vis_height)) {
	    mask = extra->mask;
	    mask.translate(mp.x(), mp.y());
	    extra->clip_sibs &= mask;
	}

	//clip away my siblings
	if(!isTopLevel() && parentWidget() && (vis_width || vis_height)) {
	    if(const QObjectList *siblst = parentWidget()->children()) {
		QPoint tmp;
		QObjectListIt it(*siblst);
		//loop to this because its in zorder, and i don't care about people behind me
		for(it.toLast(); it.current() && it.current() != this; --it) {
		    if((*it)->isWidgetType() && !(*it)->wasDeleted) {
			QWidget *sw = (QWidget *)(*it);
			tmp = posInWindow(sw);
			QRect sr(tmp.x(), tmp.y(), sw->width(), sw->height());
			if(!sw->isTopLevel() && sw->isVisible() && extra->clip_sibs.contains(sr)) {
			    QRegion sibrgn(sr);
			    if(sw->extra && !sw->extra->mask.isNull()) {
				mask = sw->extra->mask;
				mask.translate(tmp.x(), tmp.y());
				sibrgn &= mask;
			    }
			    extra->clip_sibs -= sibrgn;
			}
		    }
		}
	    }
	}

	/*Remove window decorations from the top level window, specifically this
	  means the GrowRgn*/
	if(isTopLevel()) {
	    QRegion contents;
	    RgnHandle r = qt_mac_get_rgn();
	    GetWindowRegion((WindowPtr)hd, kWindowContentRgn, r);
	    if(!EmptyRgn(r)) {
		CopyRgn(r, contents.handle(TRUE));
		contents.translate(-geometry().x(), -geometry().y());
	    }
	    qt_mac_dispose_rgn(r);
	    extra->clip_sibs &= contents;
	} else if(parentWidget()) { //clip to parent
	    extra->clip_sibs &= parentWidget()->clippedRegion(FALSE);
	}
    }

    // If they are empty set them to null() that way we have a cheaper test
    if(extra->clip_children.isEmpty())
	extra->clip_children = QRegion();
    if(extra->clip_sibs.isEmpty())
	extra->clip_sibs = QRegion();
    if(no_children) {
	extra->clip_children = extra->clip_saved = extra->clip_sibs;
    } else if(extra->clip_children.isNull() || extra->clip_sibs.isNull()) {
	extra->clip_saved = QRegion();
    } else {
	QRegion chldrgns = extra->clip_children;
	chldrgns.translate(mp.x(), mp.y());
	extra->clip_saved = extra->clip_sibs & chldrgns;
    }

    //finally return the correct region
    if(do_children)
	return extra->clip_saved;
    return extra->clip_sibs;
}

void QWidget::resetInputContext()
{
    if (hasFocus())
        qt_mac_unicode_reset_input(this);
}

/*!
    \internal
*/
void QWidget::macWidgetChangedWindow()
{
}

void QWidget::setMouseTracking(bool enable)
{
    if(enable)
	setWState(WState_MouseTracking);
    else
	clearWState(WState_MouseTracking);
}

void QWidget::setWindowOpacity(double level)
{
    if(!isTopLevel())
	return;

    level = QMIN(QMAX(level, 0), 1.0);
    QMacSavedPortInfo::setWindowAlpha(this, level);
    topData()->opacity = (uchar)(level * 255);
}

double QWidget::windowOpacity() const
{
    return isTopLevel() ? ((QWidget*)this)->topData()->opacity / 255.0 : 0.0;
}
