/***************************************************************************
AQAccessControlLists.cpp
-------------------
begin                : 09/08/2007
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

#include <QDomDocument>
#include <QDomElement>

#include <AQCore/AQCoreUtil.h>

#include "AQAccessControlLists.h"
#include "AQAccessControl.h"
#include "AQAccessControlFactory.h"

AQAccessControlLists::~AQAccessControlLists()
{
  clear();
}

void AQAccessControlLists::clear()
{
  while ( !accessControlList_.isEmpty() ) {
    AQAccessControl * ac = accessControlList_.begin().value();
    accessControlList_.erase( accessControlList_.begin() );
    delete ac;
  }
}

void AQAccessControlLists::init( const QString & aclXml )
{
  QDomDocument doc( "ACL" );

  if ( !AQCoreUtil::domDocumentSetContent( doc, aclXml ) ) {
    aqWarn( "AQAccessControlLists : " +
            AQ_TR( "AQCore", "Lista de control de acceso vacia o errónea." ) );
    return ;
  }

  clear();

  QDomElement docElem = doc.documentElement();
  QDomNode no = docElem.firstChild();

  while ( !no.isNull() ) {
    QDomElement e = no.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == "name" ) {
        name_ = e.text();
        no = no.nextSibling();
        continue;
      }

      AQAccessControl * ac = AQAccessControlFactory::create( e.tagName() );
      if ( ac && !ac->isNull() ) {
        ac->set( &e );
        accessControlList_.insert( ac->type() + "::" + ac->name() + "::" + ac->user(), ac );
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }
}

void AQAccessControlLists::process( const QString & user, QObject * obj ) const
{
  Q_ASSERT( obj );

  if ( accessControlList_.isEmpty() )
    return ;

  QString type( AQAccessControlFactory::type( obj ) );
  QString name( obj->objectName() );

  if ( type.isEmpty() || name.isEmpty() || user.isEmpty() )
    return ;

  QString key( type + "::" + name + "::" + user );

  AQAccessControl * ac = accessControlList_.value( key, 0 );
  if ( ac && !ac->isNull() )
    ac->processObject( obj );
}
