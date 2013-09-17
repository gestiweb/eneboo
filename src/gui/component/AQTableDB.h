/***************************************************************************
AQTableDB.h
-------------------
begin                : 30/08/2007
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

#ifndef AQTABLEDB_H
#define AQTABLEDB_H

#include <uic/ui_AQTableDB.h>

#include <AQUi/AQComponentDB.h>

class QSqlRecord;
class AQDataTable;
class AQFieldMD;

/**
Este componente contiene todo lo necesario para manejar
los datos de una tabla. Además de la funcionalidad de
busqueda en la tabla por un campo, mediante filtros.


@author InfoSiAL S.L.
*/
class AQTableDB: public AQComponentDB
{
  Q_OBJECT

  Q_PROPERTY( QString tableName READ tableName WRITE setTableName )
  Q_PROPERTY( QString foreignField READ foreignField WRITE setForeignField )
  Q_PROPERTY( QString fieldRelation READ fieldRelation WRITE setFieldRelation )

public:

  /**
  constructor
  */
  AQTableDB( QWidget * parent = 0 );

  /**
  destructor
  */
  ~AQTableDB();

  /**
  Establece la accion que debe utilizar el componente.

  Este método se debe llamar siempre antes que setupUi y linkUi, ya que a
  través de la accion los componente pueden conocer los metadatos que necesitan
  para autoconfigurarse.

  @param act Accion del tipo AQAction a utilizar. Si es vacia desvincula.
  */
  void setAQAction( AQSharedAction act );

  /**
  @return Objeto AQAction usada por el componente.
  */
  AQSharedAction aqAction() const;

  /**
  Levanta la interfaz de usuario

  @param form El formulario al que pertenece el componente
  */
  void setupUi( AQFormDB * form );

  /**
  Vincular Ui al cursor de la accion. Para desvincular setAQAction( AQSharedAction() )
  */
  void linkUi();

  /**
  Refresco inicial
  */
  void initRefreshUi();

  /**
  Componente para visualizar los registros
  */
  AQDataTable * tableRecords();

  /**
  Para obtener el nombre de la tabla asociada.

  @return Nombre de la tabla asociada
  */
  QString tableName() const;

  /**
  Para establecer el nombre de la tabla asociada.

  @param fT Nombre de la tabla asociada
  */
  void setTableName( const QString & fT );

  /**
  Para obtener el nombre del campo foráneo.

  @return Nombre del campo
  */
  QString foreignField() const;

  /**
  Para establecer el nombre del campo foráneo.

  @param fN Nombre del campo
  */
  void setForeignField( const QString & fN );

  /**
  Para obtener el nombre del campo relacionado.

  @return Nombre del campo
  */
  QString fieldRelation() const;

  /**
  Para establecer el nombre del campo relacionado.

  @param fN Nombre del campo
  */
  void setFieldRelation( const QString & fN );

  /**
  Establece si el componente esta en modo solo lectura o no.
  */
  void setReadOnly( bool mode );

  /**
  Establece si el componente esta en modo solo edición o no.
  */
  void setEditOnly( bool mode );

  /**
  Establece el componente a sólo inserción o no.
  */
  void setInsertOnly( bool mode );

  /**
  Establece el filtro inicial de búsqueda
  */
  void setInitSearch( const QString & iS );

  /**
  Establece el orden de las columnas de la tabla.

  @param fields Lista de los nombres de los campos ordenada según se desea que aparezcan en la tabla
  */
  void setOrderCols( const QStringList & fields );

  /**
  Establece el filtro de la tabla

  @param f Sentencia Where que establece el filtro
  */
  void setFilter( const QString & f );

  /**
  Devuelve el filtro de la tabla

  @return Filtro
  */
  QString filter() const;

  /**
  Captura de eventos
  */
  bool eventFilter( QObject * obj, QEvent * ev );

protected:

  /**
  Captura de eventos
  */
  void showEvent ( QShowEvent * ev );

private:

  /**
  Nombre de la tabla a la que esta asociado este componente.
  */
  QString tableName_;

  /**
  Nombre del campo foráneo
  */
  QString foreignField_;

