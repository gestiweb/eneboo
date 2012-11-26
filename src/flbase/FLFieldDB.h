/***************************************************************************
                              FLFieldDB.h
                          -------------------
 begin                : Wed Jul 4 2001
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
   bajo  los  t�rminos  de  la  Licencia  P�blica General de GNU   en  su
   versi�n 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#ifndef FLFIELDDB_H
#define FLFIELDDB_H

#include <qapplication.h>
#include <qvariant.h>
#include <qstring.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qpainter.h>
#include <qvalidator.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qpixmapcache.h>
#include <qtoolbutton.h>
#include <qframe.h>
#include <qcombobox.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qvbox.h>
#include <qsqldatabase.h>
#include <qaccel.h>
#include <qsplitter.h>

#include "FLWidgetFieldDB.h"

class FLSqlCursor;
class VDatePopup;
class FLPixmapView;
class FLFieldDBInterface;
class FLDataTable;

/**
PLUGIN que enlaza con el campo de una tabla.

Esta clase es utilizada como plugin y solo debe insertarse
como componente en una ventana (widget) que sea de la clase
FLFormDB, o que uno de sus padres sea un objeto FLFormDB.
Es decir solo tiene sentido insertarlo en una ventana
que est� enlazada con una tabla de la base de datos.

Este componente es utilizado en los formularios y
se autoconfigura, a partir de los metadatos. Adem�s
de servir de edici�n de un campo del registro actual del
cursor, permite mediante el bot�n de b�squeda buscar su valor
en la tabla de b�squeda asociada, o en un fichero del disco
(como por ejemplo un pixmap) seg�n corresponda.

La tabla de busqueda asociada viene a ser la tabla con la
que se relaciona la tabla origen de este campo y que
precisamente esta relacionada por dicho campo.

Por ejemplo;
si el campo es el de DIVISA de la tabla CLIENTES, el valor
de DIVISA se deber� buscar en la tabla de DIVISAS, ya que
la tabla CLIENTES est� relacionada con la tabla DIVISAS, y
la cardinalidad de la relacion ser�a

  CLIENTES <-M----1-> DIVISAS,

(un cliente tiene una divisa y una divisa puede estar en muchos
clientes). Cuando queramos asignar una divisa a un cliente
no vale cualquier valor si no uno de los que tenemos en la tabla
divisa, entonces con el bot�n de busqueda del componente se nos
abre la tabla divisas donde podemos escoger el valor oportuno.

@author InfoSiAL S.L.
*/
class FLFieldDB: public FLWidgetFieldDB
{
  Q_OBJECT

  Q_PROPERTY(QString fieldName READ fieldName WRITE setFieldName)
  Q_PROPERTY(QString tableName READ tableName WRITE setTableName)
  Q_PROPERTY(QString foreignField READ foreignField WRITE setForeignField)
  Q_PROPERTY(QString fieldRelation READ fieldRelation WRITE setFieldRelation)
  Q_PROPERTY(QString fieldAlias READ fieldAlias WRITE setFieldAlias)
  Q_PROPERTY(QString actionName READ actionName WRITE setActionName)
  Q_PROPERTY(bool showAlias READ showAlias WRITE setShowAlias)
  Q_PROPERTY(bool showEditor READ showEditor WRITE setShowEditor)
  Q_PROPERTY(int textFormat READ textFormat WRITE setTextFormat)

  friend class FLFieldDBInterface;
  friend class FLFormDB;
  friend class FLFormRecordDB;
  friend class FLFormSearchDB;

public:

  /**
  constructor
  */
  FLFieldDB(QWidget *parent = 0, const char *name = 0);

  /**
  Para obtener el nombre de la accion.

  @return Nombre de la accion
  */
  QString actionName() const;

  /**
  Para establecer el nombre de la accion.

  @param aN Nombre de la accion
  */
  void setActionName(const QString &aN);

  /**
  Para obtener el nombre del campo.

  @return Nombre del campo
  */
  QString fieldName() const;

  /**
  Para a�adir un filtro al cursor.

  */
  void setFilter(const QString &f);

  /**
  Para obtener el filtro del cursor.

  */
  QString filter() const;

  /**
  Para establecer el nombre del campo.

  @param fN Nombre del campo
  */
  void setFieldName(const QString &fN);

  /**
  Para obtener el nombre de la tabla for�nea.

  @return Nombre de la tabla
  */
  QString tableName() const;

  /**
  Para establecer el nombre de la tabla for�nea.

  @param fT Nombre de la tabla
  */
  void setTableName(const QString &fT);

  /**
  Para obtener el nombre del campo for�neo.

  @return Nombre del campo
  */
  QString foreignField() const;

