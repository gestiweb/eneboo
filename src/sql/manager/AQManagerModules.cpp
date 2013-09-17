/***************************************************************************
AQManagerModules.cpp
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

#include <QSettings>
#include <QTextStream>
#include <QSqlError>
#include <QResource>
#include <QDir>
#include <qslexer.h>
#include <qserrors.h>

#include <AQCore/AQCore.h>
#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQTableMD.h>
#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQDiskCache.h>
#include <AQCore/AQMemCache.h>
#include <AQScript/AQCompilerQSA.h>

#include "AQManagerModules.h"
#include "AQManager.h"
#include "../AQSqlDatabase.h"
#include "../AQSqlCursor.h"
#include "../filesystem/AQSqlDir.h"
#include "../AQSql.h"
#include "../qt3support/q3sqlcursor.h"

class AQInfoMod
{

public:

  AQInfoMod()
  {}
  QString idModulo;
  QString idArea;
  QString descripcion;
  QString version;
  QByteArray icono;
  QString areaDescripcion;
};

class AQManagerModulesPrivate
{
public:

  AQManagerModulesPrivate( const AQManagerModules * mn );
  AQManagerModulesPrivate( const AQSqlDatabase & db, AQManagerModules * mn );
  ~AQManagerModulesPrivate();

  /**
  Limpiar
  */
  void clear();

  /**
  Obtiene el contenido de un fichero almacenado en el sistema de ficheros.

  @param pN Ruta y nombre del fichero en el sistema de ficheros
  @param shaKey Por referencia devuelve la suma SHA del contenido del fichero
  @return QString con el contenido del fichero o vacia en caso de error.
  */
  QString contentFS( const QString & pN, QString & shaKey ) const;
  QString contentFS( const QString & pN ) const;

  /**
  Obtiene el contenido de un fichero almacenado en el sistema de ficheros.

  @param pN Ruta y nombre del fichero en el sistema de ficheros
  @param shaKey Por referencia devuelve la suma SHA del contenido del fichero
  @return QByteArray con el contenido del fichero o vacio en caso de error.
  */
  QByteArray contentByteCodeFS( const QString & pN, QString & shaKey ) const;
  QByteArray contentByteCodeFS( const QString & pN ) const;

  /**
  Guarda el estado del sistema de módulos
  */
  void writeState();

  /**
  Lee el estado del sistema de módulos
  */
  void readState();

  /**
  Carga la lista de todos los identificadores de módulos
  */
  void loadAllIdModules();

  /**
  Carga la lista de todos los identificadores de areas
  */
  void loadIdAreas();

  /**
  Carga dinámica del recurso de imágenes de los módulos instalados en la
  base de datos
  */
  void loadResourceImages();

  /**
  Mantiene el identificador del area a la que pertenece el módulo activo.
  */
  QString activeIdArea_;

  /**
  Mantiene el identificador del módulo activo.
  */
  QString activeIdModule_;

  /**
  Lista de todos los identificadores de módulos cargados, para optimizar lecturas
  */
  QStringList listAllIdModules_;

  /**
  Lista de todas los identificadores de areas cargadas, para optimizar lecturas
  */
  QStringList listIdAreas_;

  /**
  Diccionarios con información de los módulos
  */
  QHash<QString, AQInfoMod *> hashInfoMods_;
  QHash<QString, QString> hashModFiles_;
  static QHash<QString, QString> hashSharedShas_;

  /**
  Base de datos a utilizar por el manejador
  */
  AQSqlDatabase db_;

  /**
  Directorio Sql
  */
  AQSqlDir * dir_;

  /**
  Caché en memoria
  */
  AQMemCache memCache_;

  /**
  Nombre del recurso cargado dinámicamente
  */
  QString rccName_;

  /**
  Propietario
  */
  const AQManagerModules * mn_;
};

QHash<QString, QString> AQManagerModulesPrivate::hashSharedShas_;

AQManagerModulesPrivate::AQManagerModulesPrivate( const AQManagerModules * mn )
    : dir_( 0 ),
    mn_( mn )
{}

