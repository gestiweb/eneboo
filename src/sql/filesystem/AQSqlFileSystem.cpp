/***************************************************************************
AQSqlFileSystem.cpp
-------------------
begin                : 01/09/2007
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

#include <QDateTime>
#include <QSqlError>
#include <QBuffer>

#include <AQCore/AQCoreUtil.h>

#include "AQSqlFileSystem.h"
#include "../qt3support/q3sqlcursor.h"

AQSqlFileSystemPrivate::AQSqlFileSystemPrivate( AQSqlDatabase db, AQSqlFileSystem * fs ) :
    defaultPerms( "rxwr-xr-x" ),
    idRootNode( -1 ),
    idDirNode( -1 ),
    idFileNode( -1 ),
    db_( db ),
    f( fs )
{}

AQSqlFileSystemPrivate::~AQSqlFileSystemPrivate()
{}

void AQSqlFileSystemPrivate::updateDirSha( qlonglong idDir )
{
  QString sha;
  QSqlQuery q( "select sha from aqsqlfiles where iddirpadre = " + QString::number( idDir ), db_.dbAux() );

  while ( q.next() )
    sha = AQCoreUtil::sha1( sha + q.value( 0 ).toString() );

  if ( sha.isEmpty() )
    sha = "VOID";

  Q3SqlCursor cur( "aqsqldirs", true, db_.dbAux() );
  QSqlRecord * buffer;
  cur.select( "iddir = " + QString::number( idDirNode ) );

  if ( cur.next() ) {
    buffer = cur.primeUpdate();
    buffer->setValue( "sha", sha );
    cur.update();
  }
}

AQSqlFileSystem::AQSqlFileSystem() :
    d( 0 )
{}

AQSqlFileSystem::~AQSqlFileSystem()
{
  finish();
}

bool AQSqlFileSystem::init( AQSqlDatabase db )
{
  if ( !db.dbAux().isOpen() ) {
    finish();
    return false;
  }

  if ( d )
    return true;

  d = new AQSqlFileSystemPrivate( db, this );

  db.createIndex( "iddirpadre", "aqsqldirs", false );
  db.createIndex( "nombre", "aqsqldirs", false );
  db.createIndex( "sha", "aqsqldirs", false );
  db.createIndex( "flag", "aqsqldirs", false );
  db.createIndex( "specialflag", "aqsqldirs", false );
  db.createIndex( "iddirpadre", "aqsqlfiles", false );
  db.createIndex( "nombre", "aqsqlfiles", false );
  db.createIndex( "sha", "aqsqlfiles", false );
  db.createIndex( "sha", "aqsqlbinfiles", false );
  db.createIndex( "sha", "aqsqlcharfiles", false );
  db.createIndex( "flag", "aqsqlfiles", false );
  db.createIndex( "specialflag", "aqsqlfiles", false );

  Q3SqlCursor cur( "aqsqldirs", true, db.dbAux() );
  QSqlRecord * buffer;
  cur.select( "iddirpadre = 0" );

  if ( !cur.next() ) {
    buffer = cur.primeInsert();
    qlonglong idDir = db.nextSerialVal( "aqsqldirs", "iddir" ).toLongLong();
    buffer->setValue( "iddir", idDir );
    buffer->setValue( "iddirpadre", 0 );
    buffer->setValue( "nombre", "/" );
    buffer->setValue( "iduser", "root" );
    buffer->setValue( "idgroup", "root" );
    buffer->setValue( "flag", AQSql::Dir );
    buffer->setValue( "perms", d->defaultPerms );
    buffer->setValue( "datetimecre", AQ_NOW_DT );
    buffer->setValue( "datetimemod", AQ_NOW_DT );
    buffer->setValue( "datetimeacc", AQ_NOW_DT );
    buffer->setValue( "nobloqueo", false );
    buffer->setValue( "sha", "VOID" );
    cur.insert();

    if ( cur.lastError().isValid() ) {
      aqWarn( cur.lastError().text() );
      finish();
      return false;
    } else {
      d->idRootNode = idDir;
      if ( d->idRootNode > 0 ) {
        d->idDirNode = d->idRootNode;
        return true;
      }
    }
  }

  QVariant idDir( cur.value( "iddir" ) );
  if ( idDir.isValid() ) {
    d->idRootNode = idDir.toLongLong();
    if ( d->idRootNode > 0 ) {
      d->idDirNode = d->idRootNode;
      return true;
    }
  }

  aqWarn( AQ_TR( "SqlFileSystem",
                 "AQSqlFileSystem : No se pudo inicializar ( R%1:D%2:F%3 )" )
          .arg( d->idRootNode )
          .arg( d->idDirNode )
          .arg( d->idFileNode ) );

  finish();
  return false;
}

void AQSqlFileSystem::finish()
{
  delete d;
  d = 0;
}

bool AQSqlFileSystem::setIdDirNodeByName( const QString & dirName ) const
{
  if ( dirName != "." && dirName != ".." ) {
    QSqlQuery qry( "select iddir from aqsqldirs where nombre = '" + dirName + "' and iddirpadre = " +
                   QString::number( d->idDirNode ), d->db_.dbAux() );
    if ( qry.next() ) {
      d->idDirNode = qry.value( 0 ).toLongLong();
      return true;
    }
    return false;
  }

  if ( isDirNodeValid() ) {
    if ( dirName == "." )
      return true;

    if ( dirName == ".." ) {
      if ( d->idDirNode == d->idRootNode )
        return true;

      QSqlQuery qry( "select iddirpadre from aqsqldirs where iddir = " +
                     QString::number( d->idDirNode ), d->db_.dbAux() );

      if ( qry.next() ) {
        qlonglong idDirPadre = qry.value( 0 ).toLongLong();
        qlonglong backup = d->idDirNode;
        d->idDirNode = idDirPadre;

        if ( !isDirNodeValid() ) {
          d->idDirNode = backup;
          aqWarn( AQ_TR( "SqlFileSystem",
                         "AQSqlFileSystem : Directorio huérfano ( R%1:D%2:F%3 )" )
                  .arg( d->idRootNode )
                  .arg( d->idDirNode )
                  .arg( d->idFileNode ) );
          return false;
        } else
          return true;
      }
    }
  }

  return false;
}

bool AQSqlFileSystem::setIdFileNodeByName( const QString & fileName ) const
{
  QSqlQuery qry( "select idfile from aqsqlfiles where nombre = '" + fileName +
                 "' and iddirpadre = " + QString::number( d->idDirNode ), d->db_.dbAux() );
  if ( qry.next() ) {
    d->idFileNode = qry.value( 0 ).toLongLong();
    return true;
  }
  return false;
}

bool AQSqlFileSystem::isDirNodeValid() const
{
  if ( isReady() && d->idDirNode > 0 ) {
    QSqlQuery qry( "select iddir from aqsqldirs where iddir = " +
                   QString::number( d->idDirNode ), d->db_.dbAux() );
    return ( qry.size() > 0 );
  }
  return false;
}

bool AQSqlFileSystem::isFileNodeValid() const
{
  if ( isReady() && d->idFileNode > 0 ) {
    QSqlQuery qry( "select idfile from aqsqlfiles where idfile = " +
                   QString::number( d->idFileNode ), d->db_.dbAux() );
    return ( qry.size() > 0 );
  }
  return false;
}

qlonglong AQSqlFileSystem::addDirNode() const
{
  if ( isDirNodeValid() ) {
    Q3SqlCursor cur( "aqsqldirs", true, d->db_.dbAux() );
    QSqlRecord * buffer;

    buffer = cur.primeInsert();
    qlonglong idDir = d->db_.nextSerialVal( "aqsqldirs", "iddir" ).toLongLong();
    buffer->setValue( "iddir", idDir );
    buffer->setValue( "iddirpadre", d->idDirNode );
    buffer->setValue( "nombre", "New Dir " + QString::number( idDir ) );
    buffer->setValue( "iduser", d->db_.user() );
    buffer->setValue( "idgroup", d->db_.user() );
    buffer->setValue( "flag", AQSql::Dir );
    buffer->setValue( "perms", d->defaultPerms );
    buffer->setValue( "datetimecre", AQ_NOW_DT );
    buffer->setValue( "datetimemod", AQ_NOW_DT );
    buffer->setValue( "datetimeacc", AQ_NOW_DT );
    buffer->setValue( "nobloqueo", true );
    buffer->setValue( "sha", "VOID" );
    cur.insert();

    if ( !cur.lastError().isValid() ) {
      d->idDirNode = idDir;
      return idDir;
    } else
      aqWarn( cur.lastError().text() );
  }

  return -1;
}

qlonglong AQSqlFileSystem::delDirNode() const
{
  if ( isDirNodeValid() ) {
    Q3SqlCursor cur( "aqsqldirs", true, d->db_.dbAux() );
    cur.select( "iddir = " + QString::number( d->idDirNode ) );

    if ( cur.next() ) {
      qlonglong idDirPadre = cur.value( "iddirpadre" ).toLongLong();
      cur.primeDelete();
      cur.del();
      if ( !cur.lastError().isValid() ) {
        d->idDirNode = idDirPadre;
        return idDirPadre;
      } else
        aqWarn( cur.lastError().text() );
    }
  }

  return -1;
}

qlonglong AQSqlFileSystem::addFileNode() const
{
  if ( isDirNodeValid() ) {
    Q3SqlCursor cur( "aqsqlfiles", true, d->db_.dbAux() );
    QSqlRecord * buffer;

    buffer = cur.primeInsert();
    qlonglong idFile = d->db_.nextSerialVal( "aqsqlfiles", "idfile" ).toLongLong();
    buffer->setValue( "idfile", idFile );
    buffer->setValue( "iddirpadre", d->idDirNode );
    buffer->setValue( "nombre", "New File " + QString::number( idFile ) );
    buffer->setValue( "iduser", d->db_.user() );
    buffer->setValue( "idgroup", d->db_.user() );
    buffer->setValue( "flag", AQSql::File );
    buffer->setValue( "perms", d->defaultPerms );
    buffer->setValue( "datetimecre", AQ_NOW_DT );
    buffer->setValue( "datetimemod", AQ_NOW_DT );
    buffer->setValue( "datetimeacc", AQ_NOW_DT );
    buffer->setValue( "nobloqueo", true );
    buffer->setValue( "sha", "VOID" );
    cur.insert();

    if ( !cur.lastError().isValid() ) {
      d->updateDirSha( d->idDirNode );
      d->idFileNode = idFile;
      return idFile;
    } else
      aqWarn( cur.lastError().text() );
  }

  return -1;
}

qlonglong AQSqlFileSystem::delFileNode() const
{
  if ( isFileNodeValid() ) {
    Q3SqlCursor cur( "aqsqlfiles", true, d->db_.dbAux() );
    cur.select( "idfile = " + QString::number( d->idFileNode ) );

    if ( cur.next() ) {
      qlonglong idDirPadre = cur.value( "iddirpadre" ).toLongLong();
      cur.primeDelete();
      cur.del();

      if ( !cur.lastError().isValid() ) {
        d->idDirNode = idDirPadre;
        cur.select( "iddirpadre = " + QString::number( idDirPadre ) );
        if ( cur.next() )
          d->idFileNode = cur.value( "idfile" ).toLongLong();
        else
          d->idFileNode = -2;
        d->updateDirSha( d->idDirNode );
        return d->idFileNode;
      } else
        aqWarn( cur.lastError().text() );
    }
  }

  return -1;
}

bool AQSqlFileSystem::setNodeInfo( const AQSqlNodeInfo & nodeInfo ) const
{
  if ( nodeInfo.isValid() ) {
    Q3SqlCursor cur( QString(), false, d->db_.dbAux() );

    if ( nodeInfo.flag.testFlag( AQSql::Dir ) ) {
      cur.setName( "aqsqldirs" );
      cur.select( "iddir = " + QString::number( nodeInfo.idNode() ) );
    } else {
      cur.setName( "aqsqlfiles" );
      cur.select( "idfile = " + QString::number( nodeInfo.idNode() ) );
    }

    if ( cur.next() ) {
      QSqlRecord * buffer;

      buffer = cur.primeUpdate();
      buffer->setValue( "nombre", nodeInfo.name );
      buffer->setValue( "iddirpadre", nodeInfo.idParentNode );
      buffer->setValue( "iduser", nodeInfo.idUser );
      buffer->setValue( "idgroup", nodeInfo.idGroup );
      buffer->setValue( "flag", QVariant( nodeInfo.flag ) );
      buffer->setValue( "specialflag", nodeInfo.specialFlag );
      buffer->setValue( "perms", nodeInfo.perms );
      buffer->setValue( "datetimemod", AQ_NOW_DT );
      cur.update();

      if ( !cur.lastError().isValid() ) {
        d->updateDirSha( nodeInfo.idParentNode );
        return true;
      } else
        aqWarn( cur.lastError().text() );
    }
  }

  return false;
}

AQSqlNodeInfo AQSqlFileSystem::nodeDirInfo() const
{
  AQSqlNodeInfo ret;
  if ( isDirNodeValid() ) {
    QSqlQuery qry( "select nombre,iddirpadre,iduser,idgroup,flag,specialflag,perms,iddir,datetimecre,datetimemod,datetimeacc,sha "
                   "from aqsqldirs where iddir = " + QString::number( d->idDirNode ), d->db_.dbAux() );

    if ( qry.next() ) {
      ret.name = qry.value( 0 ).toString();
      ret.idParentNode = qry.value( 1 ).toLongLong();
      ret.idUser = qry.value( 2 ).toString();
      ret.idGroup = qry.value( 3 ).toString();
      ret.flag = ( AQSql::SqlFlagsNode ) qry.value( 4 ).toUInt();
      ret.specialFlag = qry.value( 5 ).toString();
      ret.perms = qry.value( 6 ).toString();
      ret.idNode_ = qry.value( 7 ).toLongLong();
      ret.dateCre_ = qry.value( 8 ).toString();
      ret.dateMod_ = qry.value( 9 ).toString();
      ret.dateAcc_ = qry.value( 10 ).toString();
      ret.sha_ = qry.value( 11 ).toString();
    }
  }

  return ret;
}

AQSqlNodeInfo AQSqlFileSystem::nodeFileInfo() const
{
  AQSqlNodeInfo ret;

  if ( isDirNodeValid() ) {
    QSqlQuery qry( "select nombre,iddirpadre,iduser,idgroup,flag,specialflag,perms,idfile,datetimecre,datetimemod,datetimeacc,sha "
                   "from aqsqlfiles where idfile = " + QString::number( d->idFileNode ), d->db_.dbAux() );

    if ( qry.next() ) {
      ret.name = qry.value( 0 ).toString();
      ret.idParentNode = qry.value( 1 ).toLongLong();
      ret.idUser = qry.value( 2 ).toString();
      ret.idGroup = qry.value( 3 ).toString();
      ret.flag = ( AQSql::SqlFlagsNode ) qry.value( 4 ).toUInt();
      ret.specialFlag = qry.value( 5 ).toString();
      ret.perms = qry.value( 6 ).toString();
      ret.idNode_ = qry.value( 7 ).toLongLong();
      ret.dateCre_ = qry.value( 8 ).toString();
      ret.dateMod_ = qry.value( 9 ).toString();
      ret.dateAcc_ = qry.value( 10 ).toString();
      ret.sha_ = qry.value( 11 ).toString();
    }
  }

  return ret;
}

AQSqlNodeInfoList AQSqlFileSystem::nodeInfoList( const QString & filterSqlWhere ) const
{
  AQSqlNodeInfoList list;

  if ( isDirNodeValid() ) {
    AQSqlNodeInfo thisNode( nodeDirInfo() );

    thisNode.name = ".";
    list << thisNode;

    if ( thisNode.idParentNode == 0 ) {
      thisNode.name = "..";
      list << thisNode;
    } else {
      d->idDirNode = thisNode.idParentNode;
      AQSqlNodeInfo parentNode( nodeDirInfo() );
      parentNode.name = "..";
      list << parentNode;
      d->idDirNode = thisNode.idNode_;
    }

    QString filter = "iddirpadre = " + QString::number( d->idDirNode );
    if ( !filterSqlWhere.isEmpty() )
      filter += " and " + filterSqlWhere;

    QSqlQuery qry( "select nombre,iddirpadre,iduser,idgroup,flag,specialflag,perms,iddir,datetimecre,datetimemod,datetimeacc "
                   "from aqsqldirs where " + filter + " order by nombre", d->db_.dbAux() );
    while ( qry.next() ) {
      AQSqlNodeInfo ret;
      ret.name = qry.value( 0 ).toString();
      ret.idParentNode = qry.value( 1 ).toLongLong();
      ret.idUser = qry.value( 2 ).toString();
      ret.idGroup = qry.value( 3 ).toString();
      ret.flag = ( AQSql::SqlFlagsNode ) qry.value( 4 ).toUInt();
      ret.specialFlag = qry.value( 5 ).toString();
      ret.perms = qry.value( 6 ).toString();
      ret.idNode_ = qry.value( 7 ).toLongLong();
      ret.dateCre_ = qry.value( 9 ).toString();
      ret.dateMod_ = qry.value( 9 ).toString();
      ret.dateAcc_ = qry.value( 10 ).toString();
      list << ret;
    }

    qry.exec( "select nombre,iddirpadre,iduser,idgroup,flag,specialflag,perms,idfile,datetimecre,datetimemod,datetimeacc,sha "
              "from aqsqlfiles where " + filter + "order by nombre" );
    while ( qry.next() ) {
      AQSqlNodeInfo ret;
      ret.name = qry.value( 0 ).toString();
      ret.idParentNode = qry.value( 1 ).toLongLong();
      ret.idUser = qry.value( 2 ).toString();
      ret.idGroup = qry.value( 3 ).toString();
      ret.flag = ( AQSql::SqlFlagsNode ) qry.value( 4 ).toUInt();
      ret.specialFlag = qry.value( 5 ).toString();
      ret.perms = qry.value( 6 ).toString();
      ret.idNode_ = qry.value( 7 ).toLongLong();
      ret.dateCre_ = qry.value( 8 ).toString();
      ret.dateMod_ = qry.value( 9 ).toString();
      ret.dateAcc_ = qry.value( 10 ).toString();
      ret.sha_ = qry.value( 11 ).toString();
      list << ret;
    }
  }

  return list;
}

QStringList AQSqlFileSystem::nameList( const QString & filterSqlWhere ) const
{
  QStringList list;

  if ( isDirNodeValid() ) {
    list << "." << "..";

    QString filter = "iddirpadre = " + QString::number( d->idDirNode );
    if ( !filterSqlWhere.isEmpty() )
      filter += " and " + filterSqlWhere;

    QSqlQuery qry( "select nombre from aqsqldirs where " + filter + "order by nombre", d->db_.dbAux() );
    while ( qry.next() )
      list << qry.value( 0 ).toString();

    qry.exec( "select nombre from aqsqlfiles where " + filter + "order by nombre" );
    while ( qry.next() )
      list << qry.value( 0 ).toString();
  }

  return list;
}

bool AQSqlFileSystem::setData( const QByteArray & content ) const
{
  if ( isFileNodeValid() ) {
    AQSqlNodeInfo nodeInfo( nodeFileInfo() );

    if ( nodeInfo.flag.testFlag( AQSql::CharFile ) )
      return setData( content );

    Q3SqlCursor cur( "aqsqlbinfiles", true, d->db_.dbAux() );
    QSqlRecord * buffer;
    QString sha( AQCoreUtil::sha1( content ) );

    cur.select( "sha = '" + sha + "'" );
    if ( !cur.next() ) {
      buffer = cur.primeInsert();
      buffer->setValue( "sha", sha );
      buffer->setValue( "contenido", content );
      cur.insert();
      if ( cur.lastError().isValid() ) {
        aqWarn( cur.lastError().text() );
        return false;
      }
    }

    qlonglong idDirPadre = -1;
    cur.setName( "aqsqlfiles" );
    cur.select( "idfile = " + QString::number( nodeInfo.idNode_ ) );
    if ( cur.next() ) {
      idDirPadre = cur.value( "iddirpadre" ).toLongLong();
      buffer = cur.primeUpdate();
      buffer->setValue( "sha", sha );
      buffer->setValue( "datetimemod", AQ_NOW_DT );
      cur.update();
      if ( cur.lastError().isValid() ) {
        aqWarn( cur.lastError().text() );
        return false;
      }
    }

    d->updateDirSha( idDirPadre );
    return true;
  }

  return false;
}

bool AQSqlFileSystem::setData( const QString & content ) const
{
  if ( isFileNodeValid() ) {
    AQSqlNodeInfo nodeInfo( nodeFileInfo() );
    if ( nodeInfo.flag.testFlag( AQSql::CharFile ) ) {
      Q3SqlCursor cur( "aqsqlcharfiles", true, d->db_.dbAux() );
      QSqlRecord * buffer;
      QString sha( AQCoreUtil::sha1( content ) );

      cur.select( "sha = '" + sha + "'" );
      if ( !cur.next() ) {
        buffer = cur.primeInsert();
        buffer->setValue( "sha", sha );
        buffer->setValue( "contenido", content );
        cur.insert();
        if ( cur.lastError().isValid() ) {
          aqWarn( cur.lastError().text() );
          return false;
        }
      }

      qlonglong idDirPadre = -1;
      cur.setName( "aqsqlfiles" );
      cur.select( "idfile = " + QString::number( nodeInfo.idNode_ ) );
      if ( cur.next() ) {
        idDirPadre = cur.value( "iddirpadre" ).toLongLong();
        buffer = cur.primeUpdate();
        buffer->setValue( "sha", sha );
        buffer->setValue( "datetimemod", AQ_NOW_DT );
        cur.update();
        if ( cur.lastError().isValid() ) {
          aqWarn( cur.lastError().text() );
          return false;
        }
      }

      d->updateDirSha( idDirPadre );
      return true;
    }
  }

  return false;
}

QByteArray AQSqlFileSystem::data() const
{
  if ( isFileNodeValid() ) {
    AQSqlNodeInfo nodeInfo( nodeFileInfo() );
    Q3SqlCursor cur( QString(), false, d->db_.dbAux() );

    if ( nodeInfo.flag.testFlag( AQSql::CharFile ) )
      cur.setName( "aqsqlcharfiles" );
    else
      cur.setName( "aqsqlbinfiles" );

    cur.select( "sha = '" + nodeInfo.sha_ + "'" );
    if ( cur.next() ) {
      QVariant v = cur.value( "contenido" );
      if ( nodeInfo.flag.testFlag( AQSql::CharFile ) )
        return v.toString().toUtf8();
      else
        return v.toByteArray();
    }
  }

  return QByteArray();
}

bool AQSqlFileSystem::cpData( const AQSqlNodeInfo & srcNode, const AQSqlNodeInfo & dstNode ) const
{
  if ( srcNode.isValid() && dstNode.isValid() ) {
    QSqlQuery qry( "select sha,flag,specialflag from aqsqlfiles where idfile = " +
                   QString::number( srcNode.idNode_ ), d->db_.dbAux() );

    if ( qry.next() ) {
      Q3SqlCursor cur( "aqsqlfiles", true, d->db_.dbAux() );
      cur.select( "idfile = " + QString::number( dstNode.idNode_ ) );

      if ( cur.next() ) {
        qlonglong idDirPadre = cur.value( "iddirpadre" ).toLongLong();
        QSqlRecord * buffer;
        buffer = cur.primeUpdate();
        buffer->setValue( "sha", qry.value( 0 ) );
        buffer->setValue( "flag", qry.value( 1 ) );
        buffer->setValue( "specialflag", qry.value( 2 ) );
        buffer->setValue( "datetimemod", AQ_NOW_DT );
        cur.update();
        if ( !cur.lastError().isValid() ) {
          d->updateDirSha( idDirPadre );
          return true;
        } else
          aqWarn( cur.lastError().text() );
      }
    }
  }

  return false;
}

QStringList AQSqlFileSystem::orphanData() const
{
  QStringList ret;
  QSqlQuery qry( "select sha from aqsqlbinfiles where sha not in ( select sha from aqsqlfiles )",
                 d->db_.dbAux() );
  while ( qry.next() )
    ret << qry.value( 0 ).toString();
  qry.exec( "select sha from aqsqlcharfiles where sha not in ( select sha from aqsqlfiles )" );
  while ( qry.next() )
    ret << qry.value( 0 ).toString();
  return ret;
}

QList<qlonglong> AQSqlFileSystem::voidNodes() const
{
  QList<qlonglong> ret;
  QSqlQuery qry( "select idfile from aqsqlfiles where sha='VOID' or sha not in "
                 "( select sha from aqsqlcharfiles union select sha from aqsqlbinfiles )",
                 d->db_.dbAux() );
  while ( qry.next() )
    ret << qry.value( 0 ).toLongLong();
  return ret;
}

bool AQSqlFileSystem::freeData( const QString & sha ) const
{
  Q3SqlCursor cur( "aqsqlbinfiles", true, d->db_.dbAux() );

  cur.select( "sha = '" + sha + "'" );
  if ( cur.next() ) {
    cur.primeDelete();
    cur.del();
  }

  cur.setName( "aqsqlcharfiles" );
  cur.select( "sha = '" + sha + "'" );
  if ( cur.next() ) {
    cur.primeDelete();
    cur.del();
  }

  return ( !cur.lastError().isValid() );
}

AQSqlNodeInfoList AQSqlFileSystem::globalSearchByContent( const QString & filterContent,
    const QString & filterName, const QString & filterSqlWhere ) const
{
  AQSqlNodeInfoList list;
  QSqlQuery qry( QString(), d->db_.dbAux() );
  QString fields( "f.nombre,f.iddirpadre,f.iduser,f.idgroup,f.flag,f.specialflag,f.perms,f.idfile,"
                  "f.datetimecre,f.datetimemod,f.datetimeacc,f.sha" );
  QString where( ( filterSqlWhere.isEmpty() ? QString() : QString::fromLatin1( "and  %1" ).arg( filterSqlWhere ) ) );

  qry.exec( QString::fromLatin1( "select %1 from aqsqlfiles f inner join aqsqlcharfiles fc on " ).arg( fields ) +
            QString::fromLatin1( "f.sha = fc.sha where fc.contenido like '%1' escape '|' " ).arg( filterContent ) +
            QString::fromLatin1( "and f.nombre like '%1' " ).arg( filterName ) +
            where );
  while ( qry.next() ) {
    AQSqlNodeInfo ret;

    ret.name = qry.value( 0 ).toString();
    ret.idParentNode = qry.value( 1 ).toLongLong();
    ret.idUser = qry.value( 2 ).toString();
    ret.idGroup = qry.value( 3 ).toString();
    ret.flag = ( AQSql::SqlFlagsNode ) qry.value( 4 ).toUInt();
    ret.specialFlag = qry.value( 5 ).toString();
    ret.perms = qry.value( 6 ).toString();
    ret.idNode_ = qry.value( 7 ).toLongLong();
    ret.dateCre_ = qry.value( 8 ).toString();
    ret.dateMod_ = qry.value( 9 ).toString();
    ret.dateAcc_ = qry.value( 10 ).toString();
    ret.sha_ = qry.value( 11 ).toString();
    list << ret;
  }

  return list;
}
