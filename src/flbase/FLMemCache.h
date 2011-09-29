/***************************************************************************
                            FLMemCache.h
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

#ifndef FLMEMCACHE_H
#define FLMEMCACHE_H

#include <qstring.h>
#include <qcache.h>
#include <qobject.h>

class FLMemCache
{
public:

  static QString *find(const QString &key) {
    return str_cache->find(key);
  }

  static bool insert(const QString &key, const QString &str) {
    QString *s = new QString(str);
    if (!str_cache->insert(key, s)){
      delete s;
      return false;
    }
    return true;
  }

  static void init();
  static void clear();

private:

  static QCache<QString> *str_cache;
};

#endif
