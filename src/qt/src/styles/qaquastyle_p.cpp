/****************************************************************************
** $Id: qt/qaquastyle_p.cpp   3.3.8   edited Mar 7 2006 $
**
** Definition of Aqua-style guidelines functions
**
** Created : 001129
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

#include "private/qaquastyle_p.h"
#include <qapplication.h>
#include <qobjectlist.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qptrlist.h>
#include <qguardedptr.h>
#include <qtextedit.h>
#include <qtoolbutton.h>
#include <qmainwindow.h>
#include <qlineedit.h>
#include <qsize.h>
#include <qslider.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qtable.h>
#ifdef Q_WS_MAC
#  include <qt_mac.h>
#  include <qmacstyle_mac.h>
#endif
#include <stdlib.h>

#define QMAC_QAQUASTYLE_SIZE_CONSTRAIN

/*****************************************************************************
  QAquaStyle debug facilities
 *****************************************************************************/
//#define DEBUG_SIZE_CONSTRAINT

QAquaFocusWidget::QAquaFocusWidget(bool noerase, QWidget *w)
    : QWidget(w, "magicFocusWidget", (noerase ? (WResizeNoErase | WRepaintNoErase) : 0)), d(NULL)
{
    if(noerase)
	setBackgroundMode(NoBackground);
}
#if 0
/* It's a real bummer I cannot use this, but you'll notice that sometimes
   the widget will scroll "offscreen" and the focus widget will remain visible
   (which looks quite bad). --Sam */
#define FOCUS_WIDGET_PARENT(x) x->topLevelWidget()
#else
#define FOCUS_WIDGET_PARENT(x) x->parentWidget(TRUE)
#endif
void QAquaFocusWidget::setFocusWidget(QWidget * widget)
{
    hide();
    if(d) {
	if(d->parentWidget())
	    d->parentWidget()->removeEventFilter(this);
	d->removeEventFilter(this);
    }
    d = NULL;
    if(widget && widget->parentWidget()) {
	d = widget;
	reparent(FOCUS_WIDGET_PARENT(d), pos());
	d->installEventFilter(this);
	d->parentWidget()->installEventFilter(this); //we do this so we can trap the ChildAdded event
        if (QComboBox *combo = ::qt_cast<QComboBox *>(d))
            combo->lineEdit()->installEventFilter(this);
	QPoint p(widget->mapTo(parentWidget(), QPoint(0, 0)));
	setGeometry(p.x() - focusOutset(), p.y() - focusOutset(), 
		    widget->width() + (focusOutset() * 2), widget->height() + (focusOutset() * 2));
	setPalette(widget->palette());
        setBackgroundMode(widget->backgroundMode());
	setMask(QRegion(rect()) - focusRegion());
	raise();
	show();
    }
}
bool QAquaFocusWidget::eventFilter(QObject * o, QEvent * e)
{
    if((e->type() == QEvent::ChildInserted || e->type() == QEvent::ChildRemoved) &&
	((QChildEvent*)e)->child() == this) {
	if(e->type() == QEvent::ChildRemoved)
	    o->removeEventFilter(this); //once we're removed, stop listening
	return TRUE; //block child events
    } else if(o == d) {
	switch (e->type()) {
	case QEvent::PaletteChange:
	    setPalette(d->palette());
	    break;
	case QEvent::Hide:
	    hide();
	    break;
	case QEvent::Show:
	    show();
	    break;
	case QEvent::Move: {
	    QPoint p(d->mapTo(parentWidget(), QPoint(0, 0)));
	    move(p.x() - focusOutset(), p.y() - focusOutset());
	    break;
	}
	case QEvent::Resize: {
	    QResizeEvent *re = (QResizeEvent*)e;
	    resize(re->size().width() + (focusOutset() * 2), 
		    re->size().height() + (focusOutset() * 2));
	    setMask(QRegion(rect()) - focusRegion());
	    break;
	}
	case QEvent::Reparent: {
	    QWidget *newp = FOCUS_WIDGET_PARENT(d);
	    QPoint p(d->mapTo(newp, QPoint(0, 0)));
	    newp->installEventFilter(this);
	    reparent(newp, p);
	    raise();
	    break; }
	default:
	    break;
	}
    } else if (QComboBox *combo = ::qt_cast<QComboBox *>(d)) {
        if (combo->lineEdit() == o && e->type() == QEvent::FocusOut)
            QApplication::sendEvent(combo, e);
    }
    return FALSE;
}

