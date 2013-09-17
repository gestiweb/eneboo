/***************************************************************************
AQManager.cpp
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

#include <QDomDocument>
#include <QDomElement>
#include <QTextStream>
#include <QSqlError>
#include <QDir>

#include <AQCore/AQCore.h>
#include <AQCore/AQTableMD.h>
#include <AQCore/AQActionMD.h>
#include <AQCore/AQFieldMD.h>
#include <AQCore/AQRelationMD.h>
#include <AQCore/AQCompKeyMD.h>
#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQShellCmd.h>

#include "AQManager.h"
#include "AQManagerModules.h"
#include "../AQSqlQuery.h"
#include "../AQSqlDatabase.h"
#include "../util/AQParameterQuery.h"
#include "../util/AQGroupByQuery.h"
#include "../shell/AQSqlShell.h"
#include "../qt3support/q3sqlcursor.h"

class AQManagerPrivate
{
public:

  AQManagerPrivate( const AQManager * mn );
  AQManagerPrivate( const AQSqlDatabase & db, AQManager * mn );
  ~AQManagerPrivate();

  /**
  Inicializacion
  */
  void init();

  /**
  Limpiar
  */
  void clear();

  /**
  Carga en la lista de tablas los nombres de las tablas de la base de datos
  */
  void loadTables();

  /**
  Crea un objeto AQFieldMD a partir de un elemento XML.

  Dado un elemento XML, que contiene la descripción de un
  campo de una tabla construye y agrega a una lista de descripciones
  de campos el objeto AQFieldMD correspondiente, que contiene
  dicha definición del campo. Tambien lo agrega a una lista de claves
  compuesta, si el campo construido pertenece a una clave compuesta.
  NO SE HACEN CHEQUEOS DE ERRORES SINTÁCTICOS EN EL XML.

  @param field Elemento XML con la descripción del campo
  @param v Valor utilizado para la propiedad visible
  @param ed Valor utilizado para la propiedad editable
  @return Objeto AqFieldMD que contiene la descripción del campo
  */
  AQFieldMD * metadataField( QDomElement * field, bool v = true, bool ed = true );

  /**
  Crea un objeto AQRelationMD a partir de un elemento XML.

  Dado un elemento XML, que contiene la descripción de una
  relación entre tablas, construye y devuelve el objeto AQRelationMD
  correspondiente, que contiene dicha definición de la relación.
  NO SE HACEN CHEQUEOS DE ERRORES SINTÁCTICOS EN EL XML.

  @param relation Elemento XML con la descripción de la relación
  @return Objeto AQRelationMD que contiene la descrición de la relación
  */
  AQRelationMD * metadataRelation( QDomElement * relation );

  /**
  Crea un objeto AQParameterQuery a partir de un elemento XML.

  Dado un elemento XML, que contiene la descripción de una
  parámetro de una consulta, construye y devuelve el objeto AQParameterQuery
  correspondiente.
  NO SE HACEN CHEQUEOS DE ERRORES SINTÁCTICOS EN EL XML.

  @param parameter Elemento XML con la descripción del parámetro de una consulta
  @return Objeto AQParameterQuery que contiene la descrición del parámetro
  */
  AQParameterQuery * queryParameter( QDomElement * parameter );

  /**
  Crea un objeto AQGroupByQuery a partir de un elemento XML.

  Dado un elemento XML, que contiene la descripción de un nivel de agrupamiento
  de una consulta, construye y devuelve el objeto AQGroupByQuery correspondiente.
  NO SE HACEN CHEQUEOS DE ERRORES SINTÁCTICOS EN EL XML.

  @param group Elemento XML con la descripción del nivel de agrupamiento de una consulta.
  @return Objeto AQGroupByQuery que contiene la descrición del nivel de agrupamiento
  */
  AQGroupByQuery * queryGroup( QDomElement * group );

  /**
  @return Clave global sha de los módulos cargados en la antigua estructura de tablas 
  de sistema de FacturaLUX. Si no existe la antigua estructura devuelve una cadena
  vacía.
  */
  QString shaGlobalFL();

  /**
  @return Clave global sha de módulos de la antigua estructura de tablas de sistema de
  FacturaLUX, correspondiente a la última actualización realizada de dichos módulos a
  la nueva estructura de tablas.
  */
  QString shaLastUpdateFL();

  /**
  Base de datos a utilizar por el manejador
  */
  AQSqlDatabase db_;

  /**
  Lista de las tablas de la base de datos, para optimizar lecturas
  */
  QStringList listTables_;

  /**
  Diccionario de claves de metadatos, para optimizar lecturas
  */
  AQStringHash hashKeyMD_;

  /**
  Lista con los nombre de las tablas del sistema
  */
  QStringList listSysTables_;

  /**
  Diccionarios de objeto AQTableMD, para optimizar lecturas
  */
  AQTableMDHash hashTableMD_;

  /**
  Diccionarios de objeto AQActionMD, para optimizar lecturas
  */
  AQActionHash hashAction_;

  /**
   Caché de valores grandes de campos
   */
  QCache <QString, QVariant> cacheLargeValues_;

  /**
  Propietario
  */
  const AQManager * mn_;
};

