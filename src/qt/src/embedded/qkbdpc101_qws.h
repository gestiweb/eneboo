/****************************************************************************
**
** Definition of Qt/Embedded keyboards
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

#ifndef QKBDPC101_QWS_H
#define QKBDPC101_QWS_H

#ifndef QT_H
#include "qkbd_qws.h"
#endif // QT_H

#ifndef QT_NO_QWS_KEYBOARD

#ifndef QT_NO_QWS_KBD_PC101

typedef struct QWSKeyMap {
    ushort key_code;
    ushort unicode;
    ushort shift_unicode;
    ushort ctrl_unicode;
};

class QWSPC101KeyboardHandler : public QWSKeyboardHandler
{
public:
    QWSPC101KeyboardHandler(const QString&);
    virtual ~QWSPC101KeyboardHandler();

    virtual void doKey(uchar scancode);
    virtual const QWSKeyMap *keyMap() const;

protected:
    bool shift;
    bool alt;
    bool ctrl;
    bool caps;
#if defined(QT_QWS_IPAQ)
    uint ipaq_return_pressed:1;
#endif
    uint extended:2;
    int modifiers;
    int prevuni;
    int prevkey;
};

#endif // QT_NO_QWS_KBD_PC101

#endif // QT_NO_QWS_KEYBOARD

#endif // QKBDTTY_QWS_H

