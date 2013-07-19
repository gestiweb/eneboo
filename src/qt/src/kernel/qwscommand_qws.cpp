/****************************************************************************
**
** Implementation of Qt/FB central server
**
** Created : 991025
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

#include "qwscommand_qws.h"

/*********************************************************************
 *
 * Functions to read/write commands on/from a socket
 *
 *********************************************************************/
#ifndef QT_NO_QWS_MULTIPROCESS
void qws_write_command( QWSSocket *socket, int type,
			       char *simpleData, int simpleLen, char *rawData, int rawLen )
{
    qws_write_uint( socket, type );
    qws_write_uint( socket, rawLen == -1 ? 0 : rawLen );
    if ( simpleData && simpleLen )
	socket->writeBlock( simpleData, simpleLen );
    if ( rawLen && rawData )
	socket->writeBlock( rawData, rawLen );
}

bool qws_read_command( QWSSocket *socket, char *&simpleData, int &simpleLen,
			      char *&rawData, int &rawLen,
			      int &bytesRead )
{
    if ( rawLen == -1 ) {
	if ( socket->size() < sizeof( rawLen ) )
	    return FALSE;
	rawLen = qws_read_uint( socket );
    }

    if ( !bytesRead ) {
	if ( simpleLen ) {
	    if ( socket->size() < uint(simpleLen) )
		return FALSE;
	    bytesRead = socket->readBlock( simpleData, simpleLen );
	} else
	    bytesRead = 1; // hack!
    }

    if ( bytesRead ) {
	if ( !rawLen )
	    return TRUE;
	if ( socket->size() < uint(rawLen) )
	    return FALSE;
	rawData = new char[ rawLen ];
	bytesRead += socket->readBlock( rawData, rawLen );
	return TRUE;
    }
    return FALSE;
}
#endif
/*********************************************************************
 *
 * QWSCommand base class - only use derived classes from that
 *
 *********************************************************************/


QWSProtocolItem::~QWSProtocolItem() { if (deleteRaw) delete [] rawDataPtr; }

#ifndef QT_NO_QWS_MULTIPROCESS
void QWSProtocolItem::write( QWSSocket *s ) {
    //qDebug( "sending type %d", type );
    qws_write_command( s, type, simpleDataPtr, simpleLen, rawDataPtr, rawLen );
}

bool QWSProtocolItem::read( QWSSocket *s ) {
    bool b = qws_read_command( s, simpleDataPtr, simpleLen,
			       rawDataPtr, rawLen, bytesRead );
    if ( b ) {
	setData(rawDataPtr, rawLen, FALSE);
	deleteRaw = TRUE;
    }
    return b;
}
#endif
void QWSProtocolItem::copyFrom( const QWSProtocolItem *item ) {
    if ( this == item )
	return;
    simpleLen = item->simpleLen;
    memcpy( simpleDataPtr, item->simpleDataPtr, simpleLen );
    setData( item->rawDataPtr, item->rawLen );
}

void QWSProtocolItem::setData( char *data, int len, bool allocateMem ) {
    if ( !data && !len ) {
	rawDataPtr = 0;
	rawLen = 0;
	return;
    }
    if ( len < 0 )
	return;
    if ( deleteRaw )
	delete [] rawDataPtr;
    if ( allocateMem ) {
	rawDataPtr = new char[ len ];
	if (data)
	    memcpy( rawDataPtr, data, len );
	deleteRaw = TRUE;
    } else {
	rawDataPtr = data;
	deleteRaw = FALSE;
    }
    rawLen = len;
}

QWSCommand *QWSCommand::factory( int type )
{
    QWSCommand *command = 0;
    switch ( type ) {
    case QWSCommand::Create:
	command = new QWSCreateCommand;
	break;
    case QWSCommand::Region:
	command = new QWSRegionCommand;
	break;
    case QWSCommand::RegionMove:
	command = new QWSRegionMoveCommand;
	break;
    case QWSCommand::RegionDestroy:
	command = new QWSRegionDestroyCommand;
	break;
    case QWSCommand::AddProperty:
	command = new QWSAddPropertyCommand;
	break;
    case QWSCommand::SetProperty:
	command = new QWSSetPropertyCommand;
	break;
    case QWSCommand::RemoveProperty:
	command = new QWSRemovePropertyCommand;
	break;
    case QWSCommand::GetProperty:
	command = new QWSGetPropertyCommand;
	break;
    case QWSCommand::SetSelectionOwner:
	command = new QWSSetSelectionOwnerCommand;
	break;
    case QWSCommand::RequestFocus:
	command = new QWSRequestFocusCommand;
	break;
    case QWSCommand::ChangeAltitude:
	command = new QWSChangeAltitudeCommand;
	break;
    case QWSCommand::DefineCursor:
	command = new QWSDefineCursorCommand;
	break;
    case QWSCommand::SelectCursor:
	command = new QWSSelectCursorCommand;
	break;
    case QWSCommand::GrabMouse:
	command = new QWSGrabMouseCommand;
	break;
    case QWSCommand::GrabKeyboard:
	command = new QWSGrabKeyboardCommand;
	break;
#ifndef QT_NO_SOUND
    case QWSCommand::PlaySound:
	command = new QWSPlaySoundCommand;
	break;
#endif
#ifndef QT_NO_COP
    case QWSCommand::QCopRegisterChannel:
	command = new QWSQCopRegisterChannelCommand;
	break;
    case QWSCommand::QCopSend:
	command = new QWSQCopSendCommand;
	break;
#endif
    case QWSCommand::RegionName:
	command = new QWSRegionNameCommand;
	break;
    case QWSCommand::Identify:
	command = new QWSIdentifyCommand;
	break;
    case QWSCommand::RepaintRegion:
	command = new QWSRepaintRegionCommand;
	break;
#ifndef QT_NO_QWS_IM
    case QWSCommand::SetMicroFocus:
	command = new QWSSetMicroFocusCommand;
	break;
    case QWSCommand::ResetIM:
	command = new QWSResetIMCommand;
	break;
#endif
    default:
	qDebug( "QWSCommand::factory : Type error - got %08x!", type );
    }
    return command;
}
