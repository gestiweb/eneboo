/****************************************************************************
** $Id: qt/qcursor_mac.cpp   3.3.8   edited Jan 11 14:38 $
**
** Implementation of QCursor class for mac
**
** Created : 001019
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

#include "qcursor.h"
#include "qbitmap.h"
#include "qimage.h"
#include "qapplication.h"
#include "qdatastream.h"
#include "qnamespace.h"
#include "qt_mac.h"
#ifdef Q_WS_MACX
# include <ApplicationServices/ApplicationServices.h>
#endif
#if QT_MACOSX_VERSION >= 0x1030
# define QMAC_USE_BIG_CURSOR_API
#endif
#ifdef QMAC_USE_BIG_CURSOR_API
# include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>


/*****************************************************************************
  Global cursors
 *****************************************************************************/

static QCursor cursorTable[Qt::LastCursor+1];
static const int arrowCursorIdx = 0;
QT_STATIC_CONST_IMPL QCursor & Qt::arrowCursor = cursorTable[0];
QT_STATIC_CONST_IMPL QCursor & Qt::upArrowCursor = cursorTable[1];
QT_STATIC_CONST_IMPL QCursor & Qt::crossCursor = cursorTable[2];
QT_STATIC_CONST_IMPL QCursor & Qt::waitCursor = cursorTable[3];
QT_STATIC_CONST_IMPL QCursor & Qt::ibeamCursor = cursorTable[4];
QT_STATIC_CONST_IMPL QCursor & Qt::sizeVerCursor = cursorTable[5];
QT_STATIC_CONST_IMPL QCursor & Qt::sizeHorCursor = cursorTable[6];
QT_STATIC_CONST_IMPL QCursor & Qt::sizeBDiagCursor = cursorTable[7];
QT_STATIC_CONST_IMPL QCursor & Qt::sizeFDiagCursor = cursorTable[8];
QT_STATIC_CONST_IMPL QCursor & Qt::sizeAllCursor = cursorTable[9];
QT_STATIC_CONST_IMPL QCursor & Qt::blankCursor = cursorTable[10];
QT_STATIC_CONST_IMPL QCursor & Qt::splitVCursor = cursorTable[11];
QT_STATIC_CONST_IMPL QCursor & Qt::splitHCursor = cursorTable[12];
QT_STATIC_CONST_IMPL QCursor & Qt::pointingHandCursor = cursorTable[13];
QT_STATIC_CONST_IMPL QCursor & Qt::forbiddenCursor = cursorTable[14];
QT_STATIC_CONST_IMPL QCursor & Qt::whatsThisCursor = cursorTable[15];
QT_STATIC_CONST_IMPL QCursor & Qt::busyCursor = cursorTable[16];

/*****************************************************************************
  Internal QCursorData class
 *****************************************************************************/

extern QPixmap qt_mac_bigEndianCopy(const QPixmap &pixmap); //qpixmap_mac.cpp

class QMacAnimateCursor : public QObject
{
    int timerId, step;
    ThemeCursor curs;
public:
    QMacAnimateCursor() : QObject(), timerId(-1) { }
    void start(ThemeCursor c) {
	step = 1;
	if(timerId != -1)
	    killTimer(timerId);
	timerId = startTimer(600);
	curs = c;
    }
    void stop() {
	if(timerId != -1) {
	    killTimer(timerId);
	    timerId = -1;
	}
    }
protected:
    void timerEvent(QTimerEvent *e) {
	if(e->timerId() == timerId) {
	    if(SetAnimatedThemeCursor(curs, step++) == themeBadCursorIndexErr)
		stop();
	}
    }
};

