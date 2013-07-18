/****************************************************************************
**
** Declaration of Qt/Embedded window manager decorations
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

#ifndef QWSDEFAULTDECORATION_QWS_H
#define QWSDEFAULTDECORATION_QWS_H

#ifndef QT_H
#include "qwsmanager_qws.h"
#endif // QT_H

#ifndef QT_NO_QWS_MANAGER

#define CORNER_GRAB	16
#define BORDER_WIDTH	4
#define BOTTOM_BORDER_WIDTH	2*BORDER_WIDTH


class QWSDefaultDecoration : public QWSDecoration
{
public:
    QWSDefaultDecoration();
    virtual ~QWSDefaultDecoration();

    virtual QRegion region(const QWidget *, const QRect &rect, Region);
    virtual void paint(QPainter *, const QWidget *);
    virtual void paintButton(QPainter *, const QWidget *, Region, int state);

protected:
    virtual const QPixmap* pixmapFor(const QWidget *, Region, bool, int&, int&);

    /* Added these virtual functions to enable other styles to be added more easily */
    virtual int getTitleWidth(const QWidget *);
    virtual int getTitleHeight(const QWidget *);

#ifndef QT_NO_IMAGEIO_XPM
    virtual const char **menuPixmap();
    virtual const char **closePixmap();
    virtual const char **minimizePixmap();
    virtual const char **maximizePixmap();
    virtual const char **normalizePixmap();
#endif

private:

    static QPixmap * staticMenuPixmap;
    static QPixmap * staticClosePixmap;
    static QPixmap * staticMinimizePixmap;
    static QPixmap * staticMaximizePixmap;
    static QPixmap * staticNormalizePixmap;

};

#endif // QT_NO_QWS_MANAGER

#endif // QWSDEFAULTDECORATION_QWS_H
