/***************************************************************************
AQUiManager.h
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

#ifndef AQUIMANAGER_H_
#define AQUIMANAGER_H_

#include <AQSql/AQSqlDatabase.h>

#include <QString>

class Pixmap;
class QWidget;

class AQUiManager
{
public:

  /**
  Crea un formulario a partir de su fichero de descripción.

  Utiliza el método contentCached() del manejador de módulos de la
  base de datos indicada para obtener el texto XML que describe el
  formulario.

  @param  filename              Nombre del fichero que contiene la descripción del formulario.
  @param  parent                QWidget padre para el formulario a crear
  @param  db                    Base de datos a utilizar. Si no se especifica se utiliza la base de datos
                                por defecto de la aplicación.
  @param  forceExecInGuiThread  Para forzar que la construcción del formulario se ejecute en el contexto
                                del hilo principal GUI
  @return QWidget correspondiente al formulario construido.
  */
  static QWidget * createUI( const QString & filename, QWidget * parent,
                             AQSqlDatabase db, bool forceExecInGuiThread = false );

  /**
  Para obtener el icono asociado a un módulo.

  Utiliza el método contentCached() del manejador de módulos de la
  base de datos indicada para obtener el contenido del icono.

  @param  idM Identificador del módulo del que obtener el icono
  @param  db  Base de datos a utilizar. Si no se especifica se utiliza la base de datos
              por defecto de la aplicación.

  @return QPixmap con el icono
  */
  static QPixmap iconModule( const QString & idM, AQSqlDatabase db );

  /**
  Convierte el contenido pasado de un fichero de definición de formulario de la
  versión Qt3 a la versión Qt4

  Para que funcione correctamente se debe tener instalada y accesible en la ruta
  de ejecutables la utilidad "uic3" incluida en el toolkit Qt.

  El nuevo formato de Qt4 no admite imágenes empotradas por lo que estas se extraen
  y se guardan en la base de datos como binarios

  @param  fileName        Nombre del fichero
  @param  contentDefFile  Contenido del fichero de formulario a convertir
  @param  db              Base de datos donde copiar imágenes del formulario

  @return Devuelve la definición de fichero convertida a Qt4
  */
  static QString convertUiToQt4( const QString & fileName, const QString & contentDefFile,
                                 AQSqlDatabase db );

  /**
  Realiza un preproceso de adaptación de un fichero de definición de formulario de la
  versión Qt3.

  Es utilizado por convertUiToQt4() para realizar algunos ajustes previos en los formularios,
  como cambiar algunos nombres de objetos o redimensionar componentes.

  @param  fileName        Nombre del fichero
  @param  contentDefFile  Contenido del fichero de formulario a convertir

  @return Devuelve la definición de fichero modificada
  */
  static QString uiPreprocessor( const QString & fileName, const QString & contentDefFile );

  /**
  Crea el fichero recursos compilado, referente a todas las imágenes de los formularios
  de los módulos

  Trabaja sobre el sistema de ficheros SQL y espera encontrar las imágenes en AQ_DIR_MOD/imagenes

  El fichero creado lo guarda en el el directorio local de la caché de disco con el nombre
  modulos.rcc.

  Para que funcione correctamente se debe tener instalada y accesible en la ruta
  de ejecutables la utilidad "rcc" incluida en el toolkit Qt.

  @param  db  Base de datos sobre la que trabajar

  @return La ruta absoluta en el sistema de ficheros local del fichero de recursos compilado,
    si la operación no pudo realizar devuelve una cadena vacía
  */
  static QString compileResourceImages( AQSqlDatabase db );

  /**
  Convierte el contenido pasado de un fichero de definición de versiones anteriores
  al formato AbanQ 3.

  Automáticamente detecta el tipo de fichero y aplica las procesos de conversión.
  El nuevo fichero también se convierte desde la antigua codificación local ISO8859-15
  a la nueva UTF-8 utilizada por AbanQ3.

  @param  fileName        Nombre del fichero
  @param  contentDefFile  Contenido del fichero de definición a convertir
  @param  type            Tipo de fichero
  @param  db              Base de datos sobre la que trabajar

  @return Devuelve el contenido convertido a AbanQ 3
  */
  static QString convertToAbanQ3( const QString & fileName, const QString & contentDefFile,
                                  AQCore::TypeDefFile type, AQSqlDatabase db );

  /**
  Traspasa la metainformación de los módulos cargados en la antigua estructura de tablas
  de sistema de AbanQ v2 a la nueva estructura de tablas. No hace nada si no existe la
  antigua estructura o si se determina que la metainformación de la nueva estructura ya
  está actualizada comparando shaGlobalFL con shaLastUpdateFL. Si el traspaso se realiza
  con éxito actualiza shaLastUpdateFL.

  AVISO : Esta acción es destructiva, toda la metainformación existente en la nueva
  estructura de tablas de AbanQ v3 es borrada y sobreescrita con la metainformación
  proveniente de la antigua estructura de AbanQ v2.

  En el proceso de traspaso también se harán conversiones de la metainformación para adaptarla
  a nuevos formatos, por ejemplo, los formularios en formato de Qt3 se deben convertir al formato de Qt4.

  @param  db  Base de datos sobre la que trabajar
  */
  static bool updateFromAbanQ2( AQSqlDatabase db );
};

#endif /*AQUIMANAGER_H_*/
