/***************************************************************************
AQMemCache.cpp
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

#include <QCache>

#include "AQMemCache.h"

class AQMemCachePrivate
{
public:

  QCache<QString, QString> cache_;
};

AQMemCache::AQMemCache()
{
  d = new AQMemCachePrivate();
}

AQMemCache::~AQMemCache()
{
  delete d;
}

bool AQMemCache::find( const QString & key, QString & str )
{
  QString * s = d->cache_.object( key );
  if ( s )
    str = *s;
  return !!s;
}

bool AQMemCache::insert( const QString & key, const QString & str )
{
  return d->cache_.insert( key, new QString( str ) );
}

void AQMemCache::clear()
{
  d->cache_.clear();
}
