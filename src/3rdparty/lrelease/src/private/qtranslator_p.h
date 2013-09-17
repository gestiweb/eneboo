/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QTRANSLATOR_P_H
#define QTRANSLATOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qfontencodings_x11.cpp and qfont_x11.cpp.  This header file may
// change from version to version without notice, or even be removed.
//
// We mean it.
//

enum {
    EQ          = 0x01,
    LT          = 0x02,
    LEQ         = 0x03,
    BETWEEN     = 0x04,

    NOT         = 0x08,
    MOD_10      = 0x10,
    MOD_100     = 0x20,

    AND         = 0xFD,
    OR          = 0xFE,
    NEWRULE     = 0xFF,

    OP_MASK     = 0x07,

    NEQ         = NOT | EQ,
    GT          = NOT | LEQ,
    GEQ         = NOT | LT,
    NOT_BETWEEN = NOT | BETWEEN
};

#endif
