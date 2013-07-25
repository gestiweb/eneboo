/****************************************************************************
**
** Implementation of QMouseDriverPlugin
**
** Created : 20020218
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

#include "qmousedriverplugin_qws.h"

#ifndef QT_NO_COMPONENT

#include "qmousedriverinterface_p.h"
#include "qmouse_qws.h"

/*!
    \class QMouseDriverPlugin qmousedriverplugin_qws.h
    \brief The QMouseDriverPlugin class provides an abstract base for
    Qt/Embedded mouse driver plugins.

    \ingroup plugins

    The mouse driver plugin is a simple plugin interface that makes it
    easy to create custom mouse drivers.

    Writing a mouse driver plugin is achieved by subclassing this base
    class, reimplementing the pure virtual functions keys() and
    create(), and exporting the class with the \c Q_EXPORT_PLUGIN
    macro. See the \link plugins-howto.html Plugins
    documentation\endlink for details.

    This class is only available in Qt/Embedded.
*/

/*!
    \fn QStringList QMouseDriverPlugin::keys() const

    Returns the list of mouse drivers this plugin supports.

    \sa create()
*/


class QMouseDriverPluginPrivate : public QMouseDriverInterface
{
public:
    QMouseDriverPluginPrivate( QMouseDriverPlugin *p )
	: plugin( p )
    {
    }
    virtual ~QMouseDriverPluginPrivate();

    QRESULT queryInterface( const QUuid &iid, QUnknownInterface **iface );
    Q_REFCOUNT;

#ifndef QT_NO_STRINGLIST
    QStringList featureList() const;
#endif

    QWSMouseHandler* create( const QString& driver, const QString &device );

private:
    QMouseDriverPlugin *plugin;
};

QMouseDriverPluginPrivate::~QMouseDriverPluginPrivate()
{
    delete plugin;
}

QRESULT QMouseDriverPluginPrivate::queryInterface( const QUuid &iid, QUnknownInterface **iface )
{
    *iface = 0;

    if ( iid == IID_QUnknown )
	*iface = this;
    else if ( iid == IID_QFeatureList )
	*iface = this;
    else if ( iid == IID_QMouseDriver )
	*iface = this;
    else
	return QE_NOINTERFACE;

    (*iface)->addRef();
    return QS_OK;
}

#ifndef QT_NO_STRINGLIST
QStringList QMouseDriverPluginPrivate::featureList() const
{
    return plugin->keys();
}
#endif

QWSMouseHandler* QMouseDriverPluginPrivate::create( const QString& driver, const QString &device )
{
    qDebug( "Loading plugin: %s", driver.latin1() );
    return plugin->create( driver, device );
}

/*!
    Constructs a mouse driver plugin. This is invoked automatically by
    the \c Q_EXPORT_PLUGIN macro.
*/
QMouseDriverPlugin::QMouseDriverPlugin()
    : QGPlugin( d = new QMouseDriverPluginPrivate( this ) )
{
}

/*!
    Destroys the mouse driver plugin.

    You never have to call this explicitly. Qt destroys a plugin
    automatically when it is no longer used.
*/
QMouseDriverPlugin::~QMouseDriverPlugin()
{
}


/*!
    \fn QScreen* QMouseDriverPlugin::create( const QString &driver, const QString& device )

    Creates a driver matching the type specified by \a driver and
    which uses device \a device.

    \sa keys()
*/

QWSMouseHandler* QMouseDriverPlugin::create( const QString& driver, const QString &device )
{
    Q_UNUSED( driver )
    Q_UNUSED( device )
    return 0;
}

#endif // QT_NO_COMPONENT
