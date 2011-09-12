/****************************************************************************
** $Id: quickcolorbutton.cpp  1.1.5   edited 2006-02-23T15:39:57$
**
** Copyright (C) 2001-2006 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt Script for Applications framework (QSA).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding a valid QSA Beta Evaluation Version license may use
** this file in accordance with the QSA Beta Evaluation Version License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about QSA Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
*****************************************************************************/

#include "quickcolorbutton.h"
#include <qcolordialog.h>
#include <qpixmap.h>

ColorButton::ColorButton( QWidget *parent, const char *name )
    : QPushButton( parent, name )
{
    col = black;
    createPixmap();
    connect( this, SIGNAL( clicked() ), this, SLOT( getColor() ) );
}

QString ColorButton::color() const
{
    return col.name();
}

void ColorButton::setColor( const QString &c )
{
    col = QColor( c );
    createPixmap();
}

void ColorButton::getColor()
{
    QColor c = QColorDialog::getColor( col );
    if  ( !c.isValid() )
	return;
    col = c;
    createPixmap();
    emit colorChanged( col );
}

void ColorButton::createPixmap()
{
    QPixmap pix( 16, 16 );
    pix.fill( col );
    setPixmap( pix );
}
