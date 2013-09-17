/***************************************************************************
                        AQGroupByQuery.h
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

#ifndef AQGROUPBYQUERY_H_
#define AQGROUPBYQUERY_H_

#include <QString>

class AQGroupByQueryPrivate;

/**
Representa un nivel de agrupamiento de una consulta.
 
Esta clase sirve para adaptar el contenido de una consulta a los niveles de
agrupamiento que requiere la librería Kugar.
 
@author InfoSiAL S.L.
*/
class AQGroupByQuery
{

public:

  /**
  constructor.

  @param l Nivel de agrupamiento
  @param f Nombre del campo por el que se agrupa
  */
  AQGroupByQuery( const int l, const QString & f );

  /**
  destructor
  */
  ~AQGroupByQuery();

  /**
  Obtiene el nivel de agrupamiento.

  @return Nivel de agrupamiento
  */
  int level() const;

  /**
  Obtiene el nombre del campo por el que se agrupa.

  @return Nombre del campo
  */
  QString field() const;

private:

  /**
  Privado
  */
  AQGroupByQueryPrivate * d;
};

#endif /*AQGROUPBYQUERY_H_*/
