/****************************************************************************
**
** Definition of QGfxDriverPlugin
**
** Created : 20020212
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

#ifndef QGFXDRIVERPLUGIN_QWS_H
#define QGFXDRIVERPLUGIN_QWS_H

#ifndef QT_H
#include "qgplugin.h"
#include "qstringlist.h"
#endif // QT_H

#ifndef QT_NO_COMPONENT

class QScreen;
class QGfxDriverPluginPrivate;

class Q_EXPORT QGfxDriverPlugin : public QGPlugin
{
    Q_OBJECT
public:
    QGfxDriverPlugin();
    ~QGfxDriverPlugin();

#ifndef QT_NO_STRINGLIST
    virtual QStringList keys() const = 0;
#endif
    virtual QScreen* create( const QString& driver, int displayId ) = 0;

private:
    QGfxDriverPluginPrivate *d;
};

#endif // QT_NO_COMPONENT

#endif // QGFXDRIVERPLUGIN_QWS_H
