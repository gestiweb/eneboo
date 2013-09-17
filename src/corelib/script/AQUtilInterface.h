/***************************************************************************
AQUtilInterface.h
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

#ifndef AQUTILINTERFACE_H_
#define AQUTILINTERFACE_H_

#include <QDate>
#include <QStringList>

#include <AQScript/AQScript.h>
#include <AQScript/AQObjectInterfaceQSA.h>
#include <AQScript/AQScriptEngineQSA.h>

class AQUiUtil;

/**
Interfaz estática para AQ*Util
*/
class AQUtilInterface : public AQObjectInterfaceQSA
{
  Q_OBJECT

public Q_SLOTS:

  /**
  Obtiene la parte entera de un número.

  Dado un número devuelve la parte entera correspondiente, es decir,
  cifras en la parte izquierda de la coma decimal.

  @param  n Número del que obtener la parte entera. Debe ser positivo
  @return La parte entera del número, que puede ser cero
  */
  long partInteger( double n ) const;

  /**
  Obtiene la parte decimal de un número.

  Dado un número devuelve la parte decimal correspondiente, es decir,
  cifras en la parte derecha de la coma decimal.

  @param  n Número del que obtener la parte decimal. Debe ser positivo
  @return La parte decimal del número, que puede ser cero
  */
  long partDecimal( double n ) const;

  /**
  Construye un string a partir de un número, especificando el formato y precisión

  @param  v            Número a convertir a QString
  @param  tipo         Formato del número
  @param  partDecimal  Precisión (número de cifras decimales) del número
  @return Cadena que contiene el número formateado
  */
  QString buildNumber( const QVariant & v, char tipo, int partDecimal ) const;

  /**
  Lee el valor de un setting en el directorio de la instalación de AbanQ

  @param  key Clave identificadora del setting
  @param  def Valor por defecto en el caso de que el setting no esté establecido
  @param  ok  Indicador de que la lectura es correcta
  @return Valor del setting
  */
  QVariant readSettingEntry( const QString & key, const QString & def = QString(), bool * ok = 0 ) const;

  /**
  Establece el valor de un setting en el directorio de instalación de AbanQ

  @param  key Clave identificadora del setting
  @param  Valor del setting
  @return Indicador de si la escritura del settings se realiza correctamente
  */
  bool writeSettingEntry( const QString & key, const QString & value ) const;

  /**
  Lee el valor de un setting en la tabla flsettings

  @param  key       Clave identificadora del setting
  @param  connName  Nombre de la conexion
  @return Valor del setting
  */
  QVariant readDBSettingEntry( const QString & key, const QString & connName = "default" ) const;

  /**
  Establece el valor de un setting en la tabla flsettings

  @param  key       Clave identificadora del setting
  @param  value     Valor del setting
  @param  connName  Nombre de la conexion
  @return Indicador de si la escritura del settings se realiza correctamente
  */
  bool writeDBSettingEntry( const QString & key, const QString & value,
                            const QString & connName = "default" ) const;

  /**
  Enunciado de las unidades de un número.

  @param n Número a tratar. Debe ser positivo
  */
  QString unidades( long n ) const;

  /**
  Enunciado de las centenas de millar de un número.

  @param n Número a tratar. Debe ser positivo
  */
  QString centenamillar( long n ) const;

  /**
  Enunciado de las decenas de un número.

  @param n Número a tratar. Debe ser positivo
  */
  QString decenas( long n ) const;

  /**
  Enunciado de las centenas de un número.

  @param n Número a tratar. Debe ser positivo
  */
  QString centenas( long n ) const;

  /**
  Enunciado de las unidades de millar de un número.

  @param n Número a tratar. Debe ser positivo
  */
  QString unidadesmillar( long n ) const;

  /**
  Pasa una cadena a codificación utf-8

  @param  s Cadena
  @return Cadena en formato UTF-8
  */
  QByteArray utf8( const QString & s ) const;

  /**
  Enunciado de las decenas de millar de un número.

  @param n Número a tratar. Debe ser positivo
  */
  QString decenasmillar( long n ) const;

  /**
  Obtiene la expresión en texto de como se enuncia un número, en castellano.

  Dado un número entero, devuelve su expresión en texto de como se
  enuncia de forma hablada; por ejemplo dado el número 130,
  devolverá la cadena de texto "ciento treinta".

  @param  n   Número a transladar a su forma hablada. Debe ser positivo
  @return Cadena de texto con su expresión hablada
  */
  QString enLetra( long n ) const;

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
  QString enLetraMoneda( double n, const QString & m ) const;

