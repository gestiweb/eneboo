/****************************************************************************
** $Id: qt/qmacstyle_mac.cpp   3.3.8   edited Jan 19 16:58 $
**
** Implementation of Mac native theme
**
** Created : 001018
**
** Copyright (C) 1992-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of the style module of the Qt GUI Toolkit.
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

#include "qmacstyle_mac.h"

#if defined(Q_WS_MAC) && !defined(QT_NO_STYLE_MAC)

#include <qpainter.h>

class QMacPainter : public QPainter
{
public:
    QMacPainter(QPaintDevice *p) : QPainter(p) { }
    QPoint domap(int x, int y) { map(x, y, &x, &y); return QPoint(x, y); }
    void setport();
    void setfont() { QPainter::updateFont(); }
};

#include <qt_mac.h>
#include <private/qaquastyle_p.h>
#include <private/qdialogbuttons_p.h>
#include <private/qtitlebar_p.h>
#include <private/qmacstylepixmaps_mac_p.h>
#include <qapplication.h>
#include <qbitmap.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qdrawutil.h>
#include <qguardedptr.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qscrollbar.h>
#include <qscrollview.h>
#include <qspinbox.h>
#include <qsplitter.h>
#include <qtable.h>
#include <qtabbar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qwidgetlist.h>
#include <qtabwidget.h>

/* I need these to simulate the pushbutton pulse */
#include <qpixmapcache.h>
#include <qimage.h>

#include <string.h>

//externals
RgnHandle qt_mac_get_rgn(); //qregion_mac.cpp
void qt_mac_dispose_rgn(RgnHandle r); //qregion_mac.cpp
extern QPaintDevice *qt_mac_safe_pdev; //qapplication_mac.cpp

#if (MAC_OS_X_VERSION_MAX_ALLOWED == MAC_OS_X_VERSION_10_2)
// It's really silly, the define isn't there, but you can pass it and it works, dumb luck I guess.
enum { kThemeComboBox = 16 }; // From the 10.3 Appearance.h file.
#endif

//static utility variables
static const QDialogButtons::Button macBtnOrder[] = { QDialogButtons::Reject, QDialogButtons::Accept, //reverse order (right to left)
						      QDialogButtons::All, QDialogButtons::Apply, QDialogButtons::Abort,
                                                      QDialogButtons::Retry, QDialogButtons::Ignore };
static ThemeWindowType macWinType = kThemeUtilityWindow;
static const int macSpinBoxSep        = 5;    // distance between spinwidget and the lineedit
static const int macItemFrame         = 2;    // menu item frame width
static const int macItemHMargin       = 3;    // menu item hor text margin
static const int macItemVMargin       = 2;    // menu item ver text margin
static const int macRightBorder       = 12;   // right border on mac

#define QMAC_DO_SECONDARY_GROUPBOXES

void QMacPainter::setport()
{
    QPainter::initPaintDevice(TRUE);
    NormalizeThemeDrawingState(); //just to be extra sure
}

// Utility to generate correct rectangles for AppManager internals
static inline const Rect *qt_glb_mac_rect(const QRect &qr, const QPaintDevice *pd=NULL,
					  bool off=TRUE, const QRect &rect=QRect())
{
    static Rect r;
    bool use_rect = (rect.x() || rect.y() || rect.width() || rect.height());
    QPoint tl(qr.topLeft());
    if(pd && pd->devType() == QInternal::Widget) {
	QWidget *w = (QWidget*)pd;
	tl = w->mapTo(w->topLevelWidget(), tl);
    }
    if(use_rect)
	tl += rect.topLeft();
    int offset = 0;
    if(off)
	offset = 1;
    SetRect(&r, tl.x(), tl.y(), (tl.x() + qr.width()) - offset, (tl.y() + qr.height()) - offset);
    if(use_rect) {
	r.right -= rect.width();
	r.bottom -= rect.height();
    }
    return &r;
}
static inline const Rect *qt_glb_mac_rect(const QRect &qr, const QPainter *p,
					  bool off=TRUE, const QRect &rect=QRect())
{
    QPoint pt = qr.topLeft();
    QRect r(((QMacPainter*)p)->domap(pt.x(), pt.y()), qr.size());
    return qt_glb_mac_rect(r, p->device(), off, rect);
}

//utility to figure out the size (from the painter)
QAquaWidgetSize qt_mac_get_size_for_painter(QPainter *p)
{
    if(p && p->device()->devType() == QInternal::Widget)
	return qt_aqua_size_constrain((QWidget*)p->device());
    return qt_aqua_size_constrain(NULL);
}

//private
class QMacStyleFocusWidget : public QAquaFocusWidget
{
public:
    QMacStyleFocusWidget(QWidget *w) : QAquaFocusWidget(FALSE, w) { }

protected:
    void drawFocusRect(QMacPainter *p);

    virtual QRegion focusRegion();
    virtual void paintEvent(QPaintEvent *);
    virtual int focusOutset();
};
QRegion QMacStyleFocusWidget::focusRegion()
{
    const QRgb fillColor = qRgb(192, 191, 190);
    QImage img;
    {
	QPixmap pix(size(), 32);
	pix.fill(fillColor);
	QMacPainter p(&pix);
	drawFocusRect(&p);
	img = pix;
    }
    QImage mask(img.width(), img.height(), 1, 2, QImage::LittleEndian);
    for(int y = 0; y < img.height(); y++) {
	for(int x = 0; x < img.width(); x++) {
	    QRgb clr = img.pixel(x, y);
	    int diff = (((qRed(clr)-qRed(fillColor))*((qRed(clr)-qRed(fillColor)))) +
			((qGreen(clr)-qGreen(fillColor))*((qGreen(clr)-qGreen(fillColor)))) +
			((qBlue(clr)-qBlue(fillColor))*((qBlue(clr)-qBlue(fillColor)))));
	    mask.setPixel(x, y, diff < 100);
	}
    }
    QBitmap qmask;
    qmask = mask;
    return QRegion(qmask);
}
void QMacStyleFocusWidget::paintEvent(QPaintEvent *)
{
    QMacPainter p(this);
    drawFocusRect(&p);
}
void QMacStyleFocusWidget::drawFocusRect(QMacPainter *p)
{
    p->setport();
    int offset = focusOutset();
    QRect r(offset, offset, width() - (offset*2),
	    height() - (offset*2));
    DrawThemeFocusRect(qt_glb_mac_rect(r, p, TRUE, QRect(1, 1, 1, 1)), true);
}
int QMacStyleFocusWidget::focusOutset()
{
    SInt32 ret = 0;
    GetThemeMetric(kThemeMetricFocusRectOutset, &ret);
    return ret + 2;
}

class QMacStylePrivate : public QAquaAnimate
{
    QGuardedPtr<QMacStyleFocusWidget> focusWidget;
public:
    struct ButtonState {
	int frame;
	enum { ButtonDark, ButtonLight } dir;
    } buttonState;
    struct ProgressBarState {
	int frame;
    } progressbarState;
    struct ListViewItemState {
	QMap<QListViewItem*, int> lvis;
    } lviState;
    struct PolicyState {
	static QMap<QWidget*, QMacStyle::FocusRectPolicy> focusMap;
	static QMap<QWidget*, QMacStyle::WidgetSizePolicy> sizeMap;
	static void watchObject(QObject *o);
    };
    QMacStylePrivate();
    ~QMacStylePrivate();
protected:
    bool doAnimate(QAquaAnimate::Animates);
    void doFocus(QWidget *);
};
QMap<QWidget*, QMacStyle::FocusRectPolicy> QMacStylePrivate::PolicyState::focusMap;
QMap<QWidget*, QMacStyle::WidgetSizePolicy> QMacStylePrivate::PolicyState::sizeMap;
class QMacStylePrivateObjectWatcher : public QObject
{
    Q_OBJECT
public:
    QMacStylePrivateObjectWatcher(QObject *p) : QObject(p) { }
public slots:
    void destroyedObject(QObject *);
};

#include "qmacstyle_mac.moc"

void QMacStylePrivate::PolicyState::watchObject(QObject *o)
{
    static QGuardedPtr<QMacStylePrivateObjectWatcher> watcher;
    if(!watcher)
	watcher = new QMacStylePrivateObjectWatcher(NULL);
    QObject::connect(o, SIGNAL(destroyed(QObject*)), watcher, SLOT(destroyedObject(QObject*)));
}
void QMacStylePrivateObjectWatcher::destroyedObject(QObject *o)
{
    QMacStylePrivate::PolicyState::focusMap.remove((QWidget*)o);
    QMacStylePrivate::PolicyState::sizeMap.remove((QWidget*)o);
}
QMacStylePrivate::QMacStylePrivate() : QAquaAnimate()
{
    progressbarState.frame = 0;
    buttonState.frame = 0;
    buttonState.dir = ButtonState::ButtonDark;
}
QMacStylePrivate::~QMacStylePrivate()
{
    buttonState.frame = 0;
    buttonState.dir = ButtonState::ButtonDark;
    progressbarState.frame = 0;
}
bool QMacStylePrivate::doAnimate(QAquaAnimate::Animates as)
{
    if(as == AquaPushButton) {
	if(buttonState.frame == 10 && buttonState.dir == ButtonState::ButtonDark)
	    buttonState.dir = ButtonState::ButtonLight;
	else if(!buttonState.frame && buttonState.dir == ButtonState::ButtonLight)
	    buttonState.dir = ButtonState::ButtonDark;
	buttonState.frame += ((buttonState.dir == ButtonState::ButtonDark) ? 1 : -1);
    } else if(as == AquaProgressBar) {
	progressbarState.frame++;
    } else if(as == AquaListViewItemOpen) {
	for(QMap<QListViewItem*, int>::Iterator it = lviState.lvis.begin(); it != lviState.lvis.end(); ++it) {
	    QListViewItem *i = it.key();
	    int &frame = it.data();
	    if(i->isOpen()) {
		if(frame == 4) {
		    stopAnimate(AquaListViewItemOpen, i);
		    lviState.lvis.remove(it);
		    if(lviState.lvis.isEmpty())
			break;
		} else {
		    frame++;
		}
	    } else {
		if(frame == 0) {
		    stopAnimate(AquaListViewItemOpen, i);
		    lviState.lvis.remove(it);
		    if(lviState.lvis.isEmpty())
			break;
		} else {
		    frame--;
		}
	    }
	}
    }
    return TRUE;
}
void QMacStylePrivate::doFocus(QWidget *w)
{
    if(!focusWidget)
	focusWidget = new QMacStyleFocusWidget(w);
    focusWidget->setFocusWidget(w);
}

#define private public //ugh, what I'll do, guess we have to wait until 4.0
                       //to access positionToValue()..
#include <qslider.h>
#undef private

static int mac_count = 0;

/*!
    \class QMacStyle qmacstyle_mac.h
    \brief The QMacStyle class implements an Appearance Manager style.

    \ingroup appearance

    This class is implemented as a wrapper to the Apple Appearance
    Manager. This allows your application to be styled by whatever
    theme your Macintosh is using. This is done by having primitives
    in QStyle implemented in terms of what the Macintosh would
    normally theme (i.e. the Finder).

    There are additional issues that should be taken
    into consideration to make an application compatible with the
    \link http://developer.apple.com/documentation/UserExperience/Conceptual/OSXHIGuidelines/index.html
    Aqua Style Guidelines \endlink. Some of these issues are outlined
    below.

    \list

    \i Layout - The restrictions on window layout are such that some
    aspects of layout that are style-dependent cannot be achieved
    using QLayout. Changes are being considered (and feedback would be
    appreciated) to make layouts QStyle-able. Some of the restrictions
    involve horizontal and vertical widget alignment and widget size
    (covered below).

    \i Widget size - Aqua allows widgets to have specific fixed sizes.  Qt
    does not fully implement this behaviour so as to maintain multiplatform
    compatibility. As a result some widgets sizes may be inappropriate (and
    subsequently not rendered correctly by the Appearance Manager).The
    QWidget::sizeHint() will return the appropriate size for many
    managed widgets (widgets enumerated in \l QStyle::ContentsType).

    \i Effects - QMacStyle (in contrast to QAquaStyle) is not emulating
    (except where Appearance Manager does not provide certain
    capabilities), for example QPushButton pulsing effects. In this case a
    near matching emulation has been implemented, but naturally this will
    not be identical to the similar functionality built into the Appearance
    Manager. Please report any issues you see in effects or non-standard
    widgets.

    \endlist

    There are other issues that need to be considered in the feel of
    your application (including the general color scheme to match the
    Aqua colors). The Guidelines mentioned above will remain current
    with new advances and design suggestions for Mac OS X.

    Note that the functions provided by QMacStyle are
    reimplementations of QStyle functions; see QStyle for their
    documentation.
*/


/*!
    \enum QMacStyle::WidgetSizePolicy

    \value SizeSmall
    \value SizeLarge
    \value SizeNone
    \value SizeDefault
*/

/*!
    Constructs a QMacStyle object.
*/
QMacStyle::QMacStyle()  : QWindowsStyle()
{
    d = new QMacStylePrivate;
    if(!mac_count++)
	RegisterAppearanceClient();
}

/*!
    Destructs a QMacStyle object.
*/
QMacStyle::~QMacStyle()
{
    if(!(--mac_count))
	UnregisterAppearanceClient();
    delete d;
}

/*! \reimp */
void QMacStyle::polish(QApplication* app)
{
    QPalette pal = app->palette();
    QPixmap px(200, 200, 32);
    QColor pc(black);
    {
	QPainter p(&px);
	((QMacPainter *)&p)->setport();
	SetThemeBackground(kThemeBrushDialogBackgroundActive, px.depth(), true);
	EraseRect(qt_glb_mac_rect(QRect(0, 0, px.width(), px.height()), (QPaintDevice*)0, FALSE));
	RGBColor c;
	GetThemeBrushAsColor(kThemeBrushDialogBackgroundActive, 32, true, &c);
	pc = QColor(c.red / 256, c.green / 256, c.blue / 256);
    }
    QBrush background(pc, px);
    pal.setBrush(QColorGroup::Background, background);
    pal.setBrush(QColorGroup::Button, background);
    app->setPalette(pal, TRUE);
}

