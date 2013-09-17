/***************************************************************************
                         AQParameterQuery.cpp
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

#include "AQParameterQuery.h"

class AQParameterQueryPrivate
{

public:

  AQParameterQueryPrivate( const QString & n, const QString & a, int t );

  /**
  Nombre del parámetro
  */
  QString name_;

  /**
  Alias del parámetro
  */
  QString alias_;

  /**
  Tipo de datos del parámetros
  */
  int type_;

  /**
  Valor del parámetro
  */
  QVariant value_;
};

AQParameterQueryPrivate::AQParameterQueryPrivate( const QString & n, const QString & a, int t ) :
    name_( n ), alias_( a ), type_( t )
{}

AQParameterQuery::AQParameterQuery( const QString & n, const QString & a, int t )
{
  d = new AQParameterQueryPrivate( n, a, t );
}

AQParameterQuery::~AQParameterQuery()
{
  delete d;
}

QString AQParameterQuery::name() const
{
  return d->name_;
}

QString AQParameterQuery::alias() const
{
  return d->alias_;
}

int AQParameterQuery::type() const
{
  return d->type_;
}

QVariant AQParameterQuery::value() const
{
  return d->value_;
}

void AQParameterQuery::setValue( const QVariant & v ) const
{
  d->value_ = v;
}
