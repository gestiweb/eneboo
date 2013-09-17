/***************************************************************************
AQAccessControl.cpp
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
#include <QStringList>

#include "AQAccessControl.h"
#include "AQAccessControl_p.h"

AQAccessControl::AQAccessControl() :
    d_( new AQAccessControlPrivate )
{}

AQAccessControl::~AQAccessControl()
{
  delete d_;
}


bool AQAccessControl::isNull() const
{
  return d_->perm_.isEmpty() && d_->acosPerms_.isEmpty();
}

void AQAccessControl::clear()
{
  d_->clear();
}

QString AQAccessControl::name() const
{
  return d_->name_;
}

QString AQAccessControl::user() const
{
  return d_->user_;
}

QString AQAccessControl::perm() const
{
  return d_->perm_;
}

void AQAccessControl::setName( const QString & n )
{
  d_->name_ = n;
}

void AQAccessControl::setUser( const QString & u )
{
  d_->user_ = u;
}

void AQAccessControl::setPerm( const QString & p )
{
  d_->perm_ = p;
}

void AQAccessControl::set( QDomElement * e )
{
  Q_ASSERT( e );

  d_->acosPerms_.clear();
  d_->perm_ = e->attribute( "perm" );
  QDomNode no = e->firstChild();

  while ( !no.isNull() ) {
    QDomElement e = no.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == "name" ) {
        d_->name_ = e.text();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "user" ) {
        d_->user_ = e.text();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "aco" ) {
        d_->acosPerms_.insert( e.text(), e.attribute( "perm" ) );
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }
}

void AQAccessControl::get( QDomDocument * d ) const
{
  if ( this->type().isEmpty() )
    return ;

  Q_ASSERT( d );

  QDomElement root = d->firstChild().toElement();
  QDomElement e = d->createElement( this->type() );
  e.setAttribute( "perm", d_->perm_ );
  root.appendChild( e );

  QDomElement name = d->createElement( "name" );
  e.appendChild( name );
  QDomText n = d->createTextNode( d_->name_ );
  name.appendChild( n );

  QDomElement user = d->createElement( "user" );
  e.appendChild( user );
  QDomText u = d->createTextNode( d_->user_ );
  user.appendChild( u );

  for ( AQStringHash::const_iterator it = d_->acosPerms_.begin(); it != d_->acosPerms_.end(); ++it ) {
    QDomElement aco = d->createElement( "aco" );
    aco.setAttribute( "perm", it.value() );
    e.appendChild( aco );
    QDomText t = d->createTextNode( it.key() );
    aco.appendChild( t );
  }
}

void AQAccessControl::setAcos( QStringList & acos )
{
  d_->acosPerms_.clear();
  QString nameAcos;

  for ( QStringList::const_iterator it = acos.begin(); it != acos.end(); ++it ) {
    nameAcos = *it;
    ++it;
    d_->acosPerms_.insert( nameAcos, *it );
  }
}

QStringList AQAccessControl::getAcos() const
{
  QStringList acos;

  for ( AQStringHash::const_iterator it = d_->acosPerms_.begin(); it != d_->acosPerms_.end(); ++it ) {
    acos.append( it.key() );
    acos.append( it.value() );
  }

  return acos;
}
