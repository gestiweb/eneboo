/***************************************************************************
AQSqlDir.cpp
-------------------
begin                : 02/09/2007
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

#include <QBuffer>
#include <QDir>
#include <QTextStream>

#include <AQCore/AQCore.h>
#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQDiskCache.h>

#include "AQSqlDir.h"
#include "AQSqlFileSystem.h"
#include "AQSqlNodeInfo.h"

class AQSqlDirPrivate
{
public:

  AQSqlDirPrivate();
  ~AQSqlDirPrivate();

  /**
  Auxiliares
  */
  bool cd( QStringList & pathList ) const;
  bool cp( const AQSqlNodeInfo & srcInfo, qlonglong newParentNode,
           const QString & newName = QString() );
  bool is_X_AncestorOf_Y_( const AQSqlNodeInfo & _X_, const AQSqlNodeInfo & _Y_ ) const;
  bool rm( const QString & name, bool recursive = false, bool trans = true );


  AQSqlFileSystem * fs;
  QString prefixCache;
};

AQSqlDirPrivate::AQSqlDirPrivate() :
    fs( 0 )
{}

AQSqlDirPrivate::~AQSqlDirPrivate()
{
  delete fs;
}

bool AQSqlDirPrivate::is_X_AncestorOf_Y_( const AQSqlNodeInfo & _X_, const AQSqlNodeInfo & _Y_ ) const
{
  if ( !_X_.flag.testFlag( AQSql::Dir ) )
    return false;

  if ( _X_.idNode() == _Y_.idNode() || _X_.idNode() == _Y_.idParentNode )
    return true;

  if ( _Y_.idNode() == fs->idRootNode() || _X_.idParentNode == _Y_.idParentNode )
    return false;

  qlonglong pwd = fs->idDirNode();
  bool ret = false;

  fs->setIdDirNode( _Y_.idParentNode );
  ret = is_X_AncestorOf_Y_( _X_, fs->nodeDirInfo() );
  fs->setIdDirNode( pwd );

  return ret;
}

AQSqlDir::AQSqlDir( AQSqlDatabase db, const QString & path )
{
  d = new AQSqlDirPrivate();
  d->fs = new AQSqlFileSystem();
  d->prefixCache = QLatin1String( "/sqlfilesystem/" ) + db.database();
  d->fs->init( db );
  d->fs->setIdDirNode( d->fs->idRootNode() );
  cd( path.isEmpty() ? "/" : path );
}

AQSqlDir::~AQSqlDir()
{
  delete d;
}

bool AQSqlDir::isReady() const
{
  return d->fs->isReady();
}

bool AQSqlDirPrivate::cd( QStringList & pathList ) const
{
  if ( pathList.isEmpty() )
    return fs->isDirNodeValid();
  return ( fs->setIdDirNodeByName( pathList.takeFirst() ) ? cd( pathList ) : false );
}

bool AQSqlDir::cd( const QString & dirName ) const
{
  if ( dirName.isEmpty() )
    return d->fs->isDirNodeValid();

  QStringList pathList( dirName.split( "/", QString::SkipEmptyParts ) );
  qlonglong pwd = d->fs->idDirNode();

  if ( dirName.startsWith( "/" ) )
    d->fs->setIdDirNode( d->fs->idRootNode() );

  if ( d->cd( pathList ) )
    return true;

  d->fs->setIdDirNode( pwd );
  return false;
}

bool AQSqlDir::cdUp() const
{
  return cd( ".." );
}

bool AQSqlDir::exists( const QString & name ) const
{
  if ( name.isEmpty() )
    return d->fs->isDirNodeValid();

  QStringList pathList( name.split( "/", QString::SkipEmptyParts ) );
  QString name_;
  qlonglong pwd = d->fs->idDirNode();
  qlonglong pwdf = d->fs->idFileNode();
  bool ret = false;

  if ( !pathList.isEmpty() )
    name_ = pathList.takeLast();
  if ( name.startsWith( "/" ) )
    d->fs->setIdDirNode( d->fs->idRootNode() );
  ret = ( d->cd( pathList ) && ( d->fs->setIdFileNodeByName( name_ ) || d->fs->setIdDirNodeByName( name_ ) ) );

  d->fs->setIdDirNode( pwd );
  d->fs->setIdFileNode( pwdf );
  return ret;
}

