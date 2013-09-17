/***************************************************************************
AQCoreUtil.h
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

#ifndef AQCOREUTIL_H_
#define AQCOREUTIL_H_

#include <AQCore/AQCore.h>

#include <QString>
#include <QVariant>
#include <QDate>
#include <QByteArray>

class QDomDocument;

/**
Clase con métodos, herramientas y utiles necesarios para ciertas operaciones.
 
Es esta clase se encontrarán métodos genéricos que
realizan operaciones muy específicas pero que
son necesarios para ciertos procesos habituales
en las distintas tareas a desempeñar en la gestión
empresarial.
 
@author InfoSiAL S.L.
*/
class AQCoreUtil
{
public:

  /**
  Obtiene la parte entera de un número.

  Dado un número devuelve la parte entera correspondiente, es decir,
  cifras en la parte izquierda de la coma decimal.

  @param  n Número del que obtener la parte entera. Debe ser positivo
  @return La parte entera del número, que puede ser cero
  */
  static long partInteger( double n );

  /**
  Obtiene la parte decimal de un número.

  Dado un número devuelve la parte decimal correspondiente, es decir,
  cifras en la parte derecha de la coma decimal.

  @param  n Número del que obtener la parte decimal. Debe ser positivo
  @return La parte decimal del número, que puede ser cero
  */
  static long partDecimal( double n );

  /**
  Construye un string a partir de un número, especificando el formato y precisión

  @param  v            Número a convertir a QString
  @param  tipo         Formato del número
  @param  partDecimal  Precisión (número de cifras decimales) del número
  @return Cadena que contiene el número formateado
  */
  static QString buildNumber( const QVariant & v, char tipo, int partDecimal );

  /**
  Lee el valor de un setting en el directorio de la instalación de AbanQ

  @param  key Clave identificadora del setting
  @param  def Valor por defecto en el caso de que el setting no esté establecido
  @param  ok  Indicador de que la lectura es correcta
  @return Valor del setting
  */
  static QVariant readSettingEntry( const QString & key, const QString & def = QString(), bool * ok = 0 );

  /**
  Establece el valor de un setting en el directorio de instalación de AbanQ

  @param  key Clave identificadora del setting
  @param  Valor del setting
  @return Indicador de si la escritura del settings se realiza correctamente
  */
  static bool writeSettingEntry( const QString & key, const QString & value );

  /**
  Enunciado de las unidades de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString unidades( long n );

  /**
  Enunciado de las centenas de millar de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString centenamillar( long n );

  /**
  Enunciado de las decenas de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString decenas( long n );

  /**
  Enunciado de las centenas de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString centenas( long n );

  /**
  Enunciado de las unidades de millar de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString unidadesmillar( long n );

  /**
  Pasa una cadena a codificación utf-8

  @param  s Cadena
  @return Cadena en formato UTF-8
  */
  static QByteArray utf8( const QString & s );

  /**
  Enunciado de las decenas de millar de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString decenasmillar( long n );

  /**
  Obtiene la expresión en texto de como se enuncia un número, en castellano.

  Dado un número entero, devuelve su expresión en texto de como se
  enuncia de forma hablada; por ejemplo dado el número 130,
  devolverá la cadena de texto "ciento treinta".

  @param  n   Número a transladar a su forma hablada. Debe ser positivo
  @return Cadena de texto con su expresión hablada
  */
  static QString enLetra( long n );

  /**
  Obtiene la expresión en texto de como se enuncia una cantidad monetaria, en castellano
  y en cualquier moneda indicada.

  Dado un número doble, devuelve su expresión en texto de como se
  enuncia de forma hablada en la moneda indicada; por ejemplo dado el número 130.25,
  devolverá la cadena de texto "ciento treinta 'moneda' con veinticinco céntimos".

  @param  n Número a transladar a su forma hablada. Debe ser positivo
  @param  m Nombre de la moneda
  @return Cadena de texto con su expresión hablada
  */
  static QString enLetraMoneda( double n, const QString & m );