AQManagerModulesPrivate::AQManagerModulesPrivate( const AQSqlDatabase & db, AQManagerModules * mn )
    : db_( db ),
    dir_( 0 ),
    mn_( mn )
{}

AQManagerModulesPrivate::~AQManagerModulesPrivate()
{
  writeState();
  clear();
  listAllIdModules_.clear();
  listIdAreas_.clear();
  delete dir_;
}

void AQManagerModulesPrivate::clear()
{
  while ( !hashInfoMods_.isEmpty() ) {
    AQInfoMod * i = hashInfoMods_.begin().value();
    hashInfoMods_.erase( hashInfoMods_.begin() );
    delete i;
  }
  hashModFiles_.clear();
}

void AQManagerModulesPrivate::loadAllIdModules()
{
  if ( !db_.dbAux().isOpen() )
    return ;

  listAllIdModules_.clear();
  listAllIdModules_.append( "sys" );

  clear();

  QSqlQuery q( QString(), db_.dbAux() );
  q.setForwardOnly( true );
  q.exec( "select idmodulo,aqmodules.idarea,aqmodules.descripcion,version,icono,aqareas.descripcion "
          "from aqmodules left join aqareas on aqmodules.idarea = aqareas.idarea" );
  while ( q.next() ) {
    AQInfoMod * infoMod = new AQInfoMod();
    infoMod->idModulo = q.value( 0 ).toString();
    infoMod->idArea = q.value( 1 ).toString();
    infoMod->descripcion = q.value( 2 ).toString();
    infoMod->version = q.value( 3 ).toString();
    infoMod->icono = q.value( 4 ).toByteArray();
    infoMod->areaDescripcion = q.value( 5 ).toString();
    hashInfoMods_.insert( infoMod->idModulo.toUpper(), infoMod );

    if ( infoMod->idModulo != "sys" )
      listAllIdModules_.append( infoMod->idModulo );

    QStringList list( dir_->lsNames( "../" + infoMod->idArea + "/" + infoMod->idModulo + "/tables/" ) );
    for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it )
      hashModFiles_.insert( ( *it ).toUpper(), infoMod->idModulo );
  }
}

void AQManagerModulesPrivate::loadResourceImages()
{
  if ( !rccName_.isEmpty() )
    QResource::registerResource( rccName_ );
}

void AQManagerModules::loadResourceImages( const QString & rccName ) const
{
  if ( !d || !d->db_.dbAux().isOpen() )
    return ;

  d->rccName_ = rccName;
  d->loadResourceImages();
}

bool AQManagerModules::init( const AQSqlDatabase & db )
{
  finish();
  d = new AQManagerModulesPrivate( db, this );
  QSqlRecord * buffer;
  Q3SqlCursor cur( QString(), false, db.dbAux() );

  cur.setName( "aqsettings" );
  cur.select( "aqkey = 'sysmodver'" );
  if ( !cur.next() || cur.value( "valor" ).toString() != QString( AQVERSION ) ) {

    QSqlQuery qry( QString(), db.dbAux() );
    qry.exec( "drop table aqserial" );
    if ( !db.manager() ->createSystemTable( "aqserial" ) ) {
      finish();
      return false;
    }
    qry.exec( "drop table aqvar" );
    if ( !db.manager() ->createSystemTable( "aqvar" ) ) {
      finish();
      return false;
    }

    if ( cur.isValid() ) {
      buffer = cur.primeUpdate();
      buffer->setValue( "aqkey", "sysmodver" );
      buffer->setValue( "valor", QString( AQVERSION ) );
      cur.update();
      if ( cur.lastError().isValid() )
        aqWarn( cur.lastError().text() );
    } else {
      buffer = cur.primeInsert();
      buffer->setValue( "aqkey", "sysmodver" );
      buffer->setValue( "valor", QString( AQVERSION ) );
      cur.insert();
      if ( cur.lastError().isValid() )
        aqWarn( cur.lastError().text() );
    }
  }

  cur.setName( "aqareas" );
  cur.select( "idarea = 'sys'" );
  if ( !cur.next() ) {
    buffer = cur.primeInsert();
    buffer->setValue( "idarea", "sys" );
    buffer->setValue( "descripcion", AQ_TR( "MetaData", "Sistema" ) );
    buffer->setValue( "bloqueo", QVariant( false ) );
    cur.insert();
    if ( cur.lastError().isValid() )
      aqWarn( cur.lastError().text() );
  }

  cur.setName( "aqmodules" );
  cur.select( "idmodulo = 'sys'" );
  if ( !cur.next() ) {
    buffer = cur.primeInsert();
    buffer->setValue( "idmodulo", "sys" );
    buffer->setValue( "idarea", "sys" );
    buffer->setValue( "descripcion", AQ_TR( "MetaData", "Administración" ) );
    buffer->setValue( "icono", d->contentFS( AQDATA + "/sys.xpm" ) );
    buffer->setValue( "bloqueo", QVariant( false ) );
    buffer->setValue( "version", AQVERSION );
    cur.insert();
    if ( cur.lastError().isValid() )
      aqWarn( cur.lastError().text() );
  }
  d->dir_ = new AQSqlDir( db );
  d->dir_->mkdir( AQ_DIR_MOD );
  d->dir_->mkdir( AQ_DIR_MOD + "/todos" );
  d->dir_->cd( AQ_DIR_MOD + "/todos" );
  d->readState();
  d->loadAllIdModules();
  return true;
}

