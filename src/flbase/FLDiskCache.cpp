/***************************************************************************
                           FLDiskCache.cpp
                         -------------------
begin                : lun ago 02 2004
copyright            : (C) 2004-2005 by InfoSiAL, S.L.
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

#include "FLDiskCache.h"
#include "FLApplication.h"
#include "FLSqlDatabase.h"
#include "AQConfig.h"

AQ_EXPORT QString FLDiskCache::absoluteDirPath;

AQ_EXPORT bool aqDiskCacheFind(const QString &key, QString &str)
{
  return FLDiskCache::find(key, str);
}
AQ_EXPORT bool aqDiskCacheInsert(const QString &key, const QString &str)
{
  return FLDiskCache::insert(key, str);
}

bool FLDiskCache::find(const QString &key, QString &str)
{
  QString fileCache(AQ_DISKCACHE_DIRPATH + '/' + key);
  QFile fi(fileCache);
  if (!fi.open(IO_ReadOnly))
    return false;
  QTextStream t(&fi);
  str = t.read();
  fi.close();
  return true;
}

bool FLDiskCache::find(const QString &key, QByteArray &ba)
{
  QString fileCache(AQ_DISKCACHE_DIRPATH + '/' + key +
                    QString::fromLatin1("-BIN"));
  QFile fi(fileCache);
  if (!fi.open(IO_ReadOnly))
    return false;
  QDataStream dat(&fi);
  dat >> ba;
  fi.close();
  return true;
}

bool FLDiskCache::insert(const QString &key, const QString &str)
{
  QString fileCache(AQ_DISKCACHE_DIRPATH + '/' + key);
  QFile fi(fileCache);
  QDir d(AQ_DISKCACHE_DIRPATH);
  if (!d.exists())
    d.mkdir(AQ_DISKCACHE_DIRPATH);
  else if (fi.exists())
    return true;
  if (!str.isEmpty()) {
    if (fi.open(IO_WriteOnly)) {
      QTextStream t(&fi);
      t << str;
      fi.close();
      return true;
    }
  }
  return false;
}

bool FLDiskCache::insert(const QString &key, const QByteArray &ba)
{
  QString fileCache(AQ_DISKCACHE_DIRPATH + '/' + key +
                    QString::fromLatin1("-BIN"));
  QFile fi(fileCache);
  QDir d(AQ_DISKCACHE_DIRPATH);
  if (!d.exists())
    d.mkdir(AQ_DISKCACHE_DIRPATH);
  else if (fi.exists())
    return true;
  if (!ba.isEmpty()) {
    if (fi.open(IO_WriteOnly)) {
      QDataStream dat(&fi);
      dat << ba;
      fi.close();
      return true;
    }
  }
  return false;
}

void FLDiskCache::clear()
{
  QDir d(AQ_DISKCACHE_DIRPATH);
  if (d.exists()) {
    QStringList lst = d.entryList("*; *.*");
    for (QStringList::Iterator it = lst.begin(); it != lst.end(); ++it)
      d.remove(AQ_DISKCACHE_DIRPATH + '/' + (*it));
  }
#ifndef QSDEBUGGER
  QTextCodec *codec = QTextCodec::codecForLocale();
  QString localEncode(codec ? codec->mimeName() : "");
  QString path(AQ_USRHOME + "/.eneboocache/" + localEncode);
  QDir d2(path);
  if (d2.exists()) {
    QStringList lst = d2.entryList("sys*");
    for (QStringList::Iterator it = lst.begin(); it != lst.end(); ++it)
      d2.remove(path + '/' + (*it));
  }
#endif
}

QString FLDiskCache::absoluteFilePath(const QString &key)
{
  QString fileCache(AQ_DISKCACHE_DIRPATH + '/' + key);
  if (!QFile::exists(fileCache))
    return QString::null;
  return fileCache;
}

static inline void aqSetAndCreateDirPath(const QString &path)
{
  AQ_DISKCACHE_DIRPATH = path;
  QDir d(AQ_DISKCACHE_DIRPATH);
  if (!d.exists())
    d.mkdir(AQ_DISKCACHE_DIRPATH);
}

void FLDiskCache::init(FLApplication *app)
{
  QTextCodec *codec = QTextCodec::codecForLocale();
  QString localEncode(codec ? codec->mimeName() : "");
  if (!app) {
    aqSetAndCreateDirPath(AQ_USRHOME + "/.eneboocache");
    if (!localEncode.isEmpty())
      aqSetAndCreateDirPath(AQ_DISKCACHE_DIRPATH + '/' + localEncode);
  } else {
    if ( app->db()->driverName() != "FLsqlite")
    aqSetAndCreateDirPath(
      AQ_USRHOME + "/.eneboocache/" +
      app->db()->database()
    );
    else
    {
    // --> Aulla : Reconstruimos un nombre válido para crear el directorio de la caché cuando usamos el driver SQLite
    QString DBName = app->db()->database();
    DBName.replace(AQ_DISKCACHE_DIRPATH,""); //Limpiamos el path
    DBName.replace(".s3db","");//Limpiamos la extensión
    aqSetAndCreateDirPath(
      AQ_USRHOME + "/.eneboocache/" +
      DBName
    );
    // <-- Aulla:
    }
    
    
    if (!localEncode.isEmpty())
      aqSetAndCreateDirPath(AQ_DISKCACHE_DIRPATH + '/' + localEncode);

    QDir d(AQ_DISKCACHE_DIRPATH);
    if (d.exists()) {
      QStringList lst = d.entryList("*.*", QDir::Files);
      for (QStringList::Iterator it = lst.begin(); it != lst.end(); ++it) {
        QString item(*it);
        if (!item.endsWith(".qm"))
          d.remove(AQ_DISKCACHE_DIRPATH + '/' + item);
      }
    }
  }
}
