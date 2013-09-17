/***************************************************************************
AQCoreObjectFactoryQSA.h
-------------------
begin                : 24/09/2007
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

#ifndef AQCOREOBJECTFACTORYQSA_H_
#define AQCOREOBJECTFACTORYQSA_H_

#include <AQScript/AQObjectFactoryQSA.h>

class AQCoreObjectFactoryQSA : public AQObjectFactoryQSA
{
public:

  AQCoreObjectFactoryQSA( AQScriptEngineQSA * eng );

  QObject * create( const QString & className,
                    const QVariantList & arguments,
                    QObject * context );

  AQObjectInterfaceQSA * createInterface( const QString & className,
                                          const QVariantList & arguments = QVariantList(),
                                          QObject * context = 0 );

  bool canCreateInterface( const QString & ) const;
};

#endif /*AQCOREOBJECTFACTORYQSA_H_*/
