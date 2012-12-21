/***************************************************************************
                              FLUtil.h
                          -------------------
 begin                : Sun Jan 27 2002
 copyright            : (C) 2002-2005 by InfoSiAL S.L.,
                        Eloy Ordu�a Gonz�lez, Andr�s Ot�n Urbano
 email                : mail@infosial.com
***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************
   Este  programa es software libre. Puede redistribuirlo y/o modificarlo
   bajo  los  t�rminos  de  la  Licencia  P�blica General de GNU   en  su
   versi�n 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#ifndef FLUTIL_H
#define FLUTIL_H

#include <qapplication.h>
#include <qvariant.h>
#include <qdict.h>

class QDomDocument;
class QProgressDialog;
class QDate;
class FLSqlCursor;
class FLSqlQuery;

/**
Clase con m�todos, herramientas y utiles necesarios para ciertas operaciones.

Es esta clase se encontrar�n m�todos gen�ricos que
realizan operaciones muy espec�ficas pero que
son necesarios para ciertos procesos habituales
en las distintas tareas a desempe�ar en la gesti�n
empresarial.

@author InfoSiAL S.L.
*/
class FLUtil
{

public:

  /**
  Obtiene la parte entera de un n�mero.

  Dado un n�mero devuelve la parte entera correspondiente, es decir,
  cifras en la parte izquierda de la coma decimal.

  @param n N�mero del que obtener la parte entera. Debe ser positivo
  @return La parte entera del n�mero, que puede ser cero
  */
  static Q_ULLONG partInteger(double n);

  /**
  Obtiene la parte decimal de un n�mero.

  Dado un n�mero devuelve la parte decimal correspondiente, es decir,
  cifras en la parte derecha de la coma decimal.

  @param n N�mero del que obtener la parte decimal. Debe ser positivo
  @return La parte decimal del n�mero, que puede ser cero
  */
  static Q_ULLONG partDecimal(double n);

  /**
  Enunciado de las unidades de un n�mero.

  @param n N�mero a tratar. Debe ser positivo
  */
  static QString unidades(long n);

  /**
  Pasa una cadena a codificaci�n utf-8

  @param s: Cadena
  @return Cadena en formato UTF-8
  */
  static QCString utf8(const QString &s);

  /**
  Enunciado de las centenas de millar de un n�mero.

  @param n N�mero a tratar. Debe ser positivo
  */
  static QString centenamillar(long n);

  /**
  Enunciado de las decenas de un n�mero.

  @param n N�mero a tratar. Debe ser positivo
  */
  static QString decenas(long n);

  /**
  Enunciado de las centenas de un n�mero.

  @param n N�mero a tratar. Debe ser positivo
  */
  static QString centenas(long n);

  /**
  Enunciado de las unidades de millar de un n�mero.

  @param n N�mero a tratar. Debe ser positivo
  */
  static QString unidadesmillar(long n);

  /**
  Enunciado de las decenas de millar de un n�mero.

  @param n N�mero a tratar. Debe ser positivo
  */
  static QString decenasmillar(long n);

  /**
  Obtiene la expresi�n en texto de como se enuncia un n�mero, en castellano.

  Dado un n�mero entero, devuelve su expresi�n en texto de como se
  enuncia de forma hablada; por ejemplo dado el n�mero 130,
  devolver� la cadena de texto "ciento treinta".

  @param n N�mero a transladar a su forma hablada. Debe ser positivo
  @return Cadena de texto con su expresi�n hablada
  */
  static QString enLetra(long n);

  /**
  Obtiene la expresi�n en texto de como se enuncia una cantidad monetaria, en castellano
  y en cualquier moneda indicada.

  Dado un n�mero doble, devuelve su expresi�n en texto de como se
  enuncia de forma hablada en la moneda indicada; por ejemplo dado el n�mero 130.25,
  devolver� la cadena de texto "ciento treinta 'moneda' con veinticinco c�ntimos".

  @param n N�mero a transladar a su forma hablada. Debe ser positivo
  @param m Nombre de la moneda
  @return Cadena de texto con su expresi�n hablada
  */
  static QString enLetraMoneda(double n, const QString &m);

  /**
  Obtiene la expresi�n en texto de como se enuncia una cantidad monetaria, en castellano
  y en Euros.

  Dado un n�mero doble, devuelve su expresi�n en texto de como se
  enuncia de forma hablada en euros; por ejemplo dado el n�mero 130.25,
  devolver� la cadena de texto "ciento treinta euros con veinticinco c�ntimos".

  @param n N�mero a transladar a su forma hablada. Debe ser positivo
  @return Cadena de texto con su expresi�n hablada
  */
  static QString enLetraMonedaEuro(double n);

