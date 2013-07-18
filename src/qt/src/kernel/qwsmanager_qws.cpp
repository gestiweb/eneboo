/****************************************************************************
**
** Implementation of Qt/Embedded window manager
**
** Created : 000101
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
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
#include "qwsmanager_qws.h"
#include "qcursor.h"

#ifndef QT_NO_QWS_MANAGER

#include "qdrawutil.h"
#include "qapplication.h"
#include "qaccel.h"
#include "qstyle.h"
#include "qwidget.h"
#include "qpopupmenu.h"
#include "qpainter.h"
#include "qregion.h"
#include "qevent.h"
#include "qcursor.h"
#include "qgfx_qws.h"
#include "qwsdisplay_qws.h"
#include "qwsregionmanager_qws.h"
#include "qwsdefaultdecoration_qws.h"


enum WMStyle {
    Default_WMStyle = 1, /* Starting at zero stuffs up menus */
    KDE_WMStyle,
    KDE2_WMStyle,
    BeOS_WMStyle,
    Windows_WMStyle,
    Hydro_WMStyle,
};


#ifndef QT_NO_QWS_WINDOWS_WM_STYLE
#include "qwswindowsdecoration_qws.h"
QWSDefaultDecoration *new_Windows_WMDecorations() { return new QWSWindowsDecoration(); }
#endif // QT_NO_QWS_WINDOWS_WM_STYLE
#ifndef QT_NO_QWS_KDE_WM_STYLE
#include "qwskdedecoration_qws.h"
QWSDefaultDecoration *new_KDE_WMDecorations() { return new QWSKDEDecoration(); }
#endif // QT_NO_QWS_KDE_WM_STYLE
#ifndef QT_NO_QWS_KDE2_WM_STYLE
#include "qwskde2decoration_qws.h"
QWSDefaultDecoration *new_KDE2_WMDecorations() { return new QWSKDE2Decoration(); }
#endif // QT_NO_QWS_KDE2_WM_STYLE
#ifndef QT_NO_QWS_BEOS_WM_STYLE
#include "qwsbeosdecoration_qws.h"
QWSDefaultDecoration *new_BeOS_WMDecorations() { return new QWSBeOSDecoration(); }
#endif // QT_NO_QWS_BEOS_WM_STYLE
#ifndef QT_NO_QWS_HYDRO_WM_STYLE
#include "qwshydrodecoration_qws.h"
QWSDefaultDecoration *new_Hydro_WMDecorations() { return new QWSHydroDecoration(); }
#endif // QT_NO_QWS_HYDRO_WM_STYLE

#include "qwsdefaultdecoration_qws.h"
QWSDefaultDecoration *new_Default_WMDecorations() { return new QWSDefaultDecoration(); }


struct WMStyleFactoryItem {
	WMStyle WMStyleType;
	QString WMStyleName;
	QWSDefaultDecoration *(*new_WMDecorations)();
} WMStyleList[] = {
#ifndef QT_NO_QWS_WINDOWS_WM_STYLE
    { Windows_WMStyle, QT_TRANSLATE_NOOP("QWSDecoration", "Windows"), new_Windows_WMDecorations },
#endif // QT_NO_QWS_WINDOWS_WM_STYLE
#ifndef QT_NO_QWS_KDE_WM_STYLE
    { KDE_WMStyle, QT_TRANSLATE_NOOP("QWSDecoration", "KDE"), new_KDE_WMDecorations },
#endif // QT_NO_QWS_KDE_WM_STYLE
#ifndef QT_NO_QWS_KDE2_WM_STYLE
    { KDE2_WMStyle, QT_TRANSLATE_NOOP("QWSDecoration", "KDE2"), new_KDE2_WMDecorations },
#endif // QT_NO_QWS_KDE2_WM_STYLE
#ifndef QT_NO_QWS_BEOS_WM_STYLE
    { BeOS_WMStyle, QT_TRANSLATE_NOOP("QWSDecoration", "BeOS"), new_BeOS_WMDecorations },
#endif // QT_NO_QWS_BEOS_WM_STYLE
#ifndef QT_NO_QWS_HYDRO_WM_STYLE
    { Hydro_WMStyle, QT_TRANSLATE_NOOP("QWSDecoration", "Hydro"), new_Hydro_WMDecorations },
#endif // QT_NO_QWS_HYDRO_WM_STYLE

    { Default_WMStyle, QT_TRANSLATE_NOOP("QWSDecoration", "Default"), new_Default_WMDecorations },
    { Default_WMStyle, NULL, NULL }
};


