/***************************************************************************
AQTableMD.cpp
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

#include "AQTableMD.h"
#include "AQFieldMD.h"
#include "AQFieldMD_p.h"
#include "AQRelationMD.h"
#include "AQCompKeyMD.h"
#include "../AQCore.h"

class AQTableMDPrivate
{

public:

  AQTableMDPrivate( const QString & n, const QString & a, const QString & q );
  ~AQTableMDPrivate();

  /**
  Limpiar
  */
  void clear();

  /**
  Añade el nombre de un campo a la cadena de nombres de campos, ver fieldsNames()

  @param n Nombre del campo
  */
  void addFieldName( const QString & n );

  /**
  Elimina el nombre de un campo a la cadena de nombres de campos, ver fieldsNames()

  @param n Nombre del campo
  */
  void removeFieldName( const QString & n );

  /**
  Formatea el alias del campo indicado para evitar duplicados

  @param  f   Campo objeto cuyo alias se desea formatear
  */
  void formatAlias( AQFieldMD * f );

  /**
  Nombre de la tabla
  */
  QString name_;

  /**
  Alias de la tabla
  */
  QString alias_;

  /**
  Lista de campos que tiene esta tabla
  */
  AQFieldMDHash fieldList_;

  /**
  Clave compuesta de la tabla
  */
  AQCompKeyMD compoundKey_;

  /**
  Nombre de la consulta (fichero .qry) de la que define los metadatos
  */
  QString query_;

  /**
  Cadena de texto con los nombres de los campos separados por comas
  */
  QString fieldsNames_;

  /**
  Mapas alias<->nombre
  */
  QMap<QString, QString> aliasFieldMap_;
  QMap<QString, QString> fieldAliasMap_;

  /**
  Lista de nombres de campos de la tabla que son del tipo AQCore::Unlock
  */
  QStringList fieldsNamesUnlock_;
};

AQTableMDPrivate::AQTableMDPrivate( const QString & n, const QString & a, const QString & q ) :
    name_( n.toLower() ),
    alias_( a ),
    query_( q )
{}

AQTableMDPrivate::~AQTableMDPrivate()
{
  clear();
}

void AQTableMDPrivate::addFieldName( const QString & n )
{
  if ( fieldsNames_.isEmpty() )
    fieldsNames_ += n.toLower();
  else
    fieldsNames_ += "," + n.toLower();
}

void AQTableMDPrivate::removeFieldName( const QString & n )
{
  if ( !n.isEmpty() )
    fieldsNames_.replace( "," + n + ",", "," );
}

void AQTableMDPrivate::formatAlias( AQFieldMD * f )
{
  if ( f ) {
    QString alias( f->alias() );
    QString field( f->name().toLower() );
    QMap<QString, QString>::const_iterator it( aliasFieldMap_.find( alias ) );

    if ( it != aliasFieldMap_.end() ) {
      alias += "(" + QString::number( aliasFieldMap_.size() + 1 ) + ")";
      f->d->alias_ = alias;
    }

    aliasFieldMap_[ alias ] = field;
    fieldAliasMap_[ field ] = alias;
  }
}

void AQTableMDPrivate::clear()
{
  while ( !fieldList_.isEmpty() ) {
    AQFieldMD * f = fieldList_.begin().value();
    fieldList_.erase( fieldList_.begin() );
    delete f;
  }
}

AQTableMD::AQTableMD( const QString & n, const QString & a, const QString & q )
{
  d = new AQTableMDPrivate( n, a, q );
}

AQTableMD::~AQTableMD()
{
  delete d;
}

void AQTableMD::addFieldMD( AQFieldMD * f )
{
  if ( f ) {
    f->d->mtd_ = this;
    d->fieldList_.insert( f->d->name_.toLower(), f );
    d->addFieldName( f->d->name_ );
    d->formatAlias( f );
    if ( f->d->type_ == AQCore::Unlock )
      d->fieldsNamesUnlock_.append( f->d->name_ );
  }
}

void AQTableMD::removeFieldMD( const QString & fN )
{
  d->fieldList_.erase( d->fieldList_.find( fN.toLower() ) );
}

void AQTableMD::setCompoundKey( const AQCompKeyMD & cK )
{
  d->compoundKey_ = cK;
}

QString AQTableMD::fieldNameToAlias( const QString & fN ) const
{
  if ( fN.isEmpty() )
    return fN;

  QMap<QString, QString>::const_iterator it( d->fieldAliasMap_.find( fN.toLower() ) );
  if ( it != d->fieldAliasMap_.end() )
    return * it;

  return fN;
}

