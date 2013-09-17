/***************************************************************************
AQSqlCursor.cpp
-------------------
begin                : 08/08/2007
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

#include <QTimer>
#include <QDateTime>
#include <QSqlField>
#include <QSqlDriver>

#include <AQCore/AQCore.h>
#include <AQCore/AQTableMD.h>
#include <AQCore/AQRelationMD.h>
#include <AQCore/AQFieldMD.h>
#include <AQCore/AQActionMD.h>
#include <AQScript/AQScriptEngineQSA.h>
#include <AQScript/AQObjectInterfaceQSA.h>
#include <AQAcl/AQAccessControl.h>
#include <AQAcl/AQAccessControlFactory.h>

#include "AQSqlCursor.h"
#include "AQSqlDatabase.h"
#include "AQSqlConnections.h"
#include "AQSqlQuery.h"
#include "util/AQSqlUtil.h"
#include "util/AQSqlSavePoint.h"
#include "manager/AQManager.h"
#include "manager/AQManagerModules.h"
#include "qt3support/q3sqlrecordinfo.h"
#include "qt3support/q3sqlindex_p.h"

#ifdef AQ_WARN
long AQSqlCursor::countRefCursor = 0;
#endif

#define DB_MGR d->db_.manager()
#define CUR_RL d->cursorMaster_
#define REL_RL d->relDetailToMaster_

class AQSqlCursorPrivate
{

public:

  AQSqlCursorPrivate();
  ~AQSqlCursorPrivate();

  /**
  Si el cursor viene de una consulta, realiza el proceso de agregar la defición
  de los campos al mismo
  */
  void populateCursor();

  /**
  Cuando el cursor viene de una consulta, realiza el proceso que marca como
  no generados (no se tienen en cuenta en INSERT, EDIT, DEL) los campos del buffer
  que no pertenecen a la tabla principal
  */
  void setNotGenerateds();

  /**
  Buffer con un registro del cursor.

  Según el modo de acceso AQSqlCursor::Mode establecido para el cusor, este buffer contendr
  el registro activo de dicho cursor listo para insertar,editar,borrar o navegar.
  */
  QSqlRecord * buffer_;

  /**
  Copia del buffer.

  Aqui se guarda una copia del AQSqlCursor::buffer_ actual mediante el metodo AQSqlCursor::updateBufferCopy().
  */
  QSqlRecord * bufferCopy_;

  /**
  Metadatos de la tabla asociada al cursor.
  */
  AQTableMD * metadata_;

  /**
  Mantiene el modo de acceso actual del cursor, ver AQSqlCursor::Mode.
  */
  AQSql::Mode modeAccess_;

  /**
  Cursor relacionado con este.
  */
  AQSqlCursor * cursorMaster_;

  /**
  Relación que determina como se relaciona con el cursor relacionado.
  */
  AQRelationMD * relDetailToMaster_;

  /**
  Filtro principal para el cursor.

  Este filtro persiste y se aplica al cursor durante toda su existencia,
  los filtros posteriores, siempre se ejecutaran unidos con 'AND' a este.
  */
  QString mainFilter_;

  /**
  Indica si estan o no activos los chequeos de integridad referencial
  */
  bool activatedCheckIntegrity_;

  /**
  Indica si estan o no activas las acciones a realiar antes y después del Commit
  */
  bool activatedCommitActions_;

  /**
  Crónometro interno
  */
  QTimer * timer_;

  /**
  Cuando el cursor proviene de una consulta indica si ya se han agregado al mismo
  la definicón de los campos que lo componen
  */
  bool populated_;

  /**
  Cuando el cursor proviene de una consulta indica el nombre de la misma
  */
  QString query_;

  /**
  Base de datos sobre la que trabaja
  */
  AQSqlDatabase db_;

  /**
  Mapa de últimas posiciones de registros utilizando el valor de la clave primaria. Sus valores se actualizan
  cada vez que se consulta la posición de un registro con AQSqlCursor::atFrom(). El mapa se reinicializa cada vez
  que se actualiza el cursor.
  */
  QMap<QString, int> mapPosByPK;

  /**
  Filtro persistente para incluir en el cursor los registros recientemente insertados aunque estos no
  cumplan los filtros principales. Esto es necesario para que dichos registros sean válidos dentro del
  cursor y así poder posicionarse sobre ellos durante los posibles refrescos que puedan producirse en
  el proceso de inserción. Este filtro se agrega a los filtros principales mediante el operador OR.
  */
  QString persistentFilter_;

  /**
  Cursor propietario
  */
  AQSqlCursor * cursor_;

  /**
  Lista de nombres de campos de la tabla que son del tipo AQCore::Unlock
  */
  QStringList fieldsNamesUnlock_;

  /**
  Nombre de la tabla de definida por los metadatos
  */
  QString mtdname_;

  /**
  Indica que hay que insertar copiando el actual, modo AQSql::INSERTCOPY
  */
  bool insertCopy_;

  /**
  Para el control de acceso dinámico en función del contenido de los registros
  */
  void doAcl();
  void undoAcl();

  AQAccessControl * acTable_;
  QString acPermTable_;
  QString acPermBackupTable_;
  QStringList acosTable_;
  QStringList acosBackupTable_;
  QString acosCondName_;
  int acosCond_;
  QVariant acosCondVal_;
  int lastAt_;
  bool aclDone_;
  int idAc_, idAcos_, idCond_;
  QString id_;

  /** Gancho al AQAction que controla el cursor, sólo para compatiblidad con AbanQ v2 */
  QObject * aqActionHandler_;
};

AQSqlCursorPrivate::AQSqlCursorPrivate() :
    buffer_( 0 ),
    bufferCopy_( 0 ),
    metadata_( 0 ),
    cursorMaster_( 0 ),
    relDetailToMaster_( 0 ),
    activatedCheckIntegrity_( true ),
    activatedCommitActions_( true ),
    timer_( 0 ),
    populated_( false ),
    insertCopy_( false ),
    acTable_( 0 ),
    lastAt_( -1 ),
    aclDone_( false ),
    idAc_( 0 ),
    idAcos_( 0 ),
    idCond_( 0 ),
    id_( "000" ),
    aqActionHandler_( 0 )
{
#ifdef AQ_WARN
  ++AQSqlCursor::countRefCursor;
#endif
}

AQSqlCursorPrivate::~AQSqlCursorPrivate()
{
  undoAcl();

  delete bufferCopy_;
  delete relDetailToMaster_;
  delete acTable_;

#ifdef AQ_WARN
  --AQSqlCursor::countRefCursor;
#endif
}

void AQSqlCursorPrivate::populateCursor()
{
  if ( metadata_->isQuery() ) {

    cursor_->Q3SqlCursor::setName( mtdname_, true );
    QStringList fieldsNames( metadata_->fieldList( false ).split( "," ) );
    int i = 0;

    for ( QStringList::const_iterator it = fieldsNames.begin(); it != fieldsNames.end(); ++it ) {

      if ( !cursor_->Q3SqlCursor::contains( *it ) ) {

        QVariant::Type type = ( QVariant::Type ) metadata_->fieldType( *it );

        if ( type == ( QVariant::Type ) AQCore::Serial )
          type = QVariant::UInt;

        cursor_->Q3SqlCursor::insert( i, Q3SqlFieldInfo( *it, type, -1,
                                      metadata_->fieldLength( *it ), -1, QVariant(), 0, true ) );
      }

      ++i;
    }

    populated_ = true;
  }
}

