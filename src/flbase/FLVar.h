/***************************************************************************
                                FLVar.h
                          -------------------
 begin                : Thu Jul 29 2004
 copyright            : (C) 2004-2005 by InfoSiAL S.L.,
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

#ifndef FLVAR_H
#define FLVAR_H

#include <qstring.h>

/**
Clase usada para crear, leer y borrar valores globales accesibles desde los scripts.

Esta clase mantiene una tabla (flvar) donde se guardan los determinados que hacen las veces
de variables globales de aplicación desde los sctipts

@author InfoSiAL S.L.
*/
class AQ_EXPORT FLVar
{

public:

  /**
  Establece un valor en la tabla de variables de sesión

  @param n Nombre de la variable
  @param v Valor asociado
  @return Verdadero en caso de establecer el valor con éxito, falso en cualquier otro caso
  */
  static bool set(const QString &n, const QVariant &v);

  /**
  Lee un valor en la tabla de variables de sesión

  @param n Nombre de la variable a leer
  @return Valor asociado, en caso de leer el valor con éxito, falso en cualquier otro caso
  */
  static QVariant get(const QString &n);

  /**
  Borra la variable de sesión

  @param n Nombre de la variable a borrar
  @return True si se borra correctamente, false en caso contrario
  */
  static bool del(const QString &n);

  /**
  Borra todas las variables correspondientes a una sesión

  @return Verdadero si el borrado se completa con éxito, falso en otro caso
  */
  static bool clean();
};

#endif