void AQManagerModules::finish()
{
  if ( d ) {
    d->db_ = AQSqlDatabase();
    if ( d->dir_ ) {
      delete d->dir_;
      d->dir_ = 0;
    }
    if ( !d->rccName_.isEmpty() )
      QResource::unregisterResource( d->rccName_ );
    delete d;
    d = 0;
  }
}

QString AQManagerModules::content( const QString & n, QString & shaKey, QString & cacheKey ) const
{
  if ( n.isEmpty() || n.length() <= 3 )
    return QString();

  QString ret;
  if ( d && d->memCache_.find( n, ret ) ) {
    aqDebug( "Hit MemCache : " + n );
    return ret;
  }

  shaKey = AQManagerModulesPrivate::hashSharedShas_.value( n );
  if ( !shaKey.isEmpty() ) {
    cacheKey = "shared/sys/" + shaKey;
    AQ_DISKCACHE_FIND( cacheKey, ret );
  }

  if ( ret.isEmpty() ) {
    if ( n.endsWith( ".xml" ) )
      ret = ( d ? d->contentFS( AQDATA + "/" + n, shaKey )
              : AQManagerModulesPrivate( this ).contentFS( AQDATA + "/" + n, shaKey ) );
    else if ( n.endsWith( ".qs" ) )
      ret = ( d ? d->contentFS( AQDATA + "/scripts/" + n, shaKey )
              : AQManagerModulesPrivate( this ).contentFS( AQDATA + "/scripts/" + n, shaKey ) );
    else if ( n.endsWith( ".mtd" ) )
      ret = ( d ? d->contentFS( AQDATA + "/tables/" + n, shaKey )
              : AQManagerModulesPrivate( this ).contentFS( AQDATA + "/tables/" + n, shaKey ) );
    else if ( n.endsWith( ".ui" ) )
      ret = ( d ? d->contentFS( AQDATA + "/forms/" + n, shaKey )
              : AQManagerModulesPrivate( this ).contentFS( AQDATA + "/forms/" + n, shaKey ) );
    else if ( n.endsWith( ".kut" ) )
      ret = ( d ? d->contentFS( AQDATA + "/reports/" + n, shaKey )
              : AQManagerModulesPrivate( this ).contentFS( AQDATA + "/reports/" + n, shaKey ) );
    else if ( n.endsWith( ".qry" ) )
      ret = ( d ? d->contentFS( AQDATA + "/queries/" + n, shaKey )
              : AQManagerModulesPrivate( this ).contentFS( AQDATA + "/queries/" + n, shaKey ) );
    else if ( n.endsWith( ".ts" ) )
      ret = ( d ? d->contentFS( AQDATA + "/translations/" + n, shaKey )
              : AQManagerModulesPrivate( this ).contentFS( AQDATA + "/translations/" + n, shaKey ) );

    if ( !ret.isEmpty() ) {
      cacheKey = "shared/sys/" + shaKey;
      AQManagerModulesPrivate::hashSharedShas_.insert( n, shaKey );

      if ( !AQ_DISKCACHE_FIND( cacheKey, ret ) )
        AQ_DISKCACHE_INS( cacheKey, ret );

    } else if ( d && d->dir_ && n.left( 3 ) != "sys" && !d->db_.manager() ->isSystemTable( n ) )
      ret = d->dir_->fileData( n, shaKey, cacheKey );
  }

  if ( d )
    d->memCache_.insert( n, ret );
  return ret;
}