#ifndef QMAC_NO_FAKECURSOR
#include <qpainter.h>
class QMacCursorWidget : public QWidget
{
    Q_OBJECT
    QBitmap bitmap;
public:
    QMacCursorWidget(const QBitmap *b, const QBitmap *m) :
	QWidget(0, "fake_cursor", WType_Dialog | WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop)
	{
	    setAcceptDrops(TRUE); //bleh
	    hide();
	    int attribs = kWindowNoShadowAttribute;
#if QT_MACOSX_VERSION >= 0x1020 && QT_MACOSX_VERSION < 0x1030
	    attribs |= kWindowIgnoreClicksAttribute;
#endif
	    ChangeWindowAttributes((WindowPtr)handle(), attribs, 0);
	    int w = b->width(), h = b->height();
	    resize(w, h);

	    QImage bi, mi;
	    bitmap = *b;
	    bi = bitmap;
	    mi = *m;
	    for(int y = 0; y < bi.height(); y++) {
		for(int x = 0; x < bi.width(); x++)
		    mi.setPixel(x, y, !(bi.pixel(x, y) && mi.pixel(x, y)));
	    }
	    QBitmap mask;
	    mask = mi;
#if 0
	    bitmap.setMask(mask);
	    setBackgroundColor(blue);
#else
	    setMask(mask);
#endif
	}
    ~QMacCursorWidget() { }
protected:
    void paintEvent(QPaintEvent *) { bitBlt(this, 0, 0, &bitmap); }
};
#include "qcursor_mac.moc"
#endif

struct QCursorData : public QShared
{
    QCursorData(int s = 0);
   ~QCursorData();

    int id;
    int	      cshape;
    int hx, hy;
    QBitmap  *bm, *bmm;

    enum { TYPE_None, TYPE_CursorImage, TYPE_CursPtr, TYPE_ThemeCursor, TYPE_FakeCursor, TYPE_BigCursor } type;
    union {
	struct {
	    uint my_cursor:1;
	    CursPtr   hcurs;
	} cp;
#ifndef QMAC_NO_FAKECURSOR
	struct {
	    QMacCursorWidget *widget;
	    CursPtr empty_curs;
	} fc;
#endif
#ifdef QMAC_USE_BIG_CURSOR_API
	char *big_cursor_name;
#endif
#ifdef QMAC_NO_FAKECURSOR
	CursorImageRec *ci;
#endif
	struct {
	    QMacAnimateCursor *anim;
	    ThemeCursor curs;
	} tc;
    } curs;
};

static QCursorData *currentCursor = NULL; //current cursor
#ifndef QMAC_NO_FAKECURSOR
static Point currentPoint = { 0, 0 };
#endif
void qt_mac_set_cursor(const QCursor *c, const Point *p)
{
    (void)c->handle(); //force the cursor to get loaded, if it's not

#ifndef QMAC_NO_FAKECURSOR
    if(c->data->type == QCursorData::TYPE_FakeCursor &&
	(currentCursor != c->data || currentPoint.h != p->h || currentPoint.v != p->v)) {
	/* That's right folks, I want nice big cursors - if apple won't give them to me, why
	   I'll just take them!!! */
	c->data->curs.fc.widget->move(p->h - c->data->curs.fc.empty_curs->hotSpot.h,
				      p->v - c->data->curs.fc.empty_curs->hotSpot.v);
	SetCursor(c->data->curs.fc.empty_curs);
 	if(currentCursor && currentCursor != c->data && currentCursor->type == QCursorData::TYPE_FakeCursor)
	    currentCursor->curs.fc.widget->hide();
	if(!c->data->curs.fc.widget->isVisible())
	    c->data->curs.fc.widget->show();
    } else
#else
	Q_UNUSED(p);
#endif
    if(currentCursor != c->data) {
#ifndef QMAC_NO_FAKECURSOR
	if(currentCursor && currentCursor->type == QCursorData::TYPE_FakeCursor)
	    currentCursor->curs.fc.widget->hide();
#endif
	if(currentCursor && currentCursor->type == QCursorData::TYPE_ThemeCursor && currentCursor->curs.tc.anim)
	    currentCursor->curs.tc.anim->stop();

	if(c->data->type == QCursorData::TYPE_CursPtr) {
	    SetCursor(c->data->curs.cp.hcurs);
	} else if(c->data->type == QCursorData::TYPE_CursorImage) {

	} else if(c->data->type == QCursorData::TYPE_ThemeCursor) {
	    if(SetAnimatedThemeCursor(c->data->curs.tc.curs, 0) == themeBadCursorIndexErr) {
		SetThemeCursor(c->data->curs.tc.curs);
	    } else {
		if(!c->data->curs.tc.anim)
		    c->data->curs.tc.anim = new QMacAnimateCursor;
		c->data->curs.tc.anim->start(c->data->curs.tc.curs);
	    }
#ifdef QMAC_USE_BIG_CURSOR_API
	} else if(c->data->type == QCursorData::TYPE_BigCursor) {
	    QDSetNamedPixMapCursor(c->data->curs.big_cursor_name);
#endif
	} else {
//	    qDebug("Qt: internal: WH0A. Unexpected condition reached!");
	}
    }
    currentCursor = c->data;
}

