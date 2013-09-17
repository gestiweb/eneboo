/***************************************************************************
AQMemCache.h
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

#ifndef AQMEMCACHE_H_
#define AQMEMCACHE_H_

#include <QString>

class AQMemCachePrivate;

/**
Caché interna en memoria
 
@author InfoSiAL S.L.
*/
class AQMemCache
{
public:

  /**
  constructor
  */
  AQMemCache();

  /**
  destructor
  */
  ~AQMemCache();

  /**
  Busca en caché según la clave indicada.

  Si hay acierto en el segundo parámetro se devuelve por referencia el contenido

  @param  key Clave de búsqueda
  @param  str De salida. El contenido encontrado.
  @return TRUE si hubo acierto
  */
  bool find( const QString & key, QString & str );

  /**
  Inserta en caché un contenido asignándole una clave

  @param  key Clave asignada
  @param  str Contenido
  @return TRUE si la inserción tubo éxito, FALSE en caso contrario
  */
  bool insert( const QString & key, const QString & str );

  /**
  Limpia completamente el contenido de la caché
  */
  void clear();

private:

  /**
  Privado
  */
  AQMemCachePrivate * d;
};

#endif /*AQMEMCACHE_H_*/
