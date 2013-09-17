/***************************************************************************
AQSqlObjectFactory.h
-------------------
begin                : 27/09/2007
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

#ifndef AQSQLOBJECTFACTORY_H_
#define AQSQLOBJECTFACTORY_H_

#include <AQScript/AQObjectFactoryQSA.h>

class AQSqlObjectFactoryQSA : public AQObjectFactoryQSA
{
public:

  AQSqlObjectFactoryQSA( AQScriptEngineQSA * eng );

  QObject * create( const QString & className,
                    const QVariantList & arguments,
                    QObject * context );

  AQObjectInterfaceQSA * createInterface( const QString & className,
                                          const QVariantList & arguments = QVariantList(),
                                          QObject * context = 0 );

  AQObjectInterfaceQSA * createInterface( AQSharedObjectQSA obj );

  AQObjectInterfaceQSA * createInterfaceAQ2( AQSharedObjectQSA obj );

  AQObjectInterfaceQSA * createInterface( QObject * obj );

  AQObjectInterfaceQSA * createInterfaceAQ2( QObject * obj );

  bool canCreateInterface( const QString & className ) const;
};

#endif /*AQSQLOBJECTFACTORY_H_*/
