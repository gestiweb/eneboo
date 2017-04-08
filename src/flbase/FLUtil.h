/***************************************************************************
                              FLUtil.h
                          -------------------
 begin                : Sun Jan 27 2002
 copyright            : (C) 2002-2005 by InfoSiAL S.L.,
                        Eloy Orduña González, Andrés Otón Urbano
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

#ifndef FLUTIL_H
#define FLUTIL_H

#include <qapplication.h>
#include <qvariant.h>
#include <qdict.h>

#include "AQGlobal.h"

class QDomDocument;
class QProgressDialog;
class QDate;
class FLSqlCursor;
class FLSqlQuery;

/**
Clase con métodos, herramientas y utiles necesarios para ciertas operaciones.

Es esta clase se encontrarán métodos genéricos que
realizan operaciones muy específicas pero que
son necesarios para ciertos procesos habituales
en las distintas tareas a desempeñar en la gestión
empresarial.

@author InfoSiAL S.L.
*/
class AQ_EXPORT FLUtil
{

public:

  /**
  Obtiene la parte entera de un número.

  Dado un número devuelve la parte entera correspondiente, es decir,
  cifras en la parte izquierda de la coma decimal.

  @param n Número del que obtener la parte entera. Debe ser positivo
  @return La parte entera del número, que puede ser cero
  */
  static Q_ULLONG partInteger(double n);

  /**
  Obtiene la parte decimal de un número.

  Dado un número devuelve la parte decimal correspondiente, es decir,
  cifras en la parte derecha de la coma decimal.

  @param n Número del que obtener la parte decimal. Debe ser positivo
  @return La parte decimal del número, que puede ser cero
  */
  static Q_ULLONG partDecimal(double n);

  /**
  Enunciado de las unidades de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString unidades(long n);

  /**
  Pasa una cadena a codificación utf-8

  @param s: Cadena
  @return Cadena en formato UTF-8
  */
  static QCString utf8(const QString &s);

  /**
  Enunciado de las centenas de millar de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString centenamillar(long n);

  /**
  Enunciado de las decenas de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString decenas(long n);

  /**
  Enunciado de las centenas de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString centenas(long n);

  /**
  Enunciado de las unidades de millar de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString unidadesmillar(long n);

  /**
  Enunciado de las decenas de millar de un número.

  @param n Número a tratar. Debe ser positivo
  */
  static QString decenasmillar(long n);

  /**
  Obtiene la expresión en texto de como se enuncia un número, en castellano.

  Dado un número entero, devuelve su expresión en texto de como se
  enuncia de forma hablada; por ejemplo dado el número 130,
  devolverá la cadena de texto "ciento treinta".

  @param n Número a transladar a su forma hablada. Debe ser positivo
  @return Cadena de texto con su expresión hablada
  */
  static QString enLetra(long n);

  /**
  Obtiene la expresión en texto de como se enuncia una cantidad monetaria, en castellano
  y en cualquier moneda indicada.

  Dado un número doble, devuelve su expresión en texto de como se
  enuncia de forma hablada en la moneda indicada; por ejemplo dado el número 130.25,
  devolverá la cadena de texto "ciento treinta 'moneda' con veinticinco céntimos".

  @param n Número a transladar a su forma hablada. Debe ser positivo
  @param m Nombre de la moneda
  @return Cadena de texto con su expresión hablada
  */
  static QString enLetraMoneda(double n, const QString &m);

  /**
  Obtiene la expresión en texto de como se enuncia una cantidad monetaria, en castellano
  y en Euros.

  Dado un número doble, devuelve su expresión en texto de como se
  enuncia de forma hablada en euros; por ejemplo dado el número 130.25,
  devolverá la cadena de texto "ciento treinta euros con veinticinco céntimos".

  @param n Número a transladar a su forma hablada. Debe ser positivo
  @return Cadena de texto con su expresión hablada
  */
  static QString enLetraMonedaEuro(double n);

  /**
  Obtiene la letra asociada al némero del D.N.I. español.

  @param n Numero de D.N.I
  @return Caracter asociado al núemro de D.N.I
  */
  static QChar letraDni(long n);

  /**
  Obtiene la lista de nombres de campos de la tabla especificada.
  El primer string de la lista contiene el número de campos de la tabla

  @param tabla. Nombre de la tabla
  @return Lista de campos
  */
  static QStringList nombreCampos(const QString &tabla);

