/***************************************************************************
AQFormDB.h
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

#ifndef AQFORMDB_H_
#define AQFORMDB_H_

#include <uic/ui_AQFormDB.h>

#include <AQUi/AQUi.h>

class QShortcut;

/**
Formularios de AbanQ.
 
Un formulario se podrá asociar a una acción (AQAction)
que será utilizada para construir, levantar y enlazar a
la base de datos la interfaz gráfica de usuario.
 
Un formulario podrá tener activa en un momento dado una
interfaz de usuario que se puede construir dinámicamente
según la información de la acción o que se puede indicar
explícitamente con los métodos set*Ui.
 
De forma general un formulario podrá ser de tipo maestro
o de tipo registro, dependiendo si visualiza un sólo registro
en forma detallada o si visualiza un conjunto de registro en
forma de lista. 
 
@author InfoSiAL S.L.
*/
class AQFormDB : public QMainWindow
{
  Q_OBJECT

public:

  /**
  Constructor
  */
  explicit AQFormDB( QWidget * parent = 0 );

  /**
  Destructor
  */
  virtual ~AQFormDB();

  /**
  @return Devuelve el tipo del formulario de la última inicialización ( setup.. )
  */
  AQUi::FormType type() const;

  /**
  Establece la accion asociada al formulario

  Es necesario asignar una acción al formulario para que este pueda 
  construir, levantar y enlazar a la base de datos la interfaz gráfica de usuario.

  @param act Accion del tipo AQAction a utilizar. Si es vacia desvincula.
  */
  void setAQAction( AQSharedAction act );

  /**
  @return Objeto AQAction usada por el componente.
  */
  AQSharedAction aqAction() const;

  /**
  Asigna explícitamente la interfaz a usar para el formulario 
  de tipo maestro.

  @param uiMaster Interfaz de usuario a usar en tipo maestro. El formulario toma
                  en propiedad esta interfaz y puede ser invalidada en cualquier
                  momento
  */
  void setUiMaster( QWidget * uiMaster );

  /**
  @return La interfaz de usuario usada en tipo maestro
  */
  QWidget * uiMaster() const;

  /**
  Asigna explícitamente la interfaz a usar para el formulario
  en tipo registro.

  @param uiMaster Interfaz de usuario a usar en tipo registro. El formulario toma
                  en propiedad esta interfaz y puede ser invalidada en cualquier
                  momento 
  */
  void setUiRecord( QWidget * uiRecord );

  /**
  @return La interfaz de usuario usada en tipo registro
  */
  QWidget * uiRecord() const;

  /**
  @return La intefaz de usuario actualmente activa
  */
  QWidget * uiActive() const;

  /**
  Para inhabilitar el menu emergente
  */
  QMenu * createPopupMenu();

#ifdef AQ_WARN
  static long countRefForm;
#endif

public Q_SLOTS:

  /**
  Activa en el formulario la interfaz de usuario correspondiente al tipo maestro 
  indicada en la acción o establecida explícitamente con setMasterUi.

  La interfaz activada y todos sus componetes del tipo AQComponentDB se vinculan a la accion 
  establecida para el formulario.

  Para desactivar la interfaz, la accion asociada se debe establecer a vacia con setAQAction( AQSharedAction() ).
  */
  void activateMasterUi();

  /**
  Esencialmente hace lo mismo que el método anterior salvo que en este caso
  el formulario se configura como un formulario maestro de detalle, incrustado
  en un formulario de registro.
  */
  void activateDetailUi();

  /**
  Esencialmente hace lo mismo que el método anterior salvo que en este caso
  configura el formulario con la interfaz en tipo registro indicada en la acción
  o establecida explícitamente con setRecordUi.
  */
  void activateRecordUi();

  /**
  Muestra el formulario con la interfaz activada en tipo maestro y como un diálogo
  en un nuevo bucle de evento para esperar a seleccionar registro.

  Se espera el nombre de un campo del cursor devolviendo el valor de dicho 
  campo en el registro posicionado cuando se acepta el formulario
  y un QVariant::Invalid si se cancela.

  @param  fieldName  Nombre del un campo del cursor del formulario
  @return El valor del campo si se acepta, o QVariant::Invalid si se cancela
  */
  QVariant exec( const QString & fieldName );

  /**
  Muestra el formulario de registro como un diálogo en un nuevo bucle de eventos.
  */
  void exec();

  /**
  Aceptar formulario
  */
  void accept();

  /**
  Cancelar formulario
  */
  void reject();

  /**
  @return El código resultado de ejecutar el formulario, Accepted o Rejected
  */
  AQUi::DialogCode result() const;

  /**
  Establece que se ignore o no el evento de cerrar ventana.

  Aunque se ignore el evento de cerrar, se seguirá emitiendo la señal closeRequest,
  que se puede caputar para realizar accciones antes de cerrar y/o delegar
  el cierre y destrucción del formulario en otro objeto externo

  Por defecto el formulario tiene establecido setIgnoreCloseEvent( false )
  */
  void setIgnoreCloseEvent( bool ign = true );

  /**
  @return Si el formulario ignora el evento de cerrar
  */
  bool ignoreCloseEvent() const;

  /**
  Establece que el formulario sea destruido ( con deleteLater() ) o no cuando este sea cerrado.

  Si se estableció setIgnoreCloseEvent( true ) o el formulario tiene un padre, la destrucción no
  se llevará a cabo aunque se indique lo contrario llamando a este método

  Por defecto el formulario tiene establecido setDeleteOnClose( false )
  */
  void setDeleteOnClose( bool del = true );

  /**
  @return Si el formulario se destruye al cerrarlo
  */
  bool deleteOnClose() const;

protected:

  /**
  Captura de eventos
  */
  void closeEvent( QCloseEvent * e );
  void showEvent ( QShowEvent * e );

Q_SIGNALS:

  /**
  Señal emitida justo antes de cerrar el formulario. Usando ignoreCloseEvent
  permite controlar como se debe cerrar el formulario
  */
  void closeRequest( AQFormDB * form = 0 );

  /**
  Cuando el formulario actua como un diálogo modal emite estas señales
  */
  void accepted();
  void rejected();

private Q_SLOTS:

  /**
  Uso interno
  */
  void on_actLast_triggered();
  void on_actNext_triggered();
  void on_actPrevious_triggered();
  void on_actFirst_triggered();
  void on_actRecord_triggered();
  void on_actExit_triggered();

private:

  Q_DISABLE_COPY( AQFormDB )

  void buildUi();
  void linkUi();
  void unlinkUi();
  void addUi();

  void refreshButtons();
  void save();
  void restore();
  void setupCaption( QWidget * f );

  Ui::AQFormDB ui;
  AQUi::FormType type_;
  AQUi::FormType savedType_;

  bool detailInit_;
  bool dialogInit_;
  bool ignoreCloseEvent_;
  bool deleteOnClose_;
  bool uiRecordNeedUpdate_;
  bool uiMasterNeedUpdate_;

  AQSharedAction act_;

  QWidget * uiActive_;
  QWidget * savedUiActive_;
  QWidget * uiMaster_;
  QWidget * uiRecord_;

  QShortcut * escShortcut_;
  AQUi::DialogCode result_;
};

#endif /*AQFORMDB_H_*/
