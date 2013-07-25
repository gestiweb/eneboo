/****************************************************************************
**
** Implementation of Qt/Embedded events
**
** Created : 000101
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "qwsevent_qws.h"

QWSEvent *QWSEvent::factory( int type )
{
    QWSEvent *event = 0;
    switch ( type ) {
    case QWSEvent::Connected:
	event = new QWSConnectedEvent;
	break;
    case QWSEvent::MaxWindowRect:
	event = new QWSMaxWindowRectEvent;
	break;
    case QWSEvent::Mouse:
	event = new QWSMouseEvent;
	break;
    case QWSEvent::Focus:
	event = new QWSFocusEvent;
	break;
    case QWSEvent::Key:
	event = new QWSKeyEvent;
	break;
    case QWSEvent::RegionModified:
	event = new QWSRegionModifiedEvent;
	break;
    case QWSEvent::Creation:
	event = new QWSCreationEvent;
	break;
#ifndef QT_NO_QWS_PROPERTIES
    case QWSEvent::PropertyNotify:
	event = new QWSPropertyNotifyEvent;
	break;
    case QWSEvent::PropertyReply:
	event = new QWSPropertyReplyEvent;
	break;
#endif // QT_NO_QWS_PROPERTIES	
    case QWSEvent::SelectionClear:
	event = new QWSSelectionClearEvent;
	break;
    case QWSEvent::SelectionRequest:
	event = new QWSSelectionRequestEvent;
	break;
    case QWSEvent::SelectionNotify:
	event = new QWSSelectionNotifyEvent;
	break;
#ifndef QT_NO_COP
    case QWSEvent::QCopMessage:
	event = new QWSQCopMessageEvent;
	break;
#endif
    case QWSEvent::WindowOperation:
	event = new QWSWindowOperationEvent;
	break;

#ifndef QT_NO_QWS_IM
    case QWSEvent::IMEvent:
	event = new QWSIMEvent;
	break;
#endif
    default:
	qDebug( "QWSDisplayData::readMore() : Protocol error - got %08x!", type );
    }
    return event;
}

