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

#include "qwsproperty_qws.h"

#ifndef QT_NO_QWS_PROPERTIES
#include "qwscommand_qws.h"
#include "qwindowsystem_qws.h"
#include "qintdict.h"

#include <stdlib.h>
#include <stdio.h>

class QWSPropertyManager::Data {
public:
    Data()
    {
	properties.setAutoDelete( TRUE );
    }
    struct Property {
	~Property() { if ( data ) delete [] data; }
	int len;
	char *data;
    };
    Property* find( int winId, int property )
    {
	QIntDict<Property>* wp = properties.find(winId);
	if ( !wp ) return 0;
	Property* prop = wp->find(property);
	return prop;
    }

    QIntDict< QIntDict<Property> > properties;
};

/*********************************************************************
 *
 * Class: QWSPropertyManager
 *
 *********************************************************************/

QWSPropertyManager::QWSPropertyManager()
{
    d = new Data;
}

QWSPropertyManager::~QWSPropertyManager()
{
    delete d;
}


//#define QWS_PROPERTY_DEBUG


bool QWSPropertyManager::setProperty( int winId, int property, int mode, const char *data, int len )
{
    Data::Property* prop = d->find(winId,property);
    if ( !prop ) return FALSE;

    switch ( mode ) {
    case PropReplace: {
#ifdef QWS_PROPERTY_DEBUG
	qDebug( "PropReplace" );
#endif
	delete [] prop->data;
	char *nd = new char[len]; //###Must make sure this is deleted
	memcpy(nd, data, len );
	prop->len = len;
	prop->data = nd;
    } break;
    case PropAppend: {
#ifdef QWS_PROPERTY_DEBUG
	qDebug( "PropAppend" );
#endif
	int origLen = prop->len;
	char *nd = new char[ len + origLen ];
	memcpy( nd, prop->data, origLen );
	memcpy( nd+origLen, data, len );
	delete [] prop->data;
	prop->len = len + origLen;
	prop->data = nd;
    } break;
    case PropPrepend: {
#ifdef QWS_PROPERTY_DEBUG
	qDebug( "PropPrepend" );
#endif
	int origLen = origLen = prop->len;
	char *nd = new char[ len + origLen ];
	memcpy( nd, data, len );
	memcpy( nd+len, prop->data, origLen );
	delete [] prop->data;
	prop->len = len + origLen;
	prop->data = nd;
    } break;
    }
#ifdef QWS_PROPERTY_DEBUG
    qDebug( "QWSPropertyManager::setProperty: %d %d (%s) to %s", winId, property, key,
	    d->properties.find( key )->data );
#endif

    return TRUE;
}

bool QWSPropertyManager::hasProperty( int winId, int property )
{
    Data::Property* prop = d->find(winId,property);
    return !!prop;
}

bool QWSPropertyManager::removeProperty( int winId, int property )
{
#ifdef QWS_PROPERTY_DEBUG
    qDebug( "QWSPropertyManager::removeProperty %d %d (%s)", winId, property, key );
#endif
    QIntDict<Data::Property>* wp = d->properties.find(winId);
    if ( !wp ) return FALSE;
    Data::Property* prop = wp->find(property);
    if ( !prop ) return FALSE;
    wp->remove(property);
    if ( wp->count() == 0 )
	d->properties.remove(winId);
    return TRUE;
}

bool QWSPropertyManager::addProperty( int winId, int property )
{
    QIntDict<Data::Property>* wp = d->properties.find(winId);
    if ( !wp ) {
	d->properties.insert(winId,wp = new QIntDict<Data::Property>);
	wp->setAutoDelete(TRUE);
    }
    Data::Property* prop = wp->find(property);
    if ( prop ) return FALSE;
    wp->insert(property, prop = new Data::Property);

    prop->len = -1;
    prop->data = 0;
#ifdef QWS_PROPERTY_DEBUG
    qDebug( "QWSPropertyManager::addProperty: %d %d (%s)", winId, property, key );
#endif
    return TRUE;
}

bool QWSPropertyManager::getProperty( int winId, int property, char *&data, int &len )
{
    Data::Property* prop = d->find(winId,property);
    if ( !prop ) {
	data = 0;
	len = -1;
	return FALSE;
    }

    len = prop->len;
    data = prop->data;
#ifdef QWS_PROPERTY_DEBUG
    qDebug( "QWSPropertyManager::getProperty: %d %d (%s) %d", winId, property,
	    key, len );
    if ( len > 0 ) {
	for ( int i = 0; i < len; i++ )
	    printf( "%c",data[i] );
	printf( "\n" );
    }
#endif

    return TRUE;
}

bool QWSPropertyManager::removeProperties( int winId )
{
    return d->properties.remove(winId);
}

#endif //QT_NO_QWS_PROPERTIES
