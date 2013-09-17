/***************************************************************************
AQRelationMD.cpp
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

#include "AQRelationMD.h"

class AQRelationMDPrivate
{

public:

  AQRelationMDPrivate( const QString & fT, const QString & fF, int rC,
                       bool dC, bool uC, bool cI );


  /**
  Nombre del campo a relacionar
  */
  QString field_;

  /**
  Nombre de la tabla foránea a relacionar
  */
  QString foreignTable_;

  /**
  Nombre del campo foráneo relacionado
  */
  QString foreignField_;

  /**
  Cardinalidad de la relación
  */
  int cardinality_;

  /**
  Indica si los borrados serán en cascada, en relaciones M1
  */
  bool deleteCascade_;

  /**
  Indica si las modificaciones serán en cascada, en relaciones M1
  */
  bool updateCascade_;

  /**
  Indica si se deben aplicar la reglas de integridad en esta relación
  */
  bool checkIn_;
};
AQRelationMDPrivate::AQRelationMDPrivate( const QString & fT, const QString & fF, int rC, bool dC, bool uC, bool cI ) :
    foreignTable_( fT.toLower() ),
    foreignField_( fF.toLower() ),
    cardinality_( rC ),
    deleteCascade_( dC ),
    updateCascade_( uC ),
    checkIn_( cI )
{}

AQRelationMD::AQRelationMD( const QString & fT, const QString & fF, int rC, bool dC, bool uC, bool cI )
{
  d = new AQRelationMDPrivate( fT, fF, rC, dC, uC, cI );
}

AQRelationMD::~AQRelationMD()
{
  delete d;
}

void AQRelationMD::setField( const QString & fN ) const
{
  d->field_ = fN.toLower();
}

QString AQRelationMD::field() const
{
  return d->field_;
}

QString AQRelationMD::foreignTable() const
{
  return d->foreignTable_;
}

QString AQRelationMD::foreignField() const
{
  return d->foreignField_;
}

int AQRelationMD::cardinality() const
{
  return d->cardinality_;
}

bool AQRelationMD::deleteCascade() const
{
  return ( d->deleteCascade_ && d->cardinality_ == AQCore::RELATION_M1 );
}

bool AQRelationMD::updateCascade() const
{
  return ( d->updateCascade_ && d->cardinality_ == AQCore::RELATION_M1 );
}

bool AQRelationMD::checkIn() const
{
  return d->checkIn_;
}