struct QAquaAnimatePrivate 
{
    QWidget *focus; //the focus widget
    QGuardedPtr<QPushButton> defaultButton, noPulse; //default pushbuttons
    int buttonTimerId;
    QPtrList<QProgressBar> progressBars; //progress bar information
    int progressTimerId;
    QPtrList<QListViewItem> lvis;
    int lviTimerID;
};
QAquaAnimate::QAquaAnimate() 
{
    d = new QAquaAnimatePrivate;
    d->focus = d->defaultButton = d->noPulse = NULL;
    d->lviTimerID = d->progressTimerId = d->buttonTimerId = -1;
}
QAquaAnimate::~QAquaAnimate()
{ 
    delete d; 
}
bool QAquaAnimate::addWidget(QWidget *w) 
{
    if(focusable(w)) {
	if(w->hasFocus()) 
	    setFocusWidget(w);
	w->installEventFilter(this);
    }
    if(w == d->defaultButton || d->progressBars.contains((QProgressBar*)w)) //already knew of it
	return FALSE;

    if(w->inherits("QPushButton")){
        QPushButton * btn = (QPushButton *) w;
        if(btn->isDefault() || (btn->autoDefault() && btn->hasFocus())){
	    d->defaultButton = btn;
            btn->installEventFilter(this);
            if(btn->isVisible() && d->buttonTimerId == -1) 
                d->buttonTimerId = startTimer(50);
        }
	return TRUE;
    } else if(w->inherits("QProgressBar")){
	w->installEventFilter(this);
	QObject::connect(w, SIGNAL(destroyed(QObject*)), this, SLOT(objDestroyed(QObject*)));
	if(w->isVisible() && d->progressTimerId == -1) {
	    d->progressBars.append((QProgressBar*)w);
	    d->progressTimerId = startTimer(50);
	}
	return TRUE;
    } else if(w->inherits("QListView")) {
#if 0
	QObject::connect(w, SIGNAL(collapsed(QListViewItem*)), this, SLOT(lvi(QListViewItem*)));
	QObject::connect(w, SIGNAL(expanded(QListViewItem*)),  this, SLOT(lvi(QListViewItem*)));
#endif
    }
    return FALSE;
}
void QAquaAnimate::removeWidget(QWidget *w) 
{
    if(focusWidget() == w) 
	setFocusWidget(NULL);

    if(w->inherits("QPushButton")) {
        QPushButton * btn = (QPushButton *) w;
        if(btn == d->defaultButton)
	    d->defaultButton = 0;
        if(d->buttonTimerId != -1) {
            killTimer(d->buttonTimerId);
            d->buttonTimerId = -1;
        }
    } else if(w->inherits("QProgressBar")) {
	d->progressBars.remove((QProgressBar *) w);
	if(d->progressBars.isEmpty() && d->progressTimerId != -1) {
	    killTimer(d->progressTimerId);
	    d->progressTimerId = -1;
	}
    } else if(w->inherits("QListView")) {
	QObject::disconnect(w, SIGNAL(collapsed(QListViewItem*)), this, SLOT(lvi(QListViewItem*)));
	QObject::disconnect(w, SIGNAL(expanded(QListViewItem*)),  this, SLOT(lvi(QListViewItem*)));
    }
}
void QAquaAnimate::lvi(QListViewItem *l)
{
    if(d->lvis.find(l) == -1) 
	d->lvis.append(l);
    if(d->lviTimerID == -1) 
	d->lviTimerID = startTimer(50);
}
void QAquaAnimate::objDestroyed(QObject *o)
{
    if(o == d->focus) 
	setFocusWidget(NULL);
    while(d->progressBars.remove((QProgressBar*)o));
}
bool QAquaAnimate::animatable(QAquaAnimate::Animates as, QListViewItem *l)
{
    if(as == AquaListViewItemOpen && d->lvis.find(l) != -1)
	return TRUE;
    return FALSE;
}
bool QAquaAnimate::animatable(QAquaAnimate::Animates as, QWidget *w)
{
    if(as == AquaPushButton && w->inherits("QPushButton")) {
	QPushButton *btn = (QPushButton *)w;
	if((!d->noPulse || (QPushButton*)d->noPulse == btn || !d->noPulse->isDown()) &&
	   btn->isEnabled() && (btn->isDefault() || (btn->autoDefault() && btn->hasFocus())) && 
	   ((QPushButton*)d->defaultButton == btn) && w == d->defaultButton)
	    return TRUE;
    } else if(as == AquaProgressBar && d->progressBars.find((QProgressBar*)w) != -1) {
	return TRUE;
    }
    return FALSE;
}
void QAquaAnimate::stopAnimate(QAquaAnimate::Animates as, QWidget *w)
{
    if(as == AquaPushButton && w->inherits("QPushButton")) {
	if((QPushButton*)d->defaultButton == (QPushButton*)w) {
	    d->defaultButton = NULL;
	    if(d->buttonTimerId != -1) {
		killTimer(d->buttonTimerId);
		d->buttonTimerId = -1;
	    }
	}
    } else if(as == AquaProgressBar) {
	d->progressBars.remove((QProgressBar*)w);
	if(d->progressTimerId != -1 && d->progressBars.isEmpty()) {
	    killTimer(d->progressTimerId);
	    d->progressTimerId = -1;
	}
    }
}
void QAquaAnimate::stopAnimate(QAquaAnimate::Animates as, QListViewItem *l)
{
    if(as == AquaListViewItemOpen) {
	d->lvis.remove(l);
	if(d->lviTimerID != -1 && d->lvis.isEmpty()) {
	    killTimer(d->lviTimerID);
	    d->lviTimerID = -1;
	}
    }
}
void QAquaAnimate::timerEvent(QTimerEvent * te)
{
    if(te->timerId() == d->buttonTimerId) {
	if(d->defaultButton && d->defaultButton->isEnabled() && d->defaultButton->isVisibleTo(0) &&
	    (d->defaultButton->isDefault() || (d->defaultButton->autoDefault() && d->defaultButton->hasFocus()) )) {
	    if(doAnimate(AquaPushButton)) 
		d->defaultButton->repaint(FALSE);
	}
    } else if(te->timerId() == d->lviTimerID && !d->lvis.isEmpty()) {
	if(doAnimate(AquaListViewItemOpen)) {
	    if(d->lvis.count() == 1) {
		d->lvis.first()->repaint();
	    } else {
		for(QPtrListIterator<QListViewItem> it(d->lvis); it.current(); ++it) 
		    (*it)->repaint();
	    }
	}
    } else if(te->timerId() == d->progressTimerId && !d->progressBars.isEmpty()) {
	if(doAnimate(AquaProgressBar)) {
	    if(d->progressBars.count() == 1) {
		QProgressBar *b = d->progressBars.first();
		if(b->progress() > 0)
		    b->repaint(FALSE);
	    } else {
		for(QPtrListIterator<QProgressBar> it(d->progressBars); it.current(); ++it) {
		    if((*it)->progress() > 0)
			(*it)->repaint(FALSE);
		}
	    }
	}
    }
}
bool QAquaAnimate::eventFilter(QObject * o, QEvent * e)
{
    //focus
    if(o->isWidgetType() && focusWidget() && focusable((QWidget *)o) &&
       ((e->type() == QEvent::FocusOut && focusWidget() == o) ||
	(e->type() == QEvent::FocusIn && focusWidget() != o)))  { //restore it
	if(((QFocusEvent *)e)->reason() != QFocusEvent::Popup) 
	    setFocusWidget(NULL);
    }
    if(o && o->isWidgetType() && e->type() == QEvent::FocusIn) {
	QWidget *w = (QWidget *)o;
	if(focusable(w)) 
	    setFocusWidget(w);
    }
    //animate
    if(o && o->isWidgetType() && e->type() == QEvent::FocusIn) {
	QWidget *w = (QWidget *)o;
	if(o->inherits("QPushButton") && ((QPushButton *)w)->autoDefault()) {
	    // Kb Focus received - make this the default button
	    d->defaultButton = (QPushButton *) w;
            if(w->isVisible() && d->buttonTimerId == -1)
                d->buttonTimerId = startTimer(50);
	}
    } else if(e->type() == QEvent::Show && o->inherits("QProgressBar")) {
	d->progressBars.append((QProgressBar*)o);
	if(d->progressTimerId == -1)
	    d->progressTimerId = startTimer(50);
    } else if(e->type() == QEvent::Hide && d->progressBars.find((QProgressBar*)o) != -1) {
	while(d->progressBars.remove((QProgressBar*)o));
	if(d->progressBars.isEmpty() && d->progressTimerId != -1) {
	    killTimer(d->progressTimerId);
	    d->progressTimerId = -1;
	}
    } else if(e->type() == QEvent::Hide && d->defaultButton == o) {
	d->defaultButton = NULL;
	if(d->buttonTimerId != -1) {
	    killTimer(d->buttonTimerId);
	    d->buttonTimerId = -1;
	}
    } else if((e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonRelease) &&
	       o->inherits("QPushButton")) {
	QMouseEvent *me = (QMouseEvent*)e;
	d->noPulse = NULL;
	if(me->type() == QEvent::MouseButtonPress && me->button() == Qt::LeftButton)
	    d->noPulse = (QPushButton*)o;
    } else if(o && (e->type() == QEvent::FocusOut || e->type() == QEvent::Show) &&
	       o->inherits("QPushButton")) {
	QPushButton *btn = (QPushButton *)o;
	// Find the correct button to use as default button
	QObjectList *list = btn->topLevelWidget()->queryList("QPushButton");
	QObjectListIt it(*list);
	QPushButton * pb;
	while((pb = (QPushButton*)it.current())) {
	    ++it;
	    if(((e->type() == QEvent::FocusOut) && (pb->isDefault() ||
						    (pb->autoDefault() && pb->hasFocus())) && (pb != btn)) ||
		((e->type() == QEvent::Show) && pb->isDefault()))
	    {
		QPushButton * tmp = d->defaultButton;
		d->defaultButton = 0;
		if(tmp)
		    tmp->repaint(FALSE);
		if(pb->topLevelWidget()->isActiveWindow())
		    d->defaultButton = pb;
		break;
	    }
	}
	delete list;
	if(d->defaultButton) {
	    if(d->buttonTimerId == -1)
                d->buttonTimerId = startTimer(50);
	} else if(d->buttonTimerId != -1) {
	    killTimer(d->buttonTimerId);
	    d->buttonTimerId = -1;
	}
    }
    return FALSE;
}
QWidget *QAquaAnimate::focusWidget() const
{
    return d->focus;
}
void QAquaAnimate::setFocusWidget(QWidget *w)
{
    if(w) {
	QWidget *top = w->parentWidget();
	while(!top->isTopLevel() && !top->testWFlags(WSubWindow))
	    top = top->parentWidget();
	if(top->inherits("QMainWindow")) {
	    QWidget *central = ((QMainWindow*)top)->centralWidget();
	    for(QWidget *par = w; par; par = par->parentWidget(TRUE)) {
		if(par == central) {
		    top = central;
		    break;
		}
	    }
	}
	if(top && (w->width() < top->width() - 30 || w->height() < top->height() - 40)) {
	    if(QSpinWidget *spin = ::qt_cast<QSpinWidget *>(w)) //transfer to the editor
		w = spin->editWidget();
            if (::qt_cast<QLineEdit *>(w) && ::qt_cast<QComboBox *>(w->parentWidget())) {
                w = w->parentWidget();
            }
	} else {
	    w = NULL;
	}
    }
    if(w == d->focus)
	return;
    doFocus(w);
    if(d->focus)
	QObject::disconnect(d->focus, SIGNAL(destroyed(QObject*)), this, SLOT(objDestroyed(QObject*)));
    if((d->focus = w))
	QObject::connect(w, SIGNAL(destroyed(QObject*)), this, SLOT(objDestroyed(QObject*)));
}
bool QAquaAnimate::focusable(QWidget *w)
{
#ifdef Q_WS_MAC
    QStyle *style = &w->style();
    if (style->inherits("QMacStyle")) {
	QMacStyle::FocusRectPolicy p = ((QMacStyle*)style)->focusRectPolicy(w);
	if (p == QMacStyle::FocusDisabled)
	    return false;
	if (p == QMacStyle::FocusEnabled)
	    return true;
    }
#endif
    return (w && w->parentWidget(TRUE) && !w->isHidden() &&
	    (w->inherits("QSpinWidget") || w->inherits("QDateTimeEditor") ||
	     (w->inherits("QLineEdit") && w->parentWidget()->inherits("QSpinWidget")) ||
	      (w->inherits("QTextEdit") && !((QTextEdit*)w)->isReadOnly()) ||
            (w->inherits("QLineEdit") &&
             (((QFrame*)w)->frameStyle() != QFrame::NoFrame
                || ::qt_cast<QComboBox *>(w->parentWidget())))
            || (w->inherits("QComboBox") && ((QComboBox *)w)->editable())
             || w->inherits("QListBox") || w->inherits("QListView")));
}