void AQSqlCursorPrivate::setNotGenerateds()
{
  if ( metadata_->isQuery() ) {
    Q_ASSERT( buffer_ );

    Q3SqlRecordInfo inf( static_cast<QSqlQuery *>( cursor_ ) ->record() );

    for ( Q3SqlRecordInfo::const_iterator it = inf.begin(); it != inf.end(); ++it ) {

      if ( !metadata_->field( ( *it ).name() ) ) {
        buffer_->setGenerated( ( *it ).name(), false );
        continue;
      }

      if ( !metadata_->field( ( *it ).name() ) ->generated() )
        buffer_->setGenerated( ( *it ).name(), false );
    }

  }
}

void AQSqlCursorPrivate::doAcl()
{
  if ( !acTable_ ) {
    acTable_ = AQAccessControlFactory::create( "table" );
    acTable_->setFromObject( metadata_ );
    acosBackupTable_ = acTable_->getAcos();
    acPermBackupTable_ = acTable_->perm();
    acTable_->clear();
  }

  if ( modeAccess_ == AQSql::INSERT || ( lastAt_ != -1 && lastAt_ == cursor_->at() ) )
    return ;

  lastAt_ = cursor_->at();

  if ( !acosCondName_.isEmpty() ) {

    bool condTrue_ = false;

    switch ( acosCond_ ) {
    case AQSql::VALUE:
      condTrue_ = ( cursor_->Q3SqlCursor::value( acosCondName_ ) == acosCondVal_ );
      break;
    case AQSql::REGEXP:
      condTrue_ = ( QRegExp( acosCondVal_.toString() ).exactMatch( cursor_->Q3SqlCursor::value( acosCondName_ ).toString() ) );
      break;
    case AQSql::FUNCTION: {
        buffer_ = cursor_->Q3SqlCursor::editBuffer( true );
        QObject * iface = db_.scriptEngine() ->createInterface( cursor_ );
        condTrue_ = (
                      acosCondVal_ ==
                      db_.scriptEngine() ->call( acosCondName_,
                                                 QVariantList() << qVariantFromValue( iface ), 0 )
                    );
      }
      break;
    }

    if ( condTrue_ ) {
      if ( acTable_->name() != id_ ) {
        acTable_->clear();
        acTable_->setName( id_ );
        acTable_->setPerm( acPermTable_ );
        acTable_->setAcos( acosTable_ );
        acTable_->processObject( metadata_ );
        aclDone_ = true;
      }
      return ;
    }
  } else
    if ( cursor_->isLocked() ) {
      if ( acTable_->name() != id_ ) {
        acTable_->clear();
        acTable_->setName( id_ );
        acTable_->setPerm( "r-" );
        acTable_->processObject( metadata_ );
        aclDone_ = true;
      }
      return ;
    }

  undoAcl();
}

void AQSqlCursorPrivate::undoAcl()
{
  if ( acTable_ && aclDone_ ) {
    aclDone_ = false;
    acTable_->clear();
    acTable_->setPerm( acPermBackupTable_ );
    acTable_->setAcos( acosBackupTable_ );
    acTable_->processObject( metadata_ );
  }
}

AQSqlCursor::AQSqlCursor( const QString & name, bool autopopulate,
                          const QString & connName, AQSqlCursor * cursorMaster,
                          AQRelationMD * relDetailToMaster, QObject * parent ) :
    QObject( parent ),
    Q3SqlCursor( QString(), autopopulate, AQSqlConnections::database( connName ).db() )
{
  setObjectName( name );

  d = new AQSqlCursorPrivate();
  d->db_ = AQSqlConnections::database( connName );
  d->cursor_ = this;

  if ( !DB_MGR->isReady() )
    return ;

  if ( !name.isEmpty() ) {
    if ( !DB_MGR->existsTable( name ) )
      d->metadata_ = DB_MGR->createTable( name );
    else
      d->metadata_ = DB_MGR->metadata( name );
  }

  CUR_RL = cursorMaster;
  if ( relDetailToMaster ) {
    REL_RL = new AQRelationMD( relDetailToMaster->foreignTable(), relDetailToMaster->foreignField(),
                               relDetailToMaster->cardinality(), relDetailToMaster->deleteCascade(),
                               relDetailToMaster->updateCascade(), relDetailToMaster->checkIn() );
    REL_RL->setField( relDetailToMaster->field() );
  } else
    REL_RL = 0;

  if ( !d->metadata_ )
    d->metadata_ = DB_MGR->metadata( "aqvoid" );

  d->mtdname_ = d->metadata_->name();

  if ( d->metadata_->isQuery() ) {
    AQSqlQuery * qry = DB_MGR->query( d->metadata_->query() );
    if ( qry ) {
      d->query_ = qry->sql();
      if ( !d->query_.isEmpty() )
        exec( d->query_ );
      delete qry;
    }
  } else
    Q3SqlCursor::setName( d->mtdname_, autopopulate );

  d->modeAccess_ = AQSql::BROWSE;

  d->timer_ = new QTimer( this );
  d->timer_->setSingleShot( true );
  connect( d->timer_, SIGNAL( timeout() ), SLOT( refreshDelayed() ) );

  d->fieldsNamesUnlock_ = d->metadata_->fieldsNamesUnlock();

  if ( CUR_RL && REL_RL ) {

    if ( CUR_RL->isValid() ) {
      setFilter( "" );
      Q3SqlCursor::select();
    }

    connect( CUR_RL, SIGNAL( bufferChanged( const QString & ) ), this, SLOT( refresh( const QString & ) ) );
    connect( CUR_RL, SIGNAL( newBuffer() ), this, SLOT( refresh() ) );
  }

  first( false );
}

AQSqlCursor::~AQSqlCursor()
{
  delete d;
}

void AQSqlCursor::refreshDelayed( int msec )
{
  if ( msec > 0 ) {
    QObject * obj = sender();

    if ( obj ) {
      if ( !qobject_cast<QTimer *>( obj ) ) {
        d->timer_->start( msec );
        return ;
      }
    } else {
      d->timer_->start( msec );
      return ;
    }
  }

  d->timer_->stop();

  QString cFilter( Q3SqlCursor::filter() );
  setFilter( "" );

  if ( cFilter == Q3SqlCursor::filter() && isValid() )
    return ;

  Q3SqlCursor::select();
  int pos = atFrom();

  if ( pos >= size() )
    pos = size() - 1;

  Q_EMIT newSelect();

  if ( !seek( pos, false, true ) ) {
    Q3SqlCursor::seek( QSql::BeforeFirstRow );
    d->buffer_ = 0;
    Q_EMIT newBuffer();
  } else if ( CUR_RL && REL_RL ) {
    QVariant v( valueBuffer( REL_RL->field() ) );

    if ( CUR_RL->valueBuffer( REL_RL->foreignField() ) != v )
      CUR_RL->setValueBuffer( REL_RL->foreignField(), v );
  }

}

