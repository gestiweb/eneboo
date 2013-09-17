/***************************************************************************
AQFieldDB.h
-------------------
begin                : 28/08/2007
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

#ifndef AQFIELDDB_H_
#define AQFIELDDB_H_

#include <uic/ui_AQFieldDB.h>

#include <AQUi/AQComponentDB.h>

class AQImageView;
class Q3Accel;
class QCalendarWidget;

/**
Este componente es utilizado en los formularios y
se autoconfigura, a partir de los metadatos. Además
de servir de edición de un campo del registro actual del
cursor, permite mediante el botón de búsqueda buscar su valor
en la tabla de búsqueda asociada, o en un fichero del disco
(como por ejemplo un pixmap) según corresponda.

La tabla de busqueda asociada viene a ser la tabla con la
que se relaciona la tabla origen de este campo y que
precisamente esta relacionada por dicho campo.

Por ejemplo;
si el campo es el de DIVISA de la tabla CLIENTES, el valor
de DIVISA se deberá buscar en la tabla de DIVISAS, ya que
la tabla CLIENTES está relacionada con la tabla DIVISAS, y
la cardinalidad de la relacion sería

  CLIENTES <-M----1-> DIVISAS,

(un cliente tiene una divisa y una divisa puede estar en muchos
clientes). Cuando queramos asignar una divisa a un cliente
no vale cualquier valor si no uno de los que tenemos en la tabla
divisa, entonces con el botón de busqueda del componente se nos
abre la tabla divisas donde podemos escoger el valor oportuno.

@author InfoSiAL S.L.
*/
class AQFieldDB: public AQComponentDB
{
  Q_OBJECT

  Q_PROPERTY( QString fieldName READ fieldName WRITE setFieldName )
  Q_PROPERTY( QString tableName READ tableName WRITE setTableName )
  Q_PROPERTY( QString foreignField READ foreignField WRITE setForeignField )
  Q_PROPERTY( QString fieldRelation READ fieldRelation WRITE setFieldRelation )
  Q_PROPERTY( QString actionName READ actionName WRITE setActionName )
  Q_PROPERTY( bool showAlias READ showAlias WRITE setShowAlias )
  Q_PROPERTY( bool showEditor READ showEditor WRITE setShowEditor )
  Q_PROPERTY( int textFormat READ textFormat WRITE setTextFormat )

public:

  /**
  constructor
  */
  AQFieldDB( QWidget * parent = 0 );

  /**
  desctructor
  */
  ~AQFieldDB();

  /**
  Establece la accion que debe utilizar el componente.

  Este método se debe llamar siempre antes que setupUi y linkUi, ya que a
  través de la accion los componente pueden conocer los metadatos que necesitan
  para autoconfigurarse.

  @param  act Accion del tipo AQAction a utilizar. Si es vacia desvincula.
  */
  void setAQAction( AQSharedAction act );

  /**
  @return Objeto AQAction usada por el componente.
  */
  AQSharedAction aqAction() const;

  /**
  Levanta la interfaz de usuario

  @param  form  El formulario al que pertenece el componente
  */
  void setupUi( AQFormDB * form );

  /**
  Vincular Ui al cursor. Para desvincular setAQAction( AQSharedAction() )
  */
  void linkUi();

  /**
  Refresco inicial
  */
  void initRefreshUi();

  /**
  Para obtener el nombre de la accion.

  @return Nombre de la accion
  */
  QString actionName() const;

  /**
  Para establecer el nombre de la accion.

  @param aN Nombre de la accion
  */
  void setActionName( const QString & aN );

  /**
  Para obtener el nombre del campo.

  @return Nombre del campo
  */
  QString fieldName() const;

  /**
  Para añadir un filtro al cursor.

  */
  void setFilter( const QString & f );

  /**
  Para establecer el nombre del campo.

  @param fN Nombre del campo
  */
  void setFieldName( const QString & fN );

  /**
  Para obtener el nombre de la tabla foránea.

  @return Nombre de la tabla
  */
  QString tableName() const;

  /**
  Para establecer el nombre de la tabla foránea.

  @param fT Nombre de la tabla
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
  Para obtener el widget editor.

  @return Objeto con el editor del campo
  */
  QWidget * editor() const;

