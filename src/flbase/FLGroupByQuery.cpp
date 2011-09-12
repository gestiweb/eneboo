/***************************************************************************
                          FLGroupByQuery.cpp
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

#include "FLGroupByQuery.h"

FLGroupByQuery::FLGroupByQuery(const int l, const QString &f) : level_(l),
  field_(f) {}

FLGroupByQuery::~FLGroupByQuery() {}
