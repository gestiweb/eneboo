/***************************************************************************
AQAccessControlLists.h
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

#ifndef AQACCESSCONTROLLISTS_H_
#define AQACCESSCONTROLLISTS_H_

class AQAccessControl;
class QObject;

/**
Gestiona listas de control de acceso.
 
Los controles de acceso o reglas de control de acceso que pertenecen a una lista son de la
clase AQAccessControl. Con el método AQAccessControlLists::init se puede establecer una nueva lista
de controles de acceso, para tal fin este método utiliza AQAccessControl::set y una descripción en XML.
 
La lista establecida mediante AQAccessControlLists::init será la utilizada a partir de ese momento
para el control de acceso de los objetos de alto nivel que sean pasados por el método AQAccessControlLists::process.
 
Varios métodos de esta clase utilizan AQAccessControlFactory, por lo tanto los tipos de objetos controladores que se
espera que gestionen distintas reglas de control de acceso deberán estar registrados con anterioridad en AQAccessControlFactory.
 
Ejemplo de descripción XML de una lista de control de acceso :
 
\code
 
<!DOCTYPE ACL>
<ACL>
 <name>prueba</name>
 <form perm="" >
  <name>formRecordclientes</name>
  <user>falbujer</user>
  <aco perm="r-" >pbDomFacturacion</aco>
 </form>
 <mainwindow perm="" >
  <name>container</name>
  <user>falbujer</user>
  <aco perm="--" >flfactteso</aco>
  <aco perm="--" >flfactalma</aco>
 </mainwindow>
 <mainwindow perm="" >
  <name>flfactppal</name>
  <group>contables</group>
  <aco perm="--" >agentes</aco>
 </mainwindow>
 <table perm="" >
  <name>clientes</name>
  <user>falbujer</user>
  <aco perm="--" >cifnif</aco>
 </table>
</ACL>
 
\endcode
 
@author InfoSiAL S.L.
*/
class AQAccessControlLists
{

public:

  /**
  Constructor
  */
  AQAccessControlLists()
  {}

  /**
  Destructor
  */
  ~AQAccessControlLists();

  /**
  Limpia la lista vaciándola y liberando todos los recursos
  */
  void clear();

  /**
  Para obtener el nombre que identifica la lista de control de acceso actualmente establecida.

  @return Nombre la lista de control de acceso actual.
  */
  QString name() const
  {
    return name_;
  }

  /**
  Establece una nueva lista de control de acceso a partir de la definición en XML

  @param  aclXml  Contenido XML con la definición de la lista de control de acceso.
  */
  void init( const QString & aclXml );

  /**
  Procesa un objeto de alto nivel según la lista de control de acceso establecida.

  @param obj  Objeto de alto nivel al que aplicar el control de acceso. Debe ser o heredar de la clase QObject.
  @param user Nombre del usuario a utilizar para el control de acceso
  */
  void process( const QString & user, QObject * obj ) const;

private:

  /**
  Nombre que identifica la lista de control de acceso actualmente establecida.
  */
  QString name_;

  /**
  Diccionario (lista) que mantiene los objetos de las reglas de control de acceso establecidas.
  La clave que identifica a cada objeto está formada por el siguiente literal:

  \code

  AQAccessControl::type + "::" + AQAccessControl::name + "::" + AQAccessControl::user

  \endcode
  */
  QHash<QString, AQAccessControl *> accessControlList_;
};

#endif /*AQACCESSCONTROLLISTS_H_*/
