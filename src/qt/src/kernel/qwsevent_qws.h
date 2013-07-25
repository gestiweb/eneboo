/****************************************************************************
**
** Definition of Qt/Embedded events
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
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

#ifndef QWSEVENT_QWS_H
#define QWSEVENT_QWS_H

#ifndef QT_H
#include "qwsutils_qws.h"
#include "qwscommand_qws.h" //QWSProtocolItem lives there, for now
#endif // QT_H

struct QWSMouseEvent;

struct QWSEvent : QWSProtocolItem {

    QWSEvent( int t, int len, char *ptr ) : QWSProtocolItem(t,len,ptr) {}

    enum Type {
	NoEvent,
	Connected,
	Mouse, Focus, Key,
	RegionModified,
	Creation,
	PropertyNotify,
	PropertyReply,
	SelectionClear,
	SelectionRequest,
	SelectionNotify,
	MaxWindowRect,
	QCopMessage,
	WindowOperation,
	IMEvent,
	NEvent
    };

    QWSMouseEvent *asMouse()
	{ return type == Mouse ? (QWSMouseEvent*)this : 0; }
    int window() { return *((int*)simpleDataPtr); }
    static QWSEvent *factory( int type );
};


//All events must start with windowID

struct QWSConnectedEvent : QWSEvent {
    QWSConnectedEvent()
	: QWSEvent( QWSEvent::Connected, sizeof( simpleData ),
		(char*)&simpleData ) {}

    void setData( char *d, int len, bool allocateMem = TRUE ) {
	QWSEvent::setData( d, len, allocateMem );
	display = (char*)rawDataPtr;
    }

    struct SimpleData {
	int window;
	int len;
	int clientId;
    } simpleData;

    char *display;
};

struct QWSMaxWindowRectEvent : QWSEvent {
    QWSMaxWindowRectEvent()
	: QWSEvent( MaxWindowRect, sizeof( simpleData ), (char*)&simpleData ) { }
    struct SimpleData {
	int window;
	QRect rect;
    } simpleData;
};

struct QWSMouseEvent : QWSEvent {
    QWSMouseEvent()
	: QWSEvent( QWSEvent::Mouse, sizeof( simpleData ),
		(char*)&simpleData ) {}
    struct SimpleData {
	int window;
	int x_root, y_root, state;
	int time; // milliseconds
    } simpleData;
};

struct QWSFocusEvent : QWSEvent {
    QWSFocusEvent()
	: QWSEvent( QWSEvent::Focus, sizeof( simpleData ), (char*)&simpleData )
	{ memset((char*)&simpleData,0,sizeof(simpleData)); }
    struct SimpleData {
	int window;
	uint get_focus:1;
    } simpleData;
};

struct QWSKeyEvent: QWSEvent {
    QWSKeyEvent()
	: QWSEvent( QWSEvent::Key, sizeof( simpleData ),
	      (char*)&simpleData ) {}
    struct SimpleData {
	int window;
	ushort unicode;
	ushort keycode;
	int modifiers;
	uint is_press:1;
	uint is_auto_repeat:1;
    } simpleData;
};


struct QWSCreationEvent : QWSEvent {
    QWSCreationEvent()
	: QWSEvent( QWSEvent::Creation, sizeof( simpleData ),
	      (char*)&simpleData ) {}
    struct SimpleData {
	int objectid;
    } simpleData;	
};

#ifndef QT_NO_QWS_PROPERTIES
struct QWSPropertyNotifyEvent : QWSEvent {
    QWSPropertyNotifyEvent()
	: QWSEvent( QWSEvent::PropertyNotify, sizeof( simpleData ),
	      (char*)&simpleData ) {}
    enum State {
        PropertyNewValue,
        PropertyDeleted
    };
    struct SimpleData {
	int window;
	int property;
	int state;
    } simpleData;
};
#endif

struct QWSSelectionClearEvent : QWSEvent {
    QWSSelectionClearEvent()
	: QWSEvent( QWSEvent::SelectionClear, sizeof( simpleData ),
	      (char*)&simpleData ) {}
    struct SimpleData {
	int window;
    } simpleData;
};

struct QWSSelectionRequestEvent : QWSEvent {
    QWSSelectionRequestEvent()
	: QWSEvent( QWSEvent::SelectionRequest, sizeof( simpleData ),
	      (char*)&simpleData ) {}
    struct SimpleData {
	int window;
	int requestor; // window which wants the selection
	int property; // property on requestor into which the selection should be stored, normally QWSProperty::PropSelection
	int mimeTypes; // Value is stored in the property mimeType on the requestor window. This value may contain
	// multiple mimeTypes separated by ;; where the order reflects the priority
    } simpleData;
};

struct QWSSelectionNotifyEvent : QWSEvent {
    QWSSelectionNotifyEvent()
	: QWSEvent( QWSEvent::SelectionNotify, sizeof( simpleData ),
	      (char*)&simpleData ) {}
    struct SimpleData {
	int window;
	int requestor; // the window which wanted the selection and to which this event is sent
	int property; // property of requestor in which the data of the selection is stored
	int mimeType; // a property on the requestor in which the mime type in which the selection is, is stored
    } simpleData;
};

//complex events:

struct QWSRegionModifiedEvent : QWSEvent {
    QWSRegionModifiedEvent()
	: QWSEvent( QWSEvent::RegionModified, sizeof( simpleData ),
		(char*)&simpleData )
	{ memset((char*)&simpleData,0,sizeof(simpleData)); }

    void setData( char *d, int len, bool allocateMem = TRUE ) {
	QWSEvent::setData( d, len, allocateMem );
	rectangles = (QRect*)rawDataPtr;
    }

    struct SimpleData {
	int window;
	int nrectangles;
	uint is_ack:1;
    } simpleData;

    QRect *rectangles;
};
#ifndef QT_NO_QWS_PROPERTIES
struct QWSPropertyReplyEvent : QWSEvent {
    QWSPropertyReplyEvent()
	: QWSEvent( QWSEvent::PropertyReply, sizeof( simpleData ),
		(char*)&simpleData ) {}

    void setData( char *d, int len, bool allocateMem = TRUE ) {
	QWSEvent::setData( d, len, allocateMem );
	data = (char*)rawDataPtr;
    }

    struct SimpleData {
	int window;
	int property;
	int len;
    } simpleData;
    char *data;
};
#endif //QT_NO_QWS_PROPERTIES

#ifndef QT_NO_COP
struct QWSQCopMessageEvent : QWSEvent {
    QWSQCopMessageEvent()
	: QWSEvent( QWSEvent::QCopMessage, sizeof( simpleData ),
		(char*)&simpleData )
	{ memset((char*)&simpleData,0,sizeof(simpleData)); }

    void setData( char *d, int len, bool allocateMem = TRUE ) {
	QWSEvent::setData( d, len, allocateMem );
	char* p = (char*) rawDataPtr;
	channel = QCString( p, simpleData.lchannel + 1 );
	p += simpleData.lchannel;
	message = QCString( p, simpleData.lmessage + 1 );
	p += simpleData.lmessage;
	data.duplicate( p, simpleData.ldata );
    }

    struct SimpleData {
	bool is_response;
	int lchannel;
	int lmessage;
	int ldata;
    } simpleData;

    QCString channel;
    QCString message;
    QByteArray data;
};

#endif

struct QWSWindowOperationEvent : QWSEvent {
    QWSWindowOperationEvent()
	: QWSEvent( WindowOperation, sizeof( simpleData ), (char*)&simpleData ) { }

    enum Operation { Show, Hide, ShowMaximized, ShowNormal, ShowMinimized, Close };
    struct SimpleData {
	int window;
	Operation op;
    } simpleData;
};

#ifndef QT_NO_QWS_IM
struct QWSIMEvent : QWSEvent {
    QWSIMEvent()
	: QWSEvent( IMEvent, sizeof( simpleData ), (char*)&simpleData ) { }

    struct SimpleData {
	int window;
	int type;
	int cpos;
	int selLen;
	int textLen;
    } simpleData;

    void setData( char *d, int len, bool allocateMem = TRUE ) {
	QWSEvent::setData( d, len, allocateMem );
	text = (QChar*)rawDataPtr;
    }

    QChar *text;
};
#endif

#endif // QWSEVENT_QWS_H