  /**
  Obtiene el número del digito de control, para cuentas bancarias.

  Los números de las cuentas corrientes se organizan de la forma siguiente:

  4 Digitos----->Código del banco   (ej. 0136 Banco Arabe español)
  4 Digitos----->Código de la oficina
  1 Digito de control------>de los 8 primeros digitos
  1 Digito de control------>del número de cuenta (de los 10 ultimos digitos)
  10 Digitos del número de la cuenta

  Para comprobar el numero de cuenta se pasa primero los 8 primeros digitos
  obteniendo asi el primer digito de control, después se pasan los 10 digitos
  del número de la cuenta obteniendo el segundo digito de control.

  @param n Número del que se debe obtener el dígito de control
  @return Caracter con el dígito de control asociado al número dado
  */
  static QChar calcularDC(const QString &n);

  /**
  Convierte fechas del tipo DD-MM-AAAA, DD/MM/AAAA o
  DDMMAAAA al tipo AAAA-MM-DD.

  @param  f Cadena de texto con la fecha a transformar
  @return Cadena de texto con la fecha transformada
  */
  static QString dateDMAtoAMD(const QString &f);

  /**
  Convierte fechas del tipo AAAA-MM-DD, AAAA-MM-DD o
  AAAAMMDD al tipo DD-MM-AAAA.

  @param  f Cadena de texto con la fecha a transformar
  @return Cadena de texto con la fecha transformada
  */
  static QString dateAMDtoDMA(const QString &f);

  /**
  Formatea una cadena de texto poniéndole separadores de miles.

  La cadena que se pasa se supone que un número, convirtiendola
  con QString::toDouble(), si la cadena no es número el resultado es imprevisible.

  @param s Cadena de texto a la que se le quieren poder separadores de miles
  @return Devuelve la cadena formateada con los separadores de miles
  */
  static QString formatoMiles(const QString &s);

  /**
  Traducción de una cadena al idioma local

  Se hace una llamada a la función tr() de la clase QObject para hacer la traducción.
  Se utiliza para traducciones desde fuera de objetos QObject

  @param contexto Contexto en el que se encuentra la cadena, generalmente se refiere a la clase en la que está definida
  @param s Cadena de texto a traducir
  @return Devuelve la cadena traducida al idioma local
  */
  static QString translate(const QString &contexto, const QString &s) {
    return qApp->translate(contexto, s);
  }

  /**
  Devuelve si el numero de tarjeta de Credito es valido.

  El parametro que se pasa es la cadena de texto que contiene el numero de tarjeta.

  @param num Cadena de texto con el numero de tarjeta
  @return Devuelve verdadero si el numero de tarjeta es valido
  */
  static bool numCreditCard(const QString &num);

  /**
  Este metodo devuelve el siguiente valor de un campo tipo contador de una tabla.

  Este metodo es muy util cuando se insertan registros en los que
  la referencia es secuencial y no nos acordamos de cual fue el ultimo
  numero usado. El valor devuelto es un QVariant del tipo de campo es
  el que se busca la ultima referencia. Lo más aconsejable es que el tipo
  del campo sea 'String' porque así se le puede dar formato y ser
  usado para generar un código de barras. De todas formas la función
  soporta tanto que el campo sea de tipo 'String' como de tipo 'double'.

  @param name Nombre del campo
  @param cursor_ Cursor a la tabla donde se encuentra el campo.
  @return Qvariant con el numero siguiente.
  @author Andrés Otón Urbano.
  */
  static QVariant nextCounter(const QString &name, FLSqlCursor *cursor_);

  /**
  dpinelo: Este método es una extensión de nextCounter pero permitiendo la introducción de una primera
  secuencia de caracteres. Es útil cuando queremos mantener diversos contadores dentro de una misma tabla.
  Ejemplo, Tabla Grupo de clientes: Agregamos un campo prefijo, que será una letra: A, B, C, D.
  Queremos que la numeración de los clientes sea del tipo A00001, o B000023. Con esta función, podremos
  seguir usando los métodos counter cuando agregamos esa letra.
  
  Este metodo devuelve el siguiente valor de un campo tipo contador de una tabla para una serie determinada.

  Este metodo es muy util cuando se insertan registros en los que
  la referencia es secuencial según una secuencia y no nos acordamos de cual fue el último
  numero usado. El valor devuelto es un QVariant del tipo de campo es
  el que se busca la ultima referencia. Lo más aconsejable es que el tipo
  del campo sea 'String' porque así se le puede dar formato y ser
  usado para generar un código de barras. De todas formas la función
  soporta tanto que el campo sea de tipo 'String' como de tipo 'double'.

  @param serie serie que diferencia los contadores
  @param name Nombre del campo
  @param cursor_ Cursor a la tabla donde se encuentra el campo.
  @return Qvariant con el numero siguiente.
  @author Andrés Otón Urbano.
	 */
  static QVariant nextCounter( const QString &serie, const QString & name, FLSqlCursor * cursor_ );