  /**
  Obtiene la letra asociada al n�mero del D.N.I. espa�ol.

  @param n Numero de D.N.I
  @return Caracter asociado al n�emro de D.N.I
  */
  static QChar letraDni(long n);

  /**
  Obtiene la lista de nombres de campos de la tabla especificada.
  El primer string de la lista contiene el n�mero de campos de la tabla

  @param tabla. Nombre de la tabla
  @return Lista de campos
  */
  static QStringList nombreCampos(const QString &tabla);

  /**
  Obtiene el n�mero del digito de control, para cuentas bancarias.

  Los n�meros de las cuentas corrientes se organizan de la forma siguiente:

  4 Digitos----->C�digo del banco � (ej. 0136 Banco Arabe espa�ol)
  4 Digitos----->C�digo de la oficina
  1 Digito de control------>de los 8 primeros digitos
  1 Digito de control------>del n�mero de cuenta (de los 10 ultimos digitos)
  10 Digitos del n�mero de la cuenta

  Para comprobar el numero de cuenta se pasa primero los 8 primeros digitos
  obteniendo asi el primer digito de control, despu�s se pasan los 10 digitos
  del n�mero de la cuenta obteniendo el segundo digito de control.

  @param n N�mero del que se debe obtener el d�gito de control
  @return Caracter con el d�gito de control asociado al n�mero dado
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
  Formatea una cadena de texto poni�ndole separadores de miles.

  La cadena que se pasa se supone que un n�mero, convirtiendola
  con QString::toDouble(), si la cadena no es n�mero el resultado es imprevisible.

  @param s Cadena de texto a la que se le quieren poder separadores de miles
  @return Devuelve la cadena formateada con los separadores de miles
  */
  static QString formatoMiles(const QString &s);

  /**
  Traducci�n de una cadena al idioma local

  Se hace una llamada a la funci�n tr() de la clase QObject para hacer la traducci�n.
  Se utiliza para traducciones desde fuera de objetos QObject

  @param contexto Contexto en el que se encuentra la cadena, generalmente se refiere a la clase en la que est� definida
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
  el que se busca la ultima referencia. Lo m�s aconsejable es que el tipo
  del campo sea 'String' porque as� se le puede dar formato y ser
  usado para generar un c�digo de barras. De todas formas la funci�n
  soporta tanto que el campo sea de tipo 'String' como de tipo 'double'.

  @param name Nombre del campo
  @param cursor_ Cursor a la tabla donde se encuentra el campo.
  @return Qvariant con el numero siguiente.
  @author Andr�s Ot�n Urbano.
  */
  static QVariant nextCounter(const QString &name, FLSqlCursor *cursor_);

  /**
  dpinelo: Este m�todo es una extensi�n de nextCounter pero permitiendo la introducci�n de una primera
  secuencia de caracteres. Es �til cuando queremos mantener diversos contadores dentro de una misma tabla.
  Ejemplo, Tabla Grupo de clientes: Agregamos un campo prefijo, que ser� una letra: A, B, C, D.
  Queremos que la numeraci�n de los clientes sea del tipo A00001, o B000023. Con esta funci�n, podremos
  seguir usando los m�todos counter cuando agregamos esa letra.
  
  Este metodo devuelve el siguiente valor de un campo tipo contador de una tabla para una serie determinada.

  Este metodo es muy util cuando se insertan registros en los que
  la referencia es secuencial seg�n una secuencia y no nos acordamos de cual fue el �ltimo
  numero usado. El valor devuelto es un QVariant del tipo de campo es
  el que se busca la ultima referencia. Lo m�s aconsejable es que el tipo
  del campo sea 'String' porque as� se le puede dar formato y ser
  usado para generar un c�digo de barras. De todas formas la funci�n
  soporta tanto que el campo sea de tipo 'String' como de tipo 'double'.

  @param serie serie que diferencia los contadores
  @param name Nombre del campo
  @param cursor_ Cursor a la tabla donde se encuentra el campo.
  @return Qvariant con el numero siguiente.
  @author Andr�s Ot�n Urbano.
	 */
  static QVariant nextCounter( const QString &serie, const QString & name, FLSqlCursor * cursor_ );

