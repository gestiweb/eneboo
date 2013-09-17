/***************************************************************************
AQScriptEngineQSA.cpp
-------------------
begin                : 24/09/2007
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

#include <QDomDocument>
#include <QDomElement>
#include <QDir>

#include <qsinterpreter.h>
#include <qsutilfactory.h>
#include <qsinputdialogfactory.h>
#include <qsworkbench.h>

#include <AQCore/AQCore.h>
#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQDiskCache.h>
#include <AQUi/AQUi.h>
#include <AQUi/AQUiUtil.h>
#include <AQUi/AQApplication.h>
#include <AQSql/AQSqlConnections.h>
#include <AQSql/AQSqlDatabase.h>
#include <AQSql/AQManagerModules.h>
#include <AQScript/AQCoreObjectFactoryQSA.h>
#include <AQScript/AQUiObjectFactoryQSA.h>
#include <AQScript/AQSqlObjectFactoryQSA.h>

#include "AQScriptEngineQSA.h"
#include "AQObjectInterfaceQSA.h"
#include "compiler/AQCompilerQSA.h"

#include <AQScript/AQSqlCursorInterface.h>
#include <AQScript/AQSqlQueryInterface.h>
#include <AQScript/AQMainWindowInterface.h>
#include <AQScript/AQActionInterface.h>
#include <AQScript/AQFieldDBInterface.h>
#include <AQScript/AQTableDBInterface.h>
#include <AQScript/AQFormDBInterface.h>

void AQScriptEngineQSA::registerInterfaceMetaObjects()
{
  qRegisterMetaType<AQActionInterface*>();
  qRegisterMetaType<AQSqlCursorInterface*>();

  QSInterpreter::registerMetaObject( &AQSqlCursorInterface::staticMetaObject );
  QSInterpreter::registerMetaObject( &AQSqlQueryInterface::staticMetaObject );
  QSInterpreter::registerMetaObject( &AQComponentDBInterface::staticMetaObject );
  QSInterpreter::registerMetaObject( &AQMainWindowInterface::staticMetaObject );
  QSInterpreter::registerMetaObject( &AQActionInterface::staticMetaObject );
  QSInterpreter::registerMetaObject( &AQFieldDBInterface::staticMetaObject );
  QSInterpreter::registerMetaObject( &AQTableDBInterface::staticMetaObject );
  QSInterpreter::registerMetaObject( &AQFormDBInterface::staticMetaObject );
}

class AQScriptInfo
{
public:

  enum Type {
    Invalid = -1,
    AP = 0,
    F = 1,
    FR = 2,
    FS = 3,
  };

  AQScriptInfo() :
      obj( 0 ),
      loaded( false ),
      pub( false ),
      type( AQScriptInfo::Invalid )
  {}

  AQScriptInfo( const QString & s = QString(), const QString & n = QString(), Type t = AP, bool p = false ) :
      scrName( s ),
      objName( n ),
      obj( 0 ),
      loaded( false ),
      pub( p ),
      type( t )
  {}

  AQScriptInfo( const AQScriptInfo & scriptInfo )
  {
    copy( scriptInfo );
  }


  AQScriptInfo & operator=( const AQScriptInfo & scriptInfo )
  {
    copy( scriptInfo );
    return *this;
  }

  bool operator==( const AQScriptInfo & scriptInfo ) const
  {
    return ( scrName == scriptInfo.scrName &&
             objName == scriptInfo.objName &&
             obj == scriptInfo.obj &&
             loaded == scriptInfo.loaded &&
             pub == scriptInfo.pub &&
             type == scriptInfo.type );
  }

  QString scrName;
  QString objName;
  QObject * obj;
  bool loaded;
  bool pub;
  Type type;

private:

  void copy( const AQScriptInfo & scriptInfo )
  {
    scrName = scriptInfo.scrName;
    objName = scriptInfo.objName;
    obj = scriptInfo.obj;
    loaded = scriptInfo.loaded;
    pub = scriptInfo.pub;
    type = scriptInfo.type;
  }
};

class AQScriptEngineQSAPrivate
{
public:

  AQScriptEngineQSAPrivate( const AQSqlDatabase & db, AQScriptEngineQSA * scr );

  void clear();
  void init();

  void reevaluate();

  QObject * loadScript( AQScriptInfo & scrInfo, AQObjectFactoryQSA * guiFactory = 0 );
  QObject * loadScript( const QString & objName, AQObjectFactoryQSA * guiFactory = 0 );

  void loadScriptsFromModule( const QString & idM );

  AQObjectFactoryQSA * factoryForClass( const QString & className ) const;

  AQScriptEngineQSA * scr_;
  QSInterpreter * ip_;
  AQWorkbench * wb_;

  AQSqlDatabase db_;

  QMap<QString, AQScriptInfo> mapScriptInfos_;
  QStringList listPub_;
  QList<AQObjectFactoryQSA *> objFactorys_;

  bool cleared_;
};

AQWorkbench::~AQWorkbench()
{
  if ( wb_ ) {
    wb_->close();
    wb_->deleteLater();
    wb_ = 0;
  }
}

void AQWorkbench::evaluate()
{
  eng_->reevaluate();
}

AQScriptEngineQSAPrivate::AQScriptEngineQSAPrivate( const AQSqlDatabase & db, AQScriptEngineQSA * scr ) :
    scr_( scr ),
    ip_( 0 ),
    wb_( 0 ),
    db_( db ),
    cleared_( true )
{}

void AQScriptEngineQSAPrivate::clear()
{
  if ( wb_ ) {
    wb_->deleteLater();
    wb_ = 0;
  }

  if ( ip_ ) {
#ifndef AQ_WUI
    ip_->project() ->deleteLater();
#endif
    ip_->deleteLater();
    ip_ = 0;
  }

  mapScriptInfos_.clear();
  listPub_.clear();
  objFactorys_.clear();

  cleared_ = true;
}

void AQScriptEngineQSAPrivate::init()
{
  if ( !cleared_ )
    return ;

  QStringList list( db_.managerModules() ->listAllIdModules() );
  AQObjectFactoryQSA * factory;

  if ( !ip_ ) {
#ifndef AQ_WUI
    QSProject * pro = new QSProject;
    ip_ = pro->interpreter();
#else
    ip_ = new QSInterpreter;
#endif
    ip_->setErrorMode( QSInterpreter::Notify );

    factory = new AQSqlObjectFactoryQSA( scr_ );
    objFactorys_ << factory;
    ip_->addObjectFactory( factory );

    factory = new AQCoreObjectFactoryQSA( scr_ );
    objFactorys_ << factory;
    ip_->addObjectFactory( factory );

    factory = new AQUiObjectFactoryQSA( scr_ );
    objFactorys_ << factory;
    ip_->addObjectFactory( factory );
#ifndef AQ_WUI
    ip_->addObjectFactory( new QSInputDialogFactory() );
#endif
    ip_->addObjectFactory( new QSUtilFactory() );
  }

  listPub_ = db_.managerModules() ->publicScripts();
  for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it )
    loadScriptsFromModule( *it );

  for ( QMap<QString, AQScriptInfo>::iterator it = mapScriptInfos_.begin(); it != mapScriptInfos_.end(); ++it ) {
    if ( ( *it ).pub )
      loadScript( *it, factory );
  }

  cleared_ = false;
}

void AQScriptEngineQSAPrivate::reevaluate()
{
  if ( cleared_ )
    return ;

  AQUiUtil util;
  int p = 0;

  util.createProgressDialog( AQ_TR( "ScriptEngineQSA", "Evaluando código" ), mapScriptInfos_.size() );

  for ( QMap<QString, AQScriptInfo>::iterator it = mapScriptInfos_.begin(); it != mapScriptInfos_.end(); ++it ) {
    util.setLabelText( QString( "%1" ).arg( ( *it ).scrName ) );

    if ( ( *it ).loaded ) {
      ( *it ).loaded = false;
      loadScript( *it );
    }

    util.setProgress( ++p );
  }

  util.destroyProgressDialog();
}

QObject * AQScriptEngineQSAPrivate::loadScript( AQScriptInfo & scrInfo, AQObjectFactoryQSA * guiFactory )
{
  if ( scrInfo.loaded )
    return scrInfo.obj;

  if ( !scrInfo.obj ) {
    QObject * obj;

    if ( !guiFactory && scrInfo.type != AQScriptInfo::Invalid ) {
      guiFactory = factoryForClass( "AQFormDB" );
      if ( !guiFactory )
        return 0;
    }

    switch ( scrInfo.type ) {
    case AQScriptInfo::AP:
      obj = guiFactory->create( "FLApplication", QVariantList(), 0 );
      break;
    case AQScriptInfo::F:
      obj = guiFactory->create( "FLFormDBNoUi", QVariantList(), 0 );
      break;
    case AQScriptInfo::FS:
      obj = guiFactory->create( "FLFormSearchDBNoUi", QVariantList(), 0 );
      break;
    case AQScriptInfo::FR:
      obj = guiFactory->create( "FLFormRecordDBNoUi", QVariantList(), 0 );
      break;
    default:
      return 0;
    }

    obj->setObjectName( scrInfo.objName );
    obj->setParent( ip_ );
    scrInfo.obj = obj;
  }

  QString shaKey, cacheKey;

  if ( db_.managerModules() ->exists( scrInfo.scrName + ".qso", shaKey, cacheKey ) ) {
    QFile fi( AQ_DISKCACHE_DIRPATH() + cacheKey );

    if ( fi.open( QIODevice::ReadOnly ) ) {
      QDataStream in( &fi );
      ip_->evaluateByteCode( in, scrInfo.obj, scrInfo.scrName );
      scrInfo.loaded = true;
    }
  }

  if ( !scrInfo.loaded ) {
    QByteArray code( db_.managerModules() ->contentByteCode( scrInfo.scrName, shaKey, cacheKey ) );

    ip_->addTransientObject( scrInfo.obj );

    if ( code.isEmpty() || !AQCompilerQSA::isCompatibleByteCode( code ) ) {
      ip_->evaluate( db_.managerModules() ->content( scrInfo.scrName, shaKey, cacheKey ), scrInfo.obj, scrInfo.scrName );
    } else {
      ip_->evaluateByteCode( code, scrInfo.obj, scrInfo.scrName );
    }

    scrInfo.loaded = true;
  }

  return scrInfo.obj;
}

QObject * AQScriptEngineQSAPrivate::loadScript( const QString & objName, AQObjectFactoryQSA * guiFactory )
{
  QMap<QString, AQScriptInfo>::iterator it( mapScriptInfos_.find( objName ) );

  if ( it == mapScriptInfos_.end() )
    return 0;

  return loadScript( *it, guiFactory );
}

void AQScriptEngineQSAPrivate::loadScriptsFromModule( const QString & idM )
{
  QDomDocument doc( "scripts" );
  QString shaKey, cacheKey;

  if ( !AQCoreUtil::domDocumentSetContent( doc, db_.managerModules() ->content( idM + ".xml", shaKey, cacheKey ) ) ) {
    aqWarn( "AQScriptEngineQSA : " +
            AQ_TR( "ScriptEngineQSA", "Error al cargar las acciones del módulo %1" ).arg( idM ) );
    return ;
  }

  mapScriptInfos_.insert( idM,
                          AQScriptInfo( idM + ".qs", idM, AQScriptInfo::AP,
                                        listPub_.contains( idM + ".qs" ) ) );

  QDomElement docElem = doc.documentElement();
  QDomNode no = docElem.firstChild();
  QString scriptForm, scriptFormRecord, name, tag;

  while ( !no.isNull() ) {
    QDomElement e = no.toElement();
    if ( !e.isNull() && e.tagName() == "action" ) {
      QDomNode no2 = e.firstChild();
      scriptForm = scriptFormRecord = name = QString();
      while ( !no2.isNull() ) {
        QDomElement e2 = no2.toElement();
        if ( !e2.isNull() ) {
          tag = e2.tagName();
          if ( tag == "name" ) {
            name = e2.text();
            no2 = no2.nextSibling();
            continue;
          } else if ( tag == "scriptformrecord" ) {
            scriptFormRecord = e2.text() + ".qs";
            no2 = no2.nextSibling();
            continue;
          } else if ( tag == "scriptform" ) {
            scriptForm = e2.text() + ".qs";
            no2 = no2.nextSibling();
            continue;
          } else {
            no2 = no2.nextSibling();
            continue;
          }
        }
        no2 = no.nextSibling();
      }

      if ( !scriptForm.isEmpty() ) {
        QString nameF( QLatin1String( "form" ) + name );
        QString nameFS( QLatin1String( "formSearch" ) + name );

        bool p = listPub_.contains( scriptForm );

        mapScriptInfos_.insert( nameF, AQScriptInfo( scriptForm, nameF, AQScriptInfo::F, p ) );
        mapScriptInfos_.insert( nameFS, AQScriptInfo( scriptForm, nameFS, AQScriptInfo::FS, p ) );
      }
      if ( !scriptFormRecord.isEmpty() ) {
        QString nameFR( QLatin1String( "formRecord" ) + name );

        mapScriptInfos_.insert( nameFR,
                                AQScriptInfo( scriptFormRecord, nameFR, AQScriptInfo::FR,
                                              listPub_.contains( scriptFormRecord ) ) );
      }

      no = no.nextSibling();
      continue;
    }
    no = no.nextSibling();
  }
}

AQObjectFactoryQSA * AQScriptEngineQSAPrivate::factoryForClass( const QString & className ) const
{
  AQObjectFactoryQSA * factory = 0;

  for ( QList<AQObjectFactoryQSA *>::const_iterator it = objFactorys_.begin(); it != objFactorys_.end(); ++it ) {
    factory = *it;

    if ( factory->canCreateInterface( className ) )
      break;
  }

  return factory;
}

bool AQScriptEngineQSA::init( const AQSqlDatabase & db )
{
  finish();

  d = new AQScriptEngineQSAPrivate( db, this );
  d->init();

  return true;
}

void AQScriptEngineQSA::finish()
{
  if ( d ) {
    d->clear();
    d->db_ = AQSqlDatabase();
    delete d;
    d = 0;
  }
}

QVariant AQScriptEngineQSA::call( const QString & function, const QVariantList & arguments,
                                  QObject * objectContext ) const
{
  QVariant ret;

  if ( d && d->ip_ ) {
    if ( !objectContext ) {
      if ( d->ip_->hasFunction( function ) )
        ret = d->ip_->call( function, arguments, objectContext );
      else {
        QString o( function.section( '.', 0, 0 ) );
        QString m( function.section( '.', 1, 1 ) );
        QStringList classes = d->ip_->classes( o );

        for ( QStringList::const_iterator it = classes.begin(); it != classes.end(); ++it ) {
          if ( d->ip_->hasFunction( o + "." + *it + "." + m ) ) {
            ret = d->ip_->call( o + "." + "iface." + m, arguments, objectContext );
            break;
          }
        }
      }
    } else {
      QString objName( objectContext->objectName() );

      if ( d->ip_->hasFunction( objName + "." + function ) )
        ret = d->ip_->call( function, arguments, objectContext );
      else {
        QStringList classes = d->ip_->classes( objectContext );

        for ( QStringList::const_iterator it = classes.begin(); it != classes.end(); ++it ) {
          if ( d->ip_->hasFunction( objName + "." + *it + "." + function ) ) {
            ret = d->ip_->call( "iface." + function, arguments, objectContext );
            break;
          }
        }
      }
    }
  }

  return ret;
}

QVariant AQScriptEngineQSA::call( const QString & function, const QVariantList & arguments,
                                  const QString & nameObjectContext ) const
{
  QVariant ret;

  if ( d ) {
    QObject * obj = d->loadScript( nameObjectContext );
    ret = call( function, arguments, obj );
  }

  return ret;
}

QVariant AQScriptEngineQSA::call( AQSqlCursor * cursor, const QString & function,
                                  const QVariantList & arguments, const QString & nameObjectContext ) const
{
  QVariant ret;

  if ( d ) {
    AQFormDBInterfaceAQ2 * obj = qobject_cast<AQFormDBInterfaceAQ2 *>( d->loadScript( nameObjectContext ) );

    Q_ASSERT( obj );

    obj->cursorNoUi_ = cursor;
    ret = call( function, arguments, obj );
  }

  return ret;
}

AQSqlDatabase AQScriptEngineQSA::db() const
{
  return d ? d->db_ : AQSqlDatabase();
}

AQObjectInterfaceQSA *
AQScriptEngineQSA::createInterface( const QString & className,
                                    const QVariantList & arguments,
                                    QObject * context ) const
{
  if ( d ) {
    AQObjectFactoryQSA * factory = d->factoryForClass( className );

    if ( factory )
      return factory->createInterface( className, arguments, context );
  }

  return 0;
}

AQObjectInterfaceQSA *
AQScriptEngineQSA::createInterfaceAQ2( const QString & className,
                                       const QVariantList & arguments,
                                       QObject * context ) const
{
  if ( d ) {
    AQObjectFactoryQSA * factory = d->factoryForClass( className );

    if ( factory )
      return factory->createInterfaceAQ2( className, arguments, context );
  }

  return 0;
}

AQObjectInterfaceQSA *
AQScriptEngineQSA::createInterface( AQSharedObjectQSA obj ) const
{
  if ( d && obj ) {
    AQObjectFactoryQSA * factory = d->factoryForClass( obj.data() ->metaObject() ->className() );

    if ( factory )
      return factory->createInterface( obj );
  }

  return 0;
}

AQObjectInterfaceQSA *
AQScriptEngineQSA::createInterfaceAQ2( AQSharedObjectQSA obj ) const
{
  if ( d && obj ) {
    AQObjectFactoryQSA * factory = d->factoryForClass( obj.data() ->metaObject() ->className() );

    if ( factory )
      return factory->createInterfaceAQ2( obj );
  }

  return 0;
}

AQObjectInterfaceQSA *
AQScriptEngineQSA::createInterface( QObject * obj ) const
{
  if ( d && obj ) {
    AQObjectFactoryQSA * factory = d->factoryForClass( obj->metaObject() ->className() );

    if ( factory )
      return factory->createInterface( obj );
  }

  return 0;
}

AQObjectInterfaceQSA *
AQScriptEngineQSA::createInterfaceAQ2( QObject * obj ) const
{
  if ( d && obj ) {
    AQObjectFactoryQSA * factory = d->factoryForClass( obj->metaObject() ->className() );

    if ( factory )
      return factory->createInterfaceAQ2( obj );
  }

  return 0;
}

QSWorkbench * AQScriptEngineQSA::workbench() const
{
#ifndef AQ_WUI
  if ( d && !d->wb_ && d->ip_ ) {
    QSProject * pro = d->ip_->project();

    d->wb_ = new AQWorkbench( d->scr_ );
    d->wb_->setWb( new QSWorkbench( pro ) );

    QObject::connect( pro, SIGNAL( projectEvaluated() ), d->wb_, SLOT( evaluate() ) );
  }

  return d->wb_->wb();
#else
  return 0;
#endif
}

QString AQScriptEngineQSA::connectionName() const
{
  return ( d ? d->db_.connectionName() : "default" );
}

QString AQScriptEngineQSA::aqScriptAndLineState() const
{
  return ( d && d->ip_ ? d->ip_->aqScriptAndLineState() : QString() );
}

QObject * AQScriptEngineQSA::currentContext() const
{
  return ( d && d->ip_ ? d->ip_->currentContext() : 0 );
}

void AQScriptEngineQSA::reevaluate()
{
  if ( d )
    d->reevaluate();
}
