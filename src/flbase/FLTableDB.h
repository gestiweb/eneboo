/***************************************************************************
                              FLTableDB.h
                          -------------------
 begin                : Sun Jul 1 2001
 copyright            : (C) 2001-2005 by InfoSiAL S.L.
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

#ifndef FLTABLEDB_H
#define FLTABLEDB_H

#include <qlayout.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qtimer.h>
#include <qvalidator.h>
#include <qstringlist.h>
#include <qwidgetstack.h>
#include <qbuttongroup.h>
#include <qprogressdialog.h>
#include "FLWidgetTableDB.h"

#include "AQGlobal.h"

class FLSqlCursor;
class FLDataTable;
class FLFieldMetaData;
class FLTableDBInterface;

/**
PLUGIN que contiene una tabla de la base de datos.

Este objeto contiene todo lo necesario para manejar
los datos de una tabla. Además de la funcionalidad de
busqueda en la tabla por un campo, mediante filtros.

Este plugin para que sea funcional debe tener como uno
de sus padres o antecesor a un objeto FLFormDB.

@author InfoSiAL S.L.
*/
class AQ_EXPORT FLTableDB: public FLWidgetTableDB
{

  Q_OBJECT

  Q_PROPERTY(QString tableName READ tableName WRITE setTableName)
  Q_PROPERTY(QString foreignField READ foreignField WRITE setForeignField)
  Q_PROPERTY(QString fieldRelation READ fieldRelation WRITE setFieldRelation)
  Q_PROPERTY(bool checkColumnEnabled READ checkColumnEnabled WRITE setCheckColumnEnabled)
  Q_PROPERTY(QString aliasCheckColumn READ aliasCheckColumn WRITE setAliasCheckColumn)
  Q_PROPERTY(bool findHidden READ findHidden WRITE setFindHidden)
  Q_PROPERTY(bool filterHidden READ filterHidden WRITE setFilterHidden)
  Q_PROPERTY(bool showAllPixmaps READ showAllPixmaps WRITE setShowAllPixmaps)
  Q_PROPERTY(QString functionGetColor READ functionGetColor WRITE setFunctionGetColor)
  Q_PROPERTY(bool onlyTable READ onlyTable WRITE setOnlyTable)
  Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)
  Q_PROPERTY(bool editOnly READ editOnly WRITE setEditOnly)
  Q_PROPERTY(bool insertOnly READ insertOnly WRITE setInsertOnly)
  Q_PROPERTY(bool autoSortColumn READ autoSortColumn WRITE setAutoSortColumn)

  friend class FLTableDBInterface;
  friend class FLFormDB;