AQManagerPrivate::AQManagerPrivate( const AQManager * mn )
    : mn_( mn )
{
  init();
}

AQManagerPrivate::AQManagerPrivate( const AQSqlDatabase & db, AQManager * mn ) :
    db_( db ), mn_( mn )
{
  init();
}

AQManagerPrivate::~AQManagerPrivate()
{
  clear();
}

void AQManagerPrivate::init()
{
  listSysTables_ << "aqmetadata" << "aqmodules" << "aqareas" << "aqserial"
  << "aqvar" << "aqsettings" << "aqseqs" << "aqgroups" << "aqusers"
  << "aqsqldirs" << "aqsqlfiles" << "aqsqlbinfiles" << "aqsqlcharfiles";
}

void AQManagerPrivate::clear()
{
  while ( !hashTableMD_.isEmpty() ) {
    AQTableMD * tmd = hashTableMD_.begin().value();
    hashTableMD_.erase( hashTableMD_.begin() );
    delete tmd;
  }
  while ( !hashAction_.isEmpty() ) {
    AQActionMD * a = hashAction_.begin().value();
    hashAction_.erase( hashAction_.begin() );
    delete a;
  }
  hashKeyMD_.clear();
  listTables_.clear();
  listSysTables_.clear();
  cacheLargeValues_.clear();
}

QString AQManagerPrivate::shaGlobalFL()
{
  if ( !db_.dbAux().isOpen() )
    return QString();

  QStringList tables( db_.dbAux().tables() );
  if ( !tables.contains( "flserial" ) || !tables.contains( "flfiles" ) )
    return QString();

  QString sha;
  QSqlQuery q( QString(), db_.dbAux() );
  q.exec( "select sha from flserial" );
  if ( q.next() )
    sha = q.value( 0 ).toString();
  if ( sha.isEmpty() ) {
    if ( q.exec( "select sha from flfiles" ) ) {
      if ( q.next() ) {
        sha = AQCoreUtil::sha1( q.value( 0 ).toString() );
        while ( q.next() )
          sha = AQCoreUtil::sha1( sha + q.value( 0 ).toString() );
      }
    }
  }
  return sha;
}

QString AQManagerPrivate::shaLastUpdateFL()
{
  if ( !db_.dbAux().isOpen() )
    return QString();
  QString sha;
  QSqlQuery q( QString(), db_.dbAux() );
  if ( q.exec( "select valor from aqsettings where aqkey = 'shalastupdatefl'" ) )
    if ( q.next() )
      sha = q.value( 0 ).toString();
  return sha;
}

QString AQManager::shaGlobalFL() const
{
  return ( d ? d->shaGlobalFL() : QString() );
}

QString AQManager::shaLastUpdateFL() const
{
  return ( d ? d->shaLastUpdateFL() : QString() );
}

void AQManagerPrivate::loadTables()
{
  if ( db_.dbAux().isOpen() )
    listTables_ = db_.dbAux().tables();
}

bool AQManager::checkMetaData( const QString & mtd1, const QString & mtd2 ) const
{
  return ( !QString::compare( mtd1, mtd2 ) );
}