void AQSqlCursor::refresh( const QString & fN )
{
  if ( CUR_RL && REL_RL ) {
    d->persistentFilter_ = QString();

    if ( !CUR_RL->metadata() )
      return ;

    if ( CUR_RL->metadata() ->primaryKey() == fN &&
         CUR_RL->modeAccess() == AQSql::INSERT )
      return ;

    if ( fN.isEmpty() || REL_RL->foreignField() == fN ) {
      Q3SqlCursor::seek( QSql::BeforeFirstRow );
      d->buffer_ = 0;
      refreshDelayed();
    }
  } else {
    Q3SqlCursor::select();
    int pos = atFrom();

    if ( pos >= size() )
      pos = size() - 1;

    Q_EMIT newSelect();

    if ( !seek( pos, false, true ) ) {
      Q3SqlCursor::seek( QSql::BeforeFirstRow );
      d->buffer_ = 0;
      Q_EMIT newBuffer();
    }
  }
}

bool AQSqlCursor::commitBufferCursorMaster()
{
  switch ( d->modeAccess_ ) {
  case AQSql::INSERT:
    if ( CUR_RL && REL_RL ) {

      if ( CUR_RL->metadata() && CUR_RL->modeAccess() == AQSql::INSERT ) {

        if ( !CUR_RL->commitBuffer() ) {
          d->modeAccess_ = AQSql::BROWSE;
          return false;
        }

        setFilter( "" );
        CUR_RL->refresh();
        CUR_RL->setModeAccess( AQSql::EDIT );
        CUR_RL->refreshBuffer();
      }
    }
    break;

  case AQSql::BROWSE:
  case AQSql::EDIT:
    if ( CUR_RL && REL_RL ) {

      if ( CUR_RL->metadata() && CUR_RL->modeAccess() == AQSql::INSERT ) {

        if ( !CUR_RL->commitBuffer() ) {
          d->modeAccess_ = AQSql::BROWSE;
          return false;
        }

        CUR_RL->refresh();
        CUR_RL->setModeAccess( AQSql::EDIT );
        CUR_RL->refreshBuffer();
      }
    }
    break;

  default:
    break;
  }

  return true;
}

bool AQSqlCursor::refreshBuffer()
{
  if ( !isValid() && d->modeAccess_ != AQSql::INSERT )
    return false;

  switch ( d->modeAccess_ ) {
  case AQSql::INSERTCOPY:
  case AQSql::INSERT: {
      if ( !commitBufferCursorMaster() )
        return false;

      QSqlRecord * bufferAux = 0;

      if ( d->insertCopy_ && ( d->buffer_ = editBuffer( true ) ) )
        bufferAux = new QSqlRecord( *d->buffer_ );

      d->buffer_ = primeInsert();
      d->setNotGenerateds();

      AQFieldMDHash fieldList( d->metadata_->fieldList() );

      if ( !fieldList.isEmpty() ) {
        AQFieldMD * field;
        QString fiName;
        int type;
        QVariant::Type aqtype;
        QVariant defVal;

        for ( AQFieldMDHash::const_iterator it = fieldList.begin(); it != fieldList.end(); ++it ) {
          field = *it;
          fiName = field->name();

          if ( bufferAux && !field->isPrimaryKey() )
            d->buffer_->setValue( fiName, bufferAux->value( fiName ) );
          else
            d->buffer_->setNull( fiName );

          if ( !d->buffer_->isGenerated( fiName ) )
            continue;

          type = field->type();
          aqtype = AQFieldMD::aqDecodeType( type );
          defVal = field->defaultValue();

          if ( defVal.isValid() ) {
            defVal.convert( aqtype );
            d->buffer_->setValue( fiName, defVal );
          }

          if ( field->type() == AQCore::Serial )
            d->buffer_->setValue( fiName, d->db_.nextSerialVal( d->mtdname_, fiName ).toUInt() );

          if ( field->isCounter() ) {
            QVariant siguiente( d->db_.scriptEngine() ->call( this, QLatin1String( "calculateCounter" ),
                                QVariantList() << fiName, QLatin1String( "formRecord" ) + d->mtdname_ ) );

            if ( siguiente.isValid() ) {
              d->buffer_->setValue( fiName, siguiente );
            } else {
              siguiente = AQSqlUtil::nextCounter( fiName, this );
              if ( siguiente.isValid() )
                d->buffer_->setValue( fiName, siguiente );
            }
          }
        }
      }

      delete bufferAux;

      if ( CUR_RL && REL_RL && CUR_RL->metadata() )
        setValueBuffer( REL_RL->field(), CUR_RL->valueBuffer( REL_RL->foreignField() ) );

      updateBufferCopy();
      Q_EMIT newBuffer();
    }
    break;

  case AQSql::EDIT: {
      if ( !commitBufferCursorMaster() )
        return false;

      if ( isLocked() )
        d->modeAccess_ = AQSql::BROWSE;

      d->buffer_ = primeUpdate();
      d->setNotGenerateds();

      updateBufferCopy();
      Q_EMIT newBuffer();
    }
    break;

  case AQSql::DEL:
    if ( isLocked() ) {
      aqDebug( tr( "AQSqlCursor::refreshBuffer() : Registro bloqueado, no se puede eliminar" ) );
      d->modeAccess_ = AQSql::BROWSE;
      return false;
    }

    d->buffer_ = primeDelete();
    d->setNotGenerateds();

    updateBufferCopy();
    break;

  case AQSql::BROWSE:
    d->buffer_ = editBuffer( true );
    d->setNotGenerateds();
    Q_EMIT newBuffer();
    break;
  }

  return true;
}

