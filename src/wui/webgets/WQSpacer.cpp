/***************************************************************************
WQSpacer.cpp
-------------------
begin                : 13/02/2010
copyright            : (C) 2003-2010 by InfoSiAL S.L.
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

#include <WContainerWidget>
#include <WBoxLayout>

#include <QSpacerItem>

#include "WQSpacer.h"
#include "WQSpacer_p.h"

WQSpacerPrivate::WQSpacerPrivate() :
    WQWidgetPrivate()
{
  canContain_ = false;
  isWidget_ = false;
  isLayout_ = false;
  isSpacer_ = true;
}

WQSpacerPrivate::~WQSpacerPrivate()
{}

WQSpacer::WQSpacer( WQWidget * parent ) :
    WQWidget( *new WQSpacerPrivate, static_cast<WObject *> ( parent ) )
{}
