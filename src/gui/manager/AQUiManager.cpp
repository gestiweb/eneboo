/***************************************************************************
AQUiManager.cpp
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

#include <QWidget>
#include <QPixmap>
#include <QPixmapCache>
#include <QFormBuilder>
#include <QBuffer>
#include <QTextStream>
#include <QLibraryInfo>
#include <QProcess>
#include <Q3SqlCursor>
#include <QSqlError>
#include <QDomDocument>
#include <QDomElement>

#include <AQCore/AQCore.h>
#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQDiskCache.h>
#include <AQCore/AQShellCmd.h>
#include <AQSql/AQManagerModules.h>
#include <AQSql/AQManager.h>
#include <AQSql/AQSqlDir.h>
#include <AQSql/AQSqlNodeInfo.h>
#include <AQSql/AQSqlShell.h>

#include "../util/AQUiUtil.h"
#include "../AQInvokerThreadContext.h"
#include "AQUiManager.h"

static QWidget * aq_create_ui( const QString & filename, QWidget * parent, AQSqlDatabase * db )
{
  QFormBuilder uiBuilder;
  uiBuilder.setWorkingDirectory( AQPREFIX + "/bin" );
  uiBuilder.addPluginPath( AQPREFIX + "/plugins/designer" );
  uiBuilder.addPluginPath( AQQTDIR + "/plugins/designer" );

  QWidget * w = 0;
  QString shaKey, cacheKey;
  QString content = db->managerModules() ->content( filename, shaKey, cacheKey );

  if ( !content.isEmpty() && AQCoreUtil::isAQDefFile( content.left( 100 ) ) ) {
    QBuffer b;
    b.open( QIODevice::ReadWrite );
    QTextStream t( &b );
    t << content;
    t.flush();
    b.reset();
    w = uiBuilder.load( &b, parent );
    b.close();
  }

  return w;
}

static QPixmap aq_icon_module( const QString & idM, AQSqlDatabase * db )
{
  QPixmap pix;
  QByteArray a = db->managerModules() ->rawIconModule( idM );
  if ( !a.isEmpty() ) {
    if ( !QPixmapCache::find( a.left( 100 ), pix ) ) {
      pix.loadFromData( a );
      QPixmapCache::insert( a.left( 100 ), pix );
    }
  }
  return pix;
}

class AQWuiManagerPrivate
{
public:

  QWidget * createUI( const QString & filename, QWidget * parent, AQSqlDatabase * db )
  {
    return aq_create_ui( filename, parent, db );
  }
  QPixmap iconModule( const QString & idM, AQSqlDatabase * db )
  {
    return aq_icon_module( idM, db );
  }
};

QWidget * AQUiManager::createUI( const QString & filename, QWidget * parent,
                                 AQSqlDatabase db, bool forceExecInGuiThread )
{

  if ( forceExecInGuiThread && !AQ_ISGUI_THREAD ) {
    AQWuiManagerPrivate wuiManager;
    AQ_INVOKER<AQWuiManagerPrivate, QWidget *, QString, QWidget *, AQSqlDatabase *> ivk;

    return ivk.invoke( &wuiManager, &AQWuiManagerPrivate::createUI, filename, parent, &db );
  }

  return aq_create_ui( filename, parent, &db );
}

QPixmap AQUiManager::iconModule( const QString & idM, AQSqlDatabase db )
{
  if ( !AQ_ISGUI_THREAD ) {
    AQWuiManagerPrivate wuiManager;
    AQ_INVOKER<AQWuiManagerPrivate, QPixmap, QString, AQSqlDatabase *> ivk;

    return ivk.invoke( &wuiManager, &AQWuiManagerPrivate::iconModule, idM, &db );
  }

  return aq_icon_module( idM, &db );
}

QString AQUiManager::convertUiToQt4( const QString & fileName, const QString & contentDefFile, AQSqlDatabase db )
{
  QString fileName_( fileName );
  fileName_.remove( ".ui" );

  QString content_( uiPreprocessor( fileName_, contentDefFile ) );

  QString key( "tmp/uictmp/" + AQCoreUtil::sha1( content_ ) );
  AQ_DISKCACHE_INS( key, content_, "ISO 8859-15" );

  QString uicWorkPath( AQ_DISKCACHE_DIRPATH() + "/tmp/uicwork" );
  QDir uicWorkDir( uicWorkPath );
  if ( !uicWorkDir.exists() )
    uicWorkDir.mkdir( uicWorkPath );

  QStringList argv;
  QString binary = QLibraryInfo::location( QLibraryInfo::BinariesPath );
  binary += "/uic3";
  argv += "-extract";
  argv += fileName_ + ".qrc";
  argv += "-convert";
  argv += AQ_DISKCACHE_FILEPATH( key ) ;

  QProcess uic;
  uic.setWorkingDirectory( uicWorkPath );
  uic.start( binary, argv );
  if ( !uic.waitForStarted() ) {
    aqWarn( AQ_TR( "Manager", "Imposible ejecutar %1." ).arg( binary ) );
    return QString();
  }
  if ( !uic.waitForFinished() ) {
    aqWarn( AQ_TR( "Manager", "%1 tiempo excedido." ).arg( binary ) );
    return QString();
  }
  if ( uic.exitCode() ) {
    aqWarn( uic.readAllStandardError() );
    return QString();
  }

  QString ret = uic.readAllStandardOutput();

  uicWorkDir.cd( "images" );
  AQSqlDir sqlDir( db );
  sqlDir.mkdir( AQ_DIR_MOD + "/images" );
  QFileInfoList list( uicWorkDir.entryInfoList( QStringList() << "*_img*.*", QDir::Files ) );
  for ( QFileInfoList::const_iterator it = list.begin(); it != list.end(); ++it ) {
    ret.replace( QRegExp( ">" + ( *it ).baseName() + "<" ),
                 ">:" + AQ_DIR_MOD + "/images/" + ( *it ).baseName() + ".png<" );
    QImage img( ( *it ).absoluteFilePath() );
    QByteArray ba;
    QBuffer bu( &ba );
    bu.open( QIODevice::WriteOnly );
    if ( img.scaledToHeight( 16, Qt::SmoothTransformation ).save( &bu, "PNG", 100 ) )
      sqlDir.mkfile( AQ_DIR_MOD + "/images/" + ( *it ).baseName() + ".png", ba );
    bu.close();
    uicWorkDir.remove( ( *it ).absoluteFilePath() );
  }
  return ret;
}

QString AQUiManager::uiPreprocessor( const QString & fileName, const QString & contentDefFile )
{
  QDomDocument doc( fileName );
  if ( !AQCoreUtil::domDocumentSetContent( doc, contentDefFile ) ) {
    aqWarn( "AQManager : " +
            AQ_TR( "Manager", "Error al cargar la definición XML del formulario %1" ).arg( fileName ) );
    return contentDefFile;
  }

  QDomNodeList nodeList( doc.elementsByTagName( "widget" ) );
  QDomElement e;

  for ( int i = 0; i < nodeList.size(); ++i ) {
    e = nodeList.at( i ).toElement();

    if ( e.attribute( "class" ) == "FLFieldDB" ) {
      e.setAttribute( "class", "AQFieldDB" );
      continue;
    }

    if ( e.attribute( "class" ) == "FLTableDB" ) {
      e.setAttribute( "class", "AQTableDB" );
      continue;
    }

    if ( e.attribute( "class" ) == "Q3MainWindow" || e.attribute( "class" ) == "QMainWindow" ) {
      e.setAttribute( "class", "AQ3MainWindow" );
      continue;
    }

    if ( e.attribute( "class" ) == "QFrame" ) {
      QDomNodeList props( e.elementsByTagName( "property" ) );
      QDomElement prop, propMinSize;
      bool removePropMinSize = false;

      for ( int j = 0; j < props.size(); ++j ) {
        prop = props.at( j ).toElement();

        if ( prop.attribute( "name" ) == "frameShape" ) {
          QDomElement propChild( prop.firstChild().toElement() );
          if ( propChild.text() == "GroupBoxPanel" || propChild.text() == "LineEditPanel" ||
               propChild.text() == "MenuBarPanel" || propChild.text() == "PopupPanel" ||
               propChild.text() == "TabWidgetPanel" || propChild.text() == "ToolBarPanel" )
            propChild.firstChild().setNodeValue( "QFrame::StyledPanel" );
          continue;
        }

        if ( prop.attribute( "name" ) == "name" && prop.text().contains( "barraBotones" ) ) {
          if ( propMinSize.isNull() )
            removePropMinSize = true;
          else
            e.removeChild( propMinSize );
          continue;
        }

        if ( prop.attribute( "name" ) == "minimumSize" ) {
          if ( removePropMinSize )
            e.removeChild( prop );
          else
            propMinSize = prop;
          continue;
        }
      }

      continue;
    }

    if ( e.attribute( "class" ) == "QToolButton" || e.attribute( "class" ) == "QPushButton" ) {
      QDomNodeList props( e.elementsByTagName( "property" ) );
      QDomElement prop, propMin, propMax;

      for ( int j = 0; j < props.size(); ++j ) {
        prop = props.at( j ).toElement();

        if ( prop.attribute( "name" ) == "minimumSize" ) {
          QDomNodeList sizeChilds( prop.firstChild().childNodes() );
          sizeChilds.at( 0 ).firstChild().setNodeValue( "24" );
          sizeChilds.at( 1 ).firstChild().setNodeValue( "24" );
          propMin = prop;
          if ( !propMax.isNull() )
            break;
          continue;
        }

        if ( prop.attribute( "name" ) == "maximumSize" ) {
          QDomNodeList sizeChilds( prop.firstChild().childNodes() );
          sizeChilds.at( 0 ).firstChild().setNodeValue( "24" );
          sizeChilds.at( 1 ).firstChild().setNodeValue( "24" );
          propMax = prop;
          if ( !propMin.isNull() )
            break;
          continue;
        }
      }

      if ( propMax.isNull() && !propMin.isNull() ) {
        propMax = propMin.cloneNode( true ).toElement();
        propMax.setAttribute( "name", "maximumSize" );
        e.appendChild( propMax );
      }

      continue;
    }
  }

  nodeList = doc.elementsByTagName( "menubar" );
  for ( int i = 0; i < nodeList.size(); ++i ) {
    e = nodeList.at( i ).toElement();
    QDomNodeList props( e.elementsByTagName( "property" ) );
    QDomElement prop;

    for ( int j = 0; j < props.size(); ++j ) {
      prop = props.at( j ).toElement();
      if ( prop.attribute( "name" ) == "frameShape" ) {
        prop.parentNode().removeChild( prop );
        break;
      }
    }
  }

  nodeList = doc.elementsByTagName( "signal" );
  for ( int i = 0; i < nodeList.size(); ++i ) {
    e = nodeList.at( i ).toElement();
    if ( e.text() == "activated()" )
      e.firstChild().setNodeValue( "triggered()" );
  }

  nodeList = doc.elementsByTagName( "image" );
  for ( int i = 0; i < nodeList.size(); ++i ) {
    e = nodeList.at( i ).toElement();
    QString name( e.attribute( "name" ) );
    e.setAttribute( "name", name.replace( QRegExp( "image(\\d+)" ), fileName + "_img\\1" ) );
  }

  nodeList = doc.elementsByTagName( "iconset" );
  for ( int i = 0; i < nodeList.size(); ++i ) {
    e = nodeList.at( i ).toElement();
    QString name( e.text() );
    e.firstChild().setNodeValue( name.replace( QRegExp( "image(\\d+)" ), fileName + "_img\\1" ) );
  }

  nodeList = doc.elementsByTagName( "pixmap" );
  for ( int i = 0; i < nodeList.size(); ++i ) {
    e = nodeList.at( i ).toElement();
    QString name( e.text() );
    e.firstChild().setNodeValue( name.replace( QRegExp( "image(\\d+)" ), fileName + "_img\\1" ) );
  }

  return doc.toString();
}

QString AQUiManager::compileResourceImages( AQSqlDatabase db )
{
  QString prefix = AQ_DISKCACHE_DIRPATH() + "/rcc/" + db.database();
  QString rccName = prefix + "/modulos.rcc";
  QFile rccFile( rccName );
  AQSqlDir sqlDir( db );
  if ( rccFile.exists() ) {
    sqlDir.cd( AQ_DIR_MOD + "/images" );
    QString sha;
    QSqlQuery q( QString(), db.dbAux() );
    if ( q.exec( "select valor from aqsettings where aqkey = 'sharccimages'" ) ) {
      if ( q.next() ) {
        sha = q.value( 0 ).toString();
        if ( sqlDir.dirSha() == sha )
          return rccName;
      }
    }
  }

  AQSqlNodeInfoList list( sqlDir.ls( AQ_DIR_MOD + "/images" ) );

  if ( list.isEmpty() )
    return QString();

  QString qrcFile = QString( "<RCC>\n" ) + "<qresource prefix=\"" + AQ_DIR_MOD + "\" >\n";
  QString path = prefix + "/images/";
  QDir d( path );

  if ( !d.exists() && !d.mkpath( path ) )
    return QString();

  QString shaKey, cacheKey;

  for ( AQSqlNodeInfoList::const_iterator it = list.begin(); it != list.end(); ++it ) {
    if ( ( *it ).flag.testFlag( AQSql::File ) ) {
      QByteArray ba( sqlDir.fileData( AQ_DIR_MOD + "/images/" + ( *it ).name, shaKey, cacheKey ) );
      QImage img;
      if ( img.loadFromData( ba ) && img.save( path + ( *it ).name ) )
        qrcFile += "<file>images/" + ( *it ).name + "</file>\n";
    }
  }

  qrcFile += "</qresource>\n</RCC>\n";
  QFile file( prefix + "/modulos.qrc" );

  if ( !file.open( QIODevice::WriteOnly ) )
    return QString();

  QTextStream out( &file );
  out << qrcFile;
  file.close();

  QStringList argv;
  QString binary = QLibraryInfo::location( QLibraryInfo::BinariesPath );
  binary += "/rcc";
  argv += "-binary";
  argv += "-o";
  argv += "modulos.rcc";
  argv += "modulos.qrc";

  QProcess rcc;
  rcc.setWorkingDirectory( prefix );
  rcc.start( binary, argv );
  if ( !rcc.waitForStarted() ) {
    aqWarn( AQ_TR( "Manager", "Imposible ejecutar %1." ).arg( binary ) );
    return QString();
  }
  if ( !rcc.waitForFinished() ) {
    aqWarn( AQ_TR( "Manager", "%1 tiempo excedido." ).arg( binary ) );
    return QString();
  }
  if ( rcc.exitCode() ) {
    aqWarn( rcc.readAllStandardError() );
    return QString();
  }

  sqlDir.cd( AQ_DIR_MOD + "/images" );

  if ( sqlDir.dirSha().isEmpty() )
    return QString();

  Q3SqlCursor cur( "aqsettings", true, db.dbAux() );
  QSqlRecord * buffer;

  if ( cur.select( "aqkey = 'sharccimages'" ) ) {
    if ( cur.next() ) {
      buffer = cur.primeUpdate();
      buffer->setValue( "valor", sqlDir.dirSha() );
      cur.update();
      if ( cur.lastError().isValid() ) {
        aqWarn( cur.lastError().text() );
        return QString();
      }
    } else {
      buffer = cur.primeInsert();
      buffer->setValue( "aqkey", "sharccimages" );
      buffer->setValue( "valor", sqlDir.dirSha() );
      cur.insert();
      if ( cur.lastError().isValid() ) {
        aqWarn( cur.lastError().text() );
        return QString();
      }
    }
  }

  return rccName;
}

QString AQUiManager::convertToAbanQ3( const QString & fileName, const QString & contentDefFile,
                                      AQCore::TypeDefFile type, AQSqlDatabase db )
{
  QString ret;

  switch ( type ) {
    case AQCore::UiDef:
      ret = convertUiToQt4( fileName, contentDefFile, db );
      break;

    default:
      ret = contentDefFile;
  }

  return ret.toUtf8();
}

bool AQUiManager::updateFromAbanQ2( AQSqlDatabase db )
{
  const AQManager * mn_ = db.manager();

  QString shaFL( mn_->shaGlobalFL() );

  if ( shaFL == mn_->shaLastUpdateFL() )
    return false;

  AQUiUtil util;
  uint p;

  Q3SqlCursor cur( QString(), false, db.dbAux() );
  QSqlRecord * buffer;
  QSqlQuery q( QString(), db.dbAux() );

  AQSqlShell sqlSh( db );
  AQShellCmd cmd;

  if ( q.exec( "select f.nombre,f.contenido,m.idmodulo,a.idarea from flfiles f inner join flmodules m on "
               "f.idmodulo = m.idmodulo inner join flareas a on m.idarea = a.idarea" ) ) {
    util.createProgressDialog( AQ_TR( "Manager", "Conversor AbanQ v3" ), q.size() );
    p = 0;

    while ( q.next() ) {
      util.setLabelText( AQ_TR( "Manager", "Convirtiendo %1" )
                         .arg( q.value( 0 ).toString() ) );

      AQCore::TypeDefFile type = AQCoreUtil::typeAQDefFile( q.value( 1 ).toString() );
      QString file;

      QString converted( convertToAbanQ3( q.value( 0 ).toString(), q.value( 1 ).toString(), type, db ) );

      switch ( type ) {
        case AQCore::UiDef:
          file = AQ_DIR_MOD + "/" + q.value( 3 ).toString() + "/" + q.value( 2 ).toString() +
                 "/forms/" + q.value( 0 ).toString();
          break;
        case AQCore::QryDef:
          file = AQ_DIR_MOD + "/" + q.value( 3 ).toString() + "/" + q.value( 2 ).toString() +
                 "/queries/" + q.value( 0 ).toString();
          break;
        case AQCore::KutDef:
          file = AQ_DIR_MOD + "/" + q.value( 3 ).toString() + "/" + q.value( 2 ).toString() +
                 "/reports/" + q.value( 0 ).toString();
          break;
        case AQCore::MtdDef:
          file = AQ_DIR_MOD + "/" + q.value( 3 ).toString() + "/" + q.value( 2 ).toString() +
                 "/tables/" + q.value( 0 ).toString();
          break;
        case AQCore::TsDef:
          file = AQ_DIR_MOD + "/" + q.value( 3 ).toString() + "/" + q.value( 2 ).toString() +
                 "/translations/" + q.value( 0 ).toString();
          break;
        default:
          if ( q.value( 0 ).toString().endsWith( ".qs" ) )
            file = AQ_DIR_MOD + "/" + q.value( 3 ).toString() + "/" + q.value( 2 ).toString() +
                   "/scripts/" + q.value( 0 ).toString();
          else
            file = AQ_DIR_MOD + "/" + q.value( 3 ).toString() + "/" + q.value( 2 ).toString() +
                   "/" + q.value( 0 ).toString();
      }

      cmd.cmd = "mkfile";
      cmd.argList = QStringList() << file << converted;
      sqlSh.cmd( cmd );

      cmd.cmd = "cp";
      cmd.argList = QStringList() << file << AQ_DIR_MOD + "/todos";
      sqlSh.cmd( cmd );

      util.setProgress( ++p );
    }

    util.destroyProgressDialog();
  }

  AQSqlDir sqlDir( db, AQ_DIR_MOD + "/todos" );
  QStringList qsFiles( sqlDir.lsNames( ".", "nombre like '%.qs'" ) );

  if ( !qsFiles.isEmpty() ) {
    util.createProgressDialog( AQ_TR( "Manager", "Compilador QSA" ), qsFiles.size() );
    p = 0;

    cmd.cmd = "qsac";

    for ( QStringList::const_iterator it = qsFiles.begin(); it != qsFiles.end(); ++it ) {
      util.setLabelText( AQ_TR( "Manager", "Compilando %1" ).arg( *it ) );

      cmd.argList = QStringList() << AQ_DIR_MOD + "/todos/" + *it;
      sqlSh.cmd( cmd );

      util.setProgress( ++p );
    }

    util.destroyProgressDialog();
  }

  cur.setName( "aqsettings" );

  if ( cur.select( "aqkey = 'shalastupdatefl'" ) ) {
    if ( cur.next() ) {
      buffer = cur.primeUpdate();
      buffer->setValue( "aqkey", "shalastupdatefl" );
      buffer->setValue( "valor", shaFL );
      cur.update();

      if ( cur.lastError().isValid() )
        aqWarn( cur.lastError().text() );
    } else {
      buffer = cur.primeInsert();
      buffer->setValue( "aqkey", "shalastupdatefl" );
      buffer->setValue( "valor", shaFL );
      cur.insert();

      if ( cur.lastError().isValid() )
        aqWarn( cur.lastError().text() );
    }
  }

  return true;
}
