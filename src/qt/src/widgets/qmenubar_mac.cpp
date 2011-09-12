/****************************************************************************
** $Id: qt/qmenubar_mac.cpp   3.3.8   edited Jan 11 14:38 $
**
** Implementation of QMenuBar bindings for Apple System Menubar
**
** Copyright (C) 1992-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
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

#include "qplatformdefs.h"
#if defined(Q_WS_MAC)
#include "qt_mac.h"
#if !defined(QMAC_QMENUBAR_NO_NATIVE)

#include <qaccel.h>
#include <qapplication.h>
#include <qdockarea.h>
#include <qdockwindow.h>
#include <qguardedptr.h>
#include <qintdict.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qptrdict.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtoolbar.h>

/*****************************************************************************
  QMenubar debug facilities
 *****************************************************************************/
//#define DEBUG_MENUBAR_ACTIVATE

IconRef qt_mac_create_iconref(const QPixmap &px); //qpixmap_mac.cpp
extern QString cfstring2qstring(CFStringRef); //qglobal.cpp
extern CFStringRef qstring2cfstring(const QString &); //qglobal.cpp
void qt_event_request_menubarupdate(); //qapplication_mac.cpp
bool qt_modal_state(); //qapplication_mac.cpp

#endif

void qt_mac_command_set_enabled(UInt32 cmd, bool b)
{
#if 0
    qDebug("setting %c%c%c%c to %s", (char)(cmd >> 24) & 0xFF, (char)(cmd >> 16) & 0xFF,
	   (char)(cmd >> 8) & 0xFF, (char)cmd & 0xFF,  b ? "on" : "off");
#endif

    if(b) {
	EnableMenuCommand(NULL, cmd);
	if(MenuRef mr = GetApplicationDockTileMenu())
	    EnableMenuCommand(mr, cmd);
    } else {
	DisableMenuCommand(NULL, cmd);
	if(MenuRef mr = GetApplicationDockTileMenu())
	    DisableMenuCommand(mr, cmd);
    }
}

#if !defined(QMAC_QMENUBAR_NO_NATIVE)

static bool qt_mac_no_menubar_icons = FALSE;
void qt_mac_set_no_menubar_icons(bool b) { qt_mac_no_menubar_icons = b; } //backdoor to disable menubar icons

//internal class
class QMenuBar::MacPrivate {
public:
    MacPrivate() : commands(0), popups(0), mac_menubar(0),
	apple_menu(0), in_apple(0), dirty(FALSE), modal(FALSE) { }
    ~MacPrivate() { clear(); delete popups; delete commands; }

    class CommandBinding {
    public:
	CommandBinding(QPopupMenu *m, uint i) : qpopup(m), index(i) { }
	QGuardedPtr<QPopupMenu> qpopup;
	int index;
    };
    QIntDict<CommandBinding> *commands;
    inline CommandBinding *findCommandBinding(int cmd) const {
        CommandBinding *ret = 0;
        if(commands && (ret = commands->find(cmd)) && !ret->qpopup)
            ret = 0;
        return ret;
    }

    class PopupBinding {
    public:
	PopupBinding(QPopupMenu *m, MenuRef r, int i, bool b) : qpopup(m), macpopup(r), id(i), tl(b) { }
	~PopupBinding() { if(tl) DeleteMenu(GetMenuID(macpopup)); DisposeMenu(macpopup); }
	QGuardedPtr<QPopupMenu> qpopup;
	MenuRef macpopup;
	int id;
	bool tl;
    };
    QIntDict<PopupBinding> *popups;
    inline PopupBinding *findPopupBinding(int id) const {
        PopupBinding *ret = 0;
        if(popups && (ret = popups->find(id)) && !ret->qpopup)
            ret = 0;
        return ret;
    }

    MenuBarHandle mac_menubar;
    MenuRef apple_menu;
    int in_apple;
    bool dirty, modal;


    void clear() {
	in_apple = 0;
	if(apple_menu) {
	    DeleteMenu(GetMenuID(apple_menu));
	    DisposeMenu(apple_menu);
	}
	if(popups)
	    popups->clear();
	if(commands)
	    commands->clear();
	if(mac_menubar) {
	    DisposeMenuBar(mac_menubar);
	    mac_menubar = 0;
	}
    }
};
static QGuardedPtr<QMenuBar> fallbackMenuBar; //The current global menubar
static QGuardedPtr<QMenuBar> activeMenuBar; //The current global menubar



