/****************************************************************************
**
** Implementation of QKbdDriverPlugin
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

#include "qkbddriverplugin_qws.h"

#ifndef QT_NO_COMPONENT

#include "qkbddriverinterface_p.h"
#include "qkbd_qws.h"

/*!
    \class QKbdDriverPlugin qkbddriverplugin_qws.h
    \brief The QKbdDriverPlugin class provides an abstract base for
    Qt/Embedded keyboard driver plugins.

    \ingroup plugins

    The keyboard driver plugin is a simple plugin interface that makes
    it easy to create custom keyboard drivers.

    Writing a keyboard driver plugin is achieved by subclassing this
    base class, reimplementing the pure virtual functions keys() and
    create(), and exporting the class with the \c Q_EXPORT_PLUGIN
    macro. See the \link plugins-howto.html Plugins
    documentation\endlink for details.

    This class is only available in Qt/Embedded.
*/

/*!
    \fn QStringList QKbdDriverPlugin::keys() const

    Returns the list of keyboard drivers this plugin supports.

    \sa create()
*/


class QKbdDriverPluginPrivate : public QKbdDriverInterface
{
public:
    QKbdDriverPluginPrivate( QKbdDriverPlugin *p )
	: plugin( p )
    {
    }
    virtual ~QKbdDriverPluginPrivate();

    QRESULT queryInterface( const QUuid &iid, QUnknownInterface **iface );
    Q_REFCOUNT;

#ifndef QT_NO_STRINGLIST
    QStringList featureList() const;
#endif

    QWSKeyboardHandler* create( const QString& driver, const QString &device );

private:
    QKbdDriverPlugin *plugin;
};

QKbdDriverPluginPrivate::~QKbdDriverPluginPrivate()
{
    delete plugin;
}

QRESULT QKbdDriverPluginPrivate::queryInterface( const QUuid &iid, QUnknownInterface **iface )
{
    *iface = 0;

    if ( iid == IID_QUnknown )
	*iface = this;
    else if ( iid == IID_QFeatureList )
	*iface = this;
    else if ( iid == IID_QKbdDriver )
	*iface = this;
    else
	return QE_NOINTERFACE;

    (*iface)->addRef();
    return QS_OK;
}

#ifndef QT_NO_STRINGLIST
QStringList QKbdDriverPluginPrivate::featureList() const
{
    return plugin->keys();
}
#endif

QWSKeyboardHandler* QKbdDriverPluginPrivate::create( const QString& driver, const QString &device )
{
    return plugin->create( driver, device );
}

/*!
    Constructs a keyboard driver plugin. This is invoked automatically
    by the \c Q_EXPORT_PLUGIN macro.
*/
QKbdDriverPlugin::QKbdDriverPlugin()
    : QGPlugin( d = new QKbdDriverPluginPrivate( this ) )
{
}

/*!
    Destroys the keyboard driver plugin.

    You never have to call this explicitly. Qt destroys a plugin
    automatically when it is no longer used.
*/
QKbdDriverPlugin::~QKbdDriverPlugin()
{
}


/*!
    \fn QScreen* QKbdDriverPlugin::create( const QString &driver, const QString &device )

    Creates a driver matching the type specified by \a driver and \a device.

    \sa keys()
*/

QWSKeyboardHandler* QKbdDriverPlugin::create( const QString& driver, const QString &device )
{
    Q_UNUSED( driver )
    Q_UNUSED( device )
    return 0;
}

#endif // QT_NO_COMPONENT
