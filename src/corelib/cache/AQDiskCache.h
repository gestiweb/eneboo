/***************************************************************************
AQDiskCache.h
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

#ifndef AQDISKCACHE_H_
#define AQDISKCACHE_H_

#include <QString>

/**
Caché interna en disco
 
@author InfoSiAL S.L.
*/
class AQDiskCache
{
public:

  /**
  Busca en caché según la clave indicada.

  Si hay acierto en el segundo parámetro se devuelve por referencia el contenido

  @param  key       Clave de búsqueda
  @param  str       De salida. El contenido encontrado.
  @param  codecName El contenido lo codifica con 'codecName'
  @return TRUE si hubo acierto
  */
  bool find( const QString & key, QString & str, const char * codecName = 0 );

  /**
  Esencialmente hace lo mismo que el anterior método pero sobre un array
  de bytes
  */
  bool find( const QString & key, QByteArray & str );

  /**
  Esencialmente hace lo mismo que el anterior método pero no obtiene el contenido
  sólo indica si existe la clave en caché
  */
  bool find( const QString & key );

  /**
  Inserta en caché un contenido asignándole una clave

  @param  key       Clave asignada
  @param  str       Contenido
  @param  codecName El contenido lo codifica con 'codecName'
  @return TRUE si la inserción tubo éxito, FALSE en caso contrario
  */
  bool insert( const QString & key, const QString & str, const char * codecName = 0 );

  /**
  Esencialmente hace lo mismo que el anterior método pero sobre un array
  de bytes
  */
  bool insert( const QString & key, const QByteArray & str );

  /**
  Limpia carpetas de la caché
  */
  void clear( const QString & path = QString() );

  /**
  Obtiene la ruta absoluta del fichero que almacena el contenido referenciado por la clave indicada

  @param  key Clave que referencia el contenido
  @return Ruta absoluta del fichero. Si no existe un fichero asociado a la clave indicada devuelve una
          cadena vacía
  */
  QString absoluteFilePath( const QString & key );

  /**
  @return El directorio principal utilizado por la caché para almacenar los archivos
  */
  QString absoluteDirPath();
};

#endif /*AQDISKCACHE_H_*/
