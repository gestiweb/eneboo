/****************************************************************************
** $Id: qt/qcolor_mac.cpp   3.3.8   edited Jan 11 14:38 $
**
** Implementation of QColor class for mac
**
** Created : 001019
**
** Copyright (C) 1992-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech ASA of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
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
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qcolor.h"
#include "private/qcolor_p.h"
#include "string.h"
#include "qpaintdevice.h"
#include "qapplication.h"
#include "private/qapplication_p.h"

// NOT REVISED

#include "qintdict.h"


/*
  This function is called from the event loop. It resets the colors_avail
  flag so that the application can retry to allocate read-only colors
  that other applications may have deallocated lately.

  The g_our_alloc and g_carr are global arrays that optimize color
  approximation when there are no more colors left to allocate.
*/

void qt_reset_color_avail()
{
}

/*****************************************************************************
  QColor static member functions
 *****************************************************************************/

int QColor::maxColors()
{
    return -1;
}

int QColor::numBitPlanes()
{
    return 32;
}


void QColor::initialize()
{
    if ( color_init )
	return;

    color_init = TRUE;
}

void QColor::cleanup()
{
}


/*****************************************************************************
  QColor member functions
 *****************************************************************************/

uint QColor::alloc()
{
    d.d32.pix = qRed( d.argb ) << 16 | qGreen( d.argb ) << 8 | qBlue( d.argb );
    return d.d32.pix;
}


void QColor::setSystemNamedColor( const QString& name )
{
    // setSystemNamedColor should look up rgb values from the built in
    // color tables first (see qcolor_p.cpp), and failing that, use
    // the window system's interface for translating names to rgb values...
    // we do this so that things like uic can load an XPM file with named colors
    // and convert it to a png without having to use window system functions...
    d.argb = qt_get_rgb_val( name.latin1() );
    QRgb rgb;
    if ( qt_get_named_rgb( name.latin1(), &rgb ) ) {
	d.argb = rgb;
	if ( colormodel == d8 ) {
	    d.d8.invalid = FALSE;
	    d.d8.dirty = TRUE;
	    d.d8.pix = 0;
	} else {
	    alloc();
	}
    } else {
	// set to invalid color
	*this = QColor();
    }
}


int QColor::enterAllocContext()
{
    return 1;
}


void QColor::leaveAllocContext()
{
}


int QColor::currentAllocContext()
{
    return 0;
}


void QColor::destroyAllocContext( int )
{
}

