/****************************************************************************
**
** Implementation of QWidget and QWindow classes for FB
**
** Created : 991026
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

#include "qcursor.h"
#include "qapplication.h"
#include "qapplication_p.h"
#include "qpaintdevicemetrics.h"
#include "qpainter.h"
#include "qbitmap.h"
#include "qimage.h"
#include "qwidgetlist.h"
#include "qwidgetintdict.h"
#include "qobjectlist.h"
#include "qaccel.h"
#include "qdragobject.h"
#include "qfocusdata.h"
#include "qlayout.h"
#include "qtextcodec.h"
#include "qcursor.h"
#include "qinputcontext_p.h"

#include "qwsdisplay_qws.h"
#include "qgfx_qws.h"
#include "qwsmanager_qws.h"
#include "qwsregionmanager_qws.h"
#include "qinputcontext_p.h"

void qt_insert_sip( QWidget*, int, int );	// defined in qapplication_x11.cpp
int  qt_sip_count( QWidget* );			// --- "" ---

// Paint event clipping magic
extern void qt_set_paintevent_clipping( QPaintDevice* dev, const QRegion& region);
extern void qt_clear_paintevent_clipping();

extern bool qt_xdnd_enable( QWidget* w, bool on );

extern void qt_deferred_map_add( QWidget* ); // defined in qapplication_x11.const
extern void qt_deferred_map_take( QWidget* );// defined in qapplication_x11.const
extern int *qt_last_x;
extern int *qt_last_y;
extern WId qt_last_cursor;
extern bool qws_overrideCursor;
extern QWidget *qt_pressGrab;
extern QWidget *qt_mouseGrb;

extern QRect qt_maxWindowRect;

extern void qwsUpdateActivePainters();

static QWidget *keyboardGrb = 0;

static int takeLocalId()
{
    static int n=-1000;
    return --n;
}

// This repaints all children within a widget.

static void paint_children(QWidget * p,const QRegion& r, bool post)
{
    if(!p)
	return;
    QObjectList * childObjects=(QObjectList*)p->children();
    if(childObjects) {
	QObject * o;
	for(o=childObjects->first();o!=0;o=childObjects->next()) {
	    if( o->isWidgetType() ) {
		QWidget *w = (QWidget *)o;
		if ( w->testWState(Qt::WState_Visible) ) {
		    QRegion wr( QRegion(w->geometry()) & r );
		    if ( !wr.isEmpty() ) {
			wr.translate(-w->x(),-w->y());
			if ( post )
			    QApplication::postEvent(w,new QPaintEvent(wr,
				   !w->testWFlags(QWidget::WRepaintNoErase) ) );
			else
			    w->repaint(wr, !w->testWFlags(QWidget::WRepaintNoErase));
			paint_children(w,wr,post);
		    }
		}
	    }
	}
    }
}

// Paint the widget and its children

static void paint_heirarchy(QWidget *w, bool post)
{
    if ( w && w->testWState(Qt::WState_Visible) ) {
	if ( post )
	    QApplication::postEvent(w,new QPaintEvent(w->rect(),
			!w->testWFlags(QWidget::WRepaintNoErase) ) );
	else
	    w->repaint(w->rect(),
		    !w->testWFlags(QWidget::WRepaintNoErase));

	QObjectList *childObjects=(QObjectList*)w->children();
	if ( childObjects ) {
	    QObject * o;
	    for(o=childObjects->first();o!=0;o=childObjects->next()) {
		if( o->isWidgetType() ) {
		    paint_heirarchy((QWidget *)o,post);
		}
	    }
	}
    }
}

/*****************************************************************************
  QWidget member functions
 *****************************************************************************/

void QWidget::create( WId window, bool initializeWindow, bool /*destroyOldWindow*/)
{
    if ( testWState(WState_Created) && window == 0 )
	return;
    setWState( WState_Created );			// set created flag

    if ( !parentWidget() || parentWidget()->isDesktop() )
	setWFlags( WType_TopLevel );		// top-level widget

    alloc_region_index = -1;
    alloc_region_revision = -1;
    isSettingGeometry = FALSE;
    overlapping_children = -1;

    bool topLevel = testWFlags(WType_TopLevel);
    bool popup = testWFlags(WType_Popup);
    bool dialog = testWFlags(WType_Dialog);
    bool desktop = testWFlags(WType_Desktop);
    WId	   id;
    QWSDisplay* dpy = qwsDisplay();

    if ( !window )				// always initialize
	initializeWindow = TRUE;


    if ( popup ) {
	setWFlags(WStyle_Tool); // a popup is a tool window
	setWFlags(WStyle_StaysOnTop); // a popup stays on top
    }
    if ( topLevel && parentWidget() ) {
	// if our parent has WStyle_StaysOnTop, so must we
	QWidget *ptl = parentWidget()->topLevelWidget();
	if ( ptl && ptl->testWFlags( WStyle_StaysOnTop ) )
	    setWFlags(WStyle_StaysOnTop);
    }

    int sw = dpy->width();
    int sh = dpy->height();

    if ( dialog || popup || desktop ) {		// these are top-level, too
	topLevel = TRUE;
	setWFlags( WType_TopLevel );
    }

    if ( desktop ) {				// desktop widget
	dialog = popup = FALSE;			// force these flags off
	crect.setRect( 0, 0, sw, sh );
    } else if ( topLevel ) {			// calc pos/size from screen
	crect.setRect( 0, 0, sw/2, 4*sh/10 );
    } else {					// child widget
	crect.setRect( 0, 0, 100, 30 );
    }

    if ( window ) {				// override the old window
	id = window;
	setWinId( window );
    } else if ( desktop ) {			// desktop widget
	id = (WId)-2;				// id = root window
	QWidget *otherDesktop = find( id );	// is there another desktop?
	if ( otherDesktop && otherDesktop->testWFlags(WPaintDesktop) ) {
	    otherDesktop->setWinId( 0 );	// remove id from widget mapper
	    setWinId( id );			// make sure otherDesktop is
	    otherDesktop->setWinId( id );	//   found first
	} else {
	    setWinId( id );
	}
    } else {
	id = topLevel ? dpy->takeId() : takeLocalId();
	setWinId( id );				// set widget id/handle + hd
    }

    if ( !topLevel ) {
	if ( !testWFlags(WStyle_Customize) )
	    setWFlags( WStyle_NormalBorder | WStyle_Title | WStyle_MinMax | WStyle_SysMenu  );
    } else if ( !(desktop || popup) ) {
	if ( testWFlags(WStyle_Customize) ) {	// customize top-level widget
	    if ( testWFlags(WStyle_NormalBorder) ) {
		// XXX ...
	    } else {
		if ( !testWFlags(WStyle_DialogBorder) ) {
		    // XXX ...
		}
	    }
	    if ( testWFlags(WStyle_Tool) ) {
		// XXX ...
	    }
	} else {				// normal top-level widget
	    setWFlags( WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu |
		       WStyle_MinMax );
	}
    }

    alloc_region_dirty=FALSE;
    paintable_region_dirty=FALSE;

    if ( !initializeWindow ) {
	// do no initialization
    } else if ( popup ) {			// popup widget
    } else if ( topLevel && !desktop ) {	// top-level widget
	QWidget *p = parentWidget();	// real parent
	if (p)
	    p = p->topLevelWidget();
	if ( testWFlags(WStyle_DialogBorder)
	     || testWFlags(WStyle_StaysOnTop)
	     || testWFlags(WStyle_Dialog)
	     || testWFlags(WStyle_Tool) ) {
	    // XXX ...
	}

	// find the real client leader, i.e. a toplevel without parent
	while ( p && p->parentWidget()) {
	    p = p->parentWidget()->topLevelWidget();
	}

	// XXX ...
    }

    if ( initializeWindow ) {
    }

    setWState( WState_MouseTracking );
    setMouseTracking( FALSE );			// also sets event mask
    if ( desktop ) {
	setWState( WState_Visible );
    } else if ( topLevel ) {			// set X cursor
	//QCursor *oc = QApplication::overrideCursor();
	if ( initializeWindow ) {
	    //XXX XDefineCursor( dpy, winid, oc ? oc->handle() : cursor().handle() );
	}
	setWState( WState_OwnCursor );
#ifndef QT_NO_WIDGET_TOPEXTRA
	qwsDisplay()->nameRegion( winId(), name(""), caption() );
#else
	qwsDisplay()->nameRegion( winId(), name(""), QString::null );
#endif
    }

    if ( topLevel ) {
#if !defined(QT_NO_WIDGET_TOPEXTRA) && !defined(QT_NO_QWS_MANAGER)
	createTLExtra();
#endif
#ifndef QT_NO_QWS_MANAGER
	if ( testWFlags(WStyle_DialogBorder)
	     || testWFlags(WStyle_NormalBorder))
	{
	    // get size of wm decoration
	    QRegion r = QApplication::qwsDecoration().region(this, crect);
	    QRect br( r.boundingRect() );
	    crect.moveBy( crect.x()-br.x(), crect.y()-br.y() );
	    extra->topextra->fleft = crect.x()-br.x();
	    extra->topextra->ftop = crect.y()-br.y();
	    extra->topextra->fright = br.right()-crect.right();
	    extra->topextra->fbottom = br.bottom()-crect.bottom();
	    topData()->qwsManager = new QWSManager(this);
	} else if ( topData()->qwsManager ) {
	    delete topData()->qwsManager;
	    topData()->qwsManager = 0;
	    crect.moveBy( -extra->topextra->fleft, -extra->topextra->ftop );
	    extra->topextra->fleft = extra->topextra->ftop =
		extra->topextra->fright = extra->topextra->fbottom = 0;
	}
#endif
	// declare the widget's object name as window role

	qt_fbdpy->addProperty(id,QT_QWS_PROPERTY_WINDOWNAME);
	qt_fbdpy->setProperty(id,QT_QWS_PROPERTY_WINDOWNAME,0,name());

	// If we are session managed, inform the window manager about it
	if ( extra && !extra->mask.isNull() ) {
	    req_region = extra->mask;
	    req_region.translate(crect.x(),crect.y());
	    req_region &= crect; //??? this is optional
	} else {
	    req_region = crect;
	}
	req_region = qt_screen->mapToDevice( req_region, QSize(qt_screen->width(), qt_screen->height()) );
    } else {
	if ( extra && extra->topextra )	{ // already allocated due to reparent?
	    extra->topextra->fleft = 0;
	    extra->topextra->ftop = 0;
	    extra->topextra->fright = 0;
	    extra->topextra->fbottom = 0;
	}
	//updateRequestedRegion( mapToGlobal(QPoint(0,0)) );
    }
}


