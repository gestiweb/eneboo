/***************************************************************************
AQXmlObjectFactoryQSA.h
-------------------
begin                : 03/03/2009
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

#ifndef AQXMLOBJECTFACTORYQSA_H_
#define AQXMLOBJECTFACTORYQSA_H_

#include <AQScript/AQObjectFactoryQSA.h>

class AQXmlObjectFactoryQSA : public AQObjectFactoryQSA
{
public:

  AQXmlObjectFactoryQSA( AQScriptEngineQSA * eng );

  QObject * create( const QString & className,
                    const QVariantList & arguments,
                    QObject * context );

  AQObjectInterfaceQSA * createInterface( const QString & className,
                                          const QVariantList & arguments = QVariantList(),
                                          QObject * context = 0 );

  AQObjectInterfaceQSA * createInterface( AQSharedObjectQSA obj );

  AQObjectInterfaceQSA * createInterface( QObject * obj );

  bool canCreateInterface( const QString & className ) const;
};

#endif /*AQXMLOBJECTFACTORYQSA_H_*/