QByteArray AQManagerModules::contentByteCode( const QString & n, QString & shaKey, QString & cacheKey ) const
{
  if ( n.isEmpty() || n.length() <= 3 || !n.endsWith( ".qs" ) )
    return QByteArray();

  QByteArray ret;
  shaKey = AQManagerModulesPrivate::hashSharedShas_.value( n + ".qso" );
  if ( !shaKey.isEmpty() ) {
    cacheKey = "shared/sys/" + shaKey;
    AQ_DISKCACHE_FIND( cacheKey, ret );
  }

  if ( ret.isEmpty() ) {
    QString shaKey2;

    ret = d->contentByteCodeFS( AQDATA + "/scripts/" + n + ".qso", shaKey2 );

    if ( ret.isEmpty() ) {
      if ( d && d->dir_ && n.left( 3 ) != "sys" ) {
        QString fileName( n + ".qso" );
        if ( d->dir_->exists( fileName ) )
          ret = d->dir_->fileData( fileName, shaKey, cacheKey );
      }
    } else {
      shaKey = shaKey2;
      cacheKey = "shared/sys/" + shaKey;
      AQManagerModulesPrivate::hashSharedShas_.insert( n + ".qso", shaKey );
      if ( !AQ_DISKCACHE_FIND( cacheKey, ret ) )
        AQ_DISKCACHE_INS( cacheKey, ret );
    }
  }

  return ret;
}

QStringList AQManagerModules::makeByteCode( const QString & n ) const
{
  QStringList ret;
  if ( n.isEmpty() || n.length() <= 3 || !n.endsWith( ".qs" ) || n.left( 3 ) == "sys" )
    ret << AQ_TR( "Manager", "Nombre de script no válido; %1" ).arg( n );

  if ( ret.isEmpty() && d && d->dir_ ) {
    QString shaKey, cacheKey, fileSrcCode( n ), fileByteCode( n + ".qso" );
    QSLexer lex;
    AQCompilerQSA compiler( &lex );
    QByteArray byteCode( compiler.byteCode( content( fileSrcCode, shaKey, cacheKey ) ) );

    if ( compiler.errorType() != QSErrNoError )
      for ( int i = 0; i < compiler.errorLines().size(); ++i )
        ret << AQ_TR( "Manager", "Linea %1 : %2" ).arg( compiler.errorLines().at( i ) ).arg( compiler.errorMessages().at( i ) );
    else
      if ( byteCode.isEmpty() )
        ret << AQ_TR( "Manager", "Error general de compilación, el fichero fuente no existe ( %1 ) "
                                      "o no se ha podido guardar el fichero compilado ( %1 ) " ).arg( fileSrcCode ).arg( fileByteCode );

    if ( ret.isEmpty() ) {
      d->dir_->rm( fileByteCode );
      d->dir_->mkfile( fileByteCode, byteCode );
    }
  }

  return ret;
}

bool AQManagerModules::exists( const QString & fileName, QString & shaKey, QString & cacheKey ) const
{
  shaKey = AQManagerModulesPrivate::hashSharedShas_.value( fileName );
  if ( !shaKey.isEmpty() ) {
    cacheKey = "shared/sys/" + shaKey;
    return true;
  }
  bool ret = false;
  if ( d && d->dir_ ) {
    ret = d->dir_->exists( fileName );
    if ( ret )
      shaKey = d->dir_->fileSha( fileName, cacheKey );
  }
  return ret;
}

