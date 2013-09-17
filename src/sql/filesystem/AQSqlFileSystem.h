/***************************************************************************
AQSqlFileSystem.h
-------------------
begin                : 01/09/2007
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

#ifndef AQSQLFILESYSTEM_H_
#define AQSQLFILESYSTEM_H_

#include <AQSql/AQSql.h>
#include <AQSql/AQSqlDatabase.h>
#include <AQSql/AQSqlNodeInfo.h>

class AQSqlFileSystemPrivate;

/**
Virtualización de un sistema de ficheros básico que
utiliza como medio de almacenamiento la base de datos SQL
 
@author InfoSiAL S.L.
*/
class AQSqlFileSystem
{

  friend class AQSqlDir;
  friend class AQSqlDirPrivate;

protected:

  /**
  constructor
  */
  AQSqlFileSystem();

  /**
  destructor
  */
  ~AQSqlFileSystem();

public:

  /**
  Acciones de inicialización.

  @return TRUE si la inicialización tuvo éxito
  */
  bool init( AQSqlDatabase db );

  /**
  Acciones de finalización.
  */
  void finish();

  /**
  @return TRUE si el objeto se ha inicializado correctamente y está listo para trabajar
  */
  bool isReady() const;

  /**
  Establece el identificador del nodo de directorio actual

  Esta acción siempre se realiza aunque el identificador
  establezca el directorio actual en una posicion no válida

  @param idDirNode Identificador del nodo
  */
  void setIdDirNode( qlonglong idDirNode ) const;

  /**
  Establece el identificador del nodo de directorio actual
  indicando el nombre de uno de sus directorios hijo

  Si no existe un hijo con ese nombre no hace nada

  @param dirName Nombre del directorio hijo
  @return FALSE si no se pudo establecer el identificador mediante ese nombre
  */
  bool setIdDirNodeByName( const QString & dirName ) const;

  /**
  Establece el identificador del nodo de fichero actual

  Esta acción siempre se realiza aunque el identificador
  establezca el directorio actual en una posicion no válida

  @param idFileNode Identificador del nodo
  */
  void setIdFileNode( qlonglong idFileNode ) const;

  /**
  Establece el identificador del nodo de fichero actual
  indicando el nombre de uno de los ficheros hijo del
  directorio actual

  Si no existe un hijo con ese nombre no hace nada

  @param fileName Nombre del fichero hijo del actual directorio
  @return FALSE si no se pudo establecer el identificador mediante ese nombre
  */
  bool setIdFileNodeByName( const QString & fileName ) const;

  /**
  @return El identificador del nodo raiz
  */
  qlonglong idRootNode() const;

  /**
  @return El identificador del nodo de directorio actual
  */
  qlonglong idDirNode() const;

  /**
  @return El identificador del nodo de fichero actual
  */
  qlonglong idFileNode() const;

  /**
  @return TRUE si el nodo de directorio actual es válido
  */
  bool isDirNodeValid() const;

  /**
  @return TRUE si el nodo de fichero actual es válido
  */
  bool isFileNodeValid() const;

  /**
  Añade un nuevo nodo de directorio dentro del nodo de directorio actual

  Si no se produce un error el nodo creado pasa a ser el actual nodo
  de directorio

  @return El identicador del nodo creado o -1 si hubo error
  */
  qlonglong addDirNode() const;

  /**
  Elimina un nodo de directorio dentro del nodo de directorio actual

  Si no se produce un error el padre del nodo borrado pasa a ser el actual
  nodo de directorio

  @return El identicador del nodo padre o -1 si hubo error
  */
  qlonglong delDirNode() const;

  /**
  Añade un nodo de fichero dentro del nodo de directorio actual

  Si no se produce un error el nodo creado pasa a ser el actual nodo
  de fichero

  @return El identicador del nodo creado o -1 si hubo error
  */
  qlonglong addFileNode() const;

  /**
  Elimina un nodo de fichero
     
  Si no se produce un error uno de sus hermanos pasa a ser el actual
  nodo de fichero, si no tiene hermanos el nodo actual de fichero pasa
  a ser invalido con valor -2.
  En cualquier caso el padre del nodo borrado pasa a ser el actual nodo
  de directorio.

  @return El identicador del nodo padre o -1 si hubo error
  */
  qlonglong delFileNode() const;

  /**
  Establece la información de un nodo

  Si encuentra el nodo con el identificador indicado en la
  información de nodo pasada como paŕametro lo actualiza con
  dicha información.
  No cambia el nodo actual.

  @param nodeInfo  AQSqlNodeInfo con la información de nodo
  @return FALSE si hay error
  */
  bool setNodeInfo( const AQSqlNodeInfo & nodeInfo ) const;

  /**
  @return Un nodo vacio, es decir, el identificador de nodo y el de
      su padre a -1, y todos los valores inicializados
  */
  AQSqlNodeInfo nodeVoidInfo() const;

  /**
  @return Información del nodo de directorio actual
  */
  AQSqlNodeInfo nodeDirInfo() const;

  /**
  @return Información del nodo de fichero actual
  */
  AQSqlNodeInfo nodeFileInfo() const;

  /**
  Lista de informaciones de los nodos hijos del nodo de directorio
  actual

  Se puede establecer un filtro en el formato de una claúsula Where utilizado
  en el lenguaje SQL. Ver estrucutura de las tablas 'aqsqldirs' y 'aqsqlfiles'
  para conocer los campos que se pueden utilizar en el filtrado.

  Incluye "." y ".."

  @param filterSqlWhere Filtro en formato SQL de la claúsula Where
  @return Lista de informaciones de los nodos hijos directos (no es recursivo)
  */
  AQSqlNodeInfoList nodeInfoList( const QString & filterSqlWhere = QString() ) const;