  /**
  Para establecer el nombre del campo relacionado.

  @param fN Nombre del campo
  */
  void setFieldRelation( const QString & fN );

  /**
  Establece el formato del texto

  @param f Formato del campo
  */
  void setTextFormat( int f );

  /**
  @return El formato del texto
  */
  int textFormat() const;

  /**
  Establece el valor contenido en elcampo.

  @param val Valor a establecer
  */
  void setValue( const QVariant & val );

  /**
  Obtiene el valor contenido en el campo.
  */
  QVariant value();

  /**
  Marca como seleccionado el contenido del campo.
  */
  void selectAll();

  /**
  Devuelve el valor de la propiedad showAlias. Esta propiedad es
  usada para saber si hay que mostrar el alias cuando se está
  en modo de cursor relacionado.
  */
  bool showAlias() const;

  /**
  Establece el estado de la propiedad showAlias.
  */
  void setShowAlias( bool value );

  /**
  Inserta como acelerador de teclado una combinación de teclas, devociendo su identificador

  @param key Cadena de texto que representa la combinación de teclas (p.e. "Ctrl+Shift+O")
  @return El identificador asociado internamente a la combinación de teclas aceleración insertada
  */
  int insertAccel( const QString & key );

  /**
  Elimina, desactiva, una combinación de teclas de aceleración según su identificador.

  @param id Identificador de la combinación de teclas de aceleración
  */
  void removeAccel( int id );

  /**
  Establece la capacidad de mantener el componente deshabilitado ignorando posibles
  habilitaciones por refrescos. Ver AQFieldDB::keepDisabled_ .

  @param keep TRUE para activar el mantenerse deshabilitado y FALSE para desactivar
  */
  void setKeepDisabled( bool keep );

  /**
  Devuelve el valor de la propiedad showEditor.
  */
  bool showEditor() const;

  /**
  Establece el valor de la propiedad showEditor.
  */
  void setShowEditor( bool show );

  /**
  Captura de eventos
  */
  bool eventFilter( QObject * obj, QEvent * ev );

protected Q_SLOTS:

  /**
  Refresca el contenido del campo con los valores del cursor de la tabla origen.

  Si se indica el nombre de un campo sólo "refresca" si el campo indicado
  coincide con la propiedad fieldRelation, tomando como filtro el valor del campo
  fieldRelation de la tabla relacionada. Si no se indica nigún nombre de
  campo, el refresco es llevado a cabo siempre.

  @param fN Nombre de un campo
  */
  void refresh( const QString & fN = QString() );

  /**
  Refresco rápido
  */
  void refreshQuick( const QString & fN = QString() );

  /**
  Actualiza el valor del campo con una cadena de texto.

  @param t Cadena de texto para actualizar el campo
  */
  void updateValue( const QString & t );

  /**
  Actualiza el valor del campo con una fecha.

  @param d Fecha para actualizar el campo
  */
  void updateValue( const QDate & d );

  /**
  Actualiza el valor del campo con una hora.

  @param t Hora para actualizar el campo
  */
  void updateValue( const QTime & t );

  /**
  Actualiza el valor del campo con un valor logico.

  @param b Valor logico para actualizar el campo
  */
  void updateValue( bool b );

  /**
  Actualiza el valor del campo con un texto, si el componente
  es del tipo QTextEdit
  */
  void updateValue();

  /**
  Borra imagen en campos tipo Pixmap.
  */
  void clearPixmap();

  /**
  Guarda imagen en campos tipo Pixmap.

  @param f Indica el formato con el que guardar la imagen
  */
  void savePixmap( int f );

public Q_SLOTS:

  /**
  Abre un dialogo para buscar en una tabla o en ficheros
  el valor del campo
  */
  void searchValue();

  /**
  Abre un dialogo para buscar un fichero de imagen.

  Si el campo no es de tipo Pixmap no hace nada
  */
  void searchPixmap();

  /**
  Emite la señal de foco perdido
  */
  void emitLostFocus();

  /**
  Sólo se pueden conectar objetos tipo AQFielDB, es decir el que
  envía la señal conectada a este puerto es de la clase AQFieldDB.

  Establece el valor de este campo según el resultado de la consulta
  cuya claúsula 'where' es;  nombre campo del objeto que envía la señal igual
  al valor que se indica como parámetro.

  El uso normal es conectar la señal AQFieldDB::textChanged(cons QString&) a este
  slot.

  @param v Valor
  */
  void setMapValue( const QString & v );

