/***************************************************************************
AQFieldDBInterface.h
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

#ifndef AQFIELDDBINTERFACE_H_
#define AQFIELDDBINTERFACE_H_

#include <AQScript/AQComponentDBInterface.h>
#include <AQUi/AQFieldDB.h>

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQFieldDB( AQFieldDB * obj )
{
  if ( !obj->parent() )
    obj->deleteLater();
}
#endif

class AQSqlCursorInterfaceAQ2;

/**
Interfaz para AQFieldDB
*/
class AQFieldDBInterface : public AQComponentDBInterface
{
  Q_OBJECT

public:

  QSharedPointer<AQFieldDB> obj_;

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQFieldDBInterface( const AQScriptEngineQSA * eng,
                      QSharedPointer<AQFieldDB> aqFieldDB );

  /**
  Destructor
  */
  virtual ~AQFieldDBInterface();
};



/**
Interfaz de soporte para AbanQ v2
*/
class AQFieldDBInterfaceAQ2 : public AQFieldDBInterface
{
  Q_OBJECT

  Q_PROPERTY( QString fieldName READ fieldName WRITE setFieldName )
  Q_PROPERTY( QString tableName READ tableName WRITE setTableName )
  Q_PROPERTY( QString foreignField READ foreignField WRITE setForeignField )
  Q_PROPERTY( QString fieldRelation READ fieldRelation WRITE setFieldRelation )
  Q_PROPERTY( QString fieldAlias READ fieldAlias WRITE setFieldAlias )
  Q_PROPERTY( QString actionName READ actionName WRITE setActionName )
  Q_PROPERTY( bool showAlias READ showAlias WRITE setShowAlias )
  Q_PROPERTY( bool showEditor READ showEditor WRITE setShowEditor )
  Q_PROPERTY( int textFormat READ textFormat WRITE setTextFormat )

public:

  AQFieldDBInterfaceAQ2( const AQScriptEngineQSA * eng, QSharedPointer<AQFieldDB> aqFieldDB ) :
      AQFieldDBInterface( eng, aqFieldDB )
  {}

public Q_SLOTS:

  AQSqlCursorInterfaceAQ2 * cursor() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::cursor );
    return 0;
  }

  QString actionName() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::actionName );
    return QString();
  }

  void setActionName( const QString & )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setActionName );
  }

  QString fieldName() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::fieldName );
    return QString();
  }

  void setFieldName( const QString & )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setFieldName );
  }

  QString tableName() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::tableName );
    return QString();
  }

  void setTableName( const QString & )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setTableName );
  }

  QString foreignField() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::foreignField );
    return QString();
  }

  void setForeignField( const QString & )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setForeignField );
  }

  QString fieldRelation() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::fieldRelation );
    return QString();
  }

  QString fieldAlias() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::fieldAlias );
    return QString();
  }

  QWidget * editor() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::editor );
    return 0;
  }

  void setFilter( const QString & )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setFilter );
  }

  QString filter() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::filter );
    return QString();
  }

  void setFieldRelation( const QString & )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setFieldRelation );
  }

  void setFieldAlias( const QString & )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setFieldAlias );
  }

  void setValue( const QVariant & )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setValue );
  }

  void setPixmap( const QString & )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setPixmap );
  }

  void setFocus()
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setFocus );
  }

  void selectAll()
  {
    AQ_NO_PORT_AQ2( AQFieldDB::selectAll );
  }

  QVariant value() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::value );
    return QVariant();
  }

  int insertAccel( const QString & )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::insertAccel );
    return -1;
  }

  void removeAccel( int )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::removeAccel );
  }

  void setTextFormat( int )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setTextFormat );
  }

  int textFormat() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::textFormat );
    return -1;
  }

  void emitActivatedAccel( int )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::emitActivatedAccel );
  }

  bool showAlias() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::showAlias );
    return true;
  }

  void setShowAlias( bool )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setShowAlias );
  }

  bool showEditor() const
  {
    AQ_NO_PORT_AQ2( AQFieldDB::showEditor );
    return true;
  }

  void setShowEditor( bool )
  {
    AQ_NO_PORT_AQ2( AQFieldDB::setShowEditor );
  }

  AQFieldDB * obj()
  {
    AQ_NO_PORT_AQ2( AQFieldDB::obj );
    return 0;
  }

Q_SIGNALS:

  void activatedAccel( int );
};

#endif /*AQFIELDDBINTERFACE_H_*/