  /**
  Obtiene la expresión en texto de como se enuncia una cantidad monetaria, en castellano
  y en Euros.

  Dado un número doble, devuelve su expresión en texto de como se
  enuncia de forma hablada en euros; por ejemplo dado el número 130.25,
  devolverá la cadena de texto "ciento treinta euros con veinticinco céntimos".

  @param  n Número a transladar a su forma hablada. Debe ser positivo
  @return Cadena de texto con su expresión hablada
  */
  QString enLetraMonedaEuro( double n ) const;

  /**
  Calcula la letra asociada a un número de DNI

  @param  n Número de DNI
  @return Letra asociada
  **/
  QString letraDni( long n ) const;

  /**
  Calcula el dígito de control asociado a un número de cuenta bancaria

  @param  n Número de cuenta
  @return Dígito de control
  */
  QString calcularDC( const QString & n ) const;

  /**
  Convierte fechas del tipo DD-MM-AAAA, DD/MM/AAAA o
  DDMMAAAA al tipo AAAA-MM-DD.

  @param  f Cadena de texto con la fecha a transformar
  @return Cadena de texto con la fecha transformada
  */
  QString dateDMAtoAMD( const QString & f ) const;

  /**
  Convierte fechas del tipo AAAA-MM-DD, AAAA-MM-DD o
  AAAAMMDD al tipo DD-MM-AAAA.

  @param  f Cadena de texto con la fecha a transformar
  @return Cadena de texto con la fecha transformada
  */
  QString dateAMDtoDMA( const QString & f ) const;

  /**
  Formatea una cadena de texto poniéndole separadores de miles.

  La cadena que se pasa se supone que un número, convirtiendola
  con QString::toDouble(), si la cadena no es número el resultado es imprevisible.

  @param  s Cadena de texto a la que se le quieren poder separadores de miles
  @return Devuelve la cadena formateada con los separadores de miles
  */
  QString formatoMiles( const QString & s ) const;

  /**
  Traducción de una cadena al idioma local

  Se hace una llamada a la función tr() de la clase QObject para hacer la traducción.
  Se utiliza para traducciones desde fuera de objetos QObject

  @param  contexto  Contexto en el que se encuentra la cadena, generalmente se refiere a 
                    la clase en la que está definida
  @param  s         Cadena de texto a traducir
  @return Devuelve la cadena traducida al idioma local
  */
  QString translate( const QString & contexto, const QString & s ) const;

  /**
  Devuelve si el numero de tarjeta de credito es valido.

  El parametro que se pasa es la cadena de texto que contiene el numero de tarjeta.

  @param  num Cadena de texto con el numero de tarjeta
  @return Devuelve verdadero si el numero de tarjeta es valido
  */
  bool numCreditCard( const QString & num ) const;

  /**
  Este método devuelve el siguiente valor de un campo tipo contador de una tabla.

  Este método es muy util cuando se insertan registros en los que
  la referencia es secuencial y no nos acordamos de cual fue el ultimo
  numero usado. El valor devuelto es un QVariant del tipo de campo es
  el que se busca la ultima referencia. Lo más aconsejable es que el tipo
  del campo sea 'String' porque así se le puede dar formato y ser
  usado para generar un código de barras. De todas formas la función
  soporta tanto que el campo sea de tipo 'String' como de tipo 'double'.

  @param  name    Nombre del campo
  @param  cursor_ Cursor a la tabla donde se encuentra el campo.
  @return Qvariant con el numero siguiente.
  */
  QVariant nextCounter( const QString & name, QObject * cursor_ ) const;

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
  QString nextSequence( int nivel, const QString & secuencia, const QString & ultimo ) const;

  /**
  Convierte una cadena que es una serie de letras en su correspondiente valor numerico.

  @param  letter  Cadena con la serie.
  @return Una cadena pero que contiene numeros.
  */
  QString serialLettertoNumber( const QString & letter ) const;

  /**
  Convierte un número a su correspondiente secuencia de Letras.

  @param  number  Número a convertir
  */
  QString serialNumbertoLetter( int number ) const;

  /**
  Comprueba si la cabecera de un fichero de definición corresponde
  con las soportadas por AbanQ.

  Este método no sirve para los scripts, sólo para los ficheros de definición;
  mtd, ui, qry, xml, ts y kut.

  @param  head  Cadena de caracteres con la cabecera del fichero, bastaría
                con las tres o cuatro primeras linea del fichero no vacías
  @return TRUE si es un fichero soportado, FALSE en caso contrario
  */
  bool isAQDefFile( const QString & head ) const;