  /**
  Lista de nombres de los nodos hijos del nodo de directorio
  actual

  Se puede establecer un filtro en el formato de una claúsula Where utilizado
  en el lenguaje SQL. Ver estrucutura de las tablas 'aqsqldirs' y 'aqsqlfiles'
  para conocer los campos que se pueden utilizar en el filtrado.

  Incluye "." y ".."

  @param filterSqlWhere Filtro en formato SQL de la claúsula Where
  @return Lista de nombres de los nodos hijos directos (no es recursivo)
  */
  QStringList nameList( const QString & filterSqlWhere = QString() ) const;

  /**
  Establece el contenido de datos del nodo de fichero actual

  Para ficheros binarios.

  @param data ByteArray con el contenido
  @return TRUE si se estableció correctamente el contenido
  */
  bool setData( const QByteArray & content ) const;

  /**
  Establece el contenido de datos del nodo de fichero actual

  Para ficheros de texto.

  @param data Texto con el contenido
  @return TRUE si se estableció correctamente el contenido
  */
  bool setData( const QString & content ) const;

  /**
  Para obtener el contenido del nodo de fichero actual

  Si el fichero es de caracteres utiliza la codificación UTF-8

  @return El contenido de datos del nodo de fichero actual
  */
  QByteArray data() const;

  /**
  Copia el contenido de un de un nodo de fichero a otro

  @param srcNode Nodo origen, el fichero que representa debe existir
  @param dstNode Nodo destino, el fichero que representa debe existir
  @return TRUE si la copia tuvo éxito
  */
  bool cpData( const AQSqlNodeInfo & srcNode, const AQSqlNodeInfo & dstNode ) const;

  /**
  Para operaciones transaccionales
  */
  bool transaction();
  bool commit();
  bool rollback();

  /**
  Para obtener la lista de registros de datos huérfanos, es decir, no están actualmente
  asignados a un nodo.

  @return Lista de claves SHA de los registros de datos huérfanos
  */
  QStringList orphanData() const;

  /**
  Para obtener la lista de nodos vacíos, es decir, sin registro de datos asignado

  @return Lista de los identificadores de los nodo vacíos
  */
  QList<qlonglong> voidNodes() const;

  /**
  Borra un registro de datos. Usar con cuidado.

  @param sha Clave SHA que identifica al registro de datos
  @return TRUE si la operación se realizó con éxito
  */
  bool freeData( const QString & sha ) const;

  /**
  Hace una búsqueda global por contenido en ficheros de texto. La búsqueda se hace en todo
  el sistema de ficheros, para limitarla se deben indicar los filtros oportunos mediante los
  parámetros.

  @param filterContent Filtro de contenido se puede utilizar '%' y '_'como comodines. Para
   marcar caracteres de escape se debe utilizar '|'.
  @param filterName Filtro para los nombres de ficheros. Se pueden utilizar el mismo formato que
   en el parámetro fiterContent
  @return Lista de informaciones de los nodos de ficheros que pasan los filtros
  */
  AQSqlNodeInfoList
  globalSearchByContent( const QString & filterContent, const QString & filterName,
                         const QString & filterSqlWhere = QString() ) const;

private:

  /**
  Privado
  */
  AQSqlFileSystemPrivate * d;
};

class AQSqlFileSystemPrivate
{
public:

  AQSqlFileSystemPrivate( AQSqlDatabase db, AQSqlFileSystem * fs );
  ~AQSqlFileSystemPrivate();

  /**
  Auxiliares
  */
  void updateDirSha( qlonglong idDir );

  /**
  Permisos por defecto
  */
  QString defaultPerms;

  /**
  Identificador del nodo raiz
  */
  qlonglong idRootNode;

  /**
  Identificador del nodo de directorio actual
  */
  qlonglong idDirNode;

  /**
  Identificador del nodo de fichero actual
  */
  qlonglong idFileNode;

  /**
  Base de datos a utilizar por el sistema de ficheros
  */
  AQSqlDatabase db_;

  /**
  Propietario
  */
  AQSqlFileSystem * f;
};

inline AQSqlNodeInfo AQSqlFileSystem::nodeVoidInfo() const
{
  return AQSqlNodeInfo();
}

inline bool AQSqlFileSystem::isReady() const
{
  return ( d && d->db_.dbAux().isOpen() );
}

inline void AQSqlFileSystem::setIdDirNode( qlonglong idDirNode ) const
{
  if ( d )
    d->idDirNode = idDirNode;
}

inline void AQSqlFileSystem::setIdFileNode( qlonglong idFileNode ) const
{
  if ( d )
    d->idFileNode = idFileNode;
}

inline qlonglong AQSqlFileSystem::idRootNode() const
{
  return ( d ? d->idRootNode : -1 );
}

inline qlonglong AQSqlFileSystem::idDirNode() const
{
  return ( d ? d->idDirNode : -1 );
}

inline qlonglong AQSqlFileSystem::idFileNode() const
{
  return ( d ? d->idFileNode : -1 );
}

inline bool AQSqlFileSystem::transaction()
{
  return d->db_.dbAux().transaction();
}

inline bool AQSqlFileSystem::commit()
{
  return d->db_.dbAux().commit();
}

inline bool AQSqlFileSystem::rollback()
{
  return d->db_.dbAux().rollback();
}

#endif /*AQSQLFILESYSTEM_H_*/