void qt_mac_set_modal_state(bool b, QMenuBar *mb)
{
    if(mb && mb != activeMenuBar) { //shouldn't be possible, but just in case
	qWarning("%s:%d: This cannot happen!", __FILE__, __LINE__);
	mb = 0;
    }

    MenuRef mr = AcquireRootMenu();
    for(int i = 1; i < CountMenuItems(mr); i++) {
	MenuRef mr2;
	GetMenuItemHierarchicalMenu(mr, i+1, &mr2);
	bool enabled = TRUE;
	if(!b && mb && mb->mac_d->popups) {
	    QMenuBar::MacPrivate::PopupBinding *pb = mb->mac_d->findPopupBinding(GetMenuID(mr2));
	    if(pb && !pb->tl)
		qWarning("%s:%d That cannot happen either!", __FILE__, __LINE__);
	    enabled = (!pb || (pb->qpopup && pb->qpopup->isEnabled()));
	} else if(b) {
	    enabled = FALSE;
	}
	if(enabled)
	    EnableMenuItem(mr2, 0);
	else
	    DisableMenuItem(mr2, 0);
    }
    ReleaseMenu(mr);

    UInt32 commands[] = { kHICommandQuit, kHICommandPreferences, kHICommandAbout, 'CUTE', 0 };
    for(int c = 0; commands[c]; c++) {
	bool enabled = TRUE;
	if(!b && mb && mb->mac_d->commands) {
	    if(QMenuBar::MacPrivate::CommandBinding *cb = mb->mac_d->findCommandBinding(commands[c])) {
		if(cb->qpopup)
		    enabled = cb->qpopup->isItemEnabled(cb->qpopup->idAt(cb->index));
	    }
	} else if(b) {
	    enabled = FALSE;
	}
	qt_mac_command_set_enabled(commands[c], enabled);
    }
}

static void qt_mac_clear_menubar()
{
    ClearMenuBar();
    qt_mac_command_set_enabled(kHICommandPreferences, FALSE);
    InvalMenuBar();
}

#if !defined(QMAC_QMENUBAR_NO_EVENT)
//event callbacks
QMAC_PASCAL OSStatus
QMenuBar::qt_mac_menubar_event(EventHandlerCallRef er, EventRef event, void *)
{
    UInt32 ekind = GetEventKind(event), eclass = GetEventClass(event);
    bool handled_event = TRUE;
    switch(eclass) {
    case kEventClassMenu: {
	qDebug("happened %d", ekind);
	MenuRef menu;
	GetEventParameter(event, kEventParamDirectObject, typeMenuRef, 0,
			  sizeof(menu), 0, &menu);
	int mid = GetMenuID(menu);
	if(MacPrivate::PopupBinding *mpb = activeMenuBar->mac_d->findPopupBinding(mid)) {
	    short idx;
	    GetEventParameter(event, kEventParamMenuItemIndex, typeMenuItemIndex, 0,
			      sizeof(idx), 0, &idx);
	    MenuCommand cmd;
	    GetMenuItemCommandID(mpb->macpopup, idx, &cmd);
	    QMenuItem *it = mpb->qpopup->findItem(cmd);
	    if(!it->custom()) {
		handled_event = FALSE;
		break;
	    }
	    qDebug("made it here..");

	    if(ekind == kEventMenuMeasureItemHeight) {
		short h = it->custom()->sizeHint().height();
		SetEventParameter(event, kEventParamMenuItemHeight, typeShortInteger,
				  sizeof(h), &h);
	    } else if(ekind == kEventMenuMeasureItemWidth) {
		short w = it->custom()->sizeHint().width();
		SetEventParameter(event, kEventParamMenuItemWidth, typeShortInteger,
				  sizeof(w), &w);
	    } else if(ekind == kEventMenuDrawItemContent) {
		handled_event = FALSE;
	    } else {
		CallNextEventHandler(er, event);
		Rect r;
		GetEventParameter(event, kEventParamMenuTextBounds, typeQDRectangle, 0,
				  sizeof(r), 0, &r);
		QMacSavedPortInfo fi;
		::RGBColor f;
		f.red = 256*256;
		f.blue = f.green = 0;
		RGBForeColor(&f);
		PaintRect(&r);
		handled_event = FALSE;
	    }
	} else {
	    handled_event = FALSE;
	}
	break; }
    default:
	handled_event = FALSE;
	break;
    }
    if(!handled_event) //let the event go through
	return CallNextEventHandler(er, event);
    return noErr; //we eat the event
}
static EventHandlerRef mac_menubarEventHandler = 0;
static EventHandlerUPP mac_menubarEventUPP = 0;
static void qt_mac_clean_menubar_event()
{
    if(mac_menubarEventHandler) {
	RemoveEventHandler(mac_menubarEventHandler);
	mac_menubarEventHandler = 0;
    }
    if(mac_menubarEventUPP) {
	DisposeEventHandlerUPP(mac_menubarEventUPP);
	mac_menubarEventUPP = 0;
    }
}
void QMenuBar::qt_mac_install_menubar_event(MenuRef ref)
{
    if(mac_menubarEventHandler)
	return;
    static EventTypeSpec menu_events[] = {
	{ kEventClassMenu, kEventMenuMeasureItemWidth },
	{ kEventClassMenu, kEventMenuMeasureItemHeight },
	{ kEventClassMenu, kEventMenuDrawItemContent }
    };
    if(!mac_menubarEventUPP)
	mac_menubarEventUPP = NewEventHandlerUPP(qt_mac_menubar_event);
    InstallMenuEventHandler(ref, mac_menubarEventUPP,
			    GetEventTypeCount(menu_events), menu_events, 0,
			    &mac_menubarEventHandler);
    qAddPostRoutine(qt_mac_clean_menubar_event);
}
#endif