void QWidget::destroy( bool destroyWindow, bool destroySubWindows )
{
    deactivateWidgetCleanup();
    if ( testWState(WState_Created) ) {
	clearWState( WState_Created );
	if ( children() ) {
	    QObjectListIt it(*children());
	    register QObject *obj;
	    while ( (obj=it.current()) ) {	// destroy all widget children
		++it;
		if ( obj->isWidgetType() )
		    ((QWidget*)obj)->destroy(destroySubWindows,
					     destroySubWindows);
	    }
	}
	releaseMouse();
	if ( qt_pressGrab == this )
	  qt_pressGrab = 0;
#ifndef QT_NO_QWS_MANAGER
	if (QWSManager::grabbedMouse() == this) {
	    if ( extra && extra->topextra && extra->topextra->qwsManager ) {
		extra->topextra->qwsManager->releaseGrab();
	    }
	}
#endif
	if ( keyboardGrb == this )
	    releaseKeyboard();
	if ( testWFlags(WShowModal) )		// just be sure we leave modal
	    qt_leave_modal( this );
	else if ( testWFlags(WType_Popup) )
	    qApp->closePopup( this );
	if ( testWFlags(WType_Desktop) ) {
	} else {
	    if ( parentWidget() && parentWidget()->testWState(WState_Created) ) {
		hideWindow();
	    }
	    if ( destroyWindow && isTopLevel() )
		qwsDisplay()->destroyRegion( winId() );
	}
	setWinId( 0 );
    }
}


void QWidget::reparentSys( QWidget *parent, WFlags f, const QPoint &p,
			bool showIt )
{
    QWidget* oldtlw = topLevelWidget();

#ifndef QT_NO_CURSOR
    QCursor oldcurs;
    bool setcurs=testWState(WState_OwnCursor);
    if ( setcurs ) {
	oldcurs = cursor();
	unsetCursor();
    }
#endif

    WId old_winid = winid;
    if ( testWFlags(WType_Desktop) )
	old_winid = 0;

    if ( !isTopLevel() && parentWidget() && parentWidget()->testWState(WState_Created) )
	hideWindow();

    setWinId( 0 );

    if ( parentObj != parent ) {
	QWidget *oldparent = parentWidget();
	if ( oldparent ) {				// remove from parent
	    oldparent->removeChild( this );
	    oldparent->setChildrenAllocatedDirty();
	    oldparent->paintable_region_dirty = TRUE;
	}
	if ( parent ) {				// insert into new parent
	    parent->insertChild( this );
	    parent->setChildrenAllocatedDirty();
	    parent->paintable_region_dirty = TRUE;
	}
    }
    bool     enable = isEnabled();		// remember status
    FocusPolicy fp = focusPolicy();
    QSize    s	    = size();
    //QPixmap *bgp    = (QPixmap *)backgroundPixmap();
    //QColor   bgc    = bg_col;			// save colors
#ifndef QT_NO_WIDGET_TOPEXTRA
    QString capt= caption();
#endif
    widget_flags = f;
    clearWState( WState_Created | WState_Visible | WState_ForceHide );
    create();
    if ( isTopLevel() || (!parent || parent->isVisible() ) )
	setWState( WState_ForceHide );	// new widgets do not show up in already visible parents
    /*
    if ( bgp )
	XSetWindowBackgroundPixmap( dpy, winid, bgp->handle() );
    else
	XSetWindowBackground( dpy, winid, bgc.pixel() );
    */
    setGeometry( p.x(), p.y(), s.width(), s.height() );
    setEnabled( enable );
    setFocusPolicy( fp );
#ifndef QT_NO_WIDGET_TOPEXTRA
    if ( !capt.isNull() ) {
	extra->topextra->caption = QString::null;
	setCaption( capt );
    }
#endif
    if ( showIt )
	show();
    if ( (int)old_winid > 0 )
	qwsDisplay()->destroyRegion( old_winid );
#ifndef QT_NO_CURSOR
    if ( setcurs ) {
	setCursor(oldcurs);
    }
#endif
    reparentFocusWidgets( oldtlw );		// fix focus chains
}


QPoint QWidget::mapToGlobal( const QPoint &pos ) const
{
    int	   x=pos.x(), y=pos.y();
    const QWidget* w = this;
    while (w) {
	x += w->crect.x();
	y += w->crect.y();
	w = w->isTopLevel() ? 0 : w->parentWidget();
    }
    return QPoint( x, y );
}

QPoint QWidget::mapFromGlobal( const QPoint &pos ) const
{
    int	   x=pos.x(), y=pos.y();
    const QWidget* w = this;
    while (w) {
	x -= w->crect.x();
	y -= w->crect.y();
	w = w->isTopLevel() ? 0 : w->parentWidget();
    }
    return QPoint( x, y );
}

