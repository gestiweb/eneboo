/***************************************************************************
AQCompKeyMD.h
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

#ifndef AQCOMPKEYMD_H_
#define AQCOMPKEYMD_H_

#include <AQCore/AQCore.h>

class AQCompKeyMDPrivate;

/**
Para definir claves compuestas.
 
Esta clase sirve para crear objetos que contienen
una lista con los campos que conforman una clave.
En la lista de campos se guardan los metadatos de estos,
es decir objetos AQFieldMD.
 
@author InfoSiAL S.L.
*/
class AQCompKeyMD
{

public:

  /**
  constructor
  */
  AQCompKeyMD();

  /**
  destructor
  */
  ~AQCompKeyMD();

  /**
  Añade la descripción de un campo a lista  de campos que componen la clave.

  @param  f Objeto AQFieldMD con la descripción del campo a añadir. Es toma en propiedad
            por lo que no debe ser borrado
  */
  void addFieldMD( AQFieldMD * f );

  /**
  Obtiene si una campo pertenece a la clave compuesta.

  @param  fN  Nombre del campo del que se desea saber si pertenece o no a la clave compuesta
  @return TRUE si el campo forma parte de la clave compuesta, FALSE en caso contrario
  */
  bool hasField( const QString & fN ) const;

  /**
  Para obtener la lista de definiciones de campos que componen la clave.

  @return Objeto con la lista de deficiones de campos de la clave compuesta
  */
  AQFieldMDHash fieldList() const;

private:

  /**
  Privado
  */
  AQCompKeyMDPrivate * d;
};

#endif /* AQCOMPKEYMD_H_*/
