/****************************************************************************
**
** Implementation of QCOP protocol
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

#include "qcopchannel_qws.h"

#ifndef QT_NO_COP

#include "qwsdisplay_qws.h"
#include "qwscommand_qws.h"
#include "qwindowsystem_qws.h"
#include "qptrlist.h"
#include "qmap.h"

typedef QMap<QString, QPtrList<QWSClient> > QCopServerMap;
static QCopServerMap *qcopServerMap = 0;

typedef QMap<QString, QPtrList<QCopChannel> > QCopClientMap;
static QCopClientMap *qcopClientMap = 0;

class QCopChannelPrivate
{
public:
    QCString channel;
};

/*!
    \class QCopChannel qcopchannel_qws.h

    \brief The QCopChannel class provides communication capabilities
    between several clients.

    The Qt Cop (QCOP) is a COmmunication Protocol, allowing clients to
    communicate both within the same address space and between
    different processes.

    Currently, this facility is only available on Qt/Embedded. On X11
    and Windows we are exploring the use of existing standards such as
    DCOP and COM.

    QCopChannel provides send() and isRegistered() which are static
    functions usable without an object.

    The channel() function returns the name of the channel.

    In order to \e listen to the traffic on a channel, you should
    either subclass QCopChannel and reimplement receive(), or
    connect() to the received() signal.
*/

/*!
    Constructs a QCop channel and registers it with the server using
    the name \a channel. The standard \a parent and \a name arguments
    are passed on to the QObject constructor.
*/

QCopChannel::QCopChannel( const QCString& channel, QObject* parent, const char* name ) :
    QObject( parent, name )
{
    d = new QCopChannelPrivate;
    d->channel = channel;

    if ( !qt_fbdpy ) {
	qFatal( "QCopChannel: Must construct a QApplication "
		"before QCopChannel" );
	return;
    }

    if ( !qcopClientMap )
	qcopClientMap = new QCopClientMap;

    // do we need a new channel list ?
    QCopClientMap::Iterator it = qcopClientMap->find( channel );
    if ( it != qcopClientMap->end() ) {
	it.data().append( this );
	return;
    }

    it = qcopClientMap->insert( channel, QPtrList<QCopChannel>() );
    it.data().append( this );

    // inform server about this channel
    qt_fbdpy->registerChannel( channel );
}

/*!
    Destroys the client's end of the channel and notifies the server
    that the client has closed its connection. The server will keep
    the channel open until the last registered client detaches.
*/

QCopChannel::~QCopChannel()
{
    QCopClientMap::Iterator it = qcopClientMap->find( d->channel );
    Q_ASSERT( it != qcopClientMap->end() );
    it.data().removeRef( this );
    // still any clients connected locally ?
    if ( it.data().isEmpty() ) {
	QByteArray data;
	QDataStream s( data, IO_WriteOnly );
	s << d->channel;
	if ( qt_fbdpy )
	    send( "", "detach()", data );
	qcopClientMap->remove( d->channel );
    }

    delete d;
}

/*!
    Returns the name of the channel.
*/

QCString QCopChannel::channel() const
{
    return d->channel;
}

/*!
    This virtual function allows subclasses of QCopChannel to process
    data received from their channel.

    The default implementation emits the received() signal.

    Note that the format of \a data has to be well defined in order to
    extract the information it contains.

    Example:
    \code
    void MyClass::receive( const QCString &msg, const QByteArray &data )
    {
	QDataStream stream( data, IO_ReadOnly );
	if ( msg == "execute(QString,QString)" ) {
	    QString cmd, arg;
	    stream >> cmd >> arg;
	    ...
	} else if ( msg == "delete(QString)" ) {
	    QString filenname;
	    stream >> filename;
	    ...
	} else ...
    }
    \endcode
    This example assumes that the \a msg is a DCOP-style function
    signature and the \a data contains the function's arguments. (See
    send().)

    Using the DCOP convention is a recommendation, but not a
    requirement. Whatever convention you use the sender and receiver
    \e must agree on the argument types.

    \sa send()
 */
void QCopChannel::receive( const QCString &msg, const QByteArray &data )
{
    emit received( msg, data );
}

/*!
    \fn void QCopChannel::received( const QCString &msg, const QByteArray &data )

    This signal is emitted with the \a msg and \a data whenever the
    receive() function gets incoming data.
*/

/*!
    Queries the server for the existence of \a channel.

    Returns TRUE if \a channel is registered; otherwise returns FALSE.
*/

bool QCopChannel::isRegistered( const QCString& channel )
{
    QByteArray data;
    QDataStream s( data, IO_WriteOnly );
    s << channel;
    if ( !send( "", "isRegistered()", data ) )
	return FALSE;

    QWSQCopMessageEvent *e = qt_fbdpy->waitForQCopResponse();
    bool known = e->message == "known";
    delete e;
    return known;
}

/*!
    \overload
    Send the message \a msg on channel \a channel. The message will be
    distributed to all clients subscribed to the \a channel.

    \sa receive()
*/

bool QCopChannel::send(const QCString &channel, const QCString &msg )
{
    QByteArray data;
    return send( channel, msg, data );
}

/*!
    Send the message \a msg on channel \a channel with data \a data.
    The message will be distributed to all clients subscribed to the
    channel.

    Note that QDataStream provides a convenient way to fill the byte
    array with auxiliary data.

    Example:
    \code
    QByteArray ba;
    QDataStream stream( ba, IO_WriteOnly );
    stream << QString("cat") << QString("file.txt");
    QCopChannel::send( "System/Shell", "execute(QString,QString)", ba );
    \endcode
    Here the channel is "System/Shell". The \a msg is an arbitrary
    string, but in the example we've used the DCOP convention of
    passing a function signature. Such a signature is formatted as
    functionname(types) where types is a list of zero or more
    comma-separated type names, with no whitespace, no consts and no
    pointer or reference marks, i.e. no "*" or "&".

    Using the DCOP convention is a recommendation, but not a
    requirement. Whatever convention you use the sender and receiver
    \e must agree on the argument types.

    \sa receive()
*/