/*! \reimp */
void QMacStyle::polish(QWidget* w)
{
    if(!w->isTopLevel() && ::qt_cast<QSplitter*>(w) == 0 &&
       w->backgroundPixmap() &&
       qApp->palette().brush(QPalette::Active, QColorGroup::Background).pixmap() &&
	w->backgroundPixmap()->serialNumber() ==
       qApp->palette().brush(QPalette::Active, QColorGroup::Background).pixmap()->serialNumber())
	w->setBackgroundOrigin(QWidget::AncestorOrigin);
    d->addWidget(w);

#ifdef QMAC_DO_SECONDARY_GROUPBOXES
    QGroupBox *parentBox = ::qt_cast<QGroupBox*>(w->parentWidget());
    if(parentBox && (!parentBox->isFlat() && parentBox->frameShape() == QFrame::GroupBoxPanel)
       && !w->ownPalette()
       && ::qt_cast<QGroupBox*>(parentBox->parentWidget())
       && !::qt_cast<QGroupBox *>(parentBox->parentWidget())->isFlat()
       && qstrcmp(w->name(), "qt_groupbox_checkbox") != 0) {
	QPalette pal = w->palette();
	QPixmap px(200, 200, 32);
	QColor pc(white);
	{
	    QPainter p(&px);
	    ((QMacPainter *)&p)->setport();
	    Rect r; SetRect(&r, 0, 0, px.width(), px.height());
	    ApplyThemeBackground(kThemeBackgroundSecondaryGroupBox, &r,
				 kThemeStateActive, px.depth(), TRUE);
	    EraseRect(&r);
	}
	QBrush background(pc, px);
	pal.setBrush(QColorGroup::Background, background);
	pal.setBrush(QColorGroup::Button, background);
	w->setPalette(pal);
    }
#endif


    if(QLineEdit *lined = ::qt_cast<QLineEdit*>(w)) {
	SInt32 frame_size;
	GetThemeMetric(kThemeMetricEditTextFrameOutset, &frame_size);
	lined->setLineWidth(frame_size);
    } else if(QDialogButtons *btns = ::qt_cast<QDialogButtons*>(w)) {
	if(btns->buttonText(QDialogButtons::Help).isNull())
	    btns->setButtonText(QDialogButtons::Help, "?");
    } else if (QToolButton *btn = ::qt_cast<QToolButton*>(w)) {
        btn->setAutoRaise(FALSE);
    } else if(QToolBar *bar = ::qt_cast<QToolBar*>(w)) {
	QBoxLayout * layout = bar->boxLayout();
	layout->setSpacing(0);
	layout->setMargin(0);
    } else if(w->inherits("QTipLabel")) {   // QTipLabel is declared in qtooltip.cpp :-(
        QLabel *label = (QLabel*)w;
	label->setFrameStyle(QFrame::NoFrame);
	label->setLineWidth(1);
        label->setWindowOpacity(0.95);
    } else if(QPopupMenu *popup = ::qt_cast<QPopupMenu*>(w)) {
	popup->setMargin(0);
	popup->setLineWidth(0);
	w->setWindowOpacity(0.95);
    } else if(QTitleBar *tb = ::qt_cast<QTitleBar *>(w)) {
//	w->font().setPixelSize(10);
	tb->setAutoRaise(TRUE);
    }
    if(QScrollView *sv = ::qt_cast<QScrollView *>(w))
        sv->setLineWidth(1);
}

/*! \reimp */
void QMacStyle::unPolish(QWidget* w)
{
    d->removeWidget(w);
    if(QToolButton *btn = ::qt_cast<QToolButton*>(w))
        btn->setAutoRaise(TRUE);
    else if (::qt_cast<QPopupMenu*>(w))
	w->setWindowOpacity(1.0);
}

/*! \reimp */
void QMacStyle::drawItem(QPainter *p, const QRect &r,
			   int flags, const QColorGroup &cg, bool enabled,
			   const QPixmap *pixmap, const QString& text,
			   int len, const QColor* penColor) const
{
    if(flags & Qt::ShowPrefix)
        flags |= NoAccel;     //No accelerators drawn here!
    int x = r.x(), y = r.y(), w = r.width(), h = r.height();
    p->setPen(penColor ? *penColor : cg.foreground());
    if(pixmap) {
	QPixmap  pm(*pixmap);
	bool clip = (flags & Qt::DontClip) == 0;
	if(clip) {
	    if(pm.width() < w && pm.height() < h)
		clip = FALSE;
	    else
		p->setClipRect(r);
	}
	if((flags & Qt::AlignVCenter) == Qt::AlignVCenter)
	    y += h/2 - pm.height()/2;
	else if((flags & Qt::AlignBottom) == Qt::AlignBottom)
	    y += h - pm.height();
	if((flags & Qt::AlignRight) == Qt::AlignRight)
	    x += w - pm.width();
	else if((flags & Qt::AlignHCenter) == Qt::AlignHCenter)
	    x += w/2 - pm.width()/2;
	else if(((flags & Qt::AlignLeft) != Qt::AlignLeft) && QApplication::reverseLayout())
	    x += w - pm.width();

	if(!enabled) {
	    QString pmkey;
	    QTextOStream os(&pmkey);
	    const int w = pm.width(), h = pm.height();
	    os << "$qt_mac_style_draw_item_disabled_" << "_" << w << "x" << h << "_" << "_" << pm.serialNumber();
	    if(QPixmap *dblbuf = QPixmapCache::find(pmkey)) {
		pm = *dblbuf;
	    } else {
		QImage img; img = pm;
		for(int yy = 0; yy < h; yy++) {
		    for(int xx = 0; xx < w; xx++)
			img.setPixel(xx, yy, QColor(img.pixel(xx, yy)).light().rgb());
		}
		QPixmap *pix = new QPixmap(img);
		if(pm.mask())
		    pix->setMask(*pm.mask());
		pm = *pix;
		if(!QPixmapCache::insert(pmkey, pix))
		    delete pix;
	    }
	}
	p->drawPixmap(x, y, pm);
	if(clip)
	    p->setClipping(FALSE);
    } else if(!text.isNull()) {
	p->drawText(x, y, w, h, flags, text, len);
    }
}

/*! \reimp */
void QMacStyle::drawPrimitive(PrimitiveElement pe,
			       QPainter *p,
			       const QRect &r,
			       const QColorGroup &cg,
			       SFlags flags,
			       const QStyleOption& opt) const
{
    ThemeDrawState tds = kThemeStateActive;
    if(flags & Style_Down) {
	tds = kThemeStatePressed;
    } else if(qAquaActive(cg)) {
	if(!(flags & Style_Enabled))
	    tds = kThemeStateUnavailable;
    } else {
	if(flags & Style_Enabled)
	    tds = kThemeStateInactive;
	else
	    tds = kThemeStateUnavailableInactive;
    }

    switch(pe) {
	case PE_PanelTabWidget:
        if (qMacVersion() < Qt::MV_10_DOT_3) {
            QWindowsStyle::drawPrimitive(pe, p, r, cg, flags, opt);
        } else {
            bool tabsOnTop = TRUE;
            // Climb up to the tabwidget (yech)
            if (p && p->device() && p->device()->devType() == QInternal::Widget) {
                if (QTabWidget *tabWidget
                        = ::qt_cast<QTabWidget *>(((QWidget *)p->device())->parentWidget())) {
                    tabsOnTop = (tabWidget->tabPosition() != QTabWidget::Bottom);
                }
            }
            QRect wholePane = r;
            if (tabsOnTop)
                wholePane.setTop(wholePane.top() - 10);
            else
                wholePane.setBottom(wholePane.bottom() + 10);
            DrawThemePrimaryGroup(qt_glb_mac_rect(wholePane, p), tds);
        }
        break;
    case PE_Panel:
    case PE_PanelLineEdit: {
	if(flags & Style_Sunken) {
	    SInt32 frame_size;
            QColor baseColor(cg.background());
	    if(pe == PE_PanelLineEdit) {
		GetThemeMetric(kThemeMetricEditTextFrameOutset, &frame_size);
            } else {
                baseColor = QColor(150, 150, 150); //hardcoded since no query function --Sam
		GetThemeMetric(kThemeMetricListBoxFrameOutset, &frame_size);
            }

	    const int lw = opt.isDefault() ? pixelMetric(PM_DefaultFrameWidth) : opt.lineWidth();
            { //clear to base color
                p->save();
                p->setPen(QPen(baseColor, lw));
                p->setBrush(Qt::NoBrush);
                p->drawRect(r);
                p->restore();
            }

	    const Rect *rect = qt_glb_mac_rect(r, p, FALSE,
					       QRect(frame_size, frame_size, frame_size * 2, frame_size * 2));
	    ((QMacPainter *)p)->setport();
	    if(pe == PE_PanelLineEdit)
		DrawThemeEditTextFrame(rect, tds);
	    else
		DrawThemeListBoxFrame(rect, tds);
	} else {
	    QWindowsStyle::drawPrimitive(pe, p, r, cg, flags, opt);
	}
	break; }
    case PE_PanelGroupBox: {
	if(opt.isDefault())
	    break;
	QWidget *w = NULL;
	//This is terrible, if I we just passed the widget in this wouldn't be necesary!
	if (p && p->device() && p->device()->devType() == QInternal::Widget)
	    w = (QWidget*)p->device();
	((QMacPainter *)p)->setport();
#ifdef QMAC_DO_SECONDARY_GROUPBOXES
        QGroupBox *box = w ? ::qt_cast<QGroupBox *>(w->parentWidget()) : 0;
        if (box && !box->isFlat())
	    DrawThemeSecondaryGroup(qt_glb_mac_rect(r, p), kThemeStateActive);
	else
#endif
	    DrawThemePrimaryGroup(qt_glb_mac_rect(r, p), kThemeStateActive);
	break; }
    case PE_ArrowUp:
    case PE_ArrowDown:
    case PE_ArrowRight:
    case PE_ArrowLeft: {
	QPointArray a;
	if(pe == PE_ArrowDown)
	    a.setPoints(7, -4,-2, 2,-2, -3,-1, 1,-1, -2,0, 0,0, -1,1);
	else if(pe == PE_ArrowUp)
	    a.setPoints(7, -4,1, 2,1, -3,0, 1,0, -2,-1, 0,-1, -1,-2);
	else if(pe == PE_ArrowRight)
	    a.setPoints(7, -2,-3, -2,3, -1,-2, -1,2, 0,-1, 0,1, 1,0);
	else
	    a.setPoints(7, 0,-3, 0,3, -1,-2, -1,2, -2,-1, -2,1, -3,0);
	p->save();
#if 1
	if(flags & Style_Enabled) {
	    a.translate(r.x() + r.width() / 2, r.y() + r.height() / 2);
	    p->setPen(cg.text());
	    p->drawLineSegments(a, 0, 3);         // draw arrow
	    p->drawPoint(a[6]);
	} else {
	    a.translate(r.x() + r.width() / 2 + 1, r.y() + r.height() / 2 + 1);
	    p->setPen(cg.light());
	    p->drawLineSegments(a, 0, 3);         // draw arrow
	    p->drawPoint(a[6]);
	    a.translate(-1, -1);
	    p->setPen(cg.mid());
	    p->drawLineSegments(a, 0, 3);
	    p->drawPoint(a[6]);
	}
#else
	a.translate(r.x() + r.width() / 2, r.y() + r.height() / 2);
	p->setPen(cg.text());
	p->setBrush(cg.text());
	p->drawPolygon(a);
	p->setBrush(NoBrush);
#endif
	p->restore();
	break; }
    case PE_SizeGrip: {
	const Rect *rect = qt_glb_mac_rect(r, p);
	Point orig = { rect->top, rect->left };
	((QMacPainter *)p)->setport();
	ThemeGrowDirection dir = kThemeGrowRight | kThemeGrowDown;
#if 0
	if(QApplication::reverseLayout())
	    dir = kThemeGrowLeft | kThemeGrowDown;
#endif
	DrawThemeStandaloneGrowBox(orig, dir, false, kThemeStateActive);
	break; }
    case PE_FocusRect:
	break;     //This is not used because of the QAquaFocusWidget thingie..
    case PE_TabBarBase: {
	((QMacPainter *)p)->setport();
        if (qMacVersion() < Qt::MV_10_DOT_3) {
            DrawThemeTabPane(qt_glb_mac_rect(r, p), tds);
        } else {
            QRect newRect;
            if (flags & QStyle::Style_Bottom)
                newRect.setRect(r.x(), r.y() - 10, r.width(), r.height() + 10);
            else
                newRect.setRect(r.x(), r.y(), r.width(), r.height() + 10);
            DrawThemePrimaryGroup(qt_glb_mac_rect(newRect, p), tds);
        }
	break;
    }
    case PE_HeaderArrow:
#ifndef QT_NO_TABLE
        if (p && p->device() && p->device()->devType() == QInternal::Widget) {
            if (::qt_cast<QTable*>(((QWidget*)p->device())->parentWidget())) {
                DrawThemePopupArrow(qt_glb_mac_rect(r, p),
                                    flags & Style_Up ? kThemeArrowDown : kThemeArrowUp,
                                    kThemeArrow9pt, tds, 0, 0);
            }
        }
#endif
        // else drawn in HeaderSection.
	break;
    case PE_HeaderSection: {
        ThemeButtonKind bkind = kThemeListHeaderButton;
#ifndef QT_NO_TABLE
        // Grab the widget behind this thing yet again and check if it's parent is a table.
        // We do this because the kThemeListHeader apparently doesn't extend vertically.
        // Also change the sunken flag to true for items that are selected. Because of
        // design decisions we have to explictily turn it off for every header section.
        // We can tell if something is selected by looking at the boldness of the font,
        // icky, but it does the job.
        if (p && p->device() && p->device()->devType() == QInternal::Widget) {
            if (::qt_cast<QTable*>(((QWidget*)p->device())->parentWidget())) {
                bkind = kThemeBevelButton;
                if (p->font().bold())
                    flags |= Style_Sunken;
                else
                    flags &= ~Style_Sunken;
            }
        }
#endif
	ThemeButtonDrawInfo info = { kThemeStateActive, kThemeButtonOff, kThemeAdornmentNone };

        if(flags & Style_HasFocus)
	    info.adornment |= kThemeAdornmentFocus;
	if(qAquaActive(cg)) {
	    if(!(flags & Style_Enabled))
		info.state = kThemeStateUnavailable;
	    else if(flags & Style_Down)
		info.state = kThemeStatePressed;
	} else {
	    if(flags & Style_Enabled)
		info.state = kThemeStateInactive;
	    else
		info.state = kThemeStateUnavailableInactive;
	}
	if(flags & Style_Sunken
                || (!(flags & Style_Sunken) && (flags & Style_Down))) // kludge for consistency.
	    info.value = kThemeButtonOn;

	QRect ir = r;
        if (bkind == kThemeListHeaderButton) {
            if((flags & Style_Off))
                ir.setRight(ir.right() + 50);
            else if((flags & Style_Up))
                info.adornment |= kThemeAdornmentHeaderButtonSortUp;
            SInt32 headerHeight = 0;
            GetThemeMetric(kThemeMetricListHeaderHeight, &headerHeight);
            if (ir.height() > headerHeight) {
                QPixmap headerPix(ir.width(), headerHeight);
                QPainter pixPainter(&headerPix);
                Rect pixRect = *qt_glb_mac_rect(QRect(0, 0, ir.width(), headerHeight),
                                                &pixPainter, false);
                ((QMacPainter *)&pixPainter)->setport();
                DrawThemeButton(&pixRect, bkind, &info, 0, 0, 0, 0);
                p->drawPixmap(ir, headerPix);
                break;
            }
        }
	((QMacPainter *)p)->setport();
	DrawThemeButton(qt_glb_mac_rect(ir, p, FALSE), bkind, &info, 0, 0, 0, 0);
	break; }
    case PE_CheckListController:
	break;
    case PE_CheckListExclusiveIndicator:
    case PE_ExclusiveIndicatorMask:
    case PE_ExclusiveIndicator: {
	ThemeButtonDrawInfo info = { tds, kThemeButtonOff, kThemeAdornmentDrawIndicatorOnly };
	if(flags & Style_HasFocus)
	    info.adornment |= kThemeAdornmentFocus;
	if(flags & Style_On)
	    info.value = kThemeButtonOn;
	ThemeButtonKind bkind = kThemeRadioButton;
	if(qt_mac_get_size_for_painter(p) == QAquaSizeSmall)
	    bkind = kThemeSmallRadioButton;
	if(pe == PE_ExclusiveIndicatorMask) {
	    p->save();
	    QRegion rgn;
	    GetThemeButtonRegion(qt_glb_mac_rect(r, p, FALSE), bkind, &info, rgn.handle(TRUE));
	    p->setClipRegion(rgn);
	    p->fillRect(r, color1);
	    p->restore();
	} else {
	    ((QMacPainter *)p)->setport();
	    DrawThemeButton(qt_glb_mac_rect(r, p, FALSE), bkind, &info, NULL, NULL, NULL, 0);
	}
	break; }
    case PE_CheckListIndicator:
    case PE_IndicatorMask:
    case PE_Indicator: {
	ThemeButtonDrawInfo info = { tds, kThemeButtonOff, kThemeAdornmentDrawIndicatorOnly };
	if(flags & Style_HasFocus)
	    info.adornment |= kThemeAdornmentFocus;
	if(flags & Style_NoChange)
	    info.value = kThemeButtonMixed;
	else if(flags & Style_On)
	    info.value = kThemeButtonOn;
	ThemeButtonKind bkind = kThemeCheckBox;
	if(qt_mac_get_size_for_painter(p) == QAquaSizeSmall)
	    bkind = kThemeSmallCheckBox;
	if(pe == PE_IndicatorMask) {
	    p->save();
	    QRegion rgn;
	    GetThemeButtonRegion(qt_glb_mac_rect(r, p, false), bkind,
				 &info, rgn.handle(TRUE));
	    p->setClipRegion(rgn);
	    p->fillRect(r, color1);
	    p->restore();
	} else {
	    ((QMacPainter *)p)->setport();
	    DrawThemeButton(qt_glb_mac_rect(r, p, false), bkind,
			    &info, NULL, NULL, NULL, 0);
	}
	break; }
    case PE_RubberBand:
	p->fillRect(r, cg.highlight());
	break;
    default:
	QWindowsStyle::drawPrimitive(pe, p, r, cg, flags, opt);
	break;
    }
}

