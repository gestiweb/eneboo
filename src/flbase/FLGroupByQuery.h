/***************************************************************************
                            FLGroupByQuery.h
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

#ifndef FLGROUPBYQUERY_H
#define FLGROUPBYQUERY_H

#include <qstring.h>

/**
Representa un nivel de agrupamiento de una consulta.

Esta clase sirve para adaptar el contenido de una consulta a los niveles de
agrupamiento que requiere la librería Kugar.

@author InfoSiAL S.L.
*/
class FLGroupByQuery
{
public:

  /**
  constructor.

  @param l Nivel de agrupamiento
  @param f Nombre del campo por el que se agrupa
  */
  FLGroupByQuery( const int l, const QString & f );

  /**
  destructor
  */
  ~FLGroupByQuery();

  /**
  Obtiene el nivel de agrupamiento.

  @return Nivel de agrupamiento
  */
  const int level() const {
    return level_;
  }

  /**
  Obtiene el nombre del campo por el que se agrupa.

  @return Nombre del campo
  */
  const QString & field() const {
    return field_;
  }

private:

  /**
  Nivel de agrupamiento
  */
  int level_;

  /**
  Nombre del campo por el que se agrupa
  */
  QString field_;
};

#endif