bool AQManager::init( const AQSqlDatabase & db )
{
  finish();
  d = new AQManagerPrivate( db, this );
  for ( QStringList::const_iterator it = d->listSysTables_.begin(); it != d->listSysTables_.end(); ++it ) {
    if ( !createSystemTable( *it ) ) {
      finish();
      return false;
    }
  }

  if ( shaGlobalFL() != shaLastUpdateFL() && d->db_.dbAux().isOpen() ) {

    Q3SqlCursor cur( QString(), false, db.dbAux() );
    QSqlRecord * buffer;
    QSqlQuery q( QString(), db.dbAux() );

    cur.setName( "aqsettings" );
    if ( cur.select( "aqkey = 'sharccimages'" ) && cur.next() ) {
      buffer = cur.primeUpdate();
      buffer->setValue( "valor", "VOIDVOID" );
      cur.update();

      if ( cur.lastError().isValid() )
        aqWarn( cur.lastError().text() );
    }

    q.exec( "drop table aqsqldirs" );
    createSystemTable( "aqsqldirs" );
    q.exec( "drop table aqsqlfiles" );
    createSystemTable( "aqsqlfiles" );
    q.exec( "drop table aqsqlcharfiles" );
    createSystemTable( "aqsqlcharfiles" );
    q.exec( "drop table aqsqlbinfiles" );
    createSystemTable( "aqsqlbinfiles" );

    AQSqlShell sqlSh( d->db_ );
    AQShellCmd cmd;

    cmd.cmd = "mkdir";
    cmd.argList = QStringList() << "/backup";
    sqlSh.cmd( cmd );

    QString now( QDateTime::currentDateTime().toString( "yyyymmddHHmmss" ) );
    cmd.cmd = "mkdir";
    cmd.argList = QStringList() << "/backup/modulos-" + now;
    sqlSh.cmd( cmd );

    cmd.cmd = "mv";
    cmd.argList = QStringList() << AQ_DIR_MOD << "/backup/modulos-" + now;
    sqlSh.cmd( cmd );

    cmd.cmd = "mkdir";
    cmd.argList = QStringList() << AQ_DIR_MOD;
    sqlSh.cmd( cmd );

    cmd.argList = QStringList() << AQ_DIR_MOD + "/todos";
    sqlSh.cmd( cmd );

    q.exec( "drop table aqareas" );
    createSystemTable( "aqareas" );

    if ( q.exec( "select idarea,descripcion,bloqueo from flareas" ) ) {

      while ( q.next() ) {
        cur.setName( "aqareas" );

        buffer = cur.primeInsert();
        buffer->setValue( "idarea", q.value( 0 ) );
        buffer->setValue( "descripcion", q.value( 1 ) );
        buffer->setValue( "bloqueo", q.value( 2 ) );

        cur.insert();

        if ( cur.lastError().isValid() )
          aqWarn( cur.lastError().text() );

        cmd.cmd = "mkdir";
        cmd.argList = QStringList() << AQ_DIR_MOD + "/" + q.value( 0 ).toString();
        sqlSh.cmd( cmd );
      }

    }

    q.exec( "drop table aqmodules" );
    createSystemTable( "aqmodules" );

    if ( q.exec( "select idmodulo,idarea,descripcion,version,icono,bloqueo from flmodules" ) ) {

      while ( q.next() ) {
        cur.setName( "aqmodules" );

        buffer = cur.primeInsert();
        buffer->setValue( "idmodulo", q.value( 0 ) );
        buffer->setValue( "idarea", q.value( 1 ) );
        buffer->setValue( "descripcion", q.value( 2 ) );
        buffer->setValue( "version", q.value( 3 ) );
        buffer->setValue( "icono", q.value( 4 ) );
        buffer->setValue( "bloqueo", q.value( 5 ) );
        cur.insert();

        if ( cur.lastError().isValid() )
          aqWarn( cur.lastError().text() );

        QString dirModule( AQ_DIR_MOD + "/" + q.value( 1 ).toString() + "/" +
                           q.value( 0 ).toString() + "/" );
        cmd.cmd = "mkdir";
        cmd.argList = QStringList() << dirModule;
        sqlSh.cmd( cmd );

        cmd.argList = QStringList() << dirModule + "forms";
        sqlSh.cmd( cmd );

        cmd.argList = QStringList() << dirModule + "scripts";
        sqlSh.cmd( cmd );

        cmd.argList = QStringList() << dirModule + "tables";
        sqlSh.cmd( cmd );

        cmd.argList = QStringList() << dirModule + "reports";
        sqlSh.cmd( cmd );

        cmd.argList = QStringList() << dirModule + "queries";
        sqlSh.cmd( cmd );

        cmd.argList = QStringList() << dirModule + "translations";
        sqlSh.cmd( cmd );
      }

    }

  }

  QSqlQuery q( QString(), db.dbAux() );
  if ( !q.exec( "select * from aqsettings where aqkey = 'sysmodver'" ) ) {
    q.exec( "drop table aqsettings" );
    if ( !createSystemTable( "aqsettings" ) ) {
      finish();
      return false;
    }
  }

  q.exec( "select tabla,xml from aqmetadata" );
  while ( q.next() )
    d->hashKeyMD_.insert( q.value( 0 ).toString(), q.value( 1 ).toString() );

  q.exec( "select * from aqsettings where aqkey = 'sysmodver'" );
  if ( !q.next() ) {
    q.exec( "drop table aqmetadata" );
    if ( !createSystemTable( "aqmetadata" ) ) {
      finish();
      return false;
    }
    Q3SqlCursor c( "aqmetadata", true, db.dbAux() );
    QSqlRecord * buffer;
    AQStringHash::const_iterator it( d->hashKeyMD_.begin() );
    while ( it != d->hashKeyMD_.end() ) {
      buffer = c.primeInsert();
      buffer->setValue( "tabla", it.key() );
      buffer->setValue( "xml", it.value() );
      buffer->setValue( "bloqueo", false );
      buffer->setValue( "seq", 0 );
      c.insert();
      if ( c.lastError().isValid() )
        aqWarn( c.lastError().text() );
      ++it;
    }
  }

  d->loadTables();

  return true;
}

bool AQManager::existsTable( const QString & n ) const
{
  if ( !d || !d->db_.dbAux().isOpen() || n.isEmpty() )
    return false;
  if ( !d->listTables_.isEmpty() )
    return ( d->listTables_.contains( n ) ? true : ( d->db_.dbAux().tables().contains( n ) ) );
  return ( d->db_.dbAux().tables().contains( n ) );
}

void AQManager::finish()
{
  if ( d ) {
    d->db_ = AQSqlDatabase();
    delete d;
    d = 0;
  }
}

AQTableMD * AQManager::createTable( AQTableMD * tmd ) const
{
  if ( !tmd )
    return 0;

  if ( tmd->isQuery() )
    return tmd;

  if ( !d || !d->db_.createTable( tmd ) ) {
    aqWarn( "AQManager : " + AQ_TR( "MetaData", "No se ha podido crear la tabla " )
            + tmd->name() );
    return 0;
  }

  return tmd;
}