bool QCopChannel::send(const QCString &channel, const QCString &msg,
		       const QByteArray &data )
{
    if ( !qt_fbdpy ) {
	qFatal( "QCopChannel::send: Must construct a QApplication "
		"before using QCopChannel" );
	return FALSE;
    }

    qt_fbdpy->sendMessage( channel, msg, data );

    return TRUE;
}

class QWSServerSignalBridge : public QObject {
  Q_OBJECT

public:
  void emitNewChannel(const QString& channel);
  void emitRemovedChannel(const QString& channel);

  signals:
  void newChannel(const QString& channel);
  void removedChannel(const QString& channel);
};

void QWSServerSignalBridge::emitNewChannel(const QString& channel){
  emit newChannel(channel);
}

void QWSServerSignalBridge::emitRemovedChannel(const QString& channel) {
  emit removedChannel(channel);
}

/*!
    \internal
    Server side: subscribe client \a cl on channel \a ch.
*/

void QCopChannel::registerChannel( const QString &ch, const QWSClient *cl )
{
    if ( !qcopServerMap )
	qcopServerMap = new QCopServerMap;

    // do we need a new channel list ?
    QCopServerMap::Iterator it = qcopServerMap->find( ch );
    if ( it == qcopServerMap->end() )
      it = qcopServerMap->insert( ch, QList<QWSClient>() );
    
    // If this is the first client in the channel, announce the channel as being created.
    if (it.data().count() == 0) {
      QWSServerSignalBridge* qwsBridge = new QWSServerSignalBridge();
      connect(qwsBridge, SIGNAL(newChannel(const QString&)), qwsServer, SIGNAL(newChannel(const QString&)));
      qwsBridge->emitNewChannel(ch);
      delete qwsBridge;
    }

    it.data().append( cl );
}

/*!
    \internal
    Server side: unsubscribe \a cl from all channels.
*/

void QCopChannel::detach( const QWSClient *cl )
{
    if ( !qcopServerMap )
	return;

    QCopServerMap::Iterator it = qcopServerMap->begin();
    for ( ; it != qcopServerMap->end(); it++ ) {
      if (it.data().containsRef(cl)) {
	it.data().removeRef( cl );
	// If this was the last client in the channel, announce the channel as dead.
	if (it.data().count() == 0) {
	  QWSServerSignalBridge* qwsBridge = new QWSServerSignalBridge();
	  connect(qwsBridge, SIGNAL(removedChannel(const QString&)), qwsServer, SIGNAL(removedChannel(const QString&)));
	  qwsBridge->emitRemovedChannel(it.key());
	  delete qwsBridge;
	}
      }
    }
}

/*!
    \internal
    Server side: transmit the message to all clients registered to the
    specified channel.
*/

void QCopChannel::answer( QWSClient *cl, const QCString &ch,
			  const QCString &msg, const QByteArray &data )
{
    // internal commands
    if ( ch.isEmpty() ) {
	if ( msg == "isRegistered()" ) {
	    QCString c;
	    QDataStream s( data, IO_ReadOnly );
	    s >> c;
	    bool known = qcopServerMap && qcopServerMap->contains( c )
			&& !((*qcopServerMap)[ c ]).isEmpty();
	    QCString ans = known ? "known" : "unkown";
	    QWSServer::sendQCopEvent( cl, "", ans, data, TRUE );
	    return;
	} else if ( msg == "detach()" ) {
	    QCString c;
	    QDataStream s( data, IO_ReadOnly );
	    s >> c;
	    Q_ASSERT( qcopServerMap );
	    QCopServerMap::Iterator it = qcopServerMap->find( c );
	    if ( it != qcopServerMap->end() ) {
		Q_ASSERT( it.data().contains( cl ) );
		it.data().remove( cl );
		if ( it.data().isEmpty() ) {
		  // If this was the last client in the channel, announce the channel as dead
		  QWSServerSignalBridge* qwsBridge = new QWSServerSignalBridge();
		  connect(qwsBridge, SIGNAL(removedChannel(const QString&)), qwsServer, SIGNAL(removedChannel(const QString&)));
		  qwsBridge->emitRemovedChannel(it.key());
		  delete qwsBridge;
		  qcopServerMap->remove( it );
		}
	    }
	    return;
	}
	qWarning( "QCopChannel: unknown internal command %s", msg.data() );
	QWSServer::sendQCopEvent( cl, "", "bad", data );
	return;
    }

    QPtrList<QWSClient> clist = (*qcopServerMap)[ ch ];
    if ( clist.isEmpty() ) {
	qWarning( "QCopChannel: no client registered for channel %s", ch.data() );
	return;
    }

    QWSClient *c = clist.first();
    for (; c != 0; c = clist.next() ) {
	QWSServer::sendQCopEvent( c, ch, msg, data );
    }
}

/*!
    \internal
    Client side: distribute received event to the QCop instance managing the
    channel.
*/
void QCopChannel::sendLocally( const QCString &ch, const QCString &msg,
				const QByteArray &data )
{
    Q_ASSERT( qcopClientMap );

    // filter out internal events
    if ( ch.isEmpty() )
	return;

    // feed local clients with received data
    QPtrList<QCopChannel> clients = (*qcopClientMap)[ ch ];
    for ( QCopChannel *p = clients.first(); p != 0; p = clients.next() )
	p->receive( msg, data );
}
#include "qcopchannel_qws.moc"

#endif