QWSDecoration *QWSManager::newDefaultDecoration()
{
    return new QWSDefaultDecoration;
}


QWidget *QWSManager::active = 0;
QPoint QWSManager::mousePos;

QWSManager::QWSManager(QWidget *w)
    : activeRegion(QWSDecoration::None), managed(w), popup(0)
{
    dx = 0;
    dy = 0;

    menuBtn = new QWSButton(this, QWSDecoration::Menu);
    closeBtn = new QWSButton(this, QWSDecoration::Close);
    minimizeBtn = new QWSButton(this, QWSDecoration::Minimize);
    maximizeBtn = new QWSButton(this, QWSDecoration::Maximize, TRUE);
}

QWSManager::~QWSManager()
{
#ifndef QT_NO_POPUPMENU
    if (popup)
	delete popup;
#endif
    delete menuBtn;
    delete closeBtn;
    delete minimizeBtn;
    delete maximizeBtn;
    if (managed == active)
        active = 0;
}

QRegion QWSManager::region()
{
    return QApplication::qwsDecoration().region(managed, managed->geometry());
}

QWSDecoration::Region QWSManager::pointInRegion(const QPoint &p)
{
    QWSDecoration &dec = QApplication::qwsDecoration();
    QRect rect(managed->geometry());

    for (int i = QWSDecoration::LastRegion; i >= QWSDecoration::Title; i--) {
	if (dec.region(managed, rect, (QWSDecoration::Region)i).contains(p))
	    return (QWSDecoration::Region)i;
    }

    return QWSDecoration::None;
}

bool QWSManager::event(QEvent *e)
{
    if ( QObject::event( e ) )
	return TRUE;

    switch (e->type()) {
	case QEvent::MouseMove:
	    mouseMoveEvent( (QMouseEvent*)e );
            break;

        case QEvent::MouseButtonPress:
            mousePressEvent( (QMouseEvent*)e );
            break;

        case QEvent::MouseButtonRelease:
            mouseReleaseEvent( (QMouseEvent*)e );
            break;

        case QEvent::MouseButtonDblClick:
            mouseDoubleClickEvent( (QMouseEvent*)e );
            break;

	case QEvent::Paint:
	    paintEvent( (QPaintEvent*)e );
            break;

	default:
	    return FALSE;
	    break;
    }

    return TRUE;
}

void QWSManager::mousePressEvent(QMouseEvent *e)
{

    mousePos = e->globalPos();
    dx = 0;
    dy = 0;
    if ( e->button() == Qt::LeftButton ) {
        activeRegion = pointInRegion(mousePos);
        switch (activeRegion) {
        case QWSDecoration::Menu:
            menu(managed->geometry().topLeft());
            break;
        case QWSDecoration::Close:
            closeBtn->setClicked(TRUE);
            break;
        case QWSDecoration::Minimize:
            minimizeBtn->setClicked(TRUE);
            break;
        case QWSDecoration::Maximize:
            maximizeBtn->setClicked(TRUE);
            break;
        default:
            break;
        }
        if ( activeRegion != QWSDecoration::None &&
             activeRegion != QWSDecoration::Menu ) {
            active = managed;
            managed->grabMouse();
        }
        if ( activeRegion != QWSDecoration::None &&
             activeRegion != QWSDecoration::Close &&
             activeRegion != QWSDecoration::Minimize &&
             activeRegion != QWSDecoration::Menu) {
            managed->raise();
        }
    } else if (e->button() == Qt::RightButton) {
	menu(mousePos);
    }
}