/* utility functions */
static QString qt_mac_no_ampersands(QString str, CFStringRef *cf=NULL) {
    for(int w = 0; (w=str.find('&', w)) != -1; ) {
	if(w < (int)str.length()) {
	    str.remove(w, 1);
	    if(str[w] == '&')
		w++;
	}
    }
    if(cf)
	*cf = qstring2cfstring(str);
    return str;
}

/*!
    \internal
*/
bool QPopupMenu::macPopupMenu(const QPoint &p, int index)
{
#if 0
    if(index == -1 && activeMenuBar && activeMenuBar->mac_d->popups) {
	MenuRef ref = NULL;
	for(QIntDictIterator<QMenuBar::MacPrivate::PopupBinding> it(*(activeMenuBar->mac_d->popups)); it.current(); ++it) {
	    if(it.current()->qpopup == this) {
		ref = it.current()->macpopup;
		break;
	    }
	}
	if(ref) {
	    activeMenuBar->syncPopups(ref, this);
	    InvalidateMenuSize(ref);
	    PopUpMenuSelect(ref, p.y(), p.x(), -1);
	    return TRUE; //we did it for Qt..
	}
    }
#else
    Q_UNUSED(p);
    Q_UNUSED(index);
#endif
    return FALSE;
}


#if !defined(QMAC_QMENUBAR_NO_MERGE)
static bool qt_mac_no_menubar_merge = FALSE;
void qt_mac_set_no_menubar_merge(bool b) { qt_mac_no_menubar_merge = b; } //backdoor to disable merging
uint QMenuBar::isCommand(QMenuItem *it, bool just_check)
{
    if(qt_mac_no_menubar_merge || it->popup() || it->custom() || it->isSeparator())
	return 0;

    QString t = qt_mac_no_ampersands(it->text().lower());
    int st = t.findRev('\t');
    if(st != -1)
	t.remove(st, t.length()-st);
    t.replace(QRegExp(QString::fromLatin1("\\.*$")), ""); //no ellipses
    //now the fun part
    uint ret = 0;
    if(t.startsWith(tr("About").lower(), FALSE) || t.endsWith(tr("About").lower(), FALSE)) {
	if(t.find(QRegExp(QString::fromLatin1("qt$"), FALSE)) == -1)
	    ret = kHICommandAbout;
	else
	    ret = 'CUTE';
    } else if(t.find(tr("Config").lower(), 0, FALSE) == 0 || t.find(tr("Preference").lower(), 0, FALSE) == 0 ||
	      t.find(tr("Options").lower(), 0, FALSE) == 0 || t.find(tr("Setting").lower(), 0, FALSE) == 0 ||
	      t.find(tr("Setup").lower(), 0, FALSE) == 0 ) {
	ret = kHICommandPreferences;
    } else if(t.find(tr("Quit").lower(), 0, FALSE) == 0 || t.find(tr("Exit").lower(), 0, FALSE) == 0) {
	ret = kHICommandQuit;
    }
    //shall we?
    if(just_check) {
	//do nothing, we already checked
    } else if(ret && activeMenuBar &&
	      (!activeMenuBar->mac_d->commands || !activeMenuBar->mac_d->findCommandBinding(ret))) {
	if(ret == kHICommandAbout || ret == 'CUTE') {
	    if(activeMenuBar->mac_d->apple_menu) {
		QString text = qt_mac_no_ampersands(it->text());
		int st = text.findRev('\t');
		if(st != -1)
		    text.remove(st, text.length()-st);
		text.replace(QRegExp(QString::fromLatin1("\\.*$")), ""); //no ellipses
#ifdef Q_WS_MACX
		if(ret == kHICommandAbout && text.lower() == tr("About").lower())
		    text += " " + QString(qAppName());
#endif
		CFStringRef cfref;
		qt_mac_no_ampersands(text, &cfref);
		InsertMenuItemTextWithCFString(activeMenuBar->mac_d->apple_menu,
					       cfref, activeMenuBar->mac_d->in_apple++,
					       kMenuItemAttrAutoRepeat, ret);
		CFRelease(cfref);
		if(qMacVersion() >= MV_PANTHER) { //insert a separator
		    static int sep_id = 'SEP0';
		    InsertMenuItemTextWithCFString(activeMenuBar->mac_d->apple_menu,
						   0, activeMenuBar->mac_d->in_apple++,
						   kMenuItemAttrSeparator, sep_id++);
		}
	    }
	}
	qt_mac_command_set_enabled(ret, it->isEnabled());
    } else {
	ret = 0;
    }
    return ret;
}
#endif

