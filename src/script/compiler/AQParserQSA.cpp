/***************************************************************************
AQParserQSA.cpp
-------------------
begin                : 09/11/2007
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

#include <QBuffer>
#include <qserrors.h>

#include <AQCore/AQCoreUtil.h>

#include "AQParserQSA.h"
#include "AQNodesQSA.h"
#include "AQCompilerQSA.h"

AQParserQSA::AQParserQSA( QSLexer * l ) :
    lx( l )
{}

int AQParserQSA::parseByteCode( const QByteArray & byteCode ) const
{
  QBuffer byteCodeBuffer( const_cast<QByteArray *>( &byteCode ) );
  byteCodeBuffer.open( QIODevice::ReadOnly );
  QDataStream in( &byteCodeBuffer );
  return parseByteCode( in );
}

int AQParserQSA::parseByteCode( QDataStream & in ) const
{
  Q_ASSERT( lx );

  bool compatByteCode = AQCompilerQSA::isCompatibleByteCode( in );

  if ( !compatByteCode ) {
    lx->setErrorMessage( AQ_TR( "ScriptEngineQSA", "Versión de ByteCode no compatible" ) );
    lx->setState( QSLexer::Bad );
    return QSErrParseError;
  }

  lx->setState( QSLexer::Start );

  AQNode::clear();
  AQNode::unserializeNodes( in, lx );
  AQNode::clear();

  return ( ( !lx->errorMessage().isEmpty() || lx->lexerState() == QSLexer::Bad ) ? QSErrParseError : QSErrNoError );
}
