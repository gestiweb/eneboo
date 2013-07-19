/****************************************************************************
**
** Matrox defines
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

#ifndef QGFXMATROXDEFS_QWS_H
#define QGFXMATROXDEFS_QWS_H

#ifndef QT_H
#include "qglobal.h"
#endif // QT_H

#define CXLEFT 0x1ca0
#define CXRIGHT 0x1ca4
#define YTOP 0x1c98
#define YBOT 0x1c9c
#define PITCH 0x1c8c
#define YDSTORG 0x1c94
#define MACCESS 0x1c04
#define CXLEFT 0x1ca0
#define CXRIGHT 0x1ca4
#define PLNWT 0x1c1c
#define FXLEFT 0x1ca8
#define FXRIGHT 0x1cac
#define XDST 0x1cb0
#define YDST 0x1c90
#define LEN 0x1c5c
#define DWGCTL 0x1c00
#define FCOL 0x1c24
#define MATROX_STATUS 0x1e14
#define BCOL 0x1c20
#define FXBNDRY 0x1c84
#define SGN 0x1c58
#define SHIFT 0x1c50
#define SRC0 0x1c30
#define SRC1 0x1c34
#define SRC2 0x1c38
#define SRC3 0x1c3c

#define AR0 0x1c60
#define AR1 0x1c64
#define AR2 0x1c68
#define AR3 0x1c6c
#define AR4 0x1c70
#define AR5 0x1c74

#define EXEC 0x0100
#define DWG_REPLACE 0x000c0000

#define DWG_TRAP 0x04
#define DWG_LINE_CLOSE 0x02
#define DWG_SOLID 0x0800
#define DWG_ARZERO 0x1000
#define DWG_SGNZERO 0x2000
#define DWG_SHIFTZERO 0x4000
#define DWG_TRANSC 0x40000000
#define DWG_BITBLT 0x08
#define DWG_BFCOL 0x04000000

#define DWG_MODE (DWG_TRAP | DWG_SOLID | DWG_ARZERO | DWG_SGNZERO | DWG_SHIFTZERO | DWG_TRANSC | DWG_REPLACE)

#define CURPOS 0x3c0c
#define PALWTADD 0x3c00
#define X_DATAREG 0x3c0a

#define XCURCTL 0x6
#define XCURADDL 0x4
#define XCURADDH 0x5
#define XCURCOL0RED 0x8
#define XCURCOL0GREEN 0x9
#define XCURCOL0BLUE 0xa
#define XCURCOL1RED 0xc
#define XCURCOL1GREEN 0xd
#define XCURCOL1BLUE 0xe

#define XYSTRT 0x1c40
#define XYEND 0x1c44

#endif // QGFXMATROXDEFS_QWS_H