  /**
  Nos devuelve el siguiente valor de la secuencia segun la profundidad indicada por nivel.
  Para explicar el funcionamiento pondremos un ejemplo. Supongamos una secuencia tipo %A-%N.
  %A indica que se coloque en esa posicion una secuencia en letras y %N una secuencia en numero.
  La numeración de niveles va de derecha a izquierda asi el nivel 1 es %N y el nivel 2 %A.
  Si hacemos un nextSequence a nivel 1 el valor de vuelto será un %A que estubiera y un %N sumando 1
  al anterior. Si el nivel es 2 obtendremos un %A + 1, trasformado a letras, y todos los niveles a
  la derecha de este se ponen a 1 o su correspondiente en letra que seria A.

  @param nivel Indica la profundidad a la que se hace el incremento.
  @param secuencia Estructura de la secuencia.
  @param ultimo Ultimo valor de la secuencia para poder dar el siguiente valor.
  @return La secuencia en el formato facilitado.
  @author Andrés Otón Urbano
  */
  static QString nextSequence(int nivel, const QString &secuencia, const QString &ultimo);

  /**
  Para comprobar si la cabecera de un fichero de definición corresponde
  con las soportadas por AbanQ.

  Este método no sirve para los scripts, sólo para los ficheros de definición;
  mtd, ui, qry, xml, ts y kut.

  @param head Cadena de caracteres con la cabecera del fichero, bastaría
    con las tres o cuatro primeras linea del fichero no vacías
  @return TRUE si es un fichero soportado, FALSE en caso contrario
  */
  static bool isFLDefFile(const QString &head);

  /**
  Suma dias a una fecha.

  @param d Fecha con la que operar
  @param nd Numero de dias que sumar. Si es negativo resta dias
  @return Fecha con el desplazamiento de dias
  */
  static QDate addDays(const QDate &d, int nd);

  /**
  Suma meses a una fecha.

  @param d Fecha con la que operar
  @param nm Numero de meses que sumar. Si es negativo resta meses
  @return Fecha con el desplazamiento de meses
  */
  static QDate addMonths(const QDate &d, int nm);

  /**
  Suma años a una fecha.

  @param d Fecha con la que operar
  @param nm Numero de años que sumar. Si es negativo resta años
  @return Fecha con el desplazamiento de años
  */
  static QDate addYears(const QDate &d, int ny);

  /**
  Diferencia de dias desde una fecha a otra.

  @param d1 Fecha de partida
  @param d2 Fecha de destino
  @return Número de días entre d1 y d2. Será negativo si d2 es anterior a d1.
  */
  static int daysTo(const QDate &d1, const QDate &d2);

  /**
  Construye un string a partir de un número, especificando el formato y precisión

  @param v. Número a convertir a QString
  @param tipo. Formato del número
  @param partDecimal. Precisión (número de cifras decimales) del número

  @return Cadena que contiene el número formateado
  */
  static QString buildNumber(const QVariant &v, char tipo, int partDecimal);

  /**
  Lee el valor de un setting en el directorio de la instalación de AbanQ

  @param key. Clave identificadora del setting
  @param def. Valor por defecto en el caso de que el setting no esté establecido
  @param ok. Indicador de que la lectura es correcta

  @return Valor del setting
  */
  static QVariant readSettingEntry(const QString &key, const QString &def = QString::null, bool *ok = 0);

  /**
  Establece el valor de un setting en el directorio de instalación de AbanQ

  @param key. Clave identificadora del setting
  @param Valor del setting

  @return Indicador de si la escritura del settings se realiza correctamente
  */
  static bool writeSettingEntry(const QString &key, const QString &value);

