/***************************************************************************
AQAccessControl.h
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

#ifndef AQACCESSCONTROL_H_
#define AQACCESSCONTROL_H_

class QDomElement;
class QDomDocument;
class AQAccessControlPrivate;

/**
Clase base para Controles de Acceso, también denominados Reglas de Control de Acceso.
 
Una regla de control de acceso se aplica a un usuario y a un objeto de alto nivel o contenedor
(ventanas principales,tablas, etc..), que a su vez contendrán otros objetos (acciones, campos, etc..).
La regla está definida por la siguiente información como cabecera de la misma, que la identifica
unívocamente:
 
\code
 
         tipo           ;         nombre         ;      usuario           ;      permiso
--------------------------------------------------------------------------------------------------
AQAccessControl::type ; AQAccessControl::name ; AQAccessControl::user ; AQAccessControl::perm
 
\endcode
 
El tipo será el del objeto de alto nivel, el nombre será el del objeto, el usuario corresponderá al
nombre del usuario en la base de datos al que se le aplica la regla y permiso será un identificador
de texto que define el tipo de permiso que se atribuye al objeto para el usuario dado. Este permiso
es general o global y se aplicará por defecto a todos los objetos hijos o que pertenezcan al objeto
de alto nivel.
 
Al mismo tiempo una regla podrá tener una lista de Objetos de Control de Acceso (denominados ACOs,
Access Control Objects), a los que se les quiere aplicar un permiso distinto al general. Los ACOs serán
objetos hijo o pertenecientes al objeto de alto nivel. Internamente la lista de ACOs está compuesta por
tuplas de dos elementos; "nombre de objeto" y "permiso", el nombre de objeto será el que tiene asignado
dentro de la jerarquía de objetos pertenecientes al objeto de alto nivel y permiso será el permiso para ese
objeto y que sobreescribirá al permiso general.
 
Los valores de la regla se podrán establecer a partir de un nodo DOM de un documento DOM/XML, mediante
AQAccessControl::set . De forma recíproca se podrá obtener un nodo DOM con el contenido de la regla,
a insertar en un documento DOM/XML, mediante AQAccessControl::get . La estructura general en XML del nodo DOM
que representa una regla de control de acceso es la siguiente:
 
\code
 
 <[mainwindow,table,etc..] perm="XXX">
  <name>XXX</name>
  <user>XXX</user>
  <aco perm="XXX">XXX</aco>
  ....
  <aco perm="XXX">XXX</aco>
 </[mainwindow,table,etc..]>
 
\endcode
 
Por comodidad, también se proporciona el método AQAccessControl::setAcos, que permite establecer la lista de
ACOs de una regla directamente a partir de una lista de cadenas de texto.
 
Esta clase no está pensada para ser usada directamente, sino como base para clases derivadas que se
encargan específicamente del procesamiento de objetos de alto nivel. Un ejemplo sería AQAccessControlMainWindow,
que se encarga de control de acceso para objetos de alto nivel de tipo "mainwindow", es decir ventanas principales,
como el selector de módulos, o cada una de las ventanas principales de los módulos.
 
@author InfoSiAL S.L.
*/
class AQAccessControl
{
public:

  /**
  Constructor
  */
  AQAccessControl();

  /**
   Destructor
   */
  ~AQAccessControl();

  /**
  Indica si es nulo, sin contenido
  */
  bool isNull() const;

  /**
  Limpia la regla vaciándola y liberando todos los recursos
  */
  void clear();

  /**
  Obtiene el nombre del objeto de alto nivel.

  @return Cadena de texto con el nombre del objeto.
  */
  QString name() const;

  /**
  Obtiene el nombre del usuario de la base de datos.

  @return Cadena de texto con el nombre (login) del usuario.
  */
  QString user() const;

  /**
  Obtiene el permiso general.

  @return Cadena de texto que identifica el permiso a aplicar.
  */
  QString perm() const;

  /**
  Establece el nombre del objeto de alto nivel.

  @param n Nombre del objeto.
  */
  void setName( const QString & n );

  /**
  Establece el nombre del usuario de la base de datos.

  @param u Nombre (login) del usuario.
  */
  void setUser( const QString & u );

  /**
  Establece el permiso general.

  @param p Cadena de texto con el identificador del permiso.
  */
  void setPerm( const QString & p );

  /**
  Define la regla de control de acceso a partir de la información de un nodo DOM de un documento DOM/XML dado.

  @param e Elemento correspondiente al nodo DOM que se utilizará para definir la regla.
  */
  virtual void set( QDomElement * e );

  /**
  A partir del contenido de la regla de control de acceso crea un nodo DOM que insertará como
  hijo del primer nodo de un documento DOM/XML.

  @param d Documento DOM/XML donde se insertará el nodo construido a partir de la regla de control de acceso.
  */
  virtual void get( QDomDocument * d ) const;

  /**
  Establece la lista de ACOs a partir de una lista de cadenas de texto.

  Esta lista de textos deberá tener en sus componentes de orden par los nombre de los objetos,y en los
  componentes de orden impar el permiso a aplicar a ese objeto,
  p.e.: "pbAbrir", "r-", "lblTexto", "--", "tbBorrar", "rw",...

  @param acos Lista de cadenas de texto con los objetos y permisos.
  */
  virtual void setAcos( QStringList & acos );

  /**
  Obtiene una lista de cadenas de texto correspondiente a la lista de ACOs establecida

  El formato de esta lista es igual al descrito en FLAccessControl::setAcos
  p.e.: "pbAbrir", "r-", "lblTexto", "--", "tbBorrar", "rw",...

  @return Lista de cadenas de texto con los objetos y permisos.
  */
  virtual QStringList getAcos() const;

  /**
  Devuelve una constante de texto que identifica el tipo.

  Esta función deberá ser reimplementada en las clases derivadas que se
  encargan del procesamiento de un tipo de objeto concreto y devolver
  el identificador pertinente.

  @return Cadena de texto que identifica al tipo de objeto general de la regla, p.e.: "table".
  */
  virtual QString type() const = 0;

  /**
  Procesa un objeto dado aplicando la regla de control de acceso.

  Este método es virtual puro, y deberá definirse en cada una de las
  clases derivadas que se encargan de control de acceso de un tipo de
  objeto específico. Por lo tanto, qué se tiene que hacer y cómo se debe
  hacer con la información de la regla de control de acceso con respecto
  a un tipo de objeto de alto nivel concreto queda en el ámbito de la clase
  derivada que se encarga de ese tipo de objeto.

  @param obj Puntero general al objeto a procesar.
  */
  virtual void processObject( void * obj ) const = 0;

  /**
  Define la regla de control de acceso a partir de las propiedades de un objeto concreto.

  Las clases derivadas para cada uno de los tipos de objetos deberá implementar cómo se define la
  regla de control a partir de las propiedades del objeto específico.

  @param  obj Puntero general al objeto a procesar.
  */
  virtual void setFromObject( void * obj ) = 0;

protected:

  AQAccessControlPrivate * d_;
};

#endif /*AQACCESSCONTROL_H_*/