  /**
  Para establecer el nombre del campo for�neo.

  @param fN Nombre del campo
  */
  void setForeignField(const QString &fN);

  /**
  Para obtener el nombre del campo relacionado.

  @return Nombre del campo
  */
  QString fieldRelation() const;

  /**
  @return Alias del campo, es el valor mostrado en la etiqueta
  */
  QString fieldAlias() const;

  /**
  Para obtener el widget editor.

  @return Objeto con el editor del campo
  */
  QWidget *editor() const {
    return editor_;
  }

  /**
  Para establecer el nombre del campo relacionado.

  @param fN Nombre del campo
  */
  void setFieldRelation(const QString &fN);

  /**
  Para establecer el alias del campo, mostrado en su etiqueta si showAlias es true

  @param alias Alias del campo, es el valor de la etiqueta. Si es vacio no hace nada.
  */
  void setFieldAlias(const QString &alias);

  /**
  Establece el formato del texto

  @param f Formato del campo
  */
  void setTextFormat(const int &f);

  /**
  @return El formato del texto
  */
  int textFormat() const;

  /**
  Establece el valor contenido en elcampo.

  @param v Valor a establecer
  */
  void setValue(const QVariant &v);

  /**
  Obtiene el valor contenido en el campo.
  */
  QVariant value();

  /**
  Marca como seleccionado el contenido del campo.
  */
  void selectAll();

  /**
  Devuelve el cursor de donde se obtienen los datos. Muy util
  para ser usado en el modo de tabla externa (fieldName y tableName
  definidos, foreingField y fieldRelation en blanco).
  */
  FLSqlCursor *cursor();

  /**
  Devuelve el valor de la propiedad showAlias. Esta propiedad es
  usada para saber si hay que mostrar el alias cuando se est�
  en modo de cursor relacionado.
  */
  bool showAlias() const;

  /**
  Establece el estado de la propiedad showAlias.
  */
  void setShowAlias(const bool value);

  /**
  Inserta como acelerador de teclado una combinaci�n de teclas, devociendo su identificador

  @param key Cadena de texto que representa la combinaci�n de teclas (p.e. "Ctrl+Shift+O")
  @return El identificador asociado internamente a la combinaci�n de teclas aceleraci�n insertada
  */
  int insertAccel(const QString &key);

  /**
  Elimina, desactiva, una combinaci�n de teclas de aceleraci�n seg�n su identificador.

  @param id Identificador de la combinaci�n de teclas de aceleraci�n
  */
  void removeAccel(int id);

  /**
  Inhabilita el campo

  @param b: TRUE para deshabilitar el campo y FALSE para habilitarlo
  @author Silix
  */
  void setDisabled(const bool b);

  /**
  Establece la capacidad de mantener el componente deshabilitado ignorando posibles
  habilitaciones por refrescos. Ver FLFieldDB::keepDisabled_ .

  @param keep TRUE para activar el mantenerse deshabilitado y FALSE para desactivar
  */
  void setKeepDisabled(const bool keep);

  /**
  Devuelve el valor de la propiedad showEditor.
  */
  bool showEditor() const;

  /**
  Establece el valor de la propiedad showEditor.
  */
  void setShowEditor(const bool show);

  /**
  Establece el n�mero de decimales
  */
  void setPartDecimal(int d);

protected slots:

  /**
  Refresca el contenido del campo con los valores del cursor de la tabla origen.

  Si se indica el nombre de un campo s�lo "refresca" si el campo indicado
  coincide con la propiedad fieldRelation, tomando como filtro el valor del campo
  fieldRelation de la tabla relacionada. Si no se indica nig�n nombre de
  campo el refresco es llevado a cabo siempre.

  @param fN Nombre de un campo
  */
  void refresh(const QString &fN = QString::null);

  /**
  Refresco r�pido
  */
  void refreshQuick(const QString &fN = QString::null);

  /**
  Inicia el cursor segun este campo sea de la tabla origen o de
  una tabla relacionada
  */
  void initCursor();

  /**
  Crea e inicia el editor apropiado para editar el tipo de datos
  contenido en el campo (p.e: si el campo contiene una fecha crea
  e inicia un QDataEdit)
  */
  void initEditor();

  /**
  Actualiza el valor del campo con una cadena de texto.

  @param t Cadena de texto para actualizar el campo
  */
  void updateValue(const QString &t);

  /**
  Actualiza el valor del campo con una fecha.

  @param d Fecha para actualizar el campo
  */
  void updateValue(const QDate &d);

  /**
  Actualiza el valor del campo con una hora.

  @param t Hora para actualizar el campo
  */
  void updateValue(const QTime &t);

  /**
  Actualiza el valor del campo con un valor logico.

  @param b Valor logico para actualizar el campo
  */
  void updateValue(bool b);