  /**
  Obtiene el alias de un campo a partir de su nombre.

  @param  fN        Nombre del campo
  @param  tN        Nombre de la tabla
  @param  connName  Nombre de la conexion
  */
  QString fieldNameToAlias( const QString & fN, const QString & tN,
                            const QString & connName = "default" ) const;

  /**
  Obtiene el alias de una tabla a partir de su nombre.

  @param  tN        Nombre de la tabla
  @param  connName  Nombre de la conexion
  */
  QString tableNameToAlias( const QString & tN, const QString & connName = "default" ) const;

  /**
  Obtiene el nombre de un campo a partir de su alias.

  @param  aN        Nombre del alias del campo
  @param  tN        Nombre de la tabla
  @param  connName  Nombre de la conexion
  */
  QString fieldAliasToName( const QString & aN, const QString & tN,
                            const QString & connName = "default" ) const;

  /**
  Obtiene el tipo de un campo a partir de su nombre.

  @param  fN        Nombre del campo
  @param  tN        Nombre de la tabla
  @param  connName  Nombre de la conexion
  */
  int fieldType( const QString & fN, const QString & tN,
                 const QString & connName = "default" ) const;

  /**
  Obtiene la longitud de un campo a partir de su nombre.

  @param  fN        Nombre del campo
  @param  tN        Nombre de la tabla
  @param  connName  Nombre de la conexion
  */
  int fieldLength( const QString & fN, const QString & tN,
                   const QString & connName = "default" ) const;

  /**
  Obtiene la longitud de un campo a partir de su nombre.

  @param  fN        Nombre del campo
  @param  tN        Nombre de la tabla
  @param  connName  Nombre de la conexion
  */
  bool fieldAllowNull( const QString & fN, const QString & tN,
                       const QString & connName = "default" ) const;

  /**
  Suma dias a una fecha.

  @param  d   Fecha con la que operar
  @param  nd  Numero de dias que sumar. Si es negativo resta dias
  @return Fecha con el desplazamiento de dias
  */
  QDate addDays( const QString & d, int nd ) const;

  /**
  Suma meses a una fecha.

  @param  d   Fecha con la que operar
  @param  nm  Numero de meses que sumar. Si es negativo resta meses
  @return Fecha con el desplazamiento de meses
  */
  QDate addMonths( const QString & d, int nm ) const;

  /**
  Suma años a una fecha.

  @param  d   Fecha con la que operar
  @param  nm  Numero de años que sumar. Si es negativo resta años
  @return Fecha con el desplazamiento de años
  */
  QDate addYears( const QString & d, int ny ) const;

  /**
  Diferencia de dias desde una fecha a otra.

  @param  d1  Fecha de partida
  @param  d2  Fecha de destino
  @return Número de días entre d1 y d2. Será negativo si d2 es anterior a d1.
  */
  int daysTo( const QString & d1, const QString & d2 ) const;

  /**
  Devuelve una lista de los nombres de los campos de una tabla, separados por comas

  @param  table     Nombre de la tabla
  @param  connName  Nombre de la conexion
  @return Lista de todos los nombres de los campos de esa tabla
  */
  QStringList fieldsNames( const QString & table, const QString & connName = "default" ) const;

  /**
  Ejecuta una query de tipo select, devolviendo los resultados del primer registro encontrado

  @param  f         Sentencia from de la query
  @param  s         Sentencia select de la query, que será el nombre del campo a devolver
  @param  w         Sentencia where de la query
  @param  tL        Sentencia tableslist de la query. Necesario cuando en la sentencia from se 
                    incluya más de una tabla
  @param  connName  Nombre de la conexion
  @return Valor resultante de la query o falso si no encuentra nada.
  */
  QVariant sqlSelect( const QString & f, const QString & s, const QString & w,
                      const QString & tL = QString(), const QString & connName = "default" ) const;

  /**
  Realiza la inserción de un registro en una tabla mediante un objeto AQSqlCursor

  @param  t         Nombre de la tabla
  @param  fL        Lista separada con comas de los nombres de los campos
  @param  vL        Lista separada con comas de los valores correspondientes
  @param  connName  Nombre de la conexion
  @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
  */
  bool sqlInsert( const QString & t, const QString & fL, const QString & vL,
                  const QString & connName = "default" ) const;

