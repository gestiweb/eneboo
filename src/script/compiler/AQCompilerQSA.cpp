/***************************************************************************
AQCompilerQSA.cpp
-------------------
begin                : 08/11/2007
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

#include "AQCompilerQSA.h"
#include "AQNodesQSA.h"

extern int aqyyparse( QSLexer * );
extern QString qs_format_error( const QString & );

AQCompilerQSA::AQCompilerQSA( QSLexer * l ) :
    lx( l )
{}

QByteArray AQCompilerQSA::byteCode( const QString & code, int id, int lineno )
{
  if ( code.isEmpty() )
    return QByteArray();

  AQNode::clear();
  errType = 0;
  errLines.clear();
  errMsgs.clear();

  Q_ASSERT( lx );

  lx->setCode( code, id, lineno );
  int parseError = aqyyparse( lx );

  if ( parseError || lx ->lexerState() == QSLexer::Bad ) {
    errType = QSErrParseError;
    errLines.append( lx ->lineNo() );
    errMsgs.append( AQ_TR( "ScriptEngineQSA", "Parse Error: %1" ).arg(
                      qs_format_error( lx->errorMessage() ) ) );
    AQNode::clear();
    return QByteArray();
  }

  QBuffer byteCodeBuffer;
  byteCodeBuffer.open( QIODevice::WriteOnly );
  QDataStream out( &byteCodeBuffer );

  quint32 bKey = buildKey();
  out << ( quint32 ) bKey;
  AQNode::serializeNodes( out );
  byteCodeBuffer.close();
  AQNode::clear();

  return byteCodeBuffer.buffer();
}

quint32 AQCompilerQSA::buildKey()
{
  return QSLexer::hashUstr( QLatin1String( "<BYTECODE>AbanQ_Compiler_QSA_" ) +
                            AQQSAC_VERSION +
                            QLatin1String( "</BYTECODE>" ) );
}

bool AQCompilerQSA::isCompatibleByteCode( const QByteArray & byteCode )
{
  QBuffer byteCodeBuffer( const_cast<QByteArray *>( &byteCode ) );
  byteCodeBuffer.open( QIODevice::ReadOnly );
  QDataStream in( &byteCodeBuffer );
  return isCompatibleByteCode( in );
}

bool AQCompilerQSA::isCompatibleByteCode( QDataStream & in )
{
  quint32 bKey;
  in >> bKey;
  if ( bKey != buildKey() )
    in >> bKey;
  return ( bKey == buildKey() );
}

int AQCompilerQSA::errorType() const
{
  return errType;
}

QList<int> AQCompilerQSA::errorLines() const
{
  return errLines;
}

QStringList AQCompilerQSA::errorMessages() const
{
  return errMsgs;
}