QString AQManagerModulesPrivate::contentFS( const QString & pN, QString & shaKey ) const
{
  QString str_ret;
  QFile fi( pN );
  if ( fi.open( QIODevice::ReadOnly ) ) {
    QTextStream t( &fi );
    str_ret = t.readAll();
    fi.close();

    shaKey = AQCoreUtil::sha1File( ( char * ) pN.toStdString().c_str() );
  }
  return str_ret;
}

QString AQManagerModulesPrivate::contentFS( const QString & pN ) const
{
  QString str_ret;
  QFile fi( pN );
  if ( fi.open( QIODevice::ReadOnly ) ) {
    QTextStream t( &fi );
    str_ret = t.readAll();
    fi.close();
  }
  return str_ret;
}

QByteArray AQManagerModulesPrivate::contentByteCodeFS( const QString & pN, QString & shaKey ) const
{
  QString fileName( pN );
  if ( !QFile::exists( fileName ) )
    fileName = pN;

  QFile fi( fileName );
  if ( fi.open( QIODevice::ReadOnly ) ) {
    QByteArray ba( fi.readAll() );
    fi.close();

    shaKey = AQCoreUtil::sha1File( ( char * ) pN.toStdString().c_str() );
    return ba;
  }

  return QByteArray();
}

QByteArray AQManagerModulesPrivate::contentByteCodeFS( const QString & pN ) const
{
  QString fileName( pN );
  if ( !QFile::exists( fileName ) )
    fileName = pN;
  QFile fi( fileName );
  if ( fi.open( QIODevice::ReadOnly ) )
    return fi.readAll();
  return QByteArray();
}

void AQManagerModules::setActiveIdModule( const QString & id ) const
{
  if ( !d )
    return ;

  if ( id.isEmpty() || d->hashInfoMods_.isEmpty() ) {
    d->activeIdArea_ = QString();
    d->activeIdModule_ = QString();
    return ;
  }

  AQInfoMod * iM = d->hashInfoMods_.value( id.toUpper() );
  if ( iM ) {
    d->activeIdArea_ = iM->idArea;
    d->activeIdModule_ = id;
  } else {
    aqWarn( AQ_TR( "Manager", "AQManagerModules : Se ha intentando activar un módulo inexistente" ) );
    d->activeIdArea_ = QString();
    d->activeIdModule_ = QString();
  }
}

void AQManagerModulesPrivate::readState()
{
  QSettings config( QSettings::UserScope, "InfoSiAL" );
  QString idDB = "noDB";
  if ( db_.dbAux().isOpen() )
    idDB = db_.database() + db_.host() + db_.user() + db_.driverName() + QString::number( db_.port() );
  activeIdModule_ = config.value( AQ_KEYBASE + "Modules/activeIdModule/" + idDB ).toString();
  activeIdArea_ = config.value( AQ_KEYBASE + "Modules/activeIdArea/" + idDB ).toString();
  if ( activeIdModule_.isEmpty() || !mn_->listAllIdModules().contains( activeIdModule_ ) )
    mn_->setActiveIdModule( QString() );
}

void AQManagerModulesPrivate::writeState()
{
  QSettings config( QSettings::UserScope, "InfoSiAL" );
  QString idDB = "noDB";
  if ( db_.dbAux().isOpen() )
    idDB = db_.database() + db_.host() + db_.user() + db_.driverName() + QString::number( db_.port() );
  config.setValue( AQ_KEYBASE + "Modules/activeIdModule/" + idDB, activeIdModule_ );
  config.setValue( AQ_KEYBASE + "Modules/activeIdArea/" + idDB, activeIdArea_ );
}

QStringList AQManagerModules::listIdAreas() const
{
  if ( !d )
    return QStringList();
  if ( !d->listIdAreas_.isEmpty() )
    return d->listIdAreas_;
  QStringList ret;
  if ( !d->db_.dbAux().isOpen() )
    return ret;
  QSqlQuery q( QString(), d->db_.dbAux() );
  q.setForwardOnly( true );
  q.exec( "select idarea from aqareas where idarea <> 'sys'" );
  while ( q.next() )
    ret.append( q.value( 0 ).toString() );
  ret.append( "sys" );
  return ret;
}

