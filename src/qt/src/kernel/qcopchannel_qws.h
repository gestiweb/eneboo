/****************************************************************************
**
** QCOP protocol classes
**
** Created : 20000616
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

#ifndef QCOPCHANNEL_QWS_H
#define QCOPCHANNEL_QWS_H

#ifndef QT_H
#include "qobject.h"
#endif // QT_H

#ifndef QT_NO_COP

class QWSClient;
class QCopChannelPrivate;

class QCopChannel : public QObject
{
    Q_OBJECT
public:
    QCopChannel( const QCString& channel, QObject* parent=0, const char* name=0 );
    virtual ~QCopChannel();

    QCString channel() const;

    static bool isRegistered( const QCString& channel );
    static bool send( const QCString &channel, const QCString &msg );
    static bool send( const QCString &channel, const QCString &msg,
		      const QByteArray &data );

    static void sendLocally(  const QCString &ch, const QCString &msg,
			       const QByteArray &data );

    virtual void receive( const QCString &msg, const QByteArray &data );

signals:
    void received( const QCString &msg, const QByteArray &data );

private:
    // server side
    static void registerChannel( const QString &ch, const QWSClient *cl );
    static void detach( const QWSClient *cl );
    static void answer( QWSClient *cl, const QCString &ch,
			const QCString &msg, const QByteArray &data );
    // client side
    QCopChannelPrivate* d;

    friend class QWSServer;
    friend class QApplication;
};

#endif

#endif // QCOPCHANNEL_QWS_H