  /**
  Realiza la modificación de uno o más registros en una tabla mediante un objeto AQSqlCursor

  @param  t         Nombre de la tabla
  @param  fL        Lista separada con comas de los nombres de los campos
  @param  vL        Lista separada con comas de los valores correspondientes
  @param  w         Sentencia where para identificar los registros a editar.
  @param  connName  Nombre de la conexion
  @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
  */
  bool sqlUpdate( const QString & t, const QString & fL, const QString & vL,
                  const QString & w, const QString & connName = "default" ) const;

  /**
  Borra uno o más registros en una tabla mediante un objeto AQSqlCursor

  @param  t         Nombre de la tabla
  @param  w         Sentencia where para identificar los registros a borrar.
  @param  connName  Nombre de la conexion
  @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
  */
  bool sqlDelete( const QString & t, const QString & w,
                  const QString & connName = "default" ) const;

  /**
  Redondea un valor en función de la precisión especificada para un campo tipo double de la 
  base de datos

  @param  n         Número a redondear
  @param  table     Nombre de la tabla
  @param  field     Nombre del campo
  @param  connName  Nombre de la conexion
  @return Número redondeado
  */
  QString roundFieldValue( const QVariant n, const QString & table,
                           const QString & field, const QString & connName = "default" ) const;

  /**
  Crea un diálogo de progreso

  @param  l   Label del diálogo
  @param  tS  Número total de pasos a realizar
  */
  void createProgressDialog( const QString & l, int tS );

  /**
  Crea un diálogo de progreso con valores por defecto
  */
  void createProgressDialog();

  /**
  Destruye el diálogo de progreso
  */
  void destroyProgressDialog() const;

  /**
  Establece el grado de progreso del diálogo

  @param p Grado de progreso
  */
  void setProgress( int p ) const;

  /**
  Establece el número total de pasos del diálogo

  @param ts Número total de pasos
  */
  void setTotalSteps( int tS ) const;

  /**
  Cambia el texto de la etiqueta del diálogo

  @param l Etiqueta
  */
  void setLabelText( const QString & l ) const;

  /**
  Obtiene la clave SHA1 de una cadena de texto.

  @param  str Cadena de la que obtener la clave SHA1
  @return Clave correspondiente en digitos hexadecimales
  */
  QString sha1( const QString & str ) const;

  /**
  Devuelve una cadena de dos caracteres con el código de idioma del sistema

  @return Código de idioma del sistema
  */
  QString getLang() const;

  /**
  Devuelve el sistema operativo sobre el que se ejecuta el programa

  @return Código del sistema operativo (WIN32, LINUX, MACX)
  */
  QString getOS() const;

protected:

  friend class AQCoreObjectFactoryQSA;

  /**
  Constructor
  */
  AQUtilInterface( const AQScriptEngineQSA * eng ) :
      AQObjectInterfaceQSA( eng ),
      guiUtil( 0 )
  {}

  /**
  Destructor
  */
  virtual ~AQUtilInterface();

  AQUiUtil * guiUtil;
};


/**
Interfaz de soporte para AbanQ v2
*/
class AQUtilInterfaceAQ2 : public AQUtilInterface
{
  Q_OBJECT

public:

  /**
  Constructor
  */
  AQUtilInterfaceAQ2( const AQScriptEngineQSA * eng );

public Q_SLOTS:

  uint length( const QString & str ) const
  {
    AQ_PORT_AQ2( AQUtil::length( str ),
                 return str.length(); );
  }

  bool isFLDefFile( const QString & head )
  {
    AQ_PORT_AQ2( AQUtil::isFLDefFile( head ),
                 return isAQDefFile( head ); );
  }

  QStringList nombreCampos( const QString & table,
                            const QString & connName = "default" ) const
  {
    AQ_PORT_AQ2( AQUtil::nombreCampos( table ),
                 return fieldsNames( table, connName ); );
  }

  QImage snapShotUI( const QString & )
  {
    AQ_NO_PORT_AQ2( AQUtil::snapShotUI );
    return QImage();
  }

  void saveSnapShotUI( const QString &, const QString & )
  {
    AQ_NO_PORT_AQ2( AQUtil::saveSnapShotUI );
  }

  void saveIconFile( const QString &, const QString & )
  {
    AQ_NO_PORT_AQ2( AQUtil::saveIconFile );
  }

  QString getIdioma() const
  {
    AQ_PORT_AQ2( AQUtil::getIdioma(),
                 return getLang(); );
  }
};

#endif /*AQUTILINTERFACE_H_*/