void QWSManager::mouseReleaseEvent(QMouseEvent *e)
{
    managed->releaseMouse();
    if (e->button() == Qt::LeftButton) {
	handleMove();
	mousePos = e->globalPos();
	QWSDecoration::Region rgn = pointInRegion(e->globalPos());
	QWSDecoration::Region activatedRegion = activeRegion;
	activeRegion = QWSDecoration::None;
	active = 0;
	switch (activatedRegion) {
	    case QWSDecoration::Close:
		closeBtn->setClicked(FALSE);
		if (rgn == QWSDecoration::Close) {
		    close();
		    return;
		}
		break;
	    case QWSDecoration::Minimize:
		minimizeBtn->setClicked(FALSE);
		if (rgn == QWSDecoration::Minimize)
		    minimize();
		break;
	    case QWSDecoration::Maximize:
		maximizeBtn->setClicked(FALSE);
		if (rgn == QWSDecoration::Maximize)
		    toggleMaximize();
		break;
	    default:
		break;
	}
    } else if (activeRegion == QWSDecoration::None) {
	active = 0;
    }
}

void QWSManager::mouseMoveEvent(QMouseEvent *e)
{
#ifndef QT_NO_CURSOR
    static CursorShape shape[] = { ArrowCursor, ArrowCursor, ArrowCursor,
			    SizeVerCursor, SizeVerCursor, SizeHorCursor,
			    SizeHorCursor, SizeFDiagCursor, SizeBDiagCursor,
			    SizeBDiagCursor, SizeFDiagCursor, ArrowCursor,
			    ArrowCursor, ArrowCursor, ArrowCursor, ArrowCursor};

    // cursor
    QWSDisplay *qwsd = QApplication::desktop()->qwsDisplay();
    if (activeRegion == QWSDecoration::None)
    {
	if ( !QWidget::mouseGrabber() ) {
	    QWSDecoration::Region r = pointInRegion(e->globalPos());
	    qwsd->selectCursor(managed, shape[r]);
	}
    } else
	qwsd->selectCursor(managed, shape[activeRegion]);
#endif //QT_NO_CURSOR
    // resize/move regions

    // don't allow dragging to where the user probably cannot click!
    QPoint g = e->globalPos();
    extern QRect qt_maxWindowRect;
    if ( qt_maxWindowRect.isValid() ) {
	if ( g.x() < qt_maxWindowRect.x() )
	    g.setX(qt_maxWindowRect.x());
	if ( g.y() < qt_maxWindowRect.y() )
	    g.setY(qt_maxWindowRect.y());
	if ( g.x() > qt_maxWindowRect.right() )
	    g.setX(qt_maxWindowRect.right());
	if ( g.y() > qt_maxWindowRect.bottom() )
	    g.setY(qt_maxWindowRect.bottom());
    }

    dx = g.x() - mousePos.x();
    dy = g.y() - mousePos.y();

    handleMove();

    // button regions
    QWSDecoration::Region r = pointInRegion(e->globalPos());
    menuBtn->setMouseOver(r == QWSDecoration::Menu);
    closeBtn->setMouseOver(r == QWSDecoration::Close);
    minimizeBtn->setMouseOver(r == QWSDecoration::Minimize);
    maximizeBtn->setMouseOver(r == QWSDecoration::Maximize);
}