bool AQSqlCursor::checkIntegrity( bool showError ) const
{
  if ( !d->activatedCheckIntegrity_ )
    return true;

  if ( d->modeAccess_ == AQSql::INSERT || d->modeAccess_ == AQSql::EDIT ) {
    if ( !isModifiedBuffer() && d->modeAccess_ == AQSql::EDIT )
      return true;

    AQFieldMDHash fieldList( d->metadata_->fieldList() );
    bool checkedCK = false;
    AQFieldMD * field;
    QString msg( "\n" ), fiName, fmdName;
    QVariant s, ss;

    Q_ASSERT( d->buffer_ );

    for ( AQFieldMDHash::const_iterator it = fieldList.begin(); it != fieldList.end(); ++it ) {
      field = ( *it );
      fiName = field->name();

      if ( !d->buffer_->isGenerated( fiName ) )
        continue;

      s.clear();
      if ( !d->buffer_->isNull( fiName ) ) {
        s = d->buffer_->value( fiName );
        if ( s.toString().isEmpty() )
          s.clear();
      }

      AQFieldMD * fMD = field->associatedField();

      if ( fMD && s.isValid() && !s.isNull() ) {
        if ( !field->relationM1() ) {
          msg += "\n" + tr( "AQSqlCursor : Error en metadatos, el campo %1 tiene un campo asociado "
                            "pero no existe relación muchos a uno" ).arg( d->mtdname_ + ":" + fiName );
          continue;
        }

        AQTableMD * tMD = DB_MGR->metadata( field->relationM1() ->foreignTable() );
        fmdName = fMD->name();

        ss.clear();
        if ( !d->buffer_->isNull( fmdName ) ) {
          ss = d->buffer_->value( fmdName );
          if ( ss.toString().isEmpty() )
            ss.clear();
        }

        if ( ss.isValid() && !ss.isNull() ) {
          QString filter( DB_MGR->formatAssignValue( field->associatedFieldFilterTo(), fMD, ss, true ) );
          filter += " AND ";
          filter += DB_MGR->formatAssignValue( field->relationM1() ->foreignField(), field, s, true );

          AQSqlQuery q( d->db_.connectionName() );
          q.setTablesList( tMD->name() );
          q.setSelect( field->associatedFieldFilterTo() );
          q.setFrom( tMD->name() );
          q.setWhere( filter );
          q.setForwardOnly( true );
          q.exec();

          if ( !q.next() )
            msg += "\n" + d->mtdname_ + ":" + field->alias() +
                   tr( " : %1 no pertenece a %2" ).arg( s.toString(), ss.toString() );
          else
            d->buffer_->setValue( fmdName, q.value( 0 ) );
        } else
          msg += "\n" + d->mtdname_ + ":" + field->alias() +
                 tr( " : %1 no se puede asociar a un valor NULO" ).arg( s.toString() );
      }

      if ( d->modeAccess_ == AQSql::EDIT && d->buffer_->value( fiName ) == d->bufferCopy_->value( fiName ) )
        continue;

      if ( d->buffer_->isNull( fiName ) && !field->allowNull() && field->type() != AQCore::Serial )
        msg += "\n" + d->mtdname_ + ":" + field->alias() + tr( " : No puede ser nulo" );

      if ( field->isUnique() ) {
        QString pK( d->metadata_->primaryKey() );

        if ( !d->buffer_->isNull( pK ) && s.isValid() && !s.isNull() ) {
          QVariant pKV( d->buffer_->value( pK ) );

          AQSqlQuery q( d->db_.connectionName() );
          q.setTablesList( d->mtdname_ );
          q.setSelect( fiName );
          q.setFrom( d->mtdname_ );
          q.setWhere( DB_MGR->formatAssignValue( field, s, true ) +
                      " AND " + pK + "<>" + DB_MGR->formatValue( d->metadata_->fieldType( pK ), pKV ) );
          q.setForwardOnly( true );

          if ( q.exec() && q.next() )
            msg += "\n" + d->mtdname_ + ":" + field->alias() +
                   tr( " : Requiere valores únicos, y ya hay otro registro con el valor %1 "
                       "en este campo" ).arg( s.toString() );
        }
      }

      if ( field->isPrimaryKey() && d->modeAccess_ == AQSql::INSERT && s.isValid() && !s.isNull() ) {
        AQSqlQuery q( d->db_.connectionName() );
        q.setTablesList( d->mtdname_ );
        q.setSelect( fiName );
        q.setFrom( d->mtdname_ );
        q.setWhere( DB_MGR->formatAssignValue( field, s, true ) );
        q.setForwardOnly( true );

        if ( q.exec() && q.next() )
          msg += "\n" + d->mtdname_ + ":" + field->alias() +
                 tr( " : Es clave primaria y requiere valores únicos, y ya hay otro registro con el valor %1 "
                     "en este campo" ).arg( s.toString() );
      }

      if ( field->relationM1() && s.isValid() && !s.isNull() ) {
        if ( field->relationM1() ->checkIn() && field->relationM1() ->foreignTable() != d->mtdname_ ) {
          AQRelationMD * r = field->relationM1();
          AQTableMD * tMD = DB_MGR->metadata( r->foreignTable() );

          AQSqlQuery q( d->db_.connectionName() );
          q.setTablesList( tMD->name() );
          q.setSelect( r->foreignField() );
          q.setFrom( tMD->name() );
          q.setWhere( DB_MGR->formatAssignValue( r->foreignField(), field, s, true ) );
          q.setForwardOnly( true );
          q.exec();

          if ( !q.next() )
            msg += "\n" + d->mtdname_ + ":" + field->alias() +
                   tr( " : El valor %1 no existe en la tabla %2" ).arg( s.toString(), r->foreignTable() );
          else
            d->buffer_->setValue( fiName, q.value( 0 ) );
        }
      }

      AQFieldMDHash fieldListCK( d->metadata_->fieldListOfCompoundKey( field->name() ) );

      if ( !checkedCK && d->modeAccess_ == AQSql::INSERT ) {
        if ( !fieldListCK.isEmpty() ) {
          AQFieldMD * fieldCK;
          QString filter;
          QString fields ;
          QString valuesFields;
          QVariant sCK;

          for ( AQFieldMDHash::const_iterator it = fieldListCK.begin(); it != fieldListCK.end(); ++it ) {
            fieldCK = *it;
            sCK = d->buffer_->value( fieldCK->name() );

            if ( filter.isEmpty() )
              filter += DB_MGR->formatAssignValue( fieldCK, sCK, true );
            else
              filter += " AND " + DB_MGR->formatAssignValue( fieldCK, sCK, true );

            if ( fields.isEmpty() )
              fields += fieldCK->alias();
            else
              fields += "+" + fieldCK->alias();

            if ( valuesFields.isEmpty() )
              valuesFields += sCK.toString();
            else
              valuesFields += "+" + sCK.toString();
          }

          AQSqlQuery q( d->db_.connectionName() );
          q.setTablesList( d->mtdname_ );
          q.setSelect( fiName );
          q.setFrom( d->mtdname_ );
          q.setWhere( filter );
          q.setForwardOnly( true );

          if ( q.exec() && q.next() )
            msg += "\n" + fields +
                   tr( " : Requiere valor único, y ya hay otro registro con el valor %1" ).arg( valuesFields );
          checkedCK = true;
        }
      }
    }

    if ( msg != "\n" ) {
      if ( showError )
        aqCrit( tr( "No se puede validar el registro actual : %1" ).arg( msg ) );
      else
        aqDebug( tr( "No se puede validar el registro actual : %1" ).arg( msg ) );
      return false;
    }
  }

  if ( d->modeAccess_ == AQSql::DEL ) {
    AQFieldMDHash fieldList( d->metadata_->fieldList() );
    AQFieldMD * field;
    QString msg, fiName;
    QVariant s;

    Q_ASSERT( d->buffer_ );

    for ( AQFieldMDHash::const_iterator it = fieldList.begin(); it != fieldList.end(); ++it ) {
      field = *it;
      fiName = field->name();

      if ( !d->buffer_->isGenerated( fiName ) )
        continue;

      s.clear();
      if ( !d->buffer_->isNull( fiName ) ) {
        s = d->buffer_->value( fiName );
        if ( s.toString().isEmpty() )
          s.clear();
      }

      if ( !s.isValid() || s.isNull() )
        continue;

      AQRelationMDList relationList( field->relationList() );

      if ( !relationList.isEmpty() ) {
        AQRelationMD * r;

        for ( AQRelationMDList::const_iterator it = relationList.begin(); it != relationList.end(); ++it ) {
          r = *it;

          if ( !r->checkIn() )
            continue;

          AQTableMD * mtd = DB_MGR->metadata( r->foreignTable() );
          AQFieldMD * f;

          if ( mtd && ( f = mtd->field( r->foreignField() ) ) ) {

            if ( !f->relationM1() )
              continue;

            if ( f->relationM1() ->deleteCascade() ||
                 !f->relationM1() ->checkIn() )
              continue;

          } else {

            aqDebug( tr( "AQSqlCursor : Error en metadatos, %1.%2 no es válido" )
                     .arg( r->foreignTable() ).arg( r->foreignField() ) );
            continue;

          }

          AQSqlQuery q( d->db_.connectionName() );
          q.setTablesList( mtd->name() );
          q.setSelect( r->foreignField() );
          q.setFrom( mtd->name() );
          q.setWhere( DB_MGR->formatAssignValue( r->foreignField(), field, s, true ) );
          q.setForwardOnly( true );

          if ( q.exec() && q.next() )
            msg += "\n" + d->mtdname_ + ":" + field->alias() +
                   tr( " : Con el valor %1 hay registros en la tabla %2:%3" ).arg( s.toString(), mtd->name(), mtd->alias() );
        }
      }
    }

    if ( !msg.isEmpty() ) {
      if ( showError )
        aqCrit( tr( "No se puede borrar registro : %1" ).arg( msg ) );
      else
        aqDebug( tr( "No se puede borrar registro : %1" ).arg( msg ) );
      return false;
    }
  }

  return true;
}

