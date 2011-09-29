/***************************************************************************
                          FLParameterQuery.h
                          -------------------
 begin                : dom jun 23 2002
 copyright            : (C) 2002-2005 by InfoSiAL S.L.
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

#ifndef FLPARAMETERQUERY_H
#define FLPARAMETERQUERY_H

#include <qstring.h>
#include <qvariant.h>

/**
Representa objetos que encapsulan parámetros de una consulta  o un filtro

@author InfoSiAL S.L.
*/
class FL_EXPORT FLParameterQuery {

public:

  /**
  constructor.

  @param n Nombre del parámetro
  @param a Alias del parámetro
  @param t Tipo del parámetro
  */
  FLParameterQuery( const QString & n, const QString & a, const int t );

  /**
  destructor
  */
  ~FLParameterQuery();

  /**
  Obtiene el nombre del parámetro.

  @return Nombre del parámetro
  */
  const QString & name() const {
    return name_;
  }

  /**
  Obtiene el alias del parámetro.

  @return Alias del parámetro
  */
  const QString & alias() const {
    return alias_;
  }

  /**
  Obtiene el tipo del parámetro.

  @return El tipo del parámetro
  */
  const int type() const {
    return type_;
  }

  /**
  Obtiene el valor del parámetro.

  @return El valor del parámetro
  */
  const QVariant & value() const {
    return value_;
  }

  /**
  Establece el valor del parámetro.

  @param v Valor del parámetro
  */
  void setValue( const QVariant & v ) {
    value_ = v;
  }

private:

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

#endif