public:

  /**
  constructor
  */
  FLTableDB(QWidget *parent = 0, const char *name = 0);

  /**
  Obtiene el componente tabla de registros
  */
  FLDataTable *tableRecords();

  /**
  Para obtener el cursor utilizado por el componente.

  return Objeto FLSqlCursor con el cursor que contiene los registros para ser utilizados en el formulario
  */
  FLSqlCursor *cursor();

  /**
  Para obtener el nombre de la tabla asociada.

  @return Nombre de la tabla asociado
  */
  QString tableName() const;

  /**
  Para establecer el nombre de la tabla asociada.

  @param fT Nombre de la tabla asociada
  */
  void setTableName(const QString &fT);

  /**
  Para obtener el nombre del campo foráneo.

  @return Nombre del campo
  */
  QString foreignField() const;

  /**
  Para establecer el nombre del campo foráneo.

  @param fN Nombre del campo
  */
  void setForeignField(const QString &fN);

  /**
  Para obtener el nombre del campo relacionado.

  @return Nombre del campo
  */
  QString fieldRelation() const;

  /**
  Para establecer el nombre del campo relacionado.

  @param fN Nombre del campo
  */
  void setFieldRelation(const QString &fN);

  /**
  Establece si el componente esta en modo solo lectura o no.
  */
  void setReadOnly(const bool mode);
  bool readOnly() const {
    return reqReadOnly_;
  }

  /**
  Establece si el componente esta en modo solo edición o no.
  */
  void setEditOnly(const bool mode);
  bool editOnly() const {
    return reqEditOnly_;
  }

  /**
  Establece el componente a sólo inserción o no.
  */
  void setInsertOnly(const bool mode);
  bool insertOnly() const {
    return reqInsertOnly_;
  }

  /**
  Establece el filtro inicial de búsqueda
  */
  void setInitSearch(const QString &iS) {
    initSearch_ = iS;
  }

  /**
  Establece el orden de las columnas de la tabla.

  @param fields Lista de los nombres de los campos ordenada según se desea que aparezcan en la tabla de izquierda a derecha
  */
  void setOrderCols(QStringList &fields);

  /**
  Devuelve la lista de los campos ordenada por sus columnas en la tabla de izquierda a derecha
  */
  QStringList orderCols();

  /**
  Establece el filtro de la tabla

  @param f Sentencia Where que establece el filtro
  */
  void setFilter(const QString &f);

  /**
  Devuelve el filtro de la tabla

  @return Filtro
  */
  QString filter();

  /**
  Devuelve el filtro de la tabla impuesto en el Find

  @return Filtro
  */
  QString findFilter();

  /**
  Obtiene si la columna de selección está activada
  */
  bool checkColumnEnabled() const;

  /**
  Establece el estado de activación de la columna de selección

  El cambio de estado no será efectivo hasta el siguiente refresh.
  */
  void setCheckColumnEnabled(bool b);

  /**
  Obiente el texto de la etiqueta de encabezado para la columna de selección
  */
  QString aliasCheckColumn() const;

  /**
  Establece el texto de la etiqueta de encabezado para la columna de selección

  El cambio del texto de la etiqueta no será efectivo hasta el próximo refresh
  */
  void setAliasCheckColumn(const QString &t);

  /**
  Obtiene si el marco de búsqueda está oculto
  */
  bool findHidden() const;

  /**
  Oculta o muestra el marco de búsqueda

  @param  h TRUE lo oculta, FALSE lo muestra
  */
  void setFindHidden(bool h);

  /**
  Obtiene si el marco para conmutar entre datos y filtro está oculto
  */
  bool filterHidden() const;

  /**
  Oculta o muestra el marco para conmutar entre datos y filtro

  @param  h TRUE lo oculta, FALSE lo muestra
  */
  void setFilterHidden(bool h);

  /**
  Ver FLTableDB::showAllPixmaps_
  */
  bool showAllPixmaps() const;

  /**
  Ver FLTableDB::showAllPixmaps_
  */
  void setShowAllPixmaps(bool s);

  /**
  Ver FLTableDB::functionGetColor_
  */
  QString functionGetColor() const;

  /**
  Ver FLTableDB::functionGetColor_
  */
  void setFunctionGetColor(const QString &f);

  /**
    Asigna el nombre de función a llamar cuando cambia el filtro.
  */  
  void setFilterRecordsFunction( QString fn) {
    tableDB_filterRecords_functionName_ = fn;
  }
  /**
  Ver FLTableDB::onlyTable_
  */
  void setOnlyTable(bool on = true);
  bool onlyTable() const {
    return reqOnlyTable_;
  }
  
  /**
  Ver FLTableDB::autoSortColumn_
  */
  void setAutoSortColumn(bool on = true) {
    autoSortColumn_ = on;
  }
  bool autoSortColumn() const {
    return autoSortColumn_;
  }
  
protected:

  /**
  Filtro de eventos
  */
  bool eventFilter(QObject *obj, QEvent *ev);

  /**
  Captura evento mostrar
  */
  void showEvent(QShowEvent *e);