static void qt_mac_drawTab(QPainter *p, const QWidget *w, const QRect &ir, ThemeTabStyle tts, ThemeTabDirection ttd)
{
    if (ir.height() > kThemeLargeTabHeightMax) {
        QPixmap tabPix(ir.width(), kThemeLargeTabHeightMax);
        QPainter pixPainter(&tabPix);
        const QPixmap *bgPix = w->backgroundPixmap();
        if (bgPix)
            pixPainter.drawPixmap(0, 0, *bgPix);
        else
            tabPix.fill(w->backgroundColor());
        Rect pixRect = *qt_glb_mac_rect(QRect(0, 0, ir.width(), kThemeLargeTabHeightMax),
                                        &pixPainter, false);
        ((QMacPainter *)&pixPainter)->setport();
	DrawThemeTab(&pixRect, tts, ttd, 0, 0);
        p->drawPixmap(ir, tabPix);
    } else {
        ((QMacPainter *)p)->setport();
        DrawThemeTab(qt_glb_mac_rect(ir, p, false), tts, ttd, 0, 0);
    }
}

enum { TabNormalLeft, TabNormalMid, TabNormalRight, TabSelectedActiveLeft,
    TabSelectedActiveMid, TabSelectedActiveRight, TabSelectedInactiveLeft,
    TabSelectedInactiveMid, TabSelectedInactiveRight, TabSelectedActiveGraphiteLeft,
    TabSelectedActiveGraphiteMid, TabSelectedActiveGraphiteRight,
    TabPressedLeft, TabPressedMid, TabPressedRight };

static const char * const * const PantherTabXpms[] = {
                                    qt_mac_tabnrm_left,
                                    qt_mac_tabnrm_mid,
                                    qt_mac_tabnrm_right,
                                    qt_mac_tabselected_active_left,
                                    qt_mac_tabselected_active_mid,
                                    qt_mac_tabselected_active_right,
                                    qt_mac_tabselected_inactive_left,
                                    qt_mac_tabselected_inactive_mid,
                                    qt_mac_tabselected_inactive_right,
                                    qt_mac_tab_selected_active_graph_left,
                                    qt_mac_tab_selected_active_graph_mid,
                                    qt_mac_tab_selected_active_graph_right,
                                    qt_mac_tab_press_left,
                                    qt_mac_tab_press_mid,
                                    qt_mac_tab_press_right};


static void drawPantherTab(QTabBar *tabbar, const QRect &tabRect, QStyle::SFlags state, QPainter *p,
                           const QTab *tab)
{
    QString tabKey = "$qt_mac_style_tab_";
    int pantherTabStart;
    int pantherTabMid;
    int pantherTabEnd;

    ThemeTabDirection ttd;
    switch (tabbar->shape()) {
    default:
        ttd = kThemeTabNorth;
        break;
    case QTabBar::RoundedAbove:
    case QTabBar::TriangularAbove:
        ttd = kThemeTabSouth;
        break;
    }

    if (state & QStyle::Style_Selected) {
        if (!(state & QStyle::Style_Active)) {
            pantherTabStart = TabSelectedInactiveLeft;
        } else {
            QPixmap tabPix(20, 20);
            QPainter pixPainter(&tabPix);
            qt_mac_drawTab(&pixPainter, 0, QRect(0, 0, 20, 20), kThemeTabFront, kThemeTabNorth);
            pixPainter.end();
            const QRgb GraphiteColor = 0xffa7b0ba;
            QRgb pmColor = tabPix.convertToImage().pixel(10, 10);
            if (QABS(qRed(pmColor) - qRed(GraphiteColor)) < 3
                    && QABS(qGreen(pmColor) - qGreen(GraphiteColor)) < 3
                    && QABS(qBlue(pmColor) - qBlue(GraphiteColor)) < 3)
                pantherTabStart = TabSelectedActiveGraphiteLeft;
            else
                pantherTabStart = TabSelectedActiveLeft;
        }
    } else if ((state & (QStyle::Style_Sunken | QStyle::Style_MouseOver))
                    == (QStyle::Style_Sunken | QStyle::Style_MouseOver)) {
        pantherTabStart = TabPressedLeft;
    } else {
        pantherTabStart = TabNormalLeft;
    }

    enum TabPosition { TabBeginning, TabMiddle, TabEnd, OnlyOne, TabUnknown };
    TabPosition tabPos = OnlyOne;
    int i = 0;

    if (tabbar->count() > 1) {
        for (i = 0; i < tabbar->count(); ++i) {
            const QTab *checktab = tabbar->tabAt(i);
            if (tab == checktab) {
                if (i == 0)
                    tabPos = TabBeginning;
                else if (i == tabbar->count() - 1)
                    tabPos = TabEnd;
                else
                    tabPos = TabMiddle;
                break;
            }
        }
    }

    bool frontLine;
    bool backLine;
    switch (tabPos) {
    default:
    case TabBeginning:
        frontLine = false;
        backLine = true;
        pantherTabMid = pantherTabEnd = pantherTabStart + 1;
        break;
    case TabMiddle:
        frontLine = backLine = true;
        pantherTabMid = pantherTabEnd = ++pantherTabStart;
        break;
    case TabEnd:
        backLine = false;
        frontLine = true;
        pantherTabMid = ++ pantherTabStart;
        pantherTabEnd = pantherTabMid + 1;
        break;
    case OnlyOne:
        frontLine = backLine = false;
        pantherTabMid = pantherTabStart + 1;
        pantherTabEnd = pantherTabMid + 1;
        break;
    }
    QPixmap pmStart;
    if (!QPixmapCache::find(tabKey + QString::number(pantherTabStart), pmStart)) {
        pmStart = QPixmap(PantherTabXpms[pantherTabStart]);
        QPixmapCache::insert(tabKey + QString::number(pantherTabStart), pmStart);
    }

    QPixmap pmMid;
    if (!QPixmapCache::find(tabKey + QString::number(pantherTabMid), pmMid)) {
        pmMid = QPixmap(PantherTabXpms[pantherTabMid]);
        QPixmapCache::insert(tabKey + QString::number(pantherTabMid), pmMid);
    }

    QPixmap pmEnd;
    if (!QPixmapCache::find(tabKey + QString::number(pantherTabEnd), pmEnd)) {
        pmEnd = QPixmap(PantherTabXpms[pantherTabEnd]);
        QPixmapCache::insert(tabKey + QString::number(pantherTabEnd), pmEnd);
    }
    QRect tr = tabRect;
    int x = tr.x();
    int y = tr.y();
    int endX = tr.right() + 1 - pmEnd.width();

    p->drawPixmap(QRect(x, y, pmStart.width(), tr.height()), pmStart);

    for (x = x + pmStart.width(); x < endX; x += pmMid.width())
        p->drawPixmap(QRect(x, y, pmMid.width(), tr.height()), pmMid);
    p->drawPixmap(QRect(endX, y, pmEnd.width(), tr.height()), pmEnd);

    if (frontLine || backLine) {
        QPen oldPen = p->pen();
        // I don't have alpha, so I must fake the color on my own.
        static const QColor ActiveColor = QColor(64, 134, 213);
        static const QColor InactiveColor = QColor(182, 182, 182);
        bool iamActive = tabbar->currentTab() == i && (state & QStyle::Style_Active);
        bool leftGuyActive = tabbar->currentTab() == i - 1 && (state & QStyle::Style_Active);
        if (frontLine) {
            p->setPen(((iamActive || leftGuyActive) && pantherTabMid == TabSelectedActiveMid)
                       ? ActiveColor : InactiveColor);
            p->drawLine(tr.x(), y + 1, tr.x(), tr.height() - 2);
        }
        if (backLine) {
            p->setPen((iamActive && pantherTabMid == TabSelectedActiveMid) ? ActiveColor
                                                                           : InactiveColor);
            p->drawLine(tr.right() + 1, y + 1, tr.right() + 1, tr.height() - 2);
        }
        p->setPen(oldPen);
    }
}

