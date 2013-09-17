/***************************************************************************
AQObjectInterfaceQSA.cpp
-------------------
begin                : 20/10/2007
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

#include <AQSql/AQSqlConnections.h>
#include <AQSql/AQSqlDatabase.h>

#include "AQObjectInterfaceQSA.h"
#include "AQScriptEngineQSA.h"

AQObjectInterfaceQSA::AQObjectInterfaceQSA( const AQScriptEngineQSA * eng, QObject * parent ) :
    QObject( parent ),
    engine( eng )
{
  dbConName_ = eng ? eng->connectionName() : "default";
}

AQSqlDatabase AQObjectInterfaceQSA::dbCon( const QString & conn ) const
{
  return ( conn == "default" && engine ) ? engine->db() : AQSqlConnections::database( conn );
}

QString AQObjectInterfaceQSA::dbConName( const QString & conn ) const
{
  return conn == "default" ? dbConName_ : conn;
}