AQTableMD * AQManager::createTable( const QString & n ) const
{
  return createTable( metadata( n ) );
}

bool AQManager::checkMetaData( const QString & n ) const
{
  if ( !d || !d->db_.dbAux().isOpen() )
    return true;

  QString shaKey, cacheKey;
  QString mtd1 = d->db_.managerModules() ->content( n + ".mtd", shaKey, cacheKey );
  Q3SqlCursor c( "aqmetadata", true, d->db_.dbAux() );
  c.setFilter( "tabla='" + n + "'" );
  c.select();
  if ( c.next() ) {
    QSqlRecord * buffer = c.editBuffer( true );
    if ( c.lastError().isValid() ) {
      aqWarn( c.lastError().text() );
      return true;
    }
    return checkMetaData( mtd1, buffer->value( "xml" ).toString() );
  } else
    return true;
}

bool AQManager::checkMetaData( AQTableMD * tmd1, AQTableMD * tmd2 ) const
{
  if ( !tmd1 || !tmd2 )
    return ( tmd1 == tmd2 );

  AQFieldMDHash fieldList( tmd1->fieldList() );
  AQFieldMD * field1;
  AQFieldMD * field2;

  AQFieldMDHash::const_iterator it( fieldList.begin() );
  while ( it != fieldList.end() ) {
    field1 = it.value();
    field2 = tmd2->field( field1->name() );
    if ( !field2 )
      return false;
    if ( field1->type() != field2->type() || field1->allowNull() != field2->allowNull() )
      return false;
    if ( field1->isUnique() != field2->isUnique() || field1->isIndex() != field2->isIndex() )
      return false;
    if ( field1->length() != field2->length() || field1->partDecimal() != field2->partDecimal()
         || field1->partInteger() != field2->partInteger() )
      return false;
    ++it;
  }

  fieldList = tmd2->fieldList();
  AQFieldMDHash::const_iterator it2( fieldList.begin() );
  while ( it2 != fieldList.end() ) {
    field1 = it2.value();
    field2 = tmd1->field( field1->name() );
    if ( !field2 )
      return false;
    if ( field1->type() != field2->type() || field1->allowNull() != field2->allowNull() )
      return false;
    if ( field1->isUnique() != field2->isUnique() || field1->isIndex() != field2->isIndex() )
      return false;
    if ( field1->length() != field2->length() || field1->partDecimal() != field2->partDecimal()
         || field1->partInteger() != field2->partInteger() )
      return false;
    ++it2;
  }

  return true;
}