void QWSManager::handleMove()
{
    if (!dx && !dy)
	return;

    if ( managed->isMaximized() )
        return;

    int x = managed->geometry().x();
    int y = managed->geometry().y();
    int w = managed->width();
    int h = managed->height();

    QRect geom(managed->geometry());

    switch (activeRegion) {
	case QWSDecoration::Title:
	    geom = QRect(x + dx, y + dy, w, h);
	    break;
	case QWSDecoration::Top:
	    geom = QRect(x, y + dy, w, h - dy);
	    break;
	case QWSDecoration::Bottom:
	    geom = QRect(x, y, w, h + dy);
	    break;
	case QWSDecoration::Left:
	    geom = QRect(x + dx, y, w - dx, h);
	    break;
	case QWSDecoration::Right:
	    geom = QRect(x, y, w + dx, h);
	    break;
	case QWSDecoration::TopRight:
	    geom = QRect(x, y + dy, w + dx, h - dy);
	    break;
	case QWSDecoration::TopLeft:
	    geom = QRect(x + dx, y + dy, w - dx, h - dy);
	    break;
	case QWSDecoration::BottomLeft:
	    geom = QRect(x + dx, y, w - dx, h + dy);
	    break;
	case QWSDecoration::BottomRight:
	    geom = QRect(x, y, w + dx, h + dy);
	    break;
	default:
	    return;
    }

    if (geom.width() >= managed->minimumWidth()
	    && geom.width() <= managed->maximumWidth()) {
	mousePos.setX(mousePos.x() + dx);
    } else if (geom.width() < managed->minimumWidth()) {
	if ( x != geom.x() ) {
	    geom.setX(x+(w-managed->minimumWidth()));
	    mousePos.setX( geom.x() );
	} else {
	    mousePos.setX( x+managed->minimumWidth() );
	}
	geom.setWidth(managed->minimumWidth());
    } else {
	geom.setX(x);
	geom.setWidth(w);
    }
    if (geom.height() >= managed->minimumHeight()
	    && geom.height() <= managed->maximumHeight()) {
	mousePos.setY(mousePos.y() + dy);
    } else if (geom.height() < managed->minimumHeight()) {
	if ( y != geom.y() ) {
	    geom.setY(y+(h-managed->minimumHeight()));
	    mousePos.setY( mousePos.y()+(geom.y()-y) );
	} else {
	    mousePos.setY( y+managed->minimumHeight() );
	}
	geom.setHeight(managed->minimumHeight());
    } else {
	geom.setY(y);
	geom.setHeight(h);
    }

    if (geom != managed->geometry()) {
	QApplication::sendPostedEvents();
	managed->setGeometry(geom);
    }

    dx = 0;
    dy = 0;
}

void QWSManager::paintEvent(QPaintEvent *)
{
    if (!managed->isVisible())
	return;
    QWSDecoration &dec = QApplication::qwsDecoration();
    QPainter painter(managed);

    // Adjust our widget region to contain the window
    // manager decoration instead of the widget itself.
    QRegion r = managed->topData()->decor_allocated_region;
    int rgnIdx = managed->alloc_region_index;
    if ( rgnIdx >= 0 ) {
	QRegion newRegion;
	bool changed = FALSE;
	QWSDisplay::grab();
	const int *rgnRev = qt_fbdpy->regionManager()->revision( rgnIdx );
	if ( managed->alloc_region_revision != *rgnRev ) {
	     newRegion = qt_fbdpy->regionManager()->region( rgnIdx );
	     changed = TRUE;
	}
	painter.internalGfx()->setGlobalRegionIndex( rgnIdx );
	QWSDisplay::ungrab();
	if ( changed ) {
	    r &= newRegion;
	}
    }
    painter.internalGfx()->setWidgetDeviceRegion( r );

    painter.setClipRegion(dec.region(managed, managed->rect()));
    dec.paint(&painter, managed);
    painter.setClipRegion(dec.region(managed, managed->rect()));
    dec.paintButton(&painter, managed, QWSDecoration::Menu, menuBtn->state());
    dec.paintButton(&painter, managed, QWSDecoration::Close, closeBtn->state());
    dec.paintButton(&painter, managed, QWSDecoration::Minimize, minimizeBtn->state());
    dec.paintButton(&painter, managed, QWSDecoration::Maximize, maximizeBtn->state());
}