bool AQSqlDir::mkdir( const QString & dirName ) const
{
  if ( !dirName.isEmpty() ) {
    QStringList pathList( dirName.split( "/", QString::SkipEmptyParts ) );
    QString dir;
    qlonglong pwd = d->fs->idDirNode();
    qlonglong pwdf = d->fs->idFileNode();
    bool ret = false;

    if ( !pathList.isEmpty() )
      dir = pathList.takeLast();
    if ( dirName.startsWith( "/" ) )
      d->fs->setIdDirNode( d->fs->idRootNode() );

    if ( d->cd( pathList ) && !d->fs->setIdFileNodeByName( dir ) && !d->fs->setIdDirNodeByName( dir ) ) {

      if ( d->fs->transaction() && d->fs->addDirNode() != -1 ) {
        AQSqlNodeInfo info( d->fs->nodeDirInfo() );
        info.name = dir;

        if ( d->fs->setNodeInfo( info ) )
          ret = true;
        if ( ret )
          d->fs->commit();
        else
          d->fs->rollback();
      }
    }

    d->fs->setIdDirNode( pwd );
    d->fs->setIdFileNode( pwdf );
    return ret;
  }

  return false;
}

bool AQSqlDir::mkfile( const QString & fileName, const QByteArray & content ) const
{
  if ( !fileName.isEmpty() ) {
    QStringList pathList( fileName.split( "/", QString::SkipEmptyParts ) );
    QString file;
    qlonglong pwd = d->fs->idDirNode();
    bool ret = false;

    if ( !pathList.isEmpty() )
      file = pathList.takeLast();
    if ( fileName.startsWith( "/" ) )
      d->fs->setIdDirNode( d->fs->idRootNode() );

    if ( d->cd( pathList ) && !d->fs->setIdFileNodeByName( file ) && !d->fs->setIdDirNodeByName( file ) ) {

      if ( d->fs->transaction() && d->fs->addFileNode() != -1 ) {
        AQSqlNodeInfo info( d->fs->nodeFileInfo() );
        info.name = file;
        info.flag |= AQSql::BinFile;

        if ( d->fs->setNodeInfo( info ) && d->fs->setData( content ) )
          ret = true;
        if ( ret )
          d->fs->commit();
        else
          d->fs->rollback();
      }
    }

    d->fs->setIdDirNode( pwd );
    return ret;
  }

  return false;
}

bool AQSqlDir::mkfile( const QString & fileName, const QString & content ) const
{
  if ( !fileName.isEmpty() ) {
    QStringList pathList( fileName.split( "/", QString::SkipEmptyParts ) );
    QString file;
    qlonglong pwd = d->fs->idDirNode();
    bool ret = false;

    if ( !pathList.isEmpty() )
      file = pathList.takeLast();
    if ( fileName.startsWith( "/" ) )
      d->fs->setIdDirNode( d->fs->idRootNode() );

    if ( d->cd( pathList ) && !d->fs->setIdFileNodeByName( file ) && !d->fs->setIdDirNodeByName( file ) ) {

      if ( d->fs->transaction() && d->fs->addFileNode() != -1 ) {
        AQSqlNodeInfo info( d->fs->nodeFileInfo() );
        info.name = file;
        info.flag |= AQSql::CharFile;

        if ( d->fs->setNodeInfo( info ) && d->fs->setData( content ) )
          ret = true;
        if ( ret )
          d->fs->commit();
        else
          d->fs->rollback();
      }
    }

    d->fs->setIdDirNode( pwd );
    return ret;
  }

  return false;
}