  /**
  Actualiza el valor del campo con un texto, si el componente
  es del tipo QTextEdit
  */
  void updateValue();

  /**
  Muestra/Oculta el seleccionador de fechas.
  */
  void toggleDatePicker();

  /**
  Borra imagen en campos tipo Pixmap.
  */
  void clearPixmap();

  /**
  Guarda imagen en campos tipo Pixmap.

  @param fmt Indica el formato con el que guardar la imagen
  */
  void savePixmap(int f);

  /**
  Muestra/Oculta el asistente de completado autom�tico.
  */
  void toggleAutoCompletion();

  /**
  Actualiza el valor del campo a partir del contenido que
  ofrece el asistente de completado autom�tico.
  */
  void autoCompletionUpdateValue();

public slots:

  /**
  Abre un formulario de edici�n para el valor seleccionado en su acci�n correspondiente
  */
  void openFormRecordRelation();

  /**
  Abre un dialogo para buscar en la tabla relacionada
  */
  void searchValue();

  /**
  Abre un dialogo para buscar un fichero de imagen.

  Si el campo no es de tipo Pixmap no hace nada
  */
  void searchPixmap();

  /**
  Carga una imagen en el campo de tipo pixmap
  @param filename: Ruta al fichero que contiene la imagen
  */
  void setPixmap(const QString &filename);

  /**
  Emite la se�al de foco perdido
  */
  void emitLostFocus();

  /**
  Establece que el control no est� mostrado
  */
  void setNoShowed();

  /**
  Establece el valor de este campo seg�n el resultado de la consulta
  cuya cla�sula 'where' es;  nombre campo del objeto que env�a la se�al igual
  al valor que se indica como par�metro.

  S�lo se pueden conectar objetos tipo FLFielDB, y su uso normal es conectar
  la se�al FLFieldDB::textChanged(cons QString&) a este slot.

  @param v Valor
  */
  void setMapValue(const QString &v);

  /**
  Emite la se�al de keyF2Pressed.

  La se�al key_F2_Pressed del editor (s�lo si el editor es FLLineEdit)
  est� conectada a este slot.
  */
  void emitKeyF2Pressed();

  /**
  Emite la se�al de labelClicked. Se usa en los campos M1 para editar el formulario de edici�n del valor seleccionado.
  */
  void emitLabelClicked();

  /**
  Emite la se�al de textChanged.

  La se�al textChanged del editor (s�lo si el editor es FLLineEdit)
  est� conectada a este slot.
  */
  void emitTextChanged(const QString &t);

  /**
  Emite la se�al activatedAccel( int )
  */
  void emitActivatedAccel(int id);

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
  Redefinida por conveniencia
  */
  void showWidget();

  /**
  Inicializa un editor falso y no funcional.

  Esto se utiliza cuando se est� editando el formulario con el dise�ador y no
  se puede mostrar el editor real por no tener conexi�n a la base de datos.
  Crea una previsualizaci�n muy esquem�tica del editor, pero suficiente para
  ver la posisic�n y el tama�o aproximado que tendr� el editor real.
  */
  void initFakeEditor();

  /**
  Auxiliar para refrescar filtros utilizando fieldMapValue_ y mapValue_
  */
  void setMapValue();

  /**
  Editor para el contenido del campo que representa el componente
  */
  QWidget *editor_;

  /**
  Nombre del campo de la tabla al que esta asociado este componente
  */
  QString fieldName_;

  /**
  Nombre de la tabla f�ranea
  */
  QString tableName_;

  /**
  Nombre de la accion
  */
  QString actionName_;

  /**
  Nombre del campo for�neo
  */
  QString foreignField_;

  /**
  Nombre del campo de la relaci�n
  */
  QString fieldRelation_;

  /**
  Nombre del campo de la relaci�n
  */
  QString filter_;

  /**
  Cursor con los datos de la tabla origen para el componente
  */
  FLSqlCursor *cursor_;

  /**
  Cursor auxiliar de uso interno para almacenar los registros de la tabla
  relacionada con la de origen
  */
  FLSqlCursor *cursorAux;

  /**
  Indica que si ya se ha inicializado el cursor
  */
  bool cursorInit;

  /**
  Indica que si ya se ha inicializado el cursor auxiliar
  */
  bool cursorAuxInit;

  /**
  Ventana superior
  */
  QWidget *topWidget_;

  /**
  Indica que el componete ya ha sido mostrado una vez
  */
  bool showed;

  /**
  Backup del cursor por defecto para acceder al modo tabla externa
  */
  FLSqlCursor *cursorBackup_;

  /**
  Variable que almacena el estado de la propiead showAlias.
  */
  bool showAlias_;

  /**
  Seleccionador de fechas.
  */
  VDatePopup *datePopup_;
  QVBox *dateFrame_;
  bool datePickerOn_;