/*! \reimp */
void QMacStyle::drawControl(ControlElement element,
				 QPainter *p,
				 const QWidget *widget,
				 const QRect &r,
				 const QColorGroup &cg,
				 SFlags how,
				 const QStyleOption& opt) const
{
    ThemeDrawState tds = kThemeStateActive;
    if(how & Style_Down) {
	tds = kThemeStatePressed;
    } else if(qAquaActive(cg)) {
	if(!(how & Style_Enabled))
	    tds = kThemeStateUnavailable;
    } else {
	if(how & Style_Enabled)
	    tds = kThemeStateInactive;
	else
	    tds = kThemeStateUnavailableInactive;
    }

    switch(element) {
    case CE_ProgressBarLabel:
	break; //nothing to be drawn here..
    case CE_ToolBoxTab:
	QCommonStyle::drawControl(element, p, widget, r, cg, how, opt);
	break;
    case CE_PopupMenuHorizontalExtra:
    case CE_PopupMenuVerticalExtra:
    case CE_PopupMenuScroller: {
	Rect mrect = *qt_glb_mac_rect(widget->rect(), p),
	     irect = *qt_glb_mac_rect(r, p, FALSE);
	ThemeMenuState tms = kThemeMenuActive;
	ThemeMenuItemType tmit = kThemeMenuItemPlain;
	if(element == CE_PopupMenuScroller) {
	    if(how & Style_Active)
		tms |= kThemeMenuSelected;
	    if((how & Style_Down))
		tmit = kThemeMenuItemScrollDownArrow;
	    else
		tmit = kThemeMenuItemScrollUpArrow;
	}
	((QMacPainter *)p)->setport();
	DrawThemeMenuItem(&mrect, &irect, mrect.top, mrect.bottom, tms, tmit, NULL, 0);
	break; }
    case CE_MenuBarEmptyArea:
	p->fillRect(r, cg.brush(QColorGroup::Button));
	((QMacPainter*)p)->setport();
	DrawThemeMenuBarBackground(qt_glb_mac_rect(r, p, FALSE), kThemeMenuBarNormal,
				   kThemeMenuSquareMenuBar);
	break;
    case CE_PopupMenuItem: {
	if(!widget || opt.isDefault())
	    break;
	QPopupMenu *popupmenu = (QPopupMenu *)widget;
	QMenuItem *mi = opt.menuItem();

	const QColorGroup & g = cg;
	QColorGroup itemg = g;
	bool dis = mi ? !mi->isEnabled() : FALSE;
	int tab = opt.tabWidth();
	int maxpmw = opt.maxIconWidth();
	bool checked = mi ? mi->isChecked() : FALSE;
	bool checkable = popupmenu->isCheckable();
	bool act = how & Style_Active;
	Rect mrect = *qt_glb_mac_rect(popupmenu->rect(), p),
	     irect = *qt_glb_mac_rect(r, p, FALSE);

	if(checkable)
	    maxpmw = QMAX(maxpmw, 12); // space for the checkmarks

	ThemeMenuState tms = kThemeMenuActive;
	if(dis)
	    tms |= kThemeMenuDisabled;
	if(how & Style_Active)
	    tms |= kThemeMenuSelected;
	ThemeMenuItemType tmit = kThemeMenuItemPlain;
	if(mi && mi->popup())
	    tmit |= kThemeMenuItemHierarchical;
	if(mi && mi->iconSet())
	    tmit |= kThemeMenuItemHasIcon;
	((QMacPainter *)p)->setport();
	DrawThemeMenuItem(&mrect, &irect, mrect.top, mrect.bottom, tms, tmit, NULL, 0);

	if(mi && mi->isSeparator()) {
	    ((QMacPainter *)p)->setport();
	    DrawThemeMenuSeparator(&irect);
	    return;
	}

	int x, y, w, h;
	r.rect(&x, &y, &w, &h);
	int checkcol = maxpmw;
	bool reverse = QApplication::reverseLayout();
	int xpos = x;
	if(reverse)
	    xpos += w - checkcol;
	if(mi && mi->iconSet()) {              // draw iconset
	    if(checked) {
		QRect vrect = visualRect(QRect(xpos, y, checkcol, h), r);
		if(act && !dis) {
		    qDrawShadePanel(p, vrect.x(), y, checkcol, h,
				     cg, TRUE, 1, &cg.brush(QColorGroup::Button));
		} else {
		    QBrush fill(cg.light(), Dense4Pattern);
		    // set the brush origin for the hash pattern to the x/y coordinate
		    // of the menu item's checkmark... this way, the check marks have
		    // a consistent look
		    QPoint origin = p->brushOrigin();
		    p->setBrushOrigin(vrect.x(), y);
		    qDrawShadePanel(p, vrect.x(), y, checkcol, h, cg, TRUE, 1,
				     &fill);
		    // restore the previous brush origin
		    p->setBrushOrigin(origin);
		}
	    }

	    QIconSet::Mode mode = dis ? QIconSet::Disabled : QIconSet::Normal;
	    if(act && !dis)
		mode = QIconSet::Active;
	    QPixmap pixmap;
	    if(mi) {
		if(checkable && checked)
		    pixmap = mi->iconSet()->pixmap(QIconSet::Small, mode, QIconSet::On);
		else
		    pixmap = mi->iconSet()->pixmap(QIconSet::Small, mode);
	    }
	    int pixw = pixmap.width();
	    int pixh = pixmap.height();
	    if(act && !dis && checked)
		qDrawShadePanel(p, xpos, y, checkcol, h, g, FALSE, 1, &g.brush(QColorGroup::Button));
	    QRect cr(xpos, y, checkcol, h);
	    QRect pmr(0, 0, pixw, pixh);
	    pmr.moveCenter(cr.center());
	    p->setPen(itemg.text());
	    p->drawPixmap(pmr.topLeft(), pixmap);
	} else  if(checkable && checked) {  // just "checking"...
	    int mw = checkcol + macItemFrame;
	    int mh = h - 2*macItemFrame;
	    int xp = xpos;
	    if(reverse)
		xp -= macItemFrame;
	    else
		xp += macItemFrame;

	    SFlags cflags = Style_Default;
	    if(!dis)
		cflags |= Style_Enabled;
	    if(act)
		cflags |= Style_On;
	    drawPrimitive(PE_CheckMark, p, QRect(xp, y+macItemFrame, mw, mh), cg, cflags);
	}

	if(dis)
	    p->setPen(itemg.text());
	else if(act)
	    p->setPen(g.highlightedText());
	else
	    p->setPen(g.buttonText());

	int xm = macItemFrame + checkcol + macItemHMargin;
	if(reverse)
	    xpos = macItemFrame + tab;
	else
	    xpos += xm;

	if(mi && mi->custom()) {
	    int m = macItemVMargin;
	    mi->custom()->paint(p, itemg, act, !dis,
				 x+xm, y+m, w-xm-tab+1, h-2*m);
	}
	if(mi) {
	    QString s = mi->text();
	    if (!s.isNull()) {                        // draw text
		int t = s.find('\t');
		int m = macItemVMargin;
		int text_flags = AlignRight | AlignVCenter | NoAccel | DontClip | SingleLine;
		if (t >= 0) {                         // draw tab text
		    int xp;
		    if (reverse)
			xp = x + macRightBorder+macItemHMargin+macItemFrame - 1;
		    else
			xp = x + w - tab - macRightBorder-macItemHMargin-macItemFrame + 1;
		    QFont font(p->font());
		    int oldWeight = font.weight();
		    font.setWeight(QFont::Bold);
		    p->setFont(font);
		    p->drawText(xp, y + m, tab, h - 2 * m, text_flags, s.mid(t + 1));
		    s = s.left(t);
		    font.setWeight(oldWeight);
		    p->setFont(font);
		}
		text_flags ^= AlignRight;
		p->drawText(xpos, y+m, w-xm-tab+1, h-2*m, text_flags, s, t);
	    } else if(mi->pixmap()) {                        // draw pixmap
		QPixmap *pixmap = mi->pixmap();
		if(pixmap->depth() == 1)
		    p->setBackgroundMode(OpaqueMode);
		p->drawPixmap(xpos, y+macItemFrame, *pixmap);
		if(pixmap->depth() == 1)
		    p->setBackgroundMode(TransparentMode);
	    }
	}
	break; }
    case CE_MenuBarItem: {
	if(!widget)
	    break;
	const QMenuBar *mbar = (const QMenuBar *)widget;
	QRect ir(r.x(), 0, r.width(), mbar->height());
	Rect mrect = *qt_glb_mac_rect(mbar->rect(), p),
	     irect = *qt_glb_mac_rect(ir, p, FALSE);
	ThemeMenuState tms = kThemeMenuActive;
	if(!(how & Style_Active))
	    tms |= kThemeMenuDisabled;
	if(how & Style_Down)
	    tms |= kThemeMenuSelected;
	((QMacPainter *)p)->setport();
	DrawThemeMenuTitle(&mrect, &irect, tms, 0, NULL, 0);
	QCommonStyle::drawControl(element, p, widget, r, cg, how, opt);
	break; }
    case CE_ProgressBarContents: {
	if(!widget)
	    break;
	QProgressBar *pbar = (QProgressBar *) widget;
	ThemeTrackDrawInfo ttdi;
	memset(&ttdi, '\0', sizeof(ttdi));

        switch (qt_aqua_size_constrain(widget)) {
            case QAquaSizeUnknown:
            case QAquaSizeLarge:
                ttdi.kind = pbar->totalSteps() ? kThemeLargeProgressBar : kThemeLargeIndeterminateBar;
                break;
            case QAquaSizeSmall:
                ttdi.kind = pbar->totalSteps() ? kThemeProgressBar : kThemeIndeterminateBar;
                break;
            }
	ttdi.bounds = *qt_glb_mac_rect(r, p);
	ttdi.max = pbar->totalSteps();
	ttdi.value = pbar->progress();
	ttdi.attributes |= kThemeTrackHorizontal;
	ttdi.trackInfo.progress.phase = d->progressbarState.frame;
	if(!qAquaActive(cg))
	    ttdi.enableState = kThemeTrackInactive;
	else if(!pbar->isEnabled())
	    ttdi.enableState = kThemeTrackDisabled;
	((QMacPainter *)p)->setport();
	DrawThemeTrack(&ttdi, NULL, NULL, 0);
	break; }
    case CE_TabBarTab: {
	if(!widget)
	    break;
	QTabBar * tb = (QTabBar *) widget;
        if (qMacVersion() < Qt::MV_10_DOT_3) {
            ThemeTabStyle tts = kThemeTabNonFront;
            if(how & Style_Selected) {
                if(!qAquaActive(cg))
                    tts = kThemeTabFrontUnavailable;
                else if(!(how & Style_Enabled))
                    tts = kThemeTabFrontInactive;
                else
                    tts = kThemeTabFront;
            } else if(!qAquaActive(cg)) {
                tts = kThemeTabNonFrontUnavailable;
            } else if(!(how & Style_Enabled)) {
                tts = kThemeTabNonFrontInactive;
            } else if((how & Style_Sunken) && (how & Style_MouseOver)) {
                tts = kThemeTabNonFrontPressed;
            }
            ThemeTabDirection ttd = kThemeTabNorth;
            if(tb->shape() == QTabBar::RoundedBelow || tb->shape() == QTabBar::TriangularBelow)
                ttd = kThemeTabSouth;
            QRect tabr(r.x(), r.y(), r.width(), r.height() + pixelMetric(PM_TabBarBaseOverlap, widget));
            if(ttd == kThemeTabSouth)
                tabr.moveBy(0, -pixelMetric(PM_TabBarBaseOverlap, widget));
            qt_mac_drawTab(p, widget, tabr, tts, ttd);
            if(!(how & Style_Selected)) {
                //The "fudge" is just so the left and right side doesn't
                //get drawn onto my widget (not sure it will really happen)
                const int fudge = 20;
                QRect pr = QRect(r.x() - fudge, r.bottom() - 2,
                        r.width() + (fudge * 2), pixelMetric(PM_TabBarBaseHeight, widget));
                if(ttd == kThemeTabSouth)
                    pr.moveBy(0, -(r.height() + 2));
                p->save();
                p->setClipRect(QRect(pr.x() + fudge, pr.y(), pr.width() - (fudge * 2), pr.height()));
                ((QMacPainter *)p)->setport();
                ThemeDrawState tabpane_tds = kThemeStateActive;
                if(qAquaActive(cg)) {
                    if(!tb->isEnabled())
                        tabpane_tds = kThemeStateUnavailable;
                } else {
                    if(tb->isEnabled())
                        tabpane_tds = kThemeStateInactive;
                    else
                        tabpane_tds = kThemeStateUnavailableInactive;
                }
                DrawThemeTabPane(qt_glb_mac_rect(pr, p), tabpane_tds);
                p->restore();
            }
        } else {
            // Panther style tabs
            drawPantherTab(tb, r, how, p, opt.tab());
        }
	break; }
    case CE_TabBarLabel:
        if (qMacVersion() >= Qt::MV_10_DOT_3) {
            QRect tr = r;
            if (const QTabBar *tb = ::qt_cast<QTabBar *>(widget)) {
                if (!opt.isDefault()) {
                    const QTab *t = opt.tab();
                    if (t->identifier() == tb->currentTab())
                        tr.setBottom(tr.bottom() + pixelMetric(PM_DefaultFrameWidth, tb));
                }
            }
            tr.setTop(tr.top() + 1);
            tr.setLeft(tr.left() + 3); // Workaround QTabBar's 3 pixel shift.
            QWindowsStyle::drawControl(element, p, widget, tr, cg, how, opt);
        } else {
            QWindowsStyle::drawControl(element, p, widget, r, cg, how, opt);
        }
        break;
    case CE_PushButton: {
	if(!widget)
	    break;
	QPushButton *btn = (QPushButton *)widget;
	if(btn->isFlat() && !(how & (Style_Down | Style_On))) {
	    btn->erase();
	    break;
	}

	d->addWidget(btn);
	QString pmkey;
	bool do_draw = FALSE;
	QPixmap *buffer = NULL;
	bool darken = d->animatable(QAquaAnimate::AquaPushButton, (QWidget *)widget);
	int frame = d->buttonState.frame;
	if(btn->isToggleButton() && btn->isOn()) {
	    darken = TRUE;
	    frame = 12;
	    if((how & Style_Down))
		frame += 8;
	} else if(how & Style_Down) {
	    darken = FALSE;
	    frame = 0;
	}
	if(darken && qAquaActive(cg)) {
	    QTextOStream os(&pmkey);
	    os << "$qt_mac_pshbtn_" << r.width() << "x" << r.height() << "_" << how << "_" << frame;
	    tds = kThemeStatePressed;
	    if(frame && !(buffer = QPixmapCache::find(pmkey))) {
		do_draw = TRUE;
		buffer = new QPixmap(r.width(), r.height(), 32);
		buffer->fill(color0);
	    }
	}

	ThemeButtonKind bkind = kThemePushButton;
	ThemeButtonDrawInfo info = { tds, kThemeButtonOff, kThemeAdornmentNone };
	if(how & Style_HasFocus)
	    info.adornment |= kThemeAdornmentFocus;

	QRect off_rct(0, 0, 0, 0);
        { //The AppManager draws outside my rectangle, so account for that difference..
	    Rect macRect, myRect;
	    SetRect(&myRect,r.x(), r.y(), r.width(), r.height());
	    GetThemeButtonBackgroundBounds(&myRect, bkind, &info, &macRect);
	    off_rct = QRect(myRect.left - macRect.left, myRect.top - macRect.top,
			    (myRect.left - macRect.left) + (macRect.right - myRect.right),
			    (myRect.top - macRect.top) + (macRect.bottom - myRect.bottom));
	}

	((QMacPainter *)p)->setport();
	DrawThemeButton(qt_glb_mac_rect(r, p, FALSE, off_rct),
			bkind, &info, NULL, NULL, NULL, 0);
	if(buffer) {
	    if(do_draw && frame) {
		QMacSavedPortInfo savedInfo(buffer);
		const Rect *buff_rct = qt_glb_mac_rect(QRect(0, 0, r.width(), r.height()),
						       buffer, FALSE, off_rct);
		DrawThemeButton(buff_rct, bkind, &info, NULL, NULL, NULL, 0);

		QPixmap buffer_mask(buffer->size(), 32);
		{
		    buffer_mask.fill(color0);
		    ThemeButtonDrawInfo mask_info = info;
		    mask_info.state = kThemeStateActive;
		    QMacSavedPortInfo savedInfo(&buffer_mask);
		    DrawThemeButton(buff_rct, bkind, &mask_info, NULL, NULL, NULL, 0);
		}

		QImage img = buffer->convertToImage(),
                   maskimg = buffer_mask.convertToImage();
		QImage mask_out(img.width(), img.height(), 1, 2, QImage::LittleEndian);
		for(int y = 0; y < img.height(); y++) {
		    //calculate a mask
		    for(int maskx = 0; maskx < img.width(); maskx++) {
			QRgb in = img.pixel(maskx, y), out = maskimg.pixel(maskx, y);
			int diff = (((qRed(in)-qRed(out))*((qRed(in)-qRed(out)))) +
				    ((qGreen(in)-qGreen(out))*((qGreen(in)-qGreen(out)))) +
				    ((qBlue(in)-qBlue(out))*((qBlue(in)-qBlue(out)))));
			mask_out.setPixel(maskx, y, diff > 100);
		    }
		    //pulse the colours
		    uchar *bytes = img.scanLine(y);
		    for(int x = 0; x < img.bytesPerLine(); ++x)
			*(bytes + x) = (*(bytes + x) * (64 - frame)) / 64;
		}
		*buffer = img;
		{
		    QBitmap qmask;
		    qmask = mask_out;
		    buffer->setMask(qmask);
		}
	    }
	    p->drawPixmap(r, *buffer);
	    if(do_draw && !QPixmapCache::insert(pmkey, buffer))	// save in cache
		delete buffer;
	}
	break; }
    case CE_PushButtonLabel: {
        if (!widget)
            break;
        const QPushButton *btn = (const QPushButton *)widget;

        // We really don't want the label to be drawn the same as on
        // windows style if it has an icon, then it should be more like a
        // tab. So, cheat a little here.
        if ((!btn->iconSet() || btn->iconSet()->isNull())
                && (!btn->pixmap() || btn->pixmap()->isNull())) {
            QWindowsStyle::drawControl(element, p, btn, r, cg, how, opt);
        } else {
            QRect ir = r;
            if (btn->isDown() || btn->isOn()) {
                how |= Style_Sunken;
		ir.moveBy(pixelMetric(PM_ButtonShiftHorizontal, widget),
			  pixelMetric(PM_ButtonShiftVertical, widget));
            }
	    if (btn->isMenuButton()) {
		int mbi = pixelMetric(PM_MenuButtonIndicator, widget);
		QRect ar(ir.right() - mbi, ir.y() + 2, mbi - 4, ir.height() - 4);
		drawPrimitive(PE_ArrowDown, p, ar, cg, how, opt);
		ir.setWidth(ir.width() - mbi);
	    }

            QPixmap pixmap;
            QRect br = p->boundingRect(ir, Qt::AlignCenter, btn->text());
            if (btn->iconSet() && !btn->iconSet()->isNull()) {
                QIconSet::Mode mode = how & Style_Enabled ? QIconSet::Normal : QIconSet::Disabled;
                if (mode == QIconSet::Normal && how & Style_HasFocus)
                    mode = QIconSet::Active;
                QIconSet::State state = QIconSet::Off;
                if (how & Style_On)
                    state = QIconSet::On;
                pixmap = btn->iconSet()->pixmap(QIconSet::Small, mode, state);
            } else {
                // Must be a pixmap
                pixmap = *btn->pixmap();

            }
            int pixw = pixmap.width();
            int pixh = pixmap.height();
            QPoint btl = br.isEmpty() ? QPoint(ir.center().x() + pixw / 2 + 2, ir.center().y())
                                      : QPoint(br.x(), br.y() + br.height() / 2);
            QPoint pixTL(btl.x() - pixw - 2, btl.y() - pixh / 2);
	    int tf=AlignVCenter | ShowPrefix | AlignHCenter;
	    if (!styleHint(SH_UnderlineAccelerator, widget, QStyleOption::Default, 0))
		tf |= NoAccel;
            drawItem(p, QRect(pixTL, pixmap.size()), tf, cg, how & Style_Enabled, &pixmap, "");
            drawItem(p, br, tf, cg, how & Style_Enabled, 0, btn->text(), btn->text().length(),
                     &cg.buttonText());
        }
        break;
    }
#ifndef QT_NO_HEADER
    case CE_HeaderLabel:
    {
        QRect rect = r;
        const QHeader* header = (const QHeader *)widget;
        int section = opt.headerSection();

        QIconSet* icon = header->iconSet( section );
        if ( icon ) {
            QPixmap pixmap = icon->pixmap( QIconSet::Small,
                                           how & Style_Enabled ?
                                           QIconSet::Normal : QIconSet::Disabled );
            int pixw = pixmap.width();
            int pixh = pixmap.height();
            // "pixh - 1" because of tricky integer division

            QRect pixRect = rect;
            pixRect.setY( rect.center().y() - (pixh - 1) / 2 );
            drawItem ( p, pixRect, AlignVCenter, cg, how & Style_Enabled,
                       &pixmap, QString::null );
            if (QApplication::reverseLayout())
                rect.setRight(rect.right() - pixw - 2);
            else
                rect.setLeft(rect.left() + pixw + 2);
        }

	// change the color to bright text if we are a table header and selected.
        const QColor *penColor = &cg.buttonText();
#ifndef QT_NO_TABLE
        if (::qt_cast<QTable *>(header->parentWidget()) && p->font().bold())
            penColor = &cg.color(QColorGroup::BrightText);
#endif
        if (rect.isValid())
            drawItem(p, rect, AlignVCenter, cg, how & Style_Enabled,
                     0, header->label(section), -1, penColor);
        break;
    }
#endif // QT_NO_HEADER
    default:
	QWindowsStyle::drawControl(element, p, widget, r, cg, how, opt);
    }
}