bool AQSqlCursor::commitBuffer( bool emite )
{
  if ( !checkIntegrity() )
    return false;

  if ( d->modeAccess_ == AQSql::EDIT || d->modeAccess_ == AQSql::INSERT ) {
    AQFieldMDHash fieldList( d->metadata_->fieldList() );
    AQFieldMD * field;

    Q_ASSERT( d->buffer_ );

    for ( AQFieldMDHash::const_iterator it = fieldList.begin(); it != fieldList.end(); ++it ) {
      field = *it;

      if ( !d->buffer_->isGenerated( field->name() ) )
        continue;

      if ( field->calculated() ) {
        QVariant v( d->db_.scriptEngine() ->call( this, QLatin1String( "calculateField" ),
                    QVariantList() << field->name(), QLatin1String( "formRecord" ) + d->mtdname_ ) );

        if ( v.isValid() )
          setValueBuffer( field->name(), v );
      }
    }
  }

  QString functionBefore;
  QString functionAfter;
  QString idMod;

  if ( d->db_.managerModules() ->isReady() && d->modeAccess_ != AQSql::BROWSE && d->activatedCommitActions_ ) {
    idMod = d->db_.managerModules() ->idModuleOfFile( d->mtdname_ + ".mtd" );

    if ( idMod.isEmpty() )
      idMod = "sys";

    functionBefore = idMod + ".beforeCommit_" + d->mtdname_;
    functionAfter = idMod + ".afterCommit_" + d->mtdname_;

    QObject * iface = d->db_.scriptEngine() ->createInterface( this );
    QVariant v( d->db_.scriptEngine() ->call( functionBefore,
                QVariantList() << qVariantFromValue( iface ), idMod ) );

    if ( v.isValid() && !v.toBool() )
      return false;
  }

  if ( !checkIntegrity() )
    return false;

  QString pKN( d->metadata_->primaryKey() ), pKWhere;
  bool updated = false;
  AQSqlSavePoint savePoint( d->db_.connectionName() );

  switch ( d->modeAccess_ ) {
  case AQSql::INSERT: {

      if ( CUR_RL && REL_RL && CUR_RL->metadata() )
        setValueBuffer( REL_RL->field(),
                        CUR_RL->valueBuffer( REL_RL->foreignField() ) );

      QString pKWhere( DB_MGR->formatAssignValue( d->metadata_->field( pKN ), valueBuffer( pKN ) ) );

      insert( false );

      if ( !functionAfter.isEmpty() && d->activatedCommitActions_ )
        savePoint.saveInsert( pKN, d->buffer_, this );

      if ( !d->persistentFilter_.contains( pKWhere ) )
        d->persistentFilter_ = ( d->persistentFilter_.isEmpty() ?
                                 pKWhere : d->persistentFilter_ + " OR " + pKWhere );

      updated = true;
    }
    break;

  case AQSql::EDIT:
    if ( !functionAfter.isEmpty() && d->activatedCommitActions_ )
      savePoint.saveEdit( pKN, d->bufferCopy_, this );

    if ( isModifiedBuffer() ) {
      Q_ASSERT( d->buffer_ );

      for ( int i = 0; i < d->buffer_->count(); ++i ) {
        if ( ( d->buffer_->value( i ) == d->bufferCopy_->value( i ) ) &&
             ( d->buffer_->isNull( i ) == d->bufferCopy_->isNull( i ) ) )
          d->buffer_->setGenerated( i, false );
      }

      update( false );

      for ( int i = 0; i < d->buffer_->count(); ++i )
        d->buffer_->setGenerated( i, true );

      updated = true;
      d->setNotGenerateds();
    }
    break;

  case AQSql::DEL:
      if ( !functionAfter.isEmpty() && d->activatedCommitActions_ )
        savePoint.saveDel( pKN, d->bufferCopy_, this );

    del( false );
    updated = true;
    break;

  default: ;
  }

  if ( d->modeAccess_ != AQSql::BROWSE && !functionAfter.isEmpty() && d->activatedCommitActions_ ) {
    QObject * iface = d->db_.scriptEngine() ->createInterface( this );
    QVariant v( d->db_.scriptEngine() ->call( functionAfter,
                QVariantList() << qVariantFromValue( iface ), idMod ) );

    if ( v.isValid() && !v.toBool() ) {
      savePoint.undo();
      return false;
    }
  }

  d->modeAccess_ = AQSql::BROWSE;

  if ( updated ) {
    setFilter( "" );
    d->mapPosByPK.clear();
    Q3SqlCursor::seek( atFrom() );
  }

  if ( updated && emite )
    Q_EMIT cursorUpdated();

  Q_EMIT bufferCommited();

  return true;
}

bool AQSqlCursor::seek( int i, bool relative, bool emite )
{
  if ( d->modeAccess_ != AQSql::DEL && Q3SqlCursor::seek( i, relative ) ) {
    if ( d->modeAccess_ == AQSql::INSERT )
      d->modeAccess_ = AQSql::BROWSE;

    if ( emite )
      Q_EMIT currentChanged( at() );

    return refreshBuffer();
  }

  return false;
}

bool AQSqlCursor::next( bool emite )
{
  if ( d->modeAccess_ != AQSql::DEL && Q3SqlCursor::next() ) {
    if ( d->modeAccess_ == AQSql::INSERT )
      d->modeAccess_ = AQSql::BROWSE;

    if ( emite )
      Q_EMIT currentChanged( at() );

    return refreshBuffer();
  }

  return false;
}

bool AQSqlCursor::prev( bool emite )
{
  if ( d->modeAccess_ != AQSql::DEL && Q3SqlCursor::prev() ) {
    if ( d->modeAccess_ == AQSql::INSERT )
      d->modeAccess_ = AQSql::BROWSE;

    if ( emite )
      Q_EMIT currentChanged( at() );

    return refreshBuffer();
  }

  return false;
}

bool AQSqlCursor::first( bool emite )
{
  if ( d->modeAccess_ != AQSql::DEL && Q3SqlCursor::first() ) {
    if ( d->modeAccess_ == AQSql::INSERT )
      d->modeAccess_ = AQSql::BROWSE;

    if ( emite )
      Q_EMIT currentChanged( at() );

    return refreshBuffer();
  }

  return false;
}

bool AQSqlCursor::last( bool emite )
{
  if ( d->modeAccess_ != AQSql::DEL && Q3SqlCursor::last() ) {
    if ( d->modeAccess_ == AQSql::INSERT )
      d->modeAccess_ = AQSql::BROWSE;

    if ( emite )
      Q_EMIT currentChanged( at() );

    return refreshBuffer();
  }

  return false;
}