bool AQSqlDirPrivate::rm( const QString & name, bool recursive, bool trans )
{
  if ( !name.isEmpty() && name != "." && name != ".." ) {
    QStringList pathList( name.split( "/", QString::SkipEmptyParts ) );
    QString name_;
    qlonglong pwd = fs->idDirNode();
    bool ret = false;

    if ( !pathList.isEmpty() ) {
      name_ = pathList.takeLast();
      if ( name_ == "." || name_ == ".." )
        return false;
    }

    if ( name.startsWith( "/" ) )
      fs->setIdDirNode( fs->idRootNode() );

    if ( cd( pathList ) ) {
      if ( fs->setIdDirNodeByName( name_ ) ) {
        if ( recursive ) {
          qlonglong backup = fs->idDirNode();
          QStringList nameList( fs->nameList() );
          nameList.removeAt( nameList.indexOf( "." ) );
          nameList.removeAt( nameList.indexOf( ".." ) );

          while ( !nameList.isEmpty() )
            rm( nameList.takeLast(), true, false );

          fs->setIdDirNode( backup );

          if ( trans ) {
            ret = ( fs->transaction() && ( fs->delDirNode() != -1 ) );
            if ( ret )
              fs->commit();
            else
              fs->rollback();
          } else
            ret = ( fs->delDirNode() != -1 );
        }
      } else {
        if ( trans ) {
          ret = ( fs->transaction() && fs->setIdFileNodeByName( name_ ) && ( fs->delFileNode() != -1 ) );
          if ( ret )
            fs->commit();
          else
            fs->rollback();
        } else
          ret = ( fs->setIdFileNodeByName( name_ ) && ( fs->delFileNode() != -1 ) );
      }
    }

    fs->setIdDirNode( pwd );
    return ret;
  }

  return false;
}

bool AQSqlDir::rm( const QString & name, bool recursive )
{
  return d->rm( name, recursive );
}

bool AQSqlDir::mv( const QString & srcName, const QString & dstName ) const
{
  if ( !srcName.isEmpty() && !dstName.isEmpty() ) {
    QStringList srcPathList( srcName.split( "/", QString::SkipEmptyParts ) );
    QString src;
    AQSqlNodeInfo srcNodeInfo( d->fs->nodeVoidInfo() );
    qlonglong pwd = d->fs->idDirNode();

    if ( srcName.startsWith( "/" ) ) {
      d->fs->setIdDirNode( d->fs->idRootNode() );
      if ( srcPathList.isEmpty() )
        srcNodeInfo = d->fs->nodeDirInfo();
    }

    if ( !srcPathList.isEmpty() ) {
      src = srcPathList.takeLast();

      if ( d->cd( srcPathList ) ) {
        if ( d->fs->setIdDirNodeByName( src ) )
          srcNodeInfo = d->fs->nodeDirInfo();
        else
          if ( d->fs->setIdFileNodeByName( src ) )
            srcNodeInfo = d->fs->nodeFileInfo();
      }
    }

    d->fs->setIdDirNode( pwd );
    if ( !srcNodeInfo.isValid() )
      return false;

    QStringList dstPathList( dstName.split( "/", QString::SkipEmptyParts ) );
    QString dst;
    AQSqlNodeInfo dstNodeInfo( d->fs->nodeVoidInfo() );

    if ( dstName.startsWith( "/" ) ) {
      d->fs->setIdDirNode( d->fs->idRootNode() );
      if ( dstPathList.isEmpty() )
        dstNodeInfo = d->fs->nodeDirInfo();
    }

    if ( !dstPathList.isEmpty() ) {
      dst = dstPathList.takeLast();
      if ( d->cd( dstPathList ) ) {
        if ( d->fs->setIdDirNodeByName( dst ) ) {
          if ( !d->fs->setIdDirNodeByName( src ) && !d->fs->setIdFileNodeByName( src ) ) {
            dstNodeInfo = d->fs->nodeDirInfo();
            dst = QString();
          }
        } else
          if ( !d->fs->setIdFileNodeByName( dst ) )
            dstNodeInfo = d->fs->nodeDirInfo();
      }
    }

    d->fs->setIdDirNode( pwd );
    if ( !dstNodeInfo.isValid() )
      return false;

    if ( d->is_X_AncestorOf_Y_( srcNodeInfo, dstNodeInfo ) )
      return false;

    bool ret = false;

    if ( srcNodeInfo.idParentNode != dstNodeInfo.idNode() ) {
      srcNodeInfo.idParentNode = dstNodeInfo.idNode();
      if ( !dst.isEmpty() )
        srcNodeInfo.name = dst;
      ret = ( d->fs->transaction() && d->fs->setNodeInfo( srcNodeInfo ) );
      if ( ret )
        d->fs->commit();
      else
        d->fs->rollback();
    }

    d->fs->setIdDirNode( pwd );
    return ret;
  }

  return false;
}