  /**
  Nos devuelve el siguiente valor de la secuencia segun la profundidad indicada por nivel.
  Para explicar el funcionamiento pondremos un ejemplo. Supongamos una secuencia tipo %A-%N.
  %A indica que se coloque en esa posicion una secuencia en letras y %N una secuencia en numero.
  La numeraci�n de niveles va de derecha a izquierda asi el nivel 1 es %N y el nivel 2 %A.
  Si hacemos un nextSequence a nivel 1 el valor de vuelto ser� un %A que estubiera y un %N sumando 1
  al anterior. Si el nivel es 2 obtendremos un %A + 1, trasformado a letras, y todos los niveles a
  la derecha de este se ponen a 1 o su correspondiente en letra que seria A.

  @param nivel Indica la profundidad a la que se hace el incremento.
  @param secuencia Estructura de la secuencia.
  @param ultimo Ultimo valor de la secuencia para poder dar el siguiente valor.
  @return La secuencia en el formato facilitado.
  @author Andr�s Ot�n Urbano
  */
  static QString nextSequence(int nivel, const QString &secuencia, const QString &ultimo);

  /**
  Para comprobar si la cabecera de un fichero de definici�n corresponde
  con las soportadas por AbanQ.

  Este m�todo no sirve para los scripts, s�lo para los ficheros de definici�n;
  mtd, ui, qry, xml, ts y kut.

  @param head Cadena de caracteres con la cabecera del fichero, bastar�a
    con las tres o cuatro primeras linea del fichero no vac�as
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
  Suma a�os a una fecha.

  @param d Fecha con la que operar
  @param nm Numero de a�os que sumar. Si es negativo resta a�os
  @return Fecha con el desplazamiento de a�os
  */
  static QDate addYears(const QDate &d, int ny);

  /**
  Diferencia de dias desde una fecha a otra.

  @param d1 Fecha de partida
  @param d2 Fecha de destino
  @return N�mero de d�as entre d1 y d2. Ser� negativo si d2 es anterior a d1.
  */
  static int daysTo(const QDate &d1, const QDate &d2);

  /**
  Construye un string a partir de un n�mero, especificando el formato y precisi�n

  @param v. N�mero a convertir a QString
  @param tipo. Formato del n�mero
  @param partDecimal. Precisi�n (n�mero de cifras decimales) del n�mero

  @return Cadena que contiene el n�mero formateado
  */
  static QString buildNumber(const QVariant &v, char tipo, int partDecimal);

  /**
  Lee el valor de un setting en el directorio de la instalaci�n de AbanQ

  @param key. Clave identificadora del setting
  @param def. Valor por defecto en el caso de que el setting no est� establecido
  @param ok. Indicador de que la lectura es correcta

  @return Valor del setting
  */
  static QVariant readSettingEntry(const QString &key, const QString &def = QString::null, bool *ok = 0);

  /**
  Establece el valor de un setting en el directorio de instalaci�n de AbanQ

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
  Redondea un valor en funci�n de la precisi�n especificada para un campo tipo double de la base de datos

  @param n. N�mero a redondear
  @param table. Nombre de la tabla
  @param field. Nombre del campo

  @return N�mero redondeado
  */
  static QString roundFieldValue(const QVariant &n, const QString &table, const QString &field);

  /**
  Ejecuta una query de tipo select, devolviendo los resultados del primer registro encontrado

  @param f: Sentencia from de la query
  @param s: Sentencia select de la query, que ser� el nombre del campo a devolver
  @param w: Sentencia where de la query
  @param tL: Sentencia tableslist de la query. Necesario cuando en la sentencia from se incluya m�s de una tabla
  @param size: N�mero de l�neas encontradas. (-1 si hay error)
  @param connName Nombre de la conexion
  @return Valor resultante de la query o falso si no encuentra nada.
  */
  static QVariant sqlSelect(const QString &f, const QString &s, const QString &w,
                            const QString &tL = QString::null,
                            int *size = 0, const QString &connName = "default");

  /**
  Versi�n r�pida de sqlSelect. Ejecuta directamente la consulta sin realizar comprobaciones.
  Usar con precauci�n.
  */
  static QVariant quickSqlSelect(const QString &f, const QString &s, const QString &w,
                                 const QString &connName = "default");