bool QMenuBar::syncPopups(MenuRef ret, QPopupMenu *d)
{
    int added = 0;
    if(d) {
	if(!d->isEnabled())
	    DisableMenuItem(ret, 0);
	ChangeMenuAttributes(ret, !d->isCheckable() ? kMenuAttrExcludesMarkColumn : 0,
			     d->isCheckable() ? kMenuAttrExcludesMarkColumn : 0);
	int id = 1, index = 0;
	for(QMenuItemListIt it(*d->mitems); it.current(); ++it, ++index) {
#if !defined(QMAC_QMENUBAR_NO_MERGE)
	    if(activeMenuBar->mac_d->commands) {
		bool found = FALSE;
		for(QIntDictIterator<QMenuBar::MacPrivate::CommandBinding> cmd_it(*(activeMenuBar->mac_d->commands));
		    cmd_it.current() && !found; ++cmd_it)
		    found = (cmd_it.current()->index == index && d == cmd_it.current()->qpopup);
		if(found)
		    continue;
	    }
#endif

	    QMenuItem *item = (*it);
#if defined(QMAC_QMENUBAR_NO_EVENT)
	    if(item->custom())
		continue;
#endif
	    if(item->widget())
		continue;
	    if(!item->isVisible())
		continue;

	    QString text = "empty", accel; //Yes I need this, stupid!
	    if(!item->isSeparator()) {
#if !defined(QMAC_QMENUBAR_NO_MERGE)
		if(int cmd = isCommand(item)) {
		    if(!activeMenuBar->mac_d->commands) {
			activeMenuBar->mac_d->commands = new QIntDict<QMenuBar::MacPrivate::CommandBinding>();
			activeMenuBar->mac_d->commands->setAutoDelete(TRUE);
		    }
		    activeMenuBar->mac_d->commands->insert(cmd,
							   new QMenuBar::MacPrivate::CommandBinding(d, index));
		    continue;
		}
#endif
		text = item->text();
		int st = text.findRev('\t');
		if(st != -1) {
		    accel = text.right(text.length()-(st+1));
		    text.remove(st, text.length()-st);
		}
	    }
#if !defined(QMAC_QMENUBAR_NO_MERGE)
	    else if(!it.atLast()) {
		QMenuItemListIt it2 = it;
		++it2;
		if(isCommand((*it2), TRUE)) {
		    if(it2.atLast())
			continue;
		    ++it2;
		    if((*it2)->isSeparator())
			continue;
		}
	    }
#endif

	    MenuItemAttributes attr = kMenuItemAttrAutoRepeat;
#if !defined(QMAC_QMENUBAR_NO_EVENT)
	    if(item->custom())
		attr |= kMenuItemAttrCustomDraw;
#endif

	    //figure out an accelerator
	    int accel_key = Qt::Key_unknown;
	    if(accel.isEmpty() && item->key().count() > 1)
		text += " (****)"; //just to denote a multi stroke accelerator
	    if(item->key().count() == 1 && (int)item->key() != accel_key)
		accel_key = (int)item->key();
	    else if(!accel.isEmpty())
		accel_key = QAccel::stringToKey(accel);

	    {
		CFStringRef cfref;
		qt_mac_no_ampersands(text, &cfref);
		InsertMenuItemTextWithCFString(ret, cfref, id,  attr, item->id());
		CFRelease(cfref);
	    }
	    if(item->isSeparator()) {
		ChangeMenuItemAttributes(ret, id, kMenuItemAttrSeparator, 0);
	    } else {
		if(!qt_mac_no_menubar_icons) {
		    if(item->pixmap()) { 		    //handle pixmaps..
			IconRef ico = qt_mac_create_iconref(*item->pixmap());
			SetMenuItemIconHandle(ret, id, kMenuIconRefType, (Handle)ico);
		    } else if(item->iconSet()) {
			IconRef ico = qt_mac_create_iconref(item->iconSet()->pixmap(QIconSet::Small, QIconSet::Normal));
			SetMenuItemIconHandle(ret, id, kMenuIconRefType, (Handle)ico);
		    }
		}
                if(mac_d->modal || !item->isEnabled() || (item->popup() && !item->popup()->isEnabled()))
		    DisableMenuItem(ret, id);
                else
                    EnableMenuItem(ret, id);
		CheckMenuItem(ret, id, item->isChecked() ? true : false);
		if(item->popup()) {
		    SetMenuItemHierarchicalMenu(ret, id, createMacPopup(item->popup(), item->id()));
		} else if(accel_key != Qt::Key_unknown) {
		    char mod = 0;
		    if((accel_key & Qt::CTRL) != Qt::CTRL)
			mod |= kMenuNoCommandModifier;
		    if((accel_key & Qt::META) == Qt::META)
			mod |= kMenuControlModifier;
		    if((accel_key & Qt::ALT) == Qt::ALT)
			mod |= kMenuOptionModifier;
		    if((accel_key & Qt::SHIFT) == Qt::SHIFT)
			mod |= kMenuShiftModifier;
		    int keycode = (accel_key & ~(Qt::MODIFIER_MASK | Qt::UNICODE_ACCEL));
		    if(keycode) {
			SetMenuItemModifiers(ret, id, mod);
			bool do_glyph = TRUE;
			if(keycode == Qt::Key_Return)
			    keycode = kMenuReturnGlyph;
			else if(keycode == Qt::Key_Enter)
			    keycode = kMenuEnterGlyph;
			else if(keycode == Qt::Key_Tab)
			    keycode = kMenuTabRightGlyph;
			else if(keycode == Qt::Key_Backspace)
			    keycode = kMenuDeleteLeftGlyph;
			else if(keycode == Qt::Key_Delete)
			    keycode = kMenuDeleteRightGlyph;
			else if(keycode == Qt::Key_Escape)
			    keycode = kMenuEscapeGlyph;
			else if(keycode == Qt::Key_Home)
			    keycode = kMenuNorthwestArrowGlyph;
			else if(keycode == Qt::Key_End)
			    keycode = kMenuSoutheastArrowGlyph;
			else if(keycode == Qt::Key_PageUp)
			    keycode = kMenuPageUpGlyph;
			else if(keycode == Qt::Key_PageDown)
			    keycode = kMenuPageDownGlyph;
			else if(keycode == Qt::Key_Up)
			    keycode = kMenuUpArrowGlyph;
			else if(keycode == Qt::Key_Down)
			    keycode = kMenuDownArrowGlyph;
			else if(keycode == Qt::Key_Left)
			    keycode = kMenuLeftArrowGlyph;
			else if(keycode == Qt::Key_Right)
			    keycode = kMenuRightArrowGlyph;
			else if(keycode == Qt::Key_CapsLock)
			    keycode = kMenuCapsLockGlyph;
			else if(keycode >= Qt::Key_F1 && keycode <= Qt::Key_F15)
			    keycode = (keycode - Qt::Key_F1) + kMenuF1Glyph;
			else {
			    do_glyph = FALSE;
			    if(keycode < 127) { //regular ascii accel
				SetItemCmd(ret, id, (CharParameter)keycode );
			    } else { //I guess I missed one, fix above if this happens
				QKeySequence key(keycode);
				qDebug("QMenuBar: Not sure how to handle accelerator 0x%04x (%s)",
				       keycode, ((QString)key).latin1());
			    }
			}
			if(do_glyph) //"special" accelerator..
			    SetMenuItemKeyGlyph(ret, id, (SInt16)keycode);
		    }
		}
	    }
	    id++;
	    added++;
 	}
    }
    return added != 0;
}