/*! \reimp */
void QMacStyle::drawComplexControl(ComplexControl ctrl, QPainter *p,
					const QWidget *widget,
					const QRect &r,
					const QColorGroup &cg,
					SFlags flags,
					SCFlags sub,
					SCFlags subActive,
					const QStyleOption& opt) const
{
    ThemeDrawState tds = kThemeStateActive;
    if(qAquaActive(cg)) {
	if(!(flags & Style_Enabled))
	    tds = kThemeStateUnavailable;
    } else if(flags & Style_Enabled) {
	tds = kThemeStateInactive;
    } else {
	tds = kThemeStateUnavailableInactive;
    }

    switch(ctrl) {
    case CC_ToolButton: {
	if(!widget)
	    break;
	QToolButton *toolbutton = (QToolButton *) widget;
	ThemeButtonKind bkind = kThemeBevelButton;
	if(qt_aqua_size_constrain(widget) == QAquaSizeSmall)
	    bkind = kThemeSmallBevelButton;

	QRect button, menuarea;
	button   = querySubControlMetrics(ctrl, widget, SC_ToolButton, opt);
	menuarea = querySubControlMetrics(ctrl, widget, SC_ToolButtonMenu, opt);
	SFlags bflags = flags,
	       mflags = flags;
	if(subActive & SC_ToolButton)
	    bflags |= Style_Down;
	if(subActive & SC_ToolButtonMenu)
	    mflags |= Style_Down;

	if(sub & SC_ToolButton) {
	    if(bflags & (Style_Down | Style_On | Style_Raised)) {
		ThemeButtonDrawInfo info = { tds, kThemeButtonOff, kThemeAdornmentNone };
		if(flags & Style_HasFocus)
		    info.adornment = kThemeAdornmentFocus;
		if(toolbutton->isOn() || toolbutton->isDown())
		    info.value = kThemeStateActive;

		QRect off_rct(0, 0, 0, 0);
		{ //The AppManager draws outside my rectangle, so account for that difference..
		    Rect macRect, myRect;
		    SetRect(&myRect,r.x(), r.y(), r.width(), r.height());
		    GetThemeButtonBackgroundBounds(&myRect, bkind, &info, &macRect);
		    off_rct = QRect(myRect.left - macRect.left, myRect.top - macRect.top,
				    (myRect.left - macRect.left) + (macRect.right - myRect.right),
				    (myRect.top - macRect.top) + (macRect.bottom - myRect.bottom));
		}

		// If the background color is set then make the toolbutton
		// translucent so the background color is visible
		if (widget->paletteBackgroundColor() != white) {
		    p->fillRect( r, widget->backgroundColor() );
		    info.state = kThemeStateInactive;
		}

		((QMacPainter *)p)->setport();
		DrawThemeButton(qt_glb_mac_rect(button, p, FALSE, off_rct),
				bkind, &info, NULL, NULL, NULL, 0);
	    } else if(toolbutton->parentWidget() &&
			toolbutton->parentWidget()->backgroundPixmap() &&
			! toolbutton->parentWidget()->backgroundPixmap()->isNull()) {
		p->drawTiledPixmap(r, *(toolbutton->parentWidget()->backgroundPixmap()),
				    toolbutton->pos());
	    }
	}

	if(sub & SC_ToolButtonMenu) {
	    ThemeButtonDrawInfo info = { tds, kThemeButtonOff, kThemeAdornmentNone };
	    if(flags & Style_HasFocus)
		info.adornment |= kThemeAdornmentFocus;
	    if(toolbutton->isOn() || toolbutton->isDown() || (subActive & SC_ToolButtonMenu))
		info.value = kThemeButtonOn;
	    ((QMacPainter *)p)->setport();
	    DrawThemeButton(qt_glb_mac_rect(menuarea, p, FALSE), bkind, &info, NULL, NULL, NULL, 0);
	    QRect r(menuarea.x() + ((menuarea.width() / 2) - 4), menuarea.height() - 8, 8, 8);
	    DrawThemePopupArrow(qt_glb_mac_rect(r, p),
				kThemeArrowDown, kThemeArrow7pt, tds, NULL, 0);
	}
	break; }
    case CC_ListView: {
	if(sub & SC_ListView)
	    QWindowsStyle::drawComplexControl(ctrl, p, widget, r, cg, flags, sub, subActive, opt);
	if(sub & (SC_ListViewBranch | SC_ListViewExpand)) {
	    if(opt.isDefault())
		break;
	    QListViewItem *item = opt.listViewItem();
	    int y=r.y(), h=r.height();
	    ((QMacPainter *)p)->setport();
	    {
		::RGBColor f;
		f.red = widget->paletteBackgroundColor().red()*256;
		f.green = widget->paletteBackgroundColor().green()*256;
		f.blue = widget->paletteBackgroundColor().blue()*256;
		RGBBackColor(&f);
	    }

	    QPixmap pm;
	    QPainter pm_paint;
	    for(QListViewItem *child = item->firstChild(); child && y < h;
		y += child->totalHeight(), child = child->nextSibling()) {
		if(y + child->height() > 0) {
		    if(child->isVisible() && (child->isExpandable() || child->childCount())) {
			ThemeButtonDrawInfo info = { tds, kThemeDisclosureRight, kThemeAdornmentDrawIndicatorOnly };
			if(flags & Style_HasFocus)
			    info.adornment |= kThemeAdornmentFocus;

			int rot = 0, border = 2;
			QPainter *curPaint = p;
			QRect mr(r.right() - 10, (y + child->height()/2) - 4, 9, 9);
			Rect glb_r = *qt_glb_mac_rect(mr, p);
			if(d->animatable(QAquaAnimate::AquaListViewItemOpen, child)) {
			    if(!d->lviState.lvis.contains(child)) {
				d->lviState.lvis.insert(child, child->isOpen() ? 0 : 4);
			    } else {
				int frame = d->lviState.lvis[child];
				if((child->isOpen() && frame == 4) || (!child->isOpen() && frame == 0)) {
				    //nothing..
				} else {
				    if(pm.isNull()) {
					pm = QPixmap(mr.width()+(border*2), mr.height()+(border*2), 32);
					pm.fill(widget->paletteBackgroundColor());
					pm_paint.begin(&pm);
				    }
				    SetRect(&glb_r, border, border, mr.width(), mr.height());
				    curPaint = &pm_paint;
				    ((QMacPainter *)curPaint)->setport();
				    rot = frame;
				}
			    }
			}
			if(!rot && child->isOpen())
			    info.value = kThemeDisclosureDown;
			DrawThemeButton(&glb_r, kThemeDisclosureButton, &info, NULL, NULL, NULL, 0);
			if(curPaint != p) {
			    QWMatrix wm;
			    wm.translate(-(pm.width()/2), -(pm.height()/2));
			    wm.rotate((90 / 4) * rot);
			    wm.translate((pm.width()/2), (pm.height()/2));
			    p->drawPixmap(mr.topLeft()-QPoint(border, border), pm.xForm(wm));
			    ((QMacPainter *)p)->setport();
			}
		    }
		}
	    }
	}
	break; }
    case CC_SpinWidget: {
	QSpinWidget * sw = (QSpinWidget *) widget;
	if(sub & SC_SpinWidgetFrame)
	    drawPrimitive(PE_PanelLineEdit, p, querySubControlMetrics(CC_SpinWidget, sw, SC_SpinWidgetFrame),
			  cg, Style_Sunken);
	if((sub & SC_SpinWidgetDown) || (sub & SC_SpinWidgetUp)) {
	    if(!sw->isUpEnabled() && !sw->isDownEnabled())
		tds = kThemeStateUnavailable;
	    if(subActive == SC_SpinWidgetDown)
		tds = kThemeStatePressedDown;
	    else if(subActive == SC_SpinWidgetUp)
		tds = kThemeStatePressedUp;
	    ThemeButtonDrawInfo info = { tds, kThemeButtonOff, kThemeAdornmentNone };
#if 1
	    if(flags & Style_HasFocus)
		info.adornment |= kThemeAdornmentFocus;
#endif
            //p->save();
	    QRect updown = sw->upRect() | sw->downRect();
	    if(sw->backgroundPixmap())
		p->drawPixmap(updown, *sw->backgroundPixmap());
	    else
		p->fillRect(updown, sw->backgroundColor());
            //p->fillRect(sw->rect(), Qt::yellow);
            //p->setClipRect(updown);
	    ((QMacPainter *)p)->setport();
	    ThemeButtonKind kind = kThemeIncDecButton;
#if QT_MACOSX_VERSION >= 0x1030
	    if(qt_aqua_size_constrain(widget) == QAquaSizeSmall && qMacVersion() >= Qt::MV_PANTHER)
		kind = kThemeIncDecButtonSmall;
#endif
            Rect macRect, myRect;
            SetRect(&myRect, updown.x(), updown.y(),
                    updown.x() + updown.width(), updown.y() + updown.height());
            GetThemeButtonBackgroundBounds(&myRect, kind, &info, &macRect);
            QRect off_rct(myRect.left - macRect.left, myRect.top - macRect.top,
                          (myRect.left - macRect.left) + (macRect.right - myRect.right),
                          (myRect.top - macRect.top) + (macRect.bottom - myRect.bottom));
	    DrawThemeButton(qt_glb_mac_rect(updown, p, false, off_rct), kind, &info, 0, 0, 0, 0);
            //p->restore();
	}
	break; }
    case CC_TitleBar: {
	if(!widget)
	    break;
        tds = kThemeStateActive;
	QString dblbuf_key;
	QTitleBar *tbar = (QTitleBar *) widget;
	ThemeWindowMetrics twm;
	memset(&twm, '\0', sizeof(twm));
	twm.metricSize = sizeof(twm);
	twm.titleWidth = tbar->width();
	twm.titleHeight = tbar->height();
	ThemeWindowAttributes twa = kThemeWindowHasTitleText;
	if(tbar->window()) {
	    if(tbar->window()->isMinimized())
		twa |= kThemeWindowIsCollapsed;
	    twa |= kThemeWindowHasFullZoom | kThemeWindowHasCloseBox | kThemeWindowHasCollapseBox;
	} else if(tbar->testWFlags(WStyle_SysMenu)) {
	    twa |= kThemeWindowHasCloseBox;
	}

	//AppMan paints outside the given rectangle, so I have to adjust for the height properly!
	QRegion treg;
	GetThemeWindowRegion(macWinType, qt_glb_mac_rect(r),
			     tds, &twm, twa, kWindowTitleBarRgn, treg.handle(TRUE));
	QRect br = treg.boundingRect(), newr = r;
	newr.moveBy(newr.x() - br.x(), newr.y() - br.y());
	if(sub & SC_TitleBarLabel) {
	    int iw = 0;
	    if(tbar->icon()) {
		GetThemeWindowRegion(macWinType, qt_glb_mac_rect(newr),
				     tds, &twm, twa, kWindowTitleProxyIconRgn, treg.handle(TRUE));
		if(!treg.isEmpty())
		    iw = tbar->icon()->width();
	    }
	    if(!tbar->visibleText().isEmpty()) {
		QString pmkey;
		QTextOStream os(&pmkey);
		os << "$qt_mac_style_titlebar_" << "_" << newr.width()
		   << "x" << newr.height() << "_" << twa << "_" << tds << "_" << tbar->visibleText();
		if(QPixmap *dblbuf = QPixmapCache::find(pmkey)) {
		    p->drawPixmap(r.topLeft(), *dblbuf);
		} else {
		    QPixmap *pix = new QPixmap(newr.width(), newr.height(), 32);
		    QPainter pixp(pix);

		    ((QMacPainter *)&pixp)->setport();
		    DrawThemeWindowFrame(macWinType, qt_glb_mac_rect(newr, &pixp, FALSE), tds,
					 &twm, twa, NULL, 0);
		    GetThemeWindowRegion(macWinType, qt_glb_mac_rect(newr),
					 tds, &twm, twa, kWindowTitleTextRgn, treg.handle(TRUE));
		    pixp.setClipRegion(treg);
		    QRect br = treg.boundingRect();
		    int x = br.x(),
			y = br.y() + ((tbar->height() / 2) - (p->fontMetrics().height() / 2));
		    if(br.width() <= (p->fontMetrics().width(tbar->caption())+iw*2))
			x += iw;
		    else
			x += (br.width() / 2) - (p->fontMetrics().width(tbar->visibleText()) / 2);
		    if(iw)
			pixp.drawPixmap(x - iw, y, *tbar->icon());
		    pixp.drawText(x, y + p->fontMetrics().ascent(), tbar->visibleText());
		    pixp.end();
		    p->drawPixmap(r.topLeft(), *pix);
		    if(!QPixmapCache::insert(pmkey, pix))
			delete pix;
		}
	    } else {
		((QMacPainter *)p)->setport();
		DrawThemeWindowFrame(macWinType, qt_glb_mac_rect(newr, p, FALSE), tds,
				     &twm, twa, NULL, 0);
	    }
	}
	if(sub & (SC_TitleBarCloseButton | SC_TitleBarMaxButton | SC_TitleBarMinButton |
		  SC_TitleBarNormalButton)) {
	    ThemeDrawState wtds = tds;
	    if(flags & Style_MouseOver)
		wtds = kThemeStateRollover;
	    struct {
		unsigned int qt_type;
		ThemeTitleBarWidget mac_type;
		ThemeWindowAttributes mac_needs;
	    } types[] = {
		{ SC_TitleBarCloseButton, kThemeWidgetCloseBox, kThemeWindowHasCloseBox },
		{ SC_TitleBarMaxButton, kThemeWidgetZoomBox, kThemeWindowHasFullZoom },
		{ SC_TitleBarMinButton|SC_TitleBarNormalButton, kThemeWidgetCollapseBox, kThemeWindowHasCollapseBox },
		{ 0, 0, 0 } };
	    ThemeWindowMetrics tm;
	    tm.metricSize = sizeof(tm);
	    const Rect *wm_rect = qt_glb_mac_rect(newr, p, FALSE);
	    ((QMacPainter *)p)->setport();
	    for(int i = 0; types[i].qt_type; i++) {
		if(!(twa & types[i].mac_needs))
		    continue;
		ThemeDrawState ctrl_tds = wtds;
		if(qAquaActive(cg) && (subActive & types[i].qt_type))
		    ctrl_tds = kThemeStatePressed;
		DrawThemeTitleBarWidget(macWinType, wm_rect, ctrl_tds, &tm, twa, types[i].mac_type);
	    }
	}
	break; }
    case CC_ScrollBar: {
	if(!widget)
	    break;
	QScrollBar *scrollbar = (QScrollBar *) widget;
	ThemeTrackDrawInfo ttdi;
	memset(&ttdi, '\0', sizeof(ttdi));
	ttdi.kind = kThemeMediumScrollBar;
	if(qt_aqua_size_constrain(widget) == QAquaSizeSmall)
	    ttdi.kind = kThemeSmallScrollBar;
	ttdi.bounds = *qt_glb_mac_rect(r, p);
	ttdi.min = scrollbar->minValue();
	ttdi.max = scrollbar->maxValue();
	ttdi.value = scrollbar->value();
	ttdi.attributes |= kThemeTrackShowThumb;
	if(scrollbar->orientation() == Qt::Horizontal)
	    ttdi.attributes |= kThemeTrackHorizontal;
	if(!qAquaActive(cg))
	    ttdi.enableState = kThemeTrackInactive;
	else if(!scrollbar->isEnabled())
	    ttdi.enableState = kThemeTrackDisabled;
	if(subActive == SC_ScrollBarSubLine)
	    ttdi.trackInfo.scrollbar.pressState = kThemeRightInsideArrowPressed |
						  kThemeLeftOutsideArrowPressed;
	else if(subActive == SC_ScrollBarAddLine)
	    ttdi.trackInfo.scrollbar.pressState = kThemeLeftInsideArrowPressed |
						  kThemeRightOutsideArrowPressed;
	else if(subActive == SC_ScrollBarAddPage)
	    ttdi.trackInfo.scrollbar.pressState = kThemeRightTrackPressed;
	else if(subActive == SC_ScrollBarSubPage)
	    ttdi.trackInfo.scrollbar.pressState = kThemeLeftTrackPressed;
	else if(subActive == SC_ScrollBarSlider)
	    ttdi.trackInfo.scrollbar.pressState = kThemeThumbPressed;
	ttdi.trackInfo.scrollbar.viewsize = scrollbar->pageStep();
	((QMacPainter *)p)->setport();
	DrawThemeTrack(&ttdi, NULL, NULL, 0);
	break; }
    case CC_Slider: {
	if(!widget)
	    break;
	QSlider *sldr = (QSlider *)widget;
	ThemeTrackDrawInfo ttdi;
	memset(&ttdi, '\0', sizeof(ttdi));
	ttdi.kind = kThemeMediumSlider;
	if(qt_aqua_size_constrain(widget) == QAquaSizeSmall)
	    ttdi.kind = kThemeSmallSlider;
	ttdi.bounds = *qt_glb_mac_rect(widget->rect(), p);
	ttdi.min = sldr->minValue();
	ttdi.max = sldr->maxValue();
	ttdi.value = sldr->valueFromPosition(sldr->sliderStart());
	ttdi.attributes |= kThemeTrackShowThumb;
#if QT_MACOSX_VERSION >= 0x1020
	if(flags & Style_HasFocus)
	    ttdi.attributes |= kThemeTrackHasFocus;
#endif
	if(sldr->orientation() == Qt::Horizontal)
	    ttdi.attributes |= kThemeTrackHorizontal;
	if(widget->isEnabled())
	    ttdi.enableState |= kThemeTrackActive;
	if(!qAquaActive(cg))
	    ttdi.enableState = kThemeTrackInactive;
	else if(!sldr->isEnabled())
	    ttdi.enableState = kThemeTrackDisabled;
	if(sldr->tickmarks() == QSlider::Above)
	    ttdi.trackInfo.slider.thumbDir = kThemeThumbUpward;
	else
	    ttdi.trackInfo.slider.thumbDir = kThemeThumbDownward;
	if(subActive == SC_SliderGroove)
	    ttdi.trackInfo.slider.pressState = kThemeLeftTrackPressed;
	else if(subActive == SC_SliderHandle)
	    ttdi.trackInfo.slider.pressState = kThemeThumbPressed;

	sldr->erase(); //fill the area once..

	//The AppManager draws outside my rectangle, so account for that difference..
	Rect macRect;
	GetThemeTrackBounds(&ttdi, &macRect);
	ttdi.bounds.left  += ttdi.bounds.left  - macRect.left;
	ttdi.bounds.right -= macRect.right - ttdi.bounds.right;

	((QMacPainter *)p)->setport();
	DrawThemeTrack(&ttdi, NULL, NULL, 0);
	if(sub & SC_SliderTickmarks) {
	    int numTicks = ((sldr->maxValue() - sldr->minValue() + 1) / sldr->pageStep()) + 1;
	    if(sldr->tickInterval())
		numTicks = ((sldr->maxValue() - sldr->minValue() + 1) / sldr->tickInterval()) + 1;
	    DrawThemeTrackTickMarks(&ttdi, numTicks, NULL, 0);
	}
	break; }
    case CC_ComboBox: {
	if(!widget)
	    break;
	QComboBox *cbox = (QComboBox *)widget;
	ThemeButtonDrawInfo info = { tds, kThemeButtonOff, kThemeAdornmentNone };
        bool hasFocus = flags & Style_HasFocus;
	if(hasFocus && !cbox->editable())
	    info.adornment |= kThemeAdornmentFocus;
	if(subActive & QStyle::SC_ComboBoxArrow)
	    info.state = kThemeStatePressed;
	p->fillRect(r, cg.brush(QColorGroup::Button)); //make sure it is filled
        info.adornment |= kThemeAdornmentArrowLeftArrow;
        ThemeButtonKind bkind = cbox->editable() ? kThemeComboBox : kThemePopupButton;
        QRect off_rct;
        if (!cbox->editable()) {
            //The AppManager draws outside my rectangle, so account for that difference..
            Rect macRect, myRect;
            SetRect(&myRect, r.x(), r.y(), r.width(), r.height());
            GetThemeButtonBackgroundBounds(&myRect, bkind, &info, &macRect);
            int offset = hasFocus && !cbox->editable() ? -1 : 0;
            off_rct.setRect(myRect.left - macRect.left, (myRect.top - macRect.top) + offset,
                    (myRect.left - macRect.left) + (macRect.right - myRect.right) + offset,
                    (myRect.top - macRect.top)
                    + (macRect.bottom - myRect.bottom) + offset);
        }
        ((QMacPainter *)p)->setport();
        Rect finalRect = *qt_glb_mac_rect(r, p, TRUE, off_rct);
        DrawThemeButton(&finalRect, bkind , &info, 0, 0, 0, 0);
	break; }
    default:
	QWindowsStyle::drawComplexControl(ctrl, p, widget, r, cg, flags, sub, subActive, opt);
    }
}

