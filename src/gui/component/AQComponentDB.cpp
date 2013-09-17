/***************************************************************************
AQComponentDB.cpp
-------------------
begin                : 31/08/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
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

#include "AQComponentDB.h"

AQComponentDB::AQComponentDB( QWidget * parent ) :
    QFrame( parent ), form_( 0 )
{}

AQComponentDB::~AQComponentDB()
{}

void AQComponentDB::setupUi( AQFormDB * form )
{
  form_ = form;
}

