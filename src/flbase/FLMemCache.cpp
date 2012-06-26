/***************************************************************************
                           FLMemCache.cpp
                         -------------------
begin                : vie oct 10 2003
copyright            : (C) 2003-2005 by InfoSiAL, S.L.
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

#include "FLMemCache.h"

QCache<QString> *FLMemCache::str_cache = 0;

void FLMemCache::init()
{
  if (!str_cache) {
    str_cache = new QCache<QString>(503, 1009);
    str_cache->setAutoDelete(true);
  }
}

void FLMemCache::clear()
{
  if (!str_cache || str_cache->isEmpty())
    return;
#ifdef FL_DEBUG
  //  qWarning( "*************************************************" );
  //  qWarning( "FLMemCache" );
  //  qWarning( "*************************************************" );
  //  str_cache->statistics();
  //  qWarning( "*************************************************" );
  //  qWarning( "FLMemCache" );
  //  qWarning( "*************************************************" );
#endif
  str_cache->clear();
}