void QWidget::setMicroFocusHint( int x, int y, int width, int height,
				 bool text, QFont *)
{
    if ( QRect( x, y, width, height ) != microFocusHint() )
	extraData()->micro_focus_hint.setRect( x, y, width, height );
#ifndef QT_NO_QWS_IM
    if ( text ) {
	QPoint p( x, y + height );
	QPoint gp = mapToGlobal( p );
	qwsDisplay()->setMicroFocus( gp.x(), gp.y());
    }
#endif
}


void QWidget::setFontSys( QFont * )
{
}


void QWidget::setBackgroundColorDirect( const QColor &color )
{
    bg_col = color;
    if ( extra && extra->bg_pix ) {		// kill the background pixmap
	delete extra->bg_pix;
	extra->bg_pix = 0;
    }
    // XXX XSetWindowBackground( x11Display(), winId(), bg_col.pixel() );
}

static int allow_null_pixmaps = 0;


void QWidget::setBackgroundPixmapDirect( const QPixmap &pixmap )
{
    QPixmap old;
    if ( extra && extra->bg_pix )
	old = *extra->bg_pix;
    if ( !allow_null_pixmaps && pixmap.isNull() ) {
	// XXX XSetWindowBackground( x11Display(), winId(), bg_col.pixel() );
	if ( extra && extra->bg_pix ) {
	    delete extra->bg_pix;
	    extra->bg_pix = 0;
	}
    } else {
	QPixmap pm = pixmap;
	if (!pm.isNull()) {
	    if ( pm.depth() == 1 && QPixmap::defaultDepth() > 1 ) {
		pm = QPixmap( pixmap.size() );
		bitBlt( &pm, 0, 0, &pixmap, 0, 0, pm.width(), pm.height() );
	    }
	}
	if ( extra && extra->bg_pix )
	    delete extra->bg_pix;
	else
	    createExtra();
	extra->bg_pix = new QPixmap( pm );
	// XXX XSetWindowBackgroundPixmap( x11Display(), winId(), pm.handle() );
    }
}


void QWidget::setBackgroundEmpty()
{
    allow_null_pixmaps++;
    setErasePixmap(QPixmap());
    allow_null_pixmaps--;
}

#ifndef QT_NO_CURSOR

void QWidget::setCursor( const QCursor &cursor )
{
    createExtra();
    delete extra->curs;
    extra->curs = new QCursor(cursor);
    setWState( WState_OwnCursor );
    if ( isVisible() )
	updateCursor( paintableRegion() );
}

void QWidget::unsetCursor()
{
    if ( extra ) {
	delete extra->curs;
	extra->curs = 0;
    }
    clearWState( WState_OwnCursor );
    if ( isVisible() )
	updateCursor( paintableRegion() );
}
#endif //QT_NO_CURSOR

#ifndef QT_NO_WIDGET_TOPEXTRA
void QWidget::setCaption( const QString &caption )
{
    if ( extra && extra->topextra && extra->topextra->caption == caption )
	return; // for less flicker
    createTLExtra();
    extra->topextra->caption = caption;
    qwsDisplay()->setCaption(this, caption);
    QEvent e( QEvent::CaptionChange );
    QApplication::sendEvent( this, &e );
}

void QWidget::setIcon( const QPixmap &unscaledPixmap )
{
    if ( extra && extra->topextra ) {
	delete extra->topextra->icon;
	extra->topextra->icon = 0;
    } else {
	createTLExtra();
    }
    QBitmap mask;
    if ( unscaledPixmap.isNull() ) {
    } else {
	QImage unscaledIcon = unscaledPixmap.convertToImage();
	QPixmap pixmap;
#ifndef QT_NO_IMAGE_SMOOTHSCALE
	pixmap.convertFromImage( unscaledIcon.smoothScale( 16, 16 ) );
#else
	pixmap.convertFromImage( unscaledIcon );
#endif
	extra->topextra->icon = new QPixmap( pixmap );
	mask = pixmap.mask() ? *pixmap.mask() : pixmap.createHeuristicMask();
    }
    // XXX
}


void QWidget::setIconText( const QString &iconText )
{
    createTLExtra();
    extra->topextra->iconText = iconText;
    // XXX XSetIconName( x11Display(), winId(), iconText.utf8() );
    // XXX XSetWMIconName( x11Display(), winId(), qstring_to_xtp(iconText) );
}
#endif

void QWidget::grabMouse()
{
    if ( qt_mouseGrb )
	qt_mouseGrb->releaseMouse();

    qwsDisplay()->grabMouse(this,TRUE);

    qt_mouseGrb = this;
    qt_pressGrab = 0;
}

#ifndef QT_NO_CURSOR
void QWidget::grabMouse( const QCursor &cursor )
{
    if ( qt_mouseGrb )
	qt_mouseGrb->releaseMouse();

    qwsDisplay()->grabMouse(this,TRUE);
    qwsDisplay()->selectCursor(this, (unsigned int)cursor.handle());
    qt_mouseGrb = this;
    qt_pressGrab = 0;
}
#endif

void QWidget::releaseMouse()
{
    if ( qt_mouseGrb == this ) {
	qwsDisplay()->grabMouse(this,FALSE);
	qt_mouseGrb = 0;
    }
}

void QWidget::grabKeyboard()
{
    if ( keyboardGrb )
	keyboardGrb->releaseKeyboard();
    // XXX XGrabKeyboard( x11Display(), winid, TRUE, GrabModeAsync, GrabModeAsync, CurrentTime );
    qwsDisplay()->grabKeyboard(this, TRUE);
    keyboardGrb = this;
}

void QWidget::releaseKeyboard()
{
    if ( keyboardGrb == this ) {
	// XXX XUngrabKeyboard( x11Display(), CurrentTime );
	qwsDisplay()->grabKeyboard(this, FALSE);
	keyboardGrb = 0;
    }
}


QWidget *QWidget::mouseGrabber()
{
    if ( qt_mouseGrb )
	return qt_mouseGrb;
    return qt_pressGrab;
}


QWidget *QWidget::keyboardGrabber()
{
    return keyboardGrb;
}

void QWidget::setActiveWindow()
{
    QWidget *tlw = topLevelWidget();
    if ( tlw->isVisible() ) {
	qwsDisplay()->requestFocus( tlw->winId(), TRUE);
    }
}


void QWidget::update()
{
    //if ( (widget_state & (WState_Visible|WState_BlockUpdates)) ==
    //WState_Visible && isVisible() )
    //QApplication::postEvent( this, new QPaintEvent( rect() ) );
    update(0,0,width(),height());
}

void QWidget::update( int x, int y, int w, int h )
{
    if ( w && h &&
         (widget_state & (WState_Visible|WState_BlockUpdates)) == WState_Visible && isVisible() ) {
	if ( w < 0 )
	    w = crect.width()  - x;
	if ( h < 0 )
	    h = crect.height() - y;
	QApplication::postEvent(this,new QPaintEvent(QRect(x,y,w,h),
			   !testWFlags(WRepaintNoErase) ) );
	//erase will be done in QApplication::sendPostedEvents(), if necessary
    }
}

void QWidget::repaint( int x, int y, int w, int h, bool erase )
{
    if ( (widget_state & (WState_Visible|WState_BlockUpdates)) == WState_Visible && isVisible() ) {
	if ( w < 0 )
	    w = crect.width()  - x;
	if ( h < 0 )
	    h = crect.height() - y;
	QRect r(x,y,w,h);
	if ( r.isEmpty() )
	    return; // nothing to do
	if ( erase )
	    this->erase(x,y,w,h);
	QPaintEvent e( r, erase );
	qt_set_paintevent_clipping( this, r );
	QApplication::sendEvent( this, &e );
	qt_clear_paintevent_clipping();
    }
}

