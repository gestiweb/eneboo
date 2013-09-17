/***************************************************************************
AQSEditorInterface.cpp
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
#include <AQScript/AQObjectFactoryQSA.h>

#include "AQSEditorInterface.h"

AQSEditorInterfaceAQ2::AQSEditorInterfaceAQ2( const AQScriptEngineQSA * eng,
    QSharedPointer<AQSEditor> aqSEditor ) :
    AQComponentDBInterface( eng, aqSEditor.data() ),
    obj_( aqSEditor )
{
  AQObjectFactoryQSA::addInterface( obj_, this );
}

AQSEditorInterfaceAQ2::~AQSEditorInterfaceAQ2()
{
  AQObjectFactoryQSA::removeInterface( obj_ );
}

#include "moc_AQSEditorInterface.cpp"