bool AQSqlDirPrivate::cp( const AQSqlNodeInfo & srcInfo, qlonglong newParentNode, const QString & newName )
{
  qlonglong pwd = fs->idDirNode();
  fs->setIdDirNode( newParentNode );
  bool ret = false;

  if ( srcInfo.flag.testFlag( AQSql::File ) && fs->addFileNode() != -1 ) {
    AQSqlNodeInfo dstInfo( fs->nodeFileInfo() );

    if ( !newName.isEmpty() )
      dstInfo.name = newName;
    else
      dstInfo.name = srcInfo.name;

    ret = ( fs->setNodeInfo( dstInfo ) && fs->cpData( srcInfo, dstInfo ) );
  } else if ( srcInfo.flag.testFlag( AQSql::Dir ) && fs->addDirNode() != -1 ) {
    AQSqlNodeInfo dstInfo( fs->nodeDirInfo() );

    if ( !newName.isEmpty() )
      dstInfo.name = newName;
    else
      dstInfo.name = srcInfo.name;

    if ( fs->setNodeInfo( dstInfo ) ) {
      ret = true;
      fs->setIdDirNode( srcInfo.idNode() );
      AQSqlNodeInfoList list( fs->nodeInfoList() );

      for ( AQSqlNodeInfoList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        if ( ( *it ).name == "." || ( *it ).name == ".." )
          continue;
        ret = cp( *it, dstInfo.idNode() );
        if ( !ret )
          break;
      }
    }
  }

  fs->setIdDirNode( pwd );
  return ret;
}

bool AQSqlDir::cp( const QString & srcName, const QString & dstName ) const
{
  if ( !srcName.isEmpty() && !dstName.isEmpty() ) {
    QStringList srcPathList( srcName.split( "/", QString::SkipEmptyParts ) );
    QString src;
    AQSqlNodeInfo srcNodeInfo( d->fs->nodeVoidInfo() );
    qlonglong pwd = d->fs->idDirNode();

    if ( srcName.startsWith( "/" ) ) {
      d->fs->setIdDirNode( d->fs->idRootNode() );
      if ( srcPathList.isEmpty() )
        srcNodeInfo = d->fs->nodeDirInfo();
    }

    if ( !srcPathList.isEmpty() ) {
      src = srcPathList.takeLast();
      if ( d->cd( srcPathList ) ) {
        if ( d->fs->setIdDirNodeByName( src ) )
          srcNodeInfo = d->fs->nodeDirInfo();
        else
          if ( d->fs->setIdFileNodeByName( src ) )
            srcNodeInfo = d->fs->nodeFileInfo();
      }
    }
    d->fs->setIdDirNode( pwd );
    if ( !srcNodeInfo.isValid() )
      return false;

    QStringList dstPathList( dstName.split( "/", QString::SkipEmptyParts ) );
    QString dst;
    AQSqlNodeInfo dstNodeInfo( d->fs->nodeVoidInfo() );

    if ( dstName.startsWith( "/" ) ) {
      d->fs->setIdDirNode( d->fs->idRootNode() );
      if ( dstPathList.isEmpty() )
        dstNodeInfo = d->fs->nodeDirInfo();
    }
    if ( !dstPathList.isEmpty() ) {
      dst = dstPathList.takeLast();
      if ( d->cd( dstPathList ) ) {
        if ( d->fs->setIdDirNodeByName( dst ) ) {
          if ( !d->fs->setIdDirNodeByName( src ) && !d->fs->setIdFileNodeByName( src ) ) {
            dstNodeInfo = d->fs->nodeDirInfo();
            dst = QString();
          }
        } else
          if ( !d->fs->setIdFileNodeByName( dst ) )
            dstNodeInfo = d->fs->nodeDirInfo();
      }
    }
    d->fs->setIdDirNode( pwd );
    if ( !dstNodeInfo.isValid() )
      return false;

    if ( d->is_X_AncestorOf_Y_( srcNodeInfo, dstNodeInfo ) )
      return false;

    bool ret = ( d->fs->transaction() && d->cp( srcNodeInfo, dstNodeInfo.idNode(), dst ) );
    if ( ret )
      d->fs->commit();
    else
      d->fs->rollback();
    d->fs->setIdDirNode( pwd );
    return ret;
  }
  return false;
}

