/***************************************************************************
AQFieldMD.cpp
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

#include "AQFieldMD.h"
#include "AQFieldMD_p.h"
#include "AQRelationMD.h"
#include "util/AQCoreUtil.h"

AQFieldMDPrivate::AQFieldMDPrivate( const QString & n, const QString & a, bool aN,
                                    bool iPK, int t, int l, bool c,
                                    bool v, bool ed, int pI, int pD,
                                    bool iNX, bool uNI, bool coun, const QVariant & defValue,
                                    bool oT, const QString & rX, bool vG, bool gen, bool iCK ) :
    name_( n.toLower() ),
    alias_( a ),
    allowNull_( ( !c ) ? aN : true ),
    isPrimaryKey_( iPK ),
    type_( t ),
    length_( l ),
    calculated_( c ),
    visible_( v ),
    editable_( ed ),
    partInteger_( pI ),
    partDecimal_( pD ),
    isIndex_( iNX ),
    isUnique_( uNI ),
    isCounter_( coun ),
    relationM1_( 0 ),
    associatedField_( 0 ),
    defaultValue_( defValue ),
    outTransaction_( oT ),
    regExpValidator_( rX ),
    visibleGrid_( vG ),
    generated_( gen ),
    isCompoundKey_( iCK )
{
  if ( l < 0 )
    length_ = 0;
  if ( pI < 0 )
    partInteger_ = 0;
  if ( pD < 0 )
    partDecimal_ = 0;

  if ( t != QVariant::String && l != 0 )
    length_ = 0;

  if ( t != QVariant::Int && t != QVariant::UInt && t != QVariant::Double && pI != 0 )
    partInteger_ = 0;

  if ( t != QVariant::Double && pD != 0 )
    partDecimal_ = 0;

  alias_ = AQ_TR( "MetaData", alias_ );
}

AQFieldMDPrivate::~AQFieldMDPrivate()
{
  clear();
}

void AQFieldMDPrivate::clear()
{
  while ( !relationList_.isEmpty() ) {
    AQRelationMD * r = relationList_.takeFirst();
    if ( r && r != relationM1_ )
      delete r;
  }
  delete relationM1_;
  relationM1_ = 0;
}

AQFieldMD::AQFieldMD( const QString & n, const QString & a, bool aN,
                      bool iPK, int t, int l, bool c,
                      bool v, bool ed, int pI, int pD,
                      bool iNX, bool uNI, bool coun, const QVariant & defValue,
                      bool oT, const QString & rX, bool vG, bool gen, bool iCK )
{
  d = new AQFieldMDPrivate( n, a, aN, iPK, t, l, c, v, ed, pI, pD, iNX, uNI, coun, defValue,
                            oT, rX, vG, gen, iCK );
}

AQFieldMD::~AQFieldMD()
{
  delete d;
}

void AQFieldMD::addRelationMD( AQRelationMD * r ) const
{
  if ( !r )
    return ;

  if ( r->cardinality() == AQCore::RELATION_M1 && d->relationM1_ ) {
    aqWarn( "AQFieldMD: Se ha intentado crear más de una relación muchos a uno para el mismo campo" );
    return ;
  }

  r->setField( d->name_ );
  if ( r->cardinality() == AQCore::RELATION_M1 ) {
    delete d->relationM1_;
    d->relationM1_ = r;
    return ;
  }

  d->relationList_.append( r );
}

void AQFieldMD::setOptionsList( const QString & ol ) const
{
  d->optionsList_.clear();
  QString olTranslated = ol;
  if ( ol.contains( "QT_TRANSLATE_NOOP" ) ) {
    QStringList components = olTranslated.split( ';' );
    QString component;

    olTranslated.clear();
    for ( int i = 0; i < components.size(); ++i ) {
      component = components.at( i );
      component = component.mid( 30, component.length() - 32 );
      if ( i > 0 )
        olTranslated += ",";
      olTranslated += component;
    }
  }
  d->optionsList_ = olTranslated.split( ',' );
}

QString AQFieldMD::name() const
{
  return d->name_;
}

QString AQFieldMD::alias() const
{
  return d->alias_;
}

bool AQFieldMD::allowNull() const
{
  return d->allowNull_;
}

bool AQFieldMD::isPrimaryKey() const
{
  return d->isPrimaryKey_;
}

int AQFieldMD::type() const
{
  return d->type_;
}

int AQFieldMD::length() const
{
  return d->length_;
}

bool AQFieldMD::calculated() const
{
  return d->calculated_;
}

void AQFieldMD::setCalculated( bool c ) const
{
  d->calculated_ = c;
}

bool AQFieldMD::editable() const
{
  return d->editable_;
}

void AQFieldMD::setEditable( bool ed ) const
{
  d->editable_ = ed;
}

bool AQFieldMD::visible() const
{
  return d->visible_;
}

bool AQFieldMD::visibleGrid() const
{
  return d->visibleGrid_;
}

bool AQFieldMD::generated() const
{
  return d->generated_;
}

void AQFieldMD::setVisible( bool v ) const
{
  d->visible_ = v;
}

int AQFieldMD::partInteger() const
{
  return d->partInteger_;
}

int AQFieldMD::partDecimal() const
{
  return d->partDecimal_;
}

bool AQFieldMD::isCounter() const
{
  return d->isCounter_;
}

bool AQFieldMD::isIndex() const
{
  return d->isIndex_;
}

bool AQFieldMD::isUnique() const
{
  return d->isUnique_;
}

AQRelationMDList AQFieldMD::relationList() const
{
  return d->relationList_;
}

AQRelationMD * AQFieldMD::relationM1() const
{
  return d->relationM1_;
}

void AQFieldMD::setAssociatedField( AQFieldMD * r, const QString & f ) const
{
  d->associatedField_ = r;
  d->associatedFieldFilterTo_ = f;
}

void AQFieldMD::setAssociatedField( const QString & rName, const QString & f ) const
{
  d->associatedFieldName_ = rName;
  d->associatedFieldFilterTo_ = f;
}

AQFieldMD * AQFieldMD::associatedField() const
{
  return d->associatedField_;
}

QString AQFieldMD::associatedFieldFilterTo() const
{
  return d->associatedFieldFilterTo_;
}

QString AQFieldMD::associatedFieldName() const
{
  return d->associatedFieldName_;
}

QVariant AQFieldMD::defaultValue() const
{
  return d->defaultValue_;
}

bool AQFieldMD::outTransaction() const
{
  return d->outTransaction_;
}

QString AQFieldMD::regExpValidator() const
{
  return d->regExpValidator_;
}

QStringList AQFieldMD::optionsList() const
{
  return d->optionsList_;
}

bool AQFieldMD::isCompoundKey() const
{
  return d->isCompoundKey_;
}

void AQFieldMD::setMetadata( AQTableMD * mtd )
{
  d->mtd_ = mtd;
}

AQTableMD * AQFieldMD::metadata() const
{
  return d->mtd_;
}

QVariant::Type AQFieldMD::aqDecodeType( int aqtype )
{
  QVariant::Type type = QVariant::Invalid;
  switch ( aqtype ) {
  case QVariant::Int:
      type = QVariant::Int;
    break;
  case AQCore::Serial:
  case QVariant::UInt:
    type = QVariant::UInt;
    break;
  case QVariant::Bool:
  case AQCore::Unlock:
    type = QVariant::Bool;
    break;
  case QVariant::Double:
    type = QVariant::Double;
    break;
  case QVariant::Time:
    type = QVariant::Time;
    break;
  case QVariant::Date:
    type = QVariant::Date;
    break;
  case QVariant::String:
  case QVariant::Pixmap:
  case QVariant::StringList:
    type = QVariant::String;
    break;
  case QVariant::ByteArray:
    type = QVariant::ByteArray;
    break;
  }
  return type;
}