/*! \reimp */
int QMacStyle::pixelMetric(PixelMetric metric, const QWidget *widget) const
{
    SInt32 ret = 0;
    switch(metric) {
    case PM_TabBarTabVSpace:
	ret = 4;
	break;
    case PM_CheckListControllerSize:
	break;
    case PM_CheckListButtonSize: {
	ThemeMetric tm = kThemeMetricCheckBoxWidth;
	if(qt_aqua_size_constrain(widget) == QAquaSizeSmall)
	    tm = kThemeMetricSmallCheckBoxWidth;
	GetThemeMetric(tm, &ret);
	break; }
    case PM_TabBarTabShiftHorizontal:
    case PM_TabBarTabShiftVertical:
	ret = 0;
	break;
    case PM_DialogButtonsSeparator:
	ret = -5;
	break;
    case PM_DialogButtonsButtonHeight: {
	QSize sz;
	ret = qt_aqua_size_constrain(NULL, QStyle::CT_PushButton, QSize(-1, -1), &sz);
	if(sz == QSize(-1, -1))
	    ret = 32;
	else
	    ret = sz.height();
	break; }
    case PM_DialogButtonsButtonWidth: {
	QSize sz;
	ret = qt_aqua_size_constrain(NULL, QStyle::CT_PushButton, QSize(-1, -1), &sz);
	if(sz == QSize(-1, -1))
	    ret = 70;
	else
	    ret = sz.width();
	break; }
    case PM_PopupMenuScrollerHeight:
#if 0
	SInt16 ash, asw;
	GetThemeMenuItemExtra(kThemeMenuItemScrollUpArrow, &ash, &asw);
	ret = ash;
#else
	ret = 15; //I hate having magic numbers in here...
#endif
	break;
    case PM_DefaultFrameWidth:
	if (widget && (widget->isTopLevel() || !widget->parentWidget()
                || (::qt_cast<QMainWindow*>(widget->parentWidget())
                   && ((QMainWindow*)widget->parentWidget())->centralWidget() == widget))
		&& (::qt_cast<QScrollView*>(widget) || widget->inherits("QWorkspaceChild")))
	    ret = 0;
	else
	    ret = QWindowsStyle::pixelMetric(metric, widget);
	break;
    case PM_MaximumDragDistance:
	ret = -1;
	break;
    case PM_ScrollBarSliderMin:
	ret = 24;
	break;
    case PM_TabBarBaseHeight:
	ret = 8;
	break;
    case PM_SpinBoxFrameWidth:
	GetThemeMetric(kThemeMetricEditTextFrameOutset, &ret);
	ret += 2;
	break;
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
	ret = 0;
	break;
    case PM_SliderLength:
	ret = 17;
	break;
    case PM_ButtonDefaultIndicator:
	ret = 0;
	break;
    case PM_TitleBarHeight: {
	if(!widget)
	    break;
	QTitleBar *tbar = (QTitleBar*)widget;
	ThemeWindowMetrics twm;
	memset(&twm, '\0', sizeof(twm));
	twm.metricSize = sizeof(twm);
	twm.titleWidth = tbar->width();
	twm.titleHeight = tbar->height();
	ThemeWindowAttributes twa = kThemeWindowHasTitleText;
	if(tbar->window())
	    twa |= kThemeWindowHasFullZoom | kThemeWindowHasCloseBox | kThemeWindowHasCollapseBox;
	else if(tbar->testWFlags(WStyle_SysMenu))
	    twa |= kThemeWindowHasCloseBox;
	QRegion treg;
	GetThemeWindowRegion(macWinType, qt_glb_mac_rect(tbar->rect()), kThemeStateActive,
			     &twm, twa, kWindowTitleBarRgn, treg.handle(TRUE));
	ret = treg.boundingRect().height();
	break; }
    case PM_TabBarTabOverlap:
        if (qMacVersion() < Qt::MV_10_DOT_3)
            GetThemeMetric(kThemeMetricTabOverlap, &ret);
        else
            ret = 0;
	break;
    case PM_ScrollBarExtent: {
	ThemeMetric tm = kThemeMetricScrollBarWidth;
	if(qt_aqua_size_constrain(widget) == QAquaSizeSmall)
	    tm = kThemeMetricSmallScrollBarWidth;
	GetThemeMetric(tm, &ret);
	break; }
    case PM_TabBarBaseOverlap:
        if (qMacVersion() < Qt::MV_10_DOT_3)
            GetThemeMetric(kThemeMetricTabFrameOverlap, &ret);
        else
            ret = 12;
	break;
    case PM_IndicatorHeight: {
	ThemeMetric tm = kThemeMetricCheckBoxHeight;
	if(qt_aqua_size_constrain(widget) == QAquaSizeSmall)
	    tm = kThemeMetricSmallCheckBoxHeight;
	GetThemeMetric(tm, &ret);
	break; }
    case PM_IndicatorWidth: {
	ThemeMetric tm = kThemeMetricCheckBoxWidth;
	if(qt_aqua_size_constrain(widget) == QAquaSizeSmall)
	    tm = kThemeMetricSmallCheckBoxWidth;
	GetThemeMetric(tm, &ret);
	break; }
    case PM_ExclusiveIndicatorHeight: {
	ThemeMetric tm = kThemeMetricRadioButtonHeight;
	if(qt_aqua_size_constrain(widget) == QAquaSizeSmall)
	    tm = kThemeMetricSmallRadioButtonHeight;
	GetThemeMetric(tm, &ret);
	break; }
    case PM_ExclusiveIndicatorWidth: {
	ThemeMetric tm = kThemeMetricRadioButtonWidth;
	if(qt_aqua_size_constrain(widget) == QAquaSizeSmall)
	    tm = kThemeMetricSmallRadioButtonWidth;
	GetThemeMetric(tm, &ret);
	break; }
    case PM_PopupMenuFrameVerticalExtra:
	ret = 4;
	break;
    default:
	ret = QWindowsStyle::pixelMetric(metric, widget);
	break;
    }
    return ret;
}

