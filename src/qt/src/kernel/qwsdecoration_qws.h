/****************************************************************************
**
** Definition of QWSDecoration class.
**
** Created : 20000308
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

#ifndef QWSDECORATION_QWS_H
#define QWSDECORATION_QWS_H

#ifndef QT_H
#include "qregion.h"
#endif // QT_H

class QPopupMenu;

/*
 Implements decoration styles
*/
class QWSDecoration
{
public:
    QWSDecoration() {}
    virtual ~QWSDecoration() {}

    enum Region { None=0, All=1, Title=2, Top=3, Bottom=4, Left=5, Right=6,
		TopLeft=7, TopRight=8, BottomLeft=9, BottomRight=10,
		Close=11, Minimize=12, Maximize=13, Normalize=14,
		Menu=15, LastRegion=Menu };

    virtual QRegion region(const QWidget *, const QRect &rect, Region r=All) = 0;
    virtual void close( QWidget * );
    virtual void minimize( QWidget * );
    virtual void maximize( QWidget * );
#ifndef QT_NO_POPUPMENU
    virtual QPopupMenu *menu(const QWidget *, const QPoint &);
#endif
    virtual void paint(QPainter *, const QWidget *) = 0;
    virtual void paintButton(QPainter *, const QWidget *, Region, int state) = 0;
};

#endif // QWSDECORATION_QWS_H