  /**
  Nombre del campo de la relación
  */
  QString fieldRelation_;

  /**
  Accion usada por el componente
  */
  AQSharedAction act_;

  /**
  Mantiene el contenido del campo de búsquedad utilizado para filtrar registros
  */
  QString filter_;

  /**
  Almacena los metadatos del campo por el que está actualmente ordenada la tabla
  */
  AQFieldMD * sortField_;

  /**
  Crónometro interno
  */
  QTimer * timer;

  /**
  Filtro inicial de búsqueda
  */
  QString initSearch_;

  /**
  Componente para visualizar los registros
  */
  AQDataTable * tableRecords_;

  /**
  Interfaz de Usuario
  */
  Ui::AQTableDB ui;

  /**
  Inicia la accion teniendo en cuenta si el componente actua como maestro o detalle
  */
  void initAQAction();

  void setupMaster();
  void setupRecord();
  void setupTableRecords();
  void addColumns();
  void firstRefresh();
  bool firstRefresh_;

public Q_SLOTS:

  /**
  Actualiza el conjunto de registros.
  */
  void refresh( bool refreshHead = false, bool refreshData = false );

  /**
  Actualiza el conjunto de registros con un retraso.

  Acepta un lapsus de tiempo en milisegundos, activando el cronómetro interno para
  que realize el refresh definitivo al cumplirse dicho lapsus.

  @param msec Cantidad de tiempo del lapsus, en milisegundos.
  */
  void refreshDelayed( int msec = 0 );

  /**
  Operacion con registros. Comprueban el modo en el que se encuentra
  el componente ( solo edicion, solo lectura o solo inserccion ) y
  si es posible se realiza la operacion solicitada
  */
  void insertRecord();
  void editRecord();
  void browseRecord();
  void deleteRecord();
  void copyRecord();

  /**
  Coloca la columna como primera pasando el nombre del campo.

  Este slot está conectado al cuadro combinado de busqueda
  del componente. Cuando seleccionamos un campo este se coloca
  como primera columna y se reordena la tabla con esta columna.
  De esta manera siempre tendremos la tabla ordenada mediante
  el campo en el que queremos buscar.

  @param  c Nombre del campo, esta columna intercambia su posion con la primera columna
  @return Falso si no existe el campo
  */
  void putFirstCol( const QString & c );

protected Q_SLOTS:

  /**
  Coloca la columna indicada como primera.

  Este slot está conectado al cuadro combinado de busqueda
  del componente. Cuando seleccionamos un campo este se coloca
  como primera columna y se reordena la tabla con esta columna.
  De esta manera siempre tendremos la tabla ordenada mediante
  el campo en el que queremos buscar.

  @param  c Numero de la columna en la tabla, esta columna intercambia
            su posion con la primera columna
  */
  void putFirstCol( int c );

  /**
  Filtra los registros de la tabla utilizando el primer campo, según el patrón dado.

  Este slot está conectado al cuadro de texto de busqueda del componente,
  tomando el contenido de este como patrón para el filtrado.

  @param p Cadena de caracteres con el patrón de filtrado
  */
  void filterRecords( const QString & p );

  /**
  Emite la señal currentChanged, cuando el objeto tableRecords envía dicha señal
  */
  void emitCurrentChanged( QSqlRecord * );

Q_SIGNALS:

  /**
  Señal emitida cuando se establece si el componente es o no de solo lectura.
  */
  void readOnlyChanged( bool );

  /**
  Señal emitida cuando se establece si el componente es o no de solo edición.
  */
  void editOnlyChanged( bool );

  /**
  Señal emitida cuando se establece si el componente es o no de solo inserción.
  */
  void insertOnlyChanged( bool );

  /**
  Señal emitida cuando se cambia el registro seleccionado.
  */
  void currentChanged();
};

/**
Soporte para el antiguo FLTableDB

Para soporte y migración de los antiguos formularios
de Qt3 a Qt4
*/
class FLTableDB : public AQTableDB
{
  Q_OBJECT

public:

  FLTableDB( QWidget * parent = 0 );
  ~FLTableDB();
};

#endif
