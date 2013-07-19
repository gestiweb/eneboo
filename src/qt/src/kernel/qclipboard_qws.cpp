/****************************************************************************
**
** Implementation of QClipboard class for FB
**
** Created : 991026
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

#include "qclipboard.h"

#ifndef QT_NO_CLIPBOARD

#include "qapplication.h"
#include "qbitmap.h"
#include "qdatetime.h"
#include "qdragobject.h"
#include "qbuffer.h"

#include <qwsdisplay_qws.h>
#include <qwsproperty_qws.h>
#include <qwsevent_qws.h>


/*****************************************************************************
  Internal QClipboard functions for Qt/Embedded
 *****************************************************************************/

static const int TextClipboard=424242;
static bool init=FALSE;


static inline void qwsInitClipboard()
{
    //### this should go into QWSServer; it only needs to happen once.
    if ( !init ) {
	QPaintDevice::qwsDisplay()->addProperty( 0, TextClipboard );
	init = TRUE;
    }
}

#ifdef QT_NO_MIMECLIPBOARD
static QString qwsClipboardText()
{
    char * data;
    int len;
    qwsInitClipboard();
    QPaintDevice::qwsDisplay()->getProperty( 0, TextClipboard, data, len );
    //    qDebug( "Property received: %d bytes", len );

    QString s( (const QChar*)data, len/2 );
    //    qDebug( "Property received: '%s'", s.latin1());
    delete[] data;
    return s;
}


static void qwsSetClipboardText( const QString& s )
{
    qwsInitClipboard();
    QByteArray ba;
    int len =  s.length()*sizeof(QChar);
    ba.duplicate( (const char*)s.unicode(), len );
    QPaintDevice::qwsDisplay()->
	setProperty( 0, TextClipboard, QWSPropertyManager::PropReplace, ba );

}
#endif


static QWidget * owner = 0;

static void cleanup()
{
    delete owner;
    owner = 0;
}

static
void setupOwner()
{
    if ( owner )
	return;
    owner = new QWidget( 0, "internal clibpoard owner" );
    qAddPostRoutine( cleanup );
}


class QClipboardData
{
public:
    QClipboardData();
   ~QClipboardData();

    void setSource(QMimeSource* s)
	{ clear(); src = s; }
    QMimeSource* source()
	{ return src; }
    void addTransferredPixmap(QPixmap pm)
	{ /* TODO: queue them */
	    transferred[tindex] = pm;
	    tindex=(tindex+1)%2;
	}
    void clearTransfers()
	{
	    transferred[0] = QPixmap();
	    transferred[1] = QPixmap();
	}

    void clear(bool destruct=TRUE);

private:
    QMimeSource* src;

    QPixmap transferred[2];
    int tindex;
};

QClipboardData::QClipboardData()
{
    src = 0;
    tindex=0;
}

QClipboardData::~QClipboardData()
{
    delete src;
}

void QClipboardData::clear(bool destruct)
{
    if(destruct)
	delete src;
    src = 0;
}


static QClipboardData *internalCbData = 0;

static void cleanupClipboardData()
{
    delete internalCbData;
    internalCbData = 0;
}

static QClipboardData *clipboardData()
{
    if ( internalCbData == 0 ) {
	internalCbData = new QClipboardData;
	Q_CHECK_PTR( internalCbData );
	qAddPostRoutine( cleanupClipboardData );
    }
    return internalCbData;
}

void qt_clipboard_cleanup_mime_source(QMimeSource *src)
{
    if(internalCbData && internalCbData->source() == src)
	internalCbData->clear(FALSE);
}

/*****************************************************************************
  QClipboard member functions for FB.
 *****************************************************************************/

#ifdef QT_NO_MIMECLIPBOARD

QString QClipboard::text() const
{
    return qwsClipboardText();
}

void QClipboard::setText( const QString &text )
{
    qwsSetClipboardText( text );
}

QString QClipboard::text(QCString& subtype) const
{
    QString r;
    if ( subtype == "plain" )
	r = text();
    return r;
}

#endif

void QClipboard::clear( Mode mode )
{
    setText( QString::null, mode );
}


void QClipboard::ownerDestroyed()
{
}


void QClipboard::connectNotify( const char * )
{
}


bool QClipboard::event( QEvent *e )
{
    if ( e->type() != QEvent::Clipboard )
	return QObject::event( e );

    QWSPropertyNotifyEvent *event = (QWSPropertyNotifyEvent *)(((QCustomEvent *)e)->data());
    if ( event && event->simpleData.state == QWSPropertyNotifyEvent::PropertyNewValue ) {
	emit dataChanged();
    }

    return TRUE;
}

#ifndef QT_NO_MIMECLIPBOARD
QMimeSource* QClipboard::data( Mode mode ) const
{
    if ( mode != Clipboard ) return 0;

    QClipboardData *d = clipboardData();
    return d->source();
}

void QClipboard::setData( QMimeSource* src, Mode mode )
{
    if ( mode != Clipboard ) return;

    QClipboardData *d = clipboardData();
    setupOwner();

    d->setSource( src );
    emit dataChanged();
}
#endif

bool QClipboard::supportsSelection() const
{
    return FALSE;
}


bool QClipboard::ownsSelection() const
{
    return FALSE;
}


bool QClipboard::ownsClipboard() const
{
    qWarning("QClipboard::ownsClipboard: UNIMPLEMENTED!");
    return FALSE;
}


void QClipboard::setSelectionMode(bool)
{
}


bool QClipboard::selectionModeEnabled() const
{
    return FALSE;
}

#endif // QT_NO_CLIPBOARD
