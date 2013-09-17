/***************************************************************************
AQUtilInterface.cpp
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

#include <QImage>

#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQTableMD.h>
#include <AQCore/AQFieldMD.h>
#include <AQSql/AQSqlUtil.h>
#include <AQSql/AQSqlCursor.h>
#include <AQSql/AQSqlDatabase.h>
#include <AQSql/AQManager.h>
#include <AQScript/AQSqlCursorInterface.h>
#include <AQUi/AQUiUtil.h>

#include "AQUtilInterface.h"

AQUtilInterface::~AQUtilInterface()
{
  delete guiUtil;
}

long AQUtilInterface::partInteger( double n ) const
{
  return AQCoreUtil::partInteger( n );
}

long AQUtilInterface::partDecimal( double n ) const
{
  return AQCoreUtil::partDecimal( n );
}

QString
AQUtilInterface::buildNumber( const QVariant & v, char tipo,
                              int partDecimal ) const
{
  return AQCoreUtil::buildNumber( v, tipo, partDecimal );
}

QVariant
AQUtilInterface::readSettingEntry( const QString & key,
                                   const QString & def, bool * ok ) const
{
  return AQCoreUtil::readSettingEntry( key, def, ok );
}

bool
AQUtilInterface::writeSettingEntry( const QString & key,
                                    const QString & value ) const
{
  return AQCoreUtil::writeSettingEntry( key, value );
}

QVariant
AQUtilInterface::readDBSettingEntry( const QString & key,
                                     const QString & connName ) const
{
  return AQSqlUtil::readDBSettingEntry( key, dbConName( connName ) );
}

bool
AQUtilInterface::writeDBSettingEntry( const QString & key, const QString & value,
                                      const QString & connName ) const
{
  return AQSqlUtil::writeDBSettingEntry( key, value, dbConName( connName ) );
}

QString AQUtilInterface::unidades( long n ) const
{
  return AQCoreUtil::unidades( n );
}

QString AQUtilInterface::centenamillar( long n ) const
{
  return AQCoreUtil::centenamillar( n );
}

QString AQUtilInterface::decenas( long n ) const
{
  return AQCoreUtil::decenas( n );
}

QString AQUtilInterface::centenas( long n ) const
{
  return AQCoreUtil::centenas( n );
}

QString AQUtilInterface::unidadesmillar( long n ) const
{
  return AQCoreUtil::unidadesmillar( n );
}

QByteArray AQUtilInterface::utf8( const QString & s ) const
{
  return AQCoreUtil::utf8( s );
}

QString AQUtilInterface::decenasmillar( long n ) const
{
  return AQCoreUtil::decenasmillar( n );
}

QString AQUtilInterface::enLetra( long n ) const
{
  return AQCoreUtil::enLetra( n );
}

QString AQUtilInterface::enLetraMoneda( double n, const QString & m ) const
{
  return AQCoreUtil::enLetraMoneda( n, m );
}

QString AQUtilInterface::enLetraMonedaEuro( double n ) const
{
  return AQCoreUtil::enLetraMonedaEuro( n );
}

QString AQUtilInterface::letraDni( long n ) const
{
  return AQCoreUtil::letraDni( n );
}

QString AQUtilInterface::calcularDC( const QString & n ) const
{
  return AQCoreUtil::calcularDC( n );
}

QString AQUtilInterface::dateDMAtoAMD( const QString & f ) const
{
  return AQCoreUtil::dateDMAtoAMD( f );
}

QString AQUtilInterface::dateAMDtoDMA( const QString & f ) const
{
  return AQCoreUtil::dateAMDtoDMA( f );
}

QString AQUtilInterface::formatoMiles( const QString & s ) const
{
  return AQCoreUtil::formatoMiles( s );
}

QString AQUtilInterface::translate( const QString & contexto, const QString & s ) const
{
  return AQ_TR( contexto, s );
}

bool AQUtilInterface::numCreditCard( const QString & num ) const
{
  return AQCoreUtil::numCreditCard( num );
}

QVariant
AQUtilInterface::nextCounter( const QString & name,
                              QObject * cursor_ ) const
{
  AQSqlCursorInterface * c = qobject_cast<AQSqlCursorInterface *>( cursor_ );

  if ( c )
    return AQSqlUtil::nextCounter( name, c->obj_.data() );

  return QVariant();
}

QString AQUtilInterface::nextSequence( int nivel, const QString & secuencia,
                                       const QString & ultimo ) const
{
  return AQCoreUtil::nextSequence( nivel, secuencia, ultimo );
}

QString AQUtilInterface::serialLettertoNumber( const QString & letter ) const
{
  return AQCoreUtil::serialLettertoNumber( letter );
}

QString AQUtilInterface::serialNumbertoLetter( int number ) const
{
  return AQCoreUtil::serialNumbertoLetter( number );
}

bool AQUtilInterface::isAQDefFile( const QString & head ) const
{
  return AQCoreUtil::isAQDefFile( head );
}

QStringList
AQUtilInterface::fieldsNames( const QString & table,
                              const QString & connName ) const
{
  return AQSqlUtil::fieldsNames( table, dbConName( connName ) );
}

QString
AQUtilInterface::fieldNameToAlias( const QString & fN, const QString & tN,
                                   const QString & connName ) const
{
  if ( !tN.isEmpty() ) {
    AQTableMD * mtd = dbCon( connName ).manager() ->metadata( tN );
    if ( mtd )
      return mtd->fieldNameToAlias( fN );
  }
  return fN;
}

QString
AQUtilInterface::tableNameToAlias( const QString & tN,
                                   const QString & connName ) const
{
  if ( !tN.isEmpty() ) {
    AQTableMD * mtd = dbCon( connName ).manager() ->metadata( tN );
    if ( mtd )
      return mtd->alias();
  }
  return QString();
}

QString
AQUtilInterface::fieldAliasToName( const QString & aN, const QString & tN,
                                   const QString & connName ) const
{
  if ( !tN.isEmpty() ) {
    AQTableMD * mtd = dbCon( connName ).manager() ->metadata( tN );
    if ( mtd )
      return mtd->fieldAliasToName( aN );
  }
  return aN;
}

int AQUtilInterface::fieldType( const QString & fN, const QString & tN,
                                const QString & connName ) const
{
  if ( !tN.isEmpty() ) {
    AQTableMD * mtd = dbCon( connName ).manager() ->metadata( tN );
    if ( mtd )
      return mtd->fieldType( fN );
  }
  return QVariant::Invalid;
}

int AQUtilInterface::fieldLength( const QString & fN, const QString & tN,
                                  const QString & connName ) const
{
  if ( !tN.isEmpty() ) {
    AQTableMD * mtd = dbCon( connName ).manager() ->metadata( tN );
    if ( mtd )
      return mtd->fieldLength( fN );
  }
  return 0;
}

bool AQUtilInterface::fieldAllowNull( const QString & fN, const QString & tN,
                                      const QString & connName ) const
{
  if ( !tN.isEmpty() ) {
    AQTableMD * mtd = dbCon( connName ).manager() ->metadata( tN );
    if ( mtd )
      return mtd->fieldAllowNull( fN );
  }
  return false;
}

QDate AQUtilInterface::addDays( const QString & d, int nd ) const
{
  return AQCoreUtil::addDays( QDate::fromString( d, Qt::ISODate ), nd );
}

QDate AQUtilInterface::addMonths( const QString & d, int nm ) const
{
  return AQCoreUtil::addMonths( QDate::fromString( d, Qt::ISODate ), nm );
}

QDate AQUtilInterface::addYears( const QString & d, int ny ) const
{
  return AQCoreUtil::addYears( QDate::fromString( d, Qt::ISODate ), ny );
}

int AQUtilInterface::daysTo( const QString & d1, const QString & d2 ) const
{
  return AQCoreUtil::daysTo( QDate::fromString( d1, Qt::ISODate ),
                             QDate::fromString( d2, Qt::ISODate ) );
}

QVariant AQUtilInterface::sqlSelect( const QString & f, const QString & s, const QString & w,
                                     const QString & tL , const QString & connName ) const
{
  return AQSqlUtil::sqlSelect( f, s, w, tL, 0, dbConName( connName ) );
}

bool AQUtilInterface::sqlInsert( const QString & t, const QString & fL, const QString & vL,
                                 const QString & connName ) const
{
  return AQSqlUtil::sqlInsert( t, fL, vL, dbConName( connName ) );
}

bool AQUtilInterface::sqlUpdate( const QString & t, const QString & fL, const QString & vL,
                                 const QString & w, const QString & connName ) const
{
  return AQSqlUtil::sqlUpdate( t, fL, vL, w, dbConName( connName ) );
}

bool AQUtilInterface::sqlDelete( const QString & t, const QString & w,
                                 const QString & connName ) const
{
  return AQSqlUtil::sqlDelete( t, w, dbConName( connName ) );
}

void AQUtilInterface::createProgressDialog( const QString & l, int tS )
{
  if ( !guiUtil )
    guiUtil = new AQUiUtil;
  guiUtil->createProgressDialog( l, tS );
}

void AQUtilInterface::createProgressDialog()
{
  if ( !guiUtil )
    guiUtil = new AQUiUtil;
  guiUtil->createProgressDialog( QString(), 0 );
}

void AQUtilInterface::destroyProgressDialog() const
{
  if ( guiUtil )
    guiUtil->destroyProgressDialog();
}

void AQUtilInterface::setProgress( int p ) const
{
  if ( guiUtil )
    guiUtil->setProgress( p );
}

void AQUtilInterface::setTotalSteps( int tS ) const
{
  if ( guiUtil )
    guiUtil->setTotalSteps( tS );
}

void AQUtilInterface::setLabelText( const QString & l ) const
{
  if ( guiUtil )
    guiUtil->setLabelText( l );
}

QString AQUtilInterface::sha1( const QString & str ) const
{
  return AQCoreUtil::sha1( str );
}

QString AQUtilInterface::getLang() const
{
  return AQCoreUtil::getLang();
}

QString AQUtilInterface::getOS() const
{
  return AQCoreUtil::getOS();
}

QString
AQUtilInterface::roundFieldValue( const QVariant n, const QString & table,
                                  const QString & field, const QString & connName ) const
{
  AQFieldMD * fmd = dbCon( connName ).manager() ->metadata( table ) ->field( field );
  if ( !fmd )
    return 0;
  return buildNumber( n, 'f', fmd->partDecimal() );
}

AQUtilInterfaceAQ2::AQUtilInterfaceAQ2( const AQScriptEngineQSA * eng ) :
    AQUtilInterface( eng )
{
  aqWarn( engine->aqScriptAndLineState() +
          "\nLa forma 'new FLUtil' es obsoleta y no se debería usar en nuevo código.\n"
          "Se puede utilizar AQUtil como objeto global estático. Ejemplo: AQUtil.getOS()" );
}

#include "moc_AQUtilInterface.cpp"
