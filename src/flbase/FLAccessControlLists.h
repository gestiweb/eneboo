/***************************************************************************
                        FLAccessControlLists.h
                          -------------------
 begin                : Fri Oct 10 2005
 copyright            : (C) 2004-2005 by InfoSiAL S.L.
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

#ifndef FLACCESSCONTROLLISTS_H
#define FLACCESSCONTROLLISTS_H

class FLAccessControl;
class FLTableMetaData;
class FLSqlQuery;

/**
Gestiona listas de control de acceso.

Los controles de acceso o reglas de control de acceso que pertenecen a una lista son de la
clase FLAccessControl. Con el método FLAccessControlLists::init se puede establecer una nueva lista
de controles de acceso, para tal fin este método utiliza FLAccessControl::set y el contenido del fichero "acl.xml",
si este fichero no existe como parte de los ficheros de AbanQ (está en la tabla flfiles) se
establecerá una lista vacía.

La lista establecida mediante FLAccessControlLists::init será la utilizada a partir de ese momento
para el control de acceso de los objetos de alto nivel que sean pasados por el método FLAccessControlLists::process.

Adicionalmente con el método FLAccessControlLists::installACL se puede crear un nuevo fichero "acl.xml"
y almacenarlo como parte de los ficheros de AbanQ, sustituyendo al anterior si existiera. Para la
creación del fichero "acl.xml", este método utiliza la información almacenada en las siguiente tablas del sistema :

- flacls   : Distintas listas de control de acceso -> FLAccessControlLists
- flacs    : Reglas de control de acceso. Cada regla pertenecerá a una lista de control de acceso -> FLAccessControl
- flacos   : Objetos de control de acceso. Cada objeto pertenecerá a una regla de control de acceso -> FLAccessControl::acosPerms_
- flusers  : Usuarios de la base de datos -> FLAccessControl::user
- flgroups : Grupos de usuarios

El método FLAccessControlLists::installACL no regenerará la lista de control de acceso, su única función es crear
el fichero y almacenarlo en la tabla flfiles, para establecer la lista con el nuevo contenido de este fichero hay
que llamar a FLAccessControlLists::init inmediatamente después.

Varios métodos de esta clase utilizan FLAccessControlFactory, por lo tanto los tipos de objetos controladores que se
espera que gestionen distintas reglas de control de acceso deberán estar registrados con anterioridad en FLAccessFactory.

Ejemplo de fichero acl.xml :

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
class FLAccessControlLists
{
public:

  /**
  Constructor
  */
  FLAccessControlLists();

  /**
  Destructor
  */
  ~FLAccessControlLists();

  /**
  Para obtener el nombre que identifica la lista de control de acceso actualmente establecida.

  @return Nombre la lista de control de acceso actual.
  */
  QString name() {
    return name_;
  }

  /**
  Lee el fichero "acl.xml" y establece una nueva lista de control de acceso.

  Si el fichero "acl.xml" no se puede leer, la lista de control de acceso queda vacía y
  no se procesará ningún control de acceso sobre ningún objeto.
  */
  void init();

  /**
  Esencialmente hace lo mismo que el método anterior pero leyendo el contenido desde
  la cadena de texto indicada.

  @param  aclXml  Contenido XML con la definición de la lista de control de acceso.
  */
  void init(const QString &aclXml);

  /**
  Procesa un objeto de alto nivel según la lista de control de acceso establecida.

  @param obj Objeto de alto nivel al que aplicar el control de acceso. Debe ser o heredar de la clase QObject.
  */
  void process(QObject *obj);

  /**
  Crea un nuevo fichero "acl.xml" y lo almacena sustituyendo el anterior, en el caso de que exista.

  @param idacl Identificador del registro de la tabla "flacls" a utilizar para crear "acl.xml".
  */
  void installACL(const QString &idacl);

private:

  /**
  Crea el/los nodo(s) DOM correspondiente(s) a un registro de la tabla "flacs".

  Utiliza FLAccessControlLists::makeRuleUser o FLAccessControlLists::makeRuleGroup dependiendo si el registro
  al que apunta la consulta indica que la regla es para un usuario o un grupo. Si el registro indica a un
  usuario se creará una regla de usuario, si indica a un grupo se creará una regla de usuario por cada uno de
  los usuarios miembros del grupo.

  @param q Consulta sobre la tabla "flacs" posicionada en el registro a utilizar para construir la(s) regla(s).
  @param d Documento DOM/XML en el que insertará(n) el/los nodo(s) que describe(n) la(s) regla(s) de control de acceso.
  */
  void makeRule(FLSqlQuery *q, QDomDocument *d);

  /**
  Crea un nodo DOM correspondiente a un registro de la tabla "flacs" y para un usuario determinado.

  @param q Consulta sobre la tabla "flacs" posicionada en el registro a utilizar para construir la regla.
  @param d Documento DOM/XML en el que insertará el nodo que describe la regla de control de acceso.
  @param iduser Identificador del usuario utilizado en la regla de control de acceso.
  */
  void makeRuleUser(FLSqlQuery *q, QDomDocument *d, const QString &iduser);

  /**
  Crea varios nodos DOM correspondientes a un registro de la tabla "flacs" y para un grupo de usuarios determinado.

  La función de este método es crear una regla para cada uno de los usuarios miembros del grupo, utilizando
  FLAccessControlLists::makeRuleUser.

  @param q Consulta sobre la tabla "flacs" posicionada en el registro a utilizar para construir las reglas.
  @param d Documento DOM/XML en el que insertarán los nodos que describe las reglas de control de acceso.
  @param idgroup Identificador del grupo de usuarios.
  */
  void makeRuleGroup(FLSqlQuery *q, QDomDocument *d, const QString &idgroup);

  /**
  Nombre que identifica la lista de control de acceso actualmente establecida.

  Generalmente corresponderá con el identificador del registro de la tabla "flacls" que se utilizó para crear "acl.xml".
  */
  QString name_;

  /**
  Diccionario (lista) que mantiene los objetos de las reglas de control de acceso establecidas.
  La clave que identifica a cada objeto está formada por el siguiente literal:

  \code

  FLAccessControl::type + "::" + FLAccessControl::name + "::" + FLAccessControl::user

  \endcode
  */
  QDict<FLAccessControl> *accessControlList_;
};

#endif
