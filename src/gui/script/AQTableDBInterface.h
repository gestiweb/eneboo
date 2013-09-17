/***************************************************************************
AQTableDBInterface.h
-------------------
begin                : 11/02/2009
copyright            : (C) 2003-2009 by InfoSiAL S.L.
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

#ifndef AQTABLEDBINTERFACE_H_
#define AQTABLEDBINTERFACE_H_

#include <AQScript/AQComponentDBInterface.h>
#include <AQUi/AQTableDB.h>

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQTableDB( AQTableDB * obj )
{
  if ( !obj->parent() )
    obj->deleteLater();
}
#endif

class AQSqlCursorInterfaceAQ2;

/**
Interfaz para AQTableDB
*/
class AQTableDBInterface : public AQComponentDBInterface
{
  Q_OBJECT

public:

  QSharedPointer<AQTableDB> obj_;

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQTableDBInterface( const AQScriptEngineQSA * eng,
                      QSharedPointer<AQTableDB> aqTableDB );

  /**
  Destructor
  */
  virtual ~AQTableDBInterface();
};



/**
Interfaz de soporte para AbanQ v2
*/
class AQTableDBInterfaceAQ2 : public AQTableDBInterface
{
  Q_OBJECT

  Q_PROPERTY( QString tableName READ tableName WRITE setTableName )
  Q_PROPERTY( QString foreignField READ foreignField WRITE setForeignField )
  Q_PROPERTY( QString fieldRelation READ fieldRelation WRITE setFieldRelation )
  Q_PROPERTY( bool checkColumnEnabled READ checkColumnEnabled WRITE setCheckColumnEnabled )
  Q_PROPERTY( QString aliasCheckColumn READ aliasCheckColumn WRITE setAliasCheckColumn )
  Q_PROPERTY( bool findHidden READ findHidden WRITE setFindHidden )
  Q_PROPERTY( bool filterHidden READ filterHidden WRITE setFilterHidden )

public:

  AQTableDBInterfaceAQ2( const AQScriptEngineQSA * eng, QSharedPointer<AQTableDB> aqTableDB ) :
      AQTableDBInterface( eng, aqTableDB )
  {}

public Q_SLOTS:

  AQSqlCursorInterfaceAQ2 * cursor() const
  {
    AQ_NO_PORT_AQ2( AQTableDB::cursor );
    return 0;
  }

  QString tableName() const
  {
    AQ_NO_PORT_AQ2( AQTableDB::tableName );
    return QString();
  }

  void setTableName( const QString & )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setTableName );
  }

  QString foreignField() const
  {
    AQ_NO_PORT_AQ2( AQTableDB::foreignField );
    return QString();
  }

  void setForeignField( const QString & )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setForeignField );
  }

  QString fieldRelation() const
  {
    AQ_NO_PORT_AQ2( AQTableDB::fieldRelation );
    return QString();
  }

  void setFieldRelation( const QString & )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setFieldRelation );
  }

  void setReadOnly( bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setReadOnly );
  }

  void setEditOnly( bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setEditOnly );
  }

  void setInsertOnly( bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setInsertOnly );
  }

  void refresh()
  {
    AQ_NO_PORT_AQ2( AQTableDB::refresh );
  }

  void refresh( bool, bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::refresh );
  }

  void insertRecord()
  {
    AQ_NO_PORT_AQ2( AQTableDB::insertRecord );
  }

  void editRecord()
  {
    AQ_NO_PORT_AQ2( AQTableDB::editRecord );
  }

  void browseRecord()
  {
    AQ_NO_PORT_AQ2( AQTableDB::browseRecord );
  }

  void deleteRecord()
  {
    AQ_NO_PORT_AQ2( AQTableDB::deleteRecord );
  }

  void copyRecord()
  {
    AQ_NO_PORT_AQ2( AQTableDB::copyRecord );
  }

  void setFocus()
  {
    AQ_NO_PORT_AQ2( AQTableDB::setFocus );
  }

  void setFilter( const QString & )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setFilter );
  }

  QString filter()
  {
    AQ_NO_PORT_AQ2( AQTableDB::filter );
    return QString();
  }

  void setInitSearch( const QString & )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setInitSearch );
  }

  void emitReadOnlyChanged( bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::emitReadOnlyChanged );
  }

  void emitEditOnlyChanged( bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::emitEditOnlyChanged );
  }

  void emitInsertOnlyChanged( bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::emitInsertOnlyChanged );
  }

  void emitCurrentChanged()
  {
    AQ_NO_PORT_AQ2( AQTableDB::emitCurrentChanged );
  }

  void setCurrentRow( int )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setCurrentRow );
  }

  int currentRow( void )
  {
    AQ_NO_PORT_AQ2( AQTableDB::currentRow );
    return -1;
  }

  AQTableDB * obj()
  {
    AQ_NO_PORT_AQ2( AQTableDB::obj );
    return 0;
  }

  void putFirstCol( const QString & )
  {
    AQ_NO_PORT_AQ2( AQTableDB::putFirstCol );
  }

  void moveCol( const QString &, const QString & )
  {
    AQ_NO_PORT_AQ2( AQTableDB::moveCol );
  }

  void setOrderCols( const QStringList & )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setOrderCols );
  }

  /*QStringList*/
  void orderCols()
  {
    AQ_NO_PORT_AQ2( AQTableDB::orderCols );
  }

  bool checkColumnEnabled() const
  {
    AQ_NO_PORT_AQ2( AQTableDB::checkColumnEnabled );
    return false;
  }

  void setCheckColumnEnabled( bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setCheckColumnEnabled );
  }

  QString aliasCheckColumn() const
  {
    AQ_NO_PORT_AQ2( AQTableDB::aliasCheckColumn );
    return QString();
  }

  void setAliasCheckColumn( const QString & )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setAliasCheckColumn );
  }

  /*QStringList*/
  void primarysKeysChecked()
  {
    AQ_NO_PORT_AQ2( AQTableDB::primarysKeysChecked );
  }

  void clearChecked()
  {
    AQ_NO_PORT_AQ2( AQTableDB::clearChecked );
  }

  void setPrimaryKeyChecked( const QVariant &, bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setPrimaryKeyChecked );
  }

  void emitPrimaryKeyToggled( const QVariant &, bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::emitPrimaryKeyToggled );
  }

  bool findHidden() const
  {
    AQ_NO_PORT_AQ2( AQTableDB::findHidden );
    return false;
  }

  void setFindHidden( bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setFindHidden );
  }

  bool filterHidden() const
  {
    AQ_NO_PORT_AQ2( AQTableDB::filterHidden );
    return false;
  }

  void setFilterHidden( bool )
  {
    AQ_NO_PORT_AQ2( AQTableDB::setFilterHidden );
  }

Q_SIGNALS:

  void readOnlyChanged( bool );
  void editOnlyChanged( bool );
  void insertOnlyChanged( bool );
  void currentChanged();
  void primaryKeyToggled( const QVariant &, bool );
};

#endif /*AQTABLEDBINTERFACE_H_*/