AQFieldMD * AQManagerPrivate::metadataField( QDomElement * field, bool v, bool ed )
{
  Q_ASSERT( field );

  bool ck = false;
  QString n, a, ol, rX, assocBy, assocWith;
  bool aN = true, iPK = true, c = false, iNX = false, uNI = false, coun = false, oT = false, vG = true;
  int t = QVariant::Int, l = 0, pI = 4, pD = 0;
  QVariant dV;

  QDomNode no = field->firstChild();

  while ( !no.isNull() ) {
    QDomElement e = no.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == "relation" || e.tagName() == "associated" ) {
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "name" ) {
        n = e.text();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "alias" ) {
        a = e.text().mid( 30, e.text().length() - 32 );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "null" ) {
        aN = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "pk" ) {
        iPK = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "type" ) {
        if ( e.text() == "int" )
          t = QVariant::Int;
        else if ( e.text() == "uint" )
          t = QVariant::UInt;
        else if ( e.text() == "bool" )
          t = QVariant::Bool;
        else if ( e.text() == "double" )
          t = QVariant::Double;
        else if ( e.text() == "time" )
          t = QVariant::Time;
        else if ( e.text() == "date" )
          t = QVariant::Date;
        else if ( e.text() == "pixmap" )
          t = QVariant::Pixmap;
        else if ( e.text() == "string" )
          t = QVariant::String;
        else if ( e.text() == "stringlist" )
          t = QVariant::StringList;
        else if ( e.text() == "unlock" )
          t = AQCore::Unlock;
        else if ( e.text() == "serial" )
          t = AQCore::Serial;
        else if ( e.text() == "bytearray" )
          t = QVariant::ByteArray;
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "length" ) {
        l = e.text().toInt();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "regexp" ) {
        rX = e.text();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "default" ) {
        if ( e.text().contains( "QT_TRANSLATE_NOOP" ) )
          dV = QVariant( e.text().mid( 30, e.text().length() - 32 ) );
        else
          dV = QVariant( e.text() );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "outtransaction" ) {
        oT = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "counter" ) {
        coun = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "calculated" ) {
        c = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "visible" ) {
        v = ( e.text() == "true" && v );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "visiblegrid" ) {
        vG = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "editable" ) {
        ed = ( e.text() == "true" && ed );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "partI" ) {
        pI = e.text().toInt();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "partD" ) {
        pD = e.text().toInt();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "index" ) {
        iNX = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "unique" ) {
        uNI = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "ck" ) {
        ck = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "optionslist" ) {
        ol = e.text();
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  AQFieldMD * f = new AQFieldMD( n, AQ_TR( "MetaData", a ), aN, iPK, t,
                                 l, c, v, ed, pI, pD, iNX, uNI, coun, dV, oT, rX, vG, ck );

  if ( !ol.isEmpty() )
    f->setOptionsList( ol );

  no = field->firstChild();

  while ( !no.isNull() ) {
    QDomElement e = no.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == "relation" ) {
        f->addRelationMD( metadataRelation( &e ) );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "associated" ) {
        QDomNode noas = e.firstChild();
        while ( !noas.isNull() ) {
          QDomElement eas = noas.toElement();
          if ( !eas.isNull() ) {
            if ( eas.tagName() == "with" ) {
              assocWith = eas.text();
              noas = noas.nextSibling();
              continue;
            }
            if ( eas.tagName() == "by" ) {
              assocBy = eas.text();
              noas = noas.nextSibling();
              continue;
            }
          }
          noas = noas.nextSibling();
        }
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  if ( !assocWith.isEmpty() && !assocBy.isEmpty() )
    f->setAssociatedField( assocWith, assocBy );

  return f;
}

AQRelationMD * AQManagerPrivate::metadataRelation( QDomElement * relation )
{
  Q_ASSERT( relation );

  QString fT, fF;
  int rC = AQCore::RELATION_M1;
  bool dC = false, uC = false, cI = true;

  QDomNode no = relation->firstChild();

  while ( !no.isNull() ) {
    QDomElement e = no.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == "table" ) {
        fT = e.text();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "field" ) {
        fF = e.text();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "card" ) {
        if ( e.text() == "1M" ) {
          rC = AQCore::RELATION_1M;
        }
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "delC" ) {
        dC = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "updC" ) {
        uC = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "checkIn" ) {
        cI = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  return new AQRelationMD( fT, fF, rC, dC, uC, cI );
}

AQTableMD * AQManager::metadata( QDomElement * mtd, bool quick ) const
{
  Q_ASSERT( mtd );

  QString name, a, q;
  bool v = true, ed = true;
  AQTableMD * tmd = 0;

  QDomNode no = mtd->firstChild();

  while ( !no.isNull() ) {
    QDomElement e = no.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == "field" ) {
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "name" ) {
        name = e.text();
        if ( d && d->hashTableMD_.contains( name ) ) {
          tmd = d->hashTableMD_.value( name, 0 );
          /*AQAccessControlLists * acl = ( ( AQApplication * ) qApp )->acl();
               if ( acl )
                 acl->process( tmd );*/
          aqDebug( "Hit MetaData : QDomElement " + name );
          if ( tmd )
            return tmd;
        }
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "query" ) {
        q = e.text();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "alias" ) {
        a = e.text().mid( 30, e.text().length() - 32 );
        a = AQ_TR( "MetaData", a );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "visible" ) {
        v = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "editable" ) {
        ed = ( e.text() == "true" );
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  tmd = new AQTableMD( name, a, q );
  AQCompKeyMD cK;
  QStringList assocs;

  no = mtd->firstChild();

  while ( !no.isNull() ) {
    QDomElement e = no.toElement();
    if ( !e.isNull() ) {
      if ( e.tagName() == "field" ) {
        AQFieldMD * f = ( d ? d->metadataField( &e, v, ed ) : AQManagerPrivate( this ).metadataField( &e, v, ed ) );
        tmd->addFieldMD( f );
        if ( f->isCompoundKey() )
          cK.addFieldMD( f );
        if ( !f->associatedFieldName().isEmpty() ) {
          assocs.append( f->associatedFieldName() );
          assocs.append( f->associatedFieldFilterTo() );
          assocs.append( f->name() );
        }
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  tmd->setCompoundKey( cK );
  QString aWith, aBy;

  for ( QStringList::const_iterator it = assocs.begin(); it != assocs.end(); ++it ) {
    aWith = ( *it );
    ++it;
    aBy = ( *it );
    ++it;
    tmd->field( ( *it ) ) ->setAssociatedField( tmd->field( aWith ), aBy );
  }

  if ( !q.isEmpty() && !quick ) {
    AQSqlQuery * qry = query( q );
    if ( !qry ) {
      delete tmd;
      return 0;
    }
    QStringList fL = qry->fieldList();
    QString table, field;
    QString fields = tmd->fieldList( false );

    for ( QStringList::const_iterator it = fL.begin(); it != fL.end(); ++it ) {
      table = ( *it ).section( '.', 0, 0 );
      field = ( *it ).section( '.', 1, 1 );

      if ( table == name || fields.contains( field.toLower() ) )
        continue;

      AQTableMD * mtdAux = metadata( table, true );
      if ( mtdAux ) {
        AQFieldMD * fmtdAux = mtdAux->field( field );
        if ( fmtdAux ) {
          int typeAux = fmtdAux->type();
          if ( typeAux == AQCore::Serial )
            typeAux = QVariant::UInt;
          tmd->addFieldMD( new AQFieldMD( field, fmtdAux->alias(), true, false, typeAux,
                                          fmtdAux->length(), false, fmtdAux->visible(), fmtdAux->editable(), fmtdAux->partInteger(),
                                          fmtdAux->partDecimal(), false, false, false, QVariant(), false, QString(),
                                          fmtdAux->visibleGrid(), false ) );
        }
      }
    }
    delete qry;
  }

  if ( d && q.isEmpty() )
    d->hashTableMD_.insert( name, tmd );
  /*AQAccessControlLists * acl = ( ( AQApplication * ) qApp )->acl();
  if ( acl )
      acl->process( tmd );*/ 
  return tmd;
}

void AQManager::cleanupMetaData()
{
  finish();
}

AQTableMD * AQManager::metadata( const QString & n, bool quick ) const
{
  if ( n.isEmpty() || !d || !d->db_.dbAux().isOpen() )
    return 0;

  AQTableMD * ret = 0;

  if ( d->hashTableMD_.contains( n ) ) {
    ret = d->hashTableMD_.value( n, 0 );
    /*AQAccessControlLists * acl = ( ( AQApplication * ) qApp )->acl();
        if ( acl )
           acl->process( ret );*/
    aqDebug( "Hit MetaData : QString " + n );
    if ( ret )
      return ret;
  }

  QDomDocument doc( n );
  QDomElement docElem;
  QString shaKey, cacheKey;
  QString stream = d->db_.managerModules() ->content( n + ".mtd", shaKey, cacheKey );
  //bool newTable = false;

  if ( !AQCoreUtil::domDocumentSetContent( doc, stream ) ) {
    aqWarn( "AQManager::metadata : " +
            AQ_TR( "MetaData", "Error al cargar los metadatos para la tabla %1" ).arg( n ) );
    return 0;
  }

  docElem = doc.documentElement();
  ret = metadata( &docElem, quick );
  /*AQAccessControlLists * acl = ( ( AQApplication * ) qApp )->acl();
  if ( acl )
      acl->process( ret );*/ 
  return ret;
}

AQGroupByQuery * AQManagerPrivate::queryGroup( QDomElement * group )
{
  Q_ASSERT( group );

  QString level, field;
  QDomNode no = group->firstChild();

  while ( !no.isNull() ) {
    QDomElement e = no.toElement();

    if ( !e.isNull() ) {
      if ( e.tagName() == "level" ) {
        level = e.text();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "field" ) {
        field = e.text();
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  return new AQGroupByQuery( level.toInt(), field );
}

AQParameterQuery * AQManagerPrivate::queryParameter( QDomElement * parameter )
{
  Q_ASSERT( parameter );

  QString name, alias;
  int t = QVariant::Int;
  QDomNode no = parameter->firstChild();

  while ( !no.isNull() ) {
    QDomElement e = no.toElement();

    if ( !e.isNull() ) {
      if ( e.tagName() == "name" ) {
        name = e.text();
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "alias" ) {
        QString s = e.text().left( e.text().length() - 2 );

        alias = s.right( s.length() - 29 );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "type" ) {
        if ( e.text() == "int" )
          t = QVariant::Int;
        else if ( e.text() == "uint" )
          t = QVariant::UInt;
        else if ( e.text() == "bool" )
          t = QVariant::Bool;
        else if ( e.text() == "double" )
          t = QVariant::Double;
        else if ( e.text() == "time" )
          t = QVariant::Time;
        else if ( e.text() == "date" )
          t = QVariant::Date;
        else if ( e.text() == "pixmap" )
          t = QVariant::Pixmap;
        else if ( e.text() == "string" )
          t = QVariant::String;
        else if ( e.text() == "stringlist" )
          t = QVariant::StringList;
        else if ( e.text() == "serial" )
          t = AQCore::Serial;
        else if ( e.text() == "bytearray" )
          t = QVariant::ByteArray;
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  return new AQParameterQuery( name, AQ_TR( "Queries", alias ), t );
}

AQSqlQuery * AQManager::query( const QString & n ) const
{
  if ( !d || !d->db_.managerModules() ->isReady() )
    return 0;

  AQSqlQuery * q;
  QDomDocument doc( n );
  QString shaKey, cacheKey;
  if ( !AQCoreUtil::domDocumentSetContent( doc, d->db_.managerModules() ->content( n + ".qry", shaKey, cacheKey ) ) ) {
    aqWarn( "AQManager : " +
            AQ_TR( "MetaData", "Error al cargar la consulta %1" ).arg( n ) );
    return 0;
  }

  q = new AQSqlQuery( d->db_.connectionName() );

  QDomElement docElem = doc.documentElement();
  QDomNode no = docElem.firstChild();

  while ( !no.isNull() ) {
    QDomElement e = no.toElement();

    if ( !e.isNull() ) {
      if ( e.tagName() == "name" ) {
        q->setName( e.text() );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "tables" ) {
        q->setTablesList( e.text() );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "select" ) {
        q->setSelect( e.text() );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "from" ) {
        q->setFrom( e.text() );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "where" ) {
        q->setWhere( e.text() );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "param" ) {
        q->addParameter( d->queryParameter( &e ) );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "group" ) {
        q->addGroup( d->queryGroup( &e ) );
        no = no.nextSibling();
        continue;
      }
      if ( e.tagName() == "order" ) {
        q->setOrderBy( e.text() );
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  return q;
}

AQActionMD * AQManager::action( const QString & n ) const
{
  if ( !d || !d->db_.managerModules() ->isReady() )
    return 0;

  AQActionMD * a = d->hashAction_.value( n, 0 );
  if ( a ) {
    aqDebug( "Hit Action : QString " + n );
    return a;
  }

  a = new AQActionMD;

  QDomDocument doc( n );

  QStringList listModules = d->db_.managerModules() ->listAllIdModules();
  QString contentActions;
  QString shaKey, cacheKey;
  for ( QStringList::const_iterator it = listModules.begin(); it != listModules.end(); ++it ) {
    contentActions = d->db_.managerModules() ->content( ( *it ) + ".xml", shaKey, cacheKey );
    if ( contentActions.contains( "<name>" + n + "</name>" ) )
      break;
  }

  if ( !AQCoreUtil::domDocumentSetContent( doc, contentActions ) ) {
    aqWarn( "AQManager : " +
            AQ_TR( "MetaData", "Error al cargar la accion %1" ).arg( n ) );
    return 0;
  }

  QDomElement docElem = doc.documentElement();
  QDomNode no = docElem.firstChild();

  a->setName( n );
  a->setTable( n );
  while ( !no.isNull() ) {
    QDomElement e = no.toElement();

    if ( !e.isNull() ) {
      if ( e.tagName() == "action" ) {
        QDomNode no2 = e.firstChild();

        while ( !no2.isNull() ) {
          QDomElement e2 = no2.toElement();

          if ( !e2.isNull() ) {
            if ( e2.tagName() == "name" ) {
              if ( e2.text() != n )
                break;
              a->setName( e2.text() );
              no2 = no2.nextSibling();
              continue;
            }
            if ( e2.tagName() == "scriptformrecord" ) {
              a->setScriptFormRecord( e2.text() );
              no2 = no2.nextSibling();
              continue;
            }
            if ( e2.tagName() == "scriptform" ) {
              a->setScriptForm( e2.text() );
              no2 = no2.nextSibling();
              continue;
            }
            if ( e2.tagName() == "table" ) {
              a->setTable( e2.text() );
              no2 = no2.nextSibling();
              continue;
            }
            if ( e2.tagName() == "form" ) {
              a->setForm( e2.text() );
              no2 = no2.nextSibling();
              continue;
            }
            if ( e2.tagName() == "formrecord" ) {
              a->setFormRecord( e2.text() );
              no2 = no2.nextSibling();
              continue;
            }
            if ( e2.tagName() == "caption" ) {
              a->setCaption( e2.text() );
              no2 = no2.nextSibling();
              continue;
            }
            if ( e2.tagName() == "description" ) {
              //a->setCaption( e2.text() );
              no2 = no2.nextSibling();
              continue;
            }
            if ( e2.tagName() == "alias" ) {
              //a->setCaption( e2.text() );
              no2 = no2.nextSibling();
              continue;
            }
          }
          no2 = no2.nextSibling();
        }
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }

  d->hashAction_.insert( n, a );
  return a;
}

QString AQManager::formatValueLike( int t, const QVariant & v, bool upper ) const
{
  return d->db_.formatValueLike( t, v, upper );
}

QString AQManager::formatValueLike( AQFieldMD * fMD, const QVariant & v, bool upper ) const
{
  Q_ASSERT( fMD );

  return formatValueLike( fMD->type(), v, upper );
}

QString AQManager::formatAssignValueLike( const QString & fieldName, int t, const QVariant & v, bool upper ) const
{
  if ( fieldName.isEmpty() || t == QVariant::Invalid )
    return QString();

  bool isNumber = ( t == QVariant::Int || t == AQCore::Serial || t == QVariant::UInt ||
                    t == QVariant::Double || t == AQCore::Unlock || t == QVariant::Bool );
  QString formatV( formatValueLike( t, v, upper ) );

  if ( formatV.isEmpty() )
    return QString();

  QString fName( ( upper && !isNumber ? "upper(" + fieldName + ")" : fieldName ) );

  return fName + " " + formatV;
}

QString AQManager::formatAssignValueLike( const QString & fieldName, AQFieldMD * fMD, const QVariant & v, bool upper ) const
{
  Q_ASSERT( fMD );

  return formatAssignValueLike( fieldName, fMD->type(), v, upper );
}

QString AQManager::formatAssignValueLike( AQFieldMD * fMD, const QVariant & v, const bool upper ) const
{
  Q_ASSERT( fMD );

  return formatAssignValueLike( fMD->name(), fMD->type(), v, upper );
}

QString AQManager::formatValue( int t, const QVariant & v, bool upper ) const
{
  return d ? d->db_.formatValue( t, v, upper ) : QString();
}

QString AQManager::formatValue( AQFieldMD * fMD, const QVariant & v, bool upper ) const
{
  Q_ASSERT( fMD );

  return formatValue( fMD->type(), v, upper );
}

QString AQManager::formatAssignValue( const QString & fieldName, int t, const QVariant & v, bool upper ) const
{
  if ( fieldName.isEmpty() || t == QVariant::Invalid )
    return QString();

  bool isNumber = ( t == QVariant::Int || t == AQCore::Serial || t == QVariant::UInt ||
                    t == QVariant::Double || t == AQCore::Unlock || t == QVariant::Bool );
  QString formatV( formatValue( t, v, upper ) );

  if ( formatV.isEmpty() )
    return QString();

  QString fName( ( upper && !isNumber ? "upper(" + fieldName + ")" : fieldName ) );

  return fName + "=" + formatV;
}

QString AQManager::formatAssignValue( const QString & fieldName, AQFieldMD * fMD, const QVariant & v, bool upper ) const
{
  Q_ASSERT( fMD );

  return formatAssignValue( fieldName, fMD->type(), v, upper );
}

QString AQManager::formatAssignValue( AQFieldMD * fMD, const QVariant & v, bool upper ) const
{
  Q_ASSERT( fMD );

  return formatAssignValue( fMD->name(), fMD->type(), v, upper );
}

AQTableMD * AQManager::createSystemTable( const QString & n ) const
{
  AQTableMD * mtd = 0;
  QDomDocument doc( n );
  QDomElement docElem;
  QFile fi( AQDATA + "/tables/" + n + ".mtd" );
  if ( !fi.open( QIODevice::ReadOnly ) ) {
    aqWarn( "AQManager::createSystemTable : " +
            AQ_TR( "MetaData", "Los metadatos para %1 no están definidos" ).arg( n ) );
    fi.close();
  } else {
    QTextStream t;
    t.setDevice( &fi );
    QString stream = t.readAll();
    fi.close();

    if ( !AQCoreUtil::domDocumentSetContent( doc, stream ) ) {
      aqWarn( "AQManager::createSystemTable : " +
              AQ_TR( "MetaData", "Error al cargar los metadatos para la tabla %1" ).arg( n ) );
    } else {
      docElem = doc.documentElement();
      if ( !existsTable( n ) )
        mtd = createTable( metadata( &docElem, true ) );
      else
        mtd = metadata( &docElem, true );
    }
  }

  return mtd;
}

bool AQManager::isSystemTable( const QString & n ) const
{
  return ( d ? d->listSysTables_.contains( n ) : AQManagerPrivate( this ).listSysTables_.contains( n ) );
}

QString AQManager::storeLargeValue( AQTableMD * mtd, const QString & largeValue ) const
{
  if ( largeValue.left( 3 ) == "RK@" || !mtd )
    return QString();

  QString tableName( mtd->name() );
  if ( isSystemTable( tableName ) )
    return QString();

  QString tableLarge( "fllarge_" + tableName );
  if ( !existsTable( tableLarge ) ) {
    AQTableMD * mtdLarge = new AQTableMD( tableLarge, tableLarge );

    AQFieldMD * fieldLarge = new AQFieldMD( "refkey", "refkey", false, true, QVariant::String, 100 );
    mtdLarge->addFieldMD( fieldLarge );

    fieldLarge = new AQFieldMD( "sha", "sha", true, false, QVariant::String, 50 );
    mtdLarge->addFieldMD( fieldLarge );

    fieldLarge = new AQFieldMD( "contenido", "contenido", true, false, QVariant::StringList );
    mtdLarge->addFieldMD( fieldLarge );

    if ( !createTable( mtdLarge ) )
      return QString();
  }

  QString sha( AQCoreUtil::sha1( largeValue ) );
  QString refKey( "RK@" + tableName.left( 50 ) + "@" + sha );
  Q3SqlCursor curLarge( tableLarge, true, d->db_.dbAux() );
  QSqlRecord * bufLarge;

  curLarge.setFilter( "refkey='" + refKey + "'" );
  curLarge.select();

  if ( curLarge.next() ) {
    if ( curLarge.value( "sha" ).toString() != sha ) {
      bufLarge = curLarge.primeUpdate();
      bufLarge->setValue( "sha", sha );
      bufLarge->setValue( "contenido", largeValue );
      curLarge.update();
    }
  } else {
    bufLarge = curLarge.primeInsert();
    bufLarge->setValue( "refkey", refKey );
    bufLarge->setValue( "sha", sha );
    bufLarge->setValue( "contenido", largeValue );
    curLarge.insert();
  }

  return refKey;
}

QVariant AQManager::fetchLargeValue( const QString & refKey ) const
{
  if ( refKey.left( 3 ) != "RK@" )
    return QVariant();

  QString sha( refKey.section( '@', 2, 2 ) );
  QVariant * cachedV = d->cacheLargeValues_.object( sha );
  if ( cachedV )
    return QVariant( *cachedV );

  QString tableLarge( "fllarge_" + refKey.section( '@', 1, 1 ) );
  if ( !existsTable( tableLarge ) )
    return QVariant();

  QSqlQuery qryLarge( QString(), d->db_.db() );
  if ( qryLarge.exec( "SELECT contenido FROM " + tableLarge + " WHERE refkey='" + refKey + "'" ) &&
       qryLarge.next() ) {
    QVariant v( qryLarge.value( 0 ) );
    if ( v.isValid() )
      d->cacheLargeValues_.insert( sha, new QVariant( v ) );
    return v;
  }

  return QVariant();
}