/*! \reimp */
QRect QMacStyle::querySubControlMetrics(ComplexControl control,
					    const QWidget *w,
					    SubControl sc,
					    const QStyleOption& opt) const
{
    switch(control) {
    case CC_SpinWidget: {
	static const int spinner_w = 18;
	int fw = pixelMetric(PM_SpinBoxFrameWidth, w);
        int y = fw, x = w->width() - fw - spinner_w;
	switch(sc) {
	case SC_SpinWidgetUp:
	case SC_SpinWidgetDown: {
            y = fw;
            x = w->rect().width() - spinner_w + y;
            QRect ret(x, y, spinner_w, w->rect().height() - y * 2);
            ThemeButtonDrawInfo info = { kThemeStateActive, kThemeButtonOff, kThemeAdornmentNone };
            ThemeButtonKind kind = kThemeIncDecButton;
            switch (qt_aqua_size_constrain(w)) {
            case QAquaSizeSmall:
#if (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3)
                if (qMacVersion() >= Qt::MV_10_DOT_3) {
                    kind = kThemeIncDecButtonSmall;
                    break;
                }
#endif
            case QAquaSizeUnknown:
            case QAquaSizeLarge:
                kind = kThemeIncDecButton;
                break;
            }
            Rect macRect;
            GetThemeButtonBackgroundBounds(qt_glb_mac_rect(ret), kind, &info, &macRect);

            ret.setRect(macRect.left, macRect.top,
                        macRect.right - macRect.left, macRect.bottom - macRect.top);
            switch (sc) {
            case QStyle::SC_SpinWidgetUp:
                ret.setHeight(ret.height() / 2);
                break;
            case QStyle::SC_SpinWidgetDown:
                ret.setY(ret.y() + ret.height() / 2);
                break;
            default:
                Q_ASSERT(0);
                break;
            }
            return ret;
        }
	case SC_SpinWidgetButtonField:
	    return QRect(x, y, spinner_w, w->height() - (fw*2));
	case SC_SpinWidgetEditField:
	    return QRect(fw, fw, w->width() - 18 - (fw*2) - macSpinBoxSep + 1, w->height() - (fw*2) + 1);
	case SC_SpinWidgetFrame:
	    return QRect(0, 0, w->width() - spinner_w - macSpinBoxSep, w->height());
	default:
	    break;
	}
	break; }
    case CC_TitleBar: {
	if(!w)
	    return QRect();
	QTitleBar *tbar = (QTitleBar*)w;
	ThemeWindowMetrics twm;
	memset(&twm, '\0', sizeof(twm));
	twm.metricSize = sizeof(twm);
	twm.titleWidth = tbar->width();
	twm.titleHeight = tbar->height();
	ThemeWindowAttributes twa = kThemeWindowHasTitleText;
	if(tbar->window())
	    twa |= kThemeWindowHasFullZoom | kThemeWindowHasCloseBox | kThemeWindowHasCollapseBox;
	else if(tbar->testWFlags(WStyle_SysMenu))
	    twa |= kThemeWindowHasCloseBox;
	WindowRegionCode wrc = kWindowGlobalPortRgn;
	if(sc & SC_TitleBarCloseButton)
	    wrc = kWindowCloseBoxRgn;
	else if(sc & SC_TitleBarMinButton)
	    wrc = kWindowCollapseBoxRgn;
	else if(sc & SC_TitleBarMaxButton)
	    wrc = kWindowZoomBoxRgn;
	else if(sc & SC_TitleBarLabel)
	    wrc = kWindowTitleTextRgn;
	else if(sc & SC_TitleBarSysMenu)
	    return QRect(-666, -666, 10, pixelMetric(PM_TitleBarHeight)); //ugh
	if(wrc != kWindowGlobalPortRgn) {
	    //AppMan paints outside the given rectangle, so I have to adjust for the height properly!
	    QRegion treg;
	    QRect r = w->rect();
	    GetThemeWindowRegion(macWinType, qt_glb_mac_rect(r),
				 kThemeStateActive, &twm, twa, kWindowTitleBarRgn, treg.handle(TRUE));
	    QRect br = treg.boundingRect();
	    r.moveBy(r.x() - br.x(), r.y() - br.y());
	    GetThemeWindowRegion(macWinType, qt_glb_mac_rect(r),
				 kThemeStateActive, &twm, twa, wrc, treg.handle(TRUE));
	    return treg.boundingRect();
	}
	break; }
    case CC_ComboBox: {
        const QComboBox *combo = ::qt_cast<const QComboBox *>(w);
	if(!combo)
	    return QRect();
        if (sc == SC_ComboBoxEditField) {
            Rect macRect, outRect;
            SetRect(&macRect, 0, 0, w->width(), w->height());
            ThemeButtonDrawInfo bdi = { kThemeStateActive, kThemeButtonOff, kThemeAdornmentNone };
            GetThemeButtonContentBounds(&macRect, kThemePopupButton, &bdi, &outRect);
            if (combo->editable()) {
                return QRect(outRect.left - 2, outRect.top + 1,
                             (outRect.right - outRect.left) + 9,
                             (outRect.bottom - outRect.top) - 3);
            } else {
                if (w->height() < 30) {
                    return QRect(outRect.left, outRect.top - 1, outRect.right - outRect.left,
                            outRect.bottom - outRect.top);
                } else {
                    QFontMetrics fm = w->fontMetrics();
                    return QRect(outRect.left, (w->height() / 2) - fm.ascent() + 3,
                                 outRect.right - outRect.left, fm.height());
                }
            }

        } else {
            return QWindowsStyle::querySubControlMetrics(control, w, sc, opt);
        }
	break; }
    case CC_ScrollBar: {
	if(!w)
	    return QRect();
	QScrollBar *scrollbar = (QScrollBar *) w;
	ThemeTrackDrawInfo ttdi;
	memset(&ttdi, '\0', sizeof(ttdi));
	ttdi.kind = kThemeMediumScrollBar;
	if(qt_aqua_size_constrain(w) == QAquaSizeSmall)
	    ttdi.kind = kThemeSmallScrollBar;
	ttdi.bounds = *qt_glb_mac_rect(w->rect());
	ttdi.min = scrollbar->minValue();
	ttdi.max = scrollbar->maxValue();
	ttdi.value = scrollbar->value();
	ttdi.attributes |= kThemeTrackShowThumb;
	if(scrollbar->orientation() == Qt::Horizontal)
	    ttdi.attributes |= kThemeTrackHorizontal;
	if(!qAquaActive(scrollbar->colorGroup()))
	    ttdi.enableState = kThemeTrackInactive;
	else if(!scrollbar->isEnabled())
	    ttdi.enableState = kThemeTrackDisabled;
	ttdi.trackInfo.scrollbar.viewsize = scrollbar->pageStep();
	switch(sc) {
	case SC_ScrollBarGroove: {
	    Rect mrect;
	    GetThemeTrackDragRect(&ttdi, &mrect);
	    return QRect(mrect.left, mrect.top, mrect.right - mrect.left, mrect.bottom - mrect.top);
	    break; }
	case SC_ScrollBarSlider: {
	    QRegion rgn;
	    GetThemeTrackThumbRgn(&ttdi, rgn.handle(TRUE));
	    return rgn.boundingRect();
	    break; }
	default:
	    break;
	}
	break; }
    case CC_Slider: {
	if(!w)
	    return QRect();
	QSlider *sldr = (QSlider *)w;
	ThemeTrackDrawInfo ttdi;
	memset(&ttdi, '\0', sizeof(ttdi));
	ttdi.kind = kThemeMediumSlider;
	if(qt_aqua_size_constrain(w) == QAquaSizeSmall)
	    ttdi.kind = kThemeSmallSlider;
#if QT_MACOSX_VERSION >= 0x1020
	if(w->hasFocus())
	    ttdi.attributes |= kThemeTrackHasFocus;
#endif
	ttdi.bounds = *qt_glb_mac_rect(w->rect());
	ttdi.min = sldr->minValue();
	ttdi.max = sldr->maxValue();
	ttdi.value = sldr->valueFromPosition(sldr->sliderStart());
	ttdi.attributes |= kThemeTrackShowThumb;
	if(sldr->orientation() == Qt::Horizontal)
	    ttdi.attributes |= kThemeTrackHorizontal;
	if(!qAquaActive(sldr->colorGroup()))
	    ttdi.enableState = kThemeTrackInactive;
	else if(!sldr->isEnabled())
	    ttdi.enableState = kThemeTrackDisabled;
	if(sldr->tickmarks() == QSlider::Above)
	    ttdi.trackInfo.slider.thumbDir = kThemeThumbUpward;
	else
	    ttdi.trackInfo.slider.thumbDir = kThemeThumbDownward;

	//The AppManager draws outside my rectangle, so account for that difference..
	Rect macRect;
	GetThemeTrackBounds(&ttdi, &macRect);
	ttdi.bounds.left  += ttdi.bounds.left  - macRect.left;
	ttdi.bounds.right -= macRect.right - ttdi.bounds.right;

	switch(sc) {
	case SC_SliderGroove: {
	    Rect mrect;
	    GetThemeTrackBounds(&ttdi, &mrect);
	    return QRect(mrect.left, mrect.top,
			 mrect.right - mrect.left, mrect.bottom - mrect.top); }
	case SC_SliderHandle: {
	    QRegion rgn;
	    GetThemeTrackThumbRgn(&ttdi, rgn.handle(TRUE));
	    QRect ret = rgn.boundingRect();
            ret.addCoords(-3, -3, 2, 1);
	    return ret; }
	default:
	    break;
	}
	break; }
    default:
	break;
    }
    return QWindowsStyle::querySubControlMetrics(control, w, sc, opt);
}

/*! \reimp */
QRect QMacStyle::subRect(SubRect r, const QWidget *w) const
{
    QRect ret;
    switch(r) {
    case SR_ToolBoxTabContents:
        ret = QCommonStyle::subRect(r, w);
        break;
    case SR_DialogButtonAbort:
    case SR_DialogButtonRetry:
    case SR_DialogButtonIgnore:
    case SR_DialogButtonApply:
    case SR_DialogButtonAccept:
    case SR_DialogButtonReject:
    case SR_DialogButtonHelp:
    case SR_DialogButtonAll:
    case SR_DialogButtonCustom: {
	QDialogButtons::Button srch = QDialogButtons::None;
	if(r == SR_DialogButtonAccept)
	    srch = QDialogButtons::Accept;
	else if(r == SR_DialogButtonReject)
	    srch = QDialogButtons::Reject;
	else if(r == SR_DialogButtonAll)
	    srch = QDialogButtons::All;
	else if(r == SR_DialogButtonApply)
	    srch = QDialogButtons::Apply;
	else if(r == SR_DialogButtonHelp)
	    srch = QDialogButtons::Help;
	else if(r == SR_DialogButtonRetry)
	    srch = QDialogButtons::Retry;
	else if(r == SR_DialogButtonIgnore)
	    srch = QDialogButtons::Ignore;
	else if(r == SR_DialogButtonAbort)
	    srch = QDialogButtons::Abort;

	const int bwidth = pixelMetric(PM_DialogButtonsButtonWidth, w),
		 bheight = pixelMetric(PM_DialogButtonsButtonHeight, w),
		  bspace = pixelMetric(PM_DialogButtonsSeparator, w),
		      fw = pixelMetric(PM_DefaultFrameWidth, w);
	QRect wrect = w->rect();
	const QDialogButtons *dbtns = (const QDialogButtons *) w;
	int start = fw;
	if(dbtns->orientation() == Horizontal)
	    start = wrect.right() - fw;
	for(unsigned int i = 0, cnt = 0; i < (sizeof(macBtnOrder)/sizeof(macBtnOrder[0])); i++) {
	    if(dbtns->isButtonVisible(macBtnOrder[i])) {
		QSize szH = dbtns->sizeHint(macBtnOrder[i]);
		int mwidth = QMAX(bwidth, szH.width()), mheight = QMAX(bheight, szH.height());
		if(dbtns->orientation() == Horizontal) {
		    start -= mwidth;
		    if(cnt)
			start -= bspace;
		} else if(cnt) {
		    start += mheight;
		    start += bspace;
		}
		cnt++;
		if(macBtnOrder[i] == srch) {
		    if(dbtns->orientation() == Horizontal)
			ret = QRect(start, wrect.bottom() - fw - mheight, mwidth, mheight);
		    else
			ret = QRect(fw, start, mwidth, mheight);
		}
	    }
	    if(cnt == 2 && macBtnOrder[i] == QDialogButtons::Accept) { //yuck, but I need to put some extra space in there now..
		if(dbtns->orientation() == Horizontal)
		    start -= 20;
		else
		    start += 20;
	    }
	}
	int help_width = 0, help_height = 0;
	if(dbtns->isButtonVisible(QDialogButtons::Help)) {
	    if(dbtns->buttonText(QDialogButtons::Help) == "?") {
		help_width = 35;
		help_height = bheight;
	    } else {
		QSize szH = dbtns->sizeHint(QDialogButtons::Help);
		help_width = szH.width();
		help_height = szH.height();
	    }
	}
	if(r == SR_DialogButtonCustom) {
	    if(dbtns->orientation() == Horizontal)
		ret = QRect(fw + help_width, fw, start - help_width - (fw*2) - bspace, wrect.height() - (fw*2));
	    else
		ret = QRect(fw, start, wrect.width() - (fw*2), wrect.height() - help_height - start - (fw*2));
	} else if(r == SR_DialogButtonHelp && dbtns->buttonText(QDialogButtons::Help)) {
	    ret = QRect(fw, wrect.height() - help_height - fw, help_width, help_height);
	}
	break; }
    case SR_PushButtonContents: {
	Rect macRect, myRect;
	SetRect(&myRect, 0, 0, w->width(), w->height());
	ThemeButtonDrawInfo info = { kThemeStateActive, kThemeButtonOff, kThemeAdornmentNone };
	GetThemeButtonContentBounds(&myRect, kThemePushButton, &info, &macRect);
	ret = QRect(macRect.left, macRect.top - 1, QMIN(w->width()-(macRect.left*2), macRect.right-macRect.left),
		    QMIN(w->height()-(2*macRect.top), macRect.bottom-macRect.top));
	break; }
    case SR_ProgressBarLabel:
    case SR_ProgressBarGroove:
	break;
    case SR_ProgressBarContents:
	ret = w->rect();
	break;
    default:
	ret = QWindowsStyle::subRect(r, w);
	break;
    }
    return ret;
}

