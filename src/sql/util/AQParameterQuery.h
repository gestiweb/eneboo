/***************************************************************************
                        AQParameterQuery.h
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

#ifndef AQPARAMETERQUERY_H_
#define AQPARAMETERQUERY_H_

#include <QString>
#include <QVariant>

class AQParameterQueryPrivate;

/**
Representa objetos que encapsulan parámetros de una consulta  o un filtro
 
@author InfoSiAL S.L.
*/
class AQParameterQuery
{

public:

  /**
  constructor.

  @param n Nombre del parámetro
  @param a Alias del parámetro
  @param t Tipo del parámetro
  */
  AQParameterQuery( const QString & n, const QString & a, int t );

  /**
  destructor
  */
  ~AQParameterQuery();

  /**
  Obtiene el nombre del parámetro.

  @return Nombre del parámetro
  */
  QString name() const;

  /**
  Obtiene el alias del parámetro.

  @return Alias del parámetro
  */
  QString alias() const;

  /**
  Obtiene el tipo del parámetro.

  @return El tipo del parámetro
  */
  int type() const;

  /**
  Obtiene el valor del parámetro.

  @return El valor del parámetro
  */
  QVariant value() const;

  /**
  Establece el valor del parámetro.

  @param v Valor del parámetro
  */
  void setValue( const QVariant & v ) const;

private:

  /**
  Privado
  */
  AQParameterQueryPrivate * d;
};

#endif /*AQPARAMETERQUERY_H_*/