MenuRef QMenuBar::createMacPopup(QPopupMenu *d, int id, bool top_level)
{
    static int mid = 0;
    MenuAttributes attr = 0;
#if 0
    attr |= kMenuAttrAutoDisable;
#endif
#if !defined(QMAC_QMENUBAR_NO_EVENT)
    attr |= kMenuItemAttrCustomDraw;
#endif
    MenuRef ret;
    if(CreateNewMenu(0, attr, &ret) != noErr)
	return 0;
    if(d && !syncPopups(ret, d) && d->count() && !d->receivers(SIGNAL(aboutToShow()))) {
	ReleaseMenu(ret);
	ret = 0;
    } else {
	if(!activeMenuBar->mac_d->popups) {
	    activeMenuBar->mac_d->popups = new QIntDict<QMenuBar::MacPrivate::PopupBinding>();
	    activeMenuBar->mac_d->popups->setAutoDelete(TRUE);
	}
	SetMenuID(ret, ++mid);
#if !defined(QMAC_QMENUBAR_NO_EVENT)
	qt_mac_install_menubar_event(ret);
#endif
	activeMenuBar->mac_d->popups->insert((int)mid,
					     new QMenuBar::MacPrivate::PopupBinding(d, ret, id, top_level));
    }
    return ret;
}

