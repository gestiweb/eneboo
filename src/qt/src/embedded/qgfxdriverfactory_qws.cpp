/****************************************************************************
**
** Implementation of QGfxDriverFactory class
**
** Created : 20020212
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the sql module of the Qt GUI Toolkit.
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

#include "qgfxdriverinterface_p.h" // up here for GCC 2.7.* compatibility
#include "qgfxdriverfactory_qws.h"

#include "qapplication.h"
#include "qgfxlinuxfb_qws.h"
#include "qgfxtransformed_qws.h"
#include "qgfxsnap_qws.h"
#include "qgfxmach64_qws.h"
#include "qgfxvoodoo_qws.h"
#include "qgfxmatrox_qws.h"
#include "qgfxvfb_qws.h"
#include "qgfxvnc_qws.h"
#include "qgfxvga16_qws.h"
#include "qgfxshadowfb_qws.h"
#include "qgfxrepeater_qws.h"
#ifdef Q_OS_QNX6
# include "qwsgfx_qnx6.h"
#endif
#include <stdlib.h>

#if !defined(Q_OS_WIN32) || defined(QT_MAKEDLL)
#include <private/qpluginmanager_p.h>
#ifndef QT_NO_COMPONENT
class QGfxDriverFactoryPrivate : public QObject
{
public:
    QGfxDriverFactoryPrivate();
    ~QGfxDriverFactoryPrivate();

    static QPluginManager<QGfxDriverInterface> *manager;
};

static QGfxDriverFactoryPrivate *instance = 0;
QPluginManager<QGfxDriverInterface> *QGfxDriverFactoryPrivate::manager = 0;

QGfxDriverFactoryPrivate::QGfxDriverFactoryPrivate()
: QObject( qApp )
{
    manager = new QPluginManager<QGfxDriverInterface>( IID_QGfxDriver, QApplication::libraryPaths(), "/gfxdrivers", FALSE );
}

QGfxDriverFactoryPrivate::~QGfxDriverFactoryPrivate()
{
    delete manager;
    manager = 0;

    instance = 0;
}

#endif //QT_NO_COMPONENT
#endif //QT_MAKEDLL

/*!
    \class QGfxDriverFactory qgfxdriverfactory.h
    \brief The QGfxDriverFactory class creates QScreen objects for Qt/Embedded.

    The graphics driver factory creates a QScreen object for a given
    key with QGfxDriverFactory::create(key).

    The drivers are either built-in or dynamically loaded from a
    driver plugin (see \l QGfxDriverPlugin).

    QGfxDriverFactory::keys() returns a list of valid keys. Qt
    currently ships with "LinuxFb".

    This class is only available in Qt/Embedded.
*/

/*!
    Creates a QScreen object of a type that matches \a key, and with
    the ID, \a displayId. The QScreen object returned may be from a
    built-in driver, or from a driver plugin.

    \sa keys()
*/
QScreen *QGfxDriverFactory::create( const QString& key, int displayId )
{
    QString driver = key.lower();
#ifdef Q_OS_QNX6
    if ( driver == "qnxfb" || driver.isEmpty() )
	return new QQnxScreen( displayId );
#endif
#ifndef QT_NO_QWS_VFB
    if ( driver == "qvfb" || driver.isEmpty() )
	return new QVFbScreen( displayId );
#endif
#ifndef QT_NO_QWS_SNAP
    if ( driver == "snap" )
        return new QSNAPScreen( displayId );
#endif
#ifndef QT_NO_QWS_LINUXFB
    if ( driver == "linuxfb" || driver.isEmpty() )
        return new QLinuxFbScreen( displayId );
#endif
#ifndef QT_NO_QWS_VGA16
    if ( driver == "vga16" || driver.isEmpty() )
	return new QVga16Screen( displayId );
#endif
#ifndef QT_NO_QWS_TRANSFORMED
    if ( driver == "transformed" )
        return new QTransformedScreen( displayId );
#endif
#ifndef QT_NO_QWS_MACH64
    if ( driver == "mach64" )
        return new QMachScreen( displayId );
#endif
#ifndef QT_NO_QWS_VOODOO3
    if ( driver == "voodoo3" )
        return new QVoodooScreen( displayId );
#endif
#ifndef QT_NO_QWS_MATROX
    if ( driver == "matrox" )
        return new QMatroxScreen( displayId );
#endif
#ifndef QT_NO_QWS_VNC
    if ( driver == "vnc" )
	return new QVNCScreen( displayId );
#endif
#ifndef QT_NO_QWS_SHADOWFB
    if ( driver == "shadowfb" )
	return new QShadowFbScreen( displayId );
#endif
#ifndef QT_NO_QWS_REPEATER
    if ( driver == "repeater" )
	return new QRepeaterScreen( displayId );
#endif

#if !defined(Q_OS_WIN32) || defined(QT_MAKEDLL)
#ifndef QT_NO_COMPONENT
    if ( !instance )
	instance = new QGfxDriverFactoryPrivate;

    QInterfacePtr<QGfxDriverInterface> iface;
    QGfxDriverFactoryPrivate::manager->queryInterface( driver, &iface );

    if ( iface )
	return iface->create( driver, displayId );
#endif
#endif
    return 0;
}

#ifndef QT_NO_STRINGLIST
/*!
    Returns the list of keys this factory can create drivers for.

    \sa create()
*/
QStringList QGfxDriverFactory::keys()
{
    QStringList list;

#ifdef Q_OS_QNX6
    if ( !list.contains( "QnxFb" ) )
	list << "QnxFb";
#endif
#ifndef QT_NO_QWS_VFB
    if ( !list.contains( "QVFb" ) )
	list << "QVFb";
#endif
#ifndef QT_NO_QWS_SNAP
    if ( !list.contains( "snap" ) )
	list << "snap";
#endif
#ifndef QT_NO_QWS_LINUXFB
    if ( !list.contains( "LinuxFb" ) )
	list << "LinuxFb";
#endif
#ifndef QT_NO_QWS_VGA16
    if ( !list.contains( "VGA16" ) )
	list << "VGA16";
#endif
#ifndef QT_NO_QWS_TRANSFORMED
    if ( !list.contains( "Transformed" ) )
	list << "Transformed";
#endif
#ifndef QT_NO_QWS_MACH64
    if ( !list.contains( "Mach64" ) )
	list << "Mach64";
#endif
#ifndef QT_NO_QWS_VOODOO3
    if ( !list.contains( "Voodoo3" ) )
	list << "Voodoo3";
#endif
#ifndef QT_NO_QWS_MATROX
    if ( !list.contains( "Matrox" ) )
	list << "Matrox";
#endif
#ifndef QT_NO_QWS_VNC
    if ( !list.contains( "VNC" ) )
	list << "VNC";
#endif
#ifndef QT_NO_QWS_SHADOWFB
    if ( !list.contains( "ShadowFb" ) )
	list << "ShadowFb";
#endif
#ifndef QT_NO_QWS_REPEATER
     if ( !list.contains( "Repeater" ) )
	list << "Repeater";
#endif

#if !defined(Q_OS_WIN32) || defined(QT_MAKEDLL)
#ifndef QT_NO_COMPONENT
    if ( !instance )
	instance = new QGfxDriverFactoryPrivate;

    list += QGfxDriverFactoryPrivate::manager->featureList();
#endif //QT_NO_COMPONENT
#endif //QT_MAKEDLL

    return list;
}
#endif