  /**
  Lee el valor de un setting en la tabla flsettings

  @param key. Clave identificadora del setting

  @return Valor del setting
  */
  static QVariant readDBSettingEntry(const QString &key);

  /**
  Establece el valor de un setting en la tabla flsettings

  @param key. Clave identificadora del setting
  @param Valor del setting

  @return Indicador de si la escritura del settings se realiza correctamente
  */
  static bool writeDBSettingEntry(const QString &key, const QString &value);

  /**
  Redondea un valor en función de la precisión especificada para un campo tipo double de la base de datos

  @param n. Número a redondear
  @param table. Nombre de la tabla
  @param field. Nombre del campo

  @return Número redondeado
  */
  static QString roundFieldValue(const QVariant &n, const QString &table, const QString &field);

  /**
  Ejecuta una query de tipo select, devolviendo los resultados del primer registro encontrado

  @param f: Sentencia from de la query
  @param s: Sentencia select de la query, que será el nombre del campo a devolver
  @param w: Sentencia where de la query
  @param tL: Sentencia tableslist de la query. Necesario cuando en la sentencia from se incluya más de una tabla
  @param size: Número de líneas encontradas. (-1 si hay error)
  @param connName Nombre de la conexion
  @return Valor resultante de la query o falso si no encuentra nada.
  */
  static QVariant sqlSelect(const QString &f, const QString &s, const QString &w,
                            const QString &tL = QString::null,
                            int *size = 0, const QString &connName = "default");

  /**
  Versión rápida de sqlSelect. Ejecuta directamente la consulta sin realizar comprobaciones.
  Usar con precaución.
  */
  static QVariant quickSqlSelect(const QString &f, const QString &s, const QString &w,
                                 const QString &connName = "default");

  /**
  Realiza la inserción de un registro en una tabla mediante un objeto FLSqlCursor

  @param t Nombre de la tabla
  @param fL Lista separada con comas de los nombres de los campos
  @param vL Lista separada con comas de los valores correspondientes
  @param connName Nombre de la conexion
  @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
  */
  static bool sqlInsert(const QString &t, const QString &fL, const QString &vL, const QString &connName = "default");

  /**
  Realiza la modificación de uno o más registros en una tabla mediante un objeto FLSqlCursor

  @param t Nombre de la tabla
  @param fL Lista separada con comas de los nombres de los campos
  @param vL Lista separada con comas de los valores correspondientes
  @param w Sentencia where para identificar los registros a editar.
  @param connName Nombre de la conexion
  @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
  */
  static bool sqlUpdate(const QString &t, const QString &fL, const QString &vL, const QString &w, const QString &connName = "default");

  /**
  Borra uno o más registros en una tabla mediante un objeto FLSqlCursor

  @param t Nombre de la tabla
  @param w Sentencia where para identificar los registros a borrar.
  @param connName Nombre de la conexion
  @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
  */
  static bool sqlDelete(const QString &t, const QString &w, const QString &connName = "default");

  /**
  Versión rápida de sqlDelete. Ejecuta directamente la consulta sin realizar comprobaciones y sin disparar señales de commits.
  Usar con precaución.
  */
  static bool quickSqlDelete(const QString &t, const QString &w, const QString &connName = "default");

  /**
  Crea un diálogo de progreso

  @param l Label del diálogo
  @param tS Número total de pasos a realizar
  */
  static void createProgressDialog(const QString &l, int tS, const QString &id = "default");

  /**
  Destruye el diálogo de progreso
  */
  static void destroyProgressDialog(const QString &id = "default");

  /**
  Establece el grado de progreso del diálogo

  @param p Grado de progreso
  */
  static void setProgress(int p, const QString &id = "default");

  /**
  Cambia el texto de la etiqueta del diálogo

  @param l Etiqueta
  */
  static void setLabelText(const QString &l, const QString &id = "default");

  /**
  Establece el número total de pasos del diálogo

  @param ts Número total de pasos
  */
  static void setTotalSteps(int tS, const QString &id = "default");

  /**
  Establece el contenido de un documento XML.

  Establece un documento DOM a partir del XML. Chequea errores, y si existen
  muestra el error encontrado y la linea y columna donde se encuentra.

  @param doc Documento DOM a establecer
  @param content Contenido XML
  @return FALSE si hubo fallo, TRUE en caso contrario
  */
  static bool domDocumentSetContent(QDomDocument &doc, const QString &content);