bool QMenuBar::updateMenuBar()
{
    if(this != activeMenuBar)
	qDebug("Shouldn't have happened! %s:%d", __FILE__, __LINE__);
    qt_mac_clear_menubar();
    if(mac_d)
	mac_d->clear();
    if(!CreateNewMenu(0, 0, &mac_d->apple_menu)) {
	CFStringRef cfref;
	qt_mac_no_ampersands(QString(QChar(0x14)), &cfref);
	SetMenuTitleWithCFString(mac_d->apple_menu, cfref);
	CFRelease(cfref);
	InsertMenu(mac_d->apple_menu, 0);
    }

    for(QMenuItemListIt it(*mitems); it.current(); ++it) {
	QMenuItem *item = (*it);
	if (item->isSeparator() || !item->isVisible()) //mac doesn't support these
	    continue;
	if(MenuRef mp = createMacPopup(item->popup(), item->id(), TRUE)) {
	    CFStringRef cfref;
	    qt_mac_no_ampersands(item->text(), &cfref);
	    SetMenuTitleWithCFString(mp, cfref);
	    CFRelease(cfref);
	    InsertMenu(mp, 0);
	    if(item->isEnabled())
		EnableMenuItem(mp, 0);
	    else
		DisableMenuItem(mp, 0);
	}
    }
    return TRUE;
}

/* qmenubar functions */

/*!
    \internal
*/
bool QMenuBar::activateCommand(uint cmd)
{
    HiliteMenu(0);
#if !defined(QMAC_QMENUBAR_NO_MERGE)
    if(activeMenuBar && activeMenuBar->mac_d->commands) {
	if(MacPrivate::CommandBinding *mcb = activeMenuBar->mac_d->findCommandBinding(cmd)) {
#ifdef DEBUG_MENUBAR_ACTIVATE
	    qDebug("ActivateCommand: activating internal merging '%s'",
		   mcb->qpopup->text(mcb->qpopup->idAt(mcb->index)).latin1());
#endif
	    mcb->qpopup->activateItemAt(mcb->index);
	    return TRUE;
	}
    }
#endif
    return FALSE;
}

