/***************************************************************************
AQObjectFactoryQSA.h
-------------------
begin                : 30/03/2008
copyright            : (C) 2003-2008 by InfoSiAL S.L.
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

#ifndef AQOBJECTFACTORYQSA_H_
#define AQOBJECTFACTORYQSA_H_

#include <AQScript/AQScript.h>

#include <qsobjectfactory.h>

class AQScriptEngineQSA;
class AQObjectInterfaceQSA;

class AQObjectFactoryQSA : public QSObjectFactory
{
public:

  AQObjectFactoryQSA( AQScriptEngineQSA * eng );

  virtual AQObjectInterfaceQSA *
  createInterface( const QString &,
                   const QVariantList & = QVariantList(),
                   QObject * = 0 )
  {
    Q_ASSERT( 0 );
    return 0;
  }

  virtual AQObjectInterfaceQSA *
  createInterfaceAQ2( const QString &,
                      const QVariantList & = QVariantList(),
                      QObject * = 0 )
  {
    Q_ASSERT( 0 );
    return 0;
  }

  virtual AQObjectInterfaceQSA * createInterface( AQSharedObjectQSA )
  {
    Q_ASSERT( 0 );
    return 0;
  }

  virtual AQObjectInterfaceQSA * createInterfaceAQ2( AQSharedObjectQSA )
  {
    Q_ASSERT( 0 );
    return 0;
  }

  virtual AQObjectInterfaceQSA * createInterface( QObject * )
  {
    Q_ASSERT( 0 );
    return 0;
  }

  virtual AQObjectInterfaceQSA * createInterfaceAQ2( QObject * )
  {
    Q_ASSERT( 0 );
    return 0;
  }

  virtual bool canCreateInterface( const QString & ) const = 0;

  static AQObjectInterfaceQSA * getInterface( AQSharedObjectQSA obj );
  static AQObjectInterfaceQSA * getInterface( QObject * obj );
  static void addInterface( AQSharedObjectQSA, AQObjectInterfaceQSA * iface );
  static int removeInterface( AQSharedObjectQSA );

protected:

  AQScriptEngineQSA * engine;
  static QHash<QObject *, AQObjectInterfaceQSA *> ifacesHash;
};

#endif /*AQOBJECTFACTORYQSA_H_*/
