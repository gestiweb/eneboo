/***************************************************************************
AQManagerModules.h
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

#ifndef AQMANAGERMODULES_H_
#define AQMANAGERMODULES_H_

#include <QStringList>
#include <QByteArray>
#include <QSqlQuery>

class AQManagerModulesPrivate;
class AQInfoMod;
class AQSqlDatabase;

/**
Gestor de módulos.
 
Esta clase permite realizar las funciones básicas de manejo de ficheros
de texto que forman parte de los módulos de aplicación, utilizando como
soporte de almacenamiento la base de datos y el sistema de cachés de texto
para optimizar las lecturas.
 
Gestiona la carga y descarga de módulos. Mantiene cual es el módulo activo.
El módulo activo se puede establecer en cualquier momento con
AQManagerModules::setActiveIdModule().
 
Los módulos se engloban en áreas (FACTURACION, FINANCIERA, PRODUCCION, etc..) y
cada módulo tiene varios ficheros de texto XML y scripts. Toda la estructura de
módulos se almacena en las tablas flareas, flmodulos, flserial y flfiles, sirviendo esta
clase como interfaz para el manejo de dicha estructura en el entorno de trabajo
de AbanQ.
 
@author InfoSiAL S.L.
*/
class AQManagerModules
{
public:

  /**
  Obtiene el contenido de un fichero 

  @param n Nombre del fichero.
  @param shaKey Por referencia devuelve la suma SHA del contenido del fichero
  @param cacheKey Por referencia devuelve la clave de caché asignada al fichero
  @return QString con el contenido del fichero o vacio en caso de error.
  */
  QString content( const QString & n, QString & shaKey, QString & cacheKey ) const;

  /**
  Obtiene el contenido de un fichero script como ByteCode si existe en este formato.
  Si el script no existe como ByteCode ( no ha sido compilado ), se devuelve contenido
  vacio.

  @param n Nombre del script.
  @param shaKey Por referencia devuelve la suma SHA del contenido del fichero
  @param cacheKey Por referencia devuelve la clave de caché asignada al fichero
  @return QByteArray con el contenido del fichero o vacio en caso de no existir la
   version ByteCode.
  */
  QByteArray contentByteCode( const QString & n, QString & shaKey, QString & cacheKey ) const;

  /**
  Genera la compilación ByteCode del script indicado ( sólo para los scripts cargados en
  el sistema de ficheros SQL ).

  @param n Nombre del script.
  @return Si hay error de compilación devuelve una lista con los mensajes de error y
    el número de línea donde se producen.
  */
  QStringList makeByteCode( const QString & n ) const;

  /**
  Para determinar si existe el fichero en el sistema de módulos, es decir ha sido cargado

  @param fileName Nombre del script.
  @param shaKey Por referencia devuelve la suma SHA del contenido del fichero
  @param cacheKey Por referencia devuelve la clave de caché asignada al fichero
  @return TRUE si existe el fichero en el sistema de módulos
  */
  bool exists( const QString & fileName, QString & shaKey, QString & cacheKey ) const;

  /**
  Para establecer el módulo activo.

  Automáticamente también establece cual es el área correspondiente al módulo,
  ya que un módulo sólo puede pertenecer a una sola área.

  @param id Identificador del módulo
  */
  void setActiveIdModule( const QString & id ) const;

  /**
  Para obtener el area del módulo activo.

  @return Identificador del area
  */
  QString activeIdArea() const;

  /**
  Para obtener el módulo activo.

  @return Identificador del módulo
  */
  QString activeIdModule() const;

  /**
  Obtiene la lista de identificadores de area cargadas en el sistema.

  @return Lista de identificadores de areas
  */
  QStringList listIdAreas() const;

  /**
  Obtiene la lista de identificadores de módulos cargados en el sistema de una area dada.

  @param idA Identificador del área de la que se quiere obtener la lista módulos
  @return Lista de identificadores de módulos
  */
  QStringList listIdModules( const QString & idA ) const;

  /**
  Obtiene la lista de identificadores de todos los módulos cargados en el sistema.

  @return Lista de identificadores de módulos
  */
  QStringList listAllIdModules() const;

  /**
  Obtiene la descripción de un área a partir de su identificador.

  @param idA Identificador del área.
  @return Texto de descripción del área, si lo encuentra o idA si no lo encuentra.
  */
  QString idAreaToDescription( const QString & idA ) const;

  /**
  Obtiene la descripción de un módulo a partir de su identificador.

  @param idM Identificador del módulo.
  @return Texto de descripción del módulo, si lo encuentra o idM si no lo encuentra.
  */
  QString idModuleToDescription( const QString & idM ) const;

  /**
  Para obtener el identificador del módulo al que pertenece un fichero dado.

  @param n Nombre del fichero incluida la extensión
  @return Identificador del módulo al que pertenece el fichero
  */
  QString idModuleOfFile( const QString & n ) const;

  /**
  Para obtener la versión de un módulo.

  @param idM Identificador del módulo del que se quiere saber su versión
  @return Cadena con la versión
  */
  QString versionModule( const QString & idM ) const;

  /**
  Para obtener el icono asociado a un módulo en forma de array de bytes

  @param idM Identificador del módulo del que obtener el icono
  @return QByteArray con el icono
  */
  QByteArray rawIconModule( const QString & idM ) const;

  /**
  @return TRUE si el objeto se ha inicializado correctamente y está listo para trabajar
  */
  bool isReady() const;

  /**
  Marca con el flag especial 'pubscr' todos aquellos ficheros de script que tienen definida
  una interfaz pública. Este método se usa para optimizar la carga de scripts.
  */
  void markPublicScripts() const;

  /**
  Para obtener la lista de scripts marcados como públicos. Utilizado para optimizar la carga de
  scripts.

  @return Lista con los nombre de ficheros de los scripts marcados como públicos.
  */
  QStringList publicScripts() const;

  /**
  Carga dinámica del recurso de imágenes de los módulos instalados en la
  base de datos
  */
  void loadResourceImages( const QString & rccName ) const;

private:

  /**
  constructor
  */
  inline AQManagerModules()
  {
    d = 0;
  }

  /**
  constructores de copia
  */
  inline AQManagerModules( const AQManagerModules & other )
  {
    d = other.d;
  }
  inline AQManagerModules & operator=( const AQManagerModules & other )
  {
    d = other.d;
    return *this;
  }

  /**
  destructor
  */
  inline ~AQManagerModules()
  {}

  /**
  Acciones de inicialización del sistema de módulos.

  @return TRUE si la inicialización tuvo éxito
  */
  bool init( const AQSqlDatabase & db );

  /**
  Acciones de finalización del sistema de módulos.
  */
  void finish();

  /**
  Privado
  */
  AQManagerModulesPrivate * d;

  friend class AQSqlDatabase;
  friend class AQSqlDatabasePrivate;
};

inline bool AQManagerModules::isReady() const
{
  return !!d;
}

#endif /*AQMANAGERMODULES_H_*/
