/***************************************************************************
                              FLFormDB.h
                          -------------------
 begin                : Sat Jul 28 2001
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

#ifndef FLFORMDB_H
#define FLFORMDB_H

#include <qapplication.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qobjectlist.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qfocusdata.h>

class FLSqlCursor;
class FLAction;
class FLFormDBInterface;

/**
Representa un formulario que enlaza con una tabla.

Se utiliza como contenedor de componentes que quieran
enlazar con la base de datos y acceder a los registros
del cursor. Esta estructura simplifica en gran
medida el acceso a los datos ya que muchas tareas son
autom�ticamente gestionadas por este formulario contenedor.

En un principio el formulario se crea vac�o y debemos invocar
el metodo FLFormDB::setMainWidget(), pas�ndole como par�metro
otro widget (generalmente un formulario creado con QtDesigner),
el cual contiene distintos componentes, este widget se visualizar�
dentro de este contenedor, autofonfigur�ndose todos los componentes
que contiene, con los datos y metadatos del cursor. Generalmente los
componentes ser�n plugins, como FLFieldDB o FLTableDB.

@author InfoSiAL S.L.
*/
class FLFormDB: public QWidget
{
  Q_OBJECT

  friend class FLFormDBInterface;

public:

  /**
  constructor
  */
  explicit FLFormDB(QWidget *parent = 0, const char *name = 0, WFlags f = 0);

  /**
  constructor.

  @param actionName Nombre de la acci�n asociada al formulario
  */
  FLFormDB(const QString &actionName, QWidget *parent = 0, WFlags f = 0);

  /**
  constructor sobrecargado.

  @param cursor Objeto FLSqlCursor para asignar a este formulario
  @param actionName Nombre de la acci�n asociada al formulario
  */
  FLFormDB(FLSqlCursor *cursor, const QString &actionName = QString::null,
           QWidget *parent = 0, WFlags f = 0);

  /**
  destructor
  */
  virtual ~FLFormDB();

  /**
  Establece el cursor que debe utilizar el formulario.

  @param c Cursor con el que trabajar
  */
  virtual void setCursor(FLSqlCursor *c);

  /**
  Para obtener el cursor utilizado por el formulario.

  return Objeto FLSqlCursor con el cursor que contiene los registros para ser utilizados
      en el formulario
  */
  FLSqlCursor *cursor() const {
    return cursor_;
  }

  /**
  Para obtener el widget principal del formulario.

  return Objeto QWidget que corresponde con el widget principal del formulario
  */
  QWidget *mainWidget() const {
    return mainWidget_;
  }

  /**
  Establece el identificador MDI
  */
  void setIdMDI(const QString &id) {
    idMDI_ = id;
  }

  /**
  Obtiene el identificador MDI
  */
  QString idMDI() const {
    return idMDI_;
  }

  /**
  Establece widget como principal del formulario.

  Este widget contendr� componentes que quieran enlazar con la
  base de datos, por lo que esperan estar contenidos en una clase
  FLFormDB, la cual les proporciona el cursor (registros) a los que enlazar.
  Si ya existiera otro widget como principal, este ser� borrado.

  Si existe un widget principal establecido con anterioridad ser� borrado

  @param w Widget principal para el formulario
  */
  virtual void setMainWidget(QWidget *w);

  /**
  Sobrecargado de setMainWidget.

  Aqui toma el nombre de un formulario de la acci�n asociada y construye el Widget principal, a partir de �l.
  */
  virtual void setMainWidget();

  /**
  Sobrecargado de setMainWidget.

  Aqui construye el Widget principal a partir del nombre de un fichero de interfaz .ui.

  @param uiFileName Nombre del fichero de descripci�n de interfaz, incluyendo la extension .ui, p.e. clientes.ui
  */
  virtual void setMainWidget(const QString &uiFileName);

  /**
  Obtiene la imagen o captura de pantalla del formulario.
  */
  QImage snapShot();

  /**
  Salva en un fichero con formato PNG la imagen o captura de pantalla del formulario.

  @param pathFile Ruta y nombre del fichero donde guardar la imagen
  */
  void saveSnapShot(const QString &pathFile);

  /**
  Establece el t�tulo de la ventana.

  @param text Texto a establecer como t�tulo de la ventana
  @author Silix
  */
  void setCaptionWidget(const QString &text);

public slots:

  /**
  Cierra el formulario
  */
  bool close();

  /**
  Invoca a la funci�n "init" del script "masterprocess" asociado al formulario
  */
  virtual void initScript();

  /**
  Muestra el formulario sin llamar al script "init".
  Utilizado en documentaci�n para evitar conflictos al capturar los formularios
  */
  void showForDocument();

  /**
  Maximiza el formulario
  */
  void setMaximized();

private slots:

  void callInitScript();

protected:

  /**
  Inicializaci�n
  */
  void initForm();

  /**
  Captura evento cerrar
  */
  virtual void closeEvent(QCloseEvent *e);

  /**
  Captura evento mostrar
  */
  virtual void showEvent(QShowEvent *e);

  /**
  Captura evento ocultar
  */
  virtual void hideEvent(QHideEvent *h);

  /**
  Inicializa componenentes del widget principal

  @param w Widget a inicializar. Si no se establece utiliza
            por defecto el widget principal actual
  */
  void initMainWidget(QWidget *w = 0);

  /**
  Cursor, con los registros, utilizado por el formulario
  */
  FLSqlCursor *cursor_;

  /**
  Nombre de la tabla, contiene un valor no vac�o cuando
  la clase es propietaria del cursor
  */
  QString name_;

  /**
  Capa principal del formulario
  */
  QVBoxLayout *layout;

  /**
  Widget principal del formulario
  */
  QWidget *mainWidget_;

  /**
  Acci�n asociada al formulario
  */
  const FLAction *action_;

  /**
  Identificador de ventana MDI.

  Generalmente es el nombre de la acci�n que abre el formulario
  */
  QString idMDI_;

  /**
  Capa para botones
  */
  QHBoxLayout *layoutButtons;

  /**
  Boton Cancelar
  */
  QPushButton *pushButtonCancel;

  /**
  Indica que la ventana ya ha sido mostrada una vez
  */
  bool showed;

  /**
  Guarda el contexto anterior que tenia el cursor
  */
  QObject *oldCursorCtxt;

  /**
  Indica que el formulario se est� cerrando
  */
  bool isClosing_;

  /**
  Componente con el foco inicial
  */
  QWidget *initFocusWidget_;

protected slots:

  /**
  Emite se�al formulari listo. Ver FLFormDB::formReady()
  */
  void emitFormReady();

private:

  /**
  Interface para scripts
  */
  FLFormDBInterface *iface;

signals:

  /**
  Se�al emitida cuando se cierra el formulario
  */
  void closed();

  /**
  Se�al emitida cuando el formulario ya ha sido inicializado y est� listo para usarse
  */
  void formReady();
};

#endif
