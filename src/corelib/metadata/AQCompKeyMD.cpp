/***************************************************************************
AQCompKeyMD.cpp
-------------------
begin                : 06/08/2007
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

#include "AQCompKeyMD.h"
#include "AQFieldMD.h"

class AQCompKeyMDPrivate
{
public:

  AQCompKeyMDPrivate();

  /**
  Lista de con los metadatos de los campos que componen la clave
  */
  AQFieldMDHash fieldList_;
};

AQCompKeyMDPrivate::AQCompKeyMDPrivate()
{}

AQCompKeyMD::AQCompKeyMD() : d( 0 )
{}

AQCompKeyMD::~AQCompKeyMD()
{
  delete d;
}

void AQCompKeyMD::addFieldMD( AQFieldMD * f )
{
  if ( !d )
    d = new AQCompKeyMDPrivate();
  if ( f )
    d->fieldList_.insert( f->name().toLower(), f );
}

bool AQCompKeyMD::hasField( const QString & fN ) const
{
  if ( !d )
    return false;

  return d->fieldList_.contains( fN );
}

AQFieldMDHash AQCompKeyMD::fieldList() const
{
  if ( !d )
    return AQFieldMDHash();

  return d->fieldList_;
}