QByteArray AQSqlDir::fileData( const QString & fileName, QString & shaKey, QString & cacheKey ) const
{
  QByteArray ret;
  if ( !fileName.isEmpty() ) {
    QStringList pathList( fileName.split( "/", QString::SkipEmptyParts ) );
    QString file;

    if ( !pathList.isEmpty() )
      file = pathList.takeLast();
    if ( fileName.startsWith( "/" ) )
      d->fs->setIdDirNode( d->fs->idRootNode() );

    if ( d->cd( pathList ) && d->fs->setIdFileNodeByName( file ) ) {
      AQSqlNodeInfo info( d->fs->nodeFileInfo() );
      QString key = absolutePath() + "/" + info.sha();
      shaKey = info.sha();
      cacheKey = d->prefixCache + key;

      if ( info.flag.testFlag( AQSql::CharFile ) ) {
        QString strRet;

        if ( !AQ_DISKCACHE_FIND( cacheKey, strRet ) ) {
          strRet = d->fs->data();
          if ( !strRet.isEmpty() )
            AQ_DISKCACHE_INS( cacheKey, strRet );
        } else {
          aqDebug( "Hit DiskCache : AQSql::CharFile " + fileName + " : " + shaKey + " : " + cacheKey );
        }
        ret = strRet.toUtf8();
      } else {
        QByteArray baRet;
        if ( !AQ_DISKCACHE_FIND( cacheKey, baRet ) ) {
          baRet = d->fs->data();
          if ( !baRet.isEmpty() )
            AQ_DISKCACHE_INS( cacheKey, baRet );
        } else {
          aqDebug( "Hit DiskCache : AQSql::BinFile " + fileName + " : " + shaKey + " : " + cacheKey );
        }
        ret = baRet;
      }
    }
  }

  return ret;
}

QString AQSqlDir::dirName() const
{
  return d->fs->nodeDirInfo().name;
}

QString AQSqlDir::dirSha() const
{
  return d->fs->nodeDirInfo().sha();
}

QString AQSqlDir::fileName() const
{
  return d->fs->nodeFileInfo().name;
}

QString AQSqlDir::fileSha() const
{
  return d->fs->nodeFileInfo().sha();
}

QString AQSqlDir::fileSha( const QString & name, QString & cacheKey ) const
{
  QString ret;

  if ( !name.isEmpty() ) {
    QStringList pathList( name.split( "/", QString::SkipEmptyParts ) );
    QString name_;
    qlonglong pwdf = d->fs->idFileNode();

    if ( !pathList.isEmpty() )
      name_ = pathList.takeLast();

    if ( name.startsWith( "/" ) )
      d->fs->setIdDirNode( d->fs->idRootNode() );

    if ( d->cd( pathList ) && d->fs->setIdFileNodeByName( name_ ) )
      ret = d->fs->nodeFileInfo().sha();

    d->fs->setIdFileNode( pwdf );
  } else
    ret = d->fs->nodeFileInfo().sha();

  if ( !ret.isEmpty() ) {
    QString key( absolutePath() + "/" + ret );
    cacheKey = d->prefixCache + key;
  }

  return ret;
}