int AQSqlCursor::del( bool invalidate )
{
  if ( !DB_MGR->isReady() )
    return -1;

  d->db_.scriptEngine() ->call( this, QLatin1String( "recordDelBefore" ) + d->mtdname_,
                                QVariantList(), QLatin1String( "form" ) + d->mtdname_ );

  AQFieldMDHash fieldList( d->metadata_->fieldList() );
  AQFieldMD * field;
  QString fiName;
  QVariant s;

  Q_ASSERT( d->buffer_ );

  for ( AQFieldMDHash::const_iterator it = fieldList.begin(); it != fieldList.end(); ++it ) {
    field = *it;
    fiName = field->name();

    if ( !d->buffer_->isGenerated( fiName ) )
      continue;

    s.clear();
    if ( !d->buffer_->isNull( fiName ) ) {
      s = d->buffer_->value( fiName );
      if ( s.toString().isEmpty() )
        s.clear();
    }

    if ( !s.isValid() || s.isNull() )
      continue;

    AQRelationMDList relationList( field->relationList() );

    if ( !relationList.isEmpty() ) {
      AQRelationMD * r;

      for ( int j = 0; j < relationList.size(); ++j ) {
        r = relationList.at( j );

        AQSqlCursor c( r->foreignTable() );
        AQFieldMD * f = c.metadata() ->field( r->foreignField() );

        if ( f && f->relationM1() && f->relationM1() ->deleteCascade() ) {
          c.select( DB_MGR->formatAssignValue( r->foreignField(), f, s, true ) );

          while ( c.next() ) {
            c.setModeAccess( AQSql::DEL );
            c.refreshBuffer();
            if ( !c.commitBuffer( false ) )
              return false;
          }
        }
      }
    }

  }

  int r = Q3SqlCursor::del( invalidate );

  d->db_.scriptEngine() ->call( this, QLatin1String( "recordDelAfter" ) + d->mtdname_,
                                QVariantList(), QLatin1String( "form" ) + d->mtdname_ );

  return r;
}

void AQSqlCursor::setValueBuffer( const QString & fN, const QVariant & v )
{
  if ( !DB_MGR->isReady() || fN.isEmpty() )
    return ;

  if ( !d->buffer_ ) {
    Q3SqlCursor::seek( QSql::BeforeFirstRow );
    return ;
  }

  AQFieldMD * field = d->metadata_->field( fN );
  if ( !field ) {
    aqDebug( tr( "AQSqlCursor::setValueBuffer() : No existe el campo " ) + d->mtdname_ + ":" + fN );
    return ;
  }

  int type = field->type();
  QVariant::Type aqtype = AQFieldMD::aqDecodeType( type );
  QVariant vv( v );

  if ( vv.type() == QVariant::Bool && aqtype != QVariant::Bool )
    vv.clear();

  if ( vv.isValid() ) {
    vv.convert( aqtype );
    if ( !vv.isNull() && type == QVariant::Pixmap ) {
      QString largeValue( vv.toString() );
      largeValue = DB_MGR->storeLargeValue( d->metadata_, largeValue );
      if ( !largeValue.isEmpty() )
        vv = largeValue;
    }
  }

  if ( vv.isValid() )
    vv.convert( aqtype );
  d->buffer_->setValue( fN, vv );

  Q_EMIT bufferChanged( fN );
}

QVariant AQSqlCursor::valueBuffer( const QString & fN ) const
{
  if ( !DB_MGR->isReady() || fN.isEmpty() )
    return QVariant();

  if ( !d->buffer_ )
    return QVariant();

  AQFieldMD * field = d->metadata_->field( fN );
  if ( !field ) {
    aqDebug( tr( "AQSqlCursor::valueBuffer() : No existe el campo " ) + d->mtdname_ + ":" + fN );
    return QVariant();
  }

  int type = field->type();
  QVariant::Type aqtype = AQFieldMD::aqDecodeType( type );
  if ( d->buffer_->isNull( fN ) &&
       ( type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt ) )
    return 0;

  QVariant v( d->buffer_->value( fN ) );

  if ( v.isValid() )
    v.convert( aqtype );

  if ( !v.isNull() && type == QVariant::Pixmap ) {
    QVariant vLarge( DB_MGR->fetchLargeValue( v.toString() ) );
    if ( vLarge.isValid() )
      return vLarge;
  }

  return v;
}

QVariant AQSqlCursor::valueBufferCopy( const QString & fN ) const
{
  if ( !DB_MGR->isReady() || fN.isEmpty() )
    return QVariant();

  if ( !d->bufferCopy_ )
    return QVariant();

  AQFieldMD * field = d->metadata_->field( fN );
  if ( !field ) {
    aqDebug( tr( "AQSqlCursor::setValueBufferCopy() : No existe el campo " ) + d->mtdname_ + ":" + fN );
    return QVariant();
  }

  int type = field->type();
  QVariant::Type aqtype = AQFieldMD::aqDecodeType( type );
  if ( d->bufferCopy_->isNull( fN ) &&
       ( type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt ) )
    return 0;

  QVariant v( d->bufferCopy_->value( fN ) );

  if ( v.isValid() )
    v.convert( aqtype );

  if ( !v.isNull() && type == QVariant::Pixmap ) {
    QVariant vLarge( DB_MGR->fetchLargeValue( v.toString() ) );
    if ( vLarge.isValid() )
      return vLarge;
  }

  return v;
}

bool AQSqlCursor::fieldDisabled( const QString & fN ) const
{
  if ( ( d->modeAccess_ == AQSql::INSERT || d->modeAccess_ == AQSql::EDIT ) &&
       CUR_RL && REL_RL )
    return ( REL_RL->field().toLower() == fN.toLower() );

  return false;
}