QStringList AQManagerModules::listIdModules( const QString & idA ) const
{
  if ( !d )
    return QStringList();
  QStringList ret;
  if ( !d->hashInfoMods_.isEmpty() ) {
    for ( QHash<QString, AQInfoMod *>::const_iterator it = d->hashInfoMods_.begin(); it != d->hashInfoMods_.end(); ++it )
      if ( ( *it ) ->idArea.toUpper() == idA.toUpper() )
        ret.append( ( *it ) ->idModulo );
  }
  return ret;
}

QStringList AQManagerModules::listAllIdModules() const
{
  if ( !d )
    return QStringList();
  if ( !d->listAllIdModules_.isEmpty() )
    return d->listAllIdModules_;
  QStringList ret;
  if ( !d->db_.dbAux().isOpen() )
    return ret;
  ret.append( "sys" );
  QSqlQuery q( QString(), d->db_.dbAux() );
  q.setForwardOnly( true );
  q.exec( "select idmodulo from aqmodules where idmodulo <> 'sys'" );
  while ( q.next() )
    ret.append( q.value( 0 ).toString() );
  return ret;
}

void AQManagerModulesPrivate::loadIdAreas()
{
  if ( !db_.dbAux().isOpen() )
    return ;
  listIdAreas_.clear();
  QSqlQuery q( QString(), db_.dbAux() );
  q.setForwardOnly( true );
  q.exec( "select idarea from flareas where idarea <> 'sys'" );
  while ( q.next() )
    listIdAreas_.append( q.value( 0 ).toString() );
  listIdAreas_.append( "sys" );
}

QString AQManagerModules::idAreaToDescription( const QString & idA ) const
{
  if ( !d )
    return QString();
  if ( !d->hashInfoMods_.isEmpty() ) {
    for ( QHash<QString, AQInfoMod *>::const_iterator it = d->hashInfoMods_.begin(); it != d->hashInfoMods_.end(); ++it )
      if ( ( *it ) ->idArea.toUpper() == idA.toUpper() )
        return ( *it ) ->areaDescripcion;
  }
  return idA;
}

QString AQManagerModules::idModuleToDescription( const QString & idM ) const
{
  if ( !d )
    return idM;
  AQInfoMod * iM = d->hashInfoMods_.value( idM.toUpper() );
  if ( iM )
    return iM->descripcion;
  return idM;
}

QString AQManagerModules::idModuleOfFile( const QString & n ) const
{
  return d ? d->hashModFiles_.value( n.toUpper() ) : QString();
}

QByteArray AQManagerModules::rawIconModule( const QString & idM ) const
{
  if ( !d )
    return QByteArray();
  AQInfoMod * iM = d->hashInfoMods_.value( idM.toUpper() );
  if ( iM )
    return iM->icono;
  return QByteArray();
}

QString AQManagerModules::versionModule( const QString & idM ) const
{
  if ( !d )
    return idM;
  AQInfoMod * iM = d->hashInfoMods_.value( idM.toUpper() );
  if ( iM )
    return iM->version;
  return idM;
}

QString AQManagerModules::activeIdArea() const
{
  return d ? d->activeIdArea_ : QString();
}

QString AQManagerModules::activeIdModule() const
{
  return d ? d->activeIdModule_ : QString();
}

void AQManagerModules::markPublicScripts() const
{
  if ( d && d->dir_ ) {
    QStringList list( d->dir_->lsNamesByContent( "% pub|_%", "%.qs" ) );
    for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it )
      d->dir_->fileSetSpecialFlag( *it, "pubscr" );
  }
}

QStringList AQManagerModules::publicScripts() const
{
  QStringList list( "sys.qs" );
  if ( d ) {
    QSqlQuery q( QString(), d->db_.dbAux() );
    q.setForwardOnly( true );
    q.exec( "select nombre from aqsqlfiles where specialflag='pubscr'" );
    while ( q.next() )
      list << q.value( 0 ).toString();
  }
  return list;
}