QCursorData::QCursorData(int s)
{
    cshape = s;
    bm = bmm = 0;
    hx = hy  = -1;
    type = TYPE_None;
    memset(&curs, '\0', sizeof(curs));

    static int static_id = 121578; //the holy day
    id = static_id++;
}

QCursorData::~QCursorData()
{
    if(type == TYPE_CursPtr) {
	if(curs.cp.hcurs && curs.cp.my_cursor)
	    free(curs.cp.hcurs);
#ifdef QMAC_NO_FAKECURSOR
    } else if(type == TYPE_CursorImage) {
	free(curs.ci);
#endif
#ifdef QMAC_USE_BIG_CURSOR_API
    } else if(type == TYPE_BigCursor) {
	QDUnregisterNamedPixMapCursur(curs.big_cursor_name);
	free(curs.big_cursor_name);
#endif
    } else if(type == TYPE_FakeCursor) {
#ifndef QMAC_NO_FAKECURSOR
	free(curs.fc.empty_curs);
	delete curs.fc.widget;
#endif
    } else if(type == TYPE_ThemeCursor) {
	delete curs.tc.anim;
    }
    type = TYPE_None;

    if(bm)
	delete bm;
    if(bmm)
	delete bmm;
    if(currentCursor == this)
	currentCursor = NULL;
}

QCursor *QCursor::find_cur(int shape)		// find predefined cursor
{
    return (uint)shape <= LastCursor ? &cursorTable[shape] : 0;
}


static bool initialized = FALSE;
void QCursor::cleanup()
{
    if(!initialized)
	return;

    int shape;
    for(shape = 0; shape <= LastCursor; shape++) {
	delete cursorTable[shape].data;
	cursorTable[shape].data = 0;
    }
    initialized = FALSE;
}

void QCursor::initialize()
{
    InitCursor();
    int shape;
    for(shape = 0; shape <= LastCursor; shape++)
	cursorTable[shape].data = new QCursorData(shape);
    initialized = TRUE;
    qAddPostRoutine(cleanup);
}

QCursor::QCursor()
{
    if(!initialized) {
	if(qApp->startingUp()) {
	    data = 0;
	    return;
	}
	initialize();
    }
    QCursor* c = &cursorTable[arrowCursorIdx];
    c->data->ref();
    data = c->data;
}

QCursor::QCursor(int shape)
{
    if(!initialized)
	initialize();
    QCursor *c = find_cur(shape);
    if(!c)					// not found
	c = &cursorTable[arrowCursorIdx];	//   then use arrowCursor
    c->data->ref();
    data = c->data;
}

void QCursor::setBitmap(const QBitmap &bitmap, const QBitmap &mask,
			 int hotX, int hotY)
{
    if(!initialized)
	initialize();
    if(bitmap.depth() != 1 || mask.depth() != 1 || bitmap.size() != mask.size()) {
#if defined(QT_CHECK_NULL)
	qWarning("Qt: QCursor: Cannot create bitmap cursor; invalid bitmap(s)");
#endif
	QCursor *c = &cursorTable[arrowCursorIdx];
	c->data->ref();
	data = c->data;
	return;
    }
    data = new QCursorData;
    Q_CHECK_PTR(data);
    data->bm  = new QBitmap(bitmap);
    data->bmm = new QBitmap(mask);
    data->cshape = BitmapCursor;
    data->hx = hotX >= 0 ? hotX : bitmap.width()/2;
    data->hy = hotY >= 0 ? hotY : bitmap.height()/2;
}

QCursor::QCursor(const QCursor &c)
{
    if(!initialized)
	initialize();
    data = c.data;				// shallow copy
    data->ref();
}