bool AQSqlCursor::select( const QString & filter, const QSqlIndex & sort )
{
  if ( !DB_MGR->isReady() )
    return false;

  QString f( filter ), bFilter( baseFilter() );
  QString finalFilter;

  if ( bFilter.isEmpty() )
    finalFilter = f;
  else {
    if ( filter.isEmpty() || bFilter.contains( f ) )
      finalFilter = bFilter;
    else
      finalFilter = bFilter + " AND " + f;
  }

  QStringList fields;
  if ( d->metadata_->isQuery() )
    fields = sqlIndexToStringList( sort, d->mtdname_, false );
  else
    fields = sqlIndexToStringList( sort, QString(), false );

  finalFilter = finalFilter.simplified();

  while ( finalFilter.endsWith( ";" ) )
    finalFilter.truncate( finalFilter.length() - 1 );

  int posOrderBy = finalFilter.lastIndexOf( "ORDER BY", -1, Qt::CaseInsensitive );
  QString fieldsOrderBy;

  if ( posOrderBy != -1 ) {
    fieldsOrderBy = finalFilter.mid( posOrderBy + 8, finalFilter.length() - 8 );
    finalFilter.remove( posOrderBy, finalFilter.length() - 8 );
    if ( !fieldsOrderBy.isEmpty() )
      fields += fieldsOrderBy.split( "," );
  }

  if ( d->metadata_ && !sort.contains( d->metadata_->primaryKey() ) &&
       !fieldsOrderBy.toUpper().contains( d->metadata_->primaryKey().toUpper() ) )
    fields.append( d->metadata_->primaryKey() );

  if ( d->metadata_->isQuery() ) {
    AQSqlQuery * qry = DB_MGR->query( d->metadata_->query() );
    AQTableMD * mtdAux = DB_MGR->metadata( d->mtdname_, true );
    QStringList fL( mtdAux->fieldList( false ).split( ',' ) );

    for ( QStringList::const_iterator it = fL.begin(); it != fL.end(); ++it ) {
      if ( ! ( *it ).contains( "." ) ) {
        finalFilter.replace( d->mtdname_ + "." + *it, *it );
        finalFilter.replace( QRegExp( "([\\W])" + *it + "([\\W])" ), "\\1" + d->mtdname_ + "." + *it + "\\2" );
      }
    }

    if ( qry ) {
      QString where( qry->where() );

      if ( where.isEmpty() )
        qry->setWhere( finalFilter );
      else {
        if ( finalFilter.isEmpty() )
          qry->setWhere( where );
        else
          qry->setWhere( where + " AND " + finalFilter );
      }

      fields = sqlIndexToStringList( sort );
      fL = qry->fieldList();
      for ( QStringList::const_iterator it = fields.begin(); it != fields.end(); ++it ) {
        for ( QStringList::const_iterator it2 = fL.begin(); it2 != fL.end(); ++it2 )
          if ( ( *it2 ).section( '.', 1, 1 ) == ( *it ).section( ' ', 0, 0 ) ) {
            if ( !fieldsOrderBy.isEmpty() )
              fieldsOrderBy += "," + ( *it2 ) + " " + ( *it ).section( ' ', 1, 1 );
            else
              fieldsOrderBy += ( *it2 ) + " " + ( *it ).section( ' ', 1, 1 );
            break;
          }
      }

      if ( !fieldsOrderBy.isEmpty() )
        qry->setOrderBy( fieldsOrderBy );

      d->query_ = qry->sql();
      delete qry;
    }

    bool ret = exec( d->query_ );

    Q_EMIT newSelect();

    return ret;
  }

  QSqlIndex idx;
  for ( QStringList::const_iterator it = fields.begin(); it != fields.end(); ++it )
    idx.append( QSqlField( *it , ( QVariant::Type ) d->metadata_->fieldType( *it ) ) );

  bool ret = Q3SqlCursor::select( finalFilter, idx );

  Q_EMIT newSelect();

  return ret;
}

void AQSqlCursor::setSort( const QSqlIndex& sort )
{
  QStringList fields;
  QSqlIndex idx;

  if ( d->metadata_->isQuery() )
    fields = sqlIndexToStringList( sort, d->mtdname_, false );
  else
    fields = sqlIndexToStringList( sort, QString(), false );

  if ( !fields.contains( d->metadata_->primaryKey() ) )
    fields.append( d->metadata_->primaryKey() );

  for ( QStringList::const_iterator it = fields.begin(); it != fields.end(); ++it )
    idx.append( QSqlField( *it , ( QVariant::Type ) d->metadata_->fieldType( *it ) ) );

  Q3SqlCursor::setSort( idx );
}

int AQSqlCursor::size()
{
  return Q3SqlCursor::size();
}

QString AQSqlCursor::baseFilter()
{
  QString relationFilter, finalFilter;

  if ( DB_MGR->isReady() && CUR_RL && REL_RL &&
       d->metadata_ && CUR_RL->metadata() ) {

    AQFieldMD * field = d->metadata_->field( REL_RL->field() );

    if ( field ) {
      QVariant fgValue( CUR_RL->valueBuffer( REL_RL->foreignField() ) );
      relationFilter = DB_MGR->formatAssignValue( field, fgValue, true );
    } else
      aqDebug( tr( "AQSqlCursor::baseFilter() : Relación no definida o incorrecta %1<->%2" )
               .arg( d->mtdname_ ).arg( CUR_RL->metadata() ->name() ) );
  }

  if ( !d->mainFilter_.isEmpty() )
    finalFilter = d->mainFilter_;

  if ( !relationFilter.isEmpty() && !finalFilter.contains( relationFilter ) ) {
    if ( finalFilter.isEmpty() )
      finalFilter = relationFilter;
    else
      finalFilter += " AND " + relationFilter;
  }

  return finalFilter;
}

QString AQSqlCursor::curFilter()
{
  QString f( Q3SqlCursor::filter() ), bFilter( baseFilter() );

  while ( f.endsWith( ";" ) )
    f.truncate( f.length() - 1 );
  if ( bFilter.isEmpty() )
    return f;
  else {
    if ( f.isEmpty() || bFilter.contains( f ) )
      return bFilter;
    else
      return bFilter + " AND " + f;
  }
}

void AQSqlCursor::setMainFilter( const QString & f )
{
  d->mainFilter_ = f;
  refresh();
}

void AQSqlCursor::setFilter( const QString & filter )
{
  QString finalFilter( filter ), bFilter( baseFilter() );

  if ( !bFilter.isEmpty() ) {
    if ( finalFilter.isEmpty() || bFilter.contains( finalFilter ) )
      finalFilter = bFilter;
    else
      finalFilter = bFilter + " AND " + finalFilter;
  }

  if ( !finalFilter.isEmpty() && !d->persistentFilter_.isEmpty() &&
       !finalFilter.contains( d->persistentFilter_ ) )
    finalFilter += " OR " + d->persistentFilter_;

  Q3SqlCursor::setFilter( finalFilter );
}

void AQSqlCursor::updateBufferCopy() const
{
  delete d->bufferCopy_;

  Q_ASSERT( d->buffer_ );

  d->bufferCopy_ = new QSqlRecord( *d->buffer_ );
}

bool AQSqlCursor::isModifiedBuffer() const
{
  if ( !d->buffer_ || !d->bufferCopy_ )
    return false;

  for ( int i = 0; i < d->buffer_->count(); ++i ) {
    if ( ( d->buffer_->value( i ) != d->bufferCopy_->value( i ) ) ||
         ( d->buffer_->isNull( i ) != d->bufferCopy_->isNull( i ) ) )
      return true;
  }

  return false;
}

void AQSqlCursor::setUnLock( const QString & fN, bool v )
{
  if ( d->modeAccess_ != AQSql::BROWSE )
    return ;

  if ( d->metadata_->fieldType( fN ) != AQCore::Unlock ) {
    aqDebug( tr( "AQSqlCursor::setUnLock sólo permite modificar campos del tipo Unlock" ) );
    return ;
  }

  d->buffer_ = primeUpdate();
  d->buffer_->setValue( fN, v );
  update();
  refreshBuffer();
}

bool AQSqlCursor::isLocked() const
{
  if ( d->modeAccess_ != AQSql::INSERT && !d->fieldsNamesUnlock_.isEmpty() ) {
    for ( QStringList::const_iterator it = d->fieldsNamesUnlock_.begin();
          it != d->fieldsNamesUnlock_.end(); ++it ) {
      if ( !Q3SqlCursor::value( *it ).toBool() )
        return true;
    }
  }
  return ( CUR_RL ? CUR_RL->isLocked() : false );
}

bool AQSqlCursor::bufferIsNull( int i ) const
{
  return ( d->buffer_ ? d->buffer_->isNull( i ) : true );
}

bool AQSqlCursor::bufferIsNull( const QString & name ) const
{
  return ( d->buffer_ ? d->buffer_->isNull( name ) : true );
}

void AQSqlCursor::bufferSetNull( int i ) const
{
  if ( d->buffer_ )
    d->buffer_->setNull( i );
}

void AQSqlCursor::bufferSetNull( const QString & name ) const
{
  if ( d->buffer_ )
    d->buffer_->setNull( name );
}

