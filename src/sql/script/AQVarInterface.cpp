/***************************************************************************
AQVarInterface.cpp
-------------------
begin                : 01/03/2009
copyright            : (C) 2003-2009 by InfoSiAL S.L.
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

#include <AQScript/AQScriptEngineQSA.h>

#include "AQVarInterface.h"
#include "../util/AQVar.h"

AQVarInterface::AQVarInterface( const AQScriptEngineQSA * eng, const QString & connName ) :
    AQObjectInterfaceQSA( eng )
{
  obj_ = new AQVar( dbConName( connName ) );
}

AQVarInterface::~AQVarInterface()
{
  delete obj_;
}

bool AQVarInterface::set( const QString & n, const QVariant & v )
{
  return obj_->set( n, v );
}

QVariant AQVarInterface::get( const QString & n ) const
{
  return obj_->get( n );
}

#include "moc_AQVarInterface.cpp"
