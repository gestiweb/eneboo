/***************************************************************************
                          FLWidgetAction.cpp
                         -------------------
begin                : Mar Jun 17 2003
copyright            : (C) 2003-2005 by InfoSiAL S.L.
email                : mail@infosial.com
***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************
   Este  programa es software libre. Puede redistribuirlo y/o modificarlo
   bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
   versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/
/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "FLWidgetAction.h"
#include <qwidget.h>
#include <qtoolbutton.h>
#include <qstylefactory.h>
#include <qstyle.h>
#include <qlayout.h>

void FLWidgetAction::addedTo( QWidget * w, QWidget * container ) {
  if ( w->inherits( "QToolButton" ) ) {
    static_cast<QToolButton *>( w ) ->setUsesTextLabel( true );
    static_cast<QToolButton *>( w ) ->setTextPosition( QToolButton::Right );
    static_cast<QToolButton *>( w ) ->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  }
}

FLWidgetAction::~FLWidgetAction() {}

void FLWidgetAction::init( const QString & g ) {
  grp = g;
}
