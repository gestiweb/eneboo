/***************************************************************************
                            FLCompoundKey.h
                          -------------------
 begin                : Fri Feb 1 2002
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

#ifndef FLCOMPOUNDKEY_H
#define FLCOMPOUNDKEY_H

class FLFieldMetaData;

/**
Clase para definir claves compuestas.

Esta clase sirve para crear objetos que contienen
una lista con los campos que conforman una clave.
En la lista de campos se guardan los metadatos de estos,
es decir objetos FLFieldMetaData.

@author InfoSiAL S.L.
*/
class FLCompoundKey
{
public:

  /**
  constructor
  */
  FLCompoundKey();

  /**
  destructor
  */
  ~FLCompoundKey();

  /**
  Añade la descripción de un campo a lista  de campos que componen la clave.

  @param f Objeto FLFieldMetaData con la descripción del campo a añadir
  */
  void addFieldMD(const FLFieldMetaData *f);

  /**
  Obtiene si una campo pertenece a la clave compuesta.

  @param fN Nombre del campo del que se desea saber si pertenece o no a la clave compuesta
  @return TRUE si el campo forma parte de la clave compuesta, FALSE en caso contrario
  */
  bool hasField(const QString &fN) const;

  /**
  Para obtener la lista de definiciones de campos que componen la clave.

  @return Objeto con la lista de deficiones de campos de la clave compuesta
  */
  FLTableMetaData::FLFieldMetaDataList *fieldList() const {
    return fieldList_;
  }

private:

  /**
  Lista de con los metadatos de los campos que componen la clave
  */
  FLTableMetaData::FLFieldMetaDataList *fieldList_;
};

#endif