  /**
  Obtiene la expresión en texto de como se enuncia una cantidad monetaria, en castellano
  y en Euros.

  Dado un número doble, devuelve su expresión en texto de como se
  enuncia de forma hablada en euros; por ejemplo dado el número 130.25,
  devolverá la cadena de texto "ciento treinta euros con veinticinco céntimos".

  @param  n Número a transladar a su forma hablada. Debe ser positivo
  @return Cadena de texto con su expresión hablada
  */
  static QString enLetraMonedaEuro( double n );

  /**
  Calcula la letra asociada a un número de DNI

  @param  n Número de DNI
  @return Letra asociada
  **/
  static QChar letraDni( long n );

  /**
  Calcula el dígito de control asociado a un número de cuenta bancaria

  @param  n Número de cuenta
  @return Dígito de control
  */
  static QChar calcularDC( const QString & n );

  /**
  Convierte fechas del tipo DD-MM-AAAA, DD/MM/AAAA o
  DDMMAAAA al tipo AAAA-MM-DD.

  @param  f Cadena de texto con la fecha a transformar
  @return Cadena de texto con la fecha transformada
  */
  static QString dateDMAtoAMD( const QString & f );

  /**
  Convierte fechas del tipo AAAA-MM-DD, AAAA-MM-DD o
  AAAAMMDD al tipo DD-MM-AAAA.

  @param  f Cadena de texto con la fecha a transformar
  @return Cadena de texto con la fecha transformada
  */
  static QString dateAMDtoDMA( const QString & f );

  /**
  Formatea una cadena de texto poniéndole separadores de miles.

  La cadena que se pasa se supone que un número, convirtiendola
  con QString::toDouble(), si la cadena no es número el resultado es imprevisible.

  @param  s Cadena de texto a la que se le quieren poder separadores de miles
  @return Devuelve la cadena formateada con los separadores de miles
  */
  static QString formatoMiles( const QString & s );

  /**
  Traducción de una cadena al idioma local

  Se hace una llamada a la función tr() de la clase QObject para hacer la traducción.
  Se utiliza para traducciones desde fuera de objetos QObject

  @param  contexto  Contexto en el que se encuentra la cadena, generalmente se refiere a 
                    la clase en la que está definida
  @param  s         Cadena de texto a traducir
  @return Devuelve la cadena traducida al idioma local
  */
  static QString translate( const QString & contexto, const QString & s );

  /**
  Devuelve si el numero de tarjeta de credito es valido.

  El parametro que se pasa es la cadena de texto que contiene el numero de tarjeta.

  @param  num Cadena de texto con el numero de tarjeta
  @return Devuelve verdadero si el numero de tarjeta es valido
  */
  static bool numCreditCard( const QString & num );

  /**
  Traducción de una cadena al idioma local

  @param  s Cadena de texto a traducir
  @return Devuelve la cadena traducida al idioma local
  */
  static QString tr( const QString & s );

  /**
  Devuelve el siguiente valor de la secuencia segun la profundidad indicada por nivel.
  Para explicar el funcionamiento pondremos un ejemplo. Supongamos una secuencia tipo %A-%N.
  %A indica que se coloque en esa posicion una secuencia en letras y %N una secuencia en numero.
  La numeración de niveles va de derecha a izquierda asi el nivel 1 es %N y el nivel 2 %A.
  Si hacemos un nextSequence a nivel 1 el valor de vuelto será un %A que estubiera y un %N sumando 1
  al anterior. Si el nivel es 2 obtendremos un %A + 1, trasformado a letras, y todos los niveles a
  la derecha de este se ponen a 1 o su correspondiente en letra que seria A.

  @param  nivel     Indica la profundidad a la que se hace el incremento.
  @param  secuencia Estructura de la secuencia.
  @param  ultimo    Ultimo valor de la secuencia para poder dar el siguiente valor.
  @return La secuencia en el formato facilitado.
  */
  static QString nextSequence( int nivel, const QString & secuencia, const QString & ultimo );