void QWidget::repaint( const QRegion& reg, bool erase )
{
    if ( (widget_state & (WState_Visible|WState_BlockUpdates)) == WState_Visible && isVisible() ) {
	if ( erase )
	    this->erase(reg);
	QPaintEvent e( reg, erase );
	qt_set_paintevent_clipping( this, reg );
	QApplication::sendEvent( this, &e );
	qt_clear_paintevent_clipping();
    }
}

void QWidget::showWindow()
{
    if ( testWFlags(WType_TopLevel) ) {
	updateRequestedRegion( mapToGlobal(QPoint(0,0)) );
	QRegion r( req_region );
#ifndef QT_NO_QWS_MANAGER
	if ( extra && extra->topextra && extra->topextra->qwsManager ) {
	    QRegion wmr = extra->topextra->qwsManager->region();
	    wmr = qt_screen->mapToDevice( wmr, QSize(qt_screen->width(), qt_screen->height()) );
	    r += wmr;
	}
#endif
	qwsDisplay()->requestRegion(winId(), r);
	if ( !testWFlags(WStyle_Tool) ) {
	    qwsDisplay()->requestFocus(winId(),TRUE);
	}
	qwsDisplay()->setAltitude( winId(),
		testWFlags(WStyle_StaysOnTop) ? 1 : 0, TRUE );

    } else if ( !topLevelWidget()->in_show ) {
	updateRequestedRegion( mapToGlobal(QPoint(0,0)) );
	QWidget *p = parentWidget();
	p->setChildrenAllocatedDirty( geometry(), this );
	p->paintable_region_dirty = TRUE;
	p->overlapping_children = -1;
	paint_heirarchy( this, TRUE );
    }
}


void QWidget::hideWindow()
{
    deactivateWidgetCleanup();

    if (req_region.isEmpty())	// Already invisible?
	return;

    if ( testWFlags(WType_TopLevel) ) {
	releaseMouse();
#ifndef QT_NO_QWS_MANAGER
	if (QWSManager::grabbedMouse() == this) {
	    if ( extra && extra->topextra && extra->topextra->qwsManager ) {
		extra->topextra->qwsManager->releaseGrab();
	    }
	}
#endif
	qwsDisplay()->requestRegion(winId(), QRegion());
	qwsDisplay()->requestFocus(winId(),FALSE);
    } else {
	QWidget *p = parentWidget();
	if ( p ) {
	    p->setChildrenAllocatedDirty( geometry(), this );
	    p->paintable_region_dirty = TRUE;
	    if ( p->overlapping_children )
		p->overlapping_children = -1;
	    if ( p->isVisible() ) {
		QApplication::postEvent( p, new QPaintEvent(geometry(), TRUE) );
		paint_children( p,geometry(),TRUE );
	    }
	}
    }
    updateRequestedRegion( mapToGlobal(QPoint(0,0)) );
}


static uint effectiveState(uint state)
{
    if ( state & Qt::WState_Minimized )
	return Qt::WState_Minimized;
    else if ( state & Qt::WState_FullScreen )
	return Qt::WState_FullScreen;
    if ( state & Qt::WState_Maximized )
	return Qt::WState_Maximized;

    return 0;
}

void QWidget::setWindowState(uint newstate)
{
    in_show_maximized = 1;
    bool resized = testWFlags( WState_Resized );
    uint oldstate = effectiveState(widget_state);

    widget_state &= ~(WState_Minimized | WState_Maximized | WState_FullScreen);
    if (newstate & WindowMinimized)
	widget_state |= WState_Minimized;
    if (newstate & WindowMaximized)
	widget_state |= WState_Maximized;
    if (newstate & WindowFullScreen)
	widget_state |= WState_FullScreen;

    uint state = effectiveState(widget_state);

    bool needShow = FALSE;
    if (isTopLevel() && state != oldstate) {
	createTLExtra();
	if ( oldstate == 0 ) { //normal
	    topData()->normalGeometry = geometry();
	} else if ( oldstate == WState_FullScreen ) {
	    reparent( 0, topData()->savedFlags, QPoint(0,0) );
	    needShow = isVisible();
	} else if ( oldstate == WState_Minimized ) {
	    needShow = TRUE;
	}

	if ( state == WState_Minimized ) {
	    //### not ideal...
	    hide();
	    needShow = FALSE;
	} else if ( state == WState_FullScreen ) {
	    needShow = needShow || isVisible();
	    topData()->savedFlags = getWFlags();
	    reparent( 0, WType_TopLevel | WStyle_Customize | WStyle_NoBorder |
		      // preserve some widget flags
		      (getWFlags() & 0xffff0000),
		      QPoint( 0, 0));
	    const QRect screen = qApp->desktop()->screenGeometry( qApp->desktop()->screenNumber( this ) );
	    move( screen.topLeft() );
	    resize( screen.size() );
	    if ( !resized )
		clearWState( WState_Resized );
	    raise();
	} else if ( state == WState_Maximized ) {
#ifndef QT_NO_QWS_MANAGER
	    if ( extra && extra->topextra && extra->topextra->qwsManager )
		extra->topextra->qwsManager->maximize();
	    else
#endif
		setGeometry( qt_maxWindowRect );
	    if ( !resized )
		clearWState( WState_Resized );
	} else { //normal
	    QRect r = topData()->normalGeometry;
	    if ( r.width() >= 0 ) {
		topData()->normalGeometry = QRect(0,0,-1,-1);
		setGeometry( r );
	    }
	}
    }

    in_show_maximized = 0;

    if (needShow)
	show();

    if (newstate & WindowActive)
	setActiveWindow();

    QEvent e(QEvent::WindowStateChange);
    QApplication::sendEvent(this, &e);
}

void QWidget::raise()
{
    QWidget *p = parentWidget();
    if ( p && p->childObjects && p->childObjects->findRef(this) >= 0 )
	p->childObjects->append( p->childObjects->take() );
    if ( isTopLevel() ) {
#ifdef QT_NO_WINDOWGROUPHINT
	if ( !testWFlags( WStyle_Tool ) )
	    setActiveWindow();
	qwsDisplay()->setAltitude( winId(), 0 );
#else
	QWidget* act=0;
	if ( !testWFlags( WStyle_Tool ) )
	    act=this;
	qwsDisplay()->setAltitude( winId(), 0 );
	if ( childObjects ) {
	    QObjectListIt it(*childObjects);
	    QObject* o;
	    QWidgetList toraise;
	    QWidget* w;
	    while ((o=it.current())) {
		if ( o->isWidgetType() ) {
		    w = (QWidget*)o;
		    if ( w->isTopLevel() )
			toraise.append(w);
		}
		++it;
	    }
	    QWidgetListIt wit(toraise);
	    while ((w=wit.current())) {
		if ( w->isVisible() ) {
		    bool wastool = w->testWFlags( WStyle_Tool );
		    w->setWFlags( WStyle_Tool ); // avoid setActiveWindow flicker
		    w->raise();
		    if ( !wastool ) {
			w->clearWFlags( WStyle_Tool );
			act = w;
		    }
		}
		++wit;
	    }
	}
	if ( act )
	    act->setActiveWindow();
#endif // QT_NO_WINDOWGROUPHINT
    } else if ( p ) {
	p->setChildrenAllocatedDirty( geometry(), this );
	paint_heirarchy( this, TRUE );
    }
}