/*! \reimp */
QStyle::SubControl QMacStyle::querySubControl(ComplexControl control,
						 const QWidget *widget,
						 const QPoint &pos,
						 const QStyleOption& opt) const
{
    SubControl ret = SC_None;
    switch(control) {
    case CC_ScrollBar: {
	if(!widget)
	    break;
	QScrollBar *scrollbar = (QScrollBar *) widget;
	ThemeTrackDrawInfo ttdi;
	memset(&ttdi, '\0', sizeof(ttdi));
	ttdi.kind = kThemeMediumScrollBar;
	if(qt_aqua_size_constrain(widget) == QAquaSizeSmall)
	    ttdi.kind = kThemeSmallScrollBar;
	ttdi.bounds = *qt_glb_mac_rect(widget->rect());
	ttdi.min = scrollbar->minValue();
	ttdi.max = scrollbar->maxValue();
	ttdi.value = scrollbar->value();
	ttdi.attributes |= kThemeTrackShowThumb;
	if(scrollbar->orientation() == Qt::Horizontal)
	    ttdi.attributes |= kThemeTrackHorizontal;
	if(!qAquaActive(scrollbar->colorGroup()))
	    ttdi.enableState = kThemeTrackInactive;
	else if(!scrollbar->isEnabled())
	    ttdi.enableState = kThemeTrackDisabled;
	ttdi.trackInfo.scrollbar.viewsize = scrollbar->pageStep();
	Point pt = { pos.y(), pos.x() };
	Rect mrect;
	GetThemeTrackBounds(&ttdi, &mrect);
	ControlPartCode cpc;
	if(HitTestThemeScrollBarArrows(&ttdi.bounds, ttdi.enableState,
				       0, scrollbar->orientation() == Qt::Horizontal,
				       pt, &mrect, &cpc)) {
	    if(cpc == kControlUpButtonPart)
		ret = SC_ScrollBarSubLine;
	    else if(cpc == kControlDownButtonPart)
		ret = SC_ScrollBarAddLine;
	} else if(HitTestThemeTrack(&ttdi, pt, &cpc)) {
	    if(cpc == kControlPageUpPart)
		ret = SC_ScrollBarSubPage;
	    else if(cpc == kControlPageDownPart)
		ret = SC_ScrollBarAddPage;
	    else
		ret = SC_ScrollBarSlider;
	}
	break; }
    default:
	ret = QWindowsStyle::querySubControl(control, widget, pos, opt);
	break;
    }
    return ret;
}

/*! \reimp */
int QMacStyle::styleHint(StyleHint sh, const QWidget *w,
			  const QStyleOption &opt,QStyleHintReturn *d) const
{
    SInt32 ret = 0;
    switch(sh) {
    case SH_ToolButton_Uses3D:
	ret = FALSE;
	break;
    case SH_PopupMenu_AllowActiveAndDisabled:
	ret = FALSE;
	break;
    case SH_PopupMenu_SubMenuPopupDelay:
	ret = 100;
	break;
    case SH_ScrollBar_LeftClickAbsolutePosition:
	extern bool qt_scrollbar_jump_to_pos; //qapplication_mac.cpp
	ret = qt_scrollbar_jump_to_pos;
	break;
    case SH_TabBar_PreferNoArrows:
	ret = TRUE;
	break;
    case SH_LineEdit_PasswordCharacter:
	ret = 0x25AA;
	break;
    case SH_DialogButtons_DefaultButton:
	ret = QDialogButtons::Reject;
	break;
    case SH_GroupBox_TextLabelColor:
	ret = (int) ( w ? w->colorGroup().foreground().rgb() : 0 );
	break;
    case SH_PopupMenu_SloppySubMenus:
	ret = TRUE;
	break;
    case SH_GroupBox_TextLabelVerticalAlignment:
	ret = Qt::AlignTop;
	break;
    case SH_ScrollView_FrameOnlyAroundContents:
	if (w && (w->isTopLevel() || !w->parentWidget() || w->parentWidget()->isTopLevel())
            && (::qt_cast<QScrollView*>(w) || w->inherits("QWorkspaceChild")))
	    ret = TRUE;
	else
	    ret = QWindowsStyle::styleHint(sh, w, opt, d);
	break;
    case SH_PopupMenu_Scrollable:
	ret = TRUE;
	break;
    case SH_RichText_FullWidthSelection:
	ret = TRUE;
	break;
    case SH_BlinkCursorWhenTextSelected:
	ret = FALSE;
	break;
    case SH_ScrollBar_StopMouseOverSlider:
        ret = TRUE;
        break;
    case SH_ListViewExpand_SelectMouseType:
    case SH_TabBar_SelectMouseType:
        ret = QEvent::MouseButtonRelease;
        break;
    case SH_ComboBox_Popup:
	ret = (!w || !((QComboBox*)w)->editable());
        break;
    case SH_Workspace_FillSpaceOnMaximize:
        ret = TRUE;
        break;
    case SH_Widget_ShareActivation:
        ret = TRUE;
        break;
    case SH_Header_ArrowAlignment:
        ret = Qt::AlignRight;
        break;
    case SH_TabBar_Alignment:
        ret = Qt::AlignHCenter;
        break;
    default:
        ret = QWindowsStyle::styleHint(sh, w, opt, d);
        break;
    }
    return ret;
}

/*! \reimp */
QSize QMacStyle::sizeFromContents(ContentsType contents, const QWidget *widget,
				       const QSize &contentsSize, const QStyleOption& opt) const
{
    QSize sz(contentsSize);
    switch(contents) {
    case CT_DialogButtons: {
	const QDialogButtons *dbtns = (const QDialogButtons *)widget;
	int w = contentsSize.width(), h = contentsSize.height();
	const int bwidth = pixelMetric(PM_DialogButtonsButtonWidth, widget),
		  bspace = pixelMetric(PM_DialogButtonsSeparator, widget),
		 bheight = pixelMetric(PM_DialogButtonsButtonHeight, widget);
	if(dbtns->orientation() == Horizontal) {
	    if(!w)
		w = bwidth;
	} else {
	    if(!h)
		h = bheight;
	}
	for(unsigned int i = 0, cnt = 0; i < (sizeof(macBtnOrder)/sizeof(macBtnOrder[0])); i++) {
	    if(dbtns->isButtonVisible(macBtnOrder[i])) {
		QSize szH = dbtns->sizeHint(macBtnOrder[i]);
		int mwidth = QMAX(bwidth, szH.width()), mheight = QMAX(bheight, szH.height());
		if(dbtns->orientation() == Horizontal)
		    h = QMAX(h, mheight);
		else
		    w = QMAX(w, mwidth);

		if(cnt)
		    w += bspace;
		cnt++;
		if(dbtns->orientation() == Horizontal)
		    w += mwidth;
		else
		    h += mheight;
		if(cnt == 2 && macBtnOrder[i] == QDialogButtons::Accept) { //yuck, but I need to put some extra space in there now..
		    if(dbtns->orientation() == Horizontal)
			w += 20;
		    else
			h += 20;
		}
	    }
	}
	if(dbtns->isButtonVisible(QDialogButtons::Help)) {
	    if(dbtns->buttonText(QDialogButtons::Help) == "?") {
		if(dbtns->orientation() == Horizontal)
		    w += 35;
	    } else {
		QSize szH = dbtns->sizeHint(QDialogButtons::Help);
		if(dbtns->orientation() == Horizontal)
		    w += QMAX(bwidth, szH.width());
		else
		    h += QMAX(bheight, szH.height());
	    }
	}
	const int fw = pixelMetric(PM_DefaultFrameWidth, widget) * 2;
	sz = QSize(w + fw, h + fw);
	break; }
    case CT_SpinBox:
	sz.setWidth(sz.width() + macSpinBoxSep); //leave space between the spinner and the editor
	break;
    case CT_TabWidget:
	sz.setWidth(sz.width() + 15); //leave a little bit of space around the tabs.. ###
	break;
    case CT_TabBarTab: {
        if (qMacVersion() < Qt::MV_10_DOT_3) {
            SInt32 lth = kThemeLargeTabHeightMax;
            if(qt_aqua_size_constrain(widget) == QAquaSizeSmall)
                lth = kThemeSmallTabHeightMax;
            if(sz.height() > lth)
                sz.setHeight(lth);
            QTab *tab;
            if (!opt.isDefault()) {
                tab = opt.tab();
                QIconSet *ic = tab->iconSet();
                if (ic && !ic->isNull()) {
                    sz.setHeight(QMAX(sz.height(),
                                 ic->pixmap(QIconSet::Small, QIconSet::Normal).height() + 4));
                }
            }
        } else {
            sz = QWindowsStyle::sizeFromContents(contents, widget, contentsSize, opt);
            sz.setHeight(QMAX(sz.height(), 22));
        }
	break; }
    case CT_PopupMenuItem: {
	if(!widget || opt.isDefault())
	    break;
	const QPopupMenu *popup = (const QPopupMenu *) widget;
	bool checkable = popup->isCheckable();
	QMenuItem *mi = opt.menuItem();
	int maxpmw = opt.maxIconWidth();
	int w = sz.width(), h = sz.height();

	if(mi->custom()) {
	    w = mi->custom()->sizeHint().width();
	    h = mi->custom()->sizeHint().height();
	    if(! mi->custom()->fullSpan())
		h += 8;
	} else if(mi->widget()) {
	} else if(mi->isSeparator()) {
	    w = 10;
	    SInt16 ash;
	    GetThemeMenuSeparatorHeight(&ash);
	    h = ash;
	} else {
	    if(mi->pixmap())
		h = QMAX(h, mi->pixmap()->height() + 4);
	    else
		h = QMAX(h, popup->fontMetrics().height() + 2);

	    if(mi->iconSet() != 0)
		h = QMAX(h, mi->iconSet()->pixmap(QIconSet::Small,
						  QIconSet::Normal).height() + 4);
	}

	if(! mi->text().isNull()) {
	    if(mi->text().find('\t') >= 0)
		w += 12;
	}
        if (mi->popup())
            w += 20;

	if(maxpmw)
	    w += maxpmw + 6;
	if(checkable && maxpmw < 20)
	    w += 20 - maxpmw;
	if(checkable || maxpmw > 0)
	    w += 2;
        if (::qt_cast<QComboBox*>(widget->parentWidget())
            && widget->parentWidget()->isVisible())
	    w = QMAX(w, querySubControlMetrics(CC_ComboBox, widget->parentWidget(), SC_ComboBoxEditField).width());
	else
	    w += 12;
	sz = QSize(w, h);
	break; }
    case CT_PushButton:
	sz = QCommonStyle::sizeFromContents(contents, widget, contentsSize, opt);
	sz = QSize(sz.width() + 16, sz.height()); //###
	break;
    case CT_ComboBox:
        sz.rwidth() += 37;
        break;
    default:
	sz = QWindowsStyle::sizeFromContents(contents, widget, contentsSize, opt);
	break;
    }
    {
	QSize macsz;
	if(qt_aqua_size_constrain(widget, contents, sz, &macsz) != QAquaSizeUnknown) {
	    if(macsz.width() != -1)
		sz.setWidth(macsz.width());
	    if(macsz.height() != -1)
		sz.setHeight(macsz.height());
	}
	//I hate to do this, but it seems to be needed
	if(contents == CT_PushButton || contents == CT_ToolButton) {
	    ThemeButtonKind bkind = (contents == CT_PushButton) ? kThemePushButton : kThemeBevelButton;
	    if(qt_aqua_size_constrain(widget) == QAquaSizeSmall && bkind == kThemeBevelButton)
                bkind = kThemeSmallBevelButton;
	    ThemeButtonDrawInfo info = { kThemeStateActive, kThemeButtonOff, kThemeAdornmentNone };
	    Rect macRect, myRect;
	    SetRect(&myRect,0, 0, sz.width(), sz.height());
	    GetThemeButtonBackgroundBounds(&myRect, bkind, &info, &macRect);
	    sz.setWidth(sz.width() +
			(myRect.left - macRect.left) + (macRect.right - myRect.right));
	    sz.setHeight(sz.height() +
			 (myRect.top - macRect.top) + (macRect.bottom - myRect.bottom));
	}
    }
    return sz;
}

/*! \reimp */
bool QMacStyle::event(QEvent *e)
{
    return QWindowsStyle::event(e);
}

/*!
    \enum QMacStyle::FocusRectPolicy

    This type is used to signify a widget's focus rectangle policy.

    \value FocusEnabled  show a focus rectangle when the widget has focus.
    \value FocusDisabled  never show a focus rectangle for the widget.
    \value FocusDefault  show a focus rectangle when the widget has
    focus and the widget is a QSpinWidget, QDateTimeEdit, QLineEdit,
    QListBox, QListView, editable QTextEdit, or one of their
    subclasses.
*/

/*!
    Sets the focus rectangle policy of \a w. The \a policy can be one of
    \l{QMacStyle::FocusRectPolicy}.

    \sa focusRectPolicy()
*/
void QMacStyle::setFocusRectPolicy( QWidget *w, FocusRectPolicy policy )
{
    QMacStylePrivate::PolicyState::focusMap.replace( w, policy );
    QMacStylePrivate::PolicyState::watchObject(w);
    if (w->hasFocus()) {
	w->clearFocus();
	w->setFocus();
    }
}

/*!
    Returns the focus rectangle policy for the widget \a w.

    The focus rectangle policy can be one of \l{QMacStyle::FocusRectPolicy}.

    \sa setFocusRectPolicy()
*/
QMacStyle::FocusRectPolicy QMacStyle::focusRectPolicy( QWidget *w )
{
    if (QMacStylePrivate::PolicyState::focusMap.contains(w))
	return QMacStylePrivate::PolicyState::focusMap[w];
    return FocusDefault;
}

/*!
    Sets the widget size policy of \a w. The \a policy can be one of
    \l{QMacStyle::WidgetSizePolicy}.

    \sa widgetSizePolicy()
*/
void QMacStyle::setWidgetSizePolicy( QWidget *w, WidgetSizePolicy policy )
{
    QMacStylePrivate::PolicyState::sizeMap.replace( w, policy );
    QMacStylePrivate::PolicyState::watchObject(w);
}

/*!
    Returns the widget size policy for the widget \a w.

    The widget size policy can be one of \l{QMacStyle::WidgetSizePolicy}.

    \sa setWidgetSizePolicy()
*/
QMacStyle::WidgetSizePolicy QMacStyle::widgetSizePolicy( QWidget *w )
{
    WidgetSizePolicy ret = SizeDefault;
    if(w) {
	if (QMacStylePrivate::PolicyState::sizeMap.contains(w))
	    ret = QMacStylePrivate::PolicyState::sizeMap[w];
	if(ret == SizeDefault) {
	    for(QWidget *p = w->parentWidget(TRUE); p; p = p->parentWidget(TRUE)) {
                if (QMacStylePrivate::PolicyState::sizeMap.contains(p)) {
                    ret = QMacStylePrivate::PolicyState::sizeMap[p];
                    if(ret != SizeDefault)
                        break;
                }
            }
	}
    }
    return ret;
}

#endif