  /**
  Realiza la inserci�n de un registro en una tabla mediante un objeto FLSqlCursor

  @param t Nombre de la tabla
  @param fL Lista separada con comas de los nombres de los campos
  @param vL Lista separada con comas de los valores correspondientes
  @param connName Nombre de la conexion
  @return Verdadero en caso de realizar la inserci�n con �xito, falso en cualquier otro caso
  */
  static bool sqlInsert(const QString &t, const QString &fL, const QString &vL, const QString &connName = "default");

  /**
  Realiza la modificaci�n de uno o m�s registros en una tabla mediante un objeto FLSqlCursor

  @param t Nombre de la tabla
  @param fL Lista separada con comas de los nombres de los campos
  @param vL Lista separada con comas de los valores correspondientes
  @param w Sentencia where para identificar los registros a editar.
  @param connName Nombre de la conexion
  @return Verdadero en caso de realizar la inserci�n con �xito, falso en cualquier otro caso
  */
  static bool sqlUpdate(const QString &t, const QString &fL, const QString &vL, const QString &w, const QString &connName = "default");

  /**
  Borra uno o m�s registros en una tabla mediante un objeto FLSqlCursor

  @param t Nombre de la tabla
  @param w Sentencia where para identificar los registros a borrar.
  @param connName Nombre de la conexion
  @return Verdadero en caso de realizar la inserci�n con �xito, falso en cualquier otro caso
  */
  static bool sqlDelete(const QString &t, const QString &w, const QString &connName = "default");

  /**
  Versi�n r�pida de sqlDelete. Ejecuta directamente la consulta sin realizar comprobaciones y sin disparar se�ales de commits.
  Usar con precauci�n.
  */
  static bool quickSqlDelete(const QString &t, const QString &w, const QString &connName = "default");

  /**
  Crea un di�logo de progreso

  @param l Label del di�logo
  @param tS N�mero total de pasos a realizar
  */
  static void createProgressDialog(const QString &l, int tS, const QString &id = "default");

  /**
  Destruye el di�logo de progreso
  */
  static void destroyProgressDialog(const QString &id = "default");

  /**
  Establece el grado de progreso del di�logo

  @param p Grado de progreso
  */
  static void setProgress(int p, const QString &id = "default");

  /**
  Cambia el texto de la etiqueta del di�logo

  @param l Etiqueta
  */
  static void setLabelText(const QString &l, const QString &id = "default");

  /**
  Establece el n�mero total de pasos del di�logo

  @param ts N�mero total de pasos
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

  @param n Nombre del fichero que contiene la descricpci�n del formulario.
  */
  static QImage snapShotUI(const QString &n);

  /**
  Salva en un fichero con formato PNG la imagen o captura de pantalla de un formulario.

  @param n Nombre del fichero que contiene la descricpci�n del formulario.
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
  Guarda la imagen de icono de un bot�n de un formulario en un ficher png. Utilizado para documentaci�n

  @param data Contenido de la imagen en una cadena de caracteres
  @param pathFile Ruta completa al fichero donde se guadar� la imagen
  */
  static void saveIconFile(const QString &data, const QString &pathFile);

  /**
  Devuelve una cadena de dos caracteres con el c�digo de idioma del sistema

  @return C�digo de idioma del sistema
  */
  static QString getIdioma();

  /**
  Devuelve el sistema operativo sobre el que se ejecuta el programa

  @return C�digo del sistema operativo (WIN32, LINUX, MACX)
  */
  static QString getOS();

  /**
  Esta funci�n convierte una cadena que es una serie de letras en su correspondiente valor numerico.

  @param letter Cadena con la serie.
  @return Una cadena pero que contiene numeros.
  */
  static QString serialLettertoNumber(const QString &letter);

  /**
  Esta funci�n convierte un numero a su correspondiente secuencia de Letras.

  @param number N�mero a convertir
  */
  static QString serialNumbertoLetter(int number);

  /**
  Busca ficheros recursivamente en las rutas indicadas y seg�n el patr�n indicado

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


  @param  paths   Rutas de b�squeda
  @param  filter  Patr�n de filtrado para los ficheros. Admite varios separados por espacios "*.gif *.png".
                  Por defecto todos, "*"
  @param  breakOnFirstMatch Si es TRUE al encontrar el primer fichero que cumpla el patr�n indicado, termina
                            la b�squeda y devuelve el nombre de ese fichero
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
  @param filename: Ruta al fichero donde se guardar� la imagen
  @param fmt Indica el formato con el que guardar la imagen
  @author Silix
  */
  static void savePixmap(const QString &data, const QString &filename, const char *format);

private:

  /**
  Cuadro de di�logo de progreso que se llama desde los scripts
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