/*!
    \internal
*/
bool QMenuBar::activate(MenuRef menu, short idx, bool highlight, bool by_accel)
{
    if(!activeMenuBar)
	return FALSE;

    int mid = GetMenuID(menu);
    if(MacPrivate::PopupBinding *mpb = activeMenuBar->mac_d->findPopupBinding(mid)) {
	MenuCommand cmd;
	GetMenuItemCommandID(mpb->macpopup, idx, &cmd);
	if(by_accel) {
	    int key = mpb->qpopup->findItem(cmd)->key();
	    if(key == Qt::Key_unknown) {
#ifdef DEBUG_MENUBAR_ACTIVATE
		qDebug("ActivateMenuitem: ignored due to fake accelerator '%s' %d",
		       mpb->qpopup->text(cmd).latin1(), highlight);
#endif
		return FALSE;
	    }
	}
#ifdef DEBUG_MENUBAR_ACTIVATE
	qDebug("ActivateMenuitem: activating internal menubar binding '%s' %d",
	       mpb->qpopup->text(cmd).latin1(), highlight);
#endif
	if(highlight) {
	    if(mpb->qpopup->isItemEnabled(cmd))
		mpb->qpopup->hilitSig(cmd);
	} else {
	    mpb->qpopup->activateItemAt(mpb->qpopup->indexOf(cmd));
	    HiliteMenu(0);
	}
	return TRUE;
    }
    if(!highlight)
	HiliteMenu(0);
    return FALSE;
}

static bool qt_mac_no_native_menubar = FALSE;
void qt_mac_set_no_native_menubar(bool b) { qt_mac_no_native_menubar = b; } //backdoor to disable menubars
static QPtrDict<QMenuBar> *menubars = 0;
/*!
  \internal
  Internal function that cleans up the menubar.
*/
void QMenuBar::macCreateNativeMenubar()
{
    macDirtyNativeMenubar();
    QWidget *p = parentWidget();
    mac_eaten_menubar = FALSE;
    if(qt_mac_no_native_menubar) {
	//do nothing..
    } else if(!p && !fallbackMenuBar) {
	fallbackMenuBar = this;
	mac_eaten_menubar = TRUE;
	if(!mac_d)
	    mac_d = new MacPrivate;
    } else if(p && (!menubars || !menubars->find(topLevelWidget())) &&
              (((p->isDialog() || ::qt_cast<QMainWindow *>(p)) && p->isTopLevel())
               || ::qt_cast<QToolBar *>(p) || topLevelWidget() == qApp->mainWidget()
               || !qApp->mainWidget())) {
        mac_eaten_menubar = TRUE;
        if(!menubars)
            menubars = new QPtrDict<QMenuBar>();
        menubars->insert(topLevelWidget(), this);
        if(!mac_d)
            mac_d = new MacPrivate;
    }
}

/*!  \internal */
void QMenuBar::macRemoveNativeMenubar()
{
    if(mac_eaten_menubar && menubars) {
	for(QPtrDictIterator<QMenuBar> it(*menubars); it.current(); ++it) {
	    if(it.current() == this) {
		menubars->remove(it.currentKey());
		it.toFirst();
	    }
	}
    }
    mac_eaten_menubar = FALSE;
    if(this == activeMenuBar) {
	activeMenuBar = 0;
	qt_mac_clear_menubar();
    }
    if(mac_d) {
	delete mac_d;
	mac_d = 0;
    }
}

/*!  \internal */
void QMenuBar::macDirtyNativeMenubar()
{
    if(mac_eaten_menubar && mac_d) {
	mac_d->dirty = 1;
	qt_event_request_menubarupdate();
    }
}

/*!  \internal */
void QMenuBar::initialize()
{
}

/*!
    \internal
*/
void QMenuBar::cleanup()
{
    delete menubars;
    menubars = 0;
}