  /**
  Emite la señal de keyF2Pressed.

  La señal key_F2_Pressed del editor (sólo si el editor es AQLineEdit)
  está conectada a este slot.
  */
  void emitKeyF2Pressed();

  /**
  Emite la señal de textChanged.

  La señal textChanged del editor (sólo si el editor es AQLineEdit)
  está conectada a este slot.
  */
  void emitTextChanged( const QString & t );

  /**
  Emite la señal activatedAccel( int )
  */
  void emitActivatedAccel( int id );

protected:

  /**
  Captura de eventos
  */
  void showEvent ( QShowEvent * ev );

private:

  /**
  Inicia la accion segun este campo sea de la tabla origen o de
  una tabla relacionada
  */
  void initAQAction();

  /**
  Crea e inicia el editor apropiado para editar el tipo de datos
  contenido en el campo (p.e: si el campo contiene una fecha crea
  e inicia un QDataEdit)
  */
  void initEditor();

  /**
  Editor para el contenido del campo que representa el componente
  */
  QWidget * editor_;

  /**
  Nombre del campo de la tabla al que esta asociado este componente
  */
  QString fieldName_;

  /**
  Nombre de la tabla fóranea
  */
  QString tableName_;

  /**
  Nombre de la accion
  */
  QString actionName_;

  /**
  Nombre del campo foráneo
  */
  QString foreignField_;

  /**
  Nombre del campo de la relación
  */
  QString fieldRelation_;

  /**
  Nombre del campo de la relación
  */
  QString filter_;

  /**
  Accion usada por el componente
  */
  AQSharedAction act_;

  /**
  Variable que almacena el estado de la propiead showAlias.
  */
  bool showAlias_;

  /**
  Seleccionador de fechas.
  */
  QCalendarWidget * calendar_;

  /**
  Aceleradores de combinaciones de teclas
  */
  Q3Accel * accel_;

  /**
  Indica que el componente permanezca deshabilitado evitando que vuelva a
  habilitarse en sucesivos refrescos. Ver AQFieldDB::refresh().
  */
  bool keepDisabled_;

  /**
  Editor para imagenes
  */
  AQImageView * editorImg_;

  /**
  Boton auxiliar multifunción
  */
  QPushButton * pbAux_;

  /**
  Boton auxiliar multifunción
  */
  QPushButton * pbAux2_;

  /**
  Boton auxiliar multifunción
  */
  QPushButton * pbAux3_;

  /**
  Almacena el alias del campo que será mostrado en el formulario
  */
  QString fieldAlias_;

  /**
  Almacena el valor de la propiedad showEditor.

  Esta propiedad indica que se muestre o no el editor de contenido del campo
  */
  bool showEditor_;

  /**
  Tamaños maximo y minimos iniciales
  */
  QSize initMaxSize_;
  QSize initMinSize_;

  /**
  Interfaz de Usuario
  */
  Ui::AQFieldDB ui;

  /**
  Uso interno
  */
  void firstRefresh();
  bool firstRefresh_;

  /**
   Uso interno para recordar tipo de refresco a utilizar
   */
  int refreshType_;

Q_SIGNALS:

  /**
  Señal de foco perdido
  */
  void lostFocus();

  /**
  Señal emitida si se pulsa la tecla F2 en el editor
  */
  void keyF2Pressed();

  /**
  Señal emitida si se cambia el texto en el editor, sólo si es del tipo AQLineEdit
  */
  void textChanged( const QString & );

  /**
  Cuando se pulsa una combinación de teclas de aceleración se emite esta señal indicando el identificador
  de la combinación de teclas pulsada
  */
  void activatedAccel( int );
};

/**
Soporte para el antiguo FLFieldDB

Para soporte y migración de los antiguos formularios
de Qt3 a Qt4
*/
class FLFieldDB : public AQFieldDB
{
  Q_OBJECT

public:

  FLFieldDB( QWidget * parent = 0 );
  ~FLFieldDB();
};

#endif /*AQFIELDDB_H_*/
