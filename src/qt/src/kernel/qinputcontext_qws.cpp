/****************************************************************************
**
** Implementation of QInputContext class
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "qinputcontext_p.h"
#include "qstring.h"
#include "qwindowsystem_qws.h"
#include "qpaintdevice.h"
#include "qwsdisplay_qws.h"


#ifndef QT_NO_QWS_IM
QWidget* QInputContext::focusWidget = 0;
QString* QInputContext::composition = 0;

void QInputContext::translateIMEvent( QWSIMEvent *e, QWidget *keywidget )
{
    //generate end event for previous widget if focus has changed
    if ( focusWidget && focusWidget != keywidget )
	reset();

    QString txt( e->text, e->simpleData.textLen );


    if ( e->simpleData.type == QWSServer::IMStart ) {
	//We may already have generated a start event for this widget
	//(could happen if the IM is buggy)
	if ( !focusWidget ) {
	    QIMEvent out( QEvent::IMStart, txt, e->simpleData.cpos );
	    QApplication::sendSpontaneousEvent( keywidget, &out );
	    focusWidget = keywidget;
	    if ( !composition )
		composition = new QString;
	}
    } else if ( e->simpleData.type == QWSServer::IMCompose ) {
	//generate start event if we haven't done so already
	//(focus change or careless IM)
	if ( !focusWidget ) {
	    QIMEvent out( QEvent::IMStart, "", -1 );
	    QApplication::sendSpontaneousEvent( keywidget, &out );
	    focusWidget = keywidget;
	    if ( !composition )
		composition = new QString;
	}

	QIMComposeEvent out( QEvent::IMCompose, txt, 
			     e->simpleData.cpos, 
			     e->simpleData.selLen );
	QApplication::sendSpontaneousEvent( keywidget, &out );

	*composition = txt;
    } else if ( e->simpleData.type == QWSServer::IMEnd ) {
	//Make sure we don't send multiple end events (guard against buggy IM)
	if ( focusWidget ) {
	    QIMEvent out( QEvent::IMEnd, txt, e->simpleData.cpos );
	    QApplication::sendSpontaneousEvent( keywidget, &out );
	    focusWidget = 0;
	    *composition = QString::null;
	}
    }
}

void QInputContext::reset()
{
    //send appropriate IMEnd event if necessary
    if ( focusWidget ) {
	QIMEvent out( QEvent::IMEnd, *composition, -1 );
	QApplication::sendSpontaneousEvent( focusWidget, &out );
	focusWidget = 0;
	*composition = QString::null;
    }
    
    QPaintDevice::qwsDisplay()->resetIM();
}
#endif //QT_NO_QWS_IM