QString AQSqlDir::absolutePath( const QString & name ) const
{
  QString ret;
  QStringList pathList( name.split( "/", QString::SkipEmptyParts ) );
  QString name_;
  qlonglong pwd = d->fs->idDirNode();
  qlonglong pwdf = d->fs->idFileNode();

  if ( !name.isEmpty() ) {
    name_ = pathList.takeLast();
    if ( name.startsWith( "/" ) )
      d->fs->setIdDirNode( d->fs->idRootNode() );
  } else
    name_ = ".";

  if ( d->cd( pathList ) && ( d->fs->setIdFileNodeByName( name_ ) || d->fs->setIdDirNodeByName( name_ ) ) ) {
    AQSqlNodeInfo nodeInfo( d->fs->nodeDirInfo() );

    if ( nodeInfo.idParentNode == 0 )
      ret = "/";
    else {
      while ( nodeInfo.idParentNode > 0 ) {
        ret.prepend( "/" + nodeInfo.name );
        d->fs->setIdDirNode( nodeInfo.idParentNode );
        nodeInfo = d->fs->nodeDirInfo();
      }
    }
  }

  d->fs->setIdDirNode( pwd );
  d->fs->setIdFileNode( pwdf );
  return ret;
}

AQSqlNodeInfoList AQSqlDir::ls( const QString & dirName, const QString & filterSqlWhere ) const
{
  AQSqlNodeInfoList ret;

  if ( !dirName.isEmpty() ) {
    qlonglong pwd = d->fs->idDirNode();

    if ( cd( dirName ) )
      ret = d->fs->nodeInfoList( filterSqlWhere );
    else {
      QStringList pathList( dirName.split( "/", QString::SkipEmptyParts ) );
      QString name;

      if ( !pathList.isEmpty() )
        name = pathList.takeLast();
      if ( dirName.startsWith( "/" ) )
        d->fs->setIdDirNode( d->fs->idRootNode() );
      if ( d->cd( pathList ) && d->fs->setIdFileNodeByName( name ) ) {
        AQSqlNodeInfo info( d->fs->nodeFileInfo() );
        info.name = dirName;
        ret << info;
      }
    }
    d->fs->setIdDirNode( pwd );
  } else
    ret = d->fs->nodeInfoList( filterSqlWhere );
  return ret;
}

QStringList AQSqlDir::lsNames( const QString & dirName, const QString & filterSqlWhere ) const
{
  QStringList ret;

  if ( !dirName.isEmpty() ) {
    qlonglong pwd = d->fs->idDirNode();

    if ( cd( dirName ) )
      ret = d->fs->nameList( filterSqlWhere );
    else {
      QStringList pathList( dirName.split( "/", QString::SkipEmptyParts ) );
      QString name;

      if ( !pathList.isEmpty() )
        name = pathList.takeLast();
      if ( dirName.startsWith( "/" ) )
        d->fs->setIdDirNode( d->fs->idRootNode() );
      if ( d->cd( pathList ) && d->fs->setIdFileNodeByName( name ) )
        ret << dirName;
    }
    d->fs->setIdDirNode( pwd );
  } else
    ret = d->fs->nameList( filterSqlWhere );
  return ret;
}

bool AQSqlDir::cpFileToFs( const QString & srcName, const QString & dstName ) const
{
  QString shaKey, cacheKey;
  QByteArray ret = fileData( srcName, shaKey, cacheKey );

  if ( !ret.isEmpty() ) {
    QDir dirFs( dstName );

    if ( !dirFs.exists() && !dirFs.mkpath( dstName ) )
      return false;

    AQSqlNodeInfo info( d->fs->nodeFileInfo() );
    QFile file( dstName + "/" + info.name );

    if ( !file.open( QIODevice::WriteOnly ) )
      return false;

    if ( info.flag.testFlag( AQSql::CharFile ) ) {
      QTextStream out( &file );
      out << ret;
      file.close();
    } else {
      QDataStream out( &file );
      out << ret;
      file.close();
    }
  }

  return true;
}

bool AQSqlDir::cleanOrphanData( QStringList & deleted ) const
{
  QStringList list( d->fs->orphanData() );
  bool ret = true;
  for ( int i = 0; i < list.size() && ret; ++i ) {
    ret = d->fs->freeData( list.at( i ) );
    deleted << list.at( i );
  }
  return ret;
}