void QWidget::lower()
{
    QWidget *p = parentWidget();
    if ( p && p->childObjects && p->childObjects->findRef(this) >= 0 )
	p->childObjects->insert( 0, p->childObjects->take() );
    if ( isTopLevel() )
	qwsDisplay()->setAltitude( winId(), -1 );
    else if ( p ) {
	p->setChildrenAllocatedDirty( geometry() );
	paint_children( p,geometry(),TRUE );
    }
}

void QWidget::stackUnder( QWidget* w)
{
    QWidget *p = parentWidget();
    if ( !p || !w || isTopLevel() || p != w->parentWidget() )
	return;
    int loc = p->childObjects->findRef(w);
    if ( loc >= 0 && p->childObjects && p->childObjects->findRef(this) >= 0 )
	p->childObjects->insert( loc, p->childObjects->take() );
    if ( p ) {
	// #### excessive repaints
	p->setChildrenAllocatedDirty();
	paint_children( p,geometry(),TRUE );
	paint_children( p,w->geometry(),TRUE );
    }
}

/* debug
void qt_clearRegion( QWidget *w, const QRegion &r, const QColor &c, bool dev )
{
    if ( !r.isEmpty() ) {
	QGfx *gfx=w->graphicsContext( FALSE );
	gfx->setBrush( QBrush(c) );
	QSize s( qt_screen->deviceWidth(), qt_screen->deviceHeight() );
	QArray<QRect> a = r.rects();
	for ( int i = 0; i < (int)a.count(); i++ ) {
	    QRect r = a[i];
	    if ( dev )
		r = qt_screen->mapFromDevice( r, s );
	    gfx->fillRect( r.x(), r.y(), r.width(), r.height() );
	}
	delete gfx;
    }
}
*/

void QWidget::internalSetGeometry( int x, int y, int w, int h, bool isMove )
{
    if ( extra ) {				// any size restrictions?
	w = QMIN(w,extra->maxw);
	h = QMIN(h,extra->maxh);
	w = QMAX(w,extra->minw);
	h = QMAX(h,extra->minh);
    }
    if ( w < 1 )				// invalid size
	w = 1;
    if ( h < 1 )
	h = 1;

    QPoint oldp = geometry().topLeft();
    QSize olds = size();
    QRect r( x, y, w, h );

    bool isResize = olds != r.size();

    // We only care about stuff that changes the geometry, or may
    // cause the window manager to change its state
    if ( r.size() == olds && oldp == r.topLeft() )
	return;

    QRegion oldAlloc;
    if ( !isTopLevel() && isMove && ( w==olds.width() && h==olds.height() ) ) {
	oldAlloc = allocatedRegion();
    }

    if (!in_show_maximized) {
        clearWState(WState_Maximized);
        clearWState(WState_FullScreen);
        if (isTopLevel())
            topData()->normalGeometry = QRect(0, 0, -1, -1);
    }
    QPoint oldPos = pos();
    crect = r;

    if ( testWFlags(WType_Desktop) )
	return;

    if ( isTopLevel() ) {
	//### ConfigPending not implemented, do we need it?
	//setWState( WState_ConfigPending );
	if ( isMove && ( w==olds.width() && h==olds.height() ) ) {
	    // just need to translate current region
	    QSize s( qt_screen->width(), qt_screen->height() );
	    QPoint td1 = qt_screen->mapToDevice( QPoint(0,0), s );
	    QPoint td2 = qt_screen->mapToDevice( QPoint(x - oldp.x(),y - oldp.y()), s );
	    QPoint dd = QPoint( td2.x()-td1.x(), td2.y()-td1.y() );
	    req_region.translate( dd.x(), dd.y() );
	} else {
	    if ( extra && !extra->mask.isNull() ) {
		req_region = extra->mask;
		req_region.translate(crect.x(),crect.y());
		req_region &= crect; //??? this is optional
	    } else {
		req_region = crect;
	    }
	    req_region = qt_screen->mapToDevice( req_region, QSize(qt_screen->width(), qt_screen->height()) );
	}
	if ( isVisible() ) {
	    if ( isMove && !isResize && alloc_region_index >= 0 ) {
		qwsDisplay()->moveRegion(winId(), x - oldp.x(), y - oldp.y());
		setChildrenAllocatedDirty();
	    } else {
		QRegion rgn( req_region );
#ifndef QT_NO_QWS_MANAGER
		if ( extra && extra->topextra && extra->topextra->qwsManager ) {
		    QRegion wmr = extra->topextra->qwsManager->region();
		    wmr = qt_screen->mapToDevice( wmr, QSize(qt_screen->width(), qt_screen->height()) );
		    rgn += wmr;
		}
#endif
		qwsDisplay()->requestRegion(winId(), rgn);
		if ( extra && extra->topextra ) {
		    QRect br( rgn.boundingRect() );
		    br = qt_screen->mapFromDevice( br, QSize(qt_screen->deviceWidth(), qt_screen->deviceHeight()) );
		    extra->topextra->fleft = crect.x()-br.x();
		    extra->topextra->ftop = crect.y()-br.y();
		    extra->topextra->fright = br.right()-crect.right();
		    extra->topextra->fbottom = br.bottom()-crect.bottom();
		}
	    }
	}
    }

    if ( isVisible() ) {
	isSettingGeometry = TRUE;
	if ( isMove ) {
	    QMoveEvent e( pos(), oldPos );
	    QApplication::sendEvent( this, &e );
#ifndef QT_NO_QWS_MANAGER
	    if (extra && extra->topextra && extra->topextra->qwsManager)
		QApplication::sendEvent( extra->topextra->qwsManager, &e );
#endif
	}
	if ( isResize ) {
	    QResizeEvent e( r.size(), olds );
	    QApplication::sendEvent( this, &e );
#ifndef QT_NO_QWS_MANAGER
	    if (extra && extra->topextra && extra->topextra->qwsManager) {
		QResizeEvent e( r.size(), olds );
		QApplication::sendEvent(topData()->qwsManager, &e);
	    }
#endif
/*
	    if ( !testWFlags( WStaticContents ) ) {
		QApplication::postEvent(this,new QPaintEvent(clipRegion(),
					!testWFlags(WResizeNoErase) ) );
	    }
*/
	}

	updateRequestedRegion( mapToGlobal(QPoint(0,0)) );

	QWidget *p = parentWidget();
	if ( !isTopLevel() || isResize ) {
	    if ( p && !isTopLevel() ) {
		p->paintable_region_dirty = TRUE;
		QRegion oldr( QRect(oldp, olds) );
		dirtyChildren.translate( x, y );
		if ( p->isSettingGeometry ) {
		    if ( oldp != r.topLeft() ) {
			QRegion upd( (QRegion(r) | oldr) & p->rect() );
			dirtyChildren |= upd;
		    } else {
			dirtyChildren |= QRegion(r) - oldr;
			QApplication::postEvent( this, new QPaintEvent(rect(),
			    !testWFlags(QWidget::WResizeNoErase)) );
		    }
		    p->dirtyChildren |= dirtyChildren;
		} else {
		    QRegion upd( (QRegion(r) | oldr) & p->rect() );
		    dirtyChildren |= upd;
		    QRegion paintRegion = dirtyChildren;
#define FAST_WIDGET_MOVE
#ifdef FAST_WIDGET_MOVE
		    if ( isMove && ( w==olds.width() && h==olds.height() ) ) {
			QSize s( qt_screen->width(), qt_screen->height() );

			QPoint td1 = qt_screen->mapToDevice( QPoint(0,0), s );
			QPoint td2 = qt_screen->mapToDevice( QPoint(x - oldp.x(),y - oldp.y()), s );
			QPoint dd = QPoint( td2.x()-td1.x(), td2.y()-td1.y() );
			oldAlloc.translate( dd.x(), dd.y() );

			QRegion alloc( allocatedRegion() );

			QRegion scrollRegion( alloc & oldAlloc );
			if ( !scrollRegion.isEmpty() ) {
			    QGfx * gfx = p->graphicsContext(FALSE);
			    gfx->setClipDeviceRegion( scrollRegion );
			    gfx->scroll(x,y,w,h,oldp.x(),oldp.y());
			    delete gfx;

			    QSize ds( qt_screen->deviceWidth(), qt_screen->deviceHeight() );
			    scrollRegion = qt_screen->mapFromDevice( scrollRegion, ds );
			    QPoint gp = p->mapToGlobal( QPoint(0,0) );
			    scrollRegion.translate( -gp.x(), -gp.y() );
			    paintRegion -= scrollRegion;
			}
		    }
#endif
		    if ( !oldr.isEmpty() )
			QApplication::postEvent( p, new QPaintEvent(oldr, TRUE) );
		    p->setChildrenAllocatedDirty( dirtyChildren, this );
		    qwsUpdateActivePainters();
		    paint_children( p, paintRegion, isResize );
		}
		p->overlapping_children = -1;
	    } else {
		if ( oldp != r.topLeft() ) {
		    qwsUpdateActivePainters();
		    paint_heirarchy( this, TRUE );
		} else {
		    setChildrenAllocatedDirty( dirtyChildren );
		    qwsUpdateActivePainters();
		    QApplication::postEvent( this, new QPaintEvent(rect(),
			!testWFlags(QWidget::WResizeNoErase)) );
		    paint_children( this, dirtyChildren, TRUE );
		}
	    }
	} else {
	    qwsUpdateActivePainters();
	}
#ifndef QT_NO_QWS_MANAGER
	if (isResize && extra && extra->topextra && extra->topextra->qwsManager) {
	    QApplication::postEvent(topData()->qwsManager,
				    new QPaintEvent( clipRegion(), TRUE ) );
	}
#endif
	isSettingGeometry = FALSE;
	dirtyChildren = QRegion();
    } else {
	if ( isMove )
	    QApplication::postEvent( this,
				     new QMoveEvent( pos(), oldPos ) );
	if ( isResize )
	    QApplication::postEvent( this,
				     new QResizeEvent(crect.size(), olds) );
    }
}


