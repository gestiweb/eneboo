/****************************************************************************
**
** Definition of Qt/Embedded keyboards
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

#ifndef QKBD_QWS_H
#define QKBD_QWS_H

#ifndef QT_H
#include "qapplication.h"
#endif // QT_H

#ifndef QT_NO_QWS_KEYBOARD

class QWSKbPrivate;

class QWSKeyboardHandler
{
public:
    QWSKeyboardHandler();
    virtual ~QWSKeyboardHandler();

    virtual void processKeyEvent(int unicode, int keycode, int modifiers,
			    bool isPress, bool autoRepeat);

protected:
    int transformDirKey( int key );
    void beginAutoRepeat( int uni, int code, int mod );
    void endAutoRepeat();

private:
    QWSKbPrivate *d;
};

#endif // QT_NO_QWS_KEYBOARD

#endif // QKBD_QWS_H