bool AQSqlDir::cleanVoidFiles( QStringList & deleted ) const
{
  QList<qlonglong> list( d->fs->voidNodes() );
  bool ret = true;
  QString n;
  for ( int i = 0; i < list.size() && ret; ++i ) {
    d->fs->setIdFileNode( list.at( i ) );
    n = d->fs->nodeFileInfo().name;
    ret = ( d->fs->delFileNode() != -1 );
    deleted << n;
  }
  return ret;
}

QString AQSqlDir::fileSpecialFlag() const
{
  return d->fs->nodeFileInfo().specialFlag;
}

QString AQSqlDir::fileSpecialFlag( const QString & name ) const
{
  QString ret;

  if ( !name.isEmpty() ) {
    QStringList pathList( name.split( "/", QString::SkipEmptyParts ) );
    QString name_;
    qlonglong pwdf = d->fs->idFileNode();

    if ( !pathList.isEmpty() )
      name_ = pathList.takeLast();

    if ( name.startsWith( "/" ) )
      d->fs->setIdDirNode( d->fs->idRootNode() );

    if ( d->cd( pathList ) && d->fs->setIdFileNodeByName( name_ ) )
      ret = d->fs->nodeFileInfo().specialFlag;

    d->fs->setIdFileNode( pwdf );
  } else
    ret = d->fs->nodeFileInfo().specialFlag;

  return ret;
}

void AQSqlDir::fileSetSpecialFlag( const QString & sf ) const
{
  AQSqlNodeInfo info( d->fs->nodeFileInfo() );
  info.specialFlag = sf;
  d->fs->setNodeInfo( info );
}

void AQSqlDir::fileSetSpecialFlag( const QString &name, const QString & sf ) const
{
  if ( !name.isEmpty() ) {
    QStringList pathList( name.split( "/", QString::SkipEmptyParts ) );
    QString name_;
    qlonglong pwdf = d->fs->idFileNode();

    if ( !pathList.isEmpty() )
      name_ = pathList.takeLast();

    if ( name.startsWith( "/" ) )
      d->fs->setIdDirNode( d->fs->idRootNode() );

    if ( d->cd( pathList ) && d->fs->setIdFileNodeByName( name_ ) ) {
      AQSqlNodeInfo info( d->fs->nodeFileInfo() );
      info.specialFlag = sf;
      d->fs->setNodeInfo( info );
    }

    d->fs->setIdFileNode( pwdf );
  } else {
    AQSqlNodeInfo info( d->fs->nodeFileInfo() );
    info.specialFlag = sf;
    d->fs->setNodeInfo( info );
  }
}

QStringList AQSqlDir::lsNamesByContent( const QString & filterContent, const QString & filterName,
                                        const QString & dirName, const QString & filterSqlWhere ) const
{
  AQSqlNodeInfoList list;
  QStringList ret;
  qlonglong idDir = d->fs->idDirNode();

  if ( !dirName.isEmpty() ) {
    qlonglong pwd = d->fs->idDirNode();

    if ( !cd( dirName ) ) {
      QStringList pathList( dirName.split( "/", QString::SkipEmptyParts ) );
      QString name;

      if ( !pathList.isEmpty() )
        name = pathList.takeLast();

      if ( dirName.startsWith( "/" ) )
        d->fs->setIdDirNode( d->fs->idRootNode() );

      if ( d->cd( pathList ) && d->fs->setIdFileNodeByName( name ) )
        idDir = d->fs->idDirNode();
    }

    d->fs->setIdDirNode( pwd );
  }

  QString fWhere( filterSqlWhere.isEmpty() ?
                  QString::fromLatin1( "iddirpadre=%1" ).arg( idDir ) :
                  QString::fromLatin1( "%1 and iddirpadre=%2" ).arg( filterSqlWhere ).arg( idDir ) );
  list = d->fs->globalSearchByContent( filterContent, filterName, fWhere );

  for ( AQSqlNodeInfoList::const_iterator it = list.begin(); it != list.end(); ++it )
    ret << ( *it ).name;

  return ret;
}
