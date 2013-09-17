/***************************************************************************
AQDiskCache.cpp
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

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QTextCodec>

#include "AQDiskCache.h"
#include "AQConfig.h"

#define AQ_ABSDIRPATH AQ_HOME + "/.aqcache"

bool AQDiskCache::find( const QString & key, QString & str, const char * codecName )
{
  QStringList pathList( key.split( "/", QString::SkipEmptyParts ) );
  QString key_;

  if ( !pathList.isEmpty() )
    key_ = pathList.takeLast();

  QString path( QDir::cleanPath( AQ_ABSDIRPATH + "/" + pathList.join( "/" ) ) );
  QDir d( path );

  if ( !d.exists() )
    d.mkpath( path );

  QString fileCache( path + "/" + key_ );
  QFile fi( fileCache );

  if ( !fi.exists() )
    return false;

  if ( fi.open( QIODevice::ReadOnly ) ) {
    QTextStream t( &fi );

    if ( codecName )
      t.setCodec( QTextCodec::codecForName( codecName ) );

    str = t.readAll();
    fi.close();

    return true;
  } else
    return false;
}

bool AQDiskCache::find( const QString & key, QByteArray & str )
{
  QStringList pathList( key.split( "/", QString::SkipEmptyParts ) );
  QString key_;

  if ( !pathList.isEmpty() )
    key_ = pathList.takeLast();

  QString path( QDir::cleanPath( AQ_ABSDIRPATH + "/" + pathList.join( "/" ) ) );
  QDir d( path );

  if ( !d.exists() )
    d.mkpath( path );

  QString fileCache( path + "/" + key_ );

  QFile fi( fileCache );
  if ( !fi.exists() )
    return false;

  if ( fi.open( QIODevice::ReadOnly ) ) {
    QDataStream d( &fi );
    d >> str;
    fi.close();
    return true;
  } else
    return false;
}

bool AQDiskCache::find( const QString & key )
{
  QStringList pathList( key.split( "/", QString::SkipEmptyParts ) );
  QString key_;

  if ( !pathList.isEmpty() )
    key_ = pathList.takeLast();

  QString path( QDir::cleanPath( AQ_ABSDIRPATH + "/" + pathList.join( "/" ) ) );
  QDir d( path );

  if ( !d.exists() )
    d.mkpath( path );

  return QFile( path + "/" + key_ ).exists();
}

bool AQDiskCache::insert( const QString & key, const QByteArray & str )
{
  QStringList pathList( key.split( "/", QString::SkipEmptyParts ) );
  QString key_;

  if ( !pathList.isEmpty() )
    key_ = pathList.takeLast();

  QString path( QDir::cleanPath( AQ_ABSDIRPATH + "/" + pathList.join( "/" ) ) );
  QString fileCache = path + "/" + key_;
  QFile fi( fileCache );
  QDir d( path );

  if ( !d.exists() )
    d.mkpath( path );
  else
    if ( fi.exists() )
      return true;
  if ( !str.isEmpty() ) {
    if ( fi.open( QIODevice::WriteOnly ) ) {
      QDataStream d( &fi );
      d << str;
      fi.close();
      return true;
    }
    return false;
  }
  return false;
}

bool AQDiskCache::insert( const QString & key, const QString & str, const char * codecName )
{
  QStringList pathList( key.split( "/", QString::SkipEmptyParts ) );
  QString key_;

  if ( !pathList.isEmpty() )
    key_ = pathList.takeLast();

  QString path( QDir::cleanPath( AQ_ABSDIRPATH + "/" + pathList.join( "/" ) ) );
  QString fileCache = path + "/" + key_;
  QFile fi( fileCache );
  QDir d( path );

  if ( !d.exists() )
    d.mkpath( path );
  else
    if ( fi.exists() )
      return true;
  if ( !str.isEmpty() ) {
    if ( fi.open( QIODevice::WriteOnly ) ) {
      QTextStream t( &fi );
      if ( codecName )
        t.setCodec( QTextCodec::codecForName( codecName ) );
      t << str;
      fi.close();
      return true;
    }
    return false;
  }
  return false;
}

void AQDiskCache::clear( const QString & path )
{
  QString dirCache( QDir::cleanPath( AQ_ABSDIRPATH + "/" + path ) );
  QDir d( dirCache );

  if ( d.exists() ) {
    QString filter( "*;*.*" );
    QStringList lst( d.entryList( filter.split( ";" ) ) );
    foreach ( QString str, lst ) {
      d.remove( dirCache + "/" + str );
    }
  }
}

QString AQDiskCache::absoluteFilePath( const QString & key )
{
  QStringList pathList( key.split( "/", QString::SkipEmptyParts ) );
  QString key_;

  if ( !pathList.isEmpty() )
    key_ = pathList.takeLast();

  QString fileCache( QDir::cleanPath( AQ_ABSDIRPATH + "/" + pathList.join( "/" ) + "/" + key_ ) );

  if ( !QFile::exists( fileCache ) )
    return QString();

  return fileCache;
}

QString AQDiskCache::absoluteDirPath()
{
  return AQ_ABSDIRPATH;
}
