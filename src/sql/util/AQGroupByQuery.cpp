/***************************************************************************
                     AQGroupByQuery.cpp
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

#include "AQGroupByQuery.h"

class AQGroupByQueryPrivate
{

public:

  AQGroupByQueryPrivate()
  {}
  ~AQGroupByQueryPrivate()
  {}

  /**
  Nivel de agrupamiento
  */
  int level_;

  /**
  Nombre del campo por el que se agrupa
  */
  QString field_;
};

AQGroupByQuery::AQGroupByQuery( const int l, const QString & f )
{
  d = new AQGroupByQueryPrivate();
  d->level_ = l;
  d->field_ = f;
}

AQGroupByQuery::~AQGroupByQuery()
{
  delete d;
}

int AQGroupByQuery::level() const
{
  return d->level_;
}

QString AQGroupByQuery::field() const
{
  return d->field_;
}
