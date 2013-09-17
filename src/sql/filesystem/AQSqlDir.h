/***************************************************************************
AQSqlDir.h
-------------------
begin                : 02/09/2007
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

#ifndef AQSQLDIR_H_
#define AQSQLDIR_H_

#include <AQSql/AQSql.h>

#include <QString>

class AQSqlDirPrivate;
class AQSqlFileSystem;
class AQSqlDatabase;

/**
Manejo a alto nivel de directorios y ficheros 
del sistema de ficheros SQL
 
@author InfoSiAL S.L.
*/
class AQSqlDir
{
public:

  /**
  constructor

  @param  db    Base de datos sobre la que montar el sistema de ficheros
  @param  path  Ruta del directorio inicial. Si es vacia se inicia en el raiz.
  */
  AQSqlDir( AQSqlDatabase db, const QString & path = QString() );

  /**
  destructor
  */
  ~AQSqlDir();

  /**
  @return TRUE si el objeto se ha inicializado correctamente y está listo para trabajar
  */
  bool isReady() const;

  /**
  Cambia al directorio indicado

  @param  dirName Nombre del directorio, si se omite se considera el directorio actual
  @return TRUE si el directorio indicado es existe y es válido
  */
  bool cd( const QString & dirName = QString() ) const;

  /**
  Cambia al directorio padre

  @return TRUE si el directorio padre existe y es válido
  */
  bool cdUp() const;

  /**
  Indica si existe y es válido el directorio o fichero

  @param  name  Nombre del directorio o fichero, si se omite se considera el directorio actual
  @return TRUE  si el directorio o fichero indicado existe y es válido
  */
  bool exists( const QString & name = QString() ) const;

  /**
  Crea un directorio

  Si el directorio existe no hace nada

  @param  dirName Nombre del directorio
  @return TRUE si se pudo crear
  */
  bool mkdir( const QString & dirName ) const;

  /**
  Crea un fichero y establece su contenido

  Para ficheros binarios. Si el fichero
  existe no hace nada.

  @param  fileName  Nombre del fichero
  @return TRUE si se pudo crear
  */
  bool mkfile( const QString & fileName, const QByteArray & content ) const;

  /**
  Crea un fichero y establece su contenido

  Para ficheros de caracteres. Si el fichero
  existe no hace nada.

  @param  fileName  Nombre del fichero
  @return TRUE si se pudo crear
  */
  bool mkfile( const QString & fileName, const QString & content ) const;

  /**
  Borra un directorio o fichero

  Si es un directorio no lo borra al menos que se
  indique que el borrado es recursivo.

  Los directorios "." y ".." no se pueden borrar

  @param  name      Nombre del fichero o directorio
  @param  recursive Si es TRUE y es un directorio borra ese directorio y
                    todo su contenido
  @return TRUE si el borrado se realizó con éxito
  */
  bool rm( const QString & name, bool recursive = false );

  /**
  Mueve y/o renombra un directorio o fichero

  Permite renombrar si se cambia el nombre
  en el destino. Si el destino ya existe no hace nada,
  es decir, no sobreescribe

  @param  srcName Nombre del fichero o directorio origen
  @param  dstName Nombre del fichero o directorio destino
  @return TRUE si el cambio se realizó con éxito
  */
  bool mv( const QString & srcName, const QString & dstName ) const;

  /**
  Copia un directorio o fichero

  Permite renombrar si se cambia el nombre
  en el destino. Si el destino ya existe no hace nada,
  es decir, no sobreescribe

  @param  srcName Nombre del fichero o directorio origen
  @param  dstName Nombre del fichero o directorio destino
  @return TRUE si el cambio se realizó con éxito
  */
  bool cp( const QString & srcName, const QString & dstName ) const;

  /**
  Para obtener el contenido de un fichero
     
  Si el fichero es de texto utiliza la codificación UTF-8
     
  @param  fileName  Nombre del fichero
  @param  shaKey    Por referencia devuelve la suma SHA del contenido del fichero
  @param  cacheKey  Por referencia devuelve la clave de caché asignada al fichero
  @return El contenido de datos del fichero
  */
  QByteArray fileData( const QString & fileName, QString & shaKey, QString & cacheKey ) const;

  /**
  Nombre del directorio actual

  @return El nombre simple ( no la ruta completa ) del directorio actual
  */
  QString dirName() const;

  /**
  @return Suma SHA de la suma de todos los ficheros del directorio actual
  */
  QString dirSha() const;

  /**
  Nombre del último fichero accedido

  @return El nombre simple ( no la ruta completa ) del último fichero accedido
  */
  QString fileName() const;

  /**
  @return Suma SHA del contenido del último fichero accedido
  */
  QString fileSha() const;

  /**
  @param  name      Nombre del fichero
  @param  cacheKey  Por referencia devuelve la clave de caché asignada al fichero
  @return Suma SHA del contenido del fichero indicado
  */
  QString fileSha( const QString & name, QString & cacheKey ) const;

  /**
  @return El valor del flag especial del último fichero accedido
  */
  QString fileSpecialFlag() const;