bool AQSqlCursor::bufferCopyIsNull( int i ) const
{
  return ( d->bufferCopy_ ? d->bufferCopy_->isNull( i ) : true );
}

bool AQSqlCursor::bufferCopyIsNull( const QString & name ) const
{
  return ( d->bufferCopy_ ? d->bufferCopy_->isNull( name ) : true );
}

void AQSqlCursor::bufferCopySetNull( int i ) const
{
  if ( d->bufferCopy_ )
    d->bufferCopy_->setNull( i );
}

void AQSqlCursor::bufferCopySetNull( const QString & name ) const
{
  if ( d->bufferCopy_ )
    d->bufferCopy_->setNull( name );
}

int AQSqlCursor::atFrom()
{
  if ( !isValid() || !DB_MGR->isReady() )
    return 0;

  if ( !d->buffer_ ) {
    Q3SqlCursor::seek( QSql::BeforeFirstRow );
    return 0;
  }

  QString pKN( d->metadata_->primaryKey() );
  QVariant pKValue( valueBuffer( pKN ) );

  if ( d->mapPosByPK.contains( pKValue.toString() ) )
    return d->mapPosByPK.value( pKValue.toString() );

  int pos = d->db_.atFrom( this );

  if ( pos == -99 ) {
    QSqlQuery q( QString(), d->db_.db() );
    QString sql, sqlIn, cFilter( curFilter() );
    AQFieldMD * field = d->metadata_->field( pKN );

    if ( !d->metadata_->isQuery() || pKN.contains( "." ) ) {
      sql = "SELECT " + pKN + " FROM " + d->mtdname_;
    } else {
      AQSqlQuery * qry = DB_MGR->query( d->metadata_->query() );
      if ( qry ) {
        sql = "SELECT " + d->mtdname_ + "." + pKN + " FROM " + qry->from();
        delete qry;
      }
    }
    if ( ! curFilter().isEmpty() )
      sql += " WHERE " + curFilter();

    if ( field ) {
      if ( !cFilter.isEmpty() )
        sqlIn = sql + " AND " + DB_MGR->formatAssignValue( field, pKValue );
      else
        sqlIn = sql + " WHERE " + DB_MGR->formatAssignValue( field, pKValue );

      if ( q.exec( sqlIn ) && !q.next() ) {
        Q3SqlCursor::seek( at() );
        pos = isValid() ? at() : 0;
        return pos;
      }
    }

    if ( sort().count() > 0 )
      sql += " ORDER BY " + sqlIndexToString( sort() );

    bool found = false;
    pos = 0;

    if ( q.exec( sql ) && q.first() ) {
      if ( q.value( 0 ) != pKValue ) {
        pos = q.size();
        if ( q.last() && pos > 1 ) {
          --pos;
          if ( q.value( 0 ) != pKValue ) {
            while ( q.previous() && pos > 1 ) {
              --pos;
              if ( q.value( 0 ) == pKValue ) {
                found = true;
                break;
              }
            }
          } else
            found = true;
        }
      } else
        found = true;
    }

    if ( !found ) {
      Q3SqlCursor::seek( at() );
      pos = isValid() ? at() : 0;
    } else {
      d->mapPosByPK.remove( pKValue.toString() );
      d->mapPosByPK.insert( pKValue.toString(), pos );
    }

    return pos;
  }

  d->mapPosByPK.remove( pKValue.toString() );
  d->mapPosByPK.insert( pKValue.toString(), pos );
  return pos;
}

int AQSqlCursor::atFromBinarySearch( const QString & fN, const QString & v, bool orderAsc )
{
  if ( !Q3SqlCursor::contains( fN ) || !Q3SqlCursor::isActive() )
    return -1;

  bool isForward = Q3SqlCursor::isForwardOnly();
  int ret = -1, atBackup = Q3SqlCursor::at();
  int ini = 0, fin = Q3SqlCursor::size() - 1, mid, cmp;

  Q3SqlCursor::setForwardOnly( false );
  while ( ini <= fin ) {
    mid = ( ini + fin ) / 2;
    Q3SqlCursor::seek( mid );
    cmp = ( orderAsc ?
            v.localeAwareCompare( Q3SqlCursor::value( fN ).toString() ) :
            Q3SqlCursor::value( fN ).toString().localeAwareCompare( v ) );
    if ( !cmp ) {
      ret = mid;
      break;
    } else {
      if ( cmp > 0 )
        ini = mid + 1;
      else
        fin = mid - 1;
    }
    ret = ini;
  }
  Q3SqlCursor::seek( atBackup );
  Q3SqlCursor::setForwardOnly( isForward );

  return ret;
}

bool AQSqlCursor::exec( const QString & query )
{
  bool ret = Q3SqlCursor::exec( query );
  if ( ret && d->metadata_->isQuery() && ( d->query_ != query || !d->populated_ ) ) {
    d->query_ = query;
    Q3SqlCursor::clear();
    d->populated_ = false;
    d->populateCursor();
  }
  return ret;
}

AQSqlDatabase AQSqlCursor::db() const
{
  return d->db_;
}

const AQTableMD * AQSqlCursor::metadata() const
{
  return d->metadata_;
}

AQTableMD * AQSqlCursor::metadata()
{
  d->doAcl();
  return d->metadata_;
}

QString AQSqlCursor::mainFilter() const
{
  return d->mainFilter_;
}

AQSql::Mode AQSqlCursor::modeAccess() const
{
  return d->modeAccess_;
}

void AQSqlCursor::setModeAccess( AQSql::Mode m )
{
  if ( m == AQSql::INSERTCOPY ) {
    d->insertCopy_ = true;
    d->modeAccess_ = AQSql::INSERT;
  } else {
    d->insertCopy_ = false;
    d->modeAccess_ = m;
  }
}

void AQSqlCursor::setActivatedCheckIntegrity( bool a ) const
{
  d->activatedCheckIntegrity_ = a;
}

void AQSqlCursor::setActivatedCommitActions( bool a ) const
{
  d->activatedCommitActions_ = a;
}

AQSqlCursor * AQSqlCursor::cursorMaster() const
{
  return CUR_RL;
}

AQRelationMD * AQSqlCursor::relDetailToMaster() const
{
  return REL_RL;
}

void AQSqlCursor::setAcTable( const QString & ac )
{
  d->idAc_++;
  d->id_ = QString::number( d->idAc_ ) + QString::number( d->idAcos_ ) +
           QString::number( d->idCond_ );
  d->acPermTable_ = ac;
}

void AQSqlCursor::setAcosTable( const QStringList & acos )
{
  d->idAcos_++;
  d->id_ = QString::number( d->idAc_ ) + QString::number( d->idAcos_ ) +
           QString::number( d->idCond_ );
  d->acosTable_ = acos;
}

void AQSqlCursor::setAcosCondition( const QString & condName, AQSql::AcosConditionEval cond,
                                    const QVariant & condVal )
{
  d->idCond_++;
  d->id_ = QString::number( d->idAc_ ) + QString::number( d->idAcos_ ) +
           QString::number( d->idCond_ );
  d->acosCondName_ = condName;
  d->acosCond_ = cond;
  d->acosCondVal_ = condVal;
}

void AQSqlCursor::setAQActionHandler( QObject * aqActionHandler )
{
  d->aqActionHandler_ = aqActionHandler;
}

QObject * AQSqlCursor::aqActionHandler() const
{
  return d->aqActionHandler_;
}
