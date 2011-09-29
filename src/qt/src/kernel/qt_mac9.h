/****************************************************************************
** $Id: qt/qt_mac9.h   3.3.8   edited Jan 11 14:38 $
**
** Definition of ???
**
** Copyright (C) 1992-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Macintosh may use this file in accordance with the Qt Commercial
** License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QT_MAC9_H
#define QT_MAC9_H

#include "qglobal.h"
#ifdef Q_WS_MAC9

//these don't belong here, but will be moved to an
//appropriate place later
//features
#ifndef QT_NO_IMAGEIO_JPEG
#define QT_NO_IMAGEIO_JPEG
#endif
#ifndef QT_NO_IMAGEIO_MNG
#define QT_NO_IMAGEIO_MNG
#endif
#ifndef QT_NO_SQL
#define QT_NO_SQL
#endif
#define QT_FATAL_ASSERT
#define QT_NO_XINERAMA
#define QT_NO_OPENGL
#define QT_NO_STYLE_WINDOWSXP 

//mac thingy
#ifndef QMAC_ONE_PIXEL_LOCK
#define QMAC_ONE_PIXEL_LOCK
#endif
//carbon things
#define ALLOW_OLD_CARBON
#define _EVENT_HANDLERS 0
#define ALLOW_OLD_CREATE_FOO_CONTROL_PARAMETERS 0
#define CARBON_ON_MAC_O 1
#define ALLOW_OLD_BLOCKING_APIS 0

//hacks to work around wchar problems
#define __WCHARTDEF__
//#define __NO_WIDE_CHAR 1
#undef wchar_t
typedef unsigned short hide_wchar_t; 
#define wchar_t hide_wchar_t

#include <stddef.h>
#include <cstdlib> //pull in some posix stuff
using std::calloc;
using std::free;
using std::realloc;

//hacks to work around malloc(0) returning NULL
static inline void *_mac9_malloc(size_t s) { return std::malloc(s ? s : 1); }
#define malloc(x) _mac9_malloc(x)

#endif //MAC9

#endif //MAC9_H