QCursor::~QCursor()
{
    if(data && data->deref())
	delete data;
}

QCursor &QCursor::operator=(const QCursor &c)
{
    if(!initialized)
	initialize();
    c.data->ref();				// avoid c = c
    if(data->deref())
	delete data;
    data = c.data;
    return *this;
}

int QCursor::shape() const
{
    if(!initialized)
	initialize();
    return data->cshape;
}

void QCursor::setShape(int shape)
{
    if(!initialized)
	initialize();
    QCursor *c = find_cur(shape);
    if(!c)					// not found
	c = &cursorTable[arrowCursorIdx];	//   then use arrowCursor
    c->data->ref();
    data = c->data;
}

const QBitmap *QCursor::bitmap() const
{
    if(!initialized)
	initialize();
    return data->bm;
}


const QBitmap *QCursor::mask() const
{
    if(!initialized)
	initialize();
    return data->bmm;
}

QPoint QCursor::hotSpot() const
{
    if(!initialized)
	initialize();
    return QPoint(data->hx, data->hy);
}

Qt::HANDLE QCursor::handle() const
{
    if(!initialized)
	initialize();
    if(data->type == QCursorData::TYPE_None)
	update();
    return (Qt::HANDLE)data->id;
}

QPoint QCursor::pos()
{
    Point p;
    GetGlobalMouse(&p);
    return QPoint(p.h, p.v);
}


void QCursor::setPos(int x, int y)
{
#ifdef Q_WS_MACX
    CGPoint p;
    p.x = x;
    p.y = y;
    CGWarpMouseCursorPosition(p);
#else
// some kruft I found on the web.. it doesn't work, but I want to test more FIXME
#   define MTemp 0x828
#   define RawMouse 0x82c
#   define CrsrNewCouple 0x8ce
    HideCursor();
    Point where;
    where.h = x;
    where.v = y;
    *((Point *) RawMouse) = where ;
    *((Point *) MTemp) = where ;
    *((short *) CrsrNewCouple) = -1 ;
    ShowCursor() ;
#endif

    /* I'm not too keen on doing this, but this makes it a lot easier, so I just
       send the event back through the event system and let it get propagated correctly
       ideally this would not really need to be faked --Sam
    */
    if(QWidget *grb = QWidget::mouseGrabber()) {
	QMouseEvent me(QMouseEvent::MouseMove, QPoint(x, y), 0, 0);
	QApplication::sendEvent(grb, &me);
    }
}