void QWidget::setMinimumSize( int minw, int minh )
{
    if ( !qt_maxWindowRect.isEmpty() ) {
	// This is really just a work-around. Layout shouldn't be asking
	// for minimum sizes bigger than the screen.
	if ( minw > qt_maxWindowRect.width() )
	    minw = qt_maxWindowRect.width();
	if ( minh > qt_maxWindowRect.height() )
	    minh = qt_maxWindowRect.height();
    }

#if defined(QT_CHECK_RANGE)
    if ( minw < 0 || minh < 0 )
	qWarning("QWidget::setMinimumSize: The smallest allowed size is (0,0)");
#endif
    createExtra();
    if ( extra->minw == minw && extra->minh == minh )
	return;
    extra->minw = minw;
    extra->minh = minh;
    if ( minw > width() || minh > height() )
	resize( QMAX(minw,width()), QMAX(minh,height()) );
    if ( testWFlags(WType_TopLevel) ) {
	// XXX
    }
    updateGeometry();
}

void QWidget::setMaximumSize( int maxw, int maxh )
{
#if defined(QT_CHECK_RANGE)
    if ( maxw > QWIDGETSIZE_MAX || maxh > QWIDGETSIZE_MAX ) {
	qWarning("QWidget::setMaximumSize: (%s/%s) "
		"The largest allowed size is (%d,%d)",
		 name( "unnamed" ), className(), QWIDGETSIZE_MAX,
		QWIDGETSIZE_MAX );
	maxw = QMIN( maxw, QWIDGETSIZE_MAX );
	maxh = QMIN( maxh, QWIDGETSIZE_MAX );
    }
    if ( maxw < 0 || maxh < 0 ) {
	qWarning("QWidget::setMaximumSize: (%s/%s) Negative sizes (%d,%d) "
		"are not possible",
		name( "unnamed" ), className(), maxw, maxh );
	maxw = QMAX( maxw, 0 );
	maxh = QMAX( maxh, 0 );
    }
#endif
    createExtra();
    if ( extra->maxw == maxw && extra->maxh == maxh )
	return;
    extra->maxw = maxw;
    extra->maxh = maxh;
    if ( maxw < width() || maxh < height() )
	resize( QMIN(maxw,width()), QMIN(maxh,height()) );
    if ( testWFlags(WType_TopLevel) ) {
	// XXX
    }
    updateGeometry();
}

void QWidget::setSizeIncrement( int w, int h )
{
    createTLExtra();
    QTLWExtra* x = extra->topextra;
    if ( x->incw == w && x->inch == h )
	return;
    x->incw = w;
    x->inch = h;
    if ( testWFlags(WType_TopLevel) ) {
	// XXX ...
    }
}

void QWidget::setBaseSize( int basew, int baseh )
{
    createTLExtra();
    QTLWExtra* x = extra->topextra;
    if ( x->basew == basew && x->baseh == baseh )
	return;
    x->basew = basew;
    x->baseh = baseh;
    if ( testWFlags(WType_TopLevel) ) {
	// XXX
    }
}

void QWidget::erase( int x, int y, int w, int h )
{
    if ( backgroundMode() == NoBackground || !isVisible() )
	return;

    erase( QRegion( x, y, w, h ) );

}

void QWidget::erase( const QRegion& reg )
{
    //this is experimental, I need to test more, but it seems in unclipped mode erasing shouldn't happen????
    if ( backgroundMode() == NoBackground || testWFlags(WPaintUnclipped) )
	return;

    bool unclipped = testWFlags( WPaintUnclipped );
    clearWFlags( WPaintUnclipped );
    QPainter p(this);
    p.setClipRegion( reg );
    if ( extra && extra->bg_pix ) {
	if ( !extra->bg_pix->isNull() ) {
	    QPoint offset = backgroundOffset();
	    int xoff = offset.x();
	    int yoff = offset.y();

	    p.drawTiledPixmap(rect(),*extra->bg_pix,
			      QPoint(xoff%extra->bg_pix->width(),
				     yoff%extra->bg_pix->height()));
	}
    } else {
	p.fillRect(rect(),bg_col);
    }
    if ( unclipped )
	setWFlags( WPaintUnclipped );
}

void QWidget::scroll( int dx, int dy )
{
    scroll( dx, dy, QRect() );
}