  /**
  Comprueba si la cabecera de un fichero de definición corresponde
  con las soportadas por AbanQ.

  Este método no sirve para los scripts, sólo para los ficheros de definición;
  mtd, ui, qry, xml, ts y kut.

  @param  head  Cadena de caracteres con la cabecera del fichero, bastaría
                con las tres o cuatro primeras linea del fichero no vacías
  @return TRUE si es un fichero soportado, FALSE en caso contrario
  */
  static bool isAQDefFile( const QString & head );

  /**
  Devuelve el tipo de fichero de AbanQ segun el contenido de la cabecera.

  Este método no sirve para los scripts, sólo para los ficheros de definición;
  mtd, ui, qry, xml, ts y kut.

  @param  head  Cadena de caracteres con la cabecera del fichero, bastaría
                con las tres o cuatro primeras linea del fichero no vacías
  @return AQCore::TypeDefFile que identica el tipo
  */
  static AQCore::TypeDefFile typeAQDefFile( const QString & head );

  /**
  Suma dias a una fecha.

  @param  d   Fecha con la que operar
  @param  nd  Numero de dias que sumar. Si es negativo resta dias
  @return Fecha con el desplazamiento de dias
  */
  static QDate addDays( const QDate & d, int nd );

  /**
  Suma meses a una fecha.

  @param  d   Fecha con la que operar
  @param  nm  Numero de meses que sumar. Si es negativo resta meses
  @return Fecha con el desplazamiento de meses
  */
  static QDate addMonths( const QDate & d, int nm );

  /**
  Suma años a una fecha.

  @param  d   Fecha con la que operar
  @param  nm  Numero de años que sumar. Si es negativo resta años
  @return Fecha con el desplazamiento de años
  */
  static QDate addYears( const QDate & d, int ny );

  /**
  Diferencia de dias desde una fecha a otra.

  @param  d1  Fecha de partida
  @param  d2  Fecha de destino
  @return Número de días entre d1 y d2. Será negativo si d2 es anterior a d1.
  */
  static int daysTo( const QDate & d1, const QDate & d2 );

  /**
  Establece el contenido de un documento XML.

  Establece un documento DOM a partir del XML. Chequea errores, y si existen
  muestra el error encontrado y la linea y columna donde se encuentra.

  @param doc Documento DOM a establecer
  @param content Contenido XML
  @return FALSE si hubo fallo, TRUE en caso contrario
  */
  static bool domDocumentSetContent( QDomDocument & doc, const QString & content );

  /**
  Obtiene la clave SHA1 de una cadena de texto.

  @param  str Cadena de la que obtener la clave SHA1
  @return Clave correspondiente en digitos hexadecimales
  */
  static QString sha1( const QString & str );

  /**
  Hace lo mismo que el anterior método pero sobre un ByteArray
  */
  static QString sha1( const QByteArray & ba );

  /**
  Hace lo mismo que el anterior método pero sobre un fichero de disco
  */
  static QString sha1File( char *szFileName );

  /**
  Decodifica un tipo de AbanQ a un tipo QVariant

  @param  fltype  Tipo de datos de AbanQ.
  @return Tipo de datos QVariant.
  */
  static QVariant::Type aqDecodeType( int fltype );

  /**
  Devuelve una cadena de dos caracteres con el código de idioma del sistema

  @return Código de idioma del sistema
  */
  static QString getLang();

  /**
  Devuelve el sistema operativo sobre el que se ejecuta el programa

  @return Código del sistema operativo (WIN32, LINUX, MACX)
  */
  static QString getOS();

  /**
  Convierte una cadena que es una serie de letras en su correspondiente valor numerico.

  @param  letter  Cadena con la serie.
  @return Una cadena pero que contiene numeros.
  */
  static QString serialLettertoNumber( const QString & letter );

  /**
  Convierte un número a su correspondiente secuencia de Letras.

  @param  number  Número a convertir
  */
  static QString serialNumbertoLetter( int number );

private:

  /**
  Vector con literales de unidades
  */
  static const char * vecUnidades[];

  /**
  Vector con literales de decenas
  */
  static const char * vecDecenas[];

  /**
  Vector con literales de cetenas
  */
  static const char * vecCentenas[];
};

#endif /*AQCOREUTIL_H_*/