void QCursor::update() const
{
    if(!initialized)
	initialize();
    register QCursorData *d = data;		// cheat const!
    if(d->type != QCursorData::TYPE_None)				// already loaded
	return;

    /* Note to self... ***
     * mask x data
     * 0xFF x 0x00 == fully opaque white
     * 0x00 x 0xFF == xor'd black
     * 0xFF x 0xFF == fully opaque black
     * 0x00 x 0x00 == fully transparent
     */

    switch(d->cshape) {			// map Q cursor to MAC cursor
    case BitmapCursor: {
	if(d->bm->width() == 16 && d->bm->height() == 16) {
	    d->type = QCursorData::TYPE_CursPtr;
	    d->curs.cp.my_cursor = TRUE;
	    d->curs.cp.hcurs = (CursPtr)malloc(sizeof(Cursor));
	    QImage bmi, bmmi;
	    bmi = *d->bm;
	    bmmi = *d->bmm;

	    memset(d->curs.cp.hcurs->mask, 0, 32);
	    memset(d->curs.cp.hcurs->data, 0, 32);
	    for(int y = 0; y < 16; y++) {
		for(int x = 0; x < 16; x++) {
		    int bmi_val = 0, bmmi_val = 0;
		    if(!bmmi.pixel(x, y)) {
			if(bmi.pixel(x, y))
			    bmmi_val = 1;
			else
			    bmi_val = bmmi_val = 1;
		    }
		    if(bmmi_val)
			*(((uchar*)d->curs.cp.hcurs->mask) + (y*2) + (x / 8)) |= (1 << (7 - (x % 8)));
		    if(bmi_val)
			*(((uchar*)d->curs.cp.hcurs->data) + (y*2) + (x / 8)) |= (1 << (7 - (x % 8)));
		}
	    }
#ifdef QMAC_USE_BIG_CURSOR_API
	} else if(qMacVersion() >= Qt::MV_10_DOT_2 && d->bm->width() < 64 && d->bm->height() < 64) {
	    d->curs.big_cursor_name = (char *)malloc(128);
	    static int big_cursor_cnt = 0;
	    sprintf(d->curs.big_cursor_name, "qt_QCursor_%d_%d", getpid(), big_cursor_cnt++);
	    Point hotspot;
	    if((hotspot.h = data->hx) < 0)
		hotspot.h = 0;
	    if((hotspot.v = data->hy) < 0)
		hotspot.v = 0;

            const QPixmap big_bm(qt_mac_bigEndianCopy(*data->bm)),
                         big_bmm(qt_mac_bigEndianCopy(*data->bmm));
	    OSStatus ret = QDRegisterNamedPixMapCursor(GetGWorldPixMap((GWorldPtr)big_bm.handle()),
						       GetGWorldPixMap((GWorldPtr)big_bmm.handle()), hotspot,
						       d->curs.big_cursor_name);
	    if(ret == noErr)
		d->type = QCursorData::TYPE_BigCursor;
	    else
		free(d->curs.big_cursor_name);
#endif
	}
	if(d->type == QCursorData::TYPE_None) {
#ifndef QMAC_NO_FAKECURSOR
	    d->type = QCursorData::TYPE_FakeCursor;
	    d->curs.fc.widget = new QMacCursorWidget(d->bm, d->bmm);
	    //make an empty cursor
	    d->curs.fc.empty_curs = (CursPtr)malloc(sizeof(Cursor));
	    memset(d->curs.fc.empty_curs->data, 0x00, sizeof(d->curs.fc.empty_curs->data));
	    memset(d->curs.fc.empty_curs->mask, 0x00, sizeof(d->curs.fc.empty_curs->mask));
	    int hx = data->hx, hy = data->hy;
	    if(hx < 0)
		hx = 8;
	    else if(hx > 15)
		hx = 15;
	    if(hy < 0)
		hy = 8;
	    else if(hy > 15)
		hy = 15;
	    d->curs.fc.empty_curs->hotSpot.h = hx;
	    d->curs.fc.empty_curs->hotSpot.v = hy;
#else
	    d->type = QCursorData::TYPE_CursorImage;
	    d->curs.ci = (CursorImageRec*)malloc(sizeof(CursorImageRec));
	    d->curs.ci->majorVersion = kCursorImageMajorVersion;
	    d->curs.ci->minorVersion = kCursorImageMinorVersion;
	    d->curs.ci->cursorPixMap = GetGWorldPixMap((GWorldPtr)d->bm->handle());
	    d->curs.ci->cursorBitMask = (BitMap **)GetGWorldPixMap((GWorldPtr)d->bmm->handle());
#endif
	}
	break; }
    case ArrowCursor:
	d->type = QCursorData::TYPE_ThemeCursor;
	d->curs.tc.curs = kThemeArrowCursor;
	break;
    case CrossCursor:
	d->type = QCursorData::TYPE_ThemeCursor;
	d->curs.tc.curs = kThemeCrossCursor;
	break;
    case WaitCursor:
	d->type = QCursorData::TYPE_ThemeCursor;
	d->curs.tc.curs = kThemeWatchCursor;
	break;
    case IbeamCursor:
	d->type = QCursorData::TYPE_ThemeCursor;
	d->curs.tc.curs = kThemeIBeamCursor;
	break;
    case SizeAllCursor:
	d->type = QCursorData::TYPE_ThemeCursor;
	d->curs.tc.curs = kThemePlusCursor;
	break;
    case WhatsThisCursor: //for now jus tuse the pointing hand
    case PointingHandCursor:
	d->type = QCursorData::TYPE_ThemeCursor;
	d->curs.tc.curs = kThemePointingHandCursor;
	break;
    case BusyCursor:
	d->type = QCursorData::TYPE_ThemeCursor;
	d->curs.tc.curs = kThemeSpinningCursor;
	break;

#define QT_USE_APPROXIMATE_CURSORS
#ifdef QT_USE_APPROXIMATE_CURSORS
    case SizeVerCursor:
    {
	static const uchar cur_ver_bits[] = {
	    0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x0f, 0xf0,
	    0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x0f, 0xf0,
	    0x07, 0xe0, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00 };
	static const uchar mcur_ver_bits[] = {
	    0x00, 0x00, 0x03, 0x80, 0x07, 0xc0, 0x0f, 0xe0, 0x1f, 0xf0, 0x3f, 0xf8,
	    0x7f, 0xfc, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x7f, 0xfc, 0x3f, 0xf8,
	    0x1f, 0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x80 };

	d->type = QCursorData::TYPE_CursPtr;
	d->curs.cp.my_cursor = TRUE;
	d->curs.cp.hcurs = (CursPtr)malloc(sizeof(Cursor));
	memcpy(d->curs.cp.hcurs->data, cur_ver_bits, sizeof(cur_ver_bits));
	memcpy(d->curs.cp.hcurs->mask, mcur_ver_bits, sizeof(mcur_ver_bits));
	break;
    }

    case SizeHorCursor:
    {
	static const uchar cur_hor_bits[] = {
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x20, 0x18, 0x30,
	    0x38, 0x38, 0x7f, 0xfc, 0x7f, 0xfc, 0x38, 0x38, 0x18, 0x30, 0x08, 0x20,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	static const uchar mcur_hor_bits[] = {
	    0x00, 0x00, 0x00, 0x00, 0x04, 0x40, 0x0c, 0x60, 0x1c, 0x70, 0x3c, 0x78,
	    0x7f, 0xfc, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0x7f, 0xfc, 0x3c, 0x78,
	    0x1c, 0x70, 0x0c, 0x60, 0x04, 0x40, 0x00, 0x00 };

	d->type = QCursorData::TYPE_CursPtr;
	d->curs.cp.my_cursor = TRUE;
	d->curs.cp.hcurs = (CursPtr)malloc(sizeof(Cursor));
	memcpy(d->curs.cp.hcurs->data, cur_hor_bits, sizeof(cur_hor_bits));
	memcpy(d->curs.cp.hcurs->mask, mcur_hor_bits, sizeof(mcur_hor_bits));
	break;
    }

    case SizeBDiagCursor:
    {
	static const uchar cur_fdiag_bits[] = {
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0xf8, 0x00, 0x78,
	    0x00, 0xf8, 0x01, 0xd8, 0x23, 0x88, 0x37, 0x00, 0x3e, 0x00, 0x3c, 0x00,
	    0x3e, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00 };
	static const uchar mcur_fdiag_bits[] = {
	    0x00, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x03, 0xfc, 0x01, 0xfc, 0x00, 0xfc,
	    0x41, 0xfc, 0x63, 0xfc, 0x77, 0xdc, 0x7f, 0x8c, 0x7f, 0x04, 0x7e, 0x00,
	    0x7f, 0x00, 0x7f, 0x80, 0x7f, 0xc0, 0x00, 0x00 };

	d->type = QCursorData::TYPE_CursPtr;
	d->curs.cp.my_cursor = TRUE;
	d->curs.cp.hcurs = (CursPtr)malloc(sizeof(Cursor));
	memcpy(d->curs.cp.hcurs->data, cur_fdiag_bits, sizeof(cur_fdiag_bits));
	memcpy(d->curs.cp.hcurs->mask, mcur_fdiag_bits, sizeof(mcur_fdiag_bits));
	break;
    }
    case SizeFDiagCursor:
    {
	static const uchar cur_bdiag_bits[] = {
	    0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x3e, 0x00, 0x3c, 0x00, 0x3e, 0x00,
	    0x37, 0x00, 0x23, 0x88, 0x01, 0xd8, 0x00, 0xf8, 0x00, 0x78, 0x00, 0xf8,
	    0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	static const uchar mcur_bdiag_bits[] = {
	    0x00, 0x00, 0x7f, 0xc0, 0x7f, 0x80, 0x7f, 0x00, 0x7e, 0x00, 0x7f, 0x04,
	    0x7f, 0x8c, 0x77, 0xdc, 0x63, 0xfc, 0x41, 0xfc, 0x00, 0xfc, 0x01, 0xfc,
	    0x03, 0xfc, 0x07, 0xfc, 0x00, 0x00, 0x00, 0x00 };

	d->type = QCursorData::TYPE_CursPtr;
	d->curs.cp.my_cursor = TRUE;
	d->curs.cp.hcurs = (CursPtr)malloc(sizeof(Cursor));
	memcpy(d->curs.cp.hcurs->data, cur_bdiag_bits, sizeof(cur_bdiag_bits));
	memcpy(d->curs.cp.hcurs->mask, mcur_bdiag_bits, sizeof(mcur_bdiag_bits));
	break;
    }
    case ForbiddenCursor:
#if QT_MACOSX_VERSION >= 0x1020
	d->type = QCursorData::TYPE_ThemeCursor;
	d->curs.tc.curs = kThemeNotAllowedCursor;
	break;
#endif
    case BlankCursor:
    {
	d->type = QCursorData::TYPE_CursPtr;
	d->curs.cp.my_cursor = TRUE;
	d->curs.cp.hcurs = (CursPtr)malloc(sizeof(Cursor));
	memset(d->curs.cp.hcurs->data, 0x00, sizeof(d->curs.cp.hcurs->data));
	memset(d->curs.cp.hcurs->mask, 0x00, sizeof(d->curs.cp.hcurs->data));
	break;
    }
    case UpArrowCursor:
    {
	static const unsigned char cur_up_arrow_bits[] = {
	    0x00, 0x80, 0x01, 0x40, 0x01, 0x40, 0x02, 0x20, 0x02, 0x20, 0x04, 0x10,
	    0x04, 0x10, 0x08, 0x08, 0x0f, 0x78, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40,
	    0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0xc0 };
	static const unsigned char mcur_up_arrow_bits[] = {
	    0x00, 0x80, 0x01, 0xc0, 0x01, 0xc0, 0x03, 0xe0, 0x03, 0xe0, 0x07, 0xf0,
	    0x07, 0xf0, 0x0f, 0xf8, 0x0f, 0xf8, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0,
	    0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0, 0x01, 0xc0 };

	d->type = QCursorData::TYPE_CursPtr;
	d->curs.cp.my_cursor = TRUE;
	d->curs.cp.hcurs = (CursPtr)malloc(sizeof(Cursor));
	memcpy(d->curs.cp.hcurs->data, cur_up_arrow_bits, sizeof(cur_up_arrow_bits));
	memcpy(d->curs.cp.hcurs->mask, mcur_up_arrow_bits, sizeof(mcur_up_arrow_bits));
	break;
    }
    case SplitVCursor:
    {
#if QT_MACOSX_VERSION >= 0x1030
        if(qMacVersion() >= Qt::MV_PANTHER) {
            d->type = QCursorData::TYPE_ThemeCursor;
            d->curs.tc.curs = kThemeResizeUpDownCursor;
            break;
        }
#endif
#if defined( QMAC_NO_FAKECURSOR ) || 1
	static const unsigned char cur_vsplit_bits[] = {
	    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x80, 0x01, 0x00, 0x01, 0x00,
	    0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x01, 0x00, 0x01, 0x00,
	    0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
	static const unsigned char mcur_vsplit_bits[] = {
            0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x80, 0x01, 0x00, 0x01, 0x00,
	    0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x01, 0x00, 0x01, 0x00,
	    0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };

	d->type = QCursorData::TYPE_CursPtr;
	d->curs.cp.my_cursor = TRUE;
	d->curs.cp.hcurs = (CursPtr)malloc(sizeof(Cursor));
	memcpy(d->curs.cp.hcurs->data, cur_vsplit_bits, sizeof(cur_vsplit_bits));
        memcpy(d->curs.cp.hcurs->mask, mcur_vsplit_bits, sizeof(mcur_vsplit_bits));
#else
	static uchar cur_vsplit_bits[] = {
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x80, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x00,
	    0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
	    0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00,
	    0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
	    0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x00,
            0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	static uchar mcur_vsplit_bits[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
	    0x00, 0xc0, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0xf0, 0x07, 0x00,
	    0x00, 0xf8, 0x0f, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xc0, 0x01, 0x00,
            0x00, 0xc0, 0x01, 0x00, 0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00,
	    0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00,
	    0x80, 0xff, 0xff, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xc0, 0x01, 0x00,
	    0x00, 0xc0, 0x01, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x00, 0xf0, 0x07, 0x00,
            0x00, 0xe0, 0x03, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	QBitmap bm(32, 32, cur_vsplit_bits, TRUE), bmm(32, 32, mcur_vsplit_bits, TRUE);
	setBitmap(bm, bmm, 16, 16);
#endif
        break;
   }
    case SplitHCursor:
    {
#if QT_MACOSX_VERSION >= 0x1030
        if(qMacVersion() >= Qt::MV_PANTHER) {
            d->type = QCursorData::TYPE_ThemeCursor;
            d->curs.tc.curs = kThemeResizeLeftRightCursor;
            break;
        }
#endif
#if defined( QMAC_NO_FAKECURSOR ) || 1
	static const unsigned char cur_hsplit_bits[] = {
	    0x04, 0x80, 0x04, 0x80, 0x04, 0x80, 0x04, 0x80, 0x04, 0x80, 0x04, 0x80,
	    0x24, 0x90, 0x7c, 0xf8, 0x24, 0x90, 0x04, 0x80, 0x04, 0x80, 0x04, 0x80,
	    0x04, 0x80, 0x04, 0x80, 0x04, 0x80, 0x00, 0x00 };
	static const unsigned char mcur_hsplit_bits[] = {
	    0x04, 0x80, 0x04, 0x80, 0x04, 0x80, 0x04, 0x80, 0x04, 0x80, 0x04, 0x80,
	    0x24, 0x90, 0x7c, 0xf8, 0x24, 0x90, 0x04, 0x80, 0x04, 0x80, 0x04, 0x80,
	    0x04, 0x80, 0x04, 0x80, 0x04, 0x80, 0x00, 0x00 };

	d->type = QCursorData::TYPE_CursPtr;
	d->curs.cp.my_cursor = TRUE;
	d->curs.cp.hcurs = (CursPtr)malloc(sizeof(Cursor));
	memcpy(d->curs.cp.hcurs->data, cur_hsplit_bits, sizeof(cur_hsplit_bits));
	memcpy(d->curs.cp.hcurs->mask, mcur_hsplit_bits, sizeof(mcur_hsplit_bits));
#else
	static uchar cur_hsplit_bits[] = {
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
	    0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
	    0x00, 0x41, 0x82, 0x00, 0x80, 0x41, 0x82, 0x01, 0xc0, 0x7f, 0xfe, 0x03,
            0x80, 0x41, 0x82, 0x01, 0x00, 0x41, 0x82, 0x00, 0x00, 0x40, 0x02, 0x00,
	    0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
	    0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	static uchar mcur_hsplit_bits[] = {
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
	    0x00, 0xe0, 0x07, 0x00, 0x00, 0xe2, 0x47, 0x00, 0x00, 0xe3, 0xc7, 0x00,
	    0x80, 0xe3, 0xc7, 0x01, 0xc0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x07,
            0xc0, 0xff, 0xff, 0x03, 0x80, 0xe3, 0xc7, 0x01, 0x00, 0xe3, 0xc7, 0x00,
	    0x00, 0xe2, 0x47, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
	    0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        QBitmap bm(32, 32, cur_hsplit_bits, TRUE), bmm(32, 32, mcur_hsplit_bits, TRUE);
	setBitmap(bm, bmm, 16, 16);
#endif
        break;
    }
#endif
    default:
#if defined(QT_CHECK_RANGE)
	qWarning("Qt: QCursor::update: Invalid cursor shape %d", d->cshape);
#endif
	return;
    }

    if(d->type == QCursorData::TYPE_CursPtr && d->curs.cp.hcurs && d->curs.cp.my_cursor) {
	d->curs.cp.hcurs->hotSpot.h = data->hx >= 0 ? data->hx : 8;
	d->curs.cp.hcurs->hotSpot.v = data->hy >= 0 ? data->hy : 8;
    }
}
