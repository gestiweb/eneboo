/***************************************************************************
AQVar.h
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

#ifndef AQVAR_H_
#define AQVAR_H_

#include <QString>
#include <QVariant>

/**
Clase usada para crear, leer y borrar valores globales
accesibles desde los scripts, en el contexto de una
base de datos.
 
Esta clase mantiene una tabla (aqvar) donde se guardan
los determinados que hacen las veces de variables globales 
de aplicación desde los sctipts
 
@author InfoSiAL S.L.
*/
class AQVar
{
public:

  /**
  Constructor

  @param connName Nombre de la conexion de la base de datos utilizada
                  como el contexto para las variables
  */
  AQVar( const QString & connName = "default" );

  /**
  Establece un valor en la tabla de variables de sesión

  @param  n  Nombre de la variable
  @param  v  Valor asociado
  @return Verdadero en caso de establecer el valor con éxito, falso en cualquier otro caso
  */
  bool set( const QString & n, const QVariant & v );

  /**
  Lee un valor en la tabla de variables de sesión

  @param  n       Nombre de la variable a leer
  @param  exists  Valor que indica si la variable está definida o no
  @return Valor asociado, en caso de leer el valor con éxito, falso en cualquier otro caso
  */
  QVariant get( const QString & n, bool * exists = 0 ) const;

  /**
  Borra la variable de sesión

  @param  n Nombre de la variable a borrar
  @return True si se borra correctamente, false en caso contrario
  */
  bool del( const QString & n );

  /**
  Borra todas las variables correspondientes a una sesión

  @return Verdadero si el borrado se completa con éxito, falso en otro caso
  */
  bool clean();

private:

  /**
  Uso interno
  */
  QString connName_;
};

#endif /*AQVAR_H_*/
