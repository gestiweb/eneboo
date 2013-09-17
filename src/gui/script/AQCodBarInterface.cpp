/***************************************************************************
AQCodBarInterface.cpp
-------------------
begin                : 28/02/2009
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
#include <AQScript/AQObjectFactoryQSA.h>

#include "AQCodBarInterface.h"

AQCodBarInterfaceAQ2::AQCodBarInterfaceAQ2( const AQScriptEngineQSA * eng, const QString & value,
    int type, int margin, double scale, double cut, int rotation, bool text_flag,
    QColor fg, QColor bg, int res ) :
    AQObjectInterfaceQSA( eng )
{
  obj_ = QSharedPointer<AQCodBar>( new AQCodBar( value, type, margin, scale, cut, rotation, text_flag, fg, bg, res ) );

  AQObjectFactoryQSA::addInterface( obj_, this );
}

AQCodBarInterfaceAQ2::AQCodBarInterfaceAQ2( const AQScriptEngineQSA * eng, QSharedPointer<AQCodBar> aqCodBar ) :
    AQObjectInterfaceQSA( eng, aqCodBar.data() ),
    obj_( aqCodBar )
{
  AQObjectFactoryQSA::addInterface( obj_, this );
}

AQCodBarInterfaceAQ2::~AQCodBarInterfaceAQ2()
{
  AQObjectFactoryQSA::removeInterface( obj_ );
}

#include "moc_AQCodBarInterface.cpp"