void QWidget::scroll( int dx, int dy, const QRect& r )
{
    if ( testWState( WState_BlockUpdates ) && !children() )
	return;
    bool valid_rect = r.isValid();
    QRect sr = valid_rect?r:rect();
    int x1, y1, x2, y2, w=sr.width(), h=sr.height();
    if ( dx > 0 ) {
	x1 = sr.x();
	x2 = x1+dx;
	w -= dx;
    } else {
	x2 = sr.x();
	x1 = x2-dx;
	w += dx;
    }
    if ( dy > 0 ) {
	y1 = sr.y();
	y2 = y1+dy;
	h -= dy;
    } else {
	y2 = sr.y();
	y1 = y2-dy;
	h += dy;
    }

    if ( dx == 0 && dy == 0 )
	return;

    QSize s( qt_screen->width(), qt_screen->height() );
    QRegion alloc = valid_rect ? paintableRegion() : allocatedRegion();

    QRegion dAlloc = alloc;
    QPoint td1 = qt_screen->mapToDevice( QPoint(0,0), s );
    QPoint td2 = qt_screen->mapToDevice( QPoint(dx,dy), s );
    dAlloc.translate( td2.x()-td1.x(), td2.y()-td1.y() );

    QRegion scrollRegion( alloc & dAlloc );

    if ( w > 0 && h > 0 ) {
	QGfx * mygfx=graphicsContext( FALSE );
	mygfx->setClipDeviceRegion( scrollRegion );
	mygfx->scroll(x2,y2,w,h,x1,y1);
	delete mygfx;
    }

    paintable_region_dirty = TRUE;

    QPoint gpos = mapToGlobal( QPoint() );

    if ( !valid_rect && children() ) {	// scroll children
	setChildrenAllocatedDirty();
	QPoint pd( dx, dy );
	QObjectListIt it(*children());
	register QObject *object;
	while ( it ) {				// move all children
	    object = it.current();
	    if ( object->isWidgetType() ) {
		QWidget *w = (QWidget *)object;
		QPoint oldp = w->pos();
		QRect  r( w->pos() + pd, w->size() );
		w->crect = r;
		w->updateRequestedRegion( gpos + w->pos() );
		QMoveEvent e( r.topLeft(), oldp );
		QApplication::sendEvent( w, &e );
	    }
	    ++it;
	}
    }

    QSize ds( qt_screen->deviceWidth(), qt_screen->deviceHeight() );
    scrollRegion = qt_screen->mapFromDevice( scrollRegion, ds );
    scrollRegion.translate( -gpos.x(), -gpos.y() );

    QRegion update( sr );
    update -= scrollRegion;
    if ( dx ) {
	int x = x2 == sr.x() ? sr.x()+w : sr.x();
	update |= QRect( x, sr.y(), QABS(dx), sr.height() );
    }
    if ( dy ) {
	int y = y2 == sr.y() ? sr.y()+h : sr.y();
	update |= QRect( sr.x(), y, sr.width(), QABS(dy) );
    }
    repaint( update, !testWFlags(WRepaintNoErase) );
    if ( !valid_rect && children() )
	paint_children( this, update, FALSE );
}


void QWidget::drawText( int x, int y, const QString &str )
{
    if ( (widget_state & (WState_Visible|WState_BlockUpdates)) == WState_Visible && isVisible() ) {
	QPainter paint;
	paint.begin( this );
	paint.drawText( x, y, str );
	paint.end();
    }
}


int QWidget::metric( int m ) const
{
    int val;
    if ( m == QPaintDeviceMetrics::PdmWidth ) {
	val = crect.width();
    } else if ( m == QPaintDeviceMetrics::PdmWidthMM ) {
	// 75 dpi is 3dpmm
	val = (crect.width()*100)/288;
    } else if ( m == QPaintDeviceMetrics::PdmHeight ) {
	val = crect.height();
    } else if ( m == QPaintDeviceMetrics::PdmHeightMM ) {
	val = (crect.height()*100)/288;
    } else if ( m == QPaintDeviceMetrics::PdmDepth ) {
	return qwsDisplay()->depth();
    } else if ( m == QPaintDeviceMetrics::PdmDpiX || m == QPaintDeviceMetrics::PdmPhysicalDpiX ) {
	return 72;
    } else if ( m == QPaintDeviceMetrics::PdmDpiY || m == QPaintDeviceMetrics::PdmPhysicalDpiY ) {
	return 72;
    } else {
	val = QPaintDevice::metric(m);// XXX
    }
    return val;
}

void QWidget::createSysExtra()
{
}

void QWidget::deleteSysExtra()
{
}

void QWidget::createTLSysExtra()
{
}

void QWidget::deleteTLSysExtra()
{
}


bool QWidget::acceptDrops() const
{
    return testWState(WState_DND);
}

void QWidget::setAcceptDrops( bool on )
{
    if ( testWState(WState_DND) != on ) {
	if ( 1/*XXX qt_xdnd_enable( this, on )*/ ) {
	    if ( on )
		setWState(WState_DND);
	    else
		clearWState(WState_DND);
	}
    }
}

void QWidget::updateOverlappingChildren() const
{
    if ( overlapping_children != -1 || isSettingGeometry )
	return;

    QRegion r;
    const QObjectList *c = children();
    if ( c ) {
	QObjectListIt it(*c);
	QObject* ch;
	while ((ch=it.current())) {
	    ++it;
	    if ( ch->isWidgetType() && !((QWidget*)ch)->isTopLevel() ) {
		QWidget *w = (QWidget *)ch;
		if ( w->isVisible() ) {
		    QRegion rr( w->req_region );
		    QRegion ir = r & rr;
		    if ( !ir.isEmpty() ) {
			overlapping_children = 1;
			return;
		    }
		    r |= rr;
		}
	    }
	}
    }
    overlapping_children = 0;
}

void QWidget::updateRequestedRegion( const QPoint &gpos )
{
    if ( !isTopLevel() ) {
	if ( !testWState( WState_Visible ) || testWState(WState_ForceHide) ) {
	    req_region = QRegion();
	} else {
	    req_region = QRect(gpos,crect.size());
	    if ( extra && !extra->mask.isNull() ) {
		QRegion maskr = extra->mask;
		maskr.translate( gpos.x(), gpos.y() );
		req_region &= maskr;
	    }
	    req_region = qt_screen->mapToDevice( req_region, QSize(qt_screen->width(), qt_screen->height()) );
	}
    }
    const QObjectList *c = children();
    if ( c ) {
	QObjectListIt it(*c);
	QObject* ch;
	while ((ch=it.current())) {
	    ++it;
	    if ( ch->isWidgetType() && !((QWidget*)ch)->isTopLevel() ) {
		QWidget *w = (QWidget *)ch;
		w->updateRequestedRegion( gpos + w->pos() );
	    }
	}
    }
}

QRegion QWidget::requestedRegion() const
{
    return req_region;
}

void QWidget::setChildrenAllocatedDirty()
{
    const QObjectList *c = children();
    if ( c ) {
	QObjectListIt it(*c);
	QObject* ch;
	while ((ch=it.current())) {
	    ++it;
	    if ( ch->isWidgetType() ) {
		((QWidget *)ch)->alloc_region_dirty = TRUE;
	    }
	}
    }
}

void QWidget::setChildrenAllocatedDirty( const QRegion &r, const QWidget *dirty )
{
    const QObjectList *c = children();
    if ( c ) {
	QObjectListIt it(*c);
	QObject* ch;
	while ((ch=it.current())) {
	    ++it;
	    if ( ch->isWidgetType() ) {
		QWidget *w = (QWidget *)ch;
		if ( r.boundingRect().intersects( w->geometry() ) )
		    w->alloc_region_dirty = TRUE;
		if ( w == dirty )
		    break;
	    }
	}
    }
}

// check my hierarchy for dirty allocated regions
bool QWidget::isAllocatedRegionDirty() const
{
    if ( isTopLevel() )
	return FALSE;

    if ( alloc_region_dirty )
	return TRUE;

    return parentWidget()->isAllocatedRegionDirty();
}

inline bool QRect::intersects( const QRect &r ) const
{
    return ( QMAX( x1, r.x1 ) <= QMIN( x2, r.x2 ) &&
	     QMAX( y1, r.y1 ) <= QMIN( y2, r.y2 ) );
}