  /**
  @param  name  Nombre del fichero
  @return El valor del flag especial del fichero indicado
  */
  QString fileSpecialFlag( const QString & name ) const;

  /**
  Establece el valor del flag especial del último fichero accedido

  @param  name  Nombre del fichero
  @param  sf    Valor del flag. El valor anterior es sobreescrito.
  */
  void fileSetSpecialFlag( const QString & sf ) const;

  /**
  Establece el valor del flag especial del fichero indicado

  @param  name  Nombre del fichero
  @param  sf    Valor del flag. El valor anterior es sobreescrito.
  */
  void fileSetSpecialFlag( const QString &name, const QString & sf ) const;

  /**
  Obtiene la ruta absoluta del directorio o fichero indicado

  Si el directorio o el fichero no existen de vuelve una cadena vacia
  Si no se indica directorio o fichero devuelve la ruta absoluta del directorio
  actual. En el caso de ser un fichero, en la ruta absoluta no se incluye el
  nombre del fichero

  @param  name  Nombre del directorio o fichero, si se omite se considera el directorio actual
  @return La ruta absoluta de directorios ( no incluye el nombre de fichero ),
          si existe el directorio o fichero indicados, o una cadena vacia si no existe.
  */
  QString absolutePath( const QString & name = QString() ) const;

  /**
  Lista el contenido de un directorio, con toda la información

  Se puede establecer un filtro en el formato de una claúsula Where utilizado
  en el lenguaje SQL. Ver estrucutura de las tablas 'aqsqldirs' y 'aqsqlfiles'
  para conocer los campos que se pueden utilizar en el filtrado

  @param  dirName         Directorio del que se desea listar su contenido. Si es vacío
                          se considera el actual. Si es igual al nombre de un fichero del directorio
                          actual devuelve la información de ese fichero
  @param  filterSqlWhere  Filtro en formato SQL de la claúsula Where
  @return Lista de informaciones de los ficheros hijos directos (no es recursivo)
  */
  AQSqlNodeInfoList ls( const QString & dirName = QString(), const QString & filterSqlWhere = QString() ) const;

  /**
  Lista el contenido de un directorio, sólo los nombres

  Se puede establecer un filtro en el formato de una claúsula Where utilizado
  en el lenguaje SQL. Ver estrucutura de las tablas 'aqsqldirs' y 'aqsqlfiles'
  para conocer los campos que se pueden utilizar en el filtrado

  @param  dirName         Directorio del que se desea listar su contenido. Si es vacío
                          se considera el actual. Si es igual al nombre de un fichero del directorio
                          actual devuelve el nombre de ese fichero
  @param  filterSqlWhere  Filtro en formato SQL de la claúsula Where
  @return Lista de nombres de los ficheros hijos directos (no es recursivo)
  */
  QStringList lsNames( const QString & dirName = QString(), const QString & filterSqlWhere = QString() ) const;

  /**
  Copia un fichero a un directorio del sistema de ficheros del sistema operativo

  Si el destino ya existe no hace nada,
  es decir, no sobreescribe

  @param  srcName Nombre del fichero origen
  @param  dstName Nombre del directorio destino, si no existe intenta crearlo
  @return TRUE si el cambio se realizó con éxito
  */
  bool cpFileToFs( const QString & srcName, const QString & dstName ) const;

  /**
  Elimina todos los registros de datos no asignados a ficheros existentes. Usar con cuidado.

  @param  deleted Por referencia devuelve la lista de claves SHA de datos eliminados
  @return TRUE si no hubo errores
  */
  bool cleanOrphanData( QStringList & deleted ) const;

  /**
  Elimina todos los ficheros que no tienen asignado un registro de datos. Usar con cuidado.

  @param  deleted Por referencia devuelve la lista de ficheros eliminados
  @return TRUE si no hubo errores
  */
  bool cleanVoidFiles( QStringList & deleted ) const;

  /**
  Lista los ficheros de texto que contengan en su interior el patrón de texto indicado

  Se puede establecer un filtro en el formato de una claúsula Where utilizado
  en el lenguaje SQL. Ver estrucutura de las tablas 'aqsqldirs' y 'aqsqlfiles'
  para conocer los campos que se pueden utilizar en el filtrado

  @param  filterContent   Filtro de contenido se puede utilizar '%' y '_'como comodines. Para
                          marcar caracteres de escape se debe utilizar '|'.
  @param  filterName      Filtro para los nombres de ficheros. Se pueden utilizar el mismo formato que
                          en el parámetro fiterContent
  @param  dirName         Directorio del que se desea listar su contenido. Si es vacío
                          se considera el actual. Si es igual al nombre de un fichero del directorio
                          actual devuelve el nombre de ese fichero
  @param  filterSqlWhere  Filtro en formato SQL de la claúsula Where
  @return Lista de nombres de los ficheros hijos directos (no es recursivo) que cumplen con
          el patrón de contenido
  */
  QStringList lsNamesByContent( const QString & filterContent, const QString & filterName,
                                const QString & dirName = QString(),
                                const QString & filterSqlWhere = QString() ) const;

private:

  /**
  Privado
  */
  AQSqlDirPrivate * d;
};

#endif /*AQSQLDIR_H_*/