private:

  /**
  Tipos de condiciones para el filtro
  */
  enum CondType {
    All,
    Contains,
    Starts,
    End,
    Equal,
    Dist,
    Greater,
    Less,
    FromTo,
    Null,
    NotNull
  };

  /**
  Redefinida por conveniencia
  */
  void showWidget();

  /**
  Asigna el cursor actual del componente a la tabla de registros
  */
  void setTableRecordsCursor();

  /**
  Refresca la pestaña datos aplicando el filtro
  */
  void refreshTabData();

  /**
  Refresca la pestaña del filtro
  */
  void refreshTabFilter();

  /**
  Para obtener la enumeración correspondiente a una condición para el filtro a partir de
  su literal
  */
  FLTableDB::CondType decodeCondType(const QString &strCondType) const;

  /**
  Construye la claúsula de filtro en SQL a partir del contenido de los valores
  definidos en la pestaña de filtro
  */
  QString tdbFilterBuildWhere();

  /**
  Inicializa un editor falso y no funcional.

  Esto se utiliza cuando se está editando el formulario con el diseñador y no
  se puede mostrar el editor real por no tener conexión a la base de datos.
  Crea una previsualización muy esquemática del editor, pero suficiente para
  ver la posisicón y el tamaño aproximado que tendrá el editor real.
  */
  void initFakeEditor();

  /**
  Componente para visualizar los registros
  */
  FLDataTable *tableRecords_;

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
  Cursor con los datos de origen para el componente
  */
  FLSqlCursor *cursor_;

  /**
  Cursor auxiliar de uso interno para almacenar los registros de la tabla
  relacionada con la de origen
  */
  FLSqlCursor *cursorAux;

  /**
  Matiene la ventana padre
  */
  QWidget *topWidget;

  /**
  Indica que la ventana ya ha sido mostrada una vez
  */
  bool showed;

  /**
  Mantiene el filtro de la tabla
  */
  QString filter_;

  /**
  Almacena si el componente está en modo sólo lectura
  */
  bool readonly_;
  bool reqReadOnly_;

  /**
  Almacena si el componente está en modo sólo edición
  */
  bool editonly_;
  bool reqEditOnly_;

  /**
  Indica si el componente está en modo sólo permitir añadir registros
  */
  bool insertonly_;
  bool reqInsertOnly_;

  /**
  Almacena los metadatos del campo por el que está actualmente ordenada la tabla
  */
  FLFieldMetaData *sortField_;

  /**
  Almacena los metadatos del campo por el que está actualmente ordenada la tabla en segunda instancia

  @author Silix - dpinelo
   */
  FLFieldMetaData *sortField2_;

  /**
  Crónometro interno
  */
  QTimer *timer;

  /**
  Filtro inicial de búsqueda
  */
  QString initSearch_;

  /**
  Indica que la columna de seleción está activada
  */
  bool checkColumnEnabled_;

  /**
  Indica el texto de la etiqueta de encabezado para la columna de selección
  */
  QString aliasCheckColumn_;

  /**
  Indica el nombre para crear un pseudocampo en el cursor para la columna de selección
  */
  QString fieldNameCheckColumn_;

  /**
  Indica que la columna de selección está visible
  */
  bool checkColumnVisible_;

  /**
  Indica el número de columna por la que ordenar los registros
  */
  int sortColumn_;

  /**
  Indica el número de columna por la que ordenar los registros

  @author Silix - dpinelo
  */
  int sortColumn2_;

  /**
  Indica el número de columna por la que ordenar los registros

  @author Silix
  */
  int sortColumn3_;

  /**
  Indica el sentido ascendente o descendente del la ordenacion actual de los registros
  */
  bool orderAsc_;

  /**
  Indica el sentido ascendente o descendente del la ordenacion actual de los registros

  @author Silix - dpinelo
  */
  bool orderAsc2_;

  /**
  Indica el sentido ascendente o descendente del la ordenacion actual de los registros

  @author Silix
  */
  bool orderAsc3_;

  /**
  Indica si se debe establecer automáticamente la primera columna como de ordenación
  */
  bool autoSortColumn_;

  /**
  Almacena la última claúsula de filtro aplicada en el refresco
  */
  QString tdbFilterLastWhere_;

  /**
  Diccionario que relaciona literales descriptivos de una condición de filtro
  con su enumeración
  */
  QMap<QString, FLTableDB::CondType> mapCondType;

  /**
  Indica si el marco de búsqueda está oculto
  */
  bool findHidden_;

  /**
  Indica si el marco para conmutar entre datos y filtro está oculto
  */
  bool filterHidden_;

  /**
  Indica si se deben mostrar los campos tipo pixmap en todas las filas
  */
  bool showAllPixmaps_;

  /**
  Nombre de la función de script a invocar para obtener el color y estilo de las filas y celdas

  El nombre de la función debe tener la forma 'objeto.nombre_funcion' o 'nombre_funcion',
  en el segundo caso donde no se especifica 'objeto' automáticamente se añadirá como
  prefijo el nombre del formulario donde se inicializa el componente FLTableDB seguido de un punto.
  De esta forma si utilizamos un mismo formulario para varias acciones, p.e. master.ui, podemos controlar
  si usamos distintas funciones de obtener color para cada acción (distintos nombres de formularios) o
  una única función común para todas las acciones.

  Ej. Estableciendo 'tdbGetColor' si el componente se inicializa en el formulario maestro de clientes,
  se utilizará 'formclientes.tdbGetColor', si se inicializa en el fomulario maestro de proveedores, se
  utilizará 'formproveedores.tdbGetColor', etc... Si establecemos 'flfactppal.tdbGetColor' siempre se llama a
  esa función independientemente del formulario en el que se inicialize el componente.

  Cuando se está pintando una celda se llamará a esa función pasándole cinco parámentros:
  - Nombre del campo correspondiente a la celda
  - Valor del campo de la celda
  - Cursor de la tabla posicionado en el registro correspondiente a la fila que
    está pintando. AVISO: En este punto los valores del buffer son indefinidos, no se hace refreshBuffer
    por motivos de eficiencia
  - Tipo del campo, ver FLUtilInterface::Type en FLObjectFactory.h
  - Seleccionado. Si es TRUE indica que la celda a pintar está en la fila resaltada/seleccionada.
    Generalmente las celdas en la fila seleccionada se colorean de forma distinta al resto.

  La función debe devolver una array con cuatro cadenas de caracteres;

  [ "color_de_fondo", "color_lapiz", "estilo_fondo", "estilo_lapiz" ]

  En los dos primeros, el color, se puede utilizar cualquier valor aceptado por QColor::setNamedColor, ejemplos;

  "green"
  "#44ADDB"

  En los dos últimos, el estilo, se pueden utilizar los valores aceptados por QBrush::setStyle y QPen::setStyle,
  ver en FLDataTable.cpp las funciones nametoBrushStyle y nametoPenStyle, ejemplos;

  "SolidPattern"
  "DiagCrossPattern"
  "DotLine"
  "SolidLine"

  Si alguno de los valores del array es vacio "", entonces se utilizarán los colores o estilos establecidos por defecto.
  */
  QString functionGetColor_;

  /**
  Indica que no se realicen operaciones con la base de datos (abrir formularios). Modo "sólo tabla".
  */
  bool onlyTable_;
  bool reqOnlyTable_;

  /**
  Editor falso
  */
  QWidget *fakeEditor_;
  
  QString tableDB_filterRecords_functionName_;