void QWSManager::menu(const QPoint &pos)
{
#ifndef QT_NO_POPUPMENU
    if (!popup) {
	popup = QApplication::qwsDecoration().menu(managed, managed->pos());

	// Add Style menu
	QPopupMenu *styleMenu = new QPopupMenu();
	for (int i = 0; WMStyleList[i].WMStyleName != NULL; i++)
	    styleMenu->insertItem( qApp->translate("QWSDecoration", WMStyleList[i].WMStyleName), WMStyleList[i].WMStyleType );
	styleMenu->connect(styleMenu, SIGNAL(activated(int)), this, SLOT(styleMenuActivated(int)));
//	popup->insertSeparator();
//	popup->insertItem(tr("Style"), styleMenu);

	connect(popup, SIGNAL(activated(int)), SLOT(menuActivated(int)));
    }
    popup->setItemEnabled(QWSDecoration::Maximize, !managed->isMaximized());
    popup->setItemEnabled(QWSDecoration::Normalize, managed->isMaximized());
    popup->popup(pos);
#endif
}

#include <qcdestyle.h>
#include <qcommonstyle.h>
#include <qcompactstyle.h>
#include <qmotifplusstyle.h>
#include <qmotifstyle.h>
#include <qplatinumstyle.h>
#include <qsgistyle.h>
#include <qwindowsstyle.h>

void QWSManager::styleMenuActivated(int id)
{
    for (int i = 0; WMStyleList[i].WMStyleName != NULL; i++) {
    	if (id == WMStyleList[i].WMStyleType) {
	    qApp->qwsSetDecoration( WMStyleList[i].new_WMDecorations() );
    	}
    }

    // Force a repaint of the WM regions
    const QSize s = managed->size();
    managed->resize( s.width() + 1, s.height() );
    managed->resize( s.width(), s.height() );
}

void QWSManager::menuActivated(int id)
{
    switch (id) {
	case QWSDecoration::Close:
	    close();
	    return;
	case QWSDecoration::Minimize:
	    minimize();
	    break;
	case QWSDecoration::Maximize:
	case QWSDecoration::Normalize:
	    toggleMaximize();
	    break;
	case QWSDecoration::Title:
	    mousePos = QCursor::pos();
	    activeRegion = QWSDecoration::Title;
	    active = managed;
	    managed->grabMouse();
	    break;
	case QWSDecoration::BottomRight:
	    mousePos = QCursor::pos();
	    activeRegion = QWSDecoration::BottomRight;
	    active = managed;
	    managed->grabMouse();
	    break;
	default:
	    break;
    }
}

void QWSManager::close()
{
    active = 0;
    QApplication::qwsDecoration().close(managed);
}

void QWSManager::minimize()
{
    QApplication::qwsDecoration().minimize(managed);
}


void QWSManager::maximize()
{
    QApplication::qwsDecoration().maximize(managed);
    maximizeBtn->setOn(TRUE);
}

void QWSManager::toggleMaximize()
{
    if ( !managed->isMaximized() ) {
	managed->showMaximized();
	maximizeBtn->setOn(TRUE);
    } else {
	managed->showNormal();
	maximizeBtn->setOn(FALSE);
    }
}

/*
*/
QWSButton::QWSButton(QWSManager *m, QWSDecoration::Region t, bool tb)
    : flags(0), toggle(tb), type(t), manager(m)
{
}

void QWSButton::setMouseOver(bool m)
{
    int s = state();
    if (m) flags |= MouseOver;
    else flags &= ~MouseOver;
    if (state() != s)
	paint();
}

void QWSButton::setClicked(bool c)
{
    int s = state();
    if (c) flags |= Clicked;
    else flags &= ~Clicked;
    if (state() != s)
	paint();
}

void QWSButton::setOn(bool o)
{
    int s = state();
    if (o) flags |= On;
    else flags &= ~On;
    if (state() != s)
	paint();
}

void QWSButton::paint()
{
    if (manager->widget()->isVisible()) {
	QWSDecoration &dec = QApplication::qwsDecoration();
	QPainter painter(manager->widget());
	painter.setClipRegion(dec.region(manager->widget(), manager->widget()->rect()));
	dec.paintButton(&painter, manager->widget(), type, state());
    }
}

#endif //QT_NO_QWS_MANAGER