QString AQTableMD::fieldAliasToName( const QString & aN ) const
{
  if ( aN.isEmpty() )
    return aN;

  QMap<QString, QString>::const_iterator it( d->aliasFieldMap_.find( aN ) );
  if ( it != d->aliasFieldMap_.end() )
    return * it;

  return aN;
}

int AQTableMD::fieldType( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  return field ? field->d->type_ : ( int ) QVariant::Invalid;
}

int AQTableMD::fieldLength( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  return field ? field->d->length_ : 0;
}

int AQTableMD::fieldPartInteger( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  return field ? field->d->partInteger_ : 0;
}

int AQTableMD::fieldPartDecimal( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  return field ? field->d->partDecimal_ : 0;
}

bool AQTableMD::fieldIsPrimaryKey( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  return field ? field->d->isPrimaryKey_ : false;
}

QString AQTableMD::primaryKey( bool prefixTable ) const
{
  for ( AQFieldMDHash::const_iterator it = d->fieldList_.begin();
        it != d->fieldList_.end(); ++it ) {
    if ( ( *it ) ->isPrimaryKey() )
      return ( prefixTable ? d->name_ + "." + ( *it ) ->d->name_ : ( *it ) ->d->name_ );
  }

  return QString();
}

bool AQTableMD::fieldIsIndex( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  return field ? field->d->isIndex_ : false;
}

bool AQTableMD::fieldIsUnique( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  return field ? field->d->isUnique_ : false;
}

bool AQTableMD::fieldIsCounter( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  return field ? field->d->isCounter_ : false;
}

bool AQTableMD::fieldAllowNull( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  return field ? field->d->allowNull_ : false;
}

bool AQTableMD::fieldCalculated( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  return field ? field->d->calculated_ : false;
}

bool AQTableMD::fieldVisible( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  return field ? field->d->visible_ : false;
}

AQFieldMD * AQTableMD::field( const QString & fN ) const
{
  return d->fieldList_.value( fN.toLower() );
}

QString AQTableMD::fieldTableM1( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  if ( field && field->d->relationM1_ )
    return field->d->relationM1_->foreignTable();

  return QString();
}

QString AQTableMD::fieldForeignFieldM1( const QString & fN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  if ( field && field->d->relationM1_ )
    return field->d->relationM1_->foreignField();

  return QString();
}

AQRelationMD * AQTableMD::relation( const QString & fN, const QString & fFN, const QString & fTN ) const
{
  AQFieldMD * field = d->fieldList_.value( fN.toLower() );
  if ( field ) {
    if ( field->d->relationM1_ &&
         field->d->relationM1_->foreignField() == fFN.toLower() &&
         field->d->relationM1_->foreignTable() == fTN.toLower() )
      return field->d->relationM1_;

    AQRelationMD * r;
    for ( AQRelationMDList::const_iterator it = field->d->relationList_.begin();
          it != field->d->relationList_.end(); ++it ) {
      r = *it;
      if ( r->foreignField() == fFN.toLower() && r->foreignTable() == fTN.toLower() )
        return r;
    }
  }

  return 0;
}

AQFieldMDHash AQTableMD::fieldListOfCompoundKey( const QString & fN ) const
{
  if ( d->compoundKey_.hasField( fN ) )
    return d->compoundKey_.fieldList();

  return AQFieldMDHash();
}

bool AQTableMD::isQuery() const
{
  return !d->query_.isEmpty();
}

QString AQTableMD::fieldList( bool prefixTable ) const
{
  if ( !prefixTable )
    return d->fieldsNames_;

  QStringList fieldsNames = d->fieldsNames_.split( "," );
  QString ret;
  for ( QStringList::const_iterator it = fieldsNames.begin(); it != fieldsNames.end(); ++it ) {
    if ( !( *it ).contains( "." ) ) {
      if ( ret.isEmpty() )
        ret += d->name_ + "." + ( *it );
      else
        ret += "," + d->name_ + "." + ( *it );
    }
  }

  return ret;
}

QString AQTableMD::name() const
{
  return d->name_;
}

void AQTableMD::setName( const QString & n )
{
  d->name_ = n;
}

QString AQTableMD::alias() const
{
  return d->alias_;
}

QString AQTableMD::query() const
{
  return d->query_;
}

AQFieldMDHash AQTableMD::fieldList() const
{
  return d->fieldList_;
}

QStringList AQTableMD::fieldsNamesUnlock() const
{
  return d->fieldsNamesUnlock_;
}