QRegion QWidget::allocatedRegion() const
{
    if (isVisible()) {
	if ( isTopLevel() ) {
	    return alloc_region;
	} else {
	    if ( isAllocatedRegionDirty() ) {
		const QObjectList *c;
		QRegion r( req_region );
		r &= parentWidget()->allocatedRegion();
		parentWidget()->updateOverlappingChildren();
		if ( parentWidget()->overlapping_children ) {
		    c = parentWidget()->children();
		    if ( c ) {
			QObjectListIt it(*c);
			QObject* ch;
			bool clip=FALSE;
			while ((ch=it.current())) {
			    ++it;
			    if ( ch->isWidgetType() ) {
				QWidget *w = (QWidget*)ch;
				if ( w == this )
				    clip=TRUE;
				else if ( clip && !w->isTopLevel() && w->isVisible() ) {
				    if ( w->geometry().intersects( geometry() ) )
					r -= w->req_region;
				}
			    }
			}
		    }
		}

		// if I'm dirty, so are my chlidren.
		c = children();
		if ( c ) {
		    QObjectListIt it(*c);
		    QObject* ch;
		    while ((ch=it.current())) {
			++it;
			if ( ch->isWidgetType() && !((QWidget*)ch)->isTopLevel() ) {
			    ((QWidget *)ch)->alloc_region_dirty = TRUE;
			}
		    }
		}

		alloc_region = r;
		alloc_region_dirty = FALSE;
		paintable_region_dirty = TRUE;
	    }
	    return alloc_region;
	}
    } else {
	return QRegion();
    }
}

QRegion QWidget::paintableRegion() const
{
    if (isVisible()) {
	if ( paintable_region_dirty || isAllocatedRegionDirty() ) {
	    paintable_region = allocatedRegion();
	    const QObjectList *c = children();
	    if ( c ) {
		QObjectListIt it(*c);
		QObject* ch;
		while ((ch=it.current())) {
		    ++it;
		    if ( ch->isWidgetType() && !((QWidget*)ch)->isTopLevel() && ((QWidget*)ch)->isVisible() ) {
			paintable_region -= ((QWidget*)ch)->req_region;
		    }
		}
	    }
	    paintable_region_dirty = FALSE;
#ifndef QT_NO_CURSOR
	    // The change in paintable region may have result in the
	    // cursor now being within my region.
	    updateCursor( paintable_region );
#endif
	}
	if ( !isTopLevel() )
	    return paintable_region;
	else {
	    QRegion r( paintable_region );
#ifndef QT_NO_QWS_MANAGER
	    if (extra && extra->topextra)
		r += extra->topextra->decor_allocated_region;
#endif
	    return r;
	}
    }

    return QRegion();
}

void QWidget::setMask( const QRegion& region )
{
    createExtra();

    if ( region == extra->mask )
	return;

    alloc_region_dirty = TRUE;

    extra->mask = region;

    if ( isTopLevel() ) {
	if ( !region.isNull() ) {
	    req_region = extra->mask;
	    req_region.translate(crect.x(),crect.y()); //###expensive?
	    req_region &= crect; //??? this is optional
	} else
	    req_region = QRegion(crect);
	req_region = qt_screen->mapToDevice( req_region, QSize(qt_screen->width(), qt_screen->height()) );
    }
    if ( isVisible() ) {
	if ( isTopLevel() ) {
	    QRegion rgn( req_region );
#ifndef QT_NO_QWS_MANAGER
	    if ( extra && extra->topextra && extra->topextra->qwsManager ) {
		QRegion wmr = extra->topextra->qwsManager->region();
		wmr = qt_screen->mapToDevice( wmr, QSize(qt_screen->width(), qt_screen->height()) );
		rgn += wmr;
	    }
#endif
	    qwsDisplay()->requestRegion(winId(), rgn);
	} else {
	    updateRequestedRegion( mapToGlobal(QPoint(0,0)) );
	    parentWidget()->paintable_region_dirty = TRUE;
	    parentWidget()->repaint(geometry());
	    paint_children( parentWidget(),geometry(),TRUE );
	}
    }
}

void QWidget::setMask( const QBitmap &bitmap )
{
    setMask( QRegion( bitmap ) );
}

void QWidget::clearMask()
{
    setMask( QRegion() );
}

void QWidget::setName( const char *name )
{
    QObject::setName( name );
    if ( isTopLevel() ) {
	// XXX
    }
}

/*!
    \internal
*/
QGfx * QWidget::graphicsContext(bool clip_children) const
{
    QGfx * qgfx_qws;
    qgfx_qws=qwsDisplay()->screenGfx();
    updateGraphicsContext( qgfx_qws, clip_children );

    return qgfx_qws;
}

void QWidget::updateGraphicsContext( QGfx *qgfx_qws, bool clip_children ) const
{
    QPoint offset=mapToGlobal(QPoint(0,0));
    QRegion r; // empty if not visible
    if ( isVisible() && topLevelWidget()->isVisible() ) {
	int rgnIdx = topLevelWidget()->alloc_region_index;
	if ( rgnIdx >= 0 ) {
	    r = clip_children ? paintableRegion() : allocatedRegion();
	    QRegion req;
	    bool changed = FALSE;
	    QWSDisplay::grab();
	    const int *rgnRev = qwsDisplay()->regionManager()->revision( rgnIdx );
	    if ( topLevelWidget()->alloc_region_revision != *rgnRev ) {
		// The TL region has changed, so we better make sure we're
		// not writing to any regions we don't own anymore.
		// We'll get a RegionModified event soon that will get our
		// regions back in sync again.
		req = qwsDisplay()->regionManager()->region( rgnIdx );
		changed = TRUE;
	    }
	    qgfx_qws->setGlobalRegionIndex( rgnIdx );
	    QWSDisplay::ungrab();
	    if ( changed ) {
		r &= req;
	    }
	}
    }
    qgfx_qws->setWidgetDeviceRegion(r);
    qgfx_qws->setOffset(offset.x(),offset.y());
    // Clip the window decoration for TL windows.
    // It is possible for these windows to draw on the wm decoration if
    // they change the clip region.  Bug or feature?
#ifndef QT_NO_QWS_MANAGER
    if ( extra && extra->topextra && extra->topextra->qwsManager )
	qgfx_qws->setClipRegion(rect());
#endif
}

/*!
    \internal
*/
unsigned char * QWidget::scanLine(int i) const
{
    // Should add widget x() here, maybe
    unsigned char * base=qwsDisplay()->frameBuffer();
    if(base)
	base+=i*bytesPerLine();
    return base;
}

/*!
    \internal
*/
int QWidget::bytesPerLine() const
{
    return qt_screen->linestep();
}

void QWidget::resetInputContext()
{
#ifndef QT_NO_QWS_IM
    if (hasFocus())
        QInputContext::reset();
#endif
}

void QWidget::updateFrameStrut() const
{
    QWidget *that = (QWidget *) this;

    if( !isVisible() || isDesktop() ) {
	that->fstrut_dirty = isVisible();
	return;
    }

    //FIXME: need to fill in frame strut info
}

#ifndef QT_NO_CURSOR
void QWidget::updateCursor( const QRegion &r ) const
{
    if ( qt_last_x && (!QWidget::mouseGrabber() || QWidget::mouseGrabber() == this) &&
	    qt_last_cursor != (WId)cursor().handle() && !qws_overrideCursor ) {
	QSize s( qt_screen->width(), qt_screen->height() );
	QPoint pos = qt_screen->mapToDevice(QPoint(*qt_last_x, *qt_last_y), s);
	if ( r.contains(pos) )
	    qwsDisplay()->selectCursor((QWidget*)this, (unsigned int)cursor().handle());
    }
}
#endif

void QWidget::setMouseTracking( bool enable )
{
    if ( enable )
	setWState( WState_MouseTracking );
    else
	clearWState( WState_MouseTracking );
}


void QWidget::setWindowOpacity(double)
{
}

double QWidget::windowOpacity() const
{
    return 1.0;
}