  /**
  Aceleradores de combinaciones de teclas
  */
  QAccel *accel_;

  /**
  Indica que el componente permanezca deshabilitado evitando que vuelva a
  habilitarse en sucesivos refrescos. Ver FLFieldDB::refresh().
  */
  bool keepDisabled_;

  /**
  Editor para imagenes
  */
  FLPixmapView *editorImg_;

  /**
  Boton auxiliar multifunci�n
  */
  QPushButton *pbAux_;

  /**
  Boton auxiliar multifunci�n
  */
  QPushButton *pbAux2_;

  /**
  Boton auxiliar multifunci�n
  */
  QPushButton *pbAux3_;

  /**
  Almacena el alias del campo que ser� mostrado en el formulario
  */
  QString fieldAlias_;

  /**
  Almacena el valor de la propiedad showEditor.

  Esta propiedad indica que se muestre o no el editor de contenido del campo
  */
  bool showEditor_;

  /**
  Valor de cifras decimales en caso de ser distinto del definido en los metadatos del campo
  */
  int partDecimal_;

  /**
  Tama�os maximo y minimos iniciales
  */
  QSize initMaxSize_;
  QSize initMinSize_;

  /**
  Para asistente de completado autom�tico.
  */
  FLDataTable *autoComPopup_;
  QVBox *autoComFrame_;
  QString autoComFieldName_;
  QString autoComFieldRelation_;

  /**
  Auxiliares para poder repetir llamada a setMapValue y refrescar filtros
  */
  FLFieldDB *fieldMapValue_;
  QString mapValue_;

signals:

  /**
  Se�al de foco perdido
  */
  void lostFocus();

  /**
  Se�al emitida si se pulsa la tecla F2 en el editor
  */
  void keyF2Pressed();

  /**
  Se�al emitida si se hace click en el label de un campo M1
  */
  void labelClicked();

  /**
  Se�al emitida si se cambia el texto en el editor, s�lo si es del tipo FLLineEdit
  */
  void textChanged(const QString &);

  /**
  Cuando se pulsa una combinaci�n de teclas de aceleraci�n se emite esta se�al indicando el identificador
  de la combinaci�n de teclas pulsada
  */
  void activatedAccel(int);

  /**
  Se�al emitida si se pulsa la tecla F4 en el editor
  */
  void keyF4Pressed();

  /**
  Se�al emitida si se pulsa la tecla Return
  */
  void keyReturnPressed();
};

class FLPixmapView: public QScrollView, public QFilePreview
{
public:

  FLPixmapView(QWidget *parent = 0);
  void setPixmap(const QPixmap &pix);
  void drawContents(QPainter *p, int, int, int, int);
  void previewUrl(const QUrl &u);
  void clear();
  QPixmap pixmap();
  void setAutoScaled(const bool autoScaled);

private:

  QPixmap pixmap_;
  QPixmap pixmapView_;
  QString path_;
  bool autoScaled_;
};

class FLLineEdit: public QLineEdit
{
  Q_OBJECT

public:

  FLLineEdit(QWidget *parent, const char *name = 0);

  QString text() const;

  int type;
  int partDecimal;

public slots:
  virtual void setText(const QString &);

protected:

  void focusOutEvent(QFocusEvent *f);
  void focusInEvent(QFocusEvent *f);
};

// Uso interno
class FLDoubleValidator: public QDoubleValidator
{
public:

  FLDoubleValidator(QObject *parent, const char *name = 0);
  FLDoubleValidator(double bottom, double top, int decimals,
                    QObject *parent, const char *name = 0);
  QValidator::State validate(QString &input, int &) const;
};

// Uso interno
class FLIntValidator: public QIntValidator
{
public:

  FLIntValidator(QObject *parent, const char *name = 0);
  FLIntValidator(int minimum, int maximum,
                 QObject *parent, const char *name = 0);
  QValidator::State validate(QString &input, int &) const;
};

// Uso interno
class FLUIntValidator: public QIntValidator
{
public:

  FLUIntValidator(QObject *parent, const char *name = 0);
  FLUIntValidator(int minimum, int maximum,
                  QObject *parent, const char *name = 0);
  QValidator::State validate(QString &input, int &) const;
};

// Uso interno
class FLSpinBox: public QSpinBox
{
public:

  FLSpinBox(QWidget *parent = 0, const char *name = 0) :
    QSpinBox(parent, name) {
    editor() ->setAlignment(Qt::AlignRight);
  }
};

// Uso interno
class FLDateEdit: public QDateEdit
{
public:

  FLDateEdit(QWidget *parent = 0, const char *name = 0) : QDateEdit(parent, name) {}

protected:

  void fix();
};

#endif