  /**
  Obtiene la clave SHA1 de una cadena de texto.

  @param str Cadena de la que obtener la clave SHA1
  @return Clave correspondiente en digitos hexadecimales
  */
  static QString sha1(const QString &str);
  static QString usha1(uchar *data, uint len);

  /**
  Obtiene la imagen o captura de pantalla de un formulario.

  @param n Nombre del fichero que contiene la descricpción del formulario.
  */
  static QImage snapShotUI(const QString &n);

  /**
  Salva en un fichero con formato PNG la imagen o captura de pantalla de un formulario.

  @param n Nombre del fichero que contiene la descricpción del formulario.
  @param pathFile Ruta y nombre del fichero donde guardar la imagen
  */
  static void saveSnapShotUI(const QString &n, const QString &pathFile);

  /**
  Decodifica un tipo de AbanQ a un tipo QVariant

  @param fltype Tipo de datos de AbanQ.
  @return Tipo de datos QVariant.
  */
  static QVariant::Type flDecodeType(int fltype);

  /**
  Guarda la imagen de icono de un botón de un formulario en un ficher png. Utilizado para documentación

  @param data Contenido de la imagen en una cadena de caracteres
  @param pathFile Ruta completa al fichero donde se guadará la imagen
  */
  static void saveIconFile(const QString &data, const QString &pathFile);

  /**
  Devuelve una cadena de dos caracteres con el código de idioma del sistema

  @return Código de idioma del sistema
  */
  static QString getIdioma();

  /**
  Devuelve el sistema operativo sobre el que se ejecuta el programa

  @return Código del sistema operativo (WIN32, LINUX, MACX)
  */
  static QString getOS();

  /**
  Esta función convierte una cadena que es una serie de letras en su correspondiente valor numerico.

  @param letter Cadena con la serie.
  @return Una cadena pero que contiene numeros.
  */
  static QString serialLettertoNumber(const QString &letter);

  /**
  Esta función convierte un numero a su correspondiente secuencia de Letras.

  @param number Número a convertir
  */
  static QString serialNumbertoLetter(int number);

  /**
  Busca ficheros recursivamente en las rutas indicadas y según el patrón indicado

  Ejemplo:

  C++:
  QStringList filesFound = FLUtil::findFiles(QStringList() << "/home/user/Documents", "*.odt *.gif");
  for (QStringList::Iterator it = filesFound.begin(); it != filesFound.end(); ++it)
    qWarning(*it);

  QSA:
  var util = new FLUtil;
  var filesFound = util.findFiles( [ "/home/user/Documents" ], "*.odt *.gif");

  for(var i = 0; i < filesFound.length; ++i)
    debug(filesFound[i]);


  @param  paths   Rutas de búsqueda
  @param  filter  Patrón de filtrado para los ficheros. Admite varios separados por espacios "*.gif *.png".
                  Por defecto todos, "*"
  @param  breakOnFirstMatch Si es TRUE al encontrar el primer fichero que cumpla el patrón indicado, termina
                            la búsqueda y devuelve el nombre de ese fichero
  @return Lista de los nombres de los ficheros encontrados
  */
  static QStringList findFiles(const QStringList &paths, const QString &filter = "*",
                               bool breakOnFirstMatch = false);

  /**
  Uso interno
  */
  static bool execSql(const QString &sql, const QString &connName = "default");

  /**
  Guarda imagen Pixmap en una ruta determinada.

  @param data Contenido de la imagen en una cadena de caracteres
  @param filename: Ruta al fichero donde se guardará la imagen
  @param fmt Indica el formato con el que guardar la imagen
  @author Silix
  */
  static void savePixmap(const QString &data, const QString &filename, const char *format);
  
  /**
  Descarga un NOTIFY pendiente, o QString::null

  @return "NOTIFY_NAME :: NOTIFY_EXTRADATA"
  */
  static QString getNotifies();
  

private:

  /**
  Cuadro de diálogo de progreso que se llama desde los scripts
  */
  static QDict<QProgressDialog> dictProgressD_;

  /**
  Vector con literales de unidades
  */
  static const char *vecUnidades[];

  /**
  Vector con literales de decenas
  */
  static const char *vecDecenas[];

  /**
  Vector con literales de cetenas
  */
  static const char *vecCentenas[];
};

#endif
