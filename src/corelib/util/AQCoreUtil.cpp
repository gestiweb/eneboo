/***************************************************************************
AQCoreUtil.cpp
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

#include <cmath>

#include <QCoreApplication>
#include <QDomDocument>
#include <QSettings>
#include <QLocale>
#include <QRegExp>

#include <AQsha1/AQSha1.h>

#include "AQCoreUtil.h"
#include "AQCore.h"

const char * AQCoreUtil::vecUnidades[ 30 ] =
  { "", "uno", "dos", "tres", "cuatro",
    "cinco", "seis", "siete", "ocho", "nueve",
    "diez", "once", "doce", "trece", "catorce",
    "quince", "dieciseis", "diecisiete", "dieciocho", "diecinueve",
    "veinte", "veintiun", "veintidos", "veintitres", "veinticuatro",
    "veinticinco", "veintiseis", "veintisiete", "veintiocho", "veintinueve"
  };

const char * AQCoreUtil::vecDecenas[ 10 ] =
  { "", "", "", "treinta", "cuarenta", "cincuenta",
    "sesenta", "setenta", "ochenta", "noventa"
  };

const char * AQCoreUtil::vecCentenas[ 10 ] =
  { "", "ciento", "doscientos", "trescientos", "cuatrocientos",
    "quinientos", "seiscientos", "setecientos", "ochocientos", "novecientos"
  };

long AQCoreUtil::partInteger( double n )
{
  return ( long ) n;
}

long AQCoreUtil::partDecimal( double n )
{
  QString s = QString::number( n, 'f', 2 );
  s = s.right( 2 );
  return s.toLong();
}

QString AQCoreUtil::enLetra( long n )
{
  QString buffer;

  if ( n > 1000000000L ) {
    buffer = "Sólo hay capacidad hasta mil millones";
    return buffer;
  }

  if ( n < 1000000L ) {
    buffer = centenamillar( n );
    return buffer;
  } else {
    if ( n / 1000000L == 1 )
      buffer = "un millon ";
    else {
      buffer = centenas( ( long ) ( n / 1000000L ) );
      buffer = buffer + " millones ";
    }
  }

  buffer = buffer + centenamillar( n % 1000000L );

  return buffer.toUpper();
}

QString AQCoreUtil::enLetraMoneda( double n, const QString & m )
{

  double nTemp = ( n < 0.00 ) ? ( n * -1.00 ) : n;
  long entero = partInteger( nTemp ), decimal = partDecimal( nTemp );
  QString res = QString::null;

  if ( entero > 0 )
    res = enLetra( entero ) + " " + m;

  if ( entero > 0 && decimal > 0 )
    res += " con " + enLetra( decimal ) + " céntimos";

  if ( entero <= 0 && decimal > 0 )
    res = enLetra( decimal ) + " céntimos";

  if ( n < 0.00 )
    res = "menos " + res;

  return res.toUpper();
}

QString AQCoreUtil::enLetraMonedaEuro( double n )
{
  return enLetraMoneda( n, "euros" );
}

QString AQCoreUtil::unidades( long n )
{
  return vecUnidades[ n ];
}

QByteArray AQCoreUtil::utf8( const QString & s )
{
  return s.toUtf8();
}

QString AQCoreUtil::centenamillar( long n )
{
  QString buffer;

  if ( n < 10000 ) {
    buffer = decenasmillar( n );
    return buffer;
  }
  buffer = centenas( n / 1000 );
  buffer = buffer + " mil ";
  buffer = buffer + centenas( n % 1000 );

  return buffer;
}

QString AQCoreUtil::decenas( long n )
{
  QString buffer;
  if ( n < 30 )
    buffer = unidades( n );
  else {
    buffer = vecDecenas[ n / 10L ];
    if ( n % 10L ) {
      buffer = buffer + " y ";
      buffer = buffer + unidades( n % 10L );
    }
  }
  return buffer;
}

QString AQCoreUtil::centenas( long n )
{
  QString buffer;
  if ( n == 100 )
    buffer = "cien";
  else if ( n < 100 )
    buffer = decenas( n );
  else {
    buffer = buffer + vecCentenas[ n / 100L ];
    buffer = buffer + " ";
    buffer = buffer + decenas( n % 100 );
  }
  return buffer;
}


QString AQCoreUtil::unidadesmillar( long n )
{
  QString buffer;

  if ( n < 1000 )
    buffer = "";

  if ( n / 1000 == 1 )
    buffer = "mil ";

  if ( n / 1000 > 1 ) {
    buffer = unidades( n / 1000 );
    buffer = buffer + " mil ";
  }
  buffer = buffer + centenas( n % 1000 );

  return buffer;
}

QString AQCoreUtil::decenasmillar( long n )
{
  QString buffer;

  if ( n < 10000 ) {
    buffer = unidadesmillar( n );
    return buffer;
  }

  buffer = decenas( n / 1000 );
  buffer = buffer + " mil ";
  buffer = buffer + centenas( n % 10000 );

  return ( buffer );
}

QChar AQCoreUtil::letraDni( long n )
{
  QString letras = "TRWAGMYFPDXBNJZSQVHLCKE";

  return letras.at( n % 23 );
}

QChar AQCoreUtil::calcularDC( const QString & n )
{
  int Tabla[ 10 ] = {
                      6,
                      3,
                      7,
                      9,
                      10,
                      5,
                      8,
                      4,
                      2,
                      1
                    };
  int DC, Suma = 0, nDigitos;

  nDigitos = n.length() - 1;
  for ( int ct = 1; ct <= n.length(); ct++ ) {
    Suma += Tabla[ nDigitos ] * ( n.at( ct - 1 ).unicode() - 48 );
    nDigitos--;
  }

  DC = 11 - ( Suma % 11 );
  if ( DC == 11 )
    DC = 0;
  else if ( DC == 10 )
    DC = 1;

  return QChar::QChar( DC + 48 );
}

QString AQCoreUtil::dateDMAtoAMD( const QString & f )
{
  QString res( f );

  res = res.replace( QRegExp( "[\\s/-]" ), "" );

  if ( res.length() <= 2 )
    return ( res.left( 2 ) );

  if ( res.length() <= 4 )
    return ( res.right( res.length() - 2 ) + "-" + res.left( 2 ) );

  if ( res.length() <= 8 )
    return ( res.right( res.length() - 4 ) + "-" + res.mid( 2, 2 ) + "-" +
             res.left( 2 ) );

  return res;
}

QString AQCoreUtil::dateAMDtoDMA( const QString & f )
{
  QString res = f;

  res = res.replace( QRegExp( "[\\s/-]" ), "" );

  if ( res.length() > 8 )
    res = res.left( 8 );

  if ( res.length() <= 2 )
    return ( res.left( 2 ) );

  if ( res.length() <= 4 )
    return ( res.right( res.length() - 2 ) + "-" + res.left( 2 ) );

  if ( res.length() <= 8 )
    return ( res.right( res.length() - 6 ) + "-" + res.mid( 4, 2 ) + "-" + res.left( 4 ) );

  return res;
}

QString AQ_TR( const QString & contexto, const QString & s )
{
  return QCoreApplication::instance() ->translate( contexto.toUtf8(), s.toUtf8() ).toUtf8();
}

bool AQCoreUtil::numCreditCard( const QString & num )
{
  int nSum = 0;
  int nRest, nCard;
  nCard = num.toInt();
  nRest = nCard;
  for ( int i = 0; i < 10; i += 2 ) {
    nSum = nSum + ( num.mid( i, 1 ) ).toInt();
    nRest = ( num.mid( i + 1, 1 ) ).toInt() * 2;
    if ( nRest > 9 ) {
      nRest = nRest - 9;
    }
    nSum = nSum + nRest;
  }
  if ( nSum % 10 == 0 )
    return true;
  else
    return false;
}

QString AQCoreUtil::nextSequence( int nivel, const QString & secuencia, const QString & ultimo )
{
  QString cadena;
  QString valor;
  QString string;
  QString string2;

  QChar temp;
  long num;
  int pos2 = 0;
  int nivpas = 0;
  int posult = 0;

  int niveles = secuencia.count( "%A" ) + secuencia.count( "%N" );
  int pos = 1;
  pos2 = secuencia.indexOf( "%", 0 );

  while ( pos2 != -1 ) {
    if ( nivpas == 0 ) {
      cadena = secuencia.mid( 0, pos2 );
      posult = pos2;
    } else {
      string2 = secuencia.mid( pos + 2, pos2 - pos - 2 );
      posult = ultimo.indexOf( string2, posult ) + string2.length();
      cadena = cadena + secuencia.mid( pos + 2, pos2 - pos - 2 );
    }

    pos = pos2;
    valor = "";
    if ( nivpas < niveles - nivel ) {
      for ( int i = posult; i < ultimo.length(); ++i ) {
        if ( secuencia.mid( pos + 1, 1 ) == "A" ) {
          temp = ultimo.at( i );
          if ( temp.isLetter() == false ) {
            cadena = cadena + valor;
            break;
          }
          valor = valor + temp;
        }
        if ( secuencia.mid( pos + 1, 1 ) == "N" ) {
          temp = ultimo.at( i );
          if ( temp.isNumber() == false ) {
            cadena = cadena + valor;
            break;
          }
          valor = valor + temp;
        }
      }
      posult = posult + valor.length();
    } else if ( nivpas == niveles - nivel ) {
      for ( int i = posult; i < ultimo.length(); ++i ) {
        if ( secuencia.mid( pos + 1, 1 ) == "A" ) {
          temp = ultimo.at( i );
          if ( i == ( ultimo.length() - 1 ) ) {
            valor = valor + temp;
            num = serialLettertoNumber( valor ).toLong() + 1;
            cadena = cadena + serialNumbertoLetter( num );
            break;
          }
          if ( temp.isLetter() == false ) {
            num = serialLettertoNumber( valor ).toLong() + 1;
            cadena = cadena + serialNumbertoLetter( num );
            break;
          }
          valor = valor + temp;
        }
        if ( secuencia.mid( pos + 1, 1 ) == "N" ) {
          temp = ultimo.at( i );
          if ( i == ( ultimo.length() - 1 ) ) {
            valor = valor + temp;
            cadena = cadena + string.setNum( valor.toInt() + 1 );
            break;
          }
          if ( temp.isNumber() == false ) {
            cadena = cadena + string.setNum( valor.toInt() + 1 );
            break;
          }
          valor = valor + temp;
        }
      }
      posult = posult + valor.length();
    } else {
      if ( secuencia.mid( pos + 1, 1 ) == "N" )
        cadena = cadena + "1";
      if ( secuencia.mid( pos + 1, 1 ) == "A" )
        cadena = cadena + "A";
    }
    pos2 = secuencia.indexOf( "%", pos + 1 );
    nivpas++;
  }
  return cadena;
}

QString AQCoreUtil::serialLettertoNumber( const QString & letter )
{
  int len = letter.length(), num = 0;
  QString string;

  for ( int i = 1; i <= len; ++i )
    num += ( ( ( int ) pow( 26, i - 1 ) ) * ( letter.at( len - i ).unicode() - 64 ) );

  return string.setNum( num );
}

QString AQCoreUtil::serialNumbertoLetter( int number )
{
  QString letter;
  int cociente = number, resto = 0;

  while ( cociente > 26 ) {
    cociente = number / 26;
    resto = number % 26;
    letter = QString( QChar( resto + 64 ) ) + letter;
    number = cociente;
  }
  letter = QString( QChar( cociente + 64 ) ) + letter;

  return letter;
}

QString AQCoreUtil::tr( const QString & s )
{
  return QCoreApplication::instance() ->tr( s.toUtf8() ).toUtf8();
}

bool AQCoreUtil::domDocumentSetContent( QDomDocument & doc, const QString & content )
{
  if ( content.isEmpty() ) {
    aqWarn( translate( "AQCoreUtil", "AQCoreUtil : Se ha intentado cargar un fichero XML vacío" ) );
    return false;
  }
  QString ErrMsg;
  int errLine, errColumn;
  if ( !doc.setContent( content, &ErrMsg, &errLine, &errColumn ) ) {
    aqWarn( translate( "AQCoreUtil", "AQCoreUtil : Error en fichero XML.\nError : %1\nLinea : %2\nColumna : %3" )
            .arg( ErrMsg, QString::number( errLine ), QString::number( errColumn ) ) );
    return false;
  }
  return true;
}

QString AQCoreUtil::sha1( const QString & str )
{
  AQSha1 s;
  return s.sha1( str );
}

QString AQCoreUtil::sha1( const QByteArray & ba )
{
  AQSha1 s;
  return s.sha1( ba );
}

QString AQCoreUtil::sha1File( char *szFileName )
{
  AQSha1 s;
  return s.sha1File( szFileName );
}

QString AQCoreUtil::formatoMiles( const QString & s )
{
  QString ret;
  QString decimal;
  QString entera;
  QString dot = tr( "." );
  bool neg = ( s.at( 0 ) == '-' );

  if ( s.contains( '.' ) ) {
    decimal = tr( "," ) + s.section( '.', -1, -1 );
    entera = s.section( '.', -2, -2 ).remove( '-' ).remove( '.' );
  } else
    entera = s;

  int len = entera.length();

  while ( len > 3 ) {
    ret = dot + entera.right( 3 ) + ret;
    entera.remove( len - 3, 3 );
    len = entera.length();
  }

  ret = entera + ret + decimal;

  if ( neg )
    ret.prepend( '-' );

  return ret;
}

bool AQCoreUtil::isAQDefFile( const QString & head )
{
  return ( typeAQDefFile( head ) != AQCore::NoDefFile );
}

AQCore::TypeDefFile AQCoreUtil::typeAQDefFile( const QString & head )
{
  QString headt( head );
  headt.trimmed();

  if ( headt.contains( "<!DOCTYPE UI>" ) || headt.contains( "<ui version=" ) )
    return AQCore::UiDef;

  if ( headt.contains( "<!DOCTYPE QRY>" ) )
    return AQCore::QryDef;

  if ( headt.contains( "<!DOCTYPE KugarTemplate" ) )
    return AQCore::KutDef;

  if ( headt.contains( "<!DOCTYPE TMD>" ) )
    return AQCore::MtdDef;

  if ( headt.contains( "<!DOCTYPE TS>" ) )
    return AQCore::TsDef;

  if ( headt.contains( "<ACTIONS>" ) )
    return AQCore::ActDef;

  if ( headt.contains( "<!DOCTYPE ACL>" ) )
    return AQCore::AclDef;

  return AQCore::NoDefFile;
}

QDate AQCoreUtil::addDays( const QDate &d, int nd )
{
  return d.addDays( nd );
}

QDate AQCoreUtil::addMonths( const QDate &d, int nm )
{
  return d.addMonths( nm );
}

QDate AQCoreUtil::addYears( const QDate &d, int ny )
{
  return d.addYears( ny );
}

int AQCoreUtil::daysTo( const QDate &d1, const QDate &d2 )
{
  return d1.daysTo( d2 );
}

QVariant AQCoreUtil::readSettingEntry( const QString & key, const QString & def, bool * )
{
  QSettings settings( QSettings::UserScope, "InfoSiAL" );
  return settings.value( AQ_KEYBASE + key, def );
}

bool AQCoreUtil::writeSettingEntry( const QString & key, const QString & value )
{
  QSettings settings( QSettings::UserScope, "InfoSiAL" );
  settings.setValue( AQ_KEYBASE + key, value );
  return true;
}

QString AQCoreUtil::buildNumber( const QVariant & v, char tipo, int partDecimal )
{
  QString ret;
  double d = QVariant( v ).toDouble() * pow( 10, partDecimal );
  d = round( d );
  d = d / pow( 10, partDecimal );
  ret.setNum( d , tipo, partDecimal );
  return ret;
}

QVariant::Type AQCoreUtil::aqDecodeType( int aqtype )
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

QString AQCoreUtil::getLang()
{
  return QLocale::system().name().left( 2 );
}

QString AQCoreUtil::getOS()
{
#if defined(Q_OS_WIN32)
  return "WIN32";
#endif
#if defined(Q_OS_LINUX)
  return "LINUX";
#endif
#if defined(Q_OS_MACX)
  return "MACX";
#endif
}