#if defined(QMAC_QAQUASTYLE_SIZE_CONSTRAIN) || defined(DEBUG_SIZE_CONSTRAINT)
static QAquaWidgetSize qt_aqua_guess_size(const QWidget *widg, QSize large, QSize small)
{
    if(large == QSize(-1, -1)) {
	if(small == QSize(-1, -1))
	    return QAquaSizeUnknown;
	return QAquaSizeSmall;
    } else if(small == QSize(-1, -1)) {
	return QAquaSizeLarge;
    }

    if(widg->topLevelWidget()->inherits("QDockWindow") || getenv("QWIDGET_ALL_SMALL")) {
	//if(small.width() != -1 || small.height() != -1)
	    return QAquaSizeSmall;
    }

#if 0
    /* Figure out which size we're closer to, I just hacked this in, I haven't
       tested it as it would probably look pretty strange to have some widgets
       big and some widgets small in the same window?? -Sam */
    int large_delta=0;
    if(large.width() != -1) {
	int delta = large.width() - widg->width();
	large_delta += delta * delta;
    }
    if(large.height() != -1) {
	int delta = large.height() - widg->height();
	large_delta += delta * delta;
    }
    int small_delta=0;
    if(small.width() != -1) {
	int delta = small.width() - widg->width();
	small_delta += delta * delta;
    }
    if(small.height() != -1) {
	int delta = small.height() - widg->height();
	small_delta += delta * delta;
    }
    if(small_delta < large_delta) 
	return QAquaSizeSmall;
#endif
    return QAquaSizeLarge;
}
#ifdef Q_WS_MAC
static int qt_mac_aqua_get_metric(ThemeMetric met)
{
    SInt32 ret;
    GetThemeMetric(met, &ret);
    return ret;
}
#endif
static QSize qt_aqua_get_known_size(QStyle::ContentsType ct, const QWidget *widg, QSize szHint, QAquaWidgetSize sz)
{
    QSize ret(-1, -1);
    if(sz != QAquaSizeSmall && sz != QAquaSizeLarge) {
	qDebug("Not sure how to return this..");
	return ret;
    }
    if(widg && widg->ownFont()) //if you're using a custom font, no constraints for you!
	return ret;

    if(ct == QStyle::CT_CustomBase && widg) {
	if(widg->inherits("QPushButton"))
	    ct = QStyle::CT_PushButton;
	else if(widg->inherits("QRadioButton")) 
	    ct = QStyle::CT_RadioButton;
	else if(widg->inherits("QCheckBox")) 
	    ct = QStyle::CT_CheckBox;
	else if(widg->inherits("QComboBox")) 
	    ct = QStyle::CT_ComboBox;
	else if(widg->inherits("QToolButton"))
	    ct = QStyle::CT_ToolButton;
	else if(widg->inherits("QSlider"))
	    ct = QStyle::CT_Slider;
	else if(widg->inherits("QProgressBar")) 
	    ct = QStyle::CT_ProgressBar;
	else if(widg->inherits("QLineEdit")) 
	    ct = QStyle::CT_LineEdit;
	else if(widg->inherits("QHeader")) 
	    ct = QStyle::CT_Header;
	else if(widg->inherits("QMenuBar"))
	    ct = QStyle::CT_MenuBar;
	else if(widg->inherits("QSizeGrip"))
	    ct = QStyle::CT_SizeGrip;
	else
	    return ret;
    }

    if(ct == QStyle::CT_PushButton) {
	QPushButton *psh = (QPushButton*)widg;
	int minw = -1;
        // Aqua Style guidelines restrict the size of OK and Cancel buttons to 68 pixels.
        // However, this doesn't work for German, therefore only do it for English,
        // I suppose it would be better to do some sort of lookups for languages
        // that like to have really long words.
        // Also ignore this if we have a valid iconset, as the pixmap takes up space.
	if ((psh->text() == "OK" || psh->text() == "Cancel")
                && (!psh->iconSet() || (psh->iconSet() && psh->iconSet()->isNull())))
	    minw = 69;

         // Constrain the height according to Aqua guidelines.
         // Unless, *you* break them because you have '\n' in your text. Don't
         // worry, I still think you're cool.
        if (psh->text().find('\n') != -1)
            ret = QSize(minw, -1);
#ifdef Q_WS_MAC
        else if(sz == QAquaSizeLarge)
	    ret = QSize(minw, qt_mac_aqua_get_metric(kThemeMetricPushButtonHeight));
	else
	    ret = QSize(minw, qt_mac_aqua_get_metric(kThemeMetricSmallPushButtonHeight));
#else
        else if(sz == QAquaSizeLarge)
	    ret = QSize(minw, 20);
	else
	    ret = QSize(minw, 17);
#endif
#if 0 //Not sure we are applying the rules correctly for RadioButtons/CheckBoxes --Sam
    } else if(ct == QStyle::CT_RadioButton) {
	QRadioButton *rdo = (QRadioButton*)widg;
        // Exception for case where multiline radiobutton text requires no size constrainment
	if(rdo->text().find('\n') != -1) 
	    return ret;
#ifdef Q_WS_MAC
	if(sz == QAquaSizeLarge)
	    ret = QSize(-1, qt_mac_aqua_get_metric(kThemeMetricRadioButtonHeight));
	else 
	    ret = QSize(-1, qt_mac_aqua_get_metric(kThemeMetricSmallRadioButtonHeight));
#else
	if(sz == QAquaSizeLarge)
	    ret = QSize(-1, 18);
	else 
	    ret = QSize(-1, 15);
#endif
    } else if(ct == QStyle::CT_CheckBox) {
#ifdef Q_WS_MAC
	if(sz == QAquaSizeLarge)
	    ret = QSize(-1, qt_mac_aqua_get_metric(kThemeMetricCheckBoxHeight));
	else 
	    ret = QSize(-1, qt_mac_aqua_get_metric(kThemeMetricSmallCheckBoxHeight));
#else
	if(sz == QAquaSizeLarge)
	    ret = QSize(-1, 18);
	else 
	    ret = QSize(-1, 16);
#endif
#endif
    } else if(ct == QStyle::CT_SizeGrip) {
#ifdef Q_WS_MAC
	Rect r;
	Point p = { 0, 0 };
	ThemeGrowDirection dir = kThemeGrowRight | kThemeGrowDown;
	if(QApplication::reverseLayout())
	    dir = kThemeGrowLeft | kThemeGrowDown;
	if(GetThemeStandaloneGrowBoxBounds(p, dir, sz == QAquaSizeSmall, &r) == noErr) 
	    ret = QSize(r.right - r.left, r.bottom - r.top);
#else
	if(sz == QAquaSizeLarge)
	    ret = QSize(15, 15);
	else
	    ret = QSize(10, 10);
#endif
    } else if(ct == QStyle::CT_ComboBox) {
#ifdef Q_WS_MAC
        const QComboBox *cmb = ::qt_cast<const QComboBox *>(widg);
	if (sz == QAquaSizeLarge ||
            (sz == QAquaSizeSmall && cmb && cmb->editable() && qMacVersion() < Qt::MV_PANTHER)) {
            int offset = (cmb && cmb->editable()) ? 4 : 7;
	    ret = QSize(-1, qt_mac_aqua_get_metric(kThemeMetricPopupButtonHeight) + offset);
        } else {
	    ret = QSize(-1, qt_mac_aqua_get_metric(kThemeMetricSmallPopupButtonHeight)+1);
        }
#else
	if(sz == QAquaSizeLarge)
	    ret = QSize(-1, 20);
	else
	    ret = QSize(-1, 17);
#endif
    } else if(ct == QStyle::CT_ToolButton && sz == QAquaSizeSmall) {
	int width = 0, height = 0;
	if(szHint == QSize(-1, -1)) { //just 'guess'..
	    QToolButton *bt = (QToolButton*)widg;
	    if(!bt->iconSet().isNull()) {
		QIconSet::Size sz = QIconSet::Small;
		if(bt->usesBigPixmap()) 
		    sz = QIconSet::Large;
		QSize iconSize = QIconSet::iconSize(sz);
		QPixmap pm = bt->iconSet().pixmap(sz, QIconSet::Normal);
		width = QMAX(width, QMAX(iconSize.width(), pm.width()));
		height = QMAX(height, QMAX(iconSize.height(), pm.height()));
	    }
	    if(!bt->text().isNull() && bt->usesTextLabel()) {
		int text_width = bt->fontMetrics().width(bt->text()),
		   text_height = bt->fontMetrics().height();
		if(bt->textPosition() == QToolButton::Under) {
		    width = QMAX(width, text_width);
		    height += text_height;
		} else {
		    width += text_width;
		    width = QMAX(height, text_height);
		}
	    }
	} else {
	    width = szHint.width();
	    height = szHint.height();
	}
	width =  QMAX(20, width +  5); //border
	height = QMAX(20, height + 5); //border
	ret = QSize(width, height);
    } else if(ct == QStyle::CT_Slider) {
	int w = -1;
	QSlider *sld = (QSlider*)widg;
#ifdef Q_WS_MAC
	if(sz == QAquaSizeLarge) {
	    if(sld->orientation() == Qt::Horizontal) {
		w = qt_mac_aqua_get_metric(kThemeMetricHSliderHeight);
		if(sld->tickmarks() != QSlider::NoMarks) 
		    w += qt_mac_aqua_get_metric(kThemeMetricHSliderTickHeight);
	    } else {
		w = qt_mac_aqua_get_metric(kThemeMetricVSliderWidth);
		if(sld->tickmarks() != QSlider::NoMarks) 
		    w += qt_mac_aqua_get_metric(kThemeMetricVSliderTickWidth);
	    }
	} else {
	    if(sld->orientation() == Qt::Horizontal) {
		w = qt_mac_aqua_get_metric(kThemeMetricSmallHSliderHeight);
		if(sld->tickmarks() != QSlider::NoMarks) 
		    w += qt_mac_aqua_get_metric(kThemeMetricSmallHSliderTickHeight);
	    } else {
		w = qt_mac_aqua_get_metric(kThemeMetricSmallVSliderWidth);
		if(sld->tickmarks() != QSlider::NoMarks) 
		    w += qt_mac_aqua_get_metric(kThemeMetricSmallVSliderTickWidth);
	    }
	}
#else
	if(sld->tickmarks() == QSlider::NoMarks) {
	    if(sz == QAquaSizeLarge)
		w = 18;
	    else 
		w = 16;
	} else {
	    if(sz == QAquaSizeLarge)
		w = 25;
	    else 
		w = 18;
	}
#endif
	if(sld->orientation() == Qt::Horizontal) 
	    ret.setHeight(w);
	else
	    ret.setWidth(w);
    } else if(ct == QStyle::CT_ProgressBar) {
#ifdef Q_WS_MAC
	if(sz == QAquaSizeLarge)
	    ret = QSize(-1, qt_mac_aqua_get_metric(kThemeMetricLargeProgressBarThickness));
	else 
	    ret = QSize(-1, qt_mac_aqua_get_metric(kThemeMetricNormalProgressBarThickness));
#else
	if(sz == QAquaSizeLarge)
	    ret = QSize(-1, 16);
	else 
	    ret = QSize(-1, 10);
#endif
    } else if(ct == QStyle::CT_LineEdit) {
	if(!widg || !widg->parentWidget() || !widg->parentWidget()->inherits("QComboBox")) {
	    //should I take into account the font dimentions of the lineedit? -Sam
	    if(sz == QAquaSizeLarge)
		ret = QSize(-1, 22);
	    else 
		ret = QSize(-1, 19);
	}
    }
#ifdef Q_WS_MAC
    else if(ct == QStyle::CT_Header) {
	if(sz == QAquaSizeLarge
#ifndef QT_NO_TABLE
                && (widg && !::qt_cast<QTable *>(widg->parentWidget()))
#endif
          ) {
            // constrain only if we don't have a \n in any of the header sections
            bool constrain = TRUE;
            int myCount = 0;
            QHeader *header = ::qt_cast<QHeader *>(widg);
            if (header)
                myCount = header->count();
            for (int i = 0; i < myCount; ++i) {
                constrain = header->label(i).find('\n') == -1;
                if (!constrain)
                    break;
            }
            if (constrain)
                ret = QSize(-1, qt_mac_aqua_get_metric(kThemeMetricListHeaderHeight));
        }
    } else if(ct == QStyle::CT_MenuBar) {
	if(sz == QAquaSizeLarge) {
	    SInt16 size;
	    if(!GetThemeMenuBarHeight(&size))
		ret = QSize(-1, size);
	}
    }
#endif
    return ret;
}
#endif
QAquaWidgetSize qt_aqua_size_constrain(const QWidget *widg, QStyle::ContentsType ct, 
				       QSize szHint, QSize *insz)
{
#if defined(QMAC_QAQUASTYLE_SIZE_CONSTRAIN) || defined(DEBUG_SIZE_CONSTRAINT)
    if(!widg) {
	if(insz)
	    *insz = QSize();
	if(getenv("QWIDGET_ALL_SMALL")) 
	    return QAquaSizeSmall;
	return QAquaSizeUnknown;
    }
    QSize large = qt_aqua_get_known_size(ct, widg, szHint, QAquaSizeLarge),
	  small = qt_aqua_get_known_size(ct, widg, szHint, QAquaSizeSmall);
#ifdef Q_WS_MAC
    bool guess_size = false;
    QAquaWidgetSize ret = QAquaSizeUnknown;
    QStyle *style = &widg->style();
    if (style->inherits("QMacStyle")) {
	QMacStyle::WidgetSizePolicy wsp = ((QMacStyle*)style)->widgetSizePolicy((QWidget*)widg);
	if(wsp == QMacStyle::SizeDefault)
	    guess_size = true;
	else if(wsp == QMacStyle::SizeSmall)
	    ret = QAquaSizeSmall;
	else if(wsp == QMacStyle::SizeLarge)
	    ret = QAquaSizeLarge;
    }
    if(guess_size)
	ret = qt_aqua_guess_size(widg, large, small);
#else
    QAquaWidgetSize ret = qt_aqua_guess_size(widg, large, small);
#endif

    QSize *sz = NULL;
    if(ret == QAquaSizeSmall)
	sz = &small;
    else if(ret == QAquaSizeLarge)
	sz = &large;
    if(insz)
	*insz = sz ? *sz : QSize(-1, -1);
#ifdef DEBUG_SIZE_CONSTRAINT
    if(sz) {
	const char *size_desc = "Unknown";
	if(sz == &small)
	    size_desc = "Small";
	else if(sz == &large)
	    size_desc = "Large";
	qDebug("%s - %s: %s taken (%d, %d) [ %d, %d ]", widg ? widg->name() : "*Unknown*", 
	       widg ? widg->className() : "*Unknown*", size_desc, widg->width(), widg->height(), 
	       sz->width(), sz->height());
    }
#endif
    return ret;
#else
    if(insz)
	*insz = QSize();
    Q_UNUSED(widg);
    Q_UNUSED(ct);
    Q_UNUSED(szHint);
    return QAquaSizeUnknown;
#endif
}
