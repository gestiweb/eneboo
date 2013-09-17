/***************************************************************************
AQDataTable.h
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

#ifndef AQDATATABLE_H_
#define AQDATATABLE_H_

#include <Q3DataTable>
#include <QHash>

#include <AQUi/AQUi.h>

class QPixmap;
class QEvent;
class QMouseEvent;
class QContextMenuEvent;
class QFocusEvent;

/**
Clase que es una redefinicion de la clase QDataTable,
especifica para las necesidades de AbanQ.
 
@author InfoSiAL S.L.
*/
class AQDataTable: public Q3DataTable
{
  Q_OBJECT

public:

  /**
  constructor
  */
  AQDataTable( QWidget * parent = 0, const char * name = 0 );

  /**
  desctructor
  */
  ~AQDataTable();

  /**
  Establece la accion
  */
  void setAQAction( AQSharedAction act );

  /**
  Devuelve la accion
  */
  AQSharedAction aqAction() const;

  /**
  Establece la tabla a sólo lectura o no
  */
  void setAQReadOnly( bool mode );

  /**
  Establece la tabla a sólo edición o no
  */
  void setEditOnly( bool mode );

  /**
  Establece la tabla a sólo insercion o no
  */
  void setInsertOnly( bool mode );

protected:

  /**
  Filtrado de eventos
  */
  bool eventFilter( QObject * o, QEvent * e );

  /**
  Redefinido por conveniencia para pintar la celda
  */
  void paintCell( QPainter * p, int row, int col, const QRect & cr,
                  bool selected, const QColorGroup & cg );

  /**
  Redefinido por conveniencia para pintar el campo
  */
  void paintField( QPainter * p, const QSqlField * field, const QRect & cr,
                   bool selected );

  /**
  Redefinido por conveniencia, para evitar que aparezca el menu contextual
  con las opciones para editar registros
  */
  void contentsContextMenuEvent( QContextMenuEvent * e );

  /**
  Redefine por conveniencia, el comportamiento al hacer doble clic sobre una
  celda
  */
  void contentsMouseDoubleClickEvent( QMouseEvent * e );

private:

  /**
  Numero de la fila (registro) seleccionada actualmente
  */
  int rowSelected;

  /**
  Numero de la columna (campo) seleccionada actualmente
  */
  int colSelected;

  /**
  Cursor, con los registros
  */
  AQSharedAction act_;

  /**
  Almacena la tabla está en modo sólo lectura
  */
  bool readonly_;

  /**
  Almacena la tabla está en modo sólo edición
  */
  bool editonly_;

  /**
  Indica si la tabla está en modo sólo inserción
  */
  bool insertonly_;

  /**
  Texto del último campo dibujado en la tabla
  */
  QString lastTextPainted;

  /**
  Pixmap precargados
  */
  QPixmap * okPix;
  QPixmap * noPix;
  QPixmap * imgPix;

  QHash<int, int> adjustedCols;
  int queue;

public Q_SLOTS:

  /**
  Selecciona la fila completa en la que está la celda activa.

  @param  r Fila de la celda activa
  @param  c Columna de la celda activa
  */
  void selectRow( int r = -1, int c = -1 );

  /**
  Redefinida por conveniencia
  */
  void refresh();

  /**
  Refresca el componente leyen los datos actuales del cursor asociado a la accion
  */
  void refreshData();

  /**
  Refresca las etiquetas de las columnas
  */
  void refreshColumns();

  /**
  Operaciones con registros. Comprueban el modo en el que se encuentra
  el componente ( solo edicion, solo lectura o solo inserccion ) y 
  si es posible se realiza la operacion solicitada
  */
  bool insertRecord();
  bool editRecord();
  bool browseRecord();
  bool deleteRecord();
  bool copyRecord();

  /**
  Hace que la fila seleccionada esté visible
  */
  void ensureRowSelectedVisible();

private Q_SLOTS:

  void adjustCol( int c, int w );

Q_SIGNALS:

  /**
  Indica que se ha elegido un registro mediante doble click del ratón
  Si el parámetro es TRUE indica que el componente está en modo de solo lectura
  */
  void doubleClickOnRecord( bool );

  /**
  Indica que se ha elegido un registro mediante la pulsación de la tecla Enter
  Si el parámetro es TRUE indica que el componente está en modo de solo lectura
  */
  void pressEnterOnRecord( bool );

  void newAdjustCol( int c, int w );
};

#endif /*AQDATATABLE_H_*/