public slots:

  /**
  Actualiza el conjunto de registros.
  */
  void refresh(const bool refreshHead = false, const bool refreshData = false);

  /**
  Actualiza el conjunto de registros con un retraso.

  Acepta un lapsus de tiempo en milisegundos, activando el cronómetro interno para
  que realize el refresh definitivo al cumplirse dicho lapsus.

  @param msec Cantidad de tiempo del lapsus, en milisegundos.
  */
  void refreshDelayed(int msec = 50, const bool refreshData = true);

  /**
  Invoca al método FLSqlCursor::insertRecord()
  */
  void insertRecord();

  /**
  Invoca al método FLSqlCursor::editRecord()
  */
  void editRecord();

  /**
  Invoca al método FLSqlCursor::browseRecord()
  */
  void browseRecord();

  /**
  Invoca al método FLSqlCursor::deleteRecord()
  */
  void deleteRecord();

  /**
  Invoca al método FLSqlCursor::copyRecord()
  */
  void copyRecord();

  /**
  Coloca la columna como primera pasando el nombre del campo.

  Este slot está conectado al cuadro combinado de busqueda
  del componente. Cuando seleccionamos un campo este se coloca
  como primera columna y se reordena la tabla con esta columna.
  De esta manera siempre tendremos la tabla ordenada mediante
  el campo en el que queremos buscar.

  @param c Nombre del campo, esta columna intercambia su posion con la primera columna
  @return Falso si no existe el campo
  @author viernes@xmarts.com.mx
  @author InfoSiAL, S.L.
  */
  void putFirstCol(const QString &c);

  /**
  Coloca la columna como segunda pasando el nombre del campo.

  @author Silix - dpinelo
  */
  void putSecondCol( const QString & c );

  /**
  Mueve una columna de un campo origen a la columna de otro campo destino

  @param  from  Nombre del campo de la columna de origen
  @param  to    Nombre del campo de la columna de destino
  @param  firstSearch dpinelo: Indica si se mueven columnas teniendo en cuenta que esta función
          se ha llamado o no, desde el combo principal de búsqueda y filtrado
  */
  void moveCol( const QString & from, const QString & to, bool firstSearch = true );

  /**
  Inicia el cursor segun este campo sea de la tabla origen o de
  una tabla relacionada
  */
  void initCursor();

  /**
  Posiciona el cursor en un registro valido
  */
  void seekCursor();

  /**
  Redefinida por conveniencia
  */
  void setEnabled(bool);

  /**
  Establece el ancho de una columna

  @param  field Nombre del campo de la base de datos correspondiente a la columna
  @param  w     Ancho de la columna
  */
  void setColumnWidth(const QString &field, int w);

  /**
  @return Ancho de la columna
  */
  int columnWidth(int c);

  /**
  Establece el alto de una fila

  @param  row Número de orden de la fila, empezando en 0
  @param  h   Alto de la fila
  */
  void setRowHeight(int row, int h);

  /**
  @return Alto de la fila
  */
  int rowHeight(int row);

  /**
  Exporta a una hoja de cálculo ODS y la visualiza
  */
  void exportToOds();

  /**
  Conmuta el sentido de la ordenación de los registros de la tabla, de ascendente a descendente y
  viceversa. Los registros siempre se ordenan por la primera columna.
  Si la propiedad autoSortColumn es TRUE.
  */
  void switchSortOrder(int col = 0);

