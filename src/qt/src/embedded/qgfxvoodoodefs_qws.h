/****************************************************************************
**
** Voodoo defines
**
** Created : 940721
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

#ifndef QGFXVOODOODEFS_QWS_H
#define QGFXVOODOODEFS_QWS_H

#ifndef QT_H
#include "qglobal.h"
#endif // QT_H

#define VOODOOSTATUS (0x000)
#define INTCTRL (0x004+0x0100000)
#define CLIP0MIN (0x008+0x0100000)
#define CLIP0MAX (0x00c+0x0100000)
#define DSTBASEADDR (0x010+0x0100000)
#define DSTFORMAT (0x014+0x0100000)
#define SRCCOLORKEYMIN (0x018+0x0100000)
#define SRCCOLORKEYMAX (0x01c+0x0100000)
#define DSTCOLORKEYMIN (0x020+0x0100000)
#define DSTCOLORKEYMAX (0x024+0x0100000)
#define BRESERROR0 (0x028+0x0100000)
#define BRESERROR1 (0x02c+0x0100000)
#define ROP (0x030+0x0100000)
#define SRCBASEADDR (0x034+0x0100000)
#define COMMANDEXTRA (0x038+0x0100000)
#define LINESTIPPLE (0x03c+0x0100000)
#define LINESTYLE (0x040+0x0100000)
#define PATTERN0ALIAS (0x044+0x0100000)
#define PATTERN1ALIAS (0x048+0x0100000)
#define CLIP1MIN (0x04c+0x0100000)
#define CLIP1MAX (0x050+0x0100000)
#define SRCFORMAT (0x054+0x0100000)
#define SRCSIZE (0x058+0x0100000)
#define SRCXY (0x05c+0x0100000)
#define COLORBACK (0x060+0x0100000)
#define COLORFORE (0x064+0x0100000)
#define DSTSIZE (0x068+0x0100000)
#define DSTXY (0x06c+0x0100000)
#define COMMAND (0x070+0x0100000)
#define LAUNCHAREA (0x080+0x0100000)
#define COLORPATTERN (0x100+0x0100000)

#define VIDPROCCFG 0x5c
#define HWCURC0 0x68
#define HWCURC1 0x6c
#define HWCURPATADDR 0x60
#define HWCURLOC 0x64

#endif // QGFXVOODOODEFS_QWS_H