/*!
    \internal
*/
bool QMenuBar::macUpdateMenuBar()
{
    if(qt_mac_no_native_menubar) //nothing to be done..
	return TRUE;

    QMenuBar *mb = NULL;
    bool fall_back_to_empty = FALSE;
    //find a menubar
    if(menubars) {
	QWidget *w = qApp->activeWindow();
	if(!w) {
	    WindowClass c;
	    for(WindowPtr wp = FrontWindow(); wp; wp = GetNextWindow(wp)) {
		if(GetWindowClass(wp, &c))
		    break;
		if(c == kOverlayWindowClass)
		    continue;
		w = QWidget::find((WId)wp);
		break;
	    }
	}
	if(!w) //last ditch effort
	    w = qApp->mainWidget();
	if(w && menubars) {
	    mb = menubars->find(w);
	    if(!mb && (!w->parentWidget() || w->parentWidget()->isDesktop())
               && ::qt_cast<QDockWindow *>(w)) {
		if(QWidget *area = ((QDockWindow*)w)->area()) {
		    QWidget *areaTL = area->topLevelWidget();
		    if((mb = menubars->find(areaTL)))
			w = areaTL;
		}
	    }
	    while(w && /*!w->testWFlags(WShowModal) &&*/ !mb)
		mb = menubars->find((w = w->parentWidget()));
	}
	if(!w || (!w->testWFlags(WStyle_Tool) && !w->testWFlags(WType_Popup)))
	    fall_back_to_empty = TRUE;
    }
    if(!mb)
	mb = fallbackMenuBar;
    //now set it
    static bool first = TRUE;
    if(mb) {
	if(!mb->mac_eaten_menubar || (!first && !mb->mac_d->dirty && (mb == activeMenuBar))) {
	    if(mb->mac_d->modal != qt_modal_state()) {
		bool qms = qt_modal_state();
		if(!qms || (menubars && menubars->find(qApp->activeModalWidget()) != mb))
		    qt_mac_set_modal_state(mb->mac_d->modal = qms, mb);
	    }
	    return mb->mac_eaten_menubar;
	}
	first = FALSE;
	activeMenuBar = mb;
	if(mb->mac_d->dirty || !mb->mac_d->mac_menubar) {
	    mb->mac_d->dirty = 0;
	    mb->updateMenuBar();
	    mb->mac_d->mac_menubar = GetMenuBar();
	} else {
	    SetMenuBar(mb->mac_d->mac_menubar);
	    if(mb->mac_d->commands) {
		for(QIntDictIterator<QMenuBar::MacPrivate::CommandBinding> it(*(mb->mac_d->commands)); it.current(); ++it)
		    qt_mac_command_set_enabled(it.currentKey(), TRUE);
	    }
	    InvalMenuBar();
	}
	if(mb->mac_d->modal != qt_modal_state()) {
	    bool qms = qt_modal_state();
	    if(!qms || (menubars && menubars->find(qApp->activeModalWidget()) != mb))
		qt_mac_set_modal_state(mb->mac_d->modal = qms, mb);
	}
	return TRUE;
    } else if(first || fall_back_to_empty) {
	first = FALSE;
	activeMenuBar = NULL;
	qt_mac_clear_menubar();
    }
    return FALSE;
}

/*!
    \internal
*/
bool QMenuBar::macUpdatePopup(MenuRef mr)
{
    if(!mr || !activeMenuBar)
	return FALSE;

    int mid = GetMenuID(mr);
    if (MacPrivate::PopupBinding *mpb = activeMenuBar->mac_d->findPopupBinding(mid)) {
	if (mpb->qpopup) {
            emit mpb->qpopup->aboutToShow();
	    if (1 || mpb->qpopup->mac_dirty_popup) {
		mpb->qpopup->mac_dirty_popup = 0;
		DeleteMenuItems(mr, 1, CountMenuItems(mr));
		activeMenuBar->syncPopups(mr, mpb->qpopup);
		return TRUE;
	    }
	} else {
	    activeMenuBar->activateItemAt(activeMenuBar->indexOf(mpb->id));
	    HiliteMenu(0);
	}
    } else if (mr == activeMenuBar->mac_d->apple_menu && activeMenuBar->mac_d->commands) {
	QIntDictIterator<MacPrivate::CommandBinding> it(*activeMenuBar->mac_d->commands);
	for (; it.current(); ++it) {
	    MacPrivate::CommandBinding *cb = it.current();
            if(!activeMenuBar->mac_d->modal)
                qt_mac_command_set_enabled(it.currentKey(),
                                           cb->qpopup->isItemEnabled(cb->qpopup->idAt(cb->index)));
	}
    }
    return FALSE;
}

/*!
    \internal
*/
bool QMenuBar::macUpdatePopupVisible(MenuRef mr, bool vis)
{
    Q_UNUSED(vis);
    if(!mr || !activeMenuBar || !qApp)
	return FALSE;

    if(MacPrivate::PopupBinding *mpb = activeMenuBar->mac_d->findPopupBinding(GetMenuID(mr))) {
        if (!vis)
            emit mpb->qpopup->aboutToHide();
        return TRUE;
    }
    return FALSE;
}

/*!
    \internal
*/
void QMenuBar::macWidgetChangedWindow()
{
    int was_eaten = mac_eaten_menubar;
    macRemoveNativeMenubar();
    macCreateNativeMenubar();
    if(was_eaten)
	menuContentsChanged();
}

#endif //!defined(QMAC_QMENUBAR_NO_NATIVE)
#endif //WS_MAC