// protected slots:

  /**
  Coloca la columna indicada como primera.

  Este slot está conectado al cuadro combinado de busqueda
  del componente. Cuando seleccionamos un campo este se coloca
  como primera columna y se reordena la tabla con esta columna.
  De esta manera siempre tendremos la tabla ordenada mediante
  el campo en el que queremos buscar.

  @param c Numero de la columna en la tabla, esta columna intercambia
       su posion con la primera columna
  */
  void putFirstCol(int c);

  /**
  Coloca la columna indicada como segunda.

  @author Silix - dpinelo
  */
  void putSecondCol( int c );

  /**
  Mueve una columna desde una posicion origen a otra posicion destino.

  @param  from  Posicion de la columna de origen
  @param  to    Posicion de la columna de destino
  */
  void moveCol( int from, int to, bool firstSearch = true);

  /**
  Filtra los registros de la tabla utilizando el primer campo, según el patrón dado.

  Este slot está conectado al cuadro de texto de busqueda del componente,
  tomando el contenido de este como patrón para el filtrado.

  @param p Cadena de caracteres con el patrón de filtrado
  */
  void filterRecords(const QString &p);
  
  void setSortOrder(int ascending);
  bool isSortOrderAscending();
  
  /**
  Activa la tabla de datos
  */
  void activeTabData(bool);

  /**
  Activa la tabla de filtro
  */
  void activeTabFilter(bool);

  /**
  Limpia e inicializa el filtro
  */
  void tdbFilterClear();

signals:

  /**
  Señal emitida cuando se refresca por cambio de filtro
  */
  void refreshed();

  /**
  Señal emitida cuando se establece si el componente es o no de solo lectura.
  */
  void readOnlyChanged(bool);

  /**
  Señal emitida cuando se establece si el componente es o no de solo edición.
  */
  void editOnlyChanged(bool);

  /**
  Señal emitida cuando se establece si el componente es o no de solo inserción.
  */
  void insertOnlyChanged(bool);

  /**
  Señal emitida cuando se establece cambia el registro seleccionado.
  */
  void currentChanged();
};

#endif
